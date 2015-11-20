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
* $FileName: main.h$
* $Version : 3.8.3.0$
* $Date    : Sep-12-2012$
*
* Comments:
*
*   This include file is used to provide information needed by
*   applications using the SAI I/O functions.
*
*END************************************************************************/

#ifndef __lp_main_h_
#define __lp_main_h_

#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include <message.h>
#include <psptypes_legacy.h>


#define AUDIO_SAMPLE_RATE   (44100)

//#define SD_PLAYER_SHELL_SUPPORTED


#define WRITE_QUEUE 8
#define REC_BLOCK_SIZE 512


#define PLAYER_EVENT_MSK_NEXT_BTN_PRESSED       0x01
#define PLAYER_EVENT_MSK_PREV_BTN_PRESSED       (0x01 << 1)
#define PLAYER_EVENT_MSK_SD_FS_MOUNTED          (0x01 << 2)
#define PLAYER_EVENT_MSK_SD_FS_UNMOUNTED        (0x01 << 3)
#define PLAYER_EVENT_MSK_SONG_READY             (0x01 << 4)
#define PLAYER_EVENT_MSK_PLAYER_TASK_KILLED		(0x01 << 5)
#define PLAYER_EVENT_MSK_SHELL_COMMAND			(0x01 << 6)

#define PLAYER_EVENT_MSK_USB_ATTACHED		(0x01 << 7)   

//#define PLAYER_EVENT_MSK_SD_FS_UNMOUNTED_DISPOSED		(0x01 << 8)   //add by guoyifang for sync Close FS, before CloseFS, close opened file firstly .

#define PLAYER_EVENT_MSK_SONG_RESUME		(0x01 << 8)   

#define PLAYER_EVENT_MSK_SEARCH_TASK_QUIT		(0x01 << 9)
#define PLAYER_EVENT_MSK_SEARCH_TASK_KILLED		(0x01 << 10)
#define AUDIO_BUF_CNT	1

extern boolean decoding ;
extern LWEVENT_STRUCT player_event;

//#define  SD_DETECT_POLLING
#define SD_ATTACHED_EVENT    (0x1)
#define SD_DETTACHED_EVENT   (0x2)
#define SD_EVENT_MASK        (SD_ATTACHED_EVENT | SD_DETTACHED_EVENT)


extern void local_player_task(uint_32 para);
extern void pcm_flush_task(uint_32 para);
extern void sd_file_search_task(uint_32 para);


#define  USB_ACCESSORY_PLAY
#define  USB_MSD_PLAY

#define  ACCESSORY_PRINTF        //debug

//extern volatile music_play_state_type_t  cur_musicPlayMS;   //prev_musicPlayMS

extern LWSEM_STRUCT MSD_MFS_IO_SEM, SD_MFS_IO_SEM;

typedef enum _music_play_source
{
	mp_for_BT = 0,                /* Play Bluetooth music is the highest priority */
	mp_for_USB_ACC,
	mp_for_USB_MSD,
	mp_for_USB_PCstream,
	mp_for_TF,
	mp_for_NULL
}music_play_source_type_t;


typedef struct {
	music_play_source_type_t  lp_type;
	LWSEM_STRUCT * mfs_io_sem;
	char path[16];
}  lp_param_t;


#endif /* __main_h_ */

/* EOF */
