#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../../..)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = utf_agent
BOARD               = twrk22_qca4002


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
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/include/AR6002
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/include/AR6002/hw2.0
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/include
INCLUDE += $(MQX_ROOTDIR)/demo/utf_agent/src
INCLUDE += $(MQX_ROOTDIR)/rtcs/source/include
INCLUDE += $(MQX_ROOTDIR)/shell/source/include


#-----------------------------------------------------------
# common 'utf_agent' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/demo/utf_agent/src/main.c
#-----------------------------------------------------------
# 'utf_agent' driver plugin
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/demo/utf_agent/driver_plugin/atheros_wifi_init.c
SOURCES += $(MQX_ROOTDIR)/demo/utf_agent/driver_plugin/atheros_wifi_mediactl_ext.c
SOURCES += $(MQX_ROOTDIR)/demo/utf_agent/driver_plugin/atheros_wifi_raw_if.c



#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
