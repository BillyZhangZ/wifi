#ifndef __ATTCONFIG_H__
#define __ATTCONFIG_H__
/****************************************************************************
 *
 * File:          attconfig.h
 *
 * Description:   Default configuration options for the attribute protocol.
 * 
 * Created:       June, 2008
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.14 $
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
 * @file attconfig.h
 *
 * Default configuration options for the attribute protocol.
 *
 * This config may be overriden by a custom configuration in blecustomconfig.h.
 *
 * @author alexandre GIMARD
 */

#include "BleTypes.h"
#include "BleConfig.h"

/* All that is already defined before this file is pre-processed will not be
 * defined here thanks to all #ifndef.
 *
 * Rule: 0 to disable, 1 to enable.
 */


/** ATTRIBUTE_PROTOCOL defines if the local Device supports (set to 1) or 
 * does not support (set to 0) the ATTRIBUTE Protocol 
 * Default behavior is to support it.
 */
#ifndef ATTRIBUTE_PROTOCOL
  #define ATTRIBUTE_PROTOCOL	                        1
#endif //ATTRIBUTE_PROTOCOL

#if (BLE_CONNECTION_SUPPORT == 1) && (ATTRIBUTE_PROTOCOL == 0)
#error "ATTRIBUTE_PROTOCOL shall be enabled when connection is supported"
#endif //(BLE_CONNECTION_SUPPORT == 1)

#if (ATTRIBUTE_PROTOCOL == 1)


/** ATT_ROLE_CLIENT defines if the local Device supports (set to 1) or 
 * does not support (set to 0) the ATTRIBUTE Protocol CLIENT role
 * Default behavior is to support it.
 */
#ifndef ATT_ROLE_CLIENT
  #define ATT_ROLE_CLIENT                               0 
#endif //ATT_ROLE_CLIENT

/** ATT_ROLE_SERVER defines if the local Device supports (set to 1) or 
 * does not support (set to 0) the ATTRIBUTE Protocol SERVER role
 * Default behavior is to support it.
 * The SERVER Role is in fact mandatory in case of a device supporting
 * connection because a device shall host the GAP and GATT attributes.
 */
//#ifndef ATT_ROLE_SERVER
//  #define ATT_ROLE_SERVER     				        1 
//#endif //ATT_ROLE_SERVER
//clear eventual predefinition
#undef ATT_ROLE_SERVER
#if (BLE_CONNECTION_SUPPORT == 1)
#define ATT_ROLE_SERVER     				            1 
#else //(BLE_CONNECTION_SUPPORT == 1)
#define ATT_ROLE_SERVER     				            0
#endif //(BLE_CONNECTION_SUPPORT == 1)


// If attribute Protocol is enabled  (ATTRIBUTE_PROTOCOL == 1)
// The local device shall at least support one role.
//#if (ATT_ROLE_SERVER!=1) && (ATT_ROLE_CLIENT!=1)
//  #error ATT config: you shall be client and/or server!
//#endif

// Ignore if attribute Protocol Client is enabled  (ATT_ROLE_CLIENT == 1)
// if Local BLE device does not supports connetion
#if (BLE_CONNECTION_SUPPORT == 0) 
#undef ATT_ROLE_CLIENT
#define ATT_ROLE_CLIENT     				             0
#endif //(BLE_CONNECTION_SUPPORT == 0) 

// Ignore If attribute Protocol is enabled  (ATTRIBUTE_PROTOCOL == 1)
// if neither CLIENT or SERVER is enabled
#if (ATT_ROLE_SERVER!=1) && (ATT_ROLE_CLIENT!=1)
#undef ATTRIBUTE_PROTOCOL
#define ATTRIBUTE_PROTOCOL     				            0
#endif //(ATT_ROLE_SERVER!=1) && (ATT_ROLE_CLIENT!=1)

