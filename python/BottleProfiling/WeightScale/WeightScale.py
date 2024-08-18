'''
Created on Mar 13, 2019

@author: user1
'''
import time
import sys
from SerialPortCmdProcessor import SerialPortCmdProcessor
import serial
import serial.tools.list_ports as list_ports
import re
import hid #imported from hidapi
import threading
import traceback
import math

class WeightScale(object):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
        self.DATA_MODE_GRAMS  = 2
        self.DATA_MODE_OUNCES = 11
        self.GRAMS_PER_OUNCE  = 28.3495
        self.OUNCES_PER_GRAM  = 0.035274
        self.ANDScaleVID      = 0x6CD
        self.ANDScalePID      = 0x121
        self.serialAND        = None
        self.remainingML      = 0.00

    def isDYMOScaleConnected(self):
        if (self.portAND == None):
        ## Did not detect AND scale. Check if DYMO scale connected
            self.device = hid.device()

        try:
            device.open(0x0922, 0x8003)
            scale_turned_off = False
        except:
            scale_turned_off = True
            print "Unable to detect scale. Make sure it's connected, or try powering on the scale.\n"
            while (True):
                time.sleep(0.5)
                for port in list_ports.comports():
                    if (re.search(self.andPID, port[2], re.DOTALL)):
                        portAND = port[0]
                        USB_SCALE = 'AND'
                        break
                if (portAND == None):
                    try:
                        self.device.open(0x0922, 0x8003)
                        scale_turned_off = False
                        USB_SCALE = 'DYMO'
                        break
                    except:
                        scale_turned_off = True
                else:
                    break


    def connectDYMOScale(self):
        if self.USB_SCALE == 'DYMO':
            ## Start scale thread
            thread_handler = threading.Thread(target = self.DYMO_Scale_Thread, args=(self.device,))
            thread_handler.daemon = True
            thread_handler.start()

            print 'Connected to DYMO scale: '
            if (scale_units == self.DATA_MODE_OUNCES):
                print 'Scale set to ounces. Press \'kg lb\' button to switch units to grams.\n'
            while (scale_units == self.DATA_MODE_OUNCES):
                time.sleep(0.5)
            scale_enable_readings = True
            time.sleep(0.250)
            if (scale_weight_in_grams != 0):
                print 'Verify bottle is empty. Press \'TARE\' button to zero scale.\n'
            while (scale_weight_in_grams != 0):
                time.sleep(0.5)
            scale_enable_readings = False


    def isANDScaleConnected(self):
        ## Check if AND Scale connected
        self.portAND = None
        for port in list_ports.comports():
            if (re.search('VID:PID', port[2], re.DOTALL)):
                hwIDStr = port[2].split(" ")
                hwID = re.split('=|:',hwIDStr[1])
                vid=hwID[2]
                pid=hwID[3]
                if (int(vid,16) == self.ANDScaleVID and int(pid,16) == self.ANDScalePID):
                    print "Found AND Scale"
                    self.portAND   = port[0]
                    self.USB_SCALE = 'AND'
                    break
                    
            #print port.device + port.hwid
        
        return self.portAND

    def connectANDScale(self):
        if self.USB_SCALE == 'AND':
            self.serialAND = serial.Serial(port=self.portAND,\
                        baudrate=2400,\
                        parity=serial.PARITY_EVEN,\
                        stopbits=serial.STOPBITS_ONE,\
                        bytesize=serial.SEVENBITS,\
                        timeout=1.250)
            print 'Connected to AND scale:', self.serialAND.name

            ## Turn scale on
            cnt = 0
            while True:
                s = SerialPortCmdProcessor.SerialPortCmdProcessor()
                retval = s.SendCommand(self.serialAND, 'ON\r\n', ['\x06\r\n\x06\r\n'])
                cnt += 1
                if retval == '':
                    print 'Error getting response.\n'
                else:
                    if retval == '\x06\r\n\x06\r\n': ## ACK\r\nACK\r\n
                        # print 'Command Acknowledged'
                        break
                    else:
                        ec = re.search(r'EC,([a-zA-Z0-9]{3}).*', retval, re.DOTALL)
                        if (ec != None):
                            print 'Error Code: ' + ec.group(1)
                        else:
                            print 'Unknown response: ' + str(retval)
                if cnt == 2:
                    time.sleep(4)
                    sys.exit(0)

            ## Re-Zero scale
            time.sleep(3)
            cnt = 0
            while True:
                retval = s.SendCommand(self.serialAND, 'R\r\n', ['\x06\r\n\x06\r\n'])
                cnt += 1
                if retval == '':
                    print 'Error getting response.\n'
                else:
                    if retval == '\x06\r\n\x06\r\n': ## ACK\r\nACK\r\n
                        # print 'Command Acknowledged'
                        break
                    else:
                        ec = re.search(r'EC,([a-zA-Z0-9]{3}).*', retval, re.DOTALL)
                        if (ec != None):
                            print 'Error Code: ' + ec.group(1)
                        else:
                            print 'Unknown response: ' + str(retval)
                if cnt == 2:
                    time.sleep(4)
                    sys.exit(0)

    def getWeight(self):
            if self.USB_SCALE == 'DYMO':
                ## Get a stable weight measurement
                scale_enable_readings = True
                time.sleep(0.250)
                stableSeconds = 0
                while stableSeconds < 5:
                    ## Read a list of measurements to make sure reading is stable
                    for i in range(0,len(currWeightInGrams)):
                        currWeightInGrams[i] = scale_weight_in_grams
                        if currWeightInGrams[i] == None:
                            print 'Error: No readings from scale.'
                            sys.exit(1)
                        time.sleep(0.250) #time between updates
                    ## Check if readings are all the same
                    if currWeightInGrams.count(currWeightInGrams[0]) == len(currWeightInGrams):
                        stableSeconds += 1
                    else:
                        stableSeconds == 0

                print 'Stable reading at ' + str(currWeightInGrams[0]) + ' g'
                scale_enable_readings = False
                currML = int(currWeightInGrams[0])

                ## Calculate how much liquid was filled
                #diffInGrams = int(currWeightInGrams[0]) - int(lastWeightInGrams)
                #self.remainingML -= diffInGrams

            elif self.USB_SCALE == 'AND':
                ## Get stable weight measurement
                cnt = 0
                while True:
                    s = SerialPortCmdProcessor.SerialPortCmdProcessor()
                    retval = s.SendCommand(self.serialAND, 'S\r\n', ['ST,\+[\d]{5}\.[\d]{2}  g'])
                    cnt += 1
                    if retval == '':
                        print 'Error getting response.\n'
                    else:
                        temp = re.search('ST,\+([\d]{5}\.[\d]{2})  g', retval, re.DOTALL)
                        if temp: ## ACK\r\nACK\r\n
                            print 'weight = ' + str(temp.group(0)[3:])
                            currWeightInGrams = float(temp.group(1))
                            break
                        else:
                            ec = re.search(r'EC,([a-zA-Z0-9]{3}).*', retval, re.DOTALL)
                            if (ec != None):
                                print 'Error Code: ' + ec.group(1)
                            else:
                                print 'Unknown response: ' + str(retval)
                    if cnt == 2:
                        time.sleep(4)
                        sys.exit(0)
                
                #currWeightInGrams = currWeightInGrams * (1 + self.TubeVolPermm)
                #currML            = currWeightInGrams

                ## Calculate how much liquid was filled
                #diffInGrams = int(math.floor(currWeightInGrams)) - int(math.floor(lastWeightInGrams))
                #self.remainingML = remainingML - diffInGrams
                #return [self.remainingML, currML]    
                return currWeightInGrams
    
    def checkScaleOff(self):
        if self.USB_SCALE == 'DYMO':
            ## Check if scale turned off
            if scale_turned_off:
                print '\nError: Scale turned off. Cancelling test.'
                time.sleep(4)
                sys.exit(0)
                
    def off(self):
        if (self.USB_SCALE != None) and (self.USB_SCALE == 'AND'):
            try:
                ## Turn scale on
                cnt = 0
                while True:
                    s = SerialPortCmdProcessor.SerialPortCmdProcessor()
                    retval = s.SendCommand(self.serialAND, 'OFF\r\n', ['\x06\r\n'])
                    cnt += 1
                    if retval == '':
                        print 'Error getting response from scale.\n'
                    else:
                        if retval == '\x06\r\n': ## ACK\r\n
                            print 'Turned off the scale.'
                            break
                        else:
                            ec = re.search(r'EC,([a-zA-Z0-9]{3}).*', retval, re.DOTALL)
                            if (ec != None):
                                print 'Error Code: ' + ec.group(1)
                            else:
                                print 'Unknown response: ' + str(retval)
                    if cnt == 2:
                        print 'Unable to turn off scale.\n'
                        break
            except:
                print 'Unable to turn off scale.\n'
                
    def DYMO_Scale_Thread(self,device):
        global scale_units
        global scale_turned_off
        global scale_weight_in_grams
        global scale_enable_readings

        while True:
            try:
                data = device.read(6)
            except:
                scale_turned_off = True
                time.sleep(0.5)
                pass

            if data:
                scale_turned_off = False
                retryCount = 0

                ## Get scaling factor
                if data[3] == 255:
                    ounce_scaling_factor = 0.1
                elif data[3] == 254:
                    ounce_scaling_factor = 0.01
                else:
                    ounce_scaling_factor = 1

                ## Calculate raw weight
                raw_weight = data[4] + data[5] * 256

                ## Convert raw weight to ounces and grams
                if data[2] == self.DATA_MODE_OUNCES:
                    ounces = raw_weight * ounce_scaling_factor
                    grams = ounces * self.GRAMS_PER_OUNCE
                elif data[2] == self.DATA_MODE_GRAMS:
                    grams = raw_weight
                    ounces = grams * self.OUNCES_PER_GRAM

                ## Check if scale has weight
                if data[1] == 2:
                    ## Scale has no weight
                    pass
                # elif data[1] == 5:
                #     ## Scale has negative weight
                #     grams = grams * -1
                # else:
                #     ## Scale has positive weight
                #     ounces = ounces * -1
                # ^^^^^^^^^ ignore converting to negative

                ## Update global variables
                scale_units = data[2]
                if scale_enable_readings:
                    scale_weight_in_grams = grams
                else:
                    scale_weight_in_grams = None
                    data = None
            else:
                if retryCount == 3:
                    scale_turned_off = True
                else:
                    retryCount += 1
