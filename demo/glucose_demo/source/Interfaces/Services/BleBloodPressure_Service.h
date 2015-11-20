#ifndef __BLEBLOODPRESSURE_SERVICE_H
#define __BLEBLOODPRESSURE_SERVICE_H
/****************************************************************************
 *
 * File:          BleBloodPressure_Service.h
 *
 * Description:   Contains routines declaration for Blood Pressure Service.
 * 
 * Created:       August, 2012
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
 * BLE_SUPPORT_BLOODPRESSURE_SERVICE
*	Defines if the BLE local device enables access to the local  
 *	Blood Pressure service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles to include the  
 * Blood Pressure Service and characteristic into the local device attribute
 * database.
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_BLOODPRESSURE_SERVICE
#define BLE_SUPPORT_BLOODPRESSURE_SERVICE									0
#endif //BLE_SUPPORT_BLOODPRESSURE_SERVICE

#if (BLE_SUPPORT_BLOODPRESSURE_SERVICE == 1)
/**
 * BLE_BLOODPRESSURE_SUPPORT_TIMESTAMP
 *  The Time Stamp field shall be included in the Blood Pressure Measurement
 * characteristic if the device supports storing of data. Otherwise it is
 * optional. 
 * If a time stamp is supported, the sensor includes the Time Stamp in 
 * the Blood Pressure Measurement characteristic. 
 * The value of the Time Stamp field is derived from a source of date and time at
 * the time of measurement. If the Time Stamp feature is supported, a source
 * of date and time is mandatory. 
 *
 * if BLE_BLOODPRESSURE_SUPPORT_TIMESTAMP is enabled (1), the 
 * BleBloodPressureMeasurement structure passed to 
 * BLEBLOODPRESSURESERVICE_SetBloodPressureMeasurement contains the TimeStamp field.
 *
 * The default value for this option is disabled (0).
 *
 */
#ifndef BLE_BLOODPRESSURE_SUPPORT_TIMESTAMP
#define BLE_BLOODPRESSURE_SUPPORT_TIMESTAMP								0
#endif //BLE_BLOODPRESSURE_SUPPORT_TIMESTAMP


/***************************************************************************\
 * CONSTANTS
\***************************************************************************/
/* BLEINFOTYPE_BLOODPRESSURESERVICE_CLIENTCONFIG 
 * The Unique descriptor ID for a two Bytes length information to save
 * in persistent memory representing the client config for a given remote
 * bonded device.
 * It is the Unique identifier passed in the infotype fields when the PROFILE
 * call the system APIs SYSTEM_SetPersistentInformation(BD_ADDR addr,
 * U8 infoType, U8* infoValue,U8 InfoLen) and 
 * SYSTEM_GetPersistentInformation(BD_ADDR addr, U8 infoType, U8 **infoValue,
 * U8 *InfoLen). 
 */
#define BLEINFOTYPE_BLOODPRESSURESERVICE_CLIENTCONFIG					0x56	

/***************************************************************************\
 *	Type definition
\***************************************************************************/

/** 
 * BleBloodPressureSensorPulseRateSupport type
 * Defines if the pulse rate value is included in the blood pressure
 * measurement (BLEBLOODPRESSURE_SENSOR_PULSE_RATE_INCLUDED) or not 
 * (BLEBLOODPRESSURE_SENSOR_PULSE_RATE_NOT_INCLUDED)
 * This pulse rate is set for each measurement using the 
 * BLEBLOODPRESSURESERVICE_SetBloodPressureMeasurement() API.
 */
typedef U8 BleBloodPressureSensorPulseRateSupport;
#define BLEBLOODPRESSURE_SENSOR_PULSE_RATE_NOT_INCLUDED						0
#define BLEBLOODPRESSURE_SENSOR_PULSE_RATE_INCLUDED							1

/** 
 * BleBloodPressureSensorMeasurementStatusSupport type
 * Defines if the measurement status value is included in the blood pressure
 * measurement (BLEBLOODPRESSURE_SENSOR_MEASUREMENT_STATUS_INCLUDED) or not 
 * (BLEBLOODPRESSURE_SENSOR_MEASUREMENT_STATUS_NOT_INCLUDED)
 * This measurement status is set for each measurement using the 
 * BLEBLOODPRESSURESERVICE_SetBloodPressureMeasurement() API.
 */
