/******************************************************************************
 * Copyright 2013, Qualcomm Connected Experiences, Inc.
 *
 ******************************************************************************/
#include "aj_target.h"

#define ADLER_PRIME  65521

/*
 * Adler32 CRC computation
 */
uint32_t CRC(const uint8_t* data, size_t len)
{
    uint32_t adler = 1;

    while (data && len) {
        size_t l = len % 3800; // Max number of iterations before modulus must be computed
        uint32_t a = adler & 0xFFFF;
        uint32_t b = adler >> 16;
        len -= l;
        while (l--) {
            a += *data++;
            b += a;
        }
        adler = ((b % ADLER_PRIME) << 16) | (a % ADLER_PRIME);
    }
    return adler;
}

