# CalibrateProfiler.py
# Created by Luke Spradlin on March 23, 2016.
# Copyright (c) 2016 Nectar Inc. All rights reserved.

import getpass
import time
import serial
import serial.tools.list_ports as list_ports
import re
import os
import sys
import csv
import glob
import hid #imported from hidapi
import numpy
import subprocess
import threading
import traceback
import math

#### Global variables
DATA_MODE_GRAMS = 2
DATA_MODE_OUNCES = 11
GRAMS_PER_OUNCE = 28.3495
OUNCES_PER_GRAM = 0.035274
scale_units = DATA_MODE_GRAMS
scale_turned_off = True
scale_weight_in_grams = 0
scale_enable_readings = False

def DYMO_Scale_Thread(device):
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
            if data[2] == DATA_MODE_OUNCES:
                ounces = raw_weight * ounce_scaling_factor
                grams = ounces * GRAMS_PER_OUNCE
            elif data[2] == DATA_MODE_GRAMS:
                grams = raw_weight
                ounces = grams * OUNCES_PER_GRAM

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


#### Generic function to send commands to cap and pump
### Optionally wait for serial port to output a regex match to a string in substrings[]
### Cap requires commands to use \r\n
### Pump requires commands to use \n
def SendCommand(serial, command, substrings):
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

print '\n-----------------------------------'
print '-------- Calibrate Profiler -------'
print '------- Type Ctrl+Z to abort ------'
print '-----------------------------------'

