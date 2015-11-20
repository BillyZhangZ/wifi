#ifndef __BLEALERTNOTIFICATION_SERVICE_H
#define __BLEALERTNOTIFICATION_SERVICE_H
/****************************************************************************
 *
 * File:          BleAlertNotification_Service.h
 *
 * Description:   Contains routines declaration for Alert Notification
 *					Service.
 * 
 * Created:       september, 2011
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
 * BLE_SUPPORT_ALERTNOTIFICATION_SERVICE
 *	Defines if the BLE local device enables access to the local ALERT 
 * NOTIFICATION service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles to get access to the 
 * ALERT NOTIFICATION Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_ALERTNOTIFICATION_SERVICE
#define BLE_SUPPORT_ALERTNOTIFICATION_SERVICE							0
#endif //BLE_SUPPORT_ALERTNOTIFICATION_SERVICE

//TODO:BLE_ALERTNOTIFICATIONSERVICE_SUPPORT_TEXTSTRINGINFO
/**
 * BLEALERTNOTIFICATION_SERVER_SUPPORT_TEXTSTRINGINFO
 *	Defines if the BLE local device enables access to the local text string  
 *	information of the characteristic New Alert 
 * 
 * If enabled ( set to 1 ) it enables Profiles to get access to the text  
 * string information
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLEALERTNOTIFICATION_SERVER_SUPPORT_TEXTSTRINGINFO
#define BLEALERTNOTIFICATION_SERVER_SUPPORT_TEXTSTRINGINFO					0
#endif //BLEALERTNOTIFICATION_SERVER_SUPPORT_TEXTSTRINGINFO

/***************************************************************************\
 * CONSTANTS
\***************************************************************************/
/** BLEINFOTYPE_NEWALERT_CLIENTCONFIG
 ** BLEINFOTYPE_UNREADALERTSTATUS_CLIENTCONFIG
 ** BLEINFOTYPE_CONTROLPOINT_NEWALERTBITFIELD
 ** BLEINFOTYPE_CONTROLPOINT_UNREADALERTSTATUSBITFIELD
 * Unique descriptors ID for information to save
 * in persistent memory representing the client config for a given remote
 * bonded device and configured new and unread alert.
 * It is the Unique identifier passed in the infotype fields when the PROFILE
 * call the system APIs SYSTEM_SetPersistentInformation(BD_ADDR addr,
 * U8 infoType, U8* infoValue,U8 InfoLen) and 
 * SYSTEM_GetPersistentInformation(BD_ADDR addr, U8 infoType, U8 **infoValue,
 * U8 *InfoLen). 
 */
#define BLEINFOTYPE_NEWALERT_CLIENTCONFIG						0x9A	
#define BLEINFOTYPE_UNREADALERTSTATUS_CLIENTCONFIG				0x9B	


/***************************************************************************\
 *	Type definition
\***************************************************************************/

/** 
 * BleAlertNotificationServiceEvent type
 * Define the different kind of events that could be received by the 
 * BleAlertNotificationServiceCallBack
 */
typedef U8 BleAlertNotificationServiceEvent;

/** BLEALERTNOTIFICATIONSERVICE_EVENT_COMMANDENABLERECEIVED
 * Event received when a remote CLIENT requests the SERVER to enable
 * category for alert notification
 * The status field is always BLESTATUS_SUCCESS
 * The parms field is a BleAlertNotificationServiceCommandRequest structure,
 * it indicates for which connection handle representing the 
 * CLIENT the requests come, and the requested state.
 * The requested commandID can be either:
 *
 * --ENABLE NEW ALERT CATEGORY--
 * A remote CLIENT requests control point to enable New Alert 
 * category for new alert.
 *
 * --ENABLE UNREAD ALERT STATUS CATEGORY--
 * A remote CLIENT requests control point to enable Unread Alert category 
 * for unread alert.
 *
 */
#define BLEALERTNOTIFICATIONSERVICE_EVENT_COMMANDENABLERECEIVED		0xA8

/** BLEALERTNOTIFICATIONSERVICE_EVENT_COMMANDDISABLERECEIVED
 * Event received when a remote CLIENT requests the SERVER to disable
 * category for alert notification
 * The status field is always BLESTATUS_SUCCESS
 * The parms field is a BleAlertNotificationServiceCommandRequest structure,
 * it indicates for which connection handle representing the 
 * CLIENT the requests come, and the requested state.
 * The requested commandID can be either:
 *
 * --DISABLE NEW ALERT CATEGORY--
 * A remote CLIENT requests control point to disable New Alert 
 * category for new alert.
 *
 * --DISABLE UNREAD ALERT STATUS CATEGORY--
 * A remote CLIENT requests control point to disable Unread Alert category 
 * for unread alert.
 *
 */
