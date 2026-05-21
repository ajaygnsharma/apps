#!/usr/bin/env python3
"""
RXR IP Configuration Tool
Connects to an RXR device via serial port and reads or sets
network configuration (IP address, subnet mask, gateway).

Protocol observed from interceptty serial trace (sniff_RXR.log):
  - No login required — device goes straight to prompt
  - Prompt:       "RX1+1>"
  - Read syntax:  "<param>"        e.g. "cia"     → "CIA=10.10.12.44"
  - Set syntax:   "<param>=<val>"  e.g. "cia=10.10.12.41" → "CIA=10.10.12.41"
"""

import serial
import time
import argparse
import sys

PROMPT  = b"RX1+1>"
TIMEOUT = 5.0

# param key -> (command_prefix, human label)
PARAMS = {
    "CIA": ("cia", "IP Address"),
    "CIM": ("cim", "Subnet Mask"),
    "CIG": ("cig", "Gateway"),
}


# ── Low-level helpers ──────────────────────────────────────────────────────────

def read_until(ser: serial.Serial, expected: bytes, timeout: float = TIMEOUT) -> bytes:
    buf = b""
    deadline = time.time() + timeout
    while time.time() < deadline:
        chunk = ser.read(ser.in_waiting or 1)
        if chunk:
            buf += chunk
            if expected in buf:
                return buf
    raise TimeoutError(
        f"Timed out waiting for {expected!r}. Got so far: {buf!r}"
    )


def send(ser: serial.Serial, data: str) -> None:
    ser.write((data + "\r").encode())
    ser.flush()


def wait_for_prompt(ser: serial.Serial) -> None:
    """Send a CR and wait for the RXR prompt."""
    ser.write(b"\r")
    ser.flush()
    read_until(ser, PROMPT)


# ── Read / set operations ──────────────────────────────────────────────────────

def read_param(ser: serial.Serial, cmd: str, label: str) -> str:
    """Send a bare read command (e.g. 'cia') and return the value."""
    send(ser, cmd)
    response = read_until(ser, PROMPT)
    key = cmd.upper()
    for line in response.decode(errors="replace").splitlines():
        line = line.strip()
        if line.startswith(key + "="):
            return line.split("=", 1)[1].strip()
    raise ValueError(f"Could not parse response for {label}: {response!r}")


def set_param(ser: serial.Serial, cmd: str, value: str, label: str) -> str:
    """Send a set command (e.g. 'cia=10.0.0.1') and return the confirmed value."""
    send(ser, f"{cmd}={value}")
    response = read_until(ser, PROMPT)
    key = cmd.upper()
    for line in response.decode(errors="replace").splitlines():
        line = line.strip()
        if line.startswith(key + "="):
            return line.split("=", 1)[1].strip()
    raise ValueError(f"No confirmation for {label}: {response!r}")


# ── CLI ────────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(
        description="Read or set RXR network configuration via serial port",
        formatter_class=argparse.RawTextHelpFormatter,
        epilog=(
            "Examples:\n"
            "  Read current config:\n"
            "    python3 rxr_ip_set.py --port /dev/ttyUSB0\n\n"
            "  Set all three values:\n"
            "    python3 rxr_ip_set.py --port /dev/ttyUSB0 "
            "--ip 10.10.12.50 --mask 255.255.0.0 --gw 10.10.10.1\n\n"
            "  Set only IP:\n"
            "    python3 rxr_ip_set.py --port /dev/ttyUSB0 --ip 10.10.12.50\n"
        ),
    )
    parser.add_argument("--port",  default="/dev/ttyUSB0",
                        help="Serial port (default: /dev/ttyUSB0)")
    parser.add_argument("--baud",  type=int, default=115200,
                        help="Baud rate (default: 115200)")
    parser.add_argument("--ip",   metavar="ADDR",
                        help="Set IP address (CIA)")
    parser.add_argument("--mask", metavar="MASK",
                        help="Set subnet mask (CIM)")
    parser.add_argument("--gw",   metavar="GW",
                        help="Set gateway (CIG)")
    args = parser.parse_args()

    changes = {
        "CIA": args.ip,
        "CIM": args.mask,
        "CIG": args.gw,
    }
    write_mode = any(v is not None for v in changes.values())

    print(f"[*] Opening {args.port} at {args.baud} baud...")
    try:
        ser = serial.Serial(
            port=args.port, baudrate=args.baud,
            bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE, timeout=0.1,
        )
    except serial.SerialException as e:
        print(f"[!] Failed to open port: {e}", file=sys.stderr)
        sys.exit(1)

    try:
        print("[*] Waiting for prompt...")
        wait_for_prompt(ser)
        print("[+] Device ready.\n")

        if write_mode:
            # ── SET mode ────────────────────────────────────────────
            print("Setting parameters:")
            print("-" * 40)
            for key, (cmd, label) in PARAMS.items():
                new_val = changes[key]
                if new_val is None:
                    continue
                confirmed = set_param(ser, cmd, new_val, label)
                status = "OK" if confirmed == new_val else f"WARNING: got {confirmed!r}"
                print(f"  {label:<14}: {new_val}  [{status}]")
            print("-" * 40)
            print("\nReading back all values after change:")

        # ── READ (always shown) ──────────────────────────────────────
        print("\nRXR Network Configuration")
        print("-" * 40)
        for key, (cmd, label) in PARAMS.items():
            value = read_param(ser, cmd, label)
            print(f"  {label:<14}: {value}")
        print("-" * 40)

    except (TimeoutError, ValueError, RuntimeError) as e:
        print(f"\n[!] Error: {e}", file=sys.stderr)
        sys.exit(1)
    finally:
        ser.close()
        print("\n[*] Port closed.")


if __name__ == "__main__":
    main()
