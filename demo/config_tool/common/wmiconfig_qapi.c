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
/*
* $FileName: wmiconfig.c$
* $Version : $
* $Date    : May-20-2011$
*
* Comments: Handles all Atheros defined wmiconfig command implementations.
*
*END************************************************************************/


#include <main.h>
#include <throughput.h>
#if ENABLE_P2P_MODE
#include "p2p.h"
char p2p_wps_pin_val[P2P_WPS_PIN_LEN], set_channel_p2p[P2P_CHANNEL_LEN];
A_INT32 p2p_callback(A_UINT8 evt_flag);
WMI_NOA_INFO_STRUCT   p2p_noa;
#endif

#include "pmu.h"
#include "dset_api.h"
#include "qcom_api.h"

A_UINT16 ap_channel_hint;
A_UINT8 ssid_str_concurrent[MAX_SSID_LENGTH] = {0};
#define AUTOIP_BASE_ADDR 0xA9FE0100
#define AUTOIP_MAX_ADDR 0xA9FEFEFF

/* Set defaults for Base & range on AUtoIP address pool */
unsigned long   dBASE_AUTO_IP_ADDRESS = AUTOIP_BASE_ADDR;    /* 169.254.1.0 */
unsigned long   dMAX_AUTO_IP_ADDRESS  = AUTOIP_MAX_ADDR;     /* 169.254.254.255 */

/**************************Globals ************************************************/
A_UINT8 pmk_flag = 0, hidden_flag = 0, wps_flag = 0;
AP_CFG_CMD ap_config_param;
mode_t mode_flag = MODE_STATION;
WMI_POWER_MODE power_mode = REC_POWER;
char ssid[MAX_SSID_LENGTH];
A_UINT8 original_ssid[MAX_SSID_LENGTH];               //Used to store user specified SSID
wps_context_t wps_context;
A_UINT32 wifi_connected_flag = 0, concurrent_connect_flag = 0;
A_BOOL p2pMode = FALSE;
#if ENABLE_P2P_MODE
A_INT32 task2_msec = 100;
A_UINT8 p2p_connection_default = 0, p2p_persistent_done = 0, p2p_cancel_enable = 0, p2p_session_in_progress = 0; /* 
                                       0 - Allow other P2P Device to connect(Accept connection)
                                       1 - Allow IOE Device to connect(Issue connect)
                                    */
A_CHAR p2p_wps_pin[WPS_PIN_LEN+1];
A_UINT8 p2p_intent = 0;
A_BOOL autogo_newpp = A_FALSE;
WMI_PERSISTENT_MAC_LIST         p2p_pers_data[MAX_LIST_COUNT];
A_UINT8 invitation_index=0, p2p_join_mac_addr[ATH_MAC_LEN] = {0}, p2p_join_session_active = 0;
WMI_P2P_FW_CONNECT_CMD_STRUCT p2p_join_profile;
A_UINT8 p2p_persistent_go = 0;  /* This variable is used to remember persistent 
                                  information while waiting for GO_NEG complete event */
#endif
A_UINT16 channel_array[] =
{
	0,2412,2417,2422,2427,2432,2437,2442,2447,2452,2457,2462,2467,2472,2482
};
static int security_mode = 0;
A_UINT32 cipher;
A_UINT32 wpa_ver;
key_t key[MAX_NUM_WEP_KEYS];
A_UINT32 wep_keyindex;
char wpsPin[MAX_WPS_PIN_SIZE];
char wpa_passphrase[MAX_PASSPHRASE_SIZE+1]; //for avoiding overflow
volatile A_UINT8 wifi_state = 0;
A_INT32 block_on_dhcp();
int active_device = 0;
/************ Function Prototypes *********************************************/
A_UINT32 set_scan(A_UINT32 dev_num, char* ssid, A_UINT32 *pNum_ap /* [OUT] */);
void resetTarget();
void init_wep_keys();
A_INT32 clear_wep_keys();
static A_INT32 connect_handler( A_INT32 index, A_INT32 argc, char* argv[]);
static A_INT32 set_pmparams(A_INT32 index, A_INT32 argc, char* argv[]);
static A_INT32 get_rate();
static A_INT32 set_rate(A_INT32 index, A_INT32 argc, char* argv[]);
static A_INT32 set_tx_power_scale(A_INT32 index, A_INT32 argc,char* argv[]);
static A_INT32 set_gtx(char* gtx);
static A_INT32 set_lpl(char* lpl);
static A_INT32 deviceid_handler( A_INT32 index, A_INT32 argc, char* argv[]);
static A_INT32 gpio_handle(A_INT32 argc,char *argv[]);
static A_INT32 set_active_deviceid( A_UINT16 device_index);
A_INT32 get_active_device();
static A_INT32 set_wep_key(char* key_index, char* key_val);
void wifiCallback(int val, A_UINT8 devId,A_UINT8 *mac,A_BOOL bssConn);
static A_INT32 set_tcp_connection_timeout(A_INT32 argc,char* argv[]);
#if ENABLE_P2P_MODE
static int_32 p2p_handler( int_32 index, int_32 argc, char_ptr argv[]);
void P2PCallBackfn(A_UINT8 *ptr);
#endif

#if ENABLE_AP_MODE
static A_INT32 ap_handler( int_32 index, int_32 argc, char_ptr argv[]);
static A_INT32 set_ap_params(A_UINT16 param_cmd, A_UINT8 *data_val);
#endif

A_INT32 get_phy_mode(A_UINT32 dev_num, char* phy_mode);
static A_INT32 set_passphrase(char* passphrase);
A_INT32 dev_susp_start(char* susp_time);
A_INT32 get_version();
static A_INT32 ad_hoc_connect_handler( A_INT32 index, A_INT32 argc, char* argv[]);
static A_INT32 wps_handler( A_INT32 index, A_INT32 argc, char* argv[]);
#if ENABLE_P2P_MODE
static A_STATUS wmic_ether_aton(const char *orig, A_UINT8 *eth);
#endif
static A_INT32 scan_control( A_INT32 argc, char* argv[]);
static A_INT32 set_scan_para( A_INT32 argc, char* argv[]);
static A_INT32 ath_assert_dump( A_INT32 argc, char* argv[]);
unsigned char ascii_to_hex(char val);
static A_INT32 mac_store(char *argv[]);
A_INT32 wps_query(A_UINT8 block);
static A_INT32 test_promiscuous(A_INT32 argc, char* argv[]);
static A_INT32 test_raw(A_INT32 argc, char* argv[]);
static A_INT32 allow_aggr(A_INT32 argc, char* argv[]);
static A_INT32 set_wpa( A_INT32 index, A_INT32 argc, char* argv[]);
static A_INT32 set_wep( A_INT32 index, A_INT32 argc, char* argv[]);
A_INT32 get_last_error();
A_INT32 parse_ipv4_ad(unsigned long* ip_addr, unsigned* sbits, char* ip_addr_str);
int user_defined_power_mode = MAX_PERF_POWER;
static A_INT32 get_reg_domain( );

void SystemReset();
int ishexdigit(char digit);
extern void ath_tcp_rx_multi_TCP (int port);

#if ENABLE_STACK_OFFLOAD
A_INT32 ipconfig_auto_ipv4(A_INT32 argc, char* argv[],int mode);
A_INT32 custom_ipdns(void *handle,A_INT32 command, char *domain_name, IP46ADDR *dnsaddr);
A_INT32 ipconfig_dhcp_pool(A_INT32 argc, char* argv[]);
A_INT32 ip_set_bridgemode(A_INT32 argc, char* argv[]);
A_INT32 ip6_set_router_prefix(A_INT32 argc,char *argv[]);
static A_INT32 ip_set_tcp_exponential_backoff_retry(A_INT32 argc,char *argv[]);
static A_INT32 mac_store(char *argv[]);
static A_INT32 ip6_set_status(A_INT32 argc,char *argv[]);
static A_INT32 ipconfig_dhcp_release(A_INT32 argc,char *argv[]);
static A_INT32 ipconfig_tcp_rxbuf(A_INT32 argc,char *argv[]);
#if ENABLE_HTTP_SERVER
static A_INT32 ip_http_server(A_INT32 argc,char *argv[]);
static A_INT32 ip_http_server_post(A_INT32 argc,char *argv[]);
static A_INT32 ip_http_server_get(A_INT32 argc,char *argv[]);
#endif
#if ENABLE_DNS_CLIENT
static A_INT32 ip_resolve_hostname(A_INT32 argc,char *argv[]);
A_INT32 ip_get_addr_for_host_name(char *hostname,A_UINT32 *v4addr,IP6_ADDR_T *ip6addr,A_UINT32 domain);
static A_INT32 ip_gethostbyname(A_INT32 argc,char *argv[]);
static A_INT32 ip_gethostbyname2(A_INT32 argc,char *argv[]);
static A_INT32 ip_dns_client(A_INT32 argc,char *argv[]);
static A_INT32 ip_dns_server_addr(A_INT32 argc,char *argv[]);
static A_INT32 ip_dns_delete_server_addr(A_INT32 argc,char *argv[]);
#endif
#if ENABLE_SSL
static A_INT32 ssl_start(A_INT32 argc,char *argv[]);
static A_INT32 ssl_stop(A_INT32 argc,char *argv[]);
static A_INT32 ssl_config(A_INT32 argc,char *argv[]);
static A_INT32 ssl_add_cert(A_INT32 argc,char *argv[]);
static A_INT32 ssl_store_cert(A_INT32 argc,char *argv[]);
static A_INT32 ssl_delete_cert(A_INT32 argc,char *argv[]);
static A_INT32 ssl_list_cert(A_INT32 argc,char *argv[]);
#endif
static A_INT32 gpio_handle(A_INT32 argc,char *argv[]);
void configure_auto_ipv4_addr();
static A_INT32 iphostname(A_INT32 argc,char *argv[]);
#if ENABLE_DNS_SERVER
static A_INT32 ip_dns_local_domain(A_INT32 argc,char *argv[]);
static A_INT32 ipdns(A_INT32 argc,char *argv[]);
static A_INT32 ip_dns_server_enable(A_INT32 argc, char *argv[]) ;
#endif
#if ENABLE_SNTP_CLIENT
static A_INT32 ip_sntp_srvr(A_INT32 argc,char *argv[]);
static A_INT32 ip_sntp_get_time(A_INT32 argc,char *argv[]);
static A_INT32 ip_sntp_get_time_of_day(A_INT32 argc,char *argv[]);
static A_INT32 ip_sntp_modify_timezone_dse(A_INT32 argc, char *argv[]);
static A_INT32 ip_query_sntp_server_address(A_UINT32 argc,char *argv[]);
static A_INT32 ip_sntp_client_start_stop(A_UINT32 argc,char *argv[]);
#endif
#if ENABLE_HTTP_CLIENT
static A_INT32 httpc_command_parser(A_INT32 argc, char_ptr argv[]);
static void httpc_connect(HTTPC_PARAMS *p_httpc);
static void httpc_method(HTTPC_PARAMS *p_httpc);
#endif

A_INT32 ipv4_route(A_INT32 argc, char_ptr argv[]);
A_INT32 ipv6_route(A_INT32 argc, char_ptr argv[]);
#endif /* ENABLE_STACK_OFFLOAD */

#if ENABLE_KF_PERFORMANCE
A_STATUS ath_setpmu(int argc, char* argv[]);
static A_INT32 pfm_handler(A_INT32 argc,char *argv[]);
static A_INT32 diag_handler(A_INT32 argc,char *argv[]);

#endif
static A_INT32 get_temperature(A_INT32 argc,char *argv[]);
static A_INT32 ath_roam_ctrl(A_INT32 argc,char *argv[]);
A_STATUS ath_dset(int argc, char* argv[]);
A_STATUS ath_dset_api(int argc, char* argv[]);

static A_INT32 ota_upgrade(A_INT32 argc,char *argv[]);
static A_INT32 ota_read(A_INT32 argc,char *argv[]);
static A_INT32 ota_done();

