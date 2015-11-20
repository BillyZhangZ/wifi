/**HEADER********************************************************************
 * 
 * Copyright (c) 2014 Freescale Semiconductor;
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
 * Comments:
 *
 *END************************************************************************/

/**************************************************************************
 **************************************************************************/
#include <mqx.h>
#include <bsp.h>

#include <a_config.h>
#include <a_osapi.h>
#include <a_types.h>
#include <driver_cxt.h>
#include <common_api.h>
#include <custom_api.h>
#include <bmi_msg.h>
#include <targaddrs.h>
#include "bmi.h"
#include "atheros_wifi_api.h"
#include "atheros_wifi_internal.h"     
#include "hw/apb_map.h"
#include "hw/mbox_reg.h"
#include "hw4.0/hw/rtc_reg.h"

#include <lwevent.h>
#include <message.h>
#include <a_types.h>
#include "athstartpack.h"

#include "enet.h"
#include "enetprv.h"

#if MQX_KERNEL_LOGGING
#include <klog.h>
#endif

#define DEMOCFG_ENABLE_SERIAL_SHELL    0   /* enable shell task for serial console */
#define DEMOCFG_ENABLE_RTCS            1   /* enable RTCS operation */
#define DEMOCFG_ENABLE_FTP_SERVER      0   /* enable ftp server */
#define DEMOCFG_ENABLE_TELNET_SERVER   0   /* enable telnet server */
#define DEMOCFG_ENABLE_KLOG            0   /* enable kernel logging */
#define DEMOCFG_USE_POOLS              0   /* enable external memory pools for USB and RTCS */
#define DEMOCFG_USE_WIFI               1  /* USE WIFI Interface */
/* default addresses for external memory pools and klog */
#if BSP_M52259EVB
    #define DEMOCFG_RTCS_POOL_ADDR  (uint_32)(BSP_EXTERNAL_MRAM_RAM_BASE)
    #define DEMOCFG_RTCS_POOL_SIZE  0x0000A000
    #define DEMOCFG_MFS_POOL_ADDR   (uint_32)(DEMOCFG_RTCS_POOL_ADDR + DEMOCFG_RTCS_POOL_SIZE)
    #define DEMOCFG_MFS_POOL_SIZE   0x00002000
    #define DEMOCFG_KLOG_ADDR       (uint_32)(DEMOCFG_MFS_POOL_ADDR + DEMOCFG_MFS_POOL_SIZE)
    #define DEMOCFG_KLOG_SIZE       4000
#elif DEMOCFG_USE_POOLS
    #warning External pools will not be used on this board.
#endif

#if DEMOCFG_ENABLE_RTCS
   #include <rtcs.h>
   #include <ftpc.h> 
   //#include <ftpd.h> 
   

   #ifndef ENET_IPADDR
   //   #define ENET_IPADDR  IPADDR(15,15,103,25) 
     #define ENET_IPADDR  IPADDR(192,168,1,90)
   #endif

   #ifndef ENET_IPMASK
      #define ENET_IPMASK  IPADDR(255,255,255,0) 
   #endif

   #ifndef ENET_IPGATEWAY
    //  #define ENET_IPGATEWAY  IPADDR(15,15,103,10) 
     #define ENET_IPGATEWAY  IPADDR(192,168,1,1)
   #endif
#endif
#if DEMOCFG_USE_WIFI
   #include "iwcfg.h"
   
   #define DEMOCFG_SSID            "iot"
   //Possible Values managed or adhoc
   #define DEMOCFG_NW_MODE         "managed" 
   //Possible vales 
   // 1. "wep"
   // 2. "wpa"
   // 3. "wpa2"
   // 4. "none"
   #define DEMOCFG_SECURITY        "none"
   #define DEMOCFG_PASSPHRASE      NULL
   #define DEMOCFG_WEP_KEY         "abcde"
   //Possible values 1,2,3,4
   #define DEMOCFG_WEP_KEY_INDEX   1
   
#endif
#define DEMOCFG_DEFAULT_DEVICE  0  //vivi
#ifndef DEMOCFG_DEFAULT_DEVICE
   #define DEMOCFG_DEFAULT_DEVICE   BSP_DEFAULT_ENET_DEVICE
#endif
   


#include <shell.h>
#define VERSION (4)

