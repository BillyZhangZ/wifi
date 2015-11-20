#-----------------------------------------------------------
# libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/debug/bsp/bsp.lib
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/debug/bsp/bsp.lib(vectors.o)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/debug/psp/psp.lib
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/debug/psp/psp.lib(linker_symbols.o)
endif
ifeq ($(CONFIG),release)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/release/bsp/bsp.lib
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/release/bsp/bsp.lib(vectors.o)
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/release/psp/psp.lib
LIBRARIES += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/release/psp/psp.lib(linker_symbols.o)
endif


#-----------------------------------------------------------
# runtime libraries
#-----------------------------------------------------------


#-----------------------------------------------------------
# runtime library paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_PATHS += $(TOOLCHAIN_ROOTDIR)/ARM/ARMCC/lib/
endif
ifeq ($(CONFIG),release)
RT_PATHS += $(TOOLCHAIN_ROOTDIR)/ARM/ARMCC/lib/
endif


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/debug/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/debug/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/debug
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/debug/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/debug/psp
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/release/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/release/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/release
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/release/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.uv4/release/psp
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





