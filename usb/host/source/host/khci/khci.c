/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
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
* $FileName: khci.c$
* $Version : 3.8.86.0$
* $Date    : Sep-24-2012$
*
* Comments:
*
*   This file contains KHCI-specific implementations of USB interfaces
*
*END************************************************************************/
#include <mqx.h>
#include <lwevent.h>
#include <lwmsgq.h>

#include "usb.h"
#include "usb_prv.h"

#include "host_dev_list.h"


#include "mqx_host.h"
#include "khci.h"
#include "khci_prv.h"

#ifdef KHCI_DEBUG
struct debug_messaging {
  char inout;
  TR_MSG_TYPE type;
  uint_8 ep;
  uint_16 size;
};
volatile static uint_16 dm_index = 0;
volatile static struct debug_messaging dm[1024] = { 0 }; /* note, the array is for 1024 records only */
#   define KHCI_DEBUG_LOG(a, b, c, d) \
  { \
    dm[dm_index].inout = a; \
      dm[dm_index].type = b; \
      dm[dm_index].ep = c; \
      dm[dm_index].size = d; \
      dm_index++; \
  }
#else
#   define KHCI_DEBUG_LOG(a, b, c, d) {}
#endif

// KHCI task parameters
#define USB_KHCI_TASK_NUM_MESSAGES         16
#define USB_KHCI_TASK_TEMPLATE_INDEX       0
//#define USB_KHCI_TASK_ADDRESS              _usb_khci_task
#define USB_KHCI_TASK_ADDRESS              _usb_khci_schedule_task
#define USB_KHCI_TASK_STACKSIZE            2500
#define USB_KHCI_TASK_NAME                 "KHCI Task"
#define USB_KHCI_TASK_ATTRIBUTES           0
#define USB_KHCI_TASK_CREATION_PARAMETER   0
#define USB_KHCI_TASK_DEFAULT_TIME_SLICE   0

// atom transaction error results
#define KHCI_ATOM_TR_PID_ERROR  (-1)
#define KHCI_ATOM_TR_EOF_ERROR  (-2)
#define KHCI_ATOM_TR_CRC_ERROR  (-4)
#define KHCI_ATOM_TR_TO         (-16)
#define KHCI_ATOM_TR_DMA_ERROR  (-32)
#define KHCI_ATOM_TR_BTS_ERROR  (-128)
#define KHCI_ATOM_TR_NAK        (-256)
#define KHCI_ATOM_TR_DATA_ERROR (-512)
#define KHCI_ATOM_TR_STALL      (-1024)
#define KHCI_ATOM_TR_RESET      (-2048)
#define KHCI_ATOM_TR_SOF_TOK    (-4096)

#define KHCI_ATOM_TR_CANCEL            (-8192)
#define KHCI_ATOM_TR_ALLOC_MEM_ERROR   (-16384)



#define USB_KHCI_MAX_SPEED_DETECTION_COUNT 3

#if defined( __ICCCF__ ) || defined( __ICCARM__ )
    #pragma segment="USB_BDT_Z"
    #pragma data_alignment=512
    __no_init static uint_8 bdt[512] @ "USB_BDT_Z";
#elif defined (__CC_ARM) || defined(__GNUC__)
    __attribute__((section(".usb_bdt"),unused)) static uint_8 bdt[512] = { 1 };
#else
  #pragma define_section usb_bdt ".usb_bdt" RW
  __declspec(usb_bdt) static uint_8 bdt[512] = { 1 };    // DO NOT REMOVE INITIALIZATION !!! bug in CW (cw 7.1) - generate wrong binary code, data
#endif  

/* Prototypes of functions */
static USB_STATUS _usb_khci_preinit(_usb_host_handle *handle);
static USB_STATUS _usb_khci_init(_usb_host_handle handle);
static USB_STATUS _usb_khci_shutdown(_usb_host_handle handle);
static USB_STATUS _usb_khci_send(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr);
static USB_STATUS _usb_khci_send_setup(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr);
static USB_STATUS _usb_khci_recv(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr);
static USB_STATUS _usb_khci_cancel(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr);
static USB_STATUS _usb_khci_bus_control(_usb_host_handle handle, uint_8 bus_control);
static uint_32 _usb_khci_get_frame_number(_usb_host_handle handle);

static USB_STATUS _usb_khci_host_close_pipe(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr);

static void _usb_khci_process_tr_complete(KHCI_PIPE_STRUCT_PTR pipe_ptr, KHCI_TR_STRUCT_PTR tr_ptr, uint_32 required, uint_32 remain, int_32 err);
static int_32 _usb_khci_atom_tr(USB_KHCI_HOST_STATE_STRUCT_PTR host_ptr, uint_32 type, KHCI_PIPE_STRUCT_PTR pipe_desc_ptr, uint_8 *buf, uint_32 len);

const struct usb_host_callback_functions_struct _usb_khci_host_callback_table = {
   /* The Host/Device preinit function */
   _usb_khci_preinit,

  /* The Host/Device init function */
   _usb_khci_init,

   /* The function to shutdown the host/device */
   _usb_khci_shutdown,

   /* The function to send data */
   _usb_khci_send,

   /* The function to send setup data */
   _usb_khci_send_setup,

   /* The function to receive data */
   _usb_khci_recv,
   
   /* The function to cancel the transfer */
   _usb_khci_cancel,
   
   /* The function for USB bus control */
   _usb_khci_bus_control,

   NULL,

   /* The function to close pipe */
   _usb_khci_host_close_pipe,
    
   /* Get frame number function */
   _usb_khci_get_frame_number,

   /* Get microframe number function - not applicable on FS */
   NULL,
   
   /* Open pipe function */
   NULL,
   
   NULL,
   
   NULL
};

// KHCI event bits
#define KHCI_EVENT_ATTACH             0x01
#define KHCI_EVENT_RESET              0x02
#define KHCI_EVENT_TOK_DONE           0x04
#define KHCI_EVENT_SOF_TOK            0x08
#define KHCI_EVENT_DETACH             0x10
#define KHCI_EVENT_MASK               0x1f
#define KHCI_EVENT_WITHOUTSOF_MASK    0x17

#define KHCI_EVENT_TR_ARRIVE          0x20

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_isr
*  Returned Value : None
*  Comments       :
*        Service all the interrupts in the kirin usb hardware
*END*-----------------------------------------------------------------*/
static void _usb_khci_isr(_usb_host_handle handle) {
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR)handle;
    USB_MemMapPtr usb_ptr = (USB_MemMapPtr) usb_host_ptr->G.DEV_PTR;
    uchar status;

    while (1)
    {
        status = (uint_8)(usb_ptr->ISTAT & usb_ptr->INTEN);

        if (!status)
            break;
        
        usb_ptr->ISTAT = status;
    
        if (status & USB_ISTAT_ATTACH_MASK)
        {
            /* USB device is (being) attached */
            usb_ptr->INTEN &= ~USB_INTEN_ATTACHEN_MASK;
            _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_ATTACH);
        }
        
        if (status & USB_ISTAT_TOKDNE_MASK)
        {
            // atom transaction done - token done
            
            _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_TOK_DONE);
        }
        
        if (status & USB_ISTAT_USBRST_MASK)
        {
            // usb reset
            _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_RESET);
        }

        if (status & USB_ISTAT_SOFTOK_MASK)
        {
            // usb SOF TOK
            _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_SOF_TOK);
        }
    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_init_int_tr
