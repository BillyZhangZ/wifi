#ifndef __BLELOCATIONANDNAVIGATION_SERVICE_H
#define __BLELOCATIONANDNAVIGATION_SERVICE_H
/****************************************************************************
 *
 * File:          BleLocationAndNavigation_Service.h
 *
 * Description:   Contains routines declaration for Location And Navigation
 *					Service.
 * 
 * Created:       November, 2012
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.6 $
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
 * BLE_SUPPORT_LOCATIONANDNAVIGATION_SERVICE
 * Defines if the BLE local device enables access to the local LOCATION 
 * AND NAVIGATION service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles to get access to the 
 * LOCATION AND NAVIGATION Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_LOCATIONANDNAVIGATION_SERVICE
#define BLE_SUPPORT_LOCATIONANDNAVIGATION_SERVICE                                       0
#endif //BLE_SUPPORT_LOCATIONANDNAVIGATION_SERVICE

/** 
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_TOTAL_DISTANCE
 * Define if the LNS Sensor supports (1) or not (0) to set the total 
 * Distance value in location and speed.
 * The default value is not supported (0)
 */
#ifndef BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_TOTAL_DISTANCE	
#define BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_TOTAL_DISTANCE                         0
#endif //BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_TOTAL_DISTANCE

/** 
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_INSTANTANEOUS_SPEED
 * Define if the LNS Sensor supports (1) or not (0) to set the instantaneous  
 * speed value in location and speed.
 * The default value is not supported (0)
 */
#ifndef BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_INSTANTANEOUS_SPEED	
#define BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_INSTANTANEOUS_SPEED                    0
#endif //BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_INSTANTANEOUS_SPEED


/** 
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION
 * Define if the LNS Sensor supports (1) or not (0) the navigation.
 * The default value is not supported (0)
 */
#ifndef BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION	        
#define BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION                             0
#endif //BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION

/** 
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_REMAINING_DISTANCE
 * Define if the LNS Sensor supports (1) or not (0) to set the remaining
 * distance value in navigation.
 * The default value is not supported (0)
 */
#ifndef BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_REMAINING_DISTANCE	
#define BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_REMAINING_DISTANCE                     0
#endif //BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_REMAINING_DISTANCE


/***************************************************************************\
 * CONSTANTS
\***************************************************************************/
/** BLEINFOTYPE_LOCATIONANDNAVIGATIONSERVICE_CLIENTCONFIG
 * Unique descriptors ID for information to save
 * in persistent memory representing the client config for a given remote
 * bonded device and configured location and speed and navigation.
 * It is the Unique identifier passed in the infotype fields when the PROFILE
 * call the system APIs SYSTEM_SetPersistentInformation(BD_ADDR addr,
 * U8 infoType, U8* infoValue,U8 InfoLen) and 
 * SYSTEM_GetPersistentInformation(BD_ADDR addr, U8 infoType, U8 **infoValue,
 * U8 *InfoLen). 
 */
#define BLEINFOTYPE_LOCATIONANDNAVIGATIONSERVICE_LOCATIONANDSPEED_CLIENTCONFIG	(0xC0u)
#define BLEINFOTYPE_LOCATIONANDNAVIGATIONSERVICE_NAVIGATION_CLIENTCONFIG		(0xC1u)
#define BLEINFOTYPE_LOCATIONANDNAVIGATIONSERVICE_CONTROLPOINT_CLIENTCONFIG		(0xC2u)


/***************************************************************************\
 *	Type definition
\***************************************************************************/
/** 
 * BleLocationAndNavigationServiceControlNavigation type
 * Navigation control procedures in parameter of the LN Control Point
 */
