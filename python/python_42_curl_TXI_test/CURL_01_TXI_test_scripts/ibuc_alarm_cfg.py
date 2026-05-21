#!/usr/bin/env python3
"""
IBUC Alarm Configuration Monitor
Gets and sets alarm configuration from Terrasat IBUC device

Usage:
    python3 ibuc_alarm_cfg.py              # Display current config
    python3 ibuc_alarm_cfg.py --help      # Show all options
    python3 ibuc_alarm_cfg.py --temp-a Major    # Set IBUC A temp to Major
    python3 ibuc_alarm_cfg.py --temp-a Minor    # Set IBUC A temp to Minor
    python3 ibuc_alarm_cfg.py --temp-a Disabled # Set IBUC A temp to Disabled
    python3 ibuc_alarm_cfg.py --temp-s Disabled  # Set Spare temp to Disabled
    python3 ibuc_alarm_cfg.py --suppress-temp-a on   # Enable temp suppressible
    python3 ibuc_alarm_cfg.py --suppress-temp-a off  # Disable temp suppressible
"""

import requests
import re
import argparse

# Configuration
IBUC_HOST = "10.10.12.31"
USERNAME = "admin"
PASSWORD = "admin"

LOGIN_URL = f"http://{IBUC_HOST}/index"
ALARM_CONFIG_URL = f"http://{IBUC_HOST}/alm_config"

def login(session):
    resp = session.post(LOGIN_URL, data={"username": USERNAME, "passwd": PASSWORD})
    if resp.json().get("success"):
        cookie = resp.headers.get("Set-Cookie", "")
        if "userID=" in cookie:
            return cookie.split("userID=")[1].split(";")[0]
    return None

def get_alarm_config(session, user_id):
    session.cookies.set("userID", user_id)
    html = session.get(ALARM_CONFIG_URL).text
    
    # Parse radio buttons - need to find checked status within the tag
    config = {}
    
    # Find all input tags with name and value
    pattern = r'<input([^>]*?)>'
    all_inputs = re.findall(pattern, html)
    
    for tag in all_inputs:
        name_match = re.search(r'name="([^"]+)"', tag)
        value_match = re.search(r'value="([^"]+)"', tag)
        
        if name_match and value_match:
            name = name_match.group(1)
            value = value_match.group(1)
            
            # Only save if this is a threshold setting (temp_, til_, etc.) and is checked
            if any(name.startswith(prefix) for prefix in ['temp_', 'til_', 'tih_', 'tol_', 'toh_']):
                if 'checked' in tag.lower():
                    config[name] = value
    
    # Parse checkboxes
    checkbox_state = {}
    for tag in all_inputs:
        name_match = re.search(r'name="([^"]+)"', tag)
        if name_match:
            name = name_match.group(1)
            if name.startswith('temp_s_') or name.startswith('til_s_') or name.startswith('tih_s_') or \
               name.startswith('tol_s_') or name.startswith('toh_s_') or name.startswith('tts_') or \
               name.startswith('tas_') or name.startswith('taz_'):
                checkbox_state[name] = 'checked' in tag.lower()
    
    return config, checkbox_state

def set_alarm_config(session, user_id, threshold_changes=None, suppress_changes=None):
    """Set alarm configuration"""
    config, checkbox_state = get_alarm_config(session, user_id)
    
    form_data = {}
    
    # Threshold settings
    threshold_keys = ['temp', 'til', 'tih', 'tol', 'toh']
    ibucs = ['A', 'B', 'S']
    
    for key in threshold_keys:
        for ibuc in ibucs:
            form_data[f'{key}_{ibuc}'] = config.get(f'{key}_{ibuc}', '1')
    
    if threshold_changes:
        for alarm_type, ibuc, value in threshold_changes:
            form_data[f'{alarm_type}_{ibuc}'] = value
    
    # Suppressible alarms
    suppress_keys = ['temp_s', 'til_s', 'tih_s', 'tol_s', 'toh_s', 'tts', 'tas', 'taz']
    
    for key in suppress_keys:
        for ibuc in ibucs:
            form_data[f'{key}_{ibuc}'] = 'on' if checkbox_state.get(f'{key}_{ibuc}', False) else 'off'
    
    if suppress_changes:
        for alarm_type, ibuc, value in suppress_changes:
            form_data[f'{alarm_type}_{ibuc}'] = value
    
    session.cookies.set("userID", user_id)
    response = session.post(ALARM_CONFIG_URL, data=form_data)
    
    return response.status_code == 200

