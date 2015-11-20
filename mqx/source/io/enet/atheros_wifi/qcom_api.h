//------------------------------------------------------------------------------
// Copyright (c) 2014 Qualcomm Atheros, Inc.
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

#ifndef __QCOM_API_H__
#define __QCOM_API_H__

#define QCOM_WPS_MAX_KEY_LEN    64
#define WPS_MAX_DEVNAME_LEN     32
#define MAX_SSID_LEN            32
#define MAC_LEN                 6
#define MAX_SSID_LENGTH         (32+1)
#define P2P_WPS_PIN_LEN         9
#define MAX_PASSPHRASE_SIZE     64
#define P2P_MAX_NOA_DESCRIPTORS 4
#define WPS_MAX_PASSPHRASE_LEN  9
#define P2P_PERSISTENT_FLAG     0x80
#define MAX_LIST_COUNT          8
#define P2P_AUTO_CHANNEL        0
#define P2P_DEFAULT_CHAN        1

typedef void *P2P_DEV_CTXT;
typedef void (*QCOM_PROMISCUOUS_CB)(unsigned char *buf, int length);

typedef struct _qcom_scan_info
{
    A_UINT8 channel;
    A_UINT8 ssid_len;
    A_UINT8 rssi;
    A_UINT8 security_enabled;
    A_INT16 beacon_period;
    A_UINT8 preamble;
    A_UINT8 bss_type;
    A_UINT8 bssid[6];
    A_UINT8 ssid[32];
    A_UINT8 rsn_cipher;
    A_UINT8 rsn_auth;
    A_UINT8 wpa_cipher;
    A_UINT8 wpa_auth;
} QCOM_BSS_SCAN_INFO, * QCOM_BSS_SCAN_INFO_PTR;

typedef struct _qcom_scan_params
{
    A_UINT16 fgStartPeriod;
    A_UINT16 fgEndPeriod;
    A_UINT16 bgPeriod;
    A_UINT16 maxActChDwellTimeInMs;
    A_UINT16 pasChDwellTimeInMs;
    A_UINT8  shortScanRatio;
    A_UINT8  scanCtrlFlags;
    A_UINT16 minActChDwellTimeInMs;
    A_UINT16 maxActScanPerSsid;
    A_UINT32 maxDfsChActTimeInMs;
} qcom_scan_params_t;

#define QCOM_START_SCAN_PARAMS_CHANNEL_LIST_MAX 12

typedef struct _qcom_start_scan_params
{
    A_BOOL   forceFgScan;
    A_UINT32 homeDwellTimeInMs;
    A_UINT32 forceScanIntervalInMs;
    A_UINT8  scanType;
    A_UINT8  numChannels;
    A_UINT16 channelList[QCOM_START_SCAN_PARAMS_CHANNEL_LIST_MAX];
} qcom_start_scan_params_t;
                              
typedef struct sockaddr
{
    uint16_t	sa_family;
    char 	sa_data[32];
} sockaddr_t;

typedef enum {
    QCOM_WLAN_DEV_MODE_STATION = 0,
    QCOM_WLAN_DEV_MODE_AP,
    QCOM_WLAN_DEV_MODE_ADHOC,
    QCOM_WLAN_DEV_MODE_INVALID
}QCOM_WLAN_DEV_MODE;

typedef enum {
    QCOM_11A_MODE = 0x1,
    QCOM_11G_MODE = 0x2,
    QCOM_11N_MODE = 0x3,
    QCOM_11B_MODE = 0x4,
    QCOM_HT40_MODE = 0x5,
} QCOM_WLAN_DEV_PHY_MODE;

typedef enum {
    WLAN_AUTH_NONE  = 0,
    WLAN_AUTH_WPA ,
    WLAN_AUTH_WPA2  ,
    WLAN_AUTH_WPA_PSK ,
    WLAN_AUTH_WPA2_PSK ,
    WLAN_AUTH_WPA_CCKM ,
    WLAN_AUTH_WPA2_CCKM ,
    WLAN_AUTH_WPA2_PSK_SHA256 ,
    WLAN_AUTH_INVALID
} WLAN_AUTH_MODE;

typedef enum {
    WLAN_CRYPT_NONE          = 0,
    WLAN_CRYPT_WEP_CRYPT ,
    WLAN_CRYPT_TKIP_CRYPT ,
    WLAN_CRYPT_AES_CRYPT ,
    WLAN_CRYPT_WAPI_CRYPT ,
    WLAN_CRYPT_BIP_CRYPT ,
    WLAN_CRYPT_KTK_CRYPT,
    WLAN_CRYPT_INVALID
} WLAN_CRYPT_TYPE;

