#!/bin/bash
###############################
# Test the temperature sensor
###############################
#./ts_t-odp-v1.sh
#sleep 2


#./nt-odp-v1.sh  -c 10 -b -v
#sleep 2
#./nt-odp-v1.sh  -c 10 -r -v
#sleep 2
##./nt-odp-v1.sh  -c 10 -i -v
##sleep 2
##./nt-odp-v1.sh  -c 10 -e -v
##sleep 2

###############################
# Test the DDR RAM
###############################
# These below tests work fine.
#./dt-odp-v1.sh -t 1
#sleep 2
#./dt-odp-v1.sh -t 2
#sleep 2
#./dt-odp-v1.sh -t 3
#sleep 2
# this test runs for ~ 35 seconds . So use this to see the leds flash
#./dt-odp-v1.sh -t 4
#sleep 2


# These are not usually what i test.
#./dt-odp-v1.sh -t 5 -v
#sleep 2
#./dt-odp-v1.sh -t 6 -v
#sleep 2

###############################
# Test the test LEDs
###############################
#./tled_odp-v1.sh