/******************************************/
/* firmware input param value definitions */
/******************************************/
/* IOTFLASHOTP_PARAM_SKIP_OTP - instructs firmware to skip OTP operations */
#define IOTFLASHOTP_PARAM_SKIP_OTP                  (0x00000001)
/* IOTFLASHOTP_PARAM_SKIP_FLASH - instructs firmware to skip Flash operations */
#define IOTFLASHOTP_PARAM_SKIP_FLASH                (0x00000002)
/* IOTFLASHOTP_PARAM_USE_NVRAM_CONFIG_FROM_OTP - instructs firmware to use NVRAM config found in OTP
 *  to access flash. */
#define IOTFLASHOTP_PARAM_USE_NVRAM_CONFIG_FROM_OTP (0x00000004)
/* IOTFLASHOTP_PARAM_FORCE_FLASH_REWRITE - instructs firmware to ignore any build_number protection
 * when deciding whether or not to re-write the flash. normally, firmware does not re-write flash
 * if the build_number of the new image is less then the build_number of the image currently in 
 * flash. */
#define IOTFLASHOTP_PARAM_FORCE_FLASH_REWRITE       (0x00000008)
/* IOTFLASHOTP_PARAM_FORCE_MAC_OVERRIDE - instructs firmware to ignore any existing mac address 
 * in OTP and instead write the new mac address to otp. normally, if any mac address is found 
 * in OTP the firmware will avoid writing a new mac address to OTP. */
#define IOTFLASHOTP_PARAM_FORCE_MAC_OVERRIDE        (0x00000010)
/*************************************/
/* firmware return value definitions */
/*************************************/
#define IOTFLASHOTP_RESULT_OTP_SUCCESS              (0x00000001)
#define IOTFLASHOTP_RESULT_OTP_FAILED               (0x00000002)
#define IOTFLASHOTP_RESULT_OTP_NOT_WRITTEN          (0x00000004)
#define IOTFLASHOTP_RESULT_OTP_SKIPPED              (0x00000008)
#define IOTFLASHOTP_RESULT_OTP_POS_MASK             (0x0000000f)
#define IOTFLASHOTP_RESULT_OTP_POS_SHIFT            (8)
#define IOTFLASHOTP_RESULT_OTP_MASK                 (0x0000ffff)

#define IOTFLASHOTP_RESULT_FLASH_SUCCESS            (0x00010000)
#define IOTFLASHOTP_RESULT_FLASH_FAILED             (0x00020000)
#define IOTFLASHOTP_RESULT_FLASH_VALIDATE_FAILED    (0x00040000)
#define IOTFLASHOTP_RESULT_FLASH_SKIPPED            (0x00080000)
#define IOTFLASHOTP_RESULT_FLASH_OLDER_VERSION      (0x00100000) /* flash was not written because a newer version is already present. */
#define IOTFLASHOTP_RESULT_FLASH_MASK               (0xffff0000)



#define bmifn(fn) do { \
    if ((fn) < A_OK) { \
        return A_ERROR; \
    } \
} while(0)

enum task{
   FLASH_AGENT_TASK = 1

};

#pragma pack(1)

typedef struct _version {
    unsigned int        host_ver;
    unsigned int        target_ver;
    unsigned int        wlan_ver;
    unsigned int        abi_ver;
}version_t;

typedef struct _request{
	unsigned short cmdId FIELD_PACKED; 
	unsigned char buffer[1024] FIELD_PACKED;
	unsigned int addr FIELD_PACKED;
	unsigned int option FIELD_PACKED;
	unsigned short length FIELD_PACKED;		
	unsigned short crc FIELD_PACKED;
}  request_t;


typedef struct _response{
	unsigned short cmdId FIELD_PACKED;
	unsigned int result FIELD_PACKED;
	version_t version FIELD_PACKED;
} response_t;
#pragma pack(4)

enum cmd_id{
	PROGRAM = 0,
	PROGRAM_RESULT,
	EXECUTE,
	EXECUTE_RESULT,
	CRC_ERROR,
	VERSION_NUM,
	FW_VERSION,
	UNKNOWN_CMD
};

void Flash_Agent_Task(uint_32);
void WifiConnected(int val);
void reset_atheros_driver_setup_init(void);


#include "atheros_wifi_api.h"