/* WPS credential information */
typedef struct {
    A_UINT16            ap_channel;
    A_UINT8             ssid[WMI_MAX_SSID_LEN];
    A_UINT8             ssid_len;
    WLAN_AUTH_MODE      auth_type; /* WPS_AUTH_TYPE */
    WLAN_CRYPT_TYPE     encr_type; /* WPS_ENCR_TYPE */
    A_UINT8             key_idx;
    A_UINT8             key[QCOM_WPS_MAX_KEY_LEN+1];
    A_UINT8             key_len;
    A_UINT8             mac_addr[ATH_MAC_LEN];
} qcom_wps_credentials_t;

typedef enum  {
    P2P_WPS_NOT_READY, P2P_WPS_PIN_LABEL, P2P_WPS_PIN_DISPLAY, P2P_WPS_PIN_KEYPAD, P2P_WPS_PBC
} P2P_WPS_METHOD;

typedef struct {
    A_UINT8 enable;
    A_UINT8 count;
#if ENABLE_P2P_MODE                                         
    P2P_NOA_DESCRIPTOR noas[P2P_MAX_NOA_DESCRIPTORS];
#endif
} P2P_NOA_INFO;

typedef enum {
    P2P_INV_ROLE_GO,
    P2P_INV_ROLE_ACTIVE_GO,
    P2P_INV_ROLE_CLIENT,
} P2P_INV_ROLE;

typedef enum {
    P2P_CONFIG_LISTEN_CHANNEL,
    P2P_CONFIG_CROSS_CONNECT,
    P2P_CONFIG_SSID_POSTFIX,
    P2P_CONFIG_INTRA_BSS,
    P2P_CONFIG_CONCURRENT_MODE,
    P2P_CONFIG_GO_INTENT,
    P2P_CONFIG_DEV_NAME,
    P2P_CONFIG_P2P_OPMODE,
    P2P_CONFIG_CCK_RATES,    
    P2P_CONFIG_MAX,
} P2P_CONF_ID;

typedef struct { A_UINT8 addr[6]; } A_MACADDR; 

typedef struct { 
    A_UINT8 rate_index;
	A_UINT8 tries;
	A_UINT32 size;
	A_UINT32 chan;
	A_UINT8 header_type;
	A_UINT16  seq; 
	A_MACADDR addr1; 
	A_MACADDR addr2;
	A_MACADDR addr3;
	A_MACADDR addr4;
	A_UINT8 *pdatabuf;
	A_UINT32 buflen;
} QCOM_RAW_MODE_PARAM_t;


#if ENABLE_DNS_CLIENT
A_STATUS qcom_dnsc_enable(A_BOOL enable);
A_STATUS qcom_dnsc_add_server_address(A_UINT8 *ipaddress, A_UINT8 type);
A_STATUS qcom_dnsc_del_server_address(A_UINT8 *ipaddress, A_UINT8 type);
A_STATUS qcom_dnsc_get_host_by_name(A_CHAR *pname, A_UINT32 *pipaddress);
A_STATUS qcom_dnsc_get_host_by_name2(A_CHAR *pname, A_UINT32 *pipaddress, A_INT32 domain, A_UINT32 mode);
#endif

#if ENABLE_DNS_SERVER
A_STATUS qcom_dns_server_address_get(A_UINT32 pdns[], A_UINT32* number);
A_STATUS qcom_dnss_enable(A_BOOL enable);
A_STATUS qcom_dns_local_domain(A_CHAR* local_domain);
A_STATUS qcom_dns_entry_create(A_CHAR* hostname, A_UINT32 ipaddress);
A_STATUS qcom_dns_entry_delete(A_CHAR* hostname, A_UINT32 ipaddress);
A_STATUS qcom_dns_6entry_create(A_CHAR* hostname, A_UINT8* ip6addr);
A_STATUS qcom_dns_6entry_delete(A_CHAR* hostname, A_UINT8* ip6addr);
#endif

#if ENABLE_SNTP_CLIENT
void qcom_sntp_srvr_addr(int flag,char* srv_addr);
void qcom_sntp_get_time(A_UINT8 device_id, tSntpTime* time);
void qcom_sntp_get_time_of_day(A_UINT8 device_id, tSntpTM* time);
void qcom_sntp_zone(int hour,int min,int add_sub,int enable);
void qcom_sntp_query_srvr_address(A_UINT8 device_id, tSntpDnsAddr* addr);
void qcom_enable_sntp_client(int enable);
#endif