//NAVIGATION CONTROL PROCEDURES
typedef U8 BleLocationAndNavigationServiceControlNavigation;
#define BLELOCATIONANDNAVIGATIONSERVICE_NAVIGATIONCONTROL_STOPNAVIGATION				(0x00u)
#define BLELOCATIONANDNAVIGATIONSERVICE_NAVIGATIONCONTROL_STARTNAVIGATION				(0x01u)
#define BLELOCATIONANDNAVIGATIONSERVICE_NAVIGATIONCONTROL_PAUSENAVIGATION				(0x02u)
#define BLELOCATIONANDNAVIGATIONSERVICE_NAVIGATIONCONTROL_CONTINUENAVIGATION			(0x03u)
#define BLELOCATIONANDNAVIGATIONSERVICE_NAVIGATIONCONTROL_SKIPWAYPOINT					(0x04u)
#define BLELOCATIONANDNAVIGATIONSERVICE_NAVIGATIONCONTROL_STARTNOTIFICATION				(0x05u)


/** 
 * BleLocationAndNavigationServiceLocationAndSpeedFlags type
 * Bitfield defining the available location and speed flags values.
 */
typedef U16 BleLocationAndNavigationServiceLocationAndSpeedFlags;
#define BLELNSSERVICE_FLAGBITMASK_INSTANTANEOUSSPEEDPRESENT             (0x0001u)
#define BLELNSSERVICE_FLAGBITMASK_TOTALDISTANCEPRESENT		            (0x0002u)
#define BLELNSSERVICE_FLAGBITMASK_SPEEDANDDISTANCEFORMAT           	    (0x0200u)

/** 
 * BleLocationAndNavigationServiceNavigationFlags type
 * Bitfield defining the available navigation flags values.
 */
typedef U16 BleLocationAndNavigationServiceNavigationFlags;
#define BLELNSSERVICE_FLAGBITMASK_REMAININGDISTANCEPRESENT              (0x0001u)
#define BLELNSSERVICE_FLAGBITMASK_NAVIGATION_POSITIONSTATUS             (0x0018u)
#define BLELNSSERVICE_FLAGBITMASK_HEADINGSOURCE                         (0x0020u)
#define BLELNSSERVICE_FLAGBITMASK_NAVIGATIONINDICATORTYPE               (0x0040u)
#define BLELNSSERVICE_FLAGBITMASK_WAYPOINTREACHED                       (0x0080u)
#define BLELNSSERVICE_FLAGBITMASK_DESTINATIONREACHED                    (0x0100u)

/** 
 * LocationAndSpeedFlags type
 * Define the current Location And Speed flags to be set during the 
 * call of BLELOCATIONANDNAVIGATIONSERVICE_SetLocationAndSpeed()
 * It can be builded using the flag value or bit to bit definition
 */
typedef union {
	BleLocationAndNavigationServiceLocationAndSpeedFlags
                                    locationAndSpeedFlagsValue;
	struct 
	{
		unsigned int instantaneousSpeedPresent:1;
		unsigned int totalDistancePresent:1;
		//unsigned int locationPresent:1;
		//unsigned int elevationPresent:1;
		//unsigned int headingPresent:1;
		//unsigned int rollingTimePresent:1;
		//unsigned int utcTimePresent:1;
		////////
		unsigned int reserved:7;
		//unsigned int positionStatus:2;
		unsigned int speedandDistanceFormat:1;
		//unsigned int elevationSource:2;
		//unsigned int headingSource:1;
		//unsigned int reserved:3;
	} fields;
} LocationAndSpeedFlags;


#if (BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION == 1)
/** 
 * NavigationFlags type
 * Define the current Navigation flags to be set during the 
 * call of BLELOCATIONANDNAVIGATIONSERVICE_SetNavigation()
 * It can be builded using the flag value or bit to bit definition
 */
typedef union {
	BleLocationAndNavigationServiceNavigationFlags
        navigationFlagsValue;
	struct 
	{
		unsigned int remainingDistancePresent:1;
		//unsigned int remainingVerticalDistancePresent:1;
		//unsigned int estimatedTimeofArrivalPresent:1;
		unsigned int reserved:4;
		//unsigned int positionStatus:2;
		unsigned int headingSource:1;
		unsigned int navigationIndicatorType:1;
		unsigned int waypointReached:1;
		unsigned int destinationReached:1;
		//unsigned int reserved:7;
	} fields;

} NavigationFlags;
#endif //(BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION == 1)

