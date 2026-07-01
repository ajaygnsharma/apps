from rtlsdr import RtlSdr
import numpy as np
import matplotlib.pyplot as plt

rtl = RtlSdr()

rtl.sample_rate = 2.4e6
rtl.center_freq = 433e6
rtl.gain = 20

samples = rtl.read_samples(32768)

window = np.hanning(len(samples))

fft = np.fft.fftshift(
    np.fft.fft(samples * window)
)

power = 20*np.log10(np.abs(fft)+1e-12)
power -= np.max(power)

freq = np.linspace(
    rtl.center_freq - rtl.sample_rate/2,
    rtl.center_freq + rtl.sample_rate/2,
    len(power)
)

plt.plot(freq/1e6, power)
plt.xlabel("Frequency (MHz)")
plt.ylabel("Relative dB")
plt.grid()
plt.show()

rtl.close()