import re
def int_tenMHz_get() -> str:
    
    s   = "TME=1\n";
    tiaStr = re.split(r'\n|=',s);

    if (len(tiaStr) < 2):
        return "-1";
    else:
        return tiaStr[1];


if __name__ == "__main__":
    print(int_tenMHz_get())  # for test only
    pass