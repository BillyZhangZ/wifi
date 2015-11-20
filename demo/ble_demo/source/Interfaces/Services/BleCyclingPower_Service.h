#ifndef __BLECYCLINGPOWER_SERVICE_H
#define __BLECYCLINGPOWER_SERVICE_H
/****************************************************************************
 *
 * File:          BleCyclingPower_Service.h
 *
 * Description:   Contains routines declaration for Cycling Power Service.
 * 
 * Created:       May, 2013
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
 *	CONFIGURATION
\***************************************************************************/

/**
 * BLE_SUPPORT_CYCLINGPOWER_SERVICE
 * Defines if the BLE local device enables access to the local CYCLING POWER   
 * service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles or directly an Application to 
 * get access to the CYCLING POWER Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_CYCLINGPOWER_SERVICE
#define BLE_SUPPORT_CYCLINGPOWER_SERVICE									0
#endif //BLE_SUPPORT_CYCLINGPOWER_SERVICE

/***************************************************************************\
 * OPTIONAL FEATURES
\***************************************************************************/

/**BLE_CYCLINGPOWERSERVICE_SUPPORT_SENSORLOCATION
 * The Sensor Location of the device is used to describe the intended location
 * of the sensor location measurement for the device. The value of the Sensor 
 * Location characteristic is static while in a connection. 
 *
 * If BLE_CYCLINGPOWER_SUPPORT_SENSORLOCATION is enabled (1), the
 * BLECYCLINGPOWERSERVICE_SetCyclingPowerSensorLocation API will be enabled 
 * to allow the application to change the location when not connected. 
 *
 * The default value for this option is disabled (0).
 *
 */
#ifndef BLE_CYCLINGPOWERSERVICE_SUPPORT_SENSORLOCATION
#define BLE_CYCLINGPOWERSERVICE_SUPPORT_SENSORLOCATION						1
#endif //BLE_CYCLINGPOWERSERVICE_SUPPORT_SENSORLOCATION 

/** 
 * BLE_CYCLINGPOWERESERVICE_SUPPORT_SUPPORT_PEDAL_POWER_BALANCE
 * Define if the the CYCLING POWER Sensor supports (1) or not (0) to set the
 * Pedal Power Balance values in a measurement.
 * The default value is supported (1)
 */
#ifndef BLE_CYCLINGPOWERSERVICE_SUPPORT_PEDAL_POWER_BALANCE
#define BLE_CYCLINGPOWERSERVICE_SUPPORT_PEDAL_POWER_BALANCE					1
#endif //BLE_CYCLINGPOWERSERVICE_SUPPORT_PEDAL_POWER_BALANCE


/** 
 * BLE_CYCLINGPOWERESERVICE_SUPPORT ACCUMULATED TORQUE SOURCE
 * Define if the the CYCLING POWER Sensor supports (1) or not (0) to set the
 * Accumulated Torque Source values in a measurement.
 * The default value is supported (1)
 */
#ifndef BLE_CYCLINGPOWERSERVICE_SUPPORT_ACCUMULATED_TORQUE
#define BLE_CYCLINGPOWERSERVICE_SUPPORT_ACCUMULATED_TORQUE					1
#endif //BLE_CYCLINGPOWERSERVICE_SUPPORT_ACCUMULATED_TORQUE

/** 
 * BLE_CYCLINGPOWERSERVICE_SUPPORT_WHEEL_REVOLUTION_DATA
 * Define if the the CYCLINGPOWER Sensor supports (1) or not (0) to set the Wheel 
 * Revolution Data values in a measurement.
 * The default value is supported (1)
 */
#ifndef BLE_CYCLINGPOWERSERVICE_SUPPORT_WHEEL_REVOLUTION_DATA	
#define BLE_CYCLINGPOWERSERVICE_SUPPORT_WHEEL_REVOLUTION_DATA				1
#endif //BLE_CYCLINGPOWERSERVICE_SUPPORT_WHEEL_REVOLUTION_DATA

/** 
 * BLE_CYCLINGPOWERSERVICE_SUPPORT_CRANK_REVOLUTION_DATA
 * Define if the the CYCLINGPOWER Sensor supports (1) or not (0) to set the Crank 
 * Revolution Data value in a measurement.
 * The default value is supported (1)
 */
