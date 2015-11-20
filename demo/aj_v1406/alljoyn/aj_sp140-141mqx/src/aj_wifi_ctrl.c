/******************************************************************************
 * Copyright 2013-2014, Qualcomm Connected Experiences, Inc.
 *
 ******************************************************************************/

#define AJ_MODULE WIFI

#include <qca_includes.h>

#include "aj_target.h"
#include "aj_util.h"
#include "aj_status.h"
#include "aj_wifi_ctrl.h"
#include "atheros_wifi_internal.h"
#include "aj_debug.h"

/**
 * Turn on per-module debug printing by setting this variable to non-zero value
 * (usually in debugger).
 */
#ifndef NDEBUG
uint8_t dbgWIFI = 0;
#endif

static char* AddrStr(uint32_t addr)
{
    static char txt[16];
    return inet_ntoa(A_CPU2BE32(addr), txt);
}

#define ATH_SECURITY_TYPE_NONE  ENET_MEDIACTL_SECURITY_TYPE_NONE
#define ATH_SECURITY_TYPE_WEP   ENET_MEDIACTL_SECURITY_TYPE_WEP
#define ATH_SECURITY_TYPE_WPA   ENET_MEDIACTL_SECURITY_TYPE_WPA
#define ATH_SECURITY_TYPE_WPA2  ENET_MEDIACTL_SECURITY_TYPE_WPA2

static AJ_Status AJ_Network_Up();

#define MAX_SSID_LENGTH                 32
typedef struct _AJ_FW_Version {
    uint32_t host_ver;
    uint32_t target_ver;
    uint32_t wlan_ver;
    uint32_t abi_ver;
} AJ_FW_Version;

/*
 * Handle for QCA WiFi driver context
 */
_enet_handle handle;

extern void atheros_driver_setup(void);


static const uint32_t startIP = 0xC0A80101;
static const uint32_t endIP   = 0xC0A80102;

#define IP_LEASE    (60 * 60 * 1000)

static const A_UINT8 IP6RoutePrefix[16] = { 0x20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };

#define PREFIX_LEN          64
#define PREFIX_LIFETIME  12000

static AJ_WiFiConnectState connectState = AJ_WIFI_IDLE;
static uint32_t athSecType = ATH_SECURITY_TYPE_NONE;

AJ_WiFiConnectState AJ_GetWifiConnectState(void)
{
    return connectState;
}

#define RSNA_AUTH_FAILURE  10
#define RSNA_AUTH_SUCCESS  16

static void WiFiCallback(int val)
{
    AJ_InfoPrintf(("\nWiFiCallback %d\n", val));
    if (val == 0) {
        if (connectState == AJ_WIFI_DISCONNECTING || connectState == AJ_WIFI_CONNECT_OK) {
            connectState = AJ_WIFI_IDLE;
            AJ_InfoPrintf(("\nWiFi Disconnected\n"));
        } else if (connectState != AJ_WIFI_CONNECT_FAILED) {
            connectState = AJ_WIFI_CONNECT_FAILED;
            AJ_InfoPrintf(("\nWiFi Connect Failed\n"));
        }
    } else if (val == 1) {
        /*
         * With WEP or no security a callback value == 1 means we are done. In the case of wEP this
         * means there is no way to tell if the association succeeded or failed.
         */
        if ((athSecType == ATH_SECURITY_TYPE_NONE) || (athSecType == AJ_WIFI_SECURITY_WEP)) {
            connectState = AJ_WIFI_CONNECT_OK;
            AJ_InfoPrintf(("\nConnected to AP\n"));
        }
    } else if (val == RSNA_AUTH_SUCCESS) {
        connectState = AJ_WIFI_CONNECT_OK;
        AJ_InfoPrintf(("\nConnected to AP\n"));
    } else if (val == RSNA_AUTH_FAILURE) {
        connectState = AJ_WIFI_AUTH_FAILED;
        AJ_InfoPrintf(("\nWiFi Authentication Failed\n"));
    }

    /*
     * Set up for IPV6
     */
    if (connectState == AJ_WIFI_CONNECT_OK) {
        t_ip6config_router_prefix(handle, (A_UINT8 *)IP6RoutePrefix, PREFIX_LEN, PREFIX_LIFETIME, PREFIX_LIFETIME);
    }
}

