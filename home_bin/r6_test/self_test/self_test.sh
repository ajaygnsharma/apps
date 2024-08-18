#!/bin/bash
# Run scripts to download a test script, run it and upload its log.
# It also converts the uploaded log from hex to text format.
set -x
set -e

SCRIPT=./script03_pe.txt
STRING_TABLE=~/workspace/evt_trunk/Release/StringTable.gz

bob_nbc smb-vsc download script $SCRIPT
nbc smb-vsc show-script
nbc smb-vsc start
# Sleep for 15 mins
sleep 900
nbc smb-vsc status all
bob_nbc smb-vsc upload log ./log_hex.txt
TIMESTAMP=`date +%Y%m%d%H%M`
log ./log_hex.txt $STRING_TABLE > ./log_ascii$TIMESTAMP.txt


