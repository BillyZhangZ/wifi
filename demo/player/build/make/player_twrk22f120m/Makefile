#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../../..)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = player
BOARD               = twrk22f120m


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------



#-----------------------------------------------------------
# common 'mfs usb' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/demo/player/source/main.c
SOURCES += $(MQX_ROOTDIR)/demo/player/source/shell_init.c
SOURCES += $(MQX_ROOTDIR)/demo/player/source/usb_classes.c
SOURCES += $(MQX_ROOTDIR)/demo/player/source/usb_bluetooth_task.c
SOURCES += $(MQX_ROOTDIR)/demo/player/source/usb_mfi_task.c
SOURCES += $(MQX_ROOTDIR)/demo/player/source/umass_task.c
SOURCES += $(MQX_ROOTDIR)/demo/player/source/USB_File.c
SOURCES += $(MQX_ROOTDIR)/demo/player/source/adk20.c
SOURCES += $(MQX_ROOTDIR)/demo/player/source/aoa_task.c

INCLUDE += $(MQX_ROOTDIR)/externallibs/bluetooth/include

#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