static void SoftAPCallback(int val)
{
    if (val == 0) {
        if (connectState == AJ_WIFI_DISCONNECTING || connectState == AJ_WIFI_SOFT_AP_UP) {
            connectState = AJ_WIFI_IDLE;
            AJ_InfoPrintf(("Soft AP Down\n"));
        } else if (connectState == AJ_WIFI_STATION_OK) {
            connectState = AJ_WIFI_SOFT_AP_UP;
            AJ_InfoPrintf(("Soft AP Station Disconnected\n"));
        } else {
            connectState = AJ_WIFI_CONNECT_FAILED;
            AJ_InfoPrintf(("Soft AP Connect Failed\n"));
        }
    } else if (val == 1) {
        if (connectState == AJ_WIFI_SOFT_AP_INIT) {
            AJ_InfoPrintf(("Soft AP Initialized\n"));
            connectState = AJ_WIFI_SOFT_AP_UP;
        } else {
            AJ_InfoPrintf(("Soft AP Station Connected\n"));
            connectState = AJ_WIFI_STATION_OK;
        }
    }
}

static uint8_t IsDriverInitialized()
{
    A_INT32 error;
    A_UINT32 dev_status;

    error = ENET_mediactl(handle, ENET_MEDIACTL_IS_INITIALIZED, &dev_status);
    if (ENET_OK != error || dev_status == FALSE) {
        return FALSE;
    }
    return TRUE;
}

static AJ_Status SendIoctl(A_UINT32 cmd, void* data, A_UINT32 len)
{
    A_INT32 error;
    AJ_Status status;

    if (!IsDriverInitialized()) {
        return AJ_ERR_DRIVER;
    }
    switch (cmd) {
    case ATH_SET_MODE:
        error = ENET_mediactl(handle, ENET_SET_MEDIACTL_MODE, data);
        break;

    case ATH_SET_SSID:
        {
            ENET_ESSID param;
            param.essid = data;
            param.length = len;
            error = ENET_mediactl(handle, ENET_SET_MEDIACTL_ESSID, &param);
        }
        break;

    case ATH_SET_SEC_TYPE:
        error = ENET_mediactl(handle, ENET_SET_MEDIACTL_SEC_TYPE, data);
        break;

    case ATH_SET_COMMIT:
        error = ENET_mediactl(handle, ENET_SET_MEDIACTL_COMMIT, data);
        break;

    case ATH_SET_PASSPHRASE:
        {
            ENET_MEDIACTL_PARAM param;
            param.data = data;
            param.length = len;
            error = ENET_mediactl(handle, ENET_SET_MEDIACTL_PASSPHRASE, &param);
        }
        break;

    case ATH_SET_ENCODE:
        {
            error = ENET_mediactl(handle, ENET_SET_MEDIACTL_ENCODE, data);
        }
        break;

    case ATH_SET_POWER:
        {
            ENET_MEDIACTL_PARAM param;
            param.value = *((uint32_t*)data);
            error = ENET_mediactl(handle, ENET_SET_MEDIACTL_POWER, &param);
        }
        break;

    case ATH_GET_VERSION:
    default:
        {
            ATH_IOCTL_PARAM_STRUCT param;
            param.cmd_id = cmd;
            param.data = data;
            param.length = len;
            error = ENET_mediactl(handle, ENET_MEDIACTL_VENDOR_SPECIFIC, &param);
        }
        break;
    }

    if (error != ENET_OK) {
        status = (error == ENETERR_INPROGRESS) ? AJ_ERR_BUSY : AJ_ERR_DRIVER;
    } else {
        status = AJ_OK;
    }
    return status;
}
AJ_Status AJ_PrintFWVersion()
{
    AJ_Status status;
    AJ_FW_Version version;
    AJ_Network_Up();
    status = SendIoctl(ATH_GET_VERSION, &version, 16);
    if (status == AJ_OK) {
        AJ_InfoPrintf(("Host version :  %d.%d.%d.%d.%d\n",
                       (version.host_ver & 0xF0000000) >> 28,
                       (version.host_ver & 0x0F000000) >> 24,
                       (version.host_ver & 0x00FC0000) >> 18,
                       (version.host_ver & 0x0003FF00) >> 8,
                       (version.host_ver & 0x000000FF)));

        AJ_InfoPrintf(("Target version   :  0x%x\n", version.target_ver));
        AJ_InfoPrintf(("Firmware version :  %d.%d.%d.%d.%d\n",
                       (version.wlan_ver & 0xF0000000) >> 28,
                       (version.wlan_ver & 0x0F000000) >> 24,
                       (version.wlan_ver & 0x00FC0000) >> 18,
                       (version.wlan_ver & 0x0003FF00) >> 8,
                       (version.wlan_ver & 0x000000FF)));
        AJ_InfoPrintf(("Interface version:  %d\n", version.abi_ver));
    }
    return status;
}

