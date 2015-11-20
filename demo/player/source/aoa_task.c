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
* $FileName: audio_host.c$
* $Version : 3.8.38.0$
* $Date    : Oct-4-2012$
*
* Comments:
*
*   This file contains device driver for accesory audio host class. 
*
*END************************************************************************/

/**************************************************************************
Include the OS and BSP dependent files that define IO functions and
basic types. You may like to change these files for your board and RTOS 
**************************************************************************/
/**************************************************************************
Include the USB stack header files.
**************************************************************************/
#include <mqx.h>
#include <usb.h>

#include <host_main.h>
#include <host_shut.h>

#include "usb_types.h"

#include "hostapi.h"


/**************************************************************************
Local header files for this application
**************************************************************************/
#include "aoa_task.h"

#include "msi.h"

static bool snd_init_flag = FALSE;
/**************************************************************************
Class driver files for this application
**************************************************************************/
//#include <usb_host_hub_sm.h>
//#include <usb_otg_main.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif

#undef AUDIO_OUTPUT_DEBUG

//usb_host_info_t g_host_info ;  // test0306
static bool tried_accessory = FALSE;

#undef pr_debug
#define pr_debug printf

/**************************************************************************
   Global variables
**************************************************************************/
#ifdef USE_AUDIO_CONTROL
/* Input Terminal types */
static char * it_type_string[NUMBER_OF_IT_TYPE] =
{
    "Input Undefined",
    "Microphone",
    "Desktop Microphone",
    "Personal Microphone",
    "Omni directional Microphone",
    "Microphone array",
    "Processing Microphone array",
};

/* Output Terminal types */
static char * ot_type_string[NUMBER_OF_OT_TYPE] = 
{
    "Output Undefined",
    "Speaker",
    "Headphones",
    "Head Mounted Display Audio",
    "Desktop Speaker",
    "Room Speaker",
    "Communication Speaker",
    "Low Frequency Effects Speaker",
};
#endif

/* Transfer Types */
static const char * TransferType[TRANSFER_TYPE_NUM] = 
{
    "Control",
    "Isochronous",
    "Bulk",
    "Interrupt"
};
/* Sync Types */
static const char * SyncType[SYNC_TYPE_NUM] = 
{
    "No synchronization",
    "Asynchronous",
    "Adaptive",
    "Synchrounous"
};
/* Data Types */
static const char * DataType[DATA_TYPE_NUM] = 
{
    "Data endpoint",
    "Feedback endpoint",
    "Implicit feedback",
    "Reserved"
};

void default_callback(
          /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint32_t                          event_code)
{
	printf("interface match failed, and call APP default callback\n");
	return ;
}

/* local function declare */
static void usb_host_accessory_event
   (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint32_t                          event_code   );


void default_callback();

/* Tis table must be added the last of full tables */
/* Table of driver capabilities this application wants to use */
static  USB_HOST_DRIVER_INFO DriverInfoTable[] =
{
#ifdef USE_USB_AUDIO	
#ifdef USE_AUDIO_CONTROL
    {
        {0xd1,0x18}/*{0x00,0x00}*/,     /* Vendor ID per USB-IF             */
        {0xFF,0xFF},                            /* Product ID per manufacturer      */
        USB_CLASS_AUDIO,                    /* Class code                       */
        USB_SUBCLASS_AUD_CONTROL,   /* Sub-Class code                   */
        0xFF /*0x00*/,                           /* Protocol                         */
        0,                                          /* Reserved                         */
        usb_host_audio_control_event  /* Application call back function   */
    },
#endif
    {
        {0xd1,0x18}/*{0x00,0x00}*/,   /* Vendor ID per USB-IF             */
        {0xFF,0xFF},                  /* Product ID per manufacturer      */
        USB_CLASS_AUDIO,              /* Class code                       */
        USB_SUBCLASS_AUD_STREAMING,   /* Sub-Class code                   */
        0xFF /*0x00*/,                 /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_audio_stream_event   /* Application call back function   */
    },
#endif   
    /* USB accessory */
    {
        {0x00,0x00}/*{0x00,0x00}*/,    /* Vendor ID per USB-IF     18d1    */
        {0x00,0x00},                   /* Product ID per manufacturer      */
        0xFF,                          /* Class code  => 0xff for any */
        0xFF,                          /* Sub-Class code                   */
        0,                             /* Protocol                         */
        0x5,                           /* Reserved                         */
        usb_host_accessory_event       /* Application call back function   */
    },
    {
        {0x00,0x00},                  /* All-zero entry terminates        */
        {0x00,0x00},                  /* driver info list.                */
        0,
        0,
        0,
        0,
        default_callback
    }
};

/**************************************************************************
   Global variables
**************************************************************************/

struct _usb_audio_host_info_		* audio_host_info = NULL;
/* struct _usb_accessoy_host_info_		* acc_host_info; */

/*****************************************************************************
 * Local Functions
 *****************************************************************************/
static uint32_t usbh_task_create(struct _usb_audio_host_info_ * handle)
{
    _task_id task_id;
    TASK_TEMPLATE_STRUCT task_template;

    /* create task for processing interrupt deferred work */
    task_template.TASK_TEMPLATE_INDEX = USB_AOA_TASK_TEMPLATE_INDEX;
    task_template.TASK_ADDRESS = USB_AOA_TASK_ADDRESS;
    task_template.TASK_STACKSIZE = USB_AOA_TASK_STACKSIZE;
    task_template.TASK_PRIORITY = USBCFG_AOA_TASK_PRIORITY;
    task_template.TASK_NAME = USB_AOA_TASK_NAME;
    task_template.TASK_ATTRIBUTES = USB_AOA_TASK_ATTRIBUTES;
    task_template.CREATION_PARAMETER = (uint32_t)(handle->acc_info);
    task_template.DEFAULT_TIME_SLICE = USB_AOA_TASK_DEFAULT_TIME_SLICE;

    task_id = _task_create_blocked(0, 0, (uint32_t)&task_template);

    if (task_id == 0) {
        return USBERR_UNKNOWN_ERROR;
    }

    handle->acc_info->usb_acc_task_id = task_id;

    _task_ready(_task_get_td(task_id));

    handle->acc_info->acc_task_state = USBH_AOA_TASK_STATE_READY;

    return USB_OK;
}

