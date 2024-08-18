'''
Created on Mar 13, 2019

@author: user1
'''
import getpass
import time
import re
import os
import sys
import csv
import glob
import hid #imported from hidapi
import subprocess

class SerialPortCmdProcessor(object):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''

    #### Generic function to send commands to cap and pump
    ### Optionally wait for serial port to output a regex match to a string in substrings[]
    ### Cap requires commands to use \r\n
    ### Pump requires commands to use \n
    def SendCommand(self, serial, command, substrings):
        serial.flushInput()
        serial.write(command)
        # Wait to read output
        time.sleep(0.500)
        count = 0.500
        response = ''
        bytesRemaining = serial.inWaiting()
        while ((bytesRemaining < len(command)) and (count <= 4.000)):
            time.sleep(0.500)
            count += 0.500
            bytesRemaining = serial.inWaiting()
        if (substrings == None):
            return
        while bytesRemaining > 0:
            response += serial.read(1)
            bytesRemaining = serial.inWaiting()

        retry = 0
        ## Wait for substrings in response
        if (substrings is not None):
            while 1:
                for substring in substrings:
                    if re.search(substring, response, re.DOTALL):
                        return response
                if count >= 15.0:
                    print 'Error: serial reached timeout of ' + str(count) + ' seconds.\n'
                    print 'Sent command: ' + command + '\n'
                    print 'Serial response: ' + response + '\n'
                    if (retry >= 3):
                        print 'Unable to send command. Number of retries = ' + str(retry) + '\n'
                        print 'Serial response: ' + response + '\n'
                        print 'Stopping test...\n'
                        time.sleep(4)
                        sys.exit(1)
                    else:
                        serial.flushInput()
                        response = ''
                        print 'Resending serial command...\n'
                        serial.write(command)
                        time.sleep(0.400)
                        count = 0.400
                        retry += 1

                time.sleep(0.100)
                count += 0.100
                bytesRemaining = serial.inWaiting()
                while bytesRemaining > 0:
                    response += serial.read(1)
                    bytesRemaining = serial.inWaiting()
        else:
            return response

    #### Generic function to send commands to cap and pump
    ### Optionally wait for serial port to output a regex match to a string in substrings[]
    ### Cap requires commands to use \r\n
    ### Pump requires commands to use \n
    def SerialRead(self, serial):
        serial.flushInput()
        # Wait to read output
        time.sleep(0.500)
        count = 0.500
        response = ''
        bytesRemaining = serial.inWaiting()

        while bytesRemaining > 0:
            response += serial.read(1)
            bytesRemaining = serial.inWaiting()

        retry = 0
        return response
