/**HEADER********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
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
* $FileName: usb_host_aoa.h$
* $Version : 
* $Date    : 
*
* Comments:
*   
*
*END************************************************************************/

#ifndef __AOA_HOST_ACCESSORY_H__
#define __AOA_HOST_ACCESSORY_H__

/**/

#include <mqx.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_main.h"
#include "host_snd.h"
#include "host_rcv.h"
#include "host_ch9.h"


#define USE_USB_AUDIO

#ifdef USE_USB_AUDIO
  #define USE_AUDIO_CONTROL
#endif

#define CONNECT_TO_APK  /* request load adk apk */ 	

//#define USE_ACC_HID

/* Non-class Requests for accessory */
#define  USB_AOA_SEND_BULK            3
#define  USB_AOA_RECV_BULK            4


/* Class-specific Requests for accessroy  */

/*
requestType:    USB_DIR_IN | USB_TYPE_VENDOR
request:        51
value:          0
index:          0
data:           protocol version number (16 bits little endian sent from the
                device to the accessory)
*/
#define  USB_AOA_GET_PROTOCOL_VERSION        51

/*
requestType:    USB_DIR_OUT | USB_TYPE_VENDOR
request:        52
value:          0
index:          string ID
data            zero terminated UTF8 string sent from accessory to device
*/
#define  USB_AOA_SET_PROTOCOL_ZERO        52

#ifdef USE_ACC_HID
#define ACCESSORY_REGISTER_HID         54 
#define ACCESSORY_UNREGISTER_HID 55

#define ACCESSORY_SET_HID_REPORT_DESC	56 

#define ACCESSORY_SEND_HID_EVENT  57 



#define SCAN_NEXT_TRACK  0x04
#define SCAN_PREV_TRACK  0x08
#define PLAY 0x02
#define PAUSE 0x02
#define KEY_RELEASED   0x00
#endif
/*
manufacturer name:  0
model name:         1
description:        2
version:            3
URI:                4
serial number:      5

*/
#if 1  // ADK 2.0  // liunote must match with accessory filter at apk
#define DEFAULT_ACCESSORY_STRING_VENDOR "Freescale, Inc."
#define DEFAULT_ACCESSORY_STRING_NAME   "Speaker"
#define DEFAULT_ACCESSORY_STRING_LONGNAME "Android Speaker Demo"
#define DEFAULT_ACCESSORY_STRING_VERSION  "2.0"
#define DEFAULT_ACCESSORY_STRING_URL    "http://www.freescale.com"
#define DEFAULT_ACCESSORY_STRING_SERIAL "0000000012345678"
#else
#define DEFAULT_ACCESSORY_STRING_VENDOR "Google, Inc."
#define DEFAULT_ACCESSORY_STRING_NAME   "DemoKit"
#define DEFAULT_ACCESSORY_STRING_LONGNAME "DemoKit Arduino Board"
#define DEFAULT_ACCESSORY_STRING_VERSION  "2.0"
#define DEFAULT_ACCESSORY_STRING_URL    "http://www.android.com"
#define DEFAULT_ACCESSORY_STRING_SERIAL "0000000012345678"
#endif



#ifdef USE_ACC_HID
static int8_t hid_description[] = {
/*USAGE_PAGE (Consumer Devices)*/	0x05, 0x0C,
/*USAGE (Consumer Control)*/	0x09 , 0x01,
/*COLLECTION (Application)*/	0xA1 , 0x01 ,

   /*LOGICAL_MINIMUM (0)*/	0x15, 0x00 ,
  /*LOGICAL_MAXIMUM*(1)*/	0x25 , 0x01,
  /*USAGE (MUTE)*/	0x09, 0xE2,
  /*REPORT_SIZE (1)*/	0x75 , 0x01, 
  /*REPORT_COUNT (1)*/	0x95 , 0x01, 
  /*INPUT (Data,Var,Rel)*/	0x81 , 0x22, 
  
  /*LOGICAL_MINIMUM (0)*/	0x15, 0x00 ,
   /*LOGICAL_MAXIMUM*(1)*/	0x25 , 0x01,
   /*USAGE (play/pause)*/	0x09, 0xCD,
   /*REPORT_SIZE (1)*/	0x75 , 0x01, 
   /*REPORT_COUNT (1)*/	0x95 , 0x01, 
   /*INPUT (Data,Var,Rel)*/	0x81 , 0x06, 
   
   /*USAGE (Scan Next Track)*/	0x09, 0xB5,
    /*LOGICAL_MINIMUM (0)*/	0x15, 0x00 ,
    /*LOGICAL_MAXIMUM (1)*/	0x25 , 0x01 ,
    /*REPORT_SIZE (1)*/	0x75 , 0x01 ,
    /*REPORT_COUNT (1)*/	0x95 , 0x01 ,
   /* INPUT (Data,Var,Rel)*/	0x81 , 0x06, 
   
  /* USAGE (Scan Previous Track)*/	0x09 , 0xB6,
  /* LOGICAL_MINIMUM (0)*/	0x15 , 0x00 ,
  /* LOGICAL_MAXIMUM (1)*/	0x25 , 0x01 ,
   /*REPORT_SIZE (1)*/	0x75 , 0x01 ,
  /* REPORT_COUNT (1)*/	0x95 , 0x01 ,
  /* INPUT (Data,Var,Rel)*/	0x81 , 0x06, 
  /*REPORT_COUNT (4)*/	0x95 , 0x03, 
  /*INPUT (Cnst,Var,Rel)*/	0x81 , 0x07, 
/*END_COLLECTION*/	0xC0};

