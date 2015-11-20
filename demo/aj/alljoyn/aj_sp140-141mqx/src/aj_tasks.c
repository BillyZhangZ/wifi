/******************************************************************************
 * Copyright 2013, Qualcomm Connected Experiences, Inc.
 *
 ******************************************************************************/
#include "aj_tasks.h"

/*
 * task entry points 
 */
extern void AllJoyn_Start(unsigned long arg);

#define TASK_TEMPLATE_LIST_END         {0, 0, 0, 0, 0, 0, 0}


/* MQX task template list */
const TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
  {
  /* Task number                      */  ALLJOYN_TASK,
  /* Entry point                      */  (TASK_FPTR)AllJoyn_Start,
  /* Stack size                       */  ALLJOYN_TASK_STACK_SIZE,
  /* Task priority                    */  7U,
  /* Task name                        */  "AllJoyn",
  /* Task attributes                  */  MQX_AUTO_START_TASK,
  /* Task parameter                   */  (uint32_t)(0)
  },
  TASK_TEMPLATE_LIST_END
};
