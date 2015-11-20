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

 
#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include "msi.h"
#include "sai.h"
#include "codec.h"

#define AUDIO_SAMPLE_RATE   (44100)
#define AUDIO_PERIOD_BUFFER_SIZE  (512)
#define AUDIO_PERIOD_BUFFER_COUNT (4)

static FILE_PTR device_ptr;
static AUDIO_DATA_FORMAT audio_format;
static uint8_t  *periodbuffer;
static uint32_t  periodbufferoff;
static uint32_t  periodbuffersize;
static uint32_t  periodbuffercount;

/*
 * default set is the sample rate 44100, 16 bits, 2 channel
 * umute
 */
int32_t msi_snd_init(void){

    return  msi_snd_init_with_periodbuffer(AUDIO_PERIOD_BUFFER_SIZE, AUDIO_PERIOD_BUFFER_COUNT);
}

/*
 * default set is the sample rate 44100, 16 bits, 2 channel
 * umute
 */
int32_t msi_snd_init_with_periodbuffer(int32_t _periodbuffersize, int32_t _periodbuffercount){

    uint32_t          mclk_freq, fs_freq, default_sample_rate;
    _mqx_int errcode = 0;
    SAI_BUFFER_CONFIG _periodbufferconfig;


    if(device_ptr != NULL){
	printf("msi init while it is  already inited ! \n");
	return -1;
    }

    periodbuffer    = _mem_alloc_system_zero(_periodbuffersize);
    if(periodbuffer == NULL){
        printf("periodbuffer alloc failed! \n");
        return -1;
    }
    periodbufferoff = 0;

    /* Initialize audio codec */
    printf("msi_snd_init initializing audio codec............. \n ");
    errcode = CodecInit();
    if (errcode != 0) {
        printf("msi_snd_init CodecInit failed \n");
	return -1;
    }


    /* Open sai device and set it up */
    if ((device_ptr = fopen("sai:", "w")) == NULL) {
        printf("Error: Unable to open a sai device.\n");
        return -1;
    }

    /* re config period buffer  */ 

    periodbuffersize  = _periodbufferconfig.PERIOD_SIZE   = _periodbuffersize;
    periodbuffercount = _periodbufferconfig.PERIOD_NUMBER = _periodbuffercount;

    if (ioctl(device_ptr, IO_IOCTL_I2S_CONFIG_SAI_BUFFER, &_periodbufferconfig) != I2S_OK) {
        printf("Error: IO_IOCTL_I2S_CONFIG_SAI_BUFFER  failed \n");
	fclose(device_ptr);
	return -1;
    }


    default_sample_rate = AUDIO_SAMPLE_RATE;

    audio_format.ENDIAN = AUDIO_LITTLE_ENDIAN;
    audio_format.ALIGNMENT = AUDIO_ALIGNMENT_LEFT;
    audio_format.BITS = 16;
    audio_format.SIZE = 2;
    audio_format.CHANNELS = 2;

    mclk_freq = default_sample_rate * CLK_MULT;

    /* Setup audio data format in device */
    if (ioctl(device_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT, &audio_format) != I2S_OK) {
	printf("Error: Input data format not supported.\n");
	fclose(device_ptr);
	return -1;
    }

    /* Setup rest of parameters - master clock, valid data bits and sampling frequency */
    if (
        (ioctl(device_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &mclk_freq) != I2S_OK) ||
        (ioctl(device_ptr, IO_IOCTL_I2S_SET_DATA_BITS, &audio_format.BITS) != I2S_OK) ||
        (ioctl(device_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &default_sample_rate) != I2S_OK)
       ) {
        printf("  Error: Unable to setup sai device driver.\n");
        fclose(device_ptr);
        return -1;
    }

    /* Read back the sampling frequency to get real value */
    ioctl(device_ptr, IO_IOCTL_I2S_GET_FS_FREQ, &fs_freq);

    /* Print some interesting information about playback */
    printf("Playback information\n");
    printf("Sampling frequency:     %d Hz\n", fs_freq);
    printf("Bit depth:              %d bits\n", audio_format.BITS);
    printf("Channels: %s ", audio_format.CHANNELS == 1 ? "mono" : "stereo");
    
#if 0
    /* Setup audio codec */
    errcode = SetupCodec(device_ptr);
    if (errcode != 0) {
        printf("Audio codec configuration failed. Error 0x%X.\n", errcode);
        fclose(device_ptr);
        return -1;
    }
#endif
    /* Play a file */

    /* Start */
    //ioctl(device_ptr, IO_IOCTL_I2S_START_TX, NULL);


    return 0;
}

