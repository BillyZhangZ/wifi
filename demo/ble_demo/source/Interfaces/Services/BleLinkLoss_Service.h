#ifndef __BLELINKLOSS_SERVICE_H
#define __BLELINKLOSS_SERVICE_H
/****************************************************************************
 *
 * File:          BleLinkLoss_Service.h
 *
 * Description:   Contains routines declaration for LinkLoss Service.
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

/**
 * BLE_SUPPORT_LINKLOSS_SERVICE
 * Defines if the BLE local device enables access to the Link Loss Service
 * 
 * If enabled ( set to 1 ) it enables Profiles to get access to the link Loss
 * Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_LINKLOSS_SERVICE
#define BLE_SUPPORT_LINKLOSS_SERVICE									0
#endif //BLE_SUPPORT_LINKLOSS_SERVICE


/***************************************************************************\
 * OPTIONAL FEATURES
\***************************************************************************/

/***************************************************************************\
 * CONSTANTS
\***************************************************************************/

/***************************************************************************\
 *	Type definition
\***************************************************************************/
/** 
 * BleLinkLossEvent type
 * Define the different kind of events that could be received by the 
 * BleLinkLossCallBack
 */
typedef U8 BleLinkLossEvent;

/** BLELINKLOSS_EVENT_ALERT
 * Event received when a new alerting state is raised.
 * for example when the link will be disconnected the Link loss service
 * will send the BLELINKLOSS_EVENT_ALERT event with the configurated
 * alert level.
 * The specific action that occurs in the device for the mild and high
 * alerts is implementation specific. For example, this could include 
 * flashing lights, making noises, moving, or other methods to alert the
 * user.
 */
#define BLELINKLOSS_EVENT_ALERT											1

/** 
 * BleLinkLossAlertLevel type
 * Define the different kind of Alert level that could be received by the 
 * BleLinkLossCallBack within the BleLinkLossAlert parmeter in the
 * BLELINKLOSS_EVENT_ALERT.
 */
typedef U8 BleLinkLossAlertLevel;

/** BLELINKLOSS_ALERT_NOALERT
 * The alert level is no alert; no alerting shall be done on this device.
 */
#define BLELINKLOSS_ALERT_NOALERT										0

/** BLELINKLOSS_ALERT_MEDIUMALERT
 * The Alert level is medium Alert, the device shall alert.
 */
#define BLELINKLOSS_ALERT_MEDIUMALERT									1

/** BLELINKLOSS_ALERT_HIGHALERT
 * The alert level is high Alert, the device shall alert in the strongest
 * possible way.
 */
#define BLELINKLOSS_ALERT_HIGHALERT										2

/** 
 * BleLinkLossAlert type
 * Define the Alert that is received by the 
 * BleLinkLossCallBack during BLELINKLOSS_EVENT_ALERT event.
 */
typedef struct{
	// The connection from which the alert is coming
	U16 connHandle;

	// The remote device from which the alert is coming
	BD_ADDR addr;
	
	// The alert Level
	BleLinkLossAlertLevel alertlevel;

} BleLinkLossAlert;

/**
 * BleLinkLossCallBack
 *	This callback receives the LINK LOSS SERVICE events. 
 *   Each events may be assiociated with specified parameters. 
 *  The callback is executed during the stack context,  be careful to
 *	 not doing heavy process in this function.
 */
typedef void (*BleLinkLossCallBack)(BleLinkLossEvent event,
									BleStatus status,
									void *parms); 

/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
#if (BLE_SUPPORT_LINKLOSS_SERVICE == 1)
/** Localy Set the LINK LOSS Service Alert level.
 *
 * BLELINKLOSS_SetAlertLevel()
 *	This function is used to localy Set the Link Loss Service Alert 
 * level.
 *  Note that upon success, the alertLevel can be considered to be set, No
 *  callback event will be reported in the registered linkLossCallback.
 *
 * BLE_SUPPORT_LINKLOSS_SERVICE shall be enabled.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, merely because
 * Link Loss Service is not registered by any profile.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLELINKLOSS_SetAlertLevel( 
							BleLinkLossAlertLevel level );
#endif //(BLE_SUPPORT_LINKLOSS_SERVICE== 1)

#endif //__BLELINKLOSS_SERVICE_H
