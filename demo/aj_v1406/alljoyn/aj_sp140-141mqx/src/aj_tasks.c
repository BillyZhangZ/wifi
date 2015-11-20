/******************************************************************************
 * Copyright 2013, Qualcomm Connected Experiences, Inc.
 *
 ******************************************************************************/
#include "aj_tasks.h"

/*
 * task entry points 
 */
extern void AllJoyn_Start(unsigned long arg);
#if 1
extern void uart_rx_task ( uint32_t);
extern void uart_tx_task ( uint32_t);
#endif

#define TASK_TEMPLATE_LIST_END         {0, 0, 0, 0, 0, 0, 0}


/* MQX task template list */
const TASK_TEMPLATE_STRUCT MQX_template_list[] =
{

#if 1
  {
  /* Task number                      */  ALLJOYN_TASK,
  /* Entry point                      */  (TASK_FPTR)AllJoyn_Start,
  /* Stack size                       */  ALLJOYN_TASK_STACK_SIZE,
  /* Task priority                    */  7U,
  /* Task name                        */  "AllJoyn",
  /* Task attributes                  */  MQX_AUTO_START_TASK,
  /* Task parameter                   */  (uint32_t)(0)
  },
#endif
#if 0
  {
  /* Task number                      */  ALLJOYN_TASK+1,
  /* Entry point                      */  (TASK_FPTR)uart_rx_task,
  /* Stack size                       */  1024,
  /* Task priority                    */  7U,   //7U, /* Note: This must be set equal to AllJoyn task's, otherwise, when exit from wifi module low power mode( QCA is power down), can't scheduling an available task as AllJoyn task is sleepping*/
  /* Task name                        */  "Uart_Rx_Task",
  /* Task attributes                  */  MQX_AUTO_START_TASK,
  /* Task parameter                   */  (uint32_t)(0)
  },
#endif
#if 0
  {
  /* Task number                      */  ALLJOYN_TASK+2,
  /* Entry point                      */  (TASK_FPTR)uart_tx_task,
  /* Stack size                       */  1024,
  /* Task priority                    */  8U,   //8U
  /* Task name                        */  "Uart_Tx_Task",
  /* Task attributes                  */  MQX_AUTO_START_TASK,
  /* Task parameter                   */  (uint32_t)(0)
  },
#endif  
  TASK_TEMPLATE_LIST_END
};
