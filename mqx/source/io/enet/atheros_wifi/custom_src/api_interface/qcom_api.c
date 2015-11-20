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
#include "atheros_stack_offload.h"
#include "dset_api.h"
#include "common_stack_offload.h"
#include "qcom_api.h"
//------------------------------------------------------------------------------------------------------------
qcom_wps_credentials_t gWpsCredentials;

//------------------------------------------------------------------------------------------------------------
extern A_VOID *p_Driver_Cxt[];
extern uint_32 Custom_Api_Mediactl( ENET_CONTEXT_STRUCT_PTR enet_ptr, uint_32 command_id, pointer inout_param);
extern uint_32 ath_ioctl_handler(ENET_CONTEXT_STRUCT_PTR enet_ptr, ATH_IOCTL_PARAM_STRUCT_PTR param_ptr);



/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : qcom_set_deviceid()
* Returned Value  : A_ERROR is command failed, else A_OK
* Comments	: Sets device ID in driver context
*
*END*-----------------------------------------------------------------*/
static A_STATUS qcom_set_deviceid( A_UINT16 id)
{
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    
    inout_param.cmd_id = ATH_SET_DEVICE_ID;
    inout_param.data = (void*)&id;
    inout_param.length = sizeof(A_UINT16);

    return(ath_ioctl_handler(p_Driver_Cxt[id], &inout_param));
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

#if 0
static enum p2p_wps_method qcom_p2p_get_wps(P2P_WPS_METHOD wps)
{
    switch (wps) {
        case P2P_WPS_NOT_READY:
            return WPS_NOT_READY;
        case P2P_WPS_PIN_LABEL:
            return WPS_PIN_LABEL;
        case P2P_WPS_PIN_DISPLAY:
            return WPS_PIN_DISPLAY;
        case P2P_WPS_PIN_KEYPAD:
            return WPS_PIN_KEYPAD;
        case P2P_WPS_PBC:
            return WPS_PBC;
        default:
            return WPS_NOT_READY;
    }
}
#endif

//------------------------------------------------------------------------------------------------------------
int qcom_socket(int family, int type, int protocol)
{
    return(t_socket(p_Driver_Cxt[0], family, type, protocol));
}

int qcom_connect(int s, struct sockaddr* addr, int addrlen)
{
    return(t_connect(p_Driver_Cxt[0], s, addr, addrlen));
}

int qcom_bind(int s, struct sockaddr* addr, int addrlen)
{
    return(t_bind(p_Driver_Cxt[0], s, addr, addrlen));
}

int qcom_listen(int s,int backlog)
{
    return(t_listen(p_Driver_Cxt[0], s, backlog));
}

int qcom_accept(int s, struct sockaddr* addr,int *addrlen)           
{
    return(t_accept(p_Driver_Cxt[0], s, addr, *addrlen));
}
 
int qcom_setsockopt(int s, int level,int name,void* arg,int arglen)
{
    return(t_setsockopt(p_Driver_Cxt[0], s, level, name, arg, arglen));
}


A_INT32 qcom_getsockopt(int s, int level, int name, void* arg, int arglen)
{
    return(t_getsockopt(p_Driver_Cxt[0], s, level, name, arg, arglen));
}



#if ZERO_COPY
int qcom_recv(int s,char** buf,int len,int flags)
#else
int qcom_recv(int s,char* buf,int len,int flags)
#endif
{
    return(t_recv(p_Driver_Cxt[0], s, (void*)buf, len, flags));
}

#if ZERO_COPY
int qcom_recvfrom(int s, char** buf, int len, int flags, struct sockaddr* from, int* fromlen)
#else
int qcom_recvfrom(int s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen)
#endif
{
    return(t_recvfrom(p_Driver_Cxt[0], s, (void*)buf, len, flags, from, (A_UINT32*)fromlen));
}



#if ENABLE_DNS_CLIENT
A_STATUS qcom_dnsc_enable(A_BOOL enable)
{
    return((A_STATUS)custom_ip_dns_client(p_Driver_Cxt[0], enable));
}

A_STATUS qcom_dnsc_add_server_address(A_UINT8 *ipaddress, A_UINT8 type)
{
    IP46ADDR dnsaddr;

    memset(&dnsaddr, 0, sizeof(IP46ADDR));
    if( type == ATH_AF_INET) 
    {
	dnsaddr.type = ATH_AF_INET;
	dnsaddr.addr4 = (A_UINT32) *(A_UINT32 *)ipaddress;
    } else {
        dnsaddr.type = ATH_AF_INET6;
	memcpy(&dnsaddr.addr6, ipaddress, sizeof(IP6_ADDR_T));
    }
          
    return ((A_STATUS)custom_ip_dns_server_addr(p_Driver_Cxt[0], &dnsaddr));
}

A_STATUS qcom_dnsc_del_server_address(A_UINT8 *ipaddress, A_UINT8 type)
{
    IP46ADDR dnsaddr;
#define DELETE_DNS_SERVER_ADDR 1
    
    memset(&dnsaddr, 0, sizeof(IP46ADDR));
    dnsaddr.au1Rsvd[0] = DELETE_DNS_SERVER_ADDR;     
    if( type == ATH_AF_INET) 
    {
	dnsaddr.type = ATH_AF_INET;
	dnsaddr.addr4 = (A_UINT32) *(A_UINT32 *)ipaddress;
    } else {
        dnsaddr.type = ATH_AF_INET6;
	memcpy(&dnsaddr.addr6, ipaddress, sizeof(IP6_ADDR_T));
    }
          
    return ((A_STATUS)custom_ip_dns_server_addr(p_Driver_Cxt[0], &dnsaddr));
}

A_STATUS qcom_dnsc_get_host_by_name(A_CHAR *pname, A_UINT32 *pipaddress)
{
    DNC_CFG_CMD DnsCfg;
    DNC_RESP_INFO  DnsRespInfo;     

    memset(&DnsRespInfo,0,sizeof(DnsRespInfo));
    strcpy(DnsCfg.ahostname, pname);
    DnsCfg.domain = 2;
    DnsCfg.mode =  GETHOSTBYNAME;
    if (A_OK != custom_ip_resolve_hostname(p_Driver_Cxt[0], &DnsCfg,&DnsRespInfo))
    {
        return A_ERROR;
    }
    *pipaddress = A_CPU2BE32(DnsRespInfo.ipaddrs_list[0]);
    return A_OK;
}

A_STATUS qcom_dnsc_get_host_by_name2(A_CHAR *pname, A_UINT32 *pipaddress, A_INT32 domain, A_UINT32 mode)
{
    DNC_CFG_CMD DnsCfg;
    DNC_RESP_INFO  DnsRespInfo;     

    memset(&DnsRespInfo,0,sizeof(DnsRespInfo));
    strcpy(DnsCfg.ahostname, pname);
    DnsCfg.domain = domain;
    DnsCfg.mode =  mode;
    if (A_OK != custom_ip_resolve_hostname(p_Driver_Cxt[0], &DnsCfg,&DnsRespInfo))
    {
        return A_ERROR;
    }

    if (domain == 3)
        memcpy(pipaddress, &DnsRespInfo.ip6addrs_list[0], 16);
    else
        memcpy(pipaddress, &DnsRespInfo.ipaddrs_list[0], 4);    
    return A_OK;
}

#endif
#if ENABLE_DNS_SERVER
A_STATUS qcom_dns_server_address_get(A_UINT32 pdns[], A_UINT32* number)
{
    int i;
    A_STATUS error;
    IP46ADDR dnsaddr[MAX_DNSADDRS];
    
    memset(&dnsaddr[0], 0, sizeof(dnsaddr));
    error = t_ipconfig(p_Driver_Cxt[0], IPCFG_QUERY, NULL, NULL, NULL, dnsaddr, NULL);
    *number = 0;
    
    if( error == A_OK)
    {
        for(i=0; i<MAX_DNSADDRS; i++)	
        {
            if(dnsaddr[i].addr4 != 0 ) {
                pdns[i] = dnsaddr[i].addr4; 
                *number=*number+1;
            } else {
                break;
            }
        }
    }
    
    return error; 
}  

A_STATUS qcom_dnss_enable(A_BOOL enable)
{
    return((A_STATUS)custom_ip_dns_server(p_Driver_Cxt[0], (A_UINT32)enable));  
}

A_STATUS qcom_dns_local_domain(A_CHAR* local_domain)
{
    return((A_STATUS) custom_ip_dns_local_domain(p_Driver_Cxt[0], local_domain)) ;
}


A_STATUS qcom_dns_entry_create(A_CHAR* hostname, A_UINT32 ipaddress)
{
    IP46ADDR dnsaddr;
    dnsaddr.type = ATH_AF_INET;
    dnsaddr.addr4 = ipaddress;    
    return((A_STATUS)custom_ipdns(p_Driver_Cxt[0], 1, hostname, &dnsaddr));
}

A_STATUS qcom_dns_entry_delete(A_CHAR* hostname, A_UINT32 ipaddress)
{
    IP46ADDR dnsaddr;
    dnsaddr.type = ATH_AF_INET;
    dnsaddr.addr4 = ipaddress;    
    return((A_STATUS)custom_ipdns(p_Driver_Cxt[0], 2, hostname, &dnsaddr));
}

A_STATUS qcom_dns_6entry_create(A_CHAR* hostname, A_UINT8* ip6addr)
{
    IP46ADDR dnsaddr;
    dnsaddr.type = ATH_AF_INET6;
    memcpy(&dnsaddr.addr6,ip6addr,sizeof(IP6_ADDR_T));    
    return((A_STATUS)custom_ipdns(p_Driver_Cxt[0], 1, hostname,&dnsaddr));
}

A_STATUS qcom_dns_6entry_delete(A_CHAR* hostname, A_UINT8* ip6addr)
{
    IP46ADDR dnsaddr;
    dnsaddr.type = ATH_AF_INET6;
    memcpy(&dnsaddr.addr6,&ip6addr,sizeof(IP6_ADDR_T));    
    return((A_STATUS)custom_ipdns(p_Driver_Cxt[0], 2, hostname, &dnsaddr));
}

#endif

#if ENABLE_SNTP_CLIENT
void qcom_sntp_srvr_addr(int flag,char* srv_addr)
{
    custom_ip_sntp_srvr_addr(p_Driver_Cxt[0], flag, srv_addr); 
}

void qcom_sntp_get_time(A_UINT8 device_id, tSntpTime* time)
{
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return;
    }  
    custom_ip_sntp_get_time(p_Driver_Cxt[device_id], time);  
}

