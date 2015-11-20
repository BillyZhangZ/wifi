/**
 * @file
 */
/******************************************************************************
 * Copyright 2013-2014, Qualcomm Connected Experiences, Inc.
 *
 ******************************************************************************/

#define AJ_MODULE ALLJOYN

#include <limits.h>
#include "aj_wifi_ctrl.h"
#include "aj_util.h"
#include "aj_debug.h"

#include <qca_includes.h>

#ifndef NDEBUG
AJ_EXPORT uint8_t dbgALLJOYN = 0;
#endif

#define NET_UP_TIMEOUT  5000

extern void AJ_Main();

#ifdef WIFI_SSID
/*
 * This function is provided for testing convenience. Generates a 5 byte WEP hex key from an ascii
 * passphrase. We don't support the 13 byte version which uses an MD5 hash to generate the hex key
 * from the passphrase.
 */
static AJ_Status WEPKey(const char* pwd, uint8_t* hex, uint32_t hexLen)
{
    static const uint32_t WEP_MAGIC1 = 0x343FD;
    static const uint32_t WEP_MAGIC2 = 0x269EC3;
    uint32_t i;
    uint32_t seed;
    uint8_t key[5];

    for (i = 0; *pwd; ++i) {
        seed ^= *pwd++ << (i & 3) << 8;
    }
    for (i = 0; i < (hexLen / 2); ++i) {
        seed = WEP_MAGIC1 * seed + WEP_MAGIC2;
        key[i] = (seed >> 16);
    }
    return AJ_RawToHex(key, sizeof(key), hex, hexLen, FALSE);
}

static AJ_Status ConfigureWifi()
{
    AJ_Status status = AJ_ERR_CONNECT;
    static const char ssid[] = WIFI_SSID;
# ifdef WIFI_PASSPHRASE
    static const char passphrase[] = WIFI_PASSPHRASE;
    const AJ_WiFiSecurityType secType = AJ_WIFI_SECURITY_WPA2;
# else
    const char* passphrase = NULL;
    const AJ_WiFiSecurityType secType = AJ_WIFI_SECURITY_NONE;
# endif

    AJ_Printf("Trying to connect to AP %s\n", ssid);
    if (secType == AJ_WIFI_SECURITY_WEP) {
        char wepKey[11];
        WEPKey(passphrase, wepKey, sizeof(wepKey));
        status = AJ_ConnectWiFi(ssid, AJ_WIFI_SECURITY_WEP, AJ_WIFI_CIPHER_WEP, wepKey);
    } else {
        status = AJ_ConnectWiFi(ssid, secType, AJ_WIFI_CIPHER_CCMP, passphrase);
    }

    if (status != AJ_OK) {
        AJ_Printf("ConfigureWifi error\n");
    }

    if (AJ_GetWifiConnectState() == AJ_WIFI_AUTH_FAILED) {
        AJ_Printf("ConfigureWifi authentication failed\n");
        status = AJ_ERR_SECURITY;
    }
    return status;
}

static char* AddrStr(uint32_t addr)
{
    static char txt[16];
    return inet_ntoa(A_CPU2BE32(addr), txt);
}
#endif

#ifdef SOFTAP_SSID
static AJ_Status ConfigureSoftAP()
{
    AJ_Status status = AJ_ERR_CONNECT;
    static const char ssid[] = SOFTAP_SSID;
#  ifdef SOFTAP_PASSPHRASE
    static const char passphrase[] = SOFTAP_PASSPHRASE;
#  else
    const char* passphrase = NULL;
#  endif
    AJ_Printf("Configuring soft AP %s\n", ssid);
    status = AJ_EnableSoftAP(ssid, FALSE, passphrase, INT_MAX);
    if (status == AJ_ERR_TIMEOUT) {
        AJ_Printf("AJ_EnableSoftAP timeout\n");
    } else if (status != AJ_OK) {
        AJ_Printf("AJ_EnableSoftAP error\n");
    }
    return status;
}
#endif

