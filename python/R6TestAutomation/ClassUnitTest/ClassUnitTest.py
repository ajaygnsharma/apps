'''
Created on Mar 3, 2015

@author: asharma
'''
from ClassR6 import ClassR6

import time
import subprocess
import datetime
import pexpect
from pexpect import TIMEOUT
from time import sleep
from matplotlib.transforms import interval_contains

class ClassUnitTest(object):
    '''
    classdocs
    '''
    def __init__(self):
        '''
        Constructor
        '''
    
    # Unit test Super-cap for AIC
    def unitTestCaptest(self, timeout_):
        print("\n\nTesting for cap-test\n\n")
        a = datetime.datetime.now().replace(microsecond=0)
        ut1 = ClassR6.ClassR6()
        ut1.openPort()
        
        print("=================UPLOAD LOG======================\n")
        ut1.uploadLog()
        time.sleep(1)
        ut1.decodeLog()
        time.sleep(1)
        
        for i in range(0, 20):
            ut1.run_cap_test(timeout_)
        
        print("=================UPLOAD LOG======================\n")
        ut1.uploadLog()
        time.sleep(1)
        ut1.decodeLog()
        time.sleep(1)
  
        b = datetime.datetime.now().replace(microsecond=0)
        print("Time to run test") 
        print(b-a)
        print(time.strftime('%Y-%m-%d-%H:%M'))
        
        ut1.closePort()    
    
    def unit_test_release(self):
        print("\n\nTesting for release of EVT/MST 50.09.05\n\n")
        a = datetime.datetime.now().replace(microsecond=0)
        ut1 = ClassR6.ClassR6()
        ut1.run_led_test()
        ut1.run_temperature_sensor_test()
        ut1.run_cap_test()
        ut1.run_pfail_check_test()
        ut1.run_led_test()
        ut1.run_cap_test()
        ut1.run_led_test()
        ut1.show_log()
        b = datetime.datetime.now().replace(microsecond=0)
        print("Time to run test")
        print(b-a)
        print(time.strftime('%Y-%m-%d-%H:%M'))
    
    def unitTestUploadLog(self):
        print("\n\nTesting for upload log\n\n")
        a = datetime.datetime.now().replace(microsecond=0)
        ut1 = ClassR6.ClassR6()
        ut1.openPort()
        
        for i in range(0, 1):
            print("=================UPLOAD #%d======================\n" %(i+1))
            ut1.uploadLog()
            time.sleep(1)
            ut1.decodeLog()
            time.sleep(1)
            
        b = datetime.datetime.now().replace(microsecond=0)
        print("Time to run test") 
        print(b-a)
        print(time.strftime('%Y-%m-%d-%H:%M'))
            
        ut1.closePort()    
    
    
    def unitTestStartDrive(self):
        print("\n\nTesting for Powerup\n\n")
        a = datetime.datetime.now().replace(microsecond=0)
        ut1 = ClassR6.ClassR6()
        child = pexpect.spawn('/ocz/users/asharma/bin/hosttool /dev/ttyUSB0')
        try:
            while(1):
                child.sendline()
                index = child.expect('\r\nOCZ> ', timeout=10)
                if index == 0:
                    break;
                
        except TIMEOUT:
            print("timeout happened\n")
        
        b = datetime.datetime.now().replace(microsecond=0)
        print("Time to run test") 
        print(b-a)
        print(time.strftime('%Y-%m-%d-%H:%M'))
            
        child.close()
            
    '''def download_zpt_fw(self,fw_name):
        t = test_zpt.ClassZPT()
        t.download_zpt_fw(fw_name)

    def test_b6995(self):
        t = b6995.b6995()
        t.get_log("i")
        t.t1()
        t.get_log("f")

    # Test the self test with download log, run test and upload log.
    def self_test(self, script_name):
        t = mst_self_test.mst_self_test()
        t.download_script(script_name)
    
    #t.show_log()
    #t.start_test()
    #t.upload_log()

    def test_ibbl_upload(self):
        t = ibbl_upload.ibbl_upload()
        t.upload_log()
        #t.decode_log()
        t.upload_bbl()

    def hostboot(self):
        t = test_zpt.ClassZPT()
        t.hostboot_fw()'''
    
    #test_b6995()
    #test_mst_ex01()
    #test_ibbl_upload()
    #download_zpt_fw("evt")
    
    #-------------------------
    # Apply serdes patch DOES NOT WORK
    #-------------------------
    #t = patch_hostboot.ClassHostBootPatch(0)
    #t.hostboot_patch
    
    #download_zpt_fw()
    
    
    # Host boot and devel firmware
    #hostboot()
    #time.sleep(5)
    #test_ibbl_upload()
    
    #Defects get
    '''
    t = test_zpt.ClassZPT()
    t.defects_get()
    '''
    '''
    print("\n\ncase 1\n\n")
    hostboot()
    time.sleep(5)
    self_test("script_mst_autostart.txt")
    '''
    
    '''
    print("\n\ncase 2\n\n")
    hostboot()
    time.sleep(5)
    self_test("script_mst_autostart_comment.txt")
    
    print("\n\ncase 3\n\n")
    hostboot()
    time.sleep(5)
    self_test("script_mst_autostart_no_autostart.txt")
    
    print("\n\ncase 4\n\n")
    hostboot()
    time.sleep(5)
    self_test("script_mst_autostart.txt")
    self_test("script_mst_autostart_no_autostart.txt")
    hostboot()
    '''
    # Testing the upload log
    #hostboot()
    #time.sleep(5)
    
    # Download firmware to a brand new drive
    #download_zpt_fw("evt")
    
    #-------------------------------------------
    # Test script download 
    #-------------------------------------------
    #hostboot()
    #time.sleep(5)
    #self_test("script_mst_check_power_failure.txt")
    
    
    #------------------------------------------
    # Abrupt Power reset feature
    # -----------------------------------------
    def power_reset_test(self, test_section):
        print("\n\nTesting power reset\n\n")
        t1 = test_zpt.ClassZPT()
        host_boot  = 0;
        run_case_1 = 0;
        run_case_2 = 1;
        
        if(host_boot == 1): 
            t1.hostboot_fw("/ocz/users/asharma/workspace/asharma_2015_03_11/"
                       "Release/mission/FUF/mt-fw-mission.fuf")
        time.sleep(5)
            
        '''
        Case 1: Start the self test and let it run till end and see log.
        Not uploading log since it is still an issue.
        '''    
        if(run_case_1 == 1):
            t1.download_script("script_mst_check_power_failure_case01.txt")
            t1.show_script()
            t1.start_test()
            t1.get_status()
            time.sleep(60)
            t1.show_log()
        
        '''
        See the log first, start the test and remove the drive in between
        '''
        if((run_case_2 == 1) & (test_section == "2")):
            time.sleep(10)
            t1.download_script("script_mst_check_power_failure_case02.txt")
            t1.show_script()
            print("\n\nUploading log before abrupt power failure\n\n")
            #t1.upload_log()
            #time.sleep(2)
            #t1.decode_log()
            t1.start_test()
            print("Remove drive NOW or Power down\n\n")
            
        
        if((run_case_2 == 1) & (test_section == "3")):
            '''
            Wait for sometime, load firmware and see the log. It should show
            that there was a abrupt reset. It will then restart the test and so we
            wait 30 seconds for the test to complete and see the log for the result
            '''
            if(host_boot == 1):
                t1.hostboot_fw("/ocz/users/asharma/workspace/evt/"
                       "Release/mission/FUF/mt-fw-mission.fuf")
        
            if(host_boot == 1):
                t1.show_log()
            t1.show_log()
            
            '''
            t1.upload_log()
            time.sleep(5)
            t1.decode_log()
            
            if(host_boot == 1):
                t1.show_log() #Now, show log should show test completed status
        
            print("Power Cycle drive again NOW\n");
            time.sleep(30)
            t1.upload_log()
            time.sleep(2)
            t1.decode_log()
            '''
        
    def vcon_out_space(self):
        print("\n\nTesting vcon out of space\n\n")
        build = "release"
        t1 = test_zpt.ClassZPT()
        '''
        if build == "debug":
            t1.hostboot_fw("/ocz/users/asharma/workspace/asharma_vcon_out_of_space/"
                       "Release/mission/FUF/mt-fw-mission.fuf")
        else:
            t1.hostboot_fw("/ocz/users/asharma/workspace/evt/"
                       "Release/mission/FUF/mt-fw-mission.fuf")
        '''
        time.sleep(5)
    
    
        i = 0;
        while i < 1:
            #
            # Scenario 1: Download script, Start test, wait for finish and  
            # upload log.   
            t1.upload_log()
            time.sleep(10)
            t1.download_script("script_mst_1TB_bug_7844.txt")
            time.sleep(5)
            t1.show_script()
            time.sleep(5)
            t1.start_test()
            time.sleep(1800)
            
            t1.show_log() #Now, show log should show test completed status
            time.sleep(5)
            t1.upload_log()
            time.sleep(10)
            #t1.decode_log()
            #t1.upload_log()
            
            #
            # Scenario 2: Upload ibbl and pbbl. Wait for 10 seconds in between
            #
            '''time.sleep(10)
            t1.upload_ibbl()
            time.sleep(10)
            t1.upload_pbbl()
            
            time.sleep(10)
            t1.upload_ibbl()
            time.sleep(10)
            t1.upload_pbbl()'''
            
            i = i + 1; 
    
            # Scenario 3: 
    
    '''
    Do a unit test
    '''    
    def unit_test(self):
        self.vcon_out_space()
        #------------------------------------------
        # Test the abrupt Power reset feature
        # -----------------------------------------
        #power_reset_test()


    '''
    Do a full regression test
    '''
    def regression_test(self):
        self.vcon_out_space()
        self.power_reset_test()
        
    def unitTestMSTScript(self, scriptName, interval):
        print("\n\nTesting a MST simulation\n\n")
        
        ut1 = ClassR6.ClassR6()
        
        ut1.openPort()
        time.sleep(1)
        
        ut1.downloadScript(scriptName)
        
        time.sleep(1)
        
        ut1.showScript()
        time.sleep(1)
        
        ut1.startTest()
        
        if(interval != 0):
            status = "4";
            while(status == "4"):
                status = ut1.pollStatus()
                print("status="+status);
                time.sleep(30)
        else:
            print("Sleeping for %d minutes\n",interval)
            time.sleep(1*60*interval) # 15 min
            
        print("Test completed\n")  
        
        ut1.uploadLog()
        time.sleep(2)
        
        ut1.decodeLog()
        time.sleep(1)
        
        ut1.closePort()
        
    
    def RunUnitTestTemperatureSensor(self):
        inActiveSelfTestTemperatureMeasurement = 1;
        activeSelfTestTemperatureMeasurement   = 1;
        
        print("\n\nTesting a MST simulation for temperature sensor\n\n")
        #build = "release"
        ut1 = ClassR6.ClassR6()
        
        ut1.openPort()
        time.sleep(1)
        
        # Temperature sensor test while Self test is not running 
        if(inActiveSelfTestTemperatureMeasurement):
            ut1.getTemperature("", "")
        
            ut1.getTemperature("r", "")
        
            ut1.getTemperature("r", "10")
        
            time.sleep(30)
        
            ut1.getTemperature("r", "0")
        
            ut1.uploadLog()
            time.sleep(2)
        
            ut1.decodeLog()
            time.sleep(1)
        
        # Temperature sensor test while Self test is running
        if(activeSelfTestTemperatureMeasurement):
            ut1.downloadScript("scripts/R6000-MST-SCR-SA5-4TB_RUN_5MIN")
            time.sleep(1)
        
            ut1.showScript()
            time.sleep(1)
        
            ut1.startTest()
            time.sleep(5)
        
            ut1.getTemperature("r", "")
        
            status = "4";
            while(status == "4"):
                status = ut1.pollStatus()
                print("status="+status);
                time.sleep(30)
            
            print("Test completed\n")    
            
            #time.sleep(1*60*10) # Giving a 6 min delay
        
            ut1.uploadLog()
            time.sleep(2)
        
            ut1.decodeLog()
            time.sleep(1)
        
        ut1.closePort()
        
        