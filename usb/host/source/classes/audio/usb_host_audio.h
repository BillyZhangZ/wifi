#ifndef __usb_host_audio_h__
#define __usb_host_audio_h__
/**HEADER********************************************************************
*
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: usb_host_audio.h$
* $Version :
* $Date    :
*
* Comments:
*
*   This file defines a template structure for Audio Class Driver.
*
*END************************************************************************/
#include <mqx.h>
#include <lwevent.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_main.h"
#include "host_snd.h"
#include "host_rcv.h"
#include "host_ch9.h"

#define SET_REQUEST_ITF                  (0x21)
#define SET_REQUEST_EP                   (0x22)
#define GET_REQUEST_ITF                  (0xA1)
#define GET_REQUEST_EP                   (0xA2)

/* Audio class codes */
#define REQUEST_CODE_UNDEFINED           (0x00)
#define SET_CUR                          (0x01)
#define GET_CUR                          (0x81)
#define SET_MIN                          (0x02)
#define GET_MIN                          (0x82)
#define SET_MAX                          (0x03)
#define GET_MAX                          (0x83)
#define SET_RES                          (0x04)
#define GET_RES                          (0x84)
#define SET_MEM                          (0x05)
#define GET_MEM                          (0x85)
#define GET_STAT                         (0xFF)

/* FU Command codes*/
#define USB_AUDIO_GET_CUR_MUTE           (0x00)
#define USB_AUDIO_SET_CUR_MUTE           (0x01)
#define USB_AUDIO_GET_CUR_VOLUME         (0x02)
#define USB_AUDIO_SET_CUR_VOLUME         (0x03)
#define USB_AUDIO_GET_MIN_VOLUME         (0x04)
#define USB_AUDIO_SET_MIN_VOLUME         (0x05)
#define USB_AUDIO_GET_MAX_VOLUME         (0x06)
#define USB_AUDIO_SET_MAX_VOLUME         (0x07)
#define USB_AUDIO_GET_RES_VOLUME         (0x08)
#define USB_AUDIO_SET_RES_VOLUME         (0x09)
#define USB_AUDIO_GET_CUR_BASS           (0x0A)
#define USB_AUDIO_SET_CUR_BASS           (0x0B)
#define USB_AUDIO_GET_MIN_BASS           (0x0C)
#define USB_AUDIO_SET_MIN_BASS           (0x0D)
#define USB_AUDIO_GET_MAX_BASS           (0x0E)
#define USB_AUDIO_SET_MAX_BASS           (0x0F)
#define USB_AUDIO_GET_RES_BASS           (0x10)
#define USB_AUDIO_SET_RES_BASS           (0x11)
#define USB_AUDIO_GET_CUR_MID            (0x12)
#define USB_AUDIO_SET_CUR_MID            (0x13)
#define USB_AUDIO_GET_MIN_MID            (0x14)
#define USB_AUDIO_SET_MIN_MID            (0x15)
#define USB_AUDIO_GET_MAX_MID            (0x16)
#define USB_AUDIO_SET_MAX_MID            (0x17)
#define USB_AUDIO_GET_RES_MID            (0x18)
#define USB_AUDIO_SET_RES_MID            (0x19)
#define USB_AUDIO_GET_CUR_TREBLE         (0x1A)
#define USB_AUDIO_SET_CUR_TREBLE         (0x1B)
#define USB_AUDIO_GET_MIN_TREBLE         (0x1C)
#define USB_AUDIO_SET_MIN_TREBLE         (0x1D)
#define USB_AUDIO_GET_MAX_TREBLE         (0x1E)
#define USB_AUDIO_SET_MAX_TREBLE         (0x1F)
#define USB_AUDIO_GET_RES_TREBLE         (0x20)
#define USB_AUDIO_SET_RES_TREBLE         (0x21)
#define USB_AUDIO_GET_CUR_AGC            (0x22)
#define USB_AUDIO_SET_CUR_AGC            (0x23)
#define USB_AUDIO_GET_CUR_DELAY          (0x24)
#define USB_AUDIO_SET_CUR_DELAY          (0x25)
#define USB_AUDIO_GET_MIN_DELAY          (0x26)
#define USB_AUDIO_SET_MIN_DELAY          (0x27)
#define USB_AUDIO_GET_MAX_DELAY          (0x28)
#define USB_AUDIO_SET_MAX_DELAY          (0x29)
#define USB_AUDIO_GET_RES_DELAY          (0x2A)
#define USB_AUDIO_SET_RES_DELAY          (0x2B)
#define USB_AUDIO_GET_CUR_BASS_BOOST     (0x2C)
#define USB_AUDIO_SET_CUR_BASS_BOOST     (0x2D)

