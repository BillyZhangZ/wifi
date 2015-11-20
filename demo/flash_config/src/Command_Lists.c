/*
 * Copyright (c) 2014 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 * $ATH_LICENSE_TARGET_C$
 */ 
/**************************************************************************
*
* $FileName: Command_Lists.c$
* $Version : 3.5.16.0$
* $Date    : Dec-2-2009$
*
* Comments:
*
*   
*
*END************************************************************************/

#include "main.h"


const SHELL_COMMAND_STRUCT Shell_commands[] = {
   { "exit",      Shell_exit },      
   { "help",      Shell_help }, 
  // { "benchtx",   fapp_benchtx_cmd},   
  // { "benchrx",   fapp_benchrx_cmd},

#if DEMOCFG_ENABLE_RTCS
   { "netstat",   Shell_netstat },  
   { "ipconfig",  Shell_ipconfig },
#if DEMOCFG_USE_WIFI   
   { "iwconfig",  Shell_iwconfig },
  // { "wmiconfig", wmiconfig_handler },
#endif      
#if RTCSCFG_ENABLE_ICMP
   { "ping",      Shell_ping },      
#endif   
#endif
   { "?",         Shell_command_list },     
   { NULL,        NULL } 
};

const SHELL_COMMAND_STRUCT Telnet_commands[] = {
   { "exit",      Shell_exit },      
   { "help",      Shell_help }, 
#if DEMOCFG_ENABLE_USB_FILESYSTEM
   { "log",       Shell_log },
#endif

#if DEMOCFG_ENABLE_RTCS
#if RTCSCFG_ENABLE_ICMP
   { "ping",      Shell_ping },      
#endif   
#endif   

   { "?",         Shell_command_list },     
   
   { NULL,        NULL } 
};


/* EOF */
