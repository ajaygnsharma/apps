'''
Created on Dec 2, 2014

@author: asharma
'''
import os

class ClassHostBootPatch(object):
    '''
    classdocs
    '''
    #asharma_2014_12_01_bbl_with_log/Boot"
    project_root = ""
    
    def __init__(self):
        '''
        Constructor
        '''
        self.project_root = "/ocz/users/asharma/workspace/\
        evt/Boot"
            
    def hostboot_patch(self):
        os.system("cd " + self.project_root);
        os.system("patch -p1 -R <"
                  " /ocz/users/asharma/diff/pbl_script_ignore_serdes.diff");
              

        