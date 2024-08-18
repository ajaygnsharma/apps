#!/bin/bash
set -x
set -e
sudo sdparm --clear=STANDBY /dev/sdc -S
