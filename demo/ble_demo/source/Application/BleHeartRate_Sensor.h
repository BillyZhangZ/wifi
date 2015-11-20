#ifndef __BLEHEARTRATE_SENSOR_H
#define __BLEHEARTRATE_SENSOR_H
/****************************************************************************
 *
 * File:          BleHeartRate_Sensor.h
 *
 * Description:   Contains interfaces definitions for Heart Rate 
 *					Profile when the local device operates in the SENSOR
 *					role.
 * 
 * Created:       December, 2010
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


/***************************************************************************\
 *	CONFIGURATION
\***************************************************************************/

/**
 * BLEHEARTRATE_SUPPORT_SENSOR
 * Defines if the BLE HEARTRATE SENSOR Profile implementation supports the 
 * SENSOR Role
 * If enabled ( set to 1 ) it enables all the following BLE HEARTRATE 
 * SENSOR Profile SENSOR configurations and Application Interfaces.
 * The default value for this option is disabled (0).
 */
#ifndef BLEHEARTRATE_SUPPORT_SENSOR
#define BLEHEARTRATE_SUPPORT_SENSOR					0
#endif //BLEHEARTRATE_SUPPORT_SENSOR

#if (BLEHEARTRATE_SUPPORT_SENSOR== 1)

// Then check dependencies
#include "Services/BleDeviceInformation_Service.h"
#include "Services/BleHeartRate_Service.h"


// Heart Rate Service is mandatory
#if (BLE_SUPPORT_HEARTRATE_SERVICE == 0)
#error BLE_SUPPORT_HEARTRATE_SERVICE shall be enabled when BleHeartRate SENSOR Role is enabled
#endif //(BLE_SUPPORT_HEARTRATE_SERVICE == 0)

// device Information Service is mandatory
#if (BLE_SUPPORT_DEVICEINFORMATION_SERVICE == 0)
#error BLE_SUPPORT_DEVICEINFORMATION_SERVICE shall be enabled when BleHeartRate SENSOR Role is enabled
#endif //(BLE_SUPPORT_DEVICEINFORMATION_SERVICE == 0)

#if (BLEDEVICEINFORMATION_SUPPORT_SYSTEMID == 0) 
#error BLEDEVICEINFORMATION_SUPPORT_SYSTEMID shall be enabled when BleHeartRate SENSOR Role is enabled
#endif //(BLEDEVICEINFORMATION_SUPPORT_SYSTEMID == 0) 

#if (BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER == 0) 
#error BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER shall be enabled when BleHeartRate SENSOR Role is enabled
#endif //(BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER == 0) 

#if (BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME == 0) 
#error BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME shall be enabled when BleHeartRate SENSOR Role is enabled
#endif //(BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME == 0) 



/***************************************************************************\
 * OPTIONAL FEATURES FOR HEARTRATE SENSOR PROFILLE
\***************************************************************************/

/***************************************************************************\
 *	CONSTANTS definition
\***************************************************************************/

/***************************************************************************\
 *	TYPES definition
\***************************************************************************/


/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/

/** Register a BLE HEART RATE profile in SENSOR Role.
 *
 * BLEHEARTRATE_SENSOR_Register()
 *	This function is used to register and initialise a BLE HEART RATE profile 
 *  in SENSOR role.
 *  It will add the supported HEART RATE Service characteristics and
 *  the supported DEVICE INFORMATION Service characteristics into the 
 *  Attribute database.
 *	This Interface shall be the entry point of a BLE HEART RATE profile
 *  in SENSOR Role.
 *
 * BLEHEARTRATE_SUPPORT_SENSOR shall be enabled.
 *
 * @param callback: the BleHeartRateServiceCallback in wich the service
 *			specific event will be received by the application,
 *			note that the only event received in related by the remote
 *			requesting the reset of the energy expended count.
 *			This single event is received only if 
 *			BLE_HEARTRATE_SERVICE_SUPPORT_ENERGYEXPENDED is set to enabled,
 *			otherwise the callabck is ignored and the callback parameter 
 *			can be set to (BleHeartRateServiceCallback)0.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEHEARTRATE_SENSOR_Register( 
							BleHeartRateServiceCallback callback  );


/***************************************************************************\
 * OPTIONAL API functions definition HEARTRATE SENSOR PROFILE
\***************************************************************************/

#endif //(BLEHEARTRATE_SUPPORT_SENSOR== 1)

#endif /*__BLEHEARTRATE_SENSOR_H*/
