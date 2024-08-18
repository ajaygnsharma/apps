# ProfileBottle.py
# Created by Luke Spradlin on April 24, 2016.
# Ajay Sharma Mar 2019
# Copyright (c) 2016-2019 Nectar Inc. All rights reserved.

import getpass
import time
import re
import os
import sys
import csv
import glob
import hid #imported from hidapi
import numpy
import pandas
import subprocess
import ConfigParser

import traceback
import math
from Pump import Pump
from WeightScale import WeightScale
from LazerSensor import LazerSensor
from Config import Config
from Bottle import Bottle
from math import pi

print '\n-----------------------------------'
print '---------- Profile Bottle ---------'
print '------- Type Ctrl+Z to abort ------'
print '-----------------------------------'

maxML                = 0
incrementMLForBody   = 0
incrementMLForPushUp = 5
incrementMLForNeck   = 0
pushUpReached        = False
remainingVol         = 0

def calculateTubeVol(tubeOD, tubeID, height):
    # Inner tube area is ADDED to the weight being measured per ml
    TubeOD       = tubeOD
    TubeID       = tubeID
    TubeODArea   = pi * ((TubeOD/2)**2)
    TubeIDArea   = pi * ((TubeID/2)**2)
    TubeCSArea   = TubeODArea - TubeIDArea
    TubeVolPermm = (TubeCSArea * 0.001)
    TubeVol      = height * TubeVolPermm
    return TubeVol

# If push is not reached,
def interpolateVol(lastVol, currVol, n):
    vols = None
    if n > 1:
        vols = numpy.linspace(lastVol,currVol, n).tolist()
    return vols

def interpolateDepth(lastDepth, currDepth, n):
    depths = None
    if n > 1:
        rDepths = numpy.linspace(lastDepth,currDepth, n).tolist()
        depths = numpy.asarray(rDepths,dtype = int)
    return depths

def calculateRows(currVol, lastVol):
    n = int(currVol) - int(lastVol) + 1
    return n

def interpolate(lastVol, currVol, lastDepth, currDepth, pushUpReached=False):
    if ( pushUpReached ):
        n      = calculateRows(currVol, 0)
        vols   = interpolateVol(0, currVol, n)
        depths = interpolateDepth(lastDepth, currDepth, n)
    else:
        n      = calculateRows(currVol, lastVol)
        vols   = interpolateVol(lastVol, currVol, n)
        depths = interpolateDepth(lastDepth, currDepth, n)

    return [vols, depths, n]

def calculateFillML(lastML, currVol):
    fillML = 0
    global incrementMLForBody
    global incrementMLForPushUp
    global incrementMLForNeck
    global maxML
    global remainingVol
    print  "maxML=%d,currVol=%d"%(int(maxML),int(currVol))

    if (lastML == 0):
        fillML = 5 #First pour to be 10ml
        ## Fill smaller amount after bottle 80% full
    elif (currVol < (maxML * 0.80)):
        if ((remainingVol - incrementMLForBody) < 0):
            fillML = remainingVol
        elif (pushupHeightConfirmed == False):
            fillML = incrementMLForPushUp
        else:
            fillML = incrementMLForBody
    ## Fill regular amount first 80% of bottle
    else:
        if ((remainingVol - incrementMLForNeck) < 0):
            fillML = remainingVol
        else:
            fillML = incrementMLForNeck

    return fillML

