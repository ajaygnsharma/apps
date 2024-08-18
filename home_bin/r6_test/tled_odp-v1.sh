#!/bin/bash
set -x

chk_st (){
 STAT=$2;
 R=`echo $STAT | sed -ne '/0/p'`
 if [ -z $R ]
 then
	echo "test_1 failed"
 else
	echo "test_1 passed"
 fi
 sleep 5
}

test_1 (){
 STAT=`nbc smb-vsc testled IDLE`
 chk_st $STAT

 STAT=`nbc smb-vsc testled RUNNING`
 chk_st $STAT

 STAT=`nbc smb-vsc testled IDLE`
 chk_st $STAT

 STAT=`nbc smb-vsc testled FAILED 2`
 chk_st $STAT
 sleep 15

 STAT=`nbc smb-vsc testled IDLE`
 chk_st $STAT
}



test_2 (){
 STAT=`nbc smb-vsc testled TUMMY`
 chk_st $STAT

 STAT=`nbc smb-vsc testled FAILED 0`
 chk_st $STAT

 STAT=`nbc smb-vsc testled FAILED 31`
 chk_st $STAT

 STAT=`nbc smb-vsc testled FAILED 32`
 chk_st $STAT

 STAT=`nbc smb-vsc testled FAILED 300`
 chk_st $STAT

 STAT=`nbc smb-vsc testled FAILED foo`
 chk_st $STAT
}


test_1
#test_2

exit 0
