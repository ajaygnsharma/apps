#!/usr/bin/env python3
"""
IBUC Alarm Status Monitor
Pulls alarm status from Terrasat IBUC device

Usage:
    python3 ibuc_alarms.py
"""

import requests
import re

# Configuration
IBUC_HOST = "10.10.12.31"
USERNAME = "admin"
PASSWORD = "admin"

LOGIN_URL = f"http://{IBUC_HOST}/index"

def login(session):
    """Login to IBUC device"""
    resp = session.post(LOGIN_URL, data={"username": USERNAME, "passwd": PASSWORD})
    if resp.json().get("success"):
        cookie = resp.headers.get("Set-Cookie", "")
        if "userID=" in cookie:
            return cookie.split("userID=")[1].split(";")[0]
    return None

def get_alarm_status(session, user_id):
    """Get alarm status page and parse"""
    session.cookies.set("userID", user_id)
    html = session.get(f"http://{IBUC_HOST}/alm_stat").text
    
    # Parse all alarm entries
    entries = re.findall(
        r'<label[^>]*>([^<]*)</label>\s*</div>\s*<div[^>]*>\s*<img[^>]+id="([^"]+)"[^>]+src="([^"]+)"',
        html
    )
    
    # Group by alarm type
    alarm_types = {}
    for label, img_id, src in entries:
        label = label.strip()
        if not label or '<' in label:
            continue
        
        status = "✅" if 'green' in src.lower() else "🔴" if 'red' in src.lower() else "⚪"
        
        # Determine which column
        if '_a' in img_id:
            col = 'IBUC A'
        elif '_s' in img_id:
            col = 'SPARE'
        elif '_b' in img_id:
            col = 'IBUC B'
        else:
            col = 'CTRL'
        
        if label not in alarm_types:
            alarm_types[label] = {'IBUC A': '', 'SPARE': '', 'IBUC B': '', 'CTRL': ''}
        alarm_types[label][col] = status
    
    return alarm_types

def display_alarms(alarm_types):
    """Display alarm status in table format"""
    print("\n" + "="*70)
    print("                    IBUC ALARM STATUS")
    print("="*70)
    print(f"{'Alarm Type':<40} {'IBUC A':>8} {'SPARE':>8} {'IBUC B':>8}")
    print("-"*70)
    
    for label, statuses in alarm_types.items():
        a = statuses['IBUC A'] or '-'
        s = statuses['SPARE'] or '-'
        b = statuses['IBUC B'] or '-'
        print(f"{label:<40} {a:>8} {s:>8} {b:>8}")
    
    print("-"*70)
    print("✅ = OK  | 🔴 = ALARM  | - = N/A")
    print("="*70)

def main():
    session = requests.Session()
    
    print(f"Connecting to IBUC at {IBUC_HOST}...")
    user_id = login(session)
    
    if not user_id:
        print("ERROR: Login failed!")
        return
    
    print("Fetching alarm status...")
    alarm_types = get_alarm_status(session, user_id)
    display_alarms(alarm_types)

if __name__ == "__main__":
    main()