/* Terminal Control Selectors */
#define TE_CONTROL_UNDEFINED             (0x00)
#define COPY_PROTECT_CONTROL             (0x01)

/* Endpoint Control Selectors */
#define EP_CONTROL_UNDEFINED             (0x00)
#define SAMPLING_FREQ_CONTROL            (0x01)
#define PITCH_CONTROL                    (0x02)

#define SAMPLING_FREQ_MASK               (0x01)
#define PITCH_MASK                       (0x02)

/* Endpoint Command Codes*/
#define USB_AUDIO_GET_CUR_PITCH          (0x00)
#define USB_AUDIO_SET_CUR_PITCH          (0x01)
#define USB_AUDIO_GET_CUR_SAMPLING_FREQ  (0x02)
#define USB_AUDIO_SET_CUR_SAMPLING_FREQ  (0x03)
#define USB_AUDIO_GET_MIN_SAMPLING_FREQ  (0x04)
#define USB_AUDIO_SET_MIN_SAMPLING_FREQ  (0x05)
#define USB_AUDIO_GET_MAX_SAMPLING_FREQ  (0x06)
#define USB_AUDIO_SET_MAX_SAMPLING_FREQ  (0x07)
#define USB_AUDIO_GET_RES_SAMPLING_FREQ  (0x08)
#define USB_AUDIO_SET_RES_SAMPLING_FREQ  (0x09)

#define NUMBER_OF_FEATURE_COMMANDS       (0x2E)
#define NUMBER_OF_ENDPOINT_COMMANDS      (0x0A)

/* audio command struct */
typedef struct {
    uint_8   control_mask;
    uint_8   request_type;
    uint_8   request_code;
    uint_8   request_value;
    uint_8   length;
} USB_AUDIO_COMMAND, _PTR_ USB_AUDIO_COMMAND_PTR;


/* Receive interrupt state */
typedef struct {
    uint_8       Status;
    uint_8		 Originator;
} USB_AUDIO_CONTROL_STATUS, _PTR_ USB_AUDIO_CONTROL_STATUS_PTR;


/* Audio Control Subclass */
/* Header descriptor */
#define USB_DESC_SUBTYPE_AUDIO_CS_HEADER           0x01
typedef struct {
    uint_8   bFunctionLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
    uint_8   bcdCDC[2];
    uint_8	 wTotalLength[2];
    uint_8   bInCollection;
} USB_AUDIO_CTRL_DESC_HEADER, _PTR_ USB_AUDIO_CTRL_DESC_HEADER_PTR;

/* Input Terminal descriptor */
#define USB_DESC_SUBTYPE_AUDIO_CS_IT               0x02
typedef struct {
    uint_8   bFunctionLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
    uint_8   bTerminalID;
    uint_8   wTerminalType[2];
    uint_8   bAssocTerminal;
    uint_8   bNrChannels;
    uint_8   wChannelConfig[2];
    uint_8   iChannelNames;
    uint_8   iTerminal;
} USB_AUDIO_CTRL_DESC_IT, _PTR_ USB_AUDIO_CTRL_DESC_IT_PTR;

/* Output Terminal descriptor */
#define USB_DESC_SUBTYPE_AUDIO_CS_OT               0x03
typedef struct {
    uint_8   bFunctionLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
    uint_8   bTerminalID;
    uint_8   wTerminalType[2];
    uint_8   bAssocTerminal;
    uint_8   bSourceID;
    uint_8   iTerminal;
} USB_AUDIO_CTRL_DESC_OT, _PTR_ USB_AUDIO_CTRL_DESC_OT_PTR;

