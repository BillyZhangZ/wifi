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
* $FileName: RTCS.c$
* $Version : 3.5.17.0$
* $Date    : Jan-6-2010$
*
* Comments:
*
*   Example of HVAC using RTCS.
*
*END************************************************************************/

#include "main.h"

#if DEMOCFG_USE_WIFI
#include "string.h"
#endif

#if DEMOCFG_ENABLE_RTCS

#include <ipcfg.h>

_enet_handle    enet_handle[2];
_enet_handle    handle; //EY ADD
extern const SHELL_COMMAND_STRUCT Telnet_commands[];

extern int_32 clear_wep_keys();

static void Telnetd_shell_fn (pointer dummy) 
{  
   Shell(Telnet_commands,NULL);
}

const RTCS_TASK Telnetd_shell_template = {"Telnet_shell", 8, 2000, Telnetd_shell_fn, NULL};

void HVAC_initialize_networking(void) 
{
   int_32            error;
    IPCFG_IP_ADDRESS_DATA   ip_data;
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
   _RTCSTASK_stacksize = 11000;

    error = RTCS_create();

    LWDNS_server_ipaddr = ENET_IPGATEWAY;

    ip_data.ip = ENET_IPADDR;
    ip_data.mask = ENET_IPMASK;
    ip_data.gateway = ENET_IPGATEWAY;
    
    ENET_get_mac_address (DEMOCFG_DEFAULT_DEVICE, ENET_IPADDR, enet_address);
    error = ipcfg_init_device (DEMOCFG_DEFAULT_DEVICE, enet_address);
     if((error = ENET_initialize(DEMOCFG_DEFAULT_DEVICE, enet_address, 0, &enet_handle[0])) != ENETERR_INITIALIZED_DEVICE)
    {
        A_ASSERT(0);
    }
#if DEMOCFG_USE_WIFI
#if USE_ATH_CHANGES
	clear_wep_keys();
#endif	
#endif
	
    error = ipcfg_bind_staticip (DEMOCFG_DEFAULT_DEVICE, &ip_data);

#if (WLAN_NUM_OF_DEVICES > 1)
    
    ENET_get_mac_address (DEMOCFG_DEFAULT_DEVICE + 1, IPADDR(192,168,2,90), enet_address);
    error = ipcfg_init_device (DEMOCFG_DEFAULT_DEVICE + 1, enet_address);
    
     if((error = ENET_initialize(DEMOCFG_DEFAULT_DEVICE + 1, enet_address, 0, &enet_handle[1])) != ENETERR_INITIALIZED_DEVICE)
    {
        A_ASSERT(0);
    }
    
    ip_data.ip = IPADDR(192,168,2,90);
    ip_data.mask = ENET_IPMASK;
    
    error = ipcfg_bind_staticip (DEMOCFG_DEFAULT_DEVICE + 1, &ip_data);
    
#endif
    
    /* For ENABLE_IP_OFFLOAD, Use the enet handle for the first device */
    handle = enet_handle[0];
    

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
