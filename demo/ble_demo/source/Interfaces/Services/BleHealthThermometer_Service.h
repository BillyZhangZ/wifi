#ifndef __BLEHEALTHTHERMOMETER_SERVICE_H
#define __BLEHEALTHTHERMOMETER_SERVICE_H
/****************************************************************************
 *
 * File:          BleHealthThermometer_Service.h
 *
 * Description:   Contains routines declaration for Health Thermometer Service.
 * 
 * Created:       march, 2011
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
 * BLE_SUPPORT_HEALTHTHERMOMETER_SERVICE
*	Defines if the BLE local device enables access to the local  
 *	Health Thermometer service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles to get access to the  
 * Health Thermometer Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_HEALTHTHERMOMETER_SERVICE
#define BLE_SUPPORT_HEALTHTHERMOMETER_SERVICE								0
#endif //BLE_SUPPORT_HEALTHTHERMOMETER_SERVICE

/**
 * BLE_HEALTHTHERMOMETER_SUPPORT_TEMPERATURETYPE
 *  The temperature type describes the type of temperature measurement in
 * relation to the location on the human body at which the temperature was 
 * measured. 
 * If The temperature type if supported, it can be populated to the collector
 * using two exclusive methods (staticaly -default- or dynamicaly over a 
 *  connection).
 *
 * The default value for this option is disabled (0).
 *
 */
#ifndef BLE_HEALTHTHERMOMETER_SUPPORT_TEMPERATURETYPE
#define BLE_HEALTHTHERMOMETER_SUPPORT_TEMPERATURETYPE						0
#endif //BLE_HEALTHTHERMOMETER_SUPPORT_TEMPERATURETYPE

/**
 * BLE_HEALTHTHERMOMETER_STATIC_TEMPERATURETYPE
 *  
 * Defines if the Temperature type is static over the temperature measurement
 *	(BLE_HEALTHTHERMOMETER_STATIC_TEMPERATURETYPE set to 1) or not 
 * (BLE_HEALTHTHERMOMETER_STATIC_TEMPERATURETYPE set to 0).
 *
 * There are two exclusive methods to enable a Thermometer to provide
 * temperature type information to a Collector. Either one method or the other
 * is used, but not both.
 * The BLE_HEALTHTHERMOMETER_STATIC_TEMPERATURETYPE is intended to  be enabled
 * when the value is static while in a connection. For temperature type values
 * that are non-static while in a connection (e.g. configured using a switch or
 * a simple user interface on a thermometer), the Temperature Type field in the
 * Temperature Measurement characteristic (refer to Section 3.1) shall be 
 * provided, so it will add the measurement type within each temperature
 * mesurement.
 *
 * if BLE_HEALTHTHERMOMETER_STATIC_TEMPERATURETYPE is enabled (1), the 
 * BLEHEALTHTHERMOMETER_SetMeasurementType API will be enabled to 
 * allow the application to change the type when not connected.
 *
 * If BLE_HEALTHTHERMOMETER_STATIC_TEMPERATURETYPE is disabled (0), the 
 * BleHealthThermometerTemperature structure passed to 
 * BLEHEALTHTHERMOMETER_SetTemperature contains the measurementType field.
 *  
 * If the thermometer is for general use, a value of 0x02 Body (general) may be used.
 *	
 * The default value for this option is enabled (1).
 */
#ifndef BLE_HEALTHTHERMOMETER_STATIC_TEMPERATURETYPE
#define BLE_HEALTHTHERMOMETER_STATIC_TEMPERATURETYPE						1
#endif //BLE_HEALTHTHERMOMETER_STATIC_TEMPERATURETYPE


/**
 * BLE_HEALTHTHERMOMETER_SUPPORT_TIMESTAMP
 *  The Time Stamp field shall be included in the Temperature Measurement
 * characteristic if the device supports storing of data. Otherwise it is
 * optional. 
 * If a time stamp is supported, the thermometer includes the Time Stamp in 
 * the Temperature Measurement characteristic. 
 * The value of the Time Stamp field is derived from a source of date and time at
 * the time of measurement. If the Time Stamp feature is supported, a source
 * of date and time is mandatory. 
 *
 * if BLE_HEALTHTHERMOMETER_SUPPORT_TIMESTAMP is enabled (1), the 
 * BleHealthThermometerTemperature structure passed to 
 * BLEHEALTHTHERMOMETER_SetTemperature contains the TimeStamp field.
 *
 * The default value for this option is disabled (0).
 *
 */
#ifndef BLE_HEALTHTHERMOMETER_SUPPORT_TIMESTAMP
#define BLE_HEALTHTHERMOMETER_SUPPORT_TIMESTAMP								0
#endif //BLE_HEALTHTHERMOMETER_SUPPORT_TIMESTAMP

/***************************************************************************\
 * CONSTANTS
\***************************************************************************/
/* BLEINFOTYPE_HEALTHTHERMOMETER_CLIENTCONFIG 
 * The Unique descriptor ID for a two Bytes length information to save
 * in persistent memory representing the client config for a given remote
 * bonded device.
 * It is the Unique identifier passed in the infotype fields when the PROFILE
 * call the system APIs SYSTEM_SetPersistentInformation(BD_ADDR addr,
 * U8 infoType, U8* infoValue,U8 InfoLen) and 
 * SYSTEM_GetPersistentInformation(BD_ADDR addr, U8 infoType, U8 **infoValue,
 * U8 *InfoLen). 
 */
