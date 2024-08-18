#!/bin/bash
usage(){
	echo 'Perform various DDR tests'
	echo '-------------------------'
	echo 'usage: $0 -f <filename> -t <number> -s -v'
	echo '-f = enable logging to <filename>'
	echo '-t = run a test type specified by <number>'
	echo '-v = verbose'
	echo '-s = stop on error'
	exit 1
}

i=1

EN_FW_DOWNLOAD=0
EN_LOGGING=0
LOG_FILE=""
TEST_NUM=0
STOP_ON_ERROR=0

while getopts "svf:t:" o; do
	case "${o}" in
		f)
			EN_LOGGING=1
			LOG_FILE=${OPTARG}
			;;
		t)
			TEST_NUM=${OPTARG}
			;;
		v)
			set -x;
			;;
		s)
			STOP_ON_ERROR=1;
			;;
		*)
			usage
			;;
	esac
done
shift $((OPTIND-1))

CHANNEL=0x0000FFFF
TGT=0xFF
BLK_OFF=0
BLK_START=0
BLK_RD=16
BLK_CNT=16
PAGE_OFF=0
PAGE_CNT=256
VFY=0

if [ $EN_LOGGING == '1' ]
then
	exec &> >(tee -a $LOG_FILE)
fi

ADDR=0
COUNT=100
PATTERN=0
LOOPS=1

# Check to see if a DDR test passed
chk_st () {
	while [[ -z $R ]];
	do
		echo "Test running, "$SECONDS
		sleep 1
		STATUS=`nbc smb-vsc ddr-test`
		R=`echo $STATUS | sed -ne '/: 0/p'`
		#nbc smb-vsc temperature_sensor read
	done
	echo "Test passed"
}

if [ $TEST_NUM == '1' ]
then
	nbc smb-vsc ddr-test $ADDR $COUNT $PATTERN $STOP_ON_ERROR $LOOPS
	chk_st
fi

if [ $TEST_NUM == '2' ]
then
	PATTERN=1
	LOOPS=10
	nbc smb-vsc ddr-test $ADDR $COUNT $PATTERN $STOP_ON_ERROR $LOOPS
	chk_st
fi

if [ $TEST_NUM == '3' ]
then
	PATTERN=2
	LOOPS=1000
	nbc smb-vsc ddr-test $ADDR $COUNT $PATTERN $STOP_ON_ERROR $LOOPS
	chk_st
fi

if [ $TEST_NUM == '4' ]
then
	PATTERN=3
	LOOPS=1000
	COUNT=10000
	nbc smb-vsc ddr-test $ADDR $COUNT $PATTERN $STOP_ON_ERROR $LOOPS
	chk_st
fi


if [ $TEST_NUM == '5' ]
then
	PATTERN=3
	LOOPS=10000
	COUNT=10000
	nbc smb-vsc ddr-test $ADDR $COUNT $PATTERN $STOP_ON_ERROR $LOOPS
	chk_st
fi

if [ $TEST_NUM == '6' ]
then
	PATTERN=4
	LOOPS=10000
	COUNT=10000
	nbc smb-vsc ddr-test $ADDR $COUNT $PATTERN $STOP_ON_ERROR $LOOPS
	chk_st
fi