A_STATUS qcom_ota_upgrade(A_UINT8 device_id,A_UINT32 addr,A_CHAR *filename,A_UINT8 mode,A_UINT8 preserve_last,A_UINT8 protocol,A_UINT32 *resp_code,A_UINT32 *length);
A_STATUS qcom_ota_read_area(A_UINT32 offset,A_UINT32 size,A_UCHAR * buffer,A_UINT32 *ret_len);
A_STATUS qcom_ota_done();


A_STATUS qcom_set_scan(A_UINT8 device_id);
A_STATUS qcom_get_scan(A_UINT8 device_id, QCOM_BSS_SCAN_INFO** buf, A_INT16* numResults);
A_STATUS qcom_set_ssid(A_UINT8 device_id, A_CHAR *pssid);
A_STATUS qcom_get_ssid(A_UINT8 device_id, A_CHAR *pssid);
A_STATUS qcom_scan_set_mode(A_UINT8 device_id, A_UINT32 mode);
A_STATUS qcom_set_connect_callback (A_UINT8 device_id,void *callback);
A_STATUS qcom_commit(A_UINT8 device_id);
A_STATUS qcom_sta_get_rssi(A_UINT8 device_id, A_UINT8 *prssi);
A_STATUS qcom_sta_set_listen_time(A_UINT8 device_id, A_UINT32 listentime); 
A_STATUS qcom_ap_set_beacon_interval(A_UINT8 device_id, A_UINT16 beacon_interval);
A_STATUS qcom_ap_hidden_mode_enable(A_UINT8 device_id, A_BOOL enable);
A_STATUS qcom_op_get_mode(A_UINT8 device_id, A_UINT32 *pmode);
A_STATUS qcom_op_set_mode(A_UINT8 device_id, A_UINT32 mode) ; 
A_STATUS qcom_disconnect(A_UINT8 device_id);
A_STATUS qcom_set_phy_mode(A_UINT8 device_id, A_UINT8 phymode);
A_STATUS qcom_get_phy_mode(A_UINT8 device_id, A_UINT8 *pphymode);
A_STATUS qcom_set_channel(A_UINT8 device_id, A_UINT16 channel);
A_STATUS qcom_get_channel(A_UINT8 device_id, A_UINT16 *pchannel);
A_STATUS qcom_set_tx_power(A_UINT8 device_id, A_UINT32 dbm);
A_STATUS qcom_allow_aggr_set_tid(A_UINT8 device_id, A_UINT16 tx_allow_aggr,  A_UINT16 rx_allow_aggr);
A_STATUS qcom_sec_set_wepkey(A_UINT8 device_id, A_UINT32 keyindex, A_CHAR *pkey);
A_STATUS qcom_sec_get_wepkey(A_UINT8 device_id, A_UINT32 keyindex, A_CHAR *pkey);
A_STATUS qcom_sec_set_wepkey_index(A_UINT8 device_id, A_UINT32 keyindex);
A_STATUS qcom_sec_get_wepkey_index(A_UINT8 device_id, A_UINT32 *pkeyindex);
A_STATUS qcom_sec_set_auth_mode(A_UINT8 device_id, A_UINT32 mode);
A_STATUS qcom_sec_set_encrypt_mode(A_UINT8 device_id, A_UINT32 mode);
A_STATUS qcom_sec_set_passphrase(A_UINT8 device_id, A_CHAR *passphrase);
A_STATUS qcom_sec_set_pmk(A_UINT8 device_id, A_CHAR *pmk);

A_STATUS qcom_power_set_mode(A_UINT8 device_id, A_UINT32 powerMode, A_UINT8 powerModule);
A_STATUS qcom_power_get_mode(A_UINT8 device_id, A_UINT32 *powerMode);
A_STATUS qcom_suspend_enable(A_BOOL enable);
A_STATUS qcom_suspend_start(A_UINT32 susp_time);
A_STATUS qcom_power_set_parameters(A_UINT8 device_id, 
                                   A_UINT16 idlePeriod,
                                   A_UINT16 psPollNum,
                                   A_UINT16 dtimPolicy,
                                   A_UINT16 tx_wakeup_policy,
                                   A_UINT16 num_tx_to_wakeup,
                                   A_UINT16 ps_fail_event_policy);
A_STATUS qcom_get_bssid(A_UINT8 device_id, A_UINT8 mac_addr[ATH_MAC_LEN]);

