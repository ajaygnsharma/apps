'''
Created on Mar 13, 2019

@author: user1
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

class LazerSensor(object):
    '''
    classdocs
    '''
    def __init__(self,ref_offset, float_offset, shim_offset):
        '''
        Constructor
        '''
        self.VID = 0
        self.PID = 2
        self.DEBUG = False
        self.FEATURE_CALIBRATED_LAZER_DEPTH = True
        self.FEATURE_AVERAGING              = True
        self.REF_OFFSET   = ref_offset#Default 1119.3
        self.FLOAT_OFFSET = float_offset#Default1.8 
        self.SHIM_OFFSET  = shim_offset#Default 0.5 # Shim width
        self.totalOffset  = (self.REF_OFFSET - (self.FLOAT_OFFSET + self.SHIM_OFFSET))
        self.portLAZER = None
        
        for port in list_ports.comports():
            if (re.search('VID:PID', port[2], re.DOTALL)):
                hwIDStr = port[2].split(" ")
                hwID = re.split('=|:',hwIDStr[1])
                vid=hwID[2]
                pid=hwID[3]
                if (int(vid,16) == self.VID and int(pid,16) == self.PID):
                    print "Found Lazer Sensor"
                    self.portLAZER = port[0]
        if (self.portLAZER == None):
            print "Unable to detect laser sensor. Make sure it's connected and powered on.\n"
            while (True):
                time.sleep(0.5)
                for port in list_ports.comports():
                    if (re.search('0000:0002', port[2], re.DOTALL)):
                        self.portLAZER = port[0]
                        break
                    elif (re.search('67b:2303', port[2], re.DOTALL)):
                        self.portLAZER = port[0]
                        break
                if (self.portLAZER != None):
                    break
        ## Connect to ALICAT
        self.serialLAZER = serial.Serial(port=self.portLAZER, \
                                         baudrate=115200, timeout=1)
        self.stop()
        self.getDeviceGeneration()
        self.getSoftwareRevision()
        self.getSerialNum()
        self.stop()
        self.setMeasurementCharacteristic()
        self.saveConfig()
        self.stop()
        self.setFilter()
        self.saveConfig()
        
    def getDeviceGeneration(self):
        s = SerialPortCmdProcessor.SerialPortCmdProcessor()
        output = s.SendCommand(self.serialLAZER, 'dg\r\n', 'g1dg+084+0B\r\n')
        if output == '':
            print 'Error reading from Lazer sensor.\n'
            time.sleep(4)
            sys.exit(0)
        x = re.findall("^g.dg.", output)
        if x:
            if self.DEBUG:
                print output
            
    def getSoftwareRevision(self):
        s = SerialPortCmdProcessor.SerialPortCmdProcessor()
        output = s.SendCommand(self.serialLAZER, 's1sv\r\n', '^g.sv+\r\n')
        if output == '':
            print 'Error reading from Lazer sensor.\n'
            time.sleep(4)
            sys.exit(0)
        x = re.findall("^g.sv+.", output)
        if x:
            if self.DEBUG:
                print output
            
    def getSerialNum(self):
        s = SerialPortCmdProcessor.SerialPortCmdProcessor()
        output = s.SendCommand(self.serialLAZER, 's1sn\r\n', '^g.sn+\r\n')
        if output == '':
            print 'Error reading from Lazer sensor.\n'
            time.sleep(4)
            sys.exit(0)
        x = re.findall("^g.sn+.", output)
        if x:
            if self.DEBUG:
                print output              

    def stop(self):
        s = SerialPortCmdProcessor.SerialPortCmdProcessor()
        output = s.SendCommand(self.serialLAZER, 's1c\r\n', '^g.?\r\n')
        if output == '':
            print 'Error reading from Lazer sensor.\n'
            time.sleep(4)
            sys.exit(0)
        x = re.findall("^g.?.", output)
        if x:
            if self.DEBUG:
                print output              
    
    def saveConfig(self):
        s = SerialPortCmdProcessor.SerialPortCmdProcessor()
        output = s.SendCommand(self.serialLAZER, 's1s\r\n', '^g.s?\r\n')
        if output == '':
            print 'Error reading from Lazer sensor.\n'
            time.sleep(4)
            sys.exit(0)
        x = re.findall("^g.s?.", output)
        if x:
            if self.DEBUG:
                print output   
    
    def setMeasurementCharacteristic(self):
        s = SerialPortCmdProcessor.SerialPortCmdProcessor()
        output = s.SendCommand(self.serialLAZER, 's1mc+2\r\n', '^g.mc?\r\n')
        if output == '':
            print 'Error reading from Lazer sensor.\n'
            time.sleep(4)
            sys.exit(0)
        x = re.findall("^g.mc?.", output)
        if x:
            if self.DEBUG:
                print output 

    def setFilter(self):
        s = SerialPortCmdProcessor.SerialPortCmdProcessor()
        output = s.SendCommand(self.serialLAZER, 's1fi+32+6+0\r\n', '^g.fi?\r\n')
        if output == '':
            print 'Error reading from Lazer sensor.\n'
            time.sleep(4)
            sys.exit(0)
        x = re.findall("^g.fi?.", output)
        if x:
            if self.DEBUG:
                print output 
                      
    
    def startMeasurement(self):
        s = SerialPortCmdProcessor.SerialPortCmdProcessor()
        output = s.SendCommand(self.serialLAZER, 's1g\r\n','g1g.')
        x = re.findall("^g1g+.", output)
        if output == '':
            print 'Error reading from Lazer sensor.\n'
            time.sleep(4)
            sys.exit(0)
        if x:
            height = re.split("\r\n", output)
            height = re.split("\+", height[0])
            if self.DEBUG:
                print height
            return (self.totalOffset - (float(height[1])/10))
                         
    def readHeight(self):
        s = SerialPortCmdProcessor.SerialPortCmdProcessor()
        sum  = 0.0
        avgH = 0.0
        if self.FEATURE_AVERAGING:
            for i in range(0,10):
                output = s.SendCommand(self.serialLAZER, 's1g\r\n','g1g.')
                x = re.findall("^g1g+.", output)
                if output == '':
                    print 'Error reading from Lazer sensor.\n'
                    time.sleep(4)
                    sys.exit(0)
                if x:
                    height = re.split("\r\n", output)
                    height = re.split("\+", height[0])
                    if self.DEBUG:
                        print height
                    h = (self.totalOffset - (float(height[1])/10))
                    sum = sum + h
                time.sleep(0.100)
            avgH = float(sum / 10)
        else:
            output = s.SendCommand(self.serialLAZER, 's1g\r\n','g1g.')
            x = re.findall("^g1g+.", output)
            if output == '':
                print 'Error reading from Lazer sensor.\n'
                time.sleep(4)
                sys.exit(0)
            if x:
                height = re.split("\r\n", output)
                height = re.split("\+", height[0])
                if self.DEBUG:
                    print height
                h = (self.totalOffset - (float(height[1])/10))
            avgH = h    
                
        if self.FEATURE_CALIBRATED_LAZER_DEPTH:
            calibratedLaserDepth = avgH +\
                    (-1.25 + (0.0394 * avgH) +\
                    (-0.000176*(avgH ** 2)) +\
                    (0.000000493*(avgH ** 3)) +\
                    (-0.000000000944*(avgH ** 4)))
            rCalibratedLaserDepth = round(calibratedLaserDepth,2)
            return rCalibratedLaserDepth
        else:
            return avgH
            
    def clear(self):
        s = SerialPortCmdProcessor.SerialPortCmdProcessor()
        output = s.SendCommand(self.serialLAZER, 'sls\r\n', 'gls?\r\n')
        if output == '':
            print 'Error reading from Lazer sensor.\n'
            time.sleep(4)
            sys.exit(0)


    def halt(self):
        s = SerialPortCmdProcessor.SerialPortCmdProcessor()
        output = s.SendCommand(self.serialLAZER, 'slh+0\r\n', None)
        if output == '':
            print 'Error reading from Lazer sensor.\n'
            time.sleep(4)
            sys.exit(0)

def main(argv):
    l = LazerSensor()
    print "Height with Calibration"
    for i in range(0,10):
        #print(l.startMeasurement())
        print(l.readHeight())
        time.sleep(1)
    #print(l.readHeight())
    l.stop()
    

#------------------------------------------------------------------------------
# call to main()
#------------------------------------------------------------------------------
if __name__ == "__main__":
    main(sys.argv[1:])   
