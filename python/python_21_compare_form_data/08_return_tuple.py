def do_masks():
    cm1 = [0] * 3;
    cm2 = [0] * 3;
    cms = [0] * 3;
    i = 0;
    tts = ['0'] * 3;
    tas = ['0'] * 3;
    taz = ['0'] * 3;

    for i in range(3):
        print(f"cm1: {cm1[i]:04x}");
        print(f"cm2: {cm2[i]:04x}");
        print(f"cms: {cms[i]:04x}");
        print(f"tts: {tts[i]}");
        print(f"tas: {tas[i]}");
        print(f"taz: {taz[i]}");


    return (cm1, cm2, cms, tts, tas, taz);


def do_tuple(cm1, cm2, cms, tts, tas, taz):
    print(cm1);
    print(cm2);
    print(cms);
    print(tts);
    print(tas);
    print(taz);

    for i in range(3):
        print(f"cm1: {cm1[i]:04x}");
        print(f"cm2: {cm2[i]:04x}");
        print(f"cms: {cms[i]:04x}");
        print(f"tts: {tts[i]}");
        print(f"tas: {tas[i]}");
        print(f"taz: {taz[i]}");



cm1, cm2, cms, tts, tas, taz = do_masks();
do_tuple(cm1, cm2, cms, tts, tas, taz);