A_STATUS qcom_sec_get_auth_mode(A_UINT8 device_id, A_UINT32 *pmode);
A_STATUS qcom_sec_get_encrypt_mode(A_UINT8 device_id, A_UINT32 *pmode);
A_STATUS qcom_sec_get_passphrase(A_UINT8 device_id, A_CHAR *passphrase);

A_STATUS qcom_wps_start(A_UINT8 device_id, int connect, int use_pinmode, char *pin);
A_STATUS qcom_wps_connect(A_UINT8 device_id);
A_STATUS qcom_wps_set_credentials(A_UINT8 device_id, qcom_wps_credentials_t *pwps_prof);

A_STATUS qcom_p2p_func_init(A_UINT8 device_id, A_INT32 enable);
A_STATUS qcom_p2p_func_cancel(A_UINT8 device_id);
A_STATUS qcom_p2p_func_set_pass_ssid(A_UINT8 device_id, A_CHAR *ppass, A_CHAR *pssid);
A_STATUS qcom_p2p_func_auth(A_UINT8 device_id, A_INT32 dev_auth, P2P_WPS_METHOD wps_method, A_UINT8 *peer_mac, A_BOOL persistent);
A_STATUS qcom_p2p_func_connect(A_UINT8 device_id, P2P_WPS_METHOD wps_method, A_UINT8 *peer_mac, A_BOOL persistent);
A_STATUS qcom_p2p_func_set_config(A_UINT8 device_id, A_UINT32 uigo_intent, A_UINT32 uiclisten_ch, A_UINT32 uiop_ch, A_UINT32 uiage, A_UINT32 reg_class, A_UINT32 op_reg_class, A_UINT32 max_node_count);
A_STATUS qcom_p2p_func_set_oppps(A_UINT8 device_id, A_UINT8 enable, A_UINT8 ctwin);
A_STATUS qcom_p2p_func_set_noa(A_UINT8 device_id, A_UINT8 uccount, A_UINT32 uistart, A_UINT32 uiduration, A_UINT32 uiinterval);
A_STATUS qcom_p2p_func_invite(A_UINT8 device_id, A_CHAR *pssid, P2P_WPS_METHOD wps_method, A_UINT8 *pmac, A_BOOL persistent, P2P_INV_ROLE role);
A_STATUS qcom_p2p_func_find(A_UINT8 device_id, void *dev, A_UINT8 type, A_UINT32 timeout);
A_STATUS qcom_p2p_func_stop_find(A_UINT8 device_id);
A_STATUS qcom_p2p_func_get_node_list(A_UINT8 device_id, void *app_buf, A_UINT32 buf_size);
A_STATUS qcom_p2p_func_get_network_list(A_UINT8 device_id, void *app_buf, A_UINT32 buf_size);
A_STATUS qcom_p2p_func_invite_auth(A_UINT8 device_id, A_UINT8 *inv);
A_STATUS qcom_p2p_func_listen(A_UINT8 device_id, A_UINT32 timeout);
A_STATUS qcom_p2p_func_join_profile(A_UINT8 device_id, A_UINT8 *pmac);
A_STATUS qcom_p2p_set(A_UINT8 device_id, P2P_CONF_ID config_id, void *data, A_UINT32 data_length);
A_STATUS qcom_p2p_func_prov(A_UINT8 device_id, P2P_WPS_METHOD wps_method, A_UINT8 *pmac);
A_STATUS qcom_p2p_func_join(A_UINT8 device_id, P2P_WPS_METHOD wps_method, A_UINT8 *pmac, char *ppin, A_UINT16 channel);
A_STATUS qcom_p2p_func_start_go(A_UINT8 device_id, A_UINT8 *pssid, A_UINT8 *ppass, A_INT32 channel, A_BOOL ucpersistent);

int qcom_socket(int family, int type, int protocol);
int qcom_connect(int s, struct sockaddr* addr, int addrlen);
int qcom_bind(int s, struct sockaddr* addr, int addrlen);
int qcom_listen(int s,int backlog);
int qcom_accept(int s, struct sockaddr* addr,int *addrlen)  ;
int qcom_setsockopt(int s, int level,int name,void* arg,int arglen);
#if ZERO_COPY
int qcom_recv(int s,char** buf,int len,int flags);
#else
int qcom_recv(int s,char* buf,int len,int flags);
#endif

#if ZERO_COPY
int qcom_recvfrom(int s, char** buf, int len, int flags, struct sockaddr* from, int* fromlen);
#else
int qcom_recvfrom(int s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen);