A_STATUS set_power_mode(A_BOOL pwr_mode, A_UINT8 pwr_module)
{
    A_UINT32 enet_device = get_active_device();

    if(pwr_mode == REC_POWER){
      /*Allow power save mode setting from a module only if user has configured
       REC_POWER*/
      if(user_defined_power_mode != REC_POWER)
        return A_ERROR;
    }
    return qcom_power_set_mode(enet_device, pwr_mode, pwr_module); 
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : mystrtomac
* Returned Value : NA
* Comments       : converts a string to a 6 byte MAC address.
*
*END------------------------------------------------------------------*/
static int mystrtomac(const char* arg, unsigned char* dst)
{
	int i=0;
	int j=0;
	int left_nibble = 1;
	char base_char;
	unsigned char base_val;

	if(dst == NULL || arg == NULL){
		return -1;
	}

	A_MEMZERO(dst, 6);

	do{
		if(arg[i] >= '0' && arg[i] <= '9'){
			base_char = '0';
			base_val = 0x00;
		}else if(arg[i] == ':'){
			base_char = ':';
		}else if(arg[i] >= 'a' && arg[i] <= 'f'){
			base_char = 'a';
			base_val = 0x0a;
		}else if(arg[i] >= 'A' && arg[i] <= 'F'){
			base_char = 'A';
			base_val = 0x0a;
		}else{
			return -1;//error
		}

		if(base_char != ':'){
			dst[j] |= (arg[i] - base_char + base_val)<<(4*left_nibble);
			left_nibble = (left_nibble)? 0:1;

			if(left_nibble){
				j++;

				if(j>5){
					break;
				}
			}
		}

		i++;
	}while(1);

	return 0;//success
}



/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : wmiconfig_handler
* Returned Value : ERROR code on error else success
* Comments       : 1st stop for all wmiconfig commands
*
*END------------------------------------------------------------------*/

A_INT32 wmiconfig_handler(A_INT32 argc, char* argv[] )
{ /* Body */
    A_BOOL      shorthelp = FALSE;
    A_UINT8 	printhelp = 0;
    A_INT32     return_code = A_OK;
    A_UINT32    enet_device, wifimode, index = 1;
#if ENABLE_P2P_MODE
    if(p2p_session_in_progress)
    {
      printf("p2p event in progress \n");
      return A_ERROR;
    }
#endif    
    enet_device = get_active_device();
    
    if (argc > index)
    {
        if (ATH_STRCMP(argv[index], "--connect") == 0)
        {
            if(argc == index+1)
            {
                printf("Incomplete parameters for connect command\n");
                return A_ERROR;
            }
            return_code = connect_handler(index+1,argc,argv);
        }
        else if (ATH_STRCMP(argv[index], "--adhoc") == 0)
        {
            if(argc == index+1)
            {
                printf("Incomplete parameters for ad-hoc command\n");
                return A_ERROR;
            }
            qcom_op_set_mode (enet_device,QCOM_WLAN_DEV_MODE_ADHOC);
            return_code = ad_hoc_connect_handler(index+1,argc,argv);
        }
        else if (ATH_STRCMP(argv[index], "--device") == 0)
        {
            enet_device = atoi(argv[index + 1]);
            if((enet_device != 0 && enet_device != 1) ||
               (argc == index+1))
            {
                printf("usage : wmiconfig --device <id> , id is 0/1 \n");
                return A_ERROR;
            }
            return_code = deviceid_handler(index+1,argc,argv);
            if(return_code==A_OK)
            {
                active_device = enet_device;
            }
            else
            {
                printf("set device got error:\n");
                return A_ERROR;
            }
        }
#if ENABLE_AP_MODE
        else if (ATH_STRCMP(argv[index], "--mode") == 0)
        {
            if(argc == index+1)
            {
                printf("Usage:: --mode <ap[<hidden>][<wps>]|station>  \n");
                return A_ERROR;
            }
            
            qcom_set_ssid(enet_device,"");
            
            if(ATH_STRCMP(argv[index+1], "ap") == 0)
            {
                if (ATH_STRCMP(argv[index+2], "hidden") == 0)
                {
                    hidden_flag = 1;
                }
                if (ATH_STRCMP(argv[index+2], "wps") == 0)
                {
                    wps_flag = 1;
                }
                
                qcom_op_set_mode (enet_device,QCOM_WLAN_DEV_MODE_AP);
                /* Set to MAX PERF Mode */
                set_power_mode( MAX_PERF_POWER,SOFTAP);

                if (hidden_flag == 1)
                {
                    hidden_flag = 1;
                    A_UINT8 hidden_enable;
                    hidden_enable = 1;
                    if( qcom_ap_hidden_mode_enable(enet_device,hidden_enable) != A_OK )
                    {
                        printf("not able to set hidden mode for AP \n");
                        return A_ERROR;
                    }
                }
                if (wps_flag == 1)
                {
                    if (set_ap_params (AP_SUB_CMD_WPS_FLAG, (A_UINT8 *)&wps_flag) != SHELL_EXIT_SUCCESS)
                    {
                        printf("not able to set wps mode for AP \n");
                        return SHELL_EXIT_ERROR;
                    }
                }
            }
            else if(ATH_STRCMP(argv[index+1], "station") == 0)
            {
                qcom_op_get_mode(enet_device, &wifimode);
                if (wifimode == QCOM_WLAN_DEV_MODE_AP) /*check previous mode*/
                {
                    qcom_disconnect(enet_device);
                    /* AP Mode is always set to maxper; if we are switching mode and prev mode is REC_POWER then
                       retain the power mode for STA */
                    
                   set_power_mode( REC_POWER,SOFTAP);
                }
                qcom_op_set_mode (enet_device,QCOM_WLAN_DEV_MODE_STATION);
                
                
                if (ATH_STRCMP(argv[index+2], "hidden") == 0)
                {
                    printf("Usage:: --mode <ap[<hidden>]|station>  \n");
                    return A_ERROR;
                }
                if (ATH_STRCMP(argv[index+2], "wps") == 0)
                {
                    printf("Usage:: --mode <ap[<wps>]|station>  \n");
                    return A_ERROR;
                }
            }
            else
            {
                printf("Usage:: --mode <ap[<hidden>][<wps>]|station>  \n");
                return A_ERROR;
            }
        }
        else if (ATH_STRCMP(argv[index], "--ap") == 0)
        {
            if(argc == index+1)
            {
                printf ("usage:--ap [country <country code>]\n"
                         "      --ap [bconint<intvl>]\n"
                         "      --ap [dtim<val>]\n"
                         "      --ap [inact <minutes>]> \n"
                         "      --ap [psbuf <1/0-enable/disable> <buf_count for enable>]>\n");
                return A_ERROR;
            }
            return_code = ap_handler(index+1,argc,argv);
        }
#endif
        else if (ATH_STRCMP(argv[index], "--wps") == 0)
        {
            return_code = wps_handler(index+1,argc,argv);
        }
#if ENABLE_P2P_MODE
        else if (ATH_STRCMP(argv[index], "--p2p") == 0)
        {
            return_code = p2p_handler(index+1,argc,argv);
        }
#endif
        else if (ATH_STRCMP(argv[index], "--disc") == 0)
        {
            return_code = qcom_disconnect(enet_device);
        }
        else if (ATH_STRCMP(argv[index], "--settxpower") == 0)
        {
            return_code = qcom_set_tx_power(enet_device,atoi(argv[index+1]));
        }
        else if (ATH_STRCMP(argv[index], "--pwrmode") == 0)
        {
            if(ATH_STRCMP(argv[index+1], "0") == 0){
                user_defined_power_mode = MAX_PERF_POWER;
                set_power_mode( MAX_PERF_POWER,USER);             
            }else{
                qcom_op_get_mode(enet_device, &wifimode);
		if(wifimode == QCOM_WLAN_DEV_MODE_AP && !(p2pMode)) {
                    printf("Setting REC Power is not allowed MODE_AP \n");
                    return A_ERROR;
                }
                user_defined_power_mode = REC_POWER;
                set_power_mode( REC_POWER,USER);             
            }
        }
        else if (ATH_STRCMP(argv[index], "--pmparams") == 0)
        {
            return_code = set_pmparams(index+1,argc,argv);
        }
        else if (ATH_STRCMP(argv[index], "--channel") == 0)
        {
            return_code = qcom_set_channel(enet_device,atoi(argv[index+1]));
        }
        else if (ATH_STRCMP(argv[index], "--listen") == 0)
        {
            A_UINT32 listen_time;
            listen_time = atoi(argv[index+1]);
            return_code =  qcom_sta_set_listen_time(enet_device,listen_time);
        }
        else if (ATH_STRCMP(argv[index], "--wmode") == 0)
        {
            if (0 == ATH_STRCMP(argv[index+1], "a")) {
              	return_code = qcom_set_phy_mode(enet_device, (A_UINT8)QCOM_11A_MODE);
            } else if (0 == ATH_STRCMP(argv[index+1], "b")) {
                return_code = qcom_set_phy_mode(enet_device, (A_UINT8)QCOM_11B_MODE);
            } else if (0 == ATH_STRCMP(argv[index+1], "g")) {
                return_code = qcom_set_phy_mode(enet_device, (A_UINT8)QCOM_11G_MODE);
            } else if (0 == ATH_STRCMP(argv[index+1], "n")) {
                return_code = qcom_set_phy_mode(enet_device, (A_UINT8)QCOM_11N_MODE);
            } else if (0 == ATH_STRCMP(argv[index+1], "ht40")) {
                return_code = qcom_set_phy_mode(enet_device, (A_UINT8)QCOM_HT40_MODE);
            } else {
                printf("Unknown wmode, only support b/g/n/ht40\n");
                return A_ERROR;
            }
        }
        else if (ATH_STRCMP(argv[index], "--rssi") == 0)
        {
             A_UINT8 rssi;
             return_code = qcom_sta_get_rssi(enet_device,&rssi);
             printf("\nindicator = %d dB\n",rssi);
        }
        else if (ATH_STRCMP(argv[index], "--gettxstat") == 0)
        {
            return_code = get_tx_status();
        }
        else if (ATH_STRCMP(argv[index], "--getrate") == 0)
        {
            return_code = get_rate();
        }
        else if (ATH_STRCMP(argv[index], "--setrate") == 0)
        {
            return_code = set_rate(index+1, argc, argv);
        }        
        else if (ATH_STRCMP(argv[index], "--txpwrscale") == 0)
        {
            return_code = set_tx_power_scale(index+1, argc, argv);
        }       
        else if (ATH_STRCMP(argv[index], "--wepkey") == 0)
        {
            if((argc - index) > 2)
                return_code = set_wep_key(argv[index+1],argv[index+2]);
            else
            {
                printf("Missing parameters\n\n");
                return A_ERROR;
            }
        }
        else if (ATH_STRCMP(argv[index], "--p") == 0)
        {
            /*We expect one parameter with this command*/
            if((argc - index) > 1)
                return_code = set_passphrase(argv[index+1]);
            else
            {
                printf("Missing parameters\n\n");
                return A_ERROR;
            }
        }
        else if (ATH_STRCMP(argv[index], "--wpa") == 0)
        {
            return_code = set_wpa(index+1,argc,argv);
        }
        else if (ATH_STRCMP(argv[index], "--wep") == 0)
        {
            return_code = set_wep(index+1,argc,argv);
        }
        else if (ATH_STRCMP(argv[index], "--suspend") == 0)
        {
            return_code = qcom_suspend_enable(A_TRUE);
        }
        else if (ATH_STRCMP(argv[index], "--suspstart") == 0)
        {
            return_code = dev_susp_start(argv[index+1]);
        }
        else if (ATH_STRCMP(argv[index], "--version") == 0)
        {
            return_code = get_version();
        }
        else if (ATH_STRCMP(argv[index], "--help") == 0)
        {
            //   return_code = A_ERROR;
            printhelp = 1;
        }
        
        else if (ATH_STRCMP(argv[index], "--testflash") == 0)
        {
            return_code = TEST_FLASH;
        }
        else if (ATH_STRCMP(argv[index], "--scanctrl") == 0)
        {
            return_code = scan_control(argc-(index+1), &argv[index+1]);
        }
        else if (ATH_STRCMP(argv[index], "--setscanpara") == 0)
        {
            return_code = set_scan_para(argc-(index+1), &argv[index+1]);
        }
        else if (ATH_STRCMP(argv[index], "--reg") == 0)
        {
            return_code = ATH_REG_QUERY(argc-(index+1), &argv[index+1]);
        }
        else if(ATH_STRCMP(argv[index], "--fwmem") == 0)
        {
            return_code = ATH_MEM_QUERY(argc-(index+1), &argv[index+1]);
        }
        else if (ATH_STRCMP(argv[index], "--assdump") == 0)
        {
            return_code = ath_assert_dump(argc-(index+1), &argv[index+1]);
        }
        else if (ATH_STRCMP(argv[index], "--driver") == 0)
        {
            return_code = ath_driver_state(argc-(index+1), &argv[index+1]);
        }
        else if (ATH_STRCMP(argv[index], "--raw") == 0)
        {
            return_code = test_raw(argc-(index+1), &argv[index+1]);
        }
        else if (ATH_STRCMP(argv[index], "--promisc") == 0)
        {
            return_code = test_promiscuous(argc-(index+1), &argv[index+1]);
        }
        else if (ATH_STRCMP(argv[index], "--allow_aggr") == 0)
        {
            return_code = allow_aggr(argc-(index+1), &argv[index+1]);
        }
        else if (ATH_STRCMP(argv[index], "--reset") == 0)
        {
            SystemReset();
        }
        else if (ATH_STRCMP(argv[index], "--regdomain") == 0)
        {
            get_reg_domain();
        }
        else if (ATH_STRCMP(argv[index], "--getlasterr") == 0)
        {
            get_last_error();
        }
        else if (ATH_STRCMP(argv[index], "--gtx") == 0)
        {
            return_code = set_gtx (argv[index+1]);
        }
        else if (ATH_STRCMP(argv[index], "--lpl") == 0)
        {
            return_code = set_lpl (argv[index+1]);
        }
        else if (ATH_STRCMP(argv[index], "--macstore") == 0)
        {
            return_code = mac_store(&argv[index+1]);
        }
        else if (ATH_STRCMP(argv[index], "--gpio") == 0)
        {
            return_code = gpio_handle(argc,argv);
        }
        
#if ENABLE_STACK_OFFLOAD
        else if (ATH_STRCMP(argv[index], "--ipconfig") == 0)
        {
            return_code = ipconfig_query(argc,argv);
        }
        else if (ATH_STRCMP(argv[index], "--ipstatic") == 0)
        {
            return_code = ipconfig_static(argc,argv);
        }
        else if (ATH_STRCMP(argv[index], "--ipdhcp") == 0)
        {
            return_code = ipconfig_dhcp(argc,argv);
        }
        else if (ATH_STRCMP(argv[index], "--ipbridgemode") == 0)
        {
            return_code = ip_set_bridgemode(argc,argv);
        }
        else if(ATH_STRCMP(argv[index], "--ipauto") == 0)
        {
            return_code = ipconfig_auto_ipv4(argc,argv,IPCFG_AUTO);
        }
        else if (ATH_STRCMP(argv[index], "--blockdhcp") == 0)
        {
            return_code = block_on_dhcp(argc,argv);
        }
        else if (ATH_STRCMP(argv[index], "--udpecho") == 0)
        {
            //ath_udp_echo(argc,argv);
        }
        else if (ATH_STRCMP(argv[index], "--multi") == 0)
        {
            ath_tcp_rx_multi_TCP(atoi(argv[2]));
        }
        else if(ATH_STRCMP(argv[index], "--settcptimeout") == 0)
 	{
 	    return_code = set_tcp_connection_timeout( argc, argv);
 	}
        else if(ATH_STRCMP(argv[index],"--ipdhcppool") == 0)
        {
            return_code = ipconfig_dhcp_pool(argc,argv);
        }
		else if(ATH_STRCMP(argv[index],"--ip6rtprfx") == 0)
        {
            return_code = ip6_set_router_prefix(argc,argv);
        }
		else if(ATH_STRCMP(argv[index],"--tcp_backoff_retry") == 0)
		{
            return_code = ip_set_tcp_exponential_backoff_retry(argc,argv);
		}
		else if(ATH_STRCMP(argv[index],"--ipv6") == 0)
		{
            return_code = ip6_set_status(argc,argv); 
		}
        else if(ATH_STRCMP(argv[index],"--ipdhcp_release") == 0)
		{
            return_code = ipconfig_dhcp_release(argc,argv);
		}	
        else if(ATH_STRCMP(argv[index],"--tcp_rxbuf") == 0)
		{
            return_code = ipconfig_tcp_rxbuf(argc,argv);
		}
#if ENABLE_DNS_CLIENT
        else if (ATH_STRCMP(argv[index],"--ip_resolve_hostname") == 0)
        {
            return_code = ip_resolve_hostname(argc, argv);
        }
		else if(ATH_STRCMP(argv[index],"--ip_gethostbyname") == 0)
        {
            return_code = ip_gethostbyname(argc, argv);
		} 	
        else if(ATH_STRCMP(argv[index],"--ip_gethostbyname2") == 0)
		{
            return_code = ip_gethostbyname2(argc, argv);
        }
        else if(ATH_STRCMP(argv[index],"--ip_dns_client") == 0)
        {
            return_code = ip_dns_client(argc, argv);
        }
        else if(ATH_STRCMP(argv[index],"--ip_dns_server_addr") == 0)
        {
            return_code = ip_dns_server_addr(argc, argv);
		}
        else if(ATH_STRCMP(argv[index],"--ip_dns_delete_server_addr") == 0)
        {
            return_code = ip_dns_delete_server_addr(argc, argv);
	}
#endif /* ENABLE_DNS_CLIENT */
#if ENABLE_HTTP_SERVER
        else if (ATH_STRCMP(argv[index],"--ip_http_server") == 0)
        {
            return_code = ip_http_server(argc, argv);
        }
        else if (ATH_STRCMP(argv[index],"--ip_https_server") == 0)
        {
            return_code = ip_http_server(argc, argv);
        }
        else if (ATH_STRCMP(argv[index],"--ip_http_post") == 0)
        {
            return_code = ip_http_server_post(argc, argv);
        }
        else if (ATH_STRCMP(argv[index],"--ip_http_get") == 0)
        {
            return_code = ip_http_server_get(argc, argv);
        }
#endif /* ENABLE_HTTP_SERVER */
        
#if ENABLE_HTTP_CLIENT
        else if(ATH_STRCMP(argv[index], "--ip_http_client") == 0)
        {
             return_code = httpc_command_parser(argc, argv);
        }
#endif /* ENABLE_HTTP_CLIENT */
        else if(ATH_STRCMP(argv[index], "--iphostname") == 0)
        {
             return_code = iphostname(argc, argv);
        }
#if ENABLE_DNS_SERVER
        else if(ATH_STRCMP(argv[index], "--ip_dns_local_domain") == 0)
        {
             return_code = ip_dns_local_domain(argc, argv);
        }
        else if(ATH_STRCMP(argv[index], "--ip_dns") == 0)
        {
             return_code = ipdns(argc, argv);
        }
        else if (ATH_STRCMP(argv[index],"--ip_dns_server_enable") == 0) 
        {
             return_code = ip_dns_server_enable(argc, argv);
	}
#endif /* ENABLE_DNS_SERVER */
#if ENABLE_SNTP_CLIENT
	else if(ATH_STRCMP(argv[index],"--ip_sntp_srvr") == 0) 
	{
	     return_code = ip_sntp_srvr(argc,argv);  
	}
        else if(ATH_STRCMP(argv[index],"--ip_sntp_get_time") == 0)
	{
	     return_code = ip_sntp_get_time(argc,argv);  
	}
	else if(ATH_STRCMP(argv[index],"--ip_sntp_get_time_of_day") == 0)
	{
	     return_code = ip_sntp_get_time_of_day(argc,argv);  
	}
	else if(ATH_STRCMP(argv[index],"--ip_sntp_zone") == 0)
	{
 	     return_code = ip_sntp_modify_timezone_dse(argc,argv);
	}
	else if(ATH_STRCMP(argv[index],"--ip_show_sntpconfig") == 0)
	{
	     return_code = ip_query_sntp_server_address(argc,argv);
	}
	else if(ATH_STRCMP(argv[index],"--ip_sntp_client") == 0)
	{
	     return_code = ip_sntp_client_start_stop(argc,argv);
	}
#endif /* ENABLE_SNTP_CLIENT */
    else if(ATH_STRCMP(argv[index],"--ota_upgrade") == 0)
	{
	     return_code = ota_upgrade(argc,argv);
	}
	else if(ATH_STRCMP(argv[index],"--ota_read") == 0)
	{
	     return_code = ota_read(argc,argv);
	}
	else if(ATH_STRCMP(argv[index],"--ota_done") == 0)
	{
	     return_code = ota_done();
	}
	
 #if ENABLE_SSL
        else if(ATH_STRCMP(argv[index], "--ssl_start") == 0)
        {
            return_code = ssl_start(argc, argv);
        }
        else if(ATH_STRCMP(argv[index], "--ssl_stop") == 0)
        {
            return_code = ssl_stop(argc, argv);
        }
        else if(ATH_STRCMP(argv[index], "--ssl_config") == 0)
        {
            return_code = ssl_config(argc, argv);
        }
        else if(ATH_STRCMP(argv[index], "--ssl_add_cert") == 0)
        {
            return_code = ssl_add_cert(argc, argv);
        }
        else if(ATH_STRCMP(argv[index], "--ssl_store_cert") == 0)
        {
            return_code = ssl_store_cert(argc, argv);
        }
        else if(ATH_STRCMP(argv[index], "--ssl_delete_cert") == 0)
        {
            return_code = ssl_delete_cert(argc, argv);
        }
        else if(ATH_STRCMP(argv[index], "--ssl_list_cert") == 0)
        {
            return_code = ssl_list_cert(argc, argv);
        }
#endif
#if ENABLE_ROUTING_CMDS
    else if(ATH_STRCMP(argv[index],"--ipv4_route") == 0)
	{
	     return_code = ipv4_route(argc,argv);
	}
    else if(ATH_STRCMP(argv[index],"--ipv6_route") == 0)
	{
	     return_code = ipv6_route(argc,argv);
	}
#endif /* ENABLE_ROUTING_CMDS */
#endif /* ENABLE_STACK_OFFLOAD */

#if  ENABLE_KF_PERFORMANCE
    else if (ATH_STRCMP(argv[index],"--pfm_get") == 0)
        {
            return_code = pfm_handler(argc, argv);
        }
        else if (ATH_STRCMP(argv[index],"--diag") == 0)
        {
            return_code = diag_handler(argc, argv);
        }
        else if(strcmp (argv[index], "--setpmu") == 0)
        {
             return_code = ath_setpmu(argc,argv);
        }
#endif
        else if (ATH_STRCMP(argv[index],"--dset") == 0)
        {
             return_code = ath_dset(argc,argv);
        }
        else if (ATH_STRCMP(argv[index],"--dset_api") == 0)
        {
             return_code = ath_dset_api(argc,argv);
        }
        else if(ATH_STRCMP(argv[index], "--get_temp") == 0)
        {
             return_code = get_temperature(argc, argv);
        }
        else if (ATH_STRCMP(argv[index], "--roaming") == 0)
        {
             return_code = ath_roam_ctrl(argc, argv);
        }
        else
        {
            printf ("Unknown wmiconfig command!\n");
            return_code = A_ERROR;
        }
    }
    else if (argc == 1)
    {
        char data[32+1];
        A_INT32 error;
        A_UINT16 channel_val = 0;
        A_UINT32 mode;
        error = qcom_get_ssid (enet_device,(char*)data);
        
        if (error != A_OK)
            return_code = A_ERROR;
        
        printf("ssid         =   %s\n",data);
        
        //ATH_GET_SECTYPE (enet_device,(char*)data);
        //printf("security type=   %s\n",data);
        
        qcom_get_phy_mode(enet_device,(A_UINT8 *)&data[0]);
        printf("Phy mode     =   %s\n",data);
        
        qcom_power_get_mode(enet_device, &mode);
        if(mode == REC_POWER)
        {
    	    strcpy(data,"Power Save");
        }
        else
        {
    	    strcpy(data,"Max Perf");
        }
        printf("Power mode   =   %s\n",data);
        
        qcom_get_bssid(enet_device, (A_UINT8 *)&data[0]);
        printf("Mac Addr     =   %02x:%02x:%02x:%02x:%02x:%02x \n", data[0], data[1], data[2], data[3], data[4], data[5]);
        qcom_op_get_mode(enet_device, &mode);
#if ENABLE_AP_MODE
        if(mode == QCOM_WLAN_DEV_MODE_AP)
        {
            printf("mode         =   softap \n");
            if(pmk_flag)
            {
                printf("passphrase   =   %s \n",wpa_passphrase);
            }
        }
        else
#endif
            if(mode == QCOM_WLAN_DEV_MODE_STATION)
            {
                printf("mode         =   station \n");
            }
            else
            {
                printf("mode         =   adhoc \n");
            }
        if(wifi_connected_flag == 1)
        {
            qcom_get_channel(enet_device, &channel_val);
            if(channel_val < 3000)
            {
              channel_val -= ATH_IOCTL_FREQ_1;
              if((channel_val/5) == 14)
              {   
                  channel_val = 14;
              }
              else
              {
                  channel_val = (channel_val/5) + 1;
              }
            }
            else
            {
               channel_val -= ATH_IOCTL_FREQ_36;
               channel_val = 36 + (channel_val/5); // since in 11a channel 36 is the starting number               
            }
            printf("channel      =   %d \n", channel_val);
        }
    }
    
    if ((return_code != A_OK))
    {
    	printf("To see a list of commands- wmiconfig --help\n");
    }
    
    if(printhelp)
    {
    	printhelp = 0;
        if (shorthelp)
        {
            printf ("%s [<device>] [<command>]\n", argv[0]);
        }
        else
        {
#if ENABLE_STACK_OFFLOAD
            printf ("IOT Stack Offload Feature Enabled\n");
#else
            printf ("IOT Stack Offload Feature Disabled\n");
#endif
            printf ("Usage: %s [<command>]\n", argv[0]);
            printf ("  Commands:\n");
            printf ("    --version        = Displays versions\n");
            printf ("    --reset          = reset host processor\n");
            printf ("    --suspend        = Enable device suspend mode(before connect command)\n");
            printf ("    --regdomain      = Get Regulatory Domain\n");
            printf ("    --getlasterr     = Prints last driver error \n");
            printf ("    --connect <ssid>\n");
            printf ("   \n ");
            printf ("   WPA Configuration\n ");
            printf ("      --p <passphrase>\n");
            printf ("      --wpa <ver> <ucipher> <mcipher>\n");
            printf ("      --connect <ssid> \n");
            printf ("   \n ");
            printf ("   WEP Configuration\n");
            printf ("      --wepkey <key_index> <key> \n");
            printf (" 	   --wep <def_keyix> \n");
            printf ("      --connect <ssid>\n");
            printf ("              where  <ssid>   :SSID of network\n");
            printf ("                     <ver>    :1-WPA, 2-RSN\n");
            printf ("                     <ucipher>:TKIP or CCMP\n");
            printf ("                     <mcipher>:TKIP or CCMP\n");
            printf ("                     <p>    :Passphrase for WPA\n");
            //printf ("                     <mode>   :<open> or <shared>\n");
            printf ("                     <key_index>: Entered WEP key index [1-4]\n");
            printf ("                     <def_keyix>: Default WEP key index [1-4]\n");
            printf ("                     <key>    :WEP key\n");
            printf ("                     *        :optional parameters\n");
            printf ("          Note: key can only be composed of Hex characters of length 10 or 26\n");
            printf ("   \n");
            printf ("    --adhoc <ssid> --channel <1-14>  = Connect in ad-hoc mode\n");
            printf ("    --adhoc <ssid> --wep  <def_keyix> --channel <1-14> = Connect in WEP ad-hoc mode\n");
            printf ("                                    where channel is optional and ranges from 1-14\n");
            printf ("   \n");
            printf ("    --wps <connect> <mode> <*pin> <*ssid> <*mac> <*channel>\n");
            printf ("              where  <connect> : 1 - Attempts to connect after wps\n");
            printf ("                               : 0 - No Attempts to connect after wps\n");
            printf ("                     <mode>    : <push> or <pin>\n");
            printf ("                     *         :optional parameters\n");
            printf ("   \n");
            printf ("    --p2p <mode> <*>\n");
            printf ("           where <mode>  :  <on/off> <setconfig> <connect> <find> <prov> <listen> \n");
            printf ("                            <cancel> <join> <auth> <wpspin> <autogo> <invite_auth> <passphrase> \n");
            printf ("                  *      :  optional parameters that need to be entered based on mode\n");
            printf ("   \n");
            printf ("    --disc            = Disconect from current AP\n");
            printf ("    --setrate <rate> , where rate can be 1,2,5,6,9,11,12,18,24,36,48,54 mbps or MCS \n");
            printf ("    --txpwrscale <val> , where val is in dbm \n");
            printf ("      eg:  wmiconfig --setrate 54 will set rate to 54 mbps \n");
            printf ("           wmiconfig --setrate mcs 0 will set rate to mcs_0 \n");            
            printf ("    --settxpower <>   = set transmit power\n");
            printf ("    --pwrmode    <>   = set power mode 1=Power save, 0= Max Perf \n");
            printf ("    --setchannel <1-14>   = set channel hint\n");
            printf ("    --wmode      <>   = set mode <b,g,n>\n");
            printf ("    --listen     <>   = set listen interval\n");
            printf ("    --rssi       <>   = prints Link Quality (SNR) \n");
            printf ("    --suspstart <time>    = Suspend device for specified time in milliseconds\n");
            printf ("    --gettxstat           = get current status of TX from driver\n");
            printf ("    --pmparams        = Set Power management parameters\n");
            printf ("           --idle <time in ms>                        Idle period\n");
            printf ("           --np   < >                                 PS Poll Number\n");
            printf ("           --dp <1=Ignore 2=Normal 3=Stick 4=Auto>    DTIM Policy\n");
            printf ("           --txwp <1=wake 2=do not wake>              Tx Wakeup Policy\n");
            printf ("           --ntxw < >                                 Number of TX to Wakeup\n");
            printf ("           --psfp <1=send fail event 2=Ignore event>  PS Fail Event Policy\n");
            printf ("    --scanctrl [0|1] [0|1]       = Control firmware scan behavior. Specifically, disable/enable foreground OR background scanning.\n");
            printf ("    --setscanpara <max_act_ch_dwell_time_ms> <pas_act_chan_dwell_time_ms> [<fg_start_period(in secs)> <fg_end_period (in secs)> <bg_period (in secs)> <short_scan_ratio> <scan_ctrl_flags>  <min_active_chan_dwell_time_ms> <max_act_scan_per_ssid> <max_dfs_ch_act_time_in_ms>] \n");
        	printf ("    --reg [op] [addr] [value] [mask] = read or write specified AR600X register.\n");
        	printf ("    --assdump = collects assert data from AR600X and prints it to stdout.\n");
        	printf ("    --driver [mode]     \n");
        	printf ("					mode = down - Sets the driver State 0 unloads the driver \n");
        	printf ("					mode = flushdown - Waits for TX clear before unloading the driver \n");
        	printf ("					       up   - Reloads the driver\n");
        	printf ("					       raw  - Reloads the driver in raw mode\n");
        	printf ("					       quad - Reloads the driver in quad spi flash mode\n");
        	printf ("					       rawq - Reloads the driver in raw + quad mode\n");
        	printf ("					       normal - Reloads the driver in normal mode\n");
        	printf ("					       pmu - Reloads the driver in pmu enabled mode (Only MQX)\n");
        	printf ("    --allow_aggr <tx_tid_mask> <rx_tid_mask> Enables aggregation based on the provided bit mask where\n");
        	printf ("		    each bit represents a TID valid TID's are 0-7\n");
        	printf ("    --macstore <addr> = program new mac address into wifi device.\n");
        	printf ("iwconfig scan <ssid*>  =scan channels, ssid is optional, if provided will scan only specified SSID\n");
#if ENABLE_AP_MODE
            printf ("    --mode [ap <hidden><wps>|station]\n");
            printf ("    --ap [country <country code>]\n"
                    "    --ap [bconint<intvl>]\n"
                        "    --ap [dtim<val>]\n"
                            "    --ap [inact <minutes>]> \n"
                                "    --ap [psbuf <1/0-enable/disable> <buf_count for enable>]>\n");
#endif
#if ENABLE_STACK_OFFLOAD
			printf ("    --ipbridgemode                                 = Enable the bridge mode\n");
            printf ("    --ipconfig                                     = show IP parameters\n");
            printf ("    --ipstatic <ip addr> <subnet mask> <Gateway>   = set static IP parameters\n");
            printf ("    --ipdhcp                                       = Run DHCP client\n");
            printf ("    --ipauto  <link local IPv4>(optional)          = Run Auto IPv4 \n");
            printf ("    --udpecho <port>                               = run UDP echo server\n");
            printf ("    --ipdhcppool <start ipaddr> <End ipaddr> <Lease time> = set ip dhcp pool \n");
            printf ("    --ip6rtprfx <prefix> <prefixlen> <prefix_lifetime> <valid_lifetime> = set ipv6 router prefix\n");
            printf ("    --tcp_backoff_retry <no of retries> = set ip tcp max exponetial backoff retries\n");
            printf ("    --ipv6 <status> = set ip6 status as either enable or disable\n");
            printf ("    --ipdhcp_release = Release the DHCP Address\n");
            printf ("    --tcp_rxbuf = Configure the tcp rx buf space\n");
#if ENABLE_DNS_CLIENT
            printf ("    --ip_resolve_hostname [<host name> <domain_type]  = resolve hostname for domain_type (ipv4 =2, ipv6 =3)\n");
            printf ("    --ip_gethostbyname [<host name>]  = resolve hostname\n");
            printf ("    --ip_gethostbyname2 [<host name> <domain_type]  = resolve hostname for domain_type (ipv4 =2 ipv6 =3\n");
            printf ("    --ip_dns_client [<start/stop>]         = Start/Stop the DNS Client\n");
            printf ("    --ip_dns_server_addr <ip addr>         = Address of the DNS Server\n");
            printf ("    --ip_dns_delete_server_addr <ip addr>         = Address of the DNS Server to be deleted \n");
	    printf("     --ip_dns_server_enable <enable/disable>         = Enable/Disable if DNS Server will answer client requests. \n");

#endif /* ENABLE_DNS_CLIENT */
#if ENABLE_HTTP_SERVER
            printf ("    --ip_http_server [<start/stop> <port>]         = Start/Stop the HTTP server\n");
            printf ("    --ip_https_server [<start/stop> <port>]        = Start/Stop the HTTPs server\n");
            printf ("    --ip_http_post <page name>  <obj_name> <obj_type> <obj_len> <obj_value>         = Post/Update the object in HTML page\n");
            printf ("               where <page name>       = page name to update\n");
            printf ("                     <obj_name>        = object name to update\n");
            printf ("                     <obj_type>        = <1-Bool 2-Integer 3-String>\n");
            printf ("                     <obj_len>         = Length of object<1-Bool 4-Integer Length-String>\n");
            printf ("                     <obj_value>       = Object value\n");
            printf ("    --ip_http_get <page name>  <obj_name>          = Get the object in HTML page\n");
            printf ("               where <page name>       = page name to get\n");
            printf ("                     <obj_name>        = object name to get\n");
#endif
#if ENABLE_HTTP_CLIENT
            printf ("    --ip_http_client [<connect/get/post/query/disc> <data1> <data2>]\n");
            printf ("              where <connect> - Used to connect to HTTP server\n");
            printf ("                              <data1> - Hostname or IPaddress of http server\n");
            printf ("                              <data2> - port no of http server(optional)\n");
            printf ("              where <get>     - Used to get a page from HTTP server\n");
            printf ("                              <data1> - Page name to retrieve\n");
            printf ("              where <post>    - Used to post to HTTP server\n");
            printf ("                              <data1> - URL to post\n");
            printf ("              where <query>   - Used to update a variable for get/post\n");
            printf ("                              <data1> - Name of variable\n");
            printf ("                              <data2> - value of variable\n");
            printf ("              where <disc>    - Used to disconnect from HTTP server\n");
#endif
            printf ("    --iphostname [<domain name>]  = configures local host name \n");
#if ENABLE_DNS_SERVER
            printf ("    --ip_dns_local_domain [<domain name>]  = configures local domain \n");
            printf ("    --ip_dns <add/delete> <Host name> <addr>  = configures the domain name and its corresponding address \n");
#endif
#if ENABLE_SNTP_CLIENT
            printf ("    --ip_sntp_srvr <add/delete> <Host name>  = configures the host name or the IP address of the sntp server\n");
            printf ("    --ip_sntp_get_time  = Command to Display the UTC Time \n");
            printf ("    --ip_sntp_get_time_of_day  = Command to Display the seconds \n");
            printf ("    --ip_sntp_zone [<UTC+hour:min>] [DSE] [enable/disable] = \n Command to specify time zone and DSE(day light saving enable/disable)Eg:UTC+05:30 dse enable/disable \n");
            printf ("    --ip_show_sntpconfig = Displays the SNTP server address name \n");
            printf ("    --ip_sntp_client [<start/stop>]         = Start/Stop the SNTP Client\n");
            
#endif
#if ENABLE_SSL
            printf ("    --ssl_start server|client = start SSL as either server or client\n");
            printf ("    --ssl_stop server|client = stop SSL server or client\n");
            printf ("    --ssl_config server|client [protocol <protocol>] [time 0|1] [domain 0|<name>] [cipher <cipher>] = configure SSL server or client\n");
            printf ("        where protocol <protocol> = select protocol: SSL3, TLS1.0, TLS1.1, TLS1.2 \n");
            printf ("              time 0|1            = disable|enable certificate time validation (optional)\n");
            printf ("              domain 0|<name>     = disable|enable validation of peer's domain name against <name>\n");
            printf ("              alert 0|1           = disable|enable sending of SSL alert if certificate validation fails.\n");
            printf ("              cipher <cipher>     = select <cipher> (enum name) suite to use, can be repeated 8 times (optional)\n");
            printf ("    --ssl_add_cert server|client  certificate|calist [<name>] = add a certificate or CA list to either SSL server or client. \n");
            printf ("        where <name> = name of file to load from FLASH. A default certificate or CA list will be added if <name> is omitted.\n");
            printf ("    --ssl_store_cert <name> = store a certificate or CA list in FLASH with <name>\n");
            printf ("    --ssl_delete_cert [<name>] = delete the certificate or CA list with <name> from FLASH. \n");
            printf ("    --ssl_list_cert  = list the names of the certificates and CA lists stored in FLASH. \n");
#endif
            printf ("    --ota_upgrade <addr> <filename> <mode> <preserve_last> <protocol>. \n");
			printf ("    --ota_read <offset> <size> \n");
			printf ("    --ota_done \n");
			printf ("    --roaming <enable/disable> \n");
#endif /* ENABLE_STACK_OFFLOAD */
        }
    }
    
    return return_code;
} /* Endbody */

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : wifiCallback
* Returned Value : N/A
* Comments       : Called from driver on a WiFI connection event
*
*END------------------------------------------------------------------*/
void wifiCallback(int val, A_UINT8 devId,A_UINT8 * mac,A_BOOL bssConn)
{
	wifi_state = val;
	if(val == A_TRUE)
	{
	    wifi_connected_flag = 1;
	    printf("Connected for device=%d\n",devId);

            if(devId == 0){
            printf("%s connect event:\n",bssConn?"AP":"CLIENT");
            concurrent_connect_flag = 0x0E;
            }
            TURN_LED_ON;
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
                TURN_LED_OFF;
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

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ascii_to_hex()
* Returned Value  : hex counterpart for ascii
* Comments	: Converts ascii character to correesponding hex value
*
*END*-----------------------------------------------------------------*/
static unsigned char ascii_to_hex(char val)
{
	if('0' <= val && '9' >= val){
		return (unsigned char)(val - '0');
	}else if('a' <= val && 'f' >= val){
		return (unsigned char)(val - 'a') + 0x0a;
	}else if('A' <= val && 'F' >= val){
		return (unsigned char)(val - 'A') + 0x0a;
	}

	return 0xff;/* error */
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : set_passphrase
* Returned Value : A_OK
* Comments       : Store WPA Passphrase for later use
*
*END------------------------------------------------------------------*/
static A_INT32 set_passphrase(char* passphrase)
{
	strcpy(wpa_passphrase,passphrase);
	return A_OK;
}


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : set_wep_key
* Returned Value : A_ERROR on error else A_OK
* Comments       : Store WEP key for later use. Size of Key must be 10 or 26
*                  Hex characters
*
*END------------------------------------------------------------------*/
static A_INT32 set_wep_key(char* key_index, char* key_val)
{
    int key_idx = atoi(key_index);

    int i;
    if(strlen(key_val) != MIN_WEP_KEY_SIZE && strlen(key_val) != MAX_WEP_KEY_SIZE &&
	    strlen(key_val) != (MIN_WEP_KEY_SIZE/2) && strlen(key_val) != (MAX_WEP_KEY_SIZE/2))
    {
	printf("Invalid WEP Key length, only 10 or 26 HEX characters allowed (or) 5 or 13 ascii keys allowed\n\n");
	return A_ERROR;
    }

    if(key_idx < 1 || key_idx > 4)
    {
    	printf("Invalid key index, Please enter between 1-4\n\n");
    	return A_ERROR;
    }

    if((strlen(key_val) == MIN_WEP_KEY_SIZE) || (strlen(key_val) == MAX_WEP_KEY_SIZE))
    {
	for (i = 0; i < strlen(key_val); i++)
	{
	    if(ishexdigit(key_val[i]))
	    {
		continue;
	    }
	    else
	    {
    		printf("for hex enter [0-9] or [A-F]\n\n");
    		return A_ERROR;
	    }
	}
    }

    key[key_idx-1].key_index = 1;		//set flag to indicate occupancy
    memcpy(key[key_idx-1].key,key_val,strlen(key_val));
    return A_OK;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : clear_wep_keys
* Returned Value : Success
* Comments       : Clear wep keys
*
*END------------------------------------------------------------------*/
A_INT32 clear_wep_keys()
{
	int i;
	for(i=0;i<4;i++)
	{
		key[i].key_index = 0;
		key[i].key[0] = '\0';
	}
        return A_OK;
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : set_callback
* Returned Value : A_ERROR on error else A_OK
* Comments       : Sets callback function for WiFi connect/disconnect event
*
*END------------------------------------------------------------------*/
A_INT32 set_callback()
{
    int enet_device = get_active_device();
    return(qcom_set_connect_callback(enet_device, (void*)wifiCallback));
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : deviceid_handler
* Returned Value : A_ERROR on error else A_OK
* Comments       : Sets the Device ID in the driver
*
*END------------------------------------------------------------------*/
static A_INT32 deviceid_handler( A_INT32 index, A_INT32 argc, char* argv[])
{
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_UINT16 deviceid = atoi(argv[index]);
    A_INT32 error;
    if(wps_context.wps_in_progress)
    {
       printf("wps sesion in progress switching device not allowed:\n");
       return A_ERROR;
    }
    inout_param.cmd_id = ATH_SET_DEVICE_ID;
    inout_param.data = (void*)&deviceid;
    inout_param.length = sizeof(A_UINT16);

    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    error = HANDLE_IOCTL(&inout_param);
    return error;
}

static A_INT32 set_active_deviceid( A_UINT16 device_index)
{
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_UINT16 deviceid = device_index;
    A_INT32 error;
    if(wps_context.wps_in_progress)
    {
       printf("wps sesion in progress switching device not allowed:\n");
       return A_ERROR;
    }
    inout_param.cmd_id = ATH_SET_DEVICE_ID;
    inout_param.data = (void*)&deviceid;
    inout_param.length = sizeof(A_UINT16);

    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    error = HANDLE_IOCTL(&inout_param);
    return error;
}

#if ENABLE_SCC_MODE 

static A_UINT16 freq_to_channel(A_UINT16 channel_val)
{
    if(channel_val < 3000)
    {
        channel_val -= ATH_IOCTL_FREQ_1;
        if((channel_val/5) == 14)
        {   
            channel_val = 14;
        }
        else
        {
            channel_val = (channel_val/5) + 1;
        }
    }
    else
    {
        channel_val -= ATH_IOCTL_FREQ_36;
        channel_val = 36 + (channel_val/5); // since in 11a channel 36 is the starting number   
    }
    return channel_val;
}

static A_INT32 handle_connect_for_concurrent(A_UINT32 enet_device, char* ssid)
{
    A_UINT8 scan_ssid[MAX_SSID_LENGTH];

    memset(scan_ssid,0,MAX_SSID_LENGTH);    
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
    
    strcpy((char*)scan_ssid,ssid);
    strcpy((char*)ssid_str_concurrent, ssid);
    printf("remote ap ssid: %s\n", ssid_str_concurrent);
    if(qcom_set_ssid (enet_device,(char*)scan_ssid) != A_OK)
    {
      printf("Unable to set SSID for scan\n");
      return A_ERROR;
    }  

    ap_channel_hint = 0;    
    concurrent_connect_flag = 0x0F;        
    /*Do the actual scan*/
    wmi_set_scan(enet_device); 
    concurrent_connect_flag = 0x0E;    
    if( ap_channel_hint == 0)
    {
        printf("Unable to find AP: %s.....\n", ssid);
        return A_ERROR;
    }
    printf("find AP...\n");
    A_MDELAY(50);
         
    if(qcom_set_channel(enet_device, ap_channel_hint ) != A_OK )
    {
        printf("Unable to set AP channel:%d\n",ap_channel_hint);
        return A_ERROR;
    }
    A_MDELAY(50);
    return A_OK;
}



#endif

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : connect_handler()
* Returned Value  : A_OK - successful completion or
*					A_ERROR - failed.
* Comments		  : Handles Connect commands for infrastructure mode, Open
*                   WEP,WPA/WPA2 security is supported
*
*END*-----------------------------------------------------------------*/

static A_INT32 connect_handler( A_INT32 index, A_INT32 argc, char* argv[])
{
    A_INT32 error= A_OK, i;
    char ssid_str[64];
    int offset = 0, ssidLength=MAX_SSID_LENGTH ;
    A_UINT32 enet_device;
    A_UINT32 temp_mode;           
    
    enet_device = get_active_device();
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    set_callback();

#if ENABLE_SCC_MODE     
    if((concurrent_connect_flag == 0x0E) && (enet_device == 1))
    {
        error = handle_connect_for_concurrent(enet_device, argv[2]);
        if(error != A_OK)
        {
           printf("cannot connect for concurrent mode\n");
           return A_ERROR;
        }
    }
#endif
    
    qcom_op_get_mode(enet_device, &temp_mode);
    if (temp_mode == QCOM_WLAN_DEV_MODE_AP)
    {
		printf("setting to ap mode \n");
    }
    if ((SEC_MODE_OPEN == security_mode) && (temp_mode == QCOM_WLAN_DEV_MODE_AP))
    {
        printf("AP in OPEN mode!\n");
    }
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

    error = qcom_set_ssid (enet_device,ssid_str/*argv[index]*/);
    if (error != 0)
    {
        printf ("Error during setting of ssid %s error=%08x!\n",argv[index], error);
        return A_ERROR;
    }

    printf ("\nSetting SSID to %s \n\n",ssid_str/*argv[index]*/);
    strcpy((char*)original_ssid,ssid_str /*argv[index]*/);

    if (SEC_MODE_WEP == security_mode) {
        cipher = WLAN_CRYPT_WEP_CRYPT;
        error = qcom_sec_set_encrypt_mode(enet_device,cipher);

        if(error != A_OK)
        {
                return A_ERROR;
        }
    }      
    else if(SEC_MODE_WPA == security_mode)
    {
        error = qcom_sec_set_encrypt_mode(enet_device, cipher);
	if(error != A_OK)
	{
		return A_ERROR;
	}
	error = qcom_sec_set_auth_mode(enet_device, wpa_ver);
	if(error != A_OK)
	{
		return A_ERROR;
	}
	i++;
	if(strlen(wpa_passphrase) != 64)
	{
		error = qcom_sec_set_passphrase(enet_device,wpa_passphrase);
		if(error != A_OK)
		{
			return A_ERROR;
		}
		pmk_flag = 1;
	}
	else
	{
		if(qcom_sec_set_pmk(enet_device, wpa_passphrase)!= A_OK)
		{
			printf("Unable to set pmk\n");
			return A_ERROR;
		}
		pmk_flag = 1;
	}
	security_mode = SEC_MODE_OPEN;
    }

    error = qcom_commit(enet_device);
    if(error != A_OK)
    {
#if ENABLE_AP_MODE
        if(temp_mode == MODE_AP)
        {
            printf("failed to AP mode \n");
#if ENABLE_SCC_MODE 
            printf("support single concurrent channel only\n");
#endif                        
	    // Clear Invalid configurations
	    error = qcom_sec_set_auth_mode (enet_device,WLAN_AUTH_NONE);
	    if(error != A_OK)
	    {
		printf("Unable to clear Sec mode\n");
		return A_ERROR;
	    }
			
	    security_mode = SEC_MODE_OPEN;
	    pmk_flag = 0;
        }
#endif
    	return A_ERROR;
    }
#if ENABLE_AP_MODE
    if (temp_mode == MODE_AP)
	{
		/* setting the AP's default IP to 192.168.1.1 */
#if ENABLE_STACK_OFFLOAD
		//A_UINT32 ip_addr = 0xc0a80101,mask = 0xffffff00,gateway = 0xc0a80101;
		//t_ipconfig((void*)handle,IPCFG_STATIC, &ip_addr, &mask, &gateway);
#else

#define AP_IPADDR  IPADDR(192,168,1,1)
#define AP_IPMASK  IPADDR(255,255,255,0)
#define AP_IPGATEWAY  IPADDR(192,168,1,1)

                A_UINT32 error;
                IPCFG_IP_ADDRESS_DATA   ip_data;

                ip_data.ip = AP_IPADDR;
                ip_data.mask = AP_IPMASK;
                ip_data.gateway = AP_IPGATEWAY;

                error = ipcfg_bind_staticip (enet_device, &ip_data);
                if (error != 0)
                {
                    printf ("Error during static ip bind %08x!\n", error);
                    return SHELL_EXIT_ERROR;
                }
#endif
	}
#endif

    return A_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ad_hoc_connect_handler()
* Returned Value  : 1 - successful completion or
*					0 - failed.
* Comments		  : Handles Connect commands for ad-hoc mode, Open
*                   & WEP security is supported
*
*END*-----------------------------------------------------------------*/

static A_INT32 ad_hoc_connect_handler( A_INT32 index, A_INT32 argc, char* argv[])
{
    A_INT32 error= A_OK,i;
    int key_idx = 0;
    A_UINT32 enet_device;
    
    enet_device = get_active_device();

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    set_callback();
    qcom_op_set_mode (enet_device,QCOM_WLAN_DEV_MODE_ADHOC);
    error = qcom_set_ssid (enet_device,argv[index]);
    if (error != 0)
    {
        printf ("Error during setting of ssid %s error=%08x!\n",argv[index], error);
        return A_ERROR;
    }

    printf ("Setting SSID to %s \n\n",argv[index]);
	strcpy((char*)original_ssid,argv[index]);

	if(index+1 < argc)
	{
		i = index +1;
		if(ATH_STRCMP(argv[i],"--wep") == 0)
		{
		    if( argc < 4)  //User did not enter parameter
		    {
		    	printf("Missing parameters\n");
		    	return A_ERROR;
		    }

		    i++;
		    key_idx = atoi(argv[i]);

		    if(key_idx < 1 || key_idx > 4)
		    {
		    	printf("Invalid default key index, Please enter between 1-4\n");
		    	return A_ERROR;
		    }

		    if(key[key_idx-1].key_index != 1)
		    {
		    	printf("This key is not present, please enter WEP key first\n");
		    	return A_ERROR;
		    }

		    if(key[0].key[0] != '\0')
                        qcom_sec_set_wepkey(enet_device, 1, key[0].key);

		    if(key[1].key[0] != '\0')
                      qcom_sec_set_wepkey(enet_device, 2, key[1].key);

		    if(key[2].key[0] != '\0')
                      qcom_sec_set_wepkey(enet_device, 3, key[2].key);

		    if(key[3].key[0] != '\0')
                      qcom_sec_set_wepkey(enet_device, 4, key[3].key);

		    error = qcom_sec_set_wepkey_index (enet_device, key_idx);
		    if(error != A_OK)
		    {
		    	return A_ERROR;
		    }
		    clear_wep_keys();

		    if(argc > 5)
            {
              i++;
              if(ATH_STRCMP(argv[i],"--channel") == 0)
              {
                  if( argc < 7)  //User did not enter parameter
                  {
                      printf("Missing Channel parameter\n");
                      return A_ERROR;
                  }
                  i++;
                  printf("Setting Channel to %d\n",atoi(argv[i]));
                  if(qcom_set_channel(enet_device,atoi(argv[i])) == A_ERROR)
                      return A_ERROR;
              }
            }
		}
		else if(ATH_STRCMP(argv[i],"--channel") == 0)
		{
		    if( argc < 4)  //User did not enter parameter
			{
				printf("Missing Channel parameter\n");
				return A_ERROR;
			}
			i++;
		    printf("Setting Channel to %d\n",atoi(argv[i]));
		    if(qcom_set_channel(enet_device,atoi(argv[i])) == A_ERROR)
		      return A_ERROR;
		}
	}
    error = qcom_commit(enet_device);
    if(error != A_OK)
    {
    	return A_ERROR;
    }
    return A_OK;
}

#if ENABLE_P2P_MODE


/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : P2PCallbackfn
* Returned Value : N/A
* Comments       : Called from driver on a WiFI connection event
*
*END------------------------------------------------------------------*/
void P2PCallBackfn(A_UINT8 *ptr)
{
   A_UINT8 *local_ptr2 = ptr, inv_response_evt_index = 0;
   union {
       WMI_SET_PASSPHRASE_CMD          setPassPhrase;
       WMI_P2P_GRP_INIT_CMD            grpInit;
       WMI_P2P_SET_CMD                 p2p_set_params;
   }stkP2p;
   ATH_WPS_START wps_start;
   WMI_P2P_GO_NEG_RESULT_EVENT *p2pNeg;
   uint_32 error,chnl, loop_index;
   A_UINT32 enet_device, temp_device_id = 0;

   enet_device = get_active_device();
   if (((WMI_EVT_TO_APP *)ptr)->EVENT_ID == WMI_P2P_PROV_DISC_REQ_EVENTID)
   {
          local_ptr2 += sizeof(A_UINT32); 
          
          A_UINT16 wps_method;

            printf("\n source addr : %x:%x:%x:%x:%x:%x \n",    ((WMI_P2P_PROV_DISC_REQ_EVENT *)local_ptr2)->sa[0],
            ((WMI_P2P_PROV_DISC_REQ_EVENT *)local_ptr2)->sa[1],
            ((WMI_P2P_PROV_DISC_REQ_EVENT *)local_ptr2)->sa[2],
            ((WMI_P2P_PROV_DISC_REQ_EVENT *)local_ptr2)->sa[3],
            ((WMI_P2P_PROV_DISC_REQ_EVENT *)local_ptr2)->sa[4],
            ((WMI_P2P_PROV_DISC_REQ_EVENT *)local_ptr2)->sa[5]);

            printf("\n wps_config_method : %x \n",             A_LE2CPU16(((WMI_P2P_PROV_DISC_REQ_EVENT *)local_ptr2)->wps_config_method));

            wps_method = A_LE2CPU16(((WMI_P2P_PROV_DISC_REQ_EVENT *)local_ptr2)->wps_config_method);

            if(WPS_CONFIG_DISPLAY == wps_method)
            {
                printf("Provisional Disc Request - Display WPS PIN [%s] \n",p2p_wps_pin);  
              
            }else if(WPS_CONFIG_KEYPAD == wps_method)
            {
                printf("Provisional Disc Request - Enter WPS PIN \n");  
            }else if(WPS_CONFIG_PUSHBUTTON == wps_method)
            {
                printf("Provisional Disc Request - Push Button \n");  
            }else{
                printf("Invalid Provisional Request \n");
            }
   }
   else if (((WMI_EVT_TO_APP *)ptr)->EVENT_ID == WMI_P2P_REQ_TO_AUTH_EVENTID)
   {
	  local_ptr2 += sizeof(A_UINT32);
          printf("\n source addr : %x:%x:%x:%x:%x:%x \n",    ((WMI_P2P_REQ_TO_AUTH_EVENT *)local_ptr2)->sa[0],
                                                       ((WMI_P2P_REQ_TO_AUTH_EVENT *)local_ptr2)->sa[1],
                                                       ((WMI_P2P_REQ_TO_AUTH_EVENT *)local_ptr2)->sa[2],
                                                       ((WMI_P2P_REQ_TO_AUTH_EVENT *)local_ptr2)->sa[3],
                                                       ((WMI_P2P_REQ_TO_AUTH_EVENT *)local_ptr2)->sa[4],
                                                       ((WMI_P2P_REQ_TO_AUTH_EVENT *)local_ptr2)->sa[5]);

         printf("\n dev_password_id : %x \n",               ((WMI_P2P_REQ_TO_AUTH_EVENT *)local_ptr2)->dev_password_id);
   }
   else if (((WMI_EVT_TO_APP *)ptr)->EVENT_ID == WMI_P2P_LIST_PERSISTENT_NETWORK_EVENTID)
   {
         local_ptr2 += sizeof(A_UINT32);
         A_MEMZERO(p2p_pers_data, (MAX_LIST_COUNT * sizeof(WMI_PERSISTENT_MAC_LIST)));
         memcpy(p2p_pers_data, local_ptr2, (MAX_LIST_COUNT * sizeof(WMI_PERSISTENT_MAC_LIST)));
         printf("\n");
         if(p2p_cancel_enable == 0)
         {
           for(loop_index = 0; loop_index < MAX_LIST_COUNT; loop_index++)
           {
             
             printf("mac_addr[%d] : %x:%x:%x:%x:%x:%x  \n", loop_index, ((WMI_PERSISTENT_MAC_LIST *)(local_ptr2))->macaddr[0],
                    ((WMI_PERSISTENT_MAC_LIST *)(local_ptr2))->macaddr[1],
                    ((WMI_PERSISTENT_MAC_LIST *)(local_ptr2))->macaddr[2],
                    ((WMI_PERSISTENT_MAC_LIST *)(local_ptr2))->macaddr[3],
                    ((WMI_PERSISTENT_MAC_LIST *)(local_ptr2))->macaddr[4],
                    ((WMI_PERSISTENT_MAC_LIST *)(local_ptr2))->macaddr[5]);
             printf("ssid[%d] : %s \n", loop_index, ((WMI_PERSISTENT_MAC_LIST *)(local_ptr2))->ssid);
             if(((WMI_PERSISTENT_MAC_LIST *)(local_ptr2))->role == WMI_P2P_INVITE_ROLE_ACTIVE_GO)
             printf("passphrase[%d] : %s \n", loop_index, ((WMI_PERSISTENT_MAC_LIST *)(local_ptr2))->passphrase);
             local_ptr2 += sizeof(WMI_PERSISTENT_MAC_LIST);
           }
         }
         else
         {
           p2p_cancel_enable = 0;
         }
   }
   else if (((WMI_EVT_TO_APP *)ptr)->EVENT_ID == WMI_P2P_SDPD_RX_EVENTID)
   {
        local_ptr2 += sizeof(A_UINT32);
     	printf("Custom_Api_p2p_serv_disc_req event \n");
	printf("type : %d   frag id : %x \n", ((WMI_P2P_SDPD_RX_EVENT *)local_ptr2)->type, ((WMI_P2P_SDPD_RX_EVENT *)local_ptr2)->frag_id);
	printf("transaction_status : %x \n", ((WMI_P2P_SDPD_RX_EVENT *)local_ptr2)->transaction_status);
	printf("freq : %d status_code : %d comeback_delay : %d tlv_length : %d update_indic : %d \n",
												  A_LE2CPU16(((WMI_P2P_SDPD_RX_EVENT *)local_ptr2)->freq),
												  A_LE2CPU16(((WMI_P2P_SDPD_RX_EVENT *)local_ptr2)->status_code),
												  A_LE2CPU16(((WMI_P2P_SDPD_RX_EVENT *)local_ptr2)->comeback_delay),
												  A_LE2CPU16(((WMI_P2P_SDPD_RX_EVENT *)local_ptr2)->tlv_length),
												  A_LE2CPU16(((WMI_P2P_SDPD_RX_EVENT *)local_ptr2)->update_indic));
	printf("source addr : %x:%x:%x:%x:%x:%x \n", ((WMI_P2P_SDPD_RX_EVENT *)local_ptr2)->peer_addr[0],
												 ((WMI_P2P_SDPD_RX_EVENT *)local_ptr2)->peer_addr[1],
												 ((WMI_P2P_SDPD_RX_EVENT *)local_ptr2)->peer_addr[2],
												 ((WMI_P2P_SDPD_RX_EVENT *)local_ptr2)->peer_addr[3],
												 ((WMI_P2P_SDPD_RX_EVENT *)local_ptr2)->peer_addr[4],
												 ((WMI_P2P_SDPD_RX_EVENT *)local_ptr2)->peer_addr[5]);
   }   
   else if (((WMI_EVT_TO_APP *)ptr)->EVENT_ID == WMI_P2P_PROV_DISC_RESP_EVENTID)
   {   
            local_ptr2 += sizeof(A_UINT32);
            printf("\n peer addr : %x:%x:%x:%x:%x:%x \n",      ((WMI_P2P_PROV_DISC_RESP_EVENT *)local_ptr2)->peer[0], ((WMI_P2P_PROV_DISC_RESP_EVENT *)local_ptr2)->peer[1],
                                                               ((WMI_P2P_PROV_DISC_RESP_EVENT *)local_ptr2)->peer[2], ((WMI_P2P_PROV_DISC_RESP_EVENT *)local_ptr2)->peer[3],
                                                               ((WMI_P2P_PROV_DISC_RESP_EVENT *)local_ptr2)->peer[4], ((WMI_P2P_PROV_DISC_RESP_EVENT *)local_ptr2)->peer[5]);
            
            if(WPS_CONFIG_KEYPAD == A_LE2CPU16(((WMI_P2P_PROV_DISC_RESP_EVENT *)local_ptr2)->config_methods))
            {
                printf("Provisional Disc Response Keypad - WPS PIN [%s] \n",p2p_wps_pin);  
            }else if(WPS_CONFIG_DISPLAY == A_LE2CPU16(((WMI_P2P_PROV_DISC_RESP_EVENT *)local_ptr2)->config_methods))
            {
                printf("Provisional Disc Response Display \n");  
            }else if(WPS_CONFIG_PUSHBUTTON == A_LE2CPU16(((WMI_P2P_PROV_DISC_RESP_EVENT *)local_ptr2)->config_methods))
            {
                printf("Provisional Disc Response Push Button  \n");  
            }else{
                printf("Invalid Provisional Response \n");
            }
            //printf("\n config_methods : %x \n",                A_LE2CPU16(((WMI_P2P_PROV_DISC_RESP_EVENT *)local_ptr2)->config_methods));  
   }
   else if (((WMI_EVT_TO_APP *)ptr)->EVENT_ID == WMI_P2P_NODE_LIST_EVENTID)
   {
          local_ptr2 += sizeof(A_UINT32);
          A_UINT8  index, temp_val, *temp_ptr = NULL;
          //temp_val = ((WMI_P2P_NODE_LIST_EVENT *)(local_ptr2))->num_p2p_dev;
          temp_val = *local_ptr2;
          local_ptr2 ++;
          
          temp_ptr = local_ptr2;
          //local_ptr2 += sizeof(A_UINT8);
          p2p_session_in_progress = 1;
	   enet_device = get_active_device();
	   if(enet_device != 0)
	   {
		   /* P2P device should always send/receive events/commands on dev 0 if
			  the app has switched to dev 1 while event is in dev 0 send command
			  via dev 0 and then switch to dev 1*/
			  temp_device_id = enet_device;
			  enet_device = 0;
			  set_active_deviceid(enet_device);
	   }          
    if (temp_val > 0)
    {
        for (index = 0; index < temp_val; index++)
        {
          if(p2p_join_session_active) 
          {
              if(A_MEMCMP(((P2P_DEVICE_LITE *)(local_ptr2))->interface_addr, p2p_join_mac_addr, ATH_MAC_LEN) == 0)
              {
                 p2p_join_profile.go_oper_freq = ((P2P_DEVICE_LITE*)(local_ptr2))->oper_freq;
                 break;
              }
          }
          else
          {  
            printf("\n\t p2p_config_method     : %x \n",                        (((P2P_DEVICE_LITE *)(local_ptr2))->config_methods)); 
            printf(" \t p2p_device_name       : %s \n",                        ((P2P_DEVICE_LITE *)(local_ptr2))->device_name);
	    
            printf("\t p2p_primary_dev_type  : %x:%x:%x:%x:%x:%x:%x\n ",((P2P_DEVICE_LITE *)(local_ptr2))->pri_dev_type[0],((P2P_DEVICE_LITE *)(local_ptr2))->pri_dev_type[1],
			    ((P2P_DEVICE_LITE *)(local_ptr2))->pri_dev_type[2],((P2P_DEVICE_LITE *)(local_ptr2))->pri_dev_type[3],
			    ((P2P_DEVICE_LITE *)(local_ptr2))->pri_dev_type[4],((P2P_DEVICE_LITE *)(local_ptr2))->pri_dev_type[5],
			    ((P2P_DEVICE_LITE *)(local_ptr2))->pri_dev_type[6],((P2P_DEVICE_LITE *)(local_ptr2))->pri_dev_type[7]);

            printf("\t p2p_interface_addr    : %x:%x:%x:%x:%x:%x \n",         ((P2P_DEVICE_LITE *)(local_ptr2))->interface_addr[0],
                    ((P2P_DEVICE_LITE *)(local_ptr2))->interface_addr[1],
                    ((P2P_DEVICE_LITE *)(local_ptr2))->interface_addr[2],
                    ((P2P_DEVICE_LITE *)(local_ptr2))->interface_addr[3],
                    ((P2P_DEVICE_LITE *)(local_ptr2))->interface_addr[4],
                    ((P2P_DEVICE_LITE *)(local_ptr2))->interface_addr[5]);
            printf("\t p2p_device_addr       : %x:%x:%x:%x:%x:%x \n",         ((P2P_DEVICE_LITE *)(local_ptr2))->p2p_device_addr[0],
                    ((P2P_DEVICE_LITE *)(local_ptr2))->p2p_device_addr[1],
                    ((P2P_DEVICE_LITE *)(local_ptr2))->p2p_device_addr[2],
                    ((P2P_DEVICE_LITE *)(local_ptr2))->p2p_device_addr[3],
                    ((P2P_DEVICE_LITE *)(local_ptr2))->p2p_device_addr[4],
                    ((P2P_DEVICE_LITE *)(local_ptr2))->p2p_device_addr[5]);
            printf("\t p2p_device_capability : %x \n",                        ((P2P_DEVICE_LITE *)(local_ptr2))->dev_capab);
            printf("\t p2p_group_capability  : %x \n",                        ((P2P_DEVICE_LITE *)(local_ptr2))->group_capab);          
            printf("\t p2p_wps_method        : %x \n",                        ((P2P_DEVICE_LITE *)(local_ptr2))->wps_method);
            //printf("\t Peer Listen channel   : %d \n",                        A_LE2CPU16(((struct p2p_device *)(local_ptr2->dataptr))->listen_freq));
            printf("\t Peer Oper   channel   : %d \n",                        A_LE2CPU16(((P2P_DEVICE_LITE *)(local_ptr2))->oper_freq)); 
          }
             local_ptr2     += sizeof(P2P_DEVICE_LITE);
        }
        if(p2p_join_session_active) 
        {        
          p2p_join_session_active = 0;
          stkP2p.p2p_set_params.val.mode.p2pmode = P2P_CLIENT;
          if( qcom_p2p_set(enet_device, P2P_CONFIG_P2P_OPMODE, &stkP2p.p2p_set_params.val, sizeof(stkP2p.p2p_set_params.val.mode)) != A_OK )                
          {
            printf("\nStartP2P JOIN SET command did not execute properly\n");
            p2p_session_in_progress = 0;             
            return;
          } 
          if(qcom_p2p_func_join(enet_device, p2p_join_profile.wps_method, &p2p_join_mac_addr[0], p2p_wps_pin, p2p_join_profile.go_oper_freq) != A_OK)
          {
            printf("\nP2P JOIN command did not execute properly\n");
#if ENABLE_SCC_MODE
            printf("support single concurrent channel only....\n");
#endif
            p2p_session_in_progress = 0; 
            return;
          }        
        }
    }
    local_ptr2 = temp_ptr;  
    if(temp_device_id != 0){
       set_active_deviceid(temp_device_id);
    }
    p2p_session_in_progress = 0;
    if (temp_val > 0)
	   printf("shell> ");
    }
    else if (((WMI_EVT_TO_APP *)ptr)->EVENT_ID == WMI_P2P_INVITE_SENT_RESULT_EVENTID)
    {
      local_ptr2 += sizeof(A_UINT32);   
      printf("Invitation Result %d\n",((WMI_P2P_INVITE_SENT_RESULT_EVENT *)(local_ptr2))->status);
    
      if(((WMI_P2P_INVITE_SENT_RESULT_EVENT *)(local_ptr2))->status == 0) 
      {
      printf("SSID %x:%x:%x:%x:%x:%x \n",((WMI_P2P_INVITE_SENT_RESULT_EVENT *)(local_ptr2))->bssid[0], ((WMI_P2P_INVITE_SENT_RESULT_EVENT *)(local_ptr2))->bssid[1],
                                         ((WMI_P2P_INVITE_SENT_RESULT_EVENT *)(local_ptr2))->bssid[2], ((WMI_P2P_INVITE_SENT_RESULT_EVENT *)(local_ptr2))->bssid[3],
                                         ((WMI_P2P_INVITE_SENT_RESULT_EVENT *)(local_ptr2))->bssid[4], ((WMI_P2P_INVITE_SENT_RESULT_EVENT *)(local_ptr2))->bssid[5]);
      }
      
        if((p2p_pers_data[invitation_index].role == WMI_P2P_INVITE_ROLE_ACTIVE_GO)&& (p2p_persistent_done == 0) && (((WMI_P2P_INVITE_SENT_RESULT_EVENT *)(local_ptr2))->status == 0))
        {
            p2p_session_in_progress = 1;
	    enet_device = get_active_device();
	    if(enet_device != 0)
	    {
	        /* P2P device should always send/receive events/commands on dev 0 if
		   the app has switched to dev 1 while event is in dev 0 send command
		   via dev 0 and then switch to dev 1*/
	        temp_device_id = enet_device;
	        enet_device = 0;
                set_active_deviceid(enet_device);
	    }
            wps_flag = 0x01;
      
            printf("Starting Autonomous GO \n");
            if(qcom_p2p_func_start_go(enet_device, NULL, NULL, P2P_AUTO_CHANNEL, 1) != A_OK)
            {
                printf("\nStartP2P command did not execute properly\n");
                return;
            }
      
            p2p_persistent_done = 1;
      
            if(temp_device_id != 0){
                set_active_deviceid(temp_device_id);
            }
            p2p_session_in_progress = 0;
        }
   }
   else if (((WMI_EVT_TO_APP *)ptr)->EVENT_ID == WMI_P2P_INVITE_RCVD_RESULT_EVENTID)
   {
        local_ptr2 += sizeof(A_UINT32);
        int i;
        printf("Invite Result Status : %x \n",((WMI_P2P_INVITE_RCVD_RESULT_EVENT  *)(local_ptr2))->status);
        if(((WMI_P2P_INVITE_RCVD_RESULT_EVENT  *)(local_ptr2))->status == 0)
        {
            for (i=0;i<ATH_MAC_LEN;i++)
            {
                printf(" [%x] ",((WMI_P2P_INVITE_RCVD_RESULT_EVENT  *)(local_ptr2))->sa[i]);
            }
        }
        else
        {
          qcom_p2p_func_stop_find(enet_device);
        }
        printf("\n");
        if((p2p_pers_data[inv_response_evt_index].role == WMI_P2P_INVITE_ROLE_ACTIVE_GO)&& (p2p_persistent_done == 0) && (((WMI_P2P_INVITE_RCVD_RESULT_EVENT  *)(local_ptr2))->status == 0))
        {
            //
            // make AP Mode and WPS default settings for P2P GO
            //
            p2p_session_in_progress = 1; 
            enet_device = get_active_device();
	    if(enet_device != 0)
	    {
		/* P2P device should always send/receive events/commands on dev 0 if
		the app has switched to dev 1 while event is in dev 0 send command
		via dev 0 and then switch to dev 1*/
		temp_device_id = enet_device;
		enet_device = 0;
		set_active_deviceid(enet_device);
            }
          
            wps_flag = 0x01;
            printf("Starting Autonomous GO \n");
           
            if(qcom_p2p_func_start_go(enet_device, p2p_pers_data[inv_response_evt_index].ssid, p2p_pers_data[inv_response_evt_index].passphrase, P2P_AUTO_CHANNEL, 1) != A_OK)
            {
                printf("\nStartP2P command did not execute properly\n");
                return;
            }
     
            p2p_persistent_done = 1;
            inv_response_evt_index = 0;

            if(temp_device_id != 0){
                set_active_deviceid(temp_device_id);
            }
            p2p_session_in_progress = 0;            
        }    
   }
   else if (((WMI_EVT_TO_APP *)ptr)->EVENT_ID == WMI_P2P_INVITE_REQ_EVENTID)
   {
        local_ptr2 += sizeof(A_UINT32); 
        
        int i;
        WMI_P2P_FW_INVITE_REQ_RSP_CMD invite_rsp_cmd;
        
        printf("Invitation Req Received From : ");
        for (i = 0; i < ATH_MAC_LEN; i++)
        {
          printf(" %x: ",((WMI_P2P_FW_INVITE_REQ_EVENT *)(local_ptr2))->sa[i]);
          
        }
        printf("\n");
        
        A_MEMZERO(&invite_rsp_cmd,sizeof(WMI_P2P_FW_INVITE_REQ_RSP_CMD));
        
	    if (((WMI_P2P_FW_INVITE_REQ_EVENT *)(local_ptr2))->is_persistent)
	    {  
	        for (i = 0; i < MAX_LIST_COUNT; i++)
	        {
              if(A_MEMCMP(((WMI_P2P_FW_INVITE_REQ_EVENT *)(local_ptr2))->sa, p2p_pers_data[i].macaddr, ATH_MAC_LEN) == 0)
		       {
		            invite_rsp_cmd.status = 0;
                    inv_response_evt_index = i;
		            A_MEMCPY(invite_rsp_cmd.group_bssid, p2p_pers_data[i].macaddr, ATH_MAC_LEN);
		            break;
		        }
	        }

	        if(i == MAX_LIST_COUNT)
	        {
	             invite_rsp_cmd.status = 1;
	             i = 0;
	        }
        }   
        else
        {
            invite_rsp_cmd.status = 0;
            A_MEMCPY(invite_rsp_cmd.group_bssid,((WMI_P2P_FW_INVITE_REQ_EVENT *)(local_ptr2))->sa,ATH_MAC_LEN);
        }
            p2p_session_in_progress = 1;
	    enet_device = get_active_device();
	    if(enet_device != 0)
	    {
		   /* P2P device should always send/receive events/commands on dev 0 if
			  the app has switched to dev 1 while event is in dev 0 send command
			  via dev 0 and then switch to dev 1*/
			  temp_device_id = enet_device;
			  enet_device = 0;
			  set_active_deviceid(enet_device);
	    }
           /* send invite auth event */
           if(qcom_p2p_func_invite_auth(enet_device, (A_UINT8 *)&invite_rsp_cmd) != A_OK)
           {
             printf("\nStartP2P (P2P invite auth persistent)command did not execute properly\n");
             return;
           }
           
           if(temp_device_id != 0){
              set_active_deviceid(temp_device_id);
           }
           p2p_session_in_progress = 0;           
    }
    else if (((WMI_EVT_TO_APP *)ptr)->EVENT_ID == WMI_P2P_GO_NEG_RESULT_EVENTID)
    {
        local_ptr2 += sizeof(A_UINT32); 
        p2pNeg = (WMI_P2P_GO_NEG_RESULT_EVENT *)local_ptr2;
        task2_msec = 100;
        printf("P2P GO Negotion Result :\n");
        printf("Status       :%s \n",(p2pNeg->status) ? "FAILURE":"SUCCESS");
        printf("P2P Role     :%s \n",(p2pNeg->role_go) ? "P2P GO": "P2P Client");
        printf("SSID         :%s \n",p2pNeg->ssid);
        printf("Channel      :%d \n",p2pNeg->freq);
        printf("WPS Method   :%s \n",(p2pNeg->wps_method == WPS_PBC) ? "PBC": "PIN");
        p2p_session_in_progress = 1;
        enet_device = get_active_device();
	if(enet_device != 0)
	{
	    /* P2P device should always send/receive events/commands on dev 0 if
		the app has switched to dev 1 while event is in dev 0 send command
		via dev 0 and then switch to dev 1*/
		temp_device_id = enet_device;
		enet_device = 0;
		set_active_deviceid(enet_device);
	}
        if(p2pNeg->status != 0)
        {
          p2p_persistent_go = 0;
          qcom_p2p_func_stop_find(enet_device);
        }
        if((p2pNeg->status == 0) && (p2pNeg->role_go == 1)) 
        {
            wps_flag = 0x01;
            chnl = (p2pNeg->freq-2412)/5+1;     
            stkP2p.grpInit.persistent_group = p2p_persistent_go;
               
            /* Reset global p2p_persistent_go variable */
            p2p_persistent_go = 0;
            if(qcom_p2p_func_start_go(enet_device, p2pNeg->ssid, (A_UINT8 *)p2pNeg->pass_phrase, chnl, stkP2p.grpInit.persistent_group) != A_OK)
            {
                printf("\nStartP2P command did not execute properly\n");
                return;
            }
			
			/* the application should block here waiting for the connect event before issuing wps_start */
#if ENABLE_FPGA_BUILD
            app_time_delay(5000);
#else
            app_time_delay(2000);
#endif
       } else if((p2pNeg->status == 0) &&
               (p2pNeg->role_go == 0)) {
               error = qcom_set_ssid (enet_device,(char*)p2pNeg->ssid);
               if(error != A_OK)
               {
                   printf("Unable to set SSID\n");
               }
       }
       /* do WPS */
       if(p2pNeg->status == 0)
       {
           qcom_wps_credentials_t wpsCreden;
           A_MEMZERO(&wps_start, sizeof(ATH_WPS_START));
           
           if((p2pNeg->role_go == 0)) {
               qcom_op_set_mode (enet_device,QCOM_WLAN_DEV_MODE_STATION);
           } else if ((p2pNeg->role_go == 1)){
               qcom_op_set_mode (enet_device,QCOM_WLAN_DEV_MODE_AP);
           }
          
           wps_context.connect_flag = (p2pNeg->role_go) ? 0:8;
             
           if(p2pNeg->wps_method != WPS_PBC){
               wps_start.wps_mode = 0;
               wps_start.pin_length = 8;
               //FIXME: This hardcoded pin value needs to be changed
               // for production to reflect what is on a sticker/label
               memcpy (wps_start.pin, p2p_wps_pin, wps_start.pin_length);
               wps_start.pin[wps_start.pin_length] = '\0';
           }else{
               wps_start.wps_mode = 1;
           }

           memcpy(wpsCreden.ssid,p2pNeg->ssid,sizeof(p2pNeg->ssid));
           //memcpy(wpsCreden.mac_addr,p2pNeg->peer_device_addr,ATH_MAC_LEN);
           memcpy(wpsCreden.mac_addr,p2pNeg->peer_interface_addr,ATH_MAC_LEN);
           wpsCreden.ap_channel = p2pNeg->freq;
           wpsCreden.ssid_len = p2pNeg->ssid_len;

           qcom_wps_set_credentials(enet_device, &wpsCreden); 
           /* this starts WPS on the Aheros wifi */
	   if(qcom_wps_start(enet_device, wps_context.connect_flag, wps_start.wps_mode, (char *)wps_start.pin) != A_OK)           
           {
               return;
           }
           wps_context.wps_in_progress = 1;
       }
       if(temp_device_id != 0){
           set_active_deviceid(temp_device_id);
       } 
       p2p_session_in_progress = 0;
   }   
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : p2p_callback
* Returned Value : A_ERROR on error else A_OK
* Comments       : Sets callback function for WiFi P2P events
*
*END------------------------------------------------------------------*/
A_INT32 p2p_callback(A_UINT8 evt_flag)
{
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_INT32 error;
    WMI_EVT_TO_APP p2p_evt;
    if (evt_flag == 1)
    {
      inout_param.cmd_id = ATH_P2P_EVENT_CALLBACK;
    } 
    else
    { 
      inout_param.cmd_id = ATH_SET_P2P_CALLBACK;
    }
    A_MEMZERO(&p2p_evt, sizeof(WMI_EVT_TO_APP));
    inout_param.data = (void*)&p2p_evt;
    inout_param.length = sizeof(WMI_EVT_TO_APP);
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    error = HANDLE_IOCTL(&inout_param);
     if (error == A_OK)
    {
      P2PCallBackfn((A_UINT8 *)&p2p_evt);
    }
    
    return error;
}


#endif

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : set_wpa()
* Returned Value  : A_OK - successful completion or
*		    A_ERROR - failed.
* Comments	: Sets WPA mode
*
*END*-----------------------------------------------------------------*/
static A_INT32 set_wpa( A_INT32 index, A_INT32 argc, char* argv[])
{
    int i = index;
    A_UINT32 enet_device, temp_mode;

    if(5 != argc)  //User did not enter parameter
    {
	printf("Missing parameters\n\n");
	return A_ERROR;
    }

    enet_device = get_active_device();
        
    /********** wpa version ****************/
    if(ATH_STRCMP(argv[i],"1")==0)
    {
	wpa_ver = WLAN_AUTH_WPA_PSK;

	//this is a hack we currently only support wpa2 AES in AP mode
        qcom_op_get_mode(enet_device, &temp_mode);
        if (temp_mode == QCOM_WLAN_DEV_MODE_AP)                
	{
	    wpa_ver = WLAN_AUTH_WPA2_PSK;
	}
    }
    else if(ATH_STRCMP(argv[i],"2")==0)
    {
	wpa_ver = WLAN_AUTH_WPA2_PSK;
    }
    else
    {
	printf("Invalid version\n");
	return A_ERROR;
    }

    i++;
    /**************** cipher **********/
    if (!ATH_STRCMP(argv[i], argv[i+1]))
    {
        if (!ATH_STRCMP(argv[i], "TKIP"))
        {
            cipher = WLAN_CRYPT_TKIP_CRYPT;
        }
        else if (!ATH_STRCMP(argv[i], "CCMP"))
        {
            cipher = WLAN_CRYPT_AES_CRYPT;
        }
        else
        {
            printf("invaid uchipher mcipher, should be TKIP or CCMP\n");
            return A_ERROR;
        }
    }
    else
    {
        printf("invaid uchipher mcipher, should be same\n");
        return A_ERROR;
    }
                                
    security_mode = SEC_MODE_WPA;
    return A_OK;
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : set_wep()
* Returned Value  : A_OK - successful completion or
*		    A_ERROR - failed.
* Comments	: Sets WEP parameters
*
*END*-----------------------------------------------------------------*/
static A_INT32 set_wep( A_INT32 index, A_INT32 argc, char* argv[])
{
    int i = index;
    int key_idx = 0;
    A_INT32 error= A_OK;
    A_UINT32 enet_device;
    
    enet_device = get_active_device();

    if( argc < 3)  //User did not enter parameter
    {
	printf("Missing parameters\n");
	return A_ERROR;
    }

    key_idx = atoi(argv[i]);

    if(key_idx < 1 || key_idx > 4)
    {
	printf("Invalid default key index, Please enter between 1-4\n");
	return A_ERROR;
    }

    if(key[key_idx-1].key_index != 1)
    {
	printf("This key is not present, please enter WEP key first\n");
	return A_ERROR;
    }

    if(key[0].key[0] != '\0')
        qcom_sec_set_wepkey(enet_device, 1, key[0].key);

    if(key[1].key[0] != '\0')
        qcom_sec_set_wepkey(enet_device, 2, key[1].key);

    if(key[2].key[0] != '\0')
        qcom_sec_set_wepkey(enet_device, 3, key[2].key);

    if(key[3].key[0] != '\0')
        qcom_sec_set_wepkey(enet_device, 4, key[3].key);

    error = qcom_sec_set_wepkey_index (enet_device, key_idx);

    security_mode = SEC_MODE_WEP;
    clear_wep_keys();

    return error;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : wps_query()
* Returned Value  : 0 - success, 1 - failure
* Comments	: Queries WPS status
*
*END*-----------------------------------------------------------------*/
A_INT32 wps_query(A_UINT8 block)
{
    ATH_NETPARAMS netparams;
    A_INT32 status = 1;
    A_UINT32 enet_device, wifimode;
    
    enet_device = get_active_device();  
    if (wps_context.wps_in_progress){
        qcom_op_get_mode(enet_device, &wifimode);      
        if (CompleteWPS(&netparams, block,wifimode)){
            A_MEMCPY(&wps_context.netparams,&netparams,sizeof(ATH_NETPARAMS));
      	
            if(netparams.error)
            {
                wps_context.connect_flag=0;
            }
      
            if(wps_context.connect_flag){
#if ENABLE_SCC_MODE
              do {
                if((concurrent_connect_flag == 0x0E) && (enet_device == 1) )
                {
                    printf("ap_ch:%d\n", netparams.ap_channel);           
                    if(qcom_set_channel(enet_device, freq_to_channel(netparams.ap_channel)) != A_OK )
                    {
                        printf("Unable to set to AP channel:%d\n",netparams.ap_channel); 
                        break;
                    }
                    A_MDELAY(50);
                }
                if(ATH_NET_CONNECT(&netparams)){
                     printf("wps connection failed\n");
                }
              }
              while(0);
#else
                if(ATH_NET_CONNECT(&netparams)){
                    printf("connection failed\n");
                }
#endif         
            }
            status = 0;
            printf("Complete WPS Case %d\n",wps_context.wps_in_progress);
            if(wps_context.wps_in_progress == 1) {
                set_power_mode( REC_POWER,WPS);          
            }
            wps_context.wps_in_progress = 0;
        }
        else
        {
            if (A_EBUSY != netparams.error){
                wps_context.wps_in_progress = 0;
                set_power_mode( REC_POWER,WPS);          
                A_MEMCPY(&wps_context.netparams,&netparams,sizeof(ATH_NETPARAMS));
                status = 0;
            }
        }
    }
  
    return status;
}


static A_INT32 allow_aggr(A_INT32 argc, char* argv[])
{
	ATH_SET_AGGREGATION_PARAM param;
        A_UINT32 enet_device;
        enet_device = get_active_device();
        
	do{
		if(argc != 2){
			break;
		}

		param.txTIDMask = mystrtoul(argv[0], NULL, 16);
		param.rxTIDMask = mystrtoul(argv[1], NULL, 16);

		if(param.txTIDMask > 0xff || param.rxTIDMask > 0xff){
			break;
		}


                if(A_OK != qcom_allow_aggr_set_tid(enet_device, param.txTIDMask, param.rxTIDMask)){ 
	           printf("ERROR: driver command failed\n");
                   return A_ERROR;
                }

    	        return A_OK;
        }while(0);

        printf ("wmiconfig --allow_aggr <tx_tid_mask> <rx_tid_mask> Enables aggregation based on the provided bit mask where\n");
        printf ("each bit represents a TID valid TID's are 0-7\n");

        return A_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : test_promiscuous()
* Returned Value  : A_OK - success
* Comments	: Tests promiscuous mode
*
*END*-----------------------------------------------------------------*/
static A_INT32 test_promiscuous(A_INT32 argc, char* argv[])
{
    ATH_IOCTL_PARAM_STRUCT param;
    ATH_PROMISCUOUS_MODE prom_mode_details;
    
    param.cmd_id = ATH_SET_PROMISCUOUS_MODE;
    param.data = &prom_mode_details;
    param.length = sizeof(prom_mode_details);

    A_MEMZERO(&prom_mode_details, sizeof(ATH_PROMISCUOUS_MODE));

    prom_mode_details.enable = atoi(argv[0]);
    /* Set to MAX PERF Mode */
    set_power_mode( MAX_PERF_POWER,PWR_MAX);          
        
   if(argc > 1){
	prom_mode_details.filter_flags = atoi(argv[1]);

	if(argc > 2){
	   	if(mystrtomac(argv[2], &(prom_mode_details.src_mac[0])))
	   	{
	   		printf("ERROR: MAC address translation failed.\n");
	   		return A_OK;
	   	}

	   	if(argc > 3){
		   	if(mystrtomac(argv[3], &(prom_mode_details.dst_mac[0])))
		   	{
		   		printf("ERROR: MAC address translation failed.\n");
		   		return A_OK;
		   	}
		}
	}
    }

    prom_mode_details.cb = application_frame_cb;
    /* call the driver */
    if (A_OK != HANDLE_IOCTL (&param))
    {
        printf("ERROR: promiscuous command failed\n");
    }

    return A_OK;
}

/* test_raw -- makes use of special API's to send a raw MAC frame irregardless of connection state
 */
static A_INT32 test_raw(A_INT32 argc, char* argv[])
{
    A_UINT8 rate_index;
    A_UINT8 tries;
    A_UINT8 header_type;
    A_UINT32 i;
    A_UINT32 chan, size, buf_len;
    A_UINT32 status = A_ERROR;
    A_UINT8 addr[4][6];
    QCOM_RAW_MODE_PARAM_t para;
    
    /* collect user inputs if any */

    if((argc < 5) || (argc > 9)){
      goto raw_usage;
    }
    
    rate_index = atoi(argv[0]);
    tries = atoi(argv[1]);
    size = atoi(argv[2]);
    chan = atoi(argv[3]);
    header_type = atoi(argv[4]);
    A_MEMZERO(&addr[0][0], sizeof(addr));
    
    for(i = 0; i < (argc-5);i++) {
        if(mystrtomac(argv[5+i], &(addr[i][0])))
        {
            printf("ERROR: MAC address translation failed.\n");
            return status;
        }
    }
    
    if ( chan < 1 || chan > 13 ) {
        goto raw_usage;
    }
    
    if( argc == 5 )
    {
	addr[0][0] = 0xff;
	addr[0][1] = 0xff;
	addr[0][2] = 0xff;
	addr[0][3] = 0xff;
	addr[0][4] = 0xff;
	addr[0][5] = 0xff;
	addr[1][0] = 0x00;
	addr[1][1] = 0x03;
	addr[1][2] = 0x7f;
	addr[1][3] = 0xdd;
	addr[1][4] = 0xdd;
	addr[1][5] = 0xdd;
	addr[2][0] = 0x00;
	addr[2][1] = 0x03;
	addr[2][2] = 0x7f;
	addr[2][3] = 0xdd;
	addr[2][4] = 0xdd;
	addr[2][5] = 0xdd;
	addr[3][0] = 0x00;
	addr[3][1] = 0x03;
	addr[3][2] = 0x7f;
	addr[3][3] = 0xee;
	addr[3][4] = 0xee;
	addr[3][5] = 0xee;
        if(header_type == 1) {
            A_MEMCPY(&addr[0][0], &addr[1][0], ATH_MAC_LEN);
            //change destination address
            addr[2][3] = 0xaa;
            addr[2][4] = 0xaa;
            addr[2][5] = 0xaa;
        }        
    }
    
    para.rate_index = rate_index;
    para.tries = tries;
    para.size = size;
    para.chan = chan;
    para.header_type = header_type;
    para.seq = 0;
    A_MEMCPY(&para.addr1.addr[0], addr[0], ATH_MAC_LEN);
    A_MEMCPY(&para.addr2.addr[0], addr[1], ATH_MAC_LEN);
    A_MEMCPY(&para.addr3.addr[0], addr[2], ATH_MAC_LEN);
    A_MEMCPY(&para.addr4.addr[0], addr[3], ATH_MAC_LEN);
    para.pdatabuf = NULL;
    para.buflen = 0;
		
    status = qcom_raw_mode_send_pkt(&para);
    if( status != A_OK)
    {
raw_usage:
        printf("raw input error\n");
    	printf("usage = wmiconfig --raw rate num_tries num_bytes channel header_type [addr1 [addr2 [addr3 [addr4]]]]\n");
	printf("rate = rate index where 0==1mbps; 1==2mbps; 2==5.5mbps etc\n");
   	printf("num_tries = number of transmits 1 - 14\n");
   	printf("num_bytes = payload size 0 to 1400\n");
   	printf("channel = 1 - 11\n");
   	printf("header_type = 0==beacon frame; 1==QOS data frame; 2==4 address data frame\n");
        printf("addr1 = mac address xx:xx:xx:xx:xx:xx\n");
        printf("addr2 = mac address xx:xx:xx:xx:xx:xx\n");
        printf("addr3 = mac address xx:xx:xx:xx:xx:xx\n");
        printf("addr4 = mac address xx:xx:xx:xx:xx:xx\n");        
    }
    
    return status;
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : wps_handler()
* Returned Value  : 1 - successful completion or
*					0 - failed.
* Comments		  : Handles WPS related functionality
*
*END*-----------------------------------------------------------------*/
static A_INT32 wps_handler( A_INT32 index, A_INT32 argc, char* argv[])
{
    A_UINT32 i;
    A_UINT8 val;
    A_UINT8 wps_mode = 0;
    ATH_NETPARAMS netparams;
    char delimter[] = "-#";
    char *result = NULL;
    qcom_wps_credentials_t wpsScan, *wpsScan_p;
    int j;
    A_UINT32 enet_device;
    enet_device = get_active_device();      

    //init context
    wps_context.wps_in_progress = 0;
    wps_context.connect_flag = 0;
    A_MEMZERO(&netparams, sizeof(ATH_NETPARAMS));
    A_MEMZERO(wpsPin, MAX_WPS_PIN_SIZE);
    if(index+1 < argc)
    {
	i = index;
	wps_context.connect_flag = atoi(argv[i]);

	i++;
		if(ATH_STRCMP(argv[i],"push") == 0)
		{
			wps_mode = 1;
		}
		else if (ATH_STRCMP(argv[i],"pin") == 0)
		{
			wps_mode = 0;
		    if( argc < 4)  //User did not enter parameter
		    {
		    	printf("Missing parameters: please enter pin\n");
		    	return A_ERROR;
		    }

		    i++;

			result = strtok( argv[i], delimter );
			if(result == NULL)
			{
				strcpy(wpsPin, argv[i]);
			}
			else
			{
				while( result != NULL )
				{
					strcat(wpsPin, result);
					result = strtok( NULL, delimter );
				}
			}
		}

		i++;

		if(argc > i)
		{
			if((argc-i) < 3)
			{
				printf("missing parameters\n");
				return A_ERROR;
			}
			else
			{
				strcpy((char*)(wpsScan.ssid),argv[i]);
                                wpsScan.ssid_len = strlen((char *)wpsScan.ssid);
				i++;

				if(strlen(argv[i]) != 12)
				{
					printf("Invalid MAC address length\n");
					return A_ERROR;
				}

				for(j=0;j<6;j++)
				{
					wpsScan.mac_addr[j] = 0;
				}
				for(j=0;j<strlen(argv[i]);j++)
				{
					val = ascii_to_hex(argv[i][j]);
					if(val == 0xff)
					{
						printf("Invalid character\n");
						return A_ERROR;
					}
					else
					{
						if((j&1) == 0)
							val <<= 4;

						wpsScan.mac_addr[j>>1] |= val;
					}
				}

				i++;
				wpsScan.ap_channel = channel_array[atoi(argv[i])];

			}
			wpsScan_p = &wpsScan;
		}
		else
		{
			wpsScan_p = NULL;
		}
	}
        set_power_mode( MAX_PERF_POWER,WPS);
        
        qcom_wps_set_credentials(enet_device, wpsScan_p); 
	if(qcom_wps_start(enet_device, wps_context.connect_flag, wps_mode, wpsPin) != A_OK)
	{
		printf("WPS connect error\n");
                set_power_mode( REC_POWER,WPS);
        	return A_ERROR;
	}

    wps_context.wps_in_progress = 1;
    return A_OK;
}
#if ENABLE_P2P_MODE
#define isdigit(c)  ( (((c) >= '0') && ((c) <= '9')) ? (1) : (0) )
/*
 * Input an Ethernet address and convert to binary.
 */

static A_STATUS wmic_ether_aton(const char *orig, A_UINT8 *eth)
{
  const char *bufp;
  int i;

  i = 0;
  for(bufp = orig; *bufp != '\0'; ++bufp) {
    unsigned int val;
    unsigned char c = *bufp++;
    if (isdigit(c)) val = c - '0';
    else if (c >= 'a' && c <= 'f') val = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F') val = c - 'A' + 10;
    else break;

    val <<= 4;
    c = *bufp++;
    if (isdigit(c)) val |= c - '0';
    else if (c >= 'a' && c <= 'f') val |= c - 'a' + 10;
    else if (c >= 'A' && c <= 'F') val |= c - 'A' + 10;
    else break;

    eth[i] = (unsigned char) (val & 0377);
    if(++i == ATH_MAC_LEN) {
        /* That's it.  Any trailing junk? */
        if (*bufp != '\0') {
            printf("iw_ether_aton(%s): trailing junk!\n", orig);
            return(A_EINVAL);
        }
        return(A_OK);
    }
    if (*bufp != ':')
        break;
  }
  return(A_EINVAL);
}

#endif


/*FUNCTION*--------------------------------------------------------------------
*
* Function Name   : ath_assert_dump()
* Returned Value  : A_OK - on successful completion
*					A_ERROR - on any failure.
* Comments        : causes assert information to be collected from chip and
*				    printed to stdout.
*
*END*------------------------------------------------------------------------*/
static A_INT32 ath_assert_dump( A_INT32 argc, char* argv[])
{
	ATH_IOCTL_PARAM_STRUCT ath_param;
	A_INT32 error;

	ath_param.cmd_id = ATH_ASSERT_DUMP;
        ath_param.data = NULL;
        ath_param.length = 0;

	error = HANDLE_IOCTL ( &ath_param);
	return error;
}

/*FUNCTION*--------------------------------------------------------------------
*
* Function Name   : scan_control()
* Returned Value  : A_OK - on successful completion
*					A_ERROR - on any failure.
* Comments        : Disables/Enables foreground and background scan operations
*					in the Atheros device.  Both params must be provided where
*					foreground param is first followed by background param. a
*					'0' param disables the scan type while a '1' enables the
*					scan type.  Background scan -- firmware occasionally scans
*					while connected to a network. Foreground scan -- firmware
*					occasionally scans while disconnected to a network.
*
*END*------------------------------------------------------------------------*/
static A_INT32 scan_control( A_INT32 argc, char* argv[])
{
	A_INT32 error;
	A_INT32 fg, bg;
	qcom_scan_params_t scanParam;
        A_UINT32 enet_device;

	do{
		if(argc != 2) break;
		fg = atoi(argv[0]);
		if(fg != 0 && fg != 1) break;
		bg = atoi(argv[1]);
		if(bg != 0 && bg != 1) break;
                
                if( fg == 1 )
                {
                    scanParam.fgStartPeriod = 0;
                    scanParam.fgEndPeriod = 0;
                }
                else 
                {
                    scanParam.fgStartPeriod = 0xffff;
                    scanParam.fgEndPeriod = 0xffff;
                }
                
                if( bg == 1 )
                {
                    scanParam.bgPeriod = 0;
                }
                else
                {
                    scanParam.bgPeriod = 0xffff;
                }

                scanParam.maxActChDwellTimeInMs = 0;
                scanParam.pasChDwellTimeInMs = 0;
                scanParam.shortScanRatio = WMI_SHORTSCANRATIO_DEFAULT;
                scanParam.scanCtrlFlags = DEFAULT_SCAN_CTRL_FLAGS;
                scanParam.minActChDwellTimeInMs = 0;
                scanParam.maxActScanPerSsid = 0;
                scanParam.maxDfsChActTimeInMs = 0;
                
                enet_device = get_active_device();
                error = qcom_scan_params_set(enet_device, &scanParam);
		if(error == A_ERROR){
			printf("driver ioctl error\n");
			return A_ERROR;
		}
		return A_OK;
	}while(0);

	printf("param error: scan control requires 2 inputs [0|1] [0|1]\n");
	return A_ERROR;
}

/*FUNCTION*--------------------------------------------------------------------
*
* Function Name   : set_scan_para()
* Returned Value  : A_OK - on successful completion
*					A_ERROR - on any failure.
* Comments        : set scan parameters
*
*END*------------------------------------------------------------------------*/
static A_INT32 set_scan_para( A_INT32 argc, char* argv[])
{
	A_INT32 error;
	A_INT32 data;
	qcom_scan_params_t scan_params;
        A_UINT32 enet_device;

	do{
		if((argc != 2) && (argc != 10) ) break;
                data = atoi(argv[0]);
                scan_params.maxActChDwellTimeInMs = (A_UINT16) data;
                data = atoi(argv[1]);
                scan_params.pasChDwellTimeInMs    = (A_UINT16) data;
                
                if( argc == 2 ) 
                {
                    scan_params.fgStartPeriod = 0;
                    scan_params.fgEndPeriod   = 0;
                    scan_params.bgPeriod      = 0;
                    scan_params.shortScanRatio = WMI_SHORTSCANRATIO_DEFAULT;
                    scan_params.scanCtrlFlags = DEFAULT_SCAN_CTRL_FLAGS;
                    scan_params.minActChDwellTimeInMs = 0;
                    scan_params.maxActScanPerSsid = 0;
                    scan_params.maxDfsChActTimeInMs = 0;
                } 
                else
                {
                    data = atoi(argv[2]);
                    scan_params.fgStartPeriod = (A_UINT16) data;
                    data = atoi(argv[3]);
                    scan_params.fgEndPeriod = (A_UINT16) data;
                    data = atoi(argv[4]);
                    scan_params.bgPeriod = (A_UINT16) data;
                    data = atoi(argv[5]);
                    scan_params.shortScanRatio = (A_UINT8) data;
                    data = atoi(argv[6]);
                    scan_params.scanCtrlFlags = (A_UINT8) data;
                    data = atoi(argv[7]);
                    scan_params.minActChDwellTimeInMs = (A_UINT16) data; 
                    data = atoi(argv[8]);
                    scan_params.maxActScanPerSsid = (A_UINT16) data;
                    data = atoi(argv[9]);
                    scan_params.maxDfsChActTimeInMs = (A_UINT32) data;
                }  
                
                enet_device = get_active_device();
                error = qcom_scan_params_set(enet_device, &scan_params);
		if(error == A_ERROR){
			printf("driver ioctl error\n");
			return A_ERROR;
		}
		return A_OK;
	}while(0);

	printf("wmiconfig --setscanpara <max_act_ch_dwell_time_ms> <pas_act_chan_dwell_time_ms> [<fg_start_period(in secs)> <fg_end_period (in secs)> <bg_period (in secs)> <short_scan_ratio> <scan_ctrl_flags>  <min_active_chan_dwell_time_ms> <max_act_scan_per_ssid> <max_dfs_ch_act_time_in_ms>] \n");
	return A_ERROR;
}

/*FUNCTION*--------------------------------------------------------------------
*
* Function Name   : get_rate()
* Returned Value  : A_OK - on successful completion
*					A_ERROR - on any failure.
* Comments        : gets TX rate from chip
*
*END*------------------------------------------------------------------------*/
A_INT32 get_rate()
{
    ATH_IOCTL_PARAM_STRUCT ath_param;
    A_INT32 error = A_ERROR;


    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    ath_param.cmd_id = ATH_GET_RATE;
    ath_param.data   = NULL;
    ath_param.length = 0;
    error = HANDLE_IOCTL (&ath_param);

    if(error != A_OK){
    	printf("Command Failed\n");
    }
    return error;
}

/*FUNCTION*--------------------------------------------------------------------
*
* Function Name   : set_tx_power_scale()
* Returned Value  : A_OK - on successful completion
*					A_ERROR - on any failure.
* Comments        : sets TX power scale 
*
*END*------------------------------------------------------------------------*/
static A_INT32 set_tx_power_scale(A_INT32 index, A_INT32 argc,char* argv[])
{
    ATH_IOCTL_PARAM_STRUCT ath_param;
    A_INT32 error = A_ERROR; 
    WMI_SET_TX_POWER_SCALE_CMD pwr_scale;
    
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if((argc > 3) || (argc < 2))
    {
      printf("incorrect arguments refer help \n");
      return error;
    }    
    
    if((atoi(argv[index]) < -15) || (atoi(argv[index]) > 15))
    {
      printf("power value to be from -15dbm to 15dbm \n");
      return error;
    }
    
    pwr_scale.param_val = atoi(argv[index]);
    
    ath_param.cmd_id = ATH_SET_TX_PWR_SCALE;
    ath_param.data = &pwr_scale;
    ath_param.length = sizeof(pwr_scale);
    error = HANDLE_IOCTL (&ath_param);
    
    if(error != A_OK){
    	printf("Command Failed\n");
    }
    return error;      
}

/*FUNCTION*--------------------------------------------------------------------
*
* Function Name   : set_rate()
* Returned Value  : A_OK - on successful completion
*					A_ERROR - on any failure.
* Comments        : sets TX data rate 
*
*END*------------------------------------------------------------------------*/
static A_INT32 set_rate(A_INT32 index, A_INT32 argc, char* argv[])
{
    ATH_IOCTL_PARAM_STRUCT ath_param;
    A_INT32 error = A_ERROR;
    WMI_BIT_RATE_CMD bitrate; 

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc > 4)
    {
      printf("too many arguments refer help \n");
      return error;
    }
    
    if((argc == 3) && (atoi(argv[index]) > 0) && (atoi(argv[index]) <= 54))
    {
      switch(atoi(argv[index])) {
        case 1: 
        bitrate.rateIndex = RATE_1Mb;
        break;
        case 2: 
        bitrate.rateIndex = RATE_2Mb;
        break;
        case 5: 
        bitrate.rateIndex = RATE_5_5Mb;
        break;
        case 11: 
        bitrate.rateIndex = RATE_11Mb;
        break;       
        case 6: 
        bitrate.rateIndex = RATE_6Mb;
        break;
        case 9: 
        bitrate.rateIndex = RATE_9Mb;
        break;
        case 12: 
        bitrate.rateIndex = RATE_12Mb;
        break;
        case 18: 
        bitrate.rateIndex = RATE_18Mb;
        break; 
        case 24: 
        bitrate.rateIndex = RATE_24Mb;
        break;
        case 36: 
        bitrate.rateIndex = RATE_36Mb;
        break;
        case 48: 
        bitrate.rateIndex = RATE_48Mb;
        break;
        case 54: 
        bitrate.rateIndex = RATE_54Mb;
        break;
        default:
          printf("undefined rate \n");
          return error;
          break;
      }
      
    }
    else
    {
      if((argc == 3))
      {  
        printf("invalid command see help\n");
        return error;
      }
    }
   
    
    if((argc == 4) && (strcmp(argv[index], "mcs") == 0))
    {
       switch(atoi(argv[index+1])) {
        case 0: 
        bitrate.rateIndex = RATE_MCS_0_20;
        break;
        case 1: 
        bitrate.rateIndex = RATE_MCS_1_20;
        break;
        case 2: 
        bitrate.rateIndex = RATE_MCS_2_20;
        break;
        case 3: 
        bitrate.rateIndex = RATE_MCS_3_20;
        break;       
        case 4: 
        bitrate.rateIndex = RATE_MCS_4_20;
        break;  
        case 5: 
        bitrate.rateIndex = RATE_MCS_5_20;
        break;
        case 6: 
        bitrate.rateIndex = RATE_MCS_6_20;
        break;       
        case 7: 
        bitrate.rateIndex = RATE_MCS_7_20;
        break;  
        default:
          printf("undefined mcs rate \n");
          return error;
          break;        
       }
       
    }
    else
    {
       if((argc == 4))
      {  
        printf("invalid command see help\n");
        return error;
      }     
    }
    
    ath_param.cmd_id = ATH_SET_RATE;
    ath_param.data = &bitrate;
    ath_param.length = sizeof(bitrate);
    error = HANDLE_IOCTL (&ath_param);

    if(error != A_OK){

    	printf("Command Failed\n");

    }
    return error;
}

/*FUNCTION*--------------------------------------------------------------------
*
* Function Name   : get_tx_status()
* Returned Value  : A_OK - on successful completion
*					A_ERROR - on any failure.
* Comments        : gets TX status from driver
*
*END*------------------------------------------------------------------------*/
A_INT32 get_tx_status( )
{
    ATH_IOCTL_PARAM_STRUCT ath_param;
    A_INT32 error = A_ERROR;
    ATH_TX_STATUS result;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    ath_param.cmd_id = ATH_GET_TX_STATUS;
    ath_param.data = &result;
    ath_param.length = 4;


    error = HANDLE_IOCTL (&ath_param);

    if(error == A_OK){

      switch (result.status)
      {
        case ATH_TX_STATUS_IDLE:
          printf("TX Idle\n");
          break;
        case ATH_TX_STATUS_WIFI_PENDING:
          printf("TX status WIFI Pending \n");
          break;
        case ATH_TX_STATUS_HOST_PENDING:
          printf("TX status Host Pending\n");
          break;
        default:
          printf("Invalid result\n");
          break;
      }
      return result.status;
    }
    else
    {
    	printf("Command Failed\n");
    	return A_ERROR;
    }
}

#if ENABLE_P2P_MODE
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : p2p_handler()
* Returned Value  : 1 - successful completion or
*                   0 - failed.
* Comments        : Handles P2P related functionality
*
*END*-----------------------------------------------------------------*/
static int_32 p2p_handler( int_32 index, int_32 argc, char_ptr argv[])
{
#define P2P_STANDARD_TIMEOUT (300)
  union{
    WMI_P2P_SET_CMD                 p2p_set_params;
    WMI_P2P_FIND_CMD                find_params;
    WMI_P2P_FW_CONNECT_CMD_STRUCT   p2p_connect;
    WMI_P2P_FW_PROV_DISC_REQ_CMD    p2p_prov_disc;
    WMI_P2P_PROV_INFO               p2p_info;
    WMI_SET_PASSPHRASE_CMD          setPassPhrase;
    WMI_P2P_GRP_INIT_CMD            grpInit;
	WMI_P2P_SET_PROFILE_CMD 		p2p;
	WMI_P2P_FW_SET_CONFIG_CMD 	    p2p_set_config;
    WMI_P2P_INVITE_CMD              p2pInvite;
    WMI_P2P_FW_INVITE_REQ_RSP_CMD   invite_req_rsp;
    P2P_NOA_DESCRIPTOR              p2p_desc;
    WMI_OPPPS_INFO_STRUCT           p2p_oppps;
  }stack_p2p;
    A_UINT32 enet_device, wifimode;
    
    enet_device = get_active_device();  
    A_UINT32 timeout_val = 0;
    A_UINT8  p2p_invite_role;
    uint_32 error= SHELL_EXIT_SUCCESS,i,k, len;//, val;
    i = index+1;
	set_callback();
        
    if(enet_device != 0)
    {
	   printf(" dev : %d for p2p mode switch to dev 0 \n", enet_device);
       return SHELL_EXIT_ERROR;
    }  
    if (index <= argc)
    {

        if(ATH_STRCMP(argv[index],"join") == 0)
        {
            if(argc < 5)
            {
                printf("\n Incomplete parameter list for p2p join \n");
                printf("\n Usage : wmiconfig --p2p join <intf_mac_addr> \n");
                printf("                                   <push | keypad | display> <* wpspin> \n");
                printf("           * Only for PIN Mode \n");
                return SHELL_EXIT_ERROR;
            }
            else
            {
#if 0             
                /* Set P2P Mode to P2P Client */
                stack_p2p.p2p_set_params.val.mode.p2pmode = P2P_CLIENT;
                if( qcom_p2p_set(enet_device, P2P_CONFIG_P2P_OPMODE, &stack_p2p.p2p_set_params.val, sizof(stack_p2p.p2p_set_params.val.mode)) != A_OK )                
                {
                    printf("\nStartP2P command did not execute properly\n");
                    return SHELL_EXIT_ERROR;
                }
#endif                
                /* Send the join profile to fw */
                if ((wmic_ether_aton(argv[i],p2p_join_mac_addr)) != A_OK)
                {
                printf("\nInvalid PEER MAC Address\n");
                return SHELL_EXIT_ERROR;
                }
                if( qcom_p2p_func_join_profile(enet_device, p2p_join_mac_addr) != A_OK ) 
                {
                    printf("\nStartP2P command did not execute properly\n");
                    return SHELL_EXIT_ERROR;
                }
                
                printf("\n Interface MAC addr : %x:%x:%x:%x:%x:%x \n",p2p_join_mac_addr[0],
                    p2p_join_mac_addr[1],
                    p2p_join_mac_addr[2],
                    p2p_join_mac_addr[3],
                    p2p_join_mac_addr[4],
                    p2p_join_mac_addr[5]);    
                
                /* Send Join request */
                A_MEMZERO(&p2p_join_profile,sizeof( WMI_P2P_FW_CONNECT_CMD_STRUCT));
                if(qcom_get_bssid(enet_device, (A_UINT8 *)p2p_join_profile.own_interface_addr) != SHELL_EXIT_SUCCESS)
                {
                    printf("\n unable to obtain device mac address\n");
                    return SHELL_EXIT_ERROR;
                }

            printf("\n own MAC addr : %x:%x:%x:%x:%x:%x \n",p2p_join_profile.own_interface_addr[0],
                    p2p_join_profile.own_interface_addr[1],
                    p2p_join_profile.own_interface_addr[2],
                    p2p_join_profile.own_interface_addr[3],
                    p2p_join_profile.own_interface_addr[4],
                    p2p_join_profile.own_interface_addr[5]);
                
            if(ATH_STRCMP(argv[i + 1],"push") == 0)
            {
                p2p_join_profile.wps_method = WPS_PBC;
            }
            else if(ATH_STRCMP(argv[i + 1],"display") == 0)
            {
                p2p_join_profile.wps_method = WPS_PIN_DISPLAY;
            }
            else if(ATH_STRCMP(argv[i + 1],"keypad") == 0)
            {
                p2p_join_profile.wps_method = WPS_PIN_KEYPAD;
            }
#if 1           
            if(p2p_join_profile.wps_method == WPS_PIN_DISPLAY ||
               p2p_join_profile.wps_method == WPS_PIN_KEYPAD)
            {
                strcpy(stack_p2p.p2p_info.wps_pin,argv[i + 2]);
                strcpy(p2p_wps_pin,argv[i + 2]);
                wmi_save_key_info(&stack_p2p.p2p_info);
            }
#endif
            p2p_join_session_active = 1;
            if(qcom_p2p_func_get_node_list(enet_device, NULL, 0) != A_OK)
            {
                printf("\nStartP2P command did not execute properly\n");
                return SHELL_EXIT_ERROR;
            }
            app_time_delay(100);
            p2p_callback(0);            

         }
        }
        else if(ATH_STRCMP(argv[index],"invite") == 0)
        {
            if(argc < 6)
            {
                printf("Incomplete parameter list \n");
                printf("Usage: wmiconfig --p2p invite <SSID> <MACADDR> <WPS_METHOD> <persistent>\n");                
                return SHELL_EXIT_ERROR;
            }
            
            A_MEMZERO(&stack_p2p.p2pInvite,sizeof(WMI_P2P_INVITE_CMD));
            
            for(k = 0; k < MAX_LIST_COUNT; k++)
            {
               if(strncmp((char *)p2p_pers_data[k].ssid, argv[i], strlen(argv[i])) == 0)
               {
                 break;
               }
 
            }
            if(k == MAX_LIST_COUNT)
            {
               // printf("the SSID and MAC is not a persistent one \n");
               qcom_op_get_mode(enet_device, &wifimode);
               if( (wifimode == QCOM_WLAN_DEV_MODE_STATION) && (wifi_connected_flag==1) )
                {
                    p2p_invite_role=WMI_P2P_INVITE_ROLE_CLIENT;
                    stack_p2p.p2pInvite.is_persistent=0;
                }
               k = 0;
            }
            else
            {
                invitation_index = k;
                p2p_invite_role = p2p_pers_data[k].role;
                stack_p2p.p2pInvite.is_persistent=1;
            }
           
            if ((wmic_ether_aton(argv[i+1],stack_p2p.p2pInvite.peer_addr)) != A_OK)
            {
              printf("\nInvalid Invitation MAC Address\n");
              return SHELL_EXIT_ERROR;
            }
            
            if(ATH_STRCMP(argv[i+2], "push") == 0)
            {
              stack_p2p.p2pInvite.wps_method = WPS_PBC;
            }
            else if(ATH_STRCMP(argv[i+2], "display") == 0)
            {
              stack_p2p.p2pInvite.wps_method = WPS_PIN_KEYPAD;
            }
            else if(ATH_STRCMP(argv[i+2],"keypad") == 0)
            {
              stack_p2p.p2pInvite.wps_method = WPS_PIN_DISPLAY;
            }
            else
            {
              printf("wps method not proper P2P Invite \n");
              return SHELL_EXIT_ERROR;
            }
 
            if( qcom_p2p_func_invite(enet_device, argv[i], stack_p2p.p2pInvite.wps_method, stack_p2p.p2pInvite.peer_addr, stack_p2p.p2pInvite.is_persistent, p2p_invite_role) != A_OK )            
            {
                printf("\nStartP2P command did not execute properly\n");
                return SHELL_EXIT_ERROR;
            }
#if 1      
            if(p2p_invite_role == WMI_P2P_INVITE_ROLE_ACTIVE_GO) {
              
            error = qcom_sec_set_encrypt_mode(enet_device,WLAN_CRYPT_AES_CRYPT);    

            if(error != A_OK)
            {
                return A_ERROR;
            }

            error = qcom_sec_set_auth_mode(enet_device,WLAN_AUTH_WPA2_PSK);
            
            if(error != A_OK)
            {
                return A_ERROR;
            }


            if( qcom_p2p_func_set_pass_ssid(enet_device, (char *)p2p_pers_data[k].passphrase, (char *)p2p_pers_data[k].ssid ) != A_OK)
            {
                printf("\nStartP2P command did not execute properly\n");
                return SHELL_EXIT_ERROR;
            }
            }
#endif            
        }
        else if(ATH_STRCMP(argv[index],"find") == 0)
        {
            if(ATH_STRCMP(argv[i],"help") == 0){
                printf("\n Usage : wmiconfig --p2p find <* type> <* timeout> \n");
                printf("\n * - optional \n");
                return SHELL_EXIT_ERROR;
            }
            A_MEMZERO(&stack_p2p.find_params, sizeof(WMI_P2P_FIND_CMD));
            if(argc == 4)
            {
                if(ATH_STRCMP(argv[i],"1") == 0)
                {
                    stack_p2p.find_params.type = (WMI_P2P_FIND_START_WITH_FULL);
                    stack_p2p.find_params.timeout = (P2P_STANDARD_TIMEOUT);
                }
                else if(ATH_STRCMP(argv[i],"2") == 0)
                {
                    stack_p2p.find_params.type = (WMI_P2P_FIND_ONLY_SOCIAL);
                    stack_p2p.find_params.timeout = (P2P_STANDARD_TIMEOUT);
                }
                else if(ATH_STRCMP(argv[i],"3") == 0)
                {
                    stack_p2p.find_params.type = (WMI_P2P_FIND_PROGRESSIVE);
                    stack_p2p.find_params.timeout = (P2P_STANDARD_TIMEOUT);
                }
                else
                {
                    printf("\n wrong option enter option 1,2 or 3\n");
                    return SHELL_EXIT_ERROR;
                }

            }
            else if(argc == 5)
            {
                if(ATH_STRCMP(argv[i],"1") == 0)
                {
                    stack_p2p.find_params.type = (WMI_P2P_FIND_START_WITH_FULL);
                    stack_p2p.find_params.timeout = (atoi(argv[i+1]));
                }
                else if(ATH_STRCMP(argv[i],"2") == 0)
                {
                    stack_p2p.find_params.type = (WMI_P2P_FIND_ONLY_SOCIAL);
                    stack_p2p.find_params.timeout = (atoi(argv[i+1]));
                }
                else if(ATH_STRCMP(argv[i],"3") == 0)
                {
                    stack_p2p.find_params.type = (WMI_P2P_FIND_PROGRESSIVE);
                    stack_p2p.find_params.timeout = (atoi(argv[i+1]));
                }
                else
                {
                    printf("\n wrong option enter option 1,2 or 3\n");
                    return SHELL_EXIT_ERROR;
                }
            }
            else
            {
                stack_p2p.find_params.type = (WMI_P2P_FIND_ONLY_SOCIAL);
                stack_p2p.find_params.timeout = (P2P_STANDARD_TIMEOUT);
            }

            if(qcom_p2p_func_find(enet_device, NULL, stack_p2p.find_params.type, stack_p2p.find_params.timeout) != A_OK)
            {
                printf("\nStartP2P command did not execute properly\n");
                return SHELL_EXIT_ERROR;
            }
        }
        else if(ATH_STRCMP(argv[index],"list_network") == 0)
        {
 
            if(qcom_p2p_func_get_network_list(enet_device, NULL, sizeof(WPS_CREDENTIAL)) != A_OK)
            {
                printf("\nStartP2P command did not execute properly\n");
                return SHELL_EXIT_ERROR;
            }

         p2p_callback(0);         
        }
        else if(ATH_STRCMP(argv[index],"serv_disc") == 0)
        {
           p2p_callback(1);
        }        
        else if(ATH_STRCMP(argv[index],"set") == 0)
        {
            A_MEMZERO(&stack_p2p.p2p_set_params,sizeof(WMI_P2P_SET_CMD));
            if(ATH_STRCMP(argv[i],"p2pmode") == 0)
            {
                stack_p2p.p2p_set_params.config_id = P2P_CONFIG_P2P_OPMODE;
                if(ATH_STRCMP(argv[i +1],"p2pdev") == 0)
                {
                    stack_p2p.p2p_set_params.val.mode.p2pmode = P2P_DEV;
                }
                else if(ATH_STRCMP(argv[i +1],"p2pclient") == 0)
                {
                    stack_p2p.p2p_set_params.val.mode.p2pmode = P2P_CLIENT;
                }
                else if(ATH_STRCMP(argv[i +1],"p2pgo") == 0)
                {
                    stack_p2p.p2p_set_params.val.mode.p2pmode = P2P_GO;
                }
                else
                {
                    printf("Input can be \"p2pdev/p2pclient/p2pgo\"");
                }
                len = sizeof(stack_p2p.p2p_set_params.val.mode);
                printf("p2p mode :%x, Config Id\n",stack_p2p.p2p_set_params.val.mode.p2pmode,stack_p2p.p2p_set_params.config_id);
            }else if(ATH_STRCMP(argv[i],"postfix") == 0){

                stack_p2p.p2p_set_params.config_id = P2P_CONFIG_SSID_POSTFIX;
                if(strlen(argv[i+1])) {
                    A_MEMCPY(stack_p2p.p2p_set_params.val.ssid_postfix.ssid_postfix,argv[i+1],strlen(argv[i+1]));
                    stack_p2p.p2p_set_params.val.ssid_postfix.ssid_postfix_len = strlen(argv[i+1]);
                    len = sizeof(stack_p2p.p2p_set_params.val.ssid_postfix);
                    printf("PostFix string %s, Len %d\n",stack_p2p.p2p_set_params.val.ssid_postfix.ssid_postfix, stack_p2p.p2p_set_params.val.ssid_postfix.ssid_postfix_len);
                }
            }
            else if(ATH_STRCMP(argv[i], "intrabss") == 0)
            {
				stack_p2p.p2p_set_params.config_id = P2P_CONFIG_INTRA_BSS;
				stack_p2p.p2p_set_params.val.intra_bss.flag = atoi(argv[i+1]);
                                len = sizeof(stack_p2p.p2p_set_params.val.intra_bss);
			}
            else if(ATH_STRCMP(argv[i], "gointent") == 0)
	        {
		        stack_p2p.p2p_set_params.config_id = P2P_CONFIG_GO_INTENT;
		        stack_p2p.p2p_set_params.val.go_intent.value = atoi(argv[i+1]);
                        len = sizeof(stack_p2p.p2p_set_params.val.go_intent);
	        }else if (ATH_STRCMP(argv[i], "cckrates") == 0){
	        	stack_p2p.p2p_set_params.config_id = P2P_CONFIG_CCK_RATES;
	        	stack_p2p.p2p_set_params.val.cck_rates.enable = atoi(argv[i+1]);
                        len = sizeof(stack_p2p.p2p_set_params.val.cck_rates);
	        }
            if( qcom_p2p_set(enet_device, stack_p2p.p2p_set_params.config_id, &stack_p2p.p2p_set_params.val, len) != A_OK )                
            {
                printf("\nStartP2P command did not execute properly\n");
                return SHELL_EXIT_ERROR;
            }

        }
        else if(ATH_STRCMP(argv[index],"cancel") == 0)
        {
            if(argc > 3)
            {
                printf("\n Usage : wmiconfig --p2p cancel \n");
                return SHELL_EXIT_ERROR;
            } else { 
                if(qcom_p2p_func_cancel(enet_device) != A_OK)
                {
                    printf("\nStartP2P command did not execute properly\n");
                    return SHELL_EXIT_ERROR;
                }
                autogo_newpp = A_FALSE;
                
                p2p_cancel_enable = 1;
                
                if(qcom_p2p_func_get_network_list(enet_device, NULL, sizeof(WPS_CREDENTIAL)) != A_OK)
                {
                  printf("\nStartP2P command did not execute properly\n");
                  return SHELL_EXIT_ERROR;
                }
                p2p_callback(0);                 
            }
            /* Reset to mode station */
            qcom_op_set_mode (enet_device,QCOM_WLAN_DEV_MODE_STATION);
            p2p_persistent_done = 0;
            set_power_mode( REC_POWER,P2P);
        }
        else if(ATH_STRCMP(argv[index],"prov") == 0)
        {
            if(argc < 4 || (ATH_STRCMP(argv[i], "help") == 0))
            {
                printf("\n Incomplete parameter list for p2p prov discovery \n");
                printf("\n Usage : wmiconfig --p2p prov <peer_mac_addr> <wps_method> \n");
                return SHELL_EXIT_ERROR;
            }
            else //if((argc >= 5) && (argc < 7))
            {
                if(ATH_STRCMP(argv[i+1], "push") == 0)
                {
                    rand();
                    stack_p2p.p2p_prov_disc.wps_method = A_CPU2LE16((A_UINT16)WPS_CONFIG_PUSHBUTTON);
                }
                else if(ATH_STRCMP(argv[i+1], "display") == 0)
                {
                    stack_p2p.p2p_prov_disc.wps_method = A_CPU2LE16((A_UINT16)WPS_CONFIG_DISPLAY);
                }
                else if(ATH_STRCMP(argv[i+1],"keypad") == 0)
                {
                    stack_p2p.p2p_prov_disc.wps_method = A_CPU2LE16((A_UINT16)WPS_CONFIG_KEYPAD);
                }
                stack_p2p.p2p_prov_disc.dialog_token = 1;

                if ((wmic_ether_aton(argv[i],stack_p2p.p2p_prov_disc.peer)) != A_OK)
                {
                    printf("\nInvalid PEER MAC Address\n");
                    return SHELL_EXIT_ERROR;
                }
		
                if( qcom_p2p_func_prov(enet_device, stack_p2p.p2p_prov_disc.wps_method, stack_p2p.p2p_prov_disc.peer) != A_OK )		
                {
                    printf("\nStartP2P command did not execute properly\n");
                    return SHELL_EXIT_ERROR;
                }
                p2p_callback(0);
                
                /* Authorize P2P Device */
                A_MEMZERO(&stack_p2p.p2p_connect, sizeof( WMI_P2P_FW_CONNECT_CMD_STRUCT));
                if(ATH_STRCMP(argv[i+1], "push") == 0)
                {
                    stack_p2p.p2p_connect.wps_method = P2P_WPS_PBC;
                }
                else if(ATH_STRCMP(argv[i+1], "display") == 0)
                {
                    stack_p2p.p2p_connect.wps_method = P2P_WPS_NOT_READY;
                }
                else if(ATH_STRCMP(argv[i+1],"keypad") == 0)
                {
                    stack_p2p.p2p_connect.wps_method = P2P_WPS_PIN_DISPLAY;
                }

                if ((wmic_ether_aton(argv[i],stack_p2p.p2p_connect.peer_addr)) != A_OK)
                {
                    printf("\nInvalid PEER MAC Address\n");
                    return SHELL_EXIT_ERROR;
                }
                
                stack_p2p.p2p_connect.go_intent = p2p_intent;
                
                if(stack_p2p.p2p_connect.wps_method != P2P_WPS_NOT_READY) {
                   if(qcom_p2p_func_auth(enet_device, stack_p2p.p2p_connect.dev_auth, stack_p2p.p2p_connect.wps_method, stack_p2p.p2p_connect.peer_addr, stack_p2p.p2p_connect.dev_capab & P2P_PERSISTENT_FLAG?1:0)!=A_OK)
                   {
                        printf("\nStartP2P command did not execute properly\n");
                        return SHELL_EXIT_ERROR;
                    }
                }
            }

        }
        else if(ATH_STRCMP(argv[index],"auth") == 0)
        {
            A_MEMZERO(&stack_p2p.p2p_connect, sizeof( WMI_P2P_FW_CONNECT_CMD_STRUCT));
            if(argc < 5)
            {
                printf("\n Incomplete parameter list for p2p auth \n");
                printf("\n Usage : wmiconfig --p2p auth <peer_mac_addr> <wps_method> <* wps_pin> [persistent]\n");
                return SHELL_EXIT_ERROR;
            }
            else //if((argc >= 5) && (argc < 7))
            {
                if(strlen(argv[i+2]) == 8){
                    strcpy(stack_p2p.p2p_info.wps_pin,argv[i + 2]);
                    strcpy(p2p_wps_pin,argv[i + 2]);
                    wmi_save_key_info(&stack_p2p.p2p_info);
                    printf("WPS Pin %s\n",p2p_wps_pin);
                    if(ATH_STRCMP(argv[i + 3],"persistent") == 0)
                    {
                      stack_p2p.p2p_connect.dev_capab |= P2P_PERSISTENT_FLAG;
                    }                          
                }
                if(ATH_STRCMP(argv[i+1], "deauth") == 0)
                {
                    stack_p2p.p2p_connect.dev_auth = 1;
                }
                if(ATH_STRCMP(argv[i+1], "push") == 0)
                {
                    stack_p2p.p2p_connect.wps_method = WPS_PBC;
                    if(ATH_STRCMP(argv[i + 2],"persistent") == 0)
                    {
                      printf("persistent \n");
                      stack_p2p.p2p_connect.dev_capab |= P2P_PERSISTENT_FLAG;
                    }                    
                }
                else if(ATH_STRCMP(argv[i+1], "display") == 0)
                {
                    stack_p2p.p2p_connect.wps_method = WPS_PIN_DISPLAY;
                    printf("WPS PIN %s \n",p2p_wps_pin);
                }
                else if(ATH_STRCMP(argv[i+1],"keypad") == 0)
                {
                    stack_p2p.p2p_connect.wps_method = WPS_PIN_KEYPAD;
                }

                if ((wmic_ether_aton(argv[i],stack_p2p.p2p_connect.peer_addr)) != A_OK)
                {
                    printf("\nInvalid PEER MAC Address\n");
                    return SHELL_EXIT_ERROR;
                }

                /* Save P2P persistent flag as it will be needed while starting 
               the group capability in beacons if role is P2P GO  */
                if (stack_p2p.p2p_connect.dev_capab & P2P_PERSISTENT_FLAG)
                {
                    p2p_persistent_go = 1;
                }
                
                if(qcom_p2p_func_auth(enet_device, stack_p2p.p2p_connect.dev_auth, stack_p2p.p2p_connect.wps_method, stack_p2p.p2p_connect.peer_addr, stack_p2p.p2p_connect.dev_capab & P2P_PERSISTENT_FLAG?1:0) != A_OK)
                {
                    printf("\nStartP2P command did not execute properly\n");
                    p2p_persistent_go = 0;
                    return SHELL_EXIT_ERROR;
                }
            }

            task2_msec = 50;
        }
        else if(ATH_STRCMP(argv[index],"stop") == 0)
        {
            if(qcom_p2p_func_stop_find(enet_device) != A_OK)
            {
                printf("\nStartP2P command did not execute properly\n");
                return SHELL_EXIT_ERROR;
            }
        }
        else if(ATH_STRCMP(argv[index],"nodelist") == 0)
        {
            if(qcom_p2p_func_get_node_list(enet_device, NULL, 0) != A_OK)
            {
                printf("\nStartP2P command did not execute properly\n");
                return SHELL_EXIT_ERROR;
            }
            app_time_delay(100);
            p2p_callback(0);
        }
        else if(ATH_STRCMP(argv[index],"listen") == 0)
        {
            if (argc == 4)
            {
                timeout_val = atoi(argv[i]);
            }
            else
            {
                timeout_val = P2P_STANDARD_TIMEOUT;
            }
            if(qcom_p2p_func_listen(enet_device,timeout_val) != A_OK)
            {
                printf("\nStartP2P command did not execute properly\n");
                return SHELL_EXIT_ERROR;
            }
        }
        else if(ATH_STRCMP(argv[index], "connect") == 0)
        {
            if(argc < 5)
            {
                printf("\n Incomplete parameter list for p2p connect \n");
                printf("\n Usage : wmiconfig --p2p connect <peer_mac_addr> \n");
                printf("                                   <wps_method> [persistent]\n");
                return SHELL_EXIT_ERROR;
            }
            else //if ((argc >= 6) && (argc < 8))
            {
                A_MEMZERO(&stack_p2p.p2p_info,sizeof(WMI_P2P_PROV_INFO)); 
                A_MEMZERO(&stack_p2p.p2p_connect,sizeof( WMI_P2P_FW_CONNECT_CMD_STRUCT));
                
                if(strlen(argv[i+2]) == 8){
                    strcpy(stack_p2p.p2p_info.wps_pin,argv[i + 2]);
                    strcpy(p2p_wps_pin,argv[i + 2]);
                    wmi_save_key_info(&stack_p2p.p2p_info);
                    printf("WPS Pin %s\n",p2p_wps_pin);
                    
                    if(ATH_STRCMP(argv[i + 3],"persistent") == 0)
                    {
                      stack_p2p.p2p_connect.dev_capab |= P2P_PERSISTENT_FLAG;
                    }                          
                }
                
                if(qcom_get_bssid(enet_device,(A_UINT8 *)stack_p2p.p2p_connect.own_interface_addr) != A_OK)
                {
                    printf("\n unable to obtain device mac address\n");
                    return SHELL_EXIT_ERROR;
                }
                printf("\n own MAC addr : %x:%x:%x:%x:%x:%x \n", stack_p2p.p2p_connect.own_interface_addr[0],
                        stack_p2p.p2p_connect.own_interface_addr[1],
                        stack_p2p.p2p_connect.own_interface_addr[2],
                        stack_p2p.p2p_connect.own_interface_addr[3],
                        stack_p2p.p2p_connect.own_interface_addr[4],
                        stack_p2p.p2p_connect.own_interface_addr[5]);

                if ((wmic_ether_aton(argv[i],stack_p2p.p2p_connect.peer_addr)) != A_OK)
                {
                    printf("\nInvalid PEER MAC Address\n");
                    return SHELL_EXIT_ERROR;
                }
                printf("\n Peer MAC addr : %x:%x:%x:%x:%x:%x \n",stack_p2p.p2p_connect.peer_addr[0],
                        stack_p2p.p2p_connect.peer_addr[1],
                        stack_p2p.p2p_connect.peer_addr[2],
                        stack_p2p.p2p_connect.peer_addr[3],
                        stack_p2p.p2p_connect.peer_addr[4],
                        stack_p2p.p2p_connect.peer_addr[5]);
                if(ATH_STRCMP(argv[i + 1],"push") == 0)
                {
                    stack_p2p.p2p_connect.wps_method = P2P_WPS_PBC;
                    if(ATH_STRCMP(argv[i + 2],"persistent") == 0)
                    {
                      printf("persistent \n");
                      stack_p2p.p2p_connect.dev_capab |= P2P_PERSISTENT_FLAG;
                    }                    
                }
                else if(ATH_STRCMP(argv[i + 1],"display") == 0)
                {
                    stack_p2p.p2p_connect.wps_method = P2P_WPS_PIN_DISPLAY;
                    printf("WPS PIN %s \n",p2p_wps_pin);
                }
                else if(ATH_STRCMP(argv[i + 1],"keypad") == 0)
                {
                    stack_p2p.p2p_connect.wps_method = P2P_WPS_PIN_KEYPAD;
                    //strcpy(p2p_wps_pin_val, argv[i+2]);
                }
#if 0
                printf("\n go_intent : %d \n", atoi(argv[i+2]));
#endif

                /* Save P2P persistent flag as it will be needed while starting 
               the group capability in beacons if role is P2P GO  */
                if (stack_p2p.p2p_connect.dev_capab & P2P_PERSISTENT_FLAG)
                {
                    p2p_persistent_go = 1;
                }

                if(qcom_p2p_func_connect(enet_device, stack_p2p.p2p_connect.wps_method, stack_p2p.p2p_connect.peer_addr,  stack_p2p.p2p_connect.dev_capab & P2P_PERSISTENT_FLAG?1:0) != A_OK)
                {
                    printf("\nStartP2P command did not execute properly\n");
                    p2p_persistent_go = 0;
                    return SHELL_EXIT_ERROR;
                }
                p2p_callback(0);
            }
        }
        else if(ATH_STRCMP(argv[index], "autogo") == 0)
        {
            if(ATH_STRCMP(argv[i],"help") == 0){
                printf("\n Usage : wmiconfig --p2p autogo [persistent]\n");
                return SHELL_EXIT_ERROR;
            }
            p2pMode = TRUE;
            wps_flag = 0x01;

            printf("Starting Autonomous GO \n");
            if(ATH_STRCMP(argv[i],"persistent") == 0)
            {
                stack_p2p.grpInit.persistent_group = 1;
            }
            else
            {
                stack_p2p.grpInit.persistent_group = 0;
            }
            
            if(A_FALSE == autogo_newpp) {
                if(qcom_p2p_func_start_go(enet_device, "DIRECT-iO", (A_UINT8 *)wpa_passphrase, P2P_DEFAULT_CHAN, stack_p2p.grpInit.persistent_group) != A_OK)
                {
                    printf("\nStartP2P command did not execute properly\n");
                    return SHELL_EXIT_ERROR;
                }
            } else {    
                if(qcom_p2p_func_start_go(enet_device, NULL, NULL, P2P_DEFAULT_CHAN, stack_p2p.grpInit.persistent_group) != A_OK)
                {
                    printf("\nStartP2P command did not execute properly\n");
                    return SHELL_EXIT_ERROR;
                }
            }
        }
    else if((ATH_STRCMP(argv[index], "on") == 0))
    {
        if(argc > 3)
        {
            printf("\n Usage : wmiconfig --p2p <on / off> \n");
            return SHELL_EXIT_ERROR;
        } else {
            if(qcom_p2p_func_init(enet_device,TRUE) != A_OK)
            {
                printf("\nStartP2P command did not execute properly\n");
                return SHELL_EXIT_ERROR;
            }
            
            p2p_intent = 0;
            A_MEMCPY(set_channel_p2p,"6" ,1);
            A_MEMZERO(&stack_p2p.p2p_info,sizeof(WMI_P2P_PROV_INFO));
            strcpy(stack_p2p.p2p_info.wps_pin,"12345670");
            strcpy(p2p_wps_pin,"12345670");
            wmi_save_key_info(&stack_p2p.p2p_info);
            
            /* Autonomous GO configurations */
             set_passphrase("1234567890");
             strcpy((char *)original_ssid,"DIRECT-iO");
                         
	     p2pMode = TRUE;
        }
    }
    else if((ATH_STRCMP(argv[index], "off") == 0))
    {
            if(argc > 3)
            {
                printf("\n Usage : wmiconfig --p2p <on / off> \n");
                return SHELL_EXIT_ERROR;
            } else {
                if(qcom_p2p_func_init(enet_device, FALSE) != A_OK)
                {
                    printf("\nStartP2P command did not execute properly\n");
                    return SHELL_EXIT_ERROR;
                }
            }
            qcom_op_set_mode (enet_device,QCOM_WLAN_DEV_MODE_STATION);
	    p2pMode = FALSE;
    }
    else if(ATH_STRCMP(argv[index], "setopclass") == 0)
    {
            if(argc < 6 )
            {
                printf("\n Usage : wmiconfig --p2p setopclass <go_intent> <oper_regclass> <oper_regchannel>\n");
                return SHELL_EXIT_ERROR;
            } else {
                p2p_intent = atoi(argv[i]);
                A_MEMCPY(set_channel_p2p, argv[i+2], sizeof(argv[i+2])); // for autogo
                if( qcom_p2p_func_set_config(enet_device,  p2p_intent, 6, atoi(argv[i+2]), 3000, 81, atoi(argv[i+1]), 4) != A_OK) 
                {
                    printf("\nStartP2P command did not execute properly\n");
                    return SHELL_EXIT_ERROR;
                }
            }
    }
    else if(ATH_STRCMP(argv[index], "setconfig") == 0)
    {
            if(argc < 8)
            {
                printf("\n Incomplete parameter list for p2p set config \n");
                printf("\n Usage : wmiconfig --p2p setconfig <go_intent> <listen_channel> \n");
                printf("                                           <oper_channel> <country> <node_age_timeout>\n");
                return SHELL_EXIT_ERROR;
            }
            else
            {
                printf("\n Note: Cannot set country code. Use board data file or tuneables instead \n");
                p2p_intent = atoi(argv[i]);
                A_MEMCPY(set_channel_p2p, argv[i+2], sizeof(argv[i+2])); // for autogo

                if( qcom_p2p_func_set_config(enet_device,  p2p_intent, atoi(argv[i+1]), atoi(argv[i+2]), atoi(argv[i+4]), 81, 81, 5) != A_OK) 
            	{
            	    printf("\nStartP2P command did not execute properly\n");
            	    return SHELL_EXIT_ERROR;
            	}
	    }
    }
    else if(ATH_STRCMP(argv[index], "passphrase") == 0)
    {
            if(argc < 5)
            {
                    printf("\n Incomplete parameter list for p2p set config \n");
                    printf("\n Usage : wmiconfig --p2p passphrase <passphrase> <SSID> \n");
                    return SHELL_EXIT_ERROR;
            }
            else {
                error = qcom_sec_set_encrypt_mode(enet_device,WLAN_CRYPT_AES_CRYPT);    
                if(error != A_OK)
                {
                    return A_ERROR;
                }

                error = qcom_sec_set_auth_mode(enet_device,WLAN_AUTH_WPA2_PSK);
                if(error != A_OK)
                {
                    return A_ERROR;
                }

                autogo_newpp = A_TRUE;
                if (qcom_p2p_func_set_pass_ssid(enet_device, argv[index+1],argv[index+2]) != A_OK)
                {
                    printf("\nStartP2P command did not execute properly\n");
                    return SHELL_EXIT_ERROR;
                }
            }
    }
    else if(ATH_STRCMP(argv[index], "setoppps") == 0)
    {
            if(argc < 5)
            {
                printf("\n Incomplete parameter list for p2p set oppps \n");
                printf("\n Usage : wmiconfig --p2p setoppps <enable> <ctwin> \n");
                return SHELL_EXIT_ERROR;
            }
            else
            {
                if( qcom_p2p_func_set_oppps(enet_device, atoi(argv[i]), atoi(argv[i + 1])) != A_OK ) 
                {
                    printf("\nStartP2P command did not execute properly\n");
                    return SHELL_EXIT_ERROR;
                }
            }
        }
        else if(ATH_STRCMP(argv[index], "setnoa") == 0)
        {
            if(argc < 4)
            {
                printf("\n Incomplete parameter list for p2p set noa \n");
                printf("\n Usage : wmiconfig --p2p setnoa  <count [1 - 255]> <start in ms> \n");
                printf("                                   <duration in ms> <interval in ms> \n");
                return SHELL_EXIT_ERROR;
            }
            else
            {
                  stack_p2p.p2p_desc.count_or_type    = atoi(argv[i++]);
                  stack_p2p.p2p_desc.start_or_offset  = A_CPU2LE32(atoi(argv[i++]) * 1000);
                  stack_p2p.p2p_desc.duration         = A_CPU2LE32(atoi(argv[i++]) * 1000);
                  stack_p2p.p2p_desc.interval         = A_CPU2LE32(atoi(argv[i++]) * 1000);              
                if( qcom_p2p_func_set_noa(enet_device, stack_p2p.p2p_desc.count_or_type, stack_p2p.p2p_desc.start_or_offset, stack_p2p.p2p_desc.duration, stack_p2p.p2p_desc.interval) != A_OK )
                {
                    printf("\nStartP2P command did not execute properly\n");
                    return SHELL_EXIT_ERROR;
                }
            }
        }
        else
        {
            printf("\n This option is currently not supported\n");
            return SHELL_EXIT_ERROR;
        }
    }
    else
    {
        printf("\n Incorrect Incomplete parameter list \n");
        return SHELL_EXIT_ERROR;
    }
    return SHELL_EXIT_SUCCESS;
}
#endif

/*FUNCTION*--------------------------------------------------------------------
*
* Function Name   : get_reg_domain()
* Returned Value  : A_OK - on successful completion
*		    A_ERROR - on any failure.
* Comments        : gets Regulatory domain from driver
*
*END*------------------------------------------------------------------------*/
static A_INT32 get_reg_domain( )
{
    ATH_IOCTL_PARAM_STRUCT ath_param;
    A_INT32 error = A_ERROR;
    ATH_REG_DOMAIN result;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    ath_param.cmd_id = ATH_GET_REG_DOMAIN;
    ath_param.data = &result;
    ath_param.length = 0;

    error = HANDLE_IOCTL (&ath_param);

    if(error == A_OK){
      printf("Regulatory Domain 0x%x\n",result.domain);
    }
    else{
      printf("Command Failed\n");
      return error;
    }
    return A_OK;
}

static A_INT32 set_pmparams(A_INT32 index, A_INT32 argc, char* argv[])
{
    A_UINT32 enet_device, i;
    ATH_WMI_POWER_PARAMS_CMD pm;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    enet_device = get_active_device();    
    /*Fill in default values */
    pm.idle_period = 0;
    pm.pspoll_number = 10;
    pm.dtim_policy = 3;
    pm.tx_wakeup_policy = 2;
    pm.num_tx_to_wakeup = 1;
    pm.ps_fail_event_policy = 2;
	
    for(i=index; i<argc;i++)
    {
	if (ATH_STRCMP(argv[i],"--idle") == 0)
	{
	    if(i+1 == argc)  //User did not enter parameter
	    {
		printf("Missing parameter\n");
		return A_ERROR;
	    }
	    pm.idle_period = atoi(argv[i+1]);
	    i++;
	}
	else if(ATH_STRCMP(argv[i], "--np") == 0)
	{
	    if(i+1 == argc)  //User did not enter parameter
	    {
		printf("Missing parameter\n");
		return A_ERROR;
	    }

	    pm.pspoll_number = atoi(argv[i+1]);
	    i++;
	}
	else if(ATH_STRCMP(argv[i], "--dp") == 0)
	{
	    if(i+1 == argc)  //User did not enter parameter
	    {
	    	printf("Missing parameter\n");
		return A_ERROR;
	    }

	    pm.dtim_policy = atoi(argv[i+1]);
	    i++;
	}
	else if(ATH_STRCMP(argv[i], "--txwp") == 0)
	{
	    if(i+1 == argc)  //User did not enter parameter
	    {
		printf("Missing parameter\n");
		return A_ERROR;
	    }

	    pm.tx_wakeup_policy = atoi(argv[i+1]);
	    i++;
	}
	else if(ATH_STRCMP(argv[i], "--ntxw") == 0)
	{
	    if(i+1 == argc)  //User did not enter parameter
	    {
		printf("Missing parameter\n");
		return A_ERROR;
	    }

	    pm.num_tx_to_wakeup = atoi(argv[i+1]);
            i++;
	}
	else if(ATH_STRCMP(argv[i], "--psfp") == 0)
	{
	    if(i+1 == argc)  //User did not enter parameter
	    {
		printf("Missing parameter\n");
		return A_ERROR;
	    }
  	    pm.ps_fail_event_policy = atoi(argv[i+1]);
	    i++;
	}
	else
	{
	    printf("Invalid parameter\n");
	    return A_ERROR;
	}
    }

    return (qcom_power_set_parameters(enet_device,
                                      pm.idle_period,
                                      pm.pspoll_number,
                                      pm.dtim_policy,
                                      pm.tx_wakeup_policy,
                                      pm.num_tx_to_wakeup,
                                      pm.ps_fail_event_policy));
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : get_phy_mode()
* Returned Value  : A_OK if phy_mode set successfully else ERROR CODE
* Comments        : Gets the phy_mode.
*
*END*-----------------------------------------------------------------*/

A_INT32 get_phy_mode
    (
        A_UINT32 dev_num,
       char* phy_mode
    )
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    A_INT32 error;
    A_UINT32 value = 0;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    inout_param.cmd_id = ATH_GET_PHY_MODE;
    inout_param.data = &value;

    error = HANDLE_IOCTL (&inout_param);
    if (A_OK != error)
    {
        return error;
    }

    if(*(A_UINT32*)(inout_param.data) == 0x01)
    {
    	strcpy(phy_mode,"a");
    }
    else if(*(A_UINT32*)(inout_param.data) == 0x02)
    {
    	strcpy(phy_mode,"g");
    }
    else if(*(A_UINT32*)(inout_param.data) == 0x04)
    {
    	strcpy(phy_mode,"b");
    }
    else
    {
    	strcpy(phy_mode,"mixed");
    }
    return A_OK;
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : get_version()
* Returned Value  : A_OK version is retrieved successfully else ERROR CODE
* Comments        : gets driver,firmware version.
*
*END*-----------------------------------------------------------------*/
A_INT32 get_version()
{
	ATH_IOCTL_PARAM_STRUCT inout_param;
    A_INT32 error;
	versionstr_t versionstr;
	
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
	
	inout_param.cmd_id = ATH_GET_VERSION_STR;
    inout_param.data = &versionstr;
    inout_param.length = sizeof(versionstr);
	
    error = HANDLE_IOCTL(&inout_param);
    if (A_OK != error)
    {
        return error;
    }
	
	printf("Host version :  %s\n", versionstr.host_ver);
    printf("Target version   :  %s\n", versionstr.target_ver);
    printf("Firmware version :  %s\n", versionstr.wlan_ver);
    printf("Interface version:  %s\n", versionstr.abi_ver);
	
    return A_OK;
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : get_last_error()
* Returned Value  : A_OK if phy_mode set successfully else ERROR CODE
* Comments        : Gets the last error in the host driver
*
*END*-----------------------------------------------------------------*/

A_INT32 get_last_error()
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    A_INT32 error = A_ERROR;
    A_INT32 result;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    inout_param.cmd_id = ATH_GET_LAST_ERROR;
    inout_param.data = &result;
    inout_param.length = 0;

    error = HANDLE_IOCTL (&inout_param);
    if (A_OK != error)
    {
        return error;
    }

    printf("Last Driver error %d\n",(*(A_UINT32*)(inout_param.data)));
    return A_OK;
}



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : dev_susp_start()
* Returned Value  : A_OK if device suspend is started
*                   successfully else ERROR CODE
* Comments        : Suspends device for requested time period
*
*END*-----------------------------------------------------------------*/
A_INT32 dev_susp_start(char* susp_time)
{
    int suspend_time;
    A_UINT32    enet_device, wifimode;
    
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    enet_device = get_active_device();
    qcom_op_get_mode(enet_device, &wifimode);
    if (wifimode == QCOM_WLAN_DEV_MODE_AP)    
    {
		printf("Store-Recall Mechanism NOT supported in 'MODE_AP' \n");
		return A_ERROR;
    }
#if ENABLE_SSL
    if (ssl_inst[SSL_CLIENT_INST].sslCtx != NULL)
    {
		printf("Please disable SSL and start Store-Recall\n");
		return A_ERROR;
    }
#endif
    /*Check if no packets are queued, if TX is pending, then wait*/
    while(get_tx_status() != ATH_TX_STATUS_IDLE){
        ATH_TIME_DELAY_MSEC(500);
        _sched_yield();
    }
    
    suspend_time = atoi(susp_time);
    return qcom_suspend_start(suspend_time);
}



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : wmi_iwconfig()
* Returned Value  : A_OK if success else ERROR
*
* Comments        : Setup for scan command
*
*END*-----------------------------------------------------------------*/
A_INT32 wmi_iwconfig(A_INT32 argc, char* argv[])
{
    A_UINT8 scan_ssid_flag = 0;			//Flag to decide whether to scan all ssid/channels
    A_UINT8 scan_ssid[MAX_SSID_LENGTH];
    A_INT32 error = A_OK;
    A_UINT32 enet_device, wifimode;
    A_UINT8  orig_ssid[MAX_SSID_LENGTH];
#if ENABLE_P2P_MODE
    if(p2p_session_in_progress)
    {
        printf("p2p event in progress \n");
        return A_ERROR;
    }
#endif    
    enet_device = get_active_device();

    memset(scan_ssid,0,MAX_SSID_LENGTH);
    memset(orig_ssid,0,MAX_SSID_LENGTH);

    qcom_get_ssid(enet_device, (char *)orig_ssid);
    qcom_op_get_mode(enet_device, &wifimode);
    
    if(argc < 2)
    {
        printf("Missing Parameters\n");
        return A_ERROR;
    }
    else if (wifimode == QCOM_WLAN_DEV_MODE_AP)
    {
        printf("AP mode cannot scan\n");
        return A_ERROR;
    }
    else if(argc > 1)
    {
	if(ATH_STRCMP(argv[1],"scan")!=0)
	{
	    printf("Incorrect Parameter\n");
	    return A_ERROR;
	}

	if(argc == 3)
	{
	    if(strlen(argv[2]) > MAX_SSID_LENGTH)
            {
                printf("SSID length exceeds Maximum value\n");
                return A_ERROR;
            }
	    /*Scan specified SSID*/
	    scan_ssid_flag = 1;
	    strcpy((char*)scan_ssid,argv[2]);
	}
    }
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
    /*Set SSID for scan*/
    if(scan_ssid_flag)
    {
	error = qcom_set_ssid (enet_device,(char*)scan_ssid);
	if(error != A_OK)
	{
	    printf("Unable to set SSID\n");
	    return error;
	}
    }
    else
    {
	error = qcom_set_ssid (enet_device,"");
	if(error != A_OK)
	{
	    printf("Unable to set SSID\n");
	    return error;
	}
    }

    /*Do the actual scan*/
    wmi_set_scan(enet_device);

    /*Revert to original SSID*/
    error = qcom_set_ssid (enet_device,(char*)orig_ssid);
    if(error != A_OK)
    {
	printf("Unable to set SSID\n");
	return error;
    }
    return error;
}

#if ENABLE_AP_MODE
static A_INT32 ap_handler( int_32 index, int_32 argc, char_ptr argv[])
{
    uint_32  error= A_OK, i, ret_val;
    char* p = 0;
    uint_32  enet_device, wifimode;
    enet_device = get_active_device();

    qcom_op_get_mode(enet_device, &wifimode);
    if (QCOM_WLAN_DEV_MODE_AP != wifimode)
    {
      printf ("Err:Set AP Mode to apply AP settings\n");
      return A_ERROR;
    }

    if (index+1 < argc)
    {
        i = index;

        if (ATH_STRCMP(argv[i],"country") == 0)
        {
            printf("\n Cannot set country code. Use board data file or tuneables instead \n");
        }
        else
        if(ATH_STRCMP(argv[i],"inact") == 0)
        {
            ap_config_param.u.wmi_ap_conn_inact_cmd.period = strtoul(argv[i+1], &p,0);

            if(ap_config_param.u.wmi_ap_conn_inact_cmd.period == 0)
            {
     			printf ("incorrect inactive timeout \n");
      			return A_ERROR;
	    }

            ret_val = set_ap_params(AP_SUB_CMD_INACT_TIME, (A_UINT8 *)&ap_config_param.u.wmi_ap_conn_inact_cmd.period);

	    if (ret_val != A_OK)
            {
      		printf ("Err:set inactive_time\n");
      		return A_ERROR;
	    }
        }
        else
        if(ATH_STRCMP(argv[i],"bconint") == 0)
        {
            A_UINT16 beacon_Interval;
            beacon_Interval = strtoul(argv[i+1], &p,0);
            if((beacon_Interval < 100) || (beacon_Interval > 1000))
            {
      			printf ("beacon interval has to be within 100-1000 in units of 100 \n");
      			return A_ERROR;
            }
            if( qcom_ap_set_beacon_interval(enet_device,beacon_Interval) != A_OK )
            {
      			printf ("Err:set beacon interval \n");
      			return A_ERROR;              
            }
        }
        else
        if(ATH_STRCMP(argv[i],"dtim") == 0)
        {
            ret_val = strtoul(argv[i+1], &p,0);

            if((ret_val < 1) || (ret_val > 255))
            {
      		printf ("dtim interval has to be within 1-255 \n");
      		return A_ERROR;
	    }

            ap_config_param.u.wmi_ap_dtim_cmd.dtimval = ret_val; 

            ret_val = set_ap_params(AP_SUB_CMD_DTIM_INT, (A_UINT8 *)&ap_config_param.u.wmi_ap_dtim_cmd.dtimval);

            if (ret_val != A_OK)
            {
      		printf ("Err:set dtim interval \n");
      		return A_ERROR;
	    }
	}
        else
        if(ATH_STRCMP(argv[i],"psbuf") == 0)
        {
            ap_config_param.u.wmi_ap_ps_buf_cmd.ps_val[0] = strtoul(argv[i+1], &p,0);

            if(ap_config_param.u.wmi_ap_ps_buf_cmd.ps_val[0] == 0)
            {
		ap_config_param.u.wmi_ap_ps_buf_cmd.ps_val[1] = 0x0;

            	ret_val = set_ap_params(AP_SUB_CMD_PSBUF, (A_UINT8 *)ap_config_param.u.wmi_ap_ps_buf_cmd.ps_val);

            	if (ret_val != A_OK)
            	{
      			printf ("Err:set ps buf \n");
      			return A_ERROR;
		}
	    }
	    else
	    {
		ap_config_param.u.wmi_ap_ps_buf_cmd.ps_val[1] = strtoul(argv[i+2], &p,0);

            	ret_val = set_ap_params(AP_SUB_CMD_PSBUF, (A_UINT8 *)ap_config_param.u.wmi_ap_ps_buf_cmd.ps_val);

            	if (ret_val != A_OK)
            	{
      			printf ("Err:set ps buf \n");
      			return A_ERROR;
		}
	    }
	}
	else
        {
            printf ("usage:--ap [country <country code>]\n"
                    "      --ap [bconint<intvl>]\n"
                    "      --ap [dtim<val>]\n"
                    "      --ap [inact <minutes>]>\n"
                    "      --ap [psbuf <1/0-enable/disable> <buf_count for enable>]>\n");
        }
    }
    else
    {
            printf ("usage:--ap [country <country code>]\n"
                    "      --ap [bconint<intvl>]\n"
                    "      --ap [dtim<val>]\n"
                    "      --ap [inact <minutes>]>\n"
                    "      --ap [psbuf <1/0-enable/disable> <buf_count for enable>]>\n");
    }
    return error;
}

static A_INT32 set_ap_params(A_UINT16 param_cmd, A_UINT8 *data_val)
{
	ATH_IOCTL_PARAM_STRUCT inout_param;
	ATH_AP_PARAM_STRUCT ap_param;
	uint_32 error;

	inout_param.cmd_id = ATH_CONFIG_AP;
	ap_param.cmd_subset = param_cmd;
	ap_param.data		= data_val;
	inout_param.data	= &ap_param;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    error = HANDLE_IOCTL (&inout_param);
    if (A_OK != error)
    {
        return error;
    }

    return A_OK;
}
#endif

#if ENABLE_STACK_OFFLOAD

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ipconfig_query()
* Returned Value  : A_OK if success else A_ERROR
*
* Comments        : Sample function to depict ipconfig functionality.
*                   Queries IP parameters from target.
*
*END*-----------------------------------------------------------------*/
A_INT32 ipconfig_query(A_INT32 argc, char* argv[])
{
    A_UINT32 enet_device, addr = 0, mask = 0, gw = 0, num;
    IP6_ADDR_T v6Global;
    IP6_ADDR_T v6GlobalExtd;
    IP6_ADDR_T v6LinkLocal;
    IP6_ADDR_T v6DefGw;
    char ip6buf[48];
    char mac_data[6];
    char *ip6ptr =  NULL;
    A_INT32 LinkPrefix = 0;
    A_INT32 GlobalPrefix = 0;
    A_INT32 DefGwPrefix = 0;
    A_INT32 GlobalPrefixExtd = 0;
    A_UINT32 dnsaddr[MAX_DNSADDRS];
    int i = 0;
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
    enet_device = get_active_device();    
    qcom_get_bssid(enet_device, (A_UINT8 *)mac_data);
    printf("\n mac addr    =   %02x:%02x:%02x:%02x:%02x:%02x \n", mac_data[0], mac_data[1], mac_data[2],
                                                      mac_data[3], mac_data[4], mac_data[5]);

	//t_ipconfig((void*)handle,IPCFG_QUERY, &addr, &mask, &gw,dnsaddr,hostname);
        qcom_ipconfig(enet_device, IPCFG_QUERY, &addr, &mask, &gw);
        printf("IP:%x Mask:%x, Gateway:%x\n", addr, mask, gw);
	printf("IP:%d.%d.%d.%d Mask:%d.%d.%d.%d, Gateway:%d.%d.%d.%d\n",
				getByte(3, addr), getByte(2, addr), getByte(1, addr), getByte(0, addr),
				getByte(3, mask), getByte(2, mask), getByte(1, mask), getByte(0, mask),
				getByte(3, gw), getByte(2, gw), getByte(1, gw), getByte(0, gw));

        qcom_dns_server_address_get(dnsaddr, &num); 
	for(i = 0; i < num; i++)
	{
            addr = A_CPU2LE32(dnsaddr[i]);  		   
	    printf("DNS Server:%d.%d.%d.%d \n", getByte(0, addr), getByte(1,addr), 
                                                getByte(2,addr), getByte(3, addr));
	}	
        
	memset(&v6LinkLocal,0, sizeof(IP6_ADDR_T));
        memset(&v6Global,0, sizeof(IP6_ADDR_T));
        memset(&v6DefGw,0, sizeof(IP6_ADDR_T));
	memset(&v6GlobalExtd,0, sizeof(IP6_ADDR_T));
	qcom_ip6_address_get(enet_device, (A_UINT8 *)&v6Global,(A_UINT8 *)&v6LinkLocal,(A_UINT8 *)&v6DefGw,(A_UINT8 *)&v6GlobalExtd,&LinkPrefix,&GlobalPrefix,&DefGwPrefix,&GlobalPrefixExtd);

        if(v6LinkLocal.addr[0] )
	{
           ip6ptr = print_ip6(&v6LinkLocal,ip6buf);
	   if(ip6ptr)
           {
             if(LinkPrefix)
             printf("Link-local IPv6 Address ..... : %s/%d \n", ip6ptr,LinkPrefix);
	     else
             printf("Link-local IPv6 Address ..... : %s \n", ip6ptr);
	   }
           ip6ptr = NULL;
           ip6ptr = print_ip6(&v6Global,ip6buf);
           if(ip6ptr)
           {
              if(GlobalPrefix)
	      printf("Global IPv6 Address ......... : %s/%d \n", ip6ptr,GlobalPrefix);
	      else
	      printf("Global IPv6 Address ......... : %s \n", ip6ptr);
	    }
	    ip6ptr = NULL;
            ip6ptr = print_ip6(&v6DefGw, ip6buf);
	    if(ip6ptr)
	    {
	      if(DefGwPrefix)
	      printf("Default Gateway  ............. : %s/%d \n", ip6ptr,DefGwPrefix);
	      else
	      printf("Default Gateway  ............. : %s \n", ip6ptr);

	    }
            ip6ptr = NULL;
            ip6ptr = print_ip6(&v6GlobalExtd,ip6buf);
            if(ip6ptr)
            {
              if(GlobalPrefixExtd)
	      printf("Global IPv6 Address 2 ......... : %s/%d \n", ip6ptr,GlobalPrefixExtd);
	      else
	      printf("Global IPv6 Address 2 ......... : %s \n", ip6ptr);
	    }

	}
  
     	return 0; 
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ipconfig_static()
* Returned Value  : A_OK if success else A_ERROR
*
* Comments        : Sample function to depict ipconfig functionality.
*                   Sets static IPv4 parameters.
*
*END*-----------------------------------------------------------------*/
A_INT32 ipconfig_static(A_INT32 argc, char* argv[])
{
	A_UINT32 addr, mask, gw;
	char* ip_addr_string;
	char* mask_string;
	char* gw_string;
        A_INT32 error;
	unsigned int sbits;
        A_UINT32 enet_device = get_active_device(); 
        /*Check if driver is loaded*/
        if(IS_DRIVER_READY != A_OK){
            return A_ERROR;
        }
	if(argc < 5)
	{
		printf("incomplete params\n");
		return A_ERROR;
	}
	ip_addr_string = argv[2];
	mask_string = argv[3];
	gw_string = argv[4];

	error=parse_ipv4_ad((unsigned long *)&addr, &sbits, ip_addr_string);
	if(error!=0)
	{
		printf("Invalid IP address\n");
		return error;
	}
	addr = A_BE2CPU32(addr);
	error=parse_ipv4_ad((unsigned long *)&mask, &sbits, mask_string);
	if(error!=0)
	{
		printf("Invalid Mask address\n");
		return error;
	}
	mask = A_BE2CPU32(mask);
	error=parse_ipv4_ad((unsigned long *)&gw, &sbits, gw_string);
	if(error!=0)
	{
		printf("Invalid Gateway address\n");
		return error;
	}
	gw = A_BE2CPU32(gw);
	qcom_ipconfig(enet_device,IPCFG_STATIC, &addr, &mask, &gw);
	printf("IP:%x Mask:%x, Gateway:%x\n", addr, mask, gw);
	printf("IP:%d.%d.%d.%d Mask:%d.%d.%d.%d, Gateway:%d.%d.%d.%d\n",
				getByte(3, addr), getByte(2, addr), getByte(1, addr), getByte(0, addr),
				getByte(3, mask), getByte(2, mask), getByte(1, mask), getByte(0, mask),
				getByte(3, gw), getByte(2, gw), getByte(1, gw), getByte(0, gw));


	return 0;
}

void configure_auto_ipv4_addr()
{
	A_UINT32 addr, mask, gw;
	char* ip_addr_string;
	char* mask_string;
	char* gw_string;
        A_INT32 error;
	unsigned int sbits;
        A_UINT32 enet_device = get_active_device(); 
        
	ip_addr_string = "169.254.1.1";
	mask_string = "255.255.0.0";
	gw_string = "0.0.0.0";

	error=parse_ipv4_ad((unsigned long *)&addr, &sbits, ip_addr_string);
	if(error!=0)
	{
		printf("Invalid IP address\n");
		return; 
	}
	addr = A_BE2CPU32(addr);
	error=parse_ipv4_ad((unsigned long *)&mask, &sbits, mask_string);
	if(error!=0)
	{
		printf("Invalid Mask address\n");
		return;
	}
	mask = A_BE2CPU32(mask);
	error=parse_ipv4_ad((unsigned long *)&gw, &sbits, gw_string);
	if(error!=0)
	{
		printf("Invalid Gateway address\n");
		return;
	}
	gw = A_BE2CPU32(gw);
	qcom_ipconfig(enet_device,IPCFG_STATIC, &addr, &mask, &gw);
	printf("Acquired IP address via AutoIPv4\r\n");
	printf("IP:%x Mask:%x, Gateway:%x\n", addr, mask, gw);
	printf("IP:%d.%d.%d.%d Mask:%d.%d.%d.%d, Gateway:%d.%d.%d.%d\n",
				getByte(3, addr), getByte(2, addr), getByte(1, addr), getByte(0, addr),
				getByte(3, mask), getByte(2, mask), getByte(1, mask), getByte(0, mask),
				getByte(3, gw), getByte(2, gw), getByte(1, gw), getByte(0, gw));


	return ;
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ipconfig_dhcp()
* Returned Value  : A_OK if success else A_ERROR
*
* Comments        : Sample function to depict ipconfig functionality.
*                   Runs DHCP on target (only IPv4).
*
*END*-----------------------------------------------------------------*/
A_INT32 ipconfig_dhcp(A_INT32 argc, char* argv[])
{
    A_UINT32 addr, mask, gw;
    int err = 0;
    A_UINT32 enet_device;
    enet_device = get_active_device();      

    if(wifi_connected_flag == 0)
    {
      printf("ERROR: No WiFi connection available, please connect to an Access Point\n");
      return A_ERROR;
    }	    
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
    set_power_mode( MAX_PERF_POWER,DHCP);          

    err = qcom_ipconfig(enet_device,IPCFG_DHCP, &addr, &mask, &gw);
    if(err == -1 )
    {
       printf("dhcp timed out. Configuring auto ip \r\n");
       configure_auto_ipv4_addr();
       set_power_mode( REC_POWER,DHCP);          
       return 0;
    }	    
    addr = 0;
    qcom_ipconfig(enet_device,IPCFG_QUERY, &addr, &mask, &gw);
    if(addr != 0){
	printf("IP:%d.%d.%d.%d Mask:%d.%d.%d.%d, Gateway:%d.%d.%d.%d\n",
				getByte(3, addr), getByte(2, addr), getByte(1, addr), getByte(0, addr),
				getByte(3, mask), getByte(2, mask), getByte(1, mask), getByte(0, mask),
				getByte(3, gw), getByte(2, gw), getByte(1, gw), getByte(0, gw));
    }

    set_power_mode( REC_POWER,DHCP);          
    return 0;
}

A_INT32 ipconfig_auto_ipv4(A_INT32 argc, char* argv[],int mode)
{
    A_UINT32 addr = 0, mask = 0, gw = 0;
    int err = 0;
    unsigned int sbits;
    char* ip_addr_string;
    A_UINT32 enet_device = get_active_device(); 
	
    if(wifi_connected_flag == 0)
    {
      printf("ERROR: No WiFi connection available, please connect to an Access Point\n");
      return A_ERROR;
    }	    
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
	
	if(argc == 3)
	{
		ip_addr_string = argv[2];
        err=parse_ipv4_ad((unsigned long *)&addr, &sbits, ip_addr_string);   
    	if(err!=0)
    	{
    		printf("Invalid IP address\n");
    		return err;
    	}
    	addr = A_BE2CPU32(addr);

        if(addr < dBASE_AUTO_IP_ADDRESS || addr > dMAX_AUTO_IP_ADDRESS)
        {   
            printf("Invalid link local IPv4 address\n");
            return A_ERROR;
        }
	}
    err = qcom_ipconfig(enet_device,mode, &addr, &mask, &gw);
    
	if(err == -1 && mode == IPCFG_AUTO)
    {
       printf("Auto IP configure failed \r\n");
       return 0;
    }
	  
    addr = 0;
    qcom_ipconfig(enet_device,IPCFG_QUERY, &addr, &mask, &gw);
    if(addr != 0){
	printf("IP:%d.%d.%d.%d Mask:%d.%d.%d.%d, Gateway:%d.%d.%d.%d\n",
				getByte(3, addr), getByte(2, addr), getByte(1, addr), getByte(0, addr),
				getByte(3, mask), getByte(2, mask), getByte(1, mask), getByte(0, mask),
				getByte(3, gw), getByte(2, gw), getByte(1, gw), getByte(0, gw));
    }
    return 0;
}

A_INT32 block_on_dhcp()
{
    A_UINT32 addr=0, mask=0, gw=0;
    A_UINT32 enet_device = get_active_device(); 
    
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    /*Start DHCP*/
    qcom_ipconfig(enet_device,IPCFG_DHCP, &addr, &mask, &gw);

    /*Wait till we get an IP address, instead of indefinte loop, develop may
      use a timeout logic here*/
    while(addr == 0){
      qcom_ipconfig(enet_device,IPCFG_QUERY, &addr, &mask, &gw);
      A_MDELAY(100);
    }
    printf("addr 0x%x\n",addr);
    return A_OK;
}


/*FUNCTION*--------------------------------------------------------------------
*
* Function Name   : set_tcp_connection_timeout()
* Returned Value  : A_OK - on successful completion
*					A_ERROR - on any failure.
* Comments        : sets TCP Connection Time out 
*
*END*------------------------------------------------------------------------*/
static A_INT32 set_tcp_connection_timeout(A_INT32 argc,char* argv[])
{		
	A_UINT32 timeout_val;
      if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
	    if((argc > 3) || (argc < 2))
    {
      printf("incorrect arguments refer help \n");
	  printf("Usage: wmiconfig --settcptimeout <timeout> \n");
      return A_ERROR;
    }    
	
	timeout_val=atoi(argv[2]);
	  if (A_OK != qcom_tcp_conn_timeout(timeout_val))
    {
        printf("Set TCP Timeout Failed\r\n");
        return A_ERROR;
    }
    
	    return A_OK;
	
 
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ip_set_bridgemode()
* Returned Value  : A_OK if success else A_ERROR
*
* Comments        : Fucntion to enable bridge mode.
*
*END*-----------------------------------------------------------------*/

A_INT32 ip_set_bridgemode(A_INT32 argc, char* argv[])
{
	int err;
	
	if(wifi_connected_flag == 1)
	{
		printf("Please reboot and execute this as first command\n");
		return A_ERROR;
	}
	
	/*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
	
	err = qcom_bridge_mode_enable(1);
    if(err == -1)
    {
       return A_ERROR;
    }
	
	printf("Please reboot and execute this as first command\n");
	
	return A_OK;
	
}

/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ipconfig_dhcp_pool()
* Returned Value  : A_OK if success else A_ERROR
*
* Comments        : Sample function to configure dhcp pool.
*
*END*-----------------------------------------------------------------*/
A_INT32 ipconfig_dhcp_pool(A_INT32 argc, char* argv[])
{
	A_UINT32 startaddr, endaddr;
	char* start_ip_addr_string;
	char* end_ip_addr_string;
        A_INT32 error;
	unsigned int sbits;
	int leasetime;
        A_UINT32 addr=0, mask=0, gw=0;
        A_UINT32 enet_device = get_active_device(); 
        
        /*Check if driver is loaded*/
        if(IS_DRIVER_READY != A_OK){
            return A_ERROR;
        }
	if(argc < 5)
	{
		printf("incomplete params\n");
		return A_ERROR;
	}
	start_ip_addr_string = argv[2];
	end_ip_addr_string = argv[3];
	leasetime = atoi(argv[4]);

	error=parse_ipv4_ad((unsigned long *)&startaddr, &sbits, start_ip_addr_string);
	if(error!=0)
	{
		printf("Invalid start IP address\n");
		return error;
	}
	startaddr = A_BE2CPU32(startaddr);
	error=parse_ipv4_ad((unsigned long *)&endaddr, &sbits, end_ip_addr_string);
	if(error!=0)
	{
		printf("Invalid end ip address\n");
		return error;
	}
	endaddr = A_BE2CPU32(endaddr);
        qcom_ipconfig(enet_device,IPCFG_QUERY, &addr, &mask, &gw);
	if((addr & mask) != (startaddr & mask))
        {
            printf("LAN IP address and start ip address are not in the same subnet \n");
            return error;
        }
	if((addr & mask) != (endaddr & mask))
        {
            printf("LAN IP address and end ip address are not in the same subnet \n");
            return error;
        }
	if((addr  >= startaddr) && (addr  <= endaddr))
	{
            printf("Please configure pool beyond LAN IP Address \n");
            return error;
        }
	if(startaddr >= endaddr)
        {
            printf("Staraddr must be less than end addr \n");
            return error;
        }
	qcom_dhcps_set_pool(enet_device, startaddr, endaddr, leasetime);
	return 0;
}

A_INT32 ip6_set_router_prefix(A_INT32 argc,char *argv[])
{
    unsigned char  v6addr[16];
    A_INT32 retval=-1;
    int prefixlen = 0;
    int prefix_lifetime = 0;
    int valid_lifetime = 0;
    A_UINT32    enet_device, wifimode;
    
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 6)
    {
	printf("incomplete params\n");
	return A_ERROR;
    }

    enet_device = get_active_device();
    qcom_op_get_mode(enet_device, &wifimode);
    if (wifimode != QCOM_WLAN_DEV_MODE_AP)
    {
	printf("ipv6 rt prfx support not possible in station \n");
	return A_ERROR;
    }

    retval = Inet6Pton(argv[2],v6addr);
    if(retval == 1)
    {
        printf("Invalid ipv6 prefix \n");
        return (A_ERROR);
    }
	
    prefixlen =  atoi(argv[3]);
    prefix_lifetime = atoi(argv[4]);
    valid_lifetime =  atoi(argv[5]);
    qcom_ip6config_router_prefix(enet_device, v6addr,prefixlen,prefix_lifetime,valid_lifetime);
    return 0;
}

static A_INT32 ip_set_tcp_exponential_backoff_retry(A_INT32 argc,char *argv[])
{
    A_INT32 retry = 0;
    A_UINT32 enet_device = get_active_device();
    
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
          return A_ERROR;
    }
    if(argc < 2)
    {
       printf("incomplete params\n");
       return A_ERROR;
    }
    retry=atoi(argv[2]);
    if((retry > 12) || (retry < 4)) 
    {
        printf("Confiure TCP Max retries between 4 and 12");
	return (A_ERROR);	
     }	    
    qcom_tcp_set_exp_backoff(enet_device,retry);
    return A_OK;
}

static A_INT32 ip6_set_status(A_INT32 argc,char *argv[])
{

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
          return A_ERROR;
    }
    if(argc < 2)
    {
       printf("incomplete params\n");
       return A_ERROR;
    }

    if(ATH_STRCMP(argv[2],"enable") == 0)
    {
      custom_ipconfig_set_ip6_status((void *)handle,1);	    
      return A_OK;
    }	    
    else if(ATH_STRCMP(argv[2],"disable") == 0)
    {
       custom_ipconfig_set_ip6_status((void *)handle,0);
       return A_OK; 
    }	    
    printf("Allowed values are enable and disable only");
    return A_OK;
}

static A_INT32 ipconfig_dhcp_release(A_INT32 argc,char *argv[])
{
    A_UINT32 enet_device = get_active_device(); 
    
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
    qcom_dhcps_release_pool(enet_device);
    return 0;
}

static A_INT32 ipconfig_tcp_rxbuf(A_INT32 argc,char *argv[])
{
     A_INT32 rxbuf = 0;	
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 2)
    {
       printf("incomplete params\n");
       return A_ERROR;
    }
    rxbuf=atoi(argv[2]);
    if(rxbuf > 10) 
    {
        printf("Max Allowed RX Buffer is 10");
	return (A_ERROR);	
    }	    
    custom_ipconfig_set_tcp_rx_buffer((void *)handle,rxbuf);
    return A_OK;
}

#if ENABLE_DNS_CLIENT
static A_INT32 ip_dns_client(A_INT32 argc,char *argv[])
{

    A_INT32 command = -1;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 3)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }
    if(ATH_STRCMP(argv[2], "start") == 0) 
    {
        command = 1;
    }
    else if(ATH_STRCMP(argv[2], "stop") == 0)
    {
        command = 0;
    }
    else
    {
        printf("Supported commands: start, stop");
        return (A_ERROR);	
    }	    
    if (A_OK != qcom_dnsc_enable(command))
    {
        printf("Error! DNS Client %s failed\n", command?"start":"stop");
    }
    return A_OK;

}
static A_INT32 ip_dns_server_addr(A_INT32 argc, char *argv[]) 
{
	A_UINT32 addr = 0;
	char * ip_addr_string = NULL;
	A_INT32 error;
	unsigned int sbits;
	A_INT32 retval = -1;
	unsigned char v6addr[16];

	/*Check if driver is loaded*/
	if (IS_DRIVER_READY != A_OK) {
		return A_ERROR;
	}

	if (argc < 3) {
		printf("Incomplete params\n");
		return A_ERROR;
	}

	ip_addr_string = argv[2];

	error = parse_ipv4_ad((unsigned long *)&addr, &sbits, ip_addr_string);
	if (error == 0) {
		if (A_OK != qcom_dnsc_add_server_address((A_UINT8 *)&addr, ATH_AF_INET)) 
                {
			printf("Error! DNS Server address conf failed \r\n");
			return (A_ERROR);
		}
		return A_OK;
	}

	retval = Inet6Pton(argv[2], v6addr);
	if (retval == 1) {
		printf("Invalid ipv6 prefix \n");
		return (A_ERROR);
	}

	if (A_OK != qcom_dnsc_add_server_address(v6addr, ATH_AF_INET6)) {
		printf("Error! DNS Server address conf failed \r\n");
		return (A_ERROR);
	}
        
	return A_OK;
}

static A_INT32 ip_dns_delete_server_addr(A_INT32 argc,char *argv[])
{
    A_UINT32 addr = 0;
    char * ip_addr_string = NULL;
    A_INT32 error;
    unsigned int sbits;
    A_INT32 retval = -1;
    unsigned char  v6addr[16];

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 3)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }
    ip_addr_string = argv[2];

    error=parse_ipv4_ad((unsigned long *)&addr, &sbits, ip_addr_string);
    if(error == 0)
    {
        if (A_OK != qcom_dnsc_del_server_address((A_UINT8 *)&addr, ATH_AF_INET))
        {
          printf("Error! DNS Server address conf failed \r\n");
	  return (A_ERROR);
        }
	return A_OK;
    }	   

    retval = Inet6Pton(argv[2],v6addr);
    if(retval == 1)
    {
        printf("Invalid ipv6 prefix \n");
        return (A_ERROR);
    }
    if (A_OK != qcom_dnsc_del_server_address(v6addr, ATH_AF_INET6))
    {
        printf("Error! DNS Server address conf failed \r\n");
        return (A_ERROR);
    }
    return A_OK;
}
#endif /* ENABLE_DNS_CLIENT*/


#if ENABLE_HTTP_SERVER

/*Callback function called by PHOST when a client posts to HTTP server*/
void swat_http_post_callback(void* cxt, void* buf)
{
    HTTP_POST_EVENT_T* ev = (HTTP_POST_EVENT_T*)buf;
    int numTLV;
    unsigned char* data;

    if(!buf)
        return;

    numTLV = ev->numTLV;
    data = ev->data;

    while(numTLV){
        short type;
        short length;
        unsigned char* val;

        /*Parse through all TLVs*/
        GET_TLV_TYPE(data, type);
        GET_TLV_LENGTH(data, length);
        val = GET_TLV_VAL(data);
        switch(type){
            case POST_TYPE_URI:
                printf("URI: %s\n",val); 
                break;
            case POST_TYPE_NAME:
                printf("NAME: %s\n",val); 
                break;
            case POST_TYPE_VALUE: 
                printf("VALUE: %s\n",val); 
                break;
            default:
                printf("Invalid TLV\n");
                break;

        }
        data = GET_NEXT_TLV(data, length);
        numTLV--;
    }
}


static A_INT32 ip_http_server(A_INT32 argc,char *argv[])
{
    A_INT32 command = HTTP_UNKNOWN;
    A_INT32 error;
	
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
	    
    if(argc < 2)
    {
		printf("Incomplete params\n");
		return A_ERROR;
    }
	if (ATH_STRCMP(argv[1],"--ip_http_server") == 0)
	{
		if(ATH_STRCMP(argv[2], "start") == 0) 
		{
			command = HTTP_SERVER_START;
                        qcom_http_set_post_cb(NULL, swat_http_post_callback);
		}
		else if(ATH_STRCMP(argv[2], "stop") == 0)
		{
			command = HTTP_SERVER_STOP;
		}
		else
		{
			printf("Supported commands: start, stop");
			return (A_ERROR);	
		} 
	}
	else  if (ATH_STRCMP(argv[1],"--ip_https_server") == 0)
	{
		if(ATH_STRCMP(argv[2], "start") == 0) 
		{
			command = HTTPS_SERVER_START;
                        qcom_http_set_post_cb(NULL, swat_http_post_callback);
		}
		else if(ATH_STRCMP(argv[2], "stop") == 0)
		{
			command = HTTPS_SERVER_STOP;
		}
		else
		{
			printf("Supported commands: start, stop");
			return (A_ERROR);	
		} 
	}
	
	error = qcom_http_server(command);
   
	if ((command == HTTP_SERVER_START) || (command == HTTP_SERVER_STOP))
	{
		if (A_OK != error)
			printf("Error! HTTP Server %s\n", (command == HTTP_SERVER_START)?"start failed":"not running");
		else
			printf("HTTP server %s success\n", (command == HTTP_SERVER_START)?"start":"stop");
	}
	else if ((command == HTTPS_SERVER_START) || (command == HTTPS_SERVER_STOP))
	{
		if (A_OK != error)
			printf("Error! HTTPs Server %s\n", (command == HTTPS_SERVER_START)?"start failed":"not running");
		else
			printf("HTTPs server %s success\n", (command == HTTPS_SERVER_START)?"start":"stop");
	}
    return A_OK;
}

static A_INT32 ip_http_server_post(A_INT32 argc,char *argv[])
{
    A_INT32 command = HTTP_POST_METHOD;
    A_INT32 objtype, objlen;
	    
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 7)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }
    
    objtype = atoi(argv[4]);
    objlen = atoi(argv[5]);
    if((objtype != 1) && (objtype != 2) && (objtype != 3))  
    {
        printf("Error: <objtype> can be 1 ,2 or 3\n");
        return A_ERROR;
    }
	    
    if (A_OK != qcom_ip_http_server_method(command, (A_UINT8 *)argv[2], (A_UINT8 *)argv[3], objtype, objlen, (A_UINT8 *)argv[6]))
    {
        printf("Error! HTTP post failed\n");
    }
    return A_OK;
}

static A_INT32 ip_http_server_get(A_INT32 argc,char *argv[])
{
    A_INT32 command = HTTP_GET_METHOD;
    A_INT32 objtype, objlen;
    A_UINT8 *value;
	    
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 4)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }
    objtype = 0;
    objlen = 0;
#if 0
    if((objtype != 1) && (objtype != 2) && (objtype != 3))  
    {
        printf("Error: <objtype> can be 1 ,2 or 3\n");
        return A_ERROR;
    }
#endif
    value = A_MALLOC(512, MALLOC_ID_TEMPORARY);
    if (!value)
        return A_ERROR;
	    
    if (A_OK != qcom_ip_http_server_method(command, (A_UINT8 *)argv[2], (A_UINT8 *)argv[3], objtype, objlen, value))
    {
        printf("Error! HTTP post failed\n");
    }
    else
    {
        printf("Value is %s\n", value);
    }
    A_FREE(value, MALLOC_ID_TEMPORARY);
    return A_OK;
}

#endif // ENABLE_HTTP_SERVER
#if ENABLE_DNS_CLIENT
static A_INT32 ip_resolve_hostname(A_INT32 argc,char *argv[])
{
    A_UINT32 addr, domain;
    char ip6addr[16], ip6buf[48];
    char *ip6ptr =  NULL;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 4)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }
    if(strlen(argv[2]) > 32) 
    {
        printf("host name cannot be more then 32 Bytes\n");
        return A_ERROR;
    }
    domain = atoi(argv[3]); 
    
    if (A_OK != qcom_dnsc_get_host_by_name2(argv[2], (A_UINT32 *)&ip6addr[0], domain, RESOLVEHOSTNAME))
    {
        printf("Unable to resolve host name\r\n");
        return A_ERROR;
    }
    if(domain == 2)
    {	    
       addr = A_CPU2BE32((A_UINT32) *(A_UINT32 *)&ip6addr[0]);
       printf("resolved IP:%d.%d.%d.%d \r\n",getByte(3, addr), getByte(2, addr), getByte(1, addr), getByte(0, addr));
    }
    if(domain == 3)
    {
      ip6ptr = print_ip6((IP6_ADDR_T *)&ip6addr[0],ip6buf);
      printf("Resolved IP6Addr ......... : %s \n", ip6ptr);
    }	    
    return A_OK;
}

A_INT32 ip_get_addr_for_host_name(char *hostname,A_UINT32 *v4addr,IP6_ADDR_T *ip6addr,A_UINT32 domain)
{
    A_UINT32 addr = 0;
    char ip6address[16], ip6buf[48];
    char *ip6ptr =  NULL;
   
    /*Check if driver is loaded*/
    if (A_OK != qcom_dnsc_get_host_by_name2(hostname, (A_UINT32 *)&ip6address[0], domain, RESOLVEHOSTNAME))
    {
        printf("Unable to resolve host name\r\n");
        return A_ERROR;
    }
    if(domain == 2)
    {	 
      *v4addr = *(A_UINT32 *)&ip6address[0];
       addr = A_CPU2BE32(*v4addr);
       printf("resolved IP:%d.%d.%d.%d \r\n",getByte(3, addr), getByte(2, addr), 
              getByte(1, addr), getByte(0, addr));
    }
    if(domain == 3)
    {
      ip6ptr = print_ip6((IP6_ADDR_T *)&ip6address[0],ip6buf);
      printf("Resolved IP6Addr ......... : %s \n", ip6ptr);
      memcpy(ip6addr,&ip6address[0],16);
    }	    
    return A_OK;
}

static A_INT32 ip_gethostbyname(A_INT32 argc,char *argv[])
{
    A_UINT32 addr;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 3)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }
    if(strlen(argv[2]) > 32) 
    {
        printf("host name cannot be more then 32 Bytes\n");
        return A_ERROR;
    }
    
    if (A_OK != qcom_dnsc_get_host_by_name(argv[2], &addr))
    {
        printf("Unable to resolve host name\r\n");
        return A_ERROR;
    }
    printf("addr:%d.%d.%d.%d \r\n",getByte(3, addr), getByte(2, addr), getByte(1, addr), getByte(0, addr));
    return A_OK;
}
static A_INT32 ip_gethostbyname2(A_INT32 argc,char *argv[])
{
    A_INT32 domain;
    A_UINT32 addr;
    A_UINT8 ip6addr[16];
    char ip6buf[48];
    char *ip6ptr =  NULL;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 4)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }
    if(strlen(argv[2]) > 32) 
    {
        printf("host name cannot be more then 32 Bytes\n");
        return A_ERROR;
    }
    domain = atoi(argv[3]);
     
    if (A_OK != qcom_dnsc_get_host_by_name2(argv[2], (A_UINT32 *)ip6addr, domain, GETHOSTBYNAME2))
    {
        printf("Unable to resolve host name\r\n");
        return A_ERROR;
    }
    if(domain == 2) 
    { 	    
      addr = A_CPU2BE32((A_UINT32)* ((A_UINT32 *)&ip6addr[0]));
      printf("addr:%d.%d.%d.%d \r\n",getByte(3, addr), getByte(2, addr), getByte(1, addr), getByte(0, addr));
    }
    else if(domain == 3)
    {
      ip6ptr = print_ip6((IP6_ADDR_T *)&ip6addr[0],ip6buf);
      printf("Resolved IP6Addr ......... : %s \n", ip6ptr);
    } 	    
    return A_OK;
}
#endif /* ENABLE_DNS_CLIENT */

