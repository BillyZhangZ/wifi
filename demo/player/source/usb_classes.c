/**HEADER********************************************************************
* 
* Copyright (c) 2010 Freescale Semiconductor;
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
* $FileName: usb_classes.c$
* $Version : 3.7.0.1$
* $Date    : Jul-16-2012$
*
* Comments:
*
*   This file contains  the USB class driver mapping table
*
*END************************************************************************/


#include "hostapi.h"
#include "host_ch9.h"
#include "host_common.h"

/* get application-specific configuration */
#include "usb_classes.h"
#include "usb_host_bluetooth.h"

#include "mqx_host.h"

CLASS_MAP class_interface_map[] =
{
#ifdef USBCLASS_INC_MASS
   {
      usb_class_mass_init,
      sizeof(USB_MASS_CLASS_INTF_STRUCT),
      USB_CLASS_MASS_STORAGE,
      USB_SUBCLASS_MASS_UFI,
      USB_PROTOCOL_MASS_BULK,
      0xFF, 0x00, 0xFF
   },
#endif
#ifdef USBCLASS_INC_PRINTER
   {
      usb_printer_init,
      sizeof(PRINTER_INTERFACE_STRUCT),
      USB_CLASS_PRINTER,
      USB_SUBCLASS_PRINTER,
      USB_PROTOCOL_PRT_BIDIR,
      0xFF, 0xFF, 0xFF
   },
#endif
#ifdef USBCLASS_INC_HID
   {
      usb_class_hid_init,
      sizeof(USB_HID_CLASS_INTF_STRUCT),
      USB_CLASS_HID,
      0,
      0,
      0xFF, 0x00, 0x00
   },
#endif
#ifdef USBCLASS_INC_CDC
   {
      usb_class_cdc_acm_init,
      sizeof(USB_ACM_CLASS_INTF_STRUCT),
      USB_CLASS_COMMUNICATION,
      USB_SUBCLASS_COM_ABSTRACT,
      USB_PROTOCOL_COM_NOSPEC,
      0xFF, 0xFF, 0xFE
   },
   {
      usb_class_cdc_data_init,
      sizeof(USB_DATA_CLASS_INTF_STRUCT),
      USB_CLASS_DATA,
      0,
      0,
      0xFF, 0x00, 0x00
   },
#endif
/*
#ifdef USBCLASS_INC_AUDIO
   {
      usb_class_audio_control_init,
      sizeof(AUDIO_CONTROL_INTERFACE_STRUCT),
      USB_CLASS_AUDIO,
      USB_SUBCLASS_AUD_CONTROL,
      0,
      0xFF, 0xFF, 0x00
   },
   {
      usb_class_audio_stream_init,
      sizeof(AUDIO_STREAM_INTERFACE_STRUCT),
      USB_CLASS_AUDIO,
      USB_SUBCLASS_AUD_STREAMING,
      0,
      0xFF, 0xFF, 0x00
   },
#endif */
#ifdef USBCLASS_INC_AUDIO
   { /* liuadd for accessory */
      usb_aoa_init,
      sizeof(AOA_INTERFACE_STRUCT),
      0xff,
      0xff,
      0,
      0xFF, 0xFF, 0x00
   }, 
#ifdef USE_USB_AUDIO
  #ifdef USE_AUDIO_CONTROL
     {
      usb_class_audio_control_init,
      sizeof(AUDIO_CONTROL_INTERFACE_STRUCT),
      USB_CLASS_AUDIO,
      USB_SUBCLASS_AUD_CONTROL,
      0,
      0xFF, 0xFF, 0x00
   },
#endif
   {
      usb_class_audio_stream_init,
      sizeof(AUDIO_STREAM_INTERFACE_STRUCT),
      USB_CLASS_AUDIO,
      USB_SUBCLASS_AUD_STREAMING,
      0,
      0xFF, 0xFF, 0x00
   },
#endif   
#endif

#ifdef USBCLASS_INC_PHDC
   {
      usb_class_phdc_init,
      sizeof(USB_PHDC_CLASS_INTF_STRUCT),
      USB_CLASS_PHDC,
      0,
      0,
      0xFF, 0x00, 0x00
   },
#endif   
#ifdef USBCLASS_INC_HUB
   {
      usb_class_hub_init,
      sizeof(USB_HUB_CLASS_INTF_STRUCT),
      USB_CLASS_HUB,
      USB_SUBCLASS_HUB_NONE,
      USB_PROTOCOL_HUB_FS,
      0xFF, 0x00, 0x00
   },
#endif
#ifdef USBCLASS_INC_BLUETOOTH
   {
      usb_class_bluetooth_init,
      sizeof(USB_BLUETOOTH_CLASS_INTF_STRUCT),
      USB_CLASS_BLUETOOTH,
      USB_SUBCLASS_BLUETOOTH,
      USB_PROTOCOL_BLUETOOTH,
      0xFF, 0x0F, 0x0F
   },
#endif
   {
      NULL,
      0,
      0, 0, 0,
      0, 0, 0
   }
};

/* EOF */