void qcom_sntp_get_time_of_day(A_UINT8 device_id, tSntpTM* time)
{
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return;
    }  
    custom_ip_sntp_get_time_of_day(p_Driver_Cxt[device_id],time);
}
void qcom_sntp_zone(int hour,int min,int add_sub,int enable)
{
    custom_ip_sntp_modify_zone_dse(p_Driver_Cxt[0], hour, min, add_sub, enable);
}

void qcom_sntp_query_srvr_address(A_UINT8 device_id, tSntpDnsAddr* addr)
{
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return;
    }  
    custom_ip_sntp_query_srvr_address(p_Driver_Cxt[device_id],addr);
}

void qcom_enable_sntp_client(int enable)
{
    custom_ip_sntp_client(p_Driver_Cxt[0], enable);
}

#endif

/****************QCOM_OTA_APIs************************/


A_STATUS qcom_ota_upgrade(A_UINT8 device_id,A_UINT32 addr,A_CHAR *filename,A_UINT8 mode,A_UINT8 preserve_last,A_UINT8 protocol,A_UINT32 * resp_code,A_UINT32 *length)
{
  return((A_STATUS)custom_ota_upgrade(p_Driver_Cxt[device_id],addr,filename,mode,preserve_last,protocol,resp_code,length));
}

A_STATUS qcom_ota_read_area(A_UINT32 offset,A_UINT32 size,A_UCHAR *buffer,A_UINT32 *retlen){

  return((A_STATUS)custom_ota_read_area(p_Driver_Cxt[0],offset,size,buffer,retlen));
}

A_STATUS qcom_ota_done(){
  
  return((A_STATUS)custom_ota_done(p_Driver_Cxt[0]));
}


int qcom_sendto( int s, char* buffer, int length, int flags, struct sockaddr* to, int tolen)
{
    return(t_sendto(p_Driver_Cxt[0], s,(A_UINT8*)buffer, length, flags, to, tolen));
}

int qcom_send( int s, char* buffer, int length, int flags)
{
    return(t_send(p_Driver_Cxt[0], s, (A_UINT8*)buffer, length, flags));
}

int qcom_socket_close(int s)
{
    return(t_shutdown(p_Driver_Cxt[0], s));
}

A_STATUS qcom_ipconfig(A_UINT8 device_id, A_INT32 mode,A_UINT32* address,A_UINT32* submask,A_UINT32* gateway)
{
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    return(t_ipconfig(p_Driver_Cxt[device_id], mode, address, submask, gateway,NULL,NULL));
}

A_STATUS qcom_ip6_address_get( A_UINT8 device_id,A_UINT8 *v6Global, A_UINT8 *v6Link, A_UINT8 *v6DefGw,A_UINT8 *v6GlobalExtd, A_INT32 *LinkPrefix,
		      A_INT32 *GlbPrefix, A_INT32 *DefgwPrefix, A_INT32 *GlbPrefixExtd)
{
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    return(t_ip6config(p_Driver_Cxt[device_id], IPCFG_QUERY, (IP6_ADDR_T*)v6Global, (IP6_ADDR_T*)v6Link, (IP6_ADDR_T*)v6DefGw, (IP6_ADDR_T*)v6GlobalExtd, LinkPrefix, GlbPrefix, DefgwPrefix, GlbPrefixExtd));
}

A_STATUS qcom_ping(A_UINT32 host, A_UINT32 size)
{
    return(t_ping(p_Driver_Cxt[0], host, size));
}
           
A_STATUS qcom_ping6(A_UINT8* host, A_UINT32 size)
{
    return(t_ping6(p_Driver_Cxt[0], host, size));
}           

A_STATUS qcom_ip6config_router_prefix(A_UINT8 device_id,A_UINT8 *addr,A_INT32 prefixlen, A_INT32 prefix_lifetime, A_INT32 valid_lifetime)
{
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    return(t_ip6config_router_prefix(p_Driver_Cxt[device_id],addr,prefixlen, prefix_lifetime, valid_lifetime));
}

A_STATUS qcom_bridge_mode_enable(A_UINT16 bridgemode)
{
   return(custom_ipbridgemode(p_Driver_Cxt[0], bridgemode));
}

A_STATUS qcom_tcp_set_exp_backoff(A_UINT8 device_id, A_INT32 retry)
{
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    return(custom_ipconfig_set_tcp_exponential_backoff_retry(p_Driver_Cxt[device_id],retry));
}

A_STATUS qcom_ip4_route(A_UINT8 device_id, A_UINT32 cmd, A_UINT32* addr, A_UINT32* subnet, A_UINT32* gw, A_UINT32* ifindex, IPV4_ROUTE_LIST_T *rtlist)
{
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    return(custom_ipv4_route(p_Driver_Cxt[device_id], cmd, addr, subnet, gw, ifindex, rtlist));
}

A_STATUS qcom_ip6_route(A_UINT8 device_id, A_UINT32 cmd, A_UINT8* ip6addr, A_UINT32* prefixLen, A_UINT8* gw, A_UINT32* ifindex, IPV6_ROUTE_LIST_T *rtlist)
{
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    return(custom_ipv6_route(p_Driver_Cxt[device_id], cmd, ip6addr, prefixLen, gw, ifindex, rtlist));
}


A_STATUS qcom_tcp_conn_timeout(A_UINT32 timeout_val){
    return(custom_tcp_connection_timeout(p_Driver_Cxt[0],timeout_val));
}
A_INT32 qcom_dhcps_set_pool(A_UINT8 device_id, A_UINT32 startip, A_UINT32 endip,A_INT32 leasetime)
{
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    return(t_ipconfig_dhcp_pool(p_Driver_Cxt[device_id],&startip, &endip,leasetime)); 
}

A_STATUS qcom_dhcps_release_pool(A_UINT8 device_id)
{
   if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
   return(custom_ipconfig_dhcp_release(p_Driver_Cxt[device_id])); 
}  

A_STATUS qcom_http_server(A_INT32 enable){
    return(custom_ip_http_server(p_Driver_Cxt[0],enable));             
}

A_STATUS qcom_http_set_post_cb(void* cxt, void *callback)
{
    if (callback == NULL)
    {
        return A_ERROR;
    }
    return(custom_http_set_post_cb(p_Driver_Cxt[0], cxt, callback));       
}

A_STATUS qcom_ip_http_server_method(A_INT32 cmd, A_UINT8 *pagename, A_UINT8 *objname, A_INT32 objtype, A_INT32 objlen, A_UINT8 * value){
    return(custom_ip_http_server_method(p_Driver_Cxt[0], cmd, pagename, objname, objtype, objlen, value));
}

A_STATUS qcom_http_client_method(A_UINT32 cmd, A_UINT8* url, A_UINT8 *data, A_UINT8 *value){
    HTTPC_RESPONSE * output = NULL;
    A_STATUS result = A_OK;
    
    if(NULL == value)
    {
        result = custom_httpc_method(p_Driver_Cxt[0], cmd, url, data, NULL); //for HTTPC_CONNECT or HTTPC_DISC 
    } else 
    {
      result = custom_httpc_method(p_Driver_Cxt[0], cmd, url, data, (A_UINT8**)&output);
    }
    
    if (result == A_OK){
      if(output && value){
          A_MEMCPY(value, output, sizeof(HTTPC_RESPONSE) + output->length - 1);
          zero_copy_http_free((A_VOID *)output);
      }      
    }else{
        result = A_ERROR;
    }
    return result;
}             
 

SSL_CTX* qcom_SSL_ctx_new(SSL_ROLE_T role, A_INT32 inbufSize, A_INT32 outbufSize, A_INT32 reserved)
{
    return(SSL_ctx_new(role, inbufSize, outbufSize, reserved));
}

A_INT32 qcom_SSL_ctx_free(SSL_CTX *ctx){
    return(SSL_ctx_free(ctx));
}

SSL* qcom_SSL_new(SSL_CTX *ctx){
    return(SSL_new(ctx));
}

