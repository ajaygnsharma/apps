import numpy as np
import matplotlib.pyplot as plt

# Sampling setup
fs = 200_000          # samples/sec
t = np.arange(0, 0.01, 1/fs)  # 10 ms

# Use a low carrier for visualization.
# Real FM may be 100 MHz, but that is too fast to see clearly.
fc = 10_000          # carrier frequency, 10 kHz

def fm_signal(t, fc, fm, dev):
    """
    fc  = carrier frequency
    fm  = audio/modulating frequency = pitch
    dev = frequency deviation = loudness effect
    """
    beta = dev / fm
    return np.cos(2*np.pi*fc*t + beta*np.sin(2*np.pi*fm*t))

# Case 1: low pitch, small deviation
sig1 = fm_signal(t, fc, fm=200, dev=500)

# Case 2: high pitch, small deviation
sig2 = fm_signal(t, fc, fm=2000, dev=500)

# Case 3: high pitch, large deviation
sig3 = fm_signal(t, fc, fm=2000, dev=2500)

plt.figure()
plt.plot(t * 1000, sig1)
plt.title("Low pitch, small deviation")
plt.xlabel("Time (ms)")
plt.ylabel("Amplitude")
plt.grid(True)

plt.figure()
plt.plot(t * 1000, sig2)
plt.title("High pitch, small deviation")
plt.xlabel("Time (ms)")
plt.ylabel("Amplitude")
plt.grid(True)

plt.figure()
plt.plot(t * 1000, sig3)
plt.title("High pitch, large deviation")
plt.xlabel("Time (ms)")
plt.ylabel("Amplitude")
plt.grid(True)

plt.show()