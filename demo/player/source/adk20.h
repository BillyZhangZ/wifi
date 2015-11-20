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

#ifndef __ADK20_H__
#define __ADK20_H__

#include "usb_host_aoa.h"

#define CONNECT_TO_APK

/*	add for ADK 2.0 protocol */
#define MAX_PACKET_SZ                       260  //256b payload + header
#define MAX_ADK_RECV_SIZE					64 //MAX_PACKET_SZ  /* if more than 64 ,must rewite usb shecdule process or app slipt the big packet ! */

#define CMD_MASK_REPLY                      0x80
#define BT_CMD_GET_PROTO_VERSION            1    // () -> (u8 protocolVersion)
#define BT_CMD_GET_SENSORS                  2    // () -> (sensors: i32,i32,i32,i32,u16,u16,u16,u16,u16,u16,u16,i16,i16,i16,i16,i16,i16) 
#define BT_CMD_FILE_LIST                    3    // FIRST: (char name[]) -> (fileinfo or single zero byte)   OR   NONLATER: () -> (fileinfo or empty or single zero byte)
#define BT_CMD_FILE_DELETE                  4    // (char name[0-255)) -> (char success)
#define BT_CMD_FILE_OPEN                    5    // (char name[0-255]) -> (char success)
#define BT_CMD_FILE_WRITE                   6    // (u8 data[]) -> (char success)
#define BT_CMD_FILE_CLOSE                   7    // () -> (char success)
#define BT_CMD_GET_UNIQ_ID                  8    // () -> (u8 uniq[16])
#define BT_CMD_BT_NAME                      9    // (char name[]) -> () OR () -> (char name[])
#define BT_CMD_BT_PIN                      10    // (char PIN[]) -> () OR () -> (char PIN[])
#define BT_CMD_TIME                        11    // (timespec) -> (char success)) OR () > (timespec)
#define BT_CMD_SETTINGS                    12    // () -> (alarm:u8,u8,u8,brightness:u8,color:u8,u8,u8:volume:u8) or (alarm:u8,u8,u8,brightness:u8,color:u8,u8,u8:volume:u8) > (char success)
#define BT_CMD_ALARM_FILE                  13    // () -> (char file[0-255]) OR (char file[0-255]) > (char success)
#define BT_CMD_GET_LICENSE                 14    // () -> (u8 licensechunk[]) OR () if last sent
#define BT_CMD_DISPLAY_MODE                15    // () -> (u8) OR (u8) -> ()
#define BT_CMD_LOCK                        16    // () -> (u8) OR (u8) -> ()

#define BT_PROTO_VERSION_1                  1    //this line marks the end of v1.0 API, all things after this are the next version
//constants
#define BT_PROTO_VERSION_CURRENT            BT_PROTO_VERSION_1

#define ADK_BT_SSP_DONE_VAL		0x0FF00000

#define SETTINGS_NAME    "/AdkSettings.bin"   //   not use sdcard
#define SETTINGS_MAGIX   0xAF

enum AdkStates{
  AdkClock,
  AdkAlarm,
  AdkBrightness,
  AdkColor,
  AdkVolume,
  AdkDisplay,
  AdkPresets,
  
  //always last
  AdkInvalid
};

enum AdkDisplayMode{
  AdkShowAnimation,
  AdkShowAccel,
  AdkShowMag,
  AdkShowTemp,
  AdkShowHygro,
  AdkShowBaro,
  AdkShowProx,
  AdkShowColor,

  AdkShowLast	//always last
};

enum AdkAlarmStates {
  AdkAlarmIdle,
  AdkAlarmIdleWait,
  AdkAlarmAlarm,
  AdkAlarmSnooze
};

typedef struct _sensor_data_ {
	int32_t hTemp;
	int32_t  hHum;
	int32_t bPress;
	int32_t bTemp;
	uint16_t prox[7];			//prox,  clear, IR ,R, G, B, temp pProx, pClear, pR, pG, pB, pIR, pTemp;
	uint16_t proxNormalized[3];
	int16_t accel[3];
	int16_t mag[3];
	uint32_t btSSP;
	char locked;
} sensor_data;

