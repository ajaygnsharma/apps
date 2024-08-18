'''
Created on Apr 10, 2019

@author: user1
'''
#incrementMLForNeck = 5


def calculateFillVol(lastVol, currDepth, neckHeight, shoulderOffset,\
                    incrementMLForBody, incrementMLForPushUp, remainingVol,pushUpReached):
    fillVol = 0
    #global incrementMLForBody
    #global incrementMLForPushUp
    #lobal remainingVol

    if (lastVol == 0):
        fillVol = 5 #First pour to be very low
    elif (currDepth > (neckHeight + shoulderOffset)):
        if ((remainingVol - incrementMLForBody) < 0):
            fillVol = remainingVol
        elif (pushUpReached == False):
            fillVol = incrementMLForPushUp # Fill smallest amount for pushup
        else:
            fillVol = incrementMLForBody # Fill large amount for body
    else:
        if ((remainingVol - incrementMLForNeck) < 0):
            fillVol = remainingVol
        else:
            fillVol = incrementMLForNeck # Fill smaller amount for neck

    return fillVol


if __name__ == '__main__':
    lastVol      = 0
    maxVol       = 1000
    remainingVol = maxVol
    BottleHeight = 300
    currDepth    = BottleHeight

    neckHeight           = 40
    shoulderOffset       = 60

    incrementMLForBody   = 80
    incrementMLForPushUp = 5
    incrementMLForNeck   = 10

    pushUpHeight         = 15
    pushUpReached        = False

    currVol              = 0

    print "LastVol,FillVol,CurrDepth,CurrVol,RemainingVol"
    while(remainingVol > 0):
        lastVol = currVol
        fillVol = calculateFillVol(lastVol, currDepth, neckHeight, shoulderOffset,\
                    incrementMLForBody, incrementMLForPushUp, remainingVol,pushUpReached)
        currDepth -= 20
        currVol   += fillVol
        remainingVol = maxVol - currVol
        height = (BottleHeight - currDepth)
        if(height > pushUpHeight):
            pushUpReached = True

        print("%d, %d, %d, %d, %d"
              %(lastVol,fillVol,currDepth,currVol,remainingVol))



    pass