/**HEADER*******************************************************************
* 
* Copyright (c) 2014 Freescale Semiconductor;
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

#if MQX_KERNEL_LOGGING
#include <klog.h>
#endif
#if MQX_USE_LOGS
#include <log.h>
#endif



#include "main.h"
#include "usb_test.h"

#define trace printf

const SHELL_COMMAND_STRUCT Shell_commands[] = { 
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
   {0,                            0,                     0,                        0,     0,                 0,               }
};


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

   int              error;
      
#if MQX_USE_LOGS
   /* create the log component */
   log_result = _log_create_component();
   if (log_result != MQX_OK) { 
      printf("Error: log component could not be created\n");
   } /* endif */
#endif
       

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
   do {

       error = unit_test_usb_host();
       if(error != 0){
           trace("USB:E\n");
           break;
       }


   }while(1);
          
   for (;;)   
   { 
      /* Run the shell */ 
      Shell(Shell_commands, NULL); 
      printf("Shell exited, restarting...\n"); 
   } 

   

}
/* EOF */
