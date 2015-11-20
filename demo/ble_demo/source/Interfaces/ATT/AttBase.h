#ifndef __ATTBASE_H__
#define __ATTBASE_H__
/****************************************************************************
 *
 * File:          AttBase.h
 *
 * Description:   Includes common to both the client and server role.
 * 
 * Created:       June, 2008
 * Version:       0.1
 *
 * CVS Revision : $Revision: 1.9 $
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
 * @file AttBase.h
 *
 * Includes common to both the client and server role.
 *
 * @author xavier.boniface@alpwise.com
 */

#include "ATT/AttConfig.h"
#include "ATT/AttTypes.h"



#if (ATTRIBUTE_PROTOCOL == 1)

/**
 * Maximum size of a long attribute as defined in the ATT spec.
 */
#define ATT_MAX_VALUE_LENGTH 511


/**
 * Status codes in attribute PDUs (from the spec)
 */
typedef U8 AttPduStatus;
#define ATTPDU_STATUS_INVALID_HANDLE				0x01
#define ATTPDU_STATUS_READ_NOT_PERMITTED			0x02
#define ATTPDU_STATUS_WRITE_NOT_PERMITTED			0x03
#define ATTPDU_STATUS_INVALID_PDU					0x04
#define ATTPDU_STATUS_INSUFFICIENT_AUTHENTICATION	0x05
#define ATTPDU_STATUS_REQUEST_NOT_SUPPORTED			0x06
#define ATTPDU_STATUS_INVALID_OFFSET				0x07
#define ATTPDU_STATUS_INSUFFICIENT_AUTHORIZATION	0x08
#define ATTPDU_STATUS_PREPARE_WRITE_QUEUE_FULL		0x09
#define ATTPDU_STATUS_ATTRIBUTE_NOT_FOUND			0x0A
#define ATTPDU_STATUS_ATTRIBUTE_NOT_LONG			0x0B
//#define ATTPDU_STATUS_INSUFICIENT_KEY_SIZE		0x0C
#define ATTPDU_STATUS_INVALID_ATTRIBUTE_VALUE_LEN	0x0D
//#define ATTPDU_STATUS_UNLIKELY_EROR				0x0E
#define ATTPDU_STATUS_INSUFFICIENT_ENCRYPTTION		0x0F
#define ATTPDU_STATUS_UNSUPPORTED_GROUP_TYPE		0x10
//#define ATTPDU_STATUS_INSUFFICIENT_RESSOURCES		0x11

// Application error codes' range: 
#define ATTPDU_STATUS_APPLICATION_ERROR_FIRST		0x80
#define ATTPDU_STATUS_APPLICATION_ERROR_LAST		0xFF

// Not yet defined in the spec. Hence:
// But used internally
// Means that the ATT operation have succeeded
#define ATTPDU_STATUS_SUCCESS						0x00
// Means that the ATT operation have failed du to a timeout
#define ATTPDU_STATUS_TIMEOUT						0x66
// Means that the ATT operation have failed du to an invalid
//connection
#define ATTPDU_STATUS_CONNECTION					0x67


/**
 * IS_APPLICATION_ERROR_CODE
 * Macro to check whether an AttPduStatus is in the application error
 * code range.
 */
U8 IS_APPLICATION_ERROR_CODE(AttPduStatus pduStatus);
#define IS_APPLICATION_ERROR_CODE(pduStatus) \
(    ((pduStatus) >= ATTPDU_STATUS_APPLICATION_ERROR_FIRST) \
  && ((pduStatus) <= ATTPDU_STATUS_APPLICATION_ERROR_LAST)  )



/**
 * Handle of an attribute.
 */
typedef U16 AttHandle;
// The handle 0x0000 is specified as the Invalid Handle
// All other values (from 1 to 0xFFFF=65535) are valid  handles.
#define ATT_INVALID_HANDLE ((AttHandle)( 0)) 
// First minimum attribute handle is 1
#define ATT_MIN_HANDLE     ((AttHandle)( 1)) 
// Maximum attribute handle is 0xFFFF
#define ATT_MAX_HANDLE     ((AttHandle)(0xFFFF)) 


/**
 * @brief Attribute PDU operation codes. from the spec
 *
 */
typedef U8 AttOpCode;
#define ATTOPCODE_ERROR_RSP                 0x01

#define ATTOPCODE_EXCHANGE_MTU_REQ			0x02
#define ATTOPCODE_EXCHANGE_MTU_RSP			0x03

#define ATTOPCODE_FIND_INFORMATION_REQ      0x04
#define ATTOPCODE_FIND_INFORMATION_RSP      0x05

#define ATTOPCODE_FIND_BY_TYPE_VALUE_REQ    0x06
#define ATTOPCODE_FIND_BY_TYPE_VALUE_RSP    0x07

#define ATTOPCODE_READ_BY_TYPE_REQ          0x08
#define ATTOPCODE_READ_BY_TYPE_RSP          0x09

