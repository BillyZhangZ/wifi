/**HEADER********************************************************************
 * =G
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
#include "umass_task.h"

/* add for mfs usb */
#include "MFS_USB.h"

#include <string.h>
#include <lwmsgq.h>
#include <mqx_host.h>
#include <host_dev_list.h>
#include <usb_host_msd_bo.h>
#include <usb_host_hub_sm.h>

#include "USB_File.h"

#include "play.h"

#undef pr_debug
#define pr_debug printf

typedef struct {
    CLASS_CALL_STRUCT_PTR ccs;     /* class call struct of MSD instance */
    uint_8                body;    /* message body one of USB_EVENT_xxx as defined above */
} usb_msg_t;

/* The granularity of message queue is one message. Its size is the multiplier of _mqx_max_type. Get that multiplier */
#define USB_TASKQ_GRANM ((sizeof(usb_msg_t) - 1) / sizeof(_mqx_max_type) + 1)
static _mqx_max_type  usb_taskq[20 * USB_TASKQ_GRANM * sizeof(_mqx_max_type)]; /* prepare message queue for 20 events */


static void usb_host_mass_device_event (_usb_device_instance_handle dev_handle, _usb_interface_descriptor_handle intf_handle, uint_32 event_code);

/**************************************************************************
  Class driver files for this application
 **************************************************************************/




/**************************************************************************
  Global variables
 **************************************************************************/