#ifndef BLE_CYCLINGPOWERSERVICE_SUPPORT_CRANK_REVOLUTION_DATA	
#define BLE_CYCLINGPOWERSERVICE_SUPPORT_CRANK_REVOLUTION_DATA				1
#endif //BLE_CYCLINGPOWERSERVICE_SUPPORT_CRANK_REVOLUTION_DATA

/** 
 * BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_FORCE_MAGNITUDES
 * Define if the the CYCLINGPOWER Sensor supports (1) or not (0) to set the Extreme
 * Force Magnitude value in a measurement.
 * A server can not support simultaneous Extreme Torque and Extreme Force.
 * The default value is supported (1)
 */
#ifndef BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_FORCE_MAGNITUDES
#define BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_FORCE_MAGNITUDES			1
#endif //BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_FORCE_MAGNITUDES

/** 
 * BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_TORQUE_MAGNITUDES
 * Define if the the CYCLINGPOWER Sensor supports (1) or not (0) to set the Extreme
 * Torque Magnitudes value in a measurement.
 * A server can not support simultaneous Extreme Torque and Extreme Force. So :
 * The default value is not  supported (0) 
 *
 */
#ifndef BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_TORQUE_MAGNITUDES
#define BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_TORQUE_MAGNITUDES			0
#endif //BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_TORQUE_MAGNITUDES

/** 
 * A server can not support simultaneous Extreme Torque and Extreme Force. 
 */
#if ((BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_TORQUE_MAGNITUDES == 1) && (BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_FORCE_MAGNITUDES == 1) )
#error A server can not support simultaneous Extreme Torque and Extreme Force. 
#endif //((BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_TORQUE_MAGNITUDES == 1) && (BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_FORCE_MAGNITUDES == 1) )

 /* BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_TORQUE_MAGNITUDES
 * Define if the the CYCLINGPOWER Sensor supports (1) or not (0) to set the Extreme
 * Torque Magnitudes value in a measurement.
 * The default value is supported (1)
 */
#ifndef BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_ANGLES
#define BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_ANGLES						1
#endif //BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_ANGLES

/** 
 * BLE_CYCLINGPOWERESERVICE_SUPPORT_TOP_DEAD_SPOT_ANGLE
 * Define if the the CYCLING POWER Sensor supports (1) or not (0) to set the
 * Top Dead Spot Angle values in a measurement.
 * The default value is supported (1)
 */
#ifndef BLE_CYCLINGPOWERSERVICE_SUPPORT_TOP_DEAD_SPOT_ANGLE
#define BLE_CYCLINGPOWERSERVICE_SUPPORT_TOP_DEAD_SPOT_ANGLE					1
#endif //BLE_CYCLINGPOWERSERVICE_SUPPORT_TOP_DEAD_SPOT_ANGLE			
/** 
 * BLE_CYCLINGPOWERESERVICE_SUPPORT_BOTTOM_DEAD_SPOT_ANGLE
 * Define if the the CYCLING POWER Sensor supports (1) or not (0) to set the
 * Top Dead Spot Angle values in a measurement.
 * The default value is supported (1)
 */
#ifndef BLE_CYCLINGPOWERSERVICE_SUPPORT_BOTTOM_DEAD_SPOT_ANGLE
#define BLE_CYCLINGPOWERSERVICE_SUPPORT_BOTTOM_DEAD_SPOT_ANGLE			    1
#endif //BLE_CYCLINGPOWERSERVICE_SUPPORT_BOTTOM_DEAD_SPOT_ANGLE 

/** 
 * BLE_CYCLINGPOWERESERVICE_SUPPORT_ACCUMULATED_ENERGY
 * Define if the the CYCLING POWER Sensor supports (1) or not (0) to set the
 * Accumulated Energy values in a measurement.
 * The default value is supported (1)
 */
#ifndef BLE_CYCLINGPOWERSERVICE_SUPPORT_ACCUMULATED_ENERGY
#define BLE_CYCLINGPOWERSERVICE_SUPPORT_ACCUMULATED_ENERGY					1
#endif //BLE_CYCLINGPOWERSERVICE_SUPPORT_ACCUMULATED_ENERGY

/***************************************************************************\
 * CONSTANTS
\***************************************************************************/
#define BLEINFOTYPE_CYCLINGPOWERSERVICE_MEASUREMENT_CLIENTCONFIG		0xAA	 
#define BLEINFOTYPE_CYCLINGPOWERESERVICE_CONTROLPOINT_CLIENTCONFIG		0xAB

