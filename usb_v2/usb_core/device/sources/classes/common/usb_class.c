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
* $FileName: usb_class.c$
* $Version : 
* $Date    : 
*
* Comments:
*
* @brief The file contains USB stack Class module implimentation.
*
*****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "usb_device_config.h"
#include "usb.h"

#include "usb_device_stack_interface.h"

#if USBCFG_DEV_HID || USBCFG_DEV_PHDC || USBCFG_DEV_AUDIO || USBCFG_DEV_CDC || USBCFG_DEV_MSC

#include "usb_class_internal.h"
#include "usb_framework.h"

/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/

/****************************************************************************
 * Global Variables
 ****************************************************************************/
static usb_class_object_struct_t usb_class_object[USBCFG_DEV_MAX_CLASS_OBJECT];
#if USBCFG_DEV_COMPOSITE
static usb_class_handle s_class_handle = USB_UNINITIALIZED_VAL_32;
#endif
/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes 
 *****************************************************************************/

/*****************************************************************************
 * Local Variables 
 *****************************************************************************/ 
 
 /*****************************************************************************
 * Local Functions - None
 *****************************************************************************/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/

 /*************************************************************************//*!
 *
 * @name  USB_Class_Allocate_Handle
 *
 * @brief The funtion reserves entry in device array and returns the index.
 *
 * @param none.
 * @return returns the reserved handle or if no entry found device busy.      
 *
 *****************************************************************************/
static usb_status USB_Class_Allocate_Handle(usb_class_object_struct_t** pclassobj)
{
    int32_t cnt = 0;
    for (;cnt< USBCFG_DEV_MAX_CLASS_OBJECT;cnt++)
    {
       if (usb_class_object[cnt].controller_handle == NULL)
       {
           *pclassobj = &usb_class_object[cnt];
           return USB_OK;
       }
    }
    return USBERR_DEVICE_BUSY;
}

 /*************************************************************************//*!
 *
 * @name  USB_Class_Free_Handle
 *
 * @brief The funtion releases entry in device array .
 *
 * @param handle  index in device array to be released..
 * @return returns and error code or USB_OK.      
 *
 *****************************************************************************/
static usb_status USB_Class_Free_Handle(usb_class_object_struct_t* handle)
{
    int32_t cnt = 0;
    for (;cnt< USBCFG_DEV_MAX_CLASS_OBJECT;cnt++)
    {
        if (&usb_class_object[cnt] == handle)
        {
            usb_class_object[cnt].controller_handle = NULL;
            usb_class_object[cnt].usb_fw_handle     = 0;
            usb_class_object[cnt].arg               = NULL;
            usb_class_object[cnt].class_callback    = NULL;
            return USB_OK;
        }
    }

    return USBERR_INVALID_PARAM;    
}


