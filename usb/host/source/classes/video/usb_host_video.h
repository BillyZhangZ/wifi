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
 * $FileName: usb_host_video.h$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file defines a template structure for Class Drivers.
 *
 *END************************************************************************/
#ifndef __usb_host_video_h__
#define __usb_host_video_h__
#include <mqx.h> /* integer types */
#include <lwevent.h>
#include <fio.h>
#include <bsp.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_cnfg.h"
#include "hostapi.h"
#include "host_rcv.h"
#include "host_ch9.h"
#include "host_snd.h"

/* Video Tierminal Types Code */

/* USB Terminal Type Codes */
#define TT_VENDOR_SPECIFIC                           (0x0100)
#define TT_STREAMING                                  (0x0101)

/* Input Terminal Type Codes */
#define ITT_VENDOR_SPECIFIC                           (0x0200)
#define ITT_CAMERA                                     (0x0201)
#define ITT_MEDIA_TRANSPORT_INPUT                       (0x0202)

/* Output Terminal Type Codes */
#define OTT_VENDOR_SPECIFIC                           (0x0300)
#define OTT_DISPLAY                                   (0x0301)
#define OTT_MEDIA_TRANSPORT_ONTPUT                    (0x0302)

/* External Terminal Type Codes */
#define EXTERNAL_VENDOR_SPECIFIC                        (0x0400)
#define COMPOSITE_CONNECTOR                            (0x0401)
#define SVIDEO_CONNECTOR                               (0x0402)
#define COMPONENT_CONNECTOR                            (0x0403)

/* Video Interface Class Code */
#define CC_VIDEO                          (0x0E)

/* Video Interface Subclass Codes */
#define SC_UNDEFINED                      (0x00)
#define SC_VIDEOCONTROL                   (0x01)
#define SC_VIDEOSTREAMING                 (0x02)
#define SC_VIDEO_INTERFACE_COLLECTION     (0x03)

/* Video Interface Protocol Codes */
#define PC_PROTOCOL_UNDEFINED             (0x00)

/* Video Class-Specific Descriptor Types */
#define CS_UNDEFINED                      (0x20)
#define CS_DEVICE                         (0x21)
#define CS_CONFIGURATION                  (0x22)
#define CS_STRING                         (0x23)
#define CS_INTERFACE                      (0x24)
#define CS_ENDPOINT                       (0x25)

/* Video Class-Specific VC Interface Descriptor Subtypes */
#define VC_DESCRIPTOR_UNDEFINED           (0x00)
#define VC_HEADER                         (0x01)
#define VC_INPUT_TERMINAL                 (0x02)
#define VC_OUTPUT_TERMINAL                (0x03)
#define VC_SECLECTOR_UNIT                 (0x04)
#define VC_PROCESSING_UNIT                (0x05)
#define VC_EXTENSION_UNIT                 (0x06)

/* Video Class-specific VS Interface Desriptor Subtypes */
#define VS_UNDEFINED                      (0x00)
#define VS_INPUT_HEADER                   (0x01)
#define VS_OUTPUT_HEADER                  (0x02)
#define VS_STILL_IMAGE_FRAME              (0x03)
#define VS_FORMAT_UNCOMPRESSED            (0x04)
#define VS_FRAME_UNCOMPRESSED             (0x05)
#define VS_FORMAT_MJPEG                   (0x06)
#define VS_FRAME_MJPEG                    (0x07)
#define VS_FORMAT_MPEG2TS                 (0x0A)
#define VS_FORMAT_DV                      (0x0C)
#define VS_COLORFORMAT                    (0x0D)
#define VS_FORMAT_FRAME_BASED             (0x10)
#define VS_FRAME_FRAME_BASED              (0x11)
#define VS_FORMAT_STREAM_BASED            (0x12)

/* Video Class-Specific Endpoint Descriptor Subtypes */
#define EP_UNDEFINED                      (0x00)
#define EP_GENERAL                        (0x01)
#define EP_ENDPOINT                       (0x02)
#define EP_INTERRUPT                      (0x03)