static AJ_Status AJ_ConnectWiFiHelper(const char* ssid, AJ_WiFiSecurityType secType, AJ_WiFiCipherType cipherType, const char* passphrase)
{
    AJ_Status status;
    A_UINT32 mode;

    /*
     * In WifiCallback we need to know if the connection was secure
     */
    athSecType = secType;
    /*
     * Clear the old connection state
     */
    connectState = AJ_WIFI_IDLE;
    status = AJ_DisconnectWiFi();
    if (status != AJ_OK) {
        return status;
    }
    /*
     * Set the new SSID
     */
    if (strlen(ssid) > MAX_SSID_LENGTH) {
        AJ_ErrPrintf(("SSID length exceeds Maximum value\n"));
        return AJ_ERR_INVALID;
    }
    status = SendIoctl(ATH_SET_SSID, (void*)ssid, strlen(ssid));
    if (status != AJ_OK) {
        return status;
    }
    /*
     * Set the security type
     */
    status = SendIoctl(ATH_SET_SEC_TYPE, (void*)&athSecType, sizeof(athSecType));
    if (status != AJ_OK) {
        return status;
    }
    /*
     * Set infrastructure mode
     */
    mode = ENET_MEDIACTL_MODE_INFRA;
    status = SendIoctl(ATH_SET_MODE, (void*)&mode, sizeof(mode));
    if (status != AJ_OK) {
        return status;
    }
    /*
     * Setup the security parameters if needed
     */
    if (athSecType != ATH_SECURITY_TYPE_NONE) {
        uint32_t passLen = strlen(passphrase);
        cipher_t cipher;
        /*
         * Cipher type - same for unicast and multicast
         */
        switch (cipherType) {
        case AJ_WIFI_CIPHER_WEP:
            cipher.ucipher = ATH_CIPHER_TYPE_WEP;
            break;

        case AJ_WIFI_CIPHER_TKIP:
            cipher.ucipher = ATH_CIPHER_TYPE_TKIP;
            break;

        case AJ_WIFI_CIPHER_CCMP:
            cipher.ucipher = ATH_CIPHER_TYPE_CCMP;
            break;

        default:
            cipher.ucipher = 0;
            break;
        }
        cipher.mcipher = cipher.ucipher;

        status = SendIoctl(ATH_SET_CIPHER, (void*)&cipher, sizeof(cipher));
        if (status != AJ_OK) {
            return status;
        }
        if ((secType == AJ_WIFI_SECURITY_WPA || secType == AJ_WIFI_SECURITY_WPA2) && (passLen == 64)) {
            WMI_SET_PMK_CMD data;

            data.pmk_len = WMI_PMK_LEN;
            status = AJ_HexToRaw(passphrase, passLen, (uint8_t*)&data.pmk, WMI_PMK_LEN);
            if (status == AJ_OK) {
                status = SendIoctl(ATH_SET_PMK, (void*)&data, sizeof(data));
            } else {
                // not valid hex data; assume it's a password
                status = SendIoctl(ATH_SET_PASSPHRASE, (void*)passphrase, passLen);
            }
        } else if (secType == AJ_WIFI_SECURITY_WEP) {
            uint8_t key[WEP_LONG_KEY];

            if (passLen != 10 && passLen != 26) {
                status = AJ_ERR_INVALID;
            } else {
                status = AJ_HexToRaw(passphrase, passLen, key, passLen / 2);
            }
            if (status == AJ_OK) {
                ENET_WEPKEYS wepKeys;
                memset(&wepKeys, 0, sizeof(wepKeys));
                wepKeys.defKeyIndex = 1;
                wepKeys.keyLength = passLen / 2;
                wepKeys.numKeys = 1;
                wepKeys.keys[0] = (char*)key;
                status = SendIoctl(ATH_SET_ENCODE, (void*)&wepKeys, sizeof(wepKeys));
            }
        } else {
            status = SendIoctl(ATH_SET_PASSPHRASE, (void*)passphrase, passLen);
        }
        if (status != AJ_OK) {
            return status;
        }
    }
    /*
     * Set power mode to Max-Perf
     */
    mode = 0;
    status = SendIoctl(ATH_SET_POWER, (void*)&mode, sizeof(mode));
    if (status != AJ_OK) {
        return status;
    }
    /*
     * Set the callback for the connect state
     */
    status = SendIoctl(ATH_SET_CONNECT_STATE_CALLBACK, (void*)WiFiCallback, sizeof(void*));
    if (status != AJ_OK) {
        return status;
    }
    /*
     * Commit the changes
     */
    connectState = AJ_WIFI_CONNECTING;
    status = SendIoctl(ATH_SET_COMMIT, NULL, 0);
    if (status != AJ_OK) {
        connectState = AJ_WIFI_IDLE;
    }
    return status;
}

