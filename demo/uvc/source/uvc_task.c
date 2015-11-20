/**HEADER********************************************************************
* 
* Copyright (c) 2015 Freescale Semiconductor;
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
*END************************************************************************/

#include "main.h"
#include "usb_classes.h"
#include <lwmsgq.h>

#include "74hc595.h"
#include "uvc_task.h"

#define QCA4002_DEBUG                  1
#if QCA4002_DEBUG                  
#define _UVCNET_PERFORMANCE_TEST       1
#define QCA4002_USE_UDP                1
#define QCA4002_USE_TCP                0
#endif

#if QCA4002_DEBUG
#include "qca4002.h"
#endif


static void usb_host_uvc_event(_usb_device_instance_handle, _usb_interface_descriptor_handle intf_handle, uint_32);


typedef struct {
    CLASS_CALL_STRUCT_PTR ccs;     /* class call struct of MSD instance */
    uint_8                body;    /* message body one of USB_EVENT_xxx as defined above */
} usb_msg_t;

#define USB_EVENT_ATTACH    (1)
#define USB_EVENT_DETACH    (2)
#define USB_EVENT_INTF      (3)


LWSEM_STRUCT   USB_Stick;

/* The granularity of message queue is one message. Its size is the multiplier of _mqx_max_type. Get that multiplier */
#define USB_TASKQ_GRANM ((sizeof(usb_msg_t) - 1) / sizeof(_mqx_max_type) + 1)
static _mqx_max_type  usb_taskq[20 * USB_TASKQ_GRANM * sizeof(_mqx_max_type)]; /* prepare message queue for 20 events */

/************************************************************************************
Table of driver capabilities this application wants to use. See Host API document for
details on How to define a driver info table.
************************************************************************************/

static USB_HOST_DRIVER_INFO DriverInfoTable[] =
{
#ifdef USBCLASS_INC_VIDEO
    {
        {0x00, 0x00},                 /* Vendor ID per USB-IF             */
        {0x00, 0x00},                 /* Product ID per manufacturer      */
        CC_VIDEO,                     /* Class code                       */
        SC_VIDEOCONTROL,              /* Sub-Class code                   */
        0x0,                          /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_uvc_event            /* Application call back function   */
    },
#endif

    {
        {0x00, 0x00},                 /* Vendor ID per USB-IF             */
        {0x00, 0x00},                 /* Product ID per manufacturer      */
        0,                            /* Class code                       */
        0,                            /* Sub-Class code                   */
        0,                            /* Protocol                         */
        0,                            /* Reserved                         */
        NULL                          /* Application call back function   */
    }
};

#if QCA4002_DEBUG
static int udp_test(void* handle, uint32_t socket, void *address, uint32_t addresslen){

    uint32_t loops = -1;
    uint32_t loop  = 0;
    uint8_t *buf   = NULL;
    int      ret   = 0;

    while((buf = (void *)CUSTOM_ALLOC(1024)) == NULL) {
        _time_delay(1);
    }

    for(loop = 0; loop < loops; loop++){

        *((uint32_t *)&buf[0]) = loop;

        ret = t_sendto((void*)handle, socket, 
                (unsigned char*)buf, 1024, 0,
                (void *)address, addresslen);
        if(ret != 1024){
            printf("UDP test send failed \n");
            break;
        }

        _time_delay(1);
    }

    return ret;
}
#endif


/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_uvc_event
* Returned Value : None
* Comments       :
*     Called when a device is attached / detached, but not recognized by
*     any class driver.
*END*--------------------------------------------------------------------*/

