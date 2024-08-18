#!/bin/bash
# mount omv directory for easy access
set -x
OMV_DIR='/home/user1/prsnl/omv'

if [ $1 ]
then
   if [ $1 == '-u' ] 
   then
     sudo fusermount -u $OMV_DIR
   fi  
else
   sshfs -o idmap=user omv@agns.no-ip.org:files/ $OMV_DIR -p 20015
fi

