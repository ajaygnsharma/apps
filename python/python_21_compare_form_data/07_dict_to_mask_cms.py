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

cms = [0] * 3;
i = 0;

for key, value in form.items():
    # Start with nonconfigurable alarms. Set their respective Major and
    # Minor Masks
    i = 0;
    if key.startswith(("toh_s", "tol_s", "tih_s", "til_s", "temp_s")):
        mask = key[:-4]
        suffix = key[-1]
        if suffix == "A":
            i = 0;
        elif suffix == "B":
            i = 1;
        elif suffix == "S":
            i = 2;

        if mask == "toh" and value == "on":
            cms[i] |= 1 << TX_OUTPUT_LEVEL_HI_ALM;
        elif mask == "tol" and value == "on":
            cms[i] |= 1 << TX_OUTPUT_LEVEL_LO_ALM;
        elif mask == "tih" and value == "on":
            cms[i] |= 1 << TX_INPUT_LEVEL_HI_ALM;
        elif mask == "til" and value == "on":
            cms[i] |= 1 << TX_INPUT_LEVEL_LO_ALM;
        elif mask == "temp" and value == "on":
            cms[i] |= 1 << TEMP_ALM;

suppres_mask = 0;
for i in range(3):
    suppres_mask = f"0x{cms[i]:04x}";
    print(suppres_mask);
