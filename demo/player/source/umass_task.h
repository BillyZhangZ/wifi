#ifndef __UMASS_TASK_H_
#define __UMASS_TASK_H_
/**HEADER********************************************************************
 * 
 * Copyright (c) 2008 Freescale Semiconductor;
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
 * $FileName: audio_host.h$
 * $Version : 3.8.7.0$
 * $Date    : Jul-2-2012$
 *
 * Comments:
 *
 *   This file contains mass storage class application types and definitions.
 *
 *END************************************************************************/

#include "host_dev_list.h"
#include "host_common.h"

#define OTG_USE_MSD_HOST_CLASS

#define OTG_USE_HCI_HOST_CLASS

/* Application-specific definitions */
#define  MAX_PENDING_TRANSACTIONS      16
#define  MAX_FRAME_SIZE                1024
#define  HOST_CONTROLLER_NUMBER        USBCFG_DEFAULT_HOST_CONTROLLER

#define NUMBER_OF_IT_TYPE			7
#define NUMBER_OF_OT_TYPE			8

#define MAX_ISO_PACKET_SIZE         512		/* only for 44100Hz sample rate */

#define USB_AUDIO_DEVICE_DIRECTION_IN                   0x00
#define USB_AUDIO_DEVICE_DIRECTION_OUT                  0x01
#define USB_AUDIO_DEVICE_DIRECTION_UNDEFINE             0xFF

#define USB_TERMINAL_TYPE           0x01
#define INPUT_TERMINAL_TYPE         0x02
#define OUTPUT_TERMINAL_TYPE        0x03

#define  USB_DEVICE_IDLE                   (0)
#define  USB_DEVICE_ATTACHED               (1)
#define  USB_DEVICE_CONFIGURED             (2)
#define  USB_DEVICE_SET_INTERFACE_STARTED  (3)
#define  USB_DEVICE_INTERFACED             (4)
#define  USB_DEVICE_SETTING_PROTOCOL       (5)
#define  USB_DEVICE_INUSE                  (6)
#define  USB_DEVICE_DETACHED               (7)
#define  USB_DEVICE_OTHER                  (8)

#define  TRANSFER_TYPE_NUM                  4
#define  SYNC_TYPE_NUM                      4
#define  DATA_TYPE_NUM                      4

#define  TRANSFER_TYPE_SHIFT                0
#define  SYNC_TYPE_SHIFT                    2
#define  DATA_TYPE_SHIFT                    4

#define BYTE0_SHIFT							0
#define BYTE1_SHIFT							8
#define BYTE2_SHIFT							16

#define HOST_VOLUME_STEP      1
#define HOST_MIN_VOLUME       0
#define HOST_MAX_VOLUME      10

/* test0306
   typedef struct _usb_host_dev_info
   {
   void * dev_private_info;
   } usb_host_dev_info_t;
   */

typedef struct _usb_host_info
{
    _usb_host_handle	host_handle; /* host controller status etc. */
#define HOST_DEV_NONE            0x0
#define HOST_DEV_PRESENT         0x1
#define HOST_DEV_ATTACHED        0x2
#define HOST_DEV_DTACHED_MASK    0xfffd

    unsigned short host_state; 

#define HOST_MSD_ATTACHED        0x4 
    // struct _usb_host_dev_info * next;  // we don't support hub ,so also don't need dev list
    unsigned short dev_type;    
    //usb_host_dev_info_t  * list_head;
    void*  pdev_info;
} usb_host_info_t;


/*
 ** Following structs contain all states and pointers
 ** used by the application to control/operate devices.
 */
typedef struct msd_device_struct {    /* application device handle !!! */
    /* This must be the first member of this structure, because sometimes we can use it
     ** as pointer to CLASS_CALL_STRUCT, other time as a pointer to DEVICE_STRUCT
     */
    CLASS_CALL_STRUCT                ccs;
    uint_32                          DEV_STATE;
    /* Mount point, 'c' for "c:" */
    char                             mount;
} MSD_DEVICE_STRUCT,  _PTR_ MSD_DEVICE_STRUCT_PTR;

struct _usb_msd_host_info_ {
    MSD_DEVICE_STRUCT msd_device;
    //LWSEM_STRUCT   USB_Stick;
    /* Store mounting point used. A: is the first one, bit #0 assigned, Z: is the last one, bit #25 assigned */
    uint_32 fs_mountp;
};

/* Alphabetical list of Function Prototypes */

#ifdef OTG_USE_HCI_HOST_CLASS
//bluetooth_device_struct #define in the class header file usb_host_bluetooth.h
#endif

#ifdef OTG_USE_HCI_HOST_CLASS
#define USB_EVENT_ATTACH    (1)
#define USB_EVENT_DETACH    (2)
#define USB_EVENT_INTF      (3)
#else
#ifdef OTG_USE_HCI_HOST_CLASS
#define USB_EVENT_ATTACH    (1)
#define USB_EVENT_DETACH    (2)
#define USB_EVENT_INTF      (3)
#else
// nothing to do 
#endif
#endif

/* Alphabetical list of Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif

    void umass_task(uint_32 param);
    int umassRegisterDriverInfoTable(USB_HOST_DRIVER_INFO *infotable, int size);

#ifdef __cplusplus
}
#endif

#endif   /* __audio_host_h__ */

/* EOF */
