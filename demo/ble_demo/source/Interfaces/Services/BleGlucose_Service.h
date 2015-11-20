#ifndef __BLEGLUCOSE_SERVICE_H
#define __BLEGLUCOSE_SERVICE_H
/****************************************************************************\
 *
 * File:          BleGlucose_Service.h
 *
 * Description:   Contains routines declaration for Glucose Service.
 * 
 * Created:       April, 2013
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.3 $
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
#include "BleTimeTypes.h"


/****************************************************************************\
 * OPTIONAL FEATURES
\****************************************************************************/
/**
 * BLE_SUPPORT_GLUCOSE_SERVICE
 *	Defines if the BLE local device enables access to the local  
 *	Glucose service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles or directle an Aplication to 
 * get access to the Glucose Service and characteristic into the local device
 * attribute database.
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_GLUCOSE_SERVICE
#define BLE_SUPPORT_GLUCOSE_SERVICE									                    0   
#endif //BLE_SUPPORT_GLUCOSE_SERVICE

#if (BLE_SUPPORT_GLUCOSE_SERVICE == 1)
/**
 * BLE_GLUCOSESERVICE_SUPPORT_TIMEOFFSET_MEASUREMENT
 * The Time Offset field shall be included in the Glucose Measurement 
 * characteristic value whenever the value of the Time Offset changes from the
 * last reported measurement. Otherwise, it may be included even if the value
 * does not change from patient record to patient record.
 * The value of the Time Offset field is set by the application.
 *
 * If BLE_GLUCOSESERVICE_SUPPORT_TIMEOFFSET_MEASUREMENT is enabled (1), the 
 * BleGlucoseMeasurement structure passed to 
 * BLEGLUCOSESERVICE_SetGlucoseMeasurement contains the Time Offset field.
 *
 * The default value for this option is disabled (0).
 *
 */
#ifndef BLE_GLUCOSESERVICE_SUPPORT_TIMEOFFSET_MEASUREMENT
#define BLE_GLUCOSESERVICE_SUPPORT_TIMEOFFSET_MEASUREMENT					            0
#endif //BLE_GLUCOSESERVICE_SUPPORT_TIMEOFFSET_MEASUREMENT

/**
 * BLE_GLUCOSESERVICE_SUPPORT_CONCENTRATION_MEASUREMENT
 * The Concentration, Type and Location Sample fields shall be included in the
 * Glucose Measurement characteristic if the device supports concentration 
 * measurement. Otherwise it is optional. 
 * The value of the Concentration field, Type and Location Sample is 
 * configured by the user.
 *
 * If BLE_GLUCOSESERVICE_SUPPORT_CONCENTRATION_MEASUREMENT is enabled (1), the 
 * BleGlucoseMeasurement structure passed to 
 * BLEGLUCOSESERVICE_SetGlucoseMeasurement contains the Concentration, Type 
 * and Location Sample fields.
 *
 * The default value for this option is disabled (0).
 *
 */
#ifndef BLE_GLUCOSESERVICE_SUPPORT_CONCENTRATION_MEASUREMENT
#define BLE_GLUCOSESERVICE_SUPPORT_CONCENTRATION_MEASUREMENT		                    0
#endif //BLE_GLUCOSESERVICE_SUPPORT_CONCENTRATION_MEASUREMENT

/**
 * BLE_GLUCOSESERVICE_SUPPORT_XXXXXXX_FEATURE
 * Boolean indicating if feature is supported. If at least one feature is supported
 * BLE_GLUCOSESERVICE_SUPPORT_SENSOR_STATUS_ANNUNCIATION_MEASUREMENT will be set to 1
 * and Sensor Status Annunciation field shall be included in the Glucose 
 * Measurement characteristic
 *
 * The default value for this option is disabled (0).
 *
 */
#ifndef BLE_GLUCOSESERVICE_SUPPORT_LOWBATTERYDETECTIONDURINGMEASUREMENT_FEATURE
#define BLE_GLUCOSESERVICE_SUPPORT_LOWBATTERYDETECTIONDURINGMEASUREMENT_FEATURE		    0
#endif //BLE_GLUCOSESERVICE_SUPPORT_LOWBATTERYDETECTIONDURINGMEASUREMENT_FEATURE

