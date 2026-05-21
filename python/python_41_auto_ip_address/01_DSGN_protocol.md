
Documentation for the IBUC IP Configuration Query Protocol
=================================================

This document describes the protocol used by the IBUC (Inmarsat Broadband User Terminal) to query its IP configuration. The protocol is a simple command-response interaction over a serial connection.



Protocol decoded from the trace
Direction	Data	Meaning
> (device)	ibuC login:	Login prompt
< (host)	admin	Username
> (device)	Password:	Password prompt
< (host)	admin	Password
> (device)	IBUC>	Shell prompt
< (host)	cia=C	Read IP Address
> (device)	CIA=10.10.12.42	IP Address
< (host)	cim=C	Read Subnet Mask
> (device)	CIM=255.255.0.0	Subnet Mask
< (host)	cig=C	Read Gateway
> (device)	CIG=10.10.10.1	Gateway
The =C suffix is the device's "read current value" command syntax.

# Install dependency if needed
pip install pyserial

# Run with defaults (port /dev/ttyUSB1, admin/admin)
python3 ibuc_ip_query.py

# Custom port/credentials
python3 ibuc_ip_query.py --port /dev/ttyUSB0 --baud 9600 --username admin --password secret


The standard tool for this is PyInstaller — it bundles Python + all dependencies + your scripts into a single .exe. The catch is you must build on Windows (PyInstaller doesn't cross-compile).

Here's the full path:

Step 1 — Install tools on a Windows machine

pip install pyserial pyinstaller
Copy both ibuc_ip_query.py and ibuc_gui.py to the Windows machine.

Step 2 — Build the .exe

pyinstaller --onefile --windowed --name "iBUC-IP-Config" ibuc_gui.py ibuc_ip_query.py
Flag	Effect
--onefile	Packs everything into a single .exe
--windowed	No black console window behind the GUI
--name	Name of the output .exe
Output lands in dist\iBUC-IP-Config.exe — that's the only file you need to distribute.

Step 3 — Add an icon (optional but professional)
Create or get a .ico file, then:


pyinstaller --onefile --windowed --name "iBUC-IP-Config" --icon=ibuc.ico ibuc_gui.py ibuc_ip_query.py
Step 4 — Test it
Run dist\iBUC-IP-Config.exe directly — no Python install needed on the target machine.

Windows serial ports will show as COM3, COM4, etc. — the port dropdown auto-populates from serial.tools.list_ports so it handles this automatically.

If you don't have a Windows machine handy
You can use GitHub Actions to build it in CI:


# .github/workflows/build.yml
name: Build Windows EXE
on: [push]
jobs:
  build:
    runs-on: windows-latest
    steps:
      - uses: actions/checkout@v4
      - uses: actions/setup-python@v5
        with: { python-version: "3.11" }
      - run: pip install pyserial pyinstaller
      - run: pyinstaller --onefile --windowed --name "iBUC-IP-Config" ibuc_gui.py
      - uses: actions/upload-artifact@v4
        with:
          name: iBUC-IP-Config-Windows
          path: dist/iBUC-IP-Config.exe
Push to GitHub, the action runs, and you download the .exe from the Actions artifacts tab — no Windows machine needed.

One thing to watch: Windows requires a signed driver for USB-to-serial adapters (CP210x, CH340, FTDI). Tell end users to install the driver for their USB-serial chip before plugging in the device. The app itself needs no special permissions.


It bundles the Python interpreter and .pyc bytecode for the OS it runs on. A .exe built on Linux:

Uses Linux ELF binaries internally
Windows cannot execute it
Your options:

Method	Effort	Cost
Run PyInstaller on a Windows machine	Lowest	Free
GitHub Actions (windows-latest runner)	Low	Free
Wine + PyInstaller on Linux	Medium, fragile	Free
Virtual machine (VirtualBox + Windows)	Medium	Free (need Windows license)
GitHub Actions is the easiest path if you have no Windows machine — push the code, the workflow I showed earlier runs on a real Windows VM in the cloud, and you download the .exe from the artifacts tab. Takes about 2-3 minutes.


