#!/bin/bash
set -x
set -e
rsync -avz -e --progress "ssh -p 20030" uclinux/ user1@192.168.0.30:/home/user1/tmp 
