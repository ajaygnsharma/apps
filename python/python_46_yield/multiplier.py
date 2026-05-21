def make_multiplier(x):
    def multiply(y):
        return x * y
    return multiply

times10 = make_multiplier(10)

print(times10(5))  # 50