#!/bin/bash
set -x
#sudo mount -v -t cifs //192.168.0.15/visual ~/visual/ -o user=omv -o=rw
sudo mount -v 192.168.0.15:visual/ /mnt/visual 
