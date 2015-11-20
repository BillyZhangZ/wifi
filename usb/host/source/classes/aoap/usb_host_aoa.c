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
* $FileName: usb_host_aoa.c$
* $Version : 
* $Date    : 
*
* Comments:
*   
*
*END************************************************************************/


#include "usb_host_aoa.h"

#include "mqx_host.h"

#include "bsp.h"
#include "host_dev_list.h"

/*--------------------------------------------------------------**
** This anchor points to the first class/interface in a linked  **
**   chain of structs, one for each functional accessory interface. **
**   Items are added by calling "select interface" and removed  **
**   by "delete interface".  Typically an application will call **
**   these select in its attach callback routine.  It may later **
**   call delete, or the call may be automatic during detach.   **
**--------------------------------------------------------------*/

static AOA_INTERFACE_STRUCT_PTR aoa_anchor = NULL;

/* Private functions for use within this file only */
static USB_STATUS usb_aoa_bulk_inout(CLASS_CALL_STRUCT_PTR,
   TR_INIT_PARAM_STRUCT_PTR, bool);
static USB_STATUS usb_aoa_class_send(CLASS_CALL_STRUCT_PTR,
   TR_INIT_PARAM_STRUCT_PTR);
static USB_STATUS usb_aoa_class_verify(CLASS_CALL_STRUCT_PTR,
   TR_INIT_PARAM_STRUCT_PTR, tr_callback, void *);


/*extern LWEVENT_STRUCT    USB_SOF_ISOIN_COMP_EVENT; liutest*/
USB_STATUS usb_aoa_bulk_inout
   (
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR      cc_ptr,
      
      /* [IN] TR containing setup packet to be sent */
      TR_INIT_PARAM_STRUCT_PTR   tr_ptr,
      
      /* [IN] send_flag = TRUE means send, = FALSE means receive */
      bool                    send_flag
   )
{ /* Body */

   AOA_INTERFACE_STRUCT_PTR  ais_ptr;
   USB_STATUS                    error = USBERR_NO_INTERFACE;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_aoa_bulk_inout");
   #endif

   USB_lock ();

   if (usb_host_class_intf_validate(cc_ptr)) {
      ais_ptr = (AOA_INTERFACE_STRUCT_PTR)cc_ptr->class_intf_handle;

	 /*if (_lwevent_wait_ticks(&USB_SOF_ISOIN_COMP_EVENT,1,FALSE,0) != MQX_OK) {
	                printf("\USB_SOF_ISOIN_COMP_EVENT  Wait failed\n");
	                return error;
	 }  liutest */
	 
      if (send_flag) {
         error = _usb_host_send_data (ais_ptr->host_handle,
            ais_ptr->bulk_out_pipe, tr_ptr);
      } else {
         error = _usb_host_recv_data (ais_ptr->host_handle,
            ais_ptr->bulk_in_pipe, tr_ptr);

      } /* EndIf */
   } /* Endif */

   USB_unlock ();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_aoa_bulk_inout, SUCCESSFUL");
   #endif
   
   return error;   

} /* Endbody */



USB_STATUS usb_aoa_class_send
   (
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR      cc_ptr,
      
      /* [IN] TR containing setup packet to be sent */
      TR_INIT_PARAM_STRUCT_PTR   tr_ptr
   )
{ /* Body */

   DESCRIPTOR_UNION              desc;
   AOA_INTERFACE_STRUCT_PTR  ais_ptr;
   USB_STATUS                    error = USBERR_NO_INTERFACE;
   USB_SETUP_PTR                 dev_req;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_aoa_class_send");
   #endif
   
   USB_lock ();
   if (usb_host_class_intf_validate (cc_ptr)) {
      ais_ptr = (AOA_INTERFACE_STRUCT_PTR) cc_ptr->class_intf_handle;
        
      error = _usb_host_send_setup (ais_ptr->host_handle, 
            ais_ptr->control_pipe, 
            tr_ptr);
   } /* EndIf */

   USB_unlock ();

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_aoa_class_send, SUCCESSFUL");
   #endif
   
   return error;   

} /* Endbody */