/***************************************************************************\
 *	Type definition
\***************************************************************************/

/** 
 * BleCyclingPowerServiceFlags type
 * Bitfield defining the available flags values.
 */
typedef U16 BleCyclingPowerServiceFlags;

#define BLECYCLINGPOWERPSERVICE_FLAGBITMASK_PEDALPOWERBALANCEMEASUREMENT			      (0x0001u)
#define BLECYCLINGPOWERPSERVICE_FLAGBITMASK_PEDALPOWERBALANCEREFERENCEMEASUREMENT		  (0x0002u)
#define BLECYCLINGPOWERPSERVICE_FLAGBITMASK_ACCUMULATEDTORQUEMEASUREMENT				  (0x0004u)
#define BLECYCLINGPOWERPSERVICE_FLAGBITMASK_ACCUMULATEDTORQUESOURCEMEASUREMENT            (0x0008u)
#define BLECYCLINGPOWERPSERVICE_FLAGBITMASK_WHEELREVOLUTIONDATAMEASUREMENT				  (0x0010u)
#define BLECYCLINGPOWERPSERVICE_FLAGBITMASK_CRANKREVOLUTIONDATAMEASUREMENT				  (0x0020u)
#define BLECYCLINGPOWERPSERVICE_FLAGBITMASK_EXTREMEFORCEMAGNITUDESMEASUREMENT             (0x0040u)
#define BLECYCLINGPOWERPSERVICE_FLAGBITMASK_EXTREMETORQUEMAGNITUDESMEASUREMENT            (0x0080u)
#define BLECYCLINGPOWERPSERVICE_FLAGBITMASK_EXTREMANGLESMEASUREMENT					      (0x0100u)
#define BLECYCLINGPOWERPSERVICE_FLAGBITMASK_TOPDEADSPOTANGLEMEASUREMENT					  (0x0200u)
#define BLECYCLINGPOWERPSERVICE_FLAGBITMASK_BOTTOMDEADSPOTANGLEMEASUREMENT                (0x0400u)
#define BLECYCLINGPOWERPSERVICE_FLAGBITMASK_ACCUMULATEDENERGYMEASUREMENT                  (0x0800u)
#define BLECYCLINGPOWERPSERVICE_FLAGBITMASK_OFFSETCOMPENSATIONMEASUREMENT                 (0x1000u)

/** 
 * BleCyclingPowerServiceSensorLocation  type  
 * Define different emplacement of Sensor Location 
 */
typedef U8  BleCyclingPowerServiceSensorLocation;

#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_OTHER							  0
#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_TOPOFSHOE						  1
#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_INSHOE						  2
#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_HIP							  3
#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_FRONTWHEEL					  4
#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_LEFTCRANK						  5
#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_RIGHTCRANK					  6
#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_LEFTPEDAL						  7
#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_RIGHTPEDAL					  8
#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_FRONTHUB						  9
#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_REARDROPOUT					 10
#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_CHAINSTAY						 11
#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_REARWHEEL						 12
#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_REARHUB						 13
#define BLECYCLINGPOWERSERVICE_SENSORLOCATION_CHEST							 14

/** 
 *  The BleCyclingPowerServiceSensorLocationSupported   type
 *  Define the different emplacement of Sensor Location could be 
 *  supported by Cycling Power Sensor.
 *  It can be stocked in the list of sensor location supported
 */
typedef U16 BleCyclingPowerServiceSensorLocationSupported;

#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_OTHER						  (0x0001u)
#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_TOPOFSHOE					  (0x0002u)
#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_INSHOE						  (0x0004u)
#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_HIP							  (0x0008u)
#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_FRONTWHEEL					  (0x0010u)
#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_LEFTCRANK					  (0x0020u)
#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_RIGHTCRANK					  (0x0040u)
#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_LEFTPEDAL					  (0x0080u)
#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_RIGHTPEDAL					  (0x0100u)
#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_FRONTHUB						  (0x0200u)
#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_REARDROPOUT					  (0x0400u)
#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_CHAINSTAY					  (0x0800u)
#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_REARWHEEL					  (0x1000u)
#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_REARHUB						  (0x2000u)
#define BLECYCLINGPOWERSERVICE_SENSORLOCATIONSUPPORTED_CHEST						  (0x4000u)