#define BLEINFOTYPE_HEALTHTHERMOMETER_CLIENTCONFIG						0x50	



/***************************************************************************\
 *	Type definition
\***************************************************************************/

/** 
 * BleHealthThermometerTemperatureType type
 * Define the different kind of temperature type that could be set in the
 * BLEHEALTHTHERMOMETER_SetTemperature or 
 * BLEHEALTHTHERMOMETER_SetTemperatureType API it is one of the 
 * BLEHEALTHTHERMOMETER_TYPE_* defined below
 */
typedef U8	BleHealthThermometerTemperatureType;

#define BLEHEALTHTHERMOMETER_TYPE_ARMPIT			0x01
#define BLEHEALTHTHERMOMETER_TYPE_BODY				0x02
#define BLEHEALTHTHERMOMETER_TYPE_EAR				0x03
#define BLEHEALTHTHERMOMETER_TYPE_FINGER			0x04
#define BLEHEALTHTHERMOMETER_TYPE_GASTROINTESTINAL	0x05
#define BLEHEALTHTHERMOMETER_TYPE_MOUTH				0x06
#define BLEHEALTHTHERMOMETER_TYPE_RECTUM			0x07
#define BLEHEALTHTHERMOMETER_TYPE_TOE				0x08
#define BLEHEALTHTHERMOMETER_TYPE_TYMPANUM			0x09

/** 
 * BleHealthThermometerTemperature type
 * Define the current temperature value to be set during the call of
 * BLEHEALTHTHERMOMETER_SetTemperature
 */
typedef struct{
	/** the temperature measurement in the float format
	 * defined in the IEEE 11073-20601
	 * The Temperature Measurement Value field may contain special float value
	 * Not a Number (NaN)(0x007FFFFF) defined in IEEE 11073-20601 [4] to 
	 * report an invalid result from a computation step or missing data due to
	 * the hardware’s inability to provide a valid measurement. 
	 */
	U32									temperatureMeasurement;

#if (BLE_HEALTHTHERMOMETER_SUPPORT_TEMPERATURETYPE == 1)
#if (BLE_HEALTHTHERMOMETER_STATIC_TEMPERATURETYPE == 0)
	/** If the Temperature Type value is supported and is non-static, it shall 
	 * be provided and valid ( one of the  BLEHEALTHTHERMOMETER_TYPE_* ) 
	 * during each call of BLEHEALTHTHERMOMETER_SetTemperature
	 */
	BleHealthThermometerTemperatureType temperatureType;
#endif //(BLE_HEALTHTHERMOMETER_STATIC_TEMPERATURETYPE == 0)
#endif //(BLE_HEALTHTHERMOMETER_SUPPORT_TEMPERATURETYPE == 1)

#if (BLE_HEALTHTHERMOMETER_SUPPORT_TIMESTAMP == 1)
	/** If the Temperature TimeStamp value is supported, it shall be provided
	 * and valid during each call of BLEHEALTHTHERMOMETER_SetTemperature
	 */
	BleHealthThermometerTimeStamp		temperatureTimeStamp;
#endif //(BLE_HEALTHTHERMOMETER_SUPPORT_TIMESTAMP == 1)
} BleHealthThermometerTemperature;

/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
/** Set the current temperature
 *
 * BLEHEALTHTHERMOMETER_SetTemperature()
 *	This function is used to set the current temperature characteristic
 * value
 *
 * BLE_SUPPORT_HEALTHTHERMOMETER_SERVICE shall be enabled.
 *
 * @param temperature : the structure representing the temperature measurement
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
BleStatus BLEHEALTHTHERMOMETER_SetTemperature(
						BleHealthThermometerTemperature *temperature);


#if (BLE_HEALTHTHERMOMETER_SUPPORT_TEMPERATURETYPE == 1)
#if (BLE_HEALTHTHERMOMETER_STATIC_TEMPERATURETYPE == 1)
/** Set the current static temperature measurement type
 *
 * BLEHEALTHTHERMOMETER_SetTemperatureType()
 *	This function is used to set the current temperature measurement type
 * value.
 * Note that this function is not called the default temperature type is 
 * BLEHEALTHTHERMOMETER_TYPE_BODY.
 *
 * BLE_SUPPORT_HEALTHTHERMOMETER_SERVICE shall be enabled.
 * BLE_HEALTHTHERMOMETER_SUPPORT_TEMPERATURETYPE shall be enabled.
 * BLE_HEALTHTHERMOMETER_STATIC_TEMPERATURETYPE shall be enabled.
 *
 * @param temperatureType : the temperature measurement type for this 
 *					thermometer
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
BleStatus BLEHEALTHTHERMOMETER_SetTemperatureType(
						BleHealthThermometerTemperatureType temperatureType);

#endif //(BLE_HEALTHTHERMOMETER_STATIC_TEMPERATURETYPE == 1)
#endif //(BLE_HEALTHTHERMOMETER_SUPPORT_TEMPERATURETYPE == 1)

#endif //__BLEHEALTHTHERMOMETER_SERVICE_H
