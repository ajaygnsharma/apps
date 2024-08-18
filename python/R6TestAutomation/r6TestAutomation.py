#!/usr/bin/python
import sys, getopt
import time

from ClassUnitTest import ClassUnitTest

test_num                     = 0;

'''
Below are Unit tests
'''
RunUnitTestStartDrive        = 0;
RunUnitTestPowerReset        = 0;

''' 
For regression test: Enable below all
'''
RunUnitTestUploadLog         = 0;
RunUnitTestRelease           = 0;

RunUnitTestTemperatureSensor = 0;
RunUnitTestCapTest           = 0;

AIC_4TB                      = 0;
AIC_1TB                      = 0;
fullMediaTest                = 0;
shortMediaTest               = 0;
RunUnitTestMSTScript         = 1;
RunUnitTestCapTestWithDelay  = 0;
RunUnitTestCapTestInsideScriptWithDelay = 1;

def main(argv):
    try:
        opts, args = getopt.getopt(argv,"ht:",["test="])
    except getopt.GetoptError:
        print 'main.py -t <test number>'
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print 'test.py -t <test number>'
            sys.exit()
        elif opt in ("-t", "--test"):
            test_num = arg
        else:
            assert False, "unhandled option"
    
    ut1 = ClassUnitTest.ClassUnitTest()
    
    

    '''
    -----------------------------------------------------------------------
    # Power reset test
    # test num | meaning
    # +--------+--------------------------------------------------------------- 
    # |  1     | Run a test and let it finish. the flag will be raised. 
    # |        | When test finishes the flag is reset.
    # +--------+---------------------------------------------------------------
    # |  2     | Run a test, power off the PC and then turn ON.
    # |        | In the log we can see a power reset happens.
    # |        | But the flag is reset right after that.
    # +--------+---------------------------------------------------------------
    # |  3     | Run the MST firmware as is, there should be nothing happening
    # |        | to the flag.
    # +--------+---------------------------------------------------------------
    -----------------------------------------------------------------------
    '''
    if(RunUnitTestPowerReset):
        ut1.power_reset_test(test_num)
    
    if(RunUnitTestStartDrive):
        ut1.unitTestStartDrive()
    
    '''
    -----------------------------------------------------------------------
    # Was done during the 50_09_05 release
    -----------------------------------------------------------------------
    '''
    if(RunUnitTestRelease):
        ut1.unit_test_release()    
    
    if(RunUnitTestMSTScript):
        if(fullMediaTest):
            if(AIC_4TB):
                ut1.unitTestMSTScript(
                    "scripts/mt-bistscript-ea-3-a_full_media.txt",60)
            elif(AIC_1TB):
                ut1.unitTestMSTScript(
                    "scripts/mt-bistscript-ea-1-a_full_media.txt",60)
        if(shortMediaTest):
            if(AIC_4TB):
                ut1.unitTestMSTScript(
                    "scripts/mt-bistscript-ea-3-a.txt",60)
            elif(AIC_1TB):
                ut1.unitTestMSTScript(
                    "scripts/mt-bistscript-ea-1-a.txt",60)
        if(RunUnitTestCapTestInsideScriptWithDelay):
            ut1.unitTestMSTScript("scripts/super_cap_script.txt",0)
            
    '''
    -----------------------------------------------------------------------
    # Upload log test. There are problems with the uploading of log 
    # where it times out with error <err 0xY3>. 
    -----------------------------------------------------------------------
    '''
    if(RunUnitTestUploadLog):
        ut1.unitTestUploadLog()
        
    #time.sleep(5)
        
    if(RunUnitTestTemperatureSensor):
        ut1.RunUnitTestTemperatureSensor()
    
    
    # Run Unit test for Supercap on AIC. 
    if(RunUnitTestCapTest):
        ut1.unitTestCaptest();
        
        
    if(RunUnitTestCapTestWithDelay):
        ut1.unitTestCaptest(660);
    
#unit_test()
#regression_test()
#mst_test()


#------------------------------------------------------------------------------
# PATCH SERDES - To allow sbl boot on PCI slot and then host-boot.
#------------------------------------------------------------------------------
'''
p1 = patch_hostboot.ClassHostBootPatch()
p1.hostboot_patch()
'''
    
#------------------------------------------------------------------------------
# call to main()
#------------------------------------------------------------------------------
if __name__ == "__main__":
    main(sys.argv[1:])   
