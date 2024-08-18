#!/bin/bash
set -x

OMV_HOME_VD="/mnt/sata_hdd/user1/vd/vsd*.raw"
OMV_VD="/home/user1/vm/omv/omv.raw"
AUJAAR_HOME="/mnt/sata_hdd/user1/vd/aujaar_etch_home.raw"
AUJAAR="/home/user1/vm/aujaar_etch/aujaar_etch.img"
BACKUP_DIR="/mnt/usb_hdd/backup/vd/"
DEL_DIR="/mnt/usb_hdd/backup/del/"

mkdir $DEL_DIR
mv ${BACKUP_DIR}/* ${DEL_DIR}

DATE=`date +"%Y%m%d"`
tar -cf ${BACKUP_DIR}${DATE}_vd_omv.tar $OMV_HOME_VD $OMV_VD  
tar -cf ${BACKUP_DIR}${DATE}_vd_godam.tar /home/user1/vm/godam 
tar -cf ${BACKUP_DIR}${DATE}_vd_aujaar.tar $AUJAAR_HOME $AUJAAR

if [ $? -eq 0 ]
then
	rm -rf ${DEL_DIR}
fi

