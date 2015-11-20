#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../../../..)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = parking
BOARD               = k22fsh


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/include
INCLUDE += $(MQX_ROOTDIR)/rtcs/source/include


#-----------------------------------------------------------
# common 'mfs usb' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/sensors/examples/parking/main.c
SOURCES += $(MQX_ROOTDIR)/sensors/examples/parking/shell_init.c
SOURCES += $(MQX_ROOTDIR)/sensors/examples/parking/parking.c
SOURCES += $(MQX_ROOTDIR)/sensors/examples/parking/qca4002.c


#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