#define BLEALERTNOTIFICATIONSERVICE_EVENT_COMMANDDISABLERECEIVED	0xA9

/** BLEALERTNOTIFICATIONSERVICE_EVENT_COMMANDNOTIFYRECEIVED
 * Event received when a remote CLIENT requests the SERVER to control
 * the alert notification
 * The status field is always BLESTATUS_SUCCESS
 * The parms field is a BleAlertNotificationServiceCommandRequest structure,
 * it indicates for which connection handle representing the 
 * CLIENT the requests come, and the requested state.
 * The requested commandID can be either:
 *
 * --NOTIFY NEW ALERT IMMEDIATELY--
 * A remote CLIENT requests control point to Notify the New Alert 
 * characteristic immediately for the category specified if that category is 
 * enabled
 *
 * --NOTIFY UNREAD ALERT STATUS IMMEDIATELY--
 * A remote CLIENT requests control point to Notify the New Alert 
 * characteristic immediately for the category specified if that category is
 * enabled
 *
 */
#define BLEALERTNOTIFICATIONSERVICE_EVENT_COMMANDNOTIFYRECEIVED		0xAA


/** 
 * BleAlertNotificationServiceAlertCategoryBitmask type
 * Define the different kind of Supported new alert category that could be 
 * received by the BleAlertNotificationServiceCallBack
 * The categories supported for new alert and unread alert status are 
 * simple alert, email, call, missed call, sms/mms, schedule, 
 * high prioritized alert and instant message
 * It is a bitfield indicating which categories are enabled or supported 
 * Set to 0 by default (the remote device will be able to enable the categories
 * thanks to the control point).
 * If the bit is set then the category is considered to enabled or supported.
 */
typedef	U16	BleAlertNotificationServiceAlertCategoryBitmask;
#define BLEALERTNOTIFICATIONSERVICE_CATEGORY_NONE								0x0000
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYIDBITMASK_SIMPLEALERT				0x0001
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYIDBITMASK_EMAIL						0x0002
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYIDBITMASK_NEWS						0x0004
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYIDBITMASK_CALL						0x0008
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYIDBITMASK_MISSEDCALL				0x0010
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYIDBITMASK_SMSMMS					0x0020
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYIDBITMASK_VOICEMAIL					0x0040
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYIDBITMASK_SCHEDULE					0x0080
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYIDBITMASK_HIGHPRIORITIZEDALERT		0x0100
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYIDBITMASK_INSTANTMESSAGE			0x0200


/** 
 * BleAlertNotificationServiceAlertCategoryId type
 * Define the different category which are notified by  
 * BLEALERTNOTIFICATIONSERVICE_SetNewAlert() or 
 * BLEALERTNOTIFICATIONSERVICE_SetUnreadAlertStatus() APIs.
 * It defines the category which will be notified.
 * It is not a bitfield, it represents a single category.
 */
typedef U8 BleAlertNotificationServiceAlertCategoryId;
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYID_SIMPLEALERT						0
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYID_EMAIL							1
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYID_NEWS								2
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYID_CALL								3
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYID_MISSEDCALL						4
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYID_SMSMMS							5
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYID_VOICEMAIL						6
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYID_SCHEDULE							7
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYID_HIGHPRIORITIZEDALERT				8
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYID_INSTANTMESSAGE					9
#define BLEALERTNOTIFICATIONSERVICE_CATEGORYID_ALLCATEGORIESWILDCARD			0xFF


/** 
 * BleAlertNotificationServiceCommandId type
 * Define the different kind of command that a remote MONITOR
 * can request to be set by the local SERVER
 * It can be one of the following values:
 */
typedef U8 BleAlertNotificationServiceCommandId;
#define BLEALERTNOTIFICATIONSERVICE_ENABLE_NEW_ALERT_NOTIFICATION					0
#define BLEALERTNOTIFICATIONSERVICE_ENABLE_UNREAD_ALERT_STATUS_NOTIFICATION		1	
#define BLEALERTNOTIFICATIONSERVICE_DISABLE_NEW_ALERT_NOTIFICATION					2
#define BLEALERTNOTIFICATIONSERVICE_DISABLE_UNREAD_ALERT_STATUS_NOTIFICATION		3	
#define BLEALERTNOTIFICATIONSERVICE_NOTIFY_NEW_ALERT_IMMEDIATELY					4
#define BLEALERTNOTIFICATIONSERVICE_NOTIFY_UNREAD_ALERT_STATUS_IMMEDIATELY			5	