enum ath_private_ioctl_cmd
{
	ATH_REG_QUERY=ATH_CMD_LAST,
	ATH_MEM_QUERY
};



typedef struct
{
	uint_32 address;
	uint_32 value;
	uint_32 mask;
	uint_32 size;
	uint_32 operation;
}ATH_REGQUERY;

typedef struct
{
	uint_32 literals;
	uint_32 rodata;
	uint_32 data;
	uint_32 bss;
	uint_32 text;
	uint_32 heap;
}ATH_MEMQUERY;

#define ATH_REG_OP_READ 			(1)
#define ATH_REG_OP_WRITE 			(2)
#define ATH_REG_OP_RMW 				(3)
#define MAX_ACK_RETRY  			   (20)	 //Max number of times ACK issent to Peer with stats
#define MAX_SSID_LENGTH 		 (32+1)

#define MIN_WEP_KEY_SIZE			10
#define MAX_WEP_KEY_SIZE			26
#define MAX_PASSPHRASE_SIZE         64
#define MAX_NUM_WEP_KEYS             4
#define MAX_WPS_PIN_SIZE            32

#define DRIVER_UP               (0)
#define DRIVER_DOWN             (1)
#define READ_STATUS             (2)

/* Security modes */
#define SEC_MODE_OPEN 			(0)
#define SEC_MODE_WEP 			(1)
#define SEC_MODE_WPA 			(2)

#define getByte(n, data) ((data>>(8*n))&0xFF)

#if 0
enum ath_private_ioctl_cmd
{
	ATH_PROGRAM_FLASH=ATH_CMD_LAST,			/*input- buffer, length, start address*/
	ATH_EXECUTE_FLASH				/*input- execute address*/	
};




typedef struct ath_program_flash
{
	uint_8* buffer;
	uint_32 load_addr;
	uint_16 length;
	uint_32 result;	
} ATH_PROGRAM_FLASH_STRUCT;
#endif

void atheros_driver_setup_mediactl(void);
void atheros_driver_setup_init(void);


/**************************************************************************
  Local header files for this application
 **************************************************************************/

/**************************************************************************
  Global variables
 **************************************************************************/

#if DEMOCFG_USE_WIFI
#include "string.h"
#endif

#if DEMOCFG_ENABLE_RTCS

#include <ipcfg.h>

_enet_handle    handle;

static void HVAC_initialize_networking(void) 
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
#if DEMOCFG_ENABLE_FTPD
   FTPd_buffer_size = 536;
#endif

    error = RTCS_create();

    LWDNS_server_ipaddr = ENET_IPGATEWAY;

    ip_data.ip = ENET_IPADDR;
    ip_data.mask = ENET_IPMASK;
    ip_data.gateway = ENET_IPGATEWAY;
    
    ENET_get_mac_address (DEMOCFG_DEFAULT_DEVICE, ENET_IPADDR, enet_address);
    error = ipcfg_init_device (DEMOCFG_DEFAULT_DEVICE, enet_address);
    
     if((error = ENET_initialize(DEMOCFG_DEFAULT_DEVICE, enet_address, 0, &handle)) != ENETERR_INITIALIZED_DEVICE)
    {
        printf("QCA4002 wifi module init failed! \n");
        
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

   // error = ipcfg_bind_staticip (IPCFG_default_enet_device, &ip_data);
}

#else /* DEMOCFG_ENABLE_RTCS */

#if (DEMOCFG_ENABLE_FTP_SERVER+DEMOCFG_ENABLE_TELNET_SERVER) > 0
#warning RTCS network stack is disabled, the selected service will not be available.
#endif

#endif /* DEMOCFG_ENABLE_RTCS */



