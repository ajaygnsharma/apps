import adi
import numpy as np
import time

# Connect to Pluto
sdr = adi.Pluto("ip:10.10.12.48")

sample_rate = 1_000_000
center_freq = 1_000_000_000
tone_offset = 100_000   # 100 kHz away from center

sdr.sample_rate = sample_rate
sdr.tx_lo = center_freq
sdr.tx_rf_bandwidth = sample_rate
sdr.tx_hardwaregain_chan0 = -10  # start low

# Generate complex IQ tone at +100 kHz
N = 1024
t = np.arange(N) / sample_rate
iq = 0.2 * np.exp(2j * np.pi * tone_offset * t)

# Pluto wants int16-ish IQ range
iq *= 2**14

sdr.tx_cyclic_buffer = True
sdr.tx(iq)

print("Transmitting at 433.100 MHz")
print("Press Ctrl+C to stop")

try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    sdr.tx_destroy_buffer()
    print("Stopped")
