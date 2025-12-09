import os
import tkinter as tk
from tkinter import messagebox
from easysnmp import Session

# Default values
DEFAULT_IP = "10.10.12.36"
DEFAULT_OID = "TERRASAT-IBUC-MIB::ibucMute.0"  # Use the custom MIB
DEFAULT_COMMUNITY = "public"

def snmp_get(ip, oid, community, version):
    """
    Fetch SNMP data using easysnmp with custom MIB support.
    """
    try:
        # Correct SNMP version mapping
        version_map = {"1": 1, "2c": 2}
        session = Session(hostname=ip, community=community, version=version_map[version])

        # Set MIB loading path (Optional, if not globally configured)
        os.environ["MIBS"] = "+ALL"  # Loads all available MIBs
        os.environ["MIBDIRS"] = "./mibs/"  # Adjust for your system

        # Fetch the SNMP value
        value = session.get(oid)
        return f"{oid} = {value.value}"

    except Exception as e:
        return f"Error: {str(e)}"

def fetch_snmp_data():
    """
    Fetch SNMP data when the button is clicked.
    """
    ip = ip_entry.get()
    oid = oid_entry.get()
    community = community_entry.get()
    version = version_var.get()

    if not ip or not oid:
        messagebox.showerror("Error", "Please enter both IP and OID!")
        return

    result = snmp_get(ip, oid, community, version)
    result_label.config(text=result, fg="blue")

# -------------------------------------
# Tkinter GUI Setup
# -------------------------------------
root = tk.Tk()
root.title("SNMP Client (Using easysnmp)")
root.geometry("400x300")

# IP Address
tk.Label(root, text="IP Address:").pack(pady=5)
ip_entry = tk.Entry(root)
ip_entry.insert(0, DEFAULT_IP)
ip_entry.pack()

# OID
tk.Label(root, text="OID:").pack(pady=5)
oid_entry = tk.Entry(root)
oid_entry.insert(0, DEFAULT_OID)
oid_entry.pack()

# Community String
tk.Label(root, text="Community String:").pack(pady=5)
community_entry = tk.Entry(root)
community_entry.insert(0, DEFAULT_COMMUNITY)
community_entry.pack()

# SNMP Version
tk.Label(root, text="SNMP Version:").pack(pady=5)
version_var = tk.StringVar(value="2c")
tk.OptionMenu(root, version_var, "1", "2c").pack()

# Fetch Button
fetch_button = tk.Button(root, text="Get SNMP Data", command=fetch_snmp_data)
fetch_button.pack(pady=10)

# Result Display
result_label = tk.Label(root, text="", fg="black")
result_label.pack(pady=10)

# Start Tkinter main loop
root.mainloop()
