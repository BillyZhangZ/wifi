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

/****************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include "psptypes_legacy.h"

#include <fio.h>
#include <mfs.h>
#include <sdcard.h>
#include <part_mgr.h>

#include "74hc595.h"


/**************************************************************************
  Local header files for this application
 **************************************************************************/
#include "microsd.h"


#if defined (BSP_SDCARD_SPI_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_SPI_CHANNEL
#elif defined (BSP_SDCARD_ESDHC_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_ESDHC_CHANNEL
#elif defined (BSP_SDCARD_SDHC_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_SDHC_CHANNEL
#else
#error "SDCARD low level communication device not defined!"
#endif

/**************************************************************************
  Global variables
 **************************************************************************/
#ifdef BSP_SDCARD_CS_74HC595                         
static _mqx_int set_CS (uint32_t cs_mask, void *user_data) {

    if (cs_mask & BSP_SDCARD_SPI_CS) {
        if(BSP_SDCARD_CS_74HC595 & (1 <<  BSP_74HC595_SPI_S0)){
            mux_74hc595_set_bit(BSP_74HC595_0,   BSP_74HC595_SPI_S0);
        }
        else{
            mux_74hc595_clear_bit(BSP_74HC595_0,   BSP_74HC595_SPI_S0);
        }

        if(BSP_SDCARD_CS_74HC595 & (1 <<  BSP_74HC595_SPI_S1)){
            mux_74hc595_set_bit(BSP_74HC595_0,   BSP_74HC595_SPI_S1);
        }
        else{
            mux_74hc595_clear_bit(BSP_74HC595_0,   BSP_74HC595_SPI_S1);
        }

    }
    return MQX_OK;
}
#endif


/**************************************************************************
  Global variables
 **************************************************************************/
