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

#include "HVAC.h"
#include "atheros_wifi_api.h"
#if DEMOCFG_USE_WIFI
#include "string.h"
#endif

#if DEMOCFG_ENABLE_RTCS

#include <ipcfg.h>

#if (DEMOCFG_ENABLE_FTP_SERVER+DEMOCFG_ENABLE_TELNET_SERVER) != 1
#warning Please enable one of the network services. The restriction is only for RAM size limited devices.
#endif

extern const SHELL_COMMAND_STRUCT Telnet_commands[];

_enet_handle    handle;
cipher_t cipher;
static uint_32 set_sec_mode
(
    uint_32 dev_num,
    char_ptr mode
);
static uint_32 set_cipher
(
    uint_32 dev_num,
   char_ptr u_cipher, char_ptr m_cipher         
);

uint_32 set_wep_keys 
    (
        uint_32 dev_num,
        char_ptr wep_key1,
        char_ptr wep_key2,
        char_ptr wep_key3,
        char_ptr wep_key4,
        uint_32 key_len,
        uint_32 key_index
        
    );
static int_32 set_callback();
    

static void Telnetd_shell_fn (pointer dummy) 
{  
   Shell(Telnet_commands,NULL);
}

const RTCS_TASK Telnetd_shell_template = {"Telnet_shell", 8, 2000, Telnetd_shell_fn, NULL};

