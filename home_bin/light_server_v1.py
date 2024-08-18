#!/usr/bin/python
import socket
import time

UDP_IP = "192.168.0.10"
UDP_PORT = 2000

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))
while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    s = ("%s,%s" % ( data, time.strftime('%Y-%m-%d,%H:%M:%S') ) );
    print("%s" % s);
    f = open('sun.log','a')
    f.write('%s\n' % s ) 
    f.close() 
    #print("%s,%s" % (data,time.strftime('%Y-%m-%d-%H:%M:%S')));
    
