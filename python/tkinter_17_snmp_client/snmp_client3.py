import tkinter as tk
from tkinter import messagebox
import asyncio
from pysnmp.hlapi.v3arch.asyncio import (
    SnmpEngine,
    get_cmd,
    CommunityData,
    UdpTransportTarget,
    ContextData,
    ObjectType,
    ObjectIdentity,
)

# Get the running asyncio event loop
loop = asyncio.new_event_loop()
asyncio.set_event_loop(loop)

async def snmp_get(oid, ip, community, version):
    """
    Asynchronous SNMP GET request using pysnmp 7.1.16.
    """
    try:
        snmp_engine = SnmpEngine()

        # SNMP v1 or v2c selection
        community_data = CommunityData(community, mpModel=0 if version == "v1" else 1)

        # Create UDP target (must be awaited)
        udp_target = await UdpTransportTarget.create((ip, 161))

        # Perform SNMP GET
        iterator = get_cmd(
            snmp_engine,
            community_data,
            udp_target,
            ContextData(),
            ObjectType(ObjectIdentity(oid)),
        )

        error_indication, error_status, error_index, var_binds = await iterator

        # Close SNMP engine
        snmp_engine.close_dispatcher()

        # Handle response
        if error_indication:
            return f"Error: {error_indication}"
        elif error_status:
            return f"Error: {error_status.prettyPrint()}"
        else:
            return f"{var_binds[0][0]} = {var_binds[0][1]}"

    except Exception as e:
        return f"Exception: {str(e)}"

def fetch_snmp_data():
    """Fetch SNMP data and update the GUI asynchronously."""
    ip = ip_entry.get()
    oid = oid_entry.get()
    community = community_entry.get()
    version = version_var.get()

    if not ip or not oid:
        messagebox.showerror("Error", "Please enter both IP and OID!")
        return

    # Run the async function inside Tkinter's event loop safely
    asyncio.run_coroutine_threadsafe(update_gui(ip, oid, community, version), loop)

async def update_gui(ip, oid, community, version):
    """Runs the SNMP GET request and updates the GUI result label asynchronously."""
    result = await snmp_get(oid, ip, community, version)
    result_label.config(text=result, fg="blue")

# -------------------------------------
# Tkinter GUI Setup (Runs on Main Thread)
# -------------------------------------
root = tk.Tk()
root.title("Async SNMP Client")
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

# Start asyncio event loop in a separate thread
def start_asyncio_loop():
    loop.run_forever()

import threading
asyncio_thread = threading.Thread(target=start_asyncio_loop, daemon=True)
asyncio_thread.start()

# Start Tkinter main loop
root.mainloop()
