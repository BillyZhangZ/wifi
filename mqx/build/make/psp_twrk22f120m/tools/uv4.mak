#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/compiler/rv_uv4
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/compiler/rv_uv4
endif


#-----------------------------------------------------------
# runtime search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/ARM/INC/Freescale/K22
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/ARM/RV31/INC
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/ARM/CMSIS/Include
endif
ifeq ($(CONFIG),release)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/ARM/INC/Freescale/K22
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/ARM/RV31/INC
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/ARM/CMSIS/Include
endif


#-----------------------------------------------------------
# uv4 sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/compiler/rv_uv4/comp.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/compiler/rv_uv4/linker_symbols.S


#-----------------------------------------------------------
# 'debug' configuration settings
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
CC_DEFINE += MQX_DISABLE_CONFIG_CHECK=1 
CX_DEFINE += MQX_DISABLE_CONFIG_CHECK=1 
endif


#-----------------------------------------------------------
# 'release' configuration settings
#-----------------------------------------------------------
ifeq ($(CONFIG),release)
CC_DEFINE += MQX_DISABLE_CONFIG_CHECK=1 
CX_DEFINE += MQX_DISABLE_CONFIG_CHECK=1 
endif


