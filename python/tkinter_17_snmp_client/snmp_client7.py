import tkinter as tk
from tkinter import messagebox
import snmpy

# Default values
DEFAULT_IP = "10.10.12.35"
DEFAULT_OID = "SNMPv2-MIB::sysDescr.0"
DEFAULT_COMMUNITY = "public"


def snmp_get(ip, oid, community, version):
    """
    Fetch SNMP data using snmpy.
    """
    try:
        # Create SNMP client
        client = snmpy.Client(ip, community, version=int(version[-1]))  # v1 -> 1, v2c -> 2

        # Fetch SNMP data
        value = client.get(oid)

        return f"{oid} = {value}"

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
# Tkinter GUI Setup (Runs on Main Thread)
# -------------------------------------
root = tk.Tk()
root.title("SNMP Client (Using snmpy)")
root.geometry("400x300")

# IP Address
tk.Label(root, text="IP Address:").pack(pady=5)
ip_entry = tk.Entry(root)
ip_entry.insert(0, DEFAULT_IP)  # Default IP
ip_entry.pack()

# OID
tk.Label(root, text="OID:").pack(pady=5)
oid_entry = tk.Entry(root)
oid_entry.insert(0, DEFAULT_OID)  # Default OID
oid_entry.pack()

# Community String
tk.Label(root, text="Community String:").pack(pady=5)
community_entry = tk.Entry(root)
community_entry.insert(0, DEFAULT_COMMUNITY)  # Default SNMP community
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
