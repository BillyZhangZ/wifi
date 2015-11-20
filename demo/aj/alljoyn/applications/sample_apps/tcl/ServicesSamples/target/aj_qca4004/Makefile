#/******************************************************************************
# * Copyright 2012-2013, Qualcomm Innovation Center, Inc.
# *
# *    All rights reserved.
# *    This file is licensed under the 3-clause BSD license in the NOTICE.txt
# *    file for this project. A copy of the 3-clause BSD license is found at:
# *
# *        http://opensource.org/licenses/BSD-3-Clause.
# *
# *    Unless required by applicable law or agreed to in writing, software
# *    distributed under the license is distributed on an "AS IS" BASIS,
# *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# *    See the license for the specific language governing permissions and
# *    limitations under the license.
# ******************************************************************************/

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

GEN_LIBS = liballjoyn_services.a
CSRCS = $(wildcard ../../../../../services/base_tcl/config/src/*.c)
CSRCS += $(wildcard ../../../../../services/base_tcl/controlpanel/src/CPSControllee/*.c)
CSRCS += $(wildcard ../../../../../services/base_tcl/controlpanel/src/CPSControllee/Common/*.c)
CSRCS += $(wildcard ../../../../../services/base_tcl/controlpanel/src/CPSControllee/Widgets/*.c)
CSRCS += $(wildcard ../../../../../services/base_tcl/notification/src/NotificationCommon/*.c)
CSRCS += $(wildcard ../../../../../services/base_tcl/notification/src/NotificationProducer/*.c)
CSRCS += $(wildcard ../../../../../services/base_tcl/onboarding/src/*.c)
CSRCS += $(wildcard ../../../../../services/base_tcl/services_common/src/*.c)

endif

#############################################################
# Configuration i.e. compile options etc.
# Target specific stuff (defines etc.) goes in here!
# Generally values applying to a tree are captured in the
#   makefile at its root level - these are then overridden
#   for a subtree within the makefile rooted therein
#
DEFINES += -DCONFIG_SERVICE -DONBOARDING_SERVICE -DCONTROLPANEL_SERVICE -DNOTIFICATION_SERVICE_PRODUCER -DEVENTS_AND_ACTIONS
DEFINES += -DNDEBUG

SRCS    = $(CSRCS) $(SSRCS)
COBJS   = $(CSRCS:.c=.o)
SOBJS   = $(SSRCS:.S=.o)
OBJS    = $(COBJS) $(SOBJS)
DOBJS   = $(DSRCS:.c=.o)


CCFLAGS := $(CCFLAGS) 

CCFLAGS := $(CCFLAGS) -g3
CCFLAGS := $(CCFLAGS) -Os
ifeq ($(AR6002_REV), 7) # {
CCFLAGS := $(CCFLAGS) -DATH_TARGET -DAR6K -DAR6002 -DAR6002_REV7
ifeq ($(AR6002_REV7_VER), 4) # {
CCFLAGS := $(CCFLAGS) -DAR6002_REV74
endif # AR6002_REV7_VER4 }
endif # AR6002_REV7 }

CCFLAGS := $(CCFLAGS) -include fwconfig_AR6006.h -Wno-return-type
CCFLAGS := $(CCFLAGS) -fno-inline-functions -nostdlib -mlongcalls -ffunction-sections
CCFLAGS := $(CCFLAGS) $(DEFINES)

#############################################################
# Configuration i.e. compile options etc.
# Target specific stuff (defines etc.) goes in here!
# Generally values applying to a tree are captured in the
#   makefile at its root level - these are then overridden
#   for a subtree within the makefile rooted therein
#

INCLUDES := $(INCLUDES) \
			-I ../../../../../core/ajtcl/inc \
			-I ../../../../../core/ajtcl/malloc \
			-I ../../../../../core/ajtcl/external/sha2 \
			-I ../../../../../aj_qca4004/inc \
			-I$(WLANCMN_DIR)/include \
			-I$(WLANCMN_DIR)/include/$(TARGET) \
			-I$(WLANCMN_DIR)/include/$(HW_HDRS) \
			-I$(WLANFW_DIR)/import/$(BUILD_VARIANT)/include \
			-I $(WLANFW_DIR)/include 

INCLUDES := $(INCLUDES) -I . -I $(PDIR)include 
INCLUDES := $(INCLUDES) -I $(WLANCMN_DIR)/include/cust 
INCLUDES := $(INCLUDES) -I $(WLANCMN_DIR)/include/qcom 
INCLUDES := $(INCLUDES) -I $(WLANFW_DIR)/import/rev7/include/ 


INCLUDES := $(INCLUDES) -I../../../../../services/base_tcl/config/inc
INCLUDES := $(INCLUDES) -I../../../../../services/base_tcl/controlpanel/inc
INCLUDES := $(INCLUDES) -I../../../../../services/base_tcl/notification/inc
INCLUDES := $(INCLUDES) -I../../../../../services/base_tcl/onboarding/inc
INCLUDES := $(INCLUDES) -I../../../../../services/base_tcl/services_common/inc


############################################################
# Create a subdirectory for AllJoyn objects 
#

all: liballjoyn_services.a

liballjoyn_services.a: $(OBJS)
	xt-ar $(ARFLAGS) $@ $(OBJS)

clean: 
	rm -f ../../../../../services/base_tcl/config/src/*.o
	rm -f ../../../../../services/base_tcl/controlpanel/src/CPSControllee/*.o
	rm -f ../../../../../services/base_tcl/controlpanel/src/CPSControllee/Common/*.o
	rm -f ../../../../../services/base_tcl/controlpanel/src/CPSControllee/Widgets/*.o
	rm -f ../../../../../services/base_tcl/notification/src/NotificationCommon/*.o
	rm -f ../../../../../services/base_tcl/notification/src/NotificationProducer/*.o
	rm -f ../../../../../services/base_tcl/onboarding/src/*.o
	rm -f ../../../../../services/base_tcl/services_common/src/*.o
	rm -f liballjoyn_services.a

clobber: clean

%.o: %.c
	xt-xcc $(CCFLAGS) $(INCLUDES) -c $< -o $@ 


