#!/usr/bin/env python3
"""
IBUC Sensor Readings Monitor
Gets sensor readings from Terrasat IBUC device

Usage:
    python3 ibuc_sensors.py
"""

import requests
import json

# Configuration
IBUC_HOST = "10.10.12.31"
USERNAME = "admin"
PASSWORD = "admin"

LOGIN_URL = f"http://{IBUC_HOST}/index"

def login(session):
    resp = session.post(LOGIN_URL, data={"username": USERNAME, "passwd": PASSWORD})
    if resp.json().get("success"):
        cookie = resp.headers.get("Set-Cookie", "")
        if "userID=" in cookie:
            return cookie.split("userID=")[1].split(";")[0]
    return None

def get_sensor_readings(session, user_id):
    session.cookies.set("userID", user_id)
    response = session.get(f"http://{IBUC_HOST}/snsr_readings_get")
    return response.json()

def display_sensors(data):
    # Map sensor IDs to readable names
    sensor_names = {
        'cvd_a': 'Supply Voltage (V)',
        'cvd_b': 'Supply Voltage (V)',
        'cvd_s': 'Supply Voltage (V)',
        'cid_a': 'Supply Current (A)',
        'cid_b': 'Supply Current (A)',
        'cid_s': 'Supply Current (A)',
        'ctv_a': 'LO Tuning Voltage (V)',
        'ctv_b': 'LO Tuning Voltage (V)',
        'ctv_s': 'LO Tuning Voltage (V)',
        'tdt_a': 'Tx Input Level (dBm)',
        'tdt_b': 'Tx Input Level (dBm)',
        'tdt_s': 'Tx Input Level (dBm)',
        'tpo_a': 'Tx Output Level (dBm)',
        'tpo_b': 'Tx Output Level (dBm)',
        'tpo_s': 'Tx Output Level (dBm)',
        'cox_a': 'Temperature (°C)',
        'cox_b': 'Temperature (°C)',
        'cox_s': 'Temperature (°C)',
    }
    
    controller_sensors = [
        ('cvd_c_1', 'Controller Voltage 1 (V)'),
        ('cvd_c_2', 'Controller Voltage 2 (V)'),
        ('tml_c', '10MHz Level'),
        ('tme_c', '10MHz Source'),
    ]
    
    print("\n" + "="*70)
    print("                    IBUC SENSOR READINGS")
    print("="*70)
    
    # IBUC Sensors
    print(f"\n{'Sensor':<35} {'IBUC A':>10} {'SPARE':>10} {'IBUC B':>10}")
    print("-"*70)
    
    # Group by sensor type
    sensor_types = [
        ('Supply Voltage', 'cvd'),
        ('Supply Current', 'cid'),
        ('LO Tuning Voltage', 'ctv'),
        ('Tx Input Level', 'tdt'),
        ('Tx Output Level', 'tpo'),
        ('Temperature', 'cox'),
    ]
    
    for label, prefix in sensor_types:
        a = data.get(f'{prefix}_a', 'N/A')
        s = data.get(f'{prefix}_s', 'N/A')
        b = data.get(f'{prefix}_b', 'N/A')
        print(f"{label:<35} {a:>10} {s:>10} {b:>10}")
    
    # Controller sensors
    print(f"\n{'CONTROLLER UNIT':^70}")
    print("-"*70)
    
    for key, label in controller_sensors:
        val = data.get(key, 'N/A')
        print(f"{label:<35} {val:>10}")
    
    print("="*70)

def main():
    session = requests.Session()
    
    print(f"Connecting to IBUC at {IBUC_HOST}...")
    user_id = login(session)
    
    if not user_id:
        print("ERROR: Login failed!")
        return
    
    print("Fetching sensor readings...")
    data = get_sensor_readings(session, user_id)
    display_sensors(data)

if __name__ == "__main__":
    main()