/* Video Class-Specific Request Codes */
#define EP_UNDEFINED                      (0x00)
#define SET_CUR                           (0x01)
#define GET_CUR                           (0x81)
#define GET_MIN                           (0x82)
#define GET_MAX                           (0x83)
#define GET_RES                           (0x84)
#define GET_LEN                           (0x85)
#define GET_INFO                          (0x86)
#define GET_DEF                           (0x87)


/* VideoControl Interface Control Selector Codes */
#define VC_CONTROL_UNDEFINED              (0x00)
#define VC_VIDEO_POWER_MODE_CONTROL       (0x01)
#define VC_REQUEST_ERROR_CODE_CONTROL     (0x02)

/* Terminal Control Selector Codes */
#define TE_CONTROL_UNDEFINED              (0x00)

/* Selector Unit Control Selector Codes */
#define SU_CONTROL_UNDEFINED               (0x00)
#define SU_INPUT_SELECT_CONTROL            (0x01)

/* Camera Terminal Control Selector Codes */
#define CT_CONTROL_UNDEFINED                (0x00)
#define CT_SCANNING_MODE_CONTROL            (0x01)
#define CT_AE_MODE_CONTROL                  (0x02)
#define CT_AE_PRIORITY_CONTROL              (0x03)
#define CT_EXPOSURE_TIME_ABSOLUTE_CONTROL   (0x04)
#define CT_EXPOSURE_TIME_RELATIVE_CONTROL   (0x05)
#define CT_FOCUS_ABSOLUTE_CONTROL           (0x06)
#define CT_FOCUS_RELATIVE_CONTROL           (0x07)
#define CT_FOCUS_AUTO_CONTROL               (0x08)
#define CT_IRIS_ABSOLUTE_CONTROL            (0x09)
#define CT_IRIS_RELATIVE_CONTROL            (0x0A)
#define CT_ZOOM_ABSOLUTE_CONTROL            (0x0B)
#define CT_ZOOM_RELATIVE_CONTROL            (0x0C)
#define CT_PANTILT_ABSOLUTE_CONTROL         (0x0D)
#define CT_PANTILT_RELATIVE_CONTROL         (0x0E)
#define CT_ROLL_ABSOLUTE_CONTROL            (0x0F)
#define CT_ROLL_RELATIVE_CONTROL            (0x10)
#define CT_PRIVACY_CONTROL                  (0x11)

/* Processing Unit Control Selector Codes */
#define PU_CONTROL_UNDEFINED                      (0x00)
#define PU_BACKLIGHT_COMPENSATION_CONTROL         (0x01)
#define PU_BRIGHTNESS_CONTROL                     (0x02)
#define PU_CONTRACT_CONTROL                       (0x03)
#define PU_GAIN_CONTROL                           (0x04)
#define PU_POWER_LINE_FREQUENCY_CONTROL           (0x05)
#define PU_HUE_CONTROL                            (0x06)
#define PU_SATURATION_CONTROL                     (0x07)
#define PU_SHARRNESS_CONTROL                      (0x08)
#define PU_GAMMA_CONTROL                          (0x09)
#define PU_WHITE_BALANCE_TEMPERATURE_CONTROL      (0x0A)
#define PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL (0x0B)
#define PU_WHITE_BALANCE_COMPONENT_CONTROL        (0x0C)
#define PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL   (0x0D)
#define PU_DIGITAL_MULTIPLIER_CONTROL             (0x0E)
#define PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL       (0x0F)
#define PU_HUE_AUTO_CONTROL                       (0x10)
#define PU_ANALOG_VIDEO_STANDARD_CONTROL          (0x11)
#define PU_ANALOG_LOCK_STATUS_CONTROL             (0x12)

/* Extension Unit Control Selectors Codes */
#define XU_CONTROL_UNDEFINED                       (0x00)