*  Returned Value : None
*  Comments       :
*        Initialize interrupt transaction que
*END*-----------------------------------------------------------------*/
static void _usb_khci_init_int_tr(USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr) {
    _mqx_int i;
    TR_INT_QUE_ITM_STRUCT_PTR tr = usb_host_ptr->tr_int_que;
    
    for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++) {
        tr->msg.type = TR_MSG_UNKNOWN;
    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_add_int_tr
*  Returned Value : -1 que is full
*  Comments       :
*        Add new interrupt transaction to que
*END*-----------------------------------------------------------------*/
static int _usb_khci_add_int_tr(USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr, TR_MSG_STRUCT *msg, _mqx_uint period) {
    _mqx_int i = 0;
    TR_INT_QUE_ITM_STRUCT_PTR tr = usb_host_ptr->tr_int_que;
    TIME_STRUCT tm;
    
    // find free position
    for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++)
    {
        if (tr->msg.type == TR_MSG_UNKNOWN)
        {
            tr->period = period;

            _time_get(&tm);
            tr->time.SECONDS = tm.SECONDS;
            tr->time.MILLISECONDS = tm.MILLISECONDS + period;
            //tr->time.MILLISECONDS = tm.MILLISECONDS;

            tr->time.SECONDS += tr->time.MILLISECONDS / 1000;
            tr->time.MILLISECONDS %= 1000;

            USB_mem_copy(msg, &tr->msg, sizeof(TR_MSG_STRUCT));
            
            break;
        }
    }
    
    return (i < USBCFG_KHCI_MAX_INT_TR) ? i : -1;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_rm_int_tr
*  Returned Value : 0 successful
*  Comments       :
*        Remove interrupt transaction from que
*END*-----------------------------------------------------------------*/
static int _usb_khci_rm_int_tr(USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr, TR_MSG_STRUCT *msg) {
    _mqx_int i = 0;
    TR_INT_QUE_ITM_STRUCT_PTR tr = usb_host_ptr->tr_int_que;
    
    // find record
    for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++)
    {
        if (tr->msg.pipe_desc == msg->pipe_desc && tr->msg.pipe_tr == msg->pipe_tr)
        {
            //USB_mem_zero(tr, sizeof(TR_INT_QUE_ITM_STRUCT));
            tr->msg.type = TR_MSG_UNKNOWN;    
            return 0;
        }
    }
    
    return -1;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_get_hot_int_tr
*  Returned Value : 0 successful
*  Comments       :
*        Make a message copy for transaction which need evaluation
*END*-----------------------------------------------------------------*/
static int _usb_khci_get_hot_int_tr(USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr, TR_MSG_STRUCT *msg) {
    _mqx_int i, res = -1;
    register TIME_STRUCT hot_time;
    TR_INT_QUE_ITM_STRUCT_PTR tr = usb_host_ptr->tr_int_que;
    TR_INT_QUE_ITM_STRUCT_PTR hot_tr = NULL;
    TIME_STRUCT tm;
    
    hot_time.SECONDS = hot_time.MILLISECONDS = 0xFFFFFFFF;

    /* find hottest transaction */
    for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++) {
        if (tr->msg.type != TR_MSG_UNKNOWN) {
            if (hot_time.SECONDS > tr->time.SECONDS) {
                hot_time.SECONDS = tr->time.SECONDS;
                hot_time.MILLISECONDS = tr->time.MILLISECONDS;
                hot_tr = tr;
            }
            else if ((hot_time.SECONDS == tr->time.SECONDS) && (hot_time.MILLISECONDS > tr->time.MILLISECONDS)) {
                hot_time.MILLISECONDS = tr->time.MILLISECONDS;
                hot_tr = tr;
            }
        }
    }
    
    if (hot_tr) {
        /* test if hottest transaction was the last one with timeout - if yes, don't allow to block USB transfers with this interrupt */
        if (usb_host_ptr->last_to_pipe == hot_tr->msg.pipe_desc) {
            usb_host_ptr->last_to_pipe = NULL; //it is allowed to perform this interupt next time, but not now
            return res;
        }
        
        /* test if hottest transaction is enough hot to fire */
        _time_get(&tm);
            
        if ((hot_time.SECONDS < tm.SECONDS) || ((hot_time.SECONDS == tm.SECONDS) && (hot_time.MILLISECONDS < tm.MILLISECONDS))) {
            /* update time to next fire = now + period */
            hot_tr->time.SECONDS = tm.SECONDS;
            hot_tr->time.MILLISECONDS = tm.MILLISECONDS + hot_tr->period;

            hot_tr->time.SECONDS += hot_tr->time.MILLISECONDS / 1000;
            hot_tr->time.MILLISECONDS %= 1000;

            USB_mem_copy(&hot_tr->msg, msg, sizeof(TR_MSG_STRUCT));
            res = 0;
        }
    }
    
    return res;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_attach
*  Returned Value : none
*  Comments       :
*        KHCI attach event
*END*-----------------------------------------------------------------*/
static void _usb_khci_attach(USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr) {

    USB_MemMapPtr usb_ptr = (USB_MemMapPtr) usb_host_ptr->G.DEV_PTR;

    //LWGPIO_STRUCT pin;
    uint8_t speed;
    uint8_t temp;
    uint8_t index = 0;

    //lwgpio_init(&pin, ((0x4 << 5 | 0x80000000) | 8), LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);

    //lwgpio_set_functionality(&pin, 1);

    usb_ptr->ADDR = 0;

    //lwgpio_set_value(&pin, 1);
    _time_delay(150);
    //lwgpio_set_value(&pin, 0);

    usb_ptr->ADDR &= ~USB_ADDR_LSEN_MASK; 

    do
    {
        temp = ((usb_ptr->CTL & USB_CTL_JSTATE_MASK) ? 0 : 1);
        _time_delay(5);
        speed = ((usb_ptr->CTL & USB_CTL_JSTATE_MASK) ? 0 : 1);
        index++;
    }
    while ((temp != speed) && index < USB_KHCI_MAX_SPEED_DETECTION_COUNT);

    
    if (temp != speed)
    {
        printf("speed not match!\n");
        return;
    }



    if (speed == USB_SPEED_FULL)
    {
        usb_ptr->ADDR &= ~USB_ADDR_LSEN_MASK; 
    }

    usb_host_ptr->G.SPEED = speed;

    usb_ptr->ISTAT = 0xff;   // clean each int flags

    usb_ptr->INTEN &= ~(USB_INTEN_TOKDNEEN_MASK | USB_INTEN_USBRSTEN_MASK);

    // bus reset
    usb_ptr->CTL |= USB_CTL_RESET_MASK;
    _time_delay(30); //wait for 30 milliseconds (2.5 is minimum for reset, 10 recommended)
    usb_ptr->CTL &= ~USB_CTL_RESET_MASK;


    // Delay after reset was provided to be sure about speed- HS / FS. Since the KHCI does not run HS, the delay is redundant.
    // Some kinetis devices cannot recover after the delay, so it is better not to have delayed speed detection and SOF packet generation 
    // This is potentional risk as any high priority task will get CPU now, the host will not begin the enumeration process.
    //_time_delay(10);

    // enable SOF sending
    usb_ptr->CTL |= USB_CTL_USBENSOFEN_MASK;

    _time_delay(100);

    usb_ptr->ISTAT = 0xff;   // clean each int flags

    usb_ptr->INTEN = \
        USB_INTEN_TOKDNEEN_MASK |
        USB_INTEN_USBRSTEN_MASK |
        USB_INTEN_SOFTOKEN_MASK;

    usb_dev_list_attach_device((pointer)usb_host_ptr, (uint_8)(usb_host_ptr->G.SPEED), 0, 0);

      _lwevent_clear(&usb_host_ptr->khci_event, KHCI_EVENT_ATTACH);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_reset
*  Returned Value : none
*  Comments       :
*        KHCI reset event
*END*-----------------------------------------------------------------*/
static void _usb_khci_reset(USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr) {
    USB_MemMapPtr usb_ptr = (USB_MemMapPtr) usb_host_ptr->G.DEV_PTR;

    // clear attach flag
    usb_ptr->ISTAT |= USB_ISTAT_ATTACH_MASK;

    /* Speical for K22F512 120M */
    // _time_delay(150);  // remove for aoa detch


    /* Test the presence of USB device */
    if (usb_ptr->ISTAT & USB_ISTAT_ATTACH_MASK) {
        /* device attached, so really normal reset was performed */
        usb_ptr->ADDR = 0;
        usb_ptr->ENDPOINT[0].ENDPT |= USB_ENDPT_HOSTWOHUB_MASK;
    }
    else {
        /* device was detached, the reset event is false- nevermind, notify about detach */
        usb_ptr->INTEN &= ~USB_INTEN_USBRSTEN_MASK;
        _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_DETACH);
    }

    _lwevent_clear(&usb_host_ptr->khci_event, KHCI_EVENT_RESET | KHCI_EVENT_TOK_DONE | KHCI_EVENT_SOF_TOK | KHCI_EVENT_TR_ARRIVE);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_detach
*  Returned Value : none
*  Comments       :
*        KHCI detach event
*END*-----------------------------------------------------------------*/
static void _usb_khci_detach(USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr) {
    USB_MemMapPtr usb_ptr = (USB_MemMapPtr) usb_host_ptr->G.DEV_PTR;

    usb_dev_list_detach_device((pointer)usb_host_ptr, 0, 0);

    /* Cleaning interrupt transaction queue from device is done
    ** by calling _usb_khci_host_close_pipe from upper layer
    */
    
    /* Now, disable bus control for any events, disable SOFs,
    ** just prepare for attach in host mode.
    */
    usb_ptr->CTL = USB_CTL_HOSTMODEEN_MASK;
    /* This will clear all pending interrupts... In fact, there shouldnt be any
    ** after detaching a device.
    */
    usb_ptr->ISTAT = 0xFF;
    /* Now, enable only USB interrupt attach for host mode */
    usb_ptr->INTEN = USB_INTEN_ATTACHEN_MASK;

    _lwevent_clear(&usb_host_ptr->khci_event, KHCI_EVENT_DETACH);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_task
*  Returned Value : none
*  Comments       :
*        KHCI task
*END*-----------------------------------------------------------------*/
static void _usb_khci_task(uint_32 dev_inst_ptr) {
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) dev_inst_ptr;
    _mqx_uint seq_ints = 5;    
    TR_MSG_STRUCT msg;
    uint_32 remain = 0;
    uint_32 required = 0;
    _mqx_int res;
    uint_8 *buf;
    

    /* The _lwmsgq_init accepts the size of TR_MSG_STRUCT as a multiplier of sizeof(_mqx_max_type) */
#define MSG_SIZE_IN_MAX_TYPE (1 + (sizeof(TR_MSG_STRUCT) - 1) / sizeof(_mqx_max_type))
    if (_lwmsgq_init(usb_host_ptr->tr_que, USBCFG_KHCI_TR_QUE_MSG_CNT, MSG_SIZE_IN_MAX_TYPE) != MQX_OK) {
        // lwmsgq_init failed
        _mqx_exit(0);
    }
    
    _usb_khci_init_int_tr(usb_host_ptr);

    while (1) {
        if (usb_host_ptr->G.DEVICE_LIST_PTR) {

            msg.type = TR_MSG_UNKNOWN;
        
            // look for interrupt transaction
            if ((_usb_khci_get_hot_int_tr(usb_host_ptr, &msg) != 0) || (!seq_ints--)) {
                seq_ints = 5;
                if (_lwmsgq_receive(usb_host_ptr->tr_que, (uint_32 *) &msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 2 * USBCFG_KHCI_WAIT_TICK, 0) == MQX_OK)  /* this value must twice or more as system tick */
                {
                    if (msg.pipe_desc->G.PIPETYPE == USB_INTERRUPT_PIPE) {
                        _usb_khci_add_int_tr(usb_host_ptr, &msg, msg.pipe_desc->G.INTERVAL );
                        //msg.type = TR_MSG_UNKNOWN;
                    }

                    if ( (msg.pipe_desc->G.PIPETYPE == USB_BULK_PIPE) && (msg.pipe_desc->G.DIRECTION == USB_RECV)) {
                        _usb_khci_add_int_tr(usb_host_ptr, &msg, 1);
                        //msg.type = TR_MSG_UNKNOWN;
                    }
                }
            }
            
            if (msg.type != TR_MSG_UNKNOWN) {
                // evaluate message
                             
                switch (msg.type) {
                    case TR_MSG_SETUP:
//                        msg.pipe_desc->G.NEXTDATA01 = 0; done in upper layer
                        _usb_khci_atom_tr(usb_host_ptr, TR_CTRL, msg.pipe_desc, (uint_8 *) &msg.pipe_tr->G.setup_packet, 8);
                    
                        // send next token
                        if (msg.pipe_tr->G.RX_LENGTH) {
                            buf = msg.pipe_tr->G.RX_BUFFER;
                            required = remain = msg.pipe_tr->G.RX_LENGTH;
                            KHCI_DEBUG_LOG('p', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, required)
                        
                            while (remain > 0) {
                                res = _usb_khci_atom_tr(usb_host_ptr, TR_IN, msg.pipe_desc, buf, remain);
                                
                                if (res < 0)
                                    break;
                            
                                buf += res;
                                remain -=res; 
                            }
                        
                            msg.pipe_desc->G.NEXTDATA01 = 1;
                            res = _usb_khci_atom_tr(usb_host_ptr, TR_OUT, msg.pipe_desc, 0, 0);
                        }
                        else if (msg.pipe_tr->G.TX_LENGTH) {
                            buf = msg.pipe_tr->G.TX_BUFFER;
                            required = remain = msg.pipe_tr->G.TX_LENGTH;
                            KHCI_DEBUG_LOG('p', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, required)
                        
                            while (remain > 0) {
                                res = _usb_khci_atom_tr(usb_host_ptr, TR_OUT, msg.pipe_desc, buf, remain);
                        
                                if (res < 0)
                                    break;
                            
                                buf += res;
                                remain -=res;
                            }

                            msg.pipe_desc->G.NEXTDATA01 = 1;
                            res = _usb_khci_atom_tr(usb_host_ptr, TR_IN, msg.pipe_desc, 0, 0);
                        }
                        else {
                            msg.pipe_desc->G.NEXTDATA01 = 1;
                            res = _usb_khci_atom_tr(usb_host_ptr, TR_IN, msg.pipe_desc, 0, 0);
                            remain = 0;
                        }
                        break;

                    case TR_MSG_RECV: 
                        {
                            int lastValidRecvBytes = -1;
                            int retry              = msg.pipe_desc->G.NAK_COUNT;

                            buf = msg.pipe_tr->G.RX_BUFFER;
                            required = remain = msg.pipe_tr->G.RX_LENGTH;
                            KHCI_DEBUG_LOG('p', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, required);

                            do {
                                res = _usb_khci_atom_tr(usb_host_ptr, TR_IN, msg.pipe_desc, buf, remain);
                                
                                if(res > 0) {
                                    lastValidRecvBytes = res;
                                }

                                if (res < 0) {
                                    /* any error arrive, still retry if condition meet */
                                    if((msg.pipe_desc->G.PIPETYPE == USB_BULK_PIPE) &&
                                       /* only after valid data arrive, start retry!*/
                                       (lastValidRecvBytes == msg.pipe_desc->G.MAX_PACKET_SIZE) &&   
                                       (retry-- != 0)) {} else { break; }
                                }
                                else
                                {

                                    buf    += res;
                                    remain -= res;

                                    if ( (remain == 0) || (res < msg.pipe_desc->G.MAX_PACKET_SIZE) )
                                    {
                                        res = remain;
                                        break;
                                    }
                                    /* reset retry count if vaild data arrive */
                                    retry = msg.pipe_desc->G.NAK_COUNT;   
                                }

                            } while (1);
                        }

                        break;

                    case TR_MSG_SEND:
                        buf = msg.pipe_tr->G.TX_BUFFER;
                        required = remain = msg.pipe_tr->G.TX_LENGTH;
                        KHCI_DEBUG_LOG('p', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, required)
                    
                        do {
                            res = _usb_khci_atom_tr(usb_host_ptr, TR_OUT, msg.pipe_desc, buf, remain);
                        
                            if (res < 0)
                                break;
                        
                            buf += res;
                            remain -=res;
                            
                            if ((remain == 0) || (res < msg.pipe_desc->G.MAX_PACKET_SIZE)) 
                            {
                                res = remain;
                                break;
                            }
                            
                        } while (1);
                        break;

                    default:
                       break;
                }
        
                if (  (msg.pipe_desc->G.PIPETYPE == USB_INTERRUPT_PIPE) || 
                     ((msg.pipe_desc->G.PIPETYPE == USB_BULK_PIPE) && (msg.pipe_desc->G.DIRECTION == USB_RECV))
                   ) 

                //if (msg.type == TR_MSG_RECV) 
                {
                    /* for interrupt pipes, callback only if some data was received or serious error occured */
                    if ((required != remain) || (res != KHCI_ATOM_TR_NAK)) {
                        if((res < 0) && (res != KHCI_ATOM_TR_NAK)) {
                            /* serious issue */
                            printf("res is %d \n", res );
                        }
                        _usb_khci_process_tr_complete(msg.pipe_desc, msg.pipe_tr, required, remain, res);
                        _usb_khci_rm_int_tr(usb_host_ptr, &msg);
                    }
                }
                else {
                    _usb_khci_process_tr_complete(msg.pipe_desc, msg.pipe_tr, required, remain, res);
                }
            }
        }
        else {
            // wait for event if device is not attached
            _lwevent_wait_ticks(&usb_host_ptr->khci_event, KHCI_EVENT_MASK, FALSE, 0);
        }
        
        if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_MASK) {
            if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_ATTACH)
                _usb_khci_attach(usb_host_ptr);

            if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_RESET)
                _usb_khci_reset(usb_host_ptr);

            if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_DETACH) {
                printf("start _usb_khci_detach \n");
                _usb_khci_detach(usb_host_ptr);
            }
        }
    }
}
static int32_t _usb_khci_handle_ctlbulk_transfer(TR_MSG_STRUCT  *msg)
{


    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr  = NULL;
    KHCI_PIPE_STRUCT_PTR           pKPDESC       = NULL;
    KHCI_TR_STRUCT_PTR             pKTR          = NULL;
    uint8_t                        trType        = 0;


    uint8_t  *buf;
    int32_t  required;
    int32_t  res = -1;


    int32_t  retryCount;

    uint8_t  state;

    int         timeoutChecked = 0;

    usb_host_ptr = msg->pipe_tr->usb_host_ptr;
    pKPDESC      = msg->pipe_desc;
    pKTR         = msg->pipe_tr;

    
    retryCount   = pKPDESC->G.NAK_COUNT;

    do {
        switch(msg->type){
            case TR_MSG_SETUP:
                switch(pKTR->state){
                    case KHCI_TR_CTRL_SETUP:

                        trType     = TR_CTRL;
                        buf        = (uint8_t *)(&pKTR->G.setup_packet);
                        required   = 8;
                        break;
                    case KHCI_TR_CTRL_DATA:
                        trType    = pKTR->setup_data_direction == 0 ? TR_OUT : TR_IN;
                        buf       = pKTR->buf + pKTR->offset;
                        required  = pKTR->len - pKTR->offset > pKPDESC->G.MAX_PACKET_SIZE ?
                                    pKPDESC->G.MAX_PACKET_SIZE : pKTR->len - pKTR->offset;
                        break;
                    case KHCI_TR_CTRL_ACK:
                        if(pKTR->setup_data_direction == 0 || pKTR->setup_data_direction == -1)
                            trType = TR_IN;
                        else
                            trType = TR_OUT;

                        buf        = NULL;
                        required   = 0;
                        break;
                    default:
                        printf("TR_MSG_SETUP: wrong state %d \n", pKTR->state);
                }
                break;
            case TR_MSG_RECV:
            case TR_MSG_SEND:
                trType   = msg->type == TR_MSG_SEND ? TR_OUT : TR_IN;
                buf      = pKTR->buf + pKTR->offset;
                required = pKTR->len - pKTR->offset > pKPDESC->G.MAX_PACKET_SIZE ?
                           pKPDESC->G.MAX_PACKET_SIZE : pKTR->len - pKTR->offset;
                break;
            default:
                printf("_usb_khci_handle_ctlbulk_transfer only handle SETUP & SEND & RECV msg, this msg type is %d \n", msg->type);
        }


        res = _usb_khci_atom_tr(usb_host_ptr, trType, pKPDESC, buf, required);

        //if((res ==  KHCI_ATOM_TR_STALL) || res == (KHCI_ATOM_TR_TO) ) {
        if((res ==  KHCI_ATOM_TR_STALL)){
            printf("STALL or TO ? %d \n", res);
            pKTR->state = KHCI_TR_CTRL_COMPLETE;
        }
        else {


            switch(msg->type){
                case TR_MSG_SETUP:
                    switch(pKTR->state){
                        case KHCI_TR_CTRL_SETUP:
                            if(res == 8) {
                                if(pKTR->G.TX_LENGTH || pKTR->G.RX_LENGTH)
                                    pKTR->state = KHCI_TR_CTRL_DATA;
                                else
                                    pKTR->state = KHCI_TR_CTRL_ACK;

                                pKPDESC->G.NEXTDATA01 = 1;

                            }
                            else{
                                /* no retry ? */
                                printf("TR_MSG_SETUP setup phase failed! res is %d \n", res);
                                retryCount--;
                            }
                            break;
                        case KHCI_TR_CTRL_DATA:
                            if(res > 0){
                                pKTR->offset += res;
                                if( /* res less than MAX pipe length or all data been transfered */
                                        (res < pKPDESC->G.MAX_PACKET_SIZE) || (pKTR->offset == pKTR->len ) 
                                  )
                                {

                                    pKPDESC->G.NEXTDATA01 = 1;
                                    pKTR->state = KHCI_TR_CTRL_ACK;
                                }
                            }
                            else{
                                retryCount--;
                            }
                            break;
                        case KHCI_TR_CTRL_ACK:
                            if(res < 0){
                                //printf("Setup ACK phase return %d \n", res);
                                retryCount--;
                            }
                            else{
                                pKTR->state = KHCI_TR_CTRL_COMPLETE;
                            }
                            break;
                        default:
                            printf("This setup state after tr should not reach! \n", pKTR->state);
                    }
                    break;
                case TR_MSG_RECV:
                case TR_MSG_SEND:
                    if(res > 0){
                        pKTR->offset += res;
                        if( /* res less than MAX pipe length or all data been transfered */
                                (res < pKPDESC->G.MAX_PACKET_SIZE) || (pKTR->offset == pKTR->len) 
                          )
                        {
                            pKTR->state = KHCI_TR_END;
                        }
                    }
                    else{
                        if((pKTR->offset == 0) && (pKTR->G.FLAG & TR_RETRY_AFTER_VALID))
                            retryCount = 0;
                        else
                            retryCount--;
                    }
                    break;
                default:
                    printf("_usb_khci_handle_ctlbulk_transfer only handle SETUP & SEND & RECV msg after tr, this msg type is %d \n", msg->type);

            }
        }


        if((pKTR->G.TIMEOUT > 0) && (timeoutChecked == 0)){
            uint32_t    _curtime, _interval;
            TIME_STRUCT tm;

            timeoutChecked = 1;
            _time_get(&tm);
            _curtime = (tm.SECONDS * 1000) + tm.MILLISECONDS;

            _interval = _curtime - pKTR->starttime;
            
            if( (_interval / 1000) >= pKTR->G.TIMEOUT ){
                printf("TR TIME OUT ? \n");
                pKTR->state = KHCI_TR_END;
            }
        }


        state = pKTR->state;

        if(state == KHCI_TR_END) {
            /* This transfer be completed, remove this tr from list and call callback function */
            if(!(msg->pipe_tr->G.FLAG & TR_PERIOD_KEEP)){
                _mqx_dll_remove_node(pKTR->list, &pKTR->node);
                pKTR->G.FLAG    = 0;
                pKTR->G.TIMEOUT = 0;
            }

            _usb_khci_process_tr_complete(pKPDESC, pKTR, pKTR->len, pKTR->len - pKTR->offset, res);

            if(msg->pipe_tr->G.FLAG & TR_PERIOD_KEEP){
                pKTR->offset = 0;
                pKTR->state  = KHCI_TR_START;
            }

        }

    }while((state != KHCI_TR_END) &&  (retryCount > 0) && !(usb_host_ptr->khci_event.VALUE & KHCI_EVENT_SOF_TOK));
            

    return (state);
}

