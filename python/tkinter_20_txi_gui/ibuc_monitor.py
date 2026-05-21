#!/usr/bin/env python3
"""
IBUC Redundancy Monitor
Pulls and sets redundancy status from Terrasat IBUC device

Usage:
    python3 ibuc_monitor.py              # Just display status
    python3 ibuc_monitor.py --set M     # Set to Manual
    python3 ibuc_monitor.py --set A     # Set to Automatic
    python3 ibuc_monitor.py --antenna-a BUC A   # Set Antenna A
    python3 ibuc_monitor.py --antenna-a Spare    # Set Antenna A to Spare
    python3 ibuc_monitor.py --antenna-b BUC B   # Set Antenna B
    python3 ibuc_monitor.py --antenna-b Spare    # Set Antenna B to Spare
"""

import requests
import json
import argparse

# Configuration
IBUC_HOST = "10.10.12.31"
USERNAME = "admin"
PASSWORD = "admin"

# Endpoints
LOGIN_URL = f"http://{IBUC_HOST}/index"
REDUNDANCY_URL = f"http://{IBUC_HOST}/redundancy/api/status"
SWITCH_URL = f"http://{IBUC_HOST}/redundancy/api/switch"

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

def get_redundancy_status(session, user_id):
    """Get redundancy status from IBUC"""
    session.cookies.set("userID", user_id)
    status_response = session.get(REDUNDANCY_URL)
    status_data = status_response.json()
    return status_data.get("data", {})

def set_redundancy(session, user_id, mode=None, antenna_a=None, antenna_b=None, att_a=None, att_b=None):
    """
    Set redundancy configuration
    """
    # First get current values
    data = get_redundancy_status(session, user_id)
    
    # Build switch data with provided values or current values
    switch_data = {
        "bsm_C": mode if mode else data.get("bsm_C", "M"),
        "bsw_C_1": antenna_a if antenna_a else data.get("bsw_C_1", "BUC A"),
        "bsw_C_2": antenna_b if antenna_b else data.get("bsw_C_2", "BUC B"),
        "tpta_C": str(att_a if att_a is not None else data.get("tpta_C", 0)),
        "tptb_C": str(att_b if att_b is not None else data.get("tptb_C", 0))
    }
    
    session.cookies.set("userID", user_id)
    switch_resp = session.post(SWITCH_URL, json=switch_data)
    result = switch_resp.json()
    
    return result

def parse_and_display(data):
    """Parse and display redundancy status"""
    print("\n" + "="*50)
    print("       IBUC REDUNDANCY STATUS")
    print("="*50)
    
    # Redundancy Switching Type
    bsm = data.get("bsm_C", "Unknown")
    switching_type = "Manual" if bsm == "M" else "Automatic" if bsm == "A" else bsm
    print(f"  Redundancy Switching Type: {switching_type}")
    
    # Antenna A
    antenna_a = data.get("bsw_C_1", "Unknown")
    print(f"  Antenna A:                 {antenna_a}")
    
    # Antenna B
    antenna_b = data.get("bsw_C_2", "Unknown")
    print(f"  Antenna B:                {antenna_b}")
    
    # IF Switch
    ifl_pos = data.get("ifl_pos", "Unknown")
    print(f"  IF Switch:                 {ifl_pos}")
    
    # Attenuation values
    tpta = data.get("tpta_C", "N/A")
    tptb = data.get("tptb_C", "N/A")
    print(f"  Attenuation (Spare→A):     {tpta} dBm")
    print(f"  Attenuation (Spare→B):    {tptb} dBm")
    
    print("="*50 + "\n")

def main():
    parser = argparse.ArgumentParser(description="IBUC Redundancy Monitor")
    
    # Mode options
    parser.add_argument(
        "--set", "-s",
        choices=["M", "A", "Manual", "Automatic"],
        help="Set redundancy mode (M/Manual = Manual, A/Automatic = Automatic)"
    )
    
    # Antenna A options
    parser.add_argument(
        "--antenna-a", "-aa",
        choices=["BUC A", "Spare"],
        help="Set Antenna A (BUC A or Spare)"
    )
    
    # Antenna B options
    parser.add_argument(
        "--antenna-b", "-ab",
        choices=["BUC B", "Spare"],
        help="Set Antenna B (BUC B or Spare)"
    )
    
    # Attenuation options
    parser.add_argument(
        "--att-a", "-atA",
        type=float,
        help="Set Attenuation for Spare→A (dBm)"
    )
    
    parser.add_argument(
        "--att-b", "-atB",
        type=float,
        help="Set Attenuation for Spare→B (dBm)"
    )
    
    args = parser.parse_args()
    
    print(f"Connecting to IBUC at {IBUC_HOST}...")
    
    session = requests.Session()
    
    # Login
    print("Logging in...")
    user_id = login(session)
    
    if not user_id:
        print("ERROR: Login failed!")
        return
    
    print(f"Logged in as {user_id}")
    
    # Determine mode value
    mode = None
    if args.set:
        mode = args.set
        if mode in ["Manual", "Automatic"]:
            mode = "M" if mode == "Manual" else "A"
    
    # Make changes if requested
    if mode or args.antenna_a or args.antenna_b or args.att_a is not None or args.att_b is not None:
        changes = []
        if mode:
            changes.append(f"mode={mode}")
        if args.antenna_a:
            changes.append(f"antenna_a={args.antenna_a}")
        if args.antenna_b:
            changes.append(f"antenna_b={args.antenna_b}")
        if args.att_a is not None:
            changes.append(f"att_a={args.att_a}")
        if args.att_b is not None:
            changes.append(f"att_b={args.att_b}")
        
        print(f"\nSetting: {', '.join(changes)}...")
        
        result = set_redundancy(
            session, user_id,
            mode=mode,
            antenna_a=args.antenna_a,
            antenna_b=args.antenna_b,
            att_a=args.att_a,
            att_b=args.att_b
        )
        
        if result.get("ok"):
            print(f"✓ Successfully updated!")
        else:
            print(f"✗ Failed:msg', 'Unknown {result.get(' error')}")
            return
    
    # Get and display status
    print("Fetching redundancy status...")
    data = get_redundancy_status(session, user_id)
    
    # Display
    parse_and_display(data)

if __name__ == "__main__":
    main()
