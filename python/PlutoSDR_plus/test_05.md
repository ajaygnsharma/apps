That looks good — you transmitted something and the RTL received it.

What I see in your plot:

* **Large peak at exactly ~433.0 MHz** → this is almost certainly your Pluto transmission ✔
* **Smaller peak near ~432.0 MHz** → likely a real signal in the area or an image/spur
* **Noise floor around -25 to -30 dB** → normal for a normalized display

One thing stands out: I expected your test tone to appear around:

```text
433.100 MHz
```

because earlier the code used:

```python
tone = 100e3
```

but your peak is centered at **433.0 MHz**, which suggests one of these happened:

1. The transmitted tone became **DC (0 Hz)** accidentally:

```python
iq = np.ones(N)
```

or

```python
tone = 0
```

2. Pluto's TX buffer reused an earlier waveform

3. The frequency offset wasn't applied correctly

Let's verify with this explicit transmitter:

```python
import adi
import numpy as np

pluto = adi.Pluto("ip:10.10.12.46")

pluto.sample_rate = int(1e6)
pluto.tx_lo = int(433e6)

N = 4096
f = 200e3      # clearly visible offset

t = np.arange(N)

iq = 0.1 * np.exp(
    2j*np.pi*f*t/pluto.sample_rate
)

pluto.tx_destroy_buffer()     # clear old buffer
pluto.tx_cyclic_buffer = True
pluto.tx(iq)

input("Press Enter to stop")

pluto.tx_destroy_buffer()
```

Now you should see the peak move to:

```text
433.200 MHz
```

If it moves, then your whole chain is working perfectly.

Also, you just did something pretty neat: you built a **two-SDR RF link**:

```text
Python → Pluto TX
        ↓ RF @433MHz ↓
Python ← RTL-SDR RX
```

Next steps after this are things like:

* AM modulation
* FSK (simple data)
* QPSK
* FM audio
* decoding real devices at 433 MHz (weather sensors, remotes, etc.)
