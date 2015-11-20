#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../../)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = ble_uart
BOARD               = k22fsh




#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------

INCLUDE += $(MQX_ROOTDIR)/ble_demo_gcc/spi_driver
INCLUDE += $(MQX_ROOTDIR)/ble_demo_gcc/Platform_Interfaces
INCLUDE += $(MQX_ROOTDIR)/ble_demo_gcc/Interfaces
INCLUDE += $(MQX_ROOTDIR)/ble_demo_gcc/Interfaces/Services
INCLUDE += $(MQX_ROOTDIR)/ble_demo_gcc/Interfaces/ATT

#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak

#-----------------------------------------------------------
# common 'spi driver' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/ble_demo_gcc/spi_driver/init_spi.c
SOURCES += $(MQX_ROOTDIR)/ble_demo_gcc/spi_driver/spi.c
SOURCES += $(MQX_ROOTDIR)/ble_demo_gcc/spi_driver/spi_dspi.c
SOURCES += $(MQX_ROOTDIR)/ble_demo_gcc/spi_driver/spi_dspi_common.c
SOURCES += $(MQX_ROOTDIR)/ble_demo_gcc/spi_driver/spi_dspi_dma.c
SOURCES += $(MQX_ROOTDIR)/ble_demo_gcc/spi_driver/spi_mk22_dspi.c


#-----------------------------------------------------------
# common 'platform' sources
#-----------------------------------------------------------

SOURCES += $(MQX_ROOTDIR)/ble_demo_gcc/Platform_Interfaces/K22_EM9301_Radio.c
SOURCES += $(MQX_ROOTDIR)/ble_demo_gcc/Platform_Interfaces/K22_Hci_Spi.c
SOURCES += $(MQX_ROOTDIR)/ble_demo_gcc/Platform_Interfaces/K22_System.c

#-----------------------------------------------------------
# common 'main' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/ble_demo_gcc/source/main.c
SOURCES += $(MQX_ROOTDIR)/ble_demo_gcc/source/BleApp.c


#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
