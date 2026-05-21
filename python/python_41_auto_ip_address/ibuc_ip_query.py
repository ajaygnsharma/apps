#!/usr/bin/env python3
"""
iBUC IP Address Query Tool
Connects to an iBUC device via serial port, logs in, and retrieves
network configuration (IP address, subnet mask, gateway).

Protocol observed from interceptty serial trace (sniff.log):
  - Login prompt: "ibuC login: "
  - Password prompt: "Password: "
  - Shell prompt: "IBUC>"
  - Read command format: "<param>=C"  (e.g. "cia=C" reads CIA)
  - Response format: "<PARAM>=<value>" (e.g. "CIA=10.10.12.42")
"""

import serial
import time
import argparse
import sys

# Network parameter commands (read current value with =C suffix)
PARAMS = {
    "CIA": ("cia=C", "IP Address"),
    "CIM": ("cim=C", "Subnet Mask"),
    "CIG": ("cig=C", "Gateway"),
}

PROMPT   = b"IBUC>"
LOGIN    = b"login: "
PASSWORD = b"Password: "
TIMEOUT  = 5.0   # seconds to wait for a response


def read_until(ser: serial.Serial, expected: bytes, timeout: float = TIMEOUT) -> bytes:
    """Read from serial until expected bytes appear or timeout expires."""
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


def read_any(ser: serial.Serial, candidates: list, timeout: float = TIMEOUT) -> tuple:
    """Read until one of several candidate byte strings appears. Returns (matched, buf)."""
    buf = b""
    deadline = time.time() + timeout
    while time.time() < deadline:
        chunk = ser.read(ser.in_waiting or 1)
        if chunk:
            buf += chunk
            for candidate in candidates:
                if candidate in buf:
                    return candidate, buf
    return None, buf


def send(ser: serial.Serial, data: str) -> None:
    """Send a string followed by CR."""
    ser.write((data + "\r").encode())
    ser.flush()


def login(ser: serial.Serial, username: str, password: str) -> None:
    """
    Handle login sequence. The device may be in one of several states:
      - Idle / boot: will send the login prompt on its own
      - Already at IBUC> shell prompt (session still active)
      - Waiting after we prod it with a CR
    """
    # Send a CR to prod the device, then wait to see what comes back
    ser.write(b"\r")
    ser.flush()

    print("[*] Probing device state...")
    matched, buf = read_any(ser, [PROMPT, LOGIN, PASSWORD], timeout=TIMEOUT)

    if matched == PROMPT:
        print("[+] Already at shell prompt (session active).")
        return

    if matched == PASSWORD:
        print("[*] Got password prompt, sending password...")
        send(ser, password)
        read_until(ser, PROMPT)
        print("[+] Logged in.")
        return

    if matched == LOGIN:
        print("[*] Got login prompt, authenticating...")
        send(ser, username)
        read_until(ser, PASSWORD)
        send(ser, password)
        read_until(ser, PROMPT)
        print("[+] Logged in successfully.")
        return

    # Nothing recognised — print what we got and fail clearly
    raise RuntimeError(
        f"Device did not respond with a known prompt.\n"
        f"Received: {buf!r}\n"
        f"Check --port, --baud, and cable connection."
    )


def query_param(ser: serial.Serial, cmd: str, label: str) -> str:
    """Send a read command and parse the response value."""
    send(ser, cmd)
    response = read_until(ser, PROMPT)

    # Response contains lines like "CIA=10.10.12.42\r\n"
    # The key is the uppercase version of the param name (without "=C")
    key = cmd.split("=")[0].upper()
    for line in response.decode(errors="replace").splitlines():
        line = line.strip()
        if line.startswith(key + "="):
            return line.split("=", 1)[1].strip()

    raise ValueError(f"Could not parse response for {label}: {response!r}")


def main():
    parser = argparse.ArgumentParser(
        description="Query iBUC network configuration via serial port"
    )
    parser.add_argument(
        "--port", default="/dev/ttyUSB1",
        help="Serial port device (default: /dev/ttyUSB1)"
    )
    parser.add_argument(
        "--baud", type=int, default=115200,
        help="Baud rate (default: 115200)"
    )
    parser.add_argument(
        "--username", default="admin",
        help="Login username (default: admin)"
    )
    parser.add_argument(
        "--password", default="admin",
        help="Login password (default: admin)"
    )
    args = parser.parse_args()

    print(f"[*] Opening {args.port} at {args.baud} baud...")
    try:
        ser = serial.Serial(
            port=args.port,
            baudrate=args.baud,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=0.1,
        )
    except serial.SerialException as e:
        print(f"[!] Failed to open port: {e}", file=sys.stderr)
        sys.exit(1)

    try:
        login(ser, args.username, args.password)

        print()
        print("iBUC Network Configuration")
        print("-" * 35)
        results = {}
        for key, (cmd, label) in PARAMS.items():
            try:
                value = query_param(ser, cmd, label)
                results[key] = value
                print(f"  {label:<14}: {value}")
            except (TimeoutError, ValueError) as e:
                print(f"  {label:<14}: ERROR - {e}")
        print("-" * 35)

    finally:
        ser.close()
        print("\n[*] Port closed.")


if __name__ == "__main__":
    main()