/* Table of driver capabilities this application wants to use */
static  USB_HOST_DRIVER_INFO DriverInfoTable[] =
{
    /* USB 2.0 hard drive */
    {
        {0x00,0x00}, 
        {0x00,0x00}, 
        USB_CLASS_MASS_STORAGE, 
        USB_SUBCLASS_MASS_SCSI, 
        USB_PROTOCOL_MASS_BULK, 
        0, 
        usb_host_mass_device_event
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
pointer                      usb_fs_handle = NULL;

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : umass_task
* Returned Value : None
* Comments       :
*     First function called. This rouine just transfers control to host main
*END*--------------------------------------------------------------------*/

void umass_task(uint_32 param){

    usb_msg_t            msg;
    struct _usb_msd_host_info_  *msd_info;
    static lp_param_t lp_param;
    
    if (MQX_OK != _lwmsgq_init(usb_taskq, 20, USB_TASKQ_GRANM)) {
        // lwmsgq_init failed
        _task_block();
    }

    for (;;) {
        /* Wait for event sent as a message */
        _lwmsgq_receive(&usb_taskq, (_mqx_max_type *) &msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY, 0, 0);
         
        printf("umass msg.body is %d \n", msg.body);

        msd_info = (struct _usb_msd_host_info_ *)msg.ccs;

        if (msg.body == USB_EVENT_ATTACH) {
          /* This event is not so important, because it does not inform about successfull USB stack enumeration */
        } else if (msg.body == USB_EVENT_INTF )  { 

            if( msd_info->fs_mountp != 0x3FFC)//Msd_host_task_run can't be used here, it before plugin message. 
            {
                static char    drive[3];     //modify it to static by gyf for a bug that fopen fullpath failed in sd player task. 2013-12-16


                /* get free mountpoint as a string like "c:" */
                for (drive[0] = 'c'; drive[0] <= 'z'; drive[0]++)
                    if (!(msd_info->fs_mountp & (1 << (drive[0] - 'a'))))
                        break;
                drive[1] = ':';
                drive[2] = 0;

                printf("uf installed+\n");	
                usb_fs_handle = usb_filesystem_install(msg.ccs, "USB:", "PM_C1:", drive );
                printf("uf installed-\n");	

                if (usb_fs_handle) {

                    msd_info->msd_device.mount = drive[0];

                    // Mark file system as mounted
                    msd_info->fs_mountp |= 1 << (msd_info->msd_device.mount - 'a');
#ifdef _USB_PERFORMANCE_TEST
                    {
                        FILE_PTR stream;
                        stream = fopen("c:\\1.flac", "r");
                        if(stream != NULL){
                            char *buf = NULL;
                            buf = _mem_alloc(16 * 1014);
                            if(buf != NULL){
                                int i;
                                TIME_STRUCT timeA, timeB, timeD;
                                unsigned int timeInterval;
                                _time_get(&timeA);

                                for(i = 0; i < 64; i++)
                                    fread(buf, 1, (16 * 1024), stream);

                                _time_get(&timeB);

                                _time_diff(&timeA, &timeB, &timeD);
                                timeInterval = timeD.SECONDS * 1000 + timeD.MILLISECONDS;
                                printf("USB read 1Mbytes per 16k performance is %f kbytes!\n", (float)(1024 * 1000) / timeInterval);


                                _mem_free(buf);
                            }
                            fclose(stream);
                        }
                        else{
                            printf("open file 1.flac failed \n");
                        }
                    }
#endif
                    /* setup U disk  play back task */
                    strcpy(lp_param.path,"c:\\");
                    lp_param.lp_type = mp_for_USB_MSD;
                    lp_param.mfs_io_sem = &MSD_MFS_IO_SEM;
                    //msi_snd_unmute();
                    playback_setup(&lp_param);

                }	
            }
        } /* USB INTF event */
        else if (msg.body == USB_EVENT_DETACH) {

            if (msd_info->msd_device.mount >= 'a' && msd_info->msd_device.mount <= 'z') {
                playback_cancel(PLAYER_EVENT_MSK_SD_FS_UNMOUNTED);		  
                printf("uf uninstall+\n");
                // Remove the file system 
                usb_filesystem_uninstall(usb_fs_handle);
                printf("uf uninstall-\n");
                // Mark file system as unmounted
                msd_info->fs_mountp &= ~(1 << (msd_info->msd_device.mount - 'a'));
                USB_mem_free(msd_info);
            }

            usb_fs_handle = NULL;

        } /* DETACH event */
    }/* for loop */
}


/*FUNCTION*----------------------------------------------------------------
 *
 * Function Name  : usb_host_mass_device_event
 * Returned Value : None
 * Comments       :
 *     called when a mass storage device has been attached, detached, etc.
 *END*--------------------------------------------------------------------*/
static void usb_host_mass_device_event     /* USB_HOST_DRIVER_INFO  event callback !!! */
(
 /* [IN] pointer to device instance */
 _usb_device_instance_handle      dev_handle,

 /* [IN] pointer to interface descriptor */
 _usb_interface_descriptor_handle intf_handle,

 /* [IN] code number for event causing callback */
 uint_32           event_code
 )
{
    struct _usb_msd_host_info_ *msd_info;
    usb_msg_t                   msg;

    switch (event_code) {
        case USB_ATTACH_EVENT:
            printf("msd USB_ATTACH_EVENT \n");
            break;

        case USB_CONFIG_EVENT:
            printf("msd USB_CONFIG_EVENT \n");

            msd_info =  USB_mem_alloc_zero(sizeof(struct _usb_msd_host_info_));

            /* Here, the device starts its lifetime */
            if (msd_info == NULL) {
                printf("msd_host_info is NULL in attach event\n");
                break;
            }

            if (USB_OK != _usb_hostdev_select_interface(dev_handle, intf_handle, &(msd_info->msd_device.ccs))) {
                printf("msd select intf failed\n");
                break;
            }
            break;

        case USB_INTF_EVENT:
            printf("msd USB_INTF_EVENT\n");
            if (USB_OK != usb_class_mass_get_app(dev_handle, intf_handle, (CLASS_CALL_STRUCT_PTR *)&msd_info)) {
                printf("usb_class_mass_get_app failed\n");
                break;
            }


            msg.ccs  = &(msd_info->msd_device.ccs);
            msg.body = USB_EVENT_INTF;
            if (LWMSGQ_FULL == _lwmsgq_send(usb_taskq, (uint_32 *) &msg, 0)) {
                printf("Could not inform USB task about device interfaced\n");
            }

            break;

        case USB_DETACH_EVENT:
            printf("msd USB_DETACH_EVENT\n");
            if (USB_OK != usb_class_mass_get_app(dev_handle, intf_handle, (CLASS_CALL_STRUCT_PTR *)&msd_info)) {
                printf("usb_class_mass_get_app failed at USB_DEVICE_DETACHED\n");		 
                break;
            }


            msg.ccs  = &(msd_info->msd_device.ccs);
            msg.body = USB_EVENT_DETACH;

            if (LWMSGQ_FULL == _lwmsgq_send(usb_taskq, (uint_32 *) &msg, 0)) {
                printf("Could not inform USB task about device detached\n");
            }
            break;


        default:
            break;
    } 
} 

int umassRegisterDriverInfoTable(USB_HOST_DRIVER_INFO *infotable, int size) {
    int tablesize = sizeof (DriverInfoTable) / sizeof(DriverInfoTable[0]);

    if((tablesize == 0) || (tablesize > size)){
        printf("UMASS driver info size is too large:%d more than globe avaiable size %d \n", tablesize, size);
        return 0;
    }

    memcpy(infotable, DriverInfoTable, (tablesize - 1) * sizeof(DriverInfoTable[0]));

    return tablesize - 1;
}
/* end file */