/** 
 * BleCyclingPowerServiceMeasurementFlags type
 * Define the current Cycling Power Measurement flags to be set during the 
 * call of BLECYCLINGPOWERSERVICE_SetCyclingPowerMeasurement
 * It can be builded using the flag value or bit to bit definition
 */
typedef union {
    /** The flags byte value
     */
	BleCyclingPowerServiceFlags cyclingPowerMeasurementFlagsValue;
	
	struct 
	{	
        /** Pedal Power Balance Present et Pedal Power Balance Reference:
         * Fields indicating if Pedal Power Balance
         * is included in the measurement.
         * ignored if 
		 * BLE_CYCLINGPOWERSERVICE_SUPPORT_PEDAL_POWER_BALANCE
         * is not supported.
		 * The Pedal Power Balance Reference bit of the Flags field (bit 1) 
         * describes whether the value is referenced from ’left’ or 
		 * the reference is ‘unknown’.
         */
        unsigned int pedalPowerBalancePresent:1;
		unsigned int pedalPowerBalanceReference:1;
		
		
		/** Accumulated torque present and Accumulated torque source:
         * Fields indicating if Accumulated Torque 
         * is included in the measurement.
         * ignored if 
         * BLE_CYCLINGPOWERSERVICE_SUPPORT_ACCUMULATED_TORQUE
         * is not supported.
		 * The Accumulated Torque Source bit of the Flags field (bit 3) 
		 * describes whether the value is ’wheel based’ or ‘crank based’
         */
         unsigned int accumulatedTorquePresent:1;
		 unsigned int accumulatedTorqueSource:1;
		
		/** Wheel Revolution Data Present:
         * Field indicating if the wheel revolution data 
         * is included in the measurement.
         * ignored if 
         * BLE_CYCLINGPOWERSERVICE_SUPPORT_WHEEL_REVOLUTION_DATA
         * is not supported.
         */
		 unsigned int wheelRevolutionDataPresent:1;
        		
        /** Crank Revolution Data Present:
         * Field indicating if the crank revolution data 
         * is included in the measurement.
         * ignored if 
         * BLE_CYCLINGPOWERSERVICE_SUPPORT_CRANK_REVOLUTION_DATA
         * is not supported 
         */
		unsigned int crankRevolutionDataPresent:1;
       
		/** Extreme Force Magnitudes Present:
         * Field indicating if the extreme force magnitudes
         * is included in the measurement.
         * ignored if 
         * BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_FORCE_MAGNITUDES
         * is not supported 
         */
		unsigned int extremeForceMagnitudesPresent:1;
			
		/** Extreme Torque Magnitudes Present:
         * Field indicating if the extreme torque magnitudes
         * is included in the measurement.
         * ignored if 
         * BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_TORQUE_MAGNITUDES
         * is not supported 
         */
		unsigned int extremeTorqueMagnitudesPresent:1;
		
		/** Extreme Angles Present:
         * Field indicating if the extreme angles
         * is included in the measurement.
         * ignored if 
         * BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_ANGLES
         * is not supported 
         */
		unsigned int extremeAnglesPresent:1;
		
		/** Top Dead Spot Angle Present:
         * Field indicating if the top dead spot angle
         * is included in the measurement.
         * ignored if 
         * BLE_CYCLINGPOWERSERVICE_SUPPORT_TOP_DEAD_SPOT_ANGLE		
         * is not supported 
         */
		 unsigned int topDeadSpotAnglePresent:1;	
       
		/** Bottom Dead Spot Angle Present:
         * Field indicating if the bottom dead spot angle
         * is included in the measurement.
         * ignored if 
		 * BLE_CYCLINGPOWERSERVICE_SUPPORT_BOTTOM_DEAD_SPOT_ANGLE	
         * is not supported 
         */
	     unsigned int bottomDeadSpotAnglePresent:1;	
		
		 /** Accumulated Energy Present:
         * Field indicating if the accumulated energy
         * is included in the measurement.
         * ignored if 
         * BLE_CYCLINGPOWERSERVICE_SUPPORT_ACCUMULATED_ENERGY	
         * is not supported 
         */
		 unsigned int accumulatedEnergyPresent:1;	
		 
		 //The other 3 bits are reserved for future use
		 unsigned int reserved:3;
	} fields;

}BleCyclingPowerServiceMeasurementFlags;


