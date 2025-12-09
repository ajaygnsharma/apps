import os
import re
import tkinter as tk
from tkinter import messagebox
from easysnmp import Session

# -----------------------------------
# Set environment variables for MIB loading.
# -----------------------------------
# Ensure your custom MIB file is located in "./mibs"
os.environ["MIBDIRS"] = os.path.abspath("/home/asharma/")
# Tell Net-SNMP to load only the TERRASAT-IBUC-MIB module.
os.environ["MIBS"] = "TERRASAT-IBUC-MIB"

def load_all_oids():
    """
    Loads all OIDs from the TERRASAT-IBUC-MIB file by parsing it for lines
    that define OBJECT-TYPE. Returns a sorted list of object names.
    """
    mib_path = os.path.join(os.path.abspath("/home/asharma/.mibs/"), "TERRASAT-IBUC-MIB.MIB")
    try:
        with open(mib_path, "r") as f:
            content = f.read()
        # Use a regex to capture the object names that are defined as OBJECT-TYPE.
        # This regex finds lines that start with a non-space word followed by "OBJECT-TYPE".
        oids = re.findall(r"^\s*(\S+)\s+OBJECT-TYPE", content, re.MULTILINE)
        # Remove duplicates and sort the list.
        oids = sorted(set(oids))
        return oids
    except Exception as e:
        messagebox.showerror("Error", f"Failed to load MIB file: {e}")
        return []

# Default values for SNMP parameters.
DEFAULT_IP = "10.10.12.36"
DEFAULT_COMMUNITY = "public"

def snmp_get(ip, oid, community, version):
    """
    Performs an SNMP GET using easysnmp.
    """
    try:
        # Map the version string to the corresponding integer version.
        version_map = {"1": 1, "2c": 2}
        session = Session(hostname=ip, community=community, version=version_map[version])
        # Fetch the SNMP value.
        value = session.get(oid)
        return f"{oid} = {value.value}"
    except Exception as e:
        return f"Error: {str(e)}"

def fetch_snmp_data():
    """
    Called when the user clicks the button to fetch SNMP data.
    """
    ip = ip_entry.get()
    oid = oid_var.get()  # OID is taken from the drop-down selection.
    community = community_entry.get()
    version = version_var.get()
    if not ip or not oid:
        messagebox.showerror("Error", "Please enter both IP and OID!")
        return
    result = snmp_get(ip, oid, community, version)
    result_label.config(text=result, fg="blue")

# -------------------------------
# Tkinter GUI Setup
# -------------------------------
root = tk.Tk()
root.title("SNMP Client (Using easysnmp)")
root.geometry("500x400")

# IP Address input
tk.Label(root, text="IP Address:").pack(pady=5)
ip_entry = tk.Entry(root)
ip_entry.insert(0, DEFAULT_IP)
ip_entry.pack()

# OID drop-down populated from the MIB file
tk.Label(root, text="Select OID:").pack(pady=5)
oid_list = load_all_oids()
# If OIDs are successfully loaded, build the fully qualified names (adding the module name and a trailing .0)
if oid_list:
    # For example, "TERRASAT-IBUC-MIB::ibucMute.0" etc.
    oid_options = [f"TERRASAT-IBUC-MIB::{oid}.0" for oid in oid_list]
    oid_var = tk.StringVar(root)
    oid_var.set(oid_options[0])
else:
    oid_var = tk.StringVar(root)
    oid_var.set("TERRASAT-IBUC-MIB::ibucMute.0")
oid_menu = tk.OptionMenu(root, oid_var, *oid_options)
oid_menu.pack(pady=5)

# Community String input
tk.Label(root, text="Community String:").pack(pady=5)
community_entry = tk.Entry(root)
community_entry.insert(0, DEFAULT_COMMUNITY)
community_entry.pack()

# SNMP Version drop-down
tk.Label(root, text="SNMP Version:").pack(pady=5)
version_var = tk.StringVar(root, value="2c")
tk.OptionMenu(root, version_var, "1", "2c").pack(pady=5)

# Button to fetch SNMP data
fetch_button = tk.Button(root, text="Get SNMP Data", command=fetch_snmp_data)
fetch_button.pack(pady=10)

# Label to display the result
result_label = tk.Label(root, text="", fg="black")
result_label.pack(pady=10)

root.mainloop()