A_INT32 qcom_SSL_setCaList(SSL_CTX *ctx, SslCAList caList, A_UINT32 size){
    return(SSL_setCaList(ctx, caList, size));
}

A_INT32 qcom_SSL_addCert(SSL_CTX *ctx, SslCert cert, A_UINT32 size){
    return(SSL_addCert(ctx, cert, size));
}

A_INT32 qcom_SSL_storeCert(A_CHAR *name, SslCert cert, A_UINT32 size){
    return(SSL_storeCert(name, cert, size));
}
A_INT32 qcom_SSL_loadCert(SSL_CTX *ctx, SSL_CERT_TYPE_T type, char *name){
    return(SSL_loadCert(ctx, type, name));
}

A_INT32 qcom_SSL_listCert(SSL_FILE_NAME_LIST *fileNames){
    return(SSL_listCert(fileNames));
}

A_INT32 qcom_SSL_set_fd(SSL *ssl, A_UINT32 fd){
    return(SSL_set_fd(ssl, fd));
}

A_INT32 qcom_SSL_accept(SSL *ssl){
    return(SSL_accept(ssl));
}

A_INT32 qcom_SSL_connect(SSL *ssl){
    return(SSL_connect(ssl));
}

A_INT32 qcom_SSL_shutdown(SSL *ssl){
    return(SSL_shutdown(ssl));  
}

A_INT32 qcom_SSL_configure(SSL *ssl, SSL_CONFIG *cfg){
    return(SSL_configure(ssl, cfg));
}

#if ZERO_COPY
A_INT32 qcom_SSL_read(SSL *ssl, void **buf, A_INT32 num)
#else
A_INT32 qcom_SSL_read(SSL *ssl, void *buf, A_INT32 num)
#endif
{
    return(SSL_read(ssl, buf, num));
}
A_INT32 qcom_SSL_write(SSL *ssl, const void *buf, A_INT32 num)
{
    return(SSL_write(ssl, buf, num));
}

/****************************************************************************************/
A_STATUS qcom_set_scan(A_UINT8 device_id)
{
    A_STATUS error = A_OK;
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    if( Custom_Api_Mediactl(p_Driver_Cxt[device_id],ENET_SET_MEDIACTL_SCAN,NULL) != A_OK)
       error = A_ERROR;
  
    return error;
}

A_STATUS qcom_get_scan(A_UINT8 device_id, QCOM_BSS_SCAN_INFO** buf, A_INT16* numResults)
{
    ENET_SCAN_LIST  param;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    if(Custom_Api_Mediactl(p_Driver_Cxt[device_id],ENET_GET_MEDIACTL_SCAN,&param)!= A_OK){
        return A_ERROR;
    }
    *buf=(QCOM_BSS_SCAN_INFO*)param.scan_info_list;
    *numResults = param.num_scan_entries;
    
    return A_OK;
}

A_STATUS qcom_set_ssid(A_UINT8 device_id, A_CHAR *pssid)
{
    ENET_ESSID param;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }    
    if (!pssid)
        param.flags = 0;
    else
        param.flags = 1;  
    
    param.essid = pssid;
    param.length = strlen(pssid);
    
    return(Custom_Api_Mediactl(p_Driver_Cxt[device_id],ENET_SET_MEDIACTL_ESSID,&param));
}


A_STATUS qcom_get_ssid(A_UINT8 device_id, A_CHAR *pssid)
{
    ENET_MEDIACTL_PARAM  param;
    param.data = (void*)pssid;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    if(Custom_Api_Mediactl(p_Driver_Cxt[device_id],ENET_GET_MEDIACTL_ESSID,&param) != A_OK){
        return A_ERROR;
    }
    pssid[param.length] = '\0';
    return A_OK;
}

A_STATUS qcom_set_connect_callback (A_UINT8 device_id,void *callback)
{
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_STATUS error = A_OK;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    inout_param.cmd_id = ATH_SET_CONNECT_STATE_CALLBACK;
    inout_param.data = (void*)callback;
    inout_param.length = sizeof(void *);    

    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
    }
    return error;  
}

static A_STATUS qcom_get_concurrent_channel(A_UINT8 device_id, A_UINT16 *conc_channel)
{
    ATH_IOCTL_PARAM_STRUCT ath_param;
    A_STATUS error = A_OK;

     if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    ath_param.cmd_id = ATH_GET_CONC_DEV_CHANNEL;
    ath_param.data = conc_channel;
    
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&ath_param) != A_OK){
        error = A_ERROR;
    }
    return error; 
}

static A_STATUS qcom_get_channelhint(A_UINT8 device_id, A_UINT16 *pchannel)
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    A_STATUS error = A_OK;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    inout_param.cmd_id = ATH_GET_CHANNELHINT;
    inout_param.data = pchannel;


    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
    }
    return error;  
}


A_STATUS qcom_commit (A_UINT8 device_id)
{
#if ENABLE_SCC_MODE 
    A_UINT16 conc_channel, cur_channel;
    A_UINT8 ssid[MAX_SSID_LENGTH];  
    A_UINT8  val;
    int num_dev = WLAN_NUM_OF_DEVICES;
   
    if( qcom_get_ssid (device_id,(char*)ssid) != A_OK )
       return A_ERROR;
    
    /*this is connect request */ 
    if( (strlen((char *)ssid) != 0 ) && (num_dev > 1)) {
        qcom_get_channelhint(device_id, &cur_channel); 
        qcom_get_concurrent_channel(device_id, &conc_channel);
        /* if device 1 is connected and this connect request is from device 0, 
           check both devices whether stay at same channel, if not, return fail due to device 1 is primary
        */
        if( (conc_channel != 0) && (device_id == 0) && (conc_channel != cur_channel)){
 		return A_ERROR;
        }
        /* if device 0 is connected and this connect request is from device 1, 
           check both devices whether stay at same channel, if not, p2p_cancel is issue to device 0 because device 1 is promary
        */
        if( (conc_channel != 0) && (device_id == 1) && (conc_channel != cur_channel)){
            qcom_set_deviceid(0);
            qcom_set_channel(0, conc_channel);
            A_MDELAY(50);
            qcom_p2p_func_cancel(0);
            A_MDELAY(500);
            qcom_set_deviceid(1);
            qcom_set_channel(1, cur_channel);
            A_MDELAY(50);
        }
    }
    
     if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
     return(Custom_Api_Mediactl(p_Driver_Cxt[device_id],ENET_SET_MEDIACTL_COMMIT,NULL));

#else  
     if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
     return(Custom_Api_Mediactl(p_Driver_Cxt[device_id],ENET_SET_MEDIACTL_COMMIT,NULL));
#endif  //ENABLE_SCC_MODE  
}

A_STATUS qcom_sta_get_rssi(A_UINT8 device_id, A_UINT8 *prssi)
{
    A_STATUS error = A_OK;
    ATH_IOCTL_PARAM_STRUCT inout_param;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    inout_param.cmd_id = ATH_GET_RX_RSSI;
    inout_param.data = prssi;
    
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
    }
    return error;  
}

/* for hostless , the listentime is 1Tus, 1Tus = 1024 us. need to confirm the UNIT */
A_STATUS qcom_sta_set_listen_time(A_UINT8 device_id, A_UINT32 listentime) 
{
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_STATUS error = A_OK;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    inout_param.cmd_id = ATH_SET_LISTEN_INT;
    inout_param.data = &listentime;
    inout_param.length = 4;

    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
    }
    return error;  
}

A_STATUS qcom_ap_set_beacon_interval(A_UINT8 device_id, A_UINT16 beacon_interval)
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    ATH_AP_PARAM_STRUCT ap_param;
    A_STATUS error = A_OK;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    inout_param.cmd_id = ATH_CONFIG_AP;
    ap_param.cmd_subset = AP_SUB_CMD_BCON_INT;
    ap_param.data		= &beacon_interval;
    inout_param.data	= &ap_param;

    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
    }
    return error;  
}

A_STATUS qcom_ap_hidden_mode_enable(A_UINT8 device_id, A_BOOL enable)
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    ATH_AP_PARAM_STRUCT ap_param;
    A_STATUS error = A_OK;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    inout_param.cmd_id = ATH_CONFIG_AP;
    ap_param.cmd_subset = AP_SUB_CMD_HIDDEN_FLAG;
    ap_param.data	= &enable;
    inout_param.data	= &ap_param;

    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
    }
    return error;  
}


