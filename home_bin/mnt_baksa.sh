#!/bin/bash
# mount omv directory for easy access
set -x
UCL_DIR='/home/user1/prsnl/dev/uclinux'

if [ $1 ]
then
   if [ $1 == '-u' ] 
   then
     sudo fusermount -u $UCL_DIR
   fi  
else
   sshfs -o idmap=user $USER@agns.no-ip.org:/home/user1 $UCL_DIR -p 20010
fi

