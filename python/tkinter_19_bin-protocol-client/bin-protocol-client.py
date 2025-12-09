import serial
import struct
import tkinter as tk
from tkinter import ttk, messagebox

# Serial port settings
SERIAL_PORT = "/dev/ttyS0"  # Change as needed
BAUD_RATE = 9600
BUFFER_SIZE = 1024
SOP = 0x5454

# Initialize serial connection (Not open initially)
ser = None

# GUI Application
class SerialGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Serial Packet Reader")

        # Serial Connection Frame
        connection_frame = ttk.LabelFrame(root, text="Serial Connection")
        connection_frame.grid(row=0, column=0, padx=10, pady=5, sticky="w")

        self.port_label = ttk.Label(connection_frame, text="Port:")
        self.port_label.grid(row=0, column=0)
        self.port_entry = ttk.Entry(connection_frame)
        self.port_entry.insert(0, SERIAL_PORT)
        self.port_entry.grid(row=0, column=1)

        self.baud_label = ttk.Label(connection_frame, text="Baud:")
        self.baud_label.grid(row=0, column=2)
        self.baud_entry = ttk.Entry(connection_frame)
        self.baud_entry.insert(0, str(BAUD_RATE))
        self.baud_entry.grid(row=0, column=3)

        self.connect_button = ttk.Button(connection_frame, text="Connect", command=self.connect_serial)
        self.connect_button.grid(row=0, column=4, padx=10)

        # Packet Display Frame
        packet_frame = ttk.LabelFrame(root, text="Packet Details")
        packet_frame.grid(row=1, column=0, padx=10, pady=5, sticky="w")

        labels = ["SOP", "Type", "Sub Type", "Sequence", "Message Count", "Checksum"]
        self.packet_fields = {}

        for i, label in enumerate(labels):
            ttk.Label(packet_frame, text=label).grid(row=i, column=0, padx=5, pady=2, sticky="w")
            entry = ttk.Entry(packet_frame, width=30)
            entry.grid(row=i, column=1, padx=5, pady=2)
            self.packet_fields[label] = entry

        # Messages Section
        self.message_label = ttk.Label(packet_frame, text="Messages")
        self.message_label.grid(row=len(labels), column=0, padx=5, pady=2, sticky="w")

        self.message_text = tk.Text(packet_frame, height=5, width=50)
        self.message_text.grid(row=len(labels), column=1, padx=5, pady=2)

        # Start Listening Button
        self.listen_button = ttk.Button(root, text="Start Listening", command=self.start_listening)
        self.listen_button.grid(row=2, column=0, padx=10, pady=5)

    def connect_serial(self):
        """Connect to the serial port."""
        global ser
        port = self.port_entry.get()
        baud = int(self.baud_entry.get())

        try:
            ser = serial.Serial(port, baud, timeout=1)
            messagebox.showinfo("Success", f"Connected to {port} at {baud} baud")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to connect: {str(e)}")

    def start_listening(self):
        """Start listening for serial data."""
        if ser is None or not ser.is_open:
            messagebox.showerror("Error", "Serial port not connected!")
            return
        self.root.after(100, self.read_serial_data)

    def read_serial_data(self):
        """Read data from serial port and update the UI."""
        if ser and ser.is_open:
            data = ser.read(BUFFER_SIZE)
            if data:
                parsed_packet = self.parse_packet(data)
                if parsed_packet:
                    self.update_ui(parsed_packet)
        self.root.after(100, self.read_serial_data)

    def parse_packet(self, data):
        """Parse the received packet."""
        if len(data) < 6:
            return None  # Packet too small

        try:
            offset = 0
            sop, = struct.unpack(">H", data[offset:offset+2])  # Big-endian 16-bit SOP
            offset += 2

            pkt_type = data[offset]
            sub_type = data[offset + 1]
            sequence = data[offset + 2]
            message_count = data[offset + 3]
            offset += 4

            messages = []
            for _ in range(message_count):
                if offset + 4 > len(data):
                    return None  # Incomplete message

                length = data[offset]
                msg_id = data[offset + 1]
                payload_type = data[offset + 2]
                offset += 3

                if offset + length + 1 > len(data):
                    return None  # Incomplete payload

                payload = list(data[offset:offset + length])
                offset += length
                checksum = data[offset]
                offset += 1

                messages.append({
                    "length": length,
                    "id": msg_id,
                    "payload_type": payload_type,
                    "payload": payload,
                    "checksum": checksum
                })

            if offset + 2 > len(data):
                return None  # Incomplete packet checksum

            checksum, = struct.unpack(">H", data[offset:offset+2])  # Big-endian 16-bit checksum

            return {
                "sop": sop,
                "type": pkt_type,
                "sub_type": sub_type,
                "sequence": sequence,
                "message_count": message_count,
                "messages": messages,
                "checksum": checksum
            }
        except Exception as e:
            print(f"Packet parsing error: {e}")
            return None

    def update_ui(self, packet):
        """Update GUI fields with packet data."""
        self.packet_fields["SOP"].delete(0, tk.END)
        self.packet_fields["SOP"].insert(0, f"0x{packet['sop']:04X}")

        self.packet_fields["Type"].delete(0, tk.END)
        self.packet_fields["Type"].insert(0, f"0x{packet['type']:02X}")

        self.packet_fields["Sub Type"].delete(0, tk.END)
        self.packet_fields["Sub Type"].insert(0, f"0x{packet['sub_type']:02X}")

        self.packet_fields["Sequence"].delete(0, tk.END)
        self.packet_fields["Sequence"].insert(0, str(packet["sequence"]))

        self.packet_fields["Message Count"].delete(0, tk.END)
        self.packet_fields["Message Count"].insert(0, str(packet["message_count"]))

        self.packet_fields["Checksum"].delete(0, tk.END)
        self.packet_fields["Checksum"].insert(0, f"0x{packet['checksum']:04X}")

        # Update Messages Section
        self.message_text.delete("1.0", tk.END)
        for msg in packet["messages"]:
            self.message_text.insert(tk.END, f"ID: {msg['id']} | Payload: {msg['payload']} | Checksum: 0x{msg['checksum']:02X}\n")


# Run GUI
if __name__ == "__main__":
    root = tk.Tk()
    app = SerialGUI(root)
    root.mainloop()