static void usbh_adk_init(struct _usb_audio_host_info_ * handle)
{
    int i;
    handle->device_direction = USB_AUDIO_DEVICE_DIRECTION_UNDEFINE;
#ifdef USE_AUDIO_CONTROL
    handle->res_vol[1] = 0x10;		    /*{0x00, 0x10, 0x00, 0x00}*/
    handle->cur_samp[1] = 0x10;		/*{0x00, 0x10, 0x00, 0x00}*/
#endif

    /* dummy sensor init */
    handle->acc_info->dummy_sensor.hTemp = 60;
    handle->acc_info->dummy_sensor.hHum = 60;
    handle->acc_info->dummy_sensor.bPress = 60;
    handle->acc_info->dummy_sensor.bTemp = 60;
    for (i = 0; i < 7; i++) {
        handle->acc_info->dummy_sensor.prox[i] = 60;
    }
    for (i = 0; i < 3; i++) {
        handle->acc_info->dummy_sensor.accel[i] = 60;
        handle->acc_info->dummy_sensor.mag[i] = 60;
    }
    handle->acc_info->dummy_sensor.btSSP = ADK_BT_SSP_DONE_VAL;
    /* handle->acc_info->dummy_sensor.btSSPlocked = 0; */
    handle->acc_info->adk_status = WATIING_TRY_ACC;
}

#if 0
_mqx_int _usb_device_host_uninstall (void)
{ /* Body */
    void * callback_struct_table_ptr;
    callback_struct_table_ptr = _mqx_get_io_component_handle(IO_USB_COMPONENT);

    if (callback_struct_table_ptr) {
        _mem_free(callback_struct_table_ptr);
    }
    else {
        return IO_ERROR;
    }
    /* Endif */
    _mqx_set_io_component_handle(IO_USB_COMPONENT, NULL);

    return MQX_OK;
} /* EndBody */
#endif

USB_STATUS acc_host_info_init()
{
    uint8_t * acc_buffers;
    printf("acc_host_info_init\n");
    USB_lock();
    if(audio_host_info != NULL) {
        printf("audio host info is not NULL\n");
        return USB_OK;
    }

    audio_host_info = USB_mem_alloc_zero(sizeof(struct _usb_audio_host_info_));
    if(audio_host_info == NULL) {
        printf("alloc audio host info fail\n");
        goto init_fail1;
    }

    audio_host_info->wav_recv_buff = USB_mem_alloc_zero(MAX_ISO_PACKET_SIZE);
    if(audio_host_info->wav_recv_buff == NULL) {
        printf("alloc audio data buff fail\n");
        goto init_fail3;
    }

    audio_host_info->acc_info = USB_mem_alloc_zero(sizeof(struct _usb_accessoy_host_info_));
    if(audio_host_info->acc_info == NULL) {
        printf("alloc accessory host info fail\n");
        goto init_fail4;
    }

    acc_buffers = USB_mem_alloc_zero(MAX_PACKET_SZ *2);
    if(acc_buffers == NULL) {
        printf("alloc audio data buff fail\n");
        goto init_fail5;
    }

    audio_host_info->acc_info->receiveBuf = acc_buffers;
    audio_host_info->acc_info->reply = acc_buffers + MAX_PACKET_SZ;

#ifdef USE_AUDIO_CONTROL
    if (MQX_OK !=_lwevent_create(&audio_host_info->USB_Audio_FU_Request, LWEVENT_AUTO_CLEAR)) {
        printf("\n_lwevent_create USB_Audio_FU_Request failed\n");
        goto init_fail6;
    }
#endif

    if (MQX_OK !=_lwevent_create(&(audio_host_info->acc_info->acc_event), LWEVENT_AUTO_CLEAR)) {
        printf("\n_lwevent_create accessory event failed\n");
        goto init_fail7;
    }

    usbh_adk_init(audio_host_info);

    usbh_task_create(audio_host_info);   /* fixme ,can it alway create success ? */

    //g_host_info.pdev_info = (void *)audio_host_info;
    USB_unlock();

    return USB_OK;

init_fail7:
    _lwevent_destroy(&audio_host_info->USB_Audio_FU_Request);
init_fail6:
    USB_mem_free(audio_host_info->acc_info->receiveBuf);  /* ? */
init_fail5:
    USB_mem_free(audio_host_info->acc_info);
init_fail4:
    USB_mem_free(audio_host_info->wav_recv_buff);
init_fail3:
    USB_mem_free(audio_host_info);
init_fail1:
    USB_unlock();
    return USBERR_ERROR;

}