#ifndef BLE_GLUCOSESERVICE_SUPPORT_SENSORMALFUNCTIONDETECTION_FEATURE
#define BLE_GLUCOSESERVICE_SUPPORT_SENSORMALFUNCTIONDETECTION_FEATURE		            0
#endif //BLE_GLUCOSESERVICE_SUPPORT_SENSORMALFUNCTIONDETECTION_FEATURE

#ifndef BLE_GLUCOSESERVICE_SUPPORT_SENSORSAMPLESIZE_FEATURE
#define BLE_GLUCOSESERVICE_SUPPORT_SENSORSAMPLESIZE_FEATURE		                        0
#endif //BLE_GLUCOSESERVICE_SUPPORT_SENSORSAMPLESIZE_FEATURE

#ifndef BLE_GLUCOSESERVICE_SUPPORT_SENSORSTRIPINSERTIONERRORDETECTION_FEATURE
#define BLE_GLUCOSESERVICE_SUPPORT_SENSORSTRIPINSERTIONERRORDETECTION_FEATURE		    0
#endif //BLE_GLUCOSESERVICE_SUPPORT_SENSORSTRIPINSERTIONERRORDETECTION_FEATURE

#ifndef BLE_GLUCOSESERVICE_SUPPORT_SENSORSTRIPTYPEERRORDETECTION_FEATURE
#define BLE_GLUCOSESERVICE_SUPPORT_SENSORSTRIPTYPEERRORDETECTION_FEATURE		        0
#endif //BLE_GLUCOSESERVICE_SUPPORT_SENSORSTRIPTYPEERRORDETECTION_FEATURE

#ifndef BLE_GLUCOSESERVICE_SUPPORT_SENSORRESULTHIGHLOWDETECTION_FEATURE
#define BLE_GLUCOSESERVICE_SUPPORT_SENSORRESULTHIGHLOWDETECTION_FEATURE		            0
#endif //BLE_GLUCOSESERVICE_SUPPORT_SENSORRESULTHIGHLOWDETECTION_FEATURE

#ifndef BLE_GLUCOSESERVICE_SUPPORT_SENSORTEMPERATUREHIGHLOWDETECTION_FEATURE
#define BLE_GLUCOSESERVICE_SUPPORT_SENSORTEMPERATUREHIGHLOWDETECTION_FEATURE		    0
#endif //BLE_GLUCOSESERVICE_SUPPORT_SENSORTEMPERATUREHIGHLOWDETECTION_FEATURE

#ifndef BLE_GLUCOSESERVICE_SUPPORT_SENSORREADINTERRUPTDETECTION_FEATURE
#define BLE_GLUCOSESERVICE_SUPPORT_SENSORREADINTERRUPTDETECTION_FEATURE		            0
#endif //BLE_GLUCOSESERVICE_SUPPORT_SENSORREADINTERRUPTDETECTION_FEATURE

#ifndef BLE_GLUCOSESERVICE_SUPPORT_GENERALDEVICEFAULT_FEATURE
#define BLE_GLUCOSESERVICE_SUPPORT_GENERALDEVICEFAULT_FEATURE		                    0
#endif //BLE_GLUCOSESERVICE_SUPPORT_GENERALDEVICEFAULT_FEATURE

#ifndef BLE_GLUCOSESERVICE_SUPPORT_TIMEFAULT_FEATURE
#define BLE_GLUCOSESERVICE_SUPPORT_TIMEFAULT_FEATURE	                	            0
#endif //BLE_GLUCOSESERVICE_SUPPORT_TIMEFAULT_FEATURE

#ifndef BLE_GLUCOSESERVICE_SUPPORT_MULTIPLEBOND_FEATURE
#define BLE_GLUCOSESERVICE_SUPPORT_MULTIPLEBOND_FEATURE     		                    0
#endif //BLE_GLUCOSESERVICE_SUPPORT_MULTIPLEBOND_FEATURE

