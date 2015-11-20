#ifndef __BLERUNNINGSPEEDANDCADENCE_SERVICE_H
#define __BLERUNNINGSPEEDANDCADENCE_SERVICE_H
/****************************************************************************
 *
 * File:          BleRunningSpeedAndCadence_Service.h
 *
 * Description:   Contains routines declaration for Running Speed And Cadence
 *					Service.
 * 
 * Created:       December, 2012
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
 *	Macro and constant definition
\***************************************************************************/


/***************************************************************************\
 * OPTIONAL FEATURES
\***************************************************************************/

/**
 * BLE_SUPPORT_RUNNINGSPEEDANDCADENCE_SERVICE
 * Defines if the BLE local device enables access to the local RUNNING SPEED   
 * AND CADENCE service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles or directly an Application to 
 * get access to the RUNNING SPEED AND CADENCE Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_RUNNINGSPEEDANDCADENCE_SERVICE
#define BLE_SUPPORT_RUNNINGSPEEDANDCADENCE_SERVICE							        0
#endif //BLE_SUPPORT_RUNNINGSPEEDANDCADENCE_SERVICE

/** 
 * BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_TOTAL_DISTANCE
 * Define if the the RSC Sensor supports (1) or not (0) to set the total 
 * Distance value in a measurement.
 * The default value is not supported (0)
 */
#ifndef BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_TOTAL_DISTANCE	
#define BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_TOTAL_DISTANCE                    0
#endif //BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_TOTAL_DISTANCE

/** 
 * BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_INSTANTANEOUS_STRIDE_LENGTH
 * Define if the the RSC Sensor supports (1) or not (0) to set the  
 * instantaneous stride length value in a measurement.
 * The default value is not supported (0)
 */
#ifndef BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_INSTANTANEOUS_STRIDE_LENGTH	
#define BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_INSTANTANEOUS_STRIDE_LENGTH       0
#endif //BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_INSTANTANEOUS_STRIDE_LENGTH


/** 
 * BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_RUNNING_STATUS
 * Define if the the RSC Sensor supports (1) or not (0) to set the  
 * Running status bit defining if the sensor detect a run or a walk.
 * The default value is not supported (0)
 */
#ifndef BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_RUNNING_STATUS	
#define BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_RUNNING_STATUS                    0
#endif //BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_RUNNING_STATUS

/***************************************************************************\
 * CONSTANTS
\***************************************************************************/
#define BLEINFOTYPE_RUNNINGSPEEDANDCADENCESERVICE_MEASUREMENT_CLIENTCONFIG		    0xB0	
#define BLEINFOTYPE_RUNNINGSPEEDANDCADENCESERVICE_CONTROLPOINT_CLIENTCONFIG         0xB1

/***************************************************************************\
 *	Type definition
\***************************************************************************/

/** 
 * BleRunningSpeedAndCadenceServiceFlags type
 * Bitfield defining the available flags values.
 */
typedef U8 BleRunningSpeedAndCadenceServiceFlags;
#define BLERSCSERVICE_FLAGBITMASK_INSTANTANEOUSSTRIDELENGTHMEASUREMENT              (0x01u)
#define BLERSCSERVICE_FLAGBITMASK_TOTALDISTANCEMEASUREMENT		                    (0x02u)
#define BLERSCSERVICE_FLAGBITMASK_WALKINGORRUNNINGSTATUS		        	    	(0x04u)

/** 
 * BleRunningSpeedAndCadenceServiceMeasurementFlags type
 * Define the current RSC Measurement flags to be set during the 
 * call of BLERUNNINGSPEEDANDCADENCESERVICE_SetRscMeasurement
 * It can be builded using the flag value or bit to bt definition
 */
typedef union {
    /** The flags byte value
     */
	BleRunningSpeedAndCadenceServiceFlags rscMeasurementFlagsValue;

	struct 
	{
		/** instantaneousStrideLengthPresent:
         * Field indicating if the instantaneous Stride Length is included
         * in the measurement.
         * ignored if 
         * BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_INSTANTANEOUS_STRIDE_LENGTH
         * is not supported.
         */
        unsigned int instantaneousStrideLengthPresent:1;
        		
        /** totalDistancePresent:
         * Field indicating if the total Distance is included
         * in the measurement.
         * ignored if BLERSCSERVICE_FLAGBITMASK_TOTALDISTANCEMEASUREMENT
         * is not supported.
         */
		unsigned int totalDistancePresent:1;

        /** isRunning:
         * Field indicating the current running/walking status
         * isRunning = 0 : Walking ; isRunning = 1 : Running 	
         * ignored if BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_RUNNING_STATUS
         * is not supported.
         */
		unsigned int isRunning:1;
        
        //The other 5 bits are reserved for future use
		//unsigned int reserved:5;
	} fields;

} BleRunningSpeedAndCadenceServiceMeasurementFlags;


