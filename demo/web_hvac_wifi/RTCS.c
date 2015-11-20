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
* $Version : 3.5.34.0$
* $Date    : Jan-6-2010$
*
* Comments:
*
*   Example of HVAC using RTCS.
*
*END************************************************************************/

#include "HVAC.h"
#include "httpd_types.h"
#include "tfs.h"
#include "hvac_public.h"
#include "atheros_wifi_api.h"

#define SPIN_FOREVER do{}while(1)

#if DEMOCFG_USE_WIFI
#include "string.h"
#endif

#if DEMOCFG_ENABLE_RTCS


#if DEMOCFG_USE_WPS
typedef struct{
	char ssid[33];
	//TODO: need more here
}WPS_NETPARAMS;

static uint_32 GetNetworkParams(ATH_NETPARAMS *pNetparams);
static uint_32 StartWPS(ATH_NETPARAMS *pNetparams, uint_8 use_pinmode);
static uint_32 SaveNetworkParams(ATH_NETPARAMS *pNetparams);
static uint_32 ClearNetworkParams(void);
static uint_32 WaitUserWPSTrigger(void);
static uint_32 TestUserClearNetworkParams(void);

boolean HVAC_GetInput(HVAC_Input_t signal);

#endif


#include  <ipcfg.h>

#if (DEMOCFG_ENABLE_FTP_SERVER+DEMOCFG_ENABLE_TELNET_SERVER+DEMOCFG_ENABLE_WEBSERVER) != 1
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

const HTTPD_ROOT_DIR_STRUCT root_dir[] = {
    { "", "tfs:" },
    { "usb", "c:" },
    { 0, 0 }
};


void HVAC_initialize_networking(void) 
{
    int_32                  error;
    IPCFG_IP_ADDRESS_DATA   ip_data;
#if DEMOCFG_USE_WPS
	ATH_NETPARAMS netparams;
	ATH_IOCTL_PARAM_STRUCT ath_param;
#endif    

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
    /* ENET_initialize is already called from within ipcfg_init_device.  We call it again
     * so that we can get a handle to the driver allowing this code to make direct driver
     * calls as needed. */
    if((error = ENET_initialize (IPCFG_default_enet_device, IPCFG_default_enet_address, 0, &handle)) != ENETERR_INITIALIZED_DEVICE)
    {
    	SPIN_FOREVER;
    }
    
#if DEMOCFG_USE_WIFI
	set_callback();
	
#if DEMOCFG_USE_WPS
	if(TestUserClearNetworkParams()){
		ClearNetworkParams();
	}

	if(GetNetworkParams(&netparams)==0){
		while(1){
			if(WaitUserWPSTrigger()){
#if DEMOCFG_USE_WPS_PUSHBUTTON			
				if(StartWPS(&netparams, 0)){
#else
				if(StartWPS(&netparams, 1)){
#endif				
					SaveNetworkParams(&netparams);
					GetNetworkParams(&netparams);
					break;
				}else{
					/* somehow indicate to user that WPS failed 
					 * and must be retried. Perhaps an LED blink or something */
				}
			}
		}
	}

	iwcfg_set_essid (IPCFG_default_enet_device, (char*)netparams.ssid);
	if(netparams.sec_type == ENET_MEDIACTL_SECURITY_TYPE_WPA2){
		iwcfg_set_sec_type (IPCFG_default_enet_device,"wpa2"); 
		iwcfg_set_passphrase (IPCFG_default_enet_device,(char_ptr)netparams.u.passphrase);
        ath_param.cmd_id = ATH_SET_CIPHER;
    	ath_param.data = &netparams.cipher;
    	ath_param.length = 8;        
        
        if(ENET_OK != ENET_mediactl (handle,ENET_MEDIACTL_VENDOR_SPECIFIC,&ath_param)){
        	SPIN_FOREVER;
        }
	}else if (netparams.sec_type == ENET_MEDIACTL_SECURITY_TYPE_WPA){
		iwcfg_set_sec_type (IPCFG_default_enet_device,"wpa");
		iwcfg_set_passphrase (IPCFG_default_enet_device,(char_ptr)netparams.u.passphrase);
        ath_param.cmd_id = ATH_SET_CIPHER;
    	ath_param.data = &netparams.cipher;
    	ath_param.length = 8;        
        
        if(ENET_OK != ENET_mediactl (handle,ENET_MEDIACTL_VENDOR_SPECIFIC,&ath_param)){
        	SPIN_FOREVER;
        }
	}else if (netparams.sec_type == ENET_MEDIACTL_SECURITY_TYPE_WEP){
		iwcfg_set_sec_type (IPCFG_default_enet_device,"wep");
		set_wep_keys (IPCFG_default_enet_device,(char_ptr)netparams.u.wepkey,
      	NULL,NULL,NULL,strlen((char_ptr)netparams.u.wepkey),netparams.key_index);
	}else if (netparams.sec_type == ENET_MEDIACTL_SECURITY_TYPE_NONE){
		iwcfg_set_sec_type (IPCFG_default_enet_device,"none");
	}
		
	iwcfg_set_mode (IPCFG_default_enet_device,"managed");
    iwcfg_commit(IPCFG_default_enet_device);
#else /* DEMOCFG_USE_WPS */
	/* hardcoded values for SSID and security params are used to try to connect. This
	 * is fine for a demo but not very practicle for a "real world" product. Instead
	 * one option is to use WPS as shown above. */
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
	    	SPIN_FOREVER;
	    }
    }

    iwcfg_set_sec_type (IPCFG_default_enet_device,DEMOCFG_SECURITY);
    iwcfg_set_mode (IPCFG_default_enet_device,DEMOCFG_NW_MODE);
    iwcfg_commit(IPCFG_default_enet_device);
