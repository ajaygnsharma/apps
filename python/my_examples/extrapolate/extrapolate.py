
import numpy
import sys
import time
import math
from math import pi

volStack    = None
heightStack = None
maxML       = 1000
remainingVol=0
incrementMLForBody   = 100
incrementMLForPushUp = 5
incrementMLForNeck   = 5
pushUpReached= False
remainingVol          = 0
levelState = 'belowPushup'

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
        #rawVols           = numpy.linspace(1, numNewRows, \
        #                                      numNewRows).tolist()
        #log_levels        = numpy.log(rawVols)
        #distance_levels   = numpy.add(log_levels, \
        #                                  rawDistanceLevels)
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

    return [vols, depths]
'''
    else:
        numNewRows = int(currVol) - int(lastVol) + 1
        if numNewRows > 1:
            liquid_levels = numpy.linspace(lastVol,currVol,numNewRows).tolist()
            if( pushupReached == True ):
                distance_levels = numpy.linspace(lastDistance, currDistance, numNewRows).tolist()
                a = numpy.asarray(distance_levels,dtype = int)

    print a
    print liquid_levels
'''
'''
def extrapolate(lastVol, currVol, lastDistance, currDistance,pushupReached):
    global levelState
    a = None
    liquid_levels = None

    if(levelState == 'belowPushup'):
        if(pushupReached == True):
            levelState = 'afterPushup'
            numNewRows = int(currVol) - int(0) + 1

            if numNewRows > 1:
                liquid_levels = numpy.linspace(0,currVol,\
                                           numNewRows).tolist()
                distance_levels = numpy.linspace(lastDistance,\
                         (lastDistance-int(currVol)), numNewRows).tolist()
                #rawVols           = numpy.linspace(1, numNewRows, \
                #                                      numNewRows).tolist()
                #log_levels        = numpy.log(rawVols)
                #distance_levels   = numpy.add(log_levels, \
                #                                  rawDistanceLevels)
                a = numpy.asarray(distance_levels,dtype = int)
    else:
        numNewRows = int(currVol) - int(lastVol) + 1
        if numNewRows > 1:
            liquid_levels = numpy.linspace(lastVol,currVol,numNewRows).tolist()
            if( pushupReached == True ):
                distance_levels = numpy.linspace(lastDistance, currDistance, numNewRows).tolist()
                a = numpy.asarray(distance_levels,dtype = int)

    print a
    print liquid_levels
'''

def calculateFillML(lastML, currVol):
    fillML = 0
    global incrementMLForBody
    global incrementMLForPushUp
    global incrementMLForNeck
    global maxML
    global remainingVol
    print  "maxML=%d,currVol=%d"%(int(maxML),int(currVol))
    global pushUpReached

    if (lastML == 0):
        fillML = 5 #First pour to be 10ml
        ## Fill smaller amount after bottle 80% full
    elif (currVol < (maxML * 0.80)):
        if ((remainingVol - incrementMLForBody) < 0):
            fillML = remainingVol
        elif (pushUpReached == False):
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

def getWeight():
    return(volStack.pop(0))

def readHeight():
    return(heightStack.pop(0))

def runLoop():
    global incrementMLForNeck, incrementMLForBody,\
           remainingVol, pushUpReached


    currVol      = 0
    currDepth = 0
    totalDepth= 312
    cfgTubeOD    = 12.9
    cfgTubeID    = 9.3
    lastWeight = 0
    remainingVol      = 1000
    lastVol           = 0
    lastDepth         = 0
    pushupHeight = readHeight()
    pushUpReachedJustNow = True
    vols = None
    depths = None

    ## Test loop
    while (True):
        ## Fill in specified increments, or until remainingVol is 0
        lastVol = currVol
        fillML  = calculateFillML(lastVol, currVol)
        print '\n-----------------------------------'
        print '\nFilling to approximately ' + str(currVol + fillML) + ' mL...'
        print 'Measuring...'
        time.sleep(1)
        currWeight = getWeight()
        currHeight = readHeight()

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
                    vols, depths = interpolate(lastVol, currVol, totalDepth, \
                                               currDepth,pushUpReached)
                else:
                    vols, depths = interpolate(lastVol, currVol, lastDepth, \
                                               currDepth)
            else:
                currVol = int(currWeight)
                # Dont interpolate till the push Up is reached.

        ## Calculate how much liquid was filled
        diffInGrams  = int(math.floor(currWeight)) - int(math.floor(lastWeight))
        lastWeight   = currWeight
        remainingVol = int(remainingVol) - diffInGrams
        print 'Height:%.2f, Depth:%.2f'%(currHeight,currDepth)
        print "Remaining Vol=%d, currVol=%d" % (remainingVol,currVol)
        print vols
        print depths

        ## Exit if last measurement completed
        if remainingVol <= 0:
            print "Remaining Ml=%f" % remainingVol
            break


def main(argv):
    global volStack, heightStack
    volStack    = [5,10,15,20,25,30,35,40,45,50,55,60,65]
    heightStack = [18,18,18,18,18,18,18,18,20,25,35,45,55]

    #distances = [312, 5, 0]
    #extrapolate(distances)
    #distances = [294, 5, 0]
    #extrapolate(distances)
    #distances = [293, 5, 0]
    #extrapolate(distances)
    runLoop()

#------------------------------------------------------------------------------
# call to main()
#------------------------------------------------------------------------------
if __name__ == "__main__":
    main(sys.argv[1:])