/** 
 * BleCyclingPowerServiceMeasurement type
 * Define the current Cycling Power Measurement value to be set during the 
 * call of BLECYCLINGPOWERSERVICE_SetCyclingPowerMeasurement
 */
typedef struct{
    /** flagValue:
     * The flags indicating what values are included in the measurement.
     */
	BleCyclingPowerServiceMeasurementFlags	     flagValue ;

	/** Instantaneous Power:
     * Field shall be included in the Cycling Power Measurement
     * represents the value of the power measured by the Server
     * Unit is in watts with a resolution of 1.
     */
     S16								instantaneousPowerValue ;

#if (BLE_CYCLINGPOWERSERVICE_SUPPORT_PEDAL_POWER_BALANCE == 1)
	/** Pedal Power Balance Value :
	 * Field represents the ratio between the total amount of power 
	 * measured by the sensor and a reference
	 * Unit is in percentage with a resolution of 1/2
	 */
	 U8						   		    pedalPowerBalanceValue ;

#endif

#if (BLE_CYCLINGPOWERSERVICE_SUPPORT_ACCUMULATED_TORQUE == 1)
	/** Cumulative Torque Value:
	 * The Accumulated Torque field represents the cumulative value of the torque
	 * measured by the Sensor.When a connection is established, this value starts 
	 * at 0 Newton meter and is may roll over.
	 * Unit is in newton metres with a resolution of 1/32
	 */
	 U16								cumulativeTorqueValue ;
#endif

#if (BLE_CYCLINGPOWERSERVICE_SUPPORT_WHEEL_REVOLUTION_DATA == 1)
    /** Cumulative Wheel Revolution:
     * The Cumulative Wheel Revolution value represents the number of times a 
     * wheel rotates, is used in combination with the Last Wheel Event Time
     * and the wheel circumference stored on the Client to determine:
     *  - the speed of the bicycle
     *  - the distance traveled. 
	 *  - the power if combined with the Crank Revolution Data
     * This value is expected to be set to 0 (or another desired value in case 
     * of e.g. a sensor upgrade) at initial installation on a bicycle
     */
	U32									 cumulativeWheelRevolution ;

    /** Last Wheel Event:
     * The 'wheel event time' is a free-running-count of 1/2048 second units 
     * and it represents the time when the wheel revolution was detected by 
     * the wheel rotation sensor. Since several wheel events can occur between
     * transmissions, only the Last Wheel Event Time value is transmitted. 
     * The Last Wheel Event Time value rolls over every 32 seconds
     */
	U16									  lastWheelEvent;
#endif //(BLE_CYCLINGPOWERSERVICE_SUPPORT_WHEEL_REVOLUTION_DATA == 1)

#if (BLE_CYCLINGPOWERSERVICE_SUPPORT_CRANK_REVOLUTION_DATA == 1)
    /** Cumulative Crank Revolution:
     * The Cumulative Crank Revolutions value, which represents the number of 
     * times a crank rotates, is used in combination with the Last Crank Event
     * Time to determine:
     *  - if the cyclist is coasting 
     *  - the average cadence. 
     * Average cadence is not accurate unless 0 cadence events (i.e. coasting) 
     * are subtracted.  This value is intended to roll over and is not 
     * configurable.
     */
	U16									    cumulativeCrankRevolution ;

    /** Last Crank Event:
     * The 'crank event time' is a free-running-count of 1/1024 second units
     * and it represents the time when the crank revolution was detected by
     * the crank rotation sensor. Since several crank events can occur between
     * transmissions, only the Last Crank Event Time value is transmitted. 
     * This value is used in combination with the Cumulative Crank Revolutions
     * value to enable the Client to calculate cadence. 
     * The Last Crank Event Time value rolls over every 64 seconds.
     */
	U16									    lastCrankEvent;
#endif //(BLE_CYCLINGPOWERSERVICE_SUPPORT_CRANK_REVOLUTION_DATA == 1)

#if ( BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_FORCE_MAGNITUDES == 1)
    /** Maximum Force Magnitude:
     * The maximumForceMagnitude value, which represents the maximum force value 
	 * measured in  a single crank revolution.
     * Unit is in newtons with a resolution of 1. 
     */
	S16										 maximumForceMagnitude;

    /** Minimum Force Magnitude:
	 * The Maximum Force Magnitude value, which represents the minimum force value 
     * measured in a single crank revolution.
     * Unit is in newtons with a resolution of 1. 
     */
	S16										 minimumForceMagnitude;
#endif //(BLE_CYCLINGPOWERSERVICE_SUPPORT_ EXTREME_FORCE_MAGNITUDES== 1)
#if ( BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_TORQUE_MAGNITUDES == 1)
    /** maximumForceMagnitude:
     * The Maximum Force Magnitude value, which represents the maximum torque value 
	 * measured in  a single crank revolution.
     * Unit is in newtons with a resolution of 1. 
     */
	S16										 maximumTorqueMagnitude;

    /** minimumTorqueMagnitude:
	 * The MaximumForceMagnitude value, which represents the minimum torque value 
     * measured in a single crank revolution.
     * Unit is in newtons with a resolution of 1. 
     */
	S16										 minimumTorqueMagnitude;
#endif //(BLE_CYCLINGPOWERSERVICE_SUPPORT_ EXTREME_TORQUE_MAGNITUDES== 1)

#if ( BLE_CYCLINGPOWERSERVICE_SUPPORT_EXTREME_ANGLES == 1)
    /** Maximum Angle:
     * The Maximum Force Magnitude value, which represents the angle of the 
	 * crank when the maximum value is measured in a single crank revolution.
     * Unit is in degrees with a resolution of 1 
     */
	U16										 maximumAngle;

    /** Minimum Angle:
	 * The maximumAngle value, which represents the minimum force value 
     * measured in a single crank revolution.
     * Unit is in newtons with a resolution of 1. 
     */
	U16										 minimumAngle;
#endif //(BLE_CYCLINGPOWERSERVICE_SUPPORT_ EXTREME_ANGLE == 1)

#if (BLE_CYCLINGPOWERSERVICE_SUPPORT_TOP_DEAD_SPOT_ANGLE == 1)
	/** Top Dead Spot Angle Value:
	 * The Top Dead Spot Angle field represents the crank angle when the value 
	 * of the Instantaneous Power value becomes Positive
	 * Unit is in degrees with a resolution of 1.
	 */
	 U16									 topDeadSpotAngleValue ;
	 							
#endif //(BLE_CYCLINGPOWERSERVICE_SUPPORT_TOP_SPOT_ANGLE)	

#if (BLE_CYCLINGPOWERSERVICE_SUPPORT_BOTTOM_DEAD_SPOT_ANGLE == 1)
	/* Bottom Dead Spot Angle Value:
	 * The Bottom Dead Spot Angle field represents the crank angle when the value 
	 * of the Instantaneous Power value becomes Negative
	 * Unit is in degrees with a resolution of 1.
	 */
	 U16								      bottomDeadSpotAngleValue ;
	 							
#endif //(BLE_CYCLINGPOWERSERVICE_SUPPORT_BOTTOM_SPOT_ANGLE)

#if (BLE_CYCLINGPOWERSERVICE_SUPPORT_ACCUMULATED_ENERGY ==1)	
   /* Accumulated Energy Value:
    * The Accumulated Energy field represents the accumulated value of the energy 
    * measured by the sensor. When a connection is established, this value starts 
	* at 0 kilojoule and is not expected to roll over.
	* Unit is in kilojoules with a resolution of 1.
    */
	U16									      accumulatedEnergyValue ;
#endif//(BLE_CYCLINGPOWERSERVICE_SUPPORT_ACCUMULATED_ENERGY ==1)	

} BleCyclingPowerServiceMeasurement;