static int32_t _usb_khci_handle_period_transfer(TR_MSG_STRUCT  *msg){

    uint8_t  *buf;
    int32_t  required;
    int32_t  res = -1;

    /* no retry for period tansfer! */
    switch (msg->type) {
        case TR_MSG_RECV: 
            buf      = msg->pipe_tr->G.RX_BUFFER;
            required = msg->pipe_tr->G.RX_LENGTH;
            res = _usb_khci_atom_tr(msg->pipe_tr->usb_host_ptr, TR_IN, msg->pipe_desc, buf, required);
            break;
        case TR_MSG_SEND:
            buf      = msg->pipe_tr->G.TX_BUFFER;
            required = msg->pipe_tr->G.TX_LENGTH;
            if(required > 0) {
                //printf("required %d \n", required);
                res = _usb_khci_atom_tr(msg->pipe_tr->usb_host_ptr, TR_OUT, msg->pipe_desc, buf, required);
            }
            else 
                res = 0;
            break;
        default:
            printf("_usb_khci_handle_period_transfer only handle SEND & RECV msg, this msg type is %d \n", msg->type);
    }

    if( ((msg->pipe_desc->G.PIPETYPE == USB_INTERRUPT_PIPE   && res > 0) || 
         (msg->pipe_desc->G.PIPETYPE == USB_ISOCHRONOUS_PIPE && res > 0))  ||
        (msg->type ==  TR_MSG_SEND)
      ){
        /* free it ? */
        if(!(msg->pipe_tr->G.FLAG & TR_PERIOD_KEEP)){
            _mqx_dll_remove_node(msg->pipe_tr->list, &msg->pipe_tr->node);
            msg->pipe_tr->G.FLAG    = 0;
            msg->pipe_tr->G.TIMEOUT = 0;
        }
        _usb_khci_process_tr_complete(msg->pipe_desc, msg->pipe_tr, required, res > 0 ? (required - res) : required, res);
    }

    
    return 0;
}
static void _usb_khci_get_next_ctlbulk_transfer(USB_KHCI_HOST_STATE_STRUCT_PTR  usb_host_ptr, 
                                                TR_MSG_STRUCT                  *msg)

