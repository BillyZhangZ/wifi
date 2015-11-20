#ifndef __ATTTYPES_H__
#define __ATTTYPES_H__
/****************************************************************************
 *
 * File:          AttTypes.h
 *
 * Description:   Some types specific to the attribute protocol.
 * 
 * Created:       August, 2008
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.10 $
 * $Project$
 *
 * ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of ALPWISE.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of ALPWISE.
 * 
 * Use of this work is governed by a license granted ALPWISE.
 * This work contains confidential and proprietary information of
 * ALPWISE. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

/**
 * @file AttTypes.h
 *
 * Some types specific to the attribute protocol.
 *
 * @author xavier.boniface@alpwise.com
 */

#include "ATT/AttConfig.h"

#if (ATTRIBUTE_PROTOCOL == 1)
/**
 * Return codes
 */
typedef U8 AttStatus;


  #define ATTSTATUS_SUCCESS        BLESTATUS_SUCCESS
  #define ATTSTATUS_FAILED         BLESTATUS_FAILED
  #define ATTSTATUS_PENDING        BLESTATUS_PENDING
  #define ATTSTATUS_BUSY           BLESTATUS_BUSY
  #define ATTSTATUS_INVALID_PARM   BLESTATUS_INVALID_PARMS

  /** Add generic error codes which are not defined in the BLE stack
   *  as from this offset: */
  #define ATTSTATUS_OPEN_GENERIC   (BLESTATUS_INVALID_PARMS + 1)
  /* (guessed BLESTATUS_INVALID_PARM was the highest one!) */

#if (   ATTSTATUS_OPEN_GENERIC <= BLESTATUS_SUCCESS \
     || ATTSTATUS_OPEN_GENERIC <= BLESTATUS_FAILED  \
     || ATTSTATUS_OPEN_GENERIC <= BLESTATUS_PENDING \
     || ATTSTATUS_OPEN_GENERIC <= BLESTATUS_BUSY    \
     || ATTSTATUS_OPEN_GENERIC <= BLESTATUS_INVALID_PARMS)
  #error AttTypes.h: ATTSTATUS_OPEN_GENERIC shall be greater than all generic error codes!
#endif

// ATT specific error codes 
  #define ATTSTATUS_NOT_FOUND		(ATTSTATUS_OPEN_GENERIC + 0)
  #define ATTSTATUS_NO_CONNECTION	(ATTSTATUS_OPEN_GENERIC + 1)
  #define ATTSTATUS_DATABASE_FROZEN (ATTSTATUS_OPEN_GENERIC + 2)
  #define ATTSTATUS_DATABASE_FULL   (ATTSTATUS_OPEN_GENERIC + 3)
  // Bug in this implementation of the attribute protocol: 
  #define ATTSTATUS_INTERNAL        (ATTSTATUS_OPEN_GENERIC + 4)
  // The error is described by a separate AttPduStatus error code from the
  //  ATT spec. 
  #define ATTSTATUS_ATT_SPEC        (ATTSTATUS_OPEN_GENERIC + 5)


/**
 * Handle of a remote ATT device.
 */
typedef U16 AttDevice;
#define ATTSTACK_INVALID_DEVICE           0x0000



/**
 * @brief Allows to host either a 16-bits UUIDs directly or a pointer on 128-bits UUIDs.
 */
typedef union
{
  U16 uuid16;
  U8 *uuid128;
} AttUuidValue;

typedef U8 AttUuidSize;
#define ATT_UUID_SIZE_16  0
#define ATT_UUID_SIZE_128 1
/* Only these two sizes are supported by the ATT for now. */

typedef struct
{
  AttUuidSize  size ;
  AttUuidValue value;
} AttUuid;


#if (ATT_ROLE_SERVER == 1)

/**
 * Structures to store an attribute in the attribute database:
 * service attributes and all other ones.
 */

/**
 * Fields common to both the AttAttribute and teh AttServiceAttribute 
 * structures.
 *
 * Beware that data8 and data9 shall always be placed first.
 *
 * @todo: Hide the type of AttUuidValue, but how?
 */
#define COMMON_FIELDS		\
  void         *data8;		\
  void         *data9;		\
  void         *data10;		\
  union 					\
  {							\
	void        *data11;	\
    U32			data12;		\
  }	v;						\
  AttUuidValue  data7;		\
  U16           data6;		\
  U16			data13;		\
  U8            data3;		\
  U8            data5;	

/*
*data8;   // node
*data9;   // node
*data10;  //the pointer to the attribute that is the service for this attribute
*data11;  // The pointer to the value
 data12;  // The value directly saved
 data7;   // The uuid for the attribute
 data6;   // The current length of this attribute
 data8;	  // The max len of a VARIABLE attribute
 data3;   // flag (value or pointer) (16 or 32 bit) (fixed or variable) (valid o
 data5;	  // attribute permission
*/




typedef struct
{
  /**
   * All fields of this structure are internal i.e. for use by the Attribute
   * Protocol only!
   *
   * Beware that the COMMON_FIELDS shall remain at the beginning of the
   * structure.
   */

  COMMON_FIELDS

} AttAttribute;

typedef struct
{
  /**
   * All fields of this structure are internal i.e. for use by the Attribute
   * Protocol only!
   *
   * Beware that the COMMON_FIELDS shall remain at the beginning of the
   * structure.
   */

  COMMON_FIELDS

  void             *dataService1;

} AttServiceAttribute;


typedef struct
{
  /**
   * All fields of this structure are internal i.e. for use by the Attribute
   * Protocol only!
   *
   * Beware that the COMMON_FIELDS shall remain at the beginning of the
   * structure.
   */

  COMMON_FIELDS

  U16   instanceMemory[BLE_NUM_MAX_CONNECTION];
  U8    *instantiatedValues;
  const U8*   defaultValue;

#if (BLE_SECURITY == 1)
  U8    flags;
  U8    infotype;
#endif //(BLE_SECURITY == 1)

} AttInstantiatedAttribute;

typedef struct
{
  /**
   * All fields of this structure are internal i.e. for use by the Attribute
   * Protocol only!
   *
   * Beware that the COMMON_FIELDS shall remain at the beginning of the
   * structure.
   */

  COMMON_FIELDS

  U8     characteristicValue[5];

} Att16BitCharacteristicAttribute;

typedef struct
{
  /**
   * All fields of this structure are internal i.e. for use by the Attribute
   * Protocol only!
   *
   * Beware that the COMMON_FIELDS shall remain at the beginning of the
   * structure.
   */

  COMMON_FIELDS

  U8     characteristicValue[19];

} Att128BitCharacteristicAttribute;



#endif //(ATT_ROLE_SERVER == 1)

#endif //(ATTRIBUTE_PROTOCOL == 1) 

#endif // __ATTTYPES_H__
