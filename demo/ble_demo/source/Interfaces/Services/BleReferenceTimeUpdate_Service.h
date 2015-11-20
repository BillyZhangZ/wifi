#ifndef __BLEREFERENCETIMEUPDATE_SERVICE_H
#define __BLEREFERENCETIMEUPDATE_SERVICE_H
/****************************************************************************
 *
 * File:          BleReferenceTimeUpdate_Service.h
 *
 * Description:   Contains routines declaration for REFERENCE TIME UPDATE 
 *                Service.
 * 
 * Created:       december, 2011
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.2 $
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

#include "BleTypes.h"
#include "BleTimeTypes.h"

/***************************************************************************\
 *	Type definition
\***************************************************************************/

/***************************************************************************\
 * OPTIONAL FEATURES
\***************************************************************************/
/**
 * BLE_SUPPORT_REFERENCETIMEUPDATE_SERVICE
*	Defines if the BLE local device enables access to the local  
 *	REFERENCE TIME UPDATE service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles to get access to the  
 * REFERENCE TIME UPDATE Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_REFERENCETIMEUPDATE_SERVICE
#define BLE_SUPPORT_REFERENCETIMEUPDATE_SERVICE								0
#endif //BLE_SUPPORT_REFERENCETIMEUPDATE_SERVICE

/***************************************************************************\
 * CONSTANTS
\***************************************************************************/


/***************************************************************************\
 *	Type definition
\***************************************************************************/
/** 
 * BleReferenceTimeUpdateServiceEvent type
 * Define the different kind of events that could be received by the 
 * BleReferenceTimeUpdateServiceCallBack
 */
typedef U8 BleReferenceTimeUpdateServiceEvent;

/** BLEREFERENCETIMEUPDATESERVICE_EVENT_GETREFERENCETIMEUPDATE
 * Event received when a new reference time update is requested from
 *  the SERVER (the command has been issued by a remote user).
 * Upon reception of this event the SERVER MUST run a REFERENCE TIME UPDATE
 * Once complete, the SERVER MUST call BLEREFERENCETIMEUPDATE_TimeUpdateResult
 * with the associated result.
 * if the SERVER does not want to run a REFERENCE TIME UPDATE processn it can
 * call BLEREFERENCETIMEUPDATE_TimeUpdateResult with the 
 * BLEREFERENCETIMEUPDATESERVICE_RESULT_CANCELED result directly in the
 * callback
 * The status field is always BLESTATUS_SUCCESS
 * The parms field is not applicable.
 */
#define BLEREFERENCETIMEUPDATESERVICE_EVENT_STARTREFERENCEUPDATEREQUEST		0xBC

/** BLEREFERENCETIMEUPDATESERVICE_EVENT_CANCELREFERENCETIMEUPDATE
 * Event received when a new reference time update is requested from
 *  the SERVER (the command has been issued by a remote user).
 * The REFERENCE TIME UPDATE SERVER does not expect anymore to receive a 
 * BLEREFERENCETIMEUPDATE_TimeUpdateResult.
 * The status field is always BLESTATUS_SUCCESS
 * The parms field is not applicable.
 */
#define BLEREFERENCETIMEUPDATESERVICE_EVENT_CANCELREFERENCETIMEUPDATE		0xBD



/**
 * BleReferenceTimeUpdateServiceCallBack
 *	This callback receives the REFERENCE TIME UPDATE Service events. 
 *  Each events may be assiociated with specified status and parameters.
 *  The callback is executed during the stack context,  be careful to
 *	 not doing heavy process in this function.
 */
typedef void (*BleReferenceTimeUpdateServiceCallBack)(
					BleReferenceTimeUpdateServiceEvent event,
					BleStatus status,
					void *parms); 


/**
 * BleReferenceTimeUpdateServiceUpdateResult
 *	The available result of a REFERENCE TIME UPDATE procedure.
 */
typedef U8 BleReferenceTimeUpdateServiceUpdateResult;
#define BLEREFERENCETIMEUPDATESERVICE_RESULT_SUCCESSFUL				0
#define BLEREFERENCETIMEUPDATESERVICE_RESULT_CANCELED				1
#define BLEREFERENCETIMEUPDATESERVICE_RESULT_NOREFERENCECONNECTION	2
#define BLEREFERENCETIMEUPDATESERVICE_RESULT_REFERENCEERROR			3
#define BLEREFERENCETIMEUPDATESERVICE_RESULT_TIMEOUT				4
#define BLEREFERENCETIMEUPDATESERVICE_RESULT_NOUPDATEAFTERRESET		5


/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
#if (BLE_SUPPORT_REFERENCETIMEUPDATE_SERVICE == 1)
/** The REFERENCE TIME UPDATE is completed with the given result
 *
 * BLEREFERENCETIMEUPDATE_TimeUpdateResult()
 *	This function is used by the application to notify the SERVER that the 
 *   REFERENCE TIME UPDATE is completed with the given result.
 *   Note that if the REFERENCE TIME has been successfully updated, then it 
 *   is up to the application to update (or not) the CURRENT TIME SERVICE 
 *   Current Time value.
 *
 * BLE_SUPPORT_REFERENCETIMEUPDATE_SERVICE shall be enabled.
 *
 * @param result : the result of the REFERENCE TIME UPDATE
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed. It may be du
 *   that the REFERENCE TIME UPDATE has no UPDATE request pending.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function has failed because
 *		an invalid parameter.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEREFERENCETIMEUPDATE_TimeUpdateResult(
			BleReferenceTimeUpdateServiceUpdateResult result );
#endif //(BLE_SUPPORT_REFERENCETIMEUPDATE_SERVICE == 1)

#endif //__BLEREFERENCETIMEUPDATE_SERVICE_H
