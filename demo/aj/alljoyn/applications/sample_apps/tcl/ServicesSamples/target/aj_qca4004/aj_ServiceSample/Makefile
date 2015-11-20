# Copyright (c) 2013 Qualcomm Atheros, Inc..
# All Rights Reserved.
# Qualcomm Atheros Confidential and Proprietary.
#
#
#############################################################
# Required variables for each makefile
# Discard this section from all parent makefiles
# Expected variables (with automatic defaults):
#   CSRCS (all "C" files in the dir)
#   SUBDIRS (all subdirs with a Makefile)
#   GEN_LIBS - list of libs to be generated ()
#   GEN_IMAGES - list of images to be generated ()
#   COMPONENTS_xxx - a list of libs/objs in the form
#     subdir/lib to be extracted and rolled up into
#     a generated lib/image xxx.a ()
#
ifndef PDIR

GEN_LIBS = libaj_ServiceSample.a

endif


#############################################################
# Configuration i.e. compile options etc.
# Target specific stuff (defines etc.) goes in here!
# Generally values applying to a tree are captured in the
#   makefile at its root level - these are then overridden
#   for a subtree within the makefile rooted therein
#


#############################################################
# Recursion Magic - Don't touch this!!
#
# Each subtree potentially has an include directory
#   corresponding to the common APIs applicable to modules
#   rooted at that subtree. Accordingly, the INCLUDE PATH
#   of a module can only contain the include directories up
#   its parent path, and not its siblings
#
# Required for each makefile to inherit from the parent
#

INCLUDES := $(INCLUDES) -I . -I $(PDIR)include -I $(WLANCMN_DIR)/include/cust -I $(WLANCMN_DIR)/include/qcom -I $(WLANFW_DIR)/import/rev7/include/ -DREV74_TEST_ENV1

INCLUDES := $(INCLUDES) -I $(WLANFW_DIR)/import/REV74.hw/include/alljoyn
INCLUDES := $(INCLUDES) -I $(WLANFW_DIR)/import/REV74.hw/include/alljoyn/external/sha2
INCLUDES := $(INCLUDES) -I $(WLANFW_DIR)/import/REV74.hw/include/alljoyn_kf
INCLUDES := $(INCLUDES) -I $(WLANFW_DIR)/import/REV74.hw/include/alljoyn_services

DEFINES += -DCONFIG_SERVICE -DONBOARDING_SERVICE -DCONTROLPANEL_SERVICE -DNOTIFICATION_SERVICE_PRODUCER -DEVENTS_AND_ACTIONS
DEFINES += -DNDEBUG

PDIR := ../$(PDIR)
sinclude $(PDIR)Makefile
