#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../../..)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = throughput
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
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/include
INCLUDE += $(MQX_ROOTDIR)/demo/throughput_demo/custom
INCLUDE += $(MQX_ROOTDIR)/rtcs/source/include
INCLUDE += $(MQX_ROOTDIR)/shell/source/include

#-----------------------------------------------------------
# common 'throughput' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/demo/throughput_demo/common/wmiconfig.c

#-----------------------------------------------------------
# custom 'throughput' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/demo/throughput_demo/custom/Command_Lists.c
SOURCES += $(MQX_ROOTDIR)/demo/throughput_demo/custom/https.c
SOURCES += $(MQX_ROOTDIR)/demo/throughput_demo/custom/RTCS.c
SOURCES += $(MQX_ROOTDIR)/demo/throughput_demo/custom/rtcs_throughput.c
SOURCES += $(MQX_ROOTDIR)/demo/throughput_demo/custom/Shell_Task.c
SOURCES += $(MQX_ROOTDIR)/demo/throughput_demo/custom/task_manager.c
SOURCES += $(MQX_ROOTDIR)/demo/throughput_demo/custom/Tasks.c
SOURCES += $(MQX_ROOTDIR)/demo/throughput_demo/custom/throughput.c
SOURCES += $(MQX_ROOTDIR)/demo/throughput_demo/custom/utils.c
SOURCES += $(MQX_ROOTDIR)/demo/throughput_demo/custom/wmiconfig_custom.c

#-----------------------------------------------------------
# 'throughput' driver plugin
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/demo/throughput_demo/driver_plugin/atheros_wifi_mediactl_ext.c

#-----------------------------------------------------------
# 'throughput' sensor
#-----------------------------------------------------------

SOURCES += $(MQX_ROOTDIR)/demo/throughput_demo/sensor/sensor.c
SOURCES += $(MQX_ROOTDIR)/demo/throughput_demo/sensor/tmp106.c

#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