/*FUNCTION*----------------------------------------------------------------
* 
* Function Name  : usb_aoa_class_verify
* Returned Value : USB_OK or error code
* Comments       :
*     Verify that the device is still valid -- in particular that
*     it has not been detached (since that occurs without notice).
*     This function is a common prolog used by class calls.
* 
*END*--------------------------------------------------------------------*/

USB_STATUS usb_aoa_class_verify
   (
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR      cc_ptr,
      
      /* [IN] TR memory already allocated, initialized here */
      TR_INIT_PARAM_STRUCT_PTR   tr_ptr,
   
      /* [IN] callback for completion/status */
      tr_callback                callback,
   
      /* [IN] user parameter returned by callback */
      void *                    call_parm
   )
{ /* Body */

   AOA_INTERFACE_STRUCT_PTR  ais_ptr;
   USB_STATUS                    error = USBERR_NO_INTERFACE;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_aoa_class_verify");
   #endif
   
   USB_lock ();

   if (usb_host_class_intf_validate (cc_ptr)) {
      ais_ptr = (AOA_INTERFACE_STRUCT_PTR)
         cc_ptr->class_intf_handle;
      error =  usb_hostdev_validate (ais_ptr->dev_handle);

      if (error == USB_OK) {
         if (callback == NULL) {
            error = USBERR_NULL_CALLBACK;
         } else {
            usb_hostdev_tr_init (tr_ptr, callback, call_parm);
         } /* Endif */
      } /* EndIf */
   } /* Endif */

   USB_unlock ();   

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_aoa_class_verify, SUCCESSFUL");
   #endif
   
   return error;   

} /* Endbody */



#define  USB_SETUP_TYPE_VENDOR               (2<<5)
#define PKT_MAX_SIZE 64
unsigned char buffer[PKT_MAX_SIZE]; /* The size of buffermust little than max length of ADK2.0 strings  */


