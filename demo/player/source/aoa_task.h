#ifndef __audio_host_h__
#define __audio_host_h__
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
#include "usb_host_audio.h"

#include "adk20.h"

#define AUTO_CALLBACK_IN_SOF
#define MFI_USB_STACK

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

/*
** Following structs contain all states and pointers
** used by the application to control/operate devices.
*/
#ifdef USE_AUDIO_CONTROL
/* uint32_t                          dev_state;*/  /* Attach/detach state */
/* PIPE_BUNDLE_STRUCT_PTR           pbs;      */  /* Device & pipes */
typedef struct audio_host_device_struct {
   uint32_t                          DEV_STATE;    /* Attach/detach state */
   _usb_device_instance_handle      DEV_HANDLE;
   _usb_interface_descriptor_handle INTF_HANDLE;
   CLASS_CALL_STRUCT                CLASS_INTF;   /* Class-specific info */
} AUDIO_CONTROL_DEVICE_STRUCT,  * AUDIO_CONTROL_DEVICE_STRUCT_PTR;

typedef struct feature_control_struct {
    uint32_t                          FU;
} FEATURE_CONTROL_STRUCT,  * FEATURE_CONTROL_STRUCT_PTR;
#endif

typedef struct data_device_struct {
   uint32_t                          DEV_STATE;    /* Attach/detach state */
   _usb_device_instance_handle      DEV_HANDLE;
   _usb_interface_descriptor_handle INTF_HANDLE;
   CLASS_CALL_STRUCT                CLASS_INTF;   /* Class-specific info */
} AUDIO_STREAM_DEVICE_STRUCT,  * AUDIO_STREAM_DEVICE_STRUCT_PTR;

struct _usb_audio_host_info_ {
	/* some handles for communicating with USB stack */
	//_usb_host_handle	host_handle; /* host controller status etc. */

	/* check device type error */
	uint8_t         dev_check_state; /*error_state*/

	/* microphone or speaker */
	char        device_direction;

	/* buffer for receiving an audio data packet */
	char *		wav_recv_buff;

	/* audio packet size */
	uint32_t		packet_size;

	/* audio stream device */
	AUDIO_STREAM_DEVICE_STRUCT    audio_stream;

	USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR   frm_type_desc;

	/* audio stream descriptor */
	/* end point descriptor */
	ENDPOINT_DESCRIPTOR_PTR		endp;
#ifdef USE_AUDIO_CONTROL
	/* audio control device */
	AUDIO_CONTROL_DEVICE_STRUCT    audio_control;

	char *			device_string;
	
	AUDIO_COMMAND		audio_com;
	/*
	static int16_t           cur_volume, min_volume, max_volume, res_volume;
	int16_t					physic_volume;
	uint8_t					host_cur_volume;
	int16_t                  device_volume_step; */
	uint8_t  cur_mute[4];
	uint8_t  cur_vol[4];
	uint8_t  max_vol[4];
	uint8_t  min_vol[4];
	uint8_t  res_vol[4];
	uint8_t  cur_samp[4];

	LWEVENT_STRUCT		USB_Audio_FU_Request;
	USB_AUDIO_CTRL_DESC_FU_PTR		fu_desc;
#endif

	struct _usb_accessoy_host_info_ * acc_info;

	void * pcm_handle;
};

#ifdef OTG_USE_HCI_HOST_CLASS
#define USB_EVENT_ATTACH    (1)
#define USB_EVENT_DETACH    (2)
#define USB_EVENT_INTF      (3)
#else
// nothing to do 
#endif

/* Alphabetical list of Function Prototypes */
#ifdef __cplusplus
extern "C" {
#endif



/* local function declare */
static void usb_host_accessory_event(_usb_device_instance_handle,
   _usb_interface_descriptor_handle, uint32_t);
static void usb_host_audio_stream_event(_usb_device_instance_handle,
   _usb_interface_descriptor_handle, uint32_t);

#ifdef TR_CALLBACK_COMPLETE
int usb_host_audio_tr_callback(_usb_pipe_handle ,void *,
    unsigned char * ,uint32_t ,uint32_t );
#else
void usb_host_audio_tr_callback(_usb_pipe_handle ,void *,
    unsigned char * ,uint32_t ,uint32_t );
#endif


#ifdef USE_AUDIO_CONTROL
void usb_host_audio_control_event(_usb_device_instance_handle,
   _usb_interface_descriptor_handle, uint32_t);
#ifdef TR_CALLBACK_COMPLETE
int usb_host_audio_mute_ctrl_callback(_usb_pipe_handle,void *,
    unsigned char *,uint32_t,uint32_t);
int usb_host_audio_ctrl_callback(_usb_pipe_handle,void *,
    unsigned char *,uint32_t,uint32_t);
int usb_host_audio_interrupt_callback(_usb_pipe_handle,void *,
    unsigned char *,uint32_t,uint32_t);
int usb_host_audio_request_ctrl_callback(_usb_pipe_handle,void *,unsigned char *,uint32_t,uint32_t);
int config_channel(USB_AUDIO_CTRL_DESC_FU_PTR,uint8_t);
#else
void usb_host_audio_mute_ctrl_callback(_usb_pipe_handle,void *,
    unsigned char *,uint32_t,uint32_t);
void usb_host_audio_ctrl_callback(_usb_pipe_handle,void *,
    unsigned char *,uint32_t,uint32_t);
void usb_host_audio_interrupt_callback(_usb_pipe_handle,void *,
    unsigned char *,uint32_t,uint32_t);
void usb_host_audio_request_ctrl_callback(_usb_pipe_handle,void *,unsigned char *,uint32_t,uint32_t);
void config_channel(USB_AUDIO_CTRL_DESC_FU_PTR,uint8_t);
#endif
#endif

void aoa_audio_task (uint32_t param);
int aoaRegisterDriverInfoTable(USB_HOST_DRIVER_INFO *infotable, int size);
USB_STATUS acc_host_info_init();

#ifdef __cplusplus
}
#endif

#endif   /* __audio_host_h__ */

/* EOF */
