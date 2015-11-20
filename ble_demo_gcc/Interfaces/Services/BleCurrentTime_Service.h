#ifndef __BLECURRENTTIME_SERVICE_H
#define __BLECURRENTTIME_SERVICE_H
/****************************************************************************
 *
 * File:          BleCurrentTime_Service.h
 *
 * Description:   Contains routines declaration for CurrentTime Service.
 * 
 * Created:       October, 2011
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

#include "BleTypes.h"
#include "BleTimeTypes.h"

/***************************************************************************\
 *	Type definition
\***************************************************************************/
/** 
 * BleCurrentTimeServiceAdjustReason type
 * BleCurrentTimeServiceAdjustReason defines the current Time possible
 * adjust reason when the SERVER application call 
 * BLECURRENTTIME_SetCurrentTime.
 * It is a bitfields where the following bit can be activated or not:
 * BLECURRENTTIMESERVICE_ADJUSTREASON_UNKNOW if set means that the time has 
 *	been updated du to an unknown reason.
 * BLECURRENTTIMESERVICE_ADJUSTREASON_MANUAL if set means that the time has 
 *	been updated du to a manual update.
 * BLECURRENTTIMESERVICE_ADJUSTREASON_EXTERNALREFTIMEUPADTE if set means that
 *	the time has been updated du to an external reference time update.
 * BLECURRENTTIMESERVICE_ADJUSTREASON_TIMEZONECHANGE if set means that the 
 *	time has been updated du to a Time zone change.
 * BLECURRENTTIMESERVICE_ADJUSTREASON_DSTCHANGE if set means that the time has 
 *	been updated du to a DST change.
 * 
 */
typedef U8 BleCurrentTimeServiceAdjustReason;

#define BLECURRENTTIMESERVICE_ADJUSTREASON_UNKNOW						0x00
#define BLECURRENTTIMESERVICE_ADJUSTREASON_MANUAL						0x01
#define BLECURRENTTIMESERVICE_ADJUSTREASON_EXTERNALREFTIMEUPADTE		0x02
#define BLECURRENTTIMESERVICE_ADJUSTREASON_TIMEZONECHANGE				0x04
#define BLECURRENTTIMESERVICE_ADJUSTREASON_DSTCHANGE					0x08

/***************************************************************************\
 * OPTIONAL FEATURES
\***************************************************************************/
/**
 * BLE_SUPPORT_CURRENTTIME_SERVICE
*	Defines if the BLE local device enables access to the local  
 *	CurrentTime service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles to get access to the  
 * CurrentTime Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_CURRENTTIME_SERVICE
#define BLE_SUPPORT_CURRENTTIME_SERVICE								0
#endif //BLE_SUPPORT_CURRENTTIME_SERVICE

#if (BLE_SUPPORT_CURRENTTIME_SERVICE == 1)

/**
 * BLE_CURRENTTIME_SUPPORT_LOCALTIMEINFORMATION
 *	Defines if the BLE local device enables access to the local  
 *	CurrentTime service's local time Information optional characteristic.
 *  The Local Time Information characteristic hosts the local time 
 *  information that includes time zone and DST offset.
 *
 *  The Time zone field of the Local Time Information is the offset of the
 *  local standard time compared to UTC and the DST offset field of Local 
 *  Time Information is the current DST offset. If time zone and DST offset
 *  information are currently unavailable, the Current Time Server may set
 *  the fields to the values defined as 'time zone unknown' or 'DST 
 *  offset unknown'.
 * 
 * If enabled ( set to 1 ) it enables Access to the 
 * BLECURRENTTIME_SetLocalTimeInformation() 
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_CURRENTTIME_SUPPORT_LOCALTIMEINFORMATION
#define BLE_CURRENTTIME_SUPPORT_LOCALTIMEINFORMATION				0
#endif //BLE_CURRENTTIME_SUPPORT_LOCALTIMEINFORMATION

/**
 * BLE_CURRENTTIME_SUPPORT_REFERENCETIMEINFORMATION
 *	Defines if the BLE local device enables access to the local  
 *	CurrentTime service's reference time Information optional characteristic.
 *  The reference Time Information characteristic hosts the information 
 *  about the reference time source.
 *
 *  The Reference Time Information characteristic hosts the Time Source 
 *  of the best source of its current time information, Days Since Update and
 *  Hours Since last Update and the time accuracy.
 * 
 * If enabled ( set to 1 ) it enables Reception to the 
 * BLECURRENTTIMESERVICE_EVENT_REFERENCETIMEINFORMATIONREQUEST event. 
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_CURRENTTIME_SUPPORT_REFERENCETIMEINFORMATION
#define BLE_CURRENTTIME_SUPPORT_REFERENCETIMEINFORMATION			0
#endif //BLE_CURRENTTIME_SUPPORT_REFERENCETIMEINFORMATION

/***************************************************************************\
 * CONSTANTS
\***************************************************************************/
/* BLEINFOTYPE_CURRENTTIME_CLIENTCONFIG 
 * The Unique descriptor ID for a two Bytes length information to save
 * in persistent memory representing the client config for a given remote
 * bonded device.
 * It is the Unique identifier passed in the infotype fields when the PROFILE
 * call the system APIs SYSTEM_SetPersistentInformation(BD_ADDR addr,
 * U8 infoType, U8* infoValue,U8 InfoLen) and 
 * SYSTEM_GetPersistentInformation(BD_ADDR addr, U8 infoType, U8 **infoValue,
 * U8 *InfoLen). 
 */
