#ifndef _AJ_TARGET_H
#define _AJ_TARGET_H
/**
 * @file
 */
/******************************************************************************
 * Copyright 2013, Qualcomm Connected Experiences, Inc.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <assert.h>
#include <a_types.h>

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif

#ifndef min
#define min(x, y) ((x) < (y) ? (x) : (y))
#endif

#define WORD_ALIGN(x) ((x & 0x3) ? ((x >> 2) + 1) << 2 : x)

#define HOST_IS_LITTLE_ENDIAN  1
#define HOST_IS_BIG_ENDIAN     0

#define AJ_Printf    printf
#define AJ_ASSERT(x) assert(x)

typedef enum {
    AJ_SENSOR_TEMPERATURE,
    AJ_SENSOR_HUMIDITY,
    AJ_SENSOR_PM2_5,
    AJ_SENSOR_UNKNOW,
} AJ_SENSOR_TYPE;

typedef void *(*AJMODEBUTTONCB)(void *);
/**
 * Reboot the MCU
 */
void AJ_Reboot(void);
int AJ_ChangeTemperature(int step);
AJMODEBUTTONCB AJ_Get_ModeButton_CB(void);
int AJ_Register_ModeButton_CB(AJMODEBUTTONCB cb);
int AJ_Sensors_Fetch(AJ_SENSOR_TYPE type, void *buf);

#define AJ_EXPORT


/**
 * For testing purposes, if USE_MAC_FOR_GUID is defined then the AJ_GUID will be
 * created from the device's MAC address. This allows the AJ_GUID for a device
 * to remain fixed even if it is erased from NVRAM.
 * This depends on the function AJ_GetLocalGUID using the function defined by
 * AJ_CreateNewGUID to initialize the AJ_GUID for the first time.
 */
#ifdef USE_MAC_FOR_GUID
#define AJ_CreateNewGUID AJ_CreateNewGUIDFromMAC
extern void AJ_CreateNewGUIDFromMAC(uint8_t* rand, uint32_t len);
#else
#define AJ_CreateNewGUID AJ_RandBytes
#endif


#endif
