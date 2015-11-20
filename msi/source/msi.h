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
*END************************************************************************/


#ifndef _MSI_H
#define _MSI_H

/*
 * mqx sound interface
 */

/*
 * default set is the sample rate 44100, 16 bits, 2 channel
 * umute
 */
int32_t msi_snd_init(void);

int32_t msi_snd_init_with_periodbuffer(int32_t _periodbuffersize, int32_t _periodbuffercount);
/*
 * write samples to device
 * must have correct set before write
 */
int32_t msi_snd_write(uint8_t * pcmStream, uint32_t pcmCnt);

/*
 * collect resources of sound device
 */
int32_t msi_snd_deinit(void);

/*
 * set pcm sound format: sample rate, bit width, channel number
 */
int32_t msi_snd_set_format(uint32_t sampleRate, uint8_t bitWidth, uint8_t chNum);

/*
 * mute sound device
 */
int32_t msi_snd_mute(void);
/*
 * umute device
 */
int32_t msi_snd_umute(void);

/*
 * turn up sound volume
 */
int32_t msi_snd_vol_up(void);

/*
 * turn down sound volume
 */
int32_t msi_snd_vol_down(void);
/*
 * flush the pcm stream out
 * 
 */
int32_t msi_snd_flush(void);

#endif
