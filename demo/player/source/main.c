/**HEADER*******************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
**************************************************************************** 
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
****************************************************************************
*
* Comments:
*
*   This file contains main initialization for your application
*   and infinite loop
*
*END************************************************************************/

#include "main.h"
#if MQX_KERNEL_LOGGING
#include <klog.h>
#endif
#if MQX_USE_LOGS
#include <log.h>
#endif

#include "play.h"

#include "usb_bluetooth_task.h"
#if MFI_ENABLED /* if MFI enabled, SD player FLAC will be impacted by memory */
#include "usb_mfi_task.h"
#endif
#include "umass_task.h"

#include "aoa_task.h"

#include "sd_task.h"


extern int_32  Shell_bt(int_32 argc, char_ptr argv[] );

static _usb_host_handle host_handle;  /* Global handle for calling host   */

static USB_HOST_DRIVER_INFO driverinfotable[10];

const SHELL_COMMAND_STRUCT Shell_commands[] = { 



/*    Add your custom shell commands here    */ 
/* { "command_name",        Your_function }, */ 
   { "bt",        Shell_bt },  
   { "sh",        Shell_sh }, 
   { "help",      Shell_help }, 
   { "?",         Shell_command_list },     
   { "exit",      Shell_exit },  
   { NULL,        NULL }, 
}; 



TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number,             Entry point,              Stack,                    Pri,   String,            Auto? */
   {MAIN_TASK,                Main_task,                1500,                     13,   "main",           MQX_AUTO_START_TASK},
   {USB_BLUETOOTH_TASK,    USB_bluetooth_task,    USB_BLUETOOTH_TASK_STACK_SIZE,  10,   "USB_BLUETOOTH",     0                }, /* this priority must same as BT stack coop task */
#if MFI_ENABLED
   {USB_MFI_TASK,          USB_mfi_task,                1500,                     10,   "USB_MFI",           0                }, 
#endif
   {UMASS_TASK,              umass_task,               UMASS_TASK_STACK_SIZE,     10,   "UAMSS",             0                }, 
   {AOA_TASK,              aoa_audio_task,               AOA_TASK_STACK_SIZE,     10,   "aoap",              0                }, 
#if BSP_K22FSH                                 
   {SD_TASK,                      sd_task,               SD_TASK_STACK_SIZE,      10,   "sd",                0                }, 
#endif

   {0,                            0,                     0,                        0,     0,                 0,               }
};