{

    MQX_DLL_NODE_PTR    pNode;
    KHCI_TR_STRUCT_PTR  pTr   = NULL;

    /* clear msg */
    memset(msg, 0, sizeof *msg);
    msg->type = TR_MSG_UNKNOWN;

    while(!_mqx_dll_is_list_empty(&usb_host_ptr->CTLBULKTRList)){

        pNode = usb_host_ptr->CTLBULKStartNode;

        if(pNode == NULL){/* CTLBULKStartNode start from list head ? */
            pNode = usb_host_ptr->CTLBULKTRList.head_ptr;
        }

        if(pNode != NULL){

            usb_host_ptr->CTLBULKStartNode = pNode->next_ptr;

            pTr = (KHCI_TR_STRUCT_PTR)((uint32_t)(pNode) - (uint32_t)(&((KHCI_TR_STRUCT_PTR)(0))->node));
            if(pTr->cancel_flag == 1){

            if (pTr->G.CALLBACK != NULL) 
                pTr->G.CALLBACK((pointer)pTr->pipe_desc, pTr->G.CALLBACK_PARAM, 0, 0,  USBERR_DEVICE_NOT_FOUND);

                /* release this G TR */
                _mqx_dll_remove_node(pTr->list, &pTr->node);
                pTr->G.TR_INDEX  = 0;
                pTr->G.FLAG      = 0;
                pTr->cancel_flag = 0;
                continue;
            }else {
                msg->type      = pTr->type;
                msg->pipe_tr   = pTr;
                msg->pipe_desc = pTr->pipe_desc;
                break;
            }

        }else {}; /* empty list ? */
    }
    return;
}

static void _usb_khci_get_next_iso_transfer(USB_KHCI_HOST_STATE_STRUCT_PTR  usb_host_ptr, 
                                            KHCI_TR_STRUCT_PTR              curPTR,
                                            TR_MSG_STRUCT                   *msg)
{
    MQX_DLL_NODE_PTR        pNode = NULL, pNodeNext  = NULL;
    KHCI_TR_STRUCT_PTR      pTr   = NULL;

    msg->type = TR_MSG_UNKNOWN;

    if(curPTR == NULL)
        _mqx_dll_get_head_node(&usb_host_ptr->ISOTRList, &pNode);
    else 
        _mqx_dll_get_next_node(&curPTR->node, &pNode);

    while(pNode != NULL){

        pTr = (KHCI_TR_STRUCT_PTR)((uint32_t)(pNode) - (uint32_t)(&((KHCI_TR_STRUCT_PTR)(0))->node));
        if(pTr->cancel_flag == 1){

            _mqx_dll_get_next_node(pNode, &pNodeNext);

            if (pTr->G.CALLBACK != NULL) 
                pTr->G.CALLBACK((pointer)pTr->pipe_desc, pTr->G.CALLBACK_PARAM, 0, 0,  USBERR_DEVICE_NOT_FOUND);

            /* release this G TR */
            _mqx_dll_remove_node(pTr->list, &pTr->node);
            pTr->G.TR_INDEX  = 0;
            pTr->G.FLAG      = 0;
            pTr->cancel_flag = 0;
            pNode            = pNodeNext;
        }
        else {
            msg->type      = pTr->type;
            msg->pipe_tr   = pTr;
            msg->pipe_desc = pTr->pipe_desc;
            break;
        }
    }
    return;
}

static void _usb_khci_get_next_interrupt_transfer(USB_KHCI_HOST_STATE_STRUCT_PTR  usb_host_ptr, 
                                                  KHCI_TR_STRUCT_PTR              curPTR,
                                                  TR_MSG_STRUCT                  *msg)
{
    MQX_DLL_NODE_PTR        pNode = NULL, pNodeNext  = NULL;
    KHCI_TR_STRUCT_PTR      pTr   = NULL;

    msg->type = TR_MSG_UNKNOWN;

    if(curPTR == NULL)
        _mqx_dll_get_head_node(&usb_host_ptr->INTTRList, &pNode);
    else 
        _mqx_dll_get_next_node(&curPTR->node, &pNode);

    while(pNode != NULL){

        pTr = (KHCI_TR_STRUCT_PTR)((uint32_t)(pNode) - (uint32_t)(&((KHCI_TR_STRUCT_PTR)(0))->node));
        if(pTr->cancel_flag == 1){

            _mqx_dll_get_next_node(pNode, &pNodeNext);

            if (pTr->G.CALLBACK != NULL) 
                pTr->G.CALLBACK((pointer)pTr->pipe_desc, pTr->G.CALLBACK_PARAM, 0, 0,  USBERR_DEVICE_NOT_FOUND);

            /* release this G TR */
            _mqx_dll_remove_node(pTr->list, &pTr->node);
            pTr->G.TR_INDEX  = 0;
            pTr->G.FLAG      = 0;
            pTr->cancel_flag = 0;
            pNode            = pNodeNext;
        }
        else {
            msg->type      = pTr->type;
            msg->pipe_tr   = pTr;
            msg->pipe_desc = pTr->pipe_desc;
            break;
        }
    }
    return;
}

/*FUNCTION*------------------------------------------------------------- 
*  init usb host pipe transfer list *
*END*-----------------------------------------------------------------*/
static void _usb_khci_init_pipe_transfer(USB_KHCI_HOST_STATE_STRUCT_PTR  usb_host_ptr){


    /* ISO INT should merge to one list ? */
    _mqx_dll_list_init(&usb_host_ptr->ISOTRList);
    _mqx_dll_list_init(&usb_host_ptr->INTTRList);
    _mqx_dll_list_init(&usb_host_ptr->CTLBULKTRList);

    return;
}

