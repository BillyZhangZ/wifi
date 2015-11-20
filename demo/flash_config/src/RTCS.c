/*
 * Copyright (c) 2014 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 * $ATH_LICENSE_TARGET_C$
 */ 

#include "main.h"

#if DEMOCFG_USE_WIFI
#include "string.h"
#endif

#if DEMOCFG_ENABLE_RTCS

#include <ipcfg.h>

_enet_handle    handle;
extern const SHELL_COMMAND_STRUCT Telnet_commands[];

static void Telnetd_shell_fn (pointer dummy) 
{  
   Shell(Telnet_commands,NULL);
}

const RTCS_TASK Telnetd_shell_template = {"Telnet_shell", 8, 2000, Telnetd_shell_fn, NULL};

void HVAC_initialize_networking(void) 
{
   int_32            error;
   _enet_address           enet_address;

#if DEMOCFG_USE_POOLS && defined(DEMOCFG_RTCS_POOL_ADDR) && defined(DEMOCFG_RTCS_POOL_SIZE)
    /* use external RAM for RTCS buffers */
    _RTCS_mem_pool = _mem_create_pool((pointer)DEMOCFG_RTCS_POOL_ADDR, DEMOCFG_RTCS_POOL_SIZE);
#endif
   /* runtime RTCS configuration */
   _RTCSPCB_init = 4;
   _RTCSPCB_grow = 2;
   _RTCSPCB_max = 20;
   _RTCS_msgpool_init = 4;
   _RTCS_msgpool_grow = 2;
   _RTCS_msgpool_max  = 20;
   _RTCS_socket_part_init = 4;
   _RTCS_socket_part_grow = 2;
   _RTCS_socket_part_max  = 20;

    error = RTCS_create();

    LWDNS_server_ipaddr = ENET_IPGATEWAY;

    ENET_get_mac_address (DEMOCFG_DEFAULT_DEVICE, ENET_IPADDR, enet_address);
    error = ipcfg_init_device (DEMOCFG_DEFAULT_DEVICE, enet_address);
    error = ENET_initialize(DEMOCFG_DEFAULT_DEVICE, enet_address, 0, &handle);
     if( ENETERR_INITIALIZED_DEVICE != error )
    {
    	while(1)
    	{
    		
    	}
    }
#if DEMOCFG_USE_WIFI
    /*iwcfg_set_essid (IPCFG_default_enet_device,DEMOCFG_SSID);
    if ((strcmp(DEMOCFG_SECURITY,"wpa") == 0)||strcmp(DEMOCFG_SECURITY,"wpa2") == 0)
    {
        iwcfg_set_passphrase (IPCFG_default_enet_device,DEMOCFG_PASSPHRASE);

    }
#if 0    
    if (strcmp(DEMOCFG_SECURITY,"wep") == 0)
    {
      iwcfg_set_wep_key (IPCFG_default_enet_device,DEMOCFG_WEP_KEY,strlen(DEMOCFG_WEP_KEY),DEMOCFG_WEP_KEY_INDEX);
    }
#else
	if (strcmp(DEMOCFG_SECURITY,"wep") == 0)
    {
      iwcfg_set_wep_keys (IPCFG_default_enet_device,
      	DEMOCFG_WEP_KEY,
      	DEMOCFG_WEP_KEY,
      	DEMOCFG_WEP_KEY,
      	DEMOCFG_WEP_KEY,
      	strlen(DEMOCFG_WEP_KEY),
      	DEMOCFG_WEP_KEY_INDEX);
    }
#endif    
    iwcfg_set_sec_type (IPCFG_default_enet_device,DEMOCFG_SECURITY);
    iwcfg_set_mode (IPCFG_default_enet_device,DEMOCFG_NW_MODE);
    
    iwcfg_commit(IPCFG_default_enet_device);*/
#endif  

#if DEMOCFG_ENABLE_TELNET_SERVER
   TELNETSRV_init("Telnet_server", 7, 1800, (RTCS_TASK_PTR) &Telnetd_shell_template );
#endif
}

#else /* DEMOCFG_ENABLE_RTCS */

#if (DEMOCFG_ENABLE_FTP_SERVER+DEMOCFG_ENABLE_TELNET_SERVER) > 0
#warning RTCS network stack is disabled, the selected service will not be available.
#endif

#endif /* DEMOCFG_ENABLE_RTCS */

/* EOF */
