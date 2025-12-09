import math

raw_volts = 1400;

v = ((12.13 - math.sqrt(102.625 - 34.783 * raw_volts * 6.105006e-4)) / 2 - 1) * 115.0;
print(v)

calc_v = v * 1.75
print(calc_v)