static int _usb_khci_remove_cancelled_tr(MQX_DLL_LIST_PTR list){

    MQX_DLL_NODE_PTR        pNode = NULL, pNodeNext = NULL;
    KHCI_TR_STRUCT_PTR      pTr   = NULL;

    _mqx_dll_get_head_node(list, &pNode);
    
    while(pNode != NULL){
        pTr = (KHCI_TR_STRUCT_PTR)((uint32_t)(pNode) - (uint32_t)(&((KHCI_TR_STRUCT_PTR)(0))->node));
        if(pTr->cancel_flag == 1){

            _mqx_dll_get_next_node(pNode, &pNodeNext);

            if (pTr->G.CALLBACK != NULL) 
                pTr->G.CALLBACK((pointer)pTr->pipe_desc, pTr->G.CALLBACK_PARAM, 0, 0,  USBERR_DEVICE_NOT_FOUND);

            /* release this G TR */
            _mqx_dll_remove_node(pTr->list, &pTr->node);
            pTr->G.TR_INDEX  = 0;
            pTr->G.FLAG      = 0;
            pTr->cancel_flag = 0;
            pNode            = pNodeNext;
        }
    }

    return 0;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_schedule_task
*  Returned Value : none
*  Comments       :
*        KHCI ALT task
*END*-----------------------------------------------------------------*/
static void _usb_khci_schedule_task(uint_32 dev_inst_ptr) {

    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) dev_inst_ptr;
    TR_MSG_STRUCT msg;
    
    USB_MemMapPtr usb_ptr = (USB_MemMapPtr) usb_host_ptr->G.DEV_PTR;
    
    uint_32 startFRMNUML, currentFRMNUML;

    /* The _lwmsgq_init accepts the size of TR_MSG_STRUCT as a multiplier of sizeof(_mqx_max_type) */
#define MSG_SIZE_IN_MAX_TYPE (1 + (sizeof(TR_MSG_STRUCT) - 1) / sizeof(_mqx_max_type))
    if (_lwmsgq_init(usb_host_ptr->tr_que, USBCFG_KHCI_TR_QUE_MSG_CNT, MSG_SIZE_IN_MAX_TYPE) != MQX_OK) {
        // lwmsgq_init failed
        printf("_usb_khci_schedule_task lwmsgq_init failed \n");
        _mqx_exit(0);
    }
    
    /* init pipe transfer array */
    _usb_khci_init_pipe_transfer(usb_host_ptr);

    usb_host_ptr->CTLBULKStartNode = NULL;

    while (1) {
        /* device valid */
        if (usb_host_ptr->G.DEVICE_LIST_PTR) {
            KHCI_TR_STRUCT_PTR    pKTR = NULL;
            msg.type = TR_MSG_UNKNOWN;
        

            /* wait SOF event */
            if (LWEVENT_WAIT_TIMEOUT == _lwevent_wait_ticks(&usb_host_ptr->khci_event, 
                                                             KHCI_EVENT_SOF_TOK | KHCI_EVENT_TR_ARRIVE, 
                                                             FALSE, 
                                                             2 *  USBCFG_KHCI_WAIT_TICK)) {
                printf("SOF stopped ? \n");
            }

            /* New TR arrived, handle it */
            if(usb_host_ptr->khci_event.VALUE & KHCI_EVENT_TR_ARRIVE){

                _lwevent_clear(&usb_host_ptr->khci_event, KHCI_EVENT_TR_ARRIVE);

                /* receive all new transfer     */
                /* no block, must very quickly! */
                while (_lwmsgq_receive(usb_host_ptr->tr_que, (uint_32 *) &msg, 0, 0, 0) == MQX_OK) 
                    /* new transfer arrive */
                {

                    TIME_STRUCT tm;

                    if(msg.type == TR_MSG_UNKNOWN)
                        continue;

                    _time_get(&tm);

                    _mqx_dll_node_init(&(msg.pipe_tr->node));

                    msg.pipe_tr->pipe_desc    = msg.pipe_desc;
                    msg.pipe_tr->type         = msg.type;
                    msg.pipe_tr->usb_host_ptr = usb_host_ptr;
                    msg.pipe_tr->state        = KHCI_TR_START;
                    msg.pipe_tr->offset       = 0;
                    msg.pipe_tr->cancel_flag  = 0;
                    msg.pipe_tr->G.TIMEOUT    = 0; /* default no time out */
                    msg.pipe_tr->starttime    = (tm.SECONDS * 1000) + tm.MILLISECONDS ; /* as ms */


                    /* put this transfer to different pipe type transfer double link list */
                    switch(msg.pipe_desc->G.PIPETYPE){
                        case USB_ISOCHRONOUS_PIPE:
                            _mqx_dll_insert_at_tail(&usb_host_ptr->ISOTRList, &(msg.pipe_tr->node));
                            msg.pipe_tr->list = &usb_host_ptr->ISOTRList;
                            break;
                        case USB_INTERRUPT_PIPE:
                            _mqx_dll_insert_at_tail(&usb_host_ptr->INTTRList, &(msg.pipe_tr->node));
                            msg.pipe_tr->list = &usb_host_ptr->INTTRList;
                            break;
                        default:

                            if(msg.type == TR_MSG_SETUP) {
                                if(msg.pipe_tr->G.TX_LENGTH != 0){
                                    msg.pipe_tr->setup_data_direction = 0;  /* data out */
                                } else
                                    if(msg.pipe_tr->G.RX_LENGTH != 0) {
                                        msg.pipe_tr->setup_data_direction = 1;  /* data in */
                                    }
                                    else
                                        msg.pipe_tr->setup_data_direction = -1;     /* no data phase */

                                msg.pipe_tr->G.TIMEOUT = 1; /* 1s time out for control pipe */

                            }

                            if(msg.pipe_tr->G.TX_LENGTH != 0){
                                msg.pipe_tr->len = msg.pipe_tr->G.TX_LENGTH;
                                msg.pipe_tr->buf = msg.pipe_tr->G.TX_BUFFER;
                            }
                            else
                                if(msg.pipe_tr->G.RX_LENGTH != 0) {
                                    msg.pipe_tr->len = msg.pipe_tr->G.RX_LENGTH;
                                    msg.pipe_tr->buf = msg.pipe_tr->G.RX_BUFFER;
                                }
                                else{
                                    msg.pipe_tr->len = 0;
                                    msg.pipe_tr->buf = NULL;
                                }

                            if(usb_host_ptr->CTLBULKStartNode != NULL)
                                _mqx_dll_insert_before_node(&usb_host_ptr->CTLBULKTRList, 
                                                             usb_host_ptr->CTLBULKStartNode,
                                                             &(msg.pipe_tr->node));
                            else
                                _mqx_dll_insert_at_tail(&usb_host_ptr->CTLBULKTRList, &(msg.pipe_tr->node));
                            msg.pipe_tr->list = &usb_host_ptr->CTLBULKTRList;
                    }

                } /* Got TR from messge queue */

            } /* New TR arrived */ 

            /* SOF event */
            if(usb_host_ptr->khci_event.VALUE & KHCI_EVENT_SOF_TOK) {

                KHCI_TR_STRUCT_PTR  curPTR = NULL;

                _lwevent_clear(&usb_host_ptr->khci_event, KHCI_EVENT_SOF_TOK);

                startFRMNUML   = usb_ptr->FRMNUML;
                /* do iso transfer,        only one tansfer  to be done later !*/
                do {
                    msg.type = TR_MSG_UNKNOWN;
                    _usb_khci_get_next_iso_transfer(usb_host_ptr, curPTR, &msg);
                    if(msg.type != TR_MSG_UNKNOWN) {
                        _usb_khci_handle_period_transfer(&msg);
                        curPTR = msg.pipe_tr;

                    }
                }while(msg.type !=  TR_MSG_UNKNOWN);

#if 0
                if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_SOF_TOK) {
                    printf("iso has occupied a frame time! \n");
                    continue;
                }
#endif

                /* do interrrupt transfer, only one tansfer  to be done later !*/
                msg.type = TR_MSG_UNKNOWN;
                _usb_khci_get_next_interrupt_transfer(usb_host_ptr, NULL, &msg);
                if(msg.type != TR_MSG_UNKNOWN) {
                    _usb_khci_handle_period_transfer(&msg);
                }


#if 0
                if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_SOF_TOK) {
                    printf("iso and  interrupt occupied a frame time! \n");
                    continue;
                }
#endif
                currentFRMNUML = usb_ptr->FRMNUML;

                if(currentFRMNUML != startFRMNUML){
                    /* This one ISO transfer cross SOF, clear SOF event */
                    if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_SOF_TOK) {
                        printf("ISO/INT corner case, handle it ? \n");
                        _lwevent_clear(&usb_host_ptr->khci_event, KHCI_EVENT_SOF_TOK);
                    }
                }
            }

           {
               int                ret;
               KHCI_TR_STRUCT_PTR pHKTR = NULL;

               /* do ctrl bulk transfer */
               do{

                   _usb_khci_get_next_ctlbulk_transfer(usb_host_ptr, &msg);

                   if(msg.pipe_tr == pHKTR)
                       break;

                   if(msg.type != TR_MSG_UNKNOWN) {
                       ret  = _usb_khci_handle_ctlbulk_transfer(&msg);
                       if((ret != KHCI_TR_END) && (pHKTR == NULL )){
                           pHKTR = msg.pipe_tr;
                       }
                   }
               }while(!(usb_host_ptr->khci_event.VALUE & KHCI_EVENT_SOF_TOK));
           }

        }
        else {
            /* no device attach, wait attach event */
            /* wait for event if device is not attached */
            _lwevent_wait_ticks(&usb_host_ptr->khci_event, KHCI_EVENT_ATTACH, FALSE, 0);
        }
        
        if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_WITHOUTSOF_MASK) {

            if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_RESET) {
                //printf("KHCI_EVENT_RESET \n");
                _usb_khci_reset(usb_host_ptr);
            }

            if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_DETACH) {
                //printf("KHCI_EVENT_DETACH \n");
                _usb_khci_detach(usb_host_ptr);
            }

            /* ATTACH handler must after DETACH handler, TR callback will set 
             * DETACH & ATTACH event, this task must check detach firstly! */
            if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_ATTACH)   
                _usb_khci_attach(usb_host_ptr);
        }

    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_task_create
