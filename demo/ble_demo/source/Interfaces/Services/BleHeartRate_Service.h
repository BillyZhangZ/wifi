#ifndef __BLEHEARTRATE_SERVICE_H
#define __BLEHEARTRATE_SERVICE_H
/****************************************************************************
 *
 * File:          BleHeartRate_Service.h
 *
 * Description:   Contains routines declaration for Heart Rate Service.
 * 
 * Created:       march, 2011
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.7 $
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
 * OPTIONAL FEATURES
\***************************************************************************/
/**
 * BLE_SUPPORT_HEARTRATE_SERVICE
*	Defines if the BLE local device enables access to the local  
 *	Heart Rate service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles to include the  
 * Heart Rate Service and characteristic into the local device attribute
 * database.
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_HEARTRATE_SERVICE
#define BLE_SUPPORT_HEARTRATE_SERVICE								0
#endif //BLE_SUPPORT_HEARTRATE_SERVICE

#if (BLE_SUPPORT_HEARTRATE_SERVICE == 1)
/**
 * BLE_HEARTRATE_SERVICE_SUPPORT_BODYSENSORLOCATION
 * The Body Sensor Location of the device is used to describe the intended 
 *  location of the heart rate measurement for the device. 
 * The value of the Body Sensor Location characteristic is static while 
 *  in a connection.
 *
 * if BLE_HEARTRATE_SERVICE_SUPPORT_BODYSENSORLOCATION is enabled (1), the 
 * BLEHEARTRATE_SetBodySensorLocation API will be enabled to 
 * allow the application to change the location when not connected.
 *
 * The default value for this option is disabled (0).
 *
 */
#ifndef BLE_HEARTRATE_SERVICE_SUPPORT_BODYSENSORLOCATION
#define BLE_HEARTRATE_SERVICE_SUPPORT_BODYSENSORLOCATION			0
#endif //BLE_HEARTRATE_SERVICE_SUPPORT_BODYSENSORLOCATION

/**
 * BLE_HEARTRATE_SERVICE_SUPPORT_ENERGYEXPENDED
 * Define if the local Heart Rate SENSOR supports the Energy expended 
 * calculations.
 *
 * if BLE_HEARTRATE_SERVICE_SUPPORT_ENERGYEXPENDED is enabled (1), the 
 * local Heart Rate SENSOR supports the Energy expended 
 * calculations. The Energy Expended value may be added to the Heart Rate
 * measurement via the isEnergyExpendedIncluded and EnergyExpendedValue
 * field of the BleHeartRateServiceMeasurement set during the
 * BLEHEARTRATE_SetHeartRateMeasurement API call.
 * As well, when Energy Expended feature is supported, the remote COLLECTOR
 * may request to reset the current Energy Expended value count. This value
 * reset request is notified to the application thanks to the 
 * BLEHEARTRATESERVICE_EVENT_ENERGYEXPENDEDRESETREQUEST event.
 *
 * The default value for this option is disabled (0).
 *
 */
#ifndef BLE_HEARTRATE_SERVICE_SUPPORT_ENERGYEXPENDED
#define BLE_HEARTRATE_SERVICE_SUPPORT_ENERGYEXPENDED				0
#endif //BLE_HEARTRATE_SERVICE_SUPPORT_ENERGYEXPENDED

/***************************************************************************\
 * CONSTANTS
\***************************************************************************/
/* BLEINFOTYPE_HEARTRATESERVICE_CLIENTCONFIG 
 * The Unique descriptor ID for a two Bytes length information to save
 * in persistent memory representing the client config for a given remote
 * bonded device.
 * It is the Unique identifier passed in the infotype fields when the PROFILE
 * call the system APIs SYSTEM_SetPersistentInformation(BD_ADDR addr,
 * U8 infoType, U8* infoValue,U8 InfoLen) and 
 * SYSTEM_GetPersistentInformation(BD_ADDR addr, U8 infoType, U8 **infoValue,
 * U8 *InfoLen). 
 */
#define BLEINFOTYPE_HEARTRATESERVICE_CLIENTCONFIG						0x8C	



/***************************************************************************\
 *	Type definition
\***************************************************************************/

/** 
 * BleHeartRateBodySensorLocation type
 * The Body Sensor Location of the device is used to describe the intended 
 *  location of the heart rate measurement for the device.  
 * This body sensor location is set using the 
 * BLEHEARTRATE_SetBodySensorLocation() API.
 */