USB_STATUS usb_device_try_accessory
   (

   _usb_device_instance_handle      dev_handle,
   _usb_interface_descriptor_handle intf_handle,
   tr_callback                callback,
   /* [IN] user parameter returned by callback */
   void *                    call_parm,
   int  req_status
   )
{ /* Body */

   DEV_INSTANCE_PTR           dev_ptr;
   _usb_pipe_handle           pipe_handle;
   
   TR_INIT_PARAM_STRUCT          tr;
   USB_STATUS                    error;
   USB_SETUP                     dev_req;

	int i = 0;
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_try_accessory");
   #endif

   USB_lock();	
   dev_ptr = (DEV_INSTANCE_PTR)dev_handle;
   pipe_handle = dev_ptr->control_pipe;

   error = usb_hostdev_validate(dev_handle);

   if (error != USB_OK) {
   	 printf("liutest usb_hostdev_validate fail \n");
	 return USBERR_ERROR;
   }

#if 0
   if (MQX_OK !=_lwevent_create(&USB_TRY_AOA_EVENT, LWEVENT_AUTO_CLEAR)){
        printf("\n_lwevent_create USB_TRY_AOA_EVENT failed.\n");
	  return USBERR_ERROR;
   }
#endif
	// printf("liutest req_status =%d\n",req_status);

#ifdef USE_ACC_HID
    /* if (req_status == ACCESSORY_HID_REGISTER)
        return usb_device_hid_regist(dev_handle,intf_handle,callback, sizeof(hid_description));
    else if (req_status == ACCESSORY_HID_REPORT)
        return usb_device_hid_set_report_descriptor(dev_handle,intf_handle,callback);
    */
#endif  		

    /* Initialize the TR with TR index and default control callback 
   ** function if there is one registered
   */
   usb_hostdev_tr_init (&tr, callback, /*&req_status*/call_parm);
   //memset(&dev_req,0,sizeof(dev_req));	
   memset(buffer,0,PKT_MAX_SIZE);

   switch(req_status){
   	case GET_PROTOCOL_VERSION:
		tr.G.RX_BUFFER = buffer;
		tr.G.RX_LENGTH = /*sizeof(buffer)*/2;
		tr.DEV_REQ_PTR = (unsigned char *)(&dev_req);

		dev_req.BMREQUESTTYPE = USB_SETUP_TYPE_VENDOR | REQ_TYPE_IN;
		dev_req.BREQUEST = USB_AOA_GET_PROTOCOL_VERSION;

		*(uint16_t *)dev_req.WVALUE = HOST_TO_LE_SHORT(0);
		*(uint16_t *)dev_req.WINDEX = HOST_TO_LE_SHORT(0);
		*(uint16_t *)dev_req.WLENGTH = HOST_TO_LE_SHORT(2);
		break;
#ifdef CONNECT_TO_APK		
	case STRING_VENDOR:
		strcpy((char *)buffer,DEFAULT_ACCESSORY_STRING_VENDOR);
		tr.G.TX_BUFFER = buffer;
		//tr.G.TX_LENGTH = strlen(buffer)+1;
		tr.DEV_REQ_PTR = (unsigned char *)(&dev_req);
		
		dev_req.BMREQUESTTYPE = USB_SETUP_TYPE_VENDOR | REQ_TYPE_OUT;
		dev_req.BREQUEST = USB_AOA_SET_PROTOCOL_ZERO;

		*(uint16_t *)dev_req.WVALUE = HOST_TO_LE_SHORT(0);
		*(uint16_t *)dev_req.WINDEX = HOST_TO_LE_SHORT(0);
		*(uint16_t *)dev_req.WLENGTH = HOST_TO_LE_SHORT(strlen((char *)buffer)+1);
		tr.G.TX_LENGTH = SHORT_UNALIGNED_LE_TO_HOST(dev_req.WLENGTH);
		break;
	case STRING_NAME:
		strcpy((char *)buffer,DEFAULT_ACCESSORY_STRING_NAME);
		tr.G.TX_BUFFER = buffer;
		//tr.G.TX_LENGTH = strlen(buffer)+1;
		tr.DEV_REQ_PTR = (unsigned char *)(&dev_req);

		dev_req.BMREQUESTTYPE = USB_SETUP_TYPE_VENDOR | REQ_TYPE_OUT;
		dev_req.BREQUEST = USB_AOA_SET_PROTOCOL_ZERO;

		*(uint16_t *)dev_req.WVALUE = HOST_TO_LE_SHORT(0);
		*(uint16_t *)dev_req.WINDEX = HOST_TO_LE_SHORT(1);
		*(uint16_t *)dev_req.WLENGTH = HOST_TO_LE_SHORT(strlen((char *)buffer)+1);
		tr.G.TX_LENGTH = SHORT_UNALIGNED_LE_TO_HOST(dev_req.WLENGTH);

		break;
#endif		
	case STRING_LONGNAME:
		strcpy((char *)buffer,DEFAULT_ACCESSORY_STRING_LONGNAME);
		tr.G.TX_BUFFER = buffer;
		//tr.G.TX_LENGTH = strlen(buffer)+1;
		tr.DEV_REQ_PTR = (unsigned char *)(&dev_req);

		dev_req.BMREQUESTTYPE = USB_SETUP_TYPE_VENDOR | REQ_TYPE_OUT;
		dev_req.BREQUEST = USB_AOA_SET_PROTOCOL_ZERO;

		*(uint16_t *)dev_req.WVALUE = HOST_TO_LE_SHORT(0);
		*(uint16_t *)dev_req.WINDEX = HOST_TO_LE_SHORT(2);
		*(uint16_t *)dev_req.WLENGTH = HOST_TO_LE_SHORT(strlen((char *)buffer)+1);
		tr.G.TX_LENGTH = SHORT_UNALIGNED_LE_TO_HOST(dev_req.WLENGTH);

		break;
	case STRING_VERSION:
		strcpy((char *)buffer,DEFAULT_ACCESSORY_STRING_VERSION);
		tr.G.TX_BUFFER = buffer;
		//tr.G.TX_LENGTH = strlen(buffer)+1;
		tr.DEV_REQ_PTR = (unsigned char *)(&dev_req);

		dev_req.BMREQUESTTYPE = USB_SETUP_TYPE_VENDOR | REQ_TYPE_OUT;
		dev_req.BREQUEST = USB_AOA_SET_PROTOCOL_ZERO;

		*(uint16_t *)dev_req.WVALUE = HOST_TO_LE_SHORT(0);
		*(uint16_t *)dev_req.WINDEX = HOST_TO_LE_SHORT(3);
		*(uint16_t *)dev_req.WLENGTH = HOST_TO_LE_SHORT(strlen((char *)buffer)+1);
		tr.G.TX_LENGTH = SHORT_UNALIGNED_LE_TO_HOST(dev_req.WLENGTH);

		break;
	case STRING_URL:
		strcpy((char *)buffer,DEFAULT_ACCESSORY_STRING_URL);
		tr.G.TX_BUFFER = buffer;
		//tr.G.TX_LENGTH = strlen(buffer)+1;
		tr.DEV_REQ_PTR = (unsigned char *)(&dev_req);

		dev_req.BMREQUESTTYPE = USB_SETUP_TYPE_VENDOR | REQ_TYPE_OUT;
		dev_req.BREQUEST = USB_AOA_SET_PROTOCOL_ZERO;

		*(uint16_t *)dev_req.WVALUE = HOST_TO_LE_SHORT(0);
		*(uint16_t *)dev_req.WINDEX = HOST_TO_LE_SHORT(4);
		*(uint16_t *)dev_req.WLENGTH = HOST_TO_LE_SHORT(strlen((char *)buffer)+1);
		tr.G.TX_LENGTH = SHORT_UNALIGNED_LE_TO_HOST(dev_req.WLENGTH);

		break;
	case STRING_SERIAL:
		strcpy((char *)buffer,DEFAULT_ACCESSORY_STRING_SERIAL);
		tr.G.TX_BUFFER = buffer;
		//tr.G.TX_LENGTH = strlen(buffer)+1;
		tr.DEV_REQ_PTR = (unsigned char *)(&dev_req);

		dev_req.BMREQUESTTYPE = USB_SETUP_TYPE_VENDOR | REQ_TYPE_OUT;
		dev_req.BREQUEST = USB_AOA_SET_PROTOCOL_ZERO;

		*(uint16_t *)dev_req.WVALUE = HOST_TO_LE_SHORT(0);
		*(uint16_t *)dev_req.WINDEX = HOST_TO_LE_SHORT(5);
		*(uint16_t *)dev_req.WLENGTH = HOST_TO_LE_SHORT(strlen((char *)buffer)+1);
		tr.G.TX_LENGTH = SHORT_UNALIGNED_LE_TO_HOST(dev_req.WLENGTH);

		break;
#ifdef USE_USB_AUDIO	
	case ACCESSORY_AUDIO:
		strcpy((char *)buffer,DEFAULT_ACCESSORY_STRING_SERIAL);
		tr.G.TX_BUFFER = NULL;
		tr.G.TX_LENGTH = 0;
		tr.DEV_REQ_PTR = (unsigned char *)(&dev_req);

		dev_req.BMREQUESTTYPE = USB_SETUP_TYPE_VENDOR | REQ_TYPE_OUT;
		dev_req.BREQUEST = 0x3a;

		*(uint16_t *)dev_req.WVALUE = HOST_TO_LE_SHORT(1);
		*(uint16_t *)dev_req.WINDEX = HOST_TO_LE_SHORT(0);
		*(uint16_t *)dev_req.WLENGTH = HOST_TO_LE_SHORT(0);
		
		break;
	
#endif	   
	case ACCESSORY_START:
		strcpy((char *)buffer,DEFAULT_ACCESSORY_STRING_SERIAL);
		tr.G.TX_BUFFER = NULL;
		tr.G.TX_LENGTH = 0;
		tr.DEV_REQ_PTR = (unsigned char *)(&dev_req);

		dev_req.BMREQUESTTYPE = USB_SETUP_TYPE_VENDOR | REQ_TYPE_OUT;
		dev_req.BREQUEST = 0x35;

		*(uint16_t *)dev_req.WVALUE = HOST_TO_LE_SHORT(0);
		*(uint16_t *)dev_req.WINDEX = HOST_TO_LE_SHORT(0);
		*(uint16_t *)dev_req.WLENGTH = HOST_TO_LE_SHORT(0);
		break;
	default:
		printf("liutest invalid  req_status");
		break;
   	}
    
    error = _usb_host_send_setup(dev_ptr->host, pipe_handle, &tr);
    //printf("send try acc %d-%d\n",req_status,error);
    USB_unlock();

#ifdef _HOST_DEBUG_
    printf("liutest _usb_host_send_setup state=%d\n",error);
#endif
    
    return  USB_OK;

} /* Endbody */

