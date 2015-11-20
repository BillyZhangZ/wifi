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

#ifndef __main_h_
#define __main_h_

#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include <message.h>

extern void Init_task(uint32_t);
extern void Record_playback_task(uint32_t);

#define AUDIO_SAMPLE_RATE   (44100)
#ifdef BSPCFG_ENABLE_SAI
#define CLK_MULT             (384)
#else
#define CLK_MULT             (256)
#endif

#define INIT_TASK 1
#define REC_PLAY_TASK 2

#define WRITE_QUEUE 8
#define REC_BLOCK_SIZE 512

/* This struct contains a data field and a message struct. */
typedef struct
{
    /* Message header */ 
    MESSAGE_HEADER_STRUCT HEADER;
    
    /* Data length */
    uint32_t LENGTH;
    
    /* Data */
    char DATA[REC_BLOCK_SIZE];   
} REC_MESSAGE, * REC_MESSAGE_PTR;

#endif /* __main_h_ */

/* EOF */
