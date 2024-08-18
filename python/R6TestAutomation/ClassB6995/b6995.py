'''
Created on Nov 25, 2014

@author: asharma
'''
import subprocess
import time

class b6995(object):
    '''
    classdocs
    '''


    def __init__(self):
        '''
        Constructor
        '''
    def get_log(self, t):
        if t == "i":
            with open("init.log","w+") as output:
                subprocess.call(['nbc', "smb-show-log"], stdout=output);
        elif t == "f":
            with open("final.log","w+") as output:
                subprocess.call(['nbc', "smb-show-log"], stdout=output);
                    
    def t1(self):
        
        subprocess.call(['nbc', "smb-vsc", "write", "0x0000ffff", \
                         "0xf0", "0", "2048","0", "5"])
        time.sleep(5)
        
    
  
'''
t = Myfoo()
t.t1()
'''      