USB_STATUS acc_host_info_deinit()
{
    struct _usb_accessoy_host_info_	* acc_info = NULL;
    printf("acc_host_info_deinit\n");
    if(audio_host_info != NULL) {
        acc_info = audio_host_info->acc_info;
    }
    else {
        printf("audio_host_info is NULL\n");
        return USBERR_ERROR;
    }

    _task_destroy(acc_info->usb_acc_task_id);

    _lwevent_destroy(&acc_info->acc_event);
#ifdef USE_AUDIO_CONTROL
    _lwevent_destroy(&audio_host_info->USB_Audio_FU_Request);
#endif

    USB_mem_free(acc_info->receiveBuf);

    USB_mem_free(audio_host_info->acc_info);

    USB_mem_free(audio_host_info->wav_recv_buff);

    USB_mem_free(audio_host_info);

    audio_host_info = NULL;
    //g_host_info.pdev_info = NULL;

    return USB_OK;
}


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : check_device_type
* Returned Value : None
* Comments       :
*    This function check whether the attached device is out-device or in-device. 
*
*END*--------------------------------------------------------------------*/
#ifdef USE_AUDIO_CONTROL
static USB_STATUS check_device_type (
    /* [IN] Input terminal descriptor */
    USB_AUDIO_CTRL_DESC_IT_PTR      it_desc,

    /* [IN] Output terminal descriptor */
    USB_AUDIO_CTRL_DESC_OT_PTR      ot_desc,

    /* [OUT] Terminal type name */
    char * *                  device_type,

    /* [OUT] device direction */
    char *                        direction )
{
    unsigned char it_type_high, it_type_low, ot_type_low;

    it_type_high = it_desc->wTerminalType[0];
    it_type_low  = it_desc->wTerminalType[1];
    ot_type_low  = ot_desc->wTerminalType[1];

    /* Input terminal associates with audio streaming */
    if (USB_TERMINAL_TYPE == it_type_low) {
        *direction = USB_AUDIO_DEVICE_DIRECTION_IN;
    }
    /* Input terminal type */
    else if (INPUT_TERMINAL_TYPE == it_type_low) {
        /* get type device name */
        *device_type = it_type_string[it_type_high];
    }
    else {
        return USBERR_ERROR;
    }

    /* Output terminal associates with audio streaming */
    if (USB_TERMINAL_TYPE == ot_type_low) {
        if (USB_AUDIO_DEVICE_DIRECTION_IN == (*direction)) {
            *direction = USB_AUDIO_DEVICE_DIRECTION_UNDEFINE;
            return USBERR_ERROR;
        }
        else {
            *direction = USB_AUDIO_DEVICE_DIRECTION_OUT;
        }
    }
    /* Output terminal type */
    else if (OUTPUT_TERMINAL_TYPE == ot_type_low) {
        /* get type device name */
        *device_type = ot_type_string[it_type_high];
    }
    else {
        return USBERR_ERROR;
    }
    return USB_OK;
}
#endif

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : USB_Audio_Get_Packet_Size
* Returned Value : None
* Comments       :
*     This function gets the frequency supported by the attached device.
*
*END*--------------------------------------------------------------------*/
static uint32_t USB_Audio_Get_Packet_Size (
/* [IN] Point to format type descriptor */
USB_AUDIO_STREAM_DESC_FORMAT_TYPE_PTR format_type_desc )
{
#if 0
    uint32_t packet_size_tmp;
    /* calculate packet size to send to the device each mS.*/
    /* packet_size = (sample frequency (Hz) /1000) * 
    * (bit resolution/8) * number of channels */
    packet_size_tmp = (((format_type_desc->tSamFreq[2] << 16)|
    (format_type_desc->tSamFreq[1] << 8) |
    (format_type_desc->tSamFreq[0] << 0) )
    *(format_type_desc->bBitResolution/8)
    *(format_type_desc->bNrChannels)
    /1000);
    return (packet_size_tmp);
#else
    return 512;     /* only for adk2.0 */
#endif
}

#if 0
/* ADK2 set directly as 44100,16bits,2chs */
static uint_32 USB_Audio_Get_Packet_Size ( 
	uint_16 sample_freq,uint_8 bit_resolution,uint_8 nr_channels )
{
    uint_32 packet_size_tmp;
    /* calculate packet size to send to the device each mS.*/
    /* packet_size = (sample frequency (Hz) /1000) * 
    * (bit resolution/8) * number of channels */
    packet_size_tmp = ((sample_freq)*(bit_resolution/8)*(nr_channels)/1000);
    return 512;
    return (packet_size_tmp);
}
#endif