typedef U8 BleHeartRateBodySensorLocation;

#define BLEHEARTRATE_SENSOR_LOCATION_OTHER								0
#define BLEHEARTRATE_SENSOR_LOCATION_CHEST								1
#define BLEHEARTRATE_SENSOR_LOCATION_WRIST								2
#define BLEHEARTRATE_SENSOR_LOCATION_FINGER								3
#define BLEHEARTRATE_SENSOR_LOCATION_HAND								4
#define BLEHEARTRATE_SENSOR_LOCATION_EARLOBE							5
#define BLEHEARTRATE_SENSOR_LOCATION_FOOT								6

/** 
 * BleHeartRateSensorContactStatus type
 * The Sensor Contact Status bits indicate whether or not, 
 * the Sensor Contact feature is supported and if supported whether or not
 * skin contact is detected. 
 * Note than the support of the contact status must not be change while 
 * connected (from BLEHEARTRATE_SENSOR_CONTACT_STATUS_NOTSUPPORTED to 
 * BLEHEARTRATE_SENSOR_CONTACT_STATUS_DETECTED	or 
 * BLEHEARTRATE_SENSOR_CONTACT_STATUS_NOT_DETECTED or vice -versa)
 */
typedef U8 BleHeartRateSensorContactStatus;
#define BLEHEARTRATE_SENSOR_CONTACT_STATUS_NOTSUPPORTED					0
#define BLEHEARTRATE_SENSOR_CONTACT_STATUS_DETECTED						1
#define BLEHEARTRATE_SENSOR_CONTACT_STATUS_NOT_DETECTED					2

/** 
 * BleHeartRateSensorEnergyExpendedSupport type
 * Defines if the energy expended value is included in the heart Rate 
 * mesurement (BLEHEARTRATE_SENSOR_ENERGY_EXPENDED_INCLUDED) or not 
 * (BLEHEARTRATE_SENSOR_ENERGY_EXPENDED_NOT_INCLUDED)
 * This body sensor location is set for each measurement using the 
 * BLEHEARTRATE_SetHeartRateMeasurement() API.
 */
typedef U8 BleHeartRateSensorEnergyExpendedSupport;
#define BLEHEARTRATE_SENSOR_ENERGY_EXPENDED_NOT_INCLUDED				0
#define BLEHEARTRATE_SENSOR_ENERGY_EXPENDED_INCLUDED					1


/** 
 * BleHeartRateServiceMeasurement type
 * Define the current Heart Rate Measurement value to be set during the 
 * call of BLEHEARTRATE_SetHeartRateMeasurement
 */
typedef struct{
	/*
	 * the heart Rate measurement Value, in beats per minute (bpm)
	 */	
	U16										heartRateValue;

	/*
	 * define the current status of the sensor contact, one of the 
	 * BLEHEARTRATE_SENSOR_CONTACT_STATUS_ available value.
	 */
	BleHeartRateSensorContactStatus			sensorContactStatus;

#if ( BLE_HEARTRATE_SERVICE_SUPPORT_ENERGYEXPENDED == 1 )
	/* Define if the energy expended is included or not in to the 
	 * measurement.
	 * If energy expended is not used or not know, the value must set to 
	 * BLEHEARTRATE_SENSOR_ENERGY_EXPENDED_NOT_INCLUDED,
	 * the energyExpendedValue will be ignored and not be sent to the
	 * remote device.
	 * If energy expended is used and know, the value must be set to 
	 * BLEHEARTRATE_SENSOR_ENERGY_EXPENDED_INCLUDED,
	 * the energyExpendedValue will be sent to the
	 * remote device.
	 */
	BleHeartRateSensorEnergyExpendedSupport	isEnergyExpendedIncluded;

	/*
	 * The Energy Expended field represents the accumulated energy expended 
	 * in kilo Joules since the last time it was reset. 
	 *
	 * If energy expended is used, it is typically only included in the heart
	 * Rate Measurement characteristic once every 10 measurements at a regular
	 * interval.
	 *
 	 * Since Energy Expended is a UINT16, the highest value that can be 
	 * represented is 65535 kilo Joules. If the maximum value of 65535 kilo
	 * Joules is attained (0xFFFF), the field value should remain at 0xFFFF
	 * so that the client can be made aware that a reset of the Energy
	 * Expended Field is required.
	 *
	 */
	U16										energyExpendedValue;						
#endif //( BLE_HEARTRATE_SERVICE_SUPPORT_ENERGYEXPENDED == 1 )

	/*
	 * Define the number of RR interval included in this measurement,
	 * It can be zero, if the server is only interested to send the current
	 * heart Rate measurement Value beats per minutes.
	 * 
	 * The maximum number of interval included in a single 
	 * BleHeartRateMeasurement is dependant of the value of the heartRateValue
	 * and if energy expended is used or not;
	 * the maximum number is the following:
	 * if heartRateValue <= 255 and energyExpended=0 maximum is 9
	 * if heartRateValue <= 255 and energyExpended=1 maximum is 8
	 * if heartRateValue > 255 and energyExpended=0 maximum is 8
	 * if heartRateValue > 255 and energyExpended=1 maximum is 7 
	 */
	U8										numberOfRRIntervals;

	/*
	 * A pointer to an Array of RRIntervals, the number of valid RRIntervals 
	 * is numberOfRRIntervals.
	 * This field is ignored and can be (U16 *)0 if numberOfRRIntervals is 0
	 * the RRInterval units is 1/1024 seconds
	 */
	U16										*RRIntervals;

} BleHeartRateServiceMeasurement;

