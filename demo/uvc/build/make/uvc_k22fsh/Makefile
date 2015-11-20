#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../../..)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = uvc
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
# common sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/demo/uvc/source/main.c
SOURCES += $(MQX_ROOTDIR)/demo/uvc/source/shell_init.c
SOURCES += $(MQX_ROOTDIR)/demo/uvc/source/usb_classes.c
SOURCES += $(MQX_ROOTDIR)/demo/uvc/source/uvc_task.c
SOURCES += $(MQX_ROOTDIR)/demo/uvc/source/qca4002.c


#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
