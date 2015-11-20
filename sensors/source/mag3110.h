/**HEADER********************************************************************
 * 
 * Copyright (c) 2015 Freescale Semiconductor;
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

#ifndef __MAG3110_H_
#define __MAG3110_H_

typedef enum {

    MAG3110_CHANNEL_X = 0,
    MAG3110_CHANNEL_Y,
    MAG3110_CHANNEL_Z,

}MAG3110_CHANNEL;

int mag3110_get_temperature(void *handle, int *temp);
int mag3110_get_magnet(void *handle, int *x, int *y, int *z);
int mag3110_update_sample_frequency(void *handle, int val, int val2);
int mag3110_get_sample_frequency(void *handle, int *val, int *val2);
int mag3110_update_user_offset_correction(void *handle, MAG3110_CHANNEL channel, int value);
int mag3110_get_user_offset_correction(void *handle, MAG3110_CHANNEL channel, int *value);
int mag3110_get_real_rate(int *val1, int *val2);
void *mag3110_init(char *i2cbus, unsigned int speed);
int mag3110_deinit(void *handle);

#endif   

/* EOF */
