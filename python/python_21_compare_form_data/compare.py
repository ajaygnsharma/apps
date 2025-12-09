def compare_alarm_data(data_dict, form_dict):
    changed = {}

    label_map = {0: 'A', 1: 'B', 2: 'S'}  # mapping index to form suffix

    for idx, content in data_dict.items():
        suffix = label_map[idx]

        # Compare control values ('c')
        for key, val in content['c'].items():
            form_key = f"{key}{suffix}"
            form_val = form_dict.get(form_key)
            if form_val is not None and form_val != val:
                changed[form_key] = {'old': val, 'new': form_val}

        # Compare suppression values ('s')
        for key, val in content['s'].items():
            form_key = f"{key}{suffix}"
            form_val = form_dict.get(form_key)

            # Convert 'on' → '1', 'off' or missing → '0'
            form_val_bin = '1' if form_val == 'on' else '0'

            if form_val_bin != val:
                changed[form_key] = {'old': val, 'new': form_val_bin}

    return changed


def checkData():
    formData = {'tempA': '1',
     'tilA': '1',
     'tihA': '1',
     'tolA': '1',
     'tohA': '1',

     'tempB': '0',
     'tilB': '0',
     'tihB': '0',
     'tolB': '0',
     'tohB': '0',

     'tempS': '1',
     'tilS': '1',
     'tihS': '1',
     'tolS': '1',
     'tohS': '1',

     'temp_s_A': 'on',
     'til_s_A': 'on',
     'tih_s_A': 'on',
     'tol_s_A': 'on',
     'toh_s_A': 'on',
     'ttsA': 'on',

     'temp_s_B': 'on',
     'til_s_B': 'on',
     'tih_s_B': 'on',
     'tol_s_B': 'on',
     'toh_s_B': 'on',
     'ttsB': 'on',

     'temp_s_S': 'on',
     'til_s_S': 'on',
     'tih_s_S': 'on',
     'tol_s_S': 'on',
     'toh_s_S': 'on',
     'ttsS': 'on'};

    myData = dict(
        [(0,
            {'c':
                {'toh': '1', 'tol': '1', 'tih': '1', 'til': '1', 'temp': '1'},
            's':
                {'toh_s_': '1', 'tol_s_': '1', 'tih_s_': '1', 'til_s_': '0', 'temp_s_': '1', 'tts': '1', 'tas': '0', 'taz': '0'}
            }
        ),
        (1,
            {'c':
                {'toh': '0', 'tol': '0', 'tih': '0', 'til': '0', 'temp': '0'},
            's':
                {'toh_s_': '1', 'tol_s_': '1', 'tih_s_': '1', 'til_s_': '1', 'temp_s_': '1', 'tts': '1', 'tas': '0', 'taz': '0'}
            }
        ),
        (2,
            {'c':
                {'toh': '1', 'tol': '1', 'tih': '1', 'til': '1', 'temp': '1'},
            's':
                {'toh_s_': '1', 'tol_s_': '1', 'tih_s_': '1', 'til_s_': '1', 'temp_s_': '1', 'tts': '1', 'tas': '0', 'taz': '0'}
            }
        )
        ])

    diff  = compare_alarm_data(myData, formData)

checkData();