typedef struct _AdkSettings_{
  uint8_t  magix;
  uint8_t  ver;
  uint8_t R, G, B, bri, vol, almH, almM, almOn;
  char btName[16]; //null terminated
  char btPIN[17];  //null terminated
  uint16_t almSnooze;
  char almTune[32]; // null terminated
  
  uint8_t speed, displayMode;
//later settings
} AdkSettings;

/* ADK 2.0 protocol end */

/* ADK status */
enum _adk_status_
{
	WATIING_TRY_ACC,
	TRYING_ACC,
	ADK_WRITING,
	ADK_READING,
};
//typedef enum _acc_status_ acc_status;

typedef struct accessory_device_struct {
   uint32_t                          DEV_STATE;  /* Attach/detach state */
   _usb_device_instance_handle      DEV_HANDLE;
   _usb_interface_descriptor_handle INTF_HANDLE;
   CLASS_CALL_STRUCT                CLASS_INTF; /* Class-specific info */
} ACCESSORY_DEVICE_STRUCT,  * ACCESSORY_DEVICE_STRUCT_PTR;

#define USBH_TRY_AOA_START_EVENT			(1 << 0)
#define USBH_TRY_AOA_CALLBACK_EVENT			(1 << 1)
#define USBH_AOA_INTERFACED_EVENT			(1 << 2)
#define USBH_AOA_RECV_COMP_EVENT			(1 << 3)
#define USBH_AOA_RECV_FAIL_EVENT			(1 << 4)
#define USBH_AOA_SEND_COMP_EVENT			(1 << 5)
#define USBH_AOA_SEND_FAIL_EVENT			(1 << 6)
#define USBH_AOA_DETACH_EVENT				(1 << 7)
#define USBH_AOA_STOP_TASK_EVENT               (1<< 8)
#define USBH_AOA_ALL_EVENTS_MASK   (USBH_TRY_AOA_START_EVENT | USBH_TRY_AOA_CALLBACK_EVENT | \
									USBH_AOA_RECV_COMP_EVENT | USBH_AOA_SEND_COMP_EVENT | \
									USBH_AOA_RECV_FAIL_EVENT | USBH_AOA_SEND_FAIL_EVENT | \
									USBH_AOA_INTERFACED_EVENT | USBH_AOA_DETACH_EVENT | USBH_AOA_STOP_TASK_EVENT)

// usb accesory task parameters
#define USB_AOA_TASK_TEMPLATE_INDEX       0
#define USB_AOA_TASK_ADDRESS              usbh_acc_task
#define USB_AOA_TASK_STACKSIZE            1024
#define USB_AOA_TASK_NAME                 "usb_acc Task1"
#define USB_AOA_TASK_ATTRIBUTES           0
#define USB_AOA_TASK_CREATION_PARAMETER   0
#define USB_AOA_TASK_DEFAULT_TIME_SLICE   0
#define USBCFG_AOA_TASK_PRIORITY          10  /* task priority must less than khci task !*/

#define USBH_AOA_TASK_STATE_STOP           0
#define USBH_AOA_TASK_STATE_READY          1

struct _usb_accessoy_host_info_ {
	/* accessory device */
	ACCESSORY_DEVICE_STRUCT    acc_device;
	LWEVENT_STRUCT		acc_event;
	
	int16_t try_acc_status;
	int8_t adk_status;

	int8_t acc_task_state;
	_task_id usb_acc_task_id;

	AdkSettings		settings;
	/* dummy sensor */
	sensor_data dummy_sensor;
	//uint8_t volume = 255;

	uint8_t * receiveBuf;      /* MAX_PACKET_SZ */
	int received_size;
	uint8_t * reply;               
	int sent_size;
};

/*
void usb_host_accessory_event(_usb_device_instance_handle,
   _usb_interface_descriptor_handle, uint32_t); */

void usbh_acc_task(uint32_t param);

#endif
