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




