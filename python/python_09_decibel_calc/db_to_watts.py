
sensor_value = -77

dbm = float(sensor_value)
watts = (10 ** (dbm / 10)) / 1000

if watts >= 1:
    watts_str = f"{watts:.2f} W"
elif watts >= 0.001:
    watts_str = f"{watts * 1000:.2f} mW"
elif watts >= 0.000_001:
    watts_str = f"{watts * 1_000_000:.2f} µW"
elif watts >= 0.000_000_001:
    watts_str = f"{watts * 1_000_000_000:.2f} nW"
else:
    watts_str = f"{watts * 1_000_000_000_000:.2f} pW"

print(f"{dbm:.2f} dBm ({watts_str})")