/** 
 * BleAlertNotificationServiceNewAlert type
 * Define the number of new alerts (in a category) to be set 
 * during the call of BLEALERTNOTIFICATIONSERVICE_SetNewAlert()
 *
 */
typedef struct{ 
	BleAlertNotificationServiceAlertCategoryId	newAlertCategoryId;
	U8									numberOfNewAlerts;
#if (BLEALERTNOTIFICATION_SERVER_SUPPORT_TEXTSTRINGINFO == 1)
	/** If the NewAlert TextStringInfo value is supported, it shall be provided
	 * and valid during each call of BLEALERTNOTIFICATIONSERVICE_SetNewAlert 
	 */
	U8									*textStringInformation;
#endif //(BLEALERTNOTIFICATION_SERVER_SUPPORT_TEXTSTRINGINFO == 1)
} BleAlertNotificationServiceNewAlert;


/** 
 * BleAlertNotificationServiceUnreadAlertStatus type
 * Define the number of new alerts (in a category) to be set 
 * during the call of BLEALERTNOTIFICATIONSERVICE_SetUnreadAlertStatus()
 */
typedef struct{ 
	BleAlertNotificationServiceAlertCategoryId	unreadAlertStatusCategoryId;
	U8											numberOfUnreadAlerts;
} BleAlertNotificationServiceUnreadAlertStatus;


/** 
 * BleAlertNotificationServiceCommandRequest type
 * Type received as parameter in the BleAlertNotificationServiceCallBack
 * during the BLEALERTNOTIFICATIONSERVICE_EVENT_COMMANDNOTIFYRECEIVED or
 * BLEALERTNOTIFICATIONSERVICE_EVENT_COMMANDENABLERECEIVED or 
 * BLEALERTNOTIFICATIONSERVICE_EVENT_COMMANDDISABLERECEIVED event.
 * It indicates:
 * connHandle : the connection handle identifying the CLIENT from which the
 *     request is coming
 * controlPointCommandId : the remote CLIENT requested commandID.
 * requestedCategoryId : the requested category to which applies the command
 */
typedef struct{ 
	U16											connHandle;
	BleAlertNotificationServiceCommandId		controlPointCommandId;
	BleAlertNotificationServiceAlertCategoryId	requestedCategoryId;
} BleAlertNotificationServiceCommandRequest;


/**
 * BleAlertNotificationServiceCallBack
 *	This callback receives the ALERT NOTIFICATION SERVICE events. 
 *  Each events may be assiociated with specified parameters.
 *  The callback is executed during the stack context,  be careful to
 *	 not doing heavy process in this function.
 */
typedef void (*BleAlertNotificationServiceCallBack)(
	BleAlertNotificationServiceEvent	event, 
	BleStatus							status, 
	void								*parms); 

/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
#if (BLE_SUPPORT_ALERTNOTIFICATION_SERVICE == 1)
/* Localy Set the Alert Notification Service supported new alert category.
 *
 * BLEALERTNOTIFICATIONSERVICE_SetSupportedNewAlertCategory()
 *	This function is used to localy Set the Alert Notification Service  
 *  supported new alert category.
 *  Note that upon success, the supportedNewAlertCategory can be considered
 *  to be set, No callback event will be reported in the registered
 *  BleAlertNotificationServiceCallBack.
 *
 * BLE_SUPPORT_ALERTNOTIFICATION_SERVICE shall be enabled.
 *
 *
 * @param supportedNewAlertCategory: the new alert supported categories
 *			bitfield.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, merely 
 *     because Alert Notification Service is not registered by any profile.
 *
 * @author Mahault ANDRIA
 */
BleStatus BLEALERTNOTIFICATIONSERVICE_SetSupportedNewAlertCategory( 
	BleAlertNotificationServiceAlertCategoryBitmask  supportedNewAlertCategory );

