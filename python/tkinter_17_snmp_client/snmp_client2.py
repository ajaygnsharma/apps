import tkinter as tk
from tkinter import messagebox
from pysnmp.hlapi import (
    SnmpEngine,
    getCmd,
    CommunityData,
    UdpTransportTarget,
    ContextData,
    ObjectType,
    ObjectIdentity
)

def snmp_get(oid, ip, community, version):
    """Function to perform an SNMP GET request"""
    try:
        if version == "v1":
            snmp_ver = SnmpEngine()
        else:
            snmp_ver = SnmpEngine()

        iterator = getCmd(
            snmp_ver,
            CommunityData(community),
            UdpTransportTarget((ip, 161)),
            ContextData(),
            ObjectType(ObjectIdentity(oid))
        )

        error_indication, error_status, error_index, var_binds = next(iterator)

        if error_indication:
            return f"Error: {error_indication}"
        elif error_status:
            return f"Error: {error_status.prettyPrint()}"
        else:
            return f"{var_binds[0][0]} = {var_binds[0][1]}"

    except Exception as e:
        return f"Exception: {str(e)}"

def fetch_snmp_data():
    """Callback function to fetch SNMP data on button click"""
    ip = ip_entry.get()
    oid = oid_entry.get()
    community = community_entry.get()
    version = version_var.get()

    if not ip or not oid:
        messagebox.showerror("Error", "Please enter both IP and OID!")
        return

    result = snmp_get(oid, ip, community, version)
    result_label.config(text=result, fg="blue")

# Tkinter GUI Setup
root = tk.Tk()
root.title("SNMP Client")
root.geometry("400x300")

# IP Address
tk.Label(root, text="IP Address:").pack(pady=5)
ip_entry = tk.Entry(root)
ip_entry.pack()

# OID
tk.Label(root, text="OID:").pack(pady=5)
oid_entry = tk.Entry(root)
oid_entry.pack()

# Community String
tk.Label(root, text="Community String:").pack(pady=5)
community_entry = tk.Entry(root)
community_entry.insert(0, "public")  # Default SNMP community
community_entry.pack()

# SNMP Version
tk.Label(root, text="SNMP Version:").pack(pady=5)
version_var = tk.StringVar(value="v2c")
tk.OptionMenu(root, version_var, "v1", "v2c").pack()

# Fetch Button
fetch_button = tk.Button(root, text="Get SNMP Data", command=fetch_snmp_data)
fetch_button.pack(pady=10)

# Result Display
result_label = tk.Label(root, text="", fg="black")
result_label.pack(pady=10)

root.mainloop()
