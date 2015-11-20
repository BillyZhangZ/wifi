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
* $FileName: main.c$
* $Version : 3.8.3.0$
* $Date    : Sep-12-2012$
*
* Comments:
*
*   This include file is used to provide information needed by
*   applications using the SAI I/O functions.
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <part_mgr.h>
#include "main.h"
#include "log_messages.h"
#include "sgtl5000.h"
#include "sh_audio.h"

#if !BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


#if defined (BSP_TWRMCF51CN) || defined (BSP_TWRPXS20) || defined (BSP_TWR_K20D72M)

#if ! BSPCFG_ENABLE_SPI1
#error This application requires BSPCFG_ENABLE_SPI1 defined non-zero in user_config.h. Please recompile libraries with this option.
#endif
#endif

TASK_TEMPLATE_STRUCT MQX_template_list[] = 
{ 
    /*  Task number, Entry point, Stack, Pri, String,   Auto? */
    { REC_PLAY_TASK,        Record_playback_task,        2000,  11,   "record_playback", 0 },
    { INIT_TASK,            Init_task,                   1000,  9,    "init",  MQX_AUTO_START_TASK },
    { 0, 0, 0, 0, 0, 0 }
};


/*TASK*-----------------------------------------------------------------
*
* Function Name  : Record_playback_task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

uint32_t buffer_size = 32768;
uint8_t ram_buffer[32768];

void Record_playback_task
(
    uint32_t temp
)
{ 
    printf("Audio driver playback via sai application\n");

    uint8_t *wav_data;
    wav_data = (uint8_t *)ram_buffer;
    char param[] = "1:8000:16:2";
    /* First record a one second's of stereo wav into the ram */
    Sai_record(wav_data, param);
    /* Then playback the stereo wav */
    Sai_play(wav_data);
}

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Init_task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/
void Init_task
(
    uint32_t temp
)
{
   
    MQX_TICK_STRUCT time;
    _mqx_int errcode = 0;
    /* Install MQX default unexpected ISR routines */
    _int_install_unexpected_isr();
    /* Setup time */
    printf("Setting up time......................");
    time.TICKS[0] = 0L;
    time.TICKS[1] = 0L;
    time.HW_TICKS = 0;
    _time_set_ticks(&time);
    printf("[OK]\n");
    
    /* Initialize audio codec */
    printf("Initializing audio codec.............");
    errcode = InitCodec();
    if (errcode != 0)
    {
        printf("[FAIL]\n");
        printf("  Error 0x%X\n", errcode);
        _task_block();
    }
    else
    {
        printf("[OK]\n");
    }
    
    /* Create tasks */
    printf("Creating record_playback task..................");
    errcode = _task_create(0, REC_PLAY_TASK, 0);
    if (errcode == MQX_NULL_TASK_ID)
    {
        printf("[FAIL]\n");
        printf("  Error 0x%X.\n");
        _task_block();
    }
    else
    {
        printf("[OK]\n");
    }
    printf("\n\n");
    _task_abort(MQX_NULL_TASK_ID);
}

/* EOF */
