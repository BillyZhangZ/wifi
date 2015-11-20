/**HEADER*******************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
**************************************************************************** 
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
****************************************************************************
*
* Comments:
*
*
*END************************************************************************/

#include "adk20.h"

#ifndef CONNECT_TO_APK
#include "aoa_task.h"
extern  struct _usb_audio_host_info_		* audio_host_info;
#endif

#ifdef USE_ACC_HID
#ifdef TR_CALLBACK_COMPLETE
extern int usb_hid_callback
#else
extern
void usb_hid_callback
#endif
   (
      /* [IN] pointer to pipe */
      _usb_pipe_handle  pipe_handle,

      /* [IN] user-defined parameter */
      void *           user_parm,

      /* [IN] buffer address */
      uchar_ptr         buffer,

      /* [IN] length of data transferred */
      uint32_t           length_data_transfered,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      USB_STATUS        status
   );
#endif

#ifdef ADK_2012 // the apk is adk2012
static void putLE32(uint8_t * buf, uint16_t * idx, uint32_t val);
static void putLE16(uint8_t * buf, uint16_t * idx, uint32_t val);
static void readSettings(AdkSettings * Settings);
void writeSettings(AdkSettings * Settings);
void setVolume(uint8_t vol);
void rtcSet(uint16_t year, uint8_t  month, uint8_t  day, uint8_t  hour, uint8_t minute, uint8_t  second);
static uint16_t  adkProcessCommand(struct _usb_accessoy_host_info_ * acc_info, uint8_t  cmd, 
                  const uint8_t * dataIn, uint16_t sz, char fromBT, uint8_t * reply, uint16_t  maxReplySz);
#endif

// bus power pin
#define VBUSPIN              (GPIO_PORT_C | GPIO_PIN9)
#define VBUS_MUX_GPIO         LWGPIO_MUX_C9_GPIO   

