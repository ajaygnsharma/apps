#!/bin/bash
#set -x
EN_RDNG=0


chk_st (){
 STAT=$2;
TEST_NO=$4;

 R=`echo $STAT | sed -ne '/0/p'`
 if [ -z $R ]
 then
	echo "test "$TEST_NO" failed"
 else
	echo "test "$TEST_NO" passed"
 fi
}


test_1 (){
	echo "test 1: reading enabled"
	STAT=`nbc smb-vsc temperature_sensor read`
	chk_st $STAT 1
	echo
}

test_2 (){
	echo "test 2: just status"
	STAT=`nbc smb-vsc temperature_sensor`
	chk_st $STAT foo 2
	echo
}

test_3 (){
	echo "test 3: wrong param: writ"
	STAT=`nbc smb-vsc temperature_sensor writ`
	chk_st $STAT 3
	echo
}

echo 'Running Temperature Sensor test'
echo
test_1
sleep 5
test_2
sleep 5
test_3

exit 0