typedef U8 BleBloodPressureSensorMeasurementStatusSupport;
#define BLEBLOODPRESSURE_SENSOR_MEASUREMENT_STATUS_NOT_INCLUDED				0
#define BLEBLOODPRESSURE_SENSOR_MEASUREMENT_STATUS_INCLUDED					1

/** 
 * BleBloodPressureSensorPulseRateRangeDetection type
 * The Pulse Rate Range Detection bits indicate if
 * the Pulse Rate is too high, too low, or in the average
 * Note than the support of the pulse rate must not be changed while 
 * connected (from BLEBLOODPRESSURE_SENSOR_PULSE_RATE_NOT_INCLUDED to 
 * BLEBLOODPRESSURE_SENSOR_PULSE_RATE_INCLUDED or vice -versa)
 */
typedef U8 BleBloodPressureSensorPulseRateRangeDetection;
#define BLEBLOODPRESSURE_SENSOR_PULSE_RATE_RANGE_DETECTION_WITHINTHERANGE		0
#define BLEBLOODPRESSURE_SENSOR_PULSE_RATE_RANGE_DETECTION_EXCEEDUPPERLIMIT		1
#define BLEBLOODPRESSURE_SENSOR_PULSE_RATE_RANGE_DETECTION_LESSTHANLOWERLIMIT	2

typedef	U16	BleBloodPressureFeatureBitfield;
#define BLEBLOODPRESSURESERVICE_FEATUREBITMASK_BODYMOVEMENT				(0x0001u)
#define BLEBLOODPRESSURESERVICE_FEATUREBITMASK_CUFFFIT					(0x0002u)
#define BLEBLOODPRESSURESERVICE_FEATUREBITMASK_IRREGULARPULSE			(0x0004u)
#define BLEBLOODPRESSURESERVICE_FEATUREBITMASK_PULSERATERANGE			(0x0008u)
#define BLEBLOODPRESSURESERVICE_FEATUREBITMASK_MEASUREMENTPOSITION		(0x0010u)
#define BLEBLOODPRESSURESERVICE_FEATUREBITMASK_MULTIPLEBOND				(0x0020u)


#ifndef BLE_BLOODPRESSURE_BLOODPRESSUREFEATURE						
#define BLE_BLOODPRESSURE_BLOODPRESSUREFEATURE			BLEBLOODPRESSURESERVICE_FEATUREBITMASK_BODYMOVEMENT	
#endif //BLE_BLOODPRESSURE_BLOODPRESSUREFEATURE

typedef union {

	U16 measurementStatusValue;
	struct 
	{
		unsigned int bodyDetection:1;
		unsigned int cuff:1;
		unsigned int irregularPulse:1;
		unsigned int pulseRateRange:2;
		unsigned int measurementPositionDetection:1;
		//unsigned int reserved:11;
	} fields;

} BleBloodPressureMeasurementStatus;

/** 
 * BleBloodPressureServiceMeasurement type
 * Define the current Blood Pressure Measurement value to be set during the 
 * call of BLEBLOODPRESSURESERVICE_SetBloodPressureMeasurement
 */