A_STATUS qcom_op_get_mode(A_UINT8 device_id, A_UINT32 *pmode)
{
    A_UINT32 inout_param  = 0;
    A_STATUS error = A_OK;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    if(Custom_Api_Mediactl(p_Driver_Cxt[device_id],ENET_GET_MEDIACTL_MODE,&inout_param) != A_OK){
        return A_ERROR;
    }
    switch(inout_param)
    {
        case ENET_MEDIACTL_MODE_INFRA:
            *pmode = QCOM_WLAN_DEV_MODE_STATION;
            break;
        case ENET_MEDIACTL_MODE_ADHOC:
            *pmode = QCOM_WLAN_DEV_MODE_ADHOC;
            break;
        case ENET_MEDIACTL_MODE_MASTER:
            *pmode = QCOM_WLAN_DEV_MODE_AP;
            break;
        default:
              error = A_ERROR;
    }
    
    return error;
}


 A_STATUS qcom_op_set_mode(A_UINT8 device_id, A_UINT32 mode)  
 {
    A_UINT32 inout_param  = 0;
    A_STATUS error = A_OK;
  
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    if(mode == QCOM_WLAN_DEV_MODE_STATION)
    {
        inout_param = ENET_MEDIACTL_MODE_INFRA;
    }
    else if(mode == QCOM_WLAN_DEV_MODE_ADHOC)
    {
        inout_param = ENET_MEDIACTL_MODE_ADHOC;
    }
    else if(mode == QCOM_WLAN_DEV_MODE_AP)
    { 
        inout_param = ENET_MEDIACTL_MODE_MASTER;
    }
    else
    {
       return A_ERROR;
    }

    if( Custom_Api_Mediactl(p_Driver_Cxt[device_id],ENET_SET_MEDIACTL_MODE,&inout_param) != A_OK)
        return A_ERROR;
    
    return error;  

}

A_STATUS qcom_disconnect(A_UINT8 device_id)
{
   A_STATUS error = A_OK;
   
   if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
   /* set ssid to be blank string , ssid length is 0 , the fist step to disconnectl*/
   if(qcom_set_ssid(device_id,"") != A_OK)
    {
       return A_ERROR;
    }
   /* a zero length ssid + a COMMIT command is interpreted as a
   	request from the caller to disconnect.*/
   if(qcom_commit(device_id)!= A_OK)
   {
       return A_ERROR;
   }
   return error;
 }


A_STATUS qcom_get_phy_mode(A_UINT8 device_id, A_UINT8 *pphymode)
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    A_STATUS error = A_OK;
    A_UINT32 value = 0;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    inout_param.cmd_id = ATH_GET_PHY_MODE;
    inout_param.data = &value;

    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
        return error;
    }

    if(*(A_UINT32*)(inout_param.data) == 0x01)
    {
    	strcpy((char *)pphymode,"a");
    }
    else if(*(A_UINT32*)(inout_param.data) == 0x02)
    {
    	strcpy((char *)pphymode,"g");
    }
    else if(*(A_UINT32*)(inout_param.data) == 0x04)
    {
    	strcpy((char *)pphymode,"b");
    }
    else
    {
    	strcpy((char *)pphymode,"mixed");
    }
    return A_OK;
}


A_STATUS qcom_set_phy_mode(A_UINT8 device_id, A_UINT8 phymode)
{
#define BUF_LEN 20
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_STATUS error = A_OK;
    char p[BUF_LEN];

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    if (phymode ==QCOM_11A_MODE) strcpy(p,"a");
    else if (phymode ==QCOM_11B_MODE) strcpy(p,"b");
    else if (phymode ==QCOM_11G_MODE) strcpy(p,"g");
    else if (phymode ==QCOM_11N_MODE) strcpy(p,"n");
    else if (phymode ==QCOM_HT40_MODE) strcpy(p,"ht40");
    
    inout_param.cmd_id = ATH_SET_PHY_MODE;
    inout_param.data = p;    
    inout_param.length = strlen(p);

    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
    }
    return error;  
}
 

A_STATUS qcom_set_channel(A_UINT8 device_id, A_UINT16 channel)
{
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_STATUS error = A_OK;
  
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }

    if(channel < 1 || channel > 165)
    {
      return A_ERROR;
    }
    if (channel < 27) {
        channel = ATH_IOCTL_FREQ_1 + (channel-1)*5;
    } else {
        channel = (5000 + (channel*5));
    }

    inout_param.cmd_id = ATH_SET_CHANNEL;
    inout_param.data = &channel;
    inout_param.length = 4;

    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
    }
    return error;
}

A_STATUS qcom_get_channel(A_UINT8 device_id, A_UINT16 *pchannel)
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    A_STATUS error = A_OK;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    inout_param.cmd_id = ATH_GET_CHANNEL;
    inout_param.data = pchannel;


    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
    }
    return error;  
}

A_STATUS qcom_get_tx_power(A_UINT8 device_id, A_UINT32 *pdbm)
{
/* No command id for get tx_power */
   return A_OK;
} 

A_STATUS qcom_set_tx_power(A_UINT8 device_id, A_UINT32 dbm)
{
     ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_STATUS error = A_OK;
    A_UINT8 pdBm;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    pdBm = (A_UINT8)dbm;
    inout_param.cmd_id = ATH_SET_TXPWR;
    inout_param.data = &pdBm;
    inout_param.length = 1;//strlen(dBm);

    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
    }
    return error;
}

A_STATUS qcom_allow_aggr_set_tid(A_UINT8 device_id, A_UINT16 tx_allow_aggr, A_UINT16 rx_allow_aggr)
{
    A_STATUS error = A_OK;
    ATH_SET_AGGREGATION_PARAM param;
    ATH_IOCTL_PARAM_STRUCT inout_param;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
	
    param.txTIDMask = tx_allow_aggr;
    param.rxTIDMask = rx_allow_aggr;

    inout_param.cmd_id = ATH_SET_AGGREGATION;
    inout_param.data = &param;
    inout_param.length = sizeof(ATH_SET_AGGREGATION_PARAM);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
      error = A_ERROR;
    }
    return error;  
}

A_STATUS qcom_scan_set_mode(A_UINT8 device_id, A_UINT32 mode)
{
    A_STATUS error = A_OK;
    ATH_IOCTL_PARAM_STRUCT inout_param;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    inout_param.cmd_id = ATH_SCAN_CTRL;
    inout_param.data = &mode;
    inout_param.length = 4;

    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
       error = A_ERROR;
    }
    return error;  
}

A_STATUS qcom_scan_params_set(A_UINT8 device_id, qcom_scan_params_t *pParam)
{
    A_STATUS error = A_OK;
    ATH_IOCTL_PARAM_STRUCT inout_param;
    WMI_SCAN_PARAMS_CMD scan_param;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    scan_param.fg_start_period = pParam->fgStartPeriod;
    scan_param.fg_end_period = pParam->fgEndPeriod;
    scan_param.bg_period = pParam->bgPeriod;
    scan_param.maxact_chdwell_time = pParam->maxActChDwellTimeInMs;
    scan_param.pas_chdwell_time = pParam->pasChDwellTimeInMs;
    scan_param.shortScanRatio = pParam->shortScanRatio;
    scan_param.scanCtrlFlags = pParam->scanCtrlFlags;
    scan_param.minact_chdwell_time = pParam->minActChDwellTimeInMs;
    scan_param.maxact_scan_per_ssid = pParam->maxActScanPerSsid;
    scan_param.max_dfsch_act_time = pParam->maxDfsChActTimeInMs;
    
    inout_param.cmd_id = ATH_SET_SCAN_PARAM;
    inout_param.data = &scan_param;
    inout_param.length = sizeof(WMI_SCAN_PARAMS_CMD);

    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
       error = A_ERROR;
    }
    return error;  
}

/***************************************************************************************
* QCOM SECURITY APIs
***************************************************************************************/
/*FUNCTION*-----------------------------------------------------------------
*
* Function Name  : set_wep_key
* Returned Value : A_ERROR on error else A_OK
* Comments       : Store WEP key for later use. Size of Key must be 10 or 26
*                  Hex characters
*
*END------------------------------------------------------------------*/
A_STATUS qcom_sec_set_wepkey(A_UINT8 device_id, A_UINT32 keyindex, A_CHAR *pkey)
{
    A_UINT8  val;
    A_UINT32 len = strlen(pkey);
    A_DRIVER_CONTEXT *pDCxt;
    A_VOID *pCxt;
    if((pCxt =  ((ENET_CONTEXT_STRUCT_PTR) p_Driver_Cxt[device_id])->MAC_CONTEXT_PTR) == NULL)
    {
	return A_ERROR;
    }

    pDCxt = GET_DRIVER_COMMON(pCxt);
    
    keyindex--;
    
    /* copy wep keys to driver context for use later during connect */
    if(	(keyindex > WMI_MAX_KEY_INDEX) ||
	/* user passes in num digits as keyLength */
	(len != WEP_SHORT_KEY*2 && len != WEP_LONG_KEY*2) &&
	/* user passes in num digits as keyLength */
	(len != WEP_SHORT_KEY &&  len != WEP_LONG_KEY))
    {
	return A_ERROR;
    }

    if((len == WEP_SHORT_KEY) || (len == WEP_LONG_KEY))
    {
	A_MEMCPY(pDCxt->conn[device_id].wepKeyList[keyindex].key, pkey, len);
    	pDCxt->conn[device_id].wepKeyList[keyindex].keyLen = (A_UINT8) len;
    }
    else
    {
	pDCxt->conn[device_id].wepKeyList[keyindex].keyLen = (A_UINT8)(len>>1);
	A_MEMZERO(pDCxt->conn[device_id].wepKeyList[keyindex].key, MAX_WEP_KEY_SZ);
	/* convert key data from string to bytes */
	for(int ii=0 ; ii<len ; ii++){
	    if((val = ascii_to_hex(pkey[ii]))==0xff){
		return A_ERROR;
	    }
	    if((ii&1) == 0){
	        val <<= 4;
	    }
	    pDCxt->conn[device_id].wepKeyList[keyindex].key[ii>>1] |= val;
        }
    }

    return A_OK;  
}