#define ATTOPCODE_READ_REQ                  0x0A
#define ATTOPCODE_READ_RSP                  0x0B

#define ATTOPCODE_READ_BLOB_REQ             0x0C
#define ATTOPCODE_READ_BLOB_RSP             0x0D

#define ATTOPCODE_READ_MULTIPLE_REQ         0x0E
#define ATTOPCODE_READ_MULTIPLE_RSP         0x0F

#define ATTOPCODE_READ_BY_GROUP_REQ			0x10
#define ATTOPCODE_READ_BY_GROUP_RSP			0x11

#define ATTOPCODE_WRITE_REQ                 0x12
#define ATTOPCODE_WRITE_RSP                 0x13

#define ATTOPCODE_WRITE_CMD					0x52

#define ATTOPCODE_PREPARE_WRITE_REQ         0x16
#define ATTOPCODE_PREPARE_WRITE_RSP         0x17

#define ATTOPCODE_EXECUTE_WRITE_REQ         0x18
#define ATTOPCODE_EXECUTE_WRITE_RSP         0x19

#define ATTOPCODE_HANDLE_VALUE_NOTIFICATION	0x1B
#define ATTOPCODE_HANDLE_VALUE_INDICATION	0x1D
#define ATTOPCODE_HANDLE_VALUE_CNF          0x1E

//TODO: Remove no more exist but signed command only
#define ATTOPCODE_SIGNED_WRITE_REQ          0x92

#define ATTOPCODE_SIGNED_WRITE_CMD          0xD2

/* Not defined in the spec but used internally: */
#define ATTOPCODE_INVALID                   0x00
#define ATTOPCODE_CONN_FAILURE              0xFD
#define ATTOPCODE_SEND_CNF                  0xFE
#define ATTOPCODE_TIMEOUT                   0xFF

/**
 * 'Flags' parameter of Att_Client_SendExecuteRequest() and used in
 * HandleExecuteWriteRequest().
 */
typedef U8 AttFlags;
#define ATTFLAGS_CANCEL ((AttFlags) 0x00)
#define ATTFLAGS_WRITE  ((AttFlags) 0x01)

/**
 * @brief Construction of the 'flags' byte in a Read Information response.
 *
 * Values from the spec.
 */
typedef U8 AttFormat;
#define ATTFORMAT_16BIT_UUID	0x01
#define ATTFORMAT_128BIT_UUID	0x02
#define ATTFORMAT_ONLY_HANDLES	0x03

/**
 * length of the different fields of a PDU (in bytes).
 * From spec:
 * used by both the client and the server
 */
#define ATTPDU_SIZEOF_OPCODE       1
#define ATTPDU_SIZEOF_HANDLE       2
#define ATTPDU_SIZEOF_16_BIT_UUID  2
#define ATTPDU_SIZEOF_128_BIT_UUID 16
#define ATTPDU_SIZEOF_STATUS       1
#define ATTPDU_SIZEOF_FORMAT       1 // find information response
#define ATTPDU_SIZEOF_FLAGS        1 
#define ATTPDU_SIZEOF_OFFSET       2 
#define ATTPDU_SIZEOF_MTU		   2
#define ATTPDU_SIZEOF_VALUELEN	   1 // in read by type request

/**
 * AttCharacteriticProperties
 *	A bitfield describing the Availbale characteristic properties
 */
typedef U8  AttCharacteriticProperties;
// If set, permits broadcasts of the Characteristic Value using Characteristic
// Configuration Descriptor
#define	ATTPROPERTY_BROADCAST				(0x01u)
// If set, permits reads of the Characteristic Value 
#define	ATTPROPERTY_READ					(0x02u)
// If set, permit writes of the Characteristic without acknowledgement
#define	ATTPROPERTY_WRITEWITHOUTRESPONSE	(0x04u)
//If set, permits writes of the Characteristic Value
#define	ATTPROPERTY_WRITE					(0x08u)
// If set, permits notifications of a Characteristic Value without
// acknowledgement
#define	ATTPROPERTY_NOTIFY					(0x10u)
// If set, permits indications of a Characteristic Value with acknowledgement 
#define	ATTPROPERTY_INDICATE				(0x20u)
// If set, permits signed writes to the Characteristic Value 
#define	ATTPROPERTY_SIGNEDWRITE				(0x40u)
// If set, additional characteristic properties are defined in the
// Characteristic Extended Properties Descriptor 
#define	ATTPROPERTY_EXTENDEDPROPERTIES		(0x80u)


/**
 * AttClientConfig
 *	A bitfield describing the Available possible configuration to set in the
 *  Client Configuration characteristic Descriptor
 */
typedef U16 AttClientConfig;
#define ATT_CLIENTCONFIG_NOTIFICATION		(0x0001u)
#define ATT_CLIENTCONFIG_INDICATION			(0x0002u)


#endif //(ATTRIBUTE_PROTOCOL == 1) 

#endif /* __ATTBASE_H__ */
