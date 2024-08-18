#!/bin/bash
set -x
sudo mount -v -t cifs //192.168.0.15/music ~/music/ -o user=omv -o=rw
