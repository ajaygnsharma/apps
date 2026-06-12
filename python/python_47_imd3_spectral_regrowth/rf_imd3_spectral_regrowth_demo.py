#!/usr/bin/env python3
"""
rf_imd3_spectral_regrowth_demo.py

Demonstrates two common RF amplifier distortion measurements:

1) IMD3 using a two-tone signal
2) Spectral regrowth using a digitally modulated signal

The amplifier is modeled with a simple memoryless nonlinearity:

    y = x - alpha * x^3

This is not a full RF power amplifier model, but it clearly shows why
third-order products and spectral regrowth appear as drive level increases.
"""

import numpy as np
import matplotlib.pyplot as plt


def db20(x, floor_db=-160):
    """20*log10(abs(x)) with a floor to avoid log(0)."""
    mag = np.abs(x)
    return np.maximum(20 * np.log10(mag + 1e-20), floor_db)


def make_spectrum(x, fs, window=True):
    """
    Return frequency axis and normalized FFT magnitude in dB.
    Frequency axis is centered around 0 Hz.
    """
    n = len(x)

    if window:
        w = np.hanning(n)
        xw = x * w
        scale = np.sum(w) / n
    else:
        xw = x
        scale = 1.0

    X = np.fft.fftshift(np.fft.fft(xw)) / (n * scale)
    f = np.fft.fftshift(np.fft.fftfreq(n, d=1 / fs))
    mag_db = db20(X)

    # Normalize largest tone/point to 0 dB for easy viewing
    mag_db -= np.max(mag_db)

    return f, mag_db


def nonlinear_pa(x, alpha=0.7, gain=1.0):
    """
    Simple RF amplifier nonlinearity.

    Small alpha = more linear.
    Large alpha = more distortion/compression.
    """
    return gain * (x - alpha * x**3)


def demo_imd3():
    """
    Two-tone IMD3 demo.

    Input tones:
        f1 = 100 kHz
        f2 = 110 kHz

    Third-order IMD products appear at:
        2*f1 - f2 = 90 kHz
        2*f2 - f1 = 120 kHz
    """
    fs = 2_000_000
    n = 131_072
    t = np.arange(n) / fs

    f1 = 100_000
    f2 = 110_000

    # Change this amplitude to see IMD3 products grow.
    # Try 0.05, 0.15, 0.35, 0.6
    amp = 0.45

    x = amp * (np.cos(2 * np.pi * f1 * t) + np.cos(2 * np.pi * f2 * t))
    y = nonlinear_pa(x, alpha=0.7)

    f, x_db = make_spectrum(x, fs)
    _, y_db = make_spectrum(y, fs)

    plt.figure(figsize=(10, 5))
    plt.plot(f / 1e3, x_db, label="Input: mostly two tones")
    plt.plot(f / 1e3, y_db, label="After nonlinear PA: IMD3 appears")

    plt.axvline((2 * f1 - f2) / 1e3, linestyle="--", linewidth=1)
    plt.axvline(f1 / 1e3, linestyle="--", linewidth=1)
    plt.axvline(f2 / 1e3, linestyle="--", linewidth=1)
    plt.axvline((2 * f2 - f1) / 1e3, linestyle="--", linewidth=1)

    plt.text((2 * f1 - f2) / 1e3, -10, "2f1-f2", rotation=90, va="top")
    plt.text(f1 / 1e3, -10, "f1", rotation=90, va="top")
    plt.text(f2 / 1e3, -10, "f2", rotation=90, va="top")
    plt.text((2 * f2 - f1) / 1e3, -10, "2f2-f1", rotation=90, va="top")

    plt.title("Two-Tone IMD3 Demo")
    plt.xlabel("Frequency offset (kHz)")
    plt.ylabel("Relative magnitude (dB)")
    plt.xlim(60, 150)
    plt.ylim(-100, 5)
    plt.grid(True)
    plt.legend()
    plt.tight_layout()


def rrc_filter(beta, span_symbols, sps):
    """
    Root-raised-cosine filter impulse response.

    beta: roll-off factor, 0..1
    span_symbols: filter length in symbols
    sps: samples per symbol
    """
    n = span_symbols * sps
    t = np.arange(-n / 2, n / 2 + 1) / sps

    h = np.zeros_like(t, dtype=float)

    for i, ti in enumerate(t):
        if abs(ti) < 1e-12:
            h[i] = 1.0 - beta + 4 * beta / np.pi
        elif beta > 0 and abs(abs(ti) - 1 / (4 * beta)) < 1e-12:
            h[i] = (
                beta / np.sqrt(2)
                * (
                    (1 + 2 / np.pi) * np.sin(np.pi / (4 * beta))
                    + (1 - 2 / np.pi) * np.cos(np.pi / (4 * beta))
                )
            )
        else:
            numerator = (
                np.sin(np.pi * ti * (1 - beta))
                + 4 * beta * ti * np.cos(np.pi * ti * (1 + beta))
            )
            denominator = np.pi * ti * (1 - (4 * beta * ti) ** 2)
            h[i] = numerator / denominator

    # Normalize energy
    h /= np.sqrt(np.sum(h**2))
    return h


def demo_spectral_regrowth():
    """
    Spectral regrowth demo using QPSK-like baseband data.

    The clean signal has a reasonably contained spectrum.
    The nonlinear amplifier causes energy to spread outside the original channel.
    """
    rng = np.random.default_rng(1)

    fs = 1_000_000
    symbol_rate = 50_000
    sps = fs // symbol_rate
    num_symbols = 8192

    bits_i = rng.integers(0, 2, num_symbols)
    bits_q = rng.integers(0, 2, num_symbols)

    symbols = (2 * bits_i - 1) + 1j * (2 * bits_q - 1)
    symbols /= np.sqrt(2)

    upsampled = np.zeros(num_symbols * sps, dtype=complex)
    upsampled[::sps] = symbols

    h = rrc_filter(beta=0.35, span_symbols=10, sps=sps)
    x_bb = np.convolve(upsampled, h, mode="same")

    # Normalize RMS level
    x_bb /= np.sqrt(np.mean(np.abs(x_bb) ** 2))

    # Drive level into nonlinear amplifier.
    # Try 0.2 for cleaner output, 0.6 or 0.9 for strong regrowth.
    drive = 0.65
    x_drive = drive * x_bb

    # Apply nonlinearity independently to I and Q.
    # This simple model is enough to visualize spectral regrowth.
    y = nonlinear_pa(np.real(x_drive), alpha=0.6) + 1j * nonlinear_pa(
        np.imag(x_drive), alpha=0.6
    )

    f, x_db = make_spectrum(x_drive, fs)
    _, y_db = make_spectrum(y, fs)

    plt.figure(figsize=(10, 5))
    plt.plot(f / 1e3, x_db, label="Before PA")
    plt.plot(f / 1e3, y_db, label="After nonlinear PA")

    main_bw = symbol_rate * (1 + 0.35)
    plt.axvline(-main_bw / 2 / 1e3, linestyle="--", linewidth=1)
    plt.axvline(main_bw / 2 / 1e3, linestyle="--", linewidth=1)

    plt.title("Spectral Regrowth Demo")
    plt.xlabel("Frequency offset (kHz)")
    plt.ylabel("Relative magnitude (dB)")
    plt.xlim(-250, 250)
    plt.ylim(-100, 5)
    plt.grid(True)
    plt.legend()
    plt.tight_layout()


def main():
    demo_imd3()
    demo_spectral_regrowth()
    plt.show()


if __name__ == "__main__":
    main()
