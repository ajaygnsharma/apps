#!/bin/bash
set -x

mplayer -geometry 320x240+0+0  -lavdopts skipframe=nonref:skiploopfilter=all  tv://