/**
 * BLE_GLUCOSESERVICE_SUPPORT_SENSOR_STATUS_ANNUNCIATION_MEASUREMENT
 * The Sensor Status Annunciation field shall be included in the Glucose 
 * Measurement characteristic if at least one glucose feature is supported.
 * Otherwise it is optional. 
 * If a sensor status annunciation is supported, the sensor includes the Sensor
 * Status Annunciation in the Glucose Measurement characteristic. 
 * The value of the Sensor Status Annunciation field is configured by the 
 * sensor.
 *
 * If BLE_GLUCOSESERVICE_SUPPORT_SENSOR_STATUS_ANNUNCIATION_MEASUREMENT is 
 * enabled (1), the BleGlucoseMeasurement structure passed to 
 * BLEGLUCOSESERVICE_SetGlucoseMeasurement contains the Sensor Status 
 * Annunciation field.
 *
 * The default value for this option is disabled (0).
 *
 */
#ifndef BLE_GLUCOSESERVICE_SUPPORT_SENSOR_STATUS_ANNUNCIATION_MEASUREMENT
#define BLE_GLUCOSESERVICE_SUPPORT_SENSOR_STATUS_ANNUNCIATION_MEASUREMENT				BLE_GLUCOSESERVICE_SUPPORT_LOWBATTERYDETECTIONDURINGMEASUREMENT_FEATURE	|\
                                                                                        BLE_GLUCOSESERVICE_SUPPORT_SENSORMALFUNCTIONDETECTION_FEATURE	|\
                                                                                        BLE_GLUCOSESERVICE_SUPPORT_SENSORSAMPLESIZE_FEATURE |\
                                                                                        BLE_GLUCOSESERVICE_SUPPORT_SENSORSTRIPINSERTIONERRORDETECTION_FEATURE |\
                                                                                        BLE_GLUCOSESERVICE_SUPPORT_SENSORSTRIPTYPEERRORDETECTION_FEATURE |\
                                                                                        BLE_GLUCOSESERVICE_SUPPORT_SENSORRESULTHIGHLOWDETECTION_FEATURE |\
                                                                                        BLE_GLUCOSESERVICE_SUPPORT_SENSORTEMPERATUREHIGHLOWDETECTION_FEATURE |\
                                                                                        BLE_GLUCOSESERVICE_SUPPORT_SENSORREADINTERRUPTDETECTION_FEATURE |\
                                                                                        BLE_GLUCOSESERVICE_SUPPORT_GENERALDEVICEFAULT_FEATURE |\
                                                                                        BLE_GLUCOSESERVICE_SUPPORT_TIMEFAULT_FEATURE |\
                                                                                        BLE_GLUCOSESERVICE_SUPPORT_MULTIPLEBOND_FEATURE	
#endif //BLE_GLUCOSESERVICE_SUPPORT_SENSOR_STATUS_ANNUNCIATION_MEASUREMENT

/**
 * BLE_GLUCOSESERVICE_SUPPORT_CONTEXT_INFORMATION_FOLLOWS
 * The Context information follows flag shall be set to 1 if the Glucose 
 * Measurement Context characteristic is supported. Otherwise it is optional.
 * It indicates that each Glucose Measurement notification shall be followed
 * by a Glucose Measurement Context notification.
 *
 * If BLE_GLUCOSESERVICE_SUPPORT_CONTEXT_INFORMATION_FOLLOWS is 
 * enabled (1), a BleGlucoseMeasurementContext structure shall be passed to 
 * BLEGLUCOSESERVICE_SetGlucoseMeasurementContext.
 *
 * The default value for this option is disabled (0).
 *
 */
#ifndef BLE_GLUCOSESERVICE_SUPPORT_CONTEXT_INFORMATION_FOLLOWS
#define BLE_GLUCOSESERVICE_SUPPORT_CONTEXT_INFORMATION_FOLLOWS          		        0
#endif //BLE_GLUCOSESERVICE_SUPPORT_CONTEXT_INFORMATION_FOLLOWS

