'''
Created on Mar 3, 2015

@author: asharma
'''
import os
import subprocess
import pexpect
import time
import sys
import datetime
import re

class ClassR6(object):
    '''
    classdocs
    '''
    today = "";
    today_ = "";
    child = "";
    
    def __init__(self):
        '''
        Constructor
        '''
    
    def download_zpt_fw(self, fw_type):
        os.system('nbc smb-stomp-sbl')
        os.system('nbc smb-stomp-sbl')
        os.system('nbc sbl-restart')
        os.system('nbc pci-scan')
        child = 0
        if fw_type == "evt":
            child = pexpect.spawn('/usr/bin/sudo',['/ocz/tools/bin/zpt','-v',\
                '/ocz/users/asharma/workspace/evt/Release/mission/'
                'FUF/mt-bundle-sa-3-a.fuf',\
                'XA21XW061447000030','assete2']) 
        elif fw_type == "bbl":
            child = pexpect.spawn('/usr/bin/sudo',['/ocz/tools/bin/zpt','-v',\
                '/ocz/users/asharma/workspace/asharma_2014_12_01_bbl_with_log/'
                'Release/mission/FUF/mt-bundle-sa-1-a.fuf',\
                'X04:00:00','assete2'])                        
        elif fw_type == "release":
            child = pexpect.spawn('/usr/bin/sudo',['/ocz/tools/bin/zpt','-v',\
                '/ocz/users/asharma/Documents/EVT_50_09_01/'
                'mt-bundle-sa-1-a.fuf',\
                '04:00:00','assete2'])             
        
        child.expect(pexpect.EOF, timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data
            
    def hostboot_fw(self, firmware):
        child = pexpect.spawn('/usr/bin/sudo',['/ocz/tools/bin/nbc',\
                                               'sbl-restart'])
        
        child.expect(pexpect.EOF, timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data
            
        time.sleep(2)
            
        child = pexpect.spawn('/usr/bin/sudo',['/ocz/tools/bin/nbc',\
            'host-boot',\
            firmware])
        #/ocz/users/asharma/workspace/asharma_2014_12_01_bbl_with_log/'            
        child.expect(pexpect.EOF, timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data


    def defects_get(self):
        child = pexpect.spawn('/usr/bin/sudo',['/ocz/tools/bin/nbc',\
                                               'smb-vsc','defects-get'])
        
        child.expect(pexpect.EOF, timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data
        
        for i in range(0, 10):
            child = pexpect.spawn('/usr/bin/sudo',['/ocz/tools/bin/nbc',\
                                               'smb-vsc','defects-get','c'])
        
            child.expect(pexpect.EOF, timeout=None)
            cmd_show_data =  child.before
            cmd_output = cmd_show_data.split('\r\n')
            for data in cmd_output:
                print data
                
        
    def downloadScript(self, script_name):
        print '''Downloading script'''
        print '''----------------------------------------------------------'''
        self.child.sendline('download script ' + script_name)
        self.child.expect('\r\nOCZ> ', timeout=240)
        cmd_show_data =  self.child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data  
            
            
    def getTemperature(self, reading, interval):
        print '''Temperature sensor command'''
        cmd = "temperature_sensor"
        if reading != "":
            cmd+= " r"
        
        if interval != "":
            cmd+= " "
            cmd+=interval
        
        #print cmd
        
        self.child.sendline(cmd)
        self.child.expect('\r\nOCZ> ', timeout=240)
        cmd_show_data =  self.child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data  
        
        
    def showScript(self):
        print '''script data'''
        self.child.sendline('show-script')
        self.child.expect('\r\nOCZ> ', timeout=240)
        cmd_show_data =  self.child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data  
        
        #s = raw_input("\nhelp\n")
        #print """%r""" % (s)
    
    def startTest(self):
        print '''starting self test'''
        print '''----------------------------------------------------------'''            
        self.child.sendline('start')
        self.child.expect('\r\nOCZ> ', timeout=240)
        self.child.sendline('status')
        self.child.expect('\r\nOCZ> ', timeout=240)
        cmd_show_data =  self.child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data
        
    def openPort(self):
        self.child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
        self.child.expect('\r\nOCZ> ')
        
    def uploadLog(self):
        timestampFlg = 0;
        print '''uploading log'''
        print '''----------------------------------------------------------'''
        
        if(timestampFlg):
            a = datetime.datetime.now().replace(microsecond=0)            
        #subprocess.call(['/ocz/users/asharma/bin/hosttool', "/dev/ttyUSB0"])
        
        self.today_ = time.strftime('./log/tmp_%Y%m%d_%H%M')
        #print self.today_
        self.child.sendline('upload log '+self.today_+'.dat')
        self.child.expect('\r\nOCZ> ', timeout=240) # timeout after 4 minutes
        cmd_show_data =  self.child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data  
        
        if(timestampFlg):
            b = datetime.datetime.now().replace(microsecond=0)
            print("Time to download log ")
            print(b-a)
        
    def closePort(self):
        self.child.close()
        
    def decodeLog(self):
        searchForWarning = 0;
        print '''decoding log data'''
        print '''----------------------------------------------------------'''            
        child = pexpect.spawn('/ocz/users/asharma/bin/log '+self.today_+'.dat'\
                              ' StringTable.gz')
        
        child.expect(pexpect.EOF, timeout=120)
        cmd_show_data =  child.before
        
        f = open(self.today_+'.log', 'w')
        f.write(cmd_show_data)
        f.close()
                
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            #print data
            if(searchForWarning):
                searchObj = re.search( r'(.*)warn(.*?) .*', data, re.M|re.I)
                if searchObj:
                    print searchObj.group()            
                
        child.close()

        
    def show_log(self):
        child = pexpect.spawn('/usr/bin/sudo',['/ocz/tools/bin/nbc',\
                                               'smb-show-log'])
        child.expect(pexpect.EOF, timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data
        child.close()
              
    def upload_pbbl(self):
        child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
        #subprocess.call(['/ocz/users/asharma/bin/hosttool', "/dev/ttyUSB0"])
        child.expect('\r\nOCZ> ')
        a = datetime.datetime.now().replace(microsecond=0)
        #child.sendline('upload '+ type +' /tmp/'+type+'.log')
        child.sendline('upload pbbl /tmp/pbbl.log')
        child.expect('\r\nOCZ> ', timeout=120)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data  
        child.close()
        b = datetime.datetime.now().replace(microsecond=0)
        print("Time to download log ")
        print(b-a)
           
    def upload_ibbl(self):
        child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
        #subprocess.call(['/ocz/users/asharma/bin/hosttool', "/dev/ttyUSB0"])
        child.expect('\r\nOCZ> ')
        a = datetime.datetime.now().replace(microsecond=0)
        #child.sendline('upload '+ type +' /tmp/'+type+'.log')
        child.sendline('upload ibbl /tmp/ibbl.log')
        child.expect('\r\nOCZ> ', timeout=240) # timeout in 4 minutes
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data  
        child.close()   
        b = datetime.datetime.now().replace(microsecond=0)
        print("Time to download log ")
        print(b-a)
        
    def pollStatus(self):
        self.child.sendline('status\r\n')
        self.child.expect('\r\nOCZ> ', timeout=6000)
        cmd_show_data =  self.child.before # = status\r\n\r\nStatus:\r\n 0\r\n\r
        cmd_output = cmd_show_data.split(':') #['status\r\n\r\nStatus', '\r\n 0\r\n\r']
        for data in cmd_output: #Iterating over above
            status = data.split("\r\n") #['', ' 0', '\r']
            #print status
            for x in status:  # Iterating over above
                #print x
                if(x == " 0"):
                    #print x
                    return "0"
        return "4"
        
        
    def run_led_test(self):
        child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
        child.expect('\r\nOCZ> ')
        
        child.sendline('testled IDLE')
        child.expect('\r\nOCZ> ', timeout=240) # timeout in 4 minutes
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data
            
        child.sendline('testled RUNNING')
        child.expect('\r\nOCZ> ', timeout=240) # timeout in 4 minutes
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data
            
        time.sleep(20)    
              
        child.sendline('testled IDLE')
        child.expect('\r\nOCZ> ', timeout=240) # timeout in 4 minutes
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data
            
        time.sleep(5)
        
        child.sendline('testled FAILED 2')
        child.expect('\r\nOCZ> ', timeout=240) # timeout in 4 minutes
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data
        
        time.sleep(20)
        
        child.sendline('testled IDLE')
        child.expect('\r\nOCZ> ', timeout=240) # timeout in 4 minutes
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data
            
        time.sleep(5)
        
        child.close()   
        
    def run_temperature_sensor_test(self):
        child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
        child.expect('\r\nOCZ> ')
        a = datetime.datetime.now().replace(microsecond=0)
        child.sendline('temperature_sensor r')
        child.expect('\r\nOCZ> ', timeout=240) # timeout in 4 minutes
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data    
        
        child.sendline('temperature_sensor')
        child.expect('\r\nOCZ> ', timeout=240) # timeout in 4 minutes
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data
        
        child.close()   
    
    def run_cap_test(self,timeout_):
        openPort = 0;
        if(openPort):
            child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
            child.expect('\r\nOCZ> ')
            
        self.child.sendline('cap-test')
        self.child.expect('\r\nOCZ> ', timeout=timeout_) # timeout in 4 minutes
        cmd_show_data =  self.child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data    
            
        self.child.sendline('cap-test 21045 4500 23200')
        self.child.expect('\r\nOCZ> ', timeout=timeout_) # timeout in 4 minutes
        cmd_show_data =  self.child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data    
            
        if(openPort):        
            child.close()   
    
    def run_pfail_check_test(self):
        child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
        child.expect('\r\nOCZ> ')
        child.sendline('pfail-check 1')
        child.expect('\r\nOCZ> ', timeout=240) # timeout in 4 minutes
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data    
            
        child.sendline('pfail-check 0')
        child.expect('\r\nOCZ> ', timeout=240) # timeout in 4 minutes
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data    
            
        child.close()   
    
        