#-----------------------------------------------------------
# libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
LIBRARIES += $(USB_ROOTDIR)/output/twrk22f120m.gcc_arm/debug/usbotg/mqx/usbotg.a
LIBRARIES += $(USB_ROOTDIR)/../lib/twrk22f120m.gcc_arm/debug/bsp/bsp.a
LIBRARIES += $(USB_ROOTDIR)/../lib/twrk22f120m.gcc_arm/debug/psp/psp.a
endif
ifeq ($(CONFIG),release)
LIBRARIES += $(USB_ROOTDIR)/output/twrk22f120m.gcc_arm/release/usbotg/mqx/usbotg.a
LIBRARIES += $(USB_ROOTDIR)/../lib/twrk22f120m.gcc_arm/release/bsp/bsp.a
LIBRARIES += $(USB_ROOTDIR)/../lib/twrk22f120m.gcc_arm/release/psp/psp.a
endif


#-----------------------------------------------------------
# runtime libraries
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.7.4/armv7e-m/softfp/libgcc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libc.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libsupc++.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libm.a
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/lib/armv7e-m/softfp/libnosys.a
endif
ifeq ($(CONFIG),release)
RT_LIBRARIES += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.7.4/armv7e-m/softfp/libgcc.a
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
INCLUDE += $(USB_ROOTDIR)/output/twrk22f120m.gcc_arm/debug/usbotg/mqx
INCLUDE += $(USB_ROOTDIR)/../lib/twrk22f120m.gcc_arm/debug/bsp/Generated_Code
INCLUDE += $(USB_ROOTDIR)/../lib/twrk22f120m.gcc_arm/debug/bsp/Sources
INCLUDE += $(USB_ROOTDIR)/../lib/twrk22f120m.gcc_arm/debug
INCLUDE += $(USB_ROOTDIR)/../lib/twrk22f120m.gcc_arm/debug/bsp
INCLUDE += $(USB_ROOTDIR)/../lib/twrk22f120m.gcc_arm/debug/psp
endif
ifeq ($(CONFIG),release)
INCLUDE += $(USB_ROOTDIR)/output/twrk22f120m.gcc_arm/release/usbotg/mqx
INCLUDE += $(USB_ROOTDIR)/../lib/twrk22f120m.gcc_arm/release/bsp/Generated_Code
INCLUDE += $(USB_ROOTDIR)/../lib/twrk22f120m.gcc_arm/release/bsp/Sources
INCLUDE += $(USB_ROOTDIR)/../lib/twrk22f120m.gcc_arm/release
INCLUDE += $(USB_ROOTDIR)/../lib/twrk22f120m.gcc_arm/release/bsp
INCLUDE += $(USB_ROOTDIR)/../lib/twrk22f120m.gcc_arm/release/psp
endif


#-----------------------------------------------------------
# runtime search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.7.4/include
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.7.4/include-fixed
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/include
endif
ifeq ($(CONFIG),release)
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.7.4/include
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/lib/gcc/arm-none-eabi/4.7.4/include-fixed
RT_INCLUDE += $(TOOLCHAIN_ROOTDIR)/arm-none-eabi/include
endif





#-----------------------------------------------------------
# 'debug' configuration settings
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
CC_DEFINE += USBCFG_OTG=1 
endif


#-----------------------------------------------------------
# 'release' configuration settings
#-----------------------------------------------------------
ifeq ($(CONFIG),release)
CC_DEFINE += USBCFG_OTG=1 
endif