/***************************************************************************\
 * CONSTANTS
\***************************************************************************/
/* BLEINFOTYPE_GLUCOSESERVICE_GLUCOSEMEASUREMENT_CLIENTCONFIG
 * BLEINFOTYPE_GLUCOSESERVICE_RECORDACCESSCONTROLPOINT_CLIENTCONFIG
 * The Unique descriptor ID for a two Bytes length information to save
 * in persistent memory representing the client config for a given remote
 * bonded device.
 * It is the Unique identifier passed in the infotype fields when the PROFILE
 * call the system APIs SYSTEM_SetPersistentInformation(BD_ADDR addr,
 * U8 infoType, U8* infoValue,U8 InfoLen) and 
 * SYSTEM_GetPersistentInformation(BD_ADDR addr, U8 infoType, U8 **infoValue,
 * U8 *InfoLen). 
 */
#define BLEINFOTYPE_GLUCOSESERVICE_GLUCOSEMEASUREMENT_CLIENTCONFIG	                    0x5A    //2 bytes
#define BLEINFOTYPE_GLUCOSESERVICE_RECORDACCESSCONTROLPOINT_CLIENTCONFIG			    0x5B    //2 bytes



/***************************************************************************\
 *	Type definition
\***************************************************************************/


/** 
 * BleGlucoseServiceSensorStatusAnnunciationBitfield type
 * Bitfield defining sensor status annunciation available values.
 */
typedef	U16	BleGlucoseServiceSensorStatusAnnunciationBitfield;
#define BLEGLUCOSESERVICE_SENSORSTATUSANNUNCIATION_LOWBATTERYATMEASUREMENT             (0x0001u)
#define BLEGLUCOSESERVICE_SENSORSTATUSANNUNCIATION_MALFUNCTIONATMEASUREMENT       		(0x0002u)
#define BLEGLUCOSESERVICE_SENSORSTATUSANNUNCIATION_SAMPLESIZEINSUFFICIENTATMEASUREMENT (0x0004u)
#define BLEGLUCOSESERVICE_SENSORSTATUSANNUNCIATION_STRIPINSERTIONERROR		            (0x0008u)
#define BLEGLUCOSESERVICE_SENSORSTATUSANNUNCIATION_STRIPTYPEINCORRECT				    (0x0010u)
#define BLEGLUCOSESERVICE_SENSORSTATUSANNUNCIATION_RESULTHIGHERTHANTHEDEVICECANPROCESS	(0x0020u)
#define BLEGLUCOSESERVICE_SENSORSTATUSANNUNCIATION_RESULTLOWERTHANTHEDEVICECANPROCESS	(0x0040u)
#define BLEGLUCOSESERVICE_SENSORSTATUSANNUNCIATION_TEMPERATURETOOHIGHATMEASUREMENT		(0x0080u)
#define BLEGLUCOSESERVICE_SENSORSTATUSANNUNCIATION_TEMPERATURETOOLOWATMEASUREMENT		(0x0100u)
#define BLEGLUCOSESERVICE_SENSORSTATUSANNUNCIATION_STRIPPULLEDTOOSOON					(0x0200u)
#define BLEGLUCOSESERVICE_SENSORSTATUSANNUNCIATION_GENERALDEVICEFAULT					(0x0400u)
#define BLEGLUCOSESERVICE_SENSORSTATUSANNUNCIATION_TIMEFAULT							(0x0800u)

/** 
 * BleGlucoseServiceFeatureBitfield type
 * Bitfield defining the remote Glucose SENSOR capabilities.
 */