/* VideoStreming Unit Control Selector Codes */
#define VS_CONTROL_UNDEFINED                     (0x00)
#define VS_PROBE_CONTROL                         (0x01)
#define VS_COMMIT_CONTROL                        (0x02)
#define VS_STILL_PROBE_CONTROL                   (0x03)
#define VS_STILL_COMMIT_CONTROL                  (0x04)
#define VS_STILL_IMAGE_TRIGGER_CONTROL           (0x05)
#define VS_STREAM_ERROR_CODE_CONTROL             (0x06)
#define VS_GENERATE_KEY_FRAME_CONTROL            (0x07)
#define VS_UPDATE_FRAME_SEGMENT_CONTROL          (0x08)
#define VS_SYNCH_DELAY_CONTROL                   (0x09)

#define bHI_FID                                      (0x1 )
#define bHI_EOF                                      (0x2 )
#define bHI_PTS                                      (0x4 )
#define bHI_SCR                                      (0x8 )
#define bHI_RES                                      (0x10 )
#define bHI_STI                                      (0x20 )
#define bHI_ERR                                      (0x40 )
#define bHI_EOH                                      (0x80 )

/* ------------------------------------------------------------------------
 * GUIDs
 */
#define UVC_GUID_UVC_CAMERA \
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
#define UVC_GUID_UVC_OUTPUT \
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}
#define UVC_GUID_UVC_MEDIA_TRANSPORT_INPUT \
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03}
#define UVC_GUID_UVC_PROCESSING \
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01}
#define UVC_GUID_UVC_SELECTOR \
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02}