void __USB_reset_device(void){

#ifdef VBUSPIN
    LWGPIO_STRUCT vbus;
    _time_delay(50);

    /* initialize lwgpio handle vbus for PORTB 8 pin */
    if (!lwgpio_init(&vbus, VBUSPIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing VBUS PORTB 8 as GPIO output failed.\n");
        _task_block();
    }
    /* swich pin functionality (MUX) to GPIO mode */
    lwgpio_set_functionality(&vbus, VBUS_MUX_GPIO);

    /* write logical 0 to the pin */
    lwgpio_set_value(&vbus, LWGPIO_VALUE_LOW); /* set pin to 0 */
    _time_delay(1000); /* some bluetooth USB device need about 1S reset on K60 platform */
    /* write logical 1 to the pin */
    lwgpio_set_value(&vbus, LWGPIO_VALUE_HIGH); /* set pin to 1 */
    _time_delay(100);
#endif
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Main_task
* Comments       :
*    This task initializes MFS and starts SHELL.
*
*END------------------------------------------------------------------*/

void Main_task(uint_32 initial_data)
{
#if MQX_USE_LOGS || MQX_KERNEL_LOGGING
   _mqx_uint                 log_result;
#endif 

   USB_STATUS           error;

   int driverinfotableEntryAvail = sizeof(driverinfotable) / sizeof(driverinfotable[0]);
   int driverinfotableEntrySize  = sizeof(driverinfotable) / sizeof(driverinfotable[0]);
   int driverinfotableEntryUsed  = 0;
      
#if MQX_USE_LOGS
   /* create the log component */
   log_result = _log_create_component();
   if (log_result != MQX_OK) { 
      printf("Error: log component could not be created\n");
   } /* endif */
#endif
       
   /* USB hostinit */ 
    //usbh_init(); 

#if MQX_KERNEL_LOGGING == 1
   /* create log number 0 */
   log_result = _klog_create(9000, LOG_OVERWRITE);
   if (log_result != MQX_OK) { 
      printf("Error: log 0 could not be created\n");
   } /* endif */

   /* define kernel logging */
   _klog_control(0xFFFFFFFF, FALSE);
   _klog_control(
      KLOG_ENABLED                  |
      //KLOG_FUNCTIONS_ENABLED        |
      //KLOG_INTERRUPTS_ENABLED       |
      //KLOG_SYSTEM_CLOCK_INT_ENABLED |
      KLOG_CONTEXT_ENABLED          |
      //KLOG_TASKING_FUNCTIONS        |
      //KLOG_ERROR_FUNCTIONS          |
     // KLOG_MESSAGE_FUNCTIONS        |
      //KLOG_INTERRUPT_FUNCTIONS      |
      //KLOG_MEMORY_FUNCTIONS         |
      //KLOG_TIME_FUNCTIONS           |
      //KLOG_EVENT_FUNCTIONS          |
      //KLOG_NAME_FUNCTIONS           |
      //KLOG_MUTEX_FUNCTIONS          |
      //KLOG_SEMAPHORE_FUNCTIONS      |
      KLOG_WATCHDOG_FUNCTIONS, 
      TRUE
      );
#endif

   /*******************************
   * 
   * START YOUR CODING HERE
   *
   ********************************/   
   memset(driverinfotable, 0, sizeof(driverinfotable));

   driverinfotableEntryUsed   += usbBtRegisterDriverInfoTable(driverinfotable, driverinfotableEntryAvail);
   driverinfotableEntryAvail   = driverinfotableEntrySize - driverinfotableEntryUsed;

#if MFI_ENABLED
   driverinfotableEntryUsed   += usbMfiRegisterDriverInfoTable(driverinfotable + driverinfotableEntryUsed, driverinfotableEntryAvail);
   driverinfotableEntryAvail   = driverinfotableEntrySize - driverinfotableEntryUsed;
#endif

   driverinfotableEntryUsed   += umassRegisterDriverInfoTable(driverinfotable + driverinfotableEntryUsed, driverinfotableEntryAvail);
   driverinfotableEntryAvail   = driverinfotableEntrySize - driverinfotableEntryUsed;


    // aoa
   driverinfotableEntryUsed   += aoaRegisterDriverInfoTable(driverinfotable + driverinfotableEntryUsed, driverinfotableEntryAvail);
   driverinfotableEntryAvail   = driverinfotableEntrySize - driverinfotableEntryUsed;

#ifdef VBUSPIN
   __USB_reset_device();
#endif



    USB_lock();

    _int_install_unexpected_isr();



   if (MQX_OK != _usb_host_driver_install(USBCFG_DEFAULT_HOST_CONTROLLER)) {
      printf("\n Driver installation failed");
      _task_block();
   }

    error = _usb_host_init(USBCFG_DEFAULT_HOST_CONTROLLER, &host_handle);
    if (error == USB_OK) {
        error = _usb_host_driver_info_register(host_handle, (pointer)driverinfotable);
        if (error == USB_OK) {
            error = _usb_host_register_service(host_handle, USB_SERVICE_HOST_RESUME, NULL);
        }
    }

    USB_unlock();

    if (error != USB_OK) {
        _task_block();
    }

    playback_init();

   _task_create(0, USB_BLUETOOTH_TASK, 0);
#if MFI_ENABLED
   _task_create(0, USB_MFI_TASK,       0);
#endif
   _task_create(0, UMASS_TASK,         0);
          
   acc_host_info_init();
   _task_create(0, AOA_TASK,         0);
#if BSP_K22FSH                                 
   _task_create(0, SD_TASK ,         0);
#endif
          
   for (;;)   
   { 
      /* Run the shell */ 
      Shell(Shell_commands, NULL); 
      printf("Shell exited, restarting...\n"); 
   } 

   

}
/* EOF */
