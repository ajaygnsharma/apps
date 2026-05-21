#!/usr/bin/env python3
"""
RXR Network Configuration - GUI
Tkinter interface for reading and setting IP/mask/gateway on an RXR device.
Serial operations run in a background thread to keep the UI responsive.
"""

import tkinter as tk
from tkinter import ttk
import threading
import serial
import serial.tools.list_ports

from rxr_ip_set import wait_for_prompt, read_param, set_param, PARAMS


class RxrApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("RXR Network Configuration")
        self.resizable(False, False)
        self._build_ui()
        self._populate_ports()

    # ------------------------------------------------------------------
    # UI construction
    # ------------------------------------------------------------------
    def _build_ui(self):
        pad = {"padx": 8, "pady": 4}

        # ── Connection ───────────────────────────────────────────────
        conn = ttk.LabelFrame(self, text="Connection")
        conn.grid(row=0, column=0, sticky="ew", padx=12, pady=(12, 4))

        ttk.Label(conn, text="Port:").grid(row=0, column=0, sticky="w", **pad)
        self.port_var = tk.StringVar()
        self.port_combo = ttk.Combobox(conn, textvariable=self.port_var, width=18)
        self.port_combo.grid(row=0, column=1, sticky="w", **pad)

        ttk.Label(conn, text="Baud:").grid(row=0, column=2, sticky="w", **pad)
        self.baud_var = tk.StringVar(value="115200")
        ttk.Combobox(
            conn, textvariable=self.baud_var, width=10, state="readonly",
            values=["9600", "19200", "38400", "57600", "115200", "230400"],
        ).grid(row=0, column=3, sticky="w", **pad)

        ttk.Button(conn, text="Refresh Ports", command=self._populate_ports).grid(
            row=0, column=4, **pad)

        # ── Current values (read) ────────────────────────────────────
        cur = ttk.LabelFrame(self, text="Current Configuration")
        cur.grid(row=1, column=0, sticky="ew", padx=12, pady=4)

        self.current_vars = {}
        for i, (key, (_, label)) in enumerate(PARAMS.items()):
            ttk.Label(cur, text=f"{label}:", anchor="w", width=14).grid(
                row=i, column=0, sticky="w", **pad)
            var = tk.StringVar(value="—")
            self.current_vars[key] = var
            ttk.Label(cur, textvariable=var, width=20,
                      font=("Courier", 11, "bold"), foreground="#005f87").grid(
                row=i, column=1, sticky="w", **pad)

        ttk.Button(cur, text="Read Device", command=self._start_read).grid(
            row=len(PARAMS), column=0, columnspan=2, pady=6)

        # ── New values (set) ─────────────────────────────────────────
        new = ttk.LabelFrame(self, text="Set New Values  (leave blank to skip)")
        new.grid(row=2, column=0, sticky="ew", padx=12, pady=4)

        self.new_vars = {}
        self.set_status_vars = {}
        labels_order = [("CIA", "IP Address"), ("CIM", "Subnet Mask"), ("CIG", "Gateway")]
        for i, (key, label) in enumerate(labels_order):
            ttk.Label(new, text=f"{label}:", anchor="w", width=14).grid(
                row=i, column=0, sticky="w", **pad)
            var = tk.StringVar()
            self.new_vars[key] = var
            ttk.Entry(new, textvariable=var, width=20).grid(
                row=i, column=1, sticky="w", **pad)
            sv = tk.StringVar(value="")
            self.set_status_vars[key] = sv
            ttk.Label(new, textvariable=sv, width=10).grid(
                row=i, column=2, sticky="w", **pad)

        self.apply_btn = ttk.Button(new, text="Apply Changes", command=self._start_set)
        self.apply_btn.grid(row=len(labels_order), column=0, columnspan=3, pady=6)

        # ── Status bar ───────────────────────────────────────────────
        self.status_var = tk.StringVar(value="Ready.")
        self._status_label = ttk.Label(self, textvariable=self.status_var,
                                       relief="sunken", anchor="w")
        self._status_label.grid(row=3, column=0, sticky="ew", pady=(4, 0))

    def _populate_ports(self):
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.port_combo["values"] = ports
        if ports:
            preferred = next((p for p in ports if "USB" in p), ports[0])
            self.port_var.set(preferred)
        else:
            self.port_var.set("")

    # ------------------------------------------------------------------
    # Serial helpers
    # ------------------------------------------------------------------
    def _open_port(self):
        return serial.Serial(
            port=self.port_var.get().strip(),
            baudrate=int(self.baud_var.get()),
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=0.1,
        )

    def _set_status(self, msg, error=False):
        self.status_var.set(msg)
        self._status_label.config(foreground="red" if error else "black")

    def _lock_buttons(self):
        self.apply_btn.config(state="disabled")

    def _unlock_buttons(self):
        self.apply_btn.config(state="normal")

    # ------------------------------------------------------------------
    # READ
    # ------------------------------------------------------------------
    def _start_read(self):
        port = self.port_var.get().strip()
        if not port:
            self._set_status("No port selected.", error=True)
            return
        for var in self.current_vars.values():
            var.set("…")
        self._lock_buttons()
        self._set_status(f"Reading from {port}…")
        threading.Thread(target=self._run_read, daemon=True).start()

    def _run_read(self):
        try:
            ser = self._open_port()
        except serial.SerialException as e:
            self.after(0, self._set_status, f"Port error: {e}", True)
            self.after(0, self._unlock_buttons)
            return

        try:
            wait_for_prompt(ser)
            for key, (cmd, label) in PARAMS.items():
                value = read_param(ser, cmd, label)
                self.after(0, self.current_vars[key].set, value)
            self.after(0, self._set_status, "Read complete.")
        except Exception as e:
            self.after(0, self._set_status, f"Error: {e}", True)
            for var in self.current_vars.values():
                self.after(0, var.set, "ERROR")
        finally:
            ser.close()
            self.after(0, self._unlock_buttons)

    # ------------------------------------------------------------------
    # SET
    # ------------------------------------------------------------------
    def _start_set(self):
        port = self.port_var.get().strip()
        if not port:
            self._set_status("No port selected.", error=True)
            return

        changes = {k: v.get().strip() for k, v in self.new_vars.items()}
        if not any(changes.values()):
            self._set_status("No new values entered — nothing to set.", error=True)
            return

        for sv in self.set_status_vars.values():
            sv.set("")
        self._lock_buttons()
        self._set_status(f"Applying changes to {port}…")
        threading.Thread(target=self._run_set, args=(changes,), daemon=True).start()

    def _run_set(self, changes: dict):
        try:
            ser = self._open_port()
        except serial.SerialException as e:
            self.after(0, self._set_status, f"Port error: {e}", True)
            self.after(0, self._unlock_buttons)
            return

        try:
            wait_for_prompt(ser)

            for key, (cmd, label) in PARAMS.items():
                new_val = changes[key]
                if not new_val:
                    continue
                confirmed = set_param(ser, cmd, new_val, label)
                if confirmed == new_val:
                    self.after(0, self.set_status_vars[key].set, "OK")
                    self.after(0, self.current_vars[key].set, confirmed)
                else:
                    self.after(0, self.set_status_vars[key].set, "MISMATCH")
                    self.after(0, self.current_vars[key].set, confirmed)

            # Read back remaining params that weren't set
            for key, (cmd, label) in PARAMS.items():
                if not changes[key]:
                    value = read_param(ser, cmd, label)
                    self.after(0, self.current_vars[key].set, value)

            self.after(0, self._set_status, "Done. Device updated successfully.")
        except Exception as e:
            self.after(0, self._set_status, f"Error: {e}", True)
        finally:
            ser.close()
            self.after(0, self._unlock_buttons)


if __name__ == "__main__":
    app = RxrApp()
    app.mainloop()