#define DHCP_TIMEOUT 5000
AJ_Status AJ_ConnectWiFi(const char* ssid, AJ_WiFiSecurityType secType, AJ_WiFiCipherType cipherType, const char* passphrase)
{
    AJ_WiFiConnectState connectState;
    uint32_t ip;
    uint32_t mask;
    uint32_t gateway;
  
    AJ_Status status = AJ_Network_Up();
    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_ConnectWiFi(): AJ_Network_Up error"));
        return status;
    }

    AJ_SetIPAddress(0, 0, 0);
    
    status = AJ_ConnectWiFiHelper(ssid, secType, cipherType, passphrase);
    if (status != AJ_OK) {
        AJ_ErrPrintf(("ConfigureWifi error\n"));
        return status;
    }

    /*
     * Poll until we are connected or the connection fails
     */
    connectState = AJ_GetWifiConnectState();
    while (connectState == AJ_WIFI_CONNECTING) {
        AJ_InfoPrintf(("ConnectWiFi Connecting...\n"));
        AJ_Sleep(250);
        connectState = AJ_GetWifiConnectState();
    }

    if (connectState == AJ_WIFI_CONNECT_OK) {
        status = AJ_AcquireIPAddress(&ip, &mask, &gateway, DHCP_TIMEOUT);
        AJ_InfoPrintf(("AJ_ConnectWiFi(): AJ_AcquireIPAddress status=%s\n", AJ_StatusText(status)));
        if (status == AJ_OK) {
            AJ_Printf("Got IP %s\n", AddrStr(ip));
        }
    } else if (connectState == AJ_WIFI_CONNECT_FAILED) {
        AJ_ErrPrintf(("ConnectWiFi failed to connect\n"));
        status = AJ_ERR_CONNECT;
    } else if (connectState == AJ_WIFI_AUTH_FAILED) {
        AJ_ErrPrintf(("ConnectWiFi failed to authenticate\n"));
        status = AJ_ERR_SECURITY;
    } else {
        AJ_ErrPrintf(("ConnectWiFi failed\n"));
        status = AJ_ERR_UNKNOWN;
    }

    return status;
}