/** 
 * BleHeartRateServiceEvent type
 * Define the different kind of events that could be received by the 
 * BleHeartRateServiceCallback
 */
typedef U8 BleHeartRateServiceEvent;

/** BLEHEARTRATESERVICE_EVENT_ENERGYEXPENDEDRESETREQUEST
 * Event received when a remote device would like to request the application
 * to reset its current Energy Expended value.
 *
 * Only received if BLE_HEARTRATE_SERVICE_SUPPORT_ENERGYEXPENDED is enabled.
 * The status field is always BLESTATUS_SUCCESS
 * The parms field is null (void *)0 and not applicable.
 */
#define BLEHEARTRATESERVICE_EVENT_ENERGYEXPENDEDRESETREQUEST		0xDE

/**
 * BleHeartRateServiceCallback
 *	This callback receives the Heart Rate services events. 
 *  Each of these events can be associated with a defined status and 
 *  parameters.
 *	The callback is executed during the stack context,  be careful to not  
 *	running heavy process in this function.
 */
typedef void (*BleHeartRateServiceCallback)(
						BleHeartRateServiceEvent event,
						BleStatus status,
						void *parms);

/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
/** Set the current Heart Rate Measurement 
 *
 * BLEHEARTRATE_SetHeartRateMeasurement()
 *	This function is used to set the current Heart Rate Measurement 
 * characteristic value
 *
 * BLE_SUPPORT_HEARTRATE_SERVICE shall be enabled.
 *
 * @param heartRateMeasurement : the structure representing the Heart Rate 
 *		Measurement, see the field description of 
 *		BleHeartRateServiceMeasurement for more detailled information.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed. It can be 
 *   issued because the number of given RRInterval exceeds the maximum 
 *	 allowed number of intervals
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function has failed because
 *		an invalid parameter.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEHEARTRATE_SetHeartRateMeasurement(
				BleHeartRateServiceMeasurement *heartRateMeasurement);


#if (BLE_HEARTRATE_SERVICE_SUPPORT_BODYSENSORLOCATION == 1)

/** Set the current current body sensor location
 *
 * BLEHEARTRATE_SetBodySensorLocation()
 *	This function is used to set the current body sensor location.
 * Note that this function is not called by the application, the default Heart 
 *  Rate Measurement Sensor location is BLEHEARTRATE_SENSOR_LOCATION_OTHER.
 *
 * BLE_SUPPORT_HEARTRATE_SERVICE shall be enabled.
 * BLE_HEARTRATE_SERVICE_SUPPORT_BODYSENSORLOCATION shall be enabled.
 *
 * @param bodySensorLocation : the body location of the Sensor, one of the
 *		available BLEHEARTRATE_SENSOR_LOCATION_* defined value
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed. It can be 
 *		issued because this device is connected. the specification says that 
 *      the Body Sensor Location is considered to be the same over a
 *      connection.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function has failed because
 *		an invalid parameter.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEHEARTRATE_SetBodySensorLocation(
						BleHeartRateBodySensorLocation bodySensorLocation);

#endif //(BLE_HEARTRATE_SERVICE_SUPPORT_BODYSENSORLOCATION == 1)
#endif //(BLE_SUPPORT_HEARTRATE_SERVICE == 1)

#endif //__BLEHEARTRATE_SERVICE_H
