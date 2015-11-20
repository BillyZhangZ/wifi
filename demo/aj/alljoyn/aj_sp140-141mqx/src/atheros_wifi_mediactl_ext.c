//------------------------------------------------------------------------------
// Copyright (c) 2011 Qualcomm Atheros, Inc.
// All Rights Reserved.
// Qualcomm Atheros Confidential and Proprietary.
// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is
// hereby granted, provided that the above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
// USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//------------------------------------------------------------------------------
//==============================================================================
// Author(s): ="Atheros"
//==============================================================================
#include <a_config.h>
#include <a_types.h>
#include <a_osapi.h>
#include <driver_cxt.h>
#include <common_api.h>
#include <custom_api.h>
#include <targaddrs.h>
#include "mqx.h"
#include "bsp.h"
#include "enet.h"
#include "enetprv.h"

#include "atheros_wifi.h"
#include "enet_wifi.h"
#include "atheros_wifi_api.h"
#include "atheros_wifi_internal.h"
#include "qca_includes.h"


A_BOOL reg_query_bool = A_FALSE;
pointer pQuery;

static A_STATUS driver_thread_operation(A_VOID *pCxt)
{
	A_DRIVER_CONTEXT *pDCxt = GET_DRIVER_COMMON(pCxt);    	
 union{    
    	A_UINT32 param;    	
    }stackU;      
    
#define PARAM (stackU.param)                	
#define PTR_REG_MOD		((ATH_REGQUERY*)(pQuery))	
		switch(PTR_REG_MOD->operation)
		{
		case ATH_REG_OP_READ: // read			
			Driver_ReadRegDiag(pCxt, &PTR_REG_MOD->address, &(PTR_REG_MOD->value));									
			PTR_REG_MOD->value = A_CPU2LE32((PTR_REG_MOD->value));
			break;
		case ATH_REG_OP_WRITE: //write-
			PTR_REG_MOD->value = A_CPU2LE32((PTR_REG_MOD->value));
			Driver_WriteRegDiag(pCxt, &PTR_REG_MOD->address, &(PTR_REG_MOD->value));																	
			break;
		case ATH_REG_OP_RMW: //read-mod-write
			if(A_OK != Driver_ReadRegDiag(pCxt, &PTR_REG_MOD->address, &PARAM)){
				break;
			}
			
			PARAM = A_CPU2LE32(PARAM);
			PARAM &= ~PTR_REG_MOD->mask;
			PARAM |= PTR_REG_MOD->value;
			PARAM = A_CPU2LE32(PARAM);
			
			Driver_WriteRegDiag(pCxt, &PTR_REG_MOD->address, &PARAM);						
			break;
		}
	pDCxt->asynchRequest = NULL;
	reg_query_bool = A_TRUE;
	CUSTOM_DRIVER_WAKE_USER(pCxt);
#undef PTR_REG_MOD	
#undef PARAM

	return A_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ath_ioctl_handler
*  PARAMS         : 
*                   enet_ptr-> ptr to enet context
*                   inout_param -> input/output data for command.
*  Returned Value : ENET_OK or error code
*  Comments       :
*        IOCTL implementation of Atheros Wifi device.
*
*END*-----------------------------------------------------------------*/

static uint_32 ath_ioctl_handler_ext(A_VOID* enet_ptr, ATH_IOCTL_PARAM_STRUCT_PTR param_ptr)
{
	A_VOID* pCxt;
    A_DRIVER_CONTEXT *pDCxt;        
	uint_32 error = ENET_OK;
	ATH_REGQUERY regQuery;
	uint_32 *ptr,i;
   		
	if((pCxt = ((ENET_CONTEXT_STRUCT_PTR)enet_ptr)->MAC_CONTEXT_PTR) == NULL)
	{
	   	return ENET_ERROR;
	}  
	pDCxt = GET_DRIVER_COMMON(pCxt);    	
	
	switch(param_ptr->cmd_id)
	{			
		case ATH_REG_QUERY:
			/* set the operation to be executed by the driver thread */
			pQuery = param_ptr->data;
			reg_query_bool = A_FALSE;
			if(pDCxt->asynchRequest != NULL){
				break;
			}
			pDCxt->asynchRequest = driver_thread_operation;
			/* wake up driver thread */
			CUSTOM_DRIVER_WAKE_DRIVER(pCxt);
			/* wait for driver thread completion */
			Custom_Driver_WaitForCondition(pCxt, &(reg_query_bool), A_TRUE, 5000);  
		
			break;	
		case ATH_MEM_QUERY:
			// read the memory location for stat storage
			regQuery.address = TARG_VTOP(HOST_INTEREST_ITEM_ADDRESS(hi_flash_is_present));
			regQuery.operation = ATH_REG_OP_READ;
			pQuery = &regQuery;
			reg_query_bool = A_FALSE;
			
			if(pDCxt->asynchRequest != NULL){
				break;
			}
			
			pDCxt->asynchRequest = driver_thread_operation;			
			CUSTOM_DRIVER_WAKE_DRIVER(pCxt);			
			Custom_Driver_WaitForCondition(pCxt, &(reg_query_bool), A_TRUE, 5000);
			
			if(reg_query_bool == A_FALSE){
				break;
			}
			//capture the value in regQuery.address
			regQuery.address = TARG_VTOP(regQuery.value);
			ptr = (uint_32*)param_ptr->data;
			
			for(i=0 ; i<5 ; i++){
				reg_query_bool = A_FALSE;
			
				if(pDCxt->asynchRequest != NULL){
					break;
				}
				
				pDCxt->asynchRequest = driver_thread_operation;
				CUSTOM_DRIVER_WAKE_DRIVER(pCxt);
				Custom_Driver_WaitForCondition(pCxt, &(reg_query_bool), A_TRUE, 5000);
			
				if(reg_query_bool == A_FALSE){
					break;
				}
				/* CAUTION: this next line assumes that the stats are stored in 
				 * the same order that they appear in the ATH_MEMQUERY structure. */
				ptr[i] = regQuery.value;
				regQuery.address+=4;
			}
			/* for allocram remaining we query address allocram_remaining_bytes.*/
			regQuery.address = TARG_VTOP(0x541f2c);
			reg_query_bool = A_FALSE;
		
			if(pDCxt->asynchRequest != NULL){
				break;
			}
			
			pDCxt->asynchRequest = driver_thread_operation;
			CUSTOM_DRIVER_WAKE_DRIVER(pCxt);
			Custom_Driver_WaitForCondition(pCxt, &(reg_query_bool), A_TRUE, 5000);
		
			if(reg_query_bool == A_FALSE){
				break;
			}
			/* CAUTION: this next line assumes that the stats are stored in 
			 * the same order that they appear in the ATH_MEMQUERY structure. */
			ptr[5] = regQuery.value;							
			
			break;	
		default:
			error = ENET_ERROR;
			break;
	}
	
	
	return  error;
}


#ifdef BSP_LED1
#define LED_1 BSP_LED1
#endif
#if 0
#define LED_2 BSP_LED2
#endif


#ifdef KINETIS_PLATFORM

#define LED_ON  GPIO_IOCTL_WRITE_LOG0
#define LED_OFF GPIO_IOCTL_WRITE_LOG1

#define LED_STATE_ERROR (0x00000001)/* LED 3 will only light in the event of an error (because its red and red means bad) */
#define LED_STATE_PROGRESS (0x00000002) /* LED 4 will only light in the event the progress is occurring (because its yellow and yellow means indeterminate). */
#define LED_STATE_INFO_1 (0x00000008) /* LED 1 will light to indicate different information under different conditions (because its green and we like green). */
#define LED_STATE_INFO_2 (0x00000004) /* LED 2 will light to indicate different information under different conditions (because its green and we like green). */

#else
 
#define LED_ON  GPIO_IOCTL_WRITE_LOG1
#define LED_OFF GPIO_IOCTL_WRITE_LOG0

#define LED_STATE_ERROR (0x00000004)/* LED 3 will only light in the event of an error (because its red and red means bad) */
#define LED_STATE_PROGRESS (0x00000008) /* LED 4 will only light in the event the progress is occurring (because its yellow and yellow means indeterminate). */
#define LED_STATE_INFO_1 (0x00000001) /* LED 1 will light to indicate different information under different conditions (because its green and we like green). */
#define LED_STATE_INFO_2 (0x00000002) /* LED 2 will light to indicate different information under different conditions (because its green and we like green). */

#endif

static MQX_FILE_PTR output_port=NULL;
extern boolean InitializeIO(void);
boolean InitializeIO(void) 
{
	
#ifdef BSP_LED1
    const uint_32 output_set[] = {
        LED_1 | GPIO_PIN_STATUS_0,
        GPIO_LIST_END
    };

    /* Open and set port TC as output to drive LEDs (LED10 - LED13) */
    output_port = fopen("gpio:write", (char_ptr) &output_set);

    if (output_port)
        ioctl(output_port, GPIO_IOCTL_WRITE_LOG0, NULL);    

   return (output_port !=NULL);
#else
   return 1;
#endif 
}

void LED_SetOutput(uint_32 signal);
void LED_SetOutput(uint_32 signal) 
{
#if  0 
	E.Y. hide to disable the LED op
    static const uint_32 led1[] = {
        LED_1,
        GPIO_LIST_END
    };
    static const uint_32 led2[] = {
        LED_2,
        GPIO_LIST_END
    };
    static const uint_32 led3[] = {
        LED_3,
        GPIO_LIST_END
    };
    static const uint_32 led4[] = {
        LED_4,
        GPIO_LIST_END
    };
       

    if (output_port) { 
        ioctl(output_port, (signal & 0x00000001) ? LED_ON : LED_OFF, (pointer) &led1);
        ioctl(output_port, (signal & 0x00000002) ? LED_ON : LED_OFF, (pointer) &led2);
        ioctl(output_port, (signal & 0x00000004) ? LED_ON : LED_OFF, (pointer) &led3);
        ioctl(output_port, (signal & 0x00000008) ? LED_ON : LED_OFF, (pointer) &led4);    
    }   
#endif     
}


#if 1
void atheros_driver_setup(void)
{	
	ath_custom_mediactl.ath_ioctl_handler_ext = ath_ioctl_handler_ext;
	ath_custom_init.Boot_Profile = LED_SetOutput;
	InitializeIO();
}    
#else
void atheros_driver_setup(void)
{
//	ath_custom_mediactl.ath_ioctl_handler_ext = ath_ioctl_handler_ext;
}
#endif 



/* EOF */