static void usb_host_uvc_event
   (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint_32                          event_code
   )
{
   UVC_DEVICE_STRUCT_PTR          device;
   usb_msg_t                      msg;
   INTERFACE_DESCRIPTOR_PTR       intf;

   intf = (INTERFACE_DESCRIPTOR_PTR)intf_handle;

   if(intf->bInterfaceNumber != 0)
       /* only handle interface 0!*/
       return;

   switch (event_code) {
      case USB_ATTACH_EVENT:
          break;

           /* only after got CONFIG_EVENT we do select interface */
      case USB_CONFIG_EVENT:


         /* Here, the device starts its lifetime */
         device = (UVC_DEVICE_STRUCT_PTR) _mem_alloc_zero(sizeof(UVC_DEVICE_STRUCT));
         if (device == NULL)
            break;

         if (MQX_OK != _lwevent_create(&device->uvc_event, LWEVENT_AUTO_CLEAR)) {
             printf("\n USB BT device lwevent create failed \n");
             break;
         }

         if (USB_OK != _usb_hostdev_select_interface(dev_handle, intf_handle, &device->ccs))
            break;
         msg.ccs = &device->ccs;
         msg.body = USB_EVENT_ATTACH;
         if (LWMSGQ_FULL == _lwmsgq_send(usb_taskq, (uint_32 *) &msg, 0)) {
            printf("Could not inform USB task about device attached \n");
         }
         break;

      case USB_INTF_EVENT:
         if (USB_OK != usb_class_uvc_get_app(dev_handle, intf_handle, (CLASS_CALL_STRUCT_PTR *) &device)) {
            printf("usb_class_uvc_get_app failed ! \n");
            break;
         }

         if (MQX_OK != usb_class_uvc_use_lwevent(&device->ccs, &device->uvc_event)) {
            printf("usb_class_uvc_use_lwevent failed ! \n");
             break;
         }

         if (MQX_OK != usb_class_uvc_use_lwmsgq(&device->ccs, device->uvc_taskq)) {
            printf("usb_class_uvc_use_lwmsgq failed ! \n");
             break;
         }

         msg.ccs = &device->ccs;
         msg.body = USB_EVENT_INTF;
         if (LWMSGQ_FULL == _lwmsgq_send(usb_taskq, (uint_32 *) &msg, 0)) {
            printf("Could not inform USB task about device interfaced\n");
         }
         break;

      case USB_DETACH_EVENT:
         if (USB_OK != usb_class_uvc_get_app(dev_handle, intf_handle, (CLASS_CALL_STRUCT_PTR *) &device))
            break;

         printf("USB_DETACH_EVENT \n");

         //usb_class_uvc_uninstall_driver(&device->ccs);

         msg.ccs = &device->ccs;
         msg.body = USB_EVENT_DETACH;

         if (LWMSGQ_FULL == _lwmsgq_send(usb_taskq, (uint_32 *) &msg, 0)) {
            printf("Could not inform USB task about device detached\n");
         }
         //_mem_free(device);
         break;

      default:
         break;
   } 
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : USB_uvc_task
* Returned Value : None
* Comments       :
*     First function called. This rouine just transfers control to host main
*END*--------------------------------------------------------------------*/

void UVC_task(uint_32 param)
{ 
    USB_STATUS           error;
    usb_msg_t            msg;
    int                  udp = 1;
    /* Store mounting point used. A: is the first one, bit #0 assigned, Z: is the last one, bit #25 assigned */

    /* This event will inform other tasks that the filesystem on USB was successfully installed */
    _lwsem_create(&USB_Stick, 0);
    
    if (MQX_OK != _lwmsgq_init(usb_taskq, 20, USB_TASKQ_GRANM)) {
        // lwmsgq_init failed
        _task_block();
    }


#if QCA4002_DEBUG
    int   peersocket;
    SOCKADDR_T foreign_addr;
    A_UINT32  ip_address;


#if QCA4002_USE_UDP
    udp = 1;
#endif
#if QCA4002_USE_TCP
    udp = 0;
#endif


    mux_74hc595_clear_bit(BSP_74HC595_0,  BSP_74HC595_AMB_LED);

    /* init qca4002 */
    atheros_driver_setup_init();
    HVAC_initialize_networking();
    qca4002_connect("archLinux64", "none", "");
    //qca4002_connect("archLinux64", "wpa2", "haidong2014");
    //qca4002_connect("Tenda", "wpa2", "1122334455");
    //qca4002_connect("TPWEB", "none", "");


    while(!(qca4002_is_connect())){
        _time_delay(300);
    }

    qca4002_ipconfig_static("192.168.12.90", "255.255.255.0", "192.168.12.1");
    //qca4002_ipconfig_static("192.168.1.90", "255.255.255.0", "192.168.1.1");


    if((peersocket = t_socket((void*)handle, ATH_AF_INET, udp ? SOCK_DGRAM_TYPE : SOCK_STREAM_TYPE , 0)) == A_ERROR){
        printf("t_socket create failed \n");
        return;
    }


    ath_inet_aton("192.168.12.1", (A_UINT32*)&ip_address);
    //ath_inet_aton("192.168.1.103", (A_UINT32*)&ip_address);

    memset(&foreign_addr, 0, sizeof(foreign_addr));
    foreign_addr.sin_addr   = ip_address;
    foreign_addr.sin_port   = 1234;
    foreign_addr.sin_family = ATH_AF_INET;

    if(t_connect((void*)handle, peersocket, (&foreign_addr), sizeof(foreign_addr)) == A_ERROR){
        printf("t_socket connect failed \n");
        return;
    }
    else{
        mux_74hc595_set_bit(BSP_74HC595_0,  BSP_74HC595_AMB_LED);
        printf("Connect to 192.168.12.1\n");
    }

   // udp_test(handle, peersocket, &foreign_addr, sizeof(foreign_addr));

#endif
   



    for (;;) {
        /* Wait for event sent as a message */
        _lwmsgq_receive(&usb_taskq, (_mqx_max_type *) &msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
         
        printf("uvc msg.body is %d \n", msg.body);

        if (msg.body == USB_EVENT_ATTACH) {
          /* This event is not so important, because it does not inform about successfull USB stack enumeration */
        } else if (msg.body == USB_EVENT_INTF )  { 
            int_32  error = MQX_OK;

            error =  usb_class_video_install_driver(msg.ccs, "UVC:");
            if(error == MQX_OK) {
                MQX_FILE_PTR _handle = NULL;
                UVC_DEVICE_STRUCT_PTR dsp = (UVC_DEVICE_STRUCT_PTR) msg.ccs;

                _handle = fopen("UVC:", 0);
                
                if(_handle != NULL) {
                    struct   video_pix_format fmt;
                    int      ret;
                    uint8_t *fcc;

                    memset(&fmt, 0, sizeof(fmt));
                    fmt.width         = 1280;
                    fmt.height        = 720;
                    //fmt.width         = 640;
                    //fmt.height        = 480;
                    //fmt.pixelformat   = UVC_PIX_FMT_MJPEG;   
                    fmt.pixelformat   = UVC_PIX_FMT_H264;
                    //fmt.interval      = 2000000; /* 100ns unit, 5 frames/s */ 
                    //fmt.interval      = 400000;  /* 25 fames */
                    //fmt.interval      = 500000;  /* 20 fames */
                    //fmt.interval       = 1000000;/* 10 frames */

                    ret = ioctl(_handle, IO_IOCTL_UVC_VIDEO_SET_FORMAT, &fmt);
                    if(ret >= 0){

                        fcc = (void *)&fmt.pixelformat;

                        printf("UVC video pix format set as\n"
                                "width,height %d,%d \n"
                                "pixelformat 0x%x(%c%c%c%c) \n"
                                "sizeimage %d bytes \n"
                                "interval %d \n"
                                "max payload size %d \n",
                                fmt.width, fmt.height,
                                fmt.pixelformat, fcc[0],fcc[1],fcc[2],fcc[3],
                                fmt.sizeimage,
                                fmt.interval,
                                fmt.maxpayloadsize
                                );
                    }else{
                        printf("UVC set format failed \n");
                    }

                    ret = ioctl(_handle, IO_IOCTL_UVC_ENABLE_VIDEO, NULL);
                    if(ret < 0){
                        printf("UVC enable VIDEO failed \n");
                    }
                    else{ 
                        UVCPACKET pkt;
                        uint32_t  sendlen = fmt.maxpayloadsize * 2 > 1000 ? 1000 : fmt.maxpayloadsize * 2;
#if QCA4002_DEBUG 
                        while((pkt.data = (void *)CUSTOM_ALLOC(sendlen)) == NULL) {
                            //Wait till we get a buffer
                            /*Allow small delay to allow other thread to run*/
                            _time_delay(1);
                        }
#else
                        pkt.data = _mem_alloc_system(sendlen);
#endif

                        if(pkt.data != NULL){
                            uint32_t loop = 0;
                            #ifdef _UVCNET_PERFORMANCE_TEST
                            TIME_STRUCT ptimeS, ptimeE, ptimeD;
                            int         pstartP   = 1;
                            uint32_t    psendlen = 0;
                            #endif
                            do{
                                ret = read(_handle, &pkt, sendlen);
#if QCA4002_DEBUG 

                               #ifdef _UVCNET_PERFORMANCE_TEST
                                if(pstartP == 1){
                                    _time_get(&ptimeS);
                                    pstartP = 0;
                                }
                               #endif
                                if((ret > 0) && (pkt.usb_help.type == UVC_PKT_DATA)){
                                    if(udp)
                                    ret = t_sendto((void*)handle, peersocket, 
                                            (unsigned char*)pkt.data, ret, 0,
                                            (void *)(&foreign_addr), sizeof(foreign_addr));
                                    else
                                        ret = t_send((void*)handle, peersocket, 
                                                (unsigned char*)pkt.data, ret, 0);
                                #ifdef _UVCNET_PERFORMANCE_TEST
                                    psendlen += ret;
                                #endif
                                    //printf("udp send %d bytes \n", ret);
                                }

                                #ifdef _UVCNET_PERFORMANCE_TEST
                                    _time_get(&ptimeE);
                                    _time_diff(&ptimeS, &ptimeE, &ptimeD);
                                    if((ptimeD.SECONDS * 1000 + ptimeD.MILLISECONDS) > 2000){
                                        printf("UVCNET performance %s %d kbps \r", udp ? "UDP" : "TCP", (psendlen * 8 / (ptimeD.SECONDS * 1000 + ptimeD.MILLISECONDS)));
                                        pstartP  = 1;
                                        psendlen = 0;
                                    }

                                #endif

#endif
                            }while(ret > 0);
                        }
                    }
                }
                else{
                    printf("Open UVC: failed! \n");
                }
                _lwsem_post(&USB_Stick);
            }
            else{
                printf("usb_class_uvc_install_driver failed error is %d \n", error);
            }

        } else if (msg.body == USB_EVENT_DETACH) {
            UVC_DEVICE_STRUCT_PTR dsp = (UVC_DEVICE_STRUCT_PTR) msg.ccs;

            _lwevent_destroy(&dsp->uvc_event);
            /* lw msg queue need destory ? */
            //memset(dsp->uvc_taskq, 0, sizeof dsp->uvc_taskq);
            _lwmsgq_deinit(dsp->uvc_taskq);
            _lwsem_wait(&USB_Stick);
            /* Here, the device finishes its lifetime */            
            _mem_free(dsp);
        }
    }
}


int UVCRegisterDriverInfoTable(USB_HOST_DRIVER_INFO *infotable, int size) {
    int tablesize = sizeof (DriverInfoTable) / sizeof(DriverInfoTable[0]);

    if((tablesize == 0) || (tablesize > size)){
        printf("USB uvc driver info size is too large:%d more than globe avaiable size %d \n", tablesize, size);
        return 0;
    }

    memcpy(infotable, DriverInfoTable, (tablesize - 1) * sizeof(DriverInfoTable[0]));

    return tablesize - 1;
}