#define UVC_GUID_FORMAT_MJPEG \
        { 'M',  'J',  'P',  'G', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_YUY2 \
        { 'Y',  'U',  'Y',  '2', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_YUY2_ISIGHT \
        { 'Y',  'U',  'Y',  '2', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0x00, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_NV12 \
        { 'N',  'V',  '1',  '2', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_YV12 \
        { 'Y',  'V',  '1',  '2', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_I420 \
        { 'I',  '4',  '2',  '0', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_UYVY \
        { 'U',  'Y',  'V',  'Y', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y800 \
        { 'Y',  '8',  '0',  '0', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y8 \
        { 'Y',  '8',  ' ',  ' ', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y10 \
        { 'Y',  '1',  '0',  ' ', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y12 \
        { 'Y',  '1',  '2',  ' ', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y16 \
        { 'Y',  '1',  '6',  ' ', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_BY8 \
        { 'B',  'Y',  '8',  ' ', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_BA81 \
        { 'B',  'A',  '8',  '1', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_GBRG \
        { 'G',  'B',  'R',  'G', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_GRBG \
        { 'G',  'R',  'B',  'G', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_RGGB \
        { 'R',  'G',  'G',  'B', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_RGBP \
        { 'R',  'G',  'B',  'P', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_BGR3 \
        { 0x7d, 0xeb, 0x36, 0xe4, 0x4f, 0x52, 0xce, 0x11, \
         0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}
#define UVC_GUID_FORMAT_M420 \
        { 'M',  '4',  '2',  '0', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}

#define UVC_GUID_FORMAT_H264 \
        { 'H',  '2',  '6',  '4', 0x00, 0x00, 0x10, 0x00, \
         0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}

#define uvc_fourcc(a, b, c, d)\
        ((uint32_t)(a) | ((uint32_t)(b) << 8) | ((uint32_t)(c) << 16) | ((uint32_t)(d) << 24))

#define UVC_PIX_FMT_YUYV    uvc_fourcc('Y', 'U', 'Y', 'V') /* 16  YUV 4:2:2     */
#define UVC_PIX_FMT_NV12    uvc_fourcc('N', 'V', '1', '2') /* 12  Y/CbCr 4:2:0  */
#define UVC_PIX_FMT_MJPEG   uvc_fourcc('M', 'J', 'P', 'G') /* Motion-JPEG   */
#define UVC_PIX_FMT_YVU420  uvc_fourcc('Y', 'V', '1', '2') /* 12  YVU 4:2:0     */
#define UVC_PIX_FMT_YUV420  uvc_fourcc('Y', 'U', '1', '2') /* 12  YUV 4:2:0     */
#define UVC_PIX_FMT_M420    uvc_fourcc('M', '4', '2', '0') /* 12  YUV 4:2:0 2 lines y, 1 line uv interleaved */
#define UVC_PIX_FMT_UYVY    uvc_fourcc('U', 'Y', 'V', 'Y') /* 16  YUV 4:2:2     */
#define UVC_PIX_FMT_GREY    uvc_fourcc('G', 'R', 'E', 'Y') /*  8  Greyscale     */
#define UVC_PIX_FMT_Y10     uvc_fourcc('Y', '1', '0', ' ') /* 10  Greyscale     */
#define UVC_PIX_FMT_Y12     uvc_fourcc('Y', '1', '2', ' ') /* 12  Greyscale     */
#define UVC_PIX_FMT_Y16     uvc_fourcc('Y', '1', '6', ' ') /* 16  Greyscale     */
#define UVC_PIX_FMT_SBGGR8  uvc_fourcc('B', 'A', '8', '1') /*  8  BGBG.. GRGR.. */
#define UVC_PIX_FMT_SGBRG8  uvc_fourcc('G', 'B', 'R', 'G') /*  8  GBGB.. RGRG.. */
#define UVC_PIX_FMT_SGRBG8  uvc_fourcc('G', 'R', 'B', 'G') /*  8  GRGR.. BGBG.. */
#define UVC_PIX_FMT_SRGGB8  uvc_fourcc('R', 'G', 'G', 'B') /*  8  RGRG.. GBGB.. */
#define UVC_PIX_FMT_RGB565  uvc_fourcc('R', 'G', 'B', 'P') /* 16  RGB-5-6-5     */
#define UVC_PIX_FMT_BGR24   uvc_fourcc('B', 'G', 'R', '3') /* 24  BGR-8-8-8     */
#define UVC_PIX_FMT_H264    uvc_fourcc('H', '2', '6', '4') /* H264 with start codes */
#define UVC_PIX_FMT_DV      uvc_fourcc('d', 'v', 's', 'd') /* 1394          */

/* Format flags */
#define UVC_FMT_FLAG_COMPRESSED         0x00000001
#define UVC_FMT_FLAG_STREAM             0x00000002
#define UVC_FMT_FLAG_FIXEDSIZE          0x00000004


#define UVCINTBUFCOUNT                  5
#define UVCDATAISOBUFCOUNT             40 

#define IO_IOCTL_UVC_VIDEO_SET_FORMAT    _IO(40,0x01)
#define IO_IOCTL_UVC_ENABLE_VIDEO        _IO(40,0x02)

/* Class-specific VC  Interface Header Descriptor */
PACKED_STRUCT_BEGIN
struct _video_vc_interface_header_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint16_t bcdUVC;
    uint16_t wTotalLength;
    uint32_t dwClockFrequency;
    uint8_t  bInCollection;
    uint8_t  baInterfaceNr1;
} PACKED_STRUCT_END;
typedef struct _video_vc_interface_header_descriptor video_vc_interface_header_descriptor_t;


/* Input Terminal Descriptor */
PACKED_STRUCT_BEGIN
struct _video_input_terminal_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  iTerminal;
} PACKED_STRUCT_END;
typedef struct _video_input_terminal_descriptor video_input_terminal_descriptor_t;

/* Output Terminal Descriptor */
PACKED_STRUCT_BEGIN
struct _video_output_terminal_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  bSourceID;
    uint8_t  iTerminal;
} PACKED_STRUCT_END;
typedef struct _video_output_terminal_descriptor video_output_terminal_descriptor_t;

/* Camera Terminal Descriptor */
PACKED_STRUCT_BEGIN
struct _video_camera_terminal_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bTerminalID;
    uint16_t wTerminalType;
    uint8_t  bAssocTerminal;
    uint8_t  iTerminal;
    uint16_t wObjectiveFocalLengthMin;
    uint16_t wObjectiveFocalLengthMax;
    uint16_t wOcularFocalLength;
    uint8_t  bControlSize;
    uint16_t bmControls;
} PACKED_STRUCT_END;
typedef struct _video_camera_terminal_descriptor video_camera_terminal_descriptor_t;

/* Selector Unit Descriptor */
PACKED_STRUCT_BEGIN
struct _video_selector_unit_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bUnitID;
    uint8_t  bNrInPins;
} PACKED_STRUCT_END;
typedef struct _video_selector_unit_descriptor video_selector_unit_descriptor_t;

/* Processing Unit Descriptor */
PACKED_STRUCT_BEGIN
struct _video_processing_unit_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bUnitID;
    uint8_t  bSourceID;
    uint16_t wMaxMultiplier;
    uint8_t  bControlSize;
} PACKED_STRUCT_END;
typedef struct _video_processing_unit_descriptor video_processing_unit_descriptor_t;

/* Extension Unit Descriptor */
PACKED_STRUCT_BEGIN
struct _video_extension_unit_descriptor 
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bUnitID;
    uint8_t  guidExtensionCode[16];
    uint8_t  bNumControls;
    uint8_t  bNrInPins;
} PACKED_STRUCT_END;
typedef struct _video_extension_unit_descriptor video_extension_unit_descriptor_t;

PACKED_STRUCT_BEGIN
struct _video_extension_unit_descriptor_list
{
    uint32_t extension_num;
    void*   extension_descriptor_list_ptr;
} PACKED_STRUCT_END;
typedef struct _video_extension_unit_descriptor_list video_extension_unit_descriptor_list_t;

/* Class-specific VC Interrupt Endpo int Descriptor */
PACKED_STRUCT_BEGIN
struct _video_vc_interrupt_endpoint_descriptor 
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint16_t wMaxTransferSize;
} PACKED_STRUCT_END;
typedef struct _video_vc_interrupt_endpoint_descriptor video_vc_interrupt_endpoint_descriptor_t;

/* Class-specific VS  Interface Input Header Descriptor */
PACKED_STRUCT_BEGIN
struct _video_vs_interface_input_header_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bNumFormats;
    uint16_t wTotalLength;
    uint8_t  bEndpointAddress;
    uint8_t  bmInfo;
    uint8_t  bTerminalLink;
    uint8_t  bStillCaptureMethod;
    uint8_t  bTriggerSupport;
    uint8_t  bTriggerUsage;
    uint8_t  bControlSize;
} PACKED_STRUCT_END;
typedef struct _video_vs_interface_input_header_descriptor video_vs_interface_input_header_descriptor_t;

/* Class-specific VS  Interface Output Header Descriptor */
PACKED_STRUCT_BEGIN
struct _video_vs_interface_output_header_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bNumFormats;
    uint16_t wTotalLength;
    uint8_t  bEndpointAddress;
    uint8_t  bTerminalLink;
    uint8_t  bControlSize;
} PACKED_STRUCT_END;
typedef struct _video_vs_interface_output_header_descriptor video_vs_interface_output_header_descriptor_t;

/* Payload Format Descriptor */
PACKED_STRUCT_BEGIN
struct _video_payload_format_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bFormatIndex;
} PACKED_STRUCT_END;
typedef struct _video_payload_format_descriptor video_payload_format_descriptor_t;

/* Motion-JPEG Video Format Descriptor */
PACKED_STRUCT_BEGIN
struct _video_payload_mjpeg_video_format_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bFormatIndex;
    uint8_t  bNumFrameDescriptors;
    uint8_t  bmFlags;
    uint8_t  bDefaultFrameIndex;
    uint8_t  bAspectRatioX;
    uint8_t  bAspectRatioY;
    uint8_t  bmInterlaceFlags;
    uint8_t  bCopyProtect;
} PACKED_STRUCT_END;
typedef struct _video_payload_mjpeg_video_format_descriptor video_payload_mjpeg_video_format_descriptor_t;

/* MPEG-2 TS Format Descriptor */
PACKED_STRUCT_BEGIN
struct _video_payload_mpeg2ts_video_format_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bFormatIndex;
    uint8_t  bDataOffset;
    uint8_t  bPacketLength;
    uint8_t  bStrideLength;
    uint8_t  guidStrideFormat[16];
} PACKED_STRUCT_END;
typedef struct _video_payload_mpeg2ts_video_format_descriptor video_payload_mpeg2ts_video_format_descriptor_t;

PACKED_STRUCT_BEGIN 
struct _video_payload_video_frame_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bFrameIndex;
} PACKED_STRUCT_END;
typedef struct _video_payload_video_frame_descriptor video_payload_video_frame_descriptor_t;

/* Motion-JPEG Video Frame Descriptor */
PACKED_STRUCT_BEGIN 
struct _video_payload_mjpeg_video_frame_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bFrameIndex;
    uint8_t  bmCapabilities;
    uint16_t wWitd;
    uint16_t wHeight;
    uint32_t dwMinBitRate;
    uint32_t dwMaxBitRate;
    uint32_t dwMaxVideoFrameBufferSize;
    uint32_t dwDefaultFrameInterval;
    uint8_t  bFrameIntervalType;
    uint32_t dwFrameInterval;
} PACKED_STRUCT_END;
typedef struct _video_payload_mjpeg_video_frame_descriptor video_payload_mjpeg_video_frame_descriptor_t;

/* Still Image Frame Descriptor */
PACKED_STRUCT_BEGIN
struct _video_vs_still_image_frame_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bEndpointAddress;
    uint8_t  bNumImageSizePatterns;
} PACKED_STRUCT_END;
typedef struct _video_vs_still_image_frame_descriptor video_vs_still_image_frame_descriptor_t;

/* Color Matching Descriptor */
PACKED_STRUCT_BEGIN
struct _video_vs_color_matching_descriptor 
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bColorPrimaries;
    uint8_t  bTransferCharacteristics;
    uint8_t  bMatrixCoefficients;
} PACKED_STRUCT_END;
typedef struct _video_vs_color_matching_descriptor video_vs_color_matching_descriptor_t;

typedef struct _video_vs_mjpeg_frame_descriptor_list
{
    uint32_t frame_num;
    video_payload_mjpeg_video_frame_descriptor_t*   frame_descriptor_list_ptr;
} video_vs_mjpeg_frame_descriptor_list_t;

/* Payload Format Descriptor */
PACKED_STRUCT_BEGIN
struct _video_common_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
} PACKED_STRUCT_END;
typedef struct _video_common_descriptor video_common_descriptor_t;