/*
 * write samples to device
 * must have correct set before write
 */
int32_t msi_snd_write(uint8_t * pcmStream, uint32_t pcmCnt) {

    I2S_STATISTICS_STRUCT stats;

    uint8_t *audiodata           = pcmStream;
    uint32_t trywritetotallength = audio_format.SIZE * pcmCnt;

    while(trywritetotallength)
    {

        if(periodbufferoff != 0){

            if(trywritetotallength + periodbufferoff < periodbuffersize){
                _mem_copy(audiodata, periodbuffer + periodbufferoff, trywritetotallength);
                periodbufferoff     +=  trywritetotallength;
                trywritetotallength  = 0;
                break;
            }else{
                /*Waiting for there are empty blocks*/
                ioctl(device_ptr, IO_IOCTL_I2S_WAIT_TX_EVENT, NULL);
                /* Get the info where to fill the data */
                ioctl(device_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats);

                _mem_copy(periodbuffer, stats.IN_BUFFER, periodbufferoff);
                _mem_copy(audiodata, stats.IN_BUFFER + periodbufferoff,  periodbuffersize - periodbufferoff);
                trywritetotallength -= periodbuffersize - periodbufferoff;
                audiodata           += periodbuffersize - periodbufferoff;
                periodbufferoff      =  0;
            }
        }
        else {
            if(trywritetotallength < periodbuffersize){
                _mem_copy(audiodata, periodbuffer, trywritetotallength);
                periodbufferoff     =  trywritetotallength;
                trywritetotallength = 0;
                break;
            }else{
                /*Waiting for there are empty blocks*/
                ioctl(device_ptr, IO_IOCTL_I2S_WAIT_TX_EVENT, NULL);
                /* Get the info where to fill the data */
                ioctl(device_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats);

                _mem_copy(audiodata, stats.IN_BUFFER, periodbuffersize);
                trywritetotallength -= periodbuffersize;
                audiodata           += periodbuffersize;
            }
        }

        ioctl(device_ptr, IO_IOCTL_I2S_UPDATE_TX_STATUS, &stats.SIZE);
    }

    return pcmCnt;
}

/*
 * collect resources of sound device
 */
int32_t msi_snd_deinit(void) {

    if(device_ptr != NULL){
	if (fclose(device_ptr) != 0){
	    printf("msi_snd_deinit fclose device_ptr failed \n");
	    return -1;
	}
	device_ptr = NULL;
        if(periodbuffer != NULL){
            _mem_free(periodbuffer);
            periodbuffer  = NULL;
        }
        periodbuffersize  = 0;
        periodbuffercount = 0;
        CodecDeinit();
	return 0;
    }
    else {
	printf("msi deinit called while it is not inited! \n");
	return -1;
    }
}

/*
 * set pcm sound format: sample rate, bit width, channel number
 */
