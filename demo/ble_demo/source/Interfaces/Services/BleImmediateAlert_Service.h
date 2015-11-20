#ifndef __BLEIMMEDIATEALERT_SERVICE_H
#define __BLEIMMEDIATEALERT_SERVICE_H
/****************************************************************************
 *
 * File:          BleImmediateAlert_Service.h
 *
 * Description:   Contains routines declaration for Immediate Alert Service.
 * 
 * Created:       march, 2011
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.5 $
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

#include "bleTypes.h"

/***************************************************************************\
 *	Macro and constant definition
\***************************************************************************/
/***************************************************************************\
 * OPTIONAL FEATURES
\***************************************************************************/
/**
 * BLE_SUPPORT_IMMEDIATEALERT_SERVICE
*	Defines if the BLE local device enables access to the local immediate  
 *	Alert service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles to get access to the immediate 
 * Alert Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_IMMEDIATEALERT_SERVICE
#define BLE_SUPPORT_IMMEDIATEALERT_SERVICE								0
#endif //BLE_SUPPORT_IMMEDIATEALERT_SERVICE


/***************************************************************************\
 * CONSTANTS
\***************************************************************************/

/***************************************************************************\
 *	Type definition
\***************************************************************************/
/** 
 * BleImmediateAlertEvent type
 * Define the different kind of events that could be received by the 
 * BleImmediateAlertCallBack
 */
typedef U8 BleImmediateAlertEvent;

/** BLEIMMEDIATEALERT_EVENT_ALERT
 * Event received when a new alerting state is raised.
 * for example when the remote device will trig an immediate Alert,
 * the BLEIMMEDIATEALERT_EVENT_ALERT event with the configurated
 * alert level will be trigged.
 * The parms field indicates the Alert information within the 
 * BleImmediateAlertAlert type.
 */
#define BLEIMMEDIATEALERT_EVENT_ALERT									1

/** 
 * BleImmediateAlertAlertLevel type
 * Define the different kind of Alert level that could be received by the 
 * BleImmediateAlertCallBack
 * The specific action that occurs in the device for the mild and high
 * alerts is implementation specific. For example, this could include 
 * flashing lights, making noises, moving, or other methods to alert the
 * user.
 */
typedef U8 BleImmediateAlertAlertLevel;

/** BLEIMMEDIATEALERT_ALERT_NOALERT
 * The alert level is no alert; no alerting shall be done on this device.
 */
#define BLEIMMEDIATEALERT_ALERT_NOALERT									0

/** BLEIMMEDIATEALERT_ALERT_MEDIUMALERT
 * The Alert level is medium Alert, the device shall alert.
 */
#define BLEIMMEDIATEALERT_ALERT_MEDIUMALERT								1

/** BLEIMMEDIATEALERT_ALERT_HIGHALERT
 * The alert level is high Alert, the device shall alert in the strongest
 * possible way.
 */
#define BLEIMMEDIATEALERT_ALERT_HIGHALERT								2

/** 
 * BleImmediateAlertAlert type
 * Define the Alert that is received by the 
 * BleImmediateAlertCallBack during BLEIMMEDIATEALERT_EVENT_ALERT event
 */
typedef struct{
	// The connection from which the alert is coming
	U16 connHandle;
	
	// The alert Level
	BleImmediateAlertAlertLevel alertlevel;

} BleImmediateAlertAlert;

/**
 * BleImmediateAlertCallBack
 *	This callback receives the IMMEDIATE ALERT SERVICE events. 
 *  Each events may be assiociated with specified parameters.
 *  The callback is executed during the stack context,  be careful to
 *	 not doing heavy process in this function.
 */
typedef void (*BleImmediateAlertCallBack)(BleImmediateAlertEvent event,
								    BleStatus status,
									void *parms); 

/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
#if (BLE_SUPPORT_IMMEDIATEALERT_SERVICE == 1)
/** Localy Set the Immediate Alert Service Alert level.
 *
 * BLEIMMEDIATEALERT_SetAlertLevel()
 *	This function is used to localy Set the Immediate Alert Service Alert 
 * level.
 *  Note that upon success, the alertLevel can be considered to be set, No
 *  callback event will be reported in the registered
 *  BleImmediateAlertCallBack.
 *
 * BLE_SUPPORT_IMMEDIATEALERT_SERVICE shall be enabled.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, merely because
 * Immediate Alert Service is not registered by any profile.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEIMMEDIATEALERT_SetAlertLevel( 
							BleImmediateAlertAlertLevel level );
#endif //(BLE_SUPPORT_IMMEDIATEALERT_SERVICE == 1)

#endif //__BLEIMMEDIATEALERT_SERVICE_H
