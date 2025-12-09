import re

def getTenMHz_source() -> str:
    s   = "TML=Internal1,-8.6"
    tmlStr = re.split(r'\n|=',s);

    if (len(tmlStr) == 0):
        return "-1";
    else:
        for i in range(len(tmlStr)):
            print("tmlStr[", i, "] is ", tmlStr[i]);
        
        match = re.search(r'Modem|External|Internal1|Internal2', tmlStr[1]);
        print("tml match is ", match.group(0));

        if match:
            if match.group(0) == "Modem":
                return "0";
            elif match.group(0) == "External":
                return "1";
            elif match.group(0) == "Internal1":
                return "2";
            elif match.group(0) == "Internal2":
                return "3";
        else:
            return "-1";
    
r = getTenMHz_source();
print("r is ", r);