
import adi
import numpy as np
import matplotlib.pyplot as plt

# Connect
sdr = adi.Pluto("ip:10.10.12.46")

# Tune near the middle of the FM band
sdr.rx_lo = int(96e6)              # 96 MHz center
sdr.sample_rate = int(1e6)         # 1 MSPS
sdr.rx_rf_bandwidth = int(2e6)
sdr.rx_buffer_size = 16384

# Receive samples
samples = sdr.rx()
phase = np.angle(samples[1:] * np.conj(samples[:-1]))

print(phase[:10])

# FFT
fft_data = np.abs(
    np.fft.fftshift(
        np.fft.fft(samples)
    )
)

# Frequency axis
freq = np.linspace(
    sdr.rx_lo - sdr.sample_rate/2,
    sdr.rx_lo + sdr.sample_rate/2,
    len(fft_data)
)

plt.plot(freq/1e6, 20*np.log10(fft_data))
plt.xlabel("Frequency (MHz)")
plt.ylabel("Power (dB)")
plt.grid()
plt.show()