*  Returned Value : error or USB_OK
*  Comments       :
*        Create KHCI task
*END*-----------------------------------------------------------------*/
static USB_STATUS _usb_khci_task_create(_usb_host_handle handle) {
    //USB_STATUS status;
    _task_id task_id;
    TASK_TEMPLATE_STRUCT task_template;
    
    // check wait time for events - must be > 0
    if (USBCFG_KHCI_WAIT_TICK < 1)
        return USBERR_UNKNOWN_ERROR;

    /* create task for processing interrupt deferred work */
    task_template.TASK_TEMPLATE_INDEX = USB_KHCI_TASK_TEMPLATE_INDEX;
    task_template.TASK_ADDRESS = USB_KHCI_TASK_ADDRESS;
    task_template.TASK_STACKSIZE = USB_KHCI_TASK_STACKSIZE;
    task_template.TASK_PRIORITY = USBCFG_KHCI_TASK_PRIORITY;
    task_template.TASK_NAME = USB_KHCI_TASK_NAME;
    task_template.TASK_ATTRIBUTES = USB_KHCI_TASK_ATTRIBUTES;
    task_template.CREATION_PARAMETER = (uint_32)handle;   //USB_KHCI_TASK_CREATION_PARAMETER;
    task_template.DEFAULT_TIME_SLICE = USB_KHCI_TASK_DEFAULT_TIME_SLICE;

    task_id = _task_create_blocked(0, 0, (uint_32)&task_template);
    
    if (task_id == 0) {
        return USBERR_UNKNOWN_ERROR;
    }
    
    _task_ready(_task_get_td(task_id));

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_preinit
*  Returned Value : error or USB_OK
*  Comments       :
*        Allocate the structures for KHCI
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_preinit(_usb_host_handle *handle) {
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) USB_mem_alloc_zero(sizeof(USB_KHCI_HOST_STATE_STRUCT));
    KHCI_PIPE_STRUCT_PTR p, pp;
    int i;

    if (NULL != usb_host_ptr) {
        /* Allocate the USB Host Pipe Descriptors */
        usb_host_ptr->G.PIPE_DESCRIPTOR_BASE_PTR = (PIPE_STRUCT_PTR)USB_mem_alloc(sizeof(KHCI_PIPE_STRUCT) * USBCFG_MAX_PIPES);
        if (usb_host_ptr->G.PIPE_DESCRIPTOR_BASE_PTR == NULL) {
            USB_mem_free(usb_host_ptr);

            return USBERR_ALLOC;
        }
        USB_mem_zero(usb_host_ptr->G.PIPE_DESCRIPTOR_BASE_PTR, sizeof(KHCI_PIPE_STRUCT) * USBCFG_MAX_PIPES);

        p = (KHCI_PIPE_STRUCT_PTR) usb_host_ptr->G.PIPE_DESCRIPTOR_BASE_PTR;
        pp = NULL;
        for (i = 0; i < USBCFG_MAX_PIPES; i++) {
           if (pp != NULL) {
              pp->G.NEXT = (PIPE_STRUCT_PTR) p;
           }
           pp = p;
           p++;
        }

        usb_host_ptr->G.PIPE_SIZE = sizeof(KHCI_PIPE_STRUCT);
        usb_host_ptr->G.TR_SIZE = sizeof(KHCI_TR_STRUCT);

    } /* Endif */
    
    *handle = (_usb_host_handle) usb_host_ptr;
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_init
*  Returned Value : error or USB_OK
*  Comments       :
*        Initialize the kirin HCI controller
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_init(_usb_host_handle handle) {
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR)handle;
    USB_KHCI_HOST_INIT_STRUCT_PTR param = (USB_KHCI_HOST_INIT_STRUCT_PTR) usb_host_ptr->G.INIT_PARAM;
    USB_MemMapPtr           usb_ptr;
    USB_STATUS              status = USB_OK;

    /* create lwevent group */
    if (_lwevent_create(&usb_host_ptr->khci_event, 0) != MQX_OK) {
        return USBERR_GET_MEMORY_FAILED;
    }

   if (param == NULL)
       return USBERR_INIT_DATA;

    usb_ptr = (USB_MemMapPtr) param->BASE_PTR;
    usb_host_ptr->G.DEV_PTR = (USB_HOST_STATE_STRUCT_PTR) usb_ptr;
   
    _usb_khci_task_create(usb_host_ptr);
    
    // install isr
    if (!(USB_install_isr(param->VECTOR, _usb_khci_isr, (pointer)usb_host_ptr)))
        return USBERR_INSTALL_ISR;
    
    usb_ptr->ISTAT = 0xFF;
    /* Enable week pull-downs, usefull for detecting detach (effectivelly bus discharge) */
    usb_ptr->USBCTRL |= USB_USBCTRL_PDE_MASK;
    /* Remove suspend state */
    //usb_ptr->USBCTRL &= ~USB_USBCTRL_SUSP_MASK;


    usb_ptr->CTL |= USB_CTL_ODDRST_MASK;
    
    usb_ptr->BDTPAGE1 = (uint_8)((uint_32)BDT_BASE >> 8);
    usb_ptr->BDTPAGE2 = (uint_8)((uint_32)BDT_BASE >> 16);
    usb_ptr->BDTPAGE3 = (uint_8)((uint_32)BDT_BASE >> 24);
    
    /* Set SOF threshold */
    //usb_ptr->SOFTHLD = 1;
    usb_ptr->SOFTHLD = 205;

#if (MQX_CPU == PSP_CPU_MK22FN256) || (MQX_CPU == PSP_CPU_MK22FN512)
    usb_ptr->USBFRMADJUST = 0xFF;
#endif
    
    usb_ptr->CTL = USB_CTL_HOSTMODEEN_MASK;

    /* Following is for OTG control instead of internal bus control */
//    usb_ptr->OTGCTL = USB_OTGCTL_DMLOW_MASK | USB_OTGCTL_DPLOW_MASK | USB_OTGCTL_OTGEN_MASK;

    /* Wait for attach interrupt */
    usb_ptr->INTEN = USB_INTEN_ATTACHEN_MASK;
        
    return status;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_shutdown - NOT IMPLEMENT YET
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to shutdown the host
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_shutdown(_usb_host_handle handle) {
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_send
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to send data
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_send(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr) {
    USB_STATUS status = USB_OK;
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) handle;
    TR_MSG_STRUCT msg;

    msg.type = TR_MSG_SEND;
    msg.pipe_desc = (KHCI_PIPE_STRUCT_PTR) pipe_ptr;
    msg.pipe_tr = (KHCI_TR_STRUCT_PTR) tr_ptr;
    
    KHCI_DEBUG_LOG('i', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, msg.pipe_tr->G.TX_LENGTH)
    if (MQX_OK != _lwmsgq_send(usb_host_ptr->tr_que, (uint_32 *) &msg, 0)) {
        status = USBERR_TR_FAILED;
    }
    else{
        _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_TR_ARRIVE);
    }
    
    return status;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_send_setup
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to send setup data
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_send_setup(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr) {
    USB_STATUS status = USB_OK;
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) handle;
    TR_MSG_STRUCT msg;

    msg.type = TR_MSG_SETUP;
    msg.pipe_desc = (KHCI_PIPE_STRUCT_PTR) pipe_ptr;
    msg.pipe_tr = (KHCI_TR_STRUCT_PTR) tr_ptr;

    if (msg.pipe_tr->G.RX_LENGTH)
      KHCI_DEBUG_LOG('i', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, msg.pipe_tr->G.RX_LENGTH)
    else
      KHCI_DEBUG_LOG('i', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, msg.pipe_tr->G.TX_LENGTH)

    if (MQX_OK != _lwmsgq_send(usb_host_ptr->tr_que, (uint_32 *) &msg, 0)) {
        status = USBERR_TR_FAILED;
    }
    else{
        _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_TR_ARRIVE);
    }

    return status;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_recv
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to receive data
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_recv(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr) {
    USB_STATUS status = USB_OK;
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) handle;
    TR_MSG_STRUCT msg;

    msg.type = TR_MSG_RECV;
    msg.pipe_desc = (KHCI_PIPE_STRUCT_PTR) pipe_ptr;
    msg.pipe_tr = (KHCI_TR_STRUCT_PTR) tr_ptr;
    
    KHCI_DEBUG_LOG('i', msg.type, msg.pipe_desc->G.ENDPOINT_NUMBER, msg.pipe_tr->G.RX_LENGTH)
    if (MQX_OK != _lwmsgq_send(usb_host_ptr->tr_que, (uint_32 *) &msg, 0)) {
        status = USBERR_TR_FAILED;
    }
    else{
        _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_TR_ARRIVE);
    }
    
    return status;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_cancel
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to cancel the transfer
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_cancel(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_ptr, TR_STRUCT_PTR tr_ptr) {
    USB_KHCI_HOST_STATE_STRUCT_PTR usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) handle;
    KHCI_TR_STRUCT_PTR pKTR                     = (KHCI_TR_STRUCT_PTR) tr_ptr;
    TR_MSG_STRUCT msg;
    
    if(pKTR->G.FLAG & TR_CANCEL_ASYNC){
        pKTR->cancel_flag = 1;
    }
    else
    {
        // remove messages from KHCI msg que
        while (_lwmsgq_receive(usb_host_ptr->tr_que, (uint_32 *) &msg, 0, 0, 0) == MQX_OK)
        { 
            KHCI_TR_STRUCT_PTR tempKTR = msg.pipe_tr;;

            if (tempKTR->G.CALLBACK != NULL) 
                tempKTR->G.CALLBACK((pointer)tempKTR->pipe_desc, tempKTR->G.CALLBACK_PARAM, 0, 0,  USBERR_DEVICE_NOT_FOUND);
        }

        /* need procect?  if run time cancel!  not sure this cancel will take valid */

        if(pKTR != NULL) {
            if(pKTR->list) {
                if((&pKTR->node) == usb_host_ptr->CTLBULKStartNode){
                    usb_host_ptr->CTLBULKStartNode = pKTR->node.next_ptr;
                }

                if (pKTR->G.CALLBACK != NULL) 
                    pKTR->G.CALLBACK((pointer)pKTR->pipe_desc, pKTR->G.CALLBACK_PARAM, 0, 0,  USBERR_DEVICE_NOT_FOUND);

                _mqx_dll_remove_node(pKTR->list, &pKTR->node);
            }

            pKTR->G.TR_INDEX   = 0;
            pKTR->G.FLAG       = 0;
            pKTR->list         = NULL;
            pKTR->pipe_desc    = NULL;
            pKTR->type         = TR_MSG_UNKNOWN;
            pKTR->usb_host_ptr = NULL;
            pKTR->state        = KHCI_TR_START;
            pKTR->offset       = 0;
            pKTR->cancel_flag  = 0;
        }
    }

    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_bus_control