/** ATT_DEBUG_TRACES defines if the Attribute Protocol Layer exports some debug 
 * traces during Runtime.
 * The debug print are exported through the SYSTEM_Log() function
 * Default behavior is to not export traces.
 * Note that this option is ignored and overided when BLE_PRINT_DEBUG_TRACES is
 * disabled (set to 0)
 */
#ifndef ATT_DEBUG_TRACES
#define ATT_DEBUG_TRACES                                0 
#endif //ATT_DEBUG_TRACES


/**
 * Performs extra check at the cost of speed and/or memory.
 *
 * Note: this does not only significantly increase ROM, but also RAM because
 * of all asserts.
 */
#ifndef ATT_DEBUG
  #define ATT_DEBUG                                     0
#endif //ATT_DEBUG


#ifndef ATT_PARMS_CHECK
/**
 * If disabled, parameters will not be checked within the functions. This saves
 * ROM but expects the profile will not make any mistake when calling the API!
 */
#define ATT_PARMS_CHECK                                 BLE_PARMS_CHECK

#endif //ATT_PARMS_CHECK


/***************************************************************************
 *	ATTRIBUTE CLIENT SPECIFIC CONFIGURATION
 ***************************************************************************/
#if (ATT_ROLE_CLIENT==1)

/**
 * ATT_RESPONSE_TIMEOUT
 *
 * If no response to a command is received by a given time, we allow the
 * client to send another command.
 * A transaction not completed within 30 seconds shall time out. 
 * Such a transaction shall be considered to have failed and the local higher
 * layers shall be informed of this failure. 
 * No more attribute protocol requests, commands, indications or 
 * notifications shall be sent to the target device.
 *
 * Unit: milliseconds.
 * 0 means infinite (no timeout).
 */
#ifndef ATT_RESPONSE_TIMEOUT
#define ATT_RESPONSE_TIMEOUT		                    30000
#endif //ATT_RESPONSE_TIMEOUT




/*---------------------------------
 * Optional ATTRIBUTE CLIENT features
 * 0 = DISBLED
 * 1 = ENABLED
 */

/* ATT_CLIENT_SUPPORT_EXCHANGE_MTU_REQUEST
 *	Allow the client to send the EXCHANGE MTU Request
 */
#ifndef ATT_CLIENT_SUPPORT_EXCHANGE_MTU_REQUEST
#define ATT_CLIENT_SUPPORT_EXCHANGE_MTU_REQUEST			1
#endif //ATT_CLIENT_SUPPORT_EXCHANGE_MTU_REQUEST

/* ATT_CLIENT_SUPPORT_FIND_BY_TYPE_VALUE_REQUEST
 *	Allow the client to send the FIND BY TYPE VALUE Request
 */
#ifndef ATT_CLIENT_SUPPORT_FIND_BY_TYPE_VALUE_REQUEST
#define ATT_CLIENT_SUPPORT_FIND_BY_TYPE_VALUE_REQUEST	1
#endif //ATT_CLIENT_SUPPORT_FIND_BY_TYPE_VALUE_REQUEST

#endif //(ATT_ROLE_CLIENT == 1)


/***************************************************************************
 *	ATTRIBUTE SERVER SPECIFIC CONFIGURATION
 ***************************************************************************/
#if (ATT_ROLE_SERVER==1)