PACKED_STRUCT_BEGIN
struct _video_frame_common_descriptor
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bFrameIndex;
} PACKED_STRUCT_END;
typedef struct _video_frame_common_descriptor video_frame_common_descriptor_t;


struct video_pix_format {
    uint32_t                    width;
    uint32_t                    height;
    uint32_t                    pixelformat;
    uint32_t                    field;          /* enum v4l2_field */
    int32_t                     bytesperline;   /* for padding, zero if unused */
    uint32_t                    sizeimage;
    uint32_t                    colorspace;     /* enum v4l2_colorspace */
    uint32_t                    priv;           /* private data, depends on pixelformat */
    uint32_t                    flags;          /* format flags (V4L2_PIX_FMT_FLAG_*) */
    uint32_t                    interval;
    uint32_t                    maxpayloadsize;
};

/* Video Probe and Commit Controls  See USB_Video_Class_1.1 Table 4-47 */
/* 4.3.1.1. Video Probe and Commit Controls */
PACKED_STRUCT_BEGIN
struct uvc_streaming_control {
        uint16_t bmHint;
        uint8_t  bFormatIndex;
        uint8_t  bFrameIndex;
        uint32_t dwFrameInterval;
        uint16_t wKeyFrameRate;
        uint16_t wPFrameRate;
        uint16_t wCompQuality;
        uint16_t wCompWindowSize;
        uint16_t wDelay;
        uint32_t dwMaxVideoFrameSize;
        uint32_t dwMaxPayloadTransferSize;
        uint32_t dwClockFrequency;
        uint8_t  bmFramingInfo;
        uint8_t  bPreferedVersion;
        uint8_t  bMinVersion;
        uint8_t  bMaxVersion;
} PACKED_STRUCT_END;

