#ifndef __BLETYPES_H
#define __BLETYPES_H
/****************************************************************************
 *
 * File:          BleTypes.h
 *
 * Description:   Types used and exported by the BLE stack
 * 
 * Created:       January, 2008
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.21 $
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


#include "bleConfig.h"


/***************************************************************************
 *	Type definition
 ***************************************************************************/


#if (BLETYPES_ALREADY_DEFINED == 0)
/** Definition for the U8 type, unsigned 8-bit size (1 byte)
 */
typedef unsigned char	U8;

/** Definition for the U16 type, unsigned 16-bit size (2 byte)
 */
typedef unsigned short	U16;

/** Definition for the U32 type, unsigned 32-bit size (4 byte)
 */
typedef unsigned long	U32;

/** Definition for the S8 type, signed 8-bit size (1 byte)
 */
typedef signed char		S8;

/** Definition for the S16 type, signed 16-bit size (2 byte)
 */
typedef signed short	S16;

#endif //(BLETYPES_ALREADY_DEFINED == 0)


/** Definition for the BleStatus type, representing general status information
 *  in the stack
 */
typedef U8 BleStatus;

#define BLESTATUS_SUCCESS							0x00
#define BLESTATUS_FAILED							0x01
#define BLESTATUS_PENDING							0x02
#define BLESTATUS_BUSY								0x03
#define BLESTATUS_INSUFFICIENT_AUTHENTICATION		0x04
#define BLESTATUS_INSUFFICIENT_AUTHORIZATION		0x05
#define BLESTATUS_INSUFFICIENT_ENCRYPTION			0x06
#define BLESTATUS_INVALID_PARMS						0x10
#define BLESTATUS_NO_RESOURCES      				0x11

/** Definition for the BD_ADDR type, identifying a Bluetooth device Address
 */
typedef struct {
    U8    addr[6];
} BD_ADDR;


/** Definition for the AddressType type
 * The Bluetooth device Address (BD_ADDR) could be a public address 
 * (BLEADDRESS_PUBLIC) or a random address (BLEADDRESS_RANDOM)
 */
typedef U8	BleAddressType;

// The address type is a public address
#define BLEADDRESS_PUBLIC							0x00

// The address type is a random address
#define BLEADDRESS_RANDOM							0x01


#if (BLE_CONNECTION_SUPPORT==1)
/** Definition for the BleConnRole type
 *
 * Indicates the connection role of the local device in the connection 
 * information could be Master role (BLECONNROLE_MASTER) or Slave role 
 * (BLECONNROLE_SLAVE)
 */
typedef U8 BleConnRole;

// Local device has the Master role
#define	BLECONNROLE_MASTER							0x00

// Local device has the Slave role
#define	BLECONNROLE_SLAVE							0x01


/** Definition for the BleDisconnectionReason type
 *
 * Indicates the disconnection reason reported during a link layer
 * disconnection.
 * This subset is the most common disconnection error codes received
 * the complete list of possible error code can be found in the
 * Bluetooth Core Specification 4.0 Volume 2 part D: Error Code
 */
typedef U8 BleDisconnectionReason;

// Disconnection du to a connection time-out
#define	BLEDISCREASON_CONNECTIONTIMEOUT							0x08
// Disconnection requested by the remote device users
#define	BLEDISCREASON_REMOTEUSER								0x13
// Disconnection by the remote device due to low ressources
#define	BLEDISCREASON_REMOTELOWRESOURCES						0x14
// Disconnection by the remote device due to Power off
#define	BLEDISCREASON_REMOTEPOWEROFF							0x15
// Disconnection by the local device Host stack
#define	BLEDISCREASON_LOCALHOST									0x16
//Unacceptable connection Interval
#define BLEDISCREASON_UNACCEPTABLECONNECTIONINTERVAL			0x3B
// The LL initiated a connection but the connection has failed to be 
//  established.
#define	BLEDISCREASON_LL_FAILED_TO_ESTABLISH					0x3E

#endif //(BLE_CONNECTION_SUPPORT==1)

/***************************************************************************
 *	Macros definition
 ***************************************************************************/


/** LE_TO_U16 
 *	This macro convert a LSO First (Less Significant Octets first - Little endian)
 *  single byte based and formated  (U8) buffer into a 2 octets unsigned integer.
 *  The macro definitions do the same thing: 
 *  if the input buffer is 0x01 0x02 the output U16 is 0x0201.
 *  For a Big-Endian based microprocessor; there is only one choice :
 *	 - Take the first octets and adding a shift of the second octets. 
 *  For a Little-Endian based microprocessor; there is two choices:
 *	 - Also take the first octets and adding a sift of the second octets.
 *	 - Cast directly the buffer into a U16, It saves some instructions
 *	 and uses less registers ( generally, three registers are used for addition and
 *	 shifting, none for direct cast)
 *  
 *	ptr is an U8 *
 */
#if (BLEPROCESSOR_TYPE	== BLEPROCESSOR_LITTLEENDIAN)
	// The processor type is LITTLE_ENDIAN
	// there is an optimisation to do here just 
	// - cast directly the buffer into a U16.
#define LE_TO_U16(ptr)	(U16)( * ((U16 *)(ptr)) )

#else //(BLEPROCESSOR_TYPE	== BLEPROCESSOR_LITTLEENDIAN)
	// The processor type is UNKNOWN or BIG_ENDIAN
	// there is nothing to do here just 
	// - Take the first octets and adding a sift of the second octets. 
#define LE_TO_U16(ptr)	  (U16)( (U16)(*(ptr)) + (U16) (*(ptr + 1) << 8) )

#endif	//(BLEPROCESSOR_TYPE	== BLEPROCESSOR_LITTLEENDIAN)
 



#endif /*__BLETYPES_H*/
