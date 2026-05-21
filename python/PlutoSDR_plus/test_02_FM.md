Yes. That's actually one of the best first experiments with Pluto because FM broadcast stations are strong and easy to see.

Since you're already talking to the Pluto successfully, try this:

```python
import adi
import numpy as np
import matplotlib.pyplot as plt

# Connect
sdr = adi.Pluto("ip:10.10.12.46")

# Tune near the middle of the FM band
sdr.rx_lo = int(98e6)              # 98 MHz center
sdr.sample_rate = int(2e6)         # 2 MSPS
sdr.rx_rf_bandwidth = int(2e6)
sdr.rx_buffer_size = 16384

# Receive samples
samples = sdr.rx()

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
```

You should see peaks around stations:

```text
97.7 MHz      |
98.5 MHz      |||||
99.3 MHz      |||
100.1 MHz     ||||||||
```

The peaks are FM stations.

One thing to notice: with:

```python
sdr.sample_rate = 2e6
sdr.rx_lo = 98e6
```

you're only viewing:

```text
97 MHz <-------> 99 MHz
```

because:

```text
center ± sample_rate/2
98MHz ±1MHz
```

You **cannot see the entire 88–108 MHz band at once**, because Pluto's instantaneous bandwidth is limited.

To scan the whole band, sweep the LO:

```python
centers = [89e6, 93e6, 97e6, 101e6, 105e6]
```

receive at each center and stitch results together.

Since you were asking about FFT and ADC concepts earlier, this is also a nice way to think about what Pluto is doing:

```text
RF antenna (98 MHz station)
        ↓
Mixer shifts to baseband
        ↓
ADC
        ↓
I/Q samples
        ↓
FFT in Python
        ↓
Visible spectrum peaks
```

If you want the next step after this, you can go beyond just seeing the station and actually **demodulate the FM audio and listen to it in Python**.