typedef	U16	BleGlucoseServiceFeatureBitfield;
#define BLEGLUCOSESERVICE_GLUCOSEFEATURE_LOWBATTERYDETECTIONDURINGMEASUREMENT		    (0x0001u)
#define BLEGLUCOSESERVICE_GLUCOSEFEATURE_SENSORMALFUNCTIONDETECTION				    (0x0002u)
#define BLEGLUCOSESERVICE_GLUCOSEFEATURE_SENSORSAMPLESIZE							    (0x0004u)
#define BLEGLUCOSESERVICE_GLUCOSEFEATURE_SENSORSTRIPINSERTIONERRORDETECTION		    (0x0008u)
#define BLEGLUCOSESERVICE_GLUCOSEFEATURE_SENSORSTRIPTYPEERRORDETECTION				    (0x0010u)
#define BLEGLUCOSESERVICE_GLUCOSEFEATURE_SENSORRESULTHIGHLOWDETECTION				    (0x0020u)
#define BLEGLUCOSESERVICE_GLUCOSEFEATURE_SENSORTEMPERATUREHIGHLOWDETECTION			    (0x0040u)
#define BLEGLUCOSESERVICE_GLUCOSEFEATURE_SENSORREADINTERRUPTDETECTION				    (0x0080u)
#define BLEGLUCOSESERVICE_GLUCOSEFEATURE_GENERALDEVICEFAULT						    (0x0100u)
#define BLEGLUCOSESERVICE_GLUCOSEFEATURE_TIMEFAULT									    (0x0200u)
#define BLEGLUCOSESERVICE_GLUCOSEFEATURE_MULTIPLEBONDDETECTION						    (0x0400u)

/** 
 * BleGlucoseServiceMeasurementFlags type
 * Bitfield defining the available glucose measurement flags values.
 */
typedef U8 BleGlucoseServiceMeasurementFlags;
#define BLEGLUCOSESERVICE_FLAGBITMASK_TIMEOFFSETPRESENT			                            (0x01u)
#define BLEGLUCOSESERVICE_FLAGBITMASK_CONCENTRATIONPRESENT                                   (0x02u)
#define BLEGLUCOSESERVICE_FLAGBITMASK_CONCENTRATIONUNIT_KG_L	                                (0x00u)
#define BLEGLUCOSESERVICE_FLAGBITMASK_CONCENTRATIONUNIT_MOL_L                                (0x04u)
#define BLEGLUCOSESERVICE_FLAGBITMASK_SENSORSTATUSANNUNCIATIONPRESENT                        (0x08u)
#define BLEGLUCOSESERVICE_FLAGBITMASK_CONTEXTINFORMATIONFOLLOWS         	                    (0x10u)

/** 
 * BleGlucoseMeasurementFlags type
 * Define the current Glucose Measurement flags to be set during the 
 * call of BLEGLUCOSESERVICE_SetGlucoseServiceMeasurement()
 * It can be builded using the flag value or bit to bit definition
 */
typedef union {
    BleGlucoseServiceMeasurementFlags
                                    glucoseMeasurementFlagsValue;
    struct 
    {
        //The Time Offset field shall be included in the Glucose Measurement
        //characteristic value whenever the value of the Time Offset changes
        //from the last reported measurement.
        unsigned int timeOffsetPresent:1;

        //The Glucose Concentration field is optional, but if it is present, 
        //the Type-Sample Location field shall also be present.
        unsigned int concentrationPresent:1;

        //Concentration unit can be specified by using this field
        //In that case 0 is for kg/L and 1 for mol/L
        //Otherwise it can specified by setting glucoseMeasurementFlagsValue
        //In that case make a XOR with 
        //BLEGLUCOSESERVICE_FLAGBITMASK_CONCENTRATIONUNIT_KG_L (0x00u)
        //or BLEGLUCOSESERVICE_FLAGBITMASK_CONCENTRATIONUNIT_MOL_L (0x04u)
        unsigned int concentrationUnit:1;

        //The Sensor Status Annunciation field may be included in the Glucose
        //Measurement characteristic value if the device supports Sensor 
        //Status Annunciation flags.The sensor status annunciation indicates 
        //sensor features information. It has to be used if at least one 
        //glucose feature is supported.
        unsigned int sensorStatusAnnunciationPresent:1;

        //If a Glucose Measurement characteristic includes contextual 
        //information (i.e., a corresponding Glucose Measurement Context 
        //characteristic), the Context Information Follows Flag (bit 4 
        //of the Flags field) shall be set to 1, otherwise the Flag shall
        //be set to 0.
        unsigned int contextInformationFollows:1;
        unsigned int reserved:3;

    } fields;
} BleGlucoseMeasurementFlags;