static A_STATUS Driver_BMIConfig(A_VOID *pCxt)
{        
    /* The config is contained within the driver itself */
    A_UINT32 param, options, sleep, address;
	A_DRIVER_CONTEXT *pDCxt = GET_DRIVER_COMMON(pCxt);
	
	if (pDCxt->targetVersion != ATH_FIRMWARE_TARGET && pDCxt->targetVersion != TARGET_AR400X_REV2) {
		/* compatible wifi chip version is decided at compile time */
		A_ASSERT(0);
		//return A_ERROR;
	}
    /* Temporarily disable system sleep */
    address = MBOX_BASE_ADDRESS + LOCAL_SCRATCH_ADDRESS;
    bmifn(BMIReadSOCRegister(pCxt, address, &param));
    options = A_LE2CPU32(param);
    param |= A_CPU2LE32(AR6K_OPTION_SLEEP_DISABLE);
    bmifn(BMIWriteSOCRegister(pCxt, address, param));

    address = RTC_BASE_ADDRESS + SYSTEM_SLEEP_ADDRESS;
    bmifn(BMIReadSOCRegister(pCxt, address, &param));
    sleep = A_LE2CPU32(param);
    param |= A_CPU2LE32(WLAN_SYSTEM_SLEEP_DISABLE_SET(1));
    bmifn(BMIWriteSOCRegister(pCxt, address, param));        
    
    /* Run at 40/44MHz by default */
    param = CPU_CLOCK_STANDARD_SET(0);
    
    address = RTC_BASE_ADDRESS + CPU_CLOCK_ADDRESS;
    bmifn(BMIWriteSOCRegister(pCxt, address, A_CPU2LE32(param)));
	
	{
        address = RTC_BASE_ADDRESS + LPO_CAL_ADDRESS;
        param = LPO_CAL_ENABLE_SET(1);
        bmifn(BMIWriteSOCRegister(pCxt, address, A_CPU2LE32(param)));
    }
   
      	 
    return A_OK;
}
 
extern A_UINT32 ar4XXX_boot_param; 
pointer pQuery;
A_BOOL reg_query_bool = A_FALSE;

