/*! @addtogroup cci
 * @{
 *
 * Copyright (c) 2013-2014 Freescale, Inc.
 */
/*! @file errordefs.h
 *  @brief  Contains error codes.
 *
 */
 
#ifndef _ERRORDEFS_H
#define _ERRORDEFS_H

#if defined(__cplusplus)
extern "C" {
#endif
 
#ifndef __LANGUAGE_ASM__
#ifndef RT_STATUS_T_DEFINED
#define RT_STATUS_T_DEFINED
typedef int RtStatus_t;
#endif
#endif

#define SUCCESS                         (0x00000000)
#define ERROR_GENERIC                          (-1)
#define ERROR_OUT_OF_MEMORY             (0xfffffffe)

#define ERROR_MASK                      (-268435456)

#if defined(__cplusplus)
}
#endif

#endif /* _ERRORDEFS_H  */

/*
 * End of file
 * @}
 */