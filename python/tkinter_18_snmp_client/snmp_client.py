import os
import re
import tkinter as tk
from tkinter import ttk
from tkinter import messagebox
from easysnmp import Session

def main():
    # -----------------------------------
    # Set environment variables for MIB loading.
    # -----------------------------------
    os.environ["MIBDIRS"] = os.path.abspath("/home/asharma/")
    os.environ["MIBS"] = "TERRASAT-IBUC-MIB"

    # Create the main window before setting style
    root = tk.Tk()
    root.title("SNMP Client (Using easysnmp)")
    root.geometry("600x600")

    # Create a style object using the root as master and set the theme.
    style = ttk.Style(root)
    style.theme_use('alt')  # choose among 'clam', 'alt', 'default', 'classic'

    def load_all_oids():
        """
        Loads all OIDs from the TERRASAT-IBUC-MIB file by parsing it for lines
        that define OBJECT-TYPE. Returns a sorted list of object names.
        """
        mib_path = os.path.join(os.path.abspath("/home/asharma/.mibs/"), "TERRASAT-IBUC-MIB.MIB")
        try:
            with open(mib_path, "r") as f:
                content = f.read()
            oids = re.findall(r"^\s*(\S+)\s+OBJECT-TYPE", content, re.MULTILINE)
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
            version_map = {"1": 1, "2c": 2}
            session = Session(hostname=ip, community=community, version=version_map[version])
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
        # result_label.config(text=result, fg="blue")
        result_label.config(text=result, foreground="blue")

    # -------------------------------------
    # Build the GUI using ttk widgets where possible.
    # -------------------------------------
    ttk.Label(root, text="IP Address:").pack(pady=5)
    ip_entry = ttk.Entry(root)
    ip_entry.insert(0, DEFAULT_IP)
    ip_entry.pack()

    ttk.Label(root, text="Select OID:").pack(pady=5)
    oid_list = load_all_oids()
    if oid_list:
        oid_options = [f"TERRASAT-IBUC-MIB::{oid}.0" for oid in oid_list]
        oid_var = tk.StringVar(root)
        oid_var.set(oid_options[0])
    else:
        oid_var = tk.StringVar(root)
        oid_var.set("TERRASAT-IBUC-MIB::ibucMute.0")
    oid_menu = ttk.OptionMenu(root, oid_var, oid_var.get(), *oid_options)
    oid_menu.pack(pady=5)

    ttk.Label(root, text="Community String:").pack(pady=5)
    community_entry = ttk.Entry(root)
    community_entry.insert(0, DEFAULT_COMMUNITY)
    community_entry.pack()

    ttk.Label(root, text="SNMP Version:").pack(pady=5)
    version_var = tk.StringVar(root, value="2c")
    ttk.OptionMenu(root, version_var, "2c", "1", "2c").pack(pady=5)

    ttk.Button(root, text="Get SNMP Data", command=fetch_snmp_data).pack(pady=10)
    result_label = ttk.Label(root, text="", foreground="blue")
    result_label.pack(pady=10)

    root.mainloop()

if __name__ == '__main__':
    main()
