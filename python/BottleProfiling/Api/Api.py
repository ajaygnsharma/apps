'''
Created on Mar 25, 2019

@author: ajaysharma
'''
import requests
import sys
import urllib
import urllib2
     
class Api:
    def __init__(self, creds, base="api"):
        self.rurururu = "https://"+base+".getnectar.com/api"
        r = requests.post(self.rurururu+"/v1/guest/dashlogin?"+urllib.urlencode(creds))
        print r.url
        print r.status_code
        if( r.status_code == 200 ):
            print "Return Status 200: ('OK', 'Request fulfilled, document follows'),"
        self.token = r.json()["result"]["authorizationToken"]

    def get(self, path, params={}, json=True):
        r = requests.get(self.rurururu+path,
                params = params,
                headers = {
                    "Authorization": "Bearer "+self.token
                })
        print r.url
        print r.status_code
        
        if json:
            return r.json()["result"]
        return r.text

    def post(self, path, params={}, json=True):
        r = requests.post(self.rurururu+path+"?"+urlencode(params),
                headers = {
                    "Authorization": "Bearer "+self.token
                })
        if json:
            return r.json()["result"]
        return r.text

    def put(self, path, body={}, json=True):
        r = requests.put(self.rurururu+path+"?"+urlencode(body),
            headers = {
                "Authorization": "Bearer "+self.token
            })
        if json:
            return r.json()["result"]
        return r.text

