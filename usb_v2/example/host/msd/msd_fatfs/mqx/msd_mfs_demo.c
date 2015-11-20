/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
* $FileName: msd_mfs_demo.c$
* $Version : 
* $Date    : 
*
* Comments:
*
*   This file implements MAS FAT demo application.
*
*END************************************************************************/
#include "usb_host_config.h"
#include "usb.h"
#include "usb_host_stack_interface.h"
#if (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX)
#include <lwevent.h>
#include <bsp.h>
#include <mfs.h>
#include <part_mgr.h>
#include <usbmfs.h>
#include "mfs_usb.h"
#include "usb_file.h"

#include "usb_host_hub_sm.h"
#include "usb_host_msd_bo.h"
#include "usb_host_msd_ufi.h"

volatile device_struct_t   g_mass_device[USBCFG_MAX_INSTANCE] = { 0 };   /* mass storage device struct */
volatile uint8_t           g_mass_device_new_index = 0;

/* Store mounting point used. A: is the first one, bit #0 assigned, Z: is the last one, bit #25 assigned */
uint32_t                   g_fs_mountp = 0;

char                       g_drive[3];
char                       g_pm[8] = "PM_C1:";
char                       g_block[8] = "USB:";

int mfs_mount(uint8_t device_no)
{
	/* get free mountpoint as a string like "c:" */
	for (g_drive[0] = 'C'; g_drive[0] <= 'Z'; g_drive[0]++)
		if (!(g_fs_mountp & (1 << (g_drive[0] - 'A'))))
			break;
	g_drive[1] = ':';
	g_drive[2] = 0;
	
	g_pm[3] = g_drive[0];
	
	/* Install the file system, use device->ccs as a handle */
	g_mass_device[device_no].usb_mfs_handle = usb_filesystem_install( (void*)g_mass_device[device_no].CLASS_HANDLE, g_block, g_pm, g_drive );
	
	if (NULL != g_mass_device[device_no].usb_mfs_handle)
	{
		g_mass_device[device_no].mount = g_drive[0];		
		/* Mark file system as mounted */
		g_fs_mountp |= 1 << (g_mass_device[device_no].mount - 'A');
		
		return 0;
	}
	
	return -1;
}

int mfs_unmount(uint8_t device_no)
{
    if (g_mass_device[device_no].mount >= 'A' && g_mass_device[device_no].mount <= 'Z') 
    {
        /* Remove the file system  */
        usb_filesystem_uninstall(g_mass_device[device_no].usb_mfs_handle);
        /* Mark file system as unmounted */
        g_fs_mountp &= ~(1 << (g_mass_device[device_no].mount - 'A'));
        return 0;
    }
    return -1;
}

#endif /* (OS_ADAPTER_ACTIVE_OS == OS_ADAPTER_MQX) */

/* EOF */
