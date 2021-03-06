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
#include <a_osapi.h>
#include <a_types.h>
#include <driver_cxt.h>
#include <common_api.h>
#include <custom_api.h>
#include "mqx.h"
#include "bsp.h"
#include "enet.h"
#include "enetprv.h"

A_VOID *p_Global_Cxt = NULL;

/*The following pointers store the custom driver context used by QAPIs*/
A_VOID *p_Driver_Cxt[2];

#if MQX_VERSION == (402)
static uint_32 Custom_Api_Readmii(struct enet_context_struct * p_ctxt, uint_32 val, uint_32 * p_val, uint_32 val2);
static uint_32 Custom_Api_Writemii(struct enet_context_struct * p_ctxt,  uint_32 val1, uint_32 val2 , uint_32 val3);
#else
static bool Custom_Api_Readmii(struct enet_context_struct * p_ctxt, uint_32 val, uint_32 * p_val, uint_32 val2);
static bool Custom_Api_Writemii(struct enet_context_struct * p_ctxt,  uint_32 val1, uint_32 val2 , uint_32 val3);
#endif
static uint_32 Custom_Api_Join (struct enet_context_struct * p_ctxt, struct enet_mcb_struct * p_mcb);
static uint_32 Custom_Api_Rejoin(struct enet_context_struct * p_ctxt);
static uint_32 Custom_Api_Initialize(ENET_CONTEXT_STRUCT_PTR enet_ptr);
static uint_32 Custom_Api_Shutdown(struct enet_context_struct *enet_ptr);
extern uint_32 Custom_Api_Send(ENET_CONTEXT_STRUCT_PTR enet_ptr, PCB_PTR pcb_ptr, uint_32 size, uint_32 frags, uint_32 flags);
extern uint_32 Custom_Api_Mediactl( ENET_CONTEXT_STRUCT_PTR enet_ptr, uint_32 command_id, pointer inout_param);

uint_32 chip_state_ctrl(ENET_CONTEXT_STRUCT_PTR enet_ptr, uint_32 state);

const ENET_MAC_IF_STRUCT ATHEROS_WIFI_IF = {
    Custom_Api_Initialize,
    Custom_Api_Shutdown,
    Custom_Api_Send,
    Custom_Api_Readmii,
    Custom_Api_Writemii,
   #if BSPCFG_ENABLE_ENET_MULTICAST    
    Custom_Api_Join,
    Custom_Api_Rejoin,
    #endif
    Custom_Api_Mediactl   
};

#if MQX_VERSION == (402)
static uint_32 Custom_Api_Readmii(struct enet_context_struct * p_ctxt, uint_32 val, uint_32 * p_val, uint_32 val2)
#else
static bool Custom_Api_Readmii(struct enet_context_struct * p_ctxt, uint_32 val, uint_32 * p_val, uint_32 val2)
#endif
{
	/* NOTHING TO DO HERE */
	UNUSED_ARGUMENT(p_ctxt);
	UNUSED_ARGUMENT(val);
	UNUSED_ARGUMENT(p_val);
	UNUSED_ARGUMENT(val2);
	while(1){};
	return 0;
}
#if MQX_VERSION == (402)
static uint_32 Custom_Api_Writemii(struct enet_context_struct * p_ctxt,  uint_32 val1, uint_32 val2 , uint_32 val3)
#else
static bool Custom_Api_Writemii(struct enet_context_struct * p_ctxt,  uint_32 val1, uint_32 val2 , uint_32 val3)
#endif
{
	/* NOTHING TO DO HERE */
	UNUSED_ARGUMENT(p_ctxt);
	UNUSED_ARGUMENT(val1);
	UNUSED_ARGUMENT(val2);
	UNUSED_ARGUMENT(val3);
	while(1){};
	return 0;
}

static uint_32 Custom_Api_Join (struct enet_context_struct * p_ctxt, struct enet_mcb_struct * p_mcb)
{
	/* NOTHING TO DO HERE */
	UNUSED_ARGUMENT(p_ctxt);
	UNUSED_ARGUMENT(p_mcb);
	//while(1){};
	return 0;
}

