/**HEADER********************************************************************
 * 
 * Copyright (c) 2008, 2013 - 2015 Freescale Semiconductor;
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
 * $FileName: usb_host_video.c$
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   This file Contains the implementation of class driver for Video devices.
 *   Video device unplug is not implemented.
 *
 *END************************************************************************/
#include "usb_host_video.h"
#include "host_dev_list.h"
#include "host_ch9.h"
#include "host_common.h"
#include <lwmsgq.h>


#define max(a, b)  ((a) > (b) ? a : b)
#define min(a, b)  ((a) < (b) ? a : b)


static _mqx_int _io_video_open(MQX_FILE_PTR, char _PTR_, char _PTR_);
static _mqx_int _io_video_close(MQX_FILE_PTR);
static _mqx_int _io_video_read(MQX_FILE_PTR, char_ptr, _mqx_int);
static _mqx_int _io_video_write(MQX_FILE_PTR, char_ptr, _mqx_int);
static _mqx_int _io_video_ioctl(MQX_FILE_PTR, _mqx_uint, pointer);
static _mqx_int _io_video_uninstall(IO_DEVICE_STRUCT_PTR);

static pointer                         data_anchor_abstract; /* used for lower abstract layer */

/* ------------------------------------------------------------------------
 * Video formats
 */

static struct uvc_format_desc uvc_fmts[] = {
        {
                .name           = "YUV 4:2:2 (YUYV)",
                .guid           = UVC_GUID_FORMAT_YUY2,
                .fcc            = UVC_PIX_FMT_YUYV,
        },
        {
                .name           = "YUV 4:2:2 (YUYV)",
                .guid           = UVC_GUID_FORMAT_YUY2_ISIGHT,
                .fcc            = UVC_PIX_FMT_YUYV,
        },
        {
                .name           = "YUV 4:2:0 (NV12)",
                .guid           = UVC_GUID_FORMAT_NV12,
                .fcc            = UVC_PIX_FMT_NV12,
        },
        {
                .name           = "MJPEG",
                .guid           = UVC_GUID_FORMAT_MJPEG,
                .fcc            = UVC_PIX_FMT_MJPEG,
        },
        {
                .name           = "YVU 4:2:0 (YV12)",
                .guid           = UVC_GUID_FORMAT_YV12,
                .fcc            = UVC_PIX_FMT_YVU420,
        },
        {
                .name           = "YUV 4:2:0 (I420)",
                .guid           = UVC_GUID_FORMAT_I420,
                .fcc            = UVC_PIX_FMT_YUV420,
        },
        {
                .name           = "YUV 4:2:0 (M420)",
                .guid           = UVC_GUID_FORMAT_M420,
                .fcc            = UVC_PIX_FMT_M420,
        },
        {
                .name           = "YUV 4:2:2 (UYVY)",
                .guid           = UVC_GUID_FORMAT_UYVY,
                .fcc            = UVC_PIX_FMT_UYVY,
        },
        {
                .name           = "Greyscale 8-bit (Y800)",
                .guid           = UVC_GUID_FORMAT_Y800,
                .fcc            = UVC_PIX_FMT_GREY,
        },
        {
                .name           = "Greyscale 8-bit (Y8  )",
                .guid           = UVC_GUID_FORMAT_Y8,
                .fcc            = UVC_PIX_FMT_GREY,
        },
        {
                .name           = "Greyscale 10-bit (Y10 )",
                .guid           = UVC_GUID_FORMAT_Y10,
                .fcc            = UVC_PIX_FMT_Y10,
        },
        {
                .name           = "Greyscale 12-bit (Y12 )",
                .guid           = UVC_GUID_FORMAT_Y12,
                .fcc            = UVC_PIX_FMT_Y12,
        },
        {
                .name           = "Greyscale 16-bit (Y16 )",
                .guid           = UVC_GUID_FORMAT_Y16,
                .fcc            = UVC_PIX_FMT_Y16,
        },
        {
                .name           = "BGGR Bayer (BY8 )",
                .guid           = UVC_GUID_FORMAT_BY8,
                .fcc            = UVC_PIX_FMT_SBGGR8,
        },
        {
                .name           = "BGGR Bayer (BA81)",
                .guid           = UVC_GUID_FORMAT_BA81,
                .fcc            = UVC_PIX_FMT_SBGGR8,
        },
        {
                .name           = "GBRG Bayer (GBRG)",
                .guid           = UVC_GUID_FORMAT_GBRG,
                .fcc            = UVC_PIX_FMT_SGBRG8,
        },
        {
                .name           = "GRBG Bayer (GRBG)",
                .guid           = UVC_GUID_FORMAT_GRBG,
                .fcc            = UVC_PIX_FMT_SGRBG8,
        },
        {
                .name           = "RGGB Bayer (RGGB)",
                .guid           = UVC_GUID_FORMAT_RGGB,
                .fcc            = UVC_PIX_FMT_SRGGB8,
        },
        {
                .name           = "RGB565",
                .guid           = UVC_GUID_FORMAT_RGBP,
                .fcc            = UVC_PIX_FMT_RGB565,
        },
        {
                .name           = "BGR 8:8:8 (BGR3)",
                .guid           = UVC_GUID_FORMAT_BGR3,
                .fcc            = UVC_PIX_FMT_BGR24,
        },
        {
                .name           = "H.264",
                .guid           = UVC_GUID_FORMAT_H264,
                .fcc            = UVC_PIX_FMT_H264,
        },
};


/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_uvc_get_app
 * Returned Value : CLASS_CALL_STRUCT_PTR if class found
 * Comments       :
 *     This function returns stored application handle as it was passed as
 *     a param in select_interface.
 *
 *END*--------------------------------------------------------------------*/
USB_STATUS usb_class_uvc_get_app
(
 /* [IN] handle of device */
 _usb_device_instance_handle dev_ptr,

 /* [IN] pointer to interface descriptor */
 _usb_interface_descriptor_handle intf_ptr,

 /* [OUT] pointer to CLASS_CALL_STRUCT to be filled in */
 CLASS_CALL_STRUCT_PTR   _PTR_ ccs_ptr
 )
{
    USB_STATUS                    error;
    GENERAL_CLASS_PTR             parser;

    USB_lock();

    error = usb_hostdev_validate (dev_ptr);
    if (error != USB_OK) {
        USB_unlock();
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_class_hub_get_app, FAILED");
#endif
        return USB_log_error(__FILE__,__LINE__,error);
    } /* EndIf */

    for (parser = (GENERAL_CLASS_PTR) data_anchor_abstract; parser != NULL; parser = parser->next) {
        if (parser->dev_handle == dev_ptr && parser->intf_handle == intf_ptr)
            break;
    }

    if (parser != NULL) {
        UVC_CLASS_INTF_STRUCT_PTR uvcd = (UVC_CLASS_INTF_STRUCT_PTR) parser;
        *ccs_ptr = uvcd->APP;
    }
    else {
        USB_unlock();
#ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_class_uvc_get_app, not found");
#endif
        return USB_log_error(__FILE__,__LINE__,USBERR_NOT_FOUND);
    }

    USB_unlock();
#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_uvc_get_app, SUCCESSFUL");
#endif
    return USB_OK;

}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_uvc_data_use_lwevent
 * Returned Value : None
 * Comments       :
 *     This function is injector of events that are used in the class but
 *     the destruction are allowed only in task context.
 *
 *END*--------------------------------------------------------------------*/