struct uvc_format_desc {
        char    *name;
        uint8_t  guid[16];
        uint32_t fcc;
};

struct uvc_frame {
        uint8_t   bFrameIndex;
        uint8_t   bmCapabilities;
        uint16_t  wWidth;
        uint16_t  wHeight;
        uint32_t  dwMinBitRate;
        uint32_t  dwMaxBitRate;
        uint32_t  dwMaxVideoFrameBufferSize;
        uint8_t   bFrameIntervalType;
        uint32_t  dwDefaultFrameInterval;
        uint32_t *dwFrameInterval;
};

struct uvc_format {
        uint8_t type;
        uint8_t index;
        uint8_t bpp;
        uint8_t colorspace;
        uint32_t fcc;
        uint32_t flags;

        char name[32];

        unsigned int nframes;
        struct uvc_frame *frame;
};

#define UVC_PKT_DATA   0x1
#define UVC_PKT_STATUS 0x2

typedef struct {
    struct {
        uint8_t type;
    }usb_help;

    uint8_t *data;
}UVCPACKET, _PTR_ UVCPACKET_PTR;

typedef struct{
    uint32_t  pkt_type;
    uint32_t  len;
    uint32_t  offset;
    uint8_t   data[0];
} UVCDATABUF, _PTR_ UVCDATABUFP;