#endif /* DEMOCFG_USE_WPS */    
#endif /* DEMOCFG_USE_WIFI */
    error = ipcfg_bind_staticip (IPCFG_default_enet_device, &ip_data);

#if DEMOCFG_ENABLE_WEBSERVER
    {
        HTTPD_STRUCT *server;
        extern const HTTPD_CGI_LINK_STRUCT cgi_lnk_tbl[];
        extern const HTTPD_FN_LINK_STRUCT fn_lnk_tbl[];
        extern const TFS_DIR_ENTRY tfs_data[];
        
        if ((error = _io_tfs_install("tfs:", tfs_data))) {
            printf("\ninstall returned: %08x\n", error);
        }

        server = httpd_server_init((HTTPD_ROOT_DIR_STRUCT*)root_dir, "\\mqx.html");        
      
        HTTPD_SET_PARAM_CGI_TBL(server, (HTTPD_CGI_LINK_STRUCT*)cgi_lnk_tbl);
        HTTPD_SET_PARAM_FN_TBL(server, (HTTPD_FN_LINK_STRUCT*)fn_lnk_tbl);
        
        httpd_server_run(server);
    }
#endif
#if DEMOCFG_ENABLE_FTP_SERVER
   FTPd_init("FTP_server", 7, 3000 );
#endif

#if DEMOCFG_ENABLE_TELNET_SERVER
   TELNETSRV_init("Telnet_server", 7, 2000, (RTCS_TASK_PTR) &Telnetd_shell_template );
#endif

#if DEMOCFG_ENABLE_KLOG && MQX_KERNEL_LOGGING
   RTCSLOG_enable(RTCSLOG_TYPE_FNENTRY);
   RTCSLOG_enable(RTCSLOG_TYPE_PCB);
#endif

}

#else /* DEMOCFG_ENABLE_RTCS */

#if (DEMOCFG_ENABLE_FTP_SERVER+DEMOCFG_ENABLE_TELNET_SERVER+DEMOCFG_ENABLE_WEBSERVER) > 0
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
    if (ENET_OK != error)
    {
        return error;
    }
    if (dev_status == FALSE)
    {
        return ENET_ERROR;
    }

    error = ENET_mediactl (handle,ENET_MEDIACTL_VENDOR_SPECIFIC,&inout_param);
    if (ENET_OK != error)
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


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : set_callback()
* Returned Value  : SHELL_EXIT_SUCCESS if mode set successfully else ERROR CODE
* Comments        : Sets the Atheros wifi driver callback function which
*					the driver will call when the state of the wifi 
*					connection changes. 
*
*END*-----------------------------------------------------------------*/
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

#if DEMOCFG_USE_WPS