AJ_Status AJ_DisconnectWiFi(void)
{
    AJ_Status status = AJ_OK;
    AJ_WiFiConnectState oldState = connectState;
    static const char* empty = "";

    if (connectState != AJ_WIFI_DISCONNECTING) {
        status = SendIoctl(ATH_SET_SSID, (void*)empty, 0);
        if (status != AJ_OK) {
            connectState = oldState;
            return status;
        }
        /*
         * Commit the changes
         */
        if (connectState != AJ_WIFI_IDLE) {
            connectState = AJ_WIFI_DISCONNECTING;
        }
        status = SendIoctl(ATH_SET_COMMIT, NULL, 0);
        if (status != AJ_OK) {
            connectState = oldState;
        }
    }
    return status;
}

static AJ_Status AJ_EnableSoftAPHelper(const char* ssid, A_UINT8 hidden, const char* passphrase)
{
    AJ_Status status;
    A_UINT32 mode = ENET_MEDIACTL_MODE_MASTER;
    uint32_t secType = passphrase ? ATH_SECURITY_TYPE_WPA2 : ATH_SECURITY_TYPE_NONE;

    /*
     * Clear the current connection
     */
    connectState = AJ_WIFI_IDLE;
    status = AJ_DisconnectWiFi();
    if (status != AJ_OK) {
        return status;
    }
    /*
     * Set softAP mode
     */
    status = SendIoctl(ATH_SET_MODE, (void*)&mode, 4);
    if (status != AJ_OK) {
        return status;
    }
    /*
     * Set the SSID
     */
    if (strlen(ssid) > MAX_SSID_LENGTH) {
        AJ_ErrPrintf(("SSID length exceeds Maximum value\n"));
        return AJ_ERR_INVALID;
    }
    status = SendIoctl(ATH_SET_SSID, (void*)ssid, strlen(ssid));
    if (status != AJ_OK) {
        return status;
    }
    /*
     * Set flag to indicate if AP SSID is hidden
     */
    if (hidden) {
        ATH_AP_PARAM_STRUCT apParam;
        apParam.cmd_subset = AP_SUB_CMD_HIDDEN_FLAG;
        apParam.data = NULL;
        status = SendIoctl(ATH_CONFIG_AP, (void*)&apParam, sizeof(apParam));
        if (status != AJ_OK) {
            return status;
        }
    }
    /*
     * Set the security type
     */
    status = SendIoctl(ATH_SET_SEC_TYPE, (void*)&secType, sizeof(secType));
    if (status != AJ_OK) {
        return status;
    }
    /*
     * Set security parameters if AP is not open
     */
    if (secType != ATH_SECURITY_TYPE_NONE) {
        cipher_t cipher;
        /*
         * Set cipher type to CCMP
         */
        cipher.ucipher = ATH_CIPHER_TYPE_CCMP;
        cipher.mcipher = ATH_CIPHER_TYPE_CCMP;
        status = SendIoctl(ATH_SET_CIPHER, (void*)&cipher, sizeof(cipher));
        if (status != AJ_OK) {
            return status;
        }
        /*
         * Set the passphrase
         */
        status = SendIoctl(ATH_SET_PASSPHRASE, (void*)passphrase, strlen(passphrase));
        if (status != AJ_OK) {
            return status;
        }
    }
    /*
     * Set the callback for the connect state
     */
    status = SendIoctl(ATH_SET_CONNECT_STATE_CALLBACK, (void*)SoftAPCallback, sizeof(void*));
    if (status != AJ_OK) {
        return status;
    }
    /*
     * Set the IP range for DHCP
     */
    if (t_ipconfig_dhcp_pool(handle, (A_UINT32 *)&startIP, (A_UINT32 *)&endIP, IP_LEASE) != A_OK) {
        return AJ_ERR_DRIVER;
    }

    /*
     * Set up for IPV6
     */
    if (t_ip6config_router_prefix(handle, (A_UINT8 *)IP6RoutePrefix, PREFIX_LEN, PREFIX_LIFETIME, PREFIX_LIFETIME) != A_OK) {
        return AJ_ERR_DRIVER;
    }

    /*
     * Commit the changes
     */
    connectState = AJ_WIFI_SOFT_AP_INIT;
    status = SendIoctl(ATH_SET_COMMIT, NULL, 0);
    if (status != AJ_OK) {
        connectState = AJ_WIFI_IDLE;
    }
    return status;
}