#define UVC_RING_BUF_SIZE    (48*1024)
struct uvc_ringbuf {
	int      len;
	int      actual;
	int      required;
	int      wp;             /* next write index avaliable for write */
	int      rp;             /* next read  index avaliable for read  */
        uint8_t *buf;
};

typedef struct {
    UVCDATABUFP dsp;
} UVCMSG, _PTR_ UVCMSGP;

typedef struct uvc_device_struct {
    CLASS_CALL_STRUCT       ccs; /* Class-specific info */
    LWEVENT_STRUCT          uvc_event;
    /* The granularity of message queue is one message. Its size is the multiplier of _mqx_max_type. Get that multiplier */
#define UVC_TASKQ_GRANM ((sizeof(UVCMSG) - 1) / sizeof(_mqx_max_type) + 1)
    _mqx_max_type           uvc_taskq[(UVCINTBUFCOUNT + UVCDATAISOBUFCOUNT ) * UVC_TASKQ_GRANM * sizeof(_mqx_max_type)]; /* prepare message queue for 20 events */
} UVC_DEVICE_STRUCT,  _PTR_ UVC_DEVICE_STRUCT_PTR;

/*
 ** UVC Class Interface structure. This structure will be passed to
 ** all commands to this class driver.
 */
typedef struct usb_uvc_general_class {
    /* Each class must start with a GENERAL_CLASS struct */
    GENERAL_CLASS            G;

    /* Higher level callback and parameter */
    tr_callback              USER_CALLBACK;
    pointer                  USER_PARAM;

    uint_8                   IFNUM;

} UVC_GENERAL_CLASS, _PTR_ UVC_GENERAL_CLASS_PTR;


struct vsaltintf {
#define VS_MAX_ALT_INTF       15
    INTERFACE_DESCRIPTOR_PTR  intfs[VS_MAX_ALT_INTF];   /* all stream alt interfaces desc */
    ENDPOINT_DESCRIPTOR_PTR   ep[VS_MAX_ALT_INTF];      /* all stream alt interfaces endpoint desc, for every inteface, only have max one end point, for the first interface, none ep */
    int                       alts;                     /* video stream alt interfaces count  */
    int                       maxepsize;                /* max ep size of all alt interfaces  */
};

