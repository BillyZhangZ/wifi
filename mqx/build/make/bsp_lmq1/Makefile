#----------------------------------------------------------
# setup variables
#----------------------------------------------------------
MQX_ROOTDIR             = $(abspath ../../../..)
TYPE                    = library
NAME                    = bsp
BOARD                   = lmq1
LIBRARY_ROOTDIR         = $(MQX_ROOTDIR)/lib/$(BOARD).$(TOOL)
LIBRARY_DIR             = $(LIBRARY_ROOTDIR)/$(CONFIG)/bsp
LIBRARY_FILE            = $(LIBRARY_DIR)/bsp.$(LIBRARY_EXT)
POSTBUILD_CMD           = $(if $(findstring $(HOSTENV),WINDOWS),$(MQX_ROOTDIR)/mqx/build/bat/bsp_lmq1.bat $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL),$(MQX_ROOTDIR)/mqx/build/bat/bsp_lmq1.sh $(MQX_ROOTDIR) $(LIBRARY_DIR) $(TOOL))


#-----------------------------------------------------------
# load tool settings
#-----------------------------------------------------------
-include tools/$(TOOL).mak


#-----------------------------------------------------------
# search paths
#-----------------------------------------------------------
ifeq ($(CONFIG),debug)
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/cm
INCLUDE += $(MQX_ROOTDIR)/config/lmq1
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/dma
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio/kgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hwtimer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1
INCLUDE += $(MQX_ROOTDIR)/config/common
INCLUDE += $(MQX_ROOTDIR)/mqx/source/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/debug
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_mem
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_null
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pipe
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lpm
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pccard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcflash
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rnga
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rtc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/serial
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/tfs
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/timer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet                                      
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/phy                                         
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi                                
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/include                        
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/include/AR6002                 
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/include             
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/stack_common        
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/wmi                 
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/include             
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/stack_custom    
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/cpu
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/core/M4
INCLUDE += $(MQX_ROOTDIR)/mqx/source/tad
endif
ifeq ($(CONFIG),release)
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/adc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/cm
INCLUDE += $(MQX_ROOTDIR)/config/lmq1
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/dma
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/flashx
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio/kgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/gpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/hwtimer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/i2c/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1
INCLUDE += $(MQX_ROOTDIR)/config/common
INCLUDE += $(MQX_ROOTDIR)/mqx/source/include
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/debug
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_mem
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/io_null
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pipe
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lpm
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwadc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/lwgpio
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pccard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/pcflash
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rnga
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/rtc
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sai/int
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/sdcard
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/serial
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/spi
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/tfs
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/timer
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/usb
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/macnet                                      
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/phy                                         
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi                                
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/include                        
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/include/AR6002                 
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/include             
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/stack_common        
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/wmi                 
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/include             
INCLUDE += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/stack_custom    
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/cpu
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m
INCLUDE += $(MQX_ROOTDIR)/mqx/source/psp/cortex_m/core/M4
INCLUDE += $(MQX_ROOTDIR)/mqx/source/tad
endif



#-----------------------------------------------------------
# 'adc' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc/adc_mk22.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc/adc_kadc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/adc/kadc/adt_kpdb.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/adc/adc.c

#-----------------------------------------------------------
# 'cm' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/cm/cm.c

#-----------------------------------------------------------
# 'dma' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/dma/edma_kinetis.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/dma/dma.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/dma/edma.c

#-----------------------------------------------------------
# 'flashx' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale/flash_mk22.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/flashx/freescale/flash_ftfa.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/flashx/flashx.c

#-----------------------------------------------------------
# 'gpio' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/gpio/kgpio/io_gpio_kgpio.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/gpio/io_gpio.c

#-----------------------------------------------------------
# 'hwtimer' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hwtimer/hwtimer_systick.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hwtimer/hwtimer_lpt.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hwtimer/hwtimer_kinetis.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hwtimer/hwtimer_pit.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/hwtimer/hwtimer.c

#-----------------------------------------------------------
# 'i2c' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/i2c_mk22.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled/i2c_pol_ki2c.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/int/i2c_int_ki2c.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/polled/i2c_pol.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/i2c/int/i2c_int.c

#-----------------------------------------------------------
# 'init' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/bsp_cm.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_adc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_lwadc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_flashx.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_gpio.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_hw.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_i2c.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_iodebug.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_lpm.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_sai.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_sci.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_spi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_usb.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_dma.c
#SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_sdcard.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_enet.c

