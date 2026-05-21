#!/usr/bin/env python3
"""
iBUC Network Configuration - GUI
Wraps ibuc_ip_query.py with a Tkinter interface.
Runs the serial query in a background thread to keep the UI responsive.
"""

import tkinter as tk
from tkinter import ttk
import threading
import serial
import serial.tools.list_ports

# Reuse the serial logic from ibuc_ip_query
from ibuc_ip_query import login, query_param, PARAMS


class IbucApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("iBUC Network Configuration")
        self.resizable(False, False)
        self._build_ui()
        self._populate_ports()

    # ------------------------------------------------------------------
    # UI construction
    # ------------------------------------------------------------------
    def _build_ui(self):
        pad = {"padx": 8, "pady": 4}

        # ── Connection settings ──────────────────────────────────────
        conn_frame = ttk.LabelFrame(self, text="Connection")
        conn_frame.grid(row=0, column=0, sticky="ew", padx=12, pady=(12, 4))

        ttk.Label(conn_frame, text="Port:").grid(row=0, column=0, sticky="w", **pad)
        self.port_var = tk.StringVar()
        self.port_combo = ttk.Combobox(conn_frame, textvariable=self.port_var, width=18)
        self.port_combo.grid(row=0, column=1, sticky="w", **pad)

        ttk.Label(conn_frame, text="Baud:").grid(row=0, column=2, sticky="w", **pad)
        self.baud_var = tk.StringVar(value="115200")
        baud_combo = ttk.Combobox(
            conn_frame, textvariable=self.baud_var, width=10,
            values=["9600", "19200", "38400", "57600", "115200", "230400"],
            state="readonly"
        )
        baud_combo.grid(row=0, column=3, sticky="w", **pad)

        ttk.Label(conn_frame, text="Username:").grid(row=1, column=0, sticky="w", **pad)
        self.user_var = tk.StringVar(value="admin")
        ttk.Entry(conn_frame, textvariable=self.user_var, width=14).grid(
            row=1, column=1, sticky="w", **pad)

        ttk.Label(conn_frame, text="Password:").grid(row=1, column=2, sticky="w", **pad)
        self.pass_var = tk.StringVar(value="admin")
        ttk.Entry(conn_frame, textvariable=self.pass_var, show="*", width=14).grid(
            row=1, column=3, sticky="w", **pad)

        # ── Results ──────────────────────────────────────────────────
        res_frame = ttk.LabelFrame(self, text="Network Configuration")
        res_frame.grid(row=1, column=0, sticky="ew", padx=12, pady=4)

        labels = [("IP Address", "CIA"), ("Subnet Mask", "CIM"), ("Gateway", "CIG")]
        self.result_vars = {}
        for i, (label, key) in enumerate(labels):
            ttk.Label(res_frame, text=f"{label}:", anchor="w", width=14).grid(
                row=i, column=0, sticky="w", **pad)
            var = tk.StringVar(value="—")
            self.result_vars[key] = var
            ttk.Label(res_frame, textvariable=var, width=20,
                      font=("Courier", 11, "bold"), foreground="#005f87").grid(
                row=i, column=1, sticky="w", **pad)

        # ── Buttons ───────────────────────────────────────────────────
        btn_frame = ttk.Frame(self)
        btn_frame.grid(row=2, column=0, pady=4)

        self.query_btn = ttk.Button(btn_frame, text="Query Device", command=self._start_query)
        self.query_btn.grid(row=0, column=0, padx=6)

        ttk.Button(btn_frame, text="Refresh Ports", command=self._populate_ports).grid(
            row=0, column=1, padx=6)

        # ── Status bar ────────────────────────────────────────────────
        self.status_var = tk.StringVar(value="Ready.")
        status_bar = ttk.Label(self, textvariable=self.status_var,
                               relief="sunken", anchor="w")
        status_bar.grid(row=3, column=0, sticky="ew", padx=0, pady=(4, 0))

    def _populate_ports(self):
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.port_combo["values"] = ports
        if ports:
            # Pre-select ttyUSB0/ttyUSB1 if present, else first
            preferred = next((p for p in ports if "USB" in p), ports[0])
            self.port_var.set(preferred)
        else:
            self.port_var.set("")

    # ------------------------------------------------------------------
    # Query logic (runs in a background thread)
    # ------------------------------------------------------------------
    def _start_query(self):
        port = self.port_var.get().strip()
        if not port:
            self._set_status("No port selected.", error=True)
            return

        self.query_btn.config(state="disabled")
        for var in self.result_vars.values():
            var.set("…")
        self._set_status(f"Connecting to {port}…")

        thread = threading.Thread(target=self._run_query, daemon=True)
        thread.start()

    def _run_query(self):
        port   = self.port_var.get().strip()
        baud   = int(self.baud_var.get())
        user   = self.user_var.get()
        passwd = self.pass_var.get()

        try:
            ser = serial.Serial(
                port=port, baudrate=baud,
                bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE, timeout=0.1,
            )
        except serial.SerialException as e:
            self.after(0, self._set_status, f"Port error: {e}", True)
            self.after(0, self.query_btn.config, {"state": "normal"})
            return

        try:
            self.after(0, self._set_status, "Logging in…")
            login(ser, user, passwd)
            self.after(0, self._set_status, "Reading parameters…")

            results = {}
            for key, (cmd, label) in PARAMS.items():
                value = query_param(ser, cmd, label)
                results[key] = value
                self.after(0, self.result_vars[key].set, value)

            self.after(0, self._set_status,
                       f"Done. IP={results.get('CIA','?')}  "
                       f"Mask={results.get('CIM','?')}  "
                       f"GW={results.get('CIG','?')}")
        except Exception as e:
            self.after(0, self._set_status, f"Error: {e}", True)
            for var in self.result_vars.values():
                self.after(0, var.set, "ERROR")
        finally:
            ser.close()
            self.after(0, self.query_btn.config, {"state": "normal"})

    def _set_status(self, msg: str, error: bool = False):
        self.status_var.set(msg)
        # Find the status label and colour it
        for widget in self.winfo_children():
            if isinstance(widget, ttk.Label) and widget.cget("relief") == "sunken":
                widget.config(foreground="red" if error else "black")
                break


if __name__ == "__main__":
    app = IbucApp()
    app.mainloop()
