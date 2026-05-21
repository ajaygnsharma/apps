import adi
import numpy as np

pluto = adi.Pluto("ip:10.10.12.46")

pluto.sample_rate = int(1e6)
pluto.tx_lo = int(433e6)

N = 4096
f = 400e3      # clearly visible offset

t = np.arange(N)

iq = 0.1 * np.exp(
    2j*np.pi*f*t/pluto.sample_rate
)

pluto.tx_destroy_buffer()     # clear old buffer
pluto.tx_cyclic_buffer = True
pluto.tx(iq)

input("Press Enter to stop")

pluto.tx_destroy_buffer()