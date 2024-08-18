#!/bin/bash
set -x
set -e
OMVC_BAK="/mnt/usb_hdd/backup/omvc"

rsync -av /home/user1/bin ${OMVC_BAK}
rsync -av /etc/fstab ${OMVC_BAK}
rsync -av /etc/cifspasswd ${OMVC_BAK}
rsync -av /etc/network/interfaces ${OMVC_BAK}