/** 
 * BleGlucoseServiceMeasurement type
 * Define the current Glucose Measurement flags to be set during the 
 * call of BLEGLUCOSESERVICE_SetGlucoseMeasurement
 */
typedef struct{
    // The Flags field is an 8-bit bit field which indicates the unit used in
    // the Glucose Concentration field (if used), what fields are present in 
    // the Glucose Measurement characteristic, and whether or not context 
    // information is included.
    BleGlucoseMeasurementFlags						    flagsValue;

    // The Sequence Number is an unsigned 16-bit integer that represents the
    // chronological order of the patient records in the Server measurement 
    // database.
    U16													sequenceNumberValue;

    // The Base Time field represents the value of an internal real-time clock
    // or equivalent that keeps time relative to its initial setting in 
    // resolution of seconds.
    BleDateTime											baseTimeValue;

#if (BLE_GLUCOSESERVICE_SUPPORT_TIMEOFFSET_MEASUREMENT == 1)
    //The Time Offset field shall be included in the Glucose Measurement
    //characteristic value whenever the value of the Time Offset changes
    //from the last reported measurement.
    //The Time Offset field is defined as a 16-bit signed integer representing
    //the number of minutes the user-facing time differs from the Base Time.
    S16													timeOffsetValue;
#endif //(BLE_GLUCOSESERVICE_SUPPORT_TIMEOFFSET_MEASUREMENT == 1)

#if (BLE_GLUCOSESERVICE_SUPPORT_CONCENTRATION_MEASUREMENT == 1)
    //Glucose measurement Value, in kg/L or mol/L	
    //The Glucose Concentration field is optional, but if it is present, 
    //the Type-Sample Location field shall also be present
    U16     	    									concentrationValue;
    U8													sampleTypeValue;
    U8													sampleLocationValue;
#endif //(BLE_GLUCOSESERVICE_SUPPORT_CONCENTRATION_MEASUREMENT == 1)

#if (BLE_GLUCOSESERVICE_SUPPORT_SENSOR_STATUS_ANNUNCIATION_MEASUREMENT == 1)
    //The Sensor Status Annunciation field may be included in the Glucose
    //Measurement characteristic value if the device supports Sensor 
    //Status Annunciation flags.
    BleGlucoseServiceSensorStatusAnnunciationBitfield   sensorStatusAnnunciationValue;
#endif //(BLE_GLUCOSESERVICE_SUPPORT_SENSOR_STATUS_ANNUNCIATION_MEASUREMENT == 1)

} BleGlucoseServiceMeasurement;

/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
/** Set the current Glucose Measurement 
 *
 * BLEGLUCOSESERVICE_SetGlucoseMeasurement()
 *	This function is used to add the current Glucose Measurement 
 *  in record table.
 *
 * BLE_SUPPORT_GLUCOSE_SERVICE shall be enabled.
 *
 * @param glucoseMeasurement : the structure representing the Glucose 
 *		Measurement, see the field description of 
 *		BleGlucoseServiceMeasurement for more detailled information.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Guillaume FERLIN
 */
BleStatus BLEGLUCOSESERVICE_SetGlucoseMeasurement(
    BleGlucoseServiceMeasurement *glucoseMeasurement);

/** Delete Glucose Measurement Records
 *
 * BLEGLUCOSESERVICE_DeleteRecords()
 *	This function is used to delete records within range of min and max.
 *  To delete all records set min to 0x0000 and max to 0xFFFF.
 *
 * BLE_SUPPORT_GLUCOSE_SERVICE shall be enabled.
 *
 * @param min: sequence number min to delete
 * @param max: sequence number max to delete
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *      It may be explained by no record found in range.
 *
 * @author Guillaume FERLIN
 */
BleStatus BLEGLUCOSESERVICE_DeleteRecords(
    U16 min,
    U16 max);

#endif //(BLE_SUPPORT_GLUCOSE_SERVICE == 1)

#endif //__BLEGLUCOSE_SERVICE_H