#define SOFTAP_SLEEP_TIMEOUT 100
// block until somebody connects to us
AJ_Status AJ_EnableSoftAP(const char* ssid, A_UINT8 hidden, const char* passphrase, const uint32_t timeout)
{
    AJ_Status status = AJ_Network_Up();
    uint32_t time2 = 0;
    
    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_EnableSoftAP(): AJ_Network_Up error"));
        return status;
    }

    status = AJ_EnableSoftAPHelper(ssid, hidden, passphrase);
    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_EnableSoftAP error\n"));
        return status;
    }

    /*
     * Wait until a remote station connects
     */
    AJ_InfoPrintf(("Waiting for remote station to connect\n"));

    do {
        AJ_Sleep(SOFTAP_SLEEP_TIMEOUT);
        time2 += SOFTAP_SLEEP_TIMEOUT;
    } while (AJ_GetWifiConnectState() != AJ_WIFI_STATION_OK && (timeout == 0 || time2 < timeout));

    return (AJ_GetWifiConnectState() == AJ_WIFI_STATION_OK) ? AJ_OK : AJ_ERR_TIMEOUT;
}

AJ_Status AJ_WiFiScan(void* context, AJ_WiFiScanResult callback, uint8_t maxAPs)
{
    AJ_Status status = AJ_OK;
    ATH_SCANPARAMS athScan;
    ATH_GET_SCAN scanList;
    
    status = AJ_Network_Up();
    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_EnableSoftAP(): AJ_Network_Up error"));
        return status;
    }

    
    /*
     * Enable foreground and background scanning
     */
    athScan.flags = 0;
    status = SendIoctl(ATH_SCAN_CTRL, (void*)&athScan, sizeof(athScan));
    if (status != AJ_OK) {
        return status;
    }
    /*
     * Start the scan
     */
    status = SendIoctl(ATH_START_SCAN_EXT, NULL, 0);
    if (status != AJ_OK) {
        goto DisableScan;
    }
    /*
     * Get results of the scan
     */
    status = SendIoctl(ATH_GET_SCAN_EXT, (void*)&scanList, 0);
    if (status != AJ_OK) {
        goto DisableScan;
    }
    maxAPs = min(scanList.num_entries, maxAPs);
    /*
     * Callback with each result
     */
    while (maxAPs--) {
        ATH_SCAN_EXT* scan = NULL;
        uint8_t rssiMax = 0;
        int i;
        /*
         * Find entry with max RSSI - no sort heroics needed the list is short
         */
        for (i = 0; i < scanList.num_entries; ++i) {
            ATH_SCAN_EXT* ap = &scanList.scan_list[i];
            if (ap->rssi >= rssiMax) {
                rssiMax = ap->rssi;
                scan = ap;
            }
        }
        if (scan) {
            AJ_WiFiSecurityType secType = AJ_WIFI_SECURITY_NONE;
            AJ_WiFiCipherType cipherType = AJ_WIFI_CIPHER_NONE;
            char ssid[MAX_SSID_LENGTH + 1];
            uint32_t len = min(scan->ssid_len, MAX_SSID_LENGTH);
            /*
             * NUL terminate the SSID
             */
            memcpy(ssid, scan->ssid, len);
            ssid[len] = 0;
            cipherType = AJ_WIFI_CIPHER_WEP;
            /*
             * Map security info into one of our security types
             */
            if (scan->security_enabled) {
                if (scan->rsn_auth) {
                    secType = AJ_WIFI_SECURITY_WPA2;
                    if (scan->rsn_cipher & ATH_CIPHER_TYPE_CCMP) {
                        cipherType = AJ_WIFI_CIPHER_CCMP;
                    } else if (scan->rsn_cipher & ATH_CIPHER_TYPE_TKIP) {
                        cipherType = AJ_WIFI_CIPHER_TKIP;
                    }
                } else if (scan->wpa_auth) {
                    secType = AJ_WIFI_SECURITY_WPA;
                    if (scan->wpa_cipher & ATH_CIPHER_TYPE_CCMP) {
                        cipherType = AJ_WIFI_CIPHER_CCMP;
                    } else if (scan->wpa_cipher & ATH_CIPHER_TYPE_TKIP) {
                        cipherType = AJ_WIFI_CIPHER_TKIP;
                    }
                } else {
                    secType = AJ_WIFI_SECURITY_WEP;
                }
            }
            /*
             * Callback with the info
             */
            callback(context, ssid, scan->bssid, scan->rssi, secType, cipherType);
            /*
             * Set rssi so we skip this entry next time around
             */
            scan->rssi = 0;
        }
    }

