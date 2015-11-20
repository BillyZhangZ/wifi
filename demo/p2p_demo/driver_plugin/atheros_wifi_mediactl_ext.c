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
#include "main.h"

#if DEMOCFG_USE_SENSOR
#include "tmp106.h"
#endif

A_BOOL reg_query_bool = A_FALSE;
pointer pQuery;

struct event_profile {
   A_UINT16 event_id; 
   A_UINT32 timestamp;
};

#define MAX_PROFILE_SIZE	20

struct event_profile profiles[MAX_PROFILE_SIZE];
A_UINT8 profile_head = 0, profile_trail = 0;
	
A_UINT8 *event_str [] = 
{
    "wmi_ready",			// BOOT_PROFILE_WMI_READY
    "refclk read",          // BOOT_PROFILE_READ_REFCLK
    "config done",          // BOOT_PROFILE_DONE_CONFIG
    "start connect",        // BOOT_PROFILE_START_CONNECT
    "end connect",          // BOOT_PROFILE_DONE_CONNECT
    "driver ready",         // BOOT_PROFILE_DRIVE_READY
    "start scan",           // BOOT_PROFILE_START_SCAN
    "end scan",             // BOOT_PROFILE_DONE_SCAN
    "power up",             // BOOT_PROFILE_POWER_UP
    "boot param",           // BOOT_PROFILE_BOOT_PARAMETER
};

#if MQX_VERSION == (402)
#define CUSTOM_DELAY_TMR 0
#else
/* for Cusomter_Delay timer */
HWTIMER hwtimer2;
#endif

/*FUNCTION*-----------------------------------------------------
*
* Task Name    : timer_wakeup_isr
* Comments     : This isr is supposed to wake up the CPU core
*                and escape from "interrupts only" low power mode.
*
*END*-----------------------------------------------------------*/

static void timer_wakeup_isr(pointer parameter)
{
    uint_32 timer = (uint_32)parameter;

    /* Stop the timer */
#if MQX_VERSION == (402)
    _lpt_run (timer, FALSE);
    _lpt_clear_int (timer);
#else
    hwtimer_stop(&hwtimer2);
    hwtimer_deinit(&hwtimer2);
#endif    
    
    /* Do not return to sleep after isr again */
    _lpm_wakeup_core ();

}


/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : Custom_Delay
*  PARAMS         : Delay in msec
*                   
*  Returned Value : none
*  Comments       :
*        Starts a timer for specified number of msec, puts EFM in EM2 mode.
*        EFM will wake up when timer fires.
*
*END*-----------------------------------------------------------------*/
void Custom_Delay(A_UINT32 delay)
{
#if MQX_VERSION == (402)
    /* Install the timer */
    if (MQX_OK != _lpt_install (0,  delay*1000 , LPT_FLAG_CLOCK_SOURCE_LPO, 2, timer_wakeup_isr, TRUE))
    {
        printf ("\nError during installation of timer interrupt!\n");
        _task_block();
    }
        
    /* Stop the timer */
    _lpt_run (0, FALSE);
	/* start timer */
  	_lpt_run (CUSTOM_DELAY_TMR, TRUE);
#else
    if (MQX_OK != hwtimer_init(&hwtimer2, &BSP_HWTIMER2_DEV, BSP_HWTIMER2_ID, (BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX + 1)))
    {
            printf ("\nError during installation of timer interrupt!\n");
            _task_block();
    }

    hwtimer_set_period(&hwtimer2, BSP_HWTIMER2_SOURCE_CLK, delay*1000);
    hwtimer_callback_reg(&hwtimer2,(HWTIMER_CALLBACK_FPTR)timer_wakeup_isr, NULL);
    /* Start hwtimer2*/
    hwtimer_start(&hwtimer2);
#endif	
	
	/* Setting operation mode to LPM_OPERATION_MODE_SLEEP */
	_lpm_set_operation_mode (LPM_OPERATION_MODE_STOP);

    /* Return to RUN mode */
    _lpm_set_operation_mode (LPM_OPERATION_MODE_RUN);
    if (CM_ERR_OK != _lpm_set_clock_configuration(BSP_CLOCK_CONFIGURATION_DEFAULT))
    {
        printf("Cannot change clock configuration");
        _task_block();
    }
}
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
	A_STATUS status = A_OK;
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