int32_t msi_snd_set_format(uint32_t sampleRate, uint8_t bitWidth, uint8_t chNum) {

    uint32_t          mclk_freq, fs_freq, sample_rate;
    _mqx_int          errcode = 0;
    SAI_BUFFER_CONFIG _periodbufferconfig;



    if(device_ptr == NULL){
	printf("msi set format while it is not inited ! \n");
	return -1;
    }

    //return 0;

    ioctl(device_ptr, IO_IOCTL_I2S_STOP_TX, NULL);
    ioctl(device_ptr, IO_IOCTL_I2S_DISABLE_DEVICE, NULL);

    if( fclose(device_ptr) != 0){
	printf("msi_snd_set_format fclose device_ptr failed \n");
	return -1;
    }

    if(periodbuffer != NULL){
        _mem_free(periodbuffer);
    }

#if 0
    /* Initialize audio codec */
    printf("msi_snd_set_format initializing audio codec............. \n ");
    errcode = InitCodec();
    if (errcode != 0) {
        printf("msi_snd_set_format InitCodec failed \n");
	return -1;
    }
#endif

    periodbuffer    = _mem_alloc_system_zero(periodbuffersize);
    if(periodbuffer == NULL){
        printf("periodbuffer alloc failed! \n");
        return -1;
    }
    periodbufferoff = 0;

    /* re Open sai device and set it up */
    if ((device_ptr = fopen("sai:", "w")) == NULL) {
        printf("Error: Unable to open a sai device.\n");
        return -1;
    }

    /* re config period buffer  */ 

    _periodbufferconfig.PERIOD_SIZE   = periodbuffersize;
    _periodbufferconfig.PERIOD_NUMBER = periodbuffercount;

    if (ioctl(device_ptr, IO_IOCTL_I2S_CONFIG_SAI_BUFFER, &_periodbufferconfig) != I2S_OK) {
        printf("Error: IO_IOCTL_I2S_CONFIG_SAI_BUFFER  failed \n");
	fclose(device_ptr);
	return -1;
    }

    sample_rate = sampleRate;

    audio_format.ENDIAN = AUDIO_LITTLE_ENDIAN;
    audio_format.ALIGNMENT = AUDIO_ALIGNMENT_LEFT;
    audio_format.BITS = bitWidth;
    audio_format.SIZE = bitWidth / 8;
    audio_format.CHANNELS = chNum;


    mclk_freq = sample_rate * CLK_MULT;

    /* Setup audio data format in device */
    if (ioctl(device_ptr, IO_IOCTL_AUDIO_SET_IO_DATA_FORMAT, &audio_format) != I2S_OK) {
	printf("Error: Input data format not supported.\n");
	fclose(device_ptr);
	return -1;
    }

    /* Setup rest of parameters - master clock, valid data bits and sampling frequency */
    if (
        (ioctl(device_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &mclk_freq) != I2S_OK) ||
        (ioctl(device_ptr, IO_IOCTL_I2S_SET_DATA_BITS, &audio_format.BITS) != I2S_OK) ||
        (ioctl(device_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &sample_rate) != I2S_OK)
       ) {
        printf("  Error: Unable to setup sai device driver.\n");
	fclose(device_ptr);
        return -1;
    }


    /* Read back the sampling frequency to get real value */
    ioctl(device_ptr, IO_IOCTL_I2S_GET_FS_FREQ, &fs_freq);

    /* Print some interesting information about playback */
    printf("Playback information\n");
    printf("Sampling frequency:     %d Hz\n", fs_freq);
    printf("Bit depth:              %d bits\n", audio_format.BITS);
    printf("Channels: %s \n", audio_format.CHANNELS == 1 ? "mono" : "stereo");

#if 0
    /* Setup audio codec */
    errcode = SetupCodec(device_ptr);
    if (errcode != 0) {
	printf("Audio codec configuration failed. Error 0x%X.\n", errcode);
	fclose(device_ptr);
	return -1;
    }
#endif

    /* Start */
    //ioctl(device_ptr, IO_IOCTL_I2S_START_TX, NULL);

    return 0;
}

/*
 * turn up sound volume
 */
int32_t msi_snd_vol_up(void) {
    return CodecVolUp();
}

/*
 * turn down sound volume
 */
int32_t msi_snd_vol_down(void) {
    return CodecVolDown();
}

/*
 * mute sound device
 */
int32_t msi_snd_mute(void) {
    return CodecMute();
}
/*
 * umute device
 */
int32_t msi_snd_umute(void) {
    return CodecUmute();
}

/*
 * flush the pcm stream out
 * 
 */
int32_t msi_snd_flush(void) {
    if(device_ptr == NULL){
        printf("msi_snd_flush while device_ptr is NULL \n");
        return -1;
    }

    if(periodbufferoff != 0){
        I2S_STATISTICS_STRUCT stats;
        memset(periodbuffer + periodbufferoff, 0, periodbuffersize -  periodbufferoff);
        /*Waiting for there are empty blocks*/
        ioctl(device_ptr, IO_IOCTL_I2S_WAIT_TX_EVENT, NULL);
        /* Get the info where to fill the data */
        ioctl(device_ptr, IO_IOCTL_I2S_GET_STATISTICS, &stats);
        _mem_copy(periodbuffer , stats.IN_BUFFER, periodbuffersize);
        periodbufferoff = 0;
        ioctl(device_ptr, IO_IOCTL_I2S_UPDATE_TX_STATUS, &stats.SIZE);
    }

    return 0;
}