int unit_test_microsd(void){
    boolean      inserted = TRUE, readonly = FALSE, last = FALSE;
    _mqx_int     error_code;
    _mqx_uint    param;
    MQX_FILE_PTR com_handle, sdcard_handle, filesystem_handle, partman_handle;
    char         filesystem_name[] = "a:";
    char         partman_name[] = "pm:";
#if defined BSP_SDCARD_GPIO_DETECT
    LWGPIO_STRUCT      sd_detect;
#endif
#if defined BSP_SDCARD_GPIO_PROTECT
    LWGPIO_STRUCT      sd_protect;
#endif
#ifdef BSP_SDCARD_GPIO_CS
    LWGPIO_STRUCT          sd_cs;
    SPI_CS_CALLBACK_STRUCT callback;
#endif
#ifdef BSP_SDCARD_CS_74HC595                         
    SPI_CS_CALLBACK_STRUCT callback;
#endif
    
    /* Open low level communication device */
    com_handle = fopen (SDCARD_COM_CHANNEL, NULL);

    if (NULL == com_handle)
    {
        printf("Error installing communication handle.\n");
        _task_block();
    }

#ifdef BSP_SDCARD_GPIO_CS

    /* Open GPIO file for SPI CS signal emulation */
    error_code = lwgpio_init(&sd_cs, BSP_SDCARD_GPIO_CS, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);
    if (!error_code)
    {
        printf("Initializing GPIO with associated pins failed.\n");
        _task_block();
    }
    lwgpio_set_functionality(&sd_cs,BSP_SDCARD_CS_MUX_GPIO);
    lwgpio_set_attribute(&sd_cs, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
    /* Set CS callback */
    callback.MASK = BSP_SDCARD_SPI_CS;
    callback.CALLBACK = set_CS;
    callback.USERDATA = &sd_cs;
    if (SPI_OK != ioctl (com_handle, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback))
    {
        printf ("Setting CS callback failed.\n");
        _task_block();
    }

#endif

#ifdef BSP_SDCARD_CS_74HC595                         
    callback.CALLBACK = set_CS;
    callback.USERDATA = NULL;
    if (SPI_OK != ioctl (com_handle, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback))
    {
        printf ("Setting CS callback failed.\n");
        _task_block();
    }
#endif

#if defined BSP_SDCARD_GPIO_DETECT
    /* Init GPIO pins for other SD card signals */
    error_code = lwgpio_init(&sd_detect, BSP_SDCARD_GPIO_DETECT, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    if (!error_code)
    {
        printf("Initializing GPIO with sdcard detect pin failed.\n");
        _task_block();
    }
    /*Set detect and protect pins as GPIO Function */
    lwgpio_set_functionality(&sd_detect,BSP_SDCARD_DETECT_MUX_GPIO);
    lwgpio_set_attribute(&sd_detect, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

#if defined BSP_SDCARD_GPIO_PROTECT
    /* Init GPIO pins for other SD card signals */
    error_code = lwgpio_init(&sd_protect, BSP_SDCARD_GPIO_PROTECT, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    if (!error_code)
    {
        printf("Initializing GPIO with sdcard protect pin failed.\n");
        _task_block();
    }
    /*Set detect and protect pins as GPIO Function */
    lwgpio_set_functionality(&sd_protect,BSP_SDCARD_PROTECT_MUX_GPIO);
    lwgpio_set_attribute(&sd_protect, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

    /* Install SD card device */
    error_code = _io_sdcard_install("sdcard:", (pointer)&_bsp_sdcard0_init, com_handle);
    if ( error_code != MQX_OK )
    {
        printf("Error installing SD card device (0x%x)\n", error_code);
        _task_block();
    }



#if defined BSP_SDCARD_GPIO_DETECT
    inserted = lwgpio_get_value(&sd_detect);
#endif

#if defined BSP_SDCARD_GPIO_PROTECT
    /* Get value of protect pin */
    readonly = lwgpio_get_value(&sd_protect);
#endif
    while(!inserted)
        _time_delay (500);

    _time_delay (200);
    /* Open the device which MFS will be installed on */
    sdcard_handle = fopen("sdcard:", 0);
    if ( sdcard_handle == NULL )
    {
        printf("Unable to open SD card device.\n");
        _task_block();
    }

    /* Set read only flag as needed */
    param = 0;
    if (readonly)
    {
        param = IO_O_RDONLY;
    }
    if (IO_OK != ioctl(sdcard_handle, IO_IOCTL_SET_FLAGS, (char_ptr) &param))
    {
        printf("Setting device read only failed.\n");
        _task_block();
    }

    /* Install partition manager over SD card driver */
    error_code = _io_part_mgr_install(sdcard_handle, partman_name, 0);
    if (error_code != MFS_NO_ERROR)
    {
        printf("Error installing partition manager: %s\n", MFS_Error_text((uint_32)error_code));
        _task_block();
    }

    /* Open partition manager */
    partman_handle = fopen(partman_name, NULL);
    if (partman_handle == NULL)
    {
        error_code = ferror(partman_handle);
        printf("Error opening partition manager: %s\n", MFS_Error_text((uint_32)error_code));
        _task_block();
    }

    /* Validate partition 1 */
    param = 1;
    error_code = _io_ioctl(partman_handle, IO_IOCTL_VAL_PART, &param);
    if (error_code == MQX_OK)
    {
        /* Install MFS over partition 1 */
        error_code = _io_mfs_install(partman_handle, filesystem_name, param);
        if (error_code != MFS_NO_ERROR)
        {
            printf("Error initializing MFS over partition: %s\n", MFS_Error_text((uint_32)error_code));
            _task_block();
        }

    } else {
        /* Install MFS over SD card driver */
        error_code = _io_mfs_install(sdcard_handle, filesystem_name, (_file_size)0);
        if (error_code != MFS_NO_ERROR)
        {
            printf("Error initializing MFS: %s\n", MFS_Error_text((uint_32)error_code));
            _task_block();
        }
    }

    /* Open file system */
    filesystem_handle = fopen(filesystem_name, NULL);
    error_code = ferror (filesystem_handle);
    if ((error_code != MFS_NO_ERROR) && (error_code != MFS_NOT_A_DOS_DISK))
    {
        printf("Error opening filesystem: %s\n", MFS_Error_text((uint_32)error_code));
        _task_block();
    }
    if ( error_code == MFS_NOT_A_DOS_DISK )
    {
        printf("NOT A DOS DISK! You must format to continue.\n");
    }

    printf ("\n  SD card installed to \"%s\"\n", filesystem_name);

    if (readonly)
    {
        printf ("SD card is locked (read only).\n");
    }
#ifdef _SD_PERFORMANCE_TEST
    {
        FILE_PTR stream;
        stream = fopen("a:\\1.flac", "r");
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
                printf("sd read 1Mbytes per 16k performance is %f kbytes!\n", (float)(1024 * 1000) / timeInterval);


                _mem_free(buf);
            }
            fclose(stream);
        }
        else{
            printf("open file 1.flac failed \n");
        }
    }
#endif
    /* Close the filesystem */
    if (MQX_OK != fclose (filesystem_handle))
    {
        printf("Error closing filesystem.\n");
        _task_block();
    }
    filesystem_handle = NULL;

    /* Uninstall MFS  */
    error_code = _io_dev_uninstall(filesystem_name);
    if (error_code != MFS_NO_ERROR)
    {
        printf("Error uninstalling filesystem.\n");
        _task_block();
    }

    /* Close partition manager */
    if (MQX_OK != fclose (partman_handle))
    {
        printf("Unable to close partition manager.\n");
        _task_block();
    }
    partman_handle = NULL;

    /* Uninstall partition manager  */
    error_code = _io_dev_uninstall(partman_name);
    if (error_code != MFS_NO_ERROR)
    {
        printf("Error uninstalling partition manager.\n");
        _task_block();
    }

    /* Close the SD card device */
    if (MQX_OK != fclose (sdcard_handle))
    {
        printf("Unable to close SD card device.\n");
        _task_block();
    }

    sdcard_handle = NULL;

    printf ("SD card uninstalled.\n");

    /* Close the low level interface */
    if (MQX_OK != fclose (com_handle))
    {
        printf("Unable to close SD card low level interface \n");
        _task_block();
    }

    printf ("SD card low level interface uninstalled.\n");


    return 0;
}

/* end file */