#ifdef USE_ACC_HID

#ifdef TR_CALLBACK_COMPLETE
int usb_hid_callback
#else
void usb_hid_callback
#endif
   (
      /* [IN] pointer to pipe */
      _usb_pipe_handle  pipe_handle,

      /* [IN] user-defined parameter */
      pointer           user_parm,

      /* [IN] buffer address */
      unsigned char *   buffer,

      /* [IN] length of data transferred */
      uint32_t           length_data_transfered,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      USB_STATUS        status
   )
{ /* Body */
 LWEVENT_STRUCT	*	block_pevent =  (LWEVENT_STRUCT	*)user_parm;
    if(status == USB_OK) {
       printf("usb hid callback ok\n");
    }
    else
        printf("usb hid callback fail\n");
    
    _lwevent_set(block_pevent,1);
#ifdef TR_CALLBACK_COMPLETE
    return  TR_CALLBACK_COMPLETE;
#endif
}

USB_STATUS usb_device_hid_regist
   (
   _usb_device_instance_handle      dev_handle,
   _usb_interface_descriptor_handle intf_handle,
   tr_callback                callback,
   uint32_t len
   )
{ /* Body */

   DEV_INSTANCE_PTR           dev_ptr;
   _usb_pipe_handle           pipe_handle;
   TR_INIT_PARAM_STRUCT          tr;
   USB_STATUS                    error;
   USB_SETUP                     dev_req;

    LWEVENT_STRUCT		block_event;
   
   USB_lock();	
   dev_ptr = (DEV_INSTANCE_PTR)dev_handle;
   pipe_handle = dev_ptr->control_pipe;

   error = usb_hostdev_validate(dev_handle);
   if (error != USB_OK) {
#ifdef _HOST_DEBUG_
	   	 printf("liutest usb_hostdev_validate fail \n");
#endif
	    USB_unlock();
   
	 return USBERR_ERROR;
   }
   printf("liutest hid report len=%d\n",len);

    /* Initialize the TR with TR index and default control callback 
   ** function if there is one registered
   */
   usb_hostdev_tr_init (&tr, callback, /*NULL*/&block_event);
  
    //register hid
	tr.G.TX_BUFFER = NULL;
	tr.G.TX_LENGTH = 0;
	tr.DEV_REQ_PTR = (uchar_ptr)(&dev_req);

	dev_req.BMREQUESTTYPE = USB_SETUP_TYPE_VENDOR | REQ_TYPE_OUT;
	dev_req.BREQUEST = ACCESSORY_REGISTER_HID;

	*(uint16_t *)dev_req.WVALUE = HOST_TO_LE_SHORT(0);
	*(uint16_t *)dev_req.WINDEX = HOST_TO_LE_SHORT(len);
	*(uint16_t *)dev_req.WLENGTH = HOST_TO_LE_SHORT(0);
	printf("HID\n");

       if (MQX_OK !=_lwevent_create(&(block_event), LWEVENT_AUTO_CLEAR)) {
		printf("\n_lwevent_create hid block_event failed\n");
         USB_unlock();
		return USBERR_ERROR;
	}
       
	error = _usb_host_send_setup(dev_ptr->host, pipe_handle, &tr);
	
    USB_unlock();

     	if (_lwevent_wait_ticks(&block_event,1, FALSE, 500) != MQX_OK) {
			printf("waiting block_eventfail\n");
             _lwevent_destroy(&block_event);
			return USBERR_ERROR;
	}
        _lwevent_destroy(&block_event);

       printf("liutest hid register end\n");
    return  USB_OK;
} /* Endbody */