static A_INT32 iphostname(A_INT32 argc,char *argv[])
{

    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 3)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }
    if(strlen(argv[2]) > 32) 
    {
        printf("domain name cannot be more then 32 Bytes\n");
        return A_ERROR;
    }
    if (A_OK != custom_ip_hostname((void *)handle, argv[2]))
    {
        printf("Release DHCP, Configure the hostname and renew DHCP\r\n");
        return A_ERROR;
    }
    return A_OK;
}

#if ENABLE_DNS_SERVER
static A_INT32 ip_dns_local_domain(A_INT32 argc,char *argv[])
{

    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 3)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }
    if(strlen(argv[2]) > 32) 
    {
        printf("domain name cannot be more then 32 Bytes\n");
        return A_ERROR;
    }
    if (A_OK != qcom_dns_local_domain(argv[2]))
    {
        printf("Unable to configure local domain name\r\n");
        return A_ERROR;
    }
    return A_OK;
}

static A_INT32 ipdns(A_INT32 argc,char *argv[])
{
    A_UINT32 addr = 0;
    char * ip_addr_string = NULL;
    A_INT32 error;
    unsigned int sbits;
    IP46ADDR dnsaddr;
    A_INT32 retval = -1;
    unsigned char  v6addr[16];
    A_INT32 command = 0;
    A_STATUS rtn;
    
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 5)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }
    if(strcmp(argv[2] ,"add") == 0) 
    {
      command =1;
    }
    if(strcmp(argv[2],"delete") == 0) 
    {
      command = 2;
    }	    
    if(strlen(argv[3]) > 32) 
    {
        printf("domain name cannot be more then 32 Bytes\n");
        return A_ERROR;
    }
    memset(&dnsaddr,0,sizeof(IP46ADDR));
    ip_addr_string = argv[4];
    error=parse_ipv4_ad((unsigned long *)&addr, &sbits, ip_addr_string);
    if(error == 0)
    {
        if(command ==1 )
           rtn = qcom_dns_entry_create(argv[3], addr);
        else
           rtn = qcom_dns_entry_delete(argv[3], addr);
        if (A_OK != rtn )
        {
          printf("Error! DNS database conf failed \r\n");
	  return (A_ERROR);
        }
	return A_OK;
    }	   
    retval = Inet6Pton(argv[4],v6addr);
    if(retval == 1)
    {
        printf("Invalid ipv6 prefix \n");
        return (A_ERROR);
    }

    if(command ==1 )
       rtn = qcom_dns_6entry_create(argv[3], v6addr);
    else
       rtn = qcom_dns_6entry_delete(argv[3], v6addr);
    
    if (A_OK != rtn)
    {
        return A_ERROR;
    }
    return A_OK;
}

