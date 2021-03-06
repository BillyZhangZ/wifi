#-----------------------------------------------------------
# libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/shell/shell.a
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/usb/usb.a
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/msi/msi.a
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/mux/mux.a
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/sensors/sensors.a
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/rtcs/rtcs.a
endif
ifeq ($(CONFIG),release)
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/bsp/bsp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/psp/psp.a
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/shell/shell.a
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/usb/usb.a
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/msi/msi.a
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/sensors/sensors.a
LIBRARIES += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/rtcs/rtcs.a
endif


#-----------------------------------------------------------
# runtime libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/$(GCCV)/armv7e-m/softfp/libgcc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libsupc++.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libm.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libnosys.a
endif
ifeq ($(CONFIG),release)
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/$(GCCV)/armv7e-m/softfp/libgcc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libsupc++.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libm.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libnosys.a
endif


#-----------------------------------------------------------
# runtime library paths
#-----------------------------------------------------------


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/psp
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/shell
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/bluetooth
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/usb
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/msi
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/debug/mux
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/bsp/Generated_Code
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/bsp/Sources
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/bsp
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/psp
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/shell
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/bluetooth
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/usb
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/msi
INCLUDE += $(MQX_ROOTDIR)/lib/k22fsh.gcc_arm/release/mux
endif

INCLUDE += $(MQX_ROOTDIR)/sensors/source

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





