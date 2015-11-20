/**
 * @file
 */
/******************************************************************************
 * Copyright 2013, 2014 Qualcomm Connected Experiences, Inc.
 *
 ******************************************************************************/
#include <qca_includes.h>

void* AJ_Malloc(uint32_t sz)
{
    void* mem = A_MALLOC(sz, MALLOC_ID_CONTEXT);
    if (!mem) {
        AJ_Printf("!!!Failed to malloc %d bytes\n", sz);
    }
    return mem;
}

void AJ_Free(void* mem)
{
    if (mem) {
        A_FREE(mem, MALLOC_ID_CONTEXT);
    }
}
