# All possible alarms
alarms = [
    "FAULTED_BUC_A",
    "FAULTED_BUC_B",
    "TEN_MHZ",
    "IFL_ALARM",
    "VDC_LOW_1",
    "VDC_LOW_2",
    "CLONING_FAULT",
    "FAULTED_BUC_SPARE",
    "WG1_SWITCH_FAULT",
    "WG2_SWITCH_FAULT",
    "BUC_A_OFFLINE",
    "BUC_B_OFFLINE",
    "BUC_S_OFFLINE",
]

major = {
    "FAULTED_BUC_A",
    "FAULTED_BUC_B",
    "IFL_ALARM",
    "VDC_LOW_1",
    "VDC_LOW_2",
    "CLONING_FAULT",
    "FAULTED_BUC_SPARE",
    "WG1_SWITCH_FAULT",
    "WG2_SWITCH_FAULT",
}

minor = {
    "TEN_MHZ",
    "BUC_A_OFFLINE",
    "BUC_B_OFFLINE",
    "BUC_S_OFFLINE",
}

suppress = set(alarms)

def build_mask(names):
    mask = 0
    for i, name in enumerate(alarms):
        if name in names:
            mask |= (1 << i)
    return mask


major_mask = build_mask(major)
minor_mask = build_mask(minor)
suppress_mask = build_mask(suppress)


print("\nGenerated Masks\n")
print(f"MAJOR_MASK    = 0x{major_mask:04X}")
print(f"MINOR_MASK    = 0x{minor_mask:04X}")
print(f"SUPPRESS_MASK = 0x{suppress_mask:04X}")