////Optionnal
////Flags field for Optionnal Characteristic "Position Quality"
//typedef union {
//	U16 positionQualityFlagsValue;
//	struct 
//	{
//		unsigned int numberofSatellitesinSolutionPresent:1;
//		unsigned int numberofSatellitesinViewPresent:1;
//		unsigned int timetoFirstFixPresent:1;
//		unsigned int ehpePresent:1;
//		unsigned int evpePresent:1;
//		unsigned int hdopPresent:1;
//		unsigned int vdopPresent:1;
//		unsigned int positionStatus:2;
//		//unsigned int reserved:7;
//	} fields;
//
//} PositionQualityFlags;


/** 
 * BleLocationAndNavigationServiceLocationAndSpeed type
 * Define the different location and speed fields to be set during 
 * the call of BLELOCATIONANDNAVIGATIONSERVICE_SetLocationAndSpeed()
 *
 */
typedef struct{ 
    /** The flags byte value
     */
	LocationAndSpeedFlags					flagsValue;

#if (BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_INSTANTANEOUS_SPEED == 1)
	/** If the instantaneaous speed value is supported, it shall be provided
	 * and valid during each call of BLELOCATIONANDNAVIGATIONSERVICE_SetLocationAndSpeed
	 */
	U16										instantaneousSpeedValue;
#endif //(BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_INSTANTANEOUS_SPEED == 1)

#if (BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_TOTAL_DISTANCE == 1)
    /** totalDistanceValue:
     * The total distance value
     *  Unit is in meter with a resolution of 1/10 m (or decimeter).
     */
// WARNING : The U24 (3 bytes) type doesn't exist ..so you need to use
// a U32 type .. you can mask it with a 0x00FFFFFF
	U32										totalDistanceValue;
#endif //(BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_TOTAL_DISTANCE == 1)
//	S32										locationValue;
//	S24										elevationValue;
//	U8										rollingTimeValue;
//	BleDayDateTime							utcTimeValue;
} BleLocationAndNavigationServiceLocationAndSpeed;


#if (BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION == 1)
/** 
 * BleLocationAndNavigationServiceNavigation type
 * Define the different location and navigation fields to be set during 
 * the call of BLELOCATIONANDNAVIGATIONSERVICE_SetNavigation()
 *
 */
typedef struct{ 
    /** The flags byte value
     */
	NavigationFlags							flagsValue;
	U16										bearingValue;
	U16										headingValue;
#if (BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_REMAINING_DISTANCE == 1)
	U32										remainingDistanceValue;
#endif //(BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_REMAINING_DISTANCE == 1)
//	S24										remainingVerticalDistanceValue;
//	BleDayDateTime							estimatedTimeOfArrivalValue;
} BleLocationAndNavigationServiceNavigation;
#endif //(BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION == 1)


/** 
 * BleLocationAndNavigationServiceEvent type
 * Define the different kind of events that could be received by the 
 * BleLocationAndNavigationServiceCallback
 */
typedef U8 BleLocationAndNavigationServiceEvent;

/** BLELOCATIONANDNAVIGATIONSERVICE_EVENT_TOTALDISTANCEUPDATED
 * Event received when a COLLECTOR has Set a new Cumulative Value for
 * the total distance.
 * The Status field is always BLESTATUS_SUCCESS.
 * The parms field indicates the total distance value as a U32.
 * U32 totalDistance = *((U32 *) parms).
 * NOTE : The SENSOR shall apply/set the new Total Distance Value when
 * receiving this event.
 * This event is only applicable when 
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_TOTAL_DISTANCE is supported.
 */
