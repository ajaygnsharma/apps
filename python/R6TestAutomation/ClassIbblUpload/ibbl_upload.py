'''
Created on Nov 26, 2014

@author: asharma
'''
import pexpect
import sys
import os
import time

class ibbl_upload(object):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
    
    def upload_log(self):
        print '''uploading log'''
        print '''----------------------------------------------------------'''            
        child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
        #subprocess.call(['/ocz/users/asharma/bin/hosttool', "/dev/ttyUSB0"])
        child.expect('\r\nOCZ> ')
        child.sendline('upload log /tmp/tmp.log')
        child.expect('\r\nOCZ> ', timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data  
        child.close()
     
    def decode_log(self):    
        '''log data''' 
        print '''decoding log data'''
        print '''----------------------------------------------------------'''            
        child = pexpect.spawn('/ocz/users/asharma/bin/log /tmp/tmp.log StringTable.gz')
        child.expect(pexpect.EOF, timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data
        os.remove('/tmp/tmp.log')
        
        
    def upload_bbl(self):
        print '''uploading bbl'''
        print '''----------------------------------------------------------'''            
        child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
        #subprocess.call(['/ocz/users/asharma/bin/hosttool', "/dev/ttyUSB0"])
        child.expect('\r\nOCZ> ')
        
        child.sendline('ibbscan 256')
        child.expect('\r\nOCZ> ', timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data  
            
        time.sleep(5)
            
        child.sendline('defects-get')
        child.expect('\r\nOCZ> ', timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data  
            
        '''    
        time.sleep(5)
            
        child.sendline('defects-get c')
        child.expect('\r\nOCZ> ', timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data  
        ''' 
        child.sendline('upload bbl /tmp/bbl.log')
        child.expect('\r\nOCZ> ', timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print >> sys.stderr, data  
        
        child.close()
        
        time.sleep(5)
        
        child = pexpect.spawn('/usr/bin/sudo',['/ocz/tools/bin/nbc',\
                                               'smb-show-log','1'])
        
        child.expect(pexpect.EOF, timeout=None)
        cmd_show_data =  child.before
        cmd_output = cmd_show_data.split('\r\n')
        for data in cmd_output:
            print data
        