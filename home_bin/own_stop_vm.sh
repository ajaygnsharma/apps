#!/bin/bash
set -x
set -e
vboxmanage controlvm omv acpipowerbutton
vboxmanage controlvm debian_7_6_aujaar acpipowerbutton