void HVAC_initialize_networking(void) 
{
   int_32            error;
    IPCFG_IP_ADDRESS_DATA   ip_data;

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
   FTPd_buffer_size = 536;

    error = RTCS_create();

    IPCFG_default_enet_device = DEMOCFG_DEFAULT_DEVICE;
    IPCFG_default_ip_address = ENET_IPADDR;
    IPCFG_default_ip_mask = ENET_IPMASK;
    IPCFG_default_ip_gateway = ENET_IPGATEWAY;
    LWDNS_server_ipaddr = ENET_IPGATEWAY;

    ip_data.ip = IPCFG_default_ip_address;
    ip_data.mask = IPCFG_default_ip_mask;
    ip_data.gateway = IPCFG_default_ip_gateway;
    
    ENET_get_mac_address (IPCFG_default_enet_device, IPCFG_default_ip_address, IPCFG_default_enet_address);
    error = ipcfg_init_device (IPCFG_default_enet_device, IPCFG_default_enet_address);
	if((error = ENET_initialize (IPCFG_default_enet_device, IPCFG_default_enet_address, 0, &handle)) != ENETERR_INITIALIZED_DEVICE)
    {
    	while(1)
    	{
    		
    	}
    }
    
#if DEMOCFG_USE_WIFI
	set_callback();
    iwcfg_set_essid (IPCFG_default_enet_device,DEMOCFG_SSID);
    if ((strcmp(DEMOCFG_SECURITY,"wpa") == 0)||strcmp(DEMOCFG_SECURITY,"wpa2") == 0)
    {
        iwcfg_set_passphrase (IPCFG_default_enet_device,DEMOCFG_PASSPHRASE);
        set_cipher(IPCFG_default_enet_device,DEMOCFG_DEFAULT_U_CIPHER,DEMOCFG_DEFAULT_M_CIPHER);

    }

    if (strcmp(DEMOCFG_SECURITY,"wep") == 0)
    {
      	set_wep_keys (IPCFG_default_enet_device,DEMOCFG_WEP_KEY,
      	NULL,NULL,NULL,strlen(DEMOCFG_WEP_KEY),DEMOCFG_WEP_KEY_INDEX);
      	error = set_sec_mode(DEMOCFG_DEFAULT_DEVICE,DEMOCFG_DEFAULT_WEP_MODE);
	    if(error != SHELL_EXIT_SUCCESS)
	    {
	    	printf("Unable to set WEP mode\n");
	    	while(1)
	    	{
	    		
	    	};
	    }
    }

    iwcfg_set_sec_type (IPCFG_default_enet_device,DEMOCFG_SECURITY);
    iwcfg_set_mode (IPCFG_default_enet_device,DEMOCFG_NW_MODE);
    iwcfg_commit(IPCFG_default_enet_device);
#endif  
    error = ipcfg_bind_staticip (IPCFG_default_enet_device, &ip_data);

#if DEMOCFG_ENABLE_FTP_SERVER
   FTPd_init("FTP_server", 7, 3000 );
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



/*FUNCTION*--------------------------------------------------------------------
*
* Function Name   : set_wep_key()
* Returned Value  : ENET_OK if mode set successfull else ERROR CODE
* Comments        : Sets the Mode for Wifi Device. 
*
*END*------------------------------------------------------------------------*/

uint_32 set_wep_keys 
    (
        uint_32 dev_num,
        char_ptr wep_key1,
        char_ptr wep_key2,
        char_ptr wep_key3,
        char_ptr wep_key4,
        uint_32 key_len,
        uint_32 key_index
        
    )
{
    uint_32 error,dev_status;
    uint_32 flags = 0;
    //ENET_MEDIACTL_PARAM  inout_param;
    ENET_WEPKEYS wep_param;

   if (dev_num < IPCFG_DEVICE_COUNT)
    {
       
        error = ENET_mediactl (handle,ENET_MEDIACTL_IS_INITIALIZED,&dev_status);
        if (ENET_OK != error)
        {
            return error;
        }
        if (dev_status == FALSE)
        {
            return ENET_ERROR;
        }
     
        wep_param.defKeyIndex = key_index;
        wep_param.numKeys = 0;
        wep_param.keyLength = key_len;
        
        do{
	        if(wep_key1 == NULL){
	        	break;
	        }
	        
	        wep_param.keys[0] = wep_key1;
	        wep_param.numKeys++;
	        
	        if(wep_key2 == NULL){
	        	break;
	        }
	        
	        wep_param.keys[1] = wep_key2;
	        wep_param.numKeys++;
	        
	        if(wep_key3 == NULL){
	        	break;
	        }
	        
	        wep_param.keys[2] = wep_key3;
	        wep_param.numKeys++;
	        
	        if(wep_key4 == NULL){
	        	break;
	        }
	        
	        wep_param.keys[3] = wep_key4;
	        wep_param.numKeys++;
	        
		}while(0);	        
                        
        error = ENET_mediactl (handle,ENET_SET_MEDIACTL_ENCODE,&wep_param);
        if (ENET_OK != error)
        {
            return error;
        }
        return ENET_OK;
    }   
    return ENETERR_INVALID_DEVICE;  
}














/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : set_cipher()
* Returned Value  : SHELL_EXIT_SUCCESS if ESSID set successfully else ERROR CODE
* Comments        : Sets the Unicast and multicast cipher
*
*END*-----------------------------------------------------------------*/

static uint_32 set_cipher
    (
        uint_32 dev_num,
       char_ptr u_cipher, char_ptr m_cipher         
    )
{

    ATH_IOCTL_PARAM_STRUCT  inout_param;
    uint_32 error,dev_status;
 
 
 	/**************** ucipher **********/

	if(strcmp(u_cipher,"TKIP")==0)
	{
		cipher.ucipher = 0x04;
	}
	else if(strcmp(u_cipher,"CCMP")==0)
	{
		cipher.ucipher = 0x08;
	}
	else
	{
		printf("Invalid ucipher\n");
		return SHELL_EXIT_ERROR;
	}		    	
	
	
	/*********** mcipher ************/

	if(strcmp(m_cipher,"TKIP")==0)
	{
		cipher.mcipher = 0x04;
	}
	else if(strcmp(m_cipher,"CCMP")==0)
	{
		cipher.mcipher = 0x08;
	}
	else
	{
		printf("Invalid mcipher\n");
		return SHELL_EXIT_ERROR;
	}	
	
	
    inout_param.cmd_id = ATH_SET_CIPHER;
    inout_param.data = &cipher;
    inout_param.length = 8;
    error = ENET_mediactl (handle,ENET_MEDIACTL_IS_INITIALIZED,&dev_status);
    if (SHELL_EXIT_SUCCESS != error)
    {
        return error;
    }
    if (dev_status == FALSE)
    {
        return ENET_ERROR;
    }

    error = ENET_mediactl (handle,ENET_MEDIACTL_VENDOR_SPECIFIC,&inout_param);
    if (SHELL_EXIT_SUCCESS != error)
    {
        return error;
    }
    return SHELL_EXIT_SUCCESS;
}





/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : set_sec_mode()
* Returned Value  : SHELL_EXIT_SUCCESS if mode set successfully else ERROR CODE
* Comments        : Sets the mode. 
*
*END*-----------------------------------------------------------------*/

static uint_32 set_sec_mode
    (
        uint_32 dev_num,
        char_ptr mode
    )
{
    uint_32 error,dev_status;
    ATH_IOCTL_PARAM_STRUCT param;
    
    if (dev_num < IPCFG_DEVICE_COUNT)
    {       
        param.cmd_id = ATH_SET_SEC_MODE;     
        param.data = mode;
        param.length = strlen(mode);
        error = ENET_mediactl (handle,ENET_MEDIACTL_IS_INITIALIZED,&dev_status);
        if (SHELL_EXIT_SUCCESS != error)
        {
            return error;
        }
        if (dev_status == FALSE)
        {
            return ENET_ERROR;
        }
        
        error = ENET_mediactl (handle,ENET_MEDIACTL_VENDOR_SPECIFIC,&param);
        if (SHELL_EXIT_SUCCESS != error)
        {
            return error;
        }
        return SHELL_EXIT_SUCCESS;
    }
    printf("IWCONFIG_ERROR: Invalid Device number\n");
    return ENETERR_INVALID_DEVICE;
}



static int_32 set_callback()
{
     ATH_IOCTL_PARAM_STRUCT  inout_param;
    uint_32 error,dev_status;
 
    inout_param.cmd_id = ATH_SET_CONNECT_STATE_CALLBACK;
    inout_param.data = (void *)WifiConnected;
    inout_param.length = 4;
    error = ENET_mediactl (handle,ENET_MEDIACTL_IS_INITIALIZED,&dev_status);
    if (SHELL_EXIT_SUCCESS != error)
    {
        return error;
    }
    if (dev_status == FALSE)
    {
        return ENET_ERROR;
    }

    error = ENET_mediactl (handle,ENET_MEDIACTL_VENDOR_SPECIFIC,&inout_param);
    if (SHELL_EXIT_SUCCESS != error)
    {
        return error;
    }
    return SHELL_EXIT_SUCCESS;
}


/* EOF */