#ifdef WIFI_SCAN
static void ScanResult(void* context, const char* ssid, const uint8_t mac[6], uint8_t rssi, AJ_WiFiSecurityType secType, AJ_WiFiCipherType cipherType)
{
    static const char* const sec[] = { "OPEN", "WEP", "WPA", "WPA2" };
    static const char* const typ[] = { "", ":TKIP", ":CCMP", ":WEP" };

    AJ_Printf("SSID %s [%02x:%02X:%02x:%02x:%02x:%02x] RSSI=%d security=%s%s\n", ssid, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], rssi, sec[secType], typ[cipherType]);
}
#endif

static void coolmodebutton_int_service_routine(void *pin)
{
    AJMODEBUTTONCB cb = NULL;

    cb = AJ_Get_ModeButton_CB();

    if(cb != NULL){
        cb((void*)0);
    }

    return;

}

static void heatmodebutton_int_service_routine(void *pin)
{
    AJMODEBUTTONCB cb = NULL;

    cb = AJ_Get_ModeButton_CB();

    if(cb != NULL){
        cb((void*)1);
    }

    return;

}
void AllJoyn_Start(unsigned long arg)
{
    AJ_Status status = AJ_OK;

#if defined  (BSP_BUTTON1_GPIO_INT) && defined  (BSP_BUTTON2_GPIO_INT)
    do{

        GPIO_PIN_STRUCT 	pins[2];
        FILE_PTR                pinsw1, pinsw2;

        pins[0] = BSP_BUTTON1_GPIO_INT;        
        pins[1] = GPIO_LIST_END;        

        pinsw1 = fopen("gpio:input", (char_ptr)&pins);
        ioctl(pinsw1, GPIO_IOCTL_SET_IRQ_FUNCTION, (pointer)&coolmodebutton_int_service_routine);
        ioctl(pinsw1, GPIO_IOCTL_ENABLE_IRQ, NULL);  

        pins[0] = BSP_BUTTON2_GPIO_INT;
        pins[1] = GPIO_LIST_END;        

        pinsw2 = fopen("gpio:input", (char_ptr)&pins);
        ioctl(pinsw2, GPIO_IOCTL_SET_IRQ_FUNCTION, (pointer)&heatmodebutton_int_service_routine);
        ioctl(pinsw2, GPIO_IOCTL_ENABLE_IRQ, NULL);  

    }while(0);
#endif
    AJ_Printf("\n******************************************************");
    AJ_Printf("\n                AllJoyn Thin-Client");
    AJ_Printf("\n******************************************************\n");

    AJ_PoolAllocInit();

    AJ_PrintFWVersion();

    AJ_InfoPrintf(("Alljoyn Version %s\n", AJ_GetVersion()));
#ifdef AJ_CONFIGURE_WIFI_UPON_START
#ifdef WIFI_SCAN
    status = AJ_WiFiScan(NULL, ScanResult, 32);
    if (status != AJ_OK) {
        AJ_Printf("WiFi scan failed\n");
    }
#endif

#ifdef WIFI_SSID
    while (1) {
        uint32_t ip, mask, gw;
        status = ConfigureWifi();

        if (status != AJ_OK) {
            AJ_InfoPrintf(("AllJoyn_Start(): ConfigureWifi status=%s", AJ_StatusText(status)));
            continue;
        } else {
            status = AJ_AcquireIPAddress(&ip, &mask, &gw, NET_UP_TIMEOUT);
            AJ_Printf("Got IP %s\n", AddrStr(ip));
            if (status != AJ_OK) {
                AJ_InfoPrintf(("AllJoyn_Start(): AJ_AcquireIPAddress status=%s", AJ_StatusText(status)));
            }

            break;
        }
    }
#elif defined SOFTAP_SSID
    status = ConfigureSoftAP();
#endif
#endif
    if (status == AJ_OK) {
        AJ_Main();
    }

    AJ_Printf("Quitting\n");
    while (TRUE) {}
}
