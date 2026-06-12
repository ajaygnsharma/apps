These are some of the most important measurements for RF power amplifiers. They tell you not only **how much power** the amplifier produces, but also **how cleanly** it produces it.

---

# 1. Pin vs Pout

This is the simplest chart.

* **Pin** = RF input power to amplifier
* **Pout** = RF output power from amplifier

A typical graph looks like:

```text
Pout (dBm)
 ^
 |
 |                        .
 |                     .
 |                  .
 |               .
 |            .
 |         .
 |      .
 |   .
 +-----------------------------> Pin (dBm)
```

In the linear region:

```text
Pout = Pin + Gain
```

Example:

Amplifier gain = 40 dB

| Pin     | Pout   |
| ------- | ------ |
| -20 dBm | 20 dBm |
| -10 dBm | 30 dBm |
| 0 dBm   | 40 dBm |

---

## Why it eventually bends

No amplifier can produce infinite power.

Eventually:

```text
Pout
 ^
 |
 |                   _______
 |                __/
 |             __/
 |          __/
 |       __/
 |    __/
 +----------------------> Pin
```

The curve begins to flatten.

This is compression.

---

# 2. P1dB (1 dB Compression Point)

The most common amplifier specification.

Suppose ideal gain is 40 dB.

If:

```text
Pin = 0 dBm
```

you expect:

```text
Pout = 40 dBm
```

But real amplifier produces:

```text
Pout = 39 dBm
```

It is now compressed by:

```text
40 - 39 = 1 dB
```

This point is called:

**P1dB**

---

Graphically

```text
Pout
 ^
 |
 |             Ideal
 |            /
 |           /
 |          /
 |         /
 |        /
 |       /
 |      /
 |     /
 |    /
 |   /
 |  /
 | /
 |/_____________________
 |             Actual
 +-----------------------> Pin
```

The point where actual output is 1 dB below ideal is P1dB.

---

### Example

Amplifier gain:

```text
40 dB
```

At:

```text
Pin = +2 dBm
```

Expected:

```text
42 dBm
```

Measured:

```text
41 dBm
```

Therefore:

```text
P1dB = 41 dBm
```

---

# Why P1dB matters

Near P1dB:

* distortion rises rapidly
* IMD rises
* spectral regrowth increases

For communication systems you usually operate several dB below P1dB.

---

# 3. IMD3 (Third Order Intermodulation Distortion)

This is one of the most important linearity measurements.

A perfectly linear amplifier only amplifies signals.

A real amplifier also creates new frequencies.

---

## Two-tone test

Inject:

```text
f1 = 1000 MHz
f2 = 1001 MHz
```

Ideal output:

```text
1000 MHz
1001 MHz
```

Real amplifier also generates:

```text
2f1-f2 = 999 MHz
2f2-f1 = 1002 MHz
```

These are IMD3 products.

---

Spectrum:

```text
      IMD3      Carrier Carrier    IMD3

       |            |      |         |
       |            |      |         |
-------|------------|------|---------|-----
      999         1000   1001      1002
```

---

### Why third-order is bad

Second-order products often fall far away.

Third-order products fall very close to the desired channel.

Therefore they interfere with adjacent channels.

---

### IMD3 specification

Example:

```text
Carrier = +30 dBm
IMD3 = -30 dBc
```

Meaning:

```text
IMD product is 30 dB below carrier.
```

Better amplifier:

```text
IMD3 = -40 dBc
```

Even better:

```text
IMD3 = -50 dBc
```

---

# IMD3 Growth

Interesting fact:

Carrier increases:

```text
1 dB per dB
```

IMD3 increases:

```text
3 dB per dB
```

So distortion grows much faster than signal.

That's why amplifiers suddenly become dirty near saturation.

---

# 4. Spectral Regrowth

This is what communication engineers worry about most.

Instead of two tones, now apply a modulated signal:

Examples:

* QPSK
* QAM
* DVB-S2
* LTE
* 5G

---

Ideal spectrum:

```text
Power
 ^
 |
 |       #########
 |       #########
 |       #########
 +--------------------> Frequency
```

---

When amplifier becomes nonlinear:

```text
Power
 ^
 |
 |    .#############.
 |   .###############.
 |  .#################.
 +------------------------> Frequency
```

Energy spreads outside the channel.

This is spectral regrowth.

---

## Why it happens

A modulated signal contains many amplitude levels.

Near compression:

```text
high peaks compress
```

which distorts the waveform.

Distortion generates additional frequencies.

These frequencies appear as skirts around the signal.

---

# ACPR / ACLR

Spectral regrowth is usually measured using:

* ACPR (Adjacent Channel Power Ratio)
* ACLR (Adjacent Channel Leakage Ratio)

Example:

```text
Main channel = 40 dBm

Adjacent channel = -30 dBm
```

Then:

```text
ACPR = -70 dBc
```

---

# Relation Between All Four

As drive level increases:

```text
Pin ↑
```

↓

```text
Pout increases
```

↓

```text
Approach P1dB
```

↓

```text
IMD3 rises rapidly
```

↓

```text
Spectral regrowth increases
```

↓

```text
Adjacent channel interference increases
```

---

For your satellite BUC/LNB work, spectral regrowth is particularly important because satellite operators often require a certain output backoff from saturation (for example 3–6 dB) to keep IMD and spectral regrowth within limits. A 100 W BUC may be capable of 50 dBm saturated output, but you might operate it at only 44–47 dBm average power to maintain signal quality.
