import adi
import numpy as np
import time

pluto = adi.Pluto("ip:10.10.12.46")
pluto.tx_cyclic_buffer = True

pluto.sample_rate = int(1e6)
pluto.tx_lo = int(433e6)
pluto.tx_hardwaregain_chan0 = -40

N = 16384
f = 100e3
t = np.arange(N)

iq = 0.1 * np.exp(2j*np.pi*f*t/pluto.sample_rate)
iq = iq.astype(np.complex64)

#pluto.tx_destroy_buffer()
pluto.tx(iq)

print("TX running at about 433.100 MHz. Ctrl+C to stop.")

try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    pluto.tx_destroy_buffer()
    print("Stopped")