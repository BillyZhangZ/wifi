/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other restrictions.
*****************************************************************************
*
* Comments:
*
*   User configuration for MQX components
*
*
*END************************************************************************/

#ifndef __user_config_h__
#define __user_config_h__

/* mandatory CPU identification */
#define MQX_CPU                 PSP_CPU_MK22FN512

/*
** BSP settings - for defaults see mqx\source\bsp\<board_name>\<board_name>.h
*/
/* MGCT: <generated_code> */

/* BSP settings - for defaults see mqx\source\bsp\<board_name>\<board_name>.h */
#define BSPCFG_ENABLE_I2C0                        1
#define BSPCFG_ENABLE_II2C0                       0

#define BSPCFG_ENABLE_I2C1                        1
#define BSPCFG_ENABLE_II2C1                       0

#define BSPCFG_ENABLE_I2C2                        0
#define BSPCFG_ENABLE_II2C2                       0
#define BSPCFG_ENABLE_GPIODEV                     1

#define BSPCFG_ENABLE_RTCDEV                      0

/* pccard shares CS signal with MRAM - opening pccard in user application disables MRAM */
#define BSPCFG_ENABLE_PCFLASH                     0

#define BSPCFG_ENABLE_SPI0                        1                       /* QCA 4002 */
#define BSPCFG_ENABLE_ISPI0                       0
#define BSPCFG_ENABLE_SPI1                        1                       /* EEPROM ? External SPI ? */
#define BSPCFG_ENABLE_SPI2                        0

#define BSPCFG_ENABLE_ADC0                        1
#define BSPCFG_ENABLE_ADC1                        0
#define BSPCFG_ENABLE_LWADC0                      0
#define BSPCFG_ENABLE_LWADC1                      0
#define BSPCFG_ENABLE_FLASHX                      1
#define BSPCFG_ENABLE_SAI                         1
#define BSPCFG_ENABLE_IODEBUG                     0

/*need to enable the BSPCFG_ENABLE_SPI_STATS marco to know the SPI1 status*/
#define BSPCFG_ENABLE_SPI_STATS					  1

/* BSP settings - for defaults see mqx\source\bsp\<board_name>\<board_name>.h */
#define BSPCFG_ENABLE_TTYA                        1
#define BSPCFG_ENABLE_ITTYA                       0
#define BSPCFG_ENABLE_DTTYA                       0
#define BSPCFG_DB142_IOT_MODE_SUPPORT 		  1
#define BSPCFG_ENABLE_ATHEROS_WIFI                1
#define BSPCFG_ENABLE_TTYB                        0
#define BSPCFG_ENABLE_ITTYB                       0
#define BSPCFG_ENABLE_DTTYB                       0
#define BSPCFG_ENABLE_MTTYB                       1

#define BSPCFG_ENABLE_TTYC                        0
#define BSPCFG_ENABLE_ITTYC                       0

/* board-specific MQX settings - see for defaults mqx\source\include\mqx_cnfg.h */
#define MQX_USE_IDLE_TASK                         1

#define MQX_ENABLE_LOW_POWER                      0
/* USB can not work if MQX_ENABLE_HSRUN equal to 0 */
#define MQX_ENABLE_HSRUN                          1
#define MQX_USE_TIMER                             1
#define MQXCFG_ENABLE_FP                          1
#define MQX_INCLUDE_FLOATING_POINT_IO             1

/* board-specific Shell settings */
#define RTCSCFG_ENABLE_TCP                        1
#define RTCSCFG_ENABLE_ICMP                       1
#define RTCSCFG_ENABLE_IGMP                       1
#define RTCSCFG_ENABLE_LWDNS                      1

#define SHELLCFG_USES_MFS                         1
#define SHELLCFG_USES_RTCS                        1
/* MGCT: </generated_code> */

#define BSPCFG_SAI_CLK_SOURCE_FROM_PLL_OUT        0
#define BSPCFG_USB_CLK_FROM_IRC48M                0

/*
** include common settings
*/
#define RTCSCFG_ENABLE_UDP   1
/* use the rest of defaults from small-RAM-device profile */
#include "small_ram_config.h"

/* and enable verification checks in kernel */
#include "verif_enabled_config.h"

#endif /* __user_config_h__ */

