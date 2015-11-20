export HOSTENV = UNIX
export TOOLCHAIN_ROOTDIR = /home/haidong/Work/OpenK/gcc-arm-none-eabi-4_9-2014q4
export GCCV = 4.9.3
#export board = k22fsh
#export board = twrk22_qca4002
#export board = lmq1
#export board = twrk22f120m
export COMPILER = gcc
#export TARGET   = debug

pwd := $(shell pwd)
buildcmd := build_gcc_arm.sh
cleancmd := clean_gcc_arm.sh

toolchainpath := $(TOOLCHAIN_ROOTDIR)/bin/


ifeq (twrk22_qca4002, ${board})
base_build: mqx_build mfs_build shell_build rtcs_build 
base_clean: mqx_clean mfs_clean shell_clean rtcs_clean 
else
ifeq (twrk22f120m, ${board})
base_build: mqx_build mfs_build shell_build rtcs_build msi_build bluetooth_build usb_build audio_build player_build mux_build 
base_clean: mqx_clean mfs_clean shell_clean rtcs_clean msi_clean bluetooth_clean usb_clean audio_clean player_clean mux_clean 
else
ifeq (k22fsh, ${board})
base_build: mqx_build qcom_api_build mfs_build shell_build rtcs_build msi_build usb_build audio_build player_build mux_build lcd_build sensors_build 
base_clean: mqx_clean qcom_api_clean mfs_clean shell_clean rtcs_clean msi_clean usb_clean audio_clean player_clean mux_clean lcd_clean sensors_clean 
else
base_build: mqx_build mfs_build shell_build rtcs_build msi_build usb_build audio_build player_build mux_build 
base_clean: mqx_clean mfs_clean shell_clean rtcs_clean msi_clean usb_clean audio_clean player_clean mux_clean 
endif
endif
endif




mqx_build:
	make -C $(pwd)/mqx/build/make/psp_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6
	make -C $(pwd)/mqx/build/make/bsp_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

mqx_clean:
	make -C $(pwd)/mqx/build/make/psp_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6
	make -C $(pwd)/mqx/build/make/bsp_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

#/******************************************************************************************************/
#/******************************  Base Moudles *********************************************************/
#/******************************************************************************************************/
qcom_api_build:
	make -C $(pwd)/mqx/build/make/qcom_api_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

qcom_api_clean:
	make -C $(pwd)/mqx/build/make/qcom_api_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

mfs_build:
	make -C $(pwd)/mfs/build/make/mfs_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

mfs_clean:
	make -C $(pwd)/mfs/build/make/mfs_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

shell_build:
	make -C $(pwd)/shell/build/make/shell_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

shell_clean:
	make -C $(pwd)/shell/build/make/shell_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

rtcs_build:
	make -C $(pwd)/rtcs/build/make/rtcs_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

rtcs_clean:
	make -C $(pwd)/rtcs/build/make/rtcs_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

usb_build:
	make -C $(pwd)/usb/host/build/make/usb_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6
usbd_build:
	make -C $(pwd)/usb/device/build/make/usbd_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

usb_clean:
	make -C $(pwd)/usb/host/build/make/usb_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6
usbd_clean:
	make -C $(pwd)/usb/device/build/make/usbd_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

usb_v2_build:
	make -C $(pwd)/usb_v2/usb_core/host/build/make/usbh_mqx_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

usb_v2_clean:
	make -C $(pwd)/usb_v2/usb_core/host/build/make/usbh_mqx_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

usb_v2_mfs_build:
	make -C $(pwd)/usb_v2/example/host/msd/msd_fatfs/mqx/make/host_msd_fatfs_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build -j6
	cp $(pwd)/usb_v2/example/host/msd/msd_fatfs/mqx/make/host_msd_fatfs_$(board)/gcc_arm/intflash_$(TARGET)/host_msd_fatfs_$(board).elf $(pwd)/output

usb_v2_mfs_clean:
	make -C $(pwd)/usb_v2/example/host/msd/msd_fatfs/mqx/make/host_msd_fatfs_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean -j6
	rm $(pwd)/output/host_msd_fatfs_$(board).*

msi_build:
	make -C $(pwd)/msi/build/make/msi_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

msi_clean:
	make -C $(pwd)/msi/build/make/msi_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

bluetooth_build:
	make -C $(pwd)/bluetooth/build/make/bluetooth_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

bluetooth_clean:
	make -C $(pwd)/bluetooth/build/make/bluetooth_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