/** 
 * BleCyclingPowerServiceEvent type
 * Define the different kind of events that could be received by the 
 * BleCyclingPowerServiceCallBack
 */

typedef U8 BleCyclingPowerServiceEvent;

/** BLECYCLINGPOWERSERVICE_EVENT_CUMULATIVEWHEELVALUEUPDATED
 * Event received when a Collector has a new Cumulative Value for
 * the cumulative wheel revolution.
 * The Status field is always BLESTATUS_SUCCESS
 * The parms field indicates the new cumulative value as a U32
 * U32 cumulativeWheelRevolution = *((U32 *) parms).
 * NOTE : The Sensor shall apply/set the new cumulative value when
 * receiving this event.
 * This event is only applicable when 
 * BLE_CYCLINGPOWERSERVICE_SUPPORT_WHEEL_REVOLUTION_DATA is supported
 */
#define BLECYCLINGPOWERSERVICE_EVENT_CUMULATIVEWHEELVALUEUPDATED		0xAB

/** BLECYCLINGPOWERSERVICE_EVENT_SENSORLOCATIONUPDATED
 * Event received when a Collector has a new Sensor Location Value for
 * the sensor location in Cycling Power Sensor.
 * The Status field is always BLESTATUS_SUCCESS
 * The parms field indicates the new sensor location value as a U8
 * NOTE : The Sensor shall apply/set the new sensor location value when
 * receiving this event in the database.
 * This event is only applicable when the feature
 *  MULTIPLE SENSOR LOCATIONS is supported

 */ 