USB_STATUS usb_device_hid_unregist
   (
   _usb_device_instance_handle      dev_handle,
   _usb_interface_descriptor_handle intf_handle,
   tr_callback                callback
   )
{ /* Body */

   DEV_INSTANCE_PTR           dev_ptr;
   _usb_pipe_handle           pipe_handle;
   TR_INIT_PARAM_STRUCT          tr;
   USB_STATUS                    error;
   USB_SETUP                     dev_req;

   // todo add block event
   
   USB_lock();	
   dev_ptr = (DEV_INSTANCE_PTR)dev_handle;
   pipe_handle = dev_ptr->control_pipe;

   error = usb_hostdev_validate(dev_handle);

   if (error != USB_OK) {
#ifdef _HOST_DEBUG_
	   	 printf("liutest usb_hostdev_validate fail \n");
#endif
	   
	 return USBERR_ERROR;
   }

    /* Initialize the TR with TR index and default control callback 
   ** function if there is one registered
   */
   usb_hostdev_tr_init (&tr, callback, NULL);
  
    //register hid
	tr.G.TX_BUFFER = NULL;
	tr.G.TX_LENGTH = 0;
	tr.DEV_REQ_PTR = (uchar_ptr)(&dev_req);

	dev_req.BMREQUESTTYPE = USB_SETUP_TYPE_VENDOR | REQ_TYPE_OUT;
	dev_req.BREQUEST = ACCESSORY_UNREGISTER_HID;

	*(uint16_t *)dev_req.WVALUE = HOST_TO_LE_SHORT(0);
	*(uint16_t *)dev_req.WINDEX = HOST_TO_LE_SHORT(0);
	*(uint16_t *)dev_req.WLENGTH = HOST_TO_LE_SHORT(0);
	printf("HID\n");
	error = _usb_host_send_setup(dev_ptr->host, pipe_handle, &tr);
	
    USB_unlock();
    
    return  USB_OK;
}