#### Main Script
try:
    print '\nFill container with a small amount of water.'
    print 'Lower profiler until touching water.'
    print 'Verify pressure sensor reads 0 (Press enter to continue):',
    raw_input()

    USB_SCALE = None
    ## Check if user has different google drive priviledges
    path = '/Users/' + getpass.getuser() + '/Google Drive/Product/Bees - Devices/Sensors/Cap Measurement Tests/'
    if not os.path.isdir(path):
        path = '/Users/' + getpass.getuser() + '/Google Drive/Cap Measurement Tests/'
    ## Check if path exists and create if not
    path = path + 'Profiler/'
    if not os.path.isdir(path):
        os.makedirs(path)

    ## Open CSV file for saving distances
    outputFileName = 'Calibration_Data.csv'
    outputFilePath = path + '/' + outputFileName
    if os.path.isfile(outputFilePath):
        while True:
            print 'Warning: ' + outputFileName + ' already exists. Replace existing file (Y/N)? ',
            choice = raw_input()
            if choice in ['N','n','No','no']:
                print '\nCancelling calibration...\n'
                sys.exit(0)
            elif any(choice in y for y in ['Y','y','Yes','yes']):
                break
            else:
                print 'Error: input value not recognized.'

    #### Get COM PORT name for Arduino
    arduinoPID = "PID=2341:43"
    portArduino = None
    for port in list_ports.comports():
        if (re.search(arduinoPID, port[2], re.DOTALL)):
            portArduino = port[0]
            break
    if (portArduino == None):
        print "Error: Unable to detect serial port for Arduino."
        sys.exit(0)

    print '\n------ Connecting to devices ------\n'

    ## Connect to Arduino
    serialArduino = serial.Serial(port=portArduino, baudrate=9600, timeout=1)

    ## Wait for devices to connect
    time.sleep(4)

    ## Get timing information
    retval = SendCommand(serialArduino, 'fillTime', r'Fill Time Per mL = .* ms')
    if retval == '':
        print 'Error getting fill time per mL from Arduino.\n'
        time.sleep(4)
        sys.exit(0)
    else:
        retval = re.search(r'Fill Time Per mL = (\d+)', retval)
        fillSecondsPerML = float(retval.groups()[0])/1000
    print 'Connected to Arduino:', serialArduino.name

    retval = SendCommand(serialArduino, r'speed 25', r'Speed = \d+ ms')
    if retval == '':
        print 'Error setting speed of pump.\n'
    fillSecondsPerML = fillSecondsPerML * 4.0

    portALICAT = None
    for port in list_ports.comports():
        if (re.search('403:6001', port[2], re.DOTALL)):
            portALICAT = port[0]
            break
        elif (re.search('67b:2303', port[2], re.DOTALL)):
            portALICAT = port[0]
            break
    if (portALICAT == None):
        print "Unable to detect pressure sensor. Make sure it's connected and powered on.\n"
        while (True):
            time.sleep(0.5)
            for port in list_ports.comports():
                if (re.search('403:6001', port[2], re.DOTALL)):
                    portALICAT = port[0]
                    break
                elif (re.search('67b:2303', port[2], re.DOTALL)):
                    portALICAT = port[0]
                    break
            if (portALICAT != None):
                break
    ## Connect to ALICAT
    serialALICAT = serial.Serial(port=portALICAT, baudrate=19200, timeout=1)
    ## Set the address of the pressure sensor
    # output = SendCommand(serialALICAT, '*@=A\r', 'A [+|-]\d+\.\d+\n')
    output = SendCommand(serialALICAT, '*@=A\r', 'A [+|-]\d+\n')
    if output == '':
        print 'Error reading from pressure sensor.\n'
        time.sleep(4)
        sys.exit(0)
    print 'Connected to pressure sensor:', serialALICAT.name

    ## Check if AND Scale connected
    andPID = "PID=6cd:121"
    portAND = None
    for port in list_ports.comports():
        if (re.search(andPID, port[2], re.DOTALL)):
            portAND = port[0]
            USB_SCALE = 'AND'
            break
    if (portAND == None):
        ## Did not detect AND scale. Check if DYMO scale connected
        device = hid.device()
        try:
            device.open(0x0922, 0x8003)
            scale_turned_off = False
        except:
            scale_turned_off = True
            print "Unable to detect scale. Make sure it's connected, or try powering on the scale.\n"
            while (True):
                time.sleep(0.5)
                for port in list_ports.comports():
                    if (re.search(andPID, port[2], re.DOTALL)):
                        portAND = port[0]
                        USB_SCALE = 'AND'
                        break
                if (portAND == None):
                    try:
                        device.open(0x0922, 0x8003)
                        scale_turned_off = False
                        USB_SCALE = 'DYMO'
                        break
                    except:
                        scale_turned_off = True
                else:
                    break

    if USB_SCALE == 'AND':
        serialAND = serial.Serial(port=portAND,\
                    baudrate=2400,\
                    parity=serial.PARITY_EVEN,\
                    stopbits=serial.STOPBITS_ONE,\
                    bytesize=serial.SEVENBITS,\
                    timeout=1.250)
        print 'Connected to AND scale:', serialAND.name

        ## Turn scale on
        cnt = 0
        while True:
            retval = SendCommand(serialAND, 'ON\r\n', ['\x06\r\n\x06\r\n'])
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
            retval = SendCommand(serialAND, 'R\r\n', ['\x06\r\n\x06\r\n'])
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

    elif USB_SCALE == 'DYMO':
        ## Start scale thread
        thread_handler = threading.Thread(target = DYMO_Scale_Thread, args=(device,))
        thread_handler.daemon = True
        thread_handler.start()

        print 'Connected to DYMO scale: '
        if (scale_units == DATA_MODE_OUNCES):
            print 'Scale set to ounces. Press \'kg lb\' button to switch units to grams.\n'
        while (scale_units == DATA_MODE_OUNCES):
            time.sleep(0.5)
        scale_enable_readings = True
        time.sleep(0.250)
        if (scale_weight_in_grams != 0):
            print 'Verify bottle is empty. Press \'TARE\' button to zero scale.\n'
        while (scale_weight_in_grams != 0):
            time.sleep(0.5)
        scale_enable_readings = False

    #### Prompt user for test parameters
    print '\n--------- Test Parameters ---------\n'

    print 'Enter maximum amount in mL to calibrate:',
    maxML = int(raw_input())

    print 'Enter increment amount in mL:',
    incrementML = int(raw_input())

    outputFile = open(outputFilePath, 'wb')
    writer = csv.writer(outputFile)
    # Setup CSV header
    writer.writerow(['Liquid (mL)', 'Pressure Reading', 'Actual Distance (mm)'])

    #### Main test section
    print '\n------- Starting Calibration ------\n'

    ## Tare the pressure sensor
    # output = SendCommand(serialALICAT, 'A$$P\r', 'A [+|-]\d+\.\d+\n')
    output = SendCommand(serialALICAT, 'A$$P\r', 'A [+|-]\d+\n')
    if output == '':
        print 'Error reading from pressure sensor.\n'
        time.sleep(4)
        sys.exit(0)

    ## Start bottle at min mL level
    remainingML = maxML
    lastML = 0
    currML = 0
    lastPressure = 0
    currPressure = 0
    currDistance = 0
    lastDistance = 0
    if USB_SCALE == 'DYMO':
        currWeightInGrams = [0]*4
    elif USB_SCALE == 'AND':
        currWeightInGrams = 0
    lastWeightInGrams = 0

    ## Test loop
    while (True):
        ## Get actual values
        if currML > 0:
            ## Read value from pressure sensor
            output = SendCommand(serialALICAT, 'A\r', 'A [+|-]\d+\n')
            if output == '':
                print 'Error reading from pressure sensor.\n'
                time.sleep(4)
                sys.exit(0)
            # retval = re.search('A [+|-](\d+\.\d+)', output, re.DOTALL)
            retval = re.search('A [+|-](\d+)', output, re.DOTALL)
            lastPressure = currPressure
            currPressure = int(retval.group(1))
            print 'Pressure: ' + str(currPressure)

            while True:
                try:
                    lastDistance = currDistance
                    print '\nEnter actual distance in mm from bottom of bottle:',
                    raw = raw_input()
                    if (len(str(raw)) == 9) and re.search('([+,-]\d{5}\.\d{2})', raw, re.DOTALL):
                        print 'Error: invalid input format...'
                        continue
                    elif float(raw) > 450.0:
                        print 'ERROR: input value is too large...'
                        continue
                    else:
                        currDistance = float(raw)
                    break
                #### Catch ctrl-c input and exit gracefully
                except KeyboardInterrupt:
                    print '\n\n------ Ctrl-C input detected ------'
                    print '\nAborting script...'
                    try:
                        ## Rename file to note test aborted
                        os.rename(outputFilePath, outputFilePath[:-4] + '_script_failed.csv')
                        break
                    except:
                        ## Ignore error renaming output file
                        break
                except:
                    print 'ERROR: input had incorrect format'
                    currDistance = lastDistance

            if USB_SCALE == 'AND':
                ## Round weight to floor and calculate corresponding pressure and distance values
                if (currML % 1) != 0:
                    #Round down and calculate value
                    convFactor = (currML - math.floor(currML))/(currML - lastML)
                    currML = int(math.floor(currML))
                    currPressure = int(round(currPressure - convFactor*(currPressure - lastPressure)))
                    currDistance = currDistance - convFactor*(currDistance - lastDistance)

        ## Save row(s) to CSV file
        if currML == 0:
            ## Save first row
            writer.writerow([currML, currPressure, currDistance])
        else:
            numNewRows = int(currML) - int(lastML) +1
            if numNewRows > 1:
                liquid_levels = numpy.linspace(lastML,currML,numNewRows).tolist()
                pressure_levels = numpy.linspace(lastPressure,currPressure,numNewRows).tolist()
                distance_levels = numpy.linspace(lastDistance, currDistance, numNewRows).tolist()

                for i in range(1,numNewRows):
                    ## Save rows of measurements to CSV file
                    writer.writerow([int(liquid_levels[i]), int(pressure_levels[i]), '{:.2f}'.format(distance_levels[i])])

        ## Exit if last measurement completed
        if remainingML <= 0:
            break

        if USB_SCALE == 'DYMO':
            ## Check if scale turned off
            if scale_turned_off:
                print '\nError: Scale turned off. Cancelling test.'
                time.sleep(4)
                sys.exit(0)

        ## Fill in specified increments, or until remainingML is 0
        if (remainingML > 0):
            lastML = currML
            if lastML == 0:
                fillML = 100
            elif ((remainingML - incrementML) < 0):
                fillML = remainingML
            else:
                fillML = incrementML

            if USB_SCALE == 'DYMO':
                lastWeightInGrams = int(currWeightInGrams[0])
            elif USB_SCALE == 'AND':
                lastWeightInGrams = currWeightInGrams

            print '\n-----------------------------------'
            print '\nFilling to approximately ' + str(currML + fillML) + ' mL...'

            ## Fill fillML milliliters
            output = SendCommand(serialArduino, 'fill ' + str(fillML * 2) + '\n', 'Filling \d+ mL')
            time.sleep(fillSecondsPerML * fillML * 2)
            print 'Measuring...'

            if USB_SCALE == 'DYMO':
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
                diffInGrams = int(currWeightInGrams[0]) - int(lastWeightInGrams)
                remainingML -= diffInGrams

            elif USB_SCALE == 'AND':
                ## Get stable weight measurement
                cnt = 0
                while True:
                    retval = SendCommand(serialAND, 'S\r\n', ['ST,\+[\d]{5}\.[\d]{2}  g'])
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

                currML = currWeightInGrams

                ## Calculate how much liquid was filled
                diffInGrams = int(math.floor(currWeightInGrams)) - int(math.floor(lastWeightInGrams))
                remainingML -= diffInGrams

    ## Close the output csv file
    outputFile.close()

    print '\n------- Calibration Finished ------\n'