static A_INT32 ip_dns_server_enable(A_INT32 argc, char *argv[]) 
{
    A_UINT32    enet_device, wifimode;
    A_INT32 command = -1;

    /*Check if driver is loaded*/
    if (IS_DRIVER_READY != A_OK) {
	return A_ERROR;
    }

        enet_device = get_active_device();
        qcom_op_get_mode(enet_device, &wifimode);
        if (wifimode != QCOM_WLAN_DEV_MODE_AP)  /*check previous mode*/
        {
		printf("DNS Server can be enabled only in AP Mode\n");
		return A_ERROR;
        }

	if (argc < 2) {
		printf("Incomplete params\n");
		return A_ERROR;
	}

	if (ATH_STRCMP(argv[2], "enable") == 0) {
		command = 1;
	} else if (ATH_STRCMP(argv[2], "disable") == 0) {
		command = 0;
	} else {
		printf("Supported commands: enable, disable");
		return (A_ERROR);
	}
	if (A_OK != qcom_dnss_enable(command)) {
		printf("Error! DNS Server %s failed\n",
				command ? "enable" : "disable ");
	} else {
		printf("DNS Server %sed successfully\n", command ? "enabled" : "disabled");
	}
	return A_OK;
}
#endif /* ENABLE_DNS_SERVER */