USB_STATUS usb_device_hid_set_report_descriptor
   (
   _usb_device_instance_handle      dev_handle,
   _usb_interface_descriptor_handle intf_handle,
   tr_callback                callback
   )
{ /* Body */

   DEV_INSTANCE_PTR           dev_ptr;
   _usb_pipe_handle           pipe_handle;
   
   TR_INIT_PARAM_STRUCT          tr;
   USB_STATUS                    error;
   USB_SETUP                     dev_req;

    LWEVENT_STRUCT		block_event;
   

   uint32_t index = 0;
   uint32_t remain = sizeof(hid_description);
   uint32_t datalen;
   uint8_t * hid_report_desc = (uint8_t *)hid_description;

       if (MQX_OK !=_lwevent_create(&(block_event), LWEVENT_AUTO_CLEAR)) {
		printf("\n_lwevent_create hid block_event failed\n");
		return USBERR_ERROR;
	}
   
   USB_lock();	
   dev_ptr = (DEV_INSTANCE_PTR)dev_handle;
   pipe_handle = dev_ptr->control_pipe;

   error = usb_hostdev_validate(dev_handle);

   if (error != USB_OK) {
#ifdef _HOST_DEBUG_
	   	 printf("liutest usb_hostdev_validate fail \n");
#endif
	    _lwevent_destroy(&block_event);
        USB_unlock();
	 return USBERR_ERROR;
   }

    /* Initialize the TR with TR index and default control callback 
   ** function if there is one registered
   */
   usb_hostdev_tr_init (&tr, callback,/* NULL*/&block_event);
   
   while(remain > 0) {
	   if(PKT_MAX_SIZE < remain) {
		   datalen = PKT_MAX_SIZE;
	   }
	   else {
		   datalen = remain;
	   }
	
		tr.G.TX_BUFFER = hid_report_desc;
		//tr.G.TX_LENGTH = strlen(buffer)+1;
		tr.DEV_REQ_PTR = (uchar_ptr)(&dev_req);

		dev_req.BMREQUESTTYPE = USB_SETUP_TYPE_VENDOR | REQ_TYPE_OUT;
		dev_req.BREQUEST = ACCESSORY_SET_HID_REPORT_DESC;

		*(uint16_t *)dev_req.WVALUE = HOST_TO_LE_SHORT(0);
		*(uint16_t *)dev_req.WINDEX = HOST_TO_LE_SHORT(index);
		*(uint16_t *)dev_req.WLENGTH = HOST_TO_LE_SHORT(datalen);
		tr.G.TX_LENGTH = SHORT_UNALIGNED_LE_TO_HOST(dev_req.WLENGTH);
	 
	    _usb_host_send_setup(dev_ptr->host, pipe_handle, &tr);
        
        USB_unlock();

      	if (_lwevent_wait_ticks(&block_event,1, FALSE, 500) != MQX_OK) {
			printf("waiting block_eventfail\n");
             _lwevent_destroy(&block_event);
			return USBERR_ERROR;
	}
         USB_lock();
	    
	    remain -= datalen;
	    hid_report_desc += datalen;
	    index++;
   }


    USB_unlock();
    printf("HID report end\n");
   _lwevent_destroy(&block_event);
    
    return  USB_OK;
} /* Endbody */