#define LED_1 BSP_LED1
#define LED_2 BSP_LED2


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
static MQX_FILE_PTR input_port=NULL;
extern boolean InitializeIO(void);
boolean InitializeIO(void) 
{
	
#if 1
    const uint_32 output_set[] = {
        LED_1 | GPIO_PIN_STATUS_0,
        GPIO_LIST_END
    };

    const uint_32 input_set[] = {
        (GPIO_PORT_C | GPIO_PIN3),
        GPIO_LIST_END
    };

    /* Open and set port TC as output to drive LEDs (LED10 - LED13) */
    output_port = fopen("gpio:write", (char_ptr) &output_set);

    if (output_port)
        ioctl(output_port, GPIO_IOCTL_WRITE_LOG0, NULL);    
    fclose(output_port);

    input_port = fopen("gpio:read", (char_ptr) &input_set);
    fclose(input_port);

#if DEMOCFG_USE_SENSOR
    if(!tmp106_init())
    {
      printf("tmp106 device init failed ! \n");
    }
#endif     
#endif 
   return (output_port !=NULL);
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


#define CYCLE_QUEUE_INC(ndx) {ndx = (ndx + 1) % MAX_PROFILE_SIZE;}

void add_one_profile(A_UINT8 event_id, A_UINT16  timestamp)
{
    profiles[profile_head].event_id = event_id;
    profiles[profile_head].timestamp = timestamp;

    CYCLE_QUEUE_INC(profile_head);
    if (profile_head == profile_trail)
        CYCLE_QUEUE_INC(profile_trail);		
}

int_32 print_perf(int_32 argc, char_ptr argv[])
{
    int_32 ret = 0;
    A_UINT8   ndx;

    if (profile_trail == profile_head)
        return ret;

    printf ("       Event        TIME STAMP\n");
    ndx = profile_trail;
    while (ndx !=  profile_head) {
        printf("% 15s   % 6d\n", event_str[profiles[ndx].event_id], profiles[ndx].timestamp);
        CYCLE_QUEUE_INC(ndx);	
    }

    return ret;
}

void boot_profile_output(uint_32 signal) 
{
    int		i;
    A_INT32	  timestamp;
   TIME_STRUCT    time;

   _time_get_elapsed(&time);
   timestamp = (time.SECONDS * 1000 + time.MILLISECONDS);

    add_one_profile(signal, timestamp);

    //printf ("event=%s   TIME: %d\n", event_str[signal], timestamp); 
}

void gpio_process_register_value(A_UINT8 *datap, A_UINT32 len)
{
	WMIX_GPIO_DATA_EVENT  *pEvent = (WMIX_GPIO_DATA_EVENT *)datap;
 	printf ("\nprocess_register_value len:%d\n", len);
 	printf ("reg id:0x%08X    value=0x%08X\n", pEvent->reg_id, pEvent->value);
}

#define	   QUEUE_IDX_INC(a)   ((a) = ((a) + 1) % MAX_PROFILE_SIZE)
#define	 kf_profile_head   profile_head
#define	 kf_profile_trail  profile_trail
#define	 kf_profiles       profiles

void clear_kf_profile()
{
	kf_profile_head = 0;
	kf_profile_trail = 0;
}

void add_one_kf_profile(A_UINT16 event_id, A_UINT32  timestamp)
{
	kf_profiles[kf_profile_head].event_id = event_id;
	kf_profiles[kf_profile_head].timestamp = timestamp;

    QUEUE_IDX_INC(kf_profile_head);
    if (kf_profile_head == kf_profile_trail)
    	QUEUE_IDX_INC(kf_profile_trail);		
}

A_VOID      Custom_PfmDataEventRx(A_UINT8 *datap, A_UINT32 len)
{
	int		num, i;
	WMIX_PFM_DATA_EVENT  *pfm_data;
 
    pfm_data = (WMIX_PFM_DATA_EVENT *)datap;

	num = len / sizeof(WMIX_PFM_DATA_EVENT);

    for (i=0; i < num; i++, pfm_data++)
    {   
		add_one_kf_profile(pfm_data->event_id, pfm_data->log_time);
  	}

    return;
}

A_VOID      Custom_PfmDataDoneEventRx(A_VOID *pCxt, A_UINT8 *datap, A_UINT32 len)
{
    A_DRIVER_CONTEXT *pDCxt = GET_DRIVER_COMMON(pCxt);

	pDCxt->pfmDone = A_TRUE;

//    Api_BootProfile(pCxt, BOOT_PROFILE_DONE_SCAN);

	CUSTOM_API_SCAN_COMPLETE_EVENT(pCxt, status);

    return;
}

A_UINT8 *kf_once_event_str [] = 
{
	"tune start",			// BOOT_PROFILE_WMI_READY
	"wait boot param",          // BOOT_PROFILE_READ_REFCLK
    "got boot param",          // BOOT_PROFILE_DONE_CONFIG
    "radio init start",        // BOOT_PROFILE_START_CONNECT
    "radio init done",          // BOOT_PROFILE_DONE_CONNECT
    "wmi ready",         // BOOT_PROFILE_DRIVE_READY
    "argo init start",           // BOOT_PROFILE_START_SCAN
    "argo init done",             // BOOT_PROFILE_DONE_SCAN
    "init start",           // BOOT_PROFILE_START_SCAN
    "init done",             // BOOT_PROFILE_DONE_SCAN
    "reserv 1",             // BOOT_PROFILE_DONE_SCAN
    "reserv 2",             // BOOT_PROFILE_DONE_SCAN
    "reserv 3",             // BOOT_PROFILE_DONE_SCAN
    "reserv 4",             // BOOT_PROFILE_DONE_SCAN
};

A_UINT8 *kf_repeat_event_str [] = 
{
	"wmi_ready",			// BOOT_PROFILE_WMI_READY
	"refclk read",          // BOOT_PROFILE_READ_REFCLK
    "config done",          // BOOT_PROFILE_DONE_CONFIG
    "start connect",        // BOOT_PROFILE_START_CONNECT
    "end connect",          // BOOT_PROFILE_DONE_CONNECT
    "driver ready",         // BOOT_PROFILE_DRIVE_READY
    "start scan",           // BOOT_PROFILE_START_SCAN
    "end scan",             // BOOT_PROFILE_DONE_SCAN
};

A_UINT8 *kf_statistic_event_str [] = 
{
	"wmi_ready",			// BOOT_PROFILE_WMI_READY
	"refclk read",          // BOOT_PROFILE_READ_REFCLK
    "config done",          // BOOT_PROFILE_DONE_CONFIG
    "start connect",        // BOOT_PROFILE_START_CONNECT
    "end connect",          // BOOT_PROFILE_DONE_CONNECT
    "driver ready",         // BOOT_PROFILE_DRIVE_READY
    "start scan",           // BOOT_PROFILE_START_SCAN
    "end scan",             // BOOT_PROFILE_DONE_SCAN
};

void output_kf_pfm ()
{
   A_UINT8   ndx;
   A_UINT16	 id;
   A_UINT8    *type, *event_name;

    if (kf_profile_trail == kf_profile_head)
      return;

    printf ("\n    Type            Event            TIME STAMP\n");
	ndx = kf_profile_trail;
    while (ndx !=  kf_profile_head)
    {
		id = kf_profiles[ndx].event_id;
        switch ((id >> 12) & 0x0F)
        {
		case	1:
			type = "once";
			event_name = kf_once_event_str[(id & 0x0F) - 1];
			break;

		case	2:
			type = "repeat";
			event_name = kf_repeat_event_str[id & 0x0F - 1];
			break;

		case	3:
			type = "stati";
			event_name = kf_statistic_event_str[id & 0x0F - 1];
			break;
		default:
			type = "unknown";
			event_name = "unknown";
			break;
		}
    	printf ("% 8s  |  % 15s   |  % 6d\n", type, event_name, kf_profiles[ndx].timestamp);
    	QUEUE_IDX_INC(ndx);	
    } 
}

#if 1
void atheros_driver_setup(void)
{
#if MQX_VERSION == (402)	
	/* install lpt timer */
    if (MQX_OK != _lpt_install (CUSTOM_DELAY_TMR, 10*1000000, LPT_FLAG_CLOCK_SOURCE_LPO, 2, timer_wakeup_isr, TRUE))
    {
        printf ("\nError during installation of timer interrupt!\n");
        _task_block();
    }
    
    /* Stop the timer */
    _lpt_run (0, FALSE);
#endif

	ath_custom_init.Custom_Delay = Custom_Delay;

	ath_custom_mediactl.ath_ioctl_handler_ext = ath_ioctl_handler_ext;
	//ath_custom_init.Boot_Profile = LED_SetOutput;
	ath_custom_init.Boot_Profile = boot_profile_output;

    ath_custom_init.Api_GpioDataEventRx = gpio_process_register_value;


    ath_custom_init.Api_PfmDataEventRx = Custom_PfmDataEventRx;
    ath_custom_init.Api_PfmDataDoneEventRx = Custom_PfmDataDoneEventRx;

	InitializeIO();
}    
#else
void atheros_driver_setup(void)
{
//	ath_custom_mediactl.ath_ioctl_handler_ext = ath_ioctl_handler_ext;
}
#endif 



/* EOF */
