#!/bin/bash
set -x
if [ $1 == 1 ]
then
nbc smb-stomp-sbl 
echo "Power cycle and then zpt the board"
exit 0
fi

if [ $1 == 2 ]
then
cd ~/workspace/evt_trunk_latest/Release/mission/FUF
zpt -v bw-bundle-sa-1-a.fuf Xsa2-7 \"Ajay\"
fi
