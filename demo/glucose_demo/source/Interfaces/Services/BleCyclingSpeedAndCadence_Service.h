#ifndef __BLECYCLINGSPEEDANDCADENCE_SERVICE_H
#define __BLECYCLINGSPEEDANDCADENCE_SERVICE_H
/****************************************************************************
 *
 * File:          BleCyclingSpeedAndCadence_Service.h
 *
 * Description:   Contains routines declaration for Cycling Speed And Cadence
 *					Service.
 * 
 * Created:       March, 2013
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.1 $
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
 * BLE_SUPPORT_CYCLINGSPEEDANDCADENCE_SERVICE
 * Defines if the BLE local device enables access to the local CYCLING SPEED   
 * AND CADENCE service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles or directly an Application to 
 * get access to the CYCLING SPEED AND CADENCE Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_CYCLINGSPEEDANDCADENCE_SERVICE
#define BLE_SUPPORT_CYCLINGSPEEDANDCADENCE_SERVICE							        0
#endif //BLE_SUPPORT_CYCLINGSPEEDANDCADENCE_SERVICE

/** 
 * BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_WHEEL_REVOLUTION_DATA
 * Define if the the CSC Sensor supports (1) or not (0) to set the Wheel 
 * Revolution Data values in a measurement.
 * The default value is supported (1)
 */
#ifndef BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_WHEEL_REVOLUTION_DATA	
#define BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_WHEEL_REVOLUTION_DATA             1
#endif //BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_WHEEL_REVOLUTION_DATA

/** 
 * BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_CRANK_REVOLUTION_DATA
 * Define if the the CSC Sensor supports (1) or not (0) to set the Crank 
 * Revolution Data value in a measurement.
 * The default value is supported (1)
 */
#ifndef BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_CRANK_REVOLUTION_DATA	
#define BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_CRANK_REVOLUTION_DATA             1
#endif //BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_CRANK_REVOLUTION_DATA


/***************************************************************************\
 * CONSTANTS
\***************************************************************************/
#define BLEINFOTYPE_CYCLINGSPEEDANDCADENCESERVICE_MEASUREMENT_CLIENTCONFIG		    0xB7	
#define BLEINFOTYPE_CYCLINGSPEEDANDCADENCESERVICE_CONTROLPOINT_CLIENTCONFIG         0xB8

/***************************************************************************\
 *	Type definition
\***************************************************************************/

/** 
 * BleCyclingSpeedAndCadenceServiceFlags type
 * Bitfield defining the available flags values.
 */
typedef U8 BleCyclingSpeedAndCadenceServiceFlags;
#define BLECSCSERVICE_FLAGBITMASK_WHEELREVOLUTIONDATAMEASUREMENT              (0x01u)
#define BLECSCSERVICE_FLAGBITMASK_CRANKREVOLUTIONDATAMEASUREMENT              (0x02u)


/** 
 * BleCyclingSpeedAndCadenceServiceMeasurementFlags type
 * Define the current CSC Measurement flags to be set during the 
 * call of BLECYCLINGSPEEDANDCADENCESERVICE_SetCscMeasurement
 * It can be builded using the flag value or bit to bt definition
 */
typedef union {
    /** The flags byte value
     */
	BleCyclingSpeedAndCadenceServiceFlags cscMeasurementFlagsValue;

	struct 
	{
		/** wheelRevolutionDataPresent:
         * Field indicating if the wheel revolution data 
         * is included in the measurement.
         * ignored if 
         * BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_WHEEL_REVOLUTION_DATA
         * is not supported.
         */
        unsigned int wheelRevolutionDataPresent:1;
        		
        /** crankRevolutionDataPresent:
         * Field indicating if the crank revolution data 
         * is included in the measurement.
         * ignored if 
         * BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_CRANK_REVOLUTION_DATA
         * is not supported 
         */
		unsigned int crankRevolutionDataPresent:1;
        
        //The other 6 bits are reserved for future use
		//unsigned int reserved:6;
	} fields;

} BleCyclingSpeedAndCadenceServiceMeasurementFlags;


/** 
 * BleCyclingSpeedAndCadenceServiceMeasurement type
 * Define the current CSC Measurement value to be set during the 
 * call of BLECYCLINGSPEEDANDCADENCESERVICE_SetCscMeasurement
 */
