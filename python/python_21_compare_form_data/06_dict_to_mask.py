from werkzeug.datastructures import ImmutableMultiDict
form = ImmutableMultiDict([
    ('temp_A', '2'), ('til_A', '2'), ('tih_A', '2'), ('tol_A', '2'), ('toh_A', '2'),
    ('temp_B', '0'), ('til_B', '0'), ('tih_B', '0'), ('tol_B', '0'), ('toh_B', '0'),
    ('temp_S', '2'), ('til_S', '2'), ('tih_S', '2'), ('tol_S', '2'), ('toh_S', '2'),
    ('temp_s_A', 'on'), ('tih_s_A', 'on'), ('tol_s_A', 'on'), ('toh_s_A', 'on'), ('tts_A', 'on'),
    ('temp_s_B', 'on'), ('til_s_B', 'on'), ('tih_s_B', 'on'), ('tol_s_B', 'on'), ('toh_s_B', 'on'), ('tts_B', 'on'),
    ('temp_s_S', 'on'), ('til_s_S', 'on'), ('tih_s_S', 'on'), ('tol_s_S', 'on'), ('toh_s_S', 'on'), ('tts_S', 'on')
])

regular_dict = dict(form);
print(regular_dict);


AGC_ALC_OOR_ALM        = 0
TX_MUTED_ALM           = 1
BUC_LOSS_OF_LOCK_ALM   = 2
TX_OUTPUT_LEVEL_HI_ALM = 3
TX_OUTPUT_LEVEL_LO_ALM = 4
TX_INPUT_LEVEL_HI_ALM  = 5
TX_INPUT_LEVEL_LO_ALM  = 6
TEMP_ALM               = 7

TX_SIMULATED_ALM       = 8
REF_10MHZ_ALM          = 9
VDC_OOR_ALM            = 10
WG_SW_FAULT_ALM        = 11
OVERDRIVE_ALM          = 12
IDC_OOR_ALM            = 13
AGC_ALC_TGT_OOR_ALM    = 14
UNDEFINED              = 15

cm1 = [0] * 3;
cm2 = [0] * 3;
cms = [];
i = 0;

for i in range(3):
    cm2[i] = (1 << WG_SW_FAULT_ALM) | (1 << TX_SIMULATED_ALM) | \
             (1 << BUC_LOSS_OF_LOCK_ALM) | (1 << TX_MUTED_ALM);

    cm1[i] = (1 << IDC_OOR_ALM) | (1 << OVERDRIVE_ALM) | \
             (1 << VDC_OOR_ALM) | (1 << REF_10MHZ_ALM) | (1 << AGC_ALC_OOR_ALM);

for key, value in form.items():
    # Start with nonconfigurable alarms. Set their respective Major and
    # Minor Masks
    i = 0;
    if key.startswith(("toh", "tol", "tih", "til", "temp")):
        mask = key[:-2]
        suffix = key[-1]
        if suffix == "A":
            i = 0;
        elif suffix == "B":
            i = 1;
        elif suffix == "S":
            i = 2;

        if mask == "toh" and value == "1":
            cm1[i] |= 1 << TX_OUTPUT_LEVEL_HI_ALM;
        elif mask == "tol" and value == "1":
            cm1[i] |= 1 << TX_OUTPUT_LEVEL_LO_ALM;
        elif mask == "tih" and value == "1":
            cm1[i] |= 1 << TX_INPUT_LEVEL_HI_ALM;
        elif mask == "til" and value == "1":
            cm1[i] |= 1 << TX_INPUT_LEVEL_LO_ALM;
        elif mask == "temp" and value == "1":
            cm1[i] |= 1 << TEMP_ALM;

        if mask == "toh" and value == "2":
            cm2[i] |= 1 << TX_OUTPUT_LEVEL_HI_ALM;
        elif mask == "tol" and value == "2":
            cm2[i] |= 1 << TX_OUTPUT_LEVEL_LO_ALM;
        elif mask == "tih" and value == "2":
            cm2[i] |= 1 << TX_INPUT_LEVEL_HI_ALM;
        elif mask == "til" and value == "2":
            cm2[i] |= 1 << TX_INPUT_LEVEL_LO_ALM;
        elif mask == "temp" and value == "2":
            cm2[i] |= 1 << TEMP_ALM;

major_mask = 0;
minor_mask = 0;

for i in range(3):
    major_mask = f"0x{cm2[i]:04x}";
    minor_mask = f"0x{cm1[i]:04x}";
    print(minor_mask);
    print(major_mask);
