'''
Created on Mar 13, 2019

@author: Ajay Sharma
'''
import time
import sys
import os
path = os.getcwd()
sys.path.append(path+'/../')
from SerialPortCmdProcessor import SerialPortCmdProcessor
import serial
import serial.tools.list_ports as list_ports
import re

class Pump(object):
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        self.VID              = 0x2341
        self.PID              = 0x0043
        #### Get COM PORT name for Arduino
        devFileArduino        = None
        self.fillSecondsPerML = None

        for port in list_ports.comports():
            if(re.search('VID:PID', port[2], re.DOTALL)):
                hwIDStr = port[2].split(" ")
                hwID = re.split('=|:',hwIDStr[1])
                vid=hwID[2]
                pid=hwID[3]
                if (int(vid,16) == self.VID and int(pid,16) == self.PID):
                    print "Found Arduino board"
                    devFileArduino = port[0]
                    break
        if (devFileArduino == None):
            print "Error: Unable to detect serial port for Arduino."
            sys.exit(0)

        print '\n------ Connecting to Pump ------\n'
        print devFileArduino
        self.devHandleArduino = serial.Serial(port=devFileArduino, \
                                           baudrate=9600, timeout=1)
    def getPortName(self):
        if self.portArduino != None:
            return self.portArduino
        else:
            return None
    
    def getFillTime(self):
        s = SerialPortCmdProcessor.SerialPortCmdProcessor()

        ## Get timing information
        retval = s.SendCommand(self.devHandleArduino, 'fillTime',\
                                         r'Fill Time Per mL = .* ms')
        if retval == '':
            print 'Error getting fill time per mL from Arduino.\n'
            time.sleep(4)
            sys.exit(0)
        else:
            retval = re.search(r'Fill Time Per mL = (\d+)', retval)
            self.fillSecondsPerML = float(retval.groups()[0])/1000

        return self.fillSecondsPerML

    def getSpeed(self):
        s = SerialPortCmdProcessor.SerialPortCmdProcessor()

        retval = s.SendCommand(self.devHandleArduino, r'speed 30', r'Speed = \d+ ms')
        if retval == '':
            print 'Error setting speed of pump.\n'

        self.fillSecondsPerML = self.fillSecondsPerML * 4.0

        return self.fillSecondsPerML

    
    def fill(self,fillML):
        s = SerialPortCmdProcessor.SerialPortCmdProcessor()
        
        output = s.SendCommand(self.devHandleArduino, 'fill ' + str(fillML*2) + '\n', 'Filling \d+ mL')
        time.sleep(self.fillSecondsPerML * fillML * 2)
    
    def stop(self):
        try:
            if self.devHandleArduino and self.devHandleArduino.isOpen():
                s = SerialPortCmdProcessor.SerialPortCmdProcessor()
                s.SendCommand(self.devHandleArduino, 'stop\n', None)
                print 'Stopped the pump'
                self.devHandleArduino.close()
                print 'Closed serial port connection to Arduino'
        except:
            print 'Unable to stop pump and close serial port connection'
            
def main(argv):
    p = Pump()
    time.sleep(4)
    print("Fill Time = %f"%(p.getFillTime()))
    print("Speed = %f"%(p.getSpeed()))
    
    for i in range(1,5):
        print("Filling %d mL"%(i*5))
        p.fill(i*5)
        time.sleep(10)
    
    p.stop()
    

#------------------------------------------------------------------------------
# call to main()
#------------------------------------------------------------------------------
if __name__ == "__main__":
    main(sys.argv[1:])   