/* Feature Unit descriptor */
#define USB_DESC_SUBTYPE_AUDIO_CS_FU               0x06
typedef struct {
    uint_8   bLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
    uint_8   bUnitID;
    uint_8   bSourceID;
    uint_8   bControlSize;
#define USB_AUDIO_CTRL_FU_MUTE              0x01
#define USB_AUDIO_CTRL_FU_VOLUME            0x02
#define USB_AUDIO_CTRL_FU_BASS              0x03
#define USB_AUDIO_CTRL_FU_MID               0x04
#define USB_AUDIO_CTRL_FU_TREBLE            0x05
#define USB_AUDIO_CTRL_FU_GRAPHIC_EQ        0x06
#define USB_AUDIO_CTRL_FU_AGC               0x07
#define USB_AUDIO_CTRL_FU_DELAY             0x08
#define USB_AUDIO_CTRL_FU_BASS_BOOST        0x09


#define FU_MUTE_MASK                        0x01
#define FU_VOLUME_MASK                      0x02
#define FU_BASS_MASK                        0x04
#define FU_MID_MASK                         0x08
#define FU_TREBLE_MASK                      0x10
#define FU_GRAPHIC_EQ_MASK                  0x20
#define FU_AGC_MASK                         0x40
#define FU_DELAY_MASK                       0x80
#define FU_BASS_BOOST_MASK                  0x01
    uint_8 	 bmaControls[];
    //uint_8 	 iFeature;
} USB_AUDIO_CTRL_DESC_FU, _PTR_ USB_AUDIO_CTRL_DESC_FU_PTR;

/* Audio Control function descriptor */
typedef union {
    USB_AUDIO_CTRL_DESC_HEADER         header;
    USB_AUDIO_CTRL_DESC_IT             it;
    USB_AUDIO_CTRL_DESC_OT             ot;
    USB_AUDIO_CTRL_DESC_FU             fu;
} USB_AUDIO_CTRL_FUNC_DESC, _PTR_ USB_AUDIO_CTRL_FUNC_DESC_PTR;

/*
** Audio Class Interface structure. This structure will be passed to
** all commands to this class driver.
*/
typedef struct usb_audio_general_class {
    /* Each class must start with a GENERAL_CLASS struct */
    GENERAL_CLASS                             G;

    /* Higher level callback and parameter */
    tr_callback                               USER_CALLBACK;
    pointer                                   USER_PARAM;

    uint_8                                    IFNUM;

} USB_AUDIO_GENERAL_CLASS, _PTR_ USB_AUDIO_GENERAL_CLASS_PTR;

/* Audio control subclass structure */
typedef struct {
    /* Each audio subclass must start with a USB_AUDIO_GENERAL_CLASS struct */
    USB_AUDIO_GENERAL_CLASS                   AUDIO_G;

    USB_AUDIO_CTRL_DESC_HEADER_PTR            header_desc;
    USB_AUDIO_CTRL_DESC_IT_PTR                it_desc;
    USB_AUDIO_CTRL_DESC_OT_PTR                ot_desc;
    USB_AUDIO_CTRL_DESC_FU_PTR                fu_desc;

    _usb_pipe_handle                          interrupt_pipe;
    USB_AUDIO_CONTROL_STATUS                  interrupt_buffer;

    tr_callback                               interrupt_callback;
    pointer                                   interrupt_callback_param;

#define USB_AUDIO_CTRL_DETACH           0x01
#define USB_AUDIO_CTRL_PIPE_FREE        0x02
#define USB_AUDIO_CTRL_INT_PIPE_FREE    0x04
    LWEVENT_STRUCT                            control_event;

} AUDIO_CONTROL_INTERFACE_STRUCT, _PTR_ AUDIO_CONTROL_INTERFACE_STRUCT_PTR;

/* Audio Stream Subclass */
/* Class-specific Audio stream interface descriptor */
#define USB_DESC_SUBTYPE_AS_CS_GENERAL 		0x01
typedef struct {
    uint_8   bLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
    uint_8   bTerminalLink;
    uint_8	 bDelay;
    uint_8	 wFormatTag[2];
} USB_AUDIO_STREAM_DESC_SPEPIFIC_AS_IF, _PTR_ USB_AUDIO_STREAM_DESC_SPEPIFIC_AS_IF_PTR;

/* Format type descriptor */
#define USB_DESC_SUBTYPE_AS_CS_FORMAT_TYPE       0x02
typedef struct {
    uint_8   bLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
    uint_8   bFormatType;
    uint_8   bNrChannels;
    uint_8   bSubFrameSize;
    uint_8   bBitResolution;
    uint_8   bSamFreqType;
    uint_8   tSamFreq[3];
} USB_AUDIO_STREAM_DESC_FORMAT_TYPE, _PTR_ USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR;

