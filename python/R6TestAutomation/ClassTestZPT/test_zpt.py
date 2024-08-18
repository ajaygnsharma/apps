'''
Created on Dec 1, 2014

@author: asharma
'''
import os
import subprocess
import pexpect
import time
import sys
import datetime

class ClassZPT(object):
    '''
    classdocs
    '''
    today = "";
    today_ = "";
    
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
                
        
    def download_script(self, script_name):
        print '''Downloading script'''
        print '''----------------------------------------------------------'''
        child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
        #subprocess.call(['/ocz/users/asharma/bin/hosttool', "/dev/ttyUSB0"])
        child.expect('\r\nOCZ> ')
        #child.sendline('download script script_mst_autostart.txt')
        child.sendline('download script ' + script_name)
        child.expect('\r\nOCZ> ', timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data  
        child.close()    
            
    def show_script(self):
        print '''script data'''
        child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
        #subprocess.call(['/ocz/users/asharma/bin/hosttool', "/dev/ttyUSB0"])
        child.expect('\r\nOCZ> ')
        print '''----------------------------------------------------------'''            
        child.sendline('show-script')
        child.expect('\r\nOCZ> ', timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data  
        child.close()
        
        #s = raw_input("\nhelp\n")
        #print """%r""" % (s)
    def start_test(self):
        print '''starting self test'''
        print '''----------------------------------------------------------'''            
        child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
        child.expect('\r\nOCZ> ')
        child.sendline('start')
        child.expect('\r\nOCZ> ', timeout=None)
        child.sendline('status')
        child.expect('\r\nOCZ> ', timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data
        child.close()
    
    def get_status(self):
        print '''Getting self test status'''
        print '''----------------------------------------------------------'''            
        child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
        child.expect('\r\nOCZ> ')
        child.sendline('status')
        child.expect('\r\nOCZ> ', timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data
        child.close()
        
    def upload_log(self):
        print '''uploading log'''
        print '''----------------------------------------------------------'''
        a = datetime.datetime.now().replace(microsecond=0)            
        child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
        #subprocess.call(['/ocz/users/asharma/bin/hosttool', "/dev/ttyUSB0"])
        child.expect('\r\nOCZ> ')
        self.today_ = time.strftime('./log/tmp_%Y%m%d_%H%M')
        print self.today_
        child.sendline('upload log '+self.today_+'.dat')
        child.expect('\r\nOCZ> ', timeout=180) # timeout after 4 minutes
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data  
        child.close()
        b = datetime.datetime.now().replace(microsecond=0)
        print("Time to download log ")
        print(b-a)
        '''log data''' 
        
    def decode_log(self):
        print '''decoding log data'''
        print '''----------------------------------------------------------'''            
        child = pexpect.spawn('/ocz/users/asharma/bin/log '+self.today_+'.dat StringTable.gz')
        child.expect(pexpect.EOF, timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        f = open(self.today_+'.log','w')
        f.write(cmd_output)
        f.close()
        #for data in cmd_output:
        #    
        #    print data
        child.close()
        
    def show_log(self):
        child = pexpect.spawn('/usr/bin/sudo',['/ocz/tools/bin/nbc',\
                                               'smb-show-log','1'])
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
        