DisableScan:

    /*
     * Disable foreground and background scanning
     */
    athScan.flags = ATH_DISABLE_FG_SCAN | ATH_DISABLE_BG_SCAN;
    (void)SendIoctl(ATH_SCAN_CTRL, (void*)&athScan, sizeof(athScan));

    return status;
}

static A_STATUS get_tx_status()
{
    AJ_Status status;
    ATH_TX_STATUS result;

    status = SendIoctl(ATH_GET_TX_STATUS, (void*)&result, 0);
    if (status == AJ_OK) {
        switch (result.status) {
        case ATH_TX_STATUS_IDLE:
        case ATH_TX_STATUS_WIFI_PENDING:
        case ATH_TX_STATUS_HOST_PENDING:
            break;

        default:
            return A_ERROR;
        }

        return (A_STATUS)result.status;
    } else {
        return A_ERROR;
    }
}

AJ_Status AJ_SuspendWifi(uint32_t msec)
{
    static uint8_t suspendEnabled = FALSE;

    if (!suspendEnabled) {
        AJ_Status status = SendIoctl(ATH_DEVICE_SUSPEND_ENABLE, NULL, 0);
        if (status != AJ_OK) {
            return status;
        }
        suspendEnabled = TRUE;
    }
    /*uint32_t* ip, uint32_t* mask, uint32_t* gateway
     * wait until we are not transmitting
     */
    while (get_tx_status() != ATH_TX_STATUS_IDLE) {
        AJ_Sleep(500);
    }

    return SendIoctl(ATH_DEVICE_SUSPEND_START, (void*)&msec, sizeof(msec));
}


static uint8_t wifi_initialized = FALSE;
static uint32_t wifi_state = 0;

static AJ_Status AJ_Network_Up()
{
    uint_32 err;
    _enet_address enet_address;

    connectState = AJ_WIFI_IDLE;

    if (wifi_initialized == FALSE) {
        atheros_driver_setup();
        wifi_initialized = TRUE;
    }


    if (wifi_state == 0) {
        wifi_state = 1;
        /*
         * Initialize the device
         */
        err = ENET_initialize(BSP_DEFAULT_ENET_DEVICE, enet_address, 0, &handle);
        if (err != A_OK) {
            AJ_ErrPrintf(("ENET_initialize failed %d\n", err));
            return AJ_ERR_DRIVER;
        }

        /*
         * Bring WiFi driver up
         */
        return SendIoctl(ATH_CHIP_STATE, (void*) &wifi_state, sizeof(wifi_state));
    } else {
        return AJ_OK;
    }
}


static AJ_Status AJ_Network_Down()
{
    uint_32 err;
    connectState = AJ_WIFI_IDLE;

    if (wifi_state == 1) {
        wifi_state = 0;
        err = ENET_shutdown(handle);
        if (err != A_OK) {
            AJ_ErrPrintf(("ENET_shutdown failed %d\n", err));
            return AJ_ERR_DRIVER;
        }

        handle = NULL;
    }

    return AJ_OK;
}

AJ_Status AJ_ResetWiFi(void)
{
    AJ_Status status;
    AJ_InfoPrintf(("Reset WiFi driver\n"));
    connectState = AJ_WIFI_IDLE;
    status = AJ_Network_Down();
    if (status != AJ_OK) {
        AJ_ErrPrintf(("AJ_ResetWiFi(): AJ_Network_Down failed %s\n", AJ_StatusText(status)));
        return status;
    }

    return AJ_Network_Up();
}