/** HANLDES MANAGEMENT
 * It exists in the stack several level of internal handle management
 * - If ATT_SERVER_HANDLES_ARE_FREEZABLE is DISABLED and 
 *   ATT_SERVER_HANDLES_ARE_FIXED is DISABLED 
 *   then an application can add attributes or services in the local
 *   database when not connected. it is managed by the service change 
 *   characteristic and indicated to the remote device upon reconnection, in
 *   order the remote device refresh it attribute caching.
 *   This is the default behavior.
 *
 * - If ATT_SERVER_HANDLES_ARE_FREEZABLE is ENABLED and 
 *   ATT_SERVER_HANDLES_ARE_FIXED is DISABLED 
 *   then an application can add attributes or services in the local
 *   database when not connected and until it have not call the 
 *   ATT_SERVER_FreezeDatabase() API. It is managed by the "service changed" 
 *   characteristic and indicated to the remote device upon reconnection. 
 *   When freezed, the service change characteristic is removed from the 
 *   database to indicate to the remote device that the attribute will no 
 *   more be modified.
 *
 * - If ATT_SERVER_HANDLES_ARE_FIXED is ENABLED then an application can add 
 *   attributes or services in the local database when not connected and 
 *   previous to the first connection ( initiate the first connection or
 *   beeing discoverable ) then the application must care that no more 
 *   services or attributes will be added to the database.
 *   In this configuration the Handles are considered fixed during all the 
 *   product lifetime, and it is up to the application to take care of that.
 *   The "service change" characteristic is not present and not managed by the
 *   stack, it saves a several hundred of bytes of code and several bytes of
 *   data.
 *   When ATT_SERVER_HANDLES_ARE_FIXED is ENBALED then 
 *   ATT_SERVER_HANDLES_ARE_FREEZABLE is ignored.
 *
 */


/** ATT_SERVER_HANDLES_ARE_FREEZABLE 
 *   If ATT_SERVER_HANDLES_ARE_FREEZABLE is ENABLED and 
 *   ATT_SERVER_HANDLES_ARE_FIXED is DISABLED then an application can add
 *   attributes or services in the local database when not connected and until
 *   it have not call the ATT_SERVER_FreezeDatabase() API. 
 *   It is managed by the "service changed" characteristic and indicated to the
 *   remote device upon reconnection. 
 *   When freeze, the "service changed" characteristic is removed from the 
 *   database to indicate to the remote device that the attribute will no 
 *   more be modified.
 *
 *   The default value is DISABLED (0)
 */
#ifndef ATT_SERVER_HANDLES_ARE_FREEZABLE
#define ATT_SERVER_HANDLES_ARE_FREEZABLE                0
#endif //ATT_SERVER_HANDLES_ARE_FREEZABLE


/** ATT_SERVER_HANDLES_ARE_FIXED
 *   If ENABLED then an application can add attribute
 *   or service in the local database when not connected and previous to the 
 *   first connection ( initiate the first connection or being discoverable ) 
 *   then the application must care that no more service or attribute will be 
 *   added to the database.
 *   In this configuration the Handles are considered fixed during all the 
 *   product lifetime, and it is up to the application to take care of that.
 *   The "service change" characteristic is not present and not managed by the
 *   stack, it saves a several hundred of bytes of code and several bytes of
 *   data.
 *   When ATT_SERVER_HANDLES_ARE_FIXED is ENBALED then 
 *   ATT_SERVER_HANDLES_ARE_FREEZABLE is ignored.
 *
 *   The default value is DISABLED (0)
 */
#ifndef ATT_SERVER_HANDLES_ARE_FIXED
#define ATT_SERVER_HANDLES_ARE_FIXED                    0
#endif //ATT_SERVER_HANDLES_ARE_FIXED

#if (ATT_SERVER_HANDLES_ARE_FIXED == 1)
#undef ATT_SERVER_HANDLES_ARE_FREEZABLE
#define ATT_SERVER_HANDLES_ARE_FREEZABLE                0
#endif // (ATT_SERVER_HANDLES_ARE_FIXED == 1)

/**
 * @brief Optional commands' support.
 *
 * When ENABLED (set to 1), the SERVER is able to handle these requests
 *
 * The specification defines only two requests to be mandatory
 * supported, all other, requests, commands, notifications and 
 * indications are optionals
 * SPEC:
 * A server shall be able to receive and properly respond to the following 
 * requests: 
 * - Find Information Request
 * - Read Request
 * Support for all other PDU types in a server can be specified in a higher 
 * layer specification, see Section 3.4.8
 *
 * Otherwise, when DISBALED and the ATT server responds with an
 * Error Response 'invalid request' when those requests are received.
 */

