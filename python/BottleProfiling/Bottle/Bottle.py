'''
Created on Mar 26, 2019

@author: ajaysharma
'''
import sys
import os
path = os.getcwd()
sys.path.append(path+'/../')
from Api import Api

class Bottle:
    def __init__(self, upc,email, password):
        api = Api.Api({"email": email, "password": password})
        r = api.get("/v3/product", {
                    "productUpc": upc,
                    "selectFields": "bottle"
            })
        try:
            self.fullDepth  = r["bottle"]["fullDepth"]
            self.neckHeight = r["bottle"]["neckHeight"]
            self.size       = r["bottle"]["size"]
            self.bottleName = r["bottle"]["bottleName"]
            self.bottleHeight    = r["bottle"]["height"]
            self.bottomThickness = r["bottle"]["bottomThickness"]
            print r["bottle"]["fullDepth"]
            print r["bottle"]["neckHeight"]
            print r["bottle"]["size"]
            print r["bottle"]["bottleName"]
            print r["bottle"]["bottomThickness"]
            print self.bottleHeight
        except Exception, e:
            print str(e)
        #What is the name variable?
    
    def getInfo(self):
        return (self.fullDepth, self.neckHeight,\
                self.size, self.bottleName, self.bottleHeight,self.bottomThickness)

def main(argv):
    b = Bottle("10000000002881")
    b.getInfo()
    

#------------------------------------------------------------------------------
# call to main()
#------------------------------------------------------------------------------
if __name__ == "__main__":
    main(sys.argv[1:])           
    