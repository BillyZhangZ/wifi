/**
 * @file
 */
/******************************************************************************
 * Copyright 2013, Qualcomm Connected Experiences, Inc.
 *
 ******************************************************************************/


#include "aj_target.h"
#include "aj_init.h"
#include "aj_crypto.h"
#include "aj_creds.h"
#include "aj_nvram.h"

static uint8_t initialized = FALSE;

void AJ_Initialize(void)
{
    AJ_GUID localGuid;
    if (!initialized) {
        initialized = TRUE;
        AJ_NVRAM_Init();
        /*
         * This will seed the random number generator
         */
        AJ_RandBytes(NULL, 0);
        /*
         * This will initialize credentials if needed
         */
        AJ_GetLocalGUID(&localGuid);
    }
}
