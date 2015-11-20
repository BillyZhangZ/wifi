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
*END************************************************************************/

#include "main.h"
#include "usb_classes.h"

#include <lwmsgq.h>

#define ADK_INTERNAL 1

#include "fwk.h"
#include "ADK.h"
#include "btHFP.h"
#include "HCI.h"
#include "fio.h"
#include "msi.h"
#include "usb_bluetooth_task.h"


extern void usb_host_cdc_acm_event(_usb_device_instance_handle, _usb_interface_descriptor_handle intf_handle, uint_32);
extern void usb_host_cdc_data_event(_usb_device_instance_handle, _usb_interface_descriptor_handle intf_handle, uint_32);
extern void usb_host_hid_event(_usb_device_instance_handle, _usb_interface_descriptor_handle intf_handle, uint_32);
extern void usb_host_msd_event(_usb_device_instance_handle, _usb_interface_descriptor_handle intf_handle, uint_32);

static void usb_host_bluetooth_event(_usb_device_instance_handle, _usb_interface_descriptor_handle intf_handle, uint_32);

#if 0
typedef struct bluetooth_device_struct {
   /* This must be the first member of this structure, because sometimes we can use it
   ** as pointer to CLASS_CALL_STRUCT, other time as a pointer to BLUETOOTH_DEVICE_STRUCT
   */
   CLASS_CALL_STRUCT                ccs;
} BLUETOOTH_DEVICE_STRUCT,  _PTR_ BLUETOOTH_DEVICE_STRUCT_PTR;
#endif

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
#ifdef USBCLASS_INC_HUB
    /* USB 1.1 hub */
    {
        {0x00,0x00},                  /* Vendor ID per USB-IF             */
        {0x00,0x00},                  /* Product ID per manufacturer      */
        USB_CLASS_HUB,                /* Class code                       */
        USB_SUBCLASS_HUB_NONE,        /* Sub-Class code                   */
        USB_PROTOCOL_HUB_LS,          /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_hub_device_event     /* Application call back function   */
    },
#endif
#ifdef USBCLASS_INC_CDC
   /* Abstract control class */
   {
        {0x00, 0x00},                 /* Vendor ID per USB-IF             */
        {0x00, 0x00},                 /* Product ID per manufacturer      */
        USB_CLASS_COMMUNICATION,      /* Class code                       */
        USB_SUBCLASS_COM_ABSTRACT,    /* Sub-Class code                   */
        0xFF,                         /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_cdc_acm_event        /* Application call back function   */
   },
   /* Data flow class */
   {
        {0x00, 0x00},                 /* Vendor ID per USB-IF             */
        {0x00, 0x00},                 /* Product ID per manufacturer      */
        USB_CLASS_DATA,               /* Class code                       */
        0xFF,                         /* Sub-Class code                   */
        0xFF,                         /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_cdc_data_event       /* Application call back function   */
    },
#endif
#if 0
    /* Floppy drive */
    {
        {0x00, 0x00},                 /* Vendor ID per USB-IF             */
        {0x00, 0x00},                 /* Product ID per manufacturer      */
        USB_CLASS_MASS_STORAGE,       /* Class code                       */
        USB_SUBCLASS_MASS_UFI,        /* Sub-Class code                   */
        USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_msd_event            /* Application call back function   */
    },
    /* USB 2.0 hard drive */
    {
        {0x00, 0x00},                 /* Vendor ID per USB-IF             */
        {0x00, 0x00},                 /* Product ID per manufacturer      */
        USB_CLASS_MASS_STORAGE,       /* Class code                       */
        USB_SUBCLASS_MASS_SCSI,       /* Sub-Class code                   */
        USB_PROTOCOL_MASS_BULK,       /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_msd_event            /* Application call back function   */
    },
#endif
//#ifdef USBCLASS_INC_HID
#if 0
    /* Keyboard HID */
    {
        {0x00, 0x00},                 /* Vendor ID per USB-IF             */
        {0x00, 0x00},                 /* Product ID per manufacturer      */
        USB_CLASS_HID,                /* Class code                       */
        USB_SUBCLASS_HID_BOOT,        /* Sub-Class code                   */
        USB_PROTOCOL_HID_KEYBOARD,    /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_hid_event            /* Application call back function   */
    },
    /* Mouse HID */
    {
        {0x00, 0x00},                 /* Vendor ID per USB-IF             */
        {0x00, 0x00},                 /* Product ID per manufacturer      */
        USB_CLASS_HID,                /* Class code                       */
        USB_SUBCLASS_HID_BOOT,        /* Sub-Class code                   */
        USB_PROTOCOL_HID_MOUSE,       /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_hid_event            /* Application call back function   */
    },