static uint_32 Custom_Api_Rejoin(struct enet_context_struct * p_ctxt)
{
	/* NOTHING TO DO HERE */
	UNUSED_ARGUMENT(p_ctxt);
	//while(1){};
	return 0;
}



uint_32 chip_state_ctrl(ENET_CONTEXT_STRUCT_PTR enet_ptr, uint_32 state)
{
	uint_32 error = ENET_OK;
	
	if(state == 0){
		/* confirm that the driver is not already down */
		if(enet_ptr->MAC_CONTEXT_PTR != NULL)
		{
			/* shutdown chip and driver */
			error = Custom_Api_Shutdown(enet_ptr);
		}
	}else{
		/* confirm that the driver is not already up */
		if(enet_ptr->MAC_CONTEXT_PTR == NULL)
		{
			/* bring up chip and driver */
			error = Custom_Api_Initialize(enet_ptr);
		}
	}
	
	return error;
}

/*****************************************************************************/
/*  Custom_Api_Initialize - Entry point for MQX to initialize the Driver.
 *      ENET_CONTEXT_STRUCT_PTR enet_ptr - pointer to MQX ethernet object.
 * RETURNS: ENET_OK on success or ENET_ERROR otherwise. 
 *****************************************************************************/
static uint_32 Custom_Api_Initialize(ENET_CONTEXT_STRUCT_PTR enet_ptr)
{
    uint_32 error = ENET_OK;
    A_STATUS status;    
	    
#if 0       
g_totAlloc = 0;	    
#endif

    /* If it is the second device which is getting initialised, 
     * Fill the pointers and generate the MAC address from the 
     * first device
     */
    if(p_Global_Cxt)
    {
        ENET_CONTEXT_STRUCT_PTR tempenet_ptr = (ENET_CONTEXT_STRUCT_PTR)GET_DRIVER_CXT(p_Global_Cxt)->pUpperCxt[0];
        A_UINT8 devId;
    
        devId = enet_ptr->PARAM_PTR->ENET_IF->PHY_NUMBER;
        A_ASSERT(devId < WLAN_NUM_OF_DEVICES);
        /* Generating the MAC Address of the second interface using the first interface MAC */
        A_MEMCPY(enet_ptr->ADDRESS, tempenet_ptr->ADDRESS, ATH_MAC_LEN);
        enet_ptr->ADDRESS[0] = (((enet_ptr->ADDRESS[0]) ^ (1 << devId)) | 0x2);
        //enet_ptr->ADDRESS[0] = ((enet_ptr->ADDRESS[0])+ (devId << 4)) | 0x2;
        
        enet_ptr->MAC_CONTEXT_PTR = (pointer)p_Global_Cxt;
        GET_DRIVER_CXT(p_Global_Cxt)->pUpperCxt[devId] = enet_ptr;
        p_Driver_Cxt[1] = enet_ptr;
        return error;
    }

    do{        	               
        /* allocate the driver context and assign it to the enet_ptr mac_param */
        if(NULL == (p_Global_Cxt = (A_VOID*)A_MALLOC(sizeof(A_CUSTOM_DRIVER_CONTEXT), MALLOC_ID_CONTEXT))){
            error = ENET_ERROR;
            break;
        }

        A_MEMZERO(p_Global_Cxt, sizeof(A_CUSTOM_DRIVER_CONTEXT));
        /* alloc the driver common context */
        if(NULL == (GET_DRIVER_CXT(p_Global_Cxt)->pCommonCxt = A_MALLOC(sizeof(A_DRIVER_CONTEXT), MALLOC_ID_CONTEXT))){
            error = ENET_ERROR;
            break;
        }
        
        
        enet_ptr->MAC_CONTEXT_PTR = (pointer)p_Global_Cxt;
        /* initialize backwards pointers */
        GET_DRIVER_CXT(p_Global_Cxt)->pUpperCxt[0] = enet_ptr;    
        GET_DRIVER_CXT(p_Global_Cxt)->pDriverParam = enet_ptr->PARAM_PTR->MAC_PARAM;   
        /* create the 2 driver events. */    
#if DRIVER_CONFIG_MULTI_TASKING	                
        _lwevent_create(&GET_DRIVER_CXT(p_Global_Cxt)->driverWakeEvent, LWEVENT_AUTO_CLEAR);   
        _lwevent_create(&GET_DRIVER_CXT(p_Global_Cxt)->userWakeEvent, 0/* no auto clear */);  
#if T_SELECT_VER1
        _lwevent_create(&GET_DRIVER_CXT(p_Global_Cxt)->sockSelectWakeEvent, 0/* no auto clear */);       
#endif //T_SELECT_VER1
#endif        
		/* Api_InitStart() will perform any additional allocations that are done as part of 
		 * the common_driver initialization */
        if(A_OK != (status = Api_InitStart(p_Global_Cxt))){
            error = ENET_ERROR;
            break;
        }
		/* CreateDriverThread is a custom function to create or restart the driver thread.
		 * the bulk of the driver initialization is handled by the driver thread.
		 */
#if DRIVER_CONFIG_MULTI_TASKING	 
        if(A_OK != (status = Custom_Driver_CreateThread(p_Global_Cxt))){
        	error = ENET_ERROR;
        	break;
        }         
#else
		Driver_Init(p_Global_Cxt);
#endif        
		/* Api_InitFinish waits for wmi_ready event from chip. */
        if(A_OK != Api_InitFinish(p_Global_Cxt)){
        	error = ENET_ERROR;
        	break;
        }	 

        g_driverState = DRIVER_STATE_RUN;

        Api_WMIInitFinish(p_Global_Cxt);                        
    }while(0);
   
   	if(error != ENET_OK){
   		if(p_Global_Cxt != NULL){   			
   			A_FREE(GET_DRIVER_COMMON(p_Global_Cxt), MALLOC_ID_CONTEXT);
   			A_FREE(p_Global_Cxt, MALLOC_ID_CONTEXT);
   		}
   	}else{
            p_Driver_Cxt[0] = enet_ptr;
        }
   
#if 0       
if(g_totAlloc){
	A_PRINTF("init alloc: %d\n", g_totAlloc);	
	//for more information one can implement _mem_get_free() to 
	//determine how much free memory exists in the system pool.
}	
#endif
   
	Api_BootProfile(p_Global_Cxt, BOOT_PROFILE_DRIVE_READY);

    return error;
}


