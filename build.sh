#!/bin/bash
# $1: board type: current support twrk22f120m, twrk22_qca4002, k22fsh
# $2: build type: debug or release
# $3: build module or application
# $4: build or clean
export board=$1
export TARGET=$2
make ${3}_${4}
