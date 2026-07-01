import adi
import numpy as np

pluto = adi.Pluto("ip:10.10.12.46")

pluto.sample_rate = int(1e6)
pluto.tx_lo = int(433e6)

# Keep transmit level low
tone_freq = 100e3      # 100 kHz offset
N = 4096

t = np.arange(N)

iq = 0.05 * np.exp(
    2j*np.pi*tone_freq*t/pluto.sample_rate
)

pluto.tx_cyclic_buffer = True
pluto.tx(iq)

print("Transmitting...")
input("Press Enter to stop")

pluto.tx_destroy_buffer()
