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
* $FileName: play.c
* $Version : 
* $Date    : 
*
* Comments:
*
*
*END************************************************************************/

#include "play.h"

static _task_id player_task_id = MQX_NULL_TASK_ID, sd_walker_id = MQX_NULL_TASK_ID;

LWEVENT_STRUCT player_event;
LWSEM_STRUCT   player_sem;

LWSEM_STRUCT MSD_MFS_IO_SEM, SD_MFS_IO_SEM;

int playback_init(void){
    if (MQX_OK != _lwsem_create(&MSD_MFS_IO_SEM, 1)) {
        printf("lwsem_create MSD_MFS_IO_SEM failed.\n");
        return -1;
    }
    if (MQX_OK != _lwsem_create(&SD_MFS_IO_SEM, 1)) {
        printf("lwsem_create SD_MFS_IO_SEM failed.\n");
        return -1;
    }    
    if(_lwevent_create(&player_event, 0) != MQX_OK) {
        printf("playback_init create player_event failed \n");
        return -1;
    }
    _lwsem_create(&player_sem, 1);
    return 0;
}

/* Setup the play back scenario */
void playback_setup(lp_param_t * lpp_param)
{
    TASK_TEMPLATE_STRUCT task_template;
    /********* first, create the two tasks*********/
    printf("playback_setup...\n");

    _lwsem_wait(&player_sem);

    if(lpp_param->lp_type == mp_for_USB_MSD){
        lpp_param->mfs_io_sem = &MSD_MFS_IO_SEM;
    }
    else
        if(lpp_param->lp_type == mp_for_TF){
            lpp_param->mfs_io_sem = &SD_MFS_IO_SEM;
        }
        else{
            printf("lp type is not recongnized: %d \n", lpp_param->lp_type);
            _lwsem_post(&player_sem);
            return;
        }

    /* create sd_walker task*/
    if(MQX_NULL_TASK_ID == sd_walker_id)
    {
        task_template.TASK_TEMPLATE_INDEX  = 0;
        task_template.TASK_ADDRESS         = sd_file_search_task;
        task_template.TASK_STACKSIZE       = 2000;
        task_template.TASK_PRIORITY        = 12;
        task_template.TASK_NAME            = "sd_walker";
        task_template.TASK_ATTRIBUTES      = 0;
        task_template.CREATION_PARAMETER   = (uint_32)lpp_param;
        task_template.DEFAULT_TIME_SLICE   = 0;

        sd_walker_id = _task_create_blocked(0, 0, (uint_32)&task_template);

        if (sd_walker_id == MQX_NULL_TASK_ID)
        {
            printf("playback_setup create sd_file_search_task failed \n");
            _lwsem_post(&player_sem);
            return;
        }
        _task_ready(_task_get_td(sd_walker_id));
    }
    /* create player task*/
    if(MQX_NULL_TASK_ID == player_task_id )
    {
        task_template.TASK_TEMPLATE_INDEX  = 0;
        task_template.TASK_ADDRESS         = local_player_task;
        task_template.TASK_STACKSIZE       = 3200;
        task_template.TASK_PRIORITY        = 12;
        task_template.TASK_NAME            = "local_player";
        task_template.TASK_ATTRIBUTES      = 0;
        task_template.CREATION_PARAMETER   = (uint_32)lpp_param;
        task_template.DEFAULT_TIME_SLICE   = 0;

        player_task_id = _task_create_blocked(0, 0, (uint_32)&task_template);

        if (player_task_id == MQX_NULL_TASK_ID)
        {
            printf("playback_setup create player_task failed \n");
            _lwsem_post(&player_sem);
            return;
        }
        _task_ready(_task_get_td(player_task_id));
    }

    /************then send a auto play event *************/
    _lwevent_set(&player_event, PLAYER_EVENT_MSK_SD_FS_MOUNTED);     
}


/* Cancel the play back scenario */
extern boolean pause_trigger;
void playback_cancel(_mqx_uint event_type)
{	
    /********* first, end up the decoding thread *********/
    printf("playback_cancel...\n");

    decoding = FALSE;
    //printf("clear decoding.\n");
    /* Clear Song ready event, to avoid having PLAYER_EVENT_MSK_SONG_READY before PLAYER_EVENT_MSK_USB_ATTACHED */
    _lwevent_clear(&player_event, PLAYER_EVENT_MSK_SONG_READY);
    _lwevent_clear(&player_event, PLAYER_EVENT_MSK_SHELL_COMMAND);

    _lwevent_set(&player_event, PLAYER_EVENT_MSK_SEARCH_TASK_QUIT);
    //_time_delay(1);  /* give time to the search file task  exit */
    /* if pcm_flush_task is paused, resume it firstly*/
#if 0
    if(TRUE == pause_trigger) 
    {
        pause_trigger = FALSE;
        if((mp_sMicroSDplay == cur_musicPlayMS)||(mp_sMSDplay == cur_musicPlayMS))
            _lwevent_set(&player_event, PLAYER_EVENT_MSK_SONG_RESUME);

    }
#endif

    _lwevent_set(&player_event, event_type);	    //PLAYER_EVENT_MSK_USB_ATTACHED
    //_time_delay(1);  /* give time to the play task  exit */

    /*********then, Destroy local_play_task and sd_walker task *****************/
    /* wait for sd_file_search task's All resources are free*/
    _lwevent_wait_ticks(&player_event, 	PLAYER_EVENT_MSK_SEARCH_TASK_KILLED, TRUE, 0);
    _lwevent_clear(&player_event, PLAYER_EVENT_MSK_SEARCH_TASK_KILLED);
    if ( MQX_NULL_TASK_ID != sd_walker_id )
    { 
        //_task_destroy(sd_walker_id);
        //printf("Destroy sd walker task................\n");
        sd_walker_id = MQX_NULL_TASK_ID;
    }	
    /* wait for local player task's All resources are free*/
    _lwevent_wait_ticks(&player_event, 	PLAYER_EVENT_MSK_PLAYER_TASK_KILLED, TRUE, 0);   
    _lwevent_clear(&player_event, PLAYER_EVENT_MSK_PLAYER_TASK_KILLED);
    if ( MQX_NULL_TASK_ID != player_task_id )
    {
        //_task_destroy(player_task_id);
        //printf("Destroy sd player task................\n");
        player_task_id = MQX_NULL_TASK_ID;
    }

    _lwsem_post(&player_sem);
}