#define BLEINFOTYPE_CURRENTTIME_CLIENTCONFIG						0x70	



/***************************************************************************\
 *	Type definition
\***************************************************************************/
/** 
 * BleCurrentServiceEvent type
 * Define the different kind of events that could be received by the 
 * BleCurrentTimeServiceCallBack
 */
typedef U8 BleCurrentTimeServiceEvent;

/** BLECURRENTTIMESERVICE_EVENT_TIMEREQUEST
 * Event received when a new time is requested from the server.
 * for example when the remote device read the current time from the server
 * the server shall answer with the current time. It should answer as soon
 * as possible, because it exists a timeout.
 * the parms field is a BleCurrentTimeServiceTimeRequest structure, the 
 * currentDateTime field, and optionally the fraction256 field shall represent
 * the current date and time when exiting of the callback.
 */
#define BLECURRENTTIMESERVICE_EVENT_TIMEREQUEST							0xC2

/** BLECURRENTTIMESERVICE_EVENT_TIMEREFERENCEINFORMATIONREQUEST
 * Event received when the reference time information is requested from
 * the server.
 * for example when the remote device read the current reference time
 * information from the server the server shall answer with its current 
 * information.
 * It should answer as soon as possible, because it exists a timeout.
 * the parms field is a BleCurrentTimeServiceReferenceTimeInfoRequest   
 * structure, the fields shall contains reference time information when 
 * exiting of the callback, otherwise default value will be taken.
 */
#define BLECURRENTTIMESERVICE_EVENT_REFERENCETIMEINFORMATIONREQUEST		0xC3

/** 
 * BleCurrentTimeServiceTimeRequest type
 * Defines the parameters received in the BleCurrentTimeServiceCallBack during  
 * the BLECURRENTTIMESERVICE_EVENT_TIMEREQUEST event.
 *
 * it contains:
 * currentDateTime: a valid currentDateTime pointer provided by the stack 
 *			where the application shall write the current date time
 * fraction256 : a valid BleTimeFraction256 pointer provided by the stack 
 *			where the application shall write, if available, the current
 *			fraction256 of seconds
 */
typedef struct{
	BleDayDateTime						*currentDateTime;
	BleTimeFraction256					*fraction256;
} BleCurrentTimeServiceTimeRequest;