#endif
#ifdef USBCLASS_INC_PHDC
    {
        {0x00, 0x00},                 /* Vendor ID per USB-IF           */
        {0x00, 0x00},                 /* Product ID per manufacturer    */
        USB_CLASS_PHDC,               /* Class code (PHDC)              */
        0,                            /* Sub-Class code (PHDC does not assign a subclass)   */
        0,                            /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_phdc_manager_event   /* Application call back function   */
    },
#endif
#ifdef USBCLASS_INC_BLUETOOTH
    {
        {0x00, 0x00},                 /* Vendor ID per USB-IF             */
        {0x00, 0x00},                 /* Product ID per manufacturer      */
        USB_CLASS_BLUETOOTH,          /* Class code                       */
        USB_SUBCLASS_BLUETOOTH,       /* Sub-Class code                   */
        USB_PROTOCOL_BLUETOOTH,       /* Protocol                         */
        0,                            /* Reserved                         */
        usb_host_bluetooth_event      /* Application call back function   */
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


static void *btHfpCallback(void *userparam, void *param){
    printf("btHfpCallback status is %d \n", (int)(param));
    return NULL;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usbh_init
* Returned Value : None
* Comments       :
*     Initializes USB host
*END*--------------------------------------------------------------------*/


int_32  Shell_bt(int_32 argc, char_ptr argv[] ) {

    boolean              print_usage, shorthelp = FALSE;

    print_usage = Shell_check_help_request(argc, argv, &shorthelp );

    if (!print_usage)  {
        if(argc < 3) {
            print_usage = TRUE;
        }
        else {

            if(!strcmp(argv[1], "ct")){

                BTAVRCPCTKEY key = AVRCPCTUNKNOW;

                if(!strcmp(argv[2], "next")){
                    key =  AVRCPCTNEXT;
                }else
                if(!strcmp(argv[2], "pre")){
                    key =  AVRCPCTPREVIOUS;
                }else
                if(!strcmp(argv[2], "up")){
                    key =  AVRCPCTVOLUP;
                }else
                if(!strcmp(argv[2], "down")){
                    key =  AVRCPCTVOLDOWN;
                }else
                if(!strcmp(argv[2], "play")){
                    key =  AVRCPCTPLAY;
                }else
                if(!strcmp(argv[2], "pause")){
                    key =  AVRCPCTPAUSE;
                }else
                if(!strcmp(argv[2], "stop")){
                    key =  AVRCPCTSTOP;
                }else
                    print_usage = TRUE;

                if(key != AVRCPCTUNKNOW) {
                    avrcpServicePassthroughHandle(key, AVRCPCTPRESS);
#if 0
                    _time_delay(60);
                    avrcpServicePassthroughHandle(key, AVRCPCTRELEASE);
#endif

                }
            } /* ct */
            else
            if(!strcmp(argv[1], "hfp")){

                if(!strcmp(argv[2], "call")){
                    if(argc == 4){
                        btHfpCall((uint8_t *) argv[3], btHfpCallback, NULL);
                    }
                } /* call */

                if(!strcmp(argv[2], "A")){
                    btHfpAnswer(btHfpCallback, NULL);
                } /* answer */

                if(!strcmp(argv[2], "H")){
                    btHfpReject(btHfpCallback, NULL);
                } /* Reject */

                if(!strcmp(argv[2], "L")){
                    btHfpLastNumReDial(btHfpCallback, NULL);
                } /* re dial last number */

                if(!strcmp(argv[2], "SU")){
                    btHfpSpeakerVolUpSync(btHfpCallback, NULL);
                } /* speaker vol up sync */

                if(!strcmp(argv[2], "SD")){
                    btHfpSpeakerVolDownSync(btHfpCallback, NULL);
                } /* speaker vol down sync */

                if(!strcmp(argv[2], "ND")){
                     btHfpNRECDisable(btHfpCallback, NULL);
                } /* NREC disable */
            } /* hfp */
            else
                print_usage = TRUE;
        }

    }

    if (print_usage)  {
        if(shorthelp) {
            printf("%s ct  <command> \n", argv[0]);
            printf("%s hfp <command> \n", argv[0]);
        }else {
            printf("%s ct  <command> \n", argv[0]);
            printf("ct command: \n"
                    "  next: next     track \n"
                    "  pre:  previous track \n"
                    "  up:   volume up \n"
                    "  down: volume down \n"
                    "  play: play track \n"
                    "  pause:pause track \n"
                    "  stop: stop track \n"
                    "\n");
            printf("%s hfp <command> \n", argv[0]);
            printf("hfp command: \n"
                    "  call \"phonenumber\" \n"
                    "  A     Answer call   \n"
                    "  H     Reject call   \n"
                    "  L     Redial last number   \n"
                    "  SU    Speaker vol up   \n"
                    "  SD    Speaker vol down   \n"
                    "  ND    NRECDisable   \n"
                    );
        }
    }
    return 0;
}

#if 0
void usbh_init()
{
    USB_STATUS status = USB_OK;
   
    _int_disable();
    _usb_host_driver_install(USBCFG_DEFAULT_HOST_CONTROLLER);

    status = _usb_host_init
        (USBCFG_DEFAULT_HOST_CONTROLLER,   /* Use value in header file */
        &host_handle);             /* Returned pointer */
    if (status != USB_OK) 
    {
        printf("USB Host Initialization failed. STATUS: %x\n", status);
        _int_enable();
        _task_block();
    }

    /*
    ** Since we are going to act as the host driver, register the driver
    ** information for wanted class/subclass/protocols
    */
    status = _usb_host_driver_info_register(host_handle, DriverInfoTable);
    if (status != USB_OK) 
    {
        printf("Driver Registration failed. STATUS: %x\n", status);
        _int_enable();
        _task_block();
    }

    _int_enable();

    printf("\nMQX USB\nWaiting for USB device to be attached...\n");
    fflush(stdout);

    /* If a device will be attached, handler in usbh_<class>_handler.c file
    ** will catch that event
    */
}   
#endif




/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : usb_host_event
* Returned Value : None
* Comments       :
*     Called when a device is attached / detached, but not recognized by
*     any class driver.
*END*--------------------------------------------------------------------*/

static void usb_host_bluetooth_event
   (
      /* [IN] pointer to device instance */
      _usb_device_instance_handle      dev_handle,

      /* [IN] pointer to interface descriptor */
      _usb_interface_descriptor_handle intf_handle,

      /* [IN] code number for event causing callback */
      uint_32                          event_code
   )
{
   BLUETOOTH_DEVICE_STRUCT_PTR          device;
   usb_msg_t                  msg;
   INTERFACE_DESCRIPTOR_PTR             intf;

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
         device = (BLUETOOTH_DEVICE_STRUCT_PTR) _mem_alloc_zero(sizeof(BLUETOOTH_DEVICE_STRUCT));
         if (device == NULL)
            break;

         if (MQX_OK != _lwevent_create(&device->bluetooth_event, LWEVENT_AUTO_CLEAR)) {
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
         if (USB_OK != usb_class_bluetooth_get_app(dev_handle, intf_handle, (CLASS_CALL_STRUCT_PTR *) &device)) {
            printf("usb_class_bluetooth_get_app failed ! \n");
            break;
         }

         if (MQX_OK != usb_class_bluetooth_use_lwevent(&device->ccs, &device->bluetooth_event)) {
            printf("usb_class_bluetooth_use_lwevent failed ! \n");
             break;
         }

         if (MQX_OK != usb_class_bluetooth_use_lwmsgq(&device->ccs, device->bluetooth_taskq)) {
            printf("usb_class_bluetooth_use_lwmsgq failed ! \n");
             break;
         }

         msg.ccs = &device->ccs;
         msg.body = USB_EVENT_INTF;
         if (LWMSGQ_FULL == _lwmsgq_send(usb_taskq, (uint_32 *) &msg, 0)) {
            printf("Could not inform USB task about device interfaced\n");
         }
         break;

      case USB_DETACH_EVENT:
         if (USB_OK != usb_class_bluetooth_get_app(dev_handle, intf_handle, (CLASS_CALL_STRUCT_PTR *) &device))
            break;

         printf("USB_DETACH_EVENT \n");

         fclose(device->handle);
         usb_class_bluetooth_uninstall_driver(&device->ccs);

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

//////////// bt support (boring)

volatile static uint32_t btSSP = ADK_BT_SSP_DONE_VAL;
const char* btPIN = 0;
char btName[64];

#define  maxPairedDevices 4
static uint8_t numPairedDevices = 0;
static uint8_t savedMac[maxPairedDevices][BLUETOOTH_MAC_SIZE];
static uint8_t savedKey[maxPairedDevices][BLUETOOTH_LINK_KEY_SIZE];

static void adkBtSspF(const uint8_t* mac, uint32_t val){

  btSSP = val;
  printf("ssp with val %u\n", val);
}

static char adkBtConnectionRequest(const uint8_t* mac, uint32_t devClass, uint8_t linkType){	//return 1 to accept

    printf("Accepting connection from %02X:%02X:%02X:%02X:%02X:%02X LinkType:%s\n", 
               mac[5], mac[4], mac[3], mac[2], mac[1], mac[0],
               linkType == 0 ? "SCO" : (linkType == 1 ? "ACL" :(linkType == 2 ? "eSCO" : "Reserved"))
               );
    return 1;
}

static char adkBtLinkKeyRequest(const uint8_t* mac, uint8_t* buf){ //link key create

  uint8_t i, j;
  
  printf("Key request from %02X:%02X:%02X:%02X:%02X:%02X\n  -> ", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);

  for(i = 0; i < numPairedDevices; i++){
 
    for(j = 0; j < BLUETOOTH_MAC_SIZE && savedMac[i][j] == mac[j]; j++);
    if(j == BLUETOOTH_MAC_SIZE){ //match
    
        printf("{");
        for(j = 0; j < BLUETOOTH_LINK_KEY_SIZE; j++){
         
          printf(" %02X", savedKey[i][j]);
          buf[j] = savedKey[i][j];
        }
        printf(" }");
        return 1;
    }
  }
  printf("FAIL\n");
  return 0;
}

static void adkBtLinkKeyCreated(const uint8_t* mac, const uint8_t* buf){ 	//link key was just created, save it if you want it later

   uint8_t j;
   
   printf("Key created for %02X:%02X:%02X:%02X:%02X:%02X <- {", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
   
   for(j = 0; j < BLUETOOTH_LINK_KEY_SIZE; j++){
         
     printf(" %02X", buf[j]);
   }
   printf(" }\n");
    
   if(numPairedDevices < maxPairedDevices){
      
      for(j = 0; j < BLUETOOTH_LINK_KEY_SIZE; j++) savedKey[numPairedDevices][j] = buf[j];
      for(j = 0; j < BLUETOOTH_MAC_SIZE; j++) savedMac[numPairedDevices][j] = mac[j];
      numPairedDevices++;
      printf("saved to slot %d/%d\n", numPairedDevices, maxPairedDevices);
   }
   else{
      printf("out of slots...discaring\n");
   }
}

static char adkBtPinRequest(const uint8_t* mac, uint8_t* buf){		//fill buff with PIN code, return num bytes used (16 max) return 0 to decline

   uint8_t v, i = 0;

   printf("PIN request from %02X:%02X:%02X:%02X:%02X:%02X\n  -> ", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
   
   if(btPIN){
     printf(" -> using pin '%s'\n", btPIN);
     for(i = 0; btPIN[i]; i++) buf[i] = btPIN[i];
     return i;
   }
   else printf(" no PIN set. rejecting\n");
   return 0;
}

#define MAGIX	0xFA

#if 0
static uint8_t sdpDescrADK[] =
{
        //service class ID list
        SDP_ITEM_DESC(SDP_TYPE_UINT, SDP_SZ_2), 0x00, 0x01, SDP_ITEM_DESC(SDP_TYPE_ARRAY, SDP_SZ_u8), 17,
            SDP_ITEM_DESC(SDP_TYPE_UUID, SDP_SZ_16), BT_ADK_UUID,
        //ServiceId
        SDP_ITEM_DESC(SDP_TYPE_UINT, SDP_SZ_2), 0x00, 0x03, SDP_ITEM_DESC(SDP_TYPE_UUID, SDP_SZ_2), 0x11, 0x01,
        //ProtocolDescriptorList
        SDP_ITEM_DESC(SDP_TYPE_UINT, SDP_SZ_2), 0x00, 0x04, SDP_ITEM_DESC(SDP_TYPE_ARRAY, SDP_SZ_u8), 15,
            SDP_ITEM_DESC(SDP_TYPE_ARRAY, SDP_SZ_u8), 6,
                SDP_ITEM_DESC(SDP_TYPE_UUID, SDP_SZ_2), 0x01, 0x00, // L2CAP
                SDP_ITEM_DESC(SDP_TYPE_UINT, SDP_SZ_2), L2CAP_PSM_RFCOMM >> 8, L2CAP_PSM_RFCOMM & 0xFF, // L2CAP PSM
            SDP_ITEM_DESC(SDP_TYPE_ARRAY, SDP_SZ_u8), 5,
                SDP_ITEM_DESC(SDP_TYPE_UUID, SDP_SZ_2), 0x00, 0x03, // RFCOMM
                SDP_ITEM_DESC(SDP_TYPE_UINT, SDP_SZ_1), MAGIX, // port ###
        //browse group list
        SDP_ITEM_DESC(SDP_TYPE_UINT, SDP_SZ_2), 0x00, 0x05, SDP_ITEM_DESC(SDP_TYPE_ARRAY, SDP_SZ_u8), 3,
            SDP_ITEM_DESC(SDP_TYPE_UUID, SDP_SZ_2), 0x10, 0x02, // Public Browse Group
        //name
        SDP_ITEM_DESC(SDP_TYPE_UINT, SDP_SZ_2), 0x01, 0x00, SDP_ITEM_DESC(SDP_TYPE_TEXT, SDP_SZ_u8), 12, 'A', 'D', 'K', ' ', 'B', 'T', ' ', 'C', 'O', 'M', 'M', 'S'
};
#endif


static void btStart(){
    uint8_t i, dlci, mac[BT_MAC_SIZE];
    int f;
  
    ADK_btEnable(adkBtConnectionRequest, adkBtLinkKeyRequest, adkBtLinkKeyCreated, adkBtPinRequest, NULL);

#if 0
    dlci = ADK_btRfcommReserveDlci(RFCOMM_DLCI_NEED_EVEN);

    if(!dlci) printf("BTADK: failed to allocate DLCI\n");
    else{

        //change descriptor to be valid...
        for(i = 0, f = -1; i < sizeof(sdpDescrADK); i++){

            if(sdpDescrADK[i] == MAGIX){
                if(f == -1) f = i;
                else break;
            }
        }

        if(i != sizeof(sdpDescrADK) || f == -1){

            printf("BTADK: failed to find a single marker in descriptor\n");
            ADK_btRfcommReleaseDlci(dlci);
            return;
        }

        sdpDescrADK[f] = dlci >> 1;

        printf("BTADK has DLCI %u\n", dlci);

        ADK_btRfcommRegisterPort(dlci, btAdkPortOpen, btAdkPortClose, btAdkPortRx);
        ADK_btSdpServiceDescriptorAdd(sdpDescrADK, sizeof(sdpDescrADK));
    }
#endif

  numPairedDevices = 0;
  memset(savedMac, 0, sizeof savedMac);
  memset(savedKey, 0, sizeof savedKey);

  btPIN = "0000";
  if(ADK_adkbtLocalMac(mac)){
      snprintf(btName, sizeof btName, "FSLAccessory:%02x%02x", mac[1], mac[0]);
  }else{
      snprintf(btName, sizeof btName, "FSLAccessory?");
  }

  printf("ADK: setting BT name '%s' and pin '%s'\n", btName, btPIN);
  if(!ADK_btSetDeviceClass(DEVICE_CLASS_SERVICE_AUDIO | DEVICE_CLASS_SERVICE_RENDERING |
		DEVICE_CLASS_SERVICE_INFORMATION | (DEVICE_CLASS_MAJOR_AV << DEVICE_CLASS_MAJOR_SHIFT) |
		(DEVICE_CLASS_MINOR_AV_PORTBL_AUDIO << DEVICE_CLASS_MINOR_AV_SHIFT))) printf("ADK: Failed to set device class\n");
  if(!ADK_btSetLocalName(btName)) printf("ADK: failed to set BT name\n");
  if(!ADK_btDiscoverable(1)) printf("ADK: Failed to set discoverable\n");
  if(!ADK_btConnectable(1)) printf("ADK: Failed to set connectable\n");
  ADK_btSetSspCallback(adkBtSspF);
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : USB_bluetooth_task
* Returned Value : None
* Comments       :
*     First function called. This rouine just transfers control to host main
*END*--------------------------------------------------------------------*/

void USB_bluetooth_task(uint_32 param)
{ 
    USB_STATUS           error;
    usb_msg_t            msg;
    /* Store mounting point used. A: is the first one, bit #0 assigned, Z: is the last one, bit #25 assigned */
    uint_32              fs_mountp = 0;
    struct FWKFUNCS      _fwkFuncs;
   
#if 0
#if DEMOCFG_USE_POOLS && defined(DEMOCFG_MFS_POOL_ADDR) && defined(DEMOCFG_MFS_POOL_SIZE)
    _MFS_pool_id = _mem_create_pool((pointer)DEMOCFG_MFS_POOL_ADDR, DEMOCFG_MFS_POOL_SIZE);
#endif
#endif

    /* This event will inform other tasks that the filesystem on USB was successfully installed */
    _lwsem_create(&USB_Stick, 0);
    
    if (MQX_OK != _lwmsgq_init(usb_taskq, 20, USB_TASKQ_GRANM)) {
        // lwmsgq_init failed
        _task_block();
    }


    memset(&_fwkFuncs, 0, sizeof _fwkFuncs);
   // _fwkFuncs.dacFuncs.init_with_periodbuffer  =  msi_snd_init_with_periodbuffer;
   // _fwkFuncs.dacFuncs.init        =  msi_snd_init;
   // _fwkFuncs.dacFuncs.deinit      =  msi_snd_deinit;
    _fwkFuncs.taskpriority           =  10;
    _fwkFuncs.dacFuncs.setSampleRate =  msi_snd_set_format;
    _fwkFuncs.dacFuncs.tryAddBuffer  =  msi_snd_write;
    _fwkFuncs.dacFuncs.vol_up        =  msi_snd_vol_up;
    _fwkFuncs.dacFuncs.vol_down      =  msi_snd_vol_down;
    _fwkFuncs.dacFuncs.mute          =  msi_snd_mute;
    _fwkFuncs.dacFuncs.unmute        =  msi_snd_umute;
    _fwkFuncs.dacFuncs.flush         =  msi_snd_flush;


    ADK_adkInit(_fwkFuncs);



    for (;;) {
        /* Wait for event sent as a message */
        _lwmsgq_receive(&usb_taskq, (_mqx_max_type *) &msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
         
        printf("bluetooth msg.body is %d \n", msg.body);

        if (msg.body == USB_EVENT_ATTACH) {
          /* This event is not so important, because it does not inform about successfull USB stack enumeration */
        } else if (msg.body == USB_EVENT_INTF )  { 
            int_32  error;
            //printf("USB_EVENT_INTF \n");
            error =  usb_class_bluetooth_install_driver(msg.ccs, "USBBT:");
            if(error == MQX_OK) {
                MQX_FILE_PTR _handle = NULL;
                BLUETOOTH_DEVICE_STRUCT_PTR dsp = (BLUETOOTH_DEVICE_STRUCT_PTR) msg.ccs;

                msi_snd_init_with_periodbuffer(1024, 4);

                _handle = fopen("USBBT:", 0);
                
                if(_handle != NULL) {
                    dsp->handle = _handle;
                    if (ADK_btInit(_handle, 1) >= 0) {
                        btStart();
                    }
                    else
                        printf("ADK_btInit failed \n");
                }
                else{
                    printf("Open USBBT: failed! \n");
                }

                _lwsem_post(&USB_Stick);
            }
            else {
                printf("usb_class_bluetooth_install_driver failed error is %d \n", error);
            }

        } else if (msg.body == USB_EVENT_DETACH) {
            BLUETOOTH_DEVICE_STRUCT_PTR dsp = (BLUETOOTH_DEVICE_STRUCT_PTR) msg.ccs;

            printf("Call ADK_btDeinit \n");

            ADK_btDeinit();
            _lwevent_destroy(&dsp->bluetooth_event);
            /* lw msg queue need destory ? */
            //memset(dsp->bluetooth_taskq, 0, sizeof dsp->bluetooth_taskq);
            _lwmsgq_deinit(dsp->bluetooth_taskq);
            _lwsem_wait(&USB_Stick);
            /* Here, the device finishes its lifetime */            
            _mem_free(dsp);
            msi_snd_deinit();
        }
    }
}


int usbBtRegisterDriverInfoTable(USB_HOST_DRIVER_INFO *infotable, int size) {
    int tablesize = sizeof (DriverInfoTable) / sizeof(DriverInfoTable[0]);

    if((tablesize == 0) || (tablesize > size)){
        printf("USB bluetooth driver info size is too large:%d more than globe avaiable size %d \n", tablesize, size);
        return 0;
    }

    memcpy(infotable, DriverInfoTable, (tablesize - 1) * sizeof(DriverInfoTable[0]));

    return tablesize - 1;
}