typedef struct{
    /** flagValue:
     * The flags indicating what values are included in the measurement.
     */
	BleCyclingSpeedAndCadenceServiceMeasurementFlags	flagValue ;

#if (BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_WHEEL_REVOLUTION_DATA == 1)
    /** CumulativeWheelRevolution:
     * The Cumulative Wheel Revolutions value represents the number of times a 
     * wheel rotates, is used in combination with the Last Wheel Event Time
     * and the wheel circumference stored on the Client to determine:
     *  - the speed of the bicycle
     *  - the distance traveled. 
     * This value is expected to be set to 0 (or another desired value in case 
     * of e.g. a sensor upgrade) at initial installation on a bicycle
     */
	U32									     cumulativeWheelRevolution ;

    /** lastWheelEvent:
     * The 'wheel event time' is a free-running-count of 1/1024 second units 
     * and it represents the time when the wheel revolution was detected by 
     * the wheel rotation sensor. Since several wheel events can occur between
     * transmissions, only the Last Wheel Event Time value is transmitted. 
     * The Last Wheel Event Time value rolls over every 64 seconds
     */
	U16									    lastWheelEvent;
#endif //(BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_WHEEL_REVOLUTION_DATA == 1)

#if (BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_CRANK_REVOLUTION_DATA == 1)
    /** CumulativeCrankRevolution:
     * The Cumulative Crank Revolutions value, which represents the number of 
     * times a crank rotates, is used in combination with the Last Crank Event
     * Time to determine:
     *  - if the cyclist is coasting 
     *  - the average cadence. 
     * Average cadence is not accurate unless 0 cadence events (i.e. coasting) 
     * are subtracted.  This value is intended to roll over and is not 
     * configurable.
     */
	U16									     cumulativeCrankRevolution ;

    /** lastCrankEvent:
     * The 'crank event time' is a free-running-count of 1/1024 second units
     * and it represents the time when the crank revolution was detected by
     * the crank rotation sensor. Since several crank events can occur between
     * transmissions, only the Last Crank Event Time value is transmitted. 
     * This value is used in combination with the Cumulative Crank Revolutions
     * value to enable the Client to calculate cadence. 
     * The Last Crank Event Time value rolls over every 64 seconds.
     */
	U16									    lastCrankEvent;
#endif //(BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_CRANK_REVOLUTION_DATA == 1)

} BleCyclingSpeedAndCadenceServiceMeasurement;


/** 
 * BleCyclingSpeedAndCadenceServiceEvent type
 * Define the different kind of events that could be received by the 
 * BleCyclingSpeedAndCadenceServiceCallBack
 */
typedef U8 BleCyclingSpeedAndCadenceServiceEvent;

/** BLECYCLINGSPEEDANDCADENCESERVICE_EVENT_CUMULATIVEWHEELVALUEUPDATED
 * Event received when a Collector has a new Cumulative Value for
 * the cumulative wheel revolution.
 * The Status field is always BLESTATUS_SUCCESS
 * The parms field indicates the new cumulative value as a U32
 * U32 cumulativeWheelRevolution = *((U32 *) parms).
 * NOTE : The Sensor shall apply/set the new cumulative value when
 * receiving this event.
 * This event is only applicable when 
 * BLE_CYCLINGSPEEDANDCADENCESERVICE_SUPPORT_WHEEL_REVOLUTION_DATA is supported
 */
#define BLECYCLINGSPEEDANDCADENCESERVICE_EVENT_CUMULATIVEWHEELVALUEUPDATED		0xDC


/**
 * BleCyclingSpeedAndCadenceServiceCallBack
 *	This callback receives the CYCLING SPEED AND CADENCE SERVICE events. 
 *  Each events may be assiociated with specified status and parameters.
 *  The callback is executed during the stack context,  be careful to
 *	 not doing heavy process in this function.
 */
typedef void (*BleCyclingSpeedAndCadenceServiceCallBack)(
                        BleCyclingSpeedAndCadenceServiceEvent event,
                        BleStatus status,
					    void *parms); 

/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
#if (BLE_SUPPORT_CYCLINGSPEEDANDCADENCE_SERVICE == 1)
/** Set the current CSC Measurement 
 *
 * BLECYCLINGSPEEDANDCADENCESERVICE_SetCscMeasurement()
 *	This function is used to set the current CSC Measurement 
 * characteristic value
 *
 * BLE_SUPPORT_CYCLINGSPEEDANDCADENCE_SERVICE shall be enabled.
 *
 * @param cscMeasurement : the structure representing the CSC 
 *		Measurement, see the field description of 
 *		BleCyclingSpeedAndCadenceServiceMeasurement for more detailled information.
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
BleStatus BLECYCLINGSPEEDANDCADENCESERVICE_SetCscMeasurement(
						BleCyclingSpeedAndCadenceServiceMeasurement *cscMeasurement);
#endif //(BLE_SUPPORT_CYCLINGSPEEDANDCADENCE_SERVICE == 1)

#endif //__BLECYCLINGSPEEDANDCADENCE_SERVICE_H