/** 
 * BleCurrentTimeServiceReferenceTimeInfoRequest type
 * Defines the parameters received in the BleCurrentTimeServiceCallBack during  
 * the BLECURRENTTIMESERVICE_EVENT_REFERENCETIMEINFORMATIONREQUEST event.
 *
 * it contains:
 * timeSource : The Time source of the last update, if time source information
 *  is currently unavailable, the Current Time Server
 *  may set the field to the values defined as 'time source unknown'
 *  default is 'time source unknown'
 *
 * accuracy : the current Accuracy (drift) of time information in steps
 *  of 1/8 of a second (125ms) compared to a reference time source. 
 *  Valid range from 0 to 253 (0s to 31.5s). 
 *  A value of 254 means Accuracy is out of range (> 31.5s). 
 *  A value of 255 means Accuracy is unknown (defualt is not filled).
 *
 * daysSinceUpdate : the number of days since the last update.
 *  A value of 255 means 255 or more days.
 *  if not filled, has value of 255;
 *
 * hoursSinceUpdate : the number of hours since the last update.
 *  Valid range for hours from 0 to 23.
 *  if daysSinceUpdate is equal to 255 then hours since update shall be 
 *  equal to 255.
 *  if not filled, has value of 255;
 */
typedef struct{
	BleTimeSource						*timeSource;
	U8									*accuracy;
	U8									*daysSinceUpdate;
	U8									*hoursSinceUpdate;
} BleCurrentTimeServiceReferenceTimeInfoRequest;

/**
 * BleCurrentTimeServiceCallBack
 *	This callback receives the CURRENT TIME Service events. 
 *  Each events may be assiociated with specified status and parameters.
 *  The callback is executed during the stack context,  be careful to
 *	 not doing heavy process in this function.
 */
typedef void (*BleCurrentTimeServiceCallBack)(
					BleCurrentTimeServiceEvent event,
					BleStatus status,
					void *parms); 


/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
/** Set the current time
 *
 * BLECURRENTTIME_SetCurrentTime()
 *	This function is used to set the current Time value
 *
 * BLE_SUPPORT_CURRENTTIME_SERVICE shall be enabled.
 *
 * @param currentDateTime : the structure representing the current date time
 *					value
 * @param fraction256 : the current fraction of second in 1/256 format
 * @param adjustReason : the currentTime adjust reason.
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
BleStatus BLECURRENTTIME_SetCurrentTime(
						BleDayDateTime						*currentDateTime,
						BleTimeFraction256					fraction256,
						BleCurrentTimeServiceAdjustReason	adjustReason);

#if (BLE_CURRENTTIME_SUPPORT_LOCALTIMEINFORMATION == 1)
/** Set the local time information
 *
 * BLECURRENTTIME_SetLocalTimeInformation()
 *	This function is used to set the local time Information.
 *  The local time information includes time zone and DST offset.
 *
 *
 * BLE_SUPPORT_CURRENTTIME_SERVICE shall be enabled.
 * BLE_CURRENTTIME_SUPPORT_LOCALTIMEINFORMATION shall be enabled
 *
 * @param timeZone : The Time zone field is the offset of the
 *  local standard time compared to UTC.
 *	If time zone information is currently unavailable, the Current Time Server
 *  may set the field to the values defined as 'time zone unknown'
 *
 * @param dstOffset : the current DST offset.
 *	If DST offset information is currently unavailable, the Current Time Server 
 *  may set the field to the values defined as 'DST offset unknown'.
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
BleStatus BLECURRENTTIME_SetLocalTimeInformation(
						BleTimeZone					timeZone,
						BleTimeDSTOffset			dstOffset);
#endif //(BLE_CURRENTTIME_SUPPORT_LOCALTIMEINFORMATION == 1)


#endif //(BLE_SUPPORT_CURRENTTIME_SERVICE == 1)
#endif //__BLECURRENTTIME_SERVICE_H