A_STATUS qcom_sec_get_wepkey(A_UINT8 device_id, A_UINT32 keyindex, A_CHAR *pkey)
{
    A_DRIVER_CONTEXT *pDCxt;
    A_VOID *pCxt;
    if((pCxt =  ((ENET_CONTEXT_STRUCT_PTR) p_Driver_Cxt[device_id])->MAC_CONTEXT_PTR) == NULL)
    {
	return A_ERROR;
    }

    pDCxt = GET_DRIVER_COMMON(pCxt);
    
    if( keyindex < 1 || keyindex > WMI_MAX_KEY_INDEX+1)
      return A_ERROR;
  
    keyindex--;
    
    A_MEMCPY(pkey, pDCxt->conn[device_id].wepKeyList[keyindex].key, pDCxt->conn[device_id].wepKeyList[keyindex].keyLen);    
    return A_OK;    
}

A_STATUS qcom_sec_set_wepkey_index(A_UINT8 device_id, A_UINT32 keyindex)
{
    A_DRIVER_CONTEXT *pDCxt;
    A_VOID *pCxt;
    uint32_t inout_param;

    if((pCxt =  ((ENET_CONTEXT_STRUCT_PTR) p_Driver_Cxt[device_id])->MAC_CONTEXT_PTR) == NULL)
    {
	return A_ERROR;
    }    
    pDCxt = GET_DRIVER_COMMON(pCxt);
    
    if( ((keyindex - 1) < WMI_MIN_KEY_INDEX) ||
        ((keyindex - 1) > WMI_MAX_KEY_INDEX) )
      return A_ERROR;
    
    pDCxt->conn[device_id].wepDefTxKeyIndex = (A_UINT8)(keyindex-1);
 		 
    inout_param = ENET_MEDIACTL_SECURITY_TYPE_WEP;
    return (Custom_Api_Mediactl(p_Driver_Cxt[device_id],ENET_SET_MEDIACTL_SEC_TYPE,&inout_param));
}
         
A_STATUS qcom_sec_get_wepkey_index(A_UINT8 device_id, A_UINT32 *pkeyindex)
{
    A_DRIVER_CONTEXT *pDCxt;
    A_VOID *pCxt;

    if((pCxt =  ((ENET_CONTEXT_STRUCT_PTR) p_Driver_Cxt[device_id])->MAC_CONTEXT_PTR) == NULL)
    {
	return A_ERROR;
    }     
    pDCxt = GET_DRIVER_COMMON(pCxt);
    
    *pkeyindex = (A_UINT32) (pDCxt->conn[device_id].wepDefTxKeyIndex + 1);
    return A_OK;    
}  

A_STATUS qcom_sec_set_auth_mode(A_UINT8 device_id, A_UINT32 mode)
{
    uint32_t inout_param;
    A_STATUS error = A_OK;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    switch(mode) {
    case WLAN_AUTH_NONE:
        inout_param = ENET_MEDIACTL_SECURITY_TYPE_NONE;
        break;
    case WLAN_AUTH_WPA_PSK:
        inout_param = ENET_MEDIACTL_SECURITY_TYPE_WPA;
        break;
    case WLAN_AUTH_WPA2_PSK:
        inout_param = ENET_MEDIACTL_SECURITY_TYPE_WPA2;
        break;
    default:
   	error = A_ERROR;
   	break;
    }
   
    if( error == A_OK )
        error = Custom_Api_Mediactl(p_Driver_Cxt[device_id],ENET_SET_MEDIACTL_SEC_TYPE,&inout_param);

    return error;    
}

A_STATUS qcom_sec_set_encrypt_mode(A_UINT8 device_id, A_UINT32 mode)
{
    cipher_t cipher_mode;
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_STATUS error = A_OK;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    cipher_mode.ucipher = 0x1 << mode;
    cipher_mode.mcipher = cipher_mode.ucipher;

    inout_param.cmd_id = ATH_SET_CIPHER;
    inout_param.data = &cipher_mode;
    inout_param.length = sizeof(cipher_t);

    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
    }
    return error;
}

A_STATUS qcom_sec_set_passphrase(A_UINT8 device_id, A_CHAR *passphrase)
{
    ENET_MEDIACTL_PARAM param;
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    param.data = passphrase;
    param.length = strlen(passphrase);
    return(Custom_Api_Mediactl(p_Driver_Cxt[device_id],ENET_SET_MEDIACTL_PASSPHRASE,&param));
}

A_STATUS qcom_sec_set_pmk(A_UINT8 device_id, A_CHAR *pmk)
{
    int j;
    A_UINT8 val = 0;
    WMI_SET_PMK_CMD cmd;
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_STATUS error = A_OK;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    A_MEMZERO(cmd.pmk, WMI_PMK_LEN);
    cmd.pmk_len = WMI_PMK_LEN;

    for(j=0; j<64; j++)
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

    inout_param.cmd_id = ATH_SET_PMK;
    inout_param.data = &cmd;
    inout_param.length = sizeof(cmd);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
    }
    return error;
}

A_STATUS qcom_power_set_mode(A_UINT8 device_id, A_UINT32 powerMode, A_UINT8 powerModule)
{
    ATH_IOCTL_PARAM_STRUCT param;
    POWER_MODE power;
    A_STATUS error = A_OK;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    power.pwr_module = powerModule;
    power.pwr_mode   = powerMode;
    
    param.cmd_id = ATH_SET_POWER;
    param.data = (void*)&power;
    param.length = sizeof(POWER_MODE);
    
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }

    return error;
}

A_STATUS qcom_power_get_mode(A_UINT8 device_id, A_UINT32 *powerMode)
{	
    ENET_MEDIACTL_PARAM inout_param;
    A_STATUS error;
    
    error = Custom_Api_Mediactl(p_Driver_Cxt[device_id],ENET_GET_MEDIACTL_POWER,&inout_param);
    if (A_OK != error)
    {
        return A_ERROR;
    }
    
    if(inout_param.value == 1)
    {
    	*powerMode = REC_POWER;
    }
    else if(inout_param.value == 0)
    {
    	*powerMode = MAX_PERF_POWER;
    }
    else
    {
    	*powerMode = 0;
    }
    return error;
}

A_STATUS qcom_suspend_enable(A_BOOL enable)
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    A_STATUS error=A_OK;
    
    inout_param.cmd_id = ATH_DEVICE_SUSPEND_ENABLE;
    inout_param.data = NULL;
    inout_param.length = 0;

    if(ath_ioctl_handler(p_Driver_Cxt[0],&inout_param) != A_OK){
        error = A_ERROR;
    }

    return error;
}

A_STATUS qcom_suspend_start(A_UINT32 susp_time)
{
    A_UINT32 suspend_time;
    ATH_IOCTL_PARAM_STRUCT inout_param;
    A_STATUS error = A_OK;

    suspend_time = susp_time;
    inout_param.cmd_id = ATH_DEVICE_SUSPEND_START;
    inout_param.data = &suspend_time;
    inout_param.length = sizeof(A_UINT32);

    if(ath_ioctl_handler(p_Driver_Cxt[0],&inout_param) != A_OK){
        error = A_ERROR;
    }

    return error;
}

A_STATUS qcom_power_set_parameters(A_UINT8 device_id, 
                                   A_UINT16 idlePeriod,
                                   A_UINT16 psPollNum,
                                   A_UINT16 dtimPolicy,
                                   A_UINT16 tx_wakeup_policy,
                                   A_UINT16 num_tx_to_wakeup,
                                   A_UINT16 ps_fail_event_policy)  
{
    ATH_WMI_POWER_PARAMS_CMD pm;
    ATH_IOCTL_PARAM_STRUCT  inout_param;
    A_STATUS error = A_OK;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    pm.idle_period = idlePeriod;
    pm.pspoll_number = psPollNum;
    pm.dtim_policy = dtimPolicy;
    pm.tx_wakeup_policy = tx_wakeup_policy;
    pm.num_tx_to_wakeup = num_tx_to_wakeup;
    pm.ps_fail_event_policy = ps_fail_event_policy;   
      
    inout_param.cmd_id = ATH_SET_PMPARAMS;
    inout_param.data = &pm;
    inout_param.length = sizeof(ATH_WMI_POWER_PARAMS_CMD);

    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
    }

    return error;
}

A_STATUS qcom_get_bssid(A_UINT8 device_id, A_UINT8 mac_addr[ATH_MAC_LEN])
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    A_STATUS error = A_OK;

    inout_param.cmd_id = ATH_GET_MACADDR;
    inout_param.data = mac_addr;
    inout_param.length = ATH_MAC_LEN;
    
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
            error = A_ERROR;
    }
    return error;
}

/***************************************************************************************************************
* QCOM WPS APIs
****************************************************************************************************************/


