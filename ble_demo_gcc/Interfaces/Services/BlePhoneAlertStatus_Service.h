#ifndef __BLEPHONEALERTSTATUS_SERVICE_H
#define __BLEPHONEALERTSTATUS_SERVICE_H
/****************************************************************************
 *
 * File:          BlePhoneAlertStatus_Service.h
 *
 * Description:   Contains routines declaration for PhoneAlertStatus Service.
 * 
 * Created:       January, 2012
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
/** 
 * BlePhoneAlertStatusServiceAlertStatus type
 * Define the different kind of alert status that could be set in the
 * BLEPHONEALERTSTATUSSERVICE_SetAlertStatus parameter.
 * It is a bitfield indicating which alert status is active (ringer, vibrator 
 * or display)
 * if the bit is set then the alert is considered to be active.
 */
typedef U8 BlePhoneAlertStatusServiceAlertStatus;
/** BLEPHONEALERTSTATUSSERVICE_ALERT_NONE
 * There is no active alert
 */
#define BLEPHONEALERTSTATUSSERVICE_ALERT_NONE					0x00

/** BLEPHONEALERTSTATUSSERVICE_ALERT_RINGER
 * If set, means that the Ringer State is active
 */
#define BLEPHONEALERTSTATUSSERVICE_ALERT_RINGER					0x01

/** BLEPHONEALERTSTATUSSERVICE_ALERT_VIBRATOR
 * If set, means that the Vibrate State is active
 */
#define BLEPHONEALERTSTATUSSERVICE_ALERT_VIBRATOR				0x02

/** BLEPHONEALERTSTATUSSERVICE_ALERT_VIBRATOR
 * If set, means that the Display Alert Status State is active
 */
#define BLEPHONEALERTSTATUSSERVICE_ALERT_DISPLAY				0x04

/** 
 * BlePhoneAlertStatusServiceRingerSetting type
 * Define the different kind of ringer setting that could be set in the
 * BLEPHONEALERTSTATUS_SetRingerSetting parameter.
 * It defines the Setting of the Ringer. 
 * It is not a bitfield, the value can be either 
 * BLEPHONEALERTSTATUSSERVICE_RINGER_SILENT or 
 * BLEPHONEALERTSTATUSSERVICE_RINGER_NORMAL
 */
typedef U8 BlePhoneAlertStatusServiceRingerSetting;

/** BLEPHONEALERTSTATUSSERVICE_RINGER_SILENT
 * The ringer in in silent mode
 */
#define BLEPHONEALERTSTATUSSERVICE_RINGER_SILENT				0x00

/** BLEPHONEALERTSTATUSSERVICE_RINGER_NORMAL
 * The ringer in in normal mode
 */
#define BLEPHONEALERTSTATUSSERVICE_RINGER_NORMAL				0x01

/** 
 * BlePhoneAlertStatusServiceRingerRequestedState type
 * Define the different kind of ringer requested state that a remote CLIENT
 * can request to be set by the local SERVER
 * It can be one of the following values:
 */
typedef U8 BlePhoneAlertStatusServiceRingerRequestedState;

/** BLEPHONEALERTSTATUSSERVICE_RINGERREQUESTEDSTATE_SILENT
 * A remote CLIENT request the SERVER to set the ringer in SILENT mode
 */
#define BLEPHONEALERTSTATUSSERVICE_RINGERREQUESTEDSTATE_SILENT			1

/** BLEPHONEALERTSTATUSSERVICE_RINGERREQUESTEDSTATE_MUTEONCE
 * A remote CLIENT request the SERVER to mute the current ring Once
 */
#define BLEPHONEALERTSTATUSSERVICE_RINGERREQUESTEDSTATE_MUTEONCE		2

/** BLEPHONEALERTSTATUSSERVICE_RINGER_NORMAL
 * A remote CLIENT request the SERVER to set the ringer in NORMAL mode
 */
#define BLEPHONEALERTSTATUSSERVICE_RINGERREQUESTEDSTATE_NORMAL			3