/* Localy Set the Alert Notification Service supported unread alert  
 * status category
 *
 * BLEALERTNOTIFICATIONSERVICE_SetSupportedUnreadAlertStatusCategory()
 *	This function is used to localy Set the Alert Notification Service 
 *  supported unread alert status category.
 *  Note that upon success, the supportedUnreadAlertStatusCategory can be 
 *  considered to be set, No callback event will be reported in the registered
 *  BleAlertNotificationServiceCallBack.
 *
 * BLE_SUPPORT_ALERTNOTIFICATION_SERVICE shall be enabled.
 *
 * @param supportedUnreadAlertStatusCategory: the unread alert status supported 
 *			categories bitfield
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, merely because
 * Alert Notification Service is not registered by any profile.
 *
 * @author Mahault ANDRIA
 */
BleStatus BLEALERTNOTIFICATIONSERVICE_SetSupportedUnreadAlertStatusCategory( 
	BleAlertNotificationServiceAlertCategoryBitmask  supportedUnreadAlertStatusCategory );

/** Set the current New Alert
 *
 * BLEALERTNOTIFICATIONSERVICE_SetNewAlert()
 *	This function is used to set the New Alert value of this SERVER
 *  device 
 *
 * BLE_SUPPORT_ALERTNOTIFICATION_SERVICE shall be enabled.
 *
 * @param newAlertCategoryId : the category which will be notified
 *
 * @param countAlerts : the new count of alerts of the requested category
 *
 * @param textStringInfo : additional text information of the alert
 *
 * @param textStringInfoLen : the text info length
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
 * @author Mahault ANDRIA
 */
BleStatus BLEALERTNOTIFICATIONSERVICE_SetNewAlert(
			BleAlertNotificationServiceAlertCategoryId	newAlertCategoryId, 
			U8									countAlerts, 
			U8									*textStringInfo, 
			U8									textStringInfoLen);

/** Set the current Unread Alert Status 
 *
 * BLEALERTNOTIFICATIONSERVICE_SetUnreadAlertStatus()
 *	This function is used to set the New Alert value of this SERVER
 *  device 
 *
 * BLE_SUPPORT_ALERTNOTIFICATION_SERVICE shall be enabled.
 *
 * @param unreadAlertStatusCategoryId : the category which will be notified
 *
 * @param countAlerts : the new count of alerts of the requested category
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
 * @author Mahault ANDRIA
 */
BleStatus BLEALERTNOTIFICATIONSERVICE_SetUnreadAlertStatus(
	BleAlertNotificationServiceAlertCategoryId unreadAlertStatusCategoryId, 
	U8									countAlerts);



/** Localy check the New Alert Category.
 *
 * BLEALERTNOTIFICATIONSERVICE_IsNewAlertCategorySupported()
 * 	This function is used to check if the category of the new incoming  
 * 	alert is supported by the service.
 * 	It is called by the sample application before the update of the new 
 * 	alert count.
 *
 * BLE_SUPPORT_ALERTNOTIFICATION_SERVICE shall be enabled.
 *
 * @param alertCategoryId : the specified category for which you 
 *			want to know if it is supported by the service
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *    The alert count is updated and the server application calls
 *    the API BLEALERTNOTIFICATIONSERVICE_SetNewAlert.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *    If it fails, the alert count is not updated and the API 
 *    BLEALERTNOTIFICATIONSERVICE_SetNewAlert must not be called.
 *
 * @author Mahault ANDRIA
 */
BleStatus BLEALERTNOTIFICATIONSERVICE_IsNewAlertCategorySupported(
	BleAlertNotificationServiceAlertCategoryId alertCategoryId);

/** Localy check the Unread Alert Status Category.
 *
 * BLEALERTNOTIFICATIONSERVICE_IsUnreadAlertStatusCategorySupported()
 *  This function is used to check if the category of the unread incoming  
 *  alert status is supported by the service.
 *  It is called by the sample application before the update of the unread
 *  alert status count.
 *
 * BLE_SUPPORT_ALERTNOTIFICATION_SERVICE shall be enabled.
 *
 * @param unreadAlertStatusCategoryId : the ID of the category
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *    The alert count is updated and the server application calls
 *    the API BLEALERTNOTIFICATIONSERVICE_SetNewAlert.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *    If it fails, the alert count is not updated and the API 
 *    BLEALERTNOTIFICATIONSERVICE_SetUnreadAlertStatus must not be called.
 *
 * @author Mahault ANDRIA
 */
BleStatus BLEALERTNOTIFICATIONSERVICE_IsUnreadAlertStatusCategorySupported(
	BleAlertNotificationServiceAlertCategoryId	unreadAlertStatusCategoryId);


