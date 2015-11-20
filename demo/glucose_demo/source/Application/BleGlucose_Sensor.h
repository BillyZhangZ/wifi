#ifndef __BLEGLUCOSE_SENSOR_H
#define __BLEGLUCOSE_SENSOR_H
/****************************************************************************\
 *
 * File:          BleGlucose_Sensor.h
 *
 * Description:   Contains interfaces definitions for Glucose 
 *					Profile when the local device operates in the SENSOR
 *					role.
 * 
 * Created:       April, 2013
 * Version:		  0.1
 *
 * File Revision: $Rev: 2721 $
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

/****************************************************************************\
 *	CONFIGURATION
\****************************************************************************/

/**
 * BLEGLUCOSE_SUPPORT_SENSOR
 * Defines if the BLE GLUCOSE SENSOR Profile implementation supports the 
 * SENSOR Role.
 * If enabled ( set to 1 ) it enables all the following BLE GLUCOSE 
 * SENSOR Profile SENSOR configurations and Application Interfaces.
 * The default value for this option is disabled (0).
 */
#ifndef BLEGLUCOSE_SUPPORT_SENSOR
#define BLEGLUCOSE_SUPPORT_SENSOR					                1
#endif //BLEGLUCOSE_SUPPORT_SENSOR

#if (BLEGLUCOSE_SUPPORT_SENSOR== 1)
//
// Then check dependencies
#include "Services/BleDeviceInformation_Service.h"
#include "Services/BleGlucose_Service.h"


// Glucose Service is mandatory
#if (BLE_SUPPORT_GLUCOSE_SERVICE == 0)
#error BLE_SUPPORT_GLUCOSE_SERVICE shall be enabled when BleGlucose SENSOR Role is enabled
#endif //(BLE_SUPPORT_GLUCOSE_SERVICE == 0)

// device Information Service is mandatory
#if (BLE_SUPPORT_DEVICEINFORMATION_SERVICE == 0)
#error BLE_SUPPORT_DEVICEINFORMATION_SERVICE shall be enabled when BleGlucose SENSOR Role is enabled
#endif //(BLE_SUPPORT_DEVICEINFORMATION_SERVICE == 0)

#if (BLEDEVICEINFORMATION_SUPPORT_SYSTEMID == 0) 
#error BLEDEVICEINFORMATION_SUPPORT_SYSTEMID shall be enabled when BleGlucose SENSOR Role is enabled
#endif //(BLEDEVICEINFORMATION_SUPPORT_SYSTEMID == 0) 

#if (BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER == 0) 
#error BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER shall be enabled when BleGlucose SENSOR Role is enabled
#endif //(BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER == 0) 

#if (BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME == 0) 
#error BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME shall be enabled when BleGlucose SENSOR Role is enabled
#endif //(BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME == 0) 



/****************************************************************************\
 * OPTIONAL FEATURES FOR GLUCOSE SENSOR PROFILE
\****************************************************************************/

/****************************************************************************\
 *	CONSTANTS definition
\****************************************************************************/

/****************************************************************************\
 *	TYPES definition
\****************************************************************************/


/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/

/** Register a BLE GLUCOSE profile in SENSOR Role.
 *
 * BLEGLUCOSE_SENSOR_Register()
 *	This function is used to register and initialise a BLE GLUCOSE profile 
 *  in SENSOR role.
 *  It will add the supported GLUCOSE Service characteristics and
 *  the supported DEVICE INFORMATION Service characteristics into the 
 *  Attribute database.
 *	This Interface shall be the entry point of a BLE GLUCOSE profile
 *  in SENSOR Role.
 *
 * BLEGLUCOSE_SUPPORT_SENSOR shall be enabled.
 *
 * @measurementRecords : Pointer to Glucose Measurement table
 * @numberOfMeasurement : Maximum number of measurement 
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Guillaume FERLIN
 */
BleStatus BLEGLUCOSE_SENSOR_Register(     
    BleGlucoseServiceMeasurement *measurementRecords,
    U16                          numberOfMeasurement);

/****************************************************************************\
 * OPTIONAL API functions definition GLUCOSE SENSOR PROFILE
\****************************************************************************/

#endif //(BLEGLUCOSE_SUPPORT_SENSOR== 1)

#endif /*__BLEGLUCOSE_SENSOR_H*/