/* ATT_SERVER_SUPPORT_PREPARE_EXECUTE_WRITE_REQ
 * Defines if the local ATTRIBUTE SERVER is able to understand the
 * - PREPARE WRITE REQUEST
 * - EXECUTE WRITE REQUEST
 *
 * Note that PREPARE WRITE REQUEST is used when writing long attributes
 * The default behavior is to not supporting this request
 */
#ifndef ATT_SERVER_SUPPORT_PREPARE_EXECUTE_WRITE_REQ
#define ATT_SERVER_SUPPORT_PREPARE_EXECUTE_WRITE_REQ    0
#endif //ATT_SERVER_SUPPORT_PREPARE_EXECUTE_WRITE_REQ

#if (ATT_SERVER_SUPPORT_PREPARE_EXECUTE_WRITE_REQ == 1)

/* ATTSERVER_NUM_PREPARE_WRITE_VALUES_SUPPORTED
 * The depth of the PREPARE WRITE queue when PREPARE WRITE REQUEST
 * is supported
 */
#ifndef ATTSERVER_NUM_PREPARE_WRITE_VALUES_SUPPORTED
#define ATTSERVER_NUM_PREPARE_WRITE_VALUES_SUPPORTED    3
#endif //ATTSERVER_NUM_PREPARE_WRITE_VALUES_SUPPORTED

#if (ATTSERVER_NUM_PREPARE_WRITE_VALUES_SUPPORTED == 0)
#error This makes no sense to support prepare execute write in the \
    server if not even one value may be prepared! Shall be > 0.
/* Note: if you want to allow this, check also against 0 prior to check
 * against ATTSERVER_NUM_PREPARE_WRITE_VALUES_SUPPORTED - 1 in
 * servercore.c. 
 */
#endif //(ATTSERVER_NUM_PREPARE_WRITE_VALUES_SUPPORTED == 0)

#if (ATTSERVER_NUM_PREPARE_WRITE_VALUES_SUPPORTED >= 255)
#error U8 not enough to browse the prepare write queues!
/* Do not even allow the value 255 as we want ot be able to perform i++
 * on an U8 and compare it against
 * ATTSERVER_NUM_PREPARE_WRITE_VALUES_SUPPORTED 
 */
#endif //(ATTSERVER_NUM_PREPARE_WRITE_VALUES_SUPPORTED >= 255)

#endif //(ATT_SERVER_SUPPORT_PREPARE_EXECUTE_WRITE_REQ == 1)
   
/* ATT_SERVER_SUPPORT_READ_BLOB_REQ
 * Defines if the local ATTRIBUTE SERVER is able to understand the
 * - READ BLOB REQUEST
 *
 * The default behavior is to not supporting this request
 */
#ifndef ATT_SERVER_SUPPORT_READ_BLOB_REQ
#define ATT_SERVER_SUPPORT_READ_BLOB_REQ                0
#endif //ATT_SERVER_SUPPORT_READ_BLOB_REQ
  

//TODO:

/* ATT_SERVER_SUPPORT_READ_MULTIPLE_REQ
 * Defines if the local ATTRIBUTE SERVER is able to understand the
 * - READ MULTIPLE REQUEST
 *
 * The default behavior is to not supporting this request
 */
#ifndef ATT_SERVER_SUPPORT_READ_MULTIPLE_REQ
#define ATT_SERVER_SUPPORT_READ_MULTIPLE_REQ            0
#endif //ATT_SERVER_SUPPORT_READ_MULTIPLE_REQ

/* ATT_SERVER_SUPPORT_WRITE_REQUEST
 * Defines if the local ATTRIBUTE SERVER is able to understand the
 * - WRITE REQUEST
 *
 * The default behavior is to support this request,
 * Disabling this configuration save ROM memory and can be done
 * if all the attribute in the database are read only
 */
#ifndef ATT_SERVER_SUPPORT_WRITE_REQUEST
#define ATT_SERVER_SUPPORT_WRITE_REQUEST                1
#endif //ATT_SERVER_SUPPORT_WRITE_REQUEST

