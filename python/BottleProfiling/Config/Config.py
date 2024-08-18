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
import numpy
import pandas
import subprocess
import os
path = os.getcwd()
sys.path.append(path+'/../')
from Bottle import Bottle

import traceback
import math

class Config(object):
    '''
    classdocs
    '''
    def __init__(self, measurementPath):
        '''
        Constructor
        '''
        self.inputPath = ''
        self.path = '/Users/' + getpass.getuser() + measurementPath
        if not os.path.isdir(self.path):
            self.path = '/Users/' + getpass.getuser() + measurementPath
        self.RawProfileDataFilePath   = ''
        self.rawProfileDataWriter     = ''
        self.rawProfileDataFileHandle = ''
        self.ActualDistancesFilePath  = ''

    def setRawProfileDataFile(self, bottle):
        ## Create parent path of bottle
        self.path = self.path + bottle
        if not os.path.isdir(self.path):
            os.makedirs(self.path)
        
        ## Open CSV file for saving distances
        self.RawProfileDataFilePath = self.path + '/Raw_Profile_Data.csv'
        if os.path.isfile(self.RawProfileDataFilePath):
            while True:
                print 'Warning: Profile data already exists. Replace existing files (Y/N)? ',
                choice = raw_input()
                if choice in ['N','n','No','no']:
                    print '\nCancelling profile...\n'
                    sys.exit(0)
                    break
                elif any(choice in y for y in ['Y','y','Yes','yes']):
                    break
                else:
                    print 'Error: input value not recognized.'     
        
    def openRawProfileData(self, bottleName):
        self.setRawProfileDataFile(bottleName)
        self.rawProfileDataFileHandle = open(self.RawProfileDataFilePath, 'wb')
        self.rawProfileDataWriter = csv.writer(self.rawProfileDataFileHandle)
        # Setup CSV header
        self.rawProfileDataWriter.writerow(['Liquid (mL)', 'Actual Distance (mm)'])
   
    ## Save first row
    def saveRawProfileDataRow(self, currML, currDistance):
        self.rawProfileDataWriter.writerow([currML, currDistance])
    
    def closeRawProfileDataFile(self):
        ## Close the output csv file
        if not self.rawProfileDataFileHandle.closed:
            self.rawProfileDataFileHandle.close()
        
    def abort(self):
        try:
            self.closeRawProfileDataFile()
            if os.path.isfile(self.RawProfileDataFilePath):
                ## Rename file to note test aborted
                os.rename(self.RawProfileDataFilePath, self.RawProfileDataFilePath[:-4] + '_aborted.csv')
        except:
            ## Ignore error renaming output file
            pass   
        
    def fail(self):    
        if os.path.isfile(self.RawProfileDataFilePath):
            self.closeRawProfileDataFile()
            ## Rename file to note test failed
            os.rename(self.RawProfileDataFilePath, self.RawProfileDataFilePath[:-4] + '_profiling_failed.csv')

    def formatBottleUPC(self, UPC):
        bottle_UPC = ''
        while True:
            bottle_UPC = UPC
            if len(str(bottle_UPC)) >= 6 and re.match('^[0-9]+$', str(bottle_UPC), re.DOTALL):
                padding_len = 13 - len(str(bottle_UPC))
                padding = '0'*padding_len
                bottle_UPC = padding + str(bottle_UPC)
                break
            else:
                print 'Error: UPC must be at least 6 digits...'
        
        return bottle_UPC
    
    def writeActualDistances(self, bottle_UPC):
        self.closeRawProfileDataFile()
        profileData = pandas.read_csv(self.RawProfileDataFilePath, header=0, encoding='utf-8')
        maxML = None
        liquidCol = profileData.ix[:maxML,0]
        heightCol = profileData.ix[:maxML,1]
        ## Reverse row order in columns
        liquidCol = liquidCol.iloc[::-1]
        heightCol = heightCol.iloc[::-1]
        ## Output data to new profile file
        output = pandas.DataFrame({'Liquid (mL)': liquidCol, 'Actual Distance (mm)': heightCol})
        self.ActualDistancesFilePath = self.path + '/Actual_Pour_Distances_' + bottle_UPC + '.csv'
        output.to_csv(self.ActualDistancesFilePath, \
                      columns=['Liquid (mL)', 'Actual Distance (mm)'], \
                      sep=',', index=False, header=True, mode='wb')
                               
    def getActualDistancePath(self): 
        return self.ActualDistancesFilePath 
    
    def GenerateProfile(self,filepath):
        error = None
        print '----- Generating Profile Data -----\n'
        try:
            ruby_path = os.path.normpath('/Users/' + getpass.getuser() + '/dev/Scripts/Ruby/csv_profile/csv_process.rb')
            process = subprocess.Popen(['sudo','ruby',str(ruby_path),'-p',filepath], shell=False,stdin=subprocess.PIPE, stdout=subprocess.PIPE,stderr=subprocess.PIPE)
            output, error = process.communicate()
            print process.returncode
            print error
            print "output:"+output
            output = output.splitlines()
            for line in output:
                print line.replace('"','')
            process.wait()
            print '----- Profile Generation Finished -----\n'
            return
        except:
            print 'Error: unable to generate the profile data\n'
            print sys.exc_info()[0]
            print traceback.format_exc()
            if error != None:
                print error
            time.sleep(4)
            sys.exit(0)
    
def main(argv):
    config    = Config()

    print 'Enter UPC',
    bottle_UPC = config.readBottleUPC()
    bottle = Bottle.Bottle(bottle_UPC)
    fullDepth, neckHeight, vol, bottleName, bottleHeight = bottle.getInfo()
    '''
    config.openRawProfileData(bottleName)
    
    #### Prompt user for test parameters
    print '\n--------- Test Parameters ---------\n'
    maxML = int(vol)
    print 'Max mL:%d' % maxML
    totalDistance = float(bottleHeight)

    print 'Enter the height of the push up in mm:',
    pushupHeight = 0 #TODO: float(raw_input())
    print 'Push up height=%f' % pushupHeight

    #### Main test section
    print '\n------- Starting Measurements -----\n'

    ## Start bottle at 0mL
    incrementMLForNeck = ((float(vol) * 1)/100)
    incrementMLForBody = ((float(vol) * 1)/100)
    overPourML         = 0
    ## Measure a little more to account for over-pouring/pumping in future automation tests
    remainingML        = maxML + overPourML 
    lastML = 0
    currML = 0
    lastHeight = 0
    currHeight = 0
    currDistance = 0
    lastDistance = 0
    currWeightInGrams = 0
    lastWeightInGrams = 0
    print "Curr Height %f " % currHeight
    '''
    ActualDistancesPath = config.path + '/' + 'Actual_Pour_Distances_' + bottle_UPC + '.csv'
   
    
    print '\n-------- Measuring Finished -------\n'
    ## Generate profile data
    config.GenerateProfile(ActualDistancesPath)
'''    
    c = Config()
    c.setRawProfileDataFile('beaker')
    c.writeActualDistances('1000000000')
''' 

#------------------------------------------------------------------------------
# call to main()
#------------------------------------------------------------------------------
if __name__ == "__main__":
    main(sys.argv[1:])           
    