#### Main Script
try:
    weightScale = None
    pump        = None
    global incrementMLForNeck, incrementMLForBody, remainingVol, pushupHeightConfirmed

    cfg = ConfigParser.ConfigParser()
    cfg.read('config.ini')

    cfgIncrementMLForBody = float(cfg.get('DEFAULT','INCREMENT_BODY'))
    cfgIncrementMLForNeck = float(cfg.get('DEFAULT','INCREMENT_NECK'))
    cfgUPC                = cfg.get('DEFAULT','UPC')
    cfgLaserRefOffset     = float(cfg.get('LASER','REF_OFFSET'))
    cfgLaserFloatOffset   = float(cfg.get('LASER','FLOAT_OFFSET'))
    cfgLaserShimOffset    = float(cfg.get('LASER','SHIM_OFFSET'))
    cfgTubeOD             = float(cfg.get('WEIGHTSCALE','TUBEOD'))
    cfgTubeID             = float(cfg.get('WEIGHTSCALE','TUBEID'))
    cfgCapMeasurementPath = str(cfg.get('PATH','CAP_MEASUREMENT'))
    cfgEmail              = str(cfg.get('API','EMAIL'))
    cfgPassword           = str(cfg.get('API','PASSWORD'))

    config    = Config.Config(cfgCapMeasurementPath)

    #### Prompt user for test parameters
    print '\n--------- Profiler ---------\n'
    bottle_UPC = config.formatBottleUPC(cfgUPC)
    bottle     = Bottle.Bottle(bottle_UPC,cfgEmail, cfgPassword)
    fullDepth, neckHeight, vol, bottleName, bottleHeight, bottomThickness = bottle.getInfo()

    incrementMLForNeck   = ((float(vol) * cfgIncrementMLForNeck)/100)
    incrementMLForBody   = ((float(vol) * cfgIncrementMLForBody)/100)

    global maxML
    maxML = int(vol)
    print 'Max mL:%d' % maxML
    overPourML           = 0
    ## Measure a little more to account for over-pouring/pumping in future
    remainingVol         = maxML + overPourML
    lastML               = 0
    currVol              = 0
    currDistance         = 0
    lastDistance         = 0
    currWeightInGrams    = 0
    lastWeightInGrams    = 0
    pushUpReachedJustNow = True
    vols                 = None
    depths               = None

    totalDistance = (float(bottleHeight) - float(bottomThickness))

    config.openRawProfileData(bottleName)
    ## Connect to Pump
    pump = Pump.Pump()
    ## Wait for devices to connect
    time.sleep(4)
    fillSecondsPerML = pump.getFillTime()
    fillSecondsPerML = pump.getSpeed()

    # Connect to weight Scale
    weightScale = WeightScale.WeightScale()
    if( weightScale.isANDScaleConnected() != None ):
        weightScale.connectANDScale()
    elif( weightScale.isDYMOScaleConnected() != None ):
        weightScale.connectDYMOScale()

    # Connect to Laser sensor
    lazer = LazerSensor.LazerSensor(cfgLaserRefOffset,cfgLaserFloatOffset,cfgLaserShimOffset)

    #### Main test section
    print '\n------- Starting Measurements -----\n'
    pushupHeight = lazer.readHeight()
    print "Push up height=%f" % (pushupHeight)

    while (True):
                ## Fill in specified increments, or until remainingVol is 0
        lastVol = currVol
        fillML  = calculateFillML(lastVol, currVol)
        print '\n-----------------------------------'
        print '\nFilling to approximately ' + str(currVol + fillML) + ' mL...'
        print 'Measuring...'
        time.sleep(5)
        currWeight = weightScale.getWeight()
        currHeight = lazer.readHeight()

        # Some liquid filled, calculate height
        if (currWeight > 0 and currHeight > 0.0):
            ## Read value from pressure sensor
            lastDepth = currDepth
            currDepth = totalDepth - currHeight
            if (pushUpReached == False):
                if (math.floor(currHeight) > math.floor(pushupHeight)):
                    pushUpReached = True
                    print "!!!!push up confirmed"
            if (pushUpReached):
                currVol = int(currWeight +\
                            calculateTubeVol(cfgTubeOD, cfgTubeID, currHeight))
                if(pushUpReachedJustNow == True):
                    pushUpReachedJustNow = False
                    vols, depths, n = interpolate(lastVol, currVol, totalDepth,\
                                               currDepth,pushUpReached)
                else:
                    vols, depths, n = interpolate(lastVol, currVol, lastDepth,\
                                               currDepth)
            else:
                currVol = int(currWeight)
                # Dont interpolate till the push Up is reached.

            ## Calculate how much liquid was filled
            diffInGrams  = int(math.floor(currWeight)) -\
                           int(math.floor(lastWeight))
            lastWeight   = currWeight
            remainingVol = int(remainingVol) - diffInGrams
            print 'Height:%.2f, Depth:%.2f'%(currHeight,currDepth)
            print "Remaining Vol=%d, currVol=%d" % (remainingVol,currVol)
            print vols
            print depths

            ## Save rows of measurements to CSV file
            for i in range(1,n):
                config.saveRawProfileDataRow(int(vols[i]), '{:.2f}'.format(depths[i]))


        ## Exit if last measurement completed
        if remainingVol <= 0:
            print "Remaining Ml=%f" % remainingVol
            break

        weightScale.checkScaleOff()

    #### Create csv file with profiler format
    ## Read columns from File
    config.writeActualDistances(bottle_UPC)
    ActualDistancePath = config.getActualDistancePath()

    print '\n-------- Measuring Finished -------\n'
    ## Generate profile data
    config.GenerateProfile(ActualDistancePath)

#### Catch ctrl-c input and exit gracefully
except KeyboardInterrupt:
    print '\n\n------ Ctrl-C input detected ------'
    print '\nAborting script...'
    config.abort()
    pump.stop()

## Catch sys.exit() commands and exit gracefully
except SystemExit as exitType:
    ## If soft exit, do nothing
    if exitType == 0:
        pass
    elif exitType == 1:
        ## Hard exit. Test failed while communicating. Close open files and rename.
        config.fail()

    retval = re.search('Resource busy: \'(.*)\'', str(traceback.format_exc()), re.DOTALL)
    if retval and retval.groups()[0] == pump.getPortName():
        print 'Error: The Arduino (%s) is in use by another terminal session.' % retval.groups()[0],
        print 'Please close it before running this script.'
    else:
        print '\n-------- Caught Exception! --------\n'
        print traceback.format_exc()
        config.fail()

finally:
    print '\n-------- Shutdown Sequence --------\n'
    try:
        config.closeRawProfileDataFile()
    except Exception, e:
        print str(e)
        ## Ignore error closing output file
        pass
    if( weightScale != None):
        weightScale.off()
    print '\n-------- Shutdown Completed -------\n'