#### Catch ctrl-c input and exit gracefully
except KeyboardInterrupt:
    print '\n\n------ Ctrl-C input detected ------'
    print '\nAborting script...'
    try:
        ## Rename file to note test aborted
        os.rename(outputFilePath, outputFilePath[:-4] + '_script_failed.csv')
    except:
        ## Ignore error renaming output file
        pass
## Catch sys.exit() commands and exit gracefully
except SystemExit as exitType:
    ## If soft exit, do nothing
    if exitType == 0:
        pass
    elif exitType == 1:
        ## Hard exit. Test failed while communicating. Close open files and rename.
        if os.path.isfile(outputFilePath):
            if not outputFile.closed:
                outputFile.close()
            ## Rename file to note test failed
            os.rename(outputFilePath, outputFilePath[:-4] + '_calibration_failed.csv')
except:
    retval = re.search('Resource busy: \'(.*)\'', str(traceback.format_exc()), re.DOTALL)
    if retval and retval.groups()[0] == portArduino:
        print 'Error: The Arduino (%s) is in use by another terminal session.' % retval.groups()[0],
        print 'Please close it before running this script.'
    else:
        print '\n-------- Caught Exception! --------\n'
        print traceback.format_exc()
        try:
            ## Rename file to note test failed
            if not outputFile.closed:
                os.rename(outputFilePath, outputFilePath[:-4] + '_script_failed.csv')
        except:
            ## Ignore error renaming output file
            pass

finally:
    print '\n-------- Shutdown Sequence --------\n'
    try:
        ## Close the output csv file
        outputFile.close()
    except:
        ## Ignore error closing output file
        pass
    try:
        retval = SendCommand(serialArduino, r'speed 100', r'Speed = \d+ ms')
        if retval == '':
            print 'Error setting speed of pump.\n'
    except:
        pass
    try:
        if serialArduino and serialArduino.isOpen():
            SendCommand(serialArduino, 'stop\n', None)
            print 'Stopped the pump'
            serialArduino.close()
            print 'Closed serial port connection to Arduino'
    except:
        print 'Unable to stop pump and close serial port connection'
    if (USB_SCALE != None) and (USB_SCALE == 'AND'):
        try:
            ## Turn scale on
            cnt = 0
            while True:
                retval = SendCommand(serialAND, 'OFF\r\n', ['\x06\r\n'])
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
    print '\n-------- Shutdown Completed -------\n'
