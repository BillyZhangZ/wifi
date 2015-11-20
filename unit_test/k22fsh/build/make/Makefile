#----------------------------------------------------------
# setup required variables 
#----------------------------------------------------------
MQX_ROOTDIR         = $(abspath ../../../..)
LIBRARY_ROOTDIR     = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
APPLICATION_DIR     = ./$(TOOL)/$(LOADNAME)_$(CONFIG)
APPLICATION_FILE    = $(APPLICATION_DIR)/$(NAME)_$(BOARD).$(APPLICATION_EXT)
LINKER_FILE         = $(call GET_BSP_LINKER_FILE)
TYPE                = application
NAME                = test
BOARD               = k22fsh

ifeq ($(UNIT_TEST), baseboard)
CC_DEFINE += UNIT_TEST_BASEBOARD=1
endif

ifeq ($(UNIT_TEST), wifimodule)
INCLUDE += $(MQX_ROOTDIR)/rtcs/source/include
CC_DEFINE += UNIT_TEST_WIFIMODULE=1
endif

#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------

INCLUDE += $(MQX_ROOTDIR)/unit_test/source

#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/unit_test/k22fsh/source/main.c

#----------------------------------------------------------
# include partial makefiles
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/app-process.mak
#----------------------------------------------------------