typedef struct{
	U8													flagValue ;
	U8													unitFieldValue ;
	/*
	 * the Blood Pressure measurement Value, in mmHg or kPa
	 */	
	U16													compoundValueSystolic;
	U16													compoundValueDiastolic;
	U16													compoundValueMeanArterialPressure;

#if (BLE_BLOODPRESSURE_SUPPORT_TIMESTAMP == 1)
	/** If the Blood Pressure Measurement TimeStamp value is supported, it shall  
	 *  be provided and valid during each call of 
	 *	BLEBLOODPRESSURESERVICE_SetBloodPressureMeasurement
	 */
	BleBloodPressureTimeStamp							timeStampValue;
#endif //(BLE_BLOODPRESSURE_SUPPORT_TIMESTAMP == 1)


#if (BLE_BLOODPRESSURE_SUPPORT_PULSE_RATE == 1)
	/* Define if the pulse rate is included or not in to the 
	 * measurement.
	 * If pulse rate is not included in the current measurement the value 
	 * is set to BLEBLOODPRESSURE_COLLECTOR_PULSE_RATE_NOT_INCLUDED,
	 * the pulseRateValue field is not valid and does not contain  
	 * a valid pulse Rate Value, it must be ignored
	 * If energy expended is included in the current measurement the value 
	 * is set to BLEBLOODPRESSURE_COLLECTOR_PULSE_RATE_INCLUDED,
	 * the pulseRateValue is valid and contains a valid pulse Rate
	 * Value
	 */
	BleBloodPressureSensorPulseRateSupport				isPulseRateIncluded;
	
	/*
	 * the pulse Rate measurement Value, in beats per minute (bpm)
	 */
	U16													pulseRateValue;
#endif //(BLE_BLOODPRESSURE_SUPPORT_PULSE_RATE == 1)

#if (BLE_BLOODPRESSURE_SUPPORT_MEASUREMENT_STATUS == 1)
	/* Define if the measurement status is included or not in to the 
	 * measurement.
	 * If measurement status is not included in the current measurement the value 
	 * is set to BLEBLOODPRESSURE_COLLECTOR_MEASUREMENT_STATUS_NOT_INCLUDED,
	 * the measurementStatusValue field is not valid and does not contain  
	 * a valid measurement status Value, it must be ignored
	 * If measurement status is included in the current measurement the value 
	 * is set to BLEBLOODPRESSURE_COLLECTOR_MEASUREMENT_STATUS_INCLUDED,
	 * the measurementStatusValue is valid and contains a valid measurement status
	 * Value
	 */
	BleBloodPressureSensorMeasurementStatusSupport		isMeasurementStatusIncluded;
	/*
	* The measurement status field is a bitfield representing the blood pressure
	* measurement flags.
	*
	*/
	BleBloodPressureMeasurementStatus					measurementStatusValue;
#endif //(BLE_BLOODPRESSURE_SUPPORT_MEASUREMENT_STATUS== 1)


} BleBloodPressureServiceMeasurement;



/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
/** Set the current Blood Pressure Measurement 
 *
 * BLEBLOODPRESSURESERVICE_SetBloodPressureMeasurement()
 *	This function is used to set the current Blood Pressure Measurement 
 * characteristic value
 *
 * BLE_SUPPORT_BLOODPRESSURE_SERVICE shall be enabled.
 *
 * @param bloodPressureMeasurement : the structure representing the Blood  
 *		Pressure Measurement, see the field description of 
 *		BleBloodPressureServiceMeasurement for more detailled information.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed. It can be 
 *   issued because the number of given RRInterval exceed the maximum 
 *	 allowed number of intervals
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function has failed because
 *		an invalid parameter.
 *
 * @author Mahault ANDRIA
 */
BleStatus BLEBLOODPRESSURESERVICE_SetBloodPressureMeasurement(
	BleBloodPressureServiceMeasurement *bloodPressureMeasurement);


/** Get the Blood Pressure Feature 
 *
 * BLEBLOODPRESSURESERVICE_GetBloodPressureFeature()
 *	This function is used to get the current Blood Pressure Feature 
 * characteristic value
 *
 * BLE_SUPPORT_BLOODPRESSURE_SERVICE shall be enabled.
 * @param void
 *
 * @return BleBloodPressureFeatureBitfield :
 *	The bitfield of Blood Pressure Feature
 *
 * @author Mahault ANDRIA
 */
BleBloodPressureFeatureBitfield BLEBLOODPRESSURESERVICE_GetBloodPressureFeature( void );
#define BLEBLOODPRESSURESERVICE_GetBloodPressureFeature( s ) BLE_BLOODPRESSURE_BLOODPRESSUREFEATURE

#endif //(BLE_SUPPORT_BLOODPRESSURE_SERVICE == 1)

#endif //__BLEBLOODPRESSURE_SERVICE_H
