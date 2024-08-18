#!/bin/bash
set -x
#sudo mount /dev/sdb6 /mnt/sdb6
vboxmanage startvm omv --type headless
#vboxmanage startvm debian_7_6_aujaar --type headless
vboxmanage startvm aujaar_etch --type headless