/* Class-specific Isochronous Audio Data Endpoint Descriptor */
#define USB_DESC_CLASS_ENDPOINT_GENERAL          0x01
typedef struct {
    uint_8   bLength;
    uint_8   bDescriptorType;
    uint_8   bDescriptorSubtype;
    uint_8   bmAttributes;
    uint_8   bLockDlayUnits;
    uint_8   wLockDelay[2];
} USB_AUDIO_STREAM_DESC_SPECIFIC_ISO_ENDP, _PTR_ USB_AUDIO_STREAM_DESC_SPECIFIC_ISO_ENDP_PTR;


typedef struct {
	uint_8 bLength;
	uint_8 bDescriptorType;
	uint_8 bEndpointAddress;
	uint_8 bmAttributes;
	uint_8 wMaxPacketSize[2];
	uint_8 bInterval;
	uint_8 bRefresh;
} USB_AUDIO_STREAM_DESC_SYNCH_ENDP, _PTR_ USB_AUDIO_STREAM_DESC_SYNCH_ENDP_PTR;

/* Audio stream interface descriptor */
typedef union {
    USB_AUDIO_STREAM_DESC_SPEPIFIC_AS_IF         as_general;
    USB_AUDIO_STREAM_DESC_FORMAT_TYPE            frm_type;
    USB_AUDIO_STREAM_DESC_SPECIFIC_ISO_ENDP      iso_endp_specific;
} USB_AUDIO_STREAM_FUNC_DESC, _PTR_ USB_AUDIO_STREAM_FUNC_DESC_PTR;

typedef struct {
    /* Each Audio subclass must start with a USB_AUDIO_GENERAL_CLASS struct */
    USB_AUDIO_GENERAL_CLASS                     AUDIO_G;

    USB_AUDIO_STREAM_DESC_SPEPIFIC_AS_IF_PTR    as_itf_desc;
    USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR       frm_type_desc;
    USB_AUDIO_STREAM_DESC_SPECIFIC_ISO_ENDP_PTR iso_endp_spec_desc;

    PIPE_STRUCT_PTR                             iso_in_pipe;
    PIPE_STRUCT_PTR                             iso_out_pipe;

#define USB_DATA_DETACH            0x01
#define USB_DATA_READ_COMPLETE     0x02
#define USB_DATA_READ_PIPE_FREE    0x04
#define USB_DATA_SEND_COMPLETE     0x08
#define USB_DATA_SEND_PIPE_FREE    0x10
    LWEVENT_STRUCT                            stream_event;

} AUDIO_STREAM_INTERFACE_STRUCT, _PTR_ AUDIO_STREAM_INTERFACE_STRUCT_PTR;

/* Audio Command */
typedef struct {
    CLASS_CALL_STRUCT_PTR   CLASS_PTR;
    tr_callback             CALLBACK_FN;
    pointer                 CALLBACK_PARAM;
} AUDIO_COMMAND, _PTR_ AUDIO_COMMAND_PTR;

