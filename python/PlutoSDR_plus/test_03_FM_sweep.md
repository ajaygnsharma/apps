If you see signal energy but no clear peaks, a sweep across the band is a better next step. Also, FM stations are often wide (~200 kHz), so they don't look like thin spikes. They look more like broader "humps" in the spectrum.

Try this sweep script:

This will slowly tune:

```text
88 MHz
89 MHz
90 MHz
...
108 MHz
```

and stitch everything into one plot.

You should end up with something more like:

```text
Power
 ^
 |            ______
 |           /      \        _______
 |    ______/        \______/       \____
 |
 +------------------------------------------------>
   88      92      96      100     104      108 MHz
```

Those wider bumps are likely FM stations.

A few things if you still don't see much:

* Make sure an antenna is attached. Even a short wire can work for nearby stations.
* Turn off AGC initially:

```python
sdr.gain_control_mode_chan0 = "manual"
sdr.rx_hardwaregain_chan0 = 40
```

* Average several FFTs to reduce noise:

```python
avg = np.zeros(sdr.rx_buffer_size)

for i in range(10):
    samples = sdr.rx()
    avg += np.abs(np.fft.fftshift(np.fft.fft(samples)))

avg /= 10
```

That averaging often turns "random grass" into visible stations.

Also, if you're near Gilroy, there should be plenty of FM stations from the South Bay / Monterey area, so with even a simple antenna you should definitely see some structure rather than a flat noise floor.