USB_STATUS usb_device_hid_send_event
   (
   _usb_device_instance_handle      dev_handle,
   _usb_interface_descriptor_handle intf_handle,
   tr_callback                callback,
   uint32_t cmd
   )
{ /* Body */
   DEV_INSTANCE_PTR           dev_ptr;
   _usb_pipe_handle           pipe_handle;
   
   TR_INIT_PARAM_STRUCT          tr;
   USB_STATUS                    error;
   USB_SETUP                     dev_req;

   LWEVENT_STRUCT		block_event;
   
   USB_lock();	
   dev_ptr = (DEV_INSTANCE_PTR)dev_handle;
   pipe_handle = dev_ptr->control_pipe;

   error = usb_hostdev_validate(dev_handle);

   if (error != USB_OK) {
#ifdef _HOST_DEBUG_
	   	 printf("liutest usb_hostdev_validate fail \n");
#endif
  USB_unlock();
	 return USBERR_ERROR;
   }

    /* Initialize the TR with TR index and default control callback 
   ** function if there is one registered
   */
   usb_hostdev_tr_init (&tr, callback,  /* NULL */ &block_event );
 
  
    buffer[0] = cmd;
	buffer[1] = 0x00;
	buffer[2] = 0x00;
	buffer[3] = 0x00;
	buffer[4] = 0x00;
	buffer[5] = 0x00;
	buffer[6] = 0x00;
	buffer[7] = 0x00;
   			
	tr.G.TX_BUFFER = buffer;
	//tr.G.TX_LENGTH = strlen(buffer)+1;
	tr.DEV_REQ_PTR = (uchar_ptr)(&dev_req);

	dev_req.BMREQUESTTYPE = USB_SETUP_TYPE_VENDOR | REQ_TYPE_OUT;
	dev_req.BREQUEST = ACCESSORY_SEND_HID_EVENT;

	*(uint16_t *)dev_req.WVALUE = HOST_TO_LE_SHORT(0);
	*(uint16_t *)dev_req.WINDEX = HOST_TO_LE_SHORT(0);
	*(uint16_t *)dev_req.WLENGTH = HOST_TO_LE_SHORT(/*1*/8);
	tr.G.TX_LENGTH = SHORT_UNALIGNED_LE_TO_HOST(dev_req.WLENGTH);
       
    	if (MQX_OK !=_lwevent_create(&(block_event), LWEVENT_AUTO_CLEAR)) {
		printf("\n_lwevent_create hid block_event failed\n");
		return USBERR_ERROR;
	}
	
	_usb_host_send_setup(dev_ptr->host, pipe_handle, &tr);
        USB_unlock();
        
    	if (_lwevent_wait_ticks(&block_event,1, FALSE, 500) != MQX_OK) {
			printf("waiting block_eventfail\n");
             _lwevent_destroy(&block_event);
			return USBERR_ERROR;
	}
        _lwevent_destroy(&block_event);
    
    return  USB_OK;
} /* Endbody */
#endif

