/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: HVAC_Task.c$
* $Version : 3.6.21.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   
*
*END************************************************************************/


#include "hvac.h"

#include "logging_public.h"
#include "hvac_public.h"
#include "hvac_private.h"
#include "../../mqx/source/io/enet/atheros_wifi/atheros_wifi_api.h"


#if DEMOCFG_ENABLE_RTCS
#include <ipcfg.h>
#endif


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


#if ! BSPCFG_ENABLE_GPIODEV
#error This application requires BSPCFG_ENABLE_GPIODEV defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#if DEMOCFG_ENABLE_KLOG
    #if !MQX_KERNEL_LOGGING
    #warning Need MQX_KERNEL_LOGGING enabled in kernel (user_config.h and user_config.cw)
    #endif
    #if !defined(DEMOCFG_KLOG_ADDR) || !defined(DEMOCFG_KLOG_SIZE)
    #warning Need klog address and size
    #endif
#endif

uint_32 get_rssi();

HVAC_STATE  HVAC_State =  {HVAC_Off};

void HVAC_Task(uint_32 data)
{
   HVAC_Mode_t mode;
   uint_32 counter = HVAC_LOG_CYCLE_IN_CONTROL_CYCLES;
   
   _int_install_unexpected_isr();


#if DEMOCFG_ENABLE_KLOG && MQX_KERNEL_LOGGING && defined(DEMOCFG_KLOG_ADDR) && defined(DEMOCFG_KLOG_SIZE)

   /* create kernel log */
   _klog_create_at(DEMOCFG_KLOG_SIZE, 0,(pointer)DEMOCFG_KLOG_ADDR);
   
   /* Enable kernel logging */
   _klog_control(KLOG_ENABLED | KLOG_CONTEXT_ENABLED |
      KLOG_INTERRUPTS_ENABLED| 
      KLOG_FUNCTIONS_ENABLED|KLOG_RTCS_FUNCTIONS, TRUE);

   _klog_log_function(HVAC_Task);
#endif


   // Configure and reset outputs
   HVAC_InitializeIO();
   
#if DEMOCFG_ENABLE_RTCS
   HVAC_initialize_networking();
#endif


   // Initialize operatiing parameters to default values
   HVAC_InitializeParameters();

   
  // _task_create(0, ALIVE_TASK, 0);

   #if DEMOCFG_ENABLE_AUTO_LOGGING
       LogInit();
       _time_delay (2000);
       Log("HVAC Started\n");
   #endif
   

   while (TRUE) {
      // Read current temperature
      HVAC_State.ActualTemperature = HVAC_GetAmbientTemperature();

      // Examine current parameters and set state accordingly
      HVAC_State.HVACState = HVAC_Off;
      HVAC_State.FanOn = FALSE;

      mode = HVAC_GetHVACMode();
      
      if (mode == HVAC_Cool || mode == HVAC_Auto)  
      {
         if (HVAC_State.ActualTemperature > (HVAC_Params.DesiredTemperature+HVAC_TEMP_TOLERANCE)) 
         {
            HVAC_State.HVACState = HVAC_Cool;
            HVAC_State.FanOn = TRUE;
         }
      } 
      
      if (mode == HVAC_Heat || mode == HVAC_Auto) 
      {
         if (HVAC_State.ActualTemperature < (HVAC_Params.DesiredTemperature-HVAC_TEMP_TOLERANCE)) 
         {
            HVAC_State.HVACState = HVAC_Heat;
            HVAC_State.FanOn = TRUE;
         }
      }

      if (HVAC_GetFanMode() == Fan_On) {
         HVAC_State.FanOn = TRUE;
      }         

      // Set outputs to reflect new state 
      HVAC_SetOutput(HVAC_FAN_OUTPUT, HVAC_State.FanOn);
      HVAC_SetOutput(HVAC_HEAT_OUTPUT,  HVAC_State.HVACState == HVAC_Heat);
      //HVAC_SetOutput(HVAC_COOL_OUTPUT,  HVAC_State.HVACState == HVAC_Cool);

      // Log Current state
      if (++counter >= HVAC_LOG_CYCLE_IN_CONTROL_CYCLES) 
      {
         counter = 0;
         HVAC_LogCurrentState();
         
      }
      
      // Wait for a change in parameters, or a new cycle
      if (HVAC_WaitParameters(HVAC_CONTROL_CYCLE_IN_TICKS)) 
      {
         counter = HVAC_LOG_CYCLE_IN_CONTROL_CYCLES;
      }

#if DEMOCFG_ENABLE_RTCS
      ipcfg_task_poll ();
#endif
   }
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : HeartBeat_Task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

void HeartBeat_Task(uint_32 initial_data)
{ 
    // initialize IO before starting this task

    _mqx_int delay, value = 0;

#if BSPCFG_ENABLE_ADC
    HVAC_InitializeADC();
#endif // BSPCFG_ENABLE_ADC

    while (TRUE) {
    
#if BSPCFG_ENABLE_ADC
        delay = 100 + (1000 * ReadADC() / 0x0fff);
#else
        delay = 100;
#endif // BSPCFG_ENABLE_ADC

        _time_delay(delay);
        HVAC_SetOutput(HVAC_ALIVE_OUTPUT, value);
        
        value ^= 1;  // toggle next value
    }

}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : WifiConnected()
* Returned Value  : NA
* Comments        : Sets an LED to indicate that the wifi is connected
*					to the network.
*
*END*-----------------------------------------------------------------*/
void WifiConnected(int value)
{
	if(value){
		HVAC_SetOutput(HVAC_ALIVE_OUTPUT,TRUE);
	}else{
		HVAC_SetOutput(HVAC_ALIVE_OUTPUT,FALSE);
	}
}





/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : get_rssi()
* Returned Value  : ENET_OK if ESSID set successfully else ERROR CODE
* Comments        : Sets the ESSID. ESSID comes into effect only when 
*                   user commits his changes.
*
*END*-----------------------------------------------------------------*/

uint_32 get_rssi()
{
    uint_32 error,dev_status,i=0;
    uint_32  rssi = 0;  
     ATH_IOCTL_PARAM_STRUCT inout_param;
     
     
    error = ENET_mediactl (handle,ENET_MEDIACTL_IS_INITIALIZED,&dev_status);
    if (ENET_OK != error)
    {
        return error;
    }
    if (dev_status == FALSE)
    {
        return ENET_ERROR;
    }
    
	inout_param.cmd_id = ATH_GET_RX_RSSI;
	inout_param.data = &rssi;
	inout_param.length = 4;
	
    error = ENET_mediactl (handle, ENET_MEDIACTL_VENDOR_SPECIFIC, &inout_param);
    

    return rssi;

    printf("IWCONFIG_ERROR: Invalid Device number\n");
    return ENETERR_INVALID_DEVICE;
}