import adi
import numpy as np
import matplotlib.pyplot as plt
import time

# Connect Pluto
sdr = adi.Pluto("ip:10.10.12.46")

# SDR settings
sdr.sample_rate = int(2e6)
sdr.rx_rf_bandwidth = int(2e6)
sdr.rx_buffer_size = 8192

# Sweep setup
start_freq = 88e6
stop_freq  = 108e6
step_freq  = 1e6

all_freqs = []
all_power = []

for center in np.arange(start_freq, stop_freq, step_freq):

    print(f"Tuning to {center/1e6:.1f} MHz")

    sdr.rx_lo = int(center)

    # allow tuner/AGC to settle
    time.sleep(0.1)

    samples = sdr.rx()

    fft_data = np.fft.fftshift(
        np.abs(np.fft.fft(samples))
    )

    power_db = 20*np.log10(fft_data + 1)

    freq_axis = np.linspace(
        center - sdr.sample_rate/2,
        center + sdr.sample_rate/2,
        len(power_db)
    )

    all_freqs.extend(freq_axis)
    all_power.extend(power_db)

plt.figure(figsize=(12,6))
plt.plot(np.array(all_freqs)/1e6, all_power)

plt.xlabel("Frequency (MHz)")
plt.ylabel("Power (dB)")
plt.title("FM Band Sweep")
plt.grid()
plt.show()