void usb_aoa_init
   (
      /* [IN] device/descriptor/pipe handles */
      PIPE_BUNDLE_STRUCT_PTR  pbs_ptr,
      
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR   aoa_call_ptr
   )
{ /* Body */

   AOA_INTERFACE_STRUCT_PTR  aoa_intf;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_aoa_init");
   #endif

   printf("liutest usb_aoa_init+++\n");   
   /* Pointer validity-checking, clear memory, init header */
   aoa_intf = aoa_call_ptr->class_intf_handle;
   if (USB_OK != usb_host_class_intf_init (pbs_ptr, (void *)aoa_intf, (void *)&aoa_anchor, NULL))
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_aoa_init, error class init");
      #endif
      return;
   }

   USB_lock();
   aoa_call_ptr->code_key = 0;
   aoa_call_ptr->code_key = usb_host_class_intf_validate(aoa_call_ptr);

#if 0   /* liutest ,dose not need control pipe for accessory*/
   if (NULL == 
         (aoa_intf->control_pipe = usb_hostdev_get_pipe_handle        
         (pbs_ptr, USB_CONTROL_PIPE, 0) ))
      goto Bad_Exit;
#endif

   if (NULL == 
         (aoa_intf->bulk_out_pipe = usb_hostdev_get_pipe_handle
         (pbs_ptr, USB_BULK_PIPE, USB_SEND) ))
      goto Bad_Exit;

   if (NULL ==
            (aoa_intf->bulk_in_pipe = usb_hostdev_get_pipe_handle 
            (pbs_ptr, USB_BULK_PIPE, USB_RECV) ))
         goto Bad_Exit;
 

   USB_unlock ();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_aoa_init, SUCCESSFUL");
   #endif
   printf("liutest usb_aoa_init---\n");
   
   return;  /* Good exit, interface struct initialized */

   Bad_Exit:
   USB_mem_zero (aoa_intf,sizeof(AOA_INTERFACE_STRUCT_PTR));
   aoa_call_ptr->class_intf_handle = NULL;
   aoa_call_ptr->code_key = 0;
   USB_unlock ();
   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_aoa_init, bad exit");
   #endif
   
} /* Endbody */


USB_STATUS usb_aoa_recv_data 
   (
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR   cc_ptr,
      
      /* [IN] callback upon completion */
      tr_callback             callback,
      
      /* [IN] user parameter returned by callback */
      void *                 call_parm,
      
      /* [IN] data length */
      uint32_t                 buf_size,
      
      /* [IN] buffer pointer */
      unsigned char *   buffer
   )
{ /* Body */

   TR_INIT_PARAM_STRUCT    tr;
   USB_STATUS              error;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_aoa_recv_data");
   #endif

   if (USB_OK != (error = usb_aoa_class_verify 
         (cc_ptr, &tr, callback, call_parm)))
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_aoa_recv_data, error verifying class");
      #endif
      return error;

   }

   tr.G.RX_BUFFER = buffer;
   tr.G.RX_LENGTH = buf_size;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_aoa_recv_data, SUCCESSFUL");
   #endif
   return usb_aoa_bulk_inout (cc_ptr, &tr, FALSE);   

} /* Endbody */
                  

USB_STATUS usb_aoa_send_data
   (
      /* [IN] class-interface data pointer + key */
      CLASS_CALL_STRUCT_PTR   cc_ptr,
      
      /* [IN] callback upon completion */
      tr_callback             callback,
      
      /* [IN] user parameter returned by callback */
      void *                call_parm,
      
      /* [IN] data length */
      uint32_t          buf_size,
      
      /* [IN] buffer pointer */
      unsigned char *   buffer
   )
{ /* Body */

  TR_INIT_PARAM_STRUCT  tr;
  USB_STATUS            error;

  #ifdef _HOST_DEBUG_
    DEBUG_LOG_TRACE("usb_aoa_send_data");
  #endif

   if (USB_OK != (error = usb_aoa_class_verify 
         (cc_ptr, &tr, callback, call_parm)))
   {
      #ifdef _HOST_DEBUG_
         DEBUG_LOG_TRACE("usb_aoa_send_data, error verifying class");
      #endif
      return error;
   }

   tr.G.TX_BUFFER = buffer;
   tr.G.TX_LENGTH = buf_size;

   #ifdef _HOST_DEBUG_
      DEBUG_LOG_TRACE("usb_aoa_send_data, SUCCESSFUL");
   #endif

   return usb_aoa_bulk_inout (cc_ptr, &tr, TRUE);   

} /* Endbody */
  
/* EOF */
