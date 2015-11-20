#!/bin/bash
TOP=$PWD/../../../
cd $TOP/usb_core/device/build/make/usbd_mqx_twrk22f120m
./clean_gcc_arm.sh
cd $TOP/usb_core/host/build/make/usbh_mqx_twrk22f120m
./clean_gcc_arm.sh
cd $TOP/usb_core/otg/build/make/usbotg_mqx_twrk22f120m
./clean_gcc_arm.sh
