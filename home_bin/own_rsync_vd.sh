#!/bin/bash
set -x

OMV_HOME_VD="/mnt/sata_hdd/vd/vsd*.raw"
OMV_VD="/home/user1/vm/omv/omv.img"
AUJAAR_HOME="/mnt/sata_hdd/vd/aujaar_home.raw"
AUJAAR="/home/user1/vm/aujaar_etch/aujaar.img"
BACKUP_DIR="/mnt/usb_hdd/backup/vd/"

rsync -az ${OMV_HOME_VD} ${BACKUP_DIR} 
rsync -az ${OMV_VD} ${BACKUP_DIR} 
rsync -az ${AUJAAR_HOME} ${BACKUP_DIR} 
rsync -az ${AUJAAR} ${BACKUP_DIR} 
rsync -az /home/user1/vm/godam/ ${BACKUP_DIR}

