/**HEADER********************************************************************
* 
* Copyright (c) 2008, 2013 - 2014 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
* $FileName: usb_hid.h$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief The file contains USB stack HID class layer api header function.
*
*****************************************************************************/

#ifndef _USB_HID_H
#define _USB_HID_H 1

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_class_hid.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define HID_IMPLEMENT_QUEUING  0 
#define MAX_HID_CLASS_EP_NUM   1

 /* class specific requests */

/* for class specific requests */
#define REPORT_SIZE                       (4)
#define CLASS_REQ_DATA_SIZE               (0x01)
#define MAX_HID_DEVICE                    (0x05)
#if HID_IMPLEMENT_QUEUING
#define HID_MAX_QUEUE_ELEMS               (4) 
#endif

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
void    USB_Service_Hid(usb_event_struct_t* event,void* arg);
void    USB_Class_Hid_Event(uint8_t event, void* val,void* arg);
usb_status USB_HID_Requests(usb_setup_struct_t * setup_packet, uint8_t * *data, uint32_t *size,void* arg);


 /******************************************************************************
 * Types
 *****************************************************************************/
#if HID_IMPLEMENT_QUEUING     
/* structure to hold a request in the endpoint queue */
typedef struct _usb_class_hid_queue 
{
    usb_device_handle                        handle;
    uint8_t*                                 app_buff; /* buffer to send */
    uint32_t                                 size; /* size of the transfer */
    uint8_t                                  channel; 
}usb_class_hid_queue_t;
#endif

/* USB class hid endpoint data */
typedef struct _usb_class_hid_endpoint 
{
    uint8_t                                  endpoint; /* endpoint num */
    uint8_t                                  type;     /* type of endpoint (interrupt, bulk or isochronous) */ 
#if HID_IMPLEMENT_QUEUING
    uint8_t                                  bin_consumer;/* the num of queued elements */
    uint8_t                                  bin_producer;/* the num of de-queued elements */
    usb_class_hid_queue_t                    queue[HID_MAX_QUEUE_ELEMS]; /* queue data */  
#endif
}usb_class_hid_endpoint_t;

/* contains the endpoint data for non control endpoints */
typedef struct _usb_class_hid_endpoint_data 
{
    usb_class_hid_endpoint_t                   ep[MAX_HID_CLASS_EP_NUM];
}usb_class_hid_endpoint_data_t;

/* Strucutre holding HID class state information*/
typedef struct hid_device_struct
{
    usb_device_handle                            handle;
    uint32_t                                     user_handle;
    usb_class_handle                             class_handle;
    usb_endpoints_t*                             ep_desc_data;
    usb_application_callback_struct_t            hid_application_callback;
    usb_vendor_req_callback_struct_t             vendor_req_callback;
    usb_class_specific_callback_struct_t         class_specific_callback;
    usb_desc_request_notify_struct_t             desc_callback;
    usb_class_hid_endpoint_data_t                hid_endpoint_data;
}hid_device_struct_t;
 
/******************************************************************************
 * Global Functions
 *****************************************************************************/

#endif
/* EOF */