/**************************************************************************//*!
 *
 * @name  USB_Class_Init
 *
 * @brief The funtion initializes the Class Module 
 *
 * @param handle             :handle to Identify the controller
 * @param class_callback     :event callback      
 * @param other_req_callback :call back for class/vendor specific requests on 
 *                            CONTROL ENDPOINT
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
usb_class_handle USB_Class_Init
(
    usb_device_handle handle, /* [IN] the USB device controller to initialize */                  
    usb_device_notify_t  class_callback,/*[IN]*/
    usb_request_notify_t other_req_callback,/*[IN]*/
    void* user_arg,/*[IN]*/
    usb_desc_request_notify_struct_t*  desc_callback_ptr/*[IN]*/
 ) 
{
    usb_class_object_struct_t* class_object_ptr = NULL;
    usb_status ret;
    
    ret = USB_Class_Allocate_Handle(&class_object_ptr);
    if (USBERR_DEVICE_BUSY == ret) 
    {
        return USBERR_DEVICE_BUSY;
    }
        
    class_object_ptr->controller_handle = handle;
    class_object_ptr->class_callback = class_callback;
    class_object_ptr->arg = user_arg;

    usb_device_register_application_notify(handle, class_callback, user_arg);
    usb_device_register_vendor_class_request_notify(handle, other_req_callback, user_arg);
    usb_device_register_desc_request_notify(handle, desc_callback_ptr, user_arg);
    
#ifdef USBCFG_OTG
   {
        descriptor_union_t desc;
        uint32_t config_size;
        usb_status error;
        uint8_t i;
        uint8_t bm_attributes = 0;
        error = desc_callback_ptr->get_desc(desc_callback_ptr->handle,USB_DESC_TYPE_CFG,0,0,(uint8_t **)&desc,&config_size);
        if(error == USB_OK)
        {
            config_size = USB_SHORT_LE_TO_HOST(*(uint16_t*)desc.cfig->wTotalLength);
            i= desc.cfig->bLength;
            desc.word += desc.cfig->bLength;
            while(i<config_size)
            {
                if (desc.otg->bDescriptorType == USB_DESC_TYPE_OTG) 
                {
                    /* Found the OTG descriptor */
                    bm_attributes = desc.otg->bmAttributes;
                    break;
                } /* Endif */
                i += desc.otg->bLength;
                desc.word += desc.otg->bLength;
            }
        }
       _usb_device_otg_init(handle, bm_attributes);
    }
#endif    
#if USBCFG_DEV_COMPOSITE
	/* Suppose only one class handle can be assigned */
	s_class_handle = (usb_class_handle)class_object_ptr;
#endif
    return (usb_class_handle)class_object_ptr;     
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Deinit
 *
 * @brief The funtion initializes the Class Module 
 *
 * @param handle             :handle to Identify the controller
 * @param class_handle       :class handle      
 *
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
usb_status USB_Class_Deinit
(
    usb_device_handle handle, /* [IN] the USB device controller to initialize */                  
    usb_class_handle  class_handle
 ) 
{
    usb_status error = USB_OK;
    
    error = USB_Class_Free_Handle((usb_class_object_struct_t*)class_handle);

#if USBCFG_DEV_COMPOSITE
	/* Suppose only one class handle can be assigned */
	s_class_handle = USB_UNINITIALIZED_VAL_32;
#endif
    
    return error;     
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Send_Data
 *
 * @brief The funtion calls the device to send data upon recieving an IN token 
 *
 * @param handle:               handle to Identify the controller
 * @param ep_num:               The endpoint number     
 * @param buff_ptr:             buffer to send
 * @param size:                 length of transfer
 * 
 * @return status       
 *         USB_OK           : When Successfull 
 *         Others           : Errors
 *
 *****************************************************************************/
usb_status USB_Class_Send_Data
(
    usb_class_handle handle, /*[IN]*/
    uint8_t ep_num,        /* [IN] the Endpoint number */                  
    uint8_t * buff_ptr,      /* [IN] buffer to send */      
    uint32_t size           /* [IN] length of the transfer */
) 
{
    usb_status error = USB_OK;
    //uint16_t  state;
    usb_class_object_struct_t* class_object_ptr = (usb_class_object_struct_t*)handle;   
    
    if (NULL == class_object_ptr) 
        return USBERR_ERROR;

    error = usb_device_send_data(class_object_ptr->controller_handle,
                ep_num,buff_ptr,size);

    return error;      
}
 
/**************************************************************************//*!
 *
 * @name   USB_Class_Periodic_Task
 *
 * @brief  The funtion calls for periodic tasks 
 *
 * @param  None
 *
 * @return None
 *
 *****************************************************************************/
void USB_Class_Periodic_Task(void) 
{
    #ifdef DELAYED_PROCESSING  
        USB_Framework_Periodic_Task();
    #endif  
}
#if 0
/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Desc
 *
 * @brief  This function is called in to get the discriptor as specified in cmd.
 *
 * @param handle:           USB class handle. Received from
 *                          USB_Class_Init      
 * @param cmd:              command for USB discriptor to get.
 * @param in_data:          input to the Application functions.
 * @param in_buff           buffer which will contian the discriptors.
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
usb_status USB_Class_Get_Desc(usb_class_handle handle,/*[IN]*/
int32_t cmd,/*[IN]*/
uint8_t input_data,/*[IN]*/
uint8_t * *in_buf /*[OUT]*/
)
{
   USB_CLASS_OBJECT_STRUCT_PTR class_object_ptr = (USB_CLASS_OBJECT_STRUCT_PTR)handle;
   if (class_object_ptr == NULL)
        return USBERR_ERROR;
   
   return USB_Framework_GetDesc(class_object_ptr->usb_fw_handle,cmd,input_data,in_buf);
            
}
/**************************************************************************//*!
 *
 * @name  USB_Class_Set_Desc
 *
 * @brief  This function is called in to Set the discriptor as specified in cmd.
 *
 * @param handle:           USB class handle. Received from
 *                          USB_Class_Init      
 * @param cmd:              command for USB discriptor to get.
 * @param in_data:          input to the Application functions.
 * @param in_buff           buffer which will contian the discriptors.
 * @return status:       
 *                        USB_OK : When Successfull       
 *                        Others : When Error
 *
 *****************************************************************************/
usb_status USB_Class_Set_Desc(usb_class_handle handle,/*[IN]*/
int32_t cmd,/*[IN]*/
uint8_t input_data,/*[IN]*/
uint8_t * *out_buf /*[IN]*/
)
{
   USB_CLASS_OBJECT_STRUCT_PTR class_object_ptr = (USB_CLASS_OBJECT_STRUCT_PTR)handle;
   if (class_object_ptr == NULL)
        return USBERR_ERROR;
   
   return USB_Framework_SetDesc(class_object_ptr->usb_fw_handle,cmd,input_data,out_buf);
            
}
#endif
#if USBCFG_DEV_COMPOSITE
/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Class_Handle
 *
 * @brief  This function is called to return class handle.
 *
 * @return value:
 *                        class handle
 *
 *****************************************************************************/
usb_class_handle USB_Class_Get_Class_Handle()
{
    return s_class_handle;
}

/**************************************************************************//*!
 *
 * @name  USB_Class_Get_Ctrler_Handle
 *
 * @brief  This function is called to return controller handle.
 *
 * @return value:
 *                        controller handle
 *
 *****************************************************************************/
usb_device_handle USB_Class_Get_Ctrler_Handle(usb_class_handle class_handle)
{
    usb_device_handle ret;
    if(USB_UNINITIALIZED_VAL_32 != class_handle)
        ret = ((usb_class_object_struct_t*)class_handle)->controller_handle;
    else
        ret = NULL; 
    return ret;
}
#endif

#endif
/* EOF */
