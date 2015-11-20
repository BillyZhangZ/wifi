
/**HEADER********************************************************************
 * 
 * Copyright (c) 2015 Freescale Semiconductor;
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


/****************************************************************************
 * Header
 ****************************************************************************/
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
#include "atheros_stack_offload.h"
//#include "hw/apb_map.h"
//#include "hw/mbox_reg.h"
//#include "hw4.0/hw/rtc_reg.h"

#include <lwevent.h>
#include <message.h>
#include <a_types.h>
#include "athstartpack.h"

#include "enet.h"
#include "enetprv.h"

#if MQX_KERNEL_LOGGING
#include <klog.h>
#endif

#include <stdlib.h>

/****************************************************************************
 * Macro
 ****************************************************************************/
#define DEMOCFG_ENABLE_SERIAL_SHELL    0   /* enable shell task for serial console */
#define DEMOCFG_ENABLE_RTCS            1   /* enable RTCS operation */
#define DEMOCFG_ENABLE_FTP_SERVER      0   /* enable ftp server */
#define DEMOCFG_ENABLE_TELNET_SERVER   0   /* enable telnet server */
#define DEMOCFG_ENABLE_KLOG            0   /* enable kernel logging */
#define DEMOCFG_USE_POOLS              0   /* enable external memory pools for USB and RTCS */
#define DEMOCFG_USE_WIFI               1  /* USE WIFI Interface */

#if DEMOCFG_USE_WIFI
#include "string.h"
#endif


#if DEMOCFG_ENABLE_RTCS
#include <rtcs.h>

#ifndef ENET_IPADDR
#define ENET_IPADDR  IPADDR(192,168,12,90)
#endif

#ifndef ENET_IPMASK
#define ENET_IPMASK  IPADDR(255,255,255,0) 
#endif

#ifndef ENET_IPGATEWAY
#define ENET_IPGATEWAY  IPADDR(192,168,12,1)
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

#define IS_DRIVER_READY                         is_driver_initialized()
#define HANDLE_IOCTL(inout_param)               handle_ioctl(inout_param)
#define ATH_COMMIT(device)                      ath_commit(device)
#define ATH_SET_MODE(device,mode)               ath_set_mode(device,mode)
#define ATH_SET_ESSID(device, essid)            ath_set_essid(device, essid)
#define ATH_SET_SEC_TYPE(device, sectype)       ath_set_sec_type(device, sectype)
#define ATH_SET_PASSPHRASE(device, passphrase)  ath_set_passphrase(device, passphrase) 
#define ATH_SEND_WEP_PARAM(wep_param)           ath_send_wep_param(wep_param)
#define ATH_GET_ESSID(device,essid)             ath_get_essid(device,essid)
#define ATH_NET_CONNECT(pNetparams)             NetConnect(pNetparams)

/****************************************************************************
 * type
 ****************************************************************************/
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

typedef enum mode {

	MODE_STATION = 0,
	MODE_AP,
	MODE_ADHOC,
	MODE_MAXIMUM

}_mode_t;

typedef struct {
	uint_8 wps_in_progress;
	uint_8 connect_flag;
        A_UINT8 wps_pbc_interrupt;
        ATH_NETPARAMS netparams;
}wps_context_t;

/****************************************************************************
 * variable
 ****************************************************************************/
static HWTIMER hwtimer2;

static pointer pQuery;
static A_BOOL reg_query_bool = A_FALSE;

_enet_handle    handle;

static volatile A_UINT8 wifi_state = 0;
static A_UINT32 wifi_connected_flag = 0, concurrent_connect_flag = 0;
static wps_context_t wps_context;
static int user_defined_power_mode = MAX_PERF_POWER;
static A_UINT8 pmk_flag = 0, hidden_flag = 0, wps_flag = 0;
/****************************************************************************
 * local function
 ****************************************************************************/

/*FUNCTION*-----------------------------------------------------------
 *
 * Function Name  : get_active_device
 * Returned Value : A_OK
 * Comments       : Return the current active device in the application
 *
 *END------------------------------------------------------------------*/

