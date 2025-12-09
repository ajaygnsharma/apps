import re

def getTenMHz_num() -> str:
    #s   = "CC1=RXI-12NAW-0000C\n";
    s   = 'CC1=RXI-10N4W-0000C\n';

    cc1Str = re.split(r'\n|=',s);

    if (len(cc1Str) == 0):
        return "-1";
    else:
        # Parse the number from string like "RXI-12NAW-0000C"
        # Extract the digit(s) between "RXI-" and "NAW"
        match = re.search(r'RXI-(\d\d)', cc1Str[1]);
        
        if match:
            digits = match.group(1)
            if len(digits) > 1:
                return digits[1]
            else:
                return match.group(1)
        else:
            return "-1"

if __name__ == "__main__":
    print(getTenMHz_num())  # for test only
    pass