void aoa_audio_task (uint32_t param)
{ /* Body */
    /* USB_STATUS           status = USB_OK; */
    /* _usb_host_handle     host_handle = audio_host_info->host_handle; */
#ifdef USE_AUDIO_CONTROL
    FEATURE_CONTROL_STRUCT control_feature;
    uint8_t bSamFreqType_index;
#endif
	
#ifdef USB_ACCESSORY_PLAY
    static bool snd_init_flag = FALSE;
#endif

    for(;;)  {
        if (audio_host_info == NULL) {
            _time_delay(5);
            //return;
            continue;
        }

        /* Get information of audio interface */
        if (USB_DEVICE_INTERFACED == audio_host_info->audio_stream.DEV_STATE) {
#ifdef USE_AUDIO_CONTROL
            /* Audio device information */
            pr_debug("Audio device information:\n");
            pr_debug("   - Device type    : %s\n", audio_host_info->device_string);
            for (bSamFreqType_index =0; bSamFreqType_index < audio_host_info->frm_type_desc->bSamFreqType; bSamFreqType_index++) {
                if(0 == bSamFreqType_index) {
                    audio_host_info->cur_samp[0] = audio_host_info->frm_type_desc->tSamFreq[3*bSamFreqType_index + 0];
                    audio_host_info->cur_samp[1] = audio_host_info->frm_type_desc->tSamFreq[3*bSamFreqType_index + 1];
                    audio_host_info->cur_samp[2] = audio_host_info->frm_type_desc->tSamFreq[3*bSamFreqType_index + 2];
                }
                printf("   - Frequency device support : %d Hz\n", (audio_host_info->frm_type_desc->tSamFreq[3*bSamFreqType_index + 2] << 16) |
                (audio_host_info->frm_type_desc->tSamFreq[3*bSamFreqType_index + 1] << 8)  |
                (audio_host_info->frm_type_desc->tSamFreq[3*bSamFreqType_index + 0] << 0));
            }

            /* Set current sampling frequency */
            audio_host_info->audio_com.CLASS_PTR = (CLASS_CALL_STRUCT_PTR)&(audio_host_info->audio_stream.CLASS_INTF);
            audio_host_info->audio_com.CALLBACK_FN = usb_host_audio_request_ctrl_callback;
            audio_host_info->audio_com.CALLBACK_PARAM = (void *)&control_feature;
            usb_class_audio_endpoint_command(&audio_host_info->audio_com,(void *)(audio_host_info->cur_samp),USB_AUDIO_SET_CUR_SAMPLING_FREQ);
            /* print device information */
            pr_debug("   - Bit resolution : %d bits\n", audio_host_info->frm_type_desc->bBitResolution);
            pr_debug("   - Number of channels : %d channels\n", audio_host_info->frm_type_desc->bNrChannels);
            pr_debug("   - Transfer type : %s\n", TransferType[(audio_host_info->endp->bmAttributes)&EP_TYPE_MASK]);
            pr_debug("   - Sync type : %s\n", SyncType[(audio_host_info->endp->bmAttributes >> 2)&EP_TYPE_MASK]);
            pr_debug("   - Usage type : %s\n", DataType[(audio_host_info->endp->bmAttributes >> 4)&EP_TYPE_MASK]);

            /* Check device type */
            if (audio_host_info->device_direction != USB_AUDIO_DEVICE_DIRECTION_OUT) {
                pr_debug("The device is unsupported!\n");
            }
#endif
            audio_host_info->audio_stream.DEV_STATE = USB_DEVICE_INUSE;
            if (USB_DEVICE_INUSE == audio_host_info->audio_stream.DEV_STATE) {
                printf("main task interfaced \n");

#ifndef CONNECT_TO_APK
            // _lwevent_set(&(audio_host_info->acc_info->acc_event), USBH_AOA_INTERFACED_EVENT);
#endif
                if(!snd_init_flag) {
                    msi_snd_init_with_periodbuffer(512, 8);
                    msi_snd_set_format(44100, 16,2);
                    msi_snd_umute();
                    snd_init_flag = TRUE;
                    printf("msi init \n");
                }

#ifdef USE_USB_AUDIO
            /* the attached device is microphone */
#ifdef USE_AUDIO_CONTROL
                if(USB_AUDIO_DEVICE_DIRECTION_OUT == audio_host_info->device_direction) {
                    /* Recv data */
                    usb_audio_recv_data(
                    (CLASS_CALL_STRUCT_PTR)&(audio_host_info->audio_control.CLASS_INTF),
                    (CLASS_CALL_STRUCT_PTR)&(audio_host_info->audio_stream.CLASS_INTF),
                    usb_host_audio_tr_callback,
                    NULL, audio_host_info->packet_size,
                    (unsigned char *)audio_host_info->wav_recv_buff);
                }
#else
                AUDIO_STREAM_INTERFACE_STRUCT_PTR   stream_if_ptr;
                stream_if_ptr = (AUDIO_STREAM_INTERFACE_STRUCT_PTR)(((CLASS_CALL_STRUCT_PTR)&(audio_host_info->audio_stream.CLASS_INTF))->class_intf_handle);
                usb_khci_sof_intr_recv_data(host_handle, stream_if_ptr->iso_in_pipe,0);
                printf("msi sof rev start \n");
#endif
#endif
            }
        }
        else if( audio_host_info->audio_stream.DEV_STATE == USB_DEVICE_DETACHED) {
            if(snd_init_flag) {
                msi_snd_mute();
                msi_snd_deinit();   /* Note, there is a mutex lock in it for conflict*/
                snd_init_flag = FALSE;
                printf("msi deinit \n");
            }
            printf("stream detach ,audio closed\n");
            audio_host_info->audio_stream.DEV_STATE = /* USB_DEVICE_OTHER*/USB_DEVICE_IDLE;
            // return;  /* need return before otg destroy task ?  */
        }
        _time_delay(5);
    }
} /* Endbody */