static A_INT32 get_active_device()
{
    return 0;
}


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

static uint_32 ath_ioctl_handler_ext(A_VOID* enet_ptr, ATH_IOCTL_PARAM_STRUCT_PTR param_ptr) {
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


struct event_profile {
    A_UINT16 event_id; 
    A_UINT32 timestamp;
};
#define MAX_PROFILE_SIZE	20
struct event_profile profiles[MAX_PROFILE_SIZE];
A_UINT8 profile_head = 0, profile_trail = 0;

static char *event_str [] = 
{
    "wmi_ready",            // BOOT_PROFILE_WMI_READY
    "refclk read",          // BOOT_PROFILE_READ_REFCLK
    "config done",          // BOOT_PROFILE_DONE_CONFIG
    "start connect",        // BOOT_PROFILE_START_CONNECT
    "end connect",          // BOOT_PROFILE_DONE_CONNECT
    "driver ready",         // BOOT_PROFILE_DRIVE_READY
    "start scan",           // BOOT_PROFILE_START_SCAN
    "end scan",             // BOOT_PROFILE_DONE_SCAN
    "power up",             // BOOT_PROFILE_POWER_UP
    "boot param",           // BOOT_PROFILE_BOOT_PARAMETER
};


#define CYCLE_QUEUE_INC(ndx) {ndx = (ndx + 1) % MAX_PROFILE_SIZE;}

static void add_one_profile(A_UINT8 event_id, A_UINT16  timestamp) {
    profiles[profile_head].event_id = event_id;
    profiles[profile_head].timestamp = timestamp;

    CYCLE_QUEUE_INC(profile_head);
    if (profile_head == profile_trail)
        CYCLE_QUEUE_INC(profile_trail);		
}

static int_32 print_perf(int_32 argc, char_ptr argv[]) {
    int_32 ret = 0;
    A_UINT8   ndx;

    if (profile_trail == profile_head)
        return ret;

    printf ("       Event        TIME STAMP\n");
    ndx = profile_trail;
    while (ndx !=  profile_head) {
        printf("% 15s   % 6d\n", event_str[profiles[ndx].event_id], profiles[ndx].timestamp);
        CYCLE_QUEUE_INC(ndx);	
    }

    return ret;
}

static void boot_profile_output(uint_32 signal) {
    int		i;
    A_INT32	  timestamp;
    TIME_STRUCT    time;

    _time_get_elapsed(&time);
    timestamp = (time.SECONDS * 1000 + time.MILLISECONDS);

    add_one_profile(signal, timestamp);
}


#define	   QUEUE_IDX_INC(a)   ((a) = ((a) + 1) % MAX_PROFILE_SIZE)
#define	 kf_profile_head   profile_head
#define	 kf_profile_trail  profile_trail
#define	 kf_profiles       profiles

static void clear_kf_profile() {
    kf_profile_head = 0;
    kf_profile_trail = 0;
}

static void add_one_kf_profile(A_UINT16 event_id, A_UINT32  timestamp) {
    kf_profiles[kf_profile_head].event_id = event_id;
    kf_profiles[kf_profile_head].timestamp = timestamp;

    QUEUE_IDX_INC(kf_profile_head);
    if (kf_profile_head == kf_profile_trail)
        QUEUE_IDX_INC(kf_profile_trail);		
}


static A_VOID Custom_PfmDataEventRx(A_UINT8 *datap, A_UINT32 len) {
    int		num, i;
    WMIX_PFM_DATA_EVENT  *pfm_data;

    pfm_data = (WMIX_PFM_DATA_EVENT *)datap;

    num = len / sizeof(WMIX_PFM_DATA_EVENT);

    for (i=0; i < num; i++, pfm_data++) {   
        add_one_kf_profile(pfm_data->event_id, pfm_data->log_time);
    }

    return;
}

static A_VOID Custom_PfmDataDoneEventRx(A_VOID *pCxt, A_UINT8 *datap, A_UINT32 len) {
    A_DRIVER_CONTEXT *pDCxt = GET_DRIVER_COMMON(pCxt);

    pDCxt->pfmDone = A_TRUE;

    CUSTOM_API_SCAN_COMPLETE_EVENT(pCxt, status);

    return;
}

/*FUNCTION*-----------------------------------------------------
 *
 * Task Name    : timer_wakeup_isr
 * Comments     : This isr is supposed to wake up the CPU core
 *                and escape from "interrupts only" low power mode.
 *
 *END*-----------------------------------------------------------*/

static void timer_wakeup_isr(pointer parameter) {
    uint_32 timer = (uint_32)parameter;
    /* Stop the timer */
    hwtimer_stop(&hwtimer2);
    hwtimer_deinit(&hwtimer2);
}

/*FUNCTION*-------------------------------------------------------------
 *
 *  Function Name  : Custom_Delay
 *  PARAMS         : Delay in msec
 *                   
 *  Returned Value : none
 *  Comments       :
 *        Starts a timer for specified number of msec, puts EFM in EM2 mode.
 *        EFM will wake up when timer fires.
 *
 *END*-----------------------------------------------------------------*/
static void Custom_Delay(A_UINT32 delay) {
    if (MQX_OK != hwtimer_init(&hwtimer2, &BSP_HWTIMER2_DEV, BSP_HWTIMER2_ID, (BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX + 1))) {
        printf ("\nError during installation of timer interrupt!\n");
        _task_block();
    }

    hwtimer_set_period(&hwtimer2, BSP_HWTIMER2_SOURCE_CLK, delay*1000);
    hwtimer_callback_reg(&hwtimer2,(HWTIMER_CALLBACK_FPTR)timer_wakeup_isr, NULL);
    /* Start hwtimer2*/
    hwtimer_start(&hwtimer2);
}


static _enet_handle get_active_device_handle() {
    return(handle);
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ascii_to_hex()
* Returned Value  : hex counterpart for ascii
* Comments	: Converts ascii character to correesponding hex value
*
*END*-----------------------------------------------------------------*/
static unsigned char ascii_to_hex(char val) {
	if('0' <= val && '9' >= val){
		return (unsigned char)(val - '0');
	}else if('a' <= val && 'f' >= val){
		return (unsigned char)(val - 'a') + 0x0a;
	}else if('A' <= val && 'F' >= val){
		return (unsigned char)(val - 'A') + 0x0a;
	}

	return 0xff;/* error */
}

static int_32 parse_ipv4_ad(unsigned long * ip_addr,   /* pointer to IP address returned */
                            unsigned *  sbits,         /* default subnet bit number */
                            char *   stringin)
{
    int_32 error = -1;
    char *   cp;
    int   dots  =  0; /* periods imbedded in input string */
    int   number;
    union
    {
        unsigned char   c[4];
        unsigned long   l;
    } retval;

    cp = stringin;
    while (*cp)
    {
        if (*cp > '9' || *cp < '.' || *cp == '/')
            return(error);
        if (*cp == '.')dots++;
        cp++;
    }

    if ( dots < 1 || dots > 3 )
        return(error);

    cp = stringin;
    if ((number = atoi(cp)) > 255)
        return(error);

    retval.c[0] = (unsigned char)number;

    while (*cp != '.')cp++; /* find dot (end of number) */
    cp++;             /* point past dot */

    if (dots == 1 || dots == 2) retval.c[1] = 0;
    else
    {
        number = atoi(cp);
        while (*cp != '.')cp++; /* find dot (end of number) */
        cp++;             /* point past dot */
        if (number > 255) return(error);
        retval.c[1] = (unsigned char)number;
    }

    if (dots == 1) retval.c[2] = 0;
    else
    {
        number = atoi(cp);
        while (*cp != '.')cp++; /* find dot (end of number) */
        cp++;             /* point past dot */
        if (number > 255) return(error);
        retval.c[2] = (unsigned char)number;
    }

    if ((number = atoi(cp)) > 255)
        return(error);
    retval.c[3] = (unsigned char)number;

    if (retval.c[0] < 128) *sbits = 8;
    else if(retval.c[0] < 192) *sbits = 16;
    else *sbits = 24;

    *ip_addr = retval.l;
    return(0);
}



static A_STATUS is_driver_initialized() {
    A_INT32 error,dev_status; 
    _enet_handle enethandle;

    enethandle = get_active_device_handle();

    error = ENET_mediactl (enethandle,ENET_MEDIACTL_IS_INITIALIZED,&dev_status);
    if (ENET_OK != error || dev_status == A_FALSE)
    {
        return A_ERROR;
    }   

    return A_OK;
}

static A_STATUS handle_ioctl(ATH_IOCTL_PARAM_STRUCT* inout_param) { 
    _enet_handle enethandle;

    enethandle = get_active_device_handle();
    A_STATUS error = (A_STATUS) ENET_mediactl (enethandle,ENET_MEDIACTL_VENDOR_SPECIFIC,inout_param);

    return error;
}

static A_STATUS set_power_mode(A_BOOL pwr_mode, A_UINT8 pwr_module) {
    int mode;
    A_UINT32 enet_device;
    ATH_IOCTL_PARAM_STRUCT param;
    POWER_MODE power;
    A_INT32 error;

    if(pwr_mode == REC_POWER){
      /*Allow power save mode setting from a module only if user has configured
       REC_POWER*/
      if(user_defined_power_mode != REC_POWER)
        return A_ERROR;
    }
    power.pwr_module = pwr_module;
    power.pwr_mode   = pwr_mode;
    
    param.cmd_id = ATH_SET_POWER;
    param.data = (void*)&power;
    param.length = 4;
    
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    error = HANDLE_IOCTL(&param);

    return error;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : handle_pmk
* Returned Value : A_ERROR on error else A_OK
* Comments       :
*
*END------------------------------------------------------------------*/

static int handle_pmk(char* pmk) {
    int j;
    A_UINT8 val = 0;
    WMI_SET_PMK_CMD cmd;
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_INT32 error;


    memset(cmd.pmk, 0, 32);
    cmd.pmk_len = WMI_PMK_LEN;

    for(j=0;j<64;j++)
    {
        val = ascii_to_hex(pmk[j]);
        if(val == 0xff)
        {
                printf("Invalid character\n");
                return A_ERROR;
        }
        else
        {
                if((j&1) == 0)
                        val <<= 4;

                cmd.pmk[j>>1] |= val;
        }
    }

    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    inout_param.cmd_id = ATH_SET_PMK;
    inout_param.data = &cmd;
    inout_param.length = sizeof(cmd);
    error = HANDLE_IOCTL(&inout_param);

    return error;
}



/*FUNCTION*-------------------------------------------------------------
 *
 * Function Name   : get_dev_mode
 * Returned Value  : Device mode enum if success else A_ERROR 
 *                   
 * Comments        : Gets device mode
 *
 *END*-----------------------------------------------------------------*/
static A_INT32 get_dev_mode()
{
    A_UINT32 inout_param  = 0;
    A_INT32 error = A_OK;
    _enet_handle enethandle;

    enethandle = get_active_device_handle();

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }     

    error = ENET_mediactl (enethandle,ENET_GET_MEDIACTL_MODE,&inout_param);
    if (A_OK != error)
    {
        printf("Get devmode error\n");
        return A_ERROR;
    }

    switch(inout_param)
    {
        case ENET_MEDIACTL_MODE_INFRA:
            return (MODE_STATION);
            break;
        case ENET_MEDIACTL_MODE_ADHOC:
            return (MODE_ADHOC);
            break;
        case ENET_MEDIACTL_MODE_MASTER:
            return (MODE_AP);
            break;
        default:
            printf("Get devmode error\n");
            error = A_ERROR;
    }

    return error;
}

static A_STATUS ath_set_essid(A_UINT32 device, char* essid){
    return((A_STATUS) iwcfg_set_essid(device,essid)); 
}


static A_STATUS ath_set_sec_type(A_UINT32 device, char*  sec_type){
    return((A_STATUS) iwcfg_set_sec_type (device,sec_type)); 
}

static A_STATUS ath_set_passphrase(A_UINT32 device,char* passphrase){
    return((A_STATUS) iwcfg_set_passphrase (device,passphrase)); 
}

static A_STATUS ath_send_wep_param(ENET_WEPKEYS* wep_param){
    _enet_handle enethandle;

    enethandle = get_active_device_handle();
    return((A_STATUS) ENET_mediactl (enethandle,ENET_SET_MEDIACTL_ENCODE,wep_param));
}

/*FUNCTION*-------------------------------------------------------------
 *
 * Function Name   : set_cipher()
 * Returned Value  : A_OK if ESSID set successfully else ERROR CODE
 * Comments        : Sets the Unicast and multicast cipher
 *
 *END*-----------------------------------------------------------------*/

static A_INT32 set_cipher ( A_UINT32 dev_num, char* cipher) {
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_INT32 error;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
    inout_param.cmd_id = ATH_SET_CIPHER;
    inout_param.data = cipher;
    inout_param.length = 8;

    error = HANDLE_IOCTL (&inout_param);

    return error;
}


static A_STATUS ath_commit(A_UINT32 device) {
    return((A_STATUS) iwcfg_commit(device)); 
}
/*FUNCTION*-----------------------------------------------------------------
 *
 * Function Name  : wifiCallback
 * Returned Value : N/A
 * Comments       : Called from driver on a WiFI connection event
 *
 *END------------------------------------------------------------------*/
static void wifiCallback(int val, A_UINT8 devId,A_UINT8 * mac,A_BOOL bssConn) {

    wifi_state = val;
    if(val == A_TRUE) {
        wifi_connected_flag = 1;
        printf("Connected for device=%d\n",devId);

        if(devId == 0){
            printf("%s connect event:\n",bssConn?"AP":"CLIENT");
            concurrent_connect_flag = 0x0E;
        }
        //TURN_LED_ON;
    }
    else if(val == INVALID_PROFILE) // this event is used to indicate RSNA failure
    {
        printf("4 way handshake failure for device=%d n",devId);
    }
    else if(val == PEER_FIRST_NODE_JOIN_EVENT) //this event is used to RSNA success
    {
        printf("4 way handshake success for device=%d \n",devId);
    }
    else if(val == A_FALSE)
    {
        wifi_connected_flag = 0;
        printf("Not Connected for device=%d\n",devId);
        if(devId == 0){
            printf("%s disconnect event:\n",bssConn?"AP":"CLIENT");
            concurrent_connect_flag = 0x00;
        }                
        //TURN_LED_OFF;
    }
    else
    {
        printf("last tx rate : %d kbps--for device=%d\n", val,devId);
    }
    if(wps_context.wps_in_progress && devId==get_active_device())
    {
        wps_context.wps_in_progress=0;
        set_power_mode(REC_POWER,WPS);
    }
}


/*FUNCTION*-----------------------------------------------------------------
 *
 * Function Name  : set_callback
 * Returned Value : A_ERROR on error else A_OK
 * Comments       : Sets callback function for WiFi connect/disconnect event
 *
 *END------------------------------------------------------------------*/
static A_INT32 set_callback() {
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_INT32 error;

    inout_param.cmd_id = ATH_SET_CONNECT_STATE_CALLBACK;
    inout_param.data = (void*)wifiCallback;
    inout_param.length = 4;
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
    error = HANDLE_IOCTL(&inout_param);

    return error;
}


/****************************************************************************
 * global function
 ****************************************************************************/

int_32 ath_inet_aton ( const char*        name, /* [IN] dotted decimal IP address */
                       A_UINT32*     ipaddr_ptr /* [OUT] binary IP address */
                      )
{ /* Body */
    boolean     ipok = FALSE;
    uint_32     dots;
    uint_32     byte;
    _ip_address addr;

    addr = 0;
    dots = 0;
    for (;;) {

        if (!isdigit(*name)) break;

        byte = 0;
        while (isdigit(*name)) {
            byte *= 10;
            byte += *name - '0';
            if (byte > 255) break;
            name++;
        } /* Endwhile */
        if (byte > 255) break;
        addr <<= 8;
        addr += byte;

        if (*name == '.') {
            dots++;
            if (dots > 3) break;
            name++;
            continue;
        } /* Endif */

        if ((*name == '\0') && (dots == 3)) {
            ipok = TRUE;
        } /* Endif */

        break;

    } /* Endfor */

    if (!ipok) {
        return 0;
    } /* Endif */

    if (ipaddr_ptr) {
        *ipaddr_ptr = addr;
    } /* Endif */

    return -1;

} /* Endbody */


void atheros_driver_setup_init(void) {

    ath_custom_mediactl.ath_ioctl_handler_ext = ath_ioctl_handler_ext;

    ath_custom_init.Custom_Delay = Custom_Delay;
    ath_custom_init.Boot_Profile = boot_profile_output;

    ath_custom_init.Api_PfmDataEventRx = Custom_PfmDataEventRx;
    ath_custom_init.Api_PfmDataDoneEventRx = Custom_PfmDataDoneEventRx;

    return;
}     

#if DEMOCFG_ENABLE_RTCS

#include <ipcfg.h>


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


/*FUNCTION*-------------------------------------------------------------
 *
 * Function Name   :  qca4002_connect
 * Returned Value  : 
 *		
 * Comments
 *                   WPA/WPA2 security is supported
 *
 *END*-----------------------------------------------------------------*/
int qca4002_connect(char* ssid, char *securitymode, char *passwd){

    static char ssid_str[64];
    static char wpa_passphrase[MAX_PASSPHRASE_SIZE + 1]; //for avoiding overflow
    A_INT32 error= A_OK, i;
    int offset = 0, ssidLength = MAX_SSID_LENGTH ;
    A_UINT32 enet_device;
    A_UINT32 temp_mode;           

    enet_device = 0;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    set_callback();

#if 0
#if ENABLE_SCC_MODE     
    if((concurrent_connect_flag == 0x0E) && (enet_device == 1))
    {
        concurrent_connect_flag |= 0x01;
        //printf("shd come here only as special case concurrent flag : %x \n", concurrent_connect_flag);
        error = handle_connect_for_concurrent(index, argc, argv);
        if(error != A_OK)
        {
            concurrent_connect_flag = 0x0E;
            printf("cannot connect for concurrent mode\n");
            return A_ERROR;
        }
        concurrent_connect_flag = 0x0E;
    }
#endif
#endif

    temp_mode = get_dev_mode();

    if (get_dev_mode() == MODE_AP) {
        printf("setting to ap mode \n");
    }

#if 0
    if ((SEC_MODE_OPEN == security_mode) && (get_dev_mode() == MODE_AP)) {
        printf("AP in OPEN mode!\n");
    }
#endif

#if 0
    if(argc > 2)
    {
        for(i=index;i<argc;i++)
        {
            if((ssidLength < 0) || (strlen(argv[i]) > ssidLength))
            {
                printf("SSID length exceeds Maximum value\n");
                return A_ERROR;
            }
            strcpy(&ssid_str[offset], argv[i]);
            offset += strlen(argv[i]);
            ssidLength -= strlen(argv[i]);

            /*Add empty space in ssid except for last word*/
            if(i != argc-1)
            {
                strcpy(&ssid_str[offset], " ");
                offset += 1;
                ssidLength -= 1;
            }
        }
    }
    else
    {
        printf("Missing ssid\n");
        return A_ERROR;
    }
#endif

    memset(ssid_str, 0, sizeof ssid_str);
    strncpy(ssid_str, ssid, sizeof(ssid_str) - 1);

    error = ATH_SET_ESSID (enet_device,ssid_str/*argv[index]*/);
    if (error != 0) {
        //printf ("Error during setting of ssid %s error=%08x!\n",argv[index], error);
        printf ("Error during setting of ssid %s error=%08x!\n",ssid, error);
        return A_ERROR;
    }

    printf ("\nSetting SSID to %s \n\n",ssid_str/*argv[index]*/);
    //strcpy((char*)original_ssid,ssid_str /*argv[index]*/);

    //if (SEC_MODE_WEP == security_mode) {
    if (!strcmp("wep", securitymode)){
        cipher_t cipher;
        cipher.ucipher = WEP_CRYPT;
        cipher.mcipher = WEP_CRYPT;

        error = set_cipher(enet_device,(char *)&cipher);
        if(error != A_OK) {
            return A_ERROR;
        }
        error = ATH_SET_SEC_TYPE (enet_device, "wep");
        if(error != A_OK) { 
            return A_ERROR;
        }                
    }      
    //else if(SEC_MODE_WPA == security_mode)
    else if(!strcmp("wpa1", securitymode) || !strcmp("wpa2", securitymode))
    {
        cipher_t cipher;
        static char wpa_ver[8];

        memset(wpa_ver, 0, sizeof(wpa_ver));

	if(!strcmp("wpa1", securitymode)){
		cipher.ucipher = 0x04;
		cipher.mcipher = 0x04;
		strcpy(wpa_ver,"wpa");
	}
	else 
        if(!strcmp("wpa2", securitymode)){
		cipher.ucipher = 0x08;
		cipher.mcipher = 0x08;
		strcpy(wpa_ver,"wpa2");
	}
        error = set_cipher(enet_device,(char *)&cipher);
        if(error != A_OK) {
            return A_ERROR;
        }
        error = ATH_SET_SEC_TYPE (enet_device,wpa_ver);
        if(error != A_OK) {
            return A_ERROR;
        }

        memset(wpa_passphrase, 0, sizeof(wpa_passphrase));
        strncpy(wpa_passphrase, passwd, sizeof(wpa_passphrase) - 1);

        if(strlen(wpa_passphrase) != 64) {
            error = ATH_SET_PASSPHRASE (enet_device,wpa_passphrase);
            if(error != A_OK) {
                return A_ERROR;
            }
            pmk_flag = 1;
        }
        else
        {
            if(handle_pmk(wpa_passphrase)!= A_OK) {
                printf("Unable to set pmk\n");
                return A_ERROR;
            }
            pmk_flag = 1;
        }
    }

    error = ATH_COMMIT(enet_device);

    if(error != A_OK) {
        return A_ERROR;
    }

    return A_OK;
}

int qca4002_is_connect(void){

    return wifi_connected_flag;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   :  qca4002_ipconfig_static
* Returned Value  : 
*
* Comments        : Sample function to depict ipconfig functionality.
*                   Sets static IPv4 parameters.
*
*END*-----------------------------------------------------------------*/
int qca4002_ipconfig_static(char* ip_addr_string, char *mask_string, char *gw_string) {
	A_UINT32 addr, mask, gw;
        A_INT32 error;
	unsigned int sbits;

        /*Check if driver is loaded*/
        if(IS_DRIVER_READY != A_OK){
            return A_ERROR;
        }

	error = parse_ipv4_ad(&addr, &sbits, ip_addr_string);
	if(error != 0) {
		printf("Invalid IP address\n");
		return error;
	}
	addr = A_BE2CPU32(addr);

	error = parse_ipv4_ad(&mask, &sbits, mask_string);
	if(error != 0) {
		printf("Invalid Mask address\n");
		return error;
	}
	mask = A_BE2CPU32(mask);

	error = parse_ipv4_ad(&gw, &sbits, gw_string);
	if(error != 0) {
		printf("Invalid Gateway address\n");
		return error;
	}
	gw = A_BE2CPU32(gw);


	t_ipconfig((void*)handle, IPCFG_STATIC, &addr, &mask, &gw, NULL, NULL);
	printf("IP:%x Mask:%x, Gateway:%x\n", addr, mask, gw);
	printf("IP:%d.%d.%d.%d Mask:%d.%d.%d.%d, Gateway:%d.%d.%d.%d\n",
				getByte(3, addr), getByte(2, addr), getByte(1, addr), getByte(0, addr),
				getByte(3, mask), getByte(2, mask), getByte(1, mask), getByte(0, mask),
				getByte(3, gw), getByte(2, gw), getByte(1, gw), getByte(0, gw));


	return 0;
}

/* end file */

