import numpy as np
import matplotlib.pyplot as plt

# Binary sequence for "hello" (simplified for visualization)
binary_sequence = [0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1]  # Simplified example

# Generating FSK signal
t = np.linspace(0, 1, 1000)  # Time vector
fsk_signal = np.zeros_like(t)

# Frequencies for binary 0 and 1
f0 = 5  # Frequency for binary 0
f1 = 10  # Frequency for binary 1

# Modulate each bit
for i, bit in enumerate(binary_sequence):
    fsk_signal += np.sin(2 * np.pi * (f0 + (f1 - f0) * bit) * t + i) * (t >= i / len(binary_sequence)) * (t < (i + 1) / len(binary_sequence))

# Plotting
plt.figure(figsize=(10, 4))
plt.plot(t, fsk_signal)
plt.title('FSK Signal Representation of "hello" (Simplified)')
plt.xlabel('Time')
plt.ylabel('Amplitude')
plt.grid(True)
plt.gca().set_facecolor('black')
plt.gcf().set_facecolor('black')
plt.show()