/***************************************************************************\
 * OPTIONAL FEATURES
\***************************************************************************/
/**
 * BLE_SUPPORT_PHONEALERTSTATUS_SERVICE
*	Defines if the BLE local device enables access to the local  
 *	PHONE ALERT STATUS service and characteristics.
 * 
 * If enabled ( set to 1 ) it enables Profiles to get access to the  
 * PhoneAlertStatus Service and characteristics
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_PHONEALERTSTATUS_SERVICE
#define BLE_SUPPORT_PHONEALERTSTATUS_SERVICE								0
#endif //BLE_SUPPORT_PHONEALERTSTATUS_SERVICE

#if (BLE_SUPPORT_PHONEALERTSTATUS_SERVICE == 1)

/**
 * BLEPHONEALERTSTATUSSERVICE_SUPPORT_RINGERCONTROL
 *	Defines if the BLE local device enables access to the local  
 *	PHONE ALERT STATUS service's Ringer control optional characteristic.
 *  It allows the CLIENT to set the ringer state of the SERVER
 * 
 * If enabled ( set to 1 ) it enables reception of the following 
 *  BlePhoneAlertStatusServiceEvent in the registered callback:
 *   BLEPHONEALERTSTATUSSERVICE_EVENT_SETRINGERSTATE
 *
 * If the SERVER device supports the ringer setting to be controlled by the
 * remote CLIENT device then it shall set the according ringer beahvior 
 * corresponding to the event received from the CLIENT.
 *
 * Note that the Service does not automaticaly manage the state machine
 * and set the ringer setting value (BLEPHONEALERTSTATUS_SetRingerSetting)
 * it is up to the application to manage it.
 * The default value for this option is disabled (0).
 */
#ifndef BLEPHONEALERTSTATUSSERVICE_SUPPORT_RINGERCONTROL
#define BLEPHONEALERTSTATUSSERVICE_SUPPORT_RINGERCONTROL					0
#endif //BLEPHONEALERTSTATUSSERVICE_SUPPORT_RINGERCONTROL



/***************************************************************************\
 * CONSTANTS
\***************************************************************************/
/** BLEINFOTYPE_ALERTSTATUS_CLIENTCONFIG 
 ** BLEINFOTYPE_RINGERSETTING_CLIENTCONFIG
 * Two Unique descriptors ID for two two-Bytes length information to save
 * in persistent memory representing the client config for a given remote
 * bonded device.
 * It is the Unique identifier passed in the infotype fields when the PROFILE
 * call the system APIs SYSTEM_SetPersistentInformation(BD_ADDR addr,
 * U8 infoType, U8* infoValue,U8 InfoLen) and 
 * SYSTEM_GetPersistentInformation(BD_ADDR addr, U8 infoType, U8 **infoValue,
 * U8 *InfoLen). 
 */
#define BLEINFOTYPE_ALERTSTATUS_CLIENTCONFIG						0x77	
#define BLEINFOTYPE_RINGERSETTING_CLIENTCONFIG						0x78	


/***************************************************************************\
 *	Type definition
\***************************************************************************/
/** 
 * BlePhoneAlertStatusServiceEvent type
 * Define the different kind of events that could be received by the 
 * BlePhoneAlertStatusServiceCallBack
 */
typedef U8 BlePhoneAlertStatusServiceEvent;

/** BLEPHONEALERTSTATUSSERVICE_EVENT_SETRINGERSTATE
 * Event received when a remote CLIENT requests the SERVER to set the RINGER
 * state to the given state.
 * The status field is always BLESTATUS_SUCCESS.
 * The parms field is a BlePhoneAlertStatusServiceRingerRequest structure,
 * it indicates for which connection handle representing the 
 * CLIENT the requests come, and the requested state.
 * The requested state can be either:
 *
 * --SILENT--
 * The client requests ringer to go in SILENT mode.
 * - If the current ringer mode is in NORMAL mode, the SERVER application must
 * set the ringer mode in SILENT mode, upon completion the SERVER application
 * must call BLEPHONEALERTSTATUS_SetRingerSetting with ringer setting set to 
 * ringer SILENT.
 *  Eventually if the SERVER device is ringing and no more ring when
 * go to SILENT mode, then the SERVER application must call
 * BLEPHONEALERTSTATUSSERVICE_SetAlertStatus with ringer status not set.
 *
 * - If the ringer is already in SILENT mode, the SERVER application must
 * ignore this event.
 *
 * -- NORMAL--
 * A remote CLIENT requests the SERVER to set the 
 * ringer in NORMAL mode.
 * - If the current ringer mode is in SILENT mode, the SERVER application must
 * set the ringer mode in MODE mode and act according to phone settings.
 * upon completion the SERVER application
 * must call BLEPHONEALERTSTATUS_SetRingerSetting with ringer setting set to 
 * ringer NORMAL.
 *  Eventually if the SERVER device is ringing in SILENT mode and ring when
 * go to NORMAL mode, then the SERVER application must call
 * BLEPHONEALERTSTATUSSERVICE_SetAlertStatus with ringer status set.
 *
 * - If the ringer is already in NORMAL mode, the SERVER application must
 * ignore this event.
 *
 * --MUTE ONCE--
 * A remote CLIENT requests the SERVER to MUTE the 
 * current ringer once.
 * If the SERVER device is currently ringing then the SERVER application
 * shall mute the ringer still the end of the ringing state.
 * If the ringer is not active, the SERVER application must ignore this event.
 *
 * This event is only received if BLEPHONEALERTSTATUSSERVICE_SUPPORT_RINGERCONTROL
 * is enabled (set to 1)
 */