*  Returned Value : error or USB_OK
*  Comments       :
*        The function for USB bus control
*        bus_control value:
*                0: Reset 
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_bus_control(_usb_host_handle handle, uint_8 bus_control) {
    USB_KHCI_HOST_STATE_STRUCT_PTR  usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR) handle;

    if(bus_control == 0){
        _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_ATTACH);
        _lwevent_set(&usb_host_ptr->khci_event, KHCI_EVENT_DETACH);
    }
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_get_frame_number
*  Returned Value : frame number
*  Comments       :
*        The function to get frame number
*END*-----------------------------------------------------------------*/
uint_32 _usb_khci_get_frame_number(_usb_host_handle handle) {
    USB_HOST_STATE_STRUCT_PTR  usb_host_ptr = (USB_HOST_STATE_STRUCT_PTR) handle;
    USB_MemMapPtr usb_ptr = (USB_MemMapPtr) usb_host_ptr->DEV_PTR;

    return (usb_ptr->FRMNUMH << 8 | usb_ptr->FRMNUML);
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_process_tr_complete
*  Returned Value : none
*  Comments       :
*        Transaction complete
*END*-----------------------------------------------------------------*/
static void _usb_khci_process_tr_complete(
    KHCI_PIPE_STRUCT_PTR       pipe_desc_ptr,
    KHCI_TR_STRUCT_PTR         pipe_tr_ptr,
    uint_32                    required,
    uint_32                    remaining,
    int_32                     err
)
{
    uchar_ptr buffer_ptr = NULL;
    uint_32 status = 0;
    
    KHCI_DEBUG_LOG('o', TR_MSG_UNKNOWN, pipe_desc_ptr->G.ENDPOINT_NUMBER, err)

    if (err == KHCI_ATOM_TR_STALL) {
        status = USBERR_ENDPOINT_STALLED;
    }
    else
    if (err == KHCI_ATOM_TR_TO) {
        status = USBERR_TR_TO;
    }
    else if ((err == KHCI_ATOM_TR_NAK) || (err >= 0)) {
        status = USB_OK;
        
#if 0
        if ((remaining > 0) || (err == KHCI_ATOM_TR_NAK)) //remaining bytes
            status = USBERR_TR_FAILED;
#endif
#if 0
        if ( (err ==  KHCI_ATOM_TR_NAK) &&  remaining > 0) 
            status = USBERR_TR_SHORT;
        else
            if(err == KHCI_ATOM_TR_NAK)
                status = USBERR_TR_FAILED;
        //else 
#endif
        if(remaining > 0)
            status = USBERR_TR_SHORT;

        if (pipe_desc_ptr->G.PIPETYPE == USB_CONTROL_PIPE) {
            if (pipe_tr_ptr->G.SEND_PHASE) {
                buffer_ptr = pipe_tr_ptr->G.TX_BUFFER;
                pipe_tr_ptr->G.SEND_PHASE = FALSE;
            }
            else {
                buffer_ptr = pipe_tr_ptr->G.RX_BUFFER;
            }
        }
        else {
            if (pipe_desc_ptr->G.DIRECTION) {
                buffer_ptr = pipe_tr_ptr->G.TX_BUFFER;
            }
            else {
                buffer_ptr = pipe_tr_ptr->G.RX_BUFFER;
            }
        }
    }
    else if (err < 0)
        status = USBERR_TR_FAILED;

#if 1  /* liuadd */	
		if (err == KHCI_ATOM_TR_CANCEL) {	
			status = USBERR_DEVICE_NOT_FOUND;
		}
 #endif

    pipe_tr_ptr->status = USB_STATUS_IDLE;
    
    if (pipe_tr_ptr->status == USB_STATUS_IDLE) {
        /* Transfer done. Call the callback function for this 
        ** transaction if there is one (usually true). 
        */
        if (pipe_tr_ptr->G.CALLBACK != NULL) {
            pipe_tr_ptr->G.CALLBACK((pointer)pipe_desc_ptr, pipe_tr_ptr->G.CALLBACK_PARAM,
                buffer_ptr, required - remaining, status);
                
            /* If the application enqueued another request on this pipe 
            ** in this callback context then it will be at the end of the list
            */
        }
    }

    if(!(pipe_tr_ptr->G.FLAG & TR_PERIOD_KEEP)) {
    /* don't use pipe anymore */
        pipe_tr_ptr->G.TR_INDEX   = 0;
        pipe_tr_ptr->G.FLAG       = 0;
        pipe_tr_ptr->list         = NULL;
        pipe_tr_ptr->pipe_desc    = NULL;
        pipe_tr_ptr->type         = TR_MSG_UNKNOWN;
        pipe_tr_ptr->usb_host_ptr = NULL;
        pipe_tr_ptr->state        = KHCI_TR_START;
        pipe_tr_ptr->offset       = 0;
        pipe_tr_ptr->cancel_flag  = 0;
    }
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_atom_tr
*  Returned Value : 
*  Comments       :
*        Atomic transaction
*END*-----------------------------------------------------------------*/
static int_32 _usb_khci_atom_tr(
    USB_KHCI_HOST_STATE_STRUCT_PTR  usb_host_ptr,
    uint_32                    type,
    KHCI_PIPE_STRUCT_PTR       pipe_desc_ptr,
    uint_8                    *buf,
    uint_32                    len
)
{
    USB_MemMapPtr usb_ptr = (USB_MemMapPtr) usb_host_ptr->G.DEV_PTR;
    uint_32 bd;
    uint_32 *bd_ptr = NULL;
    int_32 res;
    _mqx_int retry;
    _mqx_int delay_const = 1;
    uint_32  align4    = 0;
    uint_8  *alignbuf  = NULL;
    usb_host_ptr->last_to_pipe = NULL; // at the beginning, consider that there was not timeout
    uint_32 threshholdlen = 0;

    align4 =  ((uint_32)buf % 4) == 0;
    

    if(align4 == 0){
        printf("_usb_khci_atom_tr buf not aligh with 4 \n");
        alignbuf = _mem_alloc_align((len + 4), 4);

        if(alignbuf == NULL){
            printf("alloc align buf failed \n");
            return KHCI_ATOM_TR_ALLOC_MEM_ERROR;
        }

        if (type == TR_IN){
        }
        else{
            memcpy(alignbuf, buf, len);
        }
    }
    else{
        alignbuf = buf;
    }
    
    len = (len > pipe_desc_ptr->G.MAX_PACKET_SIZE) ? pipe_desc_ptr->G.MAX_PACKET_SIZE : len;
    
    /* ADDR must be written before ENDPT0 (undocumented behavior) for to generate PRE packet */
    usb_ptr->ADDR = (uint_8)((pipe_desc_ptr->G.SPEED == USB_SPEED_FULL) ? USB_ADDR_ADDR(pipe_desc_ptr->G.DEVICE_ADDRESS) : USB_ADDR_LSEN_MASK | USB_ADDR_ADDR(pipe_desc_ptr->G.DEVICE_ADDRESS));
    
    if((pipe_desc_ptr->G.PIPETYPE != USB_ISOCHRONOUS_PIPE))
    {
        usb_ptr->ENDPOINT[0].ENDPT =
            (usb_host_ptr->G.SPEED == USB_SPEED_LOW ? USB_ENDPT_HOSTWOHUB_MASK : 0) | USB_ENDPT_RETRYDIS_MASK |
            USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPRXEN_MASK | USB_ENDPT_EPHSHK_MASK;
    }
    else
    {
        usb_ptr->ENDPOINT[0].ENDPT =
            (usb_host_ptr->G.SPEED == USB_SPEED_LOW ? USB_ENDPT_HOSTWOHUB_MASK : 0) | USB_ENDPT_RETRYDIS_MASK |
            USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPRXEN_MASK;
    }

#if 0
    switch (pipe_desc_ptr->G.PIPETYPE) {
        case USB_INTERRUPT_PIPE:
        case USB_ISOCHRONOUS_PIPE:
            retry = 0;
            break;
        case USB_BULK_PIPE:
            if(pipe_desc_ptr->G.DIRECTION == USB_RECV)
                retry = 0;
            else
                retry = pipe_desc_ptr->G.NAK_COUNT; // set retry count - do not retry interrupt transaction
            break;
        default:
            retry = pipe_desc_ptr->G.NAK_COUNT; // set retry count - do not retry interrupt transaction
            break;
    }
#endif

    //retry = pipe_desc_ptr->G.NAK_COUNT; // set retry count - do not retry interrupt transaction
    retry = 0;
    
    do {
        res = 0;
        
#if 1
        // wait for USB ready, but with timeout
        while (usb_ptr->CTL & USB_CTL_TXSUSPENDTOKENBUSY_MASK) {
            //printf("wait for USB ready, but with timeout ? \n");
            if (_lwevent_wait_ticks(&usb_host_ptr->khci_event, KHCI_EVENT_MASK, FALSE,  2 * USBCFG_KHCI_WAIT_TICK) == MQX_OK) {  /* this value must twice or more as system tick */
                //printf("wait for USB ready time out, reset ! \n");
                res = KHCI_ATOM_TR_RESET;
                break;
            }
        }
#endif
        
        if (!res) {
          // all is ok, do transaction

#if defined(KHCICFG_BASIC_SCHEDULING)
          usb_ptr->ISTAT |= USB_ISTAT_SOFTOK_MASK; //clear SOF
          while (!(usb_ptr->ISTAT & USB_ISTAT_SOFTOK_MASK))
                 /* wait for next SOF */;

          usb_ptr->SOFTHLD = 0;
#else
#if 1
          if (pipe_desc_ptr->G.SPEED == USB_SPEED_FULL)
            threshholdlen = len * 7 / 6 + KHCICFG_THSLD_DELAY;
          else
            threshholdlen = len * 12 * 7 / 6 + KHCICFG_THSLD_DELAY;
#endif
          if(threshholdlen > 255)
              threshholdlen = 255;

          usb_ptr->SOFTHLD = (uint_8)threshholdlen; 
#endif          
          usb_ptr->ISTAT |= USB_ISTAT_SOFTOK_MASK; //clear SOF
          usb_ptr->ERRSTAT = 0xff; //clear error status

          switch (type) {
            case TR_CTRL:    
                bd_ptr = (uint_32*) BD_PTR(0, 1, usb_host_ptr->tx_bd);
                *(bd_ptr + 1) = HOST_TO_LE_LONG((uint_32)alignbuf);
                *bd_ptr = HOST_TO_LE_LONG(USB_BD_BC(len) | USB_BD_OWN);
                usb_ptr->TOKEN = (uint_8)(USB_TOKEN_TOKENENDPT((uint_8)pipe_desc_ptr->G.ENDPOINT_NUMBER) | USB_TOKEN_TOKENPID_SETUP);
                usb_host_ptr->tx_bd ^= 1;
                break;
            case TR_IN:
                bd_ptr = (uint_32*) BD_PTR(0, 0, usb_host_ptr->rx_bd);
                *(bd_ptr + 1) = HOST_TO_LE_LONG((uint_32)alignbuf);
                *bd_ptr = HOST_TO_LE_LONG(USB_BD_BC(len) | USB_BD_OWN | USB_BD_DATA01(pipe_desc_ptr->G.NEXTDATA01));
                usb_ptr->TOKEN = (uint_8)(USB_TOKEN_TOKENENDPT(pipe_desc_ptr->G.ENDPOINT_NUMBER) | USB_TOKEN_TOKENPID_IN);
                usb_host_ptr->rx_bd ^= 1;
                break;
            case TR_OUT:
                bd_ptr = (uint_32*) BD_PTR(0, 1, usb_host_ptr->tx_bd);
                *(bd_ptr + 1) = HOST_TO_LE_LONG((uint_32)alignbuf);
                *bd_ptr = HOST_TO_LE_LONG(USB_BD_BC(len) | USB_BD_OWN | USB_BD_DATA01(pipe_desc_ptr->G.NEXTDATA01));
                usb_ptr->TOKEN = (uint_8)(USB_TOKEN_TOKENENDPT(pipe_desc_ptr->G.ENDPOINT_NUMBER) | USB_TOKEN_TOKENPID_OUT);
                usb_host_ptr->tx_bd ^= 1;
                break;                
            default:
                bd_ptr = NULL;
          }

          /* clear SOF TOK firstly or place this clear later ? */
          //_lwevent_clear(&usb_host_ptr->khci_event, KHCI_EVENT_SOF_TOK);

          // wait for end of transaction or other event, max. 100 msec
          //if (LWEVENT_WAIT_TIMEOUT == _lwevent_wait_ticks(&usb_host_ptr->khci_event, KHCI_EVENT_MASK, FALSE, 100 * USBCFG_KHCI_WAIT_TICK))
          if (LWEVENT_WAIT_TIMEOUT == _lwevent_wait_ticks(&usb_host_ptr->khci_event, KHCI_EVENT_WITHOUTSOF_MASK, FALSE, 2 * USBCFG_KHCI_WAIT_TICK))
              /* this value must twice or more as system tick */
          {
                printf("wait for end of transaction or other event \n");
                res = KHCI_ATOM_TR_TO;
                usb_host_ptr->last_to_pipe = pipe_desc_ptr; // remember this pipe as it had timeout last time
                continue;
          }
        }
        
try_again:
        // check for reset event (detach)
        if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_RESET) {
            res = KHCI_ATOM_TR_RESET;
            break;
        }

#if 0
        // check for SOF event 
        if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_SOF_TOK) {
            printf("SOF event receive! \n ");
            _lwevent_clear(&usb_host_ptr->khci_event, KHCI_EVENT_SOF_TOK);
            res = KHCI_ATOM_TR_SOF_TOK;
            break;
        }
#endif
    
        if (bd_ptr != NULL && (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_TOK_DONE)) {
            // transaction finished
            _lwevent_clear(&usb_host_ptr->khci_event, KHCI_EVENT_TOK_DONE);

            // check result
            bd = LONG_LE_TO_HOST(*bd_ptr);
            
            if (usb_ptr->ERRSTAT & (
                    USB_ERREN_PIDERREN_MASK |
#if defined(KHCICFG_BASIC_SCHEDULING)
                    USB_ERREN_CRC5EOFEN_MASK |
#endif                   
                    USB_ERREN_CRC16EN_MASK |
                    USB_ERREN_DFN8EN_MASK |
//                    USB_ERREN_BTO_ERR_EN_MASK | //timeout tested elsewhere
                    USB_ERREN_DMAERREN_MASK |
                    USB_ERREN_BTSERREN_MASK))
            {
#if defined(KHCICFG_BASIC_SCHEDULING)
                if (usb_ptr->ERRSTAT & USB_ERREN_CRC5EOFEN_MASK)
                    retry = 0;
#endif                   
                res = -usb_ptr->ERRSTAT;
                break;
            }
            else 
            {
                switch (bd >> 2 & 0xf) {
                    case 0x03:  // DATA0
                    case 0x0b:  // DATA1
                    case 0x02:  // ACK
                        retry = 0;
                        res = (bd >> 16) & 0x3ff;
                        pipe_desc_ptr->G.NEXTDATA01 ^= 1;     // switch data toggle
                        break;

                    case 0x0e:  // STALL
                        res = KHCI_ATOM_TR_STALL;
                        retry = 0;
                        break;

                    case 0x0a:  // NAK
                        res = KHCI_ATOM_TR_NAK;
#if 0
                        if (retry)
                            _time_delay(delay_const * (pipe_desc_ptr->G.NAK_COUNT - retry));
#endif
                        break;

                    case 0x00:  // bus timeout
                        if((pipe_desc_ptr->G.PIPETYPE != USB_ISOCHRONOUS_PIPE))
                        {
                            //wait a bit, but not too much, perhaps some error occurs on the bus
                            //if (LWEVENT_WAIT_TIMEOUT == _lwevent_wait_ticks(&usb_host_ptr->khci_event, KHCI_EVENT_MASK, FALSE, delay_const)) 
#if 0

                            if((pipe_desc_ptr->G.DIRECTION == USB_RECV) && (pipe_desc_ptr->G.PIPETYPE == USB_BULK_PIPE)) {
                                res = KHCI_ATOM_TR_NAK;
                                break;
                            }
#endif

                            //printf("bus timeout check ! \n");
                            if (LWEVENT_WAIT_TIMEOUT == _lwevent_wait_ticks(&usb_host_ptr->khci_event, KHCI_EVENT_WITHOUTSOF_MASK, FALSE, 2 * USBCFG_KHCI_WAIT_TICK ))  /* 10ms */
                            {
                                printf("bus timeout \n");
#if 0
                                // check for SOF event 
                                if (usb_host_ptr->khci_event.VALUE & KHCI_EVENT_SOF_TOK) {
                                    printf("SOF event receive while bus timeout! ");
                                    _lwevent_clear(&usb_host_ptr->khci_event, KHCI_EVENT_SOF_TOK);
                                }
#endif

                                res = KHCI_ATOM_TR_TO;
                                usb_host_ptr->last_to_pipe = pipe_desc_ptr; // remember this pipe as it had timeout last time

#if 0
                                if (retry)
                                    _time_delay(delay_const * (pipe_desc_ptr->G.NAK_COUNT - retry));
#endif
                            }
                            else {
                                goto try_again;
                            }
                        }
                        else
                        {
                            retry = 0;
                            res = (bd >> 16) & 0x3ff;
                            pipe_desc_ptr->G.NEXTDATA01 ^= 1;     /* switch data toggle */                        
                        }
                        break;

                    case 0x0f:  // data error
                        //if this event happens during enumeration, then return means not finished enumeration
                        res = KHCI_ATOM_TR_DATA_ERROR;
#if 0
                        _time_delay(delay_const * (pipe_desc_ptr->G.NAK_COUNT - retry));
#endif
                        break;
                        
                    default:
                        break;
                }
            }
        }
    } while (retry--);
    

    if(align4 == 0){
        if(alignbuf != NULL){
            if(res > 0){
                if(type == TR_IN){
                    memcpy(buf, alignbuf, res);
                }
            }
            _mem_free(alignbuf);
        }
    }
    return res;
}

