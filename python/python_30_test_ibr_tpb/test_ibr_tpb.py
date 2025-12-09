import telnetlib
import time

# === Configuration ===
HOST = "10.10.12.43"     # Replace with your device's IP address
PORT = 23                  # Default Telnet port
PASSWORD = "1234"          # Your CPE password

# === Connect to Telnet ===
tn = telnetlib.Telnet(HOST, PORT, timeout=10)

# === Step 1: Send blank line ===
tn.write(b"\r\n")
time.sleep(1)  # Wait for device to respond

# === Step 2: Read until prompt or error ===
response = tn.read_until(b"IBUC>", timeout=5)
print("Received:", response.decode(errors='ignore'))

# === Step 3: Send password ===
login_cmd = f"CPE={PASSWORD}\r\n"
tn.write(login_cmd.encode())
time.sleep(1)

# === Step 4: Wait for final prompt ===
response = tn.read_until(b"IBUC>", timeout=5)
print("Received after login:", response.decode(errors='ignore'))


for i in range(0, 100):

    login_cmd = f"TPB\r\n"
    tn.write(login_cmd.encode())
    time.sleep(0.2)

    # === Step 4: Wait for final prompt ===
    response = tn.read_until(b"IBUC>", timeout=5)
    print(response.decode(errors='ignore'))
    time.sleep(2)


# === Optionally, stay connected or close ===
tn.close()
