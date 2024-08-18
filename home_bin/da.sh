#!/bin/bash
du -ah $1 | sort -h -r | head -n 10
