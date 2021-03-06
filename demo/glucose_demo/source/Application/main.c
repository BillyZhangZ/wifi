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
*   This file contains the source for a simple example of an
*   application that writes and reads the SPI memory using the SPI driver.
*   It's already configured for onboard SPI flash where available.
*
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>

#include "BleApp.h"
#include "glucose_app.h"


extern void setGlucoseValue(void);
extern void main_task (uint32_t);

const TASK_TEMPLATE_STRUCT  MQX_template_list[] =
{
    /* Task Index,   Function,   Stack,  Priority,   Name,   Attributes,          Param, Time Slice */
    { 10L,          main_task,  3000L,  8L,         "Main", MQX_AUTO_START_TASK, 0,     0  },
    { 0 }
};

/*TASK*-------------------------------------------------------------------
*
* Task Name : main_task
* Comments  :
*
*END*----------------------------------------------------------------------*/
void main_task
   (
      uint32_t dummy
   )
{
	
	int i = 0;
	glucose_init();
	BLEAPP_Init();
	for(;;)
	{
		while(BLEAPP_Scheduling());
		if(i++ == 5000*20)
		{
			i = 0;
			setGlucoseValue();

		}

		
	}	
		
}