#if ENABLE_SNTP_CLIENT
static A_INT32 ip_sntp_srvr(A_INT32 argc,char *argv[])
{
    A_INT32 command = 0;

    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 4)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }
    if(strcmp(argv[2] ,"add") == 0) 
    {
      command =1;
    }
    if(strcmp(argv[2],"delete") == 0) 
    {
      command = 2;
    }
    if(strlen(argv[3]) > 64) 
    {
        printf("domain name or address cannot be more then 64 Bytes\n");
        return A_ERROR;
    }
    
    qcom_sntp_srvr_addr(command,argv[3]);
    return A_OK;
}

static A_INT32 ip_sntp_get_time(A_INT32 argc,char *argv[])
{
    A_UINT32    enet_device;
    tSntpTime SntpTime ;
    char *months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    char *Day[7] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
     A_UINT8 m[4];

    memset(&SntpTime,0,sizeof(tSntpTime));

    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
    
    enet_device = get_active_device();
    qcom_sntp_get_time(enet_device, &SntpTime);

    memset(m,0,4); 
    memcpy(m,&(months[SntpTime.mon][0]),3); 
    printf("TimeStamp: %s %s %d, %d %d:%d:%d\r\n",Day[SntpTime.wday],m,SntpTime.yday,SntpTime.year,SntpTime.hour,SntpTime.min,SntpTime.Sec);
    return A_OK;
}