/*FUNCTION*-------------------------------------------------------------
*
*  Function Name  : _usb_khci_host_close_pipe
*  Returned Value : error or USB_OK
*  Comments       :
*        The function to close pipe
*END*-----------------------------------------------------------------*/
USB_STATUS _usb_khci_host_close_pipe(_usb_host_handle handle, PIPE_STRUCT_PTR pipe_desc_ptr) {
    USB_KHCI_HOST_STATE_STRUCT_PTR  usb_host_ptr = (USB_KHCI_HOST_STATE_STRUCT_PTR)handle;
    TR_STRUCT_PTR                            pTr =  pipe_desc_ptr->tr_list_ptr;

    if (pipe_desc_ptr->PIPETYPE == USB_INTERRUPT_PIPE) {
        _mqx_int i;
        TR_INT_QUE_ITM_STRUCT_PTR tr = usb_host_ptr->tr_int_que;

        // find record
        for (i = 0; i < USBCFG_KHCI_MAX_INT_TR; i++, tr++) {
            if (tr->msg.pipe_desc == (KHCI_PIPE_STRUCT_PTR) pipe_desc_ptr)
                tr->msg.type = TR_MSG_UNKNOWN;    
        }
    }
    else {
        /* Now, we should check whether there is a transfer in the message pool */
    }

    return USB_OK;
}