typedef struct {
    UVC_GENERAL_CLASS        UVC_G;
    /* Address of bound control interface */
    CLASS_CALL_STRUCT_PTR    BOUND_CONTROL_INTERFACE;

    void                     *dev;

    PIPE_STRUCT_PTR          interrupt_pipe;
    PIPE_STRUCT_PTR          cmd_pipe;
    INTERFACE_DESCRIPTOR_PTR vcintf; 
    INTERFACE_DESCRIPTOR_PTR vsintf;              /* the first vs alt interface, zero endponit for this interface, zero bandwith */ 
    INTERFACE_DESCRIPTOR_PTR cur_vsintf;          /* current configured video stream interface desc  */
    uint8_t                  cur_alt;             /* current configured video stream interface index */
    struct vsaltintf         vsaltintf;
    PIPE_STRUCT_PTR          vspipedesc;          /* current configured video stream interface's endpoint desc */
    uint8_t                  streamenabled;

    PIPE_BUNDLE_STRUCT_PTR   pbs_ptr;

    char_ptr                 device_name;

#define USB_DATA_DETACH            0x01
#define USB_DATA_READ_COMPLETE     0x02
#define USB_DATA_READ_PIPE_FREE    0x04
#define USB_DATA_SEND_COMPLETE     0x08
#define USB_DATA_SEND_PIPE_FREE    0x10
#define USB_DATA_CTRL_COMPLETE     0x20
#define USB_DATA_CTRL_PIPE_FREE    0x40
#define USB_DATA_READ_READY        0x80

    LWEVENT_STRUCT_PTR        data_event;
    _mqx_max_type_ptr         uvc_taskq;


#define VS_MAX_FORMAT                             5
#define VS_MAX_FRAME                              60
#define VS_MAX_INTERVALS                          300

    uint32_t vsformatcount;
    uint32_t vsframecount;
    uint32_t vsintervalcount;
    
    struct uvc_format                             vs_format[VS_MAX_FORMAT];
    struct uvc_frame                              vs_frame[VS_MAX_FRAME];
    uint32_t                                      uvc_intervals[VS_MAX_INTERVALS];

    struct uvc_format                            *cur_format;                       /* current video stream used video fromat */
    struct uvc_frame                             *cur_frame;                        /* current video stream used video frame  */
    struct uvc_format                            *def_format;

    struct uvc_streaming_control                  vs_control;
    uint16_t                                      uvcversion;

    USB_STATUS                                    ch9status;
    uint32_t                                      ch9transfered;

    uint8_t                                       streambuf[1024];                  /* USB video stream ISO transfer buffer */

    video_vc_interface_header_descriptor_t        *vc_interface_header_ptr;
    video_input_terminal_descriptor_t             *vc_input_terminal_ptr;
    video_output_terminal_descriptor_t            *vc_output_terminal_ptr;
    video_processing_unit_descriptor_t            *vc_processing_unit_ptr;

    video_vs_interface_output_header_descriptor_t *vs_input_header_ptr;


    struct uvc_ringbuf                            ringbuf;                          /* USB video stream ring buffer, which is between streambuf and file read operation */


    CLASS_CALL_STRUCT_PTR     APP;               /* Store app handle, can help finding if lost */

} UVC_CLASS_INTF_STRUCT, _PTR_ UVC_CLASS_INTF_STRUCT_PTR;


/* Class specific functions exported by HID class driver */
#ifdef __cplusplus
extern "C" {
#endif

void              usb_class_video_init     (PIPE_BUNDLE_STRUCT_PTR  pbs_ptr, CLASS_CALL_STRUCT_PTR ccs_ptr);
extern USB_STATUS usb_class_uvc_get_app    (_usb_device_instance_handle, _usb_interface_descriptor_handle, CLASS_CALL_STRUCT_PTR _PTR_);
extern USB_STATUS usb_class_uvc_use_lwevent(CLASS_CALL_STRUCT_PTR, LWEVENT_STRUCT_PTR);
extern USB_STATUS usb_class_uvc_use_lwmsgq (CLASS_CALL_STRUCT_PTR, _mqx_max_type *);
USB_STATUS usb_class_video_install_driver  (CLASS_CALL_STRUCT_PTR, char_ptr);

#ifdef __cplusplus
}
#endif

#endif
