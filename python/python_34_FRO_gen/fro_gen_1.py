import csv
import configparser

# Read variables from cfg.ini
config = configparser.ConfigParser()
config.optionxform = str  # preserve case sensitivity
config.read("cfg.ini")

# Extract values
model = config["DEFAULT"].get("MODEL").strip('"')
rev = config["DEFAULT"].get("REV").strip('"')
rev_readme = config["DEFAULT"].get("REV_README").strip('"')
family = config["DEFAULT"].get("FAMILY").strip('"')
serial = config["DEFAULT"].get("SERIAL_NUM").strip('"')

# Generate rows
rows = [
    [f"FWF-{model}-0BIN", f"Firmware, {family} {serial}, BIN", rev],
    [f"FRN-{model}-0BIN", f"Frmwr Rls Notes, {family} {serial}", rev],
    [f"ZIP-{model}-0001", f"ZIP file, {family} {serial}, Readme", rev],
    [f"ZIP-{model}-RM01", f"ZIP file, {family} {serial}, Readme", rev_readme],
]

# Write to CSV
with open("output.csv", "w", newline="") as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(["File Name", "Description", "Revision"])
    writer.writerows(rows)

print("✅ output.csv generated successfully.")