bluetooth_usb_build:
	mkdir -p $(pwd)/output 
	make -C $(pwd)/bluetooth/examples/bluetooth_usb/build/make/bluetooth_usb_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/bluetooth/examples/bluetooth_usb/build/make/bluetooth_usb_$(board)/gcc_arm/intflash_$(TARGET)/bluetooth_usb_$(board).elf $(pwd)/output
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/bluetooth_usb_$(board).elf $(pwd)/output/bluetooth_usb_$(board).bin
	
bluetooth_usb_clean:
	make -C $(pwd)/bluetooth/examples/bluetooth_usb/build/make/bluetooth_usb_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/bluetooth_usb_$(board).*

bluetooth_uart_build:
	mkdir -p $(pwd)/output 
	make -C $(pwd)/demo/bluetooth_uart/build/make/bluetooth_uart_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/demo/bluetooth_uart/build/make/bluetooth_uart_$(board)/gcc_arm/intflash_$(TARGET)/bluetooth_uart_$(board).elf $(pwd)/output
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/bluetooth_uart_$(board).elf $(pwd)/output/bluetooth_uart_$(board).bin
	
bluetooth_uart_clean:
	make -C $(pwd)/demo/bluetooth_uart/build/make/bluetooth_uart_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/bluetooth_uart_$(board).*

ble_uart_build:
	mkdir -p $(pwd)/output 
	make -C $(pwd)/ble_demo_gcc/build/make/ble_uart_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/ble_demo_gcc/build/make/ble_uart_$(board)/gcc_arm/intflash_$(TARGET)/ble_uart_$(board).elf $(pwd)/output
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/ble_uart_$(board).elf $(pwd)/output/ble_uart_$(board).bin
	
ble_uart_clean:
	make -C $(pwd)/ble_demo_gcc/build/make/ble_uart_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/ble_uart_$(board).*


audio_build:
	make -C $(pwd)/audio/build/make/audio_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

audio_clean:
	make -C $(pwd)/audio/build/make/audio_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

player_build:
	make -C $(pwd)/player/build/make/player_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

player_clean:
	make -C $(pwd)/player/build/make/player_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

mux_build:
	make -C $(pwd)/mux/build/make/mux_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

mux_clean:
	make -C $(pwd)/mux/build/make/mux_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

lcd_build:
	make -C $(pwd)/lcd/build/make/lcd_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

lcd_clean:
	make -C $(pwd)/lcd/build/make/lcd_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

sensors_build:
	make -C $(pwd)/sensors/build/make/sensors_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

sensors_clean:
	make -C $(pwd)/sensors/build/make/sensors_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

parking_build:
	mkdir -p $(pwd)/output 
	make -C $(pwd)/sensors/examples/parking/build/make/parking_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/sensors/examples/parking/build/make/parking_$(board)/gcc_arm/intflash_$(TARGET)/parking_$(board).elf $(pwd)/output
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/parking_$(board).elf $(pwd)/output/parking_$(board).bin
	
parking_clean:
	make -C $(pwd)/sensors/examples/parking/build/make/parking_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/parking_$(board).*

unit_test_baseboard_build: 
	UNIT_TEST=baseboard make -C $(pwd)/unit_test/build/make/unit_test_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

unit_test_baseboard_clean:
	make -C $(pwd)/unit_test/build/make/unit_test_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

unit_test_wifimodule_build:
	UNIT_TEST=wifimodule make -C $(pwd)/unit_test/build/make/unit_test_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

unit_test_wifimodule_clean:
	make -C $(pwd)/unit_test/build/make/unit_test_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6


#/******************************************************************************************************/
#/******************************************************************************************************/


#/******************************************************************************************************/
#/******************************  Audio Modules ********************************************************/
#/******************************************************************************************************/
mp3_build:
	make -C $(pwd)/audio/dec/mp3/build/make/mp3_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

mp3_clean:
	make -C $(pwd)/audio/dec/mp3/build/make/mp3_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

wav_build:
	make -C $(pwd)/audio/dec/wav/build/make/wav_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

wav_clean:
	make -C $(pwd)/audio/dec/wav/build/make/wav_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

flac_build:
	make -C $(pwd)/audio/dec/flac/build/make/flac_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

flac_clean:
	make -C $(pwd)/audio/dec/flac/build/make/flac_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

ape_build:
	make -C $(pwd)/audio/dec/ape/build/make/ape_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

ape_clean:
	make -C $(pwd)/audio/dec/ape/build/make/ape_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6

metadata_build:
	make -C $(pwd)/audio/dec/metadata/build/make/metadata_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

metadata_clean:
	make -C $(pwd)/audio/dec/metadata/build/make/metadata_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6
#/******************************************************************************************************/
#/******************************************************************************************************/

#/******************************************************************************************************/
#/******************************  MFI Modules   ********************************************************/
#/******************************************************************************************************/