A_STATUS qcom_wps_start(A_UINT8 device_id, int connect, int use_pinmode, char *pin)
{
    extern A_UINT16 channel_array[];  
#define WPS_STANDARD_TIMEOUT (30)
    ATH_IOCTL_PARAM_STRUCT param;
    ATH_WPS_START wps_start;
    A_STATUS error = A_OK;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    do{
        A_MEMZERO(&wps_start, sizeof(ATH_WPS_START));
        param.cmd_id = ATH_START_WPS;
        param.data = &wps_start;
        param.length = sizeof(wps_start);

        wps_start.connect_flag = connect;
        if(use_pinmode == 0){
            wps_start.wps_mode = ATH_WPS_MODE_PIN;
            wps_start.pin_length = strlen (pin);
            //FIXME: This hardcoded pin value needs to be changed
            // for production to reflect what is on a sticker/label
            memcpy (wps_start.pin, pin, wps_start.pin_length+1);
        }else{
            wps_start.wps_mode = ATH_WPS_MODE_PUSHBUTTON;
        }
        wps_start.timeout_seconds = WPS_STANDARD_TIMEOUT;

        if(gWpsCredentials.ssid_len == 0)
        {
            wps_start.ssid_info.ssid_len = 0;
        }
        else
        {
            memcpy(wps_start.ssid_info.ssid,gWpsCredentials.ssid,sizeof(wps_start.ssid_info.ssid));
            memcpy(wps_start.ssid_info.macaddress,gWpsCredentials.mac_addr,ATH_MAC_LEN);
            wps_start.ssid_info.channel = gWpsCredentials.ap_channel;
            wps_start.ssid_info.ssid_len = gWpsCredentials.ssid_len;
        }

        /* this starts WPS on the Aheros wifi */
        if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
            error = A_ERROR;
        }
    }while(0);

    return error;
}

A_STATUS qcom_wps_connect(A_UINT8 device_id)
{
    qcom_set_ssid(device_id, (char *)gWpsCredentials.ssid);
    if(gWpsCredentials.key_idx != 0 )
    {
        qcom_sec_set_wepkey(device_id, gWpsCredentials.key_idx, (char *) gWpsCredentials.key); 
        qcom_sec_set_wepkey_index(device_id, gWpsCredentials.key_idx);
    } else if( gWpsCredentials.auth_type != WLAN_AUTH_NONE ) {
	qcom_sec_set_auth_mode(device_id, gWpsCredentials.auth_type);
        qcom_sec_set_encrypt_mode(device_id, gWpsCredentials.encr_type);
    }
              
    return qcom_commit(device_id);
}

A_STATUS qcom_wps_set_credentials(A_UINT8 device_id, qcom_wps_credentials_t *pwps_prof)
{
    /* save wps credentials */
    A_MEMZERO(&gWpsCredentials, sizeof(qcom_wps_credentials_t));
    if(pwps_prof != NULL)
        A_MEMCPY(&gWpsCredentials, pwps_prof, sizeof(qcom_wps_credentials_t)); 
    return A_OK;
}

#if ENABLE_P2P_MODE

A_STATUS qcom_p2p_enable(A_UINT8 device_id, P2P_DEV_CTXT *dev, A_INT32 enable)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    WMI_P2P_SET_PROFILE_CMD p2p;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    A_MEMZERO(&p2p,sizeof(WMI_P2P_SET_PROFILE_CMD));
    p2p.enable = enable;
 
    param.cmd_id = ATH_P2P_SWITCH;
    param.data =  &p2p;
    param.length = sizeof(p2p);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}

static A_STATUS  qcom_p2p_group_init(A_UINT8 device_id, A_UINT8 persistent_group, A_UINT8 group_formation)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    WMI_P2P_GRP_INIT_CMD grpInit;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    A_MEMZERO(&grpInit ,sizeof(WMI_P2P_GRP_INIT_CMD));
    grpInit.group_formation = group_formation;
    grpInit.persistent_group = persistent_group;
 
    param.cmd_id = ATH_P2P_APMODE;
    param.data =  &grpInit;
    param.length = sizeof(grpInit);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}

static A_STATUS qcom_set_ap_params(A_UINT8 device_id, A_UINT16 param_cmd, A_UINT8 *data_val)
{
    ATH_IOCTL_PARAM_STRUCT inout_param;
    ATH_AP_PARAM_STRUCT ap_param;
    A_STATUS error = A_OK;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    inout_param.cmd_id = ATH_CONFIG_AP;
    ap_param.cmd_subset = param_cmd;
    ap_param.data	= data_val;
    inout_param.data	= &ap_param;

    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&inout_param) != A_OK){
        error = A_ERROR;
    }
    return error;
}

A_STATUS qcom_p2p_func_start_go(A_UINT8 device_id, A_UINT8 *pssid, A_UINT8 *ppass, A_INT32 channel, A_BOOL ucpersistent)
{
    A_STATUS error = A_OK;
    A_UINT8 wps_flag;

    qcom_sec_set_auth_mode(device_id, WLAN_AUTH_WPA2_PSK);
    qcom_sec_set_encrypt_mode(device_id, WLAN_CRYPT_AES_CRYPT);
    if( pssid != NULL )
    {
        qcom_p2p_func_set_pass_ssid(device_id, ppass, pssid); 
    }
    
    qcom_op_set_mode (device_id,QCOM_WLAN_DEV_MODE_AP);

    if( channel != P2P_AUTO_CHANNEL )
    {
        qcom_set_channel(device_id, channel);
    }          

    wps_flag = 0x01;
    if (qcom_set_ap_params(device_id, AP_SUB_CMD_WPS_FLAG, &wps_flag) != A_OK)
    {
        return A_ERROR;
    }

    qcom_op_set_mode (device_id,QCOM_WLAN_DEV_MODE_AP);

    if(qcom_p2p_group_init(device_id, ucpersistent, 1) != A_OK)
    {
        return A_ERROR;
    }

    /* Set MAX PERF */
    qcom_power_set_mode(device_id, MAX_PERF_POWER, P2P);
    return error;
}


A_STATUS qcom_p2p_func_init(A_UINT8 device_id, A_INT32 enable)
{
    static A_INT32 p2p_enable_flag = 0xFEFE;

    if (enable == p2p_enable_flag) {
        return A_OK;
    }
    p2p_enable_flag = enable;
      
    qcom_p2p_enable(device_id, NULL, enable);
    qcom_p2p_func_set_config(device_id, 0, 1, 1, 3000, 81, 81, 5);

    return A_OK;
}

A_STATUS qcom_p2p_func_find(A_UINT8 device_id, void *dev, A_UINT8 type, A_UINT32 timeout)
{
    WMI_P2P_FIND_CMD find_params;    
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    find_params.type = type;
    find_params.timeout = A_CPU2LE32(timeout);
    param.cmd_id = ATH_P2P_FIND;
    param.data = &find_params;
    param.length = sizeof(WMI_P2P_FIND_CMD);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}
     
A_STATUS qcom_p2p_func_set_pass_ssid(A_UINT8 device_id, A_CHAR *ppass, A_CHAR *pssid)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    WMI_SET_PASSPHRASE_CMD  setPassPhrase;

    if( qcom_set_ssid(device_id, pssid) != A_OK )
      return A_ERROR;
    
    strcpy((char *)setPassPhrase.passphrase, ppass);
    setPassPhrase.passphrase_len = strlen(ppass);    
    strcpy((char *)setPassPhrase.ssid, pssid);
    setPassPhrase.ssid_len = strlen(pssid);    
    
    param.cmd_id = ATH_P2P_APMODE_PP;
    param.data = &setPassPhrase;
    param.length = sizeof(WMI_SET_PASSPHRASE_CMD);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    
    return error;
}

A_STATUS qcom_p2p_func_listen(A_UINT8 device_id, A_UINT32 timeout)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    A_UINT32 tout;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    tout = timeout;
    param.cmd_id = ATH_P2P_LISTEN;
    param.data = &tout;
    param.length = sizeof(tout);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}                           

A_STATUS qcom_p2p_func_connect(A_UINT8 device_id, P2P_WPS_METHOD wps_method, A_UINT8 *peer_mac, A_BOOL persistent)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    WMI_P2P_FW_CONNECT_CMD_STRUCT p2p_connect;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    A_MEMZERO(&p2p_connect, sizeof( WMI_P2P_FW_CONNECT_CMD_STRUCT));
    p2p_connect.wps_method = wps_method;
    A_MEMCPY(p2p_connect.peer_addr, peer_mac, ATH_MAC_LEN);
    
    /* Setting Default Value for now !!! */
    p2p_connect.dialog_token = 1;
    p2p_connect.go_intent = 0;
    p2p_connect.go_dev_dialog_token = 0;
    p2p_connect.dev_capab = 0x23;
    if(persistent)
    {  
        p2p_connect.dev_capab |= P2P_PERSISTENT_FLAG;
    }
    
    param.cmd_id = ATH_P2P_CONNECT_CLIENT;
    param.data = &p2p_connect;
    param.length = sizeof(WMI_P2P_FW_CONNECT_CMD_STRUCT);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}                           

A_STATUS qcom_p2p_func_auth(A_UINT8 device_id, A_INT32 dev_auth, P2P_WPS_METHOD wps_method, A_UINT8 *peer_mac, A_BOOL persistent)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    WMI_P2P_FW_CONNECT_CMD_STRUCT p2p_connect;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    A_MEMZERO(&p2p_connect, sizeof( WMI_P2P_FW_CONNECT_CMD_STRUCT));
    
    if( persistent)
      p2p_connect.dev_capab |= P2P_PERSISTENT_FLAG;
    p2p_connect.dev_auth = dev_auth;
    p2p_connect.wps_method = wps_method;
    A_MEMCPY(p2p_connect.peer_addr, peer_mac, ATH_MAC_LEN);
    /* If go_intent <= 0, wlan firmware will use the intent value configured via
     * qcom_p2p_set
     */
    p2p_connect.go_intent = 0;

    param.cmd_id = ATH_P2P_AUTH;
    param.data =  &p2p_connect;
    param.length = sizeof(WMI_P2P_FW_CONNECT_CMD_STRUCT);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   

