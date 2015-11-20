/*HEADER**********************************************************************
*
* Copyright 2012 Freescale Semiconductor, Inc.
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
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains definitions private to the SPI driver.
*
*
*END************************************************************************/

#ifndef __spi_em9301_prv_h__
#define __spi_em9301_prv_h__

#include <bsp.h>
#include "spi.h"
#include "74hc595.h"



/*--------------------------------------------------------------------------*/
/*
**                    DATATYPE DECLARATIONS
*/
#define BSP_EM9301_MOSI_PIN						(GPIO_PORT_D | GPIO_PIN6)
#define BSP_EM9301_IRQ_PIN						(GPIO_PORT_C | GPIO_PIN11)
#define BSP_EM9301_IRQ_MUX_IRQ						 LWGPIO_MUX_C11_GPIO 


#endif
