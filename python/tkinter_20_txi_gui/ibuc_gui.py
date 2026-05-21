#!/usr/bin/env python3
"""
IBUC Monitor & Control GUI
A unified tkinter GUI for monitoring and controlling Terrasat IBUC device

Usage:
    python3 ibuc_gui.py
"""

import tkinter as tk
from tkinter import ttk, messagebox
import requests
import re
import time

# Configuration
IBUC_HOST = "10.10.12.31"
USERNAME = "admin"
PASSWORD = "admin"

class IBUCClient:
    """IBUC API Client"""
    
    def __init__(self, host, username, password):
        self.host = host
        self.username = username
        self.password = password
        self.session = requests.Session()
        self.user_id = None
    
    def login(self):
        try:
            resp = self.session.post(
                f"http://{self.host}/index",
                data={"username": self.username, "passwd": self.password}
            )
            if resp.json().get("success"):
                cookie = resp.headers.get("Set-Cookie", "")
                if "userID=" in cookie:
                    self.user_id = cookie.split("userID=")[1].split(";")[0]
                    return True
        except:
            pass
        return False
    
    def get_redundancy(self):
        if not self.user_id: return None
        self.session.cookies.set("userID", self.user_id)
        try:
            resp = self.session.get(f"http://{self.host}/redundancy/api/status")
            return resp.json().get("data", {})
        except:
            return None
    
    def set_redundancy(self, mode, antenna_a=None, antenna_b=None, att_a=None, att_b=None):
        if not self.user_id: return False
        data = self.get_redundancy()
        if not data: return False
        
        payload = {
            "bsm_C": mode,
            "bsw_C_1": antenna_a if antenna_a else data.get("bsw_C_1", "BUC A"),
            "bsw_C_2": antenna_b if antenna_b else data.get("bsw_C_2", "BUC B"),
            "tpta_C": str(att_a if att_a is not None else data.get("tpta_C", 0)),
            "tptb_C": str(att_b if att_b is not None else data.get("tptb_C", 0))
        }
        
        self.session.cookies.set("userID", self.user_id)
        try:
            resp = self.session.post(f"http://{self.host}/redundancy/api/switch", json=payload)
            return resp.json().get("ok", False)
        except:
            return False
    
    def get_sensors(self):
        if not self.user_id: return None
        self.session.cookies.set("userID", self.user_id)
        try:
            resp = self.session.get(f"http://{self.host}/snsr_readings_get")
            return resp.json()
        except:
            return None
    
    def get_alarms(self):
        if not self.user_id: return {}
        self.session.cookies.set("userID", self.user_id)
        try:
            html = self.session.get(f"http://{self.host}/alm_stat").text
            entries = re.findall(
                r'<label[^>]*>([^<]*)</label>\s*</div>\s*<div[^>]*>\s*<img[^>]+id="([^"]+)"[^>]+src="([^"]+)"',
                html
            )
            
            alarm_types = {}
            for label, img_id, src in entries:
                label = label.strip()
                if not label or '<' in label:
                    continue
                status = "OK" if 'green' in src.lower() else "ALARM" if 'red' in src.lower() else "?"
                
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
        except:
            return {}
    
    def get_alarm_config(self):
        if not self.user_id: return {}, {}
        self.session.cookies.set("userID", self.user_id)
        try:
            html = self.session.get(f"http://{self.host}/alm_config").text
            
            # Parse radio buttons
            config = {}
            pattern = r'<input([^>]*?)>'
            for tag in re.findall(pattern, html):
                name_match = re.search(r'name="([^"]+)"', tag)
                value_match = re.search(r'value="([^"]+)"', tag)
                if name_match and value_match:
                    name = name_match.group(1)
                    value = value_match.group(1)
                    if any(name.startswith(prefix) for prefix in ['temp_', 'til_', 'tih_', 'tol_', 'toh_']):
                        if 'checked' in tag.lower():
                            config[name] = value
            
            # Parse checkboxes
            checkbox_state = {}
            for tag in re.findall(pattern, html):
                name_match = re.search(r'name="([^"]+)"', tag)
                if name_match:
                    name = name_match.group(1)
                    if any(name.startswith(prefix) for prefix in ['temp_s_', 'til_s_', 'tih_s_', 'tol_s_', 'toh_s_', 'tts_', 'tas_', 'taz_']):
                        checkbox_state[name] = 'checked' in tag.lower()
            
            return config, checkbox_state
        except:
            return {}, {}
    
    def set_alarm_config(self, threshold_changes=None, suppress_changes=None):
        if not self.user_id: return False
        
        config, checkbox_state = self.get_alarm_config()
        
        form_data = {}
        
        # Threshold settings
        for key in ['temp', 'til', 'tih', 'tol', 'toh']:
            for ibuc in ['A', 'B', 'S']:
                form_data[f'{key}_{ibuc}'] = config.get(f'{key}_{ibuc}', '1')
        
        if threshold_changes:
            for alarm_type, ibuc, value in threshold_changes:
                form_data[f'{alarm_type}_{ibuc}'] = value
        
        # Suppressible
        for key in ['temp_s', 'til_s', 'tih_s', 'tol_s', 'toh_s', 'tts', 'tas', 'taz']:
            for ibuc in ['A', 'B', 'S']:
                form_data[f'{key}_{ibuc}'] = 'on' if checkbox_state.get(f'{key}_{ibuc}', False) else 'off'
        
        if suppress_changes:
            for alarm_type, ibuc, value in suppress_changes:
                form_data[f'{alarm_type}_{ibuc}'] = value
        
        self.session.cookies.set("userID", self.user_id)
        try:
            resp = self.session.post(f"http://{self.host}/alm_config", data=form_data)
            return resp.status_code == 200
        except:
            return False


