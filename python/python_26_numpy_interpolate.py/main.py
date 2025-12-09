import numpy as np
import matplotlib.pyplot as plt

# Known frequency table (example)
x = np.linspace(29000, 31000, 11)  # Evenly spaced entries
y = np.array([
    -5670, -5650, -5600, -5630, -5670,
    -5700, -5610, -5670, -5710, -5770, -5700
]) / 100.0  # Convert to float early

freq = 30500
ccf = np.interp(freq, x, y)

#plt.plot(x, y)
#plt.show()
print("Interpolated value:", round(ccf, 2))