static inline void init_set_vbus_low(LWGPIO_STRUCT * vbus){

    //LWGPIO_STRUCT vbus;
    _time_delay(50);

    /* initialize lwgpio handle vbus for PORTB 8 pin */
    if (!lwgpio_init(vbus, VBUSPIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing VBUS PORTB 8 as GPIO output failed.\n");
        _task_block();
    }
    /* swich pin functionality (MUX) to GPIO mode */
    lwgpio_set_functionality(vbus, VBUS_MUX_GPIO);

    /* write logical 0 to the pin */
    lwgpio_set_value(vbus, LWGPIO_VALUE_LOW); /* set pin to 0 */
}

static inline void delay_set_vbus_high(LWGPIO_STRUCT * vbus) {
    _time_delay(150);
    /* write logical 1 to the pin */
    lwgpio_set_value(vbus, LWGPIO_VALUE_HIGH); /* set pin to 1 */
    _time_delay(200);
}


#ifdef TR_CALLBACK_COMPLETE
int usb_try_accessory_callback
#else
void usb_try_accessory_callback
#endif
   (
      /* [IN] pointer to pipe */
      _usb_pipe_handle  pipe_handle,

      /* [IN] user-defined parameter */
      void *           user_parm,

      /* [IN] buffer address */
      unsigned char *         buffer,

      /* [IN] length of data transferred */
      uint32_t           length_data_transfered,

      /* [IN] status, hopefully USB_OK or USB_DONE */
      USB_STATUS        status
   )
{ /* Body */
    struct _usb_accessoy_host_info_  * acc_host_info = (struct _usb_accessoy_host_info_  *)user_parm;
    if(status == USB_OK) {
        _lwevent_set(&(acc_host_info->acc_event), USBH_TRY_AOA_CALLBACK_EVENT);
        acc_host_info->try_acc_status++; // this function will be called by accessory hid .
        printf("try accessory callback ok\n");
    }
    else
        printf("try accessory callback fail\n");

#if 1
    if(acc_host_info->try_acc_status >= WAITING_ACCESSORY) {
        LWGPIO_STRUCT  vbus;
        init_set_vbus_low(&vbus);
        delay_set_vbus_high(&vbus);
    }
#endif    

#ifdef TR_CALLBACK_COMPLETE
    return TR_CALLBACK_COMPLETE;
#endif
}

#ifdef TR_CALLBACK_COMPLETE
static int usb_host_aoa_in_tr_callback
#else
static void usb_host_aoa_in_tr_callback
#endif
(  /* device to host */
    /* [IN] pointer to pipe */
    _usb_pipe_handle pipe_handle,
    
    /* [IN] user-defined parameter */
    void * user_parm,
    
    /* [IN] buffer address */
    unsigned char * buffer,
    
    /* [IN] length of data transferred */
    uint32_t  buflen,
    
    /* [IN] status, hopefully USB_OK or USB_DONE */
    uint32_t status )
{
    struct _usb_accessoy_host_info_  * acc_host_info = (struct _usb_accessoy_host_info_  *)user_parm;
#ifdef ACCESSORY_PRINTF
    printf("receive status=%d, buff len %d\n",status,buflen);
#endif

    if(status == USB_OK) {
        acc_host_info->received_size = buflen;
        _lwevent_set(&(acc_host_info->acc_event),USBH_AOA_RECV_COMP_EVENT);
    }
    else {
        acc_host_info->received_size = 0;
        _lwevent_set(&(acc_host_info->acc_event),USBH_AOA_RECV_FAIL_EVENT);
    }
    
#ifdef TR_CALLBACK_COMPLETE
    return TR_CALLBACK_COMPLETE;
#endif
    
}

#ifdef TR_CALLBACK_COMPLETE
static int usb_host_aoa_out_tr_callback
#else
static void usb_host_aoa_out_tr_callback
#endif
(  /* host to device */
    /* [IN] pointer to pipe */
    _usb_pipe_handle pipe_handle,
    
    /* [IN] user-defined parameter */
    void * user_parm,
    
    /* [IN] buffer address */
    unsigned char * buffer,
    
    /* [IN] length of data transferred */
    uint32_t buflen,
    
    /* [IN] status, hopefully USB_OK or USB_DONE */
    uint32_t status)
{
#ifdef ACCESSORY_PRINTF
    printf("send status=%d, buff len %d\n",status,buflen);
#endif
    struct _usb_accessoy_host_info_  * acc_host_info = (struct _usb_accessoy_host_info_  *)user_parm;

    if(status == USB_OK) {
        acc_host_info->sent_size = buflen;
        _lwevent_set(&(acc_host_info->acc_event),USBH_AOA_SEND_COMP_EVENT);
    }
    else {
        acc_host_info->sent_size = 0;
        _lwevent_set(&(acc_host_info->acc_event),USBH_AOA_SEND_FAIL_EVENT);
    }
#ifdef TR_CALLBACK_COMPLETE
    return TR_CALLBACK_COMPLETE;
#endif
}

static int acc_start_recv(struct _usb_accessoy_host_info_ * acc_host_info,unsigned int len)  /* device to host */
{
    acc_host_info->received_size  = 0;
    usb_aoa_recv_data(
        (CLASS_CALL_STRUCT_PTR)&(acc_host_info->acc_device.CLASS_INTF),
        usb_host_aoa_in_tr_callback,
        acc_host_info, /*MAX_ADK_RECV_SIZE*/len,
        (unsigned char *)acc_host_info->receiveBuf);

    return 0;
}

static int acc_start_send(struct _usb_accessoy_host_info_ * acc_host_info, unsigned int len)   /* host to device */
{
	acc_host_info->sent_size  = 0;
	usb_aoa_send_data(
	                    (CLASS_CALL_STRUCT_PTR)&(acc_host_info->acc_device.CLASS_INTF),
	                    usb_host_aoa_out_tr_callback,
	                    acc_host_info, len,
	                    (unsigned char *)acc_host_info->reply); 
         return 0;
}

void usbh_acc_task(uint32_t param)
{
    struct _usb_accessoy_host_info_  * acc_host_info = (struct _usb_accessoy_host_info_  *)param;
    _mqx_int        app_event_value;

    printf("usbh_acc_task start\n");
    while(1) {
        if (_lwevent_wait_ticks(&acc_host_info->acc_event,USBH_AOA_ALL_EVENTS_MASK, FALSE, 0) != MQX_OK) {
            printf("waiting accessory event fail\n");
            // return /*USBERR_ERROR*/;
            continue;
        }
        else {
            app_event_value = _lwevent_get_signalled();
        }

        if(app_event_value  & /* USBH_AOA_DETACH_EVENT */USBH_AOA_STOP_TASK_EVENT) {
            acc_host_info->acc_task_state = USBH_AOA_TASK_STATE_STOP;
            acc_host_info->adk_status = WATIING_TRY_ACC;
            printf("acc task stop adk status %d\n",acc_host_info->adk_status);
            return;
        }

        if( app_event_value  &  USBH_AOA_DETACH_EVENT ) {
            acc_host_info->adk_status = WATIING_TRY_ACC;
            printf("acc task detach at adk status %d\n",acc_host_info->adk_status);
            //return;  /* Can't return ,because it is detach,not plug out! */
        }

        switch(acc_host_info->adk_status) {
        case WATIING_TRY_ACC:
        {
            printf("event %d in WATIING_TRY_ACC\n",app_event_value);
            if(/* acc_host_info->acc_event.VALUE */app_event_value & USBH_TRY_AOA_START_EVENT) {
                acc_host_info->adk_status = TRYING_ACC;
                acc_host_info->try_acc_status = GET_PROTOCOL_VERSION;
                /* start the first acc try */
                
                if(usb_device_try_accessory( acc_host_info->acc_device.DEV_HANDLE,acc_host_info->acc_device.INTF_HANDLE,
                        usb_try_accessory_callback,acc_host_info,acc_host_info->try_acc_status) != USB_OK) {
                    acc_host_info->adk_status = WATIING_TRY_ACC;
                    printf("usb_device_try_accessory fail1\n");
                    break;
                }				
                //acc_host_info->try_acc_status ++;
                _time_delay(5);
            }		
            else if (app_event_value & USBH_AOA_INTERFACED_EVENT) {
#ifdef USE_ACC_HID                
#ifdef CONNECT_TO_APK
                usb_device_hid_regist(acc_host_info->acc_device.DEV_HANDLE,acc_host_info->acc_device.INTF_HANDLE,usb_hid_callback, sizeof(hid_description));
                usb_device_hid_set_report_descriptor(acc_host_info->acc_device.DEV_HANDLE,acc_host_info->acc_device.INTF_HANDLE,usb_hid_callback);
#else
                usb_device_hid_regist(audio_host_info->audio_stream.DEV_HANDLE,audio_host_info->audio_stream.INTF_HANDLE,usb_hid_callback, sizeof(hid_description));
                usb_device_hid_set_report_descriptor(audio_host_info->audio_stream.DEV_HANDLE,audio_host_info->audio_stream.INTF_HANDLE,usb_hid_callback);
#endif
#endif
                acc_host_info->adk_status = ADK_READING;
#ifdef ADK_2012 // the apk is adk2012
                readSettings(&acc_host_info->settings);
                acc_start_recv(acc_host_info,MAX_ADK_RECV_SIZE);
#endif                
            }
            else {
                /* fixme ,how to process */
                acc_host_info->adk_status = WATIING_TRY_ACC;
                printf("Invalid event %d in WATIING_TRY_ACC\n",app_event_value);
            }
        }
        break;
        case TRYING_ACC:
        {
            if(app_event_value & USBH_TRY_AOA_CALLBACK_EVENT) {
                if(acc_host_info->try_acc_status != WAITING_ACCESSORY) {
                    if(usb_device_try_accessory( acc_host_info->acc_device.DEV_HANDLE,acc_host_info->acc_device.INTF_HANDLE,
                            usb_try_accessory_callback,acc_host_info,acc_host_info->try_acc_status)!= USB_OK) {
                        acc_host_info->adk_status = WATIING_TRY_ACC;
                        printf("usb_device_try_accessory fail\n");
                        break;
                    }
                    //acc_host_info->try_acc_status ++;			
                    _time_delay(5);
                }
            }
            else if (app_event_value & USBH_AOA_INTERFACED_EVENT) {
#ifdef USE_ACC_HID
#ifdef CONNECT_TO_APK
                usb_device_hid_regist(acc_host_info->acc_device.DEV_HANDLE,acc_host_info->acc_device.INTF_HANDLE,usb_hid_callback, sizeof(hid_description));
                usb_device_hid_set_report_descriptor(acc_host_info->acc_device.DEV_HANDLE,acc_host_info->acc_device.INTF_HANDLE,usb_hid_callback);
#else
                usb_device_hid_regist(audio_host_info->audio_stream.DEV_HANDLE,audio_host_info->audio_stream.INTF_HANDLE,usb_hid_callback, sizeof(hid_description));
                usb_device_hid_set_report_descriptor(audio_host_info->audio_stream.DEV_HANDLE,audio_host_info->audio_stream.INTF_HANDLE,usb_hid_callback);
#endif        
#endif				
                acc_host_info->adk_status = ADK_READING;
#ifdef ADK_2012 // the apk is adk2012
            readSettings(&acc_host_info->settings);
            acc_start_recv(acc_host_info,MAX_ADK_RECV_SIZE);
#endif                
            }
            else {
                /* fixme */
                acc_host_info->adk_status = WATIING_TRY_ACC;
                printf("Invalid event %d in TRYING_ACC\n",app_event_value);
            }
        }
        break;
        case ADK_READING:
        {
#ifndef ADK_2012
            _time_delay(10);
            break;
#else
            if(app_event_value & USBH_AOA_RECV_COMP_EVENT) {
                if (acc_host_info->received_size >= 4) {
                    uint16_t replylen;
                    uint8_t  cmd = acc_host_info->receiveBuf[0];
                    uint8_t  seq = acc_host_info->receiveBuf[1];
                    uint16_t size = acc_host_info->receiveBuf[2] | acc_host_info->receiveBuf[3] << 8;

                    if (size + 4 > acc_host_info->received_size) {
                        // short packet
                        //return;
                        printf("short packet\n");
                    }
                    else {
                        replylen = adkProcessCommand(acc_host_info,cmd, acc_host_info->receiveBuf + 4, size, 0, acc_host_info->reply + 4, MAX_PACKET_SZ - 4);
                        if (replylen > 0) {
                            acc_host_info->reply[0] = cmd | CMD_MASK_REPLY;
                            acc_host_info->reply[1] = seq;
                            acc_host_info->reply[2] = replylen;
                            acc_host_info->reply[3] = replylen >> 8;
                            replylen += 4;
                            pr_debug("ADK: USB: sending %d bytes\n", replylen);
                            _time_delay(1);  /* add for ISO workaround */
                            acc_host_info->adk_status = ADK_WRITING;
                            acc_start_send(acc_host_info,MAX_ADK_RECV_SIZE);
                        }
                    }
                }
            }
            else if(app_event_value & USBH_AOA_RECV_FAIL_EVENT) {
                _time_delay(1);
                //printf("USBH_AOA_RECV_FAIL_EVENT,retry\n");
                readSettings(&acc_host_info->settings);
                acc_host_info->adk_status = ADK_READING;
                acc_start_recv(acc_host_info,MAX_ADK_RECV_SIZE);
            }
            else {
                printf("Invalid event %d in ADK_READING\n",app_event_value);
            }
#endif            
        }
        break;
        case ADK_WRITING:
        {
#ifdef ADK_2012 // the apk is adk2012            
            if(app_event_value & USBH_AOA_SEND_COMP_EVENT) {
                readSettings(&acc_host_info->settings);
                acc_host_info->adk_status = ADK_READING;
                acc_start_recv(acc_host_info,MAX_ADK_RECV_SIZE);
            }
            else if(app_event_value & USBH_AOA_SEND_FAIL_EVENT){
                acc_host_info->adk_status = ADK_WRITING;
                acc_start_send(acc_host_info,MAX_ADK_RECV_SIZE);
            }
            else {
                printf("Invalid event %d in ADK_WRITING\n",app_event_value);
            }
#endif            
        }
        break;
        default:
            printf("Invalid event %d",app_event_value);
        } /* end switch */
    } /* end while */
}

#ifdef ADK_2012 // the apk is adk2012
static void putLE32(uint8_t * buf, uint16_t * idx, uint32_t val) {

  buf[(*idx)++] = val;
  buf[(*idx)++] = val >> 8;
  buf[(*idx)++] = val >> 16;
  buf[(*idx)++] = val >> 24;
}

static void putLE16(uint8_t * buf, uint16_t * idx, uint32_t val) {

  buf[(*idx)++] = val;
  buf[(*idx)++] = val >> 8;
}

static void readSettings(AdkSettings * Settings) {
   
   //apply defaults
   strcpy(Settings->btName, "ADK 2012");
   strcpy(Settings->btPIN, "1337");
   Settings->magix = SETTINGS_MAGIX;
   Settings->ver = 1;

   Settings->speed = 1;
   Settings->displayMode = AdkShowAnimation;
   Settings->almSnooze = 10 * 60;	//10-minute alarm
   strcpy(Settings->almTune, "Alarm_Rooster_02.ogg");
#ifdef ACCESSORY_PRINTF   
   printf("ADK: settings: read dummy\n");
#endif
}

void writeSettings(AdkSettings * Settings){
	//printf("ADK: settings: write dummy\n");
#ifdef ACCESSORY_PRINTF
 	printf(" R=%d,G=%d,B=%d\n",  Settings->R , Settings->G,  Settings->B);
#endif
}

void setVolume(uint_8 vol){
	/* volume = vol; */
#ifdef ACCESSORY_PRINTF
    printf("setVolume %d\n",vol);
#endif
}

void rtcSet(uint_16 year, uint_8 month, uint_8 day, uint_8 hour, uint_8 minute, uint_8 second){
    printf("year=%d, month=%d, day=%d, hour=%d, minute=%d, second=%d,",year, month, day, hour, minute, second);
}

static uint_16 adkProcessCommand(struct _usb_accessoy_host_info_ * acc_info,uint8_t cmd, 
    const uint8_t * dataIn, uint16_t  sz, char fromBT, uint8_t * reply, uint16_t maxReplySz)
{  //returns num bytes to reply with (or 0 for no reply)
  uint16_t sendSz = 0;

#ifdef ACCESSORY_PRINTF
  printf("ADK: BT: have cmd 0x%x with %db of data\n", cmd, sz);
#endif
  
  //process packet
  switch(cmd){
	case BT_CMD_GET_PROTO_VERSION:
      {
        acc_info->reply[sendSz++] = BT_PROTO_VERSION_CURRENT;
      }
      break;
      
    case BT_CMD_GET_SENSORS:
      {   
        putLE32(acc_info->reply, &sendSz, acc_info->dummy_sensor.hTemp);
        putLE32(acc_info->reply, &sendSz, acc_info->dummy_sensor.hHum);
        putLE32(acc_info->reply, &sendSz, acc_info->dummy_sensor.bPress);
        putLE32(acc_info->reply, &sendSz, acc_info->dummy_sensor.bTemp);
        putLE16(acc_info->reply, &sendSz, acc_info->dummy_sensor.prox[0]);
        putLE16(acc_info->reply, &sendSz, acc_info->dummy_sensor.prox[1]);
        putLE16(acc_info->reply, &sendSz, acc_info->dummy_sensor.prox[3]);
        putLE16(acc_info->reply, &sendSz, acc_info->dummy_sensor.prox[4]);
        putLE16(acc_info->reply, &sendSz, acc_info->dummy_sensor.prox[5]);
        putLE16(acc_info->reply, &sendSz, acc_info->dummy_sensor.prox[2]);
        putLE16(acc_info->reply, &sendSz, acc_info->dummy_sensor.prox[6]);
        putLE16(acc_info->reply, &sendSz, acc_info->dummy_sensor.accel[0]);
        putLE16(acc_info->reply, &sendSz, acc_info->dummy_sensor.accel[1]);
        putLE16(acc_info->reply, &sendSz, acc_info->dummy_sensor.accel[2]);
        putLE16(acc_info->reply, &sendSz, acc_info->dummy_sensor.mag[0]);
        putLE16(acc_info->reply, &sendSz, acc_info->dummy_sensor.mag[1]);
        putLE16(acc_info->reply, &sendSz, acc_info->dummy_sensor.mag[2]);
      }
      break;

    case BT_CMD_FILE_LIST:
      {
      
        acc_info->reply[sendSz++] = 0;
      }
      break;
       
    case BT_CMD_FILE_DELETE:
      {
        acc_info->reply[sendSz++] = 0;
      }
      break;
       
    case BT_CMD_FILE_OPEN:
      {
       acc_info->reply[sendSz++] = 0;
      }
      break;
       
    case BT_CMD_FILE_WRITE:
      {acc_info->reply[sendSz++] = 0;}
      break;
       
    case BT_CMD_FILE_CLOSE:
      {
        acc_info->reply[sendSz++] = 0;
      }
      break;
       
    case BT_CMD_GET_UNIQ_ID:
      {
        putLE32(acc_info->reply, &sendSz, 0);
        putLE32(acc_info->reply, &sendSz, 1);
        putLE32(acc_info->reply, &sendSz, 2);
        putLE32(acc_info->reply, &sendSz, 3);
      }
      break;
      
    case BT_CMD_BT_NAME:
      {
        if(sz){  //set
          strcpy(acc_info->settings.btName, (char*)dataIn);
          acc_info->reply[sendSz++] = 1;
          writeSettings(&acc_info->settings);
        }
        else{
          strcpy((char*)acc_info->reply, acc_info->settings.btName);
          sendSz = strlen((char*)acc_info->reply) + 1;
        }
      }
      break;
      
    case BT_CMD_BT_PIN:
      {
        if(sz){  //set
          strcpy(acc_info->settings.btPIN, (char*)dataIn);
          acc_info->reply[sendSz++] = 1;
          writeSettings(&acc_info->settings);
        }
        else{
          strcpy((char*)acc_info->reply, acc_info->settings.btPIN);
          sendSz = strlen((char*)acc_info->reply) + 1;
        }
      }
      break;
    case BT_CMD_TIME:
      {
        if (sz >= 7) {  //set
          rtcSet(dataIn[1] << 8 | dataIn[0], dataIn[2], dataIn[3], dataIn[4], dataIn[5], dataIn[6]);
          acc_info->reply[sendSz++] = 1;
        } else if (sz == 0) {
          acc_info->reply[0] = 0;acc_info->reply[2]=0;acc_info->reply[3]=0;
          acc_info->reply[4]=0;acc_info->reply[5]=0; acc_info->reply[6]=0;
          sendSz += 7;
        }
      }
      break;
    case BT_CMD_SETTINGS:
      {
        if (sz >= 8) {  //set
          acc_info->settings.almH = dataIn[0];
          acc_info->settings.almM = dataIn[1];
          acc_info->settings.almOn = dataIn[2];
          acc_info->settings.bri = dataIn[3];
          acc_info->settings.R = dataIn[4];
          acc_info->settings.G = dataIn[5];
          acc_info->settings.B = dataIn[6];
          acc_info->settings.vol = dataIn[7];
          setVolume(acc_info->settings.vol);
          writeSettings(&acc_info->settings);
          reply[sendSz++] = 1;
        } else {  /*  get setting */
          acc_info->reply[sendSz++] = acc_info->settings.almH;
          acc_info->reply[sendSz++] = acc_info->settings.almM;
          acc_info->reply[sendSz++] = acc_info->settings.almOn;
          acc_info->reply[sendSz++] = acc_info->settings.bri;
          acc_info->reply[sendSz++] = acc_info->settings.R;
          acc_info->reply[sendSz++] = acc_info->settings.G;
          acc_info->reply[sendSz++] = acc_info->settings.B;
          acc_info->reply[sendSz++] = acc_info->settings.vol;
        }
      }
      break;
    case BT_CMD_ALARM_FILE:
      {
        if(sz){  //set
          strcpy(acc_info->settings.almTune, (char*)dataIn);
          acc_info->reply[sendSz++] = 1;
          writeSettings(&acc_info->settings);
        } else{
          strcpy((char*)acc_info->reply, acc_info->settings.almTune);
          sendSz = strlen((char*)acc_info->reply) + 1;
        }
      }
      break;
    case BT_CMD_GET_LICENSE:
      {
          acc_info->reply[sendSz++] = 0;
      }
      break;
    case BT_CMD_DISPLAY_MODE:
      {
        if (sz) {  //set
          acc_info->settings.displayMode = dataIn[0];
          acc_info->reply[sendSz++] = 1;
        } else if (sz == 0) {
          acc_info->reply[sendSz++] = acc_info->settings.displayMode;
        }
      }
      break;
    case BT_CMD_LOCK:
      {
        if (sz) {  //set
          acc_info->dummy_sensor.locked = dataIn[0] ? 2 : 0;
          acc_info->reply[sendSz++] = 1;
        } else if (sz == 0) {
          acc_info->reply[sendSz++] = acc_info->dummy_sensor.locked;
        }
      }
      break;
  }
  return sendSz;
}

#endif 

/*
void int_service_routine(void *pin)
{
    lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);
	Settings->vol += 5;
	if(Settings->vol >= 255)
		Settings->vol =0;
    printf("btn1 press\n");
}*/

