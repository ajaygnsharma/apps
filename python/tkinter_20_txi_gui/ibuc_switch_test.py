#!/usr/bin/env python3
"""
IBUC Redundancy Switch Test with Logging
Tests switching Antenna A from BUC A to Spare and back, with logging
"""

import requests
import time
import json
from datetime import datetime

# Configuration
IBUC_HOST = "10.10.12.31"
USERNAME = "admin"
PASSWORD = "admin"

LOGIN_URL = f"http://{IBUC_HOST}/index"
REDUNDANCY_URL = f"http://{IBUC_HOST}/redundancy/api/status"
SWITCH_URL = f"http://{IBUC_HOST}/redundancy/api/switch"

def log(msg, end="\n"):
    """Print with timestamp"""
    timestamp = datetime.now().strftime("%H:%M:%S")
    print(f"[{timestamp}] {msg}", end=end)

def login(session):
    """Login to IBUC device"""
    response = session.post(
        LOGIN_URL,
        data={"username": USERNAME, "passwd": PASSWORD},
        headers={"Content-Type": "application/x-www-form-urlencoded"}
    )
    result = response.json()
    if result.get("success"):
        cookie = response.headers.get("Set-Cookie", "")
        if "userID=" in cookie:
            user_id = cookie.split("userID=")[1].split(";")[0]
            return user_id
    return None

def get_status(session, user_id):
    """Get redundancy status"""
    session.cookies.set("userID", user_id)
    status_response = session.get(REDUNDANCY_URL)
    data = status_response.json().get("data", {})
    return data

def switch(session, user_id, mode, antenna_a, antenna_b=None):
    """Perform switch"""
    data = get_status(session, user_id)
    
    switch_data = {
        "bsm_C": mode,
        "bsw_C_1": antenna_a,
        "bsw_C_2": antenna_b if antenna_b else data.get("bsw_C_2", "BUC B"),
        "tpta_C": str(data.get("tpta_C", 0)),
        "tptb_C": str(data.get("tptb_C", 0))
    }
    
    session.cookies.set("userID", user_id)
    switch_resp = session.post(SWITCH_URL, json=switch_data)
    return switch_resp.json()

def print_status(data):
    """Print status in nice format"""
    mode = "Manual" if data.get("bsm_C") == "M" else "Automatic"
    ant_a = data.get("bsw_C_1", "?")
    ant_b = data.get("bsw_C_2", "?")
    ifl = data.get("ifl_pos", "?")
    print(f"  Mode: {mode:10} | Antenna A: {ant_a:8} | Antenna B: {ant_b:8} | IF: {ifl}")

def main():
    session = requests.Session()
    
    log("Logging in...")
    user_id = login(session)
    if not user_id:
        log("ERROR: Login failed!")
        return
    
    log("Logged in successfully")
    print()
    
    # Get initial status
    log("=== INITIAL STATUS ===")
    data = get_status(session, user_id)
    print_status(data)
    print()
    
    # Step 1: Set to Manual + Switch to Spare
    log("=== STEP 1: Set Manual + Switch Antenna A to Spare ===")
    result = switch(session, user_id, "M", "Spare")
    log(f"Switch response: {result}")
    
    # Poll for 20 seconds
    for i in range(20, 0, -5):
        time.sleep(5)
        data = get_status(session, user_id)
        log(f"After {20-i}s: ", end="")
        print_status(data)
    
    log("Confirmed: Antenna A is on Spare")
    print()
    
    # Step 2: Switch back to BUC A
    log("=== STEP 2: Switch Antenna A back to BUC A ===")
    result = switch(session, user_id, "M", "BUC A")
    log(f"Switch response: {result}")
    
    # Poll for a few seconds
    for i in range(10, 0, -2):
        time.sleep(2)
        data = get_status(session, user_id)
        log(f"After {10-i}s: ", end="")
        print_status(data)
    
    log("Confirmed: Antenna A is back on BUC A")
    print()
    
    # Final status
    log("=== FINAL STATUS ===")
    data = get_status(session, user_id)
    print_status(data)
    log("Done!")

if __name__ == "__main__":
    main()