AJ_Status AJ_GetIPAddress(uint32_t* ip, uint32_t* mask, uint32_t* gateway)
{
    A_INT32 ret;
    // set to zero first
    *ip = *mask = *gateway = 0;

    ret = t_ipconfig((void*) handle, IPCFG_QUERY, ip, mask, gateway, NULL, NULL);
    return (ret == A_OK ? AJ_OK : AJ_ERR_DHCP);
}

#define DHCP_WAIT       100

AJ_Status AJ_AcquireIPAddress(uint32_t* ip, uint32_t* mask, uint32_t* gateway, int32_t timeout)
{
    A_INT32 ret;
    AJ_Status status;
    AJ_WiFiConnectState wifi_state = AJ_GetWifiConnectState();

    switch (wifi_state) {
    case AJ_WIFI_CONNECT_OK:
        break;

    // no need to do anything in Soft-AP mode
    case AJ_WIFI_SOFT_AP_INIT:
    case AJ_WIFI_SOFT_AP_UP:
    case AJ_WIFI_STATION_OK:
        return AJ_OK;

    // shouldn't call this function unless already connected!
    case AJ_WIFI_IDLE:
    case AJ_WIFI_CONNECTING:
    case AJ_WIFI_CONNECT_FAILED:
    case AJ_WIFI_AUTH_FAILED:
    case AJ_WIFI_DISCONNECTING:
        return AJ_ERR_DHCP;
    }

    status = AJ_GetIPAddress(ip, mask, gateway);
    if (status != AJ_OK) {
        return status;
    }

    while (0 == *ip) {
        if (timeout < 0) {
            AJ_ErrPrintf(("AJ_AcquireIPAddress(): DHCP Timeout\n"));
            return AJ_ERR_TIMEOUT;
        }

        AJ_InfoPrintf(("Sending DHCP request\n"));
        /*
         * This call kicks off DHCP but we need to poll until the values are populated
         */
        ret = t_ipconfig((void*) handle, IPCFG_DHCP, ip, mask, gateway, NULL, NULL);
        if (ret != A_OK) {
            return AJ_ERR_DHCP;
        }

        AJ_Sleep(DHCP_WAIT);
        status = AJ_GetIPAddress(ip, mask, gateway);
        if (status != AJ_OK) {
            return status;
        }
        timeout -= DHCP_WAIT;
    }

    if (status == AJ_OK) {
        AJ_InfoPrintf(("*********** DHCP succeeded %s\n", AddrStr(*ip)));
    }

    return status;
}

AJ_Status AJ_SetIPAddress(uint32_t ip, uint32_t mask, uint32_t gateway)
{
    A_INT32 ret = t_ipconfig((void*) handle, IPCFG_STATIC, &ip, &mask, &gateway, NULL, NULL);
    return (ret == A_OK ? AJ_OK : AJ_ERR_DHCP);
}

#if 1

uint32_t AJ_iwcfg_set_power 
    (
        uint32_t dev_num,
        uint32_t pow_val,
        uint32_t flags
        
    )
{
    uint32_t error;
    bool dev_status;
    ENET_MEDIACTL_PARAM  inout_param;

    if (handle == NULL)
    {
        printf("Initialize Wifi Device Using ipconfig\n");
        return ENET_ERROR;
    }        
    error = ENET_mediactl (handle, ENET_MEDIACTL_IS_INITIALIZED, &dev_status);

    if (ENET_OK != error)
    {
        return error;
    }
    if (dev_status == FALSE)
    {
        return ENET_ERROR;
    }
    
    inout_param.value = pow_val;
    inout_param.disabled = flags;
    inout_param.flags = ENET_MEDIACTL_POWER_PERIOD;
    error = ENET_mediactl (handle, ENET_SET_MEDIACTL_POWER, &inout_param);
    if (ENET_OK != error)
    {
        return error;
    }
    return ENET_OK;
       
    return ENETERR_INVALID_DEVICE;  
}



#endif


