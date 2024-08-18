#!/bin/bash
BACKUP_DIR='/home/user1/backup/pixel2xl/'
rsync -avz ./Omnichan $BACKUP_DIR
rsync -avz ./DCIM     $BACKUP_DIR
#rsync -avz Music     $BACKUP_DIR
#rsync -avz documents $BACKUP_DIR
#rsync -avz Pictures  $BACKUP_DIR
#rsync -avz CamScanner  $BACKUP_DIR
#rsync -avz WhatsApp/Media  $BACKUP_DIR