USB_STATUS usb_class_uvc_use_lwevent
(
 /* [IN] acm call struct pointer */
 CLASS_CALL_STRUCT_PTR       ccs_ptr,

 /* [IN] acm event */
 LWEVENT_STRUCT_PTR          data_event
 )
{
    UVC_CLASS_INTF_STRUCT_PTR if_ptr = (UVC_CLASS_INTF_STRUCT_PTR)ccs_ptr->class_intf_handle;

    if (data_event == NULL) {
        return USBERR_INIT_DATA;
    }

    if_ptr->data_event = data_event;

    /* prepare events to be auto or manual */
    _lwevent_set_auto_clear(if_ptr->data_event, 
            USB_DATA_READ_PIPE_FREE | USB_DATA_SEND_PIPE_FREE | USB_DATA_CTRL_PIPE_FREE | USB_DATA_DETACH);
    /* pre-set events */
    _lwevent_clear(if_ptr->data_event, 0xFFFFFFFF);
    _lwevent_set(if_ptr->data_event, USB_DATA_READ_PIPE_FREE | USB_DATA_SEND_PIPE_FREE | USB_DATA_CTRL_PIPE_FREE);

    return USB_OK;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_uvc_data_use_lwmsgq
 * Returned Value : None
 * Comments       :
 *     This function is injector of events that are used in the class but
 *     the destruction are allowed only in task context.
 *
 *END*--------------------------------------------------------------------*/

USB_STATUS usb_class_uvc_use_lwmsgq
(
 /* [IN] acm call struct pointer */
 CLASS_CALL_STRUCT_PTR       ccs_ptr,
 _mqx_max_type_ptr           uvc_taskq

 )
{
    UVC_CLASS_INTF_STRUCT_PTR if_ptr = (UVC_CLASS_INTF_STRUCT_PTR)ccs_ptr->class_intf_handle;

    if (uvc_taskq == NULL) {
        return USBERR_INIT_DATA;
    }

    if_ptr->uvc_taskq = uvc_taskq;

    if (_lwmsgq_init(if_ptr->uvc_taskq, (UVCINTBUFCOUNT + UVCDATAISOBUFCOUNT), UVC_TASKQ_GRANM) != MQX_OK) {
        printf("usb_class_uvc_use_lwmsgq  _lwmsgq_init failed \n");
        return -1;
    }
    return USB_OK;
}

/* buf can't be user space, because this function maybe called at interupt context */
static int usb_class_video_ringbuffer_read(struct uvc_ringbuf *ringbuf, int alignLen, int len, uint8_t *buf)
{
    int    avaLen    = 0;  /* avaLen == ringbuf->actual ? */
    int   copyLen    = 0;
    int    oActual   = ringbuf->actual;

    /* align with alignLen */
    len -= len % alignLen;

    if(ringbuf->wp >= ringbuf->rp)
        /* if wp == rp  no data at all */
    {
        avaLen   = ringbuf->wp - ringbuf->rp;

        copyLen  = avaLen < len ? avaLen : len;
        copyLen -= copyLen % alignLen;

        memcpy(buf, ringbuf->buf + ringbuf->rp, copyLen);

        ringbuf->rp     += copyLen;
        ringbuf->actual -= copyLen;
    }
    else
    {
        avaLen  = ringbuf->len - ringbuf->rp + ringbuf->wp;

        copyLen  = avaLen < len ? avaLen : len;
        copyLen -= copyLen % alignLen;

        if(ringbuf->len - ringbuf->rp >= copyLen)
        {
            memcpy(buf, ringbuf->buf + ringbuf->rp, copyLen);

            ringbuf->rp += copyLen;
            if(ringbuf->rp == ringbuf->len)
                /* wrap */
                ringbuf->rp = 0;

            ringbuf->actual -= copyLen;

        }
        else
        {
            memcpy(buf, ringbuf->buf + ringbuf->rp, ringbuf->len - ringbuf->rp);
            memcpy(buf + ringbuf->len - ringbuf->rp, ringbuf->buf, copyLen - (ringbuf->len - ringbuf->rp));

            ringbuf->rp      = copyLen - (ringbuf->len - ringbuf->rp);
            ringbuf->actual -= copyLen;
        }

    }

    /* it equal to copyLen ? */
    return (oActual - ringbuf->actual);
}

/* buf can't be user space, because this function maybe called at interupt context */
static int usb_class_video_ringbuffer_write(struct uvc_ringbuf *ringbuf, int alignLen, int len, uint8_t *buf)
{
    int    avaLen    = 0;
    int   copyLen    = 0;
    int    oActual   = ringbuf->actual;
    int    ret       = 0;
    
    /* align with alignLen */
    len -= len % alignLen;

    if(ringbuf->wp >= ringbuf->rp)
	/* if wp == rp  no data at all */
    {
	avaLen  = ringbuf->len - ringbuf->wp + ringbuf->rp; 

	copyLen  = avaLen < len ? copyLen : len;
	copyLen -= copyLen % alignLen;

	if(ringbuf->len - ringbuf->wp >= copyLen)
	    /* copy no wrap */
	{
	    memcpy(ringbuf->buf + ringbuf->wp, buf, copyLen);

	    ringbuf->wp += copyLen;
	    if(ringbuf->wp == ringbuf->len)
		/* wrap ringbuf->len - ringbuf->wp == len */
		ringbuf->wp = 0;
	    ringbuf->actual += copyLen;
	}
	else
	    /* copy wrap */
	{
	    memcpy(ringbuf->buf + ringbuf->wp, buf, ringbuf->len - ringbuf->wp);
	    memcpy(ringbuf->buf, buf + ringbuf->len - ringbuf->wp, copyLen - (ringbuf->len - ringbuf->wp));

	    ringbuf->wp      = copyLen - (ringbuf->len - ringbuf->wp);
	    ringbuf->actual += copyLen;
	}
    }
    else
    {
	avaLen  = ringbuf->rp - ringbuf->wp;

	copyLen  = avaLen < len ? avaLen : len;
	copyLen -= copyLen % alignLen;

	memcpy(ringbuf->buf + ringbuf->wp, buf, copyLen);

	ringbuf->wp      = ringbuf->wp + copyLen;
	ringbuf->actual += copyLen;

    }

    /* it equal to copyLen ? */
    ret = ringbuf->actual - oActual;

    if(ringbuf->len - ringbuf->actual < alignLen)
    {
	ringbuf->rp     += (alignLen * (ringbuf->len /(alignLen * 3)));  /* if cache up read pointer, discard 1/3 ring buf */
	ringbuf->rp     %= ringbuf->len;
	ringbuf->actual -= (alignLen * (ringbuf->len /(alignLen * 3)));
	if(ringbuf->actual <= 0)
	{
	    printf("Ringbuf actual less than or equal 0, it shoud not happen! Need debug it!! \n");
	    printf("Ringbuf wp:%d, rp:%d, actual:%d \n", ringbuf->wp, ringbuf->rp, ringbuf->actual);
	}

	//printf("Ring buf full, discard the oldest 1/3 content, ring buf length is %d \n", ringbuf->len);
    }

    return (ret);
}

static void usb_class_video_printf_ctrl( struct uvc_streaming_control *control){
        printf("bmHint 0x%x \n"
               "bFormatIndex %d \n"
               "bFrameIndex %d  \n"
               "dwFrameInterval %d  \n"
               "wKeyFrameRate   %d  \n"
               "wPFrameRate     %d  \n"
               "wCompQuality    %d  \n"
               "wCompWindowSize %d  \n"
               "wDelay          %d  \n"  
               "dwMaxVideoFrameSize %d \n"
               "dwMaxPayloadTransferSize %d \n"
               "\n",
               control->bmHint,
               control->bFormatIndex,
               control->bFrameIndex,
               control->dwFrameInterval,
               control->wKeyFrameRate,
               control->wPFrameRate,
               control->wCompQuality,
               control->wCompWindowSize,
               control->wDelay,
               control->dwMaxVideoFrameSize,
               control->dwMaxPayloadTransferSize);

        return;
}

static void usb_class_video_ch9_callback
   (
      /* [IN] pointer to pipe */
      _usb_pipe_handle  pipe_handle,

      /* [IN] user-defined parameter */
      pointer           user_parm,

      /* [IN] buffer address */
      uchar_ptr         buffer,

      /* [IN] length of data transferred */
      uint_32           length_data_transfered,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      USB_STATUS        status
   )
{ /* Body */
    UVC_CLASS_INTF_STRUCT_PTR    if_ptr = (UVC_CLASS_INTF_STRUCT_PTR)user_parm;

    if_ptr->ch9status     = status;
    if_ptr->ch9transfered = length_data_transfered;
    //if_ptr->ch9done       = 1;

    if (if_ptr->data_event != NULL)
        _lwevent_set(if_ptr->data_event, USB_DATA_CTRL_COMPLETE); /* mark we are not using control pipe */

}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_video_set_ctrl
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *
 *END*--------------------------------------------------------------------*/
USB_STATUS usb_class_video_set_ctrl
(
 UVC_CLASS_INTF_STRUCT_PTR     if_ptr,
 struct uvc_streaming_control *control,
 int                           probe
 )
{
    USB_SETUP           req;
    USB_STATUS          status = -1;
    LWEVENT_STRUCT_PTR  event;
    uint16_t            size;
    uint8_t             _buf[34];

    size = if_ptr->uvcversion >= 0x0110 ? 34 : 26;

    /* Setup the request */
    req.BMREQUESTTYPE      = REQ_TYPE_OUT | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE;
    req.BREQUEST           = SET_CUR;

    *(uint_16*)req.WVALUE  = probe ? HOST_TO_LE_SHORT(VS_PROBE_CONTROL << 8) : HOST_TO_LE_SHORT(VS_COMMIT_CONTROL << 8);
    *(uint_16*)req.WINDEX  = HOST_TO_LE_SHORT(if_ptr->vsintf->bInterfaceNumber); 
    *(uint_16*)req.WLENGTH = HOST_TO_LE_SHORT(size);

    *(uint16_t *)&_buf[0]  = HOST_TO_LE_SHORT(control->bmHint);
    _buf[2]                = control->bFormatIndex;
    _buf[3]                = control->bFrameIndex;
    *(uint32_t *)&_buf[4]  = HOST_TO_LE_LONG(control->dwFrameInterval);
    *(uint16_t *)&_buf[8]  = HOST_TO_LE_SHORT(control->wKeyFrameRate);
    *(uint16_t *)&_buf[10] = HOST_TO_LE_SHORT(control->wPFrameRate);
    *(uint16_t *)&_buf[12] = HOST_TO_LE_SHORT(control->wCompQuality);
    *(uint16_t *)&_buf[14] = HOST_TO_LE_SHORT(control->wCompWindowSize);
    *(uint16_t *)&_buf[16] = HOST_TO_LE_SHORT(control->wDelay);
    *(uint32_t *)&_buf[18] = HOST_TO_LE_LONG(control->dwMaxVideoFrameSize);
    *(uint32_t *)&_buf[22] = HOST_TO_LE_LONG(control->dwMaxPayloadTransferSize);

    if (size == 34) {
        *(uint32_t *)&_buf[26] = HOST_TO_LE_LONG(control->dwClockFrequency);
        _buf[30] = control->bmFramingInfo;
        _buf[31] = control->bPreferedVersion;
        _buf[32] = control->bMinVersion;
        _buf[33] = control->bMaxVersion;
    }

    event = if_ptr->data_event;
    _lwevent_clear(event, USB_DATA_CTRL_COMPLETE);
    USB_lock();
    if (USB_STATUS_TRANSFER_QUEUED == 
       (status = _usb_hostdev_cntrl_request(if_ptr->dev, &req, _buf, usb_class_video_ch9_callback, if_ptr)))
        status = USB_OK;
    else{
        USB_unlock();
        printf("usb_class_video_set_ctrl _usb_hostdev_cntrl_request failed \n");
        return status;
    }
    USB_unlock();
    /* wait for command completion */
    if(status == USB_OK)
        _lwevent_wait_ticks(event, USB_DATA_CTRL_COMPLETE, FALSE, 0);

    if(if_ptr->ch9status != USB_OK){
        printf("usb_class_video_set_ctrl ch9 transfer failed \n");
        return  -1;
    }

    return status;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_class_video_get_ctrl
 * Returned Value : USB_OK if command has been passed on USB.
 * Comments       :
 *
 *END*--------------------------------------------------------------------*/
USB_STATUS usb_class_video_get_ctrl
(
 UVC_CLASS_INTF_STRUCT_PTR     if_ptr,
 struct uvc_streaming_control *control,
 uint8_t                       brequest,
 int                           probe
 )
{
    USB_SETUP           req;
    USB_STATUS          status = -1;
    LWEVENT_STRUCT_PTR  event;
    uint16_t            size;
    uint8_t            _buf[34];

    size = if_ptr->uvcversion >= 0x0110 ? 34 : 26;

    /* Setup the request */
    req.BMREQUESTTYPE      = REQ_TYPE_IN | REQ_TYPE_CLASS | REQ_TYPE_INTERFACE;
    req.BREQUEST           = brequest;

    *(uint_16*)req.WVALUE  = probe ? HOST_TO_LE_SHORT(VS_PROBE_CONTROL << 8) : HOST_TO_LE_SHORT(VS_COMMIT_CONTROL << 8);
    *(uint_16*)req.WINDEX  = HOST_TO_LE_SHORT(if_ptr->vsintf->bInterfaceNumber); 
    *(uint_16*)req.WLENGTH = HOST_TO_LE_SHORT(size);

    event = if_ptr->data_event;
    _lwevent_clear(event, USB_DATA_CTRL_COMPLETE);
    USB_lock();
    if (USB_STATUS_TRANSFER_QUEUED == 
       (status = _usb_hostdev_cntrl_request(if_ptr->dev, &req, _buf, usb_class_video_ch9_callback, if_ptr)))
        status = USB_OK;
    else{
        USB_unlock();
        printf("usb_class_video_get_ctrl _usb_hostdev_cntrl_request failed \n");
        return status;
    }
    USB_unlock();
    /* wait for command completion */
    if(status == USB_OK)
        _lwevent_wait_ticks(event, USB_DATA_CTRL_COMPLETE, FALSE, 0);

    if(if_ptr->ch9status != USB_OK){
        printf("usb_class_video_get_ctrl ch9 transfer failed \n");
        return  -1;
    }

    control->bmHint = SHORT_UNALIGNED_LE_TO_HOST(&_buf[0]);
    control->bFormatIndex =  _buf[2];
    control->bFrameIndex  =  _buf[3];
    control->dwFrameInterval = LONG_UNALIGNED_LE_TO_HOST(&_buf[4]);
    control->wKeyFrameRate   = SHORT_UNALIGNED_LE_TO_HOST(&_buf[8]);
    control->wPFrameRate     = SHORT_UNALIGNED_LE_TO_HOST(&_buf[10]);
    control->wCompQuality    = SHORT_UNALIGNED_LE_TO_HOST(&_buf[12]);
    control->wCompWindowSize = SHORT_UNALIGNED_LE_TO_HOST(&_buf[14]);
    control->wDelay = SHORT_UNALIGNED_LE_TO_HOST(&_buf[16]);
    control->dwMaxVideoFrameSize = LONG_UNALIGNED_LE_TO_HOST(&_buf[18]);
    control->dwMaxPayloadTransferSize = LONG_UNALIGNED_LE_TO_HOST(&_buf[22]);

    if (size == 34) {
        control->dwClockFrequency = LONG_UNALIGNED_LE_TO_HOST(&_buf[26]);
        control->bmFramingInfo    = _buf[30];
        control->bPreferedVersion = _buf[31];
        control->bMinVersion      = _buf[32];
        control->bMaxVersion      = _buf[33];
    } else {
        //control->dwClockFrequency = stream->dev->clock_frequency;
        control->bmFramingInfo = 0;
        control->bPreferedVersion = 0;
        control->bMinVersion = 0;
        control->bMaxVersion = 0;
    }


    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_video_deinit
* Returned Value : None
* Comments       :
*     This function is destructor for device instance called after detach
*
*END*--------------------------------------------------------------------*/

static void usb_class_video_deinit
   (
      /* [IN] data call struct pointer */
      _usb_class_intf_handle  if_handle
   )
{
    UVC_CLASS_INTF_STRUCT_PTR if_ptr = (UVC_CLASS_INTF_STRUCT_PTR)if_handle;
    UVCMSG msg;
    //USB_STATUS error;
    PIPE_BUNDLE_STRUCT pipe_bundle;

    if(if_ptr->data_event != NULL) /* try wakeup write call */
        _lwevent_set(if_ptr->data_event, USB_DATA_DETACH);

    /* free uvc_taskq pending data */
    while(_lwmsgq_receive(if_ptr->uvc_taskq, (_mqx_max_type *) &msg, 0, 0, 0) == MQX_OK){
        _mem_free(msg.dsp);
    }
    memset(&msg, 0, sizeof msg);
    _lwmsgq_send(if_ptr->uvc_taskq, (uint_32 *) &msg, 0);

    /* destroying lwevent is up to application */
} /* Endbody */


static void usb_class_video_stream_callback
   (
      /* [IN] pointer to pipe */
      _usb_pipe_handle  pipe,

      /* [IN] user-defined parameter */
      pointer           param,

      /* [IN] buffer address */
      pointer           buffer,

      /* [IN] length of data transferred */
      uint_32           len,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      uint_32           status
   )
{ /* Body */
    UVC_CLASS_INTF_STRUCT_PTR if_ptr = (UVC_CLASS_INTF_STRUCT_PTR)param;
    uint8_t            *buf = buffer;
    static  int         bufcount = 0;

    if(status == USBERR_DEVICE_NOT_FOUND)
        return;

    if(((status == USB_OK) || (status == USBERR_TR_SHORT)) && (len > 0))
    {
        uint_32 datalen = len - buf[0];
        if(datalen > 0){
            //printf("bHI:%d:%d %s,%s \n", len, buf[0], (buf[1] & bHI_FID)?"S":" ",  (buf[1] & bHI_EOF)?"E":" ");
            //printf("0x%x %d %d \n", buf[1], len,  bufcount++);
            usb_class_video_ringbuffer_write(&if_ptr->ringbuf, 1, datalen, (void *)(buf + buf[0]));
            if(if_ptr->ringbuf.actual >= if_ptr->ringbuf.required){
                if_ptr->ringbuf.required = -1;
                _lwevent_set(if_ptr->data_event, USB_DATA_READ_READY); /* mark we are not using control pipe */
            }
        } /* video has valid data */
    } /* it has data in USB */
} /*EndBody */


static int usb_class_video_enable_video(UVC_CLASS_INTF_STRUCT_PTR if_ptr){

    LWEVENT_STRUCT_PTR  event;
    int ret = -1;

    ret = usb_class_video_set_ctrl(if_ptr, &if_ptr->vs_control, 0);
    if(ret < 0){
        printf("usb_class_video_set_ctrl commit failed \n");
        return ret;
    }

    if (if_ptr->vsaltintf.alts > 1) {
        ENDPOINT_DESCRIPTOR_PTR   ep = NULL, best_ep = NULL;
        PIPE_STRUCT_PTR           pipe_desc;
        unsigned int best_psize = -1;
        unsigned int bandwidth;
        unsigned int altsetting = 0;
        unsigned int psize;
        int i = 0;

        /* Isochronous endpoint, select the alternate setting. */
        bandwidth = if_ptr->vs_control.dwMaxPayloadTransferSize;
        if (bandwidth == 0) {
            printf("Device requested null " "bandwidth, defaulting to lowest.\n");
            bandwidth = 1;
        } else {
            printf("Device requested %u " "B/frame bandwidth.\n", bandwidth);
        }

        for (i = 0; i < if_ptr->vsaltintf.alts; ++i) {

            ep = if_ptr->vsaltintf.ep[i];
            if (ep == NULL)
                continue;

            /* Check if the bandwidth is high enough. */
            psize = SHORT_UNALIGNED_LE_TO_HOST(ep->wMaxPacketSize);
            if (psize >= bandwidth && psize <= best_psize) {
                altsetting = if_ptr->vsaltintf.intfs[i]->bAlternateSetting;
                best_psize = psize;
                best_ep    = ep;
            }
        }

        if (best_ep == NULL) {
            printf("No fast enough alt setting " "for requested bandwidth.\n");
            return -1;
        }
        printf("Selecting alternate setting %u " "(%u B/frame bandwidth).\n", altsetting, best_psize);

        event = if_ptr->data_event;
        _lwevent_clear(event, USB_DATA_CTRL_COMPLETE);
        USB_lock();
        _usb_host_register_ch9_callback((_usb_device_instance_handle)if_ptr->dev, usb_class_video_ch9_callback, if_ptr);
        _usb_host_ch9_set_interface((_usb_device_instance_handle)if_ptr->dev, altsetting, if_ptr->vsintf->bInterfaceNumber);
        USB_unlock();
        /* wait for command completion */
        _lwevent_wait_ticks(event, USB_DATA_CTRL_COMPLETE, FALSE, 0);

        _usb_host_register_ch9_callback((_usb_device_instance_handle)if_ptr->dev, NULL, NULL);

        if_ptr->cur_alt    =  altsetting;
        if_ptr->cur_vsintf =  if_ptr->vsaltintf.intfs[altsetting];


        ret = usb_hostdev_open_pipe((void *)if_ptr->dev,
                                    (void *)if_ptr->cur_vsintf, 
                                    0, 
                                    1, 
                                    (void **)(&pipe_desc));
        if(ret != USB_OK){
            printf("UVC open video stream pipe failed!\n");
        }else{
            TR_INIT_PARAM_STRUCT  tr;
            if_ptr->vspipedesc = pipe_desc;

            usb_hostdev_tr_init(&tr, (tr_callback)usb_class_video_stream_callback, (void *)if_ptr);

            tr.G.RX_BUFFER = (uchar_ptr) if_ptr->streambuf;
            tr.G.RX_LENGTH =  pipe_desc->MAX_PACKET_SIZE;
            tr.G.FLAG      =  TR_PERIOD_KEEP;

            if (USB_STATUS_TRANSFER_QUEUED == (ret = _usb_host_recv_data(
                if_ptr->UVC_G.G.host_handle,
                if_ptr->vspipedesc,
                &tr)))
            {
                ret = USB_OK;
                if_ptr->streamenabled = 1;
            }
        }
    } 

    return ret;
}

static int usb_class_video_probe_video(UVC_CLASS_INTF_STRUCT_PTR if_ptr, struct uvc_streaming_control *probe)
{
        struct uvc_streaming_control probe_min, probe_max;
        uint16_t bandwidth;
        unsigned int i;
        int ret;

        /* Perform probing. The device should adjust the requested values
         * according to its capabilities. However, some devices, namely the
         * first generation UVC Logitech webcams, don't implement the Video
         * Probe control properly, and just return the needed bandwidth. For
         * that reason, if the needed bandwidth exceeds the maximum available
         * bandwidth, try to lower the quality.
         */
        ret = usb_class_video_set_ctrl(if_ptr, probe, 1);
        if (ret < 0)
                goto done;

        /* Get the minimum and maximum values for compression settings. */
        ret = usb_class_video_get_ctrl(if_ptr, &probe_min, GET_MIN, 1);
        if (ret < 0)
            goto done;
        ret = usb_class_video_get_ctrl(if_ptr, &probe_max, GET_MAX, 1);
        if (ret < 0)
            goto done;

        probe->wCompQuality = probe_max.wCompQuality;

        for (i = 0; i < 2; ++i) {
                ret = usb_class_video_set_ctrl(if_ptr, probe, 1);
                if (ret < 0)
                        goto done;
                memset(probe, 0, sizeof(*probe));
                ret = usb_class_video_get_ctrl(if_ptr, probe, GET_CUR, 1);
                if (ret < 0)
                        goto done;

                if (if_ptr->vsaltintf.alts == 1)
                        break;

                bandwidth = probe->dwMaxPayloadTransferSize;
                if (bandwidth <= if_ptr->vsaltintf.maxepsize)
                        break;

                /* TODO: negotiate compression parameters */
                probe->wKeyFrameRate = probe_min.wKeyFrameRate;
                probe->wPFrameRate = probe_min.wPFrameRate;
                probe->wCompQuality = probe_max.wCompQuality;
                probe->wCompWindowSize = probe_min.wCompWindowSize;
        }

done:
        return ret;
}

static uint32_t usb_class_video_try_frame_interval(struct uvc_frame *frame, uint32_t interval)
{
        unsigned int i;

        if (frame->bFrameIntervalType) {
                uint32_t best = -1, dist;

                for (i = 0; i < frame->bFrameIntervalType; ++i) {
                        dist = interval > frame->dwFrameInterval[i]
                             ? interval - frame->dwFrameInterval[i]
                             : frame->dwFrameInterval[i] - interval;

                        if (dist > best)
                                break;

                        best = dist;
                }

                interval = frame->dwFrameInterval[i-1];
        } else {
                const uint32_t min = frame->dwFrameInterval[0];
                const uint32_t max = frame->dwFrameInterval[1];
                const uint32_t step = frame->dwFrameInterval[2];

                interval = min + (interval - min + step/2) / step * step;
                if (interval > max)
                        interval = max;
        }

        return interval;
}



static int usb_class_video_try_format(UVC_CLASS_INTF_STRUCT_PTR if_ptr,
        struct video_pix_format *fmt,        struct uvc_streaming_control *probe,
        struct uvc_format      **uvc_format, struct uvc_frame            **uvc_frame)
{
        struct uvc_format *format = NULL;
        struct uvc_frame   *frame = NULL;
        uint16_t rw, rh;
        unsigned int d, maxd;
        unsigned int i;
        uint32_t interval;
        int ret = 0;
        uint8_t *fcc;


        fcc = (uint8_t *)&fmt->pixelformat;
        printf("Trying format 0x%08x (%c%c%c%c): %ux%u.\n",
                        fmt->pixelformat,
                        fcc[0], fcc[1], fcc[2], fcc[3],
                        fmt->width, fmt->height);

        /* Check if the hardware supports the requested format, use the default
         * format otherwise.
         */
        for (i = 0; i < if_ptr->vsformatcount; ++i) {
                format = &if_ptr->vs_format[i];
                if (format->fcc == fmt->pixelformat)
                        break;
        }

        if (i ==  if_ptr->vsformatcount) {
                format = if_ptr->def_format;
                fmt->pixelformat = format->fcc;
        }

        /* Find the closest image size. The distance between image sizes is
         * the size in pixels of the non-overlapping regions between the
         * requested size and the frame-specified size.
         */
        rw = fmt->width;
        rh = fmt->height;
        maxd = (unsigned int)-1;

        for (i = 0; i < format->nframes; ++i) {
                uint16_t w = format->frame[i].wWidth;
                uint16_t h = format->frame[i].wHeight;

                d = min(w, rw) * min(h, rh);
                d = w*h + rw*rh - 2*d;
                if (d < maxd) {
                        maxd = d;
                        frame = &format->frame[i];
                }

                if (maxd == 0)
                        break;
        }

        if (frame == NULL) {
                printf("Unsupported size %ux%u.\n", fmt->width, fmt->height);
                return -1;
        }

        if(!fmt->interval)
            interval =  frame->dwDefaultFrameInterval;
        else
            interval = fmt->interval;
        printf("Using frame interval %u.%u us "
                "(%u.%u fps).\n", interval/10, interval%10, 10000000/interval,
                (100000000/interval)%10);

        /* Set the format index, frame index and frame interval. */
        memset(probe, 0, sizeof *probe);
        probe->bmHint = 1;      /* dwFrameInterval */
        probe->bFormatIndex = format->index;
        probe->bFrameIndex  = frame->bFrameIndex;
        probe->dwFrameInterval = usb_class_video_try_frame_interval(frame, interval);
        /* Some webcams stall the probe control set request when the
         * dwMaxVideoFrameSize field is set to zero. The UVC specification
         * clearly states that the field is read-only from the host, so this
         * is a webcam bug. Set dwMaxVideoFrameSize to the value reported by
         * the webcam to work around the problem.
         *
         * The workaround could probably be enabled for all webcams, so the
         * quirk can be removed if needed. It's currently useful to detect
         * webcam bugs and fix them before they hit the market (providing
         * developers test their webcams with the Linux driver as well as with
         * the Windows driver).
         */

        //probe->dwMaxVideoFrameSize = if_ptr->vs_control.dwMaxVideoFrameSize;
        /* Probe the device. */
        ret = usb_class_video_probe_video(if_ptr, probe);
        if (ret < 0)
                goto done;

        fmt->width            = frame->wWidth;
        fmt->height           = frame->wHeight;
        fmt->bytesperline     = format->bpp * frame->wWidth / 8;
        fmt->sizeimage        = probe->dwMaxVideoFrameSize;
        fmt->colorspace       = format->colorspace;
        fmt->priv             = 0;
        fmt->interval         = probe->dwFrameInterval;
        fmt->maxpayloadsize   = probe->dwMaxPayloadTransferSize;

        if (uvc_format != NULL)
                *uvc_format = format;
        if (uvc_frame != NULL)
                *uvc_frame = frame;

done:
        return ret;
}


static struct uvc_format_desc *usb_class_video_format_by_guid(const uint8_t guid[16])
{
        unsigned int len = sizeof (uvc_fmts) / sizeof (uvc_fmts[0]);
        unsigned int i;

        for (i = 0; i < len; ++i) {
                if (memcmp(guid, uvc_fmts[i].guid, 16) == 0)
                        return &uvc_fmts[i];
        }

        return NULL;
}

static int usb_class_video_set_format(UVC_CLASS_INTF_STRUCT_PTR if_ptr, struct video_pix_format *fmt)
{
	struct uvc_streaming_control probe;
	struct uvc_format *format;
	struct uvc_frame *frame;
        int    ret;

	ret = usb_class_video_try_format(if_ptr, fmt, &probe, &format, &frame);
	if (ret < 0)
		return ret;

	if_ptr->vs_control = probe;
	if_ptr->cur_format = format;
	if_ptr->cur_frame  = frame;

        printf("\nProbed uvc control parameter \n");
        usb_class_video_printf_ctrl(&if_ptr->vs_control);
done:
	return ret;
}

static int usb_class_video_parse_format(UVC_CLASS_INTF_STRUCT_PTR if_ptr, unsigned char *buffer, int buflen)
{
        struct uvc_format_desc *fmtdesc;
        struct uvc_frame       *frame;
        struct uvc_format      *format;
        const unsigned char *start = buffer;
        unsigned int width_multiplier = 1;
        unsigned int interval;
        unsigned int i, n;
        uint8_t ftype;


        format        = &if_ptr->vs_format[if_ptr->vsformatcount];
        format->frame = &if_ptr->vs_frame[if_ptr->vsframecount];

        format->type  = buffer[2];
        format->index = buffer[3];

        switch (buffer[2]) {
        case VS_FORMAT_UNCOMPRESSED:
        case VS_FORMAT_FRAME_BASED:
                n = buffer[2] == VS_FORMAT_UNCOMPRESSED ? 27 : 28;
                if (buflen < n) {
                        printf("videostreaming " "FORMAT error\n");
                        return -1;
                }

                /* Find the format descriptor from its GUID. */
                fmtdesc = usb_class_video_format_by_guid(&buffer[5]);

                if (fmtdesc != NULL) {
                        strncpy(format->name, fmtdesc->name, sizeof format->name);
                        format->fcc = fmtdesc->fcc;
                } else {
                        printf("Unknown video format %pUl\n", &buffer[5]);
                        snprintf(format->name, sizeof(format->name), "%pUl\n", &buffer[5]);
                        format->fcc = 0;
                }

                format->bpp = buffer[21];

                if (buffer[2] == VS_FORMAT_UNCOMPRESSED) {
                        ftype = VS_FRAME_UNCOMPRESSED;
                } else {
                        ftype = VS_FRAME_FRAME_BASED;
                        if (buffer[27])
                                format->flags = UVC_FMT_FLAG_COMPRESSED;
                }
                break;

        case VS_FORMAT_MJPEG:
                if (buflen < 11) {
                        printf("videostreaming " "FORMAT error\n");
                        return -1;
                }

                strncpy(format->name, "MJPEG", sizeof format->name);
                format->fcc = UVC_PIX_FMT_MJPEG;
                format->flags = UVC_FMT_FLAG_COMPRESSED;
                if(buffer[5] & 0x1){
                    format->flags |= UVC_FMT_FLAG_FIXEDSIZE;
                    printf("MJPEG with fixed frame size! \n");
                }
                format->bpp = 0;
                ftype = VS_FRAME_MJPEG;
                break;

        case VS_FORMAT_DV:
                if (buflen < 9) {
                        printf("videostreaming " "FORMAT error\n");
                        return -1;
                }

                switch (buffer[8] & 0x7f) {
                case 0:
                        strncpy(format->name, "SD-DV", sizeof format->name);
                        break;
                case 1:
                        strncpy(format->name, "SDL-DV", sizeof format->name);
                        break;
                case 2:
                        strncpy(format->name, "HD-DV", sizeof format->name);
                        break;
                default:
                        printf("videostreaming " "FORMAT error\n");
                        return -1;
                }

                strncat(format->name, buffer[8] & (1 << 7) ? " 60Hz" : " 50Hz",
                        sizeof format->name);

                format->fcc = UVC_PIX_FMT_DV;
                format->flags = UVC_FMT_FLAG_COMPRESSED | UVC_FMT_FLAG_STREAM;
                format->bpp = 0;
                ftype = 0;

                /* Create a dummy frame descriptor. */
                frame = &format->frame[0];
                memset(&format->frame[0], 0, sizeof format->frame[0]);
                frame->bFrameIntervalType = 1;
                frame->dwDefaultFrameInterval = 1;
                frame->dwFrameInterval  = &if_ptr->uvc_intervals[if_ptr->vsintervalcount];
               *frame->dwFrameInterval  = 1;
                if_ptr->vsintervalcount++;
                format->nframes = 1;
                break;

        case VS_FORMAT_MPEG2TS:
        case VS_FORMAT_STREAM_BASED:
                /* Not supported yet. */
        default:
                printf("videostreaming " "FORMAT error\n");
                return -1;
        }

        printf("Found format %s.\n", format->name);

        buflen -= buffer[0];
        buffer += buffer[0];

        /* Parse the frame descriptors. Only uncompressed, MJPEG and frame
         * based formats have frame descriptors.
         */
        while (buflen > 2 && buffer[1] == CS_INTERFACE &&
               buffer[2] == ftype) {
                frame = &format->frame[format->nframes];
                if (ftype != VS_FRAME_FRAME_BASED)
                        n = buflen > 25 ? buffer[25] : 0;
                else
                        n = buflen > 21 ? buffer[21] : 0;

                n = n ? n : 3;

                if (buflen < 26 + 4*n) {
                    printf("videostreaming " "FORMAT error\n");
                    return -1;
                }

                frame->bFrameIndex    = buffer[3];
                frame->bmCapabilities = buffer[4];
                frame->wWidth         = SHORT_UNALIGNED_LE_TO_HOST(&buffer[5]) * width_multiplier;
                frame->wHeight        = SHORT_UNALIGNED_LE_TO_HOST(&buffer[7]);
                frame->dwMinBitRate   = LONG_UNALIGNED_LE_TO_HOST(&buffer[9]);
                frame->dwMaxBitRate   = LONG_UNALIGNED_LE_TO_HOST(&buffer[13]);
                if (ftype != VS_FRAME_FRAME_BASED) {
                        frame->dwMaxVideoFrameBufferSize = LONG_UNALIGNED_LE_TO_HOST(&buffer[17]);
                        frame->dwDefaultFrameInterval    = LONG_UNALIGNED_LE_TO_HOST(&buffer[21]);
                        frame->bFrameIntervalType        = buffer[25];
                } else {
                        frame->dwMaxVideoFrameBufferSize = 0;
                        frame->dwDefaultFrameInterval    = LONG_UNALIGNED_LE_TO_HOST(&buffer[17]);
                        frame->bFrameIntervalType         = buffer[21];
                }
                frame->dwFrameInterval = &if_ptr->uvc_intervals[if_ptr->vsintervalcount];

                /* Several UVC chipsets screw up dwMaxVideoFrameBufferSize
                 * completely. Observed behaviours range from setting the
                 * value to 1.1x the actual frame size to hardwiring the
                 * 16 low bits to 0. This results in a higher than necessary
                 * memory usage as well as a wrong image size information. For
                 * uncompressed formats this can be fixed by computing the
                 * value from the frame size.
                 */
                if (!(format->flags & UVC_FMT_FLAG_COMPRESSED))
                        frame->dwMaxVideoFrameBufferSize = format->bpp * frame->wWidth * frame->wHeight / 8;

                /* Some bogus devices report dwMinFrameInterval equal to
                 * dwMaxFrameInterval and have dwFrameIntervalStep set to
                 * zero. Setting all null intervals to 1 fixes the problem and
                 * some other divisions by zero that could happen.
                 */
                for (i = 0; i < n; ++i) {
                        interval = LONG_UNALIGNED_LE_TO_HOST(&buffer[26+4*i]);
                        if_ptr->uvc_intervals[if_ptr->vsintervalcount++] = interval ? interval : 1;
                }

                /* Make sure that the default frame interval stays between
                 * the boundaries.
                 */
                n -= frame->bFrameIntervalType ? 1 : 2;
                frame->dwDefaultFrameInterval =
                        min(frame->dwFrameInterval[n],
                            max(frame->dwFrameInterval[0],
                                frame->dwDefaultFrameInterval));


                printf("- %ux%u (%u.%u fps)\n",
                        frame->wWidth, frame->wHeight,
                        10000000/frame->dwDefaultFrameInterval,
                        (100000000/frame->dwDefaultFrameInterval)%10);

                format->nframes++;
                buflen -= buffer[0];
                buffer += buffer[0];
        }

        if (buflen > 2 && buffer[1] == CS_INTERFACE &&
            buffer[2] == VS_STILL_IMAGE_FRAME) {
                buflen -= buffer[0];
                buffer += buffer[0];
        }

        if (buflen > 2 && buffer[1] == CS_INTERFACE &&
            buffer[2] == VS_COLORFORMAT) {
                if (buflen < 6) {
                    printf("videostreaming " "FORMAT error\n");
                    return -1;
                }

                //format->colorspace = uvc_colorspace(buffer[3]);

                buflen -= buffer[0];
                buffer += buffer[0];
        }

        return buffer - start;
}
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_uvc_init
* Returned Value : None
* Comments       :
*     This function is called by common class to initialize the class driver
*     for AbstractClassControl. It is called in response to a select
*     interface call by application.
*
*END*--------------------------------------------------------------------*/

void usb_class_video_init
   (
      /* [IN]  structure with USB pipe information on the interface */
      PIPE_BUNDLE_STRUCT_PTR      pbs_ptr,

      /* [IN] acm call struct pointer */
      CLASS_CALL_STRUCT_PTR       ccs_ptr
   )
{ /* Body */
    UVC_CLASS_INTF_STRUCT_PTR if_ptr = (UVC_CLASS_INTF_STRUCT_PTR)ccs_ptr->class_intf_handle;
    USB_STATUS                     status;
    int                            ret;

#ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_class_uvc_init");
#endif

    INTERFACE_DESCRIPTOR_PTR intf = (INTERFACE_DESCRIPTOR_PTR)(pbs_ptr->intf_handle);
    ENDPOINT_DESCRIPTOR_PTR  endp =  NULL;

    /* Make sure the device is still attached */
    //USB_lock();  //not needed as init is called from "already locked" function
    status = usb_host_class_intf_init(pbs_ptr, if_ptr, &data_anchor_abstract, usb_class_video_deinit);
    if(status == USB_OK){
        /*
         ** We generate a code_key based on the attached device. This is used to
         ** verify that the device has not been detached and replaced with another.
         */
        ccs_ptr->code_key = 0;
        ccs_ptr->code_key = usb_host_class_intf_validate(ccs_ptr);

        //if_ptr->CDC_G.IFNUM = ((INTERFACE_DESCRIPTOR_PTR)if_ptr->CDC_G.G.intf_handle)->bInterfaceNumber;
        if_ptr->cmd_pipe       = usb_hostdev_get_pipe_handle(pbs_ptr, USB_CONTROL_PIPE,   USB_SEND);
        if_ptr->interrupt_pipe = usb_hostdev_get_pipe_handle(pbs_ptr, USB_INTERRUPT_PIPE, USB_RECV);
        if_ptr->pbs_ptr        = pbs_ptr;

        if ((if_ptr->cmd_pipe       == NULL) || 
                (if_ptr->interrupt_pipe == NULL)) {
            ccs_ptr->code_key = 0;
            USB_unlock();
            status = USBERR_OPEN_PIPE_FAILED;
            DEBUG_LOG_TRACE("usb_class_uvc_init, FAILED");
            return;
        }

        if_ptr->dev           = (DEV_INSTANCE_PTR)(pbs_ptr->dev_handle);
        if_ptr->streamenabled = 0;
        if_ptr->vcintf        = intf;

        /* Class-Specific VC Interface Descriptor */
        _usb_hostdev_get_descriptor(if_ptr->dev,
                intf,
                USB_DESC_TYPE_CS_INTERFACE,  /* Functional descriptor for interface */
                1,                          
                VC_HEADER,                   
                (pointer _PTR_)&if_ptr->vc_interface_header_ptr);

        if_ptr->uvcversion = SHORT_UNALIGNED_LE_TO_HOST(&if_ptr->vc_interface_header_ptr->bcdUVC);
        printf("UVC version: %x \n", if_ptr->uvcversion);

        /* Input Terminal Descriptor */
        _usb_hostdev_get_descriptor(if_ptr->dev,
                intf,
                USB_DESC_TYPE_CS_INTERFACE,  /* Functional descriptor for interface */
                1,                          
                VC_INPUT_TERMINAL,                   
                (pointer _PTR_)&if_ptr->vc_input_terminal_ptr);

        /* Output Terminal Descriptor */
        _usb_hostdev_get_descriptor(if_ptr->dev,  
                intf,
                USB_DESC_TYPE_CS_INTERFACE,  /* Functional descriptor for interface */
                1,                          
                VC_OUTPUT_TERMINAL,                   
                (pointer _PTR_)&if_ptr->vc_output_terminal_ptr);

        /* Processoring Unit Descriptor */
        _usb_hostdev_get_descriptor(if_ptr->dev,  
                intf,
                USB_DESC_TYPE_CS_INTERFACE,  /* Functional descriptor for interface */
                1,                          
                VC_PROCESSING_UNIT,                   
                (pointer _PTR_)&if_ptr->vc_processing_unit_ptr);


        /* Check the first streaming interface */
        if(if_ptr->vc_interface_header_ptr->bInCollection >= 1){
            unsigned char *_buf;
            int _len;


            /* Get the stream interface */
            if_ptr->vsintf = NULL;
            _usb_hostdev_get_descriptor(if_ptr->dev,  
                    NULL,
                    USB_DESC_TYPE_IF,
                    if_ptr->vc_interface_header_ptr->baInterfaceNr1,                          
                    0,
                    (pointer _PTR_)&if_ptr->vsintf);

            if(if_ptr->vsintf == NULL){
                printf("Can't find stream interface! \n");
                return;
            }

            intf = if_ptr->vsintf;

            if_ptr->vs_input_header_ptr = NULL;
            /* Class-Specific VS Interface Descriptor */
            _usb_hostdev_get_descriptor(if_ptr->dev,  
                    intf,
                    USB_DESC_TYPE_CS_INTERFACE,  /* Functional descriptor for interface */
                    1,                          
                    VS_INPUT_HEADER,                   
                    (pointer _PTR_)&if_ptr->vs_input_header_ptr);

            if( if_ptr->vs_input_header_ptr == NULL){
                printf("Can't find vs_intput_header! \n");
                return;
            }


            if_ptr->vsaltintf.alts      = 0;
            if_ptr->vsaltintf.maxepsize = 0;
            
            do{
                if_ptr->vsaltintf.intfs[if_ptr->vsaltintf.alts] = NULL;
                if_ptr->vsaltintf.ep[if_ptr->vsaltintf.alts]    = NULL;

                _usb_hostdev_get_descriptor(if_ptr->dev,  
                        NULL,
                        USB_DESC_TYPE_IF,
                        if_ptr->vc_interface_header_ptr->baInterfaceNr1,                          
                        if_ptr->vsaltintf.alts,
                        (pointer _PTR_)&if_ptr->vsaltintf.intfs[if_ptr->vsaltintf.alts]);

                if(if_ptr->vsaltintf.intfs[if_ptr->vsaltintf.alts] == NULL)
                    break;

                _usb_hostdev_get_descriptor(if_ptr->dev,  
                        if_ptr->vsaltintf.intfs[if_ptr->vsaltintf.alts],
                        USB_DESC_TYPE_EP,
                        1,
                        0,
                        (pointer _PTR_)&if_ptr->vsaltintf.ep[if_ptr->vsaltintf.alts]);

                if(if_ptr->vsaltintf.ep[if_ptr->vsaltintf.alts] != NULL){
                    uint16_t maxepsize = 
                    SHORT_UNALIGNED_LE_TO_HOST(if_ptr->vsaltintf.ep[if_ptr->vsaltintf.alts]->wMaxPacketSize);

                    if(maxepsize > if_ptr->vsaltintf.maxepsize){
                        if_ptr->vsaltintf.maxepsize = maxepsize;
                    }
                }

                if_ptr->vsaltintf.alts++;
            }while(1);


            if_ptr->vsformatcount = 0;
            if_ptr->vsframecount  = 0;

            _buf = (unsigned char *)(if_ptr->vs_input_header_ptr) + if_ptr->vs_input_header_ptr->bLength;
            _len = SHORT_UNALIGNED_LE_TO_HOST(&if_ptr->vs_input_header_ptr->wTotalLength) - if_ptr->vs_input_header_ptr->bLength;

            while (_len > 2 && _buf[1] == CS_INTERFACE) {
                switch (_buf[2]) {
                    case VS_FORMAT_UNCOMPRESSED:
                    case VS_FORMAT_MJPEG:
                    case VS_FORMAT_FRAME_BASED:
                    case VS_FORMAT_DV:

                        ret = usb_class_video_parse_format(if_ptr, _buf, _len);

                        if_ptr->vsframecount += if_ptr->vs_format[if_ptr->vsformatcount].nframes;
                        if_ptr->vsformatcount++;

                        _len -= ret;
                        _buf += ret;

                        break;

                    case VS_FORMAT_MPEG2TS:
                    case VS_FORMAT_STREAM_BASED:
                        printf( "UVC videostreaming " "FORMAT %u is not supported.\n", _buf[2]);
                        break;

                default:
                        break;
                }

                if((if_ptr->vsformatcount    >= VS_MAX_FORMAT)      || 
                   (if_ptr->vsframecount     >= VS_MAX_FRAME)       ||
                   (if_ptr->vsintervalcount  >= VS_MAX_INTERVALS)   
                   )
                    break;
            } /* Get format/frame desc */

        } /* Has stream */

    } /* Init control interface OK */

    if_ptr->def_format = &if_ptr->vs_format[0];

    /* Signal that an error has occured by setting the "code_key" */
    if (status) {
        ccs_ptr->code_key = 0;
    } /* Endif */

    if_ptr->APP = ccs_ptr;

    //USB_unlock();

#ifdef _HOST_DEBUG_
    if (status) {
        DEBUG_LOG_TRACE("usb_class_uvc_init, FAILED");
    }
    else {
        DEBUG_LOG_TRACE("usb_class_uvc_init, Sucessfully");
    }
#endif

} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_video_install_driver
* Returned Value : Success as USB_OK
* Comments       :
*     Adds / installs USB video device driver to the MQX drivers
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_video_install_driver
    (
        CLASS_CALL_STRUCT_PTR    data_instance,
        char_ptr                 device_name
    )
{
    UVC_CLASS_INTF_STRUCT_PTR  if_ptr;
    USB_STATUS      status = USBERR_NO_INTERFACE;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_class_video_install_driver");
    #endif
    
    USB_lock();
    /* Validity checking, always needed when passing data to lower API */
    if (usb_host_class_intf_validate(data_instance)) {
        if_ptr = (UVC_CLASS_INTF_STRUCT_PTR) data_instance->class_intf_handle;
    
        status = _io_dev_install_ext(device_name, 
            _io_video_open,
            _io_video_close,
            _io_video_read,
            _io_video_write,
            _io_video_ioctl,
            _io_video_uninstall,
            data_instance); /* pass a parameter to the driver to associate this driver with data instance */
        if (status == IO_OK)
            if_ptr->device_name = device_name;
    }
    
    USB_unlock();

    #ifdef _HOST_DEBUG_
    if (!status) {
        DEBUG_LOG_TRACE("usb_class_video_install_driver, SUCCESSFUL");
    }
    else {
        DEBUG_LOG_TRACE("usb_class_video_install_driver, FAILED");
    }
    #endif

    return status;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_class_video_uninstall_driver
* Returned Value : Success as USB_OK
* Comments       :
*     Removes USB video device driver from the MQX drivers
*END*--------------------------------------------------------------------*/

USB_STATUS usb_class_video_uninstall_driver
    (
        CLASS_CALL_STRUCT_PTR data_instance
    )
{
    UVC_CLASS_INTF_STRUCT_PTR  if_ptr;
    USB_STATUS      status = USBERR_NO_INTERFACE;

    #ifdef _HOST_DEBUG_
        DEBUG_LOG_TRACE("usb_class_video_uninstall_driver");
    #endif

    USB_lock();
    /* Validity checking, always needed when passing data to lower API */
    if (usb_host_class_intf_validate(data_instance)) {
        if_ptr = (UVC_CLASS_INTF_STRUCT_PTR) data_instance->class_intf_handle;
    
        status = _io_dev_uninstall(if_ptr->device_name);
    }

    USB_unlock();

    #ifdef _HOST_DEBUG_
    if (!status) {
        DEBUG_LOG_TRACE("usb_class_video_uninstall_driver, SUCCESSFUL");
    }
    else {
        DEBUG_LOG_TRACE("usb_class_video_uninstall_driver, FAILED");
    }
    #endif

    return status;
}

/*FUNCTION****************************************************************
*
* Function Name    : _io_video_open
* Returned Value   : _mqx_int error code
* Comments         :
*    This routine initializes a USB video I/O channel. It acquires
*    memory, then stores information into it about the channel.
*    It then calls the hardware interface function to initialize the channel.
*
*END**********************************************************************/

static _mqx_int _io_video_open
   (
      /* [IN] the file handle for the device being opened */
      MQX_FILE_PTR fd_ptr,

      /* [IN] the remaining portion of the name of the device */
      char       _PTR_ open_name_ptr,

      /* [IN] the flags to be used during operation:
      ** echo, translation, xon/xoff, encoded into a pointer.
      */
      char       _PTR_ flags
   )
{ /* Body */

    int i;
    USB_STATUS               error;
    LWEVENT_STRUCT_PTR       event;

    CLASS_CALL_STRUCT_PTR   video_instance  = fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    UVC_CLASS_INTF_STRUCT_PTR  if_ptr = (UVC_CLASS_INTF_STRUCT_PTR) video_instance->class_intf_handle;

    /* disable video stream interface */
    event = if_ptr->data_event;
    _lwevent_clear(event, USB_DATA_CTRL_COMPLETE);
    USB_lock();
    _usb_host_register_ch9_callback((_usb_device_instance_handle)if_ptr->dev, usb_class_video_ch9_callback, if_ptr);
    _usb_host_ch9_set_interface((_usb_device_instance_handle)if_ptr->dev, 0, if_ptr->vsintf->bInterfaceNumber);
    USB_unlock();
    /* wait for command completion */
    _lwevent_wait_ticks(event, USB_DATA_CTRL_COMPLETE, FALSE, 0);
    _usb_host_register_ch9_callback((_usb_device_instance_handle)if_ptr->dev, NULL, NULL);

    if(usb_class_video_get_ctrl(if_ptr, &if_ptr->vs_control, GET_DEF, 1) == USB_OK){
        usb_class_video_set_ctrl(if_ptr, &if_ptr->vs_control, 1);
    }

    memset(&if_ptr->vs_control, 0, sizeof(if_ptr->vs_control));
    if(usb_class_video_get_ctrl(if_ptr, &if_ptr->vs_control, GET_CUR, 1) != USB_OK){
        printf("uvc GET CUR  failed \n");
        return IO_ERROR;
    }else{
        printf("\nDefault uvc control parameter \n");
        usb_class_video_printf_ctrl(&if_ptr->vs_control);
    }

    if_ptr->ringbuf.buf          = NULL;
    if_ptr->ringbuf.buf          = _mem_alloc_system(UVC_RING_BUF_SIZE);
    if(if_ptr->ringbuf.buf == NULL){
        printf("UVC open alloc ring buf failed \n");
        return -1;
    }
    if_ptr->ringbuf.len          = UVC_RING_BUF_SIZE;
    if_ptr->ringbuf.wp           = 0;
    if_ptr->ringbuf.rp           = 0;
    if_ptr->ringbuf.actual       = 0;   /* explicit set actual to zero */
    if_ptr->ringbuf.required     = -1;   

    return IO_OK;
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_video_close
* Returned Value   : _mqx_int error code
* Comments         :
*    This routine closes the USB video I/O channel.
*
*END**********************************************************************/

static _mqx_int _io_video_close
   (
      /* [IN] the file handle for the device being closed */
      MQX_FILE_PTR fd_ptr
   )
{ /* Body */
    
    CLASS_CALL_STRUCT_PTR   video_instance  = fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    UVC_CLASS_INTF_STRUCT_PTR  if_ptr = (UVC_CLASS_INTF_STRUCT_PTR) video_instance->class_intf_handle;

    return IO_OK;
}

/*FUNCTION****************************************************************
*
* Function Name    : _io_video_read
* Returned Value   : _mqx_int number of characters read
* Comments         :
*    This routine reads data from the USB video I/O channel device
*
*END*********************************************************************/

static _mqx_int _io_video_read
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] where the characters are to be stored */
      char_ptr   data_ptr,

      /* [IN] the number of characters to input */
      _mqx_int   num
   )
{ /* Body */
    CLASS_CALL_STRUCT_PTR          video_instance;
    UVC_CLASS_INTF_STRUCT_PTR      if_ptr;
    UVCMSG                         msg;
    UVCDATABUFP                    curSP;
    int32_t                        ret = -1;
    
    UVCPACKET_PTR uvcpacketP = (UVCPACKET_PTR)data_ptr;
    LWEVENT_STRUCT_PTR  event;

    if (0 ==  num)
       return 0;
    
    video_instance = (CLASS_CALL_STRUCT_PTR) fd_ptr->DEV_PTR->DRIVER_INIT_PTR;

    USB_lock();

    /* Validity checking, always needed when passing data to lower API */
    if (!usb_host_class_intf_validate(video_instance)) {
        USB_unlock();
        fd_ptr->ERROR = USBERR_NO_INTERFACE;
        return IO_ERROR;
    }

    if_ptr = (UVC_CLASS_INTF_STRUCT_PTR)video_instance->class_intf_handle;

    if(if_ptr->ringbuf.actual < num){
        event = if_ptr->data_event;
        if_ptr->ringbuf.required = num;
        _lwevent_clear(event, USB_DATA_READ_READY);
        USB_unlock();
        _lwevent_wait_ticks(event, USB_DATA_READ_READY, FALSE, 0);
        USB_lock();
    }
    ret = usb_class_video_ringbuffer_read(&if_ptr->ringbuf, 1, num, uvcpacketP->data);
    USB_unlock();
    uvcpacketP->usb_help.type = UVC_PKT_DATA;

    return ret;
} /* Endbody */


/*FUNCTION****************************************************************
*
* Function Name    : _io_video_write
* Returned Value   : _mqx_int number of characters written
* Comments         :
*    This routine writes data to the USB video I/O channel.
*
*END**********************************************************************/

static _mqx_int _io_video_write
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR fd_ptr,

      /* [IN] where the characters are */
      char_ptr  data_ptr,

      /* [IN] the number of characters to output */
      _mqx_int  num
   )
{ /* Body */
    CLASS_CALL_STRUCT_PTR          data_instance;
    UVC_CLASS_INTF_STRUCT_PTR      if_ptr;
    TR_INIT_PARAM_STRUCT           tr;
    USB_STATUS                     status = USBERR_NO_INTERFACE;
    uint_32                        num_left;
    LWEVENT_STRUCT_PTR             event;
    PIPE_STRUCT_PTR                pipe         = NULL;
    char_ptr                       _data_ptr    = NULL;
    
    if (0 == (num_left = num))
       return 0;

    if(!data_ptr)
        return 0;

    return num;

} /* Endbody */


/*FUNCTION*****************************************************************
*
* Function Name    : _io_video_ioctl
* Returned Value   : _mqx_int
* Comments         :
*    Returns result of ioctl operation.
*
*END*********************************************************************/

static _mqx_int _io_video_ioctl
   (
      /* [IN] the file handle for the device */
      MQX_FILE_PTR   fd_ptr,

      /* [IN] the ioctl command */
      _mqx_uint      cmd,

      /* [IN] the ioctl parameters */
      pointer        param_ptr
   )
{ /* Body */
    
    CLASS_CALL_STRUCT_PTR         video_instance  = fd_ptr->DEV_PTR->DRIVER_INIT_PTR;
    UVC_CLASS_INTF_STRUCT_PTR     if_ptr;
    USB_STATUS                    error;

    USB_lock();
    /* Validity checking, always needed when passing data to lower API */
    if (!usb_host_class_intf_validate(video_instance)) {
        USB_unlock();
        fd_ptr->ERROR = USBERR_NO_INTERFACE;
        return IO_ERROR;
    }

    if_ptr = (UVC_CLASS_INTF_STRUCT_PTR) video_instance->class_intf_handle;

    USB_unlock();

    switch(cmd){
        case IO_IOCTL_UVC_VIDEO_SET_FORMAT:
            error = usb_class_video_set_format(if_ptr, (void *)param_ptr);
            break;
        case IO_IOCTL_UVC_ENABLE_VIDEO:
            error = usb_class_video_enable_video(if_ptr);
            break;
    }

    return error;

} /* Endbody */

/*FUNCTION*****************************************************************
*
* Function Name    : _io_video_uninstall
* Returned Value   : _mqx_int
* Comments         :
*    Does nothing. Must be implemented to allow device to be uninstalled.
*
*END*********************************************************************/

static _mqx_int _io_video_uninstall
   (
       IO_DEVICE_STRUCT_PTR dev
   )
{ /* Body */
    return IO_OK;
} /* Endbody */

/* EOF */

