/***************************************************************************
* 
* Copyright (c) 2015 Freescale Semiconductor;
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
*
*END************************************************************************/

#include "main.h"
#if MQX_KERNEL_LOGGING
#include <klog.h>
#endif
#if MQX_USE_LOGS
#include <log.h>
#endif

#include "parking.h"

//#include "psptypes_legacy.h"



const SHELL_COMMAND_STRUCT Shell_commands[] = { 



/*    Add your custom shell commands here    */ 
/* { "command_name",        Your_function }, */ 
   { "sh",        Shell_sh }, 
   { "help",      Shell_help }, 
   { "?",         Shell_command_list },     
   { "exit",      Shell_exit },  
   { NULL,        NULL }, 
}; 


extern void Main_task(uint_32);

TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number, Entry point, Stack,                 Pri,   String, Auto? */
   {MAIN_TASK,   Main_task,   1500,                    12,   "main", MQX_AUTO_START_TASK},
   {MAG_TASK,    Mag_task,    MAG_TASK_STACK_SIZE,     11,   "mag",  MQX_AUTO_START_TASK}, /* this priority must same as BT stack coop task */
   {0,           0,           0,     0,   0,      0,                 }
};

static void *parkingcb(PARKINGEVENT event){
    switch(event){
        case PARKING_EVENT_EMPTY:
            printf("PARKING_EVENT_EMPTY \n");
            break;
        case PARKING_EVENT_OCCUPIED:
            printf("PARKING_EVENT_OCCUPIED \n");
            break;
        default:
            printf("Unknow parkingcb event %d \n", event);
            break;
    }
    return NULL;
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Main_task
* Comments       :
*    This task initializes MFS and starts SHELL.
*
*END------------------------------------------------------------------*/

void Main_task(uint32_t initial_data)
{
#if MQX_USE_LOGS || MQX_KERNEL_LOGGING
   _mqx_uint                 log_result;
#endif 

   unsigned int collectionrateint, collectionratefrac;
      
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
      //KLOG_MESSAGE_FUNCTION S       |
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

    _int_install_unexpected_isr();

   /*******************************
   * 
   * START YOUR CODING HERE
   *
   ********************************/   
    collectionrateint  = 2;
    collectionratefrac = 0;
    while(parking_set_parameter(NULL, NULL, NULL, &collectionrateint, &collectionratefrac, parkingcb) != 0){
        _time_delay(100);
    }

          
   for (;;)   
   { 
      /* Run the shell */ 
      Shell(Shell_commands, NULL); 
      printf("Shell exited, restarting...\n"); 
   } 

   

}
/* EOF */