def display_config(config, checkbox_state):
    value_map = {'2': 'Major', '1': 'Minor', '0': 'Disabled'}
    
    labels = {
        'temp': 'Temperature',
        'til': 'Input Threshold Low',
        'tih': 'Input Threshold High',
        'tol': 'Output Threshold Low',
        'toh': 'Output Threshold High'
    }
    
    print("\n" + "="*70)
    print("              IBUC ALARM CONFIGURATION")
    print("="*70)
    
    print(f"\n{'Threshold Settings':^70}")
    print(f"\n{'Alarm Type':<30} {'IBUC A':>12} {'IBUC B':>12} {'IBUC S':>12}")
    print("-"*70)
    
    for key, label in labels.items():
        a = value_map.get(config.get(f'{key}_A', '1'), '?')
        b = value_map.get(config.get(f'{key}_B', '1'), '?')
        s = value_map.get(config.get(f'{key}_S', '0'), '?')
        print(f"{label:<30} {a:>12} {b:>12} {s:>12}")
    
    print(f"\n{'Suppressible Alarms':^70}")
    print(f"\n{'Alarm Type':<25} {'IBUC A':>15} {'IBUC B':>15} {'IBUC S':>15}")
    print("-"*70)
    
    suppress_labels = {
        'temp_s': 'Temperature',
        'til_s': 'Input Threshold Low',
        'tih_s': 'Input Threshold High',
        'tol_s': 'Output Threshold Low',
        'toh_s': 'Output Threshold High',
        'tts': 'High Temp Shutdown',
        'tas': 'Simulate Fault',
        'taz': 'Suppress Fault'
    }
    
    for key, label in suppress_labels.items():
        a = "✅" if checkbox_state.get(f'{key}_A', False) else "❌"
        b = "✅" if checkbox_state.get(f'{key}_B', False) else "❌"
        s = "✅" if checkbox_state.get(f'{key}_S', False) else "❌"
        print(f"{label:<25} {a:>15} {b:>15} {s:>15}")
    
    print("-"*70)
    print("Threshold: Major = Major alarm, Minor = Minor alarm, Disabled = Off")
    print("Suppressible: ✅ = Enabled, ❌ = Disabled")
    print("="*70)

def main():
    parser = argparse.ArgumentParser(description="IBUC Alarm Configuration")
    
    # Threshold arguments for each IBUC
    for key in ['temp', 'til', 'tih', 'tol', 'toh']:
        for ibuc in ['a', 'b', 's']:
            parser.add_argument(f"--{key}-{ibuc}", choices=['Major', 'Minor', 'Disabled'])
    
    # Suppressible arguments
    for key in ['temp_s', 'til_s', 'tih_s', 'tol_s', 'toh_s', 'tts']:
        for ibuc in ['a', 'b', 's']:
            parser.add_argument(f"--{key}-{ibuc}", choices=['on', 'off'])
    
    args = parser.parse_args()
    
    session = requests.Session()
    
    print(f"Connecting to IBUC at {IBUC_HOST}...")
    user_id = login(session)
    
    if not user_id:
        print("ERROR: Login failed!")
        return
    
    print("Logged in")
    
    # Build changes
    threshold_changes = []
    value_map = {'Major': '2', 'Minor': '1', 'Disabled': '0'}
    
    for key in ['temp', 'til', 'tih', 'tol', 'toh']:
        for ibuc in ['a', 'b', 's']:
            val = getattr(args, f'{key}_{ibuc}', None)
            if val:
                threshold_changes.append((key, ibuc.upper(), value_map[val]))
    
    suppress_changes = []
    for key in ['temp_s', 'til_s', 'tih_s', 'tol_s', 'toh_s', 'tts']:
        for ibuc in ['a', 'b', 's']:
            val = getattr(args, f'{key}_{ibuc}', None)
            if val:
                suppress_changes.append((key, ibuc.upper(), val))
    
    if threshold_changes or suppress_changes:
        print("\nApplying changes...")
        success = set_alarm_config(session, user_id, threshold_changes, suppress_changes)
        if success:
            print("✓ Configuration updated!")
        else:
            print("✗ Failed to update configuration")
    
    print("\nFetching current configuration...")
    config, checkbox_state = get_alarm_config(session, user_id)
    display_config(config, checkbox_state)

if __name__ == "__main__":
    main()