mfi_build:
	make -C $(pwd)/mfi/build/make/mfi_$(board) TOOL=gcc_arm CONFIG=$(TARGET) build -j6

mfi_clean:
	make -C $(pwd)/mfi/build/make/mfi_$(board) TOOL=gcc_arm CONFIG=$(TARGET) clean -j6
    
mfi_demo_build:
	mkdir -p $(pwd)/output 
	make -C $(pwd)/mfi/examples/mfi_demo/build/make/mfi_demo_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/mfi/examples/mfi_demo/build/make/mfi_demo_$(board)/gcc_arm/intflash_$(TARGET)/mfi_demo_$(board).elf $(pwd)/output
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/mfi_demo_$(board).elf $(pwd)/output/mfi_demo_$(board).bin
	
mfi_demo_clean:
	make -C $(pwd)/mfi/examples/mfi_demo/build/make/mfi_demo_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/mfi_demo_$(board).*
#/******************************************************************************************************/
#/******************************************************************************************************/

player_demo_build:
	mkdir -p $(pwd)/output 
	make -C $(pwd)/demo/player/build/make/player_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/demo/player/build/make/player_$(board)/gcc_arm/intflash_$(TARGET)/player_$(board).elf $(pwd)/output
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/player_$(board).elf $(pwd)/output/player_$(board).bin
	
player_demo_clean:
	make -C $(pwd)/demo/player/build/make/player_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/player_$(board).*

uvc_demo_build:
	mkdir -p $(pwd)/output 
	make -C $(pwd)/demo/uvc/build/make/uvc_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/demo/uvc/build/make/uvc_$(board)/gcc_arm/intflash_$(TARGET)/uvc_$(board).elf $(pwd)/output
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/uvc_$(board).elf $(pwd)/output/uvc_$(board).bin
	
uvc_demo_clean:
	make -C $(pwd)/demo/uvc/build/make/uvc_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/uvc_$(board).*


ifeq (twrk22_qca4002, ${board})
aj_build: aj_acserver_build aj_server_build aj_smartsensor_build
aj_clean: aj_acserver_clean aj_server_clean aj_smartsensor_clean
endif

ifeq (k22fsh, ${board})
aj_build: aj_acserver_build aj_server_build aj_smartsensor_build
aj_clean: aj_acserver_clean aj_server_clean aj_smartsensor_clean
endif

aj_acserver_build:
	mkdir -p $(pwd)/output 
	make -C $(pwd)/demo/aj/allseen/services/base_tcl/sample_apps/ACServerSample/build/make/ACServerSample_$(board)  TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/demo/aj/allseen/services/base_tcl/sample_apps/ACServerSample/build/make/ACServerSample_$(board)/gcc_arm/intflash_$(TARGET)/ACServerSample_$(board).elf  $(pwd)/output
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/ACServerSample_$(board).elf $(pwd)/output/ACServerSample_$(board).bin

aj_server_build:
	mkdir -p $(pwd)/output 
	make -C $(pwd)/demo/aj/allseen/services/base_tcl/sample_apps/ServerSample/build/make/ServerSample_$(board)  TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/demo/aj/allseen/services/base_tcl/sample_apps/ServerSample/build/make/ServerSample_$(board)/gcc_arm/intflash_$(TARGET)/ServerSample_$(board).elf  $(pwd)/output
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/ServerSample_$(board).elf $(pwd)/output/ServerSample_$(board).bin

aj_smartsensor_build:
	mkdir -p $(pwd)/output 
	make -C $(pwd)/demo/aj/allseen/services/base_tcl/sample_apps/SmartSensor/build/make/SmartSensor_$(board)  TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/demo/aj/allseen/services/base_tcl/sample_apps/SmartSensor/build/make/SmartSensor_$(board)/gcc_arm/intflash_$(TARGET)/SmartSensor_$(board).elf  $(pwd)/output
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/SmartSensor_$(board).elf $(pwd)/output/SmartSensor_$(board).bin

aj_acserver_clean:
	make -C $(pwd)/demo/aj/allseen/services/base_tcl/sample_apps/ACServerSample/build/make/ACServerSample_$(board)  TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/ACServerSample_$(board).*

aj_server_clean:
	make -C $(pwd)/demo/aj/allseen/services/base_tcl/sample_apps/ServerSample/build/make/ServerSample_$(board)  TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/ServerSample_$(board).*

aj_smartsensor_clean:
	make -C $(pwd)/demo/aj/allseen/services/base_tcl/sample_apps/SmartSensor/build/make/SmartSensor_$(board)  TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/SmartSensor_$(board).*

