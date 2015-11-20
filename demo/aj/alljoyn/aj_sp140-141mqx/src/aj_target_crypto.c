/**
 * @file
 */
/******************************************************************************
 * Copyright 2013-2014, Qualcomm Connected Experiences, Inc.
 *
 ******************************************************************************/

#include "aj_target.h"
#include "aj_crypto.h"
#include "aj_util.h"
#include "qca_includes.h"

static uint8_t seed[16];
static uint8_t key[16];

#if !BSPCFG_ENABLE_ADC0
static void GatherBits(uint8_t* buffer, uint32_t len)
{
    AJ_Time time = {0, 0};
    /*
     * This is a very crappy entropy gathering function
     */
    while (len--) {
        *buffer++ |= (uint8_t) AJ_GetElapsedTime(&time, 0);
    }
}
#else


//#define SHOW_RANDOM_BITS 1

static const ADC_INIT_STRUCT adc_init = {
    ADC_RESOLUTION_16BIT
};

static const ADC_INIT_CHANNEL_STRUCT adc_channel_params =
{
    ADC0_SOURCE_TEMP,
    ADC_CHANNEL_MEASURE_LOOP | ADC_CHANNEL_START_TRIGGERED,
    10,              /* number of samples in one run sequence */
    0,               /* time offset from trigger point in us */
    1000,            /* period in use (= 1 ms) */
    0x10000,         /* scale range of result (not used now) */
    10,              /* circular buffer size (sample count) */
    ADC_PDB_TRIGGER, /* logical trigger ID that starts this ADC channel */
#if MQX_USE_LWEVENTS
    NULL,            /* lwevent used */
    0                /* mask of event to be set */
#endif
};


static uint8_t RandBit(MQX_FILE_PTR fd_ch)
{
    ADC_RESULT_STRUCT val;
    uint32_t sanity = 1000;

    val.result = -1;
    while (read(fd_ch, &val, sizeof(val)) != sizeof(val)) {
        AJ_Sleep(1);
        if (--sanity == 0) {
            AJ_Printf("Failed to read from ADC within 1 second\n");
            break;
        }
    }
    return (uint8_t)(val.result & 1);
}

static void GatherBits(uint8_t* buffer, uint32_t len)
{
    MQX_FILE_PTR fd_adc;
    MQX_FILE_PTR fd_ch;
    int i;

    fd_adc = fopen("adc0:", (const char*)&adc_init);
    if (NULL == fd_adc) {
        AJ_Printf("ADC device open failed\n");
        return;
    }
    fd_ch = fopen("adc0:temp", (const char*)&adc_channel_params);
    if (fd_ch == NULL) {
        AJ_Printf("ADC channel open failed\n");
        fclose(fd_adc);
        return;
    }
    memset(buffer, 0, len);
    /*
     * Trigger to start acquiring data from the temperature sensor
     */
    ioctl(fd_adc, ADC_IOCTL_FIRE_TRIGGER, (void*)ADC_PDB_TRIGGER);

    for (i = 0; i < len; ++i) {
        int j;
        uint8_t r = 0;
        for (j = 0; j < 8; ++j) {
            r <<= 1;
            r |= RandBit(fd_ch);
        }
        buffer[i] = r;
    }
    fclose(fd_ch);
    fclose(fd_adc);

#ifdef SHOW_RANDOM_BITS
    for (i = 0; i < len; ++i) {
        int j;
        int r = buffer[i];
        for (j = 0; j < 8; ++j) {
            AJ_Printf("%c", '0' + (r & 1));
            r >>= 1;
        }
    }
    AJ_Printf("\n");
#endif
}
#endif

void AJ_RandBytes(uint8_t* rand, uint32_t len)
{
    /*
     * On the first call we need to accumulate entropy
     * for the seed and the key.
     */
    if (seed[0] == 0) {
        GatherBits(seed, sizeof(seed));
        GatherBits(key, sizeof(key));
    }
    AJ_AES_Enable(key);
    /*
     * This follows the NIST guidelines for using AES as a PRF
     */
    while (len) {
        uint32_t tmp[4];
        uint32_t sz = min(16, len);
        MQX_TICK_STRUCT ticks;
        _time_get_elapsed_ticks(&ticks);
        tmp[0] = (uint32_t)ticks.TICKS[0];
        tmp[1] += 1;
        AJ_AES_ECB_128_ENCRYPT(key, (uint8_t*)tmp, (uint8_t*)tmp);
        AJ_AES_CBC_128_ENCRYPT(key, seed, seed, 16, (uint8_t*)tmp);
        memcpy(rand, seed, sz);
        AJ_AES_CBC_128_ENCRYPT(key, seed, seed, 16, (uint8_t*)tmp);
        len -= sz;
        rand += sz;
    }
    AJ_AES_Disable();
}