/** Get the current Enabled New Alert Categories
 *
 * BLEALERTNOTIFICATIONSERVICE_GetEnabledNewAlertCategory()
 *	This function is used to get the current Enabled New Alert Categories 
 *	of this SERVER device. This value has been previously written by 
 *	the remote device.
 *
 * BLE_SUPPORT_ALERTNOTIFICATION_SERVICE shall be enabled.
 *
 * @param connHandle: The connection handle identifying the remote
 *			device for which the Enabled New Alert Categories need to be
 *			retrieved.
 *
 * @param alertCategoryId : A valid pointer to a 
 *         BleAlertNotificationServiceAlertCategoryId that will receive, if the
 *         function return BLESTATUS_SUCCESS,the current Enabled 
 *		   New Alert Categories of the SERVER device as previously 
 *		   written by the remote device.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Mahault ANDRIA
 */
BleStatus BLEALERTNOTIFICATIONSERVICE_GetEnabledNewAlertCategory(
	U16												  connHandle,
	BleAlertNotificationServiceAlertCategoryBitmask   *alertCategoryId);

/** Get the current Enabled Unread Alert Status Categories
 *
 * BLEALERTNOTIFICATIONSERVICE_GetEnabledUnreadAlertStatusCategory()
 *	This function is used to get the current Enabled Unread Alert Status 
 *  Categories of this SERVER device. This value has been previously  
 *	written by the remote device.
 *
 * BLE_SUPPORT_ALERTNOTIFICATION_SERVICE shall be enabled.
 *
 * @param connHandle: The connection handle identifying the remote
 *			device for which the Enabled New Alert Categories need to be
 *			retrieved.
 *
 * @param alertCategoryId : A valid pointer to a 
 *         BleAlertNotificationServiceAlertCategoryId that will receive, if the
 *         function return BLESTATUS_SUCCESS,the current Enabled 
 *		   New Alert Categories of the SERVER device as previously 
 *		   written by the remote device.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Mahault ANDRIA
 */
BleStatus BLEALERTNOTIFICATIONSERVICE_GetEnabledUnreadAlertStatusCategory(
	U16												 connHandle,
	BleAlertNotificationServiceAlertCategoryBitmask  *alertCategoryId);

/** Get the current state of New Alert Characteristic
 *
 * BLEALERTNOTIFICATION_SERVER_GetNewAlertState()
 *	This function is used to get the current state of New Alert characteristic 
 *  for a specified category
 *
 * BLE_SUPPORT_ALERTNOTIFICATION_SERVICE shall be enabled.
 *
 * @param categoryId: The category id for which the new alert
 *		  state need to be retrieved.
 *
 * @param countAlerts : A valid pointer to an 
 *         U8 that will receive, if the function returns 
 *         BLESTATUS_SUCCESS,the current count of new alert 
 *		   for the specified category of the SERVER device 
 *
 * @param textInfo : A valid pointer to an 
 *         U8 that will receive, if the function returns 
 *         BLESTATUS_SUCCESS,the current text information 
 *		   for the specified category of the SERVER device 
 *
 * @param textInfoLen : A valid pointer to an 
 *         U8 that will receive, if the function returns 
 *         BLESTATUS_SUCCESS,the current text information length
 *		   for the specified category of the SERVER device 
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Mahault ANDRIA
 */
BleStatus BLEALERTNOTIFICATION_SERVER_GetNewAlertState(
	BleAlertNotificationServiceAlertCategoryId	categoryId, 
	U8											*countAlerts, 
	U8											**textInfo, 
	U8											*textInfoLen);

/** Get the current state of Unread Alert Status Characteristic
 *
 * BLEALERTNOTIFICATION_SERVER_GetUnreadAlertState()
 *	This function is used to get the current state of Unread Alert 
 *  Status characteristic for a specified category
 *
 * BLE_SUPPORT_ALERTNOTIFICATION_SERVICE shall be enabled.
 *
 * @param categoryId: The category id for which the Unread Alert
 *		  state need to be retrieved.
 *
 * @param countAlerts : A valid pointer to an 
 *         U8 that will receive, if the function returns 
 *         BLESTATUS_SUCCESS,the current count of Unread Alert
 *		   for the specified category of the SERVER device 
 *
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Mahault ANDRIA
 */
BleStatus BLEALERTNOTIFICATION_SERVER_GetUnreadAlertState(
	BleAlertNotificationServiceAlertCategoryId	categoryId, 
	U8											*countAlerts);



#endif //(BLE_SUPPORT_ALERTNOTIFICATION_SERVICE == 1)

#endif //__BLEALERTNOTIFICATION_SERVICE_H
