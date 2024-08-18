#!/bin/bash
#set -x


chk_st (){
STAT=$1;

if [ "$STAT" -eq "0" ]
 then
	echo "passed"
 else
	echo "failed"
 fi
}


test_1 (){
	echo "test 1: upload file to PC"
	STAT=`bob_nbc smb-vsc upload log ~/test.log`
	chk_st $?
	echo
}

echo 'Running upload test'
echo
test_1
sleep 5

exit 0
