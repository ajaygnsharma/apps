import adi
import numpy as np
import matplotlib.pyplot as plt

# Connect to Pluto SDR
sdr = adi.Pluto("ip:10.10.12.46")
# Sometimes just:
# sdr = adi.Pluto()

# Configure
sdr.sample_rate = int(1e6)
sdr.rx_lo = int(100e6)      # Tune to 100 MHz
sdr.rx_rf_bandwidth = int(1e6)
sdr.rx_buffer_size = 4096

# Receive IQ samples
samples = sdr.rx()

print(samples[:10])

# FFT
fft_data = np.fft.fftshift(
    np.abs(np.fft.fft(samples))
)

freq = np.linspace(
    -sdr.sample_rate/2,
    sdr.sample_rate/2,
    len(fft_data)
)

plt.plot(freq/1e6, fft_data)
plt.xlabel("MHz")
plt.ylabel("Magnitude")
plt.show()