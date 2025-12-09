cfg = {};
for i in ['A', 'B', 'S']:
    for key in ["toh", "tol", "tih", "til", "temp"]:
        cfg[f"{key}_{i}"] = "1";

print(cfg);