static A_STATUS driver_thread_operation(A_VOID *pCxt)
{
	A_DRIVER_CONTEXT *pDCxt = GET_DRIVER_COMMON(pCxt);    	
 union{    
    	A_UINT32 param;    	
    }stackU;      
    
#define PARAM (stackU.param)                	
#define PTR_REG_MOD		((ATH_REGQUERY*)(pQuery))	
		switch(PTR_REG_MOD->operation)
		{
		case ATH_REG_OP_READ: // read			
			Driver_ReadRegDiag(pCxt, &PTR_REG_MOD->address, &(PTR_REG_MOD->value));									
			PTR_REG_MOD->value = A_CPU2LE32((PTR_REG_MOD->value));
			break;
		case ATH_REG_OP_WRITE: //write-
			PTR_REG_MOD->value = A_CPU2LE32((PTR_REG_MOD->value));
			Driver_WriteRegDiag(pCxt, &PTR_REG_MOD->address, &(PTR_REG_MOD->value));																	
			break;
		case ATH_REG_OP_RMW: //read-mod-write
			if(A_OK != Driver_ReadRegDiag(pCxt, &PTR_REG_MOD->address, &PARAM)){
				break;
			}
			
			PARAM = A_CPU2LE32(PARAM);
			PARAM &= ~PTR_REG_MOD->mask;
			PARAM |= PTR_REG_MOD->value;
			PARAM = A_CPU2LE32(PARAM);
			
			Driver_WriteRegDiag(pCxt, &PTR_REG_MOD->address, &PARAM);						
			break;
		}
	pDCxt->asynchRequest = NULL;
	reg_query_bool = A_TRUE;
	CUSTOM_DRIVER_WAKE_USER(pCxt);
#undef PTR_REG_MOD	
#undef PARAM

	return A_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : ath_ioctl_handler
*  PARAMS         : 
*                   enet_ptr-> ptr to enet context
*                   inout_param -> input/output data for command.
*  Returned Value : ENET_OK or error code
*  Comments       :
*        IOCTL implementation of Atheros Wifi device.
*
*END*-----------------------------------------------------------------*/

static uint_32 ath_ioctl_handler_ext(A_VOID* enet_ptr, ATH_IOCTL_PARAM_STRUCT_PTR param_ptr)
{
	A_VOID* pCxt;
    A_DRIVER_CONTEXT *pDCxt;        
	A_STATUS status = A_OK;
	uint_32 error = ENET_OK;
	ATH_REGQUERY regQuery;
	uint_32 *ptr,i;
   		
	if((pCxt = ((ENET_CONTEXT_STRUCT_PTR)enet_ptr)->MAC_CONTEXT_PTR) == NULL)
	{
	   	return ENET_ERROR;
	}  
	pDCxt = GET_DRIVER_COMMON(pCxt);    	
	
	switch(param_ptr->cmd_id)
	{			
		case ATH_REG_QUERY:
			/* set the operation to be executed by the driver thread */
			pQuery = param_ptr->data;
			reg_query_bool = A_FALSE;
			if(pDCxt->asynchRequest != NULL){
				break;
			}
			pDCxt->asynchRequest = driver_thread_operation;
			/* wake up driver thread */
			CUSTOM_DRIVER_WAKE_DRIVER(pCxt);
			/* wait for driver thread completion */
			Custom_Driver_WaitForCondition(pCxt, &(reg_query_bool), A_TRUE, 5000);  
		
			break;	
		case ATH_MEM_QUERY:
			// read the memory location for stat storage
			regQuery.address = TARG_VTOP(HOST_INTEREST_ITEM_ADDRESS(hi_flash_is_present));
			regQuery.operation = ATH_REG_OP_READ;
			pQuery = &regQuery;
			reg_query_bool = A_FALSE;
			
			if(pDCxt->asynchRequest != NULL){
				break;
			}
			
			pDCxt->asynchRequest = driver_thread_operation;			
			CUSTOM_DRIVER_WAKE_DRIVER(pCxt);			
			Custom_Driver_WaitForCondition(pCxt, &(reg_query_bool), A_TRUE, 5000);
			
			if(reg_query_bool == A_FALSE){
				break;
			}
			//capture the value in regQuery.address
			regQuery.address = TARG_VTOP(regQuery.value);
			ptr = (uint_32*)param_ptr->data;
			
			for(i=0 ; i<5 ; i++){
				reg_query_bool = A_FALSE;
			
				if(pDCxt->asynchRequest != NULL){
					break;
				}
				
				pDCxt->asynchRequest = driver_thread_operation;
				CUSTOM_DRIVER_WAKE_DRIVER(pCxt);
				Custom_Driver_WaitForCondition(pCxt, &(reg_query_bool), A_TRUE, 5000);
			
				if(reg_query_bool == A_FALSE){
					break;
				}
				/* CAUTION: this next line assumes that the stats are stored in 
				 * the same order that they appear in the ATH_MEMQUERY structure. */
				ptr[i] = regQuery.value;
				regQuery.address+=4;
			}
			/* for allocram remaining we query address allocram_remaining_bytes.*/
			regQuery.address = TARG_VTOP(0x541f2c);
			reg_query_bool = A_FALSE;
		
			if(pDCxt->asynchRequest != NULL){
				break;
			}
			
			pDCxt->asynchRequest = driver_thread_operation;
			CUSTOM_DRIVER_WAKE_DRIVER(pCxt);
			Custom_Driver_WaitForCondition(pCxt, &(reg_query_bool), A_TRUE, 5000);
		
			if(reg_query_bool == A_FALSE){
				break;
			}
			/* CAUTION: this next line assumes that the stats are stored in 
			 * the same order that they appear in the ATH_MEMQUERY structure. */
			ptr[5] = regQuery.value;							
			
			break;	
		default:
			error = ENET_ERROR;
			break;
	}
	
	
	return  error;
}
void atheros_driver_setup_init(void)
{
#if 0
	ath_custom_init.Driver_TargetConfig = NULL;	
	ath_custom_init.Driver_BootComm = NULL;
	ath_custom_init.Driver_BMIConfig = Driver_BMIConfig;
	ath_custom_init.skipWmi = A_TRUE;
	ath_custom_init.exitAtBmi = A_TRUE;
	
	ar4XXX_boot_param = AR4XXX_PARAM_MODE_BMI;
#endif
	ath_custom_mediactl.ath_ioctl_handler_ext = ath_ioctl_handler_ext;
}     
 
void reset_atheros_driver_setup_init(void)
{
#if 0
	ath_custom_init.Driver_TargetConfig = NULL;	
	ath_custom_init.Driver_BootComm = NULL;
	ath_custom_init.Driver_BMIConfig = NULL;
	ath_custom_init.skipWmi = A_FALSE;
	ath_custom_init.exitAtBmi = A_FALSE;
	
	ar4XXX_boot_param = AR4XXX_PARAM_MODE_NORMAL;
#endif
}     


static void init()
{

    atheros_driver_setup_init();
    HVAC_initialize_networking();
}


int unit_test_qca4002(void){
    init();
    ENET_shutdown(handle);
    return 0;
}




/* end file */