A_STATUS qcom_p2p_func_cancel(A_UINT8 device_id)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;

    if(qcom_set_deviceid(device_id) == A_ERROR){
            return A_ERROR;
        }
        
    param.cmd_id = ATH_P2P_CANCEL;
    param.data = NULL;
    param.length = 0;
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   

A_STATUS qcom_p2p_func_stop_find(A_UINT8 device_id)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    param.cmd_id = ATH_P2P_STOP;
    param.data = NULL;
    param.length = 0;
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   

/* NODE_LIST */
A_STATUS qcom_p2p_func_get_node_list(A_UINT8 device_id, void *app_buf, A_UINT32 buf_size)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    param.cmd_id = ATH_P2P_NODE_LIST;
    param.data = app_buf;
    param.length = buf_size;
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   

A_STATUS qcom_p2p_func_set_config(A_UINT8 device_id, A_UINT32 uigo_intent, A_UINT32 uiclisten_ch, A_UINT32 uiop_ch, A_UINT32 uiage, A_UINT32 reg_class, A_UINT32 op_reg_class, A_UINT32 max_node_count)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    WMI_P2P_FW_SET_CONFIG_CMD stp2p_cfg_cmd;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    A_MEMZERO(&stp2p_cfg_cmd, sizeof(stp2p_cfg_cmd));
    stp2p_cfg_cmd.go_intent = uigo_intent;
    stp2p_cfg_cmd.reg_class = reg_class;
    stp2p_cfg_cmd.op_reg_class = op_reg_class;
    stp2p_cfg_cmd.op_channel = uiop_ch;
    stp2p_cfg_cmd.listen_channel = uiclisten_ch;
    stp2p_cfg_cmd.node_age_to    = uiage;
    stp2p_cfg_cmd.max_node_count = max_node_count;
    
    param.cmd_id = ATH_P2P_SET_CONFIG;
    param.data = &stp2p_cfg_cmd;
    param.length = sizeof(WMI_P2P_FW_SET_CONFIG_CMD);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   


A_STATUS qcom_p2p_func_wps_config(A_UINT8 device_id, A_UINT8 *p2p_ptr)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    
     if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    param.cmd_id = ATH_P2P_WPS_CONFIG;
    param.data = p2p_ptr;
    param.length = sizeof(WMI_WPS_SET_CONFIG_CMD);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   

A_STATUS qcom_p2p_func_disc_req(A_UINT8 device_id, A_UINT8 *p2p_ptr)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    
    param.cmd_id = ATH_P2P_DISC_REQ;
    param.data = p2p_ptr;
    param.length = sizeof(WMI_P2P_FW_PROV_DISC_REQ_CMD);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   

A_STATUS qcom_p2p_set(A_UINT8 device_id, P2P_CONF_ID config_id, void *data, A_UINT32 data_length)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    WMI_P2P_SET_CMD    p2p_set_params;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    A_MEMZERO(&p2p_set_params, sizeof(p2p_set_params));

    switch (config_id) {
        case P2P_CONFIG_LISTEN_CHANNEL:
            config_id = WMI_P2P_CONFID_LISTEN_CHANNEL;
            break;
        case P2P_CONFIG_CROSS_CONNECT:
            config_id = WMI_P2P_CONFID_CROSS_CONNECT;
            break;
        case P2P_CONFIG_SSID_POSTFIX:
            config_id = WMI_P2P_CONFID_SSID_POSTFIX;
            break;
        case P2P_CONFIG_INTRA_BSS:
            config_id = WMI_P2P_CONFID_INTRA_BSS;
            break;
        case P2P_CONFIG_CONCURRENT_MODE:
            config_id = WMI_P2P_CONFID_CONCURRENT_MODE;
            break;
        case P2P_CONFIG_GO_INTENT:
            config_id = WMI_P2P_CONFID_GO_INTENT;
            break;
        case P2P_CONFIG_DEV_NAME:
            config_id = WMI_P2P_CONFID_DEV_NAME;
            break;
        case P2P_CONFIG_P2P_OPMODE:
            config_id = WMI_P2P_CONFID_P2P_OPMODE;
            break;
        case P2P_CONFIG_CCK_RATES:
            config_id = WMI_P2P_CONFID_CCK_RATES;
            break;
        default:
            /* Unknown parameter */
            return A_ERROR;
    }
    
    p2p_set_params.config_id = config_id;
    A_MEMCPY(&p2p_set_params.val, data, data_length);   
    
    param.cmd_id = ATH_P2P_SET;
    param.data = &p2p_set_params;
    param.length = sizeof(WMI_P2P_SET_CMD);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   

A_STATUS qcom_p2p_func_join(A_UINT8 device_id, P2P_WPS_METHOD wps_method, A_UINT8 *pmac, char *ppin, A_UINT16 channel)
{
    A_UINT16 conc_channel;
    WMI_P2P_FW_CONNECT_CMD_STRUCT p2p_join_profile;
    WMI_P2P_PROV_INFO  p2p_info;
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;

#if ENABLE_SCC_MODE
    int num_dev = WLAN_NUM_OF_DEVICES;
    if( (num_dev > 1) && (device_id == 0))
    {
        qcom_get_concurrent_channel(device_id, &conc_channel);
        if( (conc_channel != 0)&& (channel == 0)){
	    return A_ERROR;
        }            
        if( (conc_channel != 0) && (channel != conc_channel ) )
        {
	    return A_ERROR;
        }
    }
#endif /* ENABLE_SCC_MODE */    
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }

    A_MEMZERO(&p2p_join_profile,sizeof( WMI_P2P_FW_CONNECT_CMD_STRUCT));
    A_MEMCPY(p2p_join_profile.peer_addr, pmac, ATH_MAC_LEN);
    p2p_join_profile.go_oper_freq = channel;
    p2p_join_profile.wps_method = wps_method;

    if( wps_method != WPS_PBC ) {
        strcpy(p2p_info.wps_pin, ppin);
        wmi_save_key_info(&p2p_info);
    }

    param.cmd_id = ATH_P2P_JOIN;
    param.data = &p2p_join_profile;
    param.length = sizeof(WMI_P2P_FW_CONNECT_CMD_STRUCT);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}


A_STATUS qcom_p2p_func_invite_auth(A_UINT8 device_id, A_UINT8 *inv)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    param.cmd_id = ATH_P2P_INVITE_AUTH;
    param.data = inv;
    param.length = sizeof(WMI_P2P_FW_INVITE_REQ_RSP_CMD);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   

/* LIST_NETWORK */
A_STATUS qcom_p2p_func_get_network_list(A_UINT8 device_id, void *app_buf, A_UINT32 buf_size)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    param.cmd_id = ATH_P2P_PERSISTENT_LIST;
    param.data = app_buf;
    param.length = buf_size; 
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   

A_STATUS qcom_p2p_func_invite(A_UINT8 device_id, A_CHAR *pssid, P2P_WPS_METHOD wps_method, A_UINT8 *pmac, A_BOOL persistent, P2P_INV_ROLE role)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    WMI_P2P_INVITE_CMD  p2pInvite;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    
    A_MEMZERO(&p2pInvite, sizeof(p2pInvite));    

    A_MEMCPY(p2pInvite.ssid.ssid, pssid, strlen(pssid));
    p2pInvite.ssid.ssidLength = strlen(pssid);
    A_MEMCPY(p2pInvite.peer_addr, pmac, ATH_MAC_LEN);
    p2pInvite.wps_method = wps_method;
    p2pInvite.is_persistent = persistent;
    p2pInvite.role = role;
    
    param.cmd_id = ATH_P2P_INVITE;
    param.data = &p2pInvite;
    param.length = sizeof(WMI_P2P_INVITE_CMD);
       
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   

A_STATUS qcom_p2p_func_join_profile(A_UINT8 device_id, A_UINT8 *pmac)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    WMI_P2P_FW_CONNECT_CMD_STRUCT  p2p_connect;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    A_MEMZERO(&p2p_connect,sizeof(p2p_connect));
    A_MEMCPY(p2p_connect.peer_addr, pmac, ATH_MAC_LEN);       
    
    param.cmd_id = ATH_P2P_JOIN_PROFILE;
    param.data = &p2p_connect;
    param.length = sizeof(WMI_P2P_FW_CONNECT_CMD_STRUCT);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   


A_STATUS qcom_p2p_ap_mode(A_UINT8 device_id, A_UINT8 *p2p_ptr)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    param.cmd_id = ATH_P2P_APMODE;
    param.data = p2p_ptr;
    param.length = sizeof(WMI_P2P_GRP_INIT_CMD);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   


A_STATUS qcom_p2p_func_on_off(A_UINT8 device_id, A_UINT8 *p2p_ptr)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    param.cmd_id = ATH_P2P_SWITCH;
    param.data = p2p_ptr;
    param.length = sizeof(WMI_P2P_SET_PROFILE_CMD);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   