#ifdef __cplusplus
extern "C" {
#endif

extern void usb_class_audio_control_init(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);
extern void usb_class_audio_stream_init(PIPE_BUNDLE_STRUCT_PTR, CLASS_CALL_STRUCT_PTR);

extern CLASS_CALL_STRUCT_PTR usb_class_audio_get_ctrl_interface(pointer);

extern USB_STATUS usb_class_audio_control_get_descriptors(_usb_device_instance_handle, _usb_interface_descriptor_handle, USB_AUDIO_CTRL_DESC_HEADER_PTR _PTR_, USB_AUDIO_CTRL_DESC_IT_PTR _PTR_, USB_AUDIO_CTRL_DESC_OT_PTR _PTR_, USB_AUDIO_CTRL_DESC_FU_PTR _PTR_);
extern USB_STATUS usb_class_audio_stream_get_descriptors(_usb_device_instance_handle, _usb_interface_descriptor_handle, USB_AUDIO_STREAM_DESC_SPEPIFIC_AS_IF_PTR _PTR_,       USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR _PTR_, USB_AUDIO_STREAM_DESC_SPECIFIC_ISO_ENDP_PTR _PTR_);
extern USB_STATUS usb_class_audio_stream_set_descriptors(CLASS_CALL_STRUCT_PTR, USB_AUDIO_STREAM_DESC_SPEPIFIC_AS_IF_PTR, USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR, USB_AUDIO_STREAM_DESC_SPECIFIC_ISO_ENDP_PTR);
extern USB_STATUS usb_class_audio_control_set_descriptors(CLASS_CALL_STRUCT_PTR, USB_AUDIO_CTRL_DESC_HEADER_PTR, USB_AUDIO_CTRL_DESC_IT_PTR, USB_AUDIO_CTRL_DESC_OT_PTR, USB_AUDIO_CTRL_DESC_FU_PTR);

extern USB_STATUS usb_audio_recv_data(CLASS_CALL_STRUCT_PTR, CLASS_CALL_STRUCT_PTR, tr_callback, pointer, uint_32, uchar_ptr);
extern USB_STATUS usb_audio_send_data(CLASS_CALL_STRUCT_PTR, CLASS_CALL_STRUCT_PTR, tr_callback, pointer, uint_32, uchar_ptr);

extern USB_STATUS usb_class_audio_init_ipipe(CLASS_CALL_STRUCT_PTR,tr_callback,pointer);

extern USB_STATUS usb_class_audio_get_copy_protect(AUDIO_COMMAND_PTR,pointer);
extern USB_STATUS usb_class_audio_set_copy_protect(AUDIO_COMMAND_PTR,pointer);

extern USB_STATUS usb_class_audio_feature_command(AUDIO_COMMAND_PTR,uint_8,pointer,uint_32);

extern USB_STATUS usb_class_audio_get_cur_graphic_eq(AUDIO_COMMAND_PTR,uint_16,uint_8,pointer);
extern USB_STATUS usb_class_audio_set_cur_graphic_eq(AUDIO_COMMAND_PTR,uint_16,uint_8,pointer);

extern USB_STATUS usb_class_audio_get_min_graphic_eq(AUDIO_COMMAND_PTR,uint_16,uint_8,pointer);
extern USB_STATUS usb_class_audio_set_min_graphic_eq(AUDIO_COMMAND_PTR,uint_16,uint_8,pointer);

extern USB_STATUS usb_class_audio_get_max_graphic_eq(AUDIO_COMMAND_PTR,uint_16,uint_8,pointer);
extern USB_STATUS usb_class_audio_set_max_graphic_eq(AUDIO_COMMAND_PTR,uint_16,uint_8,pointer);

extern USB_STATUS usb_class_audio_get_res_graphic_eq(AUDIO_COMMAND_PTR,uint_16,uint_8,pointer);
extern USB_STATUS usb_class_audio_set_res_graphic_eq(AUDIO_COMMAND_PTR,uint_16,uint_8,pointer);

extern USB_STATUS usb_class_audio_endpoint_command(AUDIO_COMMAND_PTR,pointer,uint_32);

extern USB_STATUS usb_class_audio_get_mem_endpoint(AUDIO_COMMAND_PTR,uint_16,uint_16,pointer);
extern USB_STATUS usb_class_audio_set_mem_endpoint(AUDIO_COMMAND_PTR,uint_16,uint_16,pointer);

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_audio_cntrl_common
* Returned Value : USB_OK if command has been passed on USB.
* Comments       :
*     This function is used to send a control request.
*     It must be run with USB locked.
*
*END*--------------------------------------------------------------------*/
USB_STATUS usb_class_audio_cntrl_common
(
    /* [IN] The communication device common command structure */
    AUDIO_COMMAND_PTR           com_ptr,

    /* [IN] The audio control interface */
    //AUDIO_CONTROL_INTERFACE_STRUCT_PTR if_ctrl_ptr,

    /* [IN] Bitmask of the request type */
    uint_8                    bmrequesttype,

    /* [IN] Request code */
    uint_8                    brequest,

    /* [IN] Value to copy into WVALUE field of the REQUEST */
    uint_16                   wvalue,

    /* [IN] Value to copy into WINDEX field of the REQUEST */
    uint_16                   wIndex,

    /* [IN] Length of the data associated with REQUEST */
    uint_16                   wlength,

    /* [IN] Pointer to data buffer used to send/recv */
    uchar_ptr                 data
);

#ifdef __cplusplus
}
#endif

#endif
