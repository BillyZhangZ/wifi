//------------------------------------------------------------------------------
// Copyright (c) 2011 Qualcomm Atheros, Inc.
// All Rights Reserved.
// Qualcomm Atheros Confidential and Proprietary.
// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is
// hereby granted, provided that the above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
// USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//------------------------------------------------------------------------------
//==============================================================================
// Author(s): ="Atheros"
//==============================================================================
#include <a_config.h>
#include <a_types.h>
#include <a_osapi.h>
#include <driver_cxt.h>
#include <common_api.h>
#include <custom_api.h>
#include <wmi_api.h>
#include <bmi.h>
#include <htc.h>
#include <wmi_host.h>
#if ENABLE_P2P_MODE
#include <wmi.h>
#include "p2p.h"
#endif


#include "mqx.h"
#include "bsp.h"
#include "enet.h"
#include "enetprv.h"

#include "atheros_wifi.h"
#include "enet_wifi.h"
#include "atheros_wifi_api.h"
#include "atheros_wifi_internal.h"
#include "dset_api.h"

#if ENABLE_P2P_MODE
//extern WPS_CREDENTIAL persistCredential;
extern WMI_P2P_PROV_INFO p2p_key_val;
#endif

extern A_CONST A_UINT8 max_performance_power_param;
extern A_CONST WMI_STORERECALL_CONFIGURE_CMD default_strrcl_config_cmd;
extern A_CONST WMI_SCAN_PARAMS_CMD default_scan_param;

uint_32 Custom_Api_Mediactl( ENET_CONTEXT_STRUCT_PTR enet_ptr, uint_32 command_id, pointer inout_param);
extern uint_32 chip_state_ctrl(ENET_CONTEXT_STRUCT_PTR enet_ptr, uint_32 state);

#if !ENABLE_STACK_OFFLOAD
A_INT32 power_state_for_module = 0;
/*****************************************************************************/
/*  move_to_maxperf - Changes the power mode to max perf in firmware. There are
 *                    some commands which need to run in MAX_PERF mode like store_recall etc
 *                    This function changes the power modeto MAX_PERF and will not 
 *                    intimate the application. Later after the command succeeds,
 *                    use restore_power_state to change to original power state
 *                    Note: Application should not directly use this function
 *  A_INT32 - module which request the change
 *  Returns- A_OK on success, A_ERROR otherwise
 *****************************************************************************/
A_STATUS move_power_state_to_maxperf(void *pCxt, A_INT32 module)
{
    A_DRIVER_CONTEXT *pDCxt = (A_DRIVER_CONTEXT *)pCxt;
    
    /* Already some HIGH module changes state */
    if (power_state_for_module >= module)
    {
        return A_OK;
    }
    
    /* Change the power mode only when the current power mode is REC_POWER */
    if (pDCxt->userPwrMode==REC_POWER)
    {
        if(A_OK != wmi_cmd_start(pDCxt->pWmiCxt, &max_performance_power_param, WMI_SET_POWER_MODE_CMDID, sizeof(WMI_POWER_MODE_CMD)))
        {
            return A_ERROR;
        }
    }
    power_state_for_module = module;
    return A_OK;
}
#endif
static A_STATUS wait_scan_done(A_VOID* pCxt, A_VOID* pWmi)
{
	A_DRIVER_CONTEXT *pDCxt = GET_DRIVER_COMMON(pCxt);

	if(pDCxt->scanDone == A_FALSE){
		/* block until scan completes */
		CUSTOM_DRIVER_WAIT_FOR_CONDITION(pCxt, &(pDCxt->scanDone), A_TRUE, 5000);

		if(pDCxt->scanDone == A_FALSE){
			wmi_bssfilter_cmd(pWmi, NONE_BSS_FILTER, 0);
		}
                return A_ERROR;
	}
	pDCxt->scanDone = A_FALSE;
        return A_OK;
}

static A_UINT32 scan_setup(A_VOID *pCxt, A_VOID *pWmi)
{
    union{
	    WMI_START_SCAN_CMD scan_cmd;
	    WMI_PROBED_SSID_CMD probeParam;
	}stackU;
	A_UINT32 error = ENET_OK;
	A_DRIVER_CONTEXT *pDCxt = GET_DRIVER_COMMON(pCxt);

	do{
		if(pDCxt->conn[pDCxt->devId].ssidLen > 0){
			/* apply filter ssid */
			stackU.probeParam.entryIndex = 1;
    		stackU.probeParam.flag       = SPECIFIC_SSID_FLAG;
    		stackU.probeParam.ssidLength = pDCxt->conn[pDCxt->devId].ssidLen;
    		A_MEMCPY(stackU.probeParam.ssid, pDCxt->conn[pDCxt->devId].ssid, pDCxt->conn[pDCxt->devId].ssidLen);

			if(A_OK != wmi_cmd_start(pWmi, &stackU.probeParam, WMI_SET_PROBED_SSID_CMDID, sizeof(WMI_PROBED_SSID_CMD))){
	         	error = ENET_ERROR;
	         	break;
			}

			if(A_OK != wmi_bssfilter_cmd(pWmi, PROBED_SSID_FILTER, 0)){
				error = ENET_ERROR;
	         	break;
			}
		}else{
			/* clear any pre-existing filter ssid */
			stackU.probeParam.entryIndex = 1;
    		stackU.probeParam.flag       = DISABLE_SSID_FLAG;
    		stackU.probeParam.ssidLength = 0;

			if(A_OK != wmi_cmd_start(pWmi, &stackU.probeParam, WMI_SET_PROBED_SSID_CMDID, sizeof(WMI_PROBED_SSID_CMD))){
	         	error = ENET_ERROR;
	         	break;
			}

			if(A_OK != wmi_bssfilter_cmd(pWmi, ALL_BSS_FILTER, 0)){
				error = ENET_ERROR;
	         	break;
			}
		}

		GET_DRIVER_CXT(pCxt)->scanOutSize = ATH_MAX_SCAN_BUFFERS;
		GET_DRIVER_CXT(pCxt)->scanOutCount = 0;
		pDCxt->scanDone = A_FALSE;
		/* start the scan */
		A_MEMZERO(&stackU.scan_cmd, sizeof(WMI_START_SCAN_CMD));
	    stackU.scan_cmd.scanType = WMI_LONG_SCAN;
	    stackU.scan_cmd.forceFgScan = A_FALSE;
	    stackU.scan_cmd.isLegacy = A_FALSE;
	    stackU.scan_cmd.homeDwellTime = 0;
	    stackU.scan_cmd.forceScanInterval = 1;
	    stackU.scan_cmd.numChannels = 0;

		if(A_OK != wmi_cmd_start(pWmi, (A_VOID*)&stackU.scan_cmd, WMI_START_SCAN_CMDID, sizeof(WMI_START_SCAN_CMD))){
			error = ENET_ERROR;
			break;
		}
	}while(0);

	return error;
}

static A_STATUS config_dump_target_assert_info(A_VOID *pCxt)
{
	A_UINT32 regDumpValues[60];
	A_UINT16 length = 60;
	A_UINT16 i;
	A_DRIVER_CONTEXT *pDCxt = GET_DRIVER_COMMON(pCxt);

	if(A_OK == Driver_DumpAssertInfo(pCxt, regDumpValues, &length)){
		for (i = 0; i < length; i+=4)
	    {
	        printf ("0x%08X ", A_LE2CPU32(regDumpValues[i]));
	        printf ("0x%08X ", A_LE2CPU32(regDumpValues[i+1]));
	        printf ("0x%08X ", A_LE2CPU32(regDumpValues[i+2]));
	        printf ("0x%08X\n", A_LE2CPU32(regDumpValues[i+3]));

	    }
	}
	/* clear this request */
	pDCxt->asynchRequest = NULL;
	return A_OK;
}

static uint_8
ascii_to_hex(char val)
{
	if('0' <= val && '9' >= val){
		return (uint_8)(val - '0');
	}else if('a' <= val && 'f' >= val){
		return (uint_8)((val - 'a') + 0x0a);
	}else if('A' <= val && 'F' >= val){
		return (uint_8)((val - 'A') + 0x0a);
	}

	return 0xff;/* error */
}


/* used to translate from the WPS_ERROR_CODE to that ATH_WPS_ERROR_CODE exposed to app */
const A_UINT8 const_wps_error_code_translation[]=
{
	ATH_WPS_ERROR_SUCCESS,
	ATH_WPS_ERROR_INVALID_START_INFO,
	ATH_WPS_ERROR_MULTIPLE_PBC_SESSIONS,
	ATH_WPS_ERROR_WALKTIMER_TIMEOUT,
    ATH_WPS_ERROR_M2D_RCVD,
    ATH_WPS_ERROR_PWD_AUTH_FAIL,
    ATH_WPS_ERROR_CANCELLED,
    ATH_WPS_ERROR_INVALID_PIN,
};


