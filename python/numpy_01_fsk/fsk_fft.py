from scipy.fft import fft, fftfreq
import numpy as np
import matplotlib.pyplot as plt

# Number of sample points
N = 1000
# Sample spacing
T = 1.0 / 800.0

# x array for the time domain
x = np.linspace(0.0, N*T, N, endpoint=False)

# Generate a simplified FSK signal for demonstration (using parts of the previous code snippet)
y = np.sin(50.0 * 2.0*np.pi*x) + 0.5*np.sin(80.0 * 2.0*np.pi*x)

# Compute the FFT
yf = fft(y)
xf = fftfreq(N, T)[:N//2]

# Plotting the FFT / Frequency Domain Representation
plt.figure(figsize=(10, 4))
plt.plot(xf, 2.0/N * np.abs(yf[0:N//2]))
plt.title('Frequency Domain Representation of the FSK Signal for "hello"')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Amplitude')
plt.grid(True)
plt.gca().set_facecolor('black')
plt.gcf().set_facecolor('black')
plt.show()
