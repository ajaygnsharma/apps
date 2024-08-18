#!/bin/bash
set -x
sudo mount -v -t cifs //192.168.0.15/doc /mnt/doc/ -o user=omv -o=rw
