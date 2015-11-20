#----------------------------------------------------------
# setup variables
#----------------------------------------------------------
MQX_ROOTDIR             = $(abspath ../../../..)
TYPE                    = library
NAME                    = unit_test
BOARD                   = k22fsh
LIBRARY_ROOTDIR         = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
LIBRARY_DIR             = $(LIBRARY_ROOTDIR)/$(CONFIG)/unit_test
LIBRARY_FILE            = $(LIBRARY_DIR)/unit_test.$(LIBRARY_EXT)
POSTBUILD_CMD           = $(if $(findstring $(HOSTENV),WINDOWS),$(MQX_ROOTDIR)/unit_test/build/bat/unit_test_k22fsh.bat $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL),$(MQX_ROOTDIR)/unit_test/build/bat/unit_test_k22fsh.sh $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL))


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/unit_test/source
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/cm
INCLUDE += $(MQX_ROOTDIR)/config/k22fsh
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/dma
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio/kgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hwtimer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/bsp/k22fsh
INCLUDE += $(MQX_ROOTDIR)/config/common
INCLUDE += $(MQX_ROOTDIR)/mqx/source/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/debug
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_mem
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_null
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pipe
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lpm
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pccard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcflash
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rnga
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rtc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/serial
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/tfs
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/timer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/cpu
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/core/M4
INCLUDE += $(MQX_ROOTDIR)/mqx/source/tad
INCLUDE += $(MQX_ROOTDIR)/mux/source
INCLUDE += $(MQX_ROOTDIR)/mfs/source/include
INCLUDE += $(MQX_ROOTDIR)/unit_test/source
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/unit_test/source
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/cm
INCLUDE += $(MQX_ROOTDIR)/config/k22fsh
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/dma
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio/kgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hwtimer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/bsp/k22fsh
INCLUDE += $(MQX_ROOTDIR)/config/common
INCLUDE += $(MQX_ROOTDIR)/mqx/source/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/debug
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_mem
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_null
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pipe
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lpm
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pccard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcflash
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rnga
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rtc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/serial
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/tfs
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/timer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/cpu
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/core/M4
INCLUDE += $(MQX_ROOTDIR)/mqx/source/tad
INCLUDE += $(MQX_ROOTDIR)/mux/source
INCLUDE += $(MQX_ROOTDIR)/mfs/source/include
INCLUDE += $(MQX_ROOTDIR)/unit_test/source
endif



#-----------------------------------------------------------
# 'unit_test' sources
#-----------------------------------------------------------
ifeq ($(UNIT_TEST), baseboard)
SOURCES +=  $(MQX_ROOTDIR)/unit_test/source/usb_test.c
SOURCES +=  $(MQX_ROOTDIR)/unit_test/source/alc3261.c
SOURCES +=  $(MQX_ROOTDIR)/unit_test/source/em9301.c
SOURCES +=  $(MQX_ROOTDIR)/unit_test/source/fxas21002.c
SOURCES +=  $(MQX_ROOTDIR)/unit_test/source/mag3110.c
SOURCES +=  $(MQX_ROOTDIR)/unit_test/source/microsd.c
SOURCES +=  $(MQX_ROOTDIR)/unit_test/source/mma8653.c
SOURCES +=  $(MQX_ROOTDIR)/unit_test/source/rda5876.c
SOURCES +=  $(MQX_ROOTDIR)/unit_test/source/sht25.c
SOURCES +=  $(MQX_ROOTDIR)/unit_test/source/pm2p5.c
SOURCES +=  $(MQX_ROOTDIR)/unit_test/source/i2c_common.c
SOURCES +=  $(MQX_ROOTDIR)/unit_test/source/aaa.c
CC_DEFINE += UNIT_TEST_BASEBOARD=1
endif

ifeq ($(UNIT_TEST), wifimodule)
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/include/AR6002
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/include/AR6002/hw2.0
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/include
INCLUDE += $(MQX_ROOTDIR)/demo/flash_config/src
INCLUDE += $(MQX_ROOTDIR)/rtcs/source/include
INCLUDE += $(MQX_ROOTDIR)/shell/source/include
SOURCES += $(MQX_ROOTDIR)/unit_test/source/qca4002.c
SOURCES += $(MQX_ROOTDIR)/unit_test/source/pin.c
SOURCES += $(MQX_ROOTDIR)/unit_test/source/pm2p5.c
SOURCES += $(MQX_ROOTDIR)/unit_test/source/adc_pin.c
SOURCES += $(MQX_ROOTDIR)/unit_test/source/usb_test.c
SOURCES += $(MQX_ROOTDIR)/unit_test/source/aaa.c
CC_DEFINE += UNIT_TEST_WIFIMODULE=1
endif

#----------------------------------------------------------
# load global settings
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/lib-process.mak