#define BLELOCATIONANDNAVIGATIONSERVICE_EVENT_TOTALDISTANCEUPDATED		0x75

/** BLELOCATIONANDNAVIGATIONSERVICE_EVENT_STOPNAVIGATION
 * Event received when a COLLECTOR has sent a command to stop navigation
 * and notifications of the navigation feature via the navigation control.
 * The Status field is always BLESTATUS_SUCCESS.
 * The parms field indicates the command specified of the navigation control.
 * This event is only applicable when 
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION is supported.
 */
#define BLELOCATIONANDNAVIGATIONSERVICE_EVENT_STOPNAVIGATION			0x76

/** BLELOCATIONANDNAVIGATIONSERVICE_EVENT_STARTNAVIGATION
 * Event received when a COLLECTOR has sent a command to start navigation
 * and notifications of the navigation feature via the navigation control.
 * The Status field is always BLESTATUS_SUCCESS.
 * The parms field indicates the command specified of the navigation control.
 * This event is only applicable when 
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION is supported.
 */
#define BLELOCATIONANDNAVIGATIONSERVICE_EVENT_STARTNAVIGATION			0x77

/** BLELOCATIONANDNAVIGATIONSERVICE_EVENT_PAUSENAVIGATION
 * Event received when a COLLECTOR has sent a command to pause navigation
 * and notifications of the navigation feature via the navigation control.
 * The Status field is always BLESTATUS_SUCCESS.
 * The parms field indicates the command specified of the navigation control.
 * This event is only applicable when 
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION is supported.
 */
#define BLELOCATIONANDNAVIGATIONSERVICE_EVENT_PAUSENAVIGATION			0x78

/** BLELOCATIONANDNAVIGATIONSERVICE_EVENT_CONTINUENAVIGATION
 * Event received when a COLLECTOR has sent a command to continue navigation
 * and notifications of the navigation feature via the navigation control from
 * the nearest waypoint.
 * The Status field is always BLESTATUS_SUCCESS
 * The parms field indicates the command specified of the navigation control.
 * This event is only applicable when 
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION is supported.
 */
#define BLELOCATIONANDNAVIGATIONSERVICE_EVENT_CONTINUENAVIGATION		0x79

/** BLELOCATIONANDNAVIGATIONSERVICE_EVENT_SKIPWAYPOINT
 * Event received when a COLLECTOR has sent a command to skip a waypoint
 * via the navigation control. It does not affect notifications.
 * The Status field is always BLESTATUS_SUCCESS.
 * The parms field indicates the command specified of the navigation control.
 * This event is only applicable when 
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION is supported.
 */
#define BLELOCATIONANDNAVIGATIONSERVICE_EVENT_SKIPWAYPOINT				0x7A

/** BLELOCATIONANDNAVIGATIONSERVICE_EVENT_STARTNOTIFICATION
 * Event received when a COLLECTOR has sent a command to start 
 * notifications of the navigation feature via the navigation control from
 * the nearest waypoint.
 * The Status field is always BLESTATUS_SUCCESS.
 * The parms field indicates the command specified of the navigation control.
 * This event is only applicable when 
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION is supported.
 */
#define BLELOCATIONANDNAVIGATIONSERVICE_EVENT_STARTNOTIFICATION			0x7B

/** BLELOCATIONANDNAVIGATIONSERVICE_EVENT_SELECTROUTE
 * Event received when a COLLECTOR has sent a command to select a route
 * he wants to navigate via the navigation control.
 * The Status field is always BLESTATUS_SUCCESS.
 * The parms field indicates the command specified of the navigation control.
 * This event is only applicable when 
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION is supported.
 */
#define BLELOCATIONANDNAVIGATIONSERVICE_EVENT_SELECTROUTE				0x7C

/**
 * BleLocationAndNavigationServiceCallBack
 *	This callback receives the LOCATION AND NAVIGATION SERVICE events. 
 *  Each events may be associated with specified parameters.
 *  The callback is executed during the stack context,  be careful to
 *	 not do heavy process in this function.
 */