static A_INT32 ip_sntp_get_time_of_day(A_INT32 argc,char *argv[])
{
    A_UINT32    enet_device;
    tSntpTM SntpTm;

    memset(&SntpTm,0,sizeof(tSntpTM));

    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    enet_device = get_active_device();
    qcom_sntp_get_time_of_day(enet_device, &SntpTm);

    printf("sntp time of day done. \n\r Seconds = %d \r\n",SntpTm.tv_sec);
    return A_OK;
}

static A_INT32 ip_sntp_modify_timezone_dse(A_INT32 argc, char *argv[])
{
    tSntpTime SntpTime ;
    A_UINT8 hour,min;
    A_UINT8 dse,add_sub;
    char hr[3],mn[3],parsing_hour_min[10];
    
     memset(&SntpTime,0,sizeof(tSntpTime));
    
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 5)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }

    if(strlen(argv[2]) > 9)
     {
       printf("Error : Invalid UTC string. Valid string(UTC+hour:min).Eg UTC+05:30\n\r");
       return A_ERROR;
     }
 
    if(strlen(argv[3]) > 3)
    {
	    printf("Error : Invalid string\n");
	    return A_ERROR;
    }
    if(strlen(argv[4]) > 7)
     {
        printf("Error : Invalid DSE string.Valid string DSE\n\r");
        return A_ERROR;
     }
 
     strcpy(parsing_hour_min,argv[2]);
 
     if(parsing_hour_min[3] == '+')
     {
                add_sub = 1; // time is to be added
     }
     else if(parsing_hour_min[3] == '-')
     {
                add_sub = 0; // time is to be subtracted
     }
     else{
                printf("Error : Only + / - operation is allowed\n\r");
                return A_ERROR;
      }
 
 
     hr[0]=parsing_hour_min[4];
     hr[1]=parsing_hour_min[5];
     hr[2]='\0';
 
     hour=strtol(hr,NULL,10);
 
     mn[0]=parsing_hour_min[7];
     mn[1]=parsing_hour_min[8];
     mn[2]='\0';
 
     min=strtol(mn,NULL,10);
     
     if(add_sub == 1)
     {	     
       if((hour == 14) && (min > 0))
       {
                printf("SNTP Error:Max hour limit is 14\n");
                return A_ERROR;
       }
       if(hour > 14)
       {
                printf("SNTP Error:Max hour limit is 14\n");
                return A_ERROR;
       }
     }
     if(add_sub ==0) 
     {	     
       if((hour == 12) && (min > 0))
       {
                printf("SNTP Error:Max hour limit is 12\n");
                return A_ERROR;
       }
       if(hour > 12) 
       {
                printf("SNTP Error:Max hour limit is 12\n");
                return A_ERROR;
       }
     }
     if(ATH_STRCMP(argv[4],"enable") == 0)
     {
               // printf("Enabling dse\n\r");
                dse = 1;
     }
     else if(ATH_STRCMP(argv[4],"disable") == 0){
               // printf("disabling dse\n\r");
                dse = 0;
     }
     else{
                printf("Invalid entry for DSE\n\r");
                return A_ERROR;
     }

    qcom_sntp_zone(hour,min,add_sub,dse);
    
    return A_OK;
}