#endif
A_STATUS qcom_ipconfig(A_UINT8 device_id, A_INT32 mode,A_UINT32* address,A_UINT32* submask,A_UINT32* gateway);
SSL* qcom_SSL_new(SSL_CTX *ctx);
SSL_CTX* qcom_SSL_ctx_new(SSL_ROLE_T role, A_INT32 inbufSize, A_INT32 outbufSize, A_INT32 reserved);
A_INT32 qcom_SSL_setCaList(SSL_CTX *ctx, SslCAList caList, A_UINT32 size);
A_INT32 qcom_SSL_addCert(SSL_CTX *ctx, SslCert cert, A_UINT32 size);
A_INT32 qcom_SSL_storeCert(A_CHAR *name, SslCert cert, A_UINT32 size);
A_INT32 qcom_SSL_loadCert(SSL_CTX *ctx, SSL_CERT_TYPE_T type, char *name);
A_INT32 qcom_SSL_listCert(SSL_FILE_NAME_LIST *fileNames);
A_INT32 qcom_SSL_set_fd(SSL *ssl, A_UINT32 fd);
A_INT32 qcom_SSL_accept(SSL *ssl);
A_INT32 qcom_SSL_connect(SSL *ssl);
A_INT32 qcom_SSL_shutdown(SSL *ssl);
A_INT32 qcom_SSL_ctx_free(SSL_CTX *ctx);
A_INT32 qcom_SSL_configure(SSL *ssl, SSL_CONFIG *cfg);
#if ZERO_COPY
A_INT32 qcom_SSL_read(SSL *ssl, void **buf, A_INT32 num);
#else
A_INT32 qcom_SSL_read(SSL *ssl, void *buf, A_INT32 num);
#endif
A_INT32 qcom_SSL_write(SSL *ssl, const void *buf, A_INT32 num);

int qcom_sendto( int s, char* buffer, int length, int flags, struct sockaddr* to, int tolen);
int qcom_send( int s, char* buffer, int length, int flags);
int qcom_socket_close(int s);
A_STATUS qcom_ip6_address_get( A_UINT8 device_id,A_UINT8 *v6Global, A_UINT8 *v6Link, A_UINT8 *v6DefGw,A_UINT8 *v6GlobalExtd, A_INT32 *LinkPrefix,
		      A_INT32 *GlbPrefix, A_INT32 *DefgwPrefix, A_INT32 *GlbPrefixExtd);
A_STATUS qcom_ping(A_UINT32 host, A_UINT32 size);
A_STATUS qcom_ping6(A_UINT8* host, A_UINT32 size);
A_STATUS qcom_ip6config_router_prefix(A_UINT8 device_id,A_UINT8 *addr,A_INT32 prefixlen, A_INT32 prefix_lifetime, A_INT32 valid_lifetime);
A_INT32 qcom_dhcps_set_pool(A_UINT8 device_id, A_UINT32 startip, A_UINT32 endip,A_INT32 leasetime);
A_STATUS qcom_http_server(A_INT32 enable);
A_STATUS qcom_http_set_post_cb(void* cxt, void *callback);
A_STATUS qcom_ip_http_server_method(A_INT32 cmd, A_UINT8 *pagename, A_UINT8 *objname, A_INT32 objtype, A_INT32 objlen, A_UINT8 * value);
A_STATUS qcom_http_client_method(A_UINT32 cmd, A_UINT8* url, A_UINT8 *data, A_UINT8 *value);
A_STATUS qcom_ip6_route(A_UINT8 device_id, A_UINT32 cmd, A_UINT8* ip6addr, A_UINT32* prefixLen, A_UINT8* gw, A_UINT32* ifindex, IPV6_ROUTE_LIST_T *rtlist);
A_STATUS qcom_ip4_route(A_UINT8 device_id, A_UINT32 cmd, A_UINT32* addr, A_UINT32* subnet, A_UINT32* gw, A_UINT32* ifindex, IPV4_ROUTE_LIST_T *rtlist);
A_STATUS qcom_tcp_conn_timeout(A_UINT32 timeout_val);
A_STATUS qcom_tcp_set_exp_backoff(A_UINT8 device_id, A_INT32 retry);
A_STATUS qcom_dhcps_release_pool(A_UINT8 device_id);
A_STATUS qcom_bridge_mode_enable(A_UINT16 bridgemode);

A_STATUS qcom_raw_mode_send_pkt(QCOM_RAW_MODE_PARAM_t *ppara);
A_STATUS qcom_scan_params_set(A_UINT8 device_id, qcom_scan_params_t *pParam);

#endif