typedef void (*BleLocationAndNavigationServiceCallBack)(
	BleLocationAndNavigationServiceEvent	event, 
	BleStatus								status, 
	void									*parms); 

/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
#if (BLE_SUPPORT_LOCATIONANDNAVIGATION_SERVICE == 1)

/** Set the current Location And Speed 
 *
 * BLELOCATIONANDNAVIGATIONSERVICE_SetLocationAndSpeed()
 *	This function is used to set the current Location And Speed 
 * characteristic value
 *
 * BLE_SUPPORT_LOCATIONANDNAVIGATION_SERVICE shall be enabled.
 *
 * @param locationAndSpeed : the structure representing the Location 
 *		and Speed, see the field description of 
 *		BleLocationAndNavigationServiceLocationAndSpeed for more detailled information.
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
BleStatus BLELOCATIONANDNAVIGATIONSERVICE_SetLocationAndSpeed(
						BleLocationAndNavigationServiceLocationAndSpeed *locationAndSpeed);

#if (BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION == 1)
/** Set the current Navigation 
 *
 * BLELOCATIONANDNAVIGATIONSERVICE_SetNavigation()
 *	This function is used to set the current Location And Speed 
 * characteristic value
 *
 * BLE_SUPPORT_LOCATIONANDNAVIGATION_SERVICE shall be enabled.
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION shall be enabled.
 *
 * @param navigation : the structure representing the Location 
 *		and Speed, see the field description of 
 *		BleLocationAndNavigationServiceNavigation for more detailled information.
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
BleStatus BLELOCATIONANDNAVIGATIONSERVICE_SetNavigation(
						BleLocationAndNavigationServiceNavigation *navigation);
#endif //(BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION == 1)

#if (BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION == 1)
/** Set the current number of routes 
 *
 * BLELOCATIONANDNAVIGATIONSERVICE_SetNumberOfRoutes()
 *	This function is used to set the current number of routes 
 * characteristic value
 *
 * BLE_SUPPORT_LOCATIONANDNAVIGATION_SERVICE shall be enabled.
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION shall be enabled.
 *
 * @param nbOfRoutesStored : the number of routes stored 
 *		on the sensor.
 *
 * @return always BLESTATUS_SUCCESS.
 *
 * @author Mahault ANDRIA
 */
BleStatus BLELOCATIONANDNAVIGATIONSERVICE_SetNumberOfRoutes(
						U16 nbOfRoutesStored);
#endif //(BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION == 1)

#if (BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION == 1)
/** Get the route name 
 *
 * BLELOCATIONANDNAVIGATIONSERVICE_GetRouteName()
 *
 * This function is an application interface CALLED by the stack when
 * the service need to get the route name of the specified route.
 * This function SHALL be implemented if 
 * BLE_SUPPORT_LOCATIONANDNAVIGATION_SERVICE and 
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION are supported. 
 *
 * BLE_SUPPORT_LOCATIONANDNAVIGATION_SERVICE shall be enabled.
 * BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION shall be enabled.
 *
 * @param routeId : the desired Route ID  
 *
 * @param nameRouteInfo : a pointer on the route name to be returned
 *
 * @param nameRouteInfoLen : a pointer on the route name length
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed. 
 *
 * @author Mahault ANDRIA
 */
BleStatus BLELOCATIONANDNAVIGATIONSERVICE_GetRouteName(
						U16 routeId,
						U8** nameRouteInfo,
						U8* nameRouteInfoLen);
#endif //(BLE_LOCATIONANDNAVIGATIONSERVICE_SUPPORT_NAVIGATION == 1)

#endif //(BLE_SUPPORT_LOCATIONANDNAVIGATION_SERVICE == 1)
#endif //__BLELOCATIONANDNAVIGATION_SERVICE_H
