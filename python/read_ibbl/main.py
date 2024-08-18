#!/usr/bin/python

with open('pbbl_20150219_1703.txt', 'r') as f:
    data = f.readlines()

    for line in data:
        words = line.split()
        #channel = (int(words[0], base=16) & 0xf4) >> 3;
        page_div = ((int(words[0], base=16)) % 256);
        channel = ((int(words[0], base=16))& 0xf4000000) >> (24+3);
        target = ((int(words[0], base=16)) & 0x07000000) >> 24;
        lun = ((int(words[0], base=16)) & 0x00200000);
        
        if ( (page_div == 0) and (target <= 8) and (channel <= 32) ) : 
            ok = 1;
        if( ok == 1 ) : 
            print "channel=%d, target=%d, page_div=%d, lun=%d,%s" \
            % (channel, target, page_div, lun, words[0])
        else :
            print "error"
        
        
        