/*****************************************************************************/
/*  Custom_Api_Shutdown - Entry point for MQX to shutdown the Driver.
 *      ENET_CONTEXT_STRUCT_PTR enet_ptr - pointer to MQX ethernet object.
 * RETURNS: ENET_OK on success or ENET_ERROR otherwise. 
 *****************************************************************************/
static uint_32 Custom_Api_Shutdown(struct enet_context_struct *enet_ptr)
{	
	A_VOID *pCxt = enet_ptr->MAC_CONTEXT_PTR;
	
	if(pCxt != NULL){
		Api_DeInitStart(pCxt);
#if DRIVER_CONFIG_MULTI_TASKING
		Custom_Driver_DestroyThread(pCxt);		
#else
		Driver_DeInit(pCxt);
#endif		
		Api_DeInitFinish(pCxt);
	
#if DRIVER_CONFIG_MULTI_TASKING
		_lwevent_destroy(&GET_DRIVER_CXT(p_Global_Cxt)->userWakeEvent);
		_lwevent_destroy(&GET_DRIVER_CXT(p_Global_Cxt)->driverWakeEvent);
#if T_SELECT_VER1
		_lwevent_destroy(&GET_DRIVER_CXT(p_Global_Cxt)->sockSelectWakeEvent);
#endif //T_SELECT_VER1
#endif		
		
		if(NULL != GET_DRIVER_COMMON(pCxt)){
			A_FREE(GET_DRIVER_COMMON(pCxt), MALLOC_ID_CONTEXT);
		}
   		
   		A_FREE(pCxt, MALLOC_ID_CONTEXT);
   		p_Global_Cxt = NULL;
   		enet_ptr->MAC_CONTEXT_PTR = NULL;
	}
	
	return (uint_32) ENET_OK;
}
#if 1
void Custom_Api_PowerUpDown(A_UINT8 power_up)
{

    /* - Power up down device */
	if((p_Global_Cxt !=NULL)&&((power_up == 0)||(power_up == 1)))
        HW_PowerUpDown(p_Global_Cxt, power_up);

}
#endif