static A_STATUS
TxRawPacket(A_VOID *pCxt, A_VOID *pReq, ATH_MAC_TX_PARAMS *pParams)
{
	WMI_TX_META_V3 v3_meta;
	HTC_ENDPOINT_ID    eid;
	A_DRIVER_CONTEXT *pDCxt;
	A_STATUS status = A_ERROR;

	eid = Util_AC2EndpointID (pCxt, WMM_AC_BE);
	A_NETBUF_SET_ELEM(pReq, A_REQ_EPID, eid);
	pDCxt = GET_DRIVER_COMMON(pCxt);

	do{
		/* translate from ATH_MAC_TX_PARAMS to WMI_TX_META_V3 */
		/* how convenient the values are all the same */
		v3_meta.pktID = pParams->pktID;
		A_MEMCPY(&v3_meta.rateSched.rateSeries, &pParams->rateSched.rateSeries, sizeof(A_UINT8)*WMI_TX_MAX_RATE_SERIES);
		A_MEMCPY(&v3_meta.rateSched.trySeries, &pParams->rateSched.trySeries, sizeof(A_UINT8)*WMI_TX_MAX_RATE_SERIES);
		v3_meta.rateSched.flags = pParams->rateSched.flags;
		v3_meta.rateSched.accessCategory = pParams->rateSched.accessCategory;


		if (wmi_data_hdr_add(pDCxt->pWmiCxt, pReq, DATA_MSGTYPE, FALSE,
            	WMI_DATA_HDR_DATA_TYPE_802_11,
            	WMI_META_VERSION_3, (void*)&v3_meta) != A_OK) {
        	break;
    	}
        A_MDELAY(2);
            /* HTC interface is asynchronous, if this fails, cleanup will happen in
             * the ar6000_tx_complete callback */
        if(A_OK != Driver_SubmitTxRequest((A_VOID*)pCxt, pReq)){
    		A_NETBUF_FREE(pReq);
    		break;
    	}

    	status = A_OK;
    }while(0);

    return status;
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

uint_32
ath_ioctl_handler(ENET_CONTEXT_STRUCT_PTR enet_ptr, ATH_IOCTL_PARAM_STRUCT_PTR param_ptr)
{
    A_STATUS status = A_OK;
	uint_32 error = ENET_OK;
	A_VOID *pCxt, *pWmi;
	A_DRIVER_CONTEXT *pDCxt;
	pointer data;
	WMI_WPS_PROFILE_EVENT *pWpsEv;
	WMI_SCAN_PARAMS_CMD scan_param_cmd;
	WMI_CHANNEL_PARAMS_CMD channel_param_cmd;
    POWER_MODE pwrmode;
    A_UINT32 priority, old_priority;

#if ENABLE_P2P_MODE
    WMI_P2P_FW_CONNECT_CMD_STRUCT         *pP2PConnect;
    WMI_P2P_FW_INVITE_REQ_RSP_CMD  *pP2PInviteAuth;
    WMI_P2P_INVITE_CMD          *p2pInvite;
    WMI_SET_PASSPHRASE_CMD      *passParam;
    WMI_P2P_GRP_INIT_CMD        *p2pGroup;


    union{
        WMI_WPS_START_CMD wps_start;
        WMI_P2P_FW_CONNECT_CMD_STRUCT joinProfile;
        WMI_SET_HT_CAP_CMD ht_cap_cmd;
        WPS_CREDENTIAL *pCred;
        A_NETBUF* a_netbuf_ptr;
        A_UINT32 param;
        WMI_SCAN_PARAMS_CMD scan_param_cmd;
        WMI_LISTEN_INT_CMD listen_param;
        WMI_ALLOW_AGGR_CMD allow_aggr_param;
        WMI_STORERECALL_HOST_READY_CMD storerecall_ready_param;
        WMI_POWER_PARAMS_CMD power_param_param;
        WMI_SET_CHANNEL_CMD set_channel_param;
        WMI_SET_FILTERED_PROMISCUOUS_MODE_CMD set_prom_param;
        WMI_GREENTX_PARAMS_CMD gtx_param;
        WMI_LPL_FORCE_ENABLE_CMD lpl_param;
    }stackU;
#else
    union{
    	WMI_WPS_START_CMD wps_start;
    	WMI_SET_HT_CAP_CMD ht_cap_cmd;
    	WPS_CREDENTIAL *pCred;
    	A_NETBUF* a_netbuf_ptr;
    	A_UINT32 param;
    	WMI_SCAN_PARAMS_CMD scan_param_cmd;
    	WMI_LISTEN_INT_CMD listen_param;
    	WMI_ALLOW_AGGR_CMD allow_aggr_param;
    	WMI_STORERECALL_HOST_READY_CMD storerecall_ready_param;
    	WMI_POWER_PARAMS_CMD power_param_param;
    	WMI_SET_CHANNEL_CMD set_channel_param;
    	WMI_SET_FILTERED_PROMISCUOUS_MODE_CMD set_prom_param;
    	WMI_GREENTX_PARAMS_CMD gtx_param;
    	WMI_LPL_FORCE_ENABLE_CMD lpl_param;
    }stackU;
#endif

    #define WPS_START (stackU.wps_start)
    #define HT_CAP (stackU.ht_cap_cmd)
    #define CRED_PTR (stackU.pCred)
    #define PCB_PTR (stackU.a_netbuf_ptr)
    #define PARAM (stackU.param)
    #define SCAN_PARAM (stackU.scan_param_cmd)
    #define LISTEN_PARAM (stackU.listen_param)
    #define AGGR_PARAM (stackU.allow_aggr_param)
    #define STRRCL_RDY_PARAM (stackU.storerecall_ready_param)
    #define POWER_PARAM_PARAM (stackU.power_param_param)
    #define SET_CHANNEL_PARAM (stackU.set_channel_param)
    #define SET_PROM_PARAM (stackU.set_prom_param)
    #define GTX_PARAM (stackU.gtx_param)
    #define LPL_PARAM (stackU.lpl_param)

	#define PTR_POWER_PARAM ((WMI_POWER_PARAMS_CMD *)(data))
	#define PTR_LI    		((uint_32_ptr)(data))
	#define PTR_HOST_MODE   ((char_ptr)(data))
	#define PTR_CIPHER      ((cipher_t*)(data))
	#define PTR_SEC_MODE   ((char_ptr)(data))
	#define PTR_PHY_MODE   ((char_ptr)(data))
	#define PTR_CONNECT_CB ((ATH_CONNECT_CB)(data))
	#define PTR_STRRCL_START ((uint_32_ptr)(data))
	#define PTR_VERSION     ((ATH_VERSION_PTR)(data))
	#define PTR_VERSION_STR ((ATH_VERSION_STR_PTR)(data))
	#define PTR_WPS_START	((ATH_WPS_START*)(data))
	#define PTR_WPS_GET		((ATH_NETPARAMS*)(data))
	#define PTR_PMK_SET		((A_UINT8*)(data))
	#define PTR_SCAN_CTRL	((ATH_SCANPARAMS*)(data))
	#define PTR_SCAN_PARAMS	((WMI_SCAN_PARAMS_CMD*)(data))
	#define PTR_MAC_TX		((ATH_MAC_TX_RAW_S*)(data))
	#define PTR_SET_CHAN	((uint_32_ptr*)(data))
	#define PTR_SET_AGGR	((ATH_SET_AGGREGATION_PARAM*)(data))
	#define PTR_FLASH_CMD   ((ATH_PROGRAM_FLASH_STRUCT*)(data))
	#define PTR_TX_STATUS_CMD ((ATH_TX_STATUS*)(data))
	#define PTR_PROM_MODE	((ATH_PROMISCUOUS_MODE*)(data))
	#define PTR_REG_DOMAIN_CMD  ((ATH_REG_DOMAIN*)(data))
	#define PTR_EXT_SCAN_CMD ((ATH_GET_SCAN*)(data))
#if ENABLE_P2P_MODE
    #define INVITE_PROF (stackU.wpsProfile)
    #define JOIN_CONN_PROF (stackU.joinProfile)
    #define PTR_PASS_PHRASE ((WMI_SET_PASSPHRASE_CMD *)(data))
#endif

    #define PTR_LAST_ERROR   (uint_32*)(data)
    #define PTR_GTX         (A_UINT8*)(data)
    #define PTR_LPL         (A_UINT8*)(data)
    #define PTR_MAC_ADDR	((ATH_PROGRAM_MAC_ADDR_PARAM*)(data))

	data = param_ptr->data;

	if((pCxt = enet_ptr->MAC_CONTEXT_PTR) == NULL)
	{
	   	return ENET_ERROR;
	}

	pDCxt = GET_DRIVER_COMMON(pCxt);
	pWmi = pDCxt->pWmiCxt;

	switch(param_ptr->cmd_id)
	{
	case ATH_SCAN_CTRL:
		A_MEMCPY(&SCAN_PARAM, &default_scan_param, sizeof(WMI_SCAN_PARAMS_CMD));
	    SCAN_PARAM.fg_start_period  = ((PTR_SCAN_CTRL->flags & ATH_DISABLE_FG_SCAN)? A_CPU2LE16(0xffff):A_CPU2LE16(0));
	    SCAN_PARAM.fg_end_period    = ((PTR_SCAN_CTRL->flags & ATH_DISABLE_FG_SCAN)? A_CPU2LE16(0xffff):A_CPU2LE16(0));
	    SCAN_PARAM.bg_period        = ((PTR_SCAN_CTRL->flags & ATH_DISABLE_BG_SCAN)? A_CPU2LE16(0xffff):A_CPU2LE16(0));
	    if(A_OK != wmi_cmd_start(pWmi, &SCAN_PARAM, WMI_SET_SCAN_PARAMS_CMDID, sizeof(WMI_SCAN_PARAMS_CMD)))
        {
        	error = ENET_ERROR;
        }
	  	break;
        case ATH_SET_SCAN_PARAM:
	    A_MEMCPY(&SCAN_PARAM, PTR_SCAN_PARAMS, sizeof(WMI_SCAN_PARAMS_CMD));
             
	    if(A_OK != wmi_cmd_start(pWmi, &SCAN_PARAM, WMI_SET_SCAN_PARAMS_CMDID, sizeof(WMI_SCAN_PARAMS_CMD)))
            {
        	error = ENET_ERROR;
            }
	    break;
	case ATH_SET_TXPWR:
        if(A_OK != wmi_cmd_start(pWmi, param_ptr->data,
                WMI_SET_TX_PWR_CMDID, sizeof(WMI_SET_TX_PWR_CMD)))
        {
        	error = ENET_ERROR;
        }
	  	break;
	case ATH_SET_DEVICE_ID:
        {
            uint_16 deviceId = *(A_UINT16*)(param_ptr->data);
            /*No change needed, don't return failure*/
            if(pDCxt->devId == deviceId){                
                break;
            }
            
            if(pDCxt->wps_in_progress)
            {
                error = ENET_ERROR;
            } 
            
            if((deviceId < 0) && (deviceId >= WLAN_NUM_OF_DEVICES))
            {
                error = ENET_ERROR;
                break;
            }            
            
            ((struct wmi_t *)pWmi)->deviceid  = deviceId;
            pDCxt->devId = deviceId;
        }
        break;
	case ATH_SET_PMPARAMS:
        A_MEMZERO(&POWER_PARAM_PARAM, sizeof(WMI_POWER_PARAMS_CMD));
        POWER_PARAM_PARAM.idle_period   = A_CPU2LE16(PTR_POWER_PARAM->idle_period);
        POWER_PARAM_PARAM.pspoll_number = A_CPU2LE16(PTR_POWER_PARAM->pspoll_number);
        POWER_PARAM_PARAM.dtim_policy   = A_CPU2LE16(PTR_POWER_PARAM->dtim_policy);
        POWER_PARAM_PARAM.tx_wakeup_policy = A_CPU2LE16(PTR_POWER_PARAM->tx_wakeup_policy);
        POWER_PARAM_PARAM.num_tx_to_wakeup = A_CPU2LE16(PTR_POWER_PARAM->num_tx_to_wakeup);
        POWER_PARAM_PARAM.ps_fail_event_policy = A_CPU2LE16(PTR_POWER_PARAM->ps_fail_event_policy);

        if(A_OK != wmi_cmd_start(pWmi, &POWER_PARAM_PARAM,
                WMI_SET_POWER_PARAMS_CMDID, sizeof(WMI_POWER_PARAMS_CMD)))
        {
        	error = ENET_ERROR;
        }
		break;
	case ATH_SET_LISTEN_INT:
		LISTEN_PARAM.listenInterval = A_CPU2LE16((*PTR_LI));
		LISTEN_PARAM.numBeacons = A_CPU2LE16(0);
		if(A_OK != wmi_cmd_start(pWmi, &LISTEN_PARAM,
                WMI_SET_LISTEN_INT_CMDID, sizeof(WMI_LISTEN_INT_CMD))){
			error = ENET_ERROR;
		}
		break;
	case ATH_SET_CIPHER:
		pDCxt->conn[pDCxt->devId].wpaPairwiseCrypto = (A_UINT8)PTR_CIPHER->ucipher;
		pDCxt->conn[pDCxt->devId].wpaGroupCrypto = (A_UINT8)PTR_CIPHER->mcipher;
		break;
    case ATH_GET_CONC_DEV_CHANNEL:
                *(A_UINT16*)(param_ptr->data) = 0;     
                if( pDCxt->devId == 1)
                {
                    if ((pDCxt->conn[0].isConnected) && (!(pDCxt->conn[1].isConnected)))
                    {
                         *(A_UINT16*)(param_ptr->data) = pDCxt->conn[0].channelHint;
                    }
                }
                else if( pDCxt->devId == 0 )
                {
                    if ((pDCxt->conn[1].isConnected) && (!(pDCxt->conn[0].isConnected)))
                    {
                         *(A_UINT16*)(param_ptr->data) = pDCxt->conn[1].channelHint;
                    }
                }
         break;
	case ATH_SET_SEC_MODE:
		if(strcmp(PTR_SEC_MODE,"open") == 0)
		{
			pDCxt->conn[pDCxt->devId].dot11AuthMode  = OPEN_AUTH;
		}
		else if(strcmp(PTR_SEC_MODE,"shared") == 0)
		{
			pDCxt->conn[pDCxt->devId].dot11AuthMode = SHARED_AUTH;
		}
		else
		{
			error = ENETERR_INVALID_INIT_PARAM;
		}
		break;
	case ATH_SET_PHY_MODE:
		A_MEMZERO(&HT_CAP, sizeof(WMI_SET_HT_CAP_CMD));

		if(strcmp(PTR_PHY_MODE,"b") == 0)
		{
			pDCxt->conn[pDCxt->devId].phyMode = WMI_11B_MODE;
		}
		else if(strcmp(PTR_PHY_MODE,"g") == 0)
		{
			pDCxt->conn[pDCxt->devId].phyMode = WMI_11G_MODE;
		}
		else if(strcmp(PTR_PHY_MODE,"a") == 0)
		{
			pDCxt->conn[pDCxt->devId].phyMode = WMI_11A_MODE;
		}
		else if(strcmp(PTR_PHY_MODE,"n") == 0)
		{
			pDCxt->conn[pDCxt->devId].phyMode = (pDCxt->conn[pDCxt->devId].phyMode == WMI_11A_MODE)?WMI_11A_MODE:WMI_11G_MODE;
            HT_CAP.band              = (pDCxt->conn[pDCxt->devId].phyMode == WMI_11A_MODE)?0x01:0x00;
			HT_CAP.enable            = 1;
			HT_CAP.short_GI_20MHz    = 1;
			HT_CAP.max_ampdu_len_exp = 2;
		}
        else if(strcmp(PTR_PHY_MODE,"ht40") == 0)
        {
			pDCxt->conn[pDCxt->devId].phyMode = (pDCxt->conn[pDCxt->devId].phyMode == WMI_11A_MODE)?WMI_11A_MODE:WMI_11G_MODE;
            HT_CAP.band              = (pDCxt->conn[pDCxt->devId].phyMode == WMI_11A_MODE)?0x01:0x00;
			HT_CAP.enable = 1;
			HT_CAP.short_GI_20MHz = 1;
            HT_CAP.short_GI_40MHz    = 1;
            HT_CAP.intolerance_40MHz = 0;
			HT_CAP.max_ampdu_len_exp = 2;
            HT_CAP.chan_width_40M_supported = 1;
		}

		A_MEMZERO(&channel_param_cmd, sizeof(WMI_CHANNEL_PARAMS_CMD));
		channel_param_cmd.scanParam = 1;
		channel_param_cmd.phyMode = pDCxt->conn[pDCxt->devId].phyMode;
		if(A_OK != wmi_cmd_start(pWmi, &channel_param_cmd, WMI_SET_CHANNEL_PARAMS_CMDID, sizeof(WMI_CHANNEL_PARAMS_CMD))){
			error = ENET_ERROR;
			break;
		}

		if(A_OK != wmi_cmd_start(pWmi, &HT_CAP, WMI_SET_HT_CAP_CMDID, sizeof(WMI_SET_HT_CAP_CMD))){
			error = ENET_ERROR;
			break;
		}
		break;
	case ATH_GET_PHY_MODE:
		*(uint_32*)(param_ptr->data) = pDCxt->conn[pDCxt->devId].phyMode;
		break;
	case ATH_GET_RX_RSSI:
     	pDCxt->rssiFlag = A_TRUE;
		wmi_cmd_start(pWmi, NULL, WMI_GET_STATISTICS_CMDID, 0);

        while(pDCxt->rssiFlag == A_TRUE){
			CUSTOM_DRIVER_WAIT_FOR_CONDITION(pCxt, &(pDCxt->rssiFlag), A_FALSE, 1000);
        }

		*(A_UINT32*)(param_ptr->data) = pDCxt->rssi;
		break;
	case ATH_GET_TEMPERATURE:
  		pDCxt->temperatureValid = A_FALSE;
                if (A_OK == wmi_cmd_start(pWmi, NULL, WMI_GET_TEMPERATURE_CMDID, 0))
                {
  		    /* block here until event arrives from wifi device */
                    CUSTOM_DRIVER_WAIT_FOR_CONDITION(pCxt, &(pDCxt->temperatureValid), A_TRUE, 1000);
                }
		if (pDCxt->temperatureValid == A_FALSE)
		{
        	   error = ENET_ERROR;
		}
		else
		{
		   (((WMI_GET_TEMPERATURE_REPLY*)(param_ptr->data))->tempRegVal) = pDCxt->raw_temperature ;
                   (((WMI_GET_TEMPERATURE_REPLY*)(param_ptr->data))->tempDegree) = pDCxt->tempDegree ;
                   pDCxt->temperatureValid = A_FALSE;
		}
		break;
  case ATH_ROAM_CTRL:
                wmi_cmd_start(pWmi, param_ptr->data, WMI_SET_ROAM_CTRL_CMDID, 0)  ;
                break;
	case ATH_SET_CONNECT_STATE_CALLBACK:
		DRIVER_SHARED_RESOURCE_ACCESS_ACQUIRE(pCxt);
		GET_DRIVER_CXT(pCxt)->connectStateCB = (void*)PTR_CONNECT_CB;
		DRIVER_SHARED_RESOURCE_ACCESS_RELEASE(pCxt);
		break;
#if DRIVER_CONFIG_ENABLE_STORE_RECALL
	case ATH_DEVICE_SUSPEND_ENABLE:
		if(pDCxt->strrclState == STRRCL_ST_DISABLED &&
			A_OK == wmi_cmd_start(pDCxt->pWmiCxt, &default_strrcl_config_cmd,
                    WMI_STORERECALL_CONFIGURE_CMDID, sizeof(WMI_STORERECALL_CONFIGURE_CMD)))
		{
			pDCxt->strrclState = STRRCL_ST_INIT;
			error = ENET_OK;
		}else{
			if(pDCxt->strrclState != STRRCL_ST_INIT)
				error = ENET_ERROR;
		}
		break;
	case ATH_DEVICE_SUSPEND_START:
		error = ENET_ERROR;

		/* ensure that no power save is used during Suspend. Store-recall and Power save
		   are not supposed to work together, it is one-or-the-other */
#if 1
		if (A_OK != move_power_state_to_maxperf(pDCxt, 1))/* refer POWER_STATE_MOVED_FOR_STRRCL */
		{
                status = A_ERROR;
                break;
		}
#else
		if(A_OK != wmi_cmd_start(pWmi, &max_performance_power_param,
                WMI_SET_POWER_MODE_CMDID, sizeof(WMI_POWER_MODE_CMD))){
			error = ENET_ERROR;
		}
		pDCxt->userPwrMode = MAX_PERF_POWER;
#endif                 

#if 0
        STRRCL_RDY_PARAM.sleep_msec = A_CPU2LE32((*PTR_STRRCL_START));
        STRRCL_RDY_PARAM.store_after_tx_empty = 1;
        STRRCL_RDY_PARAM.store_after_fresh_beacon_rx = 1;

        if(pDCxt->strrclState == STRRCL_ST_INIT){
			if(((*PTR_STRRCL_START) >= MIN_STRRCL_MSEC) &&
				A_OK == wmi_cmd_start(pWmi, &STRRCL_RDY_PARAM,
                        WMI_STORERECALL_HOST_READY_CMDID,
                        sizeof(WMI_STORERECALL_HOST_READY_CMD)))
			{
				pDCxt->strrclState = STRRCL_ST_START;
				error = ENET_OK;
			}
		}
#else
        	if(pDCxt->strrclState == STRRCL_ST_INIT && 
          ((*PTR_STRRCL_START) >= MIN_STRRCL_MSEC)){
            /* set strrclBlock before call to wmi_storerecall_ready_cmd to prevent
             * any potential race conditions with other tasks calling into the 
             * driver. 
             */
            pDCxt->strrclBlock = A_TRUE;
            STRRCL_RDY_PARAM.sleep_msec = A_CPU2LE32((*PTR_STRRCL_START));
            STRRCL_RDY_PARAM.store_after_tx_empty = 1;
            STRRCL_RDY_PARAM.store_after_fresh_beacon_rx = 1;
            
            priority = 5;
            _task_set_priority(_task_get_id(), priority, &old_priority);
            
			if(A_OK == wmi_cmd_start(pWmi, &STRRCL_RDY_PARAM, 
                        WMI_STORERECALL_HOST_READY_CMDID, 
                        sizeof(WMI_STORERECALL_HOST_READY_CMD))){
				pDCxt->strrclState = STRRCL_ST_START;	
				status = A_OK;		
				error = ENET_OK;
			}else{
                pDCxt->strrclBlock = A_FALSE;
            }
            _task_set_priority(_task_get_id(), old_priority, &priority);
		}
#endif        
		break;
#endif /* DRIVER_CONFIG_ENABLE_STORE_RECALL */
	case ATH_SET_PMK:
		if(A_OK != wmi_cmd_start(pWmi, PTR_PMK_SET, WMI_SET_PMK_CMDID, sizeof(WMI_SET_PMK_CMD))){
			error = ENET_ERROR;
		}
		break;
	case ATH_GET_PMK:
  		pDCxt->conn[pDCxt->devId].wpaPmkValid = A_FALSE;

		if(A_OK == wmi_cmd_start(pWmi, NULL, WMI_GET_PMK_CMDID, 0)){
  			/* block here until event arrives from wifi device */
  			CUSTOM_DRIVER_WAIT_FOR_CONDITION(pCxt, &(pDCxt->conn[pDCxt->devId].wpaPmkValid), A_TRUE, 5000);

        	if(pDCxt->conn[pDCxt->devId].wpaPmkValid == A_FALSE){
        		error = ENET_ERROR;
        	}else{
        		A_MEMCPY((uint_8_ptr)param_ptr->data, pDCxt->conn[pDCxt->devId].wpaPmk, WMI_PMK_LEN);
        		pDCxt->conn[pDCxt->devId].wpaPmkValid = A_FALSE;
        	}
        }
		break;
	case ATH_GET_VERSION:
		PTR_VERSION->host_ver = pDCxt->hostVersion;
		PTR_VERSION->target_ver = pDCxt->targetVersion;
		PTR_VERSION->wlan_ver = pDCxt->wlanVersion;
		PTR_VERSION->abi_ver = pDCxt->abiVersion;
		break;
    case ATH_GET_VERSION_STR:
#define PRINT_HOSTFWVERSION(ver) 	(ver&0xF0000000)>>28,(ver&0x0F000000)>>24, \
                               	(ver&0x00FF0000)>>16,(ver&0x0000FFFF)
		sprintf(PTR_VERSION_STR->host_ver, "%d.%d.%d.%d", PRINT_HOSTFWVERSION(pDCxt->hostVersion));
		sprintf(PTR_VERSION_STR->target_ver, "0x%x", pDCxt->targetVersion);
		sprintf(PTR_VERSION_STR->wlan_ver, "%d.%d.%d.%d", PRINT_HOSTFWVERSION(pDCxt->wlanVersion));
		sprintf(PTR_VERSION_STR->abi_ver, "%d", pDCxt->abiVersion);
		break;		
	case ATH_GET_MACADDR:
        A_MEMCPY((int_8_ptr)param_ptr->data, enet_ptr->ADDRESS, ATH_MAC_LEN);
        break;
	case ATH_START_WPS:	
		/* FIXME: there exists the possibility of a race condition if the device has 
		 * sent a WPS event in response to a previous ATH_START_WPS then it becomes 
		 * possible for the driver to misinterpret that event as being the result of
		 * the most recent ATH_START_WPS.  To fix this the wmi command should include 
		 * an ID value that is returned by the WPS event so that the driver can 
		 * accurately match an event from the device to the most recent WPS command.
		 */
		DRIVER_SHARED_RESOURCE_ACCESS_ACQUIRE(pCxt);	
		if(pDCxt->wpsBuf){
			A_NETBUF_FREE(pDCxt->wpsBuf);
			pDCxt->wpsBuf = NULL;
		}
		
		pDCxt->wpsEvent = NULL;
		
		if(pDCxt->wpsState == A_TRUE){
			pDCxt->wpsState = A_FALSE;
		}
		
		DRIVER_SHARED_RESOURCE_ACCESS_RELEASE(pCxt);
		
		A_MEMZERO(&WPS_START, sizeof(WMI_WPS_START_CMD));
		WPS_START.timeout =PTR_WPS_START->timeout_seconds;
        WPS_START.role    = (AP_NETWORK == pDCxt->conn[pDCxt->devId].networkType)?WPS_REGISTRAR_ROLE:WPS_ENROLLEE_ROLE;
		
        if (0x02 == PTR_WPS_START->connect_flag)
        {
            WPS_START.role = WPS_AP_ENROLLEE_ROLE;
        }
		if(PTR_WPS_START->wps_mode == ATH_WPS_MODE_PIN){
		    WPS_START.config_mode = WPS_PIN_MODE;
            WPS_START.wps_pin.pin_length = PTR_WPS_START->pin_length;
			A_MEMCPY(WPS_START.wps_pin.pin, PTR_WPS_START->pin, ATH_WPS_PIN_LEN);
		}else if(PTR_WPS_START->wps_mode == ATH_WPS_MODE_PUSHBUTTON){
			WPS_START.config_mode = WPS_PBC_MODE;
		}else{
			error = A_ERROR;
			break; /* early break */
		}
		if (AP_NETWORK != pDCxt->conn[pDCxt->devId].networkType)
       {
            if (0x08 == PTR_WPS_START->connect_flag)
            {
                /* WPS Request from P2P Module */
                 WPS_START.ctl_flag |= 0x1;
            }
            if(PTR_WPS_START->ssid_info.ssid_len != 0)
            {
              A_MEMCPY(WPS_START.ssid_info.ssid,PTR_WPS_START->ssid_info.ssid,PTR_WPS_START->ssid_info.ssid_len);
              A_MEMCPY(WPS_START.ssid_info.macaddress,PTR_WPS_START->ssid_info.macaddress,6);
              WPS_START.ssid_info.channel = PTR_WPS_START->ssid_info.channel;
              WPS_START.ssid_info.ssid_len = PTR_WPS_START->ssid_info.ssid_len;
            }
             
            /* prevent background scan during WPS */				
            A_MEMCPY(&scan_param_cmd, &default_scan_param, sizeof(WMI_SCAN_PARAMS_CMD));		   
            scan_param_cmd.bg_period        = A_CPU2LE16(0xffff);	
            
            if(A_OK != wmi_cmd_start(pWmi, &scan_param_cmd, 
                                     WMI_SET_SCAN_PARAMS_CMDID, sizeof(WMI_SCAN_PARAMS_CMD)))     		
            {
              error = A_ERROR;
              break;
            }
          }
          else
          {
            WPS_START.ctl_flag |= 0x1;
          }
          
          if(A_OK != wmi_cmd_start(pWmi, &WPS_START, WMI_WPS_START_CMDID, sizeof(WMI_WPS_START_CMD))){		        
            error = A_ERROR;
            break;
        }

		pDCxt->wpsState = A_TRUE;		
		pDCxt->wps_in_progress = A_TRUE;
		break;
	case ATH_AWAIT_WPS_COMPLETION:			
		/* await wps completion */
		
		if(PTR_WPS_GET->dont_block && pDCxt->wpsState == A_TRUE){
			/* the caller does not want to block the task until completion
			 * so if not yet complete then return immediately with appropriate
			 * error code. */
			error = A_PENDING;
			break;
		}
		
		while(pDCxt->wpsState == A_TRUE){
			CUSTOM_DRIVER_WAIT_FOR_CONDITION(pCxt, &(pDCxt->wpsState), A_FALSE, 1000);    		
                }

                /* reach here means wps_in_progress is done whether wps success or fail */
                pDCxt->wps_in_progress = A_FALSE;
		if(pDCxt->wpsBuf != NULL){
			if(pDCxt->wpsEvent != NULL){
				pWpsEv = (WMI_WPS_PROFILE_EVENT*)pDCxt->wpsEvent;
				
				if(pWpsEv->status == WPS_STATUS_SUCCESS){
					CRED_PTR = &(pWpsEv->credential);
					PTR_WPS_GET->error = ATH_WPS_ERROR_SUCCESS;
					A_MEMCPY(PTR_WPS_GET->ssid, CRED_PTR->ssid, CRED_PTR->ssid_len);												
					PTR_WPS_GET->ssid_len = CRED_PTR->ssid_len;	
					PTR_WPS_GET->ssid[CRED_PTR->ssid_len] = '\0';
					PTR_WPS_GET->ap_channel = CRED_PTR->ap_channel;

					if(CRED_PTR->key_len){
						/* the encr_type may have multiple options (bits) set
						 * therefore the order in which we test for support is 
						 * important.  this code tests for AES first and so AES
						 * will be the default when AES is at least one option.
						 */
						if(CRED_PTR->encr_type & WPS_CRED_ENCR_AES){
							PTR_WPS_GET->cipher.ucipher = AES_CRYPT;
							PTR_WPS_GET->cipher.mcipher = AES_CRYPT;
						}else if(CRED_PTR->encr_type & WPS_CRED_ENCR_TKIP){
							PTR_WPS_GET->cipher.ucipher = TKIP_CRYPT;
							PTR_WPS_GET->cipher.mcipher = TKIP_CRYPT;
						}else if(CRED_PTR->encr_type & WPS_CRED_ENCR_WEP){
							PTR_WPS_GET->cipher.ucipher = WEP_CRYPT;
							PTR_WPS_GET->cipher.mcipher = WEP_CRYPT;	
						} 
						/* as with encr_type the auth_type field may contain multiple options.
						 * As such we test for options in a preferred order with WPA2 as top 
						 * followed by WPA and lastly by the 2 WEP options. */
						if(CRED_PTR->auth_type & WPS_CRED_AUTH_WPA2PSK){
							PTR_WPS_GET->sec_type = ENET_MEDIACTL_SECURITY_TYPE_WPA2;			
                            GET_DRIVER_CXT(pCxt)->securityType =ENET_MEDIACTL_SECURITY_TYPE_WPA2;
							A_MEMCPY(PTR_WPS_GET->u.passphrase, CRED_PTR->key, CRED_PTR->key_len);
							PTR_WPS_GET->u.passphrase[CRED_PTR->key_len] = '\0';
						}else if(CRED_PTR->auth_type & WPS_CRED_AUTH_WPAPSK){
							PTR_WPS_GET->sec_type = ENET_MEDIACTL_SECURITY_TYPE_WPA;				
                            GET_DRIVER_CXT(pCxt)->securityType = ENET_MEDIACTL_SECURITY_TYPE_WPA;
							A_MEMCPY(PTR_WPS_GET->u.passphrase, CRED_PTR->key, CRED_PTR->key_len);
							PTR_WPS_GET->u.passphrase[CRED_PTR->key_len] = '\0';
						}else if((CRED_PTR->auth_type & WPS_CRED_AUTH_OPEN) || 
								 (CRED_PTR->auth_type & WPS_CRED_AUTH_SHARED)){
							PTR_WPS_GET->sec_type = ENET_MEDIACTL_SECURITY_TYPE_WEP;		
                            GET_DRIVER_CXT(pCxt)->securityType = ENET_MEDIACTL_SECURITY_TYPE_WEP;
							A_MEMCPY(PTR_WPS_GET->u.wepkey, CRED_PTR->key, CRED_PTR->key_len);
							PTR_WPS_GET->u.wepkey[CRED_PTR->key_len] = '\0';
							PTR_WPS_GET->key_index = CRED_PTR->key_idx;
						}else{
							/* this driver does not support these modes: WPS_CRED_AUTH_WPA || WPS_CRED_AUTH_WPA2 */
							error = A_ERROR;
                                                        pDCxt->wps_in_progress = A_FALSE;
							break;
						}																								
					}else{
						/* no security */
						PTR_WPS_GET->sec_type = ENET_MEDIACTL_SECURITY_TYPE_NONE;
                                                GET_DRIVER_CXT(pCxt)->securityType = ENET_MEDIACTL_SECURITY_TYPE_NONE;
						PTR_WPS_GET->cipher.ucipher = NONE_CRYPT;
						PTR_WPS_GET->cipher.mcipher = NONE_CRYPT;							
					}	
				}
				else if(pWpsEv->status == WPS_STATUS_FAILURE)
				{
					if(pWpsEv->error_code < sizeof(const_wps_error_code_translation)/sizeof(A_UINT8)){
						PTR_WPS_GET->error = const_wps_error_code_translation[pWpsEv->error_code];	
					}else{
                        PTR_WPS_GET->error = const_wps_error_code_translation[pWpsEv->error_code];
					}
		                      pDCxt->wps_in_progress = A_FALSE;
				}
				pDCxt->wpsEvent = NULL;
			}
			
			A_NETBUF_FREE(pDCxt->wpsBuf);
			pDCxt->wpsBuf = NULL;
            /* ensure that the device is disconnected from the wps session */
            if (AP_NETWORK == pDCxt->conn[pDCxt->devId].networkType)
			{
		                pDCxt->wps_in_progress = A_FALSE;
				break;
			}
            
#if ENABLE_P2P_MODE
            if (1 == pDCxt->p2p_avail)
            {
                break;
            }
#endif
            if(wmi_cmd_start(pDCxt->pWmiCxt, NULL, WMI_DISCONNECT_CMDID, 0) != A_OK){
              pDCxt->wps_in_progress = A_FALSE;
              error = A_ERROR;
              break;
            }
		}else{
		        pDCxt->wps_in_progress = A_FALSE;
			error = A_ERROR;
		}
		break;		
	case ATH_MAC_TX_RAW:
			/* FIXME: need away to hold the buffer until the transmit is complete. for
			 * now we block the thread until the transmit completes. */
			if((PCB_PTR = A_NETBUF_ALLOC(0)) == NULL){
	    		error = ENETERR_ALLOC_PCB;
	    		break;
	    	}

	    	A_NETBUF_APPEND_FRAGMENT(PCB_PTR, PTR_MAC_TX->buffer, PTR_MAC_TX->length);

	    	//ensure there is enough headroom to complete the tx operation
	    	if (A_NETBUF_HEADROOM(PCB_PTR) < sizeof(WMI_DATA_HDR) +
	    						HTC_HDR_LENGTH + WMI_MAX_TX_META_SZ) {
	            error = ENETERR_ALLOC_PCB;
	    		A_NETBUF_FREE(PCB_PTR);
	         	break;
	        }

			if(A_OK != TxRawPacket(pCxt, (A_VOID*)PCB_PTR, &(PTR_MAC_TX->params))){
				A_NETBUF_FREE(PCB_PTR);
				error = ENET_ERROR;
				break;
			}
		break;
	case ATH_SET_CHANNEL:
	        pDCxt->conn[pDCxt->devId].channelHint = (A_UINT16)(*(PTR_SET_CHAN));
            SET_CHANNEL_PARAM.channel = A_CPU2LE16(pDCxt->conn[pDCxt->devId].channelHint);

            if(A_OK != wmi_cmd_start(pWmi, &SET_CHANNEL_PARAM,
                    WMI_SET_CHANNEL_CMDID, sizeof(A_UINT16))){
				error = ENET_ERROR;
				break;
			}
		break;
	case ATH_SET_AGGREGATION:
#if WLAN_CONFIG_11N_AGGR_SUPPORT
		pDCxt->txAggrTidMask = PTR_SET_AGGR->txTIDMask;
		pDCxt->rxAggrTidMask = PTR_SET_AGGR->rxTIDMask;
		AGGR_PARAM.tx_allow_aggr = A_CPU2LE16(pDCxt->txAggrTidMask);
		AGGR_PARAM.rx_allow_aggr = A_CPU2LE16(pDCxt->rxAggrTidMask);
		wmi_cmd_start(pWmi, &AGGR_PARAM, WMI_ALLOW_AGGR_CMDID, sizeof(WMI_ALLOW_AGGR_CMD));
#endif /* WLAN_CONFIG_11N_AGGR_SUPPORT */
		break;
	case ATH_ASSERT_DUMP:
		/* setup driver thread to perform operation on behalf of this thread.
		 * this will avoid accessing HCD by multiple threads. */
		if(pDCxt->asynchRequest == NULL){
			pDCxt->asynchRequest = config_dump_target_assert_info;
			CUSTOM_DRIVER_WAKE_DRIVER(pCxt);
		}
		break;
	case ATH_PROGRAM_FLASH:
		status = BMIWriteMemory(pCxt, PTR_FLASH_CMD->load_addr, (A_UCHAR *)PTR_FLASH_CMD->buffer, PTR_FLASH_CMD->length);

		if(status == A_OK){
			PTR_FLASH_CMD->result = 1;
        }else{
			PTR_FLASH_CMD->result = status;
        }
		break;
	case ATH_EXECUTE_FLASH:

		PARAM = A_CPU2LE32(PTR_FLASH_CMD->result);
		status = BMIExecute(pCxt, PTR_FLASH_CMD->load_addr, &PARAM);

		if(status == A_OK){
			PTR_FLASH_CMD->result = PARAM;
        }else{
			PTR_FLASH_CMD->result = 0;
        }
		break;
	case ATH_GET_TX_STATUS:
		PTR_TX_STATUS_CMD->status = Api_TxGetStatus(pCxt);
		break;
	case ATH_GET_RATE:
	    {
		    if(A_OK != wmi_cmd_start(pWmi, NULL, WMI_GET_BITRATE_CMDID, 0))
		    {
		    	error = ENET_ERROR;
		    }
	    }
	    break;
	case ATH_SET_PROMISCUOUS_MODE:
		pDCxt->promiscuous_mode = (PTR_PROM_MODE->enable==0)? 0:1;
		GET_DRIVER_CXT(pCxt)->promiscuous_cb = (A_VOID*)PTR_PROM_MODE->cb;
        SET_PROM_PARAM.enable = pDCxt->promiscuous_mode;
        SET_PROM_PARAM.filters = PTR_PROM_MODE->filter_flags;
        A_MEMCPY(SET_PROM_PARAM.srcAddr, &(PTR_PROM_MODE->src_mac[0]), ATH_MAC_LEN);
        A_MEMCPY(SET_PROM_PARAM.dstAddr, &(PTR_PROM_MODE->dst_mac[0]), ATH_MAC_LEN);

        if(A_OK != wmi_cmd_start(pWmi, &SET_PROM_PARAM,
                WMI_SET_FILTERED_PROMISCUOUS_MODE_CMDID,
                sizeof(WMI_SET_FILTERED_PROMISCUOUS_MODE_CMD))){
            error = ENET_ERROR;
        }
		break;
	case ATH_GET_REG_DOMAIN:
		PTR_REG_DOMAIN_CMD->domain = GET_DRIVER_COMMON(pCxt)->regCode;
		break;
	case ATH_START_SCAN_EXT:
		GET_DRIVER_CXT(pCxt)->extended_scan = 1;
		error = scan_setup(pCxt, pWmi);
		break;
	case ATH_GET_SCAN_EXT:
		wait_scan_done(pCxt, pWmi);
		PTR_EXT_SCAN_CMD->scan_list = (ATH_SCAN_EXT*)GET_DRIVER_CXT(pCxt)->pScanOut;
		PTR_EXT_SCAN_CMD->num_entries = GET_DRIVER_CXT(pCxt)->scanOutCount;
		break;
    case ATH_GET_LAST_ERROR:
		*PTR_LAST_ERROR = last_driver_error;
		break;
#if ENABLE_AP_MODE
	//case ATH_SET_EZCONN:
	//    pDCxt->ezConnectCmd = 1;
	//    break;
	case ATH_CONFIG_AP:
    {
        ATH_AP_PARAM_STRUCT *ap_param = (ATH_AP_PARAM_STRUCT *)param_ptr->data;
        if (AP_SUB_CMD_WPS_FLAG == ap_param->cmd_subset)
        {
            pDCxt->apmodeWPS = 1;

            break;
        }
		wmi_ap_set_param(pWmi, (A_VOID *)param_ptr->data);
    }
    break;
#endif /* ENABLE_AP_MODE */
	case ATH_GET_CHANNEL:
		*((A_UINT16*)param_ptr->data) = pDCxt->conn[pDCxt->devId].bssChannel;
		break;
	case ATH_GET_CHANNELHINT:
		*((A_UINT16*)param_ptr->data) = pDCxt->conn[pDCxt->devId].channelHint;
		break;                
#if ENABLE_P2P_MODE
	case ATH_SET_P2P_CALLBACK:
                
 		 if(GET_DRIVER_CXT(pCxt)->p2pEvtState == A_TRUE){

			if(CUSTOM_DRIVER_WAIT_FOR_CONDITION(pCxt, &(GET_DRIVER_CXT(pCxt)->p2pEvtState), A_FALSE, 5000) != A_OK)
                        {
				GET_DRIVER_CXT(pCxt)->p2pEvtState= A_FALSE;
                          status = A_ERROR;
                          break;
                        }
			GET_DRIVER_CXT(pCxt)->p2pEvtState= A_FALSE;

            }
                A_MEMCPY(param_ptr->data, GET_DRIVER_CXT(pCxt)->pScanOut, P2P_EVT_BUF_SIZE);
                     
		break;
    case ATH_P2P_EVENT_CALLBACK:
            if(GET_DRIVER_CXT(pCxt)->p2pevtflag == A_TRUE)
            {
              A_MEMCPY(param_ptr->data, GET_DRIVER_CXT(pCxt)->pScanOut, 516);
              GET_DRIVER_CXT(pCxt)->p2pevtflag = A_FALSE;
            }
            else
            {
              status = A_ERROR;
            }
      break;
    case ATH_P2P_APMODE:
        {
            p2pGroup = (WMI_P2P_GRP_INIT_CMD *)param_ptr->data;

            pDCxt->p2p_avail = 1;
            pDCxt->conn[pDCxt->devId].networkType = AP_NETWORK;

            if (p2pGroup->group_formation) {
                wmi_cmd_start(pWmi,p2pGroup, WMI_P2P_GRP_INIT_CMDID, sizeof(WMI_P2P_GRP_INIT_CMD));
            }

            if (Api_ConnectWiFi(pCxt) != A_OK){
				status = A_ERROR;
            }
            pDCxt->userPwrMode = MAX_PERF_POWER;
        }
        break;
    case ATH_P2P_APMODE_PP:
        {
#if 1
            A_MEMCPY(((struct wmi_t *)pWmi)->apPassPhrase.passphrase,PTR_PASS_PHRASE->passphrase,PTR_PASS_PHRASE->passphrase_len);
           ((struct wmi_t *)pWmi)->apPassPhrase.passphrase_len = PTR_PASS_PHRASE->passphrase_len;
           A_MEMCPY(((struct wmi_t *)pWmi)->apPassPhrase.ssid,PTR_PASS_PHRASE->ssid,PTR_PASS_PHRASE->ssid_len);
           ((struct wmi_t *)pWmi)->apPassPhrase.ssid_len = PTR_PASS_PHRASE->ssid_len;
            //if (((struct wmi_t *)pWmi)->apPassPhrase.ssid_len != 0) {
                if(A_OK != wmi_cmd_start(pWmi, &((struct wmi_t *)pWmi)->apPassPhrase,
                                WMI_SET_PASSPHRASE_CMDID, sizeof(WMI_SET_PASSPHRASE_CMD))){
    				status = A_ERROR;
    			}

            //}
#endif
        }
        break;
    case ATH_P2P_FIND:
        {

             pDCxt->p2p_avail = 1;
             if(wmi_cmd_start(pWmi, (WMI_P2P_FIND_CMD *)param_ptr->data, WMI_P2P_FIND_CMDID, sizeof(WMI_P2P_FIND_CMD)) != A_OK)
             {
                 status = A_ERROR;
             }
        }
        break;
    case ATH_P2P_CONNECT:
        {
#if 0
            if(wmi_p2p_go_neg_start(pWmi, (WMI_P2P_GO_NEG_START_CMD *)param_ptr->data) != A_OK)
            {
                 status = A_ERROR;
            }
#endif
        }
        break;
    case ATH_P2P_CONNECT_CLIENT:
        {
		   WMI_P2P_FW_CONNECT_CMD_STRUCT p2p_connect_param;
           GET_DRIVER_CXT(pCxt)->p2pEvtState = A_TRUE;
		   A_MEMZERO(&p2p_connect_param, sizeof(WMI_P2P_FW_CONNECT_CMD_STRUCT));
		   A_MEMCPY(&p2p_connect_param, (WMI_P2P_FW_CONNECT_CMD_STRUCT *)param_ptr->data, sizeof(WMI_P2P_FW_CONNECT_CMD_STRUCT));
		   if((p2p_connect_param.go_intent >= 10) || (p2p_connect_param.go_intent == 15))
		   {
				pDCxt->conn[pDCxt->devId].networkType = AP_NETWORK;
		   }
		   if((p2p_connect_param.go_intent <= 9) || (p2p_connect_param.go_intent == 0))
		   {
				pDCxt->conn[pDCxt->devId].networkType = INFRA_NETWORK;
		   }
           if(wmi_cmd_start(pWmi, (WMI_P2P_FW_CONNECT_CMD_STRUCT *)param_ptr->data, WMI_P2P_CONNECT_CMDID, sizeof(WMI_P2P_FW_CONNECT_CMD_STRUCT)) != A_OK)
           {
             status = A_ERROR;
           }
        }
        break;
    case ATH_P2P_LISTEN:
        {
            A_UINT32 time_out_val;
            A_MEMCPY(&time_out_val, param_ptr->data, sizeof(A_UINT32));
            if(wmi_cmd_start(pWmi, &time_out_val, WMI_P2P_LISTEN_CMDID, sizeof(time_out_val)) != A_OK)
            {
                status = A_ERROR;
            }
        }
        break;
    case ATH_P2P_CANCEL:
        {

            if(wmi_cmd_start(pWmi, NULL, WMI_P2P_CANCEL_CMDID, 0) != A_OK)
            {
                status = A_ERROR;
            }
        }
        break;
    case ATH_P2P_STOP:
        {
            if(wmi_cmd_start(pWmi, NULL, WMI_P2P_STOP_FIND_CMDID, 0) != A_OK)
            {
                status = A_ERROR;
            }
        }
        break;
    case ATH_P2P_NODE_LIST:
        {
            GET_DRIVER_CXT(pCxt)->p2pEvtState = A_TRUE;
            if(wmi_cmd_start(pWmi, NULL, WMI_P2P_GET_NODE_LIST_CMDID, 0) != A_OK)
            {
                status = A_ERROR;
            }
            
        }
        break;
    case ATH_P2P_SET_CONFIG:
        {

            if(wmi_cmd_start(pWmi, (WMI_P2P_FW_SET_CONFIG_CMD *)param_ptr->data, WMI_P2P_SET_CONFIG_CMDID, sizeof(WMI_P2P_FW_SET_CONFIG_CMD))  != A_OK)
            {
                status = A_ERROR;
            }
        }
        break;
    case ATH_P2P_WPS_CONFIG:
        {

            if(wmi_cmd_start(pWmi, (WMI_WPS_SET_CONFIG_CMD *)param_ptr->data, WMI_WPS_SET_CONFIG_CMDID, sizeof(WMI_WPS_SET_CONFIG_CMD)) != A_OK)
            {
                status = A_ERROR;
            }
        }
        break;
    case ATH_P2P_AUTH:
        {

           if(wmi_cmd_start(pWmi, (WMI_P2P_FW_CONNECT_CMD_STRUCT *)param_ptr->data, WMI_P2P_AUTH_GO_NEG_CMDID, sizeof(WMI_P2P_FW_CONNECT_CMD_STRUCT)) != A_OK)
           {
             status = A_ERROR;
           }
        }
        break;
    case ATH_P2P_DISC_REQ:
        {
           if(wmi_cmd_start(pWmi, (WMI_P2P_FW_PROV_DISC_REQ_CMD *)param_ptr->data, WMI_P2P_FW_PROV_DISC_REQ_CMDID, sizeof(WMI_P2P_FW_PROV_DISC_REQ_CMD)) != A_OK)
           {
             status = A_ERROR;
           }
           GET_DRIVER_CXT(pCxt)->p2pEvtState = A_TRUE;
        }
        break;
    case ATH_P2P_SET:
        {
           if(wmi_cmd_start(pWmi, (WMI_P2P_SET_CMD *)param_ptr->data, WMI_P2P_SET_CMDID, sizeof(WMI_P2P_SET_CMD)) != A_OK)
           {
             status = A_ERROR;
           }
        }
        break;
#if 1
    case ATH_P2P_INVITE_AUTH:
        {
           GET_DRIVER_CXT(pCxt)->p2pEvtState = A_TRUE;
           if(wmi_cmd_start(pWmi,(WMI_P2P_FW_INVITE_REQ_RSP_CMD *)param_ptr->data, WMI_P2P_INVITE_REQ_RSP_CMDID, sizeof(WMI_P2P_FW_INVITE_REQ_RSP_CMD)) != A_OK)
           {
             status = A_ERROR;
           }
        }
        break;
    case ATH_P2P_PERSISTENT_LIST:
        {
          GET_DRIVER_CXT(pCxt)->p2pEvtState = A_TRUE;
           if(wmi_cmd_start(pWmi, NULL, WMI_P2P_LIST_PERSISTENT_NETWORK_CMDID, 0) != A_OK)
           {
               status = A_ERROR;
           }          
        }
        break;
    case ATH_P2P_INVITE:
        {

           p2pInvite        =  (WMI_P2P_INVITE_CMD *)param_ptr->data;
           //p2pInvite->is_persistent = 1; // for now we do invitation for persistent clients
           p2pInvite->dialog_token  = 1;
           if(wmi_cmd_start(pWmi,(WMI_P2P_INVITE_CMD *)param_ptr->data, WMI_P2P_INVITE_CMDID, sizeof(WMI_P2P_INVITE_CMD)) != A_OK)
           {
               status = A_ERROR;
           }
        }
        break;
#endif
    case ATH_P2P_JOIN:
        {
            pP2PConnect =  (WMI_P2P_FW_CONNECT_CMD_STRUCT *)param_ptr->data;
            A_MEMZERO(&WPS_START, sizeof(WMI_WPS_START_CMD));
            WPS_START.timeout   = 30;
            WPS_START.role      = WPS_ENROLLEE_ROLE;

#if ENABLE_SCC_MODE            
            int num_dev = WLAN_NUM_OF_DEVICES;
            
            if((num_dev > 1) && (pDCxt->conn[1].isConnected == A_TRUE)&& (pP2PConnect->go_oper_freq == 0)){
                error = A_ERROR;
                break;                
            }            
            if((num_dev > 1) && (pDCxt->conn[1].isConnected == A_TRUE)){
              if(pP2PConnect->go_oper_freq != pDCxt->conn[1].channelHint){
                error = A_ERROR;
                break;                  
              }
            }
#endif /* ENABLE_SCC_MODE */            
            if(pP2PConnect->wps_method == WPS_PBC) {
                WPS_START.config_mode = WPS_PBC_MODE;
            }else if(pP2PConnect->wps_method == WPS_PIN_DISPLAY ||
                     pP2PConnect->wps_method == WPS_PIN_KEYPAD) {

                WPS_START.config_mode = WPS_PIN_MODE;
                A_MEMCPY(WPS_START.wps_pin.pin,p2p_key_val.wps_pin,WPS_PIN_LEN);
                WPS_START.wps_pin.pin_length = WPS_PIN_LEN;
            } else {

                status = A_ERROR;
                break; /* early break */
            }
            if(pP2PConnect->peer_go_ssid.ssidLength != 0)
            {
                memcpy(WPS_START.ssid_info.ssid,pP2PConnect->peer_go_ssid.ssid,pP2PConnect->peer_go_ssid.ssidLength);
                memcpy(WPS_START.ssid_info.macaddress,pP2PConnect->peer_addr,6);
                WPS_START.ssid_info.ssid_len = pP2PConnect->peer_go_ssid.ssidLength ;
            }

            /* prevent background scan during WPS */
			A_MEMCPY(&scan_param_cmd, &default_scan_param, sizeof(WMI_SCAN_PARAMS_CMD));
		    scan_param_cmd.bg_period        = A_CPU2LE16(0xffff);
		    wmi_cmd_start(pWmi, &scan_param_cmd, WMI_SET_SCAN_PARAMS_CMDID, sizeof(WMI_SCAN_PARAMS_CMD));
            WPS_START.ctl_flag |= 0x1;
            wmi_cmd_start(pWmi, &WPS_START, WMI_WPS_START_CMDID, sizeof(WMI_WPS_START_CMD));
        }
        break;
#if 0
    case ATH_P2P_INV_CONNECT:
    {
      pInvitation_connect_param =  (WMI_PERSISTENT_MAC_LIST *)param_ptr->data;
      
      p2p_invite_conn_cmd.ssidLength          = strlen(pInvitation_connect_param->ssid);
      p2p_invite_conn_cmd.networkType         = INFRA_NETWORK;
      p2p_invite_conn_cmd.dot11AuthMode       = OPEN_AUTH;
      p2p_invite_conn_cmd.authMode            = WPA2_PSK_AUTH;
      p2p_invite_conn_cmd.pairwiseCryptoType  = AES_CRYPT;
      p2p_invite_conn_cmd.pairwiseCryptoLen   = 0;
      p2p_invite_conn_cmd.groupCryptoType     = AES_CRYPT;
      p2p_invite_conn_cmd.groupCryptoLen      = 0;
      p2p_invite_conn_cmd.channel             = 0;
      p2p_invite_conn_cmd.ctrl_flags          = A_CPU2LE32(DEFAULT_CONNECT_CTRL_FLAGS |CONNECT_DO_WPA_OFFLOAD | CONNECT_IGNORE_WPAx_GROUP_CIPHER);
      
      if (pInvitation_connect_param->ssid != NULL) {
        A_MEMCPY(p2p_invite_conn_cmd.ssid, pInvitation_connect_param->ssid, strlen(pInvitation_connect_param->ssid));
      }
      
      status = wmi_cmd_start(pDCxt->pWmiCxt, (A_VOID*)&p2p_invite_conn_cmd, 
                             WMI_CONNECT_CMDID, sizeof(WMI_CONNECT_CMD));
      
      if(status != A_OK){
        break;
      }      

    }
    break;
#endif
    case ATH_P2P_JOIN_PROFILE:
    {
        pP2PConnect =  (WMI_P2P_FW_CONNECT_CMD_STRUCT *)param_ptr->data;
        A_MEMZERO(&JOIN_CONN_PROF, sizeof(WMI_P2P_FW_CONNECT_CMD_STRUCT));
        A_MEMCPY(&JOIN_CONN_PROF,pP2PConnect,sizeof(WMI_P2P_FW_CONNECT_CMD_STRUCT));
        wmi_cmd_start(pWmi,&JOIN_CONN_PROF, WMI_P2P_SET_JOIN_PROFILE_CMDID, sizeof(WMI_P2P_FW_CONNECT_CMD_STRUCT));
    }
    break;
    case ATH_P2P_SWITCH:
    {
             wmi_cmd_start(pWmi, (WMI_P2P_SET_PROFILE_CMD *)param_ptr->data, WMI_P2P_SET_PROFILE_CMDID, sizeof(WMI_P2P_SET_PROFILE_CMD));
             GET_DRIVER_CXT(pCxt)->p2pEvtState = A_FALSE;
             GET_DRIVER_CXT(pCxt)->p2pevtflag = A_FALSE;
             //TODO: p2p_avail need to set to 0 after p2p off
    }
    break;
#if 1
    case ATH_P2P_SET_NOA:
    {
        wmi_p2p_set_noa(pWmi,(WMI_NOA_INFO_STRUCT *)param_ptr->data);
    }
    break;
    case ATH_P2P_SET_OPPPS:
    {
        wmi_p2p_set_oppps(pWmi,(WMI_OPPPS_INFO_STRUCT *)param_ptr->data);
    }
    break;
    case ATH_P2P_SDPD:
    {
		wmi_sdpd_send_cmd(pWmi, (WMI_P2P_SDPD_TX_CMD *)param_ptr->data);
    }
    break;
#endif
#endif
    case ATH_ONOFF_GTX:
    {
        GTX_PARAM.enable = *((A_UINT8 *)param_ptr->data);
        GTX_PARAM.nextProbeCount = 5; //if 5 packets are successfully transmitted reduce the power level
        GTX_PARAM.forceBackOff = 0;
        if(A_OK != wmi_cmd_start(pWmi, &GTX_PARAM, WMI_GREENTX_PARAMS_CMDID, sizeof(WMI_GREENTX_PARAMS_CMD))){
            error = ENET_ERROR;
        }
    }
    break;

    case ATH_ONOFF_LPL:
    {
        //WMI_LPL_FORCE_ENABLE_CMD lplParams = {0};
        LPL_PARAM.lplPolicy = *((A_UINT8*)PTR_LPL);

        if(A_OK != wmi_cmd_start(pWmi, &LPL_PARAM, WMI_LPL_FORCE_ENABLE_CMDID, sizeof(WMI_LPL_FORCE_ENABLE_CMD))){
            error = ENET_ERROR;
        }
    }
    break;
    case ATH_SET_TX_PWR_SCALE:
      {
         if(A_OK != wmi_cmd_start(pWmi, (WMI_SET_TX_POWER_SCALE_CMD *)param_ptr->data, WMI_SET_TX_POWER_SCALE_CMDID, sizeof(WMI_SET_TX_POWER_SCALE_CMD))){
            error = ENET_ERROR;
         }        
      }
      break;
    case ATH_SET_RATE:
    {
        if(A_OK != wmi_cmd_start(pWmi, (WMI_BIT_RATE_CMD *)param_ptr->data, WMI_SET_BITRATE_CMDID, sizeof(WMI_BIT_RATE_CMD))){
            error = ENET_ERROR;
        }        
      }
      break;
	case ATH_PROGRAM_MAC_ADDR:
	{
		if(PTR_MAC_ADDR==NULL){
			error = ENET_ERROR;
		}else{
#if DRIVER_CONFIG_PROGRAM_MAC_ADDR
		if(A_OK != Api_ProgramMacAddress(pCxt, PTR_MAC_ADDR->addr, param_ptr->length, &PTR_MAC_ADDR->result)){
			error = ENET_ERROR;
		}
#else
		PTR_MAC_ADDR->result = ATH_PROGRAM_MAC_RESULT_DRIVER_FAILED;
		error = ENET_ERROR;//command not supported by this build
#endif
		}
	}
	break;

    case ATH_GPIO_CMD:
    {
      	if(A_OK != wmi_cmd_start(pWmi, param_ptr->data, WMI_EXTENSION_CMDID, param_ptr->length)){
            status = A_ERROR;
        }
        break; 
    }
    break;
    case ATH_PFM_CMD:
		pDCxt->pfmDone = A_FALSE;

      	if(A_OK != wmi_cmd_start(pWmi, param_ptr->data, WMI_PFM_GET_CMDID, param_ptr->length)){
            status = A_ERROR;
        }
   			/* block until data return */	
    	CUSTOM_DRIVER_WAIT_FOR_CONDITION(pCxt, &(pDCxt->pfmDone), A_TRUE, 5000);
        break; 
   case ATH_SET_POWER:
        if(A_OK != Api_SetPowerMode(pCxt,(POWER_MODE *)param_ptr->data)){
            status = A_ERROR;
        }
        break;
    case ATH_DSET_READ_CMD:
      	if(A_OK != wmi_cmd_start(pWmi, param_ptr->data, WMI_HOST_DSET_LARGE_READ_CMDID, param_ptr->length)){
            status = A_ERROR;
        }
        break; 
    case ATH_DSET_WRITE_CMD:
      	if(A_OK != wmi_cmd_start(pWmi, param_ptr->data, WMI_HOST_DSET_LARGE_WRITE_CMDID, param_ptr->length)){
            status = A_ERROR;
        }
        break; 
    case ATH_DSET_OP_CMD:
    {
        struct WMIX_DSET_OP_SET_CMD {
            WMIX_DSET_CMD_HDR cmd;
            uint32_t      dset_id;
	    } *pCmd;
	    HOST_DSET_HANDLE *pDsetHandle;

        pDCxt->dset_op_done = A_FALSE;

      	if(A_OK != wmi_cmd_start(pWmi, param_ptr->data, WMI_DSET_OP_CMDID, param_ptr->length)){
            status = A_ERROR;
        }

		pCmd = (struct WMIX_DSET_OP_SET_CMD *)param_ptr->data;
        pDsetHandle = dset_find_handle(pCmd->dset_id);

        if (pDsetHandle->cb == NULL)
		{
			 if (pDCxt->dset_op_done == A_FALSE)
   			/* block until data return */	
    			 CUSTOM_DRIVER_WAIT_FOR_CONDITION(pCxt, &(pDCxt->dset_op_done), A_TRUE, 5000);
			 if (pDCxt->dset_op_done == A_TRUE)
				 status = A_OK;
			 else
				 status = A_ERROR;
		}
		else
			status = A_PENDING;
    }
        break;
	default:
		if(ath_custom_mediactl.ath_ioctl_handler_ext != NULL){
			error = ath_custom_mediactl.ath_ioctl_handler_ext(enet_ptr, param_ptr);
		}else{
			error = ENET_ERROR;
		}
		break;
	}


	return  error;
}

uint_32 Custom_Api_Mediactl
   (
     ENET_CONTEXT_STRUCT_PTR enet_ptr,
     uint_32 command_id,
     pointer inout_param
   )
{
    A_UINT32 i,ii;
	A_UINT8 val;
    A_UINT8 devId;
	A_UINT32 error = ENET_OK;
    A_VOID *pCxt, *pWmi;
	A_DRIVER_CONTEXT *pDCxt;
	ENET_MEDIACTL_PARAM_PTR param_ptr = (ENET_MEDIACTL_PARAM_PTR)inout_param;
#define PTR_SCAN_INPUT 	((ENET_SCAN_INPUT*)inout_param)
#define PTR_DEV_MODE 	((uint_32_ptr)inout_param)
#define PTR_ESSID 		((ENET_ESSID*)inout_param)
#define PTR_SECTYPE 	((uint_32_ptr)inout_param)
#define PTR_FREQ 		(uint_32_ptr)(param_ptr->data)
#define PTR_RTS 		((uint_32_ptr)inout_param)
#define PTR_PASSPHRASE 	((ENET_MEDIACTL_PARAM*)inout_param)
#define PTR_SCAN_OUT	((ENET_SCAN_LIST*)inout_param)
#define PTR_WEP_IN		((ENET_WEPKEYS*)inout_param)
#define PTR_POWER_IN	((ENET_MEDIACTL_PARAM*)inout_param)
#define PTR_POWER_OUT	((ENET_MEDIACTL_PARAM*)inout_param)
#define PTR_ATH_IOCTL	((ATH_IOCTL_PARAM_STRUCT_PTR)inout_param)

	union{
		WMI_SET_PASSPHRASE_CMD passCmd;
		WMI_SET_RTS_CMD rtsCmd;
	}local;


	if (enet_ptr == NULL)
	{
		error = ENET_ERROR;
		goto MEDIACTL_DONE;
	}
	/* ATH_CHIP_STATE is a special command to bring
	 * driver + wifi device
	 * up or down without officially shutting down the
	 * driver. This allows a task to shutdown the chip
	 * but prevents other tasks from having invalid enet_ptr's
	 */
	if(command_id == ENET_MEDIACTL_VENDOR_SPECIFIC &&
		PTR_ATH_IOCTL->cmd_id == ATH_CHIP_STATE){
		error = chip_state_ctrl(enet_ptr, *((uint_32*)PTR_ATH_IOCTL->data));

		if(error != ENET_OK){
			config_dump_target_assert_info(enet_ptr->MAC_CONTEXT_PTR);
		}

		goto MEDIACTL_DONE;
	}

	if((pCxt = enet_ptr->MAC_CONTEXT_PTR) == NULL)
	{
	   	error = ENET_ERROR;
		goto MEDIACTL_DONE;
	}

	pDCxt = GET_DRIVER_COMMON(pCxt);
	pWmi = pDCxt->pWmiCxt;
    //devId = pDCxt->devId;
    devId = enet_ptr->PARAM_PTR->ENET_IF->PHY_NUMBER;

	if(command_id == ENET_MEDIACTL_IS_INITIALIZED){
		*((uint_32_ptr)inout_param) = (A_UINT32)((pDCxt->wmiReady == A_TRUE)? 1: 0);
		goto MEDIACTL_DONE;
	}
	else if(pDCxt->wmiReady == A_FALSE && ath_custom_init.skipWmi == 0){
		/* no other ioctl's allowed until driver is properly initialized */
		error = ENETERR_INIT_FAILED;
		goto MEDIACTL_DONE;
	}


	if(A_OK != Api_DriverAccessCheck(pCxt, 1, ACCESS_REQUEST_IOCTL)){
        return A_ERROR;
    }

	switch (command_id)
	{

	case ENET_MEDIACTL_VENDOR_SPECIFIC:
	    error = ath_ioctl_handler(enet_ptr, PTR_ATH_IOCTL);
	    break;
   	case ENET_SET_MEDIACTL_COMMIT:
   		if(pDCxt->conn[devId].ssidLen == 0){
   			/* a zero length ssid + a COMMIT command is interpreted as a
   			 * request from the caller to disconnect.
   			 */
   			Api_DisconnectWiFi(pCxt);
   			GET_DRIVER_CXT(pCxt)->securityType = ENET_MEDIACTL_SECURITY_TYPE_NONE;
   		}else {
                        if(Api_ConnectWiFi(pCxt) != A_OK){
                            error = ENET_ERROR;
                        }
   	  	}
	        pDCxt->wps_in_progress = A_FALSE;
   	  	break;
   	case ENET_SET_MEDIACTL_MODE:
   		switch(*PTR_DEV_MODE)
   		{
   		case ENET_MEDIACTL_MODE_INFRA:
   			pDCxt->conn[devId].networkType = INFRA_NETWORK;
   			break;
   		case ENET_MEDIACTL_MODE_ADHOC:
   			pDCxt->conn[devId].networkType = ADHOC_NETWORK;
   			break;
#if ENABLE_AP_MODE
   		case ENET_MEDIACTL_MODE_MASTER:
   			pDCxt->conn[devId].networkType = AP_NETWORK;
   			break;
#endif
   		default:
   			error = ENETERR_INVALID_INIT_PARAM;
   			break;
   		}
   	  	break;
   	case ENET_SET_MEDIACTL_ESSID:
   		if(PTR_ESSID->length < 33){
   			pDCxt->conn[devId].ssidLen = (A_INT32)PTR_ESSID->length;
   			if(pDCxt->conn[devId].ssidLen){
   				A_MEMCPY(pDCxt->conn[devId].ssid, PTR_ESSID->essid, (A_UINT32)pDCxt->conn[devId].ssidLen);
   			}else{
   				//clear ssid; this is the first step to disconnect
   				A_MEMZERO(pDCxt->conn[devId].ssid, 32);
   			}
   		}else{
   			error = ENETERR_INVALID_INIT_PARAM;
   		}

   	  	break;
   	case ENET_SET_MEDIACTL_SEC_TYPE:
   	    GET_DRIVER_CXT(pCxt)->securityType = (A_UINT8)(*PTR_SECTYPE);
   		switch(*PTR_SECTYPE)
   		{
   		case ENET_MEDIACTL_SECURITY_TYPE_NONE:
   			pDCxt->conn[devId].wpaAuthMode = NONE_AUTH;
   			pDCxt->conn[devId].wpaPairwiseCrypto = NONE_CRYPT;
   			pDCxt->conn[devId].wpaGroupCrypto = NONE_CRYPT;
   			pDCxt->conn[devId].dot11AuthMode = OPEN_AUTH;
   			pDCxt->conn[devId].connectCtrlFlags &= ~CONNECT_DO_WPA_OFFLOAD;
   			break;
   		case ENET_MEDIACTL_SECURITY_TYPE_WEP:
   			pDCxt->conn[devId].wpaAuthMode = NONE_AUTH;
   			pDCxt->conn[devId].wpaPairwiseCrypto = WEP_CRYPT;
   			pDCxt->conn[devId].wpaGroupCrypto = WEP_CRYPT;
   			/* allow either form of auth for WEP */
   			pDCxt->conn[devId].dot11AuthMode = (OPEN_AUTH | SHARED_AUTH);
   			pDCxt->conn[devId].connectCtrlFlags &= ~CONNECT_DO_WPA_OFFLOAD;
   			break;
   		case ENET_MEDIACTL_SECURITY_TYPE_WPA:
   			/* FIXME: need t allow WPA with AES */
   			pDCxt->conn[devId].wpaAuthMode = WPA_PSK_AUTH;
   			pDCxt->conn[devId].dot11AuthMode = OPEN_AUTH;
   			/* by ignoring the group cipher the wifi can connect to mixed mode AP's */
   			pDCxt->conn[devId].connectCtrlFlags |= CONNECT_DO_WPA_OFFLOAD | CONNECT_IGNORE_WPAx_GROUP_CIPHER;
   			break;
   		case ENET_MEDIACTL_SECURITY_TYPE_WPA2:
   			/* FIXME: need to allow WPA2 with TKIP */
   			pDCxt->conn[devId].wpaAuthMode = WPA2_PSK_AUTH;
   			pDCxt->conn[devId].dot11AuthMode = OPEN_AUTH;
   			/* by ignoring the group cipher the wifi can connect to mixed mode AP's */
   			pDCxt->conn[devId].connectCtrlFlags |= CONNECT_DO_WPA_OFFLOAD | CONNECT_IGNORE_WPAx_GROUP_CIPHER;
   			break;
   		default:
   			error = ENETERR_INVALID_INIT_PARAM;
   			break;
   		}
   	  	break;
	case ENET_SET_MEDIACTL_FREQ:
		/* Normally only used for starting a new Ad-hoc network */
		/* In Infrastructure mode the wifi device will scan for the
		 * SSID on allowed channels. */
		/* arChannelHint is used in connect command to wifi device */
		if( ATH_IOCTL_FREQ_1 <= *PTR_FREQ &&
			ATH_IOCTL_FREQ_14 >= *PTR_FREQ){
			pDCxt->conn[devId].channelHint = (A_UINT16)(*PTR_FREQ);
		}else{
			error = ENETERR_INVALID_INIT_PARAM;
		}
		break;
	case ENET_SET_MEDIACTL_RTS:
		local.rtsCmd.threshold = A_CPU2LE16((A_UINT16)(*PTR_RTS));
		if(A_OK != wmi_cmd_start(pWmi, &local.rtsCmd, WMI_SET_RTS_CMDID, sizeof(WMI_SET_RTS_CMD))){
			error = ENET_ERROR;
		}
   	  	break;
	case ENET_SET_MEDIACTL_PASSPHRASE:
		/* must have already set SSID prior to this call */
		if(PTR_PASSPHRASE->length > WMI_PASSPHRASE_LEN){
			error = ENETERR_INVALID_INIT_PARAM;
		}else if (pDCxt->conn[devId].ssidLen == 0) {
  	  		error = ENETERR_INIT_FAILED;
		}else{
			local.passCmd.ssid_len = (A_UINT8)pDCxt->conn[devId].ssidLen;
			A_MEMCPY(local.passCmd.ssid, pDCxt->conn[devId].ssid, (A_UINT32)pDCxt->conn[devId].ssidLen);
			local.passCmd.passphrase_len = (A_UINT8)PTR_PASSPHRASE->length;
			A_MEMCPY(local.passCmd.passphrase,
				PTR_PASSPHRASE->data,
				local.passCmd.passphrase_len);

			if(A_OK != wmi_cmd_start(pWmi, &local.passCmd, WMI_SET_PASSPHRASE_CMDID, sizeof(WMI_SET_PASSPHRASE_CMD))){
				error = ENET_ERROR;
			}
		}
   	  	break;
	case ENET_SET_MEDIACTL_SCAN:
		GET_DRIVER_CXT(pCxt)->extended_scan = 0;
		error = scan_setup(pCxt, pWmi);
		break;
   	case ENET_SET_MEDIACTL_ENCODE:
		/* copy wep keys to driver context for use later during connect */
		if (((PTR_WEP_IN->defKeyIndex - 1) < WMI_MIN_KEY_INDEX) ||
	    	((PTR_WEP_IN->defKeyIndex - 1) > WMI_MAX_KEY_INDEX) ||
	    	(PTR_WEP_IN->numKeys > WMI_MAX_KEY_INDEX+1) ||
	    	/* user passes in num digits as keyLength */
	    	(PTR_WEP_IN->keyLength != WEP_SHORT_KEY*2 &&
	    	 PTR_WEP_IN->keyLength != WEP_LONG_KEY*2) &&
	    	 /* user passes in num digits as keyLength */
	    	 (PTR_WEP_IN->keyLength != WEP_SHORT_KEY &&
	    	 PTR_WEP_IN->keyLength != WEP_LONG_KEY))
	    {
	        error = ENETERR_INVALID_INIT_PARAM;
			goto MEDIACTL_DONE;
	    }

		pDCxt->conn[devId].wepDefTxKeyIndex = (A_UINT8)(PTR_WEP_IN->defKeyIndex-1);

		for(i=0 ; i<PTR_WEP_IN->numKeys ; i++){

		    if((PTR_WEP_IN->keyLength == WEP_SHORT_KEY) || (PTR_WEP_IN->keyLength == WEP_LONG_KEY))
		    {
				A_MEMCPY(pDCxt->conn[devId].wepKeyList[i].key, PTR_WEP_IN->keys[i], PTR_WEP_IN->keyLength);
		    	pDCxt->conn[devId].wepKeyList[i].keyLen = (A_UINT8)PTR_WEP_IN->keyLength;

		    }
			else
			{
				pDCxt->conn[devId].wepKeyList[i].keyLen = (A_UINT8)(PTR_WEP_IN->keyLength>>1);
				A_MEMZERO(pDCxt->conn[devId].wepKeyList[i].key, MAX_WEP_KEY_SZ);
				/* convert key data from string to bytes */
				for(ii=0 ; ii<PTR_WEP_IN->keyLength ; ii++){
					if((val = ascii_to_hex(PTR_WEP_IN->keys[i][ii]))==0xff){
						error = ENETERR_INVALID_INIT_PARAM;
						goto MEDIACTL_DONE;
					}
					if((ii&1) == 0){
						val <<= 4;
					}
					pDCxt->conn[devId].wepKeyList[i].key[ii>>1] |= val;
				}

			}
		}
   	  	break;
   	case ENET_SET_MEDIACTL_POWER:
   		/* atheros wifi allows for 2 power modes;
   		 * 1) MAX_PERF - the radio is always on either transmitting, receiving or listening
   		 * 2) REC_MODE - the wifi device will according to its built in rules enter/exit sleep
   		 *		mode with the Access point.
   		 */
		if(PTR_POWER_IN->value){
			/* a non-zero value implies REC_MODE */
			pDCxt->userPwrMode = REC_POWER;
		}else{
			pDCxt->userPwrMode = MAX_PERF_POWER;
		}

		wmi_cmd_start(pWmi, &pDCxt->userPwrMode, WMI_SET_POWER_MODE_CMDID, sizeof(WMI_POWER_MODE_CMD));
		break;
	case ENET_GET_MEDIACTL_POWER:
		if(pDCxt->userPwrMode == REC_POWER){
			PTR_POWER_OUT->value = 1;
		}else{
			PTR_POWER_OUT->value = 0;
		}

		break;
   	case ENET_GET_MEDIACTL_SCAN:
   		wait_scan_done(pCxt, pWmi);
   		PTR_SCAN_OUT->scan_info_list = (ENET_SCAN_INFO*)GET_DRIVER_CXT(pCxt)->pScanOut;
   		PTR_SCAN_OUT->num_scan_entries = GET_DRIVER_CXT(pCxt)->scanOutCount;
   		break;
	case ENET_GET_MEDIACTL_ESSID:
  	  ((ENET_MEDIACTL_PARAM*)inout_param)->length = (A_UINT32)pDCxt->conn[devId].ssidLen;
  	  A_MEMCPY(((ENET_MEDIACTL_PARAM*)inout_param)->data, pDCxt->conn[devId].ssid,(A_UINT32)pDCxt->conn[devId].ssidLen + 1);
   	  break;
  	case ENET_GET_MEDIACTL_SEC_TYPE:
  	   *((uint_32*)inout_param) = GET_DRIVER_CXT(pCxt)->securityType;
   	  break;
    case ENET_GET_MEDIACTL_MODE:
  	  *((uint_32*)inout_param) = (A_UINT32)pDCxt->conn[devId].networkType;
      switch(pDCxt->conn[devId].networkType)
      {
          case INFRA_NETWORK:
          *((uint_32*)inout_param) = ENET_MEDIACTL_MODE_INFRA;
          break;
          case ADHOC_NETWORK:
          *((uint_32*)inout_param) = ENET_MEDIACTL_MODE_ADHOC;
          break;
#if ENABLE_AP_MODE
          case AP_NETWORK:
          *((uint_32*)inout_param) = ENET_MEDIACTL_MODE_MASTER;
          break;
#endif
          default:
          error = ENETERR_INVALID_INIT_PARAM;
          break;
      }
   	  break;
#if 0   /* currently unsupported MEDIACTLS */
  	case ENET_GET_MEDIACTL_PASSPHRASE:
  	  error =
   	  break;
	case ENET_GET_MEDIACTL_NAME:
  	  error =
   	  break;
  	case ENET_GET_MEDIACTL_FREQ:
  	  error =
   	  break;
  	case ENET_GET_MEDIACTL_RANGE:
   	  break;
  	case ENET_GET_MEDIACTL_WAP:
   	  break;
  	case ENET_GET_MEDIACTL_RATE:
  	  error =
   	  break;
  	case ENET_GET_MEDIACTL_RTS:
  	  error =
   	  break;
  	case ENET_GET_MEDIACTL_RETRY:
  	  error =
   	  break;
#endif
   	default :
   		A_PRINTF("Atheros ERROR: mediactl command not valid. command_id=0x%x\n",command_id);
   		error = ENET_ERROR;
   		break;
   		//while(1){};
   } /* Swtich*/

MEDIACTL_DONE:

   return error;
}

/* EOF */