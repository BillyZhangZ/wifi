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
 * $FileName: sd_task.c
 * $Version : 
 * $Date    : 
 *
 * Comments:
 *
 *   
 *   
 *
 *END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <mfs.h>
#include <sh_mfs.h>
#include <shell.h>
#include <sdcard.h>
#include <part_mgr.h>
#include "play.h"
#include "74hc595.h"
//#include "log_messages.h"
#include "lwmsgq.h"


#if defined (BSP_SDCARD_SPI_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_SPI_CHANNEL
#elif defined (BSP_SDCARD_ESDHC_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_ESDHC_CHANNEL
#elif defined (BSP_SDCARD_SDHC_CHANNEL)
#define SDCARD_COM_CHANNEL BSP_SDCARD_SDHC_CHANNEL
#else
#error "SDCARD low level communication device not defined!"
#endif

#define BTN_ISR_PRIORITY	5

#ifdef BSP_SW1
static LWGPIO_STRUCT  btn_next;
#endif
#ifdef BSP_SW2
static LWGPIO_STRUCT  btn_prev;
#endif

#if defined BSP_SDCARD_GPIO_DETECT
LWGPIO_STRUCT      sd_detect;
#endif

LWEVENT_STRUCT sddetect_event;

static MQX_FILE_PTR sdcard_handle, filesystem_handle, partman_handle;
static int sd_mounted = 0;
static char         filesystem_name[] = "a:";
static char         partman_name[] = "pm:";


void mfs_dir_reset(void);


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


/*TASK*-----------------------------------------------------------------
 *
 * Function Name  : sdplay_init
 * Returned Value : void
 * Comments       :
 *
 *END------------------------------------------------------------------*/

