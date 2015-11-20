/**HEADER********************************************************************
 * 
 * Copyright (c) 2014 Freescale Semiconductor;
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
 * Comments:
 *
 *END************************************************************************/

/**************************************************************************
  Include the USB stack header files.
 **************************************************************************/
#include <mqx.h>
#include <bsp.h>


#include "usb.h"
#include "usb_prv.h"


#include "host_dev_list.h"
#include "host_common.h"

#include <host_main.h>
#include <host_shut.h>
#include "usb_types.h"
#include "hostapi.h"

/**************************************************************************
  Local header files for this application
 **************************************************************************/
#include "usb_test.h"

static void usb_host_test_device_event (_usb_device_instance_handle dev_handle, _usb_interface_descriptor_handle intf_handle, uint_32 event_code);

static LWEVENT_STRUCT usb_event;

/**************************************************************************
  Global variables
 **************************************************************************/



/* Table of driver capabilities this application wants to use */
static  USB_HOST_DRIVER_INFO DriverInfoTable[] =
{
    /* USB 2.0 hard drive */
    {
        {0xFF,0xFF}, 
        {0xFF,0xFF}, 
        0xFF, 
        0xFF, 
        0xFF, 
        0, 
        usb_host_test_device_event
    },
    {
        {0x00,0x00},                  /* All-zero entry terminates        */
        {0x00,0x00},                  /* driver info list.                */
        0,
        0,
        0,
        0,
        0
    }
};

/**************************************************************************
  Global variables
 **************************************************************************/
int unit_test_usb_host(void){

    static _usb_host_handle host_handle;  
    USB_STATUS  error;

    USB_lock();

    _int_install_unexpected_isr();



    if (MQX_OK != _usb_host_driver_install(USBCFG_DEFAULT_HOST_CONTROLLER)) {
        printf("\n Driver installation failed");
        _task_block();
    }

    error = _usb_host_init(USBCFG_DEFAULT_HOST_CONTROLLER, &host_handle);
    if (error == USB_OK) {
        error = _usb_host_driver_info_register(host_handle, (pointer)DriverInfoTable);
        if (error == USB_OK) {
            error = _usb_host_register_service(host_handle, USB_SERVICE_HOST_RESUME, NULL);
        }
    }

    USB_unlock();

    if (error != USB_OK) {
        _task_block();
    }

    if (MQX_OK != _lwevent_create(&usb_event, LWEVENT_AUTO_CLEAR)) {
        return -1;
    }

    /* prepare events to be auto or manual */
    _lwevent_set_auto_clear(&usb_event, 0xFFFFFFFF);
    /* pre-set events */
    _lwevent_clear(&usb_event, 0xFFFFFFFF);

    _lwevent_wait_ticks(&usb_event, 0x1, FALSE, 0);

    return 0;
}

/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_test_device_event
 * Returned Value : None
 * Comments       :
 *END*--------------------------------------------------------------------*/
static void usb_host_test_device_event     
(
 /* [IN] pointer to device instance */
 _usb_device_instance_handle      dev_handle,

 /* [IN] pointer to interface descriptor */
 _usb_interface_descriptor_handle intf_handle,

 /* [IN] code number for event causing callback */
 uint_32           event_code
 )
{

    switch (event_code) {
        case USB_ATTACH_EVENT:
        case USB_CONFIG_EVENT:
        case USB_INTF_EVENT:
        case USB_DETACH_EVENT:
        default:
            _lwevent_set(&usb_event, 0x1); 
            break;
    } 
} 

/* end file */