static A_INT32 ip_query_sntp_server_address(A_UINT32 argc,char *argv[])
{
    A_UINT32    enet_device;  
    A_UINT8 i = 0;	
    tSntpDnsAddr SntpDnsAddr[MAX_SNTP_SERVERS];
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
   
    if(wifi_connected_flag == 0)
    {
       printf("device not connected \r\n");
       return A_ERROR;
    }   
    
    memset(&SntpDnsAddr[0],0,(sizeof(tSntpDnsAddr) * MAX_SNTP_SERVERS));
    
    enet_device = get_active_device();    
    qcom_sntp_query_srvr_address(enet_device, &SntpDnsAddr[0]);
 
    for(i = 0;i < MAX_SNTP_SERVERS;i++)
    {   
      
      if(SntpDnsAddr[i].resolve != DNS_NAME_NOT_SPECIFIED) 
      {
         printf("SNTP SERVER ADDRESS : %s\n",SntpDnsAddr[i].addr);
      }
    }

    return A_OK;
}

static A_INT32 ip_sntp_client_start_stop(A_UINT32 argc,char *argv[])
{
    A_INT32 command = -1;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 3)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }
    if(ATH_STRCMP(argv[2], "start") == 0) 
    {
        command = 1;
    }
    else if(ATH_STRCMP(argv[2], "stop") == 0)
    {
        command = 0;
    }
    else
    {
        printf("Supported commands: start, stop");
        return (A_ERROR);	
    }	    
    qcom_enable_sntp_client(command);
    return A_OK;
	
}
#endif /* ENABLE_SNTP_CLIENT */

#if ENABLE_SSL
static A_INT32 ssl_start(A_INT32 argc, char *argv[])
{
    SSL_INST *ssl;
    SSL_ROLE_T role;

    if(IS_DRIVER_READY != A_OK)
    {
        printf("Driver not loaded!\n");
        return A_ERROR;
    }

    if(argc < 3)
    {
       printf("ERROR: Incomplete params\n");
       return A_ERROR;
    }

    if (0 == strcmp(argv[2], "server"))
    {
        ssl = &ssl_inst[SSL_SERVER_INST];
        role = SSL_SERVER;
    }
    else if (0 == strcmp(argv[2], "client"))
    {
        ssl = &ssl_inst[SSL_CLIENT_INST];
        role = SSL_CLIENT;
    }
    else
    {
        printf("ERROR: Invalid parameter: %s\n", argv[2]);
        return A_ERROR;
    }

    if (ssl->sslCtx != NULL)
    {
        printf("ERROR: one SSL context only!\n");
        return A_ERROR;
    }

    // Create SSL context
    memset(ssl, 0, sizeof(SSL_INST));
    ssl->role = role;
    ssl->sslCtx = qcom_SSL_ctx_new(role, SSL_INBUF_SIZE, SSL_OUTBUF_SIZE, 0);
    if (ssl->sslCtx == NULL)
    {
        printf("ERROR: Unable to create SSL context\n");
        return A_ERROR;
    }

    // Reset config struct
    memset(&ssl->config, 0, sizeof(SSL_CONFIG));

    // Done
    printf("SSL %s started\n", argv[2]);
    return A_OK;
}

static A_INT32 ssl_stop(A_INT32 argc, char *argv[])
{
    SSL_INST *ssl;
    A_INT32 res = A_OK;
    SSL_ROLE_T role;

    if(IS_DRIVER_READY != A_OK)
    {
        printf("Driver not loaded!\n");
        return A_ERROR;
    }

    if(argc < 3)
    {
       printf("ERROR: Incomplete params\n");
       return A_ERROR;
    }

    if (0 == strcmp(argv[2], "server"))
    {
        ssl = &ssl_inst[SSL_SERVER_INST];
        role = SSL_SERVER;
    }
    else if (0 == strcmp(argv[2], "client"))
    {
        ssl = &ssl_inst[SSL_CLIENT_INST];
        role = SSL_CLIENT;
    }
    else
    {
        printf("ERROR: Invalid parameter: %s\n", argv[2]);
        return A_ERROR;
    }

    if (ssl->sslCtx == NULL || role != ssl->role)
    {
        printf("ERROR: SSL %s not started\n", argv[2]);
        return A_ERROR;
    }

    if (ssl->ssl)
    {
        qcom_SSL_shutdown(ssl->ssl);
        ssl->ssl = NULL;
    }

    if (ssl->sslCtx)
    {
        qcom_SSL_ctx_free(ssl->sslCtx);
        ssl->sslCtx = NULL;
    }

    printf("SSL %s stopped\n", argv[2]);
    return res;
}

static A_INT32 ssl_config(A_INT32 argc, char *argv[])
{
    SSL_INST *ssl;
    A_INT32 res;
    int argn, cipher_count = 0;

    if(IS_DRIVER_READY != A_OK)
    {
        printf("Driver not loaded!\n");
        return A_ERROR;
    }

    if(argc < 3)
    {
       printf("ERROR: Incomplete params\n");
    }

    if (0 == strcmp(argv[2], "server"))
    {
        ssl = &ssl_inst[SSL_SERVER_INST];
    }
    else if (0 == strcmp(argv[2], "client"))
    {
        ssl = &ssl_inst[SSL_CLIENT_INST];
    }
    else
    {
        printf("ERROR: Invalid parameter: %s\n", argv[2]);
        return A_ERROR;
    }

    if (ssl->sslCtx == NULL)
    {
        printf("ERROR: SSL %s not started\n", argv[2]);
        return A_ERROR;
    }

    argn = 3;
    ssl->config_set = 0;
    while (argn < argc)
    {
        if (argn == argc-1)
        {
            printf("ERROR: Incomplete params\n");
            return A_ERROR;
        }

        if (0 == strcmp("protocol", argv[argn]))
        {
            // Setting of protocol option is supported for SSL client only
            if (strcmp(argv[2], "client"))
            {
                printf("ERROR: Protocol option is not supported for %s\n", argv[2]);
                return A_ERROR;
            }
            argn++;
            if (0 == strcmp("SSL3", argv[argn]))
            {
                ssl->config.protocol = SSL_PROTOCOL_SSL_3_0;
            }
            else if (0 == strcmp("TLS1.0", argv[argn]))
            {
                ssl->config.protocol = SSL_PROTOCOL_TLS_1_0;
            }
            else if (0 == strcmp("TLS1.1", argv[argn]))
            {
                ssl->config.protocol = SSL_PROTOCOL_TLS_1_1;
            }
            else if (0 == strcmp("TLS1.2", argv[argn]))
            {
                ssl->config.protocol = SSL_PROTOCOL_TLS_1_2;
            }
            else
            {
                printf("ERROR: Invalid protocol: %s\n", argv[argn]);
                return A_ERROR;
            }
        }

        if (0 == strcmp("time", argv[argn]))
        {
            argn++;
            if (0 == strcmp("1", argv[argn]))
            {
                ssl->config.verify.timeValidity = 1;
            }
            else if (0 == strcmp("0", argv[argn]))
            {
                ssl->config.verify.timeValidity = 0;
            }
            else
            {
                printf("ERROR: Invalid option: %s\n", argv[argn]);
                return A_ERROR;
            }
        }

        if (0 == strcmp("alert", argv[argn]))
        {
            argn++;
            if (0 == strcmp("1", argv[argn]))
            {
                ssl->config.verify.sendAlert = 1;
            }
            else if (0 == strcmp("0", argv[argn]))
            {
                ssl->config.verify.sendAlert = 0;
            }
            else
            {
                printf("ERROR: Invalid option: %s\n", argv[argn]);
                return A_ERROR;
            }
        }

        if (0 == strcmp("domain", argv[argn]))
        {
            argn++;
            if (0 == strcmp("0", argv[argn]))
            {
                ssl->config.verify.domain = 0;
                ssl->config.matchName[0] = '\0';
            }
            else
            {
                ssl->config.verify.domain = 1;
                if (strlen(argv[argn]) >= sizeof(ssl->config.matchName))
                {
                    printf("ERROR: %s too long (max %d chars)\n", argv[argn], sizeof(ssl->config.matchName));
                    return A_ERROR;
                }
                strcpy(ssl->config.matchName, argv[argn]);
            }
        }

        if (0 == strcmp("cipher", argv[argn]))
        {
            argn++;
            if (cipher_count == 0)
            {
                memset(ssl->config.cipher, 0, sizeof(ssl->config.cipher));
            }
            if (cipher_count == SSL_CIPHERSUITE_LIST_DEPTH)
            {
                printf("ERROR: Too many cipher options (max %d)\n", argv[argn], SSL_CIPHERSUITE_LIST_DEPTH);
                return A_ERROR;
            }
            if (0 == strcmp("TLS_RSA_WITH_AES_256_GCM_SHA384", argv[argn]))
            {
                ssl->config.cipher[cipher_count] = TLS_RSA_WITH_AES_256_GCM_SHA384;
            }
            else if (0 == strcmp("TLS_RSA_WITH_AES_256_CBC_SHA256", argv[argn]))
            {
                ssl->config.cipher[cipher_count] = TLS_RSA_WITH_AES_256_CBC_SHA256;
            }
            else if (0 == strcmp("TLS_RSA_WITH_AES_256_CBC_SHA", argv[argn]))
            {
                ssl->config.cipher[cipher_count] = TLS_RSA_WITH_AES_256_CBC_SHA;
            }
            else if (0 == strcmp("TLS_RSA_WITH_AES_128_GCM_SHA256", argv[argn]))
            {
                ssl->config.cipher[cipher_count] = TLS_RSA_WITH_AES_128_GCM_SHA256;
            }
            else if (0 == strcmp("TLS_RSA_WITH_AES_128_CBC_SHA256", argv[argn]))
            {
                ssl->config.cipher[cipher_count] = TLS_RSA_WITH_AES_128_CBC_SHA256;
            }
            else if (0 == strcmp("TLS_RSA_WITH_AES_128_CBC_SHA", argv[argn]))
            {
                ssl->config.cipher[cipher_count] = TLS_RSA_WITH_AES_128_CBC_SHA;
            }
            else if (0 == strcmp("TLS_RSA_WITH_3DES_EDE_CBC_SHA", argv[argn]))
            {
                ssl->config.cipher[cipher_count] = TLS_RSA_WITH_3DES_EDE_CBC_SHA;
            }
            else
            {
                printf("ERROR: Invalid protocol: %s\n", argv[argn]);
                return A_ERROR;
            }
            cipher_count++;
        }

        argn++;
    }

    if (ssl->ssl == NULL)
    {
        // Create SSL connection object
        ssl->ssl = qcom_SSL_new(ssl->sslCtx);
        if (ssl->ssl == NULL)
        {
            printf("ERROR: SSL configure failed (Unable to create SSL context)\n");
            return A_ERROR;;
        }
    }

    // configure the SSL connection
    ssl->config_set = 1;
    res = qcom_SSL_configure(ssl->ssl, &ssl->config);
    if (res < A_OK)
    {
        printf("ERROR: SSL configure failed (%d)\n", res);
        return A_ERROR;
    }
    printf("SSL %s configuration changed\n", argv[2]);
    return A_OK;
}

static A_INT32 ssl_add_cert(A_INT32 argc,char *argv[])
{
    SSL_INST *ssl;
    SSL_CERT_TYPE_T type;
    char *name = NULL;

    if(IS_DRIVER_READY != A_OK)
    {
        printf("Driver not loaded!\n");
        return A_ERROR;
    }

    if(argc < 4)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }

    if (0 == strcmp(argv[2], "server"))
    {
        ssl = &ssl_inst[SSL_SERVER_INST];
        if (ssl->sslCtx == NULL)
        {
            printf("ERROR: SSL server not started\n");
            return A_ERROR;
        }
    }
    else if (0 == strcmp(argv[2], "client"))
    {
        ssl = &ssl_inst[SSL_CLIENT_INST];
        if (ssl->sslCtx == NULL)
        {
            printf("ERROR: SSL client not started\n");
            return A_ERROR;
        }
    }
    else
    {
        printf("ERROR: Invalid parameter: %s\n", argv[2]);
        return A_ERROR;
    }

    if (0 == strcmp("certificate", argv[3]))
    {
        type = SSL_CERTIFICATE;
    }
    else if (0 == strcmp("calist", argv[3]))
    {
        type = SSL_CA_LIST;
    }
    else
    {
        printf("ERROR: Invalid parameter: %s\n", argv[3]);
        return A_ERROR;
    }

    if (argc > 4)
    {
        name = argv[4];
    }

    // Load/add certificate
    if (name != NULL)
    {
        if (qcom_SSL_loadCert(ssl->sslCtx, type, name) < A_OK)
        {
            printf("ERROR: Unable to load %s from FLASH\n" , name);
            return A_ERROR;
        }
        printf("%s loaded from FLASH\n", name);
    }
    else
    {
        A_UINT8 *cert = ssl_cert_data_buf;
        A_UINT16 cert_len = ssl_cert_data_buf_len;
        if (type == SSL_CERTIFICATE)
        {
            if (cert_len == 0)
            {
                // Load the default certificate
                printf("Note: using the default certificate\n");
                cert = (A_UINT8*)ssl_default_cert;
                cert_len = ssl_default_cert_len;
            }
            if (qcom_SSL_addCert(ssl->sslCtx, cert, cert_len) < A_OK)
            {
                printf("ERROR: Unable to add certificate\n");
                return A_OK;
            }
            printf("Certificate added to SSL server\n");
        }
        else
        {
            if (cert_len == 0)
            {
                // Load the default CA list
                printf("Note: using the default CA list\n");
                cert = (A_UINT8*)ssl_default_calist;
                cert_len = ssl_default_calist_len;
            }
            if (qcom_SSL_setCaList(ssl->sslCtx, cert, cert_len)< A_OK)
            {
                printf("ERROR: Unable to set CA list\n");
                return A_ERROR;
            }
            printf("CA list added to SSL client\n");
        }
    }
    return A_OK;
}

static A_INT32 ssl_store_cert(A_INT32 argc,char *argv[])
{
    char *name;

    if(IS_DRIVER_READY != A_OK)
    {
        printf("Driver not loaded!\n");
        return A_ERROR;
    }

    if(argc < 3)
    {
       printf("Incomplete params\n");
       return A_ERROR;
    }
    name = argv[2];

    if (ssl_cert_data_buf_len == 0)
    {
        printf("ERROR: no certificate data.\nHint: Use the wmiconfig --ssl_get_cert to read a certificate from a certificate server.\n");
        return A_ERROR;
    }
    if (A_OK == qcom_SSL_storeCert(name, ssl_cert_data_buf, ssl_cert_data_buf_len))
    {
        printf("%s is stored in FLASH\n", name);
        return A_OK;
    }
    else
    {
        printf("ERROR: failed to store %s\n", name);
        return A_ERROR;
    }
}

static A_INT32 ssl_delete_cert(A_INT32 argc,char *argv[])
{
    char *name;

    if(IS_DRIVER_READY != A_OK)
    {
        printf("Driver not loaded!\n");
        return A_ERROR;
    }

    if(argc < 3)
    {
        ssl_cert_data_buf_len = 0;
        printf("Deleted the certificate data stored in RAM.\n");
        return A_OK;
    }
    name = argv[2];

    if (A_OK == qcom_SSL_storeCert(name, NULL, 0))
    {
        printf("Deleted %s from FLASH\n", name);
        return A_OK;
    }
    else
    {
        printf("ERROR: failed to delete %s\n", name);
        return A_ERROR;
    }
}

static A_INT32 ssl_list_cert(A_INT32 argc,char *argv[])
{
    SSL_FILE_NAME_LIST *fileNames;
    A_INT32 i, numFiles, namesLen = sizeof(SSL_FILE_NAME_LIST); // 10 files

    if(IS_DRIVER_READY != A_OK)
    {
        printf("Driver not loaded!\n");
        return A_ERROR;
    }

    fileNames = A_MALLOC(namesLen, MALLOC_ID_TEMPORARY);
    if (fileNames == NULL)
    {
        printf("ERROR: not enough memory\n");
        return A_ERROR;
    }

    numFiles = SSL_listCert(fileNames);
    if (numFiles < 0)
    {
        printf("ERROR: failed to list files (%d)\n", numFiles);
        A_FREE(fileNames, MALLOC_ID_TEMPORARY);
        return A_ERROR;
    }

    printf("%d %s stored in FLASH\n", numFiles, numFiles == 1 ? "file" : "files");
    for (i=0; i<numFiles; i++)
    {
        char str[21];
        if (fileNames->name[i][0])
        {
            strncpy(str, (char*)fileNames->name[i], sizeof(str)-1);
            str[sizeof(str)-1] = '\0';
            printf("%s\n", str);
        }
    }
    return A_OK;
}
#endif // ENABLE_SSL

static A_INT32 ota_upgrade(A_INT32 argc, char *argv[])
{
    A_UINT32 addr;
    A_UINT32 sbits;
    A_CHAR filename[128];
    A_UINT8 mode;
    A_UINT8 preserve_last;
    A_UINT8 protocol;
    int error;
    tOtaUpgradeResp resp;
    A_UINT32    enet_device;
    
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    if(argc < 6)
    {
       printf("Incomplete params\n");
       printf("Usage: wmiconfig --ota_upgrade <ip> <filename> <mode> <preseve_last> <protocol> \n");
       return A_ERROR;
    }
    
    error=parse_ipv4_ad((unsigned long *)&addr, &sbits, argv[2]);
    if(error!=0)
    {
        printf("Invalid IP address\n");
	return error;
    }
    addr = A_BE2CPU32(addr);
	
    if(strlen(argv[3]) > 128)
    {
	    printf("Error : Invalid Filename Length\n");
	    return A_ERROR;
    }
	memset(filename,'\0',strlen(argv[3])+1);
	strncpy(filename,argv[3],strlen(argv[3]));
          
	mode=strtol(argv[4],NULL,10);

    if(mode > 2)
    {
        printf("Error : Invalid Mode [valid values are 0 or 1 ]\n\r");
        return A_ERROR;
    }
 
    preserve_last=strtol(argv[5],NULL,10);
	
    if(preserve_last > 2)
     {
        printf("Error : Invalid preserve_last [valid values are 0 or 1 ]\n\r");
        return A_ERROR;
     }
	protocol=strtol(argv[6],NULL,10);
	
    if(protocol > 2)
     {
        printf("Error : Invalid protocol [valid values are 0 or 1 ]\n\r");
        return A_ERROR;
     }
    
     enet_device = get_active_device();
    
    if (A_OK != qcom_ota_upgrade(enet_device,addr,filename,mode,preserve_last,protocol,&(resp.resp_code),&(resp.size)))
    {
        printf("OTA Download Failed\r\n");
        return A_ERROR;
    }
    
    if(resp.resp_code==0){
       printf("OTA Image downloaded ,Image Size:%d\n",resp.size);
    }else{
       printf("OTA Download Failed ERR:%d\n",resp.resp_code);
    }
	
    return A_OK;
}


static A_INT32 ota_read(A_INT32 argc, char *argv[]){
    A_UINT32 offset;
    A_UINT32 len;
    A_UCHAR * databuffer;
    A_UINT32 size;
    
    
    
    if(argc < 3)
    {
        printf(" Incomplete Params \n Usage: wmiconfig --ota_read <offset> <size>!\n");
        printf("e.g wmiconfig --ota_read 100 20 to read 20 bytes from and offset 100 w.r.t start of the image!\n");
        return A_ERROR;
    }
    
    databuffer=A_MALLOC(sizeof(A_UCHAR)*MAX_OTA_AREA_READ_SIZE,MALLOC_ID_TEMPORARY);
    
    offset = atoi(argv[2]);
    
    len = atoi(argv[3]);
    
    
    
    if (A_OK != qcom_ota_read_area(offset,len,databuffer,&size))
    {
        printf("OTA Download Failed\r\n");
        A_FREE(databuffer,MALLOC_ID_TEMPORARY);
        return A_ERROR;
    }else{
      
          printf("Read %d bytes from OTA Area\n", size);      
          A_UINT32 index;
          for(index=0 ; index<size ; index++)
          {
           printf("%02x ", databuffer[index]);
            if(!((index+1) % 16))
            {
              printf("\n");
            }
          }
       
    }
    A_FREE(databuffer,MALLOC_ID_TEMPORARY);
    return A_OK;
}