static void sdplay_init(void) {
    _mqx_int errcode = 0;


    /* Init GPIOs */    
#ifdef BSP_SW1                       
    lwgpio_init(&btn_next, BSP_SW1, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(&btn_next,BSP_SW1_MUX_GPIO);
    lwgpio_set_attribute(&btn_next, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

#ifdef BSP_SW2
    lwgpio_init(&btn_prev, BSP_SW2, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(&btn_prev, BSP_SW2_MUX_GPIO);
    lwgpio_set_attribute(&btn_prev, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif


}

static void EXT_SDDETECT_ISR(void *pin)
{
    int sd_pin;
    boolean inserted;

    if((sd_pin = lwgpio_int_get_flag((LWGPIO_STRUCT_PTR) pin))) {
        inserted = lwgpio_get_value((LWGPIO_STRUCT_PTR) pin);
        if(inserted)
            _lwevent_set(&sddetect_event,SD_ATTACHED_EVENT);
        else
            _lwevent_set(&sddetect_event,SD_DETTACHED_EVENT);
    }
    lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);
}





static int mount_sdcard(void) {
    _mqx_uint    param;
    _mqx_int     error_code;
    boolean readonly = FALSE; /* TRUE; */
    //char         filesystem_name[] = "a:";
    //char         partman_name[] = "pm:";

    printf("mounte sd card +\n");

    _lwsem_wait(&SD_MFS_IO_SEM);

    if(sd_mounted) {
        _lwsem_post(&SD_MFS_IO_SEM);
        return 0;
    }

    do {
        /* Open the device which MFS will be installed on */
        sdcard_handle = fopen("sdcard:", 0);
        if (sdcard_handle == NULL ) {
            printf("Unable to open SD card device \n");
            _lwsem_post(&SD_MFS_IO_SEM);
            return -1;
        }

        /* Set read only flag as needed */
        param = 0;
        if (readonly)  {
            param = IO_O_RDONLY;
        }
        if (IO_OK != ioctl(sdcard_handle, IO_IOCTL_SET_FLAGS, (char_ptr) &param)) {
            printf("Setting device read only failed.\n");
            _task_block();
        }

        /* Install partition manager over SD card driver */
        error_code = _io_part_mgr_install(sdcard_handle, partman_name, 0);
        if (error_code != MFS_NO_ERROR) {
            printf("Error installing partition manager: %s\n", MFS_Error_text((uint_32)error_code));
            //_task_block();
            continue;       /* retry it */     
        }

        /* Open partition manager */
        partman_handle = fopen(partman_name, NULL);
        if (partman_handle == NULL) {
            error_code = ferror(partman_handle);
            printf("Error opening partition manager sd: %s\n", MFS_Error_text((uint_32)error_code));
            _task_block();
        }

        /* Validate partition 1 */
        param = 1;
        error_code = _io_ioctl(partman_handle, IO_IOCTL_VAL_PART, &param);
        if (error_code == MQX_OK) {
            /* Install MFS over partition 1 */
            error_code = _io_mfs_install(partman_handle, filesystem_name, param);
            if (error_code != MFS_NO_ERROR) {
                printf("Error initializing MFS over partition: %s\n", MFS_Error_text((uint_32)error_code));
                _task_block();
            }
        } else {
            /* Install MFS over SD card driver */
            error_code = _io_mfs_install(sdcard_handle, filesystem_name, (_file_size)0);
            if (error_code != MFS_NO_ERROR) {
                printf("Error initializing MFS: %s\n", MFS_Error_text((uint_32)error_code));
                _task_block();
            }
        }

        /* Open file system */
        filesystem_handle = fopen(filesystem_name, NULL);
        error_code = ferror (filesystem_handle);
        if ((error_code != MFS_NO_ERROR) && (error_code != MFS_NOT_A_DOS_DISK)) {
            printf("Error opening filesystem: %s\n", MFS_Error_text((uint_32)error_code));
            //_task_block();
            continue;                 /* retry it */
        }
        if ( error_code == MFS_NOT_A_DOS_DISK ) {
            printf("NOT A DOS DISK! You must format to continue.\n");
        }

        printf ("\n  SD card installed to \"%s\"\n", filesystem_name);

        if (readonly) {
            printf ("SD card is locked (read only).\n");
        }

        mfs_dir_reset();
    }while(0);

    sd_mounted = 1;   
    printf("mounted sd card -\n");
    _lwsem_post(&SD_MFS_IO_SEM);

    return 0;
}

static int umount_sdcard(void){
    _mqx_uint    param;
    _mqx_int     error_code;

    boolean readonly = FALSE;
    //char         filesystem_name[] = "a:";
    //char         partman_name[] = "pm:";

    printf("unmounte sd card +\n");
    _lwsem_wait(&SD_MFS_IO_SEM);

    if( ! sd_mounted) {
        _lwsem_post(&SD_MFS_IO_SEM);
        return 0;
    }

    /* Close the filesystem */
    if (MQX_OK != fclose (filesystem_handle)) {
        printf("Error closing filesystem.\n");
        _task_block();
    }
    filesystem_handle = NULL;

    /* Uninstall MFS  */
    error_code = _io_dev_uninstall(filesystem_name);
    if (error_code != MFS_NO_ERROR) {
        printf("Error uninstalling filesystem.\n");
        _task_block();
    }

    /* Close partition manager */
    if (MQX_OK != fclose (partman_handle)) {
        printf("Unable to close partition manager.\n");
        _task_block();
    }
    partman_handle = NULL;

    /* Uninstall partition manager  */
    error_code = _io_dev_uninstall(partman_name);
    if (error_code != MFS_NO_ERROR) {
        printf("Error uninstalling partition manager.\n");
        _task_block();
    }

    /* Close the SD card device */
    if (MQX_OK != fclose (sdcard_handle)) {
        printf("Unable to close SD card device.\n");
        _task_block();
    }
    sdcard_handle = NULL;

    sd_mounted = 0;
    printf("unmounted sd card -\n");
    _lwsem_post(&SD_MFS_IO_SEM);

    return 0;
}

/*TASK*-----------------------------------------------------------------
 *
 * Function Name  : sd_task
 * Returned Value : void
 * Comments       :
 *
 *END------------------------------------------------------------------*/
void sd_task(uint_32 param){
    boolean      inserted = TRUE;

    _mqx_int     error_code;

    MQX_FILE_PTR com_handle;

    lp_param_t lp_param;
    boolean cardBeMounted = FALSE;

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

    _mqx_int        sd_event_value;
    _mqx_uint       wait_state;


    sdplay_init();

    if (MQX_OK !=_lwevent_create(&(sddetect_event), LWEVENT_AUTO_CLEAR)){
        printf("\n_lwevent_create sddetect_event failed\n");
        _task_block();
    }

    /* Open low level communication device */
    com_handle = fopen(SDCARD_COM_CHANNEL, NULL);

    if (NULL == com_handle){
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
        printf("Setting CS callback failed.\n");
        _task_block();
    }
#endif

#if defined BSP_SDCARD_GPIO_DETECT
    /* Init GPIO pins for other SD card signals */
    error_code = lwgpio_init(&sd_detect, BSP_SDCARD_GPIO_DETECT, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    if (!error_code){
        printf("Initializing GPIO with sdcard detect pin failed.\n");
        _task_block();
    }
    /*Set detect and protect pins as GPIO Function */
    lwgpio_set_functionality(&sd_detect,BSP_SDCARD_DETECT_MUX_GPIO);
    lwgpio_set_attribute(&sd_detect, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);

    lwgpio_int_init(&sd_detect,LWGPIO_INT_MODE_RISING | LWGPIO_INT_MODE_FALLING /* LWGPIO_INT_MODE_HIGH*/);     /* falling,raising mode = 3 */

    /* install gpio interrupt service routine */
    _int_install_isr(lwgpio_int_get_vector(&sd_detect), EXT_SDDETECT_ISR, (void *) &sd_detect);
    _bsp_int_init(lwgpio_int_get_vector(&sd_detect), 5, 0, TRUE);

    lwgpio_int_enable(&sd_detect, TRUE);

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
    if ( error_code != MQX_OK ){
        printf("Error installing SD card device (0x%x)\n", error_code);
        _task_block();
    }

    _lwevent_set(&sddetect_event, SD_ATTACHED_EVENT); // set attached event at mode manager

    // use sd detect interrupt
    for (;;) {

        wait_state = _lwevent_wait_ticks(&sddetect_event, SD_EVENT_MASK, FALSE, 0);

        if (wait_state !=  MQX_OK ){
            printf("waiting sddetect_event fail\n");
            continue;
        }

        sd_event_value = _lwevent_get_signalled();

        if ( (sd_event_value == SD_ATTACHED_EVENT) || (sd_event_value == SD_DETTACHED_EVENT)){

            _time_delay(300);
            inserted = lwgpio_get_value(&sd_detect);

            if(!inserted && (cardBeMounted == TRUE)){
                umount_sdcard();
                cardBeMounted = FALSE;
                playback_cancel(PLAYER_EVENT_MSK_SD_FS_UNMOUNTED);		  
            }

            if(inserted && (cardBeMounted == FALSE)){
                if(mount_sdcard() == 0){
                    cardBeMounted = TRUE;
                    strcpy(lp_param.path,"a:\\");
                    lp_param.lp_type = mp_for_TF;
                    //lp_param.mfs_io_sem = &SD_MFS_IO_SEM;
                    playback_setup(&lp_param);
                }
            }
        } 
    }
} /* sdcard_task */

