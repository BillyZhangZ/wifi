#ifndef _demo_h_
#define _demo_h_
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
* $FileName: HVAC.h$
* $Version : 3.5.22.0$
* $Date    : Mar-25-2010$
*
* Comments:
*   The main configuration file for WEB_HVAC demo
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include <message.h>
#if MQX_KERNEL_LOGGING
#include <klog.h>
#endif

#define DEMOCFG_ENABLE_SERIAL_SHELL    0   /* enable shell task for serial console */
#define DEMOCFG_ENABLE_SWITCH_TASK     0   /* enable button sensing task (otherwise keys are polled) */
#define DEMOCFG_ENABLE_AUTO_LOGGING    0   /* enable logging to serial console (or USB drive) */
#define DEMOCFG_ENABLE_USB_FILESYSTEM  0   /* enable USB mass storage */
#define DEMOCFG_ENABLE_RTCS            1   /* enable RTCS operation */
#define DEMOCFG_ENABLE_FTP_SERVER      0   /* enable ftp server */
#define DEMOCFG_ENABLE_TELNET_SERVER   0   /* enable telnet server */
#define DEMOCFG_ENABLE_WEBSERVER       1   /* enable web server */
#define DEMOCFG_ENABLE_KLOG            0   /* enable kernel logging */
#define DEMOCFG_USE_POOLS              0   /* enable external memory pools for USB and RTCS */
#define DEMOCFG_USE_WIFI               1   /* use WiFi Interface */
#define DEMOCFG_USE_WPS				   0   /* use WPS to establish WiFi connection */
#define DEMOCFG_USE_WPS_PUSHBUTTON	   0   /* use push button mode for wps or if 0 use pin mode */

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
   #include <ftpd.h> 
   
   extern void HVAC_initialize_networking(void);

   #ifndef ENET_IPADDR
      #define ENET_IPADDR  IPADDR(192,168,1,90) 
   #endif

   #ifndef ENET_IPMASK
      #define ENET_IPMASK  IPADDR(255,255,255,0) 
   #endif

   #ifndef ENET_IPGATEWAY
      #define ENET_IPGATEWAY  IPADDR(192,168,1,2) 
   #endif
#endif
#if DEMOCFG_USE_WIFI
   #include "iwcfg.h"
   
   #define DEMOCFG_SSID            "atheros_demo"
   //Possible Values managed or adhoc
   #define DEMOCFG_NW_MODE         "managed" 
   //Possible vales 
   // 1. "wep"
   // 2. "wpa"
   // 3. "wpa2"
   // 4. "none"
   #define DEMOCFG_SECURITY        "none"	//"wep","wpa","wpa2"
   #define DEMOCFG_PASSPHRASE      "iot_testing"
   #define DEMOCFG_WEP_KEY         "04E49E18F7"
   //Possible values 1,2,3,4
   #define DEMOCFG_WEP_KEY_INDEX   1
   #define DEMOCFG_DEFAULT_DEVICE  1
   #define DEMOCFG_DEFAULT_WEP_MODE  "open"  //"shared"
   #define DEMOCFG_DEFAULT_U_CIPHER  "TKIP"	 //"CCMP"
   #define DEMOCFG_DEFAULT_M_CIPHER  "TKIP"  //"CCMP"
#endif

#ifndef DEMOCFG_DEFAULT_DEVICE
   #define DEMOCFG_DEFAULT_DEVICE   BSP_DEFAULT_ENET_DEVICE
#endif
   
#if DEMOCFG_ENABLE_USB_FILESYSTEM
   #include <mfs.h>
   #include <part_mgr.h>
   #include <usbmfs.h>

   #define LOG_FILE "c:\\hvac_log.txt"
#endif

#if DEMOCFG_ENABLE_WEBSERVER
   #include "httpd.h"
#endif

#include <shell.h>

enum 
{
   HVAC_TASK = 1,
   SWITCH_TASK,
   SHELL_TASK,
   LOGGING_TASK,
   USB_TASK,
   ALIVE_TASK
};

void HVAC_Task(uint_32);
void Switch_Task(uint_32);
void Switch_Poll(void);
void Shell_Task(uint_32);
void Logging_task(uint_32);
void USB_task(uint_32);
void HeartBeat_Task(uint_32);
uint_32 get_rssi();
void WifiConnected(int value);

extern _enet_handle    handle;
#endif