#define ACCESSORY_SEND_EVENT 100
#define ACEESORY_HID                117
#define ACCESSORY_REPORT        118
#define ACCESSORY_REPORT1       119
#define ACCESSORY_REPORT2       120

#endif

enum req_acc_type 
{
   GET_PROTOCOL_VERSION,
#ifdef  CONNECT_TO_APK  /* Don't request adk apk */ 	
   STRING_VENDOR,      
   STRING_NAME,
#endif   
   STRING_LONGNAME,
   STRING_VERSION,
   STRING_URL,
   STRING_SERIAL,
#ifdef  USE_USB_AUDIO
   ACCESSORY_AUDIO,
#endif
    ACCESSORY_START,
#ifdef USE_ACC_HID
   // ACCESSORY_HID_REGISTER,
   // ACCESSORY_HID_REPORT,
#endif
   WAITING_ACCESSORY
};


typedef enum req_acc_type req_acc_type;


/* Class-specific interface info struct, useful for
**       an application to send/receive on in/out
**       pipes by name without internal details.
 */
typedef struct aoa_interface {
   struct aoa_interface *   next;    /* next accessory */
   struct aoa_interface *   anchor;  /* list start */
   _usb_device_instance_handle      dev_handle;
   _usb_interface_descriptor_handle intf_handle;
   _usb_host_handle                 host_handle;
   uint32_t                          	key_code;       /* must = 0 (for Class_Key_Generator)*/
   _usb_pipe_handle                 control_pipe;
   _usb_pipe_handle                 bulk_in_pipe;
   _usb_pipe_handle                 bulk_out_pipe;
} AOA_INTERFACE_STRUCT, * AOA_INTERFACE_STRUCT_PTR;

/* Alphabetical list of Function Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

void usb_aoa_init(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);
USB_STATUS usb_aoa_recv_data(CLASS_CALL_STRUCT_PTR,
   tr_callback, void *, uint32_t, unsigned char *);
USB_STATUS usb_aoa_send_data(CLASS_CALL_STRUCT_PTR,
   tr_callback, void *, uint32_t, unsigned char *);

USB_STATUS usb_device_try_accessory (
#if 0   
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR      cc_ptr
      ,
      /* [IN] callback for completion/status */
      tr_callback                callback,
      
      /* [IN] user parameter returned by callback */
      void *                    call_parm     
#else
   _usb_device_instance_handle      dev_handle,
   _usb_interface_descriptor_handle intf_handle,
    tr_callback                callback,
   void *                    call_parm,
   int  req_status
#endif
);

#ifdef USE_ACC_HID
USB_STATUS usb_device_hid_regist
   (
   _usb_device_instance_handle      dev_handle,
   _usb_interface_descriptor_handle intf_handle,
   tr_callback  callback,
   uint32_t     len
   );
USB_STATUS usb_device_hid_set_report_descriptor
   (
   _usb_device_instance_handle      dev_handle,
   _usb_interface_descriptor_handle intf_handle,
   tr_callback                callback
   );
USB_STATUS usb_device_hid_send_event
   (
   _usb_device_instance_handle      dev_handle,
   _usb_interface_descriptor_handle intf_handle,
   tr_callback                callback,
   uint32_t cmd
   );
#endif

#ifdef __cplusplus
}
#endif


#endif