#define BLECYCLINGPOWERSERVICE_EVENT_SENSORLOCATIONUPDATED				0xAC

/** BLECYCLINGPOWERSERVICE_EVENT_SENSORLOCATIONSUPPORTED
 * Event received when a Collector would like to request the application
 * to get the list of sensor location supported.
 *
 * The status field is always BLESTATUS_SUCCESS
 * Only received if the feauture MULTIPLE SENSOR LOCATIONS is supported
 */

#define BLECYCLINGPOWERSERVICE_EVENT_SENSORLOCATIONSUPPORTED			0xAD
/**
 * BleCyclingPowerServiceCallBack
 *	This callback receives the CYCLING POWER events. 
 *  Each events may be assiociated with specified status and parameters.
 *  The callback is executed during the stack context,  be careful to
 *	not doing heavy process in this function.
 */
typedef void (*BleCyclingPowerServiceCallBack)(
                        BleCyclingPowerServiceEvent event,
                        BleStatus status,
					    void *parms); 


/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
#if (BLE_SUPPORT_CYCLINGPOWER_SERVICE == 1)
/** Set the current Cycling Power Measurement 
 *
 * BLECYCLINGPOWERSERVICE_SetCyclingPowerMeasurement()
 *	This function is used to set the current CYCLINGPOWER Measurement 
 * characteristic value
 *
 * BLE_SUPPORT_CYCLINGPOWER_SERVICE shall be enabled.
 *
 * @param CyclingPowerMeasurement : the structure representing the CYCLINGPOWER 
 *		Measurement, see the field description of 
 *		BleCyclingPowerServiceMeasurement for more detailled information.
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
 * @author My Huong NHAN
 */
BleStatus BLECYCLINGPOWERSERVICE_SetCyclingPowerMeasurement(
						BleCyclingPowerServiceMeasurement *cyclingpowerMeasurement);

/**BLECYCLINGPOWERSERVICE_SetCyclingPowerSensorLocation()
 *	This function is used to set the current CYCLINGPOWER Sensor Location 
 * characteristic value
 *
 * BLE_SUPPORT_CYCLINGPOWER_SERVICE shall be enabled.
 *
 * @param CyclingPowerSensorLocation : the structure representing the CYCLINGPOWER 
 *		Sensor Location, see the field description of 
 *		BleCyclingPowerServiceSensorLocation for more detailled information.
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
 * @author My Huong NHAN
 */

BleStatus BLECYCLINGPOWERSERVICE_SetCyclingPowerSensorLocation(
					 BleCyclingPowerServiceSensorLocation  cyclingPowerSensorLocation);


/**BLECYCLINGPOWERSERVICE_SetCyclingPowerSensorLocationSupported()
 *	This function is used to set the  CYCLINGPOWER Sensor Location Supported  
 * characteristic values. 
 *
 * BLE_SUPPORT_CYCLINGPOWER_SERVICE shall be enabled.
 *
 * @param sensorLocationSupported : the structure representing the CYCLINGPOWER 
 *		Sensor Location supported, see the field description of 
 *		BleCyclingPowerServiceSensorLocationSupported for more detailled information.
 *
 * @return nothing
 *
 * @author My Huong NHAN
 */
void BLECYCLINGPOWERSERVICE_SetCyclingPowerSensorLocationSupported(
		BleCyclingPowerServiceSensorLocationSupported  cyclingPowerSensorLocationSupported);


#endif //(BLE_SUPPORT_CYCLINGPOWER_SERVICE == 1)

#endif //__BLECYCLINGPOWER_SERVICE_H