#define BLEPHONEALERTSTATUSSERVICE_EVENT_SETRINGERSTATE				0xC6

/**
 * BlePhoneAlertStatusServiceCallBack
 *	This callback receives the PHONE ALERT STATUS Service events. 
 *  Each events may be assiociated with specified status and parameters.
 *  The callback is executed during the stack context,  be careful to
 *	 not doing heavy process in this function.
 */
typedef void (*BlePhoneAlertStatusServiceCallBack)(
					BlePhoneAlertStatusServiceEvent event,
					BleStatus status,
					void *parms); 


/** 
 * BlePhoneAlertStatusServiceRingerRequest type
 * Type received as parameter in the BlePhoneAlertStatusServiceCallBack
 * during the BLEPHONEALERTSTATUSSERVICE_EVENT_SETRINGERSTATE
 * It indicates:
 * connHandle : the connection handle identifying the CLIENT from which the
 *     request is coming
 * requestedState : the remote CLIENT requested state.
 */
typedef struct {
	U16                                             connHandle;
	BlePhoneAlertStatusServiceRingerRequestedState  requestedState;
}BlePhoneAlertStatusServiceRingerRequest;

/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/

/** Set the current Alert Status
 *
 * BLEPHONEALERTSTATUSSERVICE_SetAlertStatus()
 *	This function is used to set the current Alert Status value of this SERVER
 *  device (may be RINGING and/or VIBRATING and/or DISPLAYING or NONE).
 *
 * BLE_SUPPORT_PHONEALERTSTATUS_SERVICE shall be enabled.
 *
 * @param alertStatus : the current Alert status of the SERVER device.
 *		
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function has failed because
 *		an invalid parameter.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEPHONEALERTSTATUSSERVICE_SetAlertStatus(
	BlePhoneAlertStatusServiceAlertStatus alertStatus);

/** Get the current Alert Status
 *
 * BLEPHONEALERTSTATUSSERVICE_GetAlertStatus()
 *	This function is used to get the current Alert Status value of this SERVER
 *  device. This value has been previously written by the application using
 *	BLEPHONEALERTSTATUSSERVICE_SetAlertStatus().
 *
 * BLE_SUPPORT_PHONEALERTSTATUS_SERVICE shall be enabled.
 *
 * @param alertStatus : A valid pointer to a 
 *     BlePhoneAlertStatusServiceAlertStatus that will receive, if the 
 *     function return  the BLESTATUS_SUCCESS, the current Alert status of the
 *     SERVER device as	previously written by the application.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function has failed because
 *		an invalid parameter.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEPHONEALERTSTATUSSERVICE_GetAlertStatus(
	BlePhoneAlertStatusServiceAlertStatus *alertStatus);

/** Set the current ringer Setting
 *
 * BLEPHONEALERTSTATUSSERVICE_SetRingerSetting()
 *	This function is used to set the ringer setting value (may be NORMAL or 
 *  SILENT). Typically called
 *
 * BLE_SUPPORT_PHONEALERTSTATUS_SERVICE shall be enabled.
 *
 * @param ringerSetting : the current ringer setting, could be either
 *         BLEPHONEALERTSTATUSSERVICE_RINGER_SILENT or 
 *         BLEPHONEALERTSTATUSSERVICE_RINGER_NORMAL
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function has failed because
 *		an invalid parameter.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEPHONEALERTSTATUSSERVICE_SetRingerSetting(
	BlePhoneAlertStatusServiceRingerSetting ringerSetting);

/** Get the current Ringer Setting
 *
 * BLEPHONEALERTSTATUSSERVICE_GetRingerSetting()
 *	This function is used to get the current Ringer Setting value of this SERVER
 *  device. This value has been previously written by the application using
 *	BLEPHONEALERTSTATUSSERVICE_SetRingerSetting().
 *
 * BLE_SUPPORT_PHONEALERTSTATUS_SERVICE shall be enabled.
 *
 * @param ringerSetting : A valid pointer to a 
 *         BlePhoneAlertStatusServiceRingerSetting that will receive, if the
 *         function return BLESTATUS_SUCCESS, the current Ringer Setting of the
 *         SERVER device as previously written by the application.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function has failed because
 *		an invalid parameter.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEPHONEALERTSTATUSSERVICE_GetRingerSetting(
	BlePhoneAlertStatusServiceRingerSetting *ringerSetting);

#endif //(BLE_SUPPORT_PHONEALERTSTATUS_SERVICE == 1)
#endif //__BLEPHONEALERTSTATUS_SERVICE_H
