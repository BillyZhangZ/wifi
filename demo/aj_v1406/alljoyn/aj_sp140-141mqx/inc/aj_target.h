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

typedef void *(*AJMODEBUTTONCB)(void *);
/**
 * Reboot the MCU
 */
void AJ_Reboot(void);
int AJ_ChangeTemperature(int step);
AJMODEBUTTONCB AJ_Get_ModeButton_CB(void);
int AJ_Register_ModeButton_CB(AJMODEBUTTONCB cb);

#define AJ_EXPORT

#endif
