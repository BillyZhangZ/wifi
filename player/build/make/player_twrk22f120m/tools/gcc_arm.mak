#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/compiler/gcc_arm
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.gcc_arm/debug/msi
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.gcc_arm/debug/audio
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.gcc_arm/debug/mfs
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.gcc_arm/debug/shell
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/compiler/gcc_arm
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.gcc_arm/release/msi
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.gcc_arm/release/audio
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.gcc_arm/release/mfs
INCLUDE += $(MQX_ROOTDIR)/lib/twrk22f120m.gcc_arm/release/shell
endif


#-----------------------------------------------------------
# runtime search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/$(GCCV)/include
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/$(GCCV)/include-fixed
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/include
endif
ifeq ($(CONFIG),release)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/$(GCCV)/include
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/$(GCCV)/include-fixed
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/include
endif




