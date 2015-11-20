#!/bin/bash
TOP=$PWD/../../../
cd $TOP/mqx/build/make/bsp_twrk22f120m
./build_gcc_arm.sh
cd $TOP/mfs/build/make/mfs_twrk22f120m
./build_gcc_arm.sh
cd $TOP/mqx/build/make/psp_twrk22f120m
./build_gcc_arm.sh
cd $TOP/shell/build/make/shell_twrk22f120m
./build_gcc_arm.sh