A_STATUS qcom_p2p_func_set_noa(A_UINT8 device_id, A_UINT8 uccount, A_UINT32 uistart, A_UINT32 uiduration, A_UINT32 uiinterval)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    P2P_NOA_INFO p2p_noa_desc;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    if (uccount >= P2P_MAX_NOA_DESCRIPTORS) {
        return A_ERROR;
    }

    A_MEMZERO(&p2p_noa_desc, sizeof(p2p_noa_desc));
    if (uccount > 0) {
        A_UINT8 i;
        p2p_noa_desc.enable = 1;
        p2p_noa_desc.count = uccount;
 
        for (i = 0; i < uccount; i++) {
            p2p_noa_desc.noas[i].count_or_type = uccount;
            p2p_noa_desc.noas[i].duration = uiduration;
            p2p_noa_desc.noas[i].interval = uiinterval;
            p2p_noa_desc.noas[i].start_or_offset = uistart;
        }
    }
        
    param.cmd_id = ATH_P2P_SET_NOA;
    param.data = &p2p_noa_desc;
    param.length = sizeof(p2p_noa_desc);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}

A_STATUS qcom_p2p_func_set_oppps(A_UINT8 device_id, A_UINT8 enable, A_UINT8 ctwin)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    WMI_OPPPS_INFO_STRUCT p2p_oppps;
    
    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    A_MEMZERO(&p2p_oppps, sizeof(WMI_OPPPS_INFO_STRUCT));
    p2p_oppps.enable = enable;
    p2p_oppps.ctwin = ctwin;
    
    param.cmd_id = ATH_P2P_SET_OPPPS;
    param.data = &p2p_oppps;
    param.length = sizeof(WMI_OPPPS_INFO_STRUCT);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
}   

A_STATUS qcom_p2p_func_prov(A_UINT8 device_id, P2P_WPS_METHOD wps_method, A_UINT8 *pmac)
{
    ATH_IOCTL_PARAM_STRUCT param;
    A_STATUS error = A_OK;
    WMI_P2P_FW_PROV_DISC_REQ_CMD p2p_prov_disc;

    if(qcom_set_deviceid(device_id) == A_ERROR){
        return A_ERROR;
    }
    A_MEMZERO(&p2p_prov_disc, sizeof(p2p_prov_disc));
    p2p_prov_disc.dialog_token = 1;
    p2p_prov_disc.wps_method = wps_method;
    A_MEMCPY(p2p_prov_disc.peer, pmac, ATH_MAC_LEN);
     
    param.cmd_id = ATH_P2P_DISC_REQ;
    param.data =  &p2p_prov_disc;
    param.length = sizeof(p2p_prov_disc);
    if(ath_ioctl_handler(p_Driver_Cxt[device_id],&param) != A_OK){
        error = A_ERROR;
    }
    return error;
} 

A_STATUS qcom_raw_mode_send_pkt(QCOM_RAW_MODE_PARAM_t *ppara)
{
    A_UINT8 access_cat;
    A_INT32 i;
    ATH_IOCTL_PARAM_STRUCT param;
    A_UINT32 status = 0;
    ATH_MAC_TX_RAW_S details;
    A_UINT8 *buffer = NULL;
    typedef struct{
    	A_UINT8 fc[2];
    	A_UINT8 duration[2];
    	A_UINT8 addr[3][6];
    	A_UINT8 seq[2];
    }WIFI_HDR;

    typedef struct{
    	A_UINT8 fc[2];
    	A_UINT8 duration[2];
    	A_UINT8 addr[3][6];
    	A_UINT8 seq[2];
    	A_UINT8 addr_3[6];
    	A_UINT8 pad[2]; // pad required by firmware
    }WIFI_4_HDR;

    typedef struct{
    	A_UINT8 fc[2];
    	A_UINT8 duration[2];
    	A_UINT8 addr[3][6];
    	A_UINT8 seq[2];
    	A_UINT8 qos[2];
    	A_UINT8 pad[2]; // pad required by firmware
    }WIFI_QOS_HDR;
#define WIFI_HDR_SZ (24)
#define WIFI_4_HDR_SZ (32)
#define WIFI_QOS_HDR_SZ (28)
    A_UINT8 header_sz[3]={WIFI_HDR_SZ, WIFI_QOS_HDR_SZ, WIFI_4_HDR_SZ};
    WIFI_4_HDR* p4Hdr;
    WIFI_HDR* pHdr;
    WIFI_QOS_HDR *pQosHdr;
    A_STATUS error = A_OK;
    A_UINT32 enet_device;

    A_UINT8 rate_index = ppara->rate_index;
    A_UINT8 tries = ppara->tries;
    A_UINT32 size = ppara->size;
    A_UINT32 chan = ppara->chan;
    A_UINT8 header_type = ppara->header_type;
    A_UINT16 bcn_seq_num = ppara->seq;
    A_UINT8 *paddr1 = &ppara->addr1.addr[0];
    A_UINT8 *paddr2 = &ppara->addr2.addr[0];
    A_UINT8 *paddr3 = &ppara->addr3.addr[0];
    A_UINT8 *paddr4 = &ppara->addr4.addr[0];
    A_UINT8 *pdatabuf = ppara->pdatabuf;
    A_UINT32 buflen = ppara->buflen;    
    
    do{
    	if(rate_index > 19 ||
    		tries > 14 ||
    		size > 1400 ||
    		chan < 1 || chan > 14 ||
    		header_type > 2)
        {
              error = A_ERROR;
              break;
    	}

	if( (pdatabuf != NULL) && ( buflen >= size) )
	{
            error = A_ERROR;
            break;
	}

        error = qcom_set_channel(0, chan);
        if (A_OK != error)
        {
            break;
        }

    	size += header_sz[header_type];
    	if(NULL == (buffer = A_MALLOC(size,MALLOC_ID_CONTEXT))){
              error = A_ERROR;
              break;
    	}
        
    	pHdr = (WIFI_HDR*)&buffer[0];

    	A_MEMZERO(pHdr, WIFI_HDR_SZ);

        pHdr->fc[0] = 0x80; //beacon
        pHdr->seq[0] = ((bcn_seq_num & 0xf) << 4);
        pHdr->seq[1] = ((bcn_seq_num & 0xff0) >> 4);

        A_MEMCPY(&pHdr->addr[0][0], paddr1, ATH_MAC_LEN);  
        A_MEMCPY(&pHdr->addr[1][0], paddr2, ATH_MAC_LEN);  
        A_MEMCPY(&pHdr->addr[2][0], paddr3, ATH_MAC_LEN);  

        access_cat = ATH_ACCESS_CAT_BE;

        if(header_type == 1){
        	//change frame control to qos data
        	pHdr->fc[0] = 0x88;
        	pHdr->fc[1] = 0x01; //to-ds
        	pQosHdr = (WIFI_QOS_HDR*)pHdr;
        	pQosHdr->qos[0] = 0x03;
        	pQosHdr->qos[1] = 0x00;
        	//bssid == addr[0]
        	//memcpy(&(pHdr->addr[0][0]), &(pHdr->addr[1][0]), 6);
        	//change destination address
        	//pHdr->addr[2][3] = 0xaa;
        	//pHdr->addr[2][4] = 0xaa;
        	//pHdr->addr[2][5] = 0xaa;
        	access_cat = ATH_ACCESS_CAT_VI;
        }else if(header_type == 2){
        	pHdr->fc[0] = 0x08;
        	pHdr->fc[1] = 0x03; //to-ds | from-ds
        	p4Hdr = (WIFI_4_HDR*)pHdr;
        	p4Hdr->addr_3[0] = paddr4[0];
    	        p4Hdr->addr_3[1] = paddr4[1];
    	        p4Hdr->addr_3[2] = paddr4[2];
        	p4Hdr->addr_3[3] = paddr4[3];
        	p4Hdr->addr_3[4] = paddr4[4];
        	p4Hdr->addr_3[5] = paddr4[5];
        }

        /* now setup the frame for transmission */
        if( pdatabuf == NULL ) {
    	    for(i=header_sz[header_type];i<size;i++)
    	        buffer[i] = (i-header_sz[header_type])&0xff;
        } else {
            i=header_sz[header_type];
            A_MEMCPY(&(buffer[i]), pdatabuf, buflen);
        }

        A_MEMZERO(&details, sizeof(details));

        details.buffer = buffer;
        details.length = size;
        details.params.pktID = 1;
        details.params.rateSched.accessCategory = access_cat;
        /* we set the ACK bit simply to force the desired number of retries */
        details.params.rateSched.flags = ATH_MAC_TX_FLAG_ACK;
        details.params.rateSched.trySeries[0] = tries;
        details.params.rateSched.rateSeries[0] = rate_index;
        details.params.rateSched.rateSeries[1] = 0xff;//terminate series

        param.cmd_id = ATH_MAC_TX_RAW;
        param.data = &details;
        param.length = sizeof(details);

        if(ath_ioctl_handler(p_Driver_Cxt[0],&param) != A_OK){
            error = A_ERROR;
        }
    }while(0);

    if(buffer)
    	A_FREE(buffer,MALLOC_ID_CONTEXT);
    
    return error;
}


#endif
