import os
from datetime import datetime

def ibuc_log(msg):
    with open('/config/ibuc.log', 'a') as f:
        now = datetime.now()
        timestamp = now.strftime('%Y/%m/%d %H:%M:%S')

        f.write(f"{timestamp},{msg}")
        f.flush()
        os.fsync(f.fileno())


class BUCInfo:
    def __init__(self):
        self.ipList = ["192.168.1.22", "192.168.1.23", "192.168.1.24"]

    def getIP(self, buc):
        ip = ""
        if buc == 'A':
            ip = self.ipList[0]
        elif buc == 'B':
            ip = self.ipList[1]
        elif buc == 'S':
            ip = self.ipList[2]

        return ip;


#-------------------------------------------------------------------------------
#
# unquote
#
#-------------------------------------------------------------------------------
def unquote(value: str) -> str:
    return value.strip('"').strip("'") if isinstance(value, str) else value