static A_INT32 ota_done(){

    
    if (A_OK != qcom_ota_done())
    {
        printf("OTA Download Complete Command Failed\r\n");
        return A_ERROR;
    }
    else
    {
	printf("OTA Download Completed Successfully \r\n");
    }
    
    return A_OK;
}
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : qcom_get_temperature()
* Parames:      
*       regval  ---  register value for KingFisher
*       tempval ---  physical temperature value after caculation 
* Returned Value  : A_OK - on successful completion
*					A_ERROR - on any failure. * Comments        : Sample function to depict IPv6 ping functionality
*                   Not supported on RTCS
*END*-----------------------------------------------------------------*/
int qcom_get_temperature(A_UINT32 * regval,float * tempval)
{
      ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_INT32 error;
    WMI_GET_TEMPERATURE_REPLY temperature;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK) {
        return A_ERROR;
    }
	
    inout_param.cmd_id = ATH_GET_TEMPERATURE;
    inout_param.data = &temperature;
    //inout_param.length = 1;

    error = HANDLE_IOCTL (&inout_param);
    if (A_OK != error)
    {
	printf(" Get temperature Failed \n");
	return error;
    }
    *regval = temperature.tempRegVal  ;
    *tempval = ((float)temperature.tempDegree) /10 ;
    return A_OK;
}
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : qcom_roaming_ctrl()
* Parames:      
* Returned Value  : A_OK - on successful completion
*					A_ERROR - on any failure. * Comments        : Sample function to depict IPv6 ping functionality
*                   Not supported on RTCS
*END*-----------------------------------------------------------------*/
int qcom_roaming_ctrl(WMI_SET_ROAM_CTRL_CMD * roam_ctrl)
{
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_INT32 error;
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK) {
        return A_ERROR;
    }
    inout_param.cmd_id = ATH_ROAM_CTRL;
    inout_param.data = &roam_ctrl;
    error = HANDLE_IOCTL (&inout_param);
    if (A_OK != error)
    {
	printf(" roam ctrl Failed \n");
	return error;
    }    

    return A_OK;
}
/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : qcom_get_wps_net_info()
* Parames:      
*        void
* Returned Value  : ATH_NETPARAMS * - the structure point to netinfo
*		    NULL - on any failure. 
* Comments        : 
*                   Not supported on RTCS
*END*-----------------------------------------------------------------*/
ATH_NETPARAMS * qcom_get_wps_net_info(void)
{
    ATH_NETPARAMS *pNetparams = &wps_context.netparams;
    if(pNetparams->error)
      return NULL;
    else
      return pNetparams;
}
#if ENABLE_HTTP_CLIENT
/*
Command will be
httpc get 192.1.168.1.10 /index.html 
*/
/************************************************************************
* NAME: httpc_get_page
*
* DESCRIPTION:
************************************************************************/
HTTPC_PARAMS      httpc; // Since stack size is less

A_INT32 httpc_command_parser(A_INT32 argc, char_ptr argv[] )
{ /* Body */
    A_INT32           return_code = A_OK;
    	
    if (argc < 3)
    {
        /*Incorrect number of params, exit*/
        return_code = A_ERROR;
    }
    else
    {
        memset ((void *)&httpc, 0, sizeof(HTTPC_PARAMS));
        
        if(strlen((char *)httpc.url) >= 64)
        {
            printf("Maximum 64 bytes supported as argument\n");
            return A_ERROR;
        }
        
        if (argc >= 4)
	{	
            strcpy((char *)httpc.url, argv[3]);
	}    
        
        if (argc >= 5)
	{	
            strcpy((char *)httpc.data, argv[4]);
	}    
        
        if(ATH_STRCMP(argv[2], "connect") == 0)
        {
            httpc.command = HTTPC_CONNECT_CMD;
			
			if(strncmp(argv[3], "https://", 8) == 0)
			{
				int size = sizeof(httpc.url) - 8;
				memmove(httpc.url, httpc.url + 8, size);
				httpc.command = HTTPC_CONNECT_SSL_CMD;
			}
			else if(strncmp(argv[3], "http://", 7) == 0)
			{
				int size = sizeof(httpc.url) - 7;
				memmove(httpc.url, httpc.url + 7, size);
			}
			
            if(argc >= 4)  
                httpc_connect(&httpc);
            else
                return_code = A_ERROR;
        }
        else if(ATH_STRCMP(argv[2], "get") == 0)
        {
            httpc.command = HTTPC_GET_CMD;
            if(argc >= 4)  
                httpc_method(&httpc);
            else
                return_code = A_ERROR;
        }
        else if(ATH_STRCMP(argv[2], "post") == 0)
        {
            httpc.command = HTTPC_POST_CMD;
            if(argc >= 4)  
                httpc_method(&httpc);
            else
                return_code = A_ERROR;
        }
        else if(ATH_STRCMP(argv[2], "query") == 0)
        {
            httpc.command = HTTPC_DATA_CMD;
            if(argc >= 5)  
                httpc_method(&httpc);
            else
                return_code = A_ERROR;
        }
        else if(ATH_STRCMP(argv[2], "disc") == 0)
        {
            httpc.command = HTTPC_DISCONNECT_CMD;  
            httpc_connect(&httpc);
        }
        else
        {
            printf("Unknown Command \"%s\"\n", argv[2]);
            return_code = A_ERROR;
        }
    }
    if (return_code == A_ERROR)
    {
        printf ("USAGE: wmiconfig --ip_http_client [<connect/get/post/query> <data1> <data2>]\n");
    }
    return return_code;
} /* Endbody */

/************************************************************************
* NAME: httpc_connect
*
* DESCRIPTION: Process a HTTP connect request.
************************************************************************/

static void httpc_connect(HTTPC_PARAMS *p_httpc)
{
    int error = A_OK;
     
    //printf("Sedning command %s %s\n", p_httpc->url, p_httpc->data);
    error = qcom_http_client_method(p_httpc->command, p_httpc->url, p_httpc->data, NULL);
    
    printf("HTTPClient cmd %s\n", (error == A_ERROR)?"failed":"succeeded");
}

#define MAX_HTTP_BUF_SIZE 1500
/************************************************************************
* NAME: httpc_method
*
* DESCRIPTION: Process a HTTP connect request.
*              For request, output will come as last argument
*              Application must free the buffer using zero_copy_http_free
************************************************************************/
static void httpc_method(HTTPC_PARAMS *p_httpc)
{
    A_UINT8   *output = NULL;
    A_UINT32  error = A_OK;
    HTTPC_RESPONSE *temp = NULL;
    int i;
    
    output = malloc(MAX_HTTP_BUF_SIZE);
    error = qcom_http_client_method( p_httpc->command, p_httpc->url, p_httpc->data, output);
    
    if(error != A_OK)
        printf("HTTPC Command failed\n");
    else if(output != NULL)
    {
        temp = (HTTPC_RESPONSE *)output;
        
        /* For GET and POST alone, print the output */
        if ((p_httpc->command == 1) || (p_httpc->command == 2))
        {
            printf("Size:%u Resp_code:%u\n\n", temp->length, temp->resp_code);
            //printf("Flags %u Packet: %u\n", temp->flags, strlen(temp->data));
            if (temp->length)
            {
                printf("Packet:\n");
                for(i = 0; i < temp->length; i++)
                    printf("%c", temp->data[i]);
            }
        }
        free(output);
        //zero_copy_http_free((A_VOID *)output);
    }
    
    return;
}


#endif /* ENABLE_HTTP_CLIENT */


#if ENABLE_ROUTING_CMDS
/************************************************************************
* NAME: ipv4_route
*
* DESCRIPTION: Add, delete or show IPv4 route table
*               wmiconfig --ipv4route add <addr> <mask> <gw>
*               wmiconfig --ipv4route del <addr> <mask> <ifindex>
*               wmiconfig --ipv4route show
************************************************************************/
A_INT32 ipv4_route(A_INT32 argc, char_ptr argv[])
{ /* Body */
    A_INT32         error = A_OK;
    A_UINT32        ifIndex, addr, mask, gw;
    A_UINT32        sbits;
    A_UINT32        command;
    IPV4_ROUTE_LIST_T routelist;
    A_UINT32 enet_device = get_active_device();
    
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
    do 
    {
        if(argc < 3)
        {
            printf("incomplete params\n");
            error = A_ERROR;
            break;
        }
        
        if(ATH_STRCMP(argv[2], "show") != 0)
        {
            error=parse_ipv4_ad((unsigned long *)&addr, &sbits, argv[3]);
            if(error!=0)
            {
                printf("Invalid IP address\n");
                error = A_ERROR;
                break;
            }
            addr = A_BE2CPU32(addr);
            error=parse_ipv4_ad((unsigned long *)&mask, &sbits, argv[4]);
            if(error!=0)
            {
                printf("Invalid Mask address\n");
                error = A_ERROR;
                break;
            }
            mask = A_BE2CPU32(mask);
        }
        if(ATH_STRCMP(argv[2], "add") == 0)
        {
            command = ROUTE_ADD;
            error=parse_ipv4_ad((unsigned long *)&gw, &sbits, argv[5]);
            if(error!=0)
            {
                printf("Invalid Gateway address\n");
                error = A_ERROR;
                break;
            }
            gw = A_BE2CPU32(gw);
        }
        else if(ATH_STRCMP(argv[2], "del") == 0)
        {
            command = ROUTE_DEL;
            ifIndex = atoi(argv[5]);
        }
        else if(ATH_STRCMP(argv[2], "show") == 0)
        {
            command = ROUTE_SHOW;
        }
        else
        {
            printf("Unknown Command \"%s\"\n", argv[2]);
            error = A_ERROR;
            break;
        }
        
        if(A_OK == qcom_ip4_route(enet_device, command, &addr, &mask, &gw, &ifIndex, &routelist))
        {
            if(command == ROUTE_SHOW)
            {
                int i = 0;
                char *ip_str = A_MALLOC(sizeof(A_UINT8) * 48, MALLOC_ID_TEMPORARY);
                if (NULL == ip_str)
                {
                    printf("Out of Memory error\n");
                }
                else
                {
                    printf("\n..IPaddr.......mask.........nexthop...iface..\n");
                    for(i = 0; i < routelist.rtcount; i++)
                    {
                        printf("%s  %s  %s %d \n",inet_ntoa(*(A_UINT32 *)(&routelist.route[i].address), (char *)&ip_str[0]) ,
                               inet_ntoa(*(A_UINT32 *)(&routelist.route[i].mask), (char *)&ip_str[16]), 
                               inet_ntoa(*(A_UINT32 *)(&routelist.route[i].gateway), (char *)&ip_str[32]),
                               routelist.route[i].ifIndex);
                    }
                }
            }
        }
        else
        {
            printf("Route %s failed\n", (command == 0)?"addition":((command ==1)?"deletion":"show"));
        }
    }while(0);
    if (error == A_ERROR)
    {
        printf ("USAGE: wmiconfig --ipv4_route add <address> <mask> <gw>\n");
        printf ("       wmiconfig --ipv4_route del <address> <mask> <ifIndex>\n");
        printf ("       wmiconfig --ipv4_route show\n");
    }
    return error;
} /* Endbody */

/************************************************************************
* NAME: ipv6_route
*
* DESCRIPTION: Add, delete or show IPv4 route table
*               wmiconfig --ipv4route add <addr> <prfx len> <gw>
*               wmiconfig --ipv4route del <addr> <ifindex>
*               wmiconfig --ipv4route show
************************************************************************/
A_INT32 ipv6_route(A_INT32 argc, char_ptr argv[])
{ /* Body */
    A_INT32         error = A_OK;
    A_UINT32        ifIndex, prefixLen;
    A_UINT8         ip6addr[16];
    A_UINT8         gateway[16];
    A_UINT32        command;
    IPV6_ROUTE_LIST_T routelist;
    A_UINT32 enet_device = get_active_device();
    
    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }
    do 
    {
        if(argc < 3)
        {
            printf("incomplete params\n");
            error = A_ERROR;
            break;
        }
        
        if(ATH_STRCMP(argv[2], "show") != 0)
        {
            error=Inet6Pton(argv[3],ip6addr);
            if(error!=0)
            {
                printf("Invalid IP address\n");
                error = A_ERROR;
                break;
            }
            
            prefixLen = atoi(argv[4]);
        }
        if(ATH_STRCMP(argv[2], "add") == 0)
        {
            command = ROUTE_ADD;
            ifIndex = atoi(argv[6]);
            
            error=Inet6Pton(argv[5],gateway);
            if(error!=0)
            {
                printf("Invalid Gateway address\n");
                error = A_ERROR;
                break;
            }
        }
        else if(ATH_STRCMP(argv[2], "del") == 0)
        {
            command = ROUTE_DEL;
            ifIndex = atoi(argv[5]);
        }
        else if(ATH_STRCMP(argv[2], "show") == 0)
        {
            command = ROUTE_SHOW;
        }
        else
        {
            printf("Unknown Command \"%s\"\n", argv[2]);
            error = A_ERROR;
            break;
        }
        
        if(A_OK == qcom_ip6_route(enet_device, command, ip6addr, &prefixLen, gateway, &ifIndex, &routelist))
        {
            if(command == ROUTE_SHOW)
            {
                int i = 0;
                char *ip_str = A_MALLOC(sizeof(A_UINT8) * 48, MALLOC_ID_TEMPORARY);
                if (NULL == ip_str)
                {
                    printf("Out of Memory error\n");
                }
                else
                {
                    printf("\n..IPaddr.......prefixLen.........nexthop...iface..\n");
                    for(i = 0; i < routelist.rtcount; i++)
                    {
                        printf("%s  %d ", inet6_ntoa((char *)(&routelist.route[i].address), (char *)ip_str),
                               routelist.route[i].prefixlen);
                        printf("%s  %d\n", inet6_ntoa((char *)(&routelist.route[i].nexthop), (char *)ip_str),
                               routelist.route[i].ifindex);
                    }
                }
            }
        }
        else
        {
            printf("Route %s failed\n", (command == 0)?"addition":((command ==1)?"deletion":"show"));
        }
    }while(0);
    
    if (error == A_ERROR)
    {
        printf ("USAGE: wmiconfig --ipv6_route add <address> <prefixLen> <gw>\n");
        printf ("       wmiconfig --ipv6_route del <address> <prefixLen> <ifIndex>\n");
        printf ("       wmiconfig --ipv6_route show\n");
    } 
    return error;
} /* Endbody */

#endif /* ENABLE_ROUTING_CMDS */
#endif /* ENABLE_STACK_OFFLOAD */
static A_INT32 get_temperature(A_INT32 argc,char *argv[])
{
    A_UINT32 temp_reg =0;
    float   temp_val = 0;
    qcom_get_temperature(&temp_reg,&temp_val);
    printf("\nRegVal = %x  Degree = %f deg C\n",temp_reg,temp_val );
    return A_OK;
}
static A_INT32 ath_roam_ctrl(A_INT32 argc,char *argv[])
{
    A_INT32 enable_val =0;
    WMI_SET_ROAM_CTRL_CMD   roam_cmd;
    
    A_MEMZERO(&roam_cmd, sizeof(WMI_SET_ROAM_CTRL_CMD));    
    roam_cmd.roamCtrlType = 2 ;

    if(ATH_STRCMP(argv[2], "enable") == 0)
    {
      roam_cmd.roamCtrlType = 3;//enable roam
      //todo add more 
    }else   if(ATH_STRCMP(argv[2], "disable") == 0)
    {
      roam_cmd.roamCtrlType = 1;//enable roam

    }
    qcom_roaming_ctrl(&roam_cmd);
    return A_OK;
}
/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : set_gtx
* Returned Value : A_OK
* Comments       : Enable/Disable GTX
*
*END------------------------------------------------------------------*/
static A_INT32 set_gtx(char* gtx)
{
    A_UINT8 b_gtx = atoi(gtx);
    ATH_IOCTL_PARAM_STRUCT  inout_param;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    inout_param.cmd_id = ATH_ONOFF_GTX;
    inout_param.data   = &b_gtx;
    inout_param.length = sizeof (A_BOOL);

    return HANDLE_IOCTL (&inout_param);
}

/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : set_lpl
* Returned Value : A_OK
* Comments       : Enable/Disable LPL
*
*END------------------------------------------------------------------*/
static A_INT32 set_lpl(char* lpl)
{
    A_UINT8 b_lpl = atoi(lpl);
    ATH_IOCTL_PARAM_STRUCT  inout_param;

    /*Check if driver is loaded*/
    if(IS_DRIVER_READY != A_OK){
        return A_ERROR;
    }

    inout_param.cmd_id = ATH_ONOFF_LPL;
    inout_param.data   = &b_lpl;
    inout_param.length = sizeof (A_BOOL);

    return HANDLE_IOCTL (&inout_param);
}

/*FUNCTION*-----------------------------------------------------------
*
* Function Name  : get_active_device
* Returned Value : A_OK
* Comments       : Return the current active device in the application
*
*END------------------------------------------------------------------*/

A_INT32 get_active_device()
{
    return active_device;
}


static A_INT32 mac_store(char *argv[])
{
    ATH_IOCTL_PARAM_STRUCT ath_param;       
    A_UINT8 val;
   	A_INT32 j; 
   	ATH_PROGRAM_MAC_ADDR_PARAM data;
    
	ath_param.cmd_id = ATH_PROGRAM_MAC_ADDR;
    ath_param.length = sizeof(data);
	/* get mac address from argv */
	if(strlen(argv[0]) != 12)
	{
		printf("Invalid MAC address length\n");
		return A_ERROR;
	}

	for(j=0;j<6;j++)
	{
		data.addr[j] = 0;
	}
	for(j=0;j<strlen(argv[0]);j++)
	{
		val = ascii_to_hex(argv[0][j]);
		if(val == 0xff)
		{
			printf("Invalid character\n");
			return A_ERROR;
		}
		else
		{
			if((j&1) == 0)
				val <<= 4;

			data.addr[j>>1] |= val;
		}
	}
	
		    
    ath_param.data = (A_UINT8*)&data;
    
    if( HANDLE_IOCTL (&ath_param) != A_OK){
    	printf("MAC PROGRAM ERROR: unknown driver error\n");
    	return A_ERROR;	
    }else{
    	/* macaddr[0] contains result code */
    	switch(data.result)
    	{
    	case ATH_PROGRAM_MAC_RESULT_DRIVER_FAILED:
    		printf("MAC PROGRAM ERROR: the driver was unable to complete the request.\n");
    	break;
    	case ATH_PROGRAM_MAC_RESULT_SUCCESS:
    		printf("MAC PROGRAM SUCCESS.\n");
    	break;
    	case ATH_PROGRAM_MAC_RESULT_DEV_DENIED:
    		printf("MAC PROGRAM ERROR: the firmware failed to program the mac address.\n");
    		printf(" possibly the same MAC address is already programmed.\n");
    	break;
    	case ATH_PROGRAM_MAC_RESULT_DEV_FAILED:
    	default:
    		printf("MAC PROGRAM ERROR: Device unknown failure.\n");
    	break;
    	}
    }
            
    return A_OK;
                
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : gpio_handler()
* Returned Value  : 1 - successful completion or
*                   0 - failed.
* Comments        : Handles GPIO related functionality
*
*END*-----------------------------------------------------------------*/
static A_INT32 gpio_handle(A_INT32 argc,char *argv[])
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    A_INT32 error;
    A_INT32 index = 2;
 
    if(ATH_STRCMP(argv[index],"output_set") == 0)
    {
        struct WMIX_GPIO_OUTPUT_SET {
            WMIX_CMD_HDR cmd;
            WMIX_GPIO_OUTPUT_SET_CMD setting;
        } gpio_output_set;

        if(argc != 7) {
            printf("\n Incomplete parameter list for gpio output_set \n");
            printf("\n Usage : wmiconfig --gpio output_set  <set_mask> <clear_mask> <enable_mask> <disable_mask>\n");
            return A_ERROR;
        }

        gpio_output_set.cmd.commandId = WMIX_GPIO_OUTPUT_SET_CMDID;
        gpio_output_set.setting.set_mask = strtol(argv[3], NULL, 16);
        gpio_output_set.setting.clear_mask = strtol(argv[4], NULL, 16);
        gpio_output_set.setting.enable_mask = strtol(argv[5], NULL, 16);
        gpio_output_set.setting.disable_mask = strtol(argv[6], NULL, 16);
    
        inout_param.cmd_id = ATH_GPIO_CMD;
        inout_param.data = (void *)&gpio_output_set;
        inout_param.length = sizeof(gpio_output_set);

        error = HANDLE_IOCTL(&inout_param);
        return error;
    }
    else if(ATH_STRCMP(argv[index],"input_get") == 0)
    {
        WMIX_CMD_HDR cmd;
        cmd.commandId = WMIX_GPIO_INPUT_GET_CMDID;
   
        inout_param.cmd_id = ATH_GPIO_CMD;
        inout_param.data = (void *)&cmd;
        inout_param.length = sizeof(cmd);

        error = HANDLE_IOCTL(&inout_param);
        return error;
    }
    else if(ATH_STRCMP(argv[index],"register_set") == 0)
    {
        struct WMIX_GPIO_REGISTER_SET {
            WMIX_CMD_HDR cmd;
            WMIX_GPIO_REGISTER_SET_CMD setting;
        } gpio_register_set;

        if(argc != 5) {
            printf("\n Incomplete parameter list for gpio output_set \n");
            printf("\n Usage : wmiconfig --gpio register_set  <gpioreg_id:hex> <value:hex>\n");
            return A_ERROR;
        }

        gpio_register_set.cmd.commandId =WMIX_GPIO_REGISTER_SET_CMDID;
        gpio_register_set.setting.gpioreg_id = strtol(argv[3], NULL, 16);
        gpio_register_set.setting.value = strtol(argv[4], NULL, 16);
    
        inout_param.cmd_id = ATH_GPIO_CMD;
        inout_param.data = (void *)&gpio_register_set;
        inout_param.length = sizeof(gpio_register_set);

        error = HANDLE_IOCTL(&inout_param);
        return error;      
    }
    else if(ATH_STRCMP(argv[index],"register_get") == 0)
    {
        struct WMIX_GPIO_REGISTER_GET {
            WMIX_CMD_HDR cmd;
            WMIX_GPIO_REGISTER_GET_CMD setting;
        } gpio_register_get;

        if(argc != 4) {
            printf("\n Incomplete parameter list for gpio output_set \n");
            printf("\n Usage : wmiconfig --gpio register_get  <gpioreg_id:hex>\n");
            return A_ERROR;
        }

        gpio_register_get.cmd.commandId =WMIX_GPIO_REGISTER_GET_CMDID;
        gpio_register_get.setting.gpioreg_id = strtol(argv[3], NULL, 16);
    
        inout_param.cmd_id = ATH_GPIO_CMD;
        inout_param.data = (void *)&gpio_register_get;
        inout_param.length = sizeof(gpio_register_get);

        error = HANDLE_IOCTL(&inout_param);
        return error;     
    }
    else
    {
        printf("\n Incorrect Incomplete parameter list \n");
        return SHELL_EXIT_ERROR;
    }
}

#if ENABLE_KF_PERFORMANCE


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : pfm_handler()
* Returned Value  : 1 - successful completion or
*                   0 - failed.
* Comments        : Handles performance measure related functionality
*
*END*-----------------------------------------------------------------*/
static A_INT32 pfm_handler(A_INT32 argc,char *argv[])
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    A_INT32 error;
    ATH_PFM_PARAM  param;
 
    clear_kf_profile();
   
    inout_param.cmd_id = ATH_PFM_CMD;
    inout_param.data = (void *)&param;
    inout_param.length = sizeof(param);

    error = HANDLE_IOCTL(&inout_param);

//	pDCxt->pfmDone = A_FALSE;
/* block until scan completes */	
//   	CUSTOM_DRIVER_WAIT_FOR_CONDITION(pCxt, &(pDCxt->pfmDone), A_TRUE, 5000);
	output_kf_pfm ();

   	return  error;
}

static A_INT32 diag_handler(A_INT32 argc,char *argv[])
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    A_INT32 error;
    struct diag_data {
        WMIX_CMD_HDR cmd;
		WMID_EVENT_SET_CMD   setting;
    }  data;

//    data.cmd.commandId = WMID_FSM_EVENT_CMDID; 
    data.setting.enable = 1;
   
    inout_param.cmd_id = ATH_DIAG_CMD;
    inout_param.data = (void *)&data;
    inout_param.length = sizeof(data);

    error = HANDLE_IOCTL(&inout_param);

//	pDCxt->pfmDone = A_FALSE;
   			/* block until scan completes */	
//   	CUSTOM_DRIVER_WAIT_FOR_CONDITION(pCxt, &(pDCxt->pfmDone), A_TRUE, 5000);
   			            	
   	return  error;
}

A_STATUS ath_setpmu(int argc, char* argv[])
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    ATH_PMU_PARAM  param;
   
    if(argc < 3){
       return A_ERROR;
    }
    if(strcmp(argv[2],"timer") == 0) {
	   param.options = WAKE_MGR_WAKE_EVENT_TIMER;
	   param.wake_msec = atoi(argv[3]);
    }
	else if (strcmp(argv[2],"gpio30") == 0)
    {
	   if (atoi(argv[3]) != 0)
	      param.options = WAKE_MGR_WAKE_EVENT_GPIO30_HIGH;
	   else
		  param.options = WAKE_MGR_WAKE_EVENT_GPIO30_LOW;
	}
	else if (strcmp(argv[2],"gpio31") == 0)
    {
	   if (atoi(argv[3]) != 0)
	      param.options = WAKE_MGR_WAKE_EVENT_GPIO31_HIGH;
       else
	   	  param.options = WAKE_MGR_WAKE_EVENT_GPIO31_LOW;
	}
   
    inout_param.cmd_id = ATH_PMU_SET_PARAMS;
    inout_param.data = &param;
    inout_param.length = sizeof(ATH_PMU_PARAM);

    return HANDLE_IOCTL(&inout_param);
}
#endif

A_STATUS ath_dset(int argc, char* argv[])
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    ATH_DSET_PARAM  param;
   
    if(argc < 5){
       return A_ERROR;
    }
    if(strcmp(argv[2],"write") == 0) {
	   param.dset_id = atoi(argv[3]);
	   param.offset = atoi(argv[4]);
	   param.length = atoi(argv[5]);
       inout_param.cmd_id = ATH_DSET_WRITE_CMD;
    }
	else if (strcmp(argv[2],"read") == 0)
    {
	   param.dset_id = atoi(argv[3]);
	   param.offset = atoi(argv[4]);
	   param.length = atoi(argv[5]);
       inout_param.cmd_id = ATH_DSET_READ_CMD;
	}
   
    inout_param.data = &param;
    inout_param.length = sizeof(param);

    return HANDLE_IOCTL(&inout_param);
}

uint_32 remote_dset_op(DSET_OP op, HOST_DSET_HANDLE *pDsetHandle)
{
    ATH_IOCTL_PARAM_STRUCT inout_param;

	struct WMIX_GPIO_REGISTER_SET {
        WMIX_DSET_CMD_HDR cmd;
        union {
			WMIX_DSET_OP_CREATE_PARAM_CMD op_create;
			WMIX_DSET_OP_OPEN_PARAM_CMD   op_open;
			WMIX_DSET_OP_WRITE_PARAM_CMD  op_write;
			WMIX_DSET_OP_READ_PARAM_CMD  op_read;
			WMIX_DSET_OP_COMMIT_PARAM_CMD  op_commit;
			WMIX_DSET_OP_CLOSE_PARAM_CMD   op_close;
			WMIX_DSET_OP_DELETE_PARAM_CMD   op_delete;
		} u;
	} dset_op;


    pDsetHandle->done_flag = 0;

	switch (op)
	{
	case	DSET_OP_CREATE:
		dset_op.cmd.commandId = WMIX_DSET_CREATE_CMDID;

		dset_op.u.op_create.dset_id = pDsetHandle->dset_id;
		dset_op.u.op_create.flags = pDsetHandle->flags | pDsetHandle->media_id;
		dset_op.u.op_create.length = pDsetHandle->length;

		inout_param.data = (void *)&dset_op;
		inout_param.length = sizeof(dset_op);

		break;

	case	DSET_OP_OPEN:
		dset_op.cmd.commandId = WMIX_DSET_OPEN_CMDID;
		dset_op.u.op_open.dset_id = pDsetHandle->dset_id;
		dset_op.u.op_open.flags = pDsetHandle->flags;

		inout_param.data = (void *)&dset_op;
		inout_param.length = sizeof(WMIX_DSET_CMD_HDR) + sizeof(WMIX_DSET_OP_OPEN_PARAM_CMD);
		break;

	case	DSET_OP_READ:
		dset_op.cmd.commandId = WMIX_DSET_READ_CMDID;
		dset_op.u.op_read.dset_id = pDsetHandle->dset_id;
		dset_op.u.op_read.offset = pDsetHandle->offset;
		dset_op.u.op_read.length = pDsetHandle->length;
		pDsetHandle->left_len = pDsetHandle->length;

		inout_param.data = (void *)&dset_op;
		inout_param.length = sizeof(WMIX_DSET_CMD_HDR) + sizeof(WMIX_DSET_OP_READ_PARAM_CMD);
		break;
	case	DSET_OP_WRITE:
	{
		WMIX_DSET_CMD_HDR            *pCmd = (WMIX_DSET_CMD_HDR *)pDsetHandle->data_ptr;

		WMIX_DSET_OP_WRITE_PARAM_CMD *pDsetWrite = (WMIX_DSET_OP_WRITE_PARAM_CMD *)(pDsetHandle->data_ptr+sizeof(WMIX_DSET_CMD_HDR));

		pCmd->commandId = WMIX_DSET_WRITE_CMDID;

		pDsetWrite->dset_id = pDsetHandle->dset_id;
		pDsetWrite->flags = pDsetHandle->flags;
		pDsetWrite->offset = pDsetHandle->offset;
		pDsetWrite->length = pDsetHandle->length;

		pDsetHandle->left_len = pDsetHandle->length;

		inout_param.data = (void *)pDsetHandle->data_ptr;
		inout_param.length = sizeof(WMIX_DSET_CMD_HDR) + sizeof(WMIX_DSET_OP_WRITE_PARAM_CMD) + pDsetHandle->length;
        
		break;
	}
	case	DSET_OP_COMMIT:
	{
		dset_op.cmd.commandId = WMIX_DSET_COMMIT_CMDID;
		dset_op.u.op_commit.dset_id = pDsetHandle->dset_id;

		inout_param.data = (void *)&dset_op;
		inout_param.length = sizeof(WMIX_DSET_CMD_HDR) + sizeof(WMIX_DSET_OP_COMMIT_PARAM_CMD);
		break;
	}
	case	DSET_OP_CLOSE:
	{
		dset_op.cmd.commandId = WMIX_DSET_CLOSE_CMDID;
		dset_op.u.op_close.dset_id = pDsetHandle->dset_id;

		inout_param.data = (void *)&dset_op;
		inout_param.length = sizeof(WMIX_DSET_CMD_HDR) + sizeof(WMIX_DSET_OP_CLOSE_PARAM_CMD);
		break;
	}
	case	DSET_OP_DELETE:
	{
		dset_op.cmd.commandId = WMIX_DSET_DELETE_CMDID;
		dset_op.u.op_delete.dset_id = pDsetHandle->dset_id;
		dset_op.u.op_delete.flags = pDsetHandle->flags;

		inout_param.data = (void *)&dset_op;
		inout_param.length = sizeof(WMIX_DSET_CMD_HDR) + sizeof(WMIX_DSET_OP_DELETE_PARAM_CMD);
		break;
	}

	};
    
	inout_param.cmd_id = ATH_DSET_OP_CMD;

	return HANDLE_IOCTL(&inout_param);
}

HOST_DSET_HANDLE *pDsetHandle;
#define    MAX_BUFF_SIZE    3000
uint_8   buffer[MAX_BUFF_SIZE];

A_STATUS ath_dset_api(int argc, char* argv[])
{
    uint_32    status;
    uint32_t dset_id;
    uint32_t flags, length, media_id, offset;

    if(strcmp(argv[2],"create") == 0) {
	   dset_id = atoi(argv[3]);
       media_id = atoi(argv[4]);
	   flags = atoi(argv[5]);
	   length = atoi(argv[6]);
	   status = api_host_dset_create(&pDsetHandle, dset_id, media_id, length, flags, NULL, NULL);
    }
    if(strcmp(argv[2],"open") == 0) {
	   dset_id = atoi(argv[3]);
	   flags = atoi(argv[4]);
	   status = api_host_dset_open(&pDsetHandle, dset_id, flags, NULL, NULL);
    }
	else if (strcmp(argv[2],"read") == 0 && argc >= 5)
    {
	   offset = atoi(argv[3]);
	   length = atoi(argv[4]);
       if (length > MAX_BUFF_SIZE)
	   {
		   printf("max buffer size %d\n", MAX_BUFF_SIZE);
           return(A_OK);
       }
       api_host_dset_read(pDsetHandle, buffer, length, offset, NULL, NULL);
	}
	else if (strcmp(argv[2],"write") == 0 && argc >= 6)
    {
	   flags  = atoi(argv[3]);
	   offset = atoi(argv[4]);
	   length = atoi(argv[5]);
	   status = api_host_dset_write(pDsetHandle, buffer, length, offset, flags, NULL, NULL);
	}
	else if (strcmp(argv[2],"commit") == 0)
    {
	   flags  = atoi(argv[3]);
	   offset = atoi(argv[4]);
	   length = atoi(argv[5]);
	   status = api_host_dset_commit(pDsetHandle, NULL, NULL);
	}   
	else if (strcmp(argv[2],"close") == 0)
    {
	   status = api_host_dset_close(pDsetHandle, NULL, NULL);
	}
	else if (strcmp(argv[2],"size") == 0)
    {
	   status = api_host_dset_size(pDsetHandle, NULL, NULL);
	}
	else if (strcmp(argv[2],"delete") == 0 && argc > 4)
    {
	   dset_id  = atoi(argv[3]);
	   flags = atoi(argv[4]);
	   status = api_host_dset_delete(dset_id, flags, NULL, NULL);
	}
   
    return status;
}

