#!/bin/bash
TOP=$PWD/../../../
cd $TOP/mqx/build/make/bsp_twrk22_qca4002
./clean_gcc_arm.sh
cd $TOP/mqx/build/make/psp_twrk22_qca4002
./clean_gcc_arm.sh
cd $TOP/mfs/build/make/mfs_twrk22_qca4002
./clean_gcc_arm.sh
cd $TOP/rtcs/build/make/rtcs_twrk22_qca4002
./clean_gcc_arm.sh
cd $TOP/shell/build/make/shell_twrk22_qca4002
./clean_gcc_arm.sh
