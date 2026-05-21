import numpy as np
import matplotlib.pyplot as plt

# -----------------------------
# FM demo with time + FFT plots
# -----------------------------

fs = 200_000                 # sample rate
duration = 0.05             # 50 ms for better FFT resolution
t = np.arange(0, duration, 1/fs)

fc = 10_000                 # carrier (10 kHz, visible/demo only)

def fm_signal(t, fc, fm, dev):
    """
    fc  = carrier frequency
    fm  = audio pitch (modulating freq)
    dev = frequency deviation
    """
    beta = dev / fm
    return np.cos(2*np.pi*fc*t + beta*np.sin(2*np.pi*fm*t))

def fft_plot(sig, fs):
    N = len(sig)

    # Windowing improves spectrum appearance
    window = np.hanning(N)
    sigw = sig * window

    fftv = np.fft.rfft(sigw)
    freq = np.fft.rfftfreq(N, d=1/fs)

    mag = np.abs(fftv)
    mag_db = 20*np.log10(mag + 1e-12)

    return freq, mag_db

# -----------------------------
# Test cases
# -----------------------------
cases = [
    ("Low pitch, small deviation",  200,  500),
    ("High pitch, small deviation", 2000, 500),
    ("High pitch, large deviation", 2000, 2500),
]

for title, fm, dev in cases:

    sig = fm_signal(t, fc, fm, dev)
    freq, mag_db = fft_plot(sig, fs)

    # ---- Time domain ----
    plt.figure(figsize=(10,4))
    plt.plot(t[:1500]*1000, sig[:1500])   # zoom first part
    plt.title(title + "  (Time Domain)")
    plt.xlabel("Time (ms)")
    plt.ylabel("Amplitude")
    plt.grid(True)

    # ---- FFT ----
    plt.figure(figsize=(10,4))
    plt.plot(freq, mag_db)
    plt.xlim(fc - 10000, fc + 10000)
    plt.ylim(np.max(mag_db)-80, np.max(mag_db)+5)
    plt.title(title + "  (FFT Spectrum)")
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Magnitude (dB)")
    plt.grid(True)

plt.show()