#-----------------------------------------------------------
# 'init-common' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/init_bsp.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/mqx_init.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/mqx_main.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/bsp/lmq1/vectors.c

#-----------------------------------------------------------
# 'iodebug' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/debug/iodebug.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/debug/iodebug_asm.S

#-----------------------------------------------------------
# 'iomem' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/io_mem/io_mem.c

#-----------------------------------------------------------
# 'ionull' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/io_null/io_null.c

#-----------------------------------------------------------
# 'iopipe' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pipe/io_pipe.c

#-----------------------------------------------------------
# 'lpm' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lpm/lpm_smc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lpm/lpm.c

#-----------------------------------------------------------
# 'lwadc' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lwadc/lwadc_k22.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lwadc/lwadc_kadc.c

#-----------------------------------------------------------
# 'lwgpio' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lwgpio/lwgpio_kgpio.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/lwgpio/lwgpio.c

#-----------------------------------------------------------
# 'pcb' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcb/io_pcb2.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcb/io_pcb.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcb/iop_inst.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcb/mqxa/pcb_mqxa.c

#-----------------------------------------------------------
# 'pccard' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pccard/pccardflexbus.c

#-----------------------------------------------------------
# 'pcflash' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/pcflash/apcflash.c

#-----------------------------------------------------------
# 'rnga' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/rnga/krnga.c

#-----------------------------------------------------------
# 'rtc' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/rtc/krtc.c

#-----------------------------------------------------------
# 'sai' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sai/sai_mk22.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sai/int/sai_int_ksai.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sai/dma/sai_dma_ksai.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sai/int/sai_int.c

#-----------------------------------------------------------
# 'sdcard' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard_spi/sdcard_spi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/sdcard/sdcard.c

#-----------------------------------------------------------
# 'serial' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/serl_mk22.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/int/serl_int_kuart.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/polled/serl_pol_kuart.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/dma/serl_dma_kuart.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/io_sinit.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/polled/serl_pol.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/serial/int/serl_int.c

#-----------------------------------------------------------
# 'spi' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/spi/spi_mk22_dspi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/spi/spi_dspi_dma.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/spi/spi_dspi_common.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/spi/spi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/spi/spi_dspi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/spi/spi_dspi_common.c

#-----------------------------------------------------------
# 'tfs' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/tfs/tfs.c

#-----------------------------------------------------------
# 'timer' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/timer/timer_qpit.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/timer/timer_kinetis.c

#-----------------------------------------------------------
# 'usb' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/usb/usb_mk22.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/usb/usb_mk22_device.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/usb/usb_install.c

#-----------------------------------------------------------
# 'enet qca4002/4004' sources
#-----------------------------------------------------------
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enaddr.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enclose.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enerr.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/eninit.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enjoin.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enleave.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enmediactl.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enopen.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enrecv.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/ensend.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enstat.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enstop.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/enutil.c

SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/phy/atheros_phy.c

SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/api_interface/api_init.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/api_interface/api_ioctl.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/api_interface/api_txrx.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/api_interface/api_wmi_rx.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/bmi/bmi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/driver/driver_diag.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/driver/driver_init.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/driver/driver_main.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/driver/driver_netbuf.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/driver/driver_txrx.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/hcd/spi_hcd.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/htc/htc.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/hw_interface/hw_api.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/reorder/rcv_aggr.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/stack_common/api_stack_offload.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/storerecall/storerecall.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/util/util.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/common_src/wmi/wmi.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/api_interface/cust_api_init.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/api_interface/cust_api_ioctl.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/api_interface/cust_api_txrx.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/api_interface/cust_api_wmi_rx.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/driver/cust_driver_main.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/driver/cust_driver_netbuf.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/hw_interface/cust_hw_api.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/stack_custom/cust_api_stack_offload.c
SOURCES += $(MQX_ROOTDIR)/mqx/source/io/enet/atheros_wifi/custom_src/stack_custom/cust_api_stack_txrx.c

#----------------------------------------------------------
# load global settings
#----------------------------------------------------------
include $(MQX_ROOTDIR)/build/common/make/global.mak
-include $(MQX_ROOTDIR)/build/$(BOARD)/make/tools/$(TOOL).mak
include $(MQX_ROOTDIR)/build/common/make/verify.mak
include $(MQX_ROOTDIR)/build/common/make/lib-process.mak