#define SENTINAL ((uint_32)0xa55a5768)
#define CLEAR_SENTINAL ((uint_32)0xffffffff)

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : GetNetworkParams()
* Returned Value  : 1 - successful completion or 
*					0 - failed.
* Comments		  : Attempts to retrieve network parameters from some 
*					storage location.  upon successful acquisition the 
*					params are returned to the caller in the pNetParams
*					pointer memory.
*
*END*-----------------------------------------------------------------*/
static uint_32 GetNetworkParams(ATH_NETPARAMS *pNetparams)
{
	FLASHX_BLOCK_INFO_STRUCT_PTR info_ptr;
    MQX_FILE_PTR   flash_hdl;
    _mqx_uint      error_code;
    _mqx_int       numbytes;
    uint_32		   status = 0; //init as failed.
    uint_8		   flash_buffer[sizeof(SENTINAL) + sizeof(ATH_NETPARAMS)];
    _mem_size      seek_location;
	/* NOTE: Uses flashx feature to store network params */
	
	do{
		flash_hdl = fopen("flashx:", NULL);
		error_code = ioctl(flash_hdl, FLASH_IOCTL_GET_BLOCK_MAP, (uint_32 _PTR_)&info_ptr);      
		
		if(error_code != MQX_OK){
			break;
		}
		
		seek_location = info_ptr[0].START_ADDR;
		fseek(flash_hdl, seek_location, IO_SEEK_SET);
   		numbytes = read(flash_hdl, &flash_buffer, sizeof(SENTINAL) + sizeof(ATH_NETPARAMS));
	
		if(numbytes==sizeof(SENTINAL) + sizeof(ATH_NETPARAMS)){
		
			if(*((uint_32*)flash_buffer) == SENTINAL){			
				memcpy(pNetparams, &flash_buffer[sizeof(SENTINAL)], sizeof(ATH_NETPARAMS));
				/* success */
				status = 1;
			}
		}			
	}while(0);
	
	fclose(flash_hdl);
	return status;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : SaveNetworkParams()
* Returned Value  : 1 - successful completion or 
*					0 - failed.
* Comments		  : Attempts to save the network params to non-volatile
*					storage.  This storage will later be used by 
*					GetNetworkParams() to retrieve the params.
*
*END*-----------------------------------------------------------------*/
static uint_32 SaveNetworkParams(ATH_NETPARAMS *pNetparams)
{
	FLASHX_BLOCK_INFO_STRUCT_PTR info_ptr;
    MQX_FILE_PTR   flash_hdl;
    _mqx_uint      error_code;
    _mqx_int       numbytes;
    uint_32		   status = 0; //init as failed.
    uint_8		   flash_buffer[sizeof(SENTINAL) + sizeof(ATH_NETPARAMS)];
    _mem_size      seek_location;
    /* NOTE: Uses flashx feature to store network params */
	do{
		flash_hdl = fopen("flashx:", NULL);
		error_code = ioctl(flash_hdl, FLASH_IOCTL_GET_BLOCK_MAP, (uint_32 _PTR_)&info_ptr);      
		
		if(error_code != MQX_OK){
			break;
		}
		
		*((uint_32*)&flash_buffer[0]) = SENTINAL;
		memcpy(&flash_buffer[sizeof(SENTINAL)], pNetparams, sizeof(ATH_NETPARAMS));
		seek_location = info_ptr[0].START_ADDR;
		fseek(flash_hdl, seek_location, IO_SEEK_SET);
   		numbytes = write(flash_hdl, &flash_buffer, sizeof(SENTINAL) + sizeof(ATH_NETPARAMS) );
   	
   		if(numbytes == sizeof(SENTINAL) + sizeof(ATH_NETPARAMS)){
   			/* success */
   			status = 1;
   		}
   	}while(0);
   	
   	fclose(flash_hdl);
   	return status;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ClearNetworkParams()
* Returned Value  : 1 - successful completion or 
*					0 - failed.
* Comments		  : Clears the SENTINAL value from flash so that
*					GetNetworkParams will fail to find any old params. 
*
*END*-----------------------------------------------------------------*/
static uint_32 ClearNetworkParams(void)
{
	//FLASHX_BLOCK_INFO_STRUCT_PTR info_ptr;
    MQX_FILE_PTR   flash_hdl;
    _mqx_uint      error_code;
    //_mqx_int       numbytes;
    uint_32		   status = 0; //init as failed.
   // uint_8		   flash_buffer[sizeof(CLEAR_SENTINAL)];
    //_mem_size      seek_location;
    /* NOTE: Uses flashx feature to store network params */
	do{
		flash_hdl = fopen("flashx:", NULL);
		error_code = _io_ioctl(flash_hdl, FLASH_IOCTL_ERASE_CHIP, NULL);
		if(error_code != MQX_OK){
			break;
		}			
   		
   		status = 1;   		
   	}while(0);
   	
   	fclose(flash_hdl);
   	return status;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : StartWPS()
* Returned Value  : 1 - successful completion or 
*					0 - failed.
* Comments		  : Attempts to start a WPS connection with a peer.
*					The caller must specify the type of WPS to start
*					push-button mode | pin mode.  if pin mode is used
*					the caller must specify the pin value to pass to
*					the Atheros driver.
*
*END*-----------------------------------------------------------------*/
static uint_32 StartWPS(ATH_NETPARAMS *pNetparams, uint_8 use_pinmode)
{
#define WPS_STANDARD_TIMEOUT (30)
	uint_32 error,dev_status;
    ATH_IOCTL_PARAM_STRUCT param;
    uint_32 status = 0;
    ATH_WPS_START wps_start;
    
    do{              
        error = ENET_mediactl (handle,ENET_MEDIACTL_IS_INITIALIZED,&dev_status);
        if (ENET_OK != error || dev_status == FALSE)
        {
            break;
        }       
        
        param.cmd_id = ATH_START_WPS;  
        param.data = &wps_start;
        param.length = sizeof(wps_start);
        
        if(use_pinmode){
        	wps_start.wps_mode = ATH_WPS_MODE_PIN;
        	//FIXME: This hardcoded pin value needs to be changed 
        	// for production to reflect what is on a sticker/label
        	memcpy(wps_start.pin, "22916501",ATH_WPS_PIN_LEN);
        }else{
        	wps_start.wps_mode = ATH_WPS_MODE_PUSHBUTTON;
        }
        
        wps_start.timeout_seconds = WPS_STANDARD_TIMEOUT;    	                           
        
        /* this starts WPS on the Aheros wifi */
        error = ENET_mediactl (handle,ENET_MEDIACTL_VENDOR_SPECIFIC,&param);
        if (ENET_OK != error)
        {
            break;
        }
        /* set dont_block = 0 so that the driver will return only when the 
         * wps operation has completed. */
        pNetparams->dont_block = 0;
        param.cmd_id = ATH_AWAIT_WPS_COMPLETION;  
        param.data = pNetparams;
        param.length = sizeof(*pNetparams);
         
        /* this will block until the WPS operation completes or times out */
        error = ENET_mediactl (handle,ENET_MEDIACTL_VENDOR_SPECIFIC,&param);
        if (ENET_OK != error)
        {
            break;
        }
        
        
        status = 1;
    }while(0);
    
    return status;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : WaitUserWPSTrigger()
* Returned Value  : 1 - successful completion or 
*					0 - failed.
* Comments		  : WPS requires a synchronization by the user between
*					the 'Registrar' typically an Access point and the 
*					'Enrollee' which is this device. This synchronization 
*					consists of activating WPS on the AP and in this case
*					pressing the designated button on the MCU board.
*
*END*-----------------------------------------------------------------*/
static uint_32 WaitUserWPSTrigger(void)
{
	while(HVAC_GetInput(HVAC_TEMP_UP_INPUT)==FALSE);
	
	return 1;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : TestUserClearNetworkParams()
* Returned Value  : 1 - successful completion or 
*					0 - failed.
* Comments		  : The user may want to reset the network params. This 
*					function detects button presses to determine if the 
*					User wishes for the device to clear its state.
*
*END*-----------------------------------------------------------------*/
static uint_32 TestUserClearNetworkParams(void)
{
	/* both buttons are pressed at the same time which tells the device
	 * the user wants to clear the network params from flash */
	if(HVAC_GetInput(HVAC_TEMP_UP_INPUT)==TRUE &&
		HVAC_GetInput(HVAC_TEMP_DOWN_INPUT)==TRUE){
		/* wait for user to let go of one button before continuing */
		while(HVAC_GetInput(HVAC_TEMP_UP_INPUT) == TRUE);
		
		return 1;	
	}
	
	return 0;
}

#endif /* DEMOCFG_USE_WPS */




/* EOF */