#ifdef USE_AUDIO_CONTROL
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_mute_ctrl_callback
* Returned Value : None
* Comments       :
*     Called when a mute request is sent successfully.
*
*END*--------------------------------------------------------------------*/
#ifdef TR_CALLBACK_COMPLETE
int usb_host_audio_mute_ctrl_callback
#else
void usb_host_audio_mute_ctrl_callback  
#endif
/* callback when set mute command complete */
(
      /* [IN] pointer to pipe */
      _usb_pipe_handle  pipe_handle,

      /* [IN] user-defined parameter */
      void *           user_parm,

      /* [IN] buffer address */
      unsigned char *          buffer,

      /* [IN] length of data transferred */
      uint32_t           buflen,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      uint32_t           status  )
{ 
   /* Body */
	pr_debug("Set Mute successfully\n");
	// fflush(stdout);
#ifdef TR_CALLBACK_COMPLETE
    return TR_CALLBACK_COMPLETE;
#endif
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_request_ctrl_callback
* Returned Value : None
* Comments       :
*     Called when a mute request is sent successfully.
*
*END*--------------------------------------------------------------------*/
#ifdef TR_CALLBACK_COMPLETE
int usb_host_audio_request_ctrl_callback
#else
void usb_host_audio_request_ctrl_callback
#endif
(
/* [IN] pointer to pipe */
_usb_pipe_handle  pipe_handle,

/* [IN] user-defined parameter */
void *           user_parm,

/* [IN] buffer address */
unsigned char *         buffer,

/* [IN] length of data transferred */
uint32_t           buflen,

/* [IN] status, hopefully USB_OK or USB_DONE */
uint32_t           status   )
{ /* Body */
#if  0 //CODING_
    static FEATURE_CONTROL_STRUCT_PTR control_feature_ptr;
    control_feature_ptr = (FEATURE_CONTROL_STRUCT_PTR)user_parm;
    
    switch (control_feature_ptr->FU)
    {
    case USB_AUDIO_CTRL_FU_MUTE:
        {
            if (MQX_OK !=_lwevent_set(&audio_host_info->USB_Audio_FU_Request, FU_MUTE_MASK)) {
	#ifdef ACCESSORY_PRINTF
		   printf("_lwevent_set USB_Audio_FU_Request failed.\n");
	#endif
              
            }
            break;
        }
    case USB_AUDIO_CTRL_FU_VOLUME:
        {
            if (MQX_OK !=_lwevent_set(&audio_host_info->USB_Audio_FU_Request, FU_VOLUME_MASK)) {
	#ifdef ACCESSORY_PRINTF
		     printf("_lwevent_set USB_Audio_FU_Request failed.\n");
	#endif
            }
            break;
        }
    default:
        break;
    }
    fflush(stdout);
#endif
#ifdef TR_CALLBACK_COMPLETE
    return TR_CALLBACK_COMPLETE;
#endif
} /* Endbody */

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_control_event
* Returned Value : None
* Comments       :
*     Called when control interface has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
void usb_host_audio_control_event
   (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint32_t                          event_code
   )
{
    INTERFACE_DESCRIPTOR_PTR   intf_ptr =
    (INTERFACE_DESCRIPTOR_PTR)intf_handle; 
#ifdef ACCESSORY_PRINTF
        fflush(stdout); 
#endif

#ifndef CONNECT_TO_APK
    if(audio_host_info == NULL) {
        acc_host_info_init();
    }
#endif

    if(audio_host_info == NULL) {
        //if(g_host_info.host_handle != NULL)
        printf(" audio control interface attach before acc interface!\n");
        return;
    }

        printf("usb_host_audio_control_event class %0x,subclass %x, prot %x\n",
        intf_ptr->bInterfaceClass, intf_ptr->bInterfaceSubClass , intf_ptr->bInterfaceProtocol);

    switch (event_code) {
#ifndef MFI_USB_STACK
    case USB_CONFIG_EVENT:
    /* Drop through into attach, same processing */
    case USB_ATTACH_EVENT: 
#else
    case USB_ATTACH_EVENT:
        printf(" USB_ATTACH_EVENT \n");
        break;
    case USB_CONFIG_EVENT:
#endif
    {
        USB_AUDIO_CTRL_DESC_HEADER_PTR   header_desc = NULL;
        USB_AUDIO_CTRL_DESC_IT_PTR       it_desc  = NULL;
        USB_AUDIO_CTRL_DESC_OT_PTR       ot_desc  = NULL;
        USB_AUDIO_CTRL_DESC_FU_PTR       fu_desc  = NULL;

        if((USB_DEVICE_IDLE == audio_host_info->audio_stream.DEV_STATE) || \
        (USB_DEVICE_DETACHED == audio_host_info->audio_stream.DEV_STATE)) {
            audio_host_info->audio_control.DEV_HANDLE  = dev_handle;
            audio_host_info->audio_control.INTF_HANDLE = intf_handle;
            audio_host_info->audio_control.DEV_STATE   = USB_DEVICE_ATTACHED;
        }
        else {
            pr_debug("Audio device already attached\n");
            /* fflush(stdout); */
        }

        /* finds all the descriptors in the configuration */
        if (USB_OK != usb_class_audio_control_get_descriptors(
                dev_handle, intf_handle,
                &header_desc, &it_desc, &ot_desc, &fu_desc)) {
            break;
        };

        /* initialize new interface members and select this interface */
        if (USB_OK != _usb_hostdev_select_interface(dev_handle,
            intf_handle, (void *)&(audio_host_info->audio_control.CLASS_INTF))) {
            break;
        }

        /* set all info got from descriptors to the class interface struct */
        usb_class_audio_control_set_descriptors((void *)&(audio_host_info->audio_control.CLASS_INTF),
            header_desc, it_desc, ot_desc, fu_desc);

        if(USB_OK != check_device_type(
                it_desc, ot_desc,
                &audio_host_info->device_string,
                &audio_host_info->device_direction)) {
            audio_host_info->dev_check_state = 1;   /* error */
            printf("dev_check_state err\n");
            break;
        }
        pr_debug("----- Audio control interface: attach event -----\n");
        /* fflush(stdout); */
        pr_debug("State = attached");
        pr_debug("  Class = %d", intf_ptr->bInterfaceClass);
        pr_debug("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        pr_debug("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
        break;
    }
    case USB_INTF_EVENT: 
    {
        //USB_STATUS status;
        pr_debug("----- Audio control interface: interface event -----\n");
        audio_host_info->audio_control.DEV_STATE = USB_DEVICE_INTERFACED;
        break;
    }

    case USB_DETACH_EVENT:
    {
        AUDIO_CONTROL_INTERFACE_STRUCT_PTR if_ptr;

        if_ptr = (AUDIO_CONTROL_INTERFACE_STRUCT_PTR) \
        audio_host_info->audio_control.CLASS_INTF.class_intf_handle;

        _lwevent_destroy(&if_ptr->control_event);

        /*pr_debug*/printf("----- Audio control interface: detach event -----\n");
        pr_debug("State = detached");
        pr_debug("  Class = %d", intf_ptr->bInterfaceClass);
        pr_debug("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        pr_debug("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
        /* fflush(stdout); */

        audio_host_info->audio_control.DEV_HANDLE = NULL;
        audio_host_info->audio_control.INTF_HANDLE = NULL;
        audio_host_info->audio_control.DEV_STATE = USB_DEVICE_DETACHED;
        audio_host_info->dev_check_state = 0; /*error_state*/
#ifdef USE_AUDIO_CONTROL
        audio_host_info->device_direction = USB_AUDIO_DEVICE_DIRECTION_UNDEFINE;
#else /* dont check direct */
        audio_host_info->device_direction = USB_AUDIO_DEVICE_DIRECTION_OUT;
#endif
        break;
    }
    default:
        pr_debug("Audio Device: unknown control event\n");
        /* fflush(stdout); */
        break;
} /* EndSwitch */
/* fflush(stdout);  */

} /* Endbody */
#endif

/* add for KHCI call back */
USB_STATUS usb_khci_sof_intr_recv_data
(
    /* [IN] the USB Host state structure */
    _usb_host_handle           handle,

    /* [IN] the pipe handle */
    _usb_pipe_handle           pipe_handle,

    unsigned int in_interrupt )
{ /* Body */
    /* AUDIO_STREAM_INTERFACE_STRUCT_PTR   stream_if_ptr;
    AUDIO_CONTROL_INTERFACE_STRUCT_PTR  control_if_ptr; */
    TR_INIT_PARAM_STRUCT                tr;
    USB_STATUS                          status = USBERR_NO_INTERFACE;
    
    if(audio_host_info == NULL) {
        printf("audio_host_info is NULL\n");
        return USBERR_ERROR;
    }

    usb_hostdev_tr_init(&tr, (tr_callback)usb_host_audio_tr_callback, NULL);
    tr.G.RX_BUFFER = (unsigned char *) audio_host_info->wav_recv_buff;
    tr.G.RX_LENGTH = audio_host_info->packet_size;      /* MAX packet of android 4.1 */

    status = _usb_host_recv_data(handle, pipe_handle, &tr);   /* non blocking */

    return status;
} /* Endbody */

#ifdef TR_CALLBACK_COMPLETE
int usb_host_audio_tr_callback
#else
void usb_host_audio_tr_callback
#endif
(
    /* [IN] pointer to pipe */
    _usb_pipe_handle pipe_handle,
    
    /* [IN] user-defined parameter */
    void * user_parm,
    
    /* [IN] buffer address */
    unsigned char * buffer,
    
    /* [IN] length of data transferred */
    uint32_t buflen,
    
    /* [IN] status, hopefully USB_OK or USB_DONE */
    uint32_t status )
{
    
    if(USB_AUDIO_DEVICE_DIRECTION_OUT == audio_host_info->device_direction) {
#if 0
        if(buflen != 176 && buflen != 180) {
        //printf("%d\n", buflen);
        }
#endif

#ifndef AUTO_CALLBACK_IN_SOF
        usb_audio_recv_data(
        (CLASS_CALL_STRUCT_PTR)&(audio_host_info->audio_control.CLASS_INTF),
        (CLASS_CALL_STRUCT_PTR)&(audio_host_info->audio_stream.CLASS_INTF),
        usb_host_audio_tr_callback,
        NULL, audio_host_info->packet_size,
        (unsigned char *)audio_host_info->wav_recv_buff);
#endif

        if (buflen == 0){
#ifdef TR_CALLBACK_COMPLETE
            return  TR_CALLBACK_COMPLETE;
#else
            return;
#endif
        }

/*#ifdef MSI_WRAPPER
        msi_snd_write(buffer, buflen);
#else
        msi_snd_write(buffer, buflen/2);
#endif */
       msi_snd_write(buffer, buflen/2);
       //printf("%d\n", buflen);
    }

#ifdef TR_CALLBACK_COMPLETE
    return  TR_CALLBACK_COMPLETE;
#endif
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_accessory_event
* Returned Value : None
* Comments       :
*     Called when control interface has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
static void usb_host_accessory_event
(
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint32_t                          event_code  )
{
    INTERFACE_DESCRIPTOR_PTR   intf_ptr =
    (INTERFACE_DESCRIPTOR_PTR)intf_handle; 

    struct _usb_accessoy_host_info_ * acc_info;

    DEV_INSTANCE_PTR           dev_ptr = (DEV_INSTANCE_PTR)dev_handle;
    //static bool tried_accessory = FALSE;
#ifdef ACCESSORY_PRINTF
    fflush(stdout);
#endif

    printf("usb_host_accessory_event class %0x,subclass %x, prot %x\n",
        intf_ptr->bInterfaceClass, intf_ptr->bInterfaceSubClass , intf_ptr->bInterfaceProtocol);

    if((SHORT_UNALIGNED_LE_TO_HOST(dev_ptr->dev_descriptor.idVendor) != 0x18d1)
            || ((SHORT_UNALIGNED_LE_TO_HOST(dev_ptr->dev_descriptor.idProduct) & 0xfff0) != 0x2d00)){
        if(event_code == USB_CONFIG_EVENT)
            event_code = 0xff;
    }
    if(intf_ptr->bInterfaceClass != 0xff || intf_ptr->bInterfaceSubClass != 0xff || intf_ptr->bInterfaceProtocol != 0) {
      static int i =0;
      if(i++ == 3) 
        printf("Unregnized interface\n Ignore it\n");
        return;
    }
    
    if(audio_host_info)
        acc_info = audio_host_info->acc_info;

    switch (event_code) {
#ifndef MFI_USB_STACK        
    case USB_CONFIG_EVENT:
    /* Drop through into attach, same processing */
    case USB_ATTACH_EVENT:
#else
    case USB_ATTACH_EVENT:
        printf(" USB_ATTACH_EVENT \n");
        break;
    case USB_CONFIG_EVENT:
#endif        
    {
        if(audio_host_info == NULL) {
            acc_host_info_init();
            if(audio_host_info)
                acc_info = audio_host_info->acc_info;
            else{
                printf("audio_host_info is NULL!\n");
                return;
            }
        }

        pr_debug("State = %d", acc_info->acc_device.DEV_STATE);
        pr_debug("  Class = %d", intf_ptr->bInterfaceClass);
        pr_debug("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        pr_debug("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
        /*fflush(stdout);*/

        if (acc_info->acc_device.DEV_STATE == USB_DEVICE_IDLE) {
            acc_info->acc_device.DEV_HANDLE = dev_handle;
            acc_info->acc_device.INTF_HANDLE = intf_handle;
            acc_info->acc_device.DEV_STATE = USB_DEVICE_ATTACHED;
        } else {
            pr_debug("accessory device already attached - DEV_STATE = %d\n", acc_info->acc_device.DEV_STATE);
            /* fflush(stdout);*/
        } /* Endif */

        /* initialize new interface members and select this interface */
        if (USB_OK != _usb_hostdev_select_interface(dev_handle,
            intf_handle, (void *)&(acc_info->acc_device.CLASS_INTF)))  {
            pr_debug("select interface fail\n");
            break;
        }
        break;
    }
    case USB_INTF_EVENT:
    {
        tried_accessory = TRUE;
        pr_debug("----- accessory interface: interface event -----\n");
        if (acc_info->acc_device.DEV_STATE != USB_DEVICE_INTERFACED) {
            acc_info->acc_device.DEV_STATE = USB_DEVICE_INTERFACED;
            _lwevent_set(&(audio_host_info->acc_info->acc_event), USBH_AOA_INTERFACED_EVENT);
        }

        break;
    }
    case USB_DETACH_EVENT:
    {
        /* pr_debug*/printf("----- accessory interface: detach event -----%d-%d\n",acc_info->acc_device.DEV_STATE,tried_accessory);
        //if (acc_info->acc_device.DEV_STATE != USB_DEVICE_IDLE) {
        if (1){

            if(tried_accessory) {
                tried_accessory = FALSE;	 
            }
            acc_info->acc_device.DEV_HANDLE = NULL;
            acc_info->acc_device.INTF_HANDLE = NULL;
            acc_info->acc_device.DEV_STATE = USB_DEVICE_DETACHED;
            _lwevent_set(&(audio_host_info->acc_info->acc_event), USBH_AOA_DETACH_EVENT);
            //tried_accessory = FALSE;

        } else {
            pr_debug("accessory Device is not attached\n");
            /*fflush(stdout);*/
        } /* EndIf */
        break;
    }
    /* add for try accessroy */
    case 0xFF:
        /*pr_debug*/printf("event USBH_DEVICE_TRY_ACCESSORY(0xff),event_code=%d,accessory_device.DEV_STATE =%d \n",event_code,acc_info->acc_device.DEV_STATE);

        if(!tried_accessory) {
        printf("accessory try ,create task \n");
        /* fixme when audio stream interface attach before acc interface attach */

        //usbh_task_create(audio_host_info);   /* fixme ,can it alway create success ? */
        if(audio_host_info == NULL) {
            acc_host_info_init();
        }

        acc_info = audio_host_info->acc_info;
        acc_info->acc_device.DEV_HANDLE = dev_handle;
        acc_info->acc_device.INTF_HANDLE = intf_handle;
        tried_accessory = TRUE;			
        _lwevent_set(&(acc_info->acc_event),USBH_TRY_AOA_START_EVENT);
        } /* if(!tried_accessory) */
        
        // also idle, waiting next connet !!! 
        break;
    default:
        pr_debug("accessory Device: unknown control event\n");
        /* fflush(stdout); */
        break;
    } /* EndSwitch */
	/* fflush(stdout);*/
} /* Endbody */

   
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_audio_stream_event
* Returned Value : None
* Comments       :
*     Called when stream interface has been attached, detached, etc.
*END*--------------------------------------------------------------------*/
static void usb_host_audio_stream_event (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint32_t                          event_code )
{ /* Body */
    INTERFACE_DESCRIPTOR_PTR   intf_ptr = (INTERFACE_DESCRIPTOR_PTR)intf_handle;

    /* Check audio stream interface alternating 0 */
    if (0 == intf_ptr->bNumEndpoints)
        return;

#ifndef CONNECT_TO_APK
    if(audio_host_info == NULL) {
        acc_host_info_init();
    }
#endif

    /*while*/if(audio_host_info == NULL) {
        printf(" audio stream interface attach before acc interface!\n");
        return;
    }

    printf("usb_host_audio_stream_event class %0x,subclass %x, prot %x\n",
        intf_ptr->bInterfaceClass, intf_ptr->bInterfaceSubClass , intf_ptr->bInterfaceProtocol);
    
    switch (event_code) {
#ifndef MFI_USB_STACK     
    case USB_CONFIG_EVENT:
    /* Drop through into attach, same processing */
    case USB_ATTACH_EVENT: 
#else /* for changed of usb stack in mfi*/
    case USB_ATTACH_EVENT:
        printf("USB_ATTACH_EVENT \n");
        break;

    case USB_CONFIG_EVENT:
#endif
    {
#ifdef USE_AUDIO_CONTROL
        USB_AUDIO_STREAM_DESC_SPEPIFIC_AS_IF_PTR     as_itf_desc = NULL;
        USB_AUDIO_STREAM_DESC_SPECIFIC_ISO_ENDP_PTR  iso_endp_spec_desc = NULL;
#endif

        if((USB_DEVICE_IDLE == audio_host_info->audio_stream.DEV_STATE) ||\
        (USB_DEVICE_DETACHED == audio_host_info->audio_stream.DEV_STATE)) {
            audio_host_info->audio_stream.DEV_HANDLE  = dev_handle;
            audio_host_info->audio_stream.INTF_HANDLE = intf_handle;
#ifndef CONNECT_TO_APK
            /* when we don't connect to apk then we set acc intf to NULL ! */
            audio_host_info->acc_info->acc_device.DEV_HANDLE = /* dev_handle */NULL;
            audio_host_info->acc_info->acc_device.INTF_HANDLE = /* intf_handle*/NULL;
            audio_host_info->acc_info->acc_device.DEV_STATE = USB_DEVICE_DETACHED;
#endif
            audio_host_info->audio_stream.DEV_STATE   = USB_DEVICE_ATTACHED;
        }
        else {
            pr_debug("Audio device already attached\n");
            //break;  /* fixme ,need break ? */
        }

        /* finds all the descriptors in the configuration */
        if (USB_OK != usb_class_audio_stream_get_descriptors(
                dev_handle, intf_handle, &as_itf_desc,
                &audio_host_info->frm_type_desc, &iso_endp_spec_desc)) {
            break;
        };

        /* initialize new interface members and select this interface */
        if (USB_OK != _usb_hostdev_select_interface(dev_handle,
            intf_handle, (void *)&(audio_host_info->audio_stream.CLASS_INTF))) {
            break;
        }

        audio_host_info->packet_size = USB_Audio_Get_Packet_Size(audio_host_info->frm_type_desc);

        pr_debug("packet size %d\n",audio_host_info->packet_size);

        /* set all info got from descriptors to the class interface struct */
        usb_class_audio_stream_set_descriptors(
        (void *)&audio_host_info->audio_stream.CLASS_INTF,
        as_itf_desc,
        audio_host_info->frm_type_desc,
        iso_endp_spec_desc
        );

        pr_debug("----- Audio stream interface: attach event -----\n");
        pr_debug("State = attached");
        pr_debug("  Class = %d", intf_ptr->bInterfaceClass);
        pr_debug("  SubClass = %d", intf_ptr->bInterfaceSubClass);
        pr_debug("  Protocol = %d\n", intf_ptr->bInterfaceProtocol);
        /* fflush(stdout); */
        break;
    }   
    case USB_INTF_EVENT: 
    {
        if(0 == audio_host_info->dev_check_state /*error_state*/) {
            printf("stream interfaced event\n");

            audio_host_info->audio_stream.DEV_STATE = USB_DEVICE_INTERFACED;
            if (USB_OK != _usb_hostdev_get_descriptor( 
            dev_handle,
            intf_handle,
            /* Functional descriptor 
            * for interface */
            USB_DESC_TYPE_EP,  
            /* Index of descriptor */
            1,      
            /* Index of interface alternate */
            intf_ptr->bAlternateSetting,
            (void * *)&(audio_host_info->endp))
            ) {
                pr_debug("Get end point descriptor error!\n");
            }
            pr_debug("----- Audio stream interface: interface event-----\n");  

#ifndef CONNECT_TO_APK
            _lwevent_set(&(audio_host_info->acc_info->acc_event), USBH_AOA_INTERFACED_EVENT);
#else           /* work around for i.mx android 4.x */
            //_lwevent_set(&(audio_host_info->acc_info->acc_event), USBH_AOA_INTERFACED_EVENT);
#endif                    
        }
        else {
            pr_debug("The device is unsupported!\n");
            /* fflush(stdout);*/
        }
        break;
    }
    case USB_DETACH_EVENT: 
    {
        /* pr_debug*/printf("----- audio_stream interface: detach event -----%d\n",audio_host_info->audio_stream.DEV_STATE);
        //if (audio_host_info->audio_stream.DEV_STATE != USB_DEVICE_IDLE) 
        if ((audio_host_info->audio_stream.DEV_STATE == USB_DEVICE_INUSE) || 
             (audio_host_info->audio_stream.DEV_STATE == USB_DEVICE_INTERFACED)) {
            audio_host_info->audio_stream.DEV_HANDLE = NULL;
            audio_host_info->audio_stream.INTF_HANDLE = NULL;
#ifndef CONNECT_TO_APK
            // audio_host_info->acc_info->acc_device.DEV_HANDLE = NULL;
            // audio_host_info->acc_info->acc_device.INTF_HANDLE = NULL;
            // audio_host_info->acc_info->acc_device.DEV_STATE = USB_DEVICE_DETACHED;
#endif        
            audio_host_info->audio_stream.DEV_STATE = USB_DEVICE_DETACHED;

#ifndef CONNECT_TO_APK
        _lwevent_set(&(audio_host_info->acc_info->acc_event), USBH_AOA_DETACH_EVENT);
#endif                         
        } else {
            pr_debug("audio_stream Device is not attached\n");
            /*fflush(stdout);*/
        } /* EndIf */
        break;
    }
    default:
        pr_debug("Audio device: unknown data event\n");
        /* fflush(stdout); */
        break;
    }
}

#ifdef USE_ACC_HID
#ifdef TR_CALLBACK_COMPLETE
extern int usb_hid_callback
#else
extern void usb_hid_callback
#endif
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
      USB_STATUS        status );

uint32_t AccessoryReleaseKey(void)
{
    if(usb_device_hid_send_event(audio_host_info->acc_info->acc_device.DEV_HANDLE,
            audio_host_info->acc_info->acc_device.INTF_HANDLE,usb_hid_callback, KEY_RELEASED) != 0)
        return 0x01;
    else
        return 0x00;
}
 
uint32_t AccessoryNextPlay(void)
{
    printf("hid send key next\n");
    AccessoryReleaseKey();
    if(usb_device_hid_send_event(audio_host_info->acc_info->acc_device.DEV_HANDLE,
            audio_host_info->acc_info->acc_device.INTF_HANDLE,usb_hid_callback, SCAN_NEXT_TRACK) != 0)
        return 0x01;
    else
        return 0x00;
}

uint32_t AccessoryPrevPlay(void)
{
    printf("hid send key prev\n");
    AccessoryReleaseKey();
    if(usb_device_hid_send_event(audio_host_info->acc_info->acc_device.DEV_HANDLE,
            audio_host_info->acc_info->acc_device.INTF_HANDLE,usb_hid_callback, SCAN_PREV_TRACK) != 0)
        return 0x01;
    else
        return 0x00;
}

uint32_t AccessoryPausePlay(void)
{
    if(usb_device_hid_send_event(audio_host_info->acc_info->acc_device.DEV_HANDLE,
            audio_host_info->acc_info->acc_device.INTF_HANDLE,usb_hid_callback, PAUSE) != 0)
        return 0x01;
    else
        return 0x00;
}

uint32_t AccessoryResumePlay(void)
{
    if(usb_device_hid_send_event(audio_host_info->acc_info->acc_device.DEV_HANDLE,
            audio_host_info->acc_info->acc_device.INTF_HANDLE,usb_hid_callback, PLAY) != 0)
        return 0x01;
    else
        return 0x00;
}

#endif  /*  USE_ACC_HID */


int aoaRegisterDriverInfoTable(USB_HOST_DRIVER_INFO *infotable, int size) {
    int tablesize = sizeof (DriverInfoTable) / sizeof(DriverInfoTable[0]);

    if((tablesize == 0) || (tablesize > size)){
        printf("UMASS driver info size is too large:%d more than globe avaiable size %d \n", tablesize, size);
        return 0;
    }

    memcpy(infotable, DriverInfoTable, (tablesize - 1) * sizeof(DriverInfoTable[0]));

    return tablesize - 1;
}

/* end file */