/* ATT_SERVER_SUPPORT_WRITE_COMMAND
 * Defines if the local ATTRIBUTE SERVER is able to understand the
 * - WRITE COMMAND
 *
 * The default behavior is to support this request,
 * Disabling this configuration save ROM memory and can be done
 * if all the attribute in the database are read only
 */
#ifndef ATT_SERVER_SUPPORT_WRITE_COMMAND
#define ATT_SERVER_SUPPORT_WRITE_COMMAND                1
#endif //ATT_SERVER_SUPPORT_WRITE_COMMAND


/* ATT_SERVER_SUPPORT_NOTIFICATION
 * Defines if the local ATTRIBUTE SERVER is able to send the
 * - HANDLE VALUE NOTIFICATION
 *
 * The default behavior is to support this request,
 * Disabling this configuration save ROM memory
 */
#ifndef ATT_SERVER_SUPPORT_NOTIFICATION
#define ATT_SERVER_SUPPORT_NOTIFICATION                 1
#endif //ATT_SERVER_SUPPORT_NOTIFICATION

/* ATT_SERVER_SUPPORT_INDICATION
 * Defines if the local ATTRIBUTE SERVER is able to send the
 * - HANDLE VALUE INDICATION
 *
 * The default behavior is to support this request,
 * Note that this feature must be supported if ATT_SERVER_HANDLES_ARE_FIXED is 
 * DISABLED because the "service changed" characteristic may be indicated 
 * by the stack
 * Disabling this configuration save ROM memory
 */
#ifndef ATT_SERVER_SUPPORT_INDICATION
#define ATT_SERVER_SUPPORT_INDICATION                   1
#endif //ATT_SERVER_SUPPORT_INDICATION

#if ( (ATT_SERVER_HANDLES_ARE_FIXED == 0) && (ATT_SERVER_SUPPORT_INDICATION == 0) )
#error ATT_SERVER_SUPPORT_INDICATION must be set to ENABLED when ATT_SERVER_HANDLES_ARE_FIXED is DISBALED
#endif //( (ATT_SERVER_HANDLES_ARE_FIXED == 0) && (ATT_SERVER_SUPPORT_INDICATION == 0) )

///* ATT_SERVER_SUPPORT_SIGNED_COMMANDS
// * Defines if the local ATTRIBUTE SERVER is able to understand the
// * - SIGNED WRITE COMMANDS
// *
// * The default behavior is to not supporting this request
// */
//#ifndef ATT_SERVER_SUPPORT_SIGNED_COMMANDS
//#define ATT_SERVER_SUPPORT_SIGNED_COMMANDS              0
//#endif //ATT_SERVER_SUPPORT_SIGNED_COMMANDS

#endif //(ATT_ROLE_SERVER==1)

#endif //(ATTRIBUTE_PROTOCOL == 1) 

#if (ATTRIBUTE_PROTOCOL == 0)
// if attribute protocol is not enabled disable the Attribute client and
// the server
// it avoid in the other code to test first if attribute is enabled and 
// then if either client or server are enabled
#define  ATT_ROLE_CLIENT	0
#define  ATT_ROLE_SERVER	0
#endif //(ATTRIBUTE_PROTOCOL == 0)

// Clear the ATT_BROWSE_API configuration if ATTRIBUTE_PROTOCOL is not defined
// Or if the SERVER role is no supported
#if ( (ATTRIBUTE_PROTOCOL == 0) || (ATT_ROLE_SERVER == 0))
#undef ATT_BROWSE_API
#endif	//( (ATTRIBUTE_PROTOCOL == 0) || (ATT_ROLE_SERVER == 0))

/**
 * @brief Additional API used in sample application to display the database.
 */
#ifndef ATT_BROWSE_API
  #define ATT_BROWSE_API 0
#endif //ATT_BROWSE_API

#if (ATT_BROWSE_API == 0)
#define ATT_SERVER_AttAttributesChanged (void)0
#endif //(ATT_BROWSE_API == 0)

#endif //__ATTCONFIG_H__
