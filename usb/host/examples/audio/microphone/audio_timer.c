/**HEADER********************************************************************
* 
* Copyright (c) 2011 Freescale Semiconductor;
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
 **************************************************************************//*!
 *
 * @file audio_timer.c
 *
 * @author
 *
 * @version
 *
 * @date    
 *
 * @brief   
 *          
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include <string.h>
#include "audio_microphone.h"
#include "audio_timer.h"

void audio_timer_init(void);
static void AUDIO_TIMER_ISR(pointer);
extern void usb_host_audio_tr_callback(
                                        _usb_pipe_handle ,
                                        pointer ,
                                        uchar_ptr ,
                                        uint_32 ,
                                        uint_32 
                                      );

/******************************************************************************
 * Global variable
 *****************************************************************************/
extern volatile AUDIO_CONTROL_DEVICE_STRUCT audio_control;
extern volatile AUDIO_STREAM_DEVICE_STRUCT audio_stream;

extern uint_8 wav_buff[MAX_ISO_PACKET_SIZE];
extern uint_8 wav_recv_buff[MAX_ISO_PACKET_SIZE];
extern uint_8 wav_recv_buff_tmp[MAX_ISO_PACKET_SIZE];
extern uint_8 device_direction;
extern uint_32 packet_size;

extern LWEVENT_STRUCT_PTR block_rec;

/******************************************************************************
*   @name        audio_timer_init
*
*   @brief       This function initialize audio timer
*
*   @return      None
*
*   @comment     
*    
*******************************************************************************/
void audio_timer_init(void) 
{ 
    if(_int_install_isr(AUDIO_INT, AUDIO_TIMER_ISR, NULL) == NULL)
    {
        return ;
    } /* Endif */
    _bsp_int_init(AUDIO_INT, 2, 0, TRUE);
}
/******************************************************************************
*   @name        AUDIO_TIMER_ISR
*
*   @brief       This routine services RTC Interrupt
*
*   @param       None
*
*   @return      None
*
******************************************************************************
* Services Programmable Interrupt Timer 0. If a Timer Object expires, then
* removes the object from Timer Queue and Calls the callback function
* (if registered)
*****************************************************************************/
static void AUDIO_TIMER_ISR(pointer p)
{
    _audio_clear_int(AUDIO_TIMER);
    
    //Do the stuff only if both USB and the sync mechanism are ready
    if ((USB_DEVICE_INUSE == audio_stream.DEV_STATE) && (block_rec != NULL))
    {
        /* the attached device is microphone */
        if(USB_AUDIO_DEVICE_DIRECTION_OUT == device_direction)
        {
            /* Recv data */
            usb_audio_recv_data(
                                (CLASS_CALL_STRUCT_PTR)&audio_control.CLASS_INTF,
                                (CLASS_CALL_STRUCT_PTR)&audio_stream.CLASS_INTF,
                                usb_host_audio_tr_callback,
                                NULL, packet_size, 
                                (uchar_ptr)wav_recv_buff
                               );
        }
    }

    /* Enable PIT interrupt neeed due to errata on Kinetis PIT */
    _audio_timer_unmask_int(AUDIO_TIMER);
}
/* EOF */