class IBUCGUI:
    """Main GUI Application"""
    
    def __init__(self, root):
        self.root = root
        self.root.title("IBUC Monitor & Control")
        self.root.geometry("900x700")
        
        self.client = IBUCClient(IBUC_HOST, USERNAME, PASSWORD)
        self.auto_refresh = False
        self.refresh_thread = None
        
        self.setup_ui()
        self.connect()
    
    def setup_ui(self):
        # Menu bar
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)
        
        # Status bar
        self.status_label = tk.Label(self.root, text="Not connected", bd=1, relief=tk.SUNKEN, anchor=tk.W)
        self.status_label.pack(side=tk.BOTTOM, fill=tk.X)
        
        # Notebook for tabs
        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        # Create tabs
        self.tab_redundancy = ttk.Frame(self.notebook)
        self.tab_sensors = ttk.Frame(self.notebook)
        self.tab_alarms = ttk.Frame(self.notebook)
        self.tab_alarm_cfg = ttk.Frame(self.notebook)
        
        self.notebook.add(self.tab_redundancy, text="Redundancy")
        self.notebook.add(self.tab_sensors, text="Sensors")
        self.notebook.add(self.tab_alarms, text="Alarms")
        self.notebook.add(self.tab_alarm_cfg, text="Alarm Config")
        
        # Setup each tab
        self.setup_redundancy_tab()
        self.setup_sensors_tab()
        self.setup_alarms_tab()
        self.setup_alarm_cfg_tab()
        
        # Control buttons
        btn_frame = tk.Frame(self.root)
        btn_frame.pack(side=tk.BOTTOM, fill=tk.X, padx=5, pady=5)
        
        self.btn_refresh = tk.Button(btn_frame, text="Refresh", command=self.refresh_all)
        self.btn_refresh.pack(side=tk.LEFT, padx=5)
        
        self.btn_auto = tk.Button(btn_frame, text="Auto Refresh: OFF", command=self.toggle_auto_refresh)
        self.btn_auto.pack(side=tk.LEFT, padx=5)
    
    def setup_redundancy_tab(self):
        frame = self.tab_redundancy
        
        # Mode selection
        mode_frame = tk.LabelFrame(frame, text="Redundancy Mode")
        mode_frame.pack(fill=tk.X, padx=10, pady=10)
        
        self.mode_var = tk.StringVar(value="M")
        tk.Radiobutton(mode_frame, text="Manual", variable=self.mode_var, value="M").pack(side=tk.LEFT, padx=10)
        tk.Radiobutton(mode_frame, text="Automatic", variable=self.mode_var, value="A").pack(side=tk.LEFT, padx=10)
        
        # Antenna selection
        ant_frame = tk.LabelFrame(frame, text="Antenna Selection")
        ant_frame.pack(fill=tk.X, padx=10, pady=5)
        
        tk.Label(ant_frame, text="Antenna A:").grid(row=0, column=0, padx=10, pady=5)
        self.ant_a_var = tk.StringVar(value="BUC A")
        tk.Radiobutton(ant_frame, text="BUC A", variable=self.ant_a_var, value="BUC A").grid(row=0, column=1)
        tk.Radiobutton(ant_frame, text="Spare", variable=self.ant_a_var, value="Spare").grid(row=0, column=2)
        
        tk.Label(ant_frame, text="Antenna B:").grid(row=1, column=0, padx=10, pady=5)
        self.ant_b_var = tk.StringVar(value="BUC B")
        tk.Radiobutton(ant_frame, text="BUC B", variable=self.ant_b_var, value="BUC B").grid(row=1, column=1)
        tk.Radiobutton(ant_frame, text="Spare", variable=self.ant_b_var, value="Spare").grid(row=1, column=2)
        
        # Attenuation
        att_frame = tk.LabelFrame(frame, text="Attenuation (dBm)")
        att_frame.pack(fill=tk.X, padx=10, pady=5)
        
        tk.Label(att_frame, text="Spare→A:").pack(side=tk.LEFT, padx=10)
        self.att_a_var = tk.StringVar(value="0.3")
        tk.Entry(att_frame, textvariable=self.att_a_var, width=10).pack(side=tk.LEFT, padx=5)
        
        tk.Label(att_frame, text="Spare→B:").pack(side=tk.LEFT, padx=10)
        self.att_b_var = tk.StringVar(value="0.2")
        tk.Entry(att_frame, textvariable=self.att_b_var, width=10).pack(side=tk.LEFT, padx=5)
        
        # Apply button
        tk.Button(frame, text="Apply Changes", command=self.apply_redundancy, bg="#4CAF50", fg="white").pack(pady=10)
        
        # Poll feature
        poll_frame = tk.LabelFrame(frame, text="Poll / Test Mode")
        poll_frame.pack(fill=tk.X, padx=10, pady=10)
        
        tk.Label(poll_frame, text="Interval (sec):").grid(row=0, column=0, padx=5, pady=5)
        self.poll_interval = tk.StringVar(value="2")
        tk.Entry(poll_frame, textvariable=self.poll_interval, width=5).grid(row=0, column=1, padx=5)
        
        tk.Label(poll_frame, text="Count:").grid(row=0, column=2, padx=5, pady=5)
        self.poll_count = tk.StringVar(value="10")
        tk.Entry(poll_frame, textvariable=self.poll_count, width=5).grid(row=0, column=3, padx=5)
        
        self.poll_running = False
        self.poll_data = []
        
        tk.Button(poll_frame, text="Start Poll", command=self.start_poll, bg="#2196F3", fg="white").grid(row=0, column=4, padx=10)
        tk.Button(poll_frame, text="Stop", command=self.stop_poll, bg="#f44336", fg="white").grid(row=0, column=5, padx=5)
        tk.Button(poll_frame, text="A→Spare", command=lambda: self.quick_switch("A", "Spare"), bg="#FF9800", fg="white").grid(row=0, column=6, padx=2)
        tk.Button(poll_frame, text="A→BUC A", command=lambda: self.quick_switch("A", "BUC A"), bg="#FF9800", fg="white").grid(row=0, column=7, padx=2)


        # Status display
        status_frame = tk.LabelFrame(frame, text="Current Status")
        status_frame.pack(fill=tk.BOTH, expand=False, padx=10, pady=10)

        self.redundancy_display = tk.Text(status_frame, height=8, width=85)
        self.redundancy_display.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        # Poll results display
        self.poll_display = tk.Text(poll_frame, height=15, width=85)
        self.poll_display.grid(row=1, column=0, columnspan=8, padx=5, pady=5)
    
    def setup_sensors_tab(self):
        frame = self.tab_sensors
        
        # Treeview for sensors
        columns = ("Sensor", "IBUC A", "SPARE", "IBUC B")
        self.sensors_tree = ttk.Treeview(frame, columns=columns, show="headings", height=15)
        
        for col in columns:
            self.sensors_tree.heading(col, text=col)
            self.sensors_tree.column(col, width=150)
        
        self.sensors_tree.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Controller section
        tk.Label(frame, text="Controller Unit", font=("Arial", 12, "bold")).pack(pady=5)
        
        self.controller_display = tk.Text(frame, height=5, width=80)
        self.controller_display.pack(padx=10, pady=5)
    
    def setup_alarms_tab(self):
        frame = self.tab_alarms
        
        # Treeview for alarms
        columns = ("Alarm Type", "IBUC A", "SPARE", "IBUC B")
        self.alarms_tree = ttk.Treeview(frame, columns=columns, show="headings", height=20)
        
        for col in columns:
            self.alarms_tree.heading(col, text=col)
            self.alarms_tree.column(col, width=150)
        
        self.alarms_tree.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
    
    def setup_alarm_cfg_tab(self):
        frame = self.tab_alarm_cfg
        
        # Threshold settings
        threshold_frame = tk.LabelFrame(frame, text="Threshold Settings")
        threshold_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        # Create comboboxes for thresholds
        self.threshold_vars = {}
        
        row = 0
        for alarm_type in ['Temperature', 'Input Low', 'Input High', 'Output Low', 'Output High']:
            tk.Label(threshold_frame, text=alarm_type, width=20).grid(row=row, column=0, padx=5, pady=2)
            row += 1
        
        # Suppressible settings
        suppress_frame = tk.LabelFrame(frame, text="Suppressible Alarms")
        suppress_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        tk.Button(frame, text="Refresh Alarm Config", command=self.refresh_alarm_config).pack(pady=5)
    
    def connect(self):
        if self.client.login():
            self.status_label.config(text=f"Connected to {IBUC_HOST}")
            self.refresh_all()
        else:
            self.status_label.config(text="Connection failed!")
    
    def refresh_all(self):
        self.refresh_redundancy()
        self.refresh_sensors()
        self.refresh_alarms()
    
    def refresh_redundancy(self):
        if not hasattr(self, "redundancy_display"):
            return
         
        data = self.client.get_redundancy()
        if data:
            mode = "Manual" if data.get("bsm_C") == "M" else "Automatic"
            self.mode_var.set(data.get("bsm_C", "M"))
            self.ant_a_var.set(data.get("bsw_C_1", "BUC A"))
            self.ant_b_var.set(data.get("bsw_C_2", "BUC B"))
            self.att_a_var.set(str(data.get("tpta_C", 0)))
            self.att_b_var.set(str(data.get("tptb_C", 0)))
            
            ifl = data.get("ifl_pos", "Unknown")
            
            self.redundancy_display.delete(1.0, tk.END)
            self.redundancy_display.insert(tk.END, f"Mode: {mode}\n")
            self.redundancy_display.insert(tk.END, f"Antenna A: {data.get('bsw_C_1', 'N/A')}\n")
            self.redundancy_display.insert(tk.END, f"Antenna B: {data.get('bsw_C_2', 'N/A')}\n")
            self.redundancy_display.insert(tk.END, f"IF Switch: {ifl}\n")
            self.redundancy_display.insert(tk.END, f"Attenuation A: {data.get('tpta_C', 'N/A')} dBm\n")
            self.redundancy_display.insert(tk.END, f"Attenuation B: {data.get('tptb_C', 'N/A')} dBm\n")
    
    def refresh_sensors(self):
        data = self.client.get_sensors()
        if not data:
            return
        
        # Clear tree
        for item in self.sensors_tree.get_children():
            self.sensors_tree.delete(item)
        
        # Add sensor data
        sensors = [
            ("Supply Voltage (V)", "cvd"),
            ("Supply Current (A)", "cid"),
            ("LO Tuning Voltage (V)", "ctv"),
            ("Tx Input Level (dBm)", "tdt"),
            ("Tx Output Level (dBm)", "tpo"),
            ("Temperature (°C)", "cox"),
        ]
        
        for label, prefix in sensors:
            a = data.get(f'{prefix}_a', 'N/A')
            s = data.get(f'{prefix}_s', 'N/A')
            b = data.get(f'{prefix}_b', 'N/A')
            self.sensors_tree.insert("", tk.END, values=(label, a, s, b))
        
        # Controller
        self.controller_display.delete(1.0, tk.END)
        self.controller_display.insert(tk.END, f"Controller Voltage 1: {data.get('cvd_c_1', 'N/A')} V\n")
        self.controller_display.insert(tk.END, f"Controller Voltage 2: {data.get('cvd_c_2', 'N/A')} V\n")
        self.controller_display.insert(tk.END, f"10MHz Level: {data.get('tml_c', 'N/A')}\n")
        self.controller_display.insert(tk.END, f"10MHz Source: {data.get('tme_c', 'N/A')}\n")
    
    def refresh_alarms(self):
        alarms = self.client.get_alarms()
        
        # Clear tree
        for item in self.alarms_tree.get_children():
            self.alarms_tree.delete(item)
        
        for label, statuses in alarms.items():
            a = statuses.get('IBUC A', '')
            s = statuses.get('SPARE', '')
            b = statuses.get('IBUC B', '')
            self.alarms_tree.insert("", tk.END, values=(label, a, s, b))
    
    def refresh_alarm_config(self):
        config, checkbox_state = self.client.get_alarm_config()
        # Could populate UI here
    
    def apply_redundancy(self):
        mode = self.mode_var.get()
        ant_a = self.ant_a_var.get()
        ant_b = self.ant_b_var.get()
        att_a = self.att_a_var.get()
        att_b = self.att_b_var.get()
        
        success = self.client.set_redundancy(
            mode=mode,
            antenna_a=ant_a,
            antenna_b=ant_b,
            att_a=float(att_a) if att_a else None,
            att_b=float(att_b) if att_b else None
        )
        
        if success:
            messagebox.showinfo("Success", "Redundancy settings updated!")
            self.refresh_redundancy()
        else:
            messagebox.showerror("Error", "Failed to update settings!")
    
    def start_poll(self):
        """Start polling for redundancy switch test"""
        if self.poll_running:
            return
        
        try:
            interval = int(self.poll_interval.get())
            count = int(self.poll_count.get())
        except ValueError:
            messagebox.showerror("Error", "Invalid interval or count!")
            return
        
        self.poll_running = True
        self.poll_count_remaining = count
        self.poll_data = []
        
        self.poll_display.delete(1.0, tk.END)
        self.poll_display.insert(tk.END, f"Starting poll: {count} iterations every {interval}s\n")
        self.poll_display.insert(tk.END, "="*60 + "\n")
        
        self.run_poll_iteration()
    
    def run_poll_iteration(self):
        """Run a single poll iteration"""
        if not self.poll_running or self.poll_count_remaining <= 0:
            self.poll_running = False
            self.poll_display.insert(tk.END, "\n" + "="*60 + "\n")
            self.poll_display.insert(tk.END, "Poll complete!\n")
            return
        
        # Get current status
        data = self.client.get_redundancy()
        
        if data:
            mode = data.get("bsm_C", "?")
            ant_a = data.get("bsw_C_1", "?")
            ant_b = data.get("bsw_C_2", "?")
            ifl = data.get("ifl_pos", "?")
            
            timestamp = time.strftime("%H:%M:%S")
            line = f"[{timestamp}] Mode:{mode} | A:{ant_a:8} | B:{ant_b:8} | IF:{ifl}\n"
            self.poll_display.insert(tk.END, line)
            self.poll_display.see(tk.END)
            
            self.poll_data.append({
                'time': timestamp,
                'mode': mode,
                'ant_a': ant_a,
                'ant_b': ant_b,
                'ifl': ifl
            })
        
        self.poll_count_remaining -= 1
        
        if self.poll_running and self.poll_count_remaining > 0:
            interval = int(self.poll_interval.get()) * 1000
            self.root.after(interval, self.run_poll_iteration)
    
    def stop_poll(self):
        """Stop polling"""
        self.poll_running = False
        self.poll_display.insert(tk.END, "\nPoll stopped by user\n")
        self.poll_display.insert(tk.END, f"Collected {len(self.poll_data)} readings\n")
    
    def quick_switch(self, antenna, target):
        """Quick switch for testing"""
        if antenna == "A":
            ant_a = target
            ant_b = self.ant_b_var.get()
        else:
            ant_a = self.ant_a_var.get()
            ant_b = target
        
        mode = self.mode_var.get()
        
        self.poll_display.insert(tk.END, f"\n>>> Switching Antenna {antenna} to {target}...\n")
        
        success = self.client.set_redundancy(
            mode=mode,
            antenna_a=ant_a,
            antenna_b=ant_b
        )
        
        if success:
            self.poll_display.insert(tk.END, f">>> Switch command sent!\n")
        else:
            self.poll_display.insert(tk.END, f">>> FAILED to send switch command!\n")
    
    def toggle_auto_refresh(self):
        self.auto_refresh = not self.auto_refresh
        if self.auto_refresh:
            self.btn_auto.config(text="Auto Refresh: ON", bg="#4CAF50")
            self.start_auto_refresh()
        else:
            self.btn_auto.config(text="Auto Refresh: OFF")
    
    def start_auto_refresh(self):
        if self.auto_refresh:
            self.refresh_all()
            self.root.after(5000, self.start_auto_refresh)


def main():
    root = tk.Tk()
    app = IBUCGUI(root)
    root.mainloop()


if __name__ == "__main__":
    main()