qca_build:
	mkdir -p $(pwd)/output 
	make -C $(pwd)/demo/flash_config/build/make/flash_config_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/demo/flash_config/build/make/flash_config_$(board)/gcc_arm/intflash_$(TARGET)/flash_config_$(board).elf $(pwd)/output
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/flash_config_$(board).elf $(pwd)/output/flash_config_$(board).bin
	make -C $(pwd)/demo/throughput_demo/build/make/throughput_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/demo/throughput_demo/build/make/throughput_$(board)/gcc_arm/intflash_$(TARGET)/throughput_$(board).elf $(pwd)/output
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/throughput_$(board).elf $(pwd)/output/throughput_$(board).bin
	make -C $(pwd)/demo/throughput_demo/build/make/throughput_qcom_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/demo/throughput_demo/build/make/throughput_qcom_$(board)/gcc_arm/intflash_$(TARGET)/throughput_qcom_$(board).elf $(pwd)/output
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/throughput_qcom_$(board).elf $(pwd)/output/throughput_qcom_$(board).bin

qca_clean:
	make -C $(pwd)/demo/flash_config/build/make/flash_config_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	make -C $(pwd)/demo/throughput_demo/build/make/throughput_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	make -C $(pwd)/demo/throughput_demo/build/make/throughput_qcom_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/flash_config_$(board).*
	rm -f $(pwd)/output/throughput_$(board).*
	rm -f $(pwd)/output/throughput_qcom_$(board).*

flash_demo_build:
	make -C $(pwd)/mqx/examples/flashx/build/make/flashx_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp      $(pwd)/mqx/examples/flashx/build/make/flashx_$(board)/gcc_arm/intflash_$(TARGET)/flashx_$(board).elf $(pwd)/output

flash_demo_clean:
	make -C $(pwd)/mqx/examples/flashx/build/make/flashx_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/flash_demo_$(board).*


test_baseboard_build:
	mkdir -p $(pwd)/output 
	UNIT_TEST=baseboard make -C $(pwd)/unit_test/$(board)/build/make TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/unit_test/${board}/build/make/gcc_arm/intflash_$(TARGET)/test_$(board).elf $(pwd)/output/test_baseboard_$(board).elf
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/test_baseboard_$(board).elf $(pwd)/output/test_baseboard_$(board).bin

test_baseboard_clean:
	make -C $(pwd)/unit_test/$(board)/build/make TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/test_$(board).*

test_wifimodule_build:
	mkdir -p $(pwd)/output 
	UNIT_TEST=wifimodule make -C $(pwd)/unit_test/$(board)/build/make TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/unit_test/${board}/build/make/gcc_arm/intflash_$(TARGET)/test_$(board).elf $(pwd)/output/test_wifimodule_$(board).elf
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/test_wifimodule_$(board).elf $(pwd)/output/test_wifimodule_$(board).bin

test_wifimodule_clean:
	make -C $(pwd)/unit_test/$(board)/build/make TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/test_$(board).*

hello_build:
	mkdir -p $(pwd)/output 
	make -C $(pwd)/mqx/examples/hello/build/make/hello_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/mqx/examples/hello/build/make/hello_$(board)/gcc_arm/intflash_$(TARGET)/hello_$(board).elf $(pwd)/output
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/hello_$(board).elf $(pwd)/output/hello_$(board).bin

hello_clean:
	make -C $(pwd)/mqx/examples/hello/build/make/hello_$(board) TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/hello_$(board).*

msd_disk_build:
	make -C $(pwd)/usb/device/examples/msd/disk/build/make/msd_disk_$(board)  TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash build
	cp $(pwd)/usb/device/examples/msd/disk/build/make/msd_disk_$(board)/gcc_arm/intflash_$(TARGET)/msd_disk_$(board).elf $(pwd)/output 
	$(toolchainpath)/arm-none-eabi-objcopy -v  -O binary $(pwd)/output/msd_disk_$(board).elf $(pwd)/output/msd_disk_$(board).bin

msd_disk_clean:
	make -C $(pwd)/usb/device/examples/msd/disk/build/make/msd_disk_$(board)  TOOL=gcc_arm CONFIG=$(TARGET) LOAD=intflash clean
	rm -f $(pwd)/output/msd_disk_$(board).*




.PHONY: mqx_build mfs_build shell_build rtcs_build msi_build bluetooth_build usb_build audio_build player_build mux_build unit_test_build aj_acserver_build  aj_server_build aj_smartsensor_build mqx_clean mfs_clean shell_clean rtcs_clean msi_clean bluetooth_clean usb_clean audio_clean player_clean mux_clean unit_test_clean aj_acserver_clean  aj_server_clean aj_smartsensor_clean 