/** 
 * BleRunningSpeedAndCadenceServiceMeasurement type
 * Define the current RSC Measurement value to be set during the 
 * call of BLERUNNINGSPEEDANDCADENCESERVICE_SetRscMeasurement
 */
typedef struct{
    /** flagValue:
     * The flags indicating what values are included in the measurement or
     * what is the current running or walking status.
     */
	BleRunningSpeedAndCadenceServiceMeasurementFlags	flagValue ;

    /** instantaneousSpeedValue:
     * The instantaneous speed, 
     * Unit is in m/s with a resolution of 1/256 s
     */
	U16									     instantaneousSpeedValue ;

    /** instantaneousCadenceValue:
     * The instantaneous cadence, 
     * Unit is in 1/minute (or RPM) with a resolutions of 1 (1/min or 1 RPM) 
     */
	U8									    instantaneousCadenceValue;

#if (BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_INSTANTANEOUS_STRIDE_LENGTH == 1)
    /** instantaneousStrideLengthValue:
     * The instantaneous stride length, 
     *  Unit is in meter with a resolution of 1/100 m (or centimeter).
     */
    U16                                     instantaneousStrideLengthValue;
#endif //(BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_INSTANTANEOUS_STRIDE_LENGTH == 1)

#if (BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_TOTAL_DISTANCE == 1)
    /** totalDistanceValue:
     * The total distance value
     *  Unit is in meter with a resolution of 1/10 m (or decimeter).
     */
    U32                                     totalDistanceValue;
#endif //(BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_TOTAL_DISTANCE == 1)

} BleRunningSpeedAndCadenceServiceMeasurement;


/** 
 * BleRunningSpeedAndCadenceServiceEvent type
 * Define the different kind of events that could be received by the 
 * BleRunningSpeedAndCadenceServiceCallBack
 */
typedef U8 BleRunningSpeedAndCadenceServiceEvent;

/** BLERUNNINGSPEEDANDCADENCESERVICE_EVENT_TOTALDISTANCEUPDATED
 * Event received when a collector has Set a new Cumulative Value for
 * the total distance.
 * The Status field is always BLESTATUS_SUCCESS
 * The parms field indicates the total distance value as a U32
 * U32 totalDistance = *((U32 *) parms).
 * NOTE : The Sensor shall apply/set the new Total Distance Value when
 * receiving this event.
 * This event is only applicable when 
 * BLE_RUNNINGSPEEDANDCADENCESERVICE_SUPPORT_TOTAL_DISTANCE is supported
 */
#define BLERUNNINGSPEEDANDCADENCESERVICE_EVENT_TOTALDISTANCEUPDATED		0xDD


/**
 * BleRunningSpeedAndCadenceServiceCallBack
 *	This callback receives the RUNNING SPEED AND CADENCE SERVICE events. 
 *  Each events may be assiociated with specified status and parameters.
 *  The callback is executed during the stack context,  be careful to
 *	 not doing heavy process in this function.
 */
typedef void (*BleRunningSpeedAndCadenceServiceCallBack)(
                        BleRunningSpeedAndCadenceServiceEvent event,
                        BleStatus status,
					    void *parms); 

/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
#if (BLE_SUPPORT_RUNNINGSPEEDANDCADENCE_SERVICE == 1)
/** Set the current RSC Measurement 
 *
 * BLERUNNINGSPEEDANDCADENCESERVICE_SetRscMeasurement()
 *	This function is used to set the current RSC Measurement 
 * characteristic value
 *
 * BLE_SUPPORT_RUNNINGSPEEDANDCADENCE_SERVICE shall be enabled.
 *
 * @param rscMeasurement : the structure representing the RSC 
 *		Measurement, see the field description of 
 *		BleRunningSpeedAndCadenceServiceMeasurement for more detailled information.
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
BleStatus BLERUNNINGSPEEDANDCADENCESERVICE_SetRscMeasurement(
						BleRunningSpeedAndCadenceServiceMeasurement *rscMeasurement);
#endif //(BLE_SUPPORT_RUNNINGSPEEDANDCADENCE_SERVICE == 1)

#endif //__BLERUNNINGSPEEDANDCADENCE_SERVICE_H
