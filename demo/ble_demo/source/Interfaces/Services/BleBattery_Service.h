#ifndef __BLEBATTERY_SERVICE_H
#define __BLEBATTERY_SERVICE_H
/****************************************************************************
 *
 * File:          BleBattery_Service.h
 *
 * Description:   Contains routines declaration for Battery Service.
 * 
 * Created:       January, 2012
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

#include "BleTypes.h"


/***************************************************************************\
 * OPTIONAL FEATURES
\***************************************************************************/
/**
 * BLE_SUPPORT_BATTERY_SERVICE
*	Defines if the BLE local device enables access to the local  
 *	Battery service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles or directly an Application 
 * to get access to the BATTERY Service and characteristic.
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_BATTERY_SERVICE
#define BLE_SUPPORT_BATTERY_SERVICE								0
#endif //BLE_SUPPORT_BATTERY_SERVICE

#if (BLE_SUPPORT_BATTERY_SERVICE == 1)

/***************************************************************************\
 * CONSTANTS
\***************************************************************************/
/* BLEINFOTYPE_BATTERYLEVEL_CLIENTCONFIG 
 * The Unique descriptor ID for a two Bytes length information to save
 * in persistent memory representing the client config for a given remote
 * bonded device.
 * It is the Unique identifier passed in the infotype fields when the PROFILE
 * call the system APIs SYSTEM_SetPersistentInformation(BD_ADDR addr,
 * U8 infoType, U8* infoValue,U8 InfoLen) and 
 * SYSTEM_GetPersistentInformation(BD_ADDR addr, U8 infoType, U8 **infoValue,
 * U8 *InfoLen). 
 */
#define BLEINFOTYPE_BATTERYLEVEL_CLIENTCONFIG						0x8E	



/***************************************************************************\
 *	Type definition
\***************************************************************************/


/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/

/** Register a Battery Service.
 *
 * BLEBATTERY_Register()
 *	This function is used to register and initialise a Battery
 *	Service.
 * It will add the Battery service and characteristics into the
 * Attribute database.
 *
 * BLE_SUPPORT_BATTERY_SERVICE shall be enabled.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, merely because
 * Health Thermometer Service is already registered.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEBATTERYSERVICE_Register( void );


/** Set the Battery level
 *
 * BLEBATTERYSERVICE_SetBatteryLevel()
 *	This function is used to set the Battery Level value
 *
 * BLE_SUPPORT_BATTERY_SERVICE shall be enabled.
 *
 * @param batteryLevel :  The current battery level as a percentage from 0% 
 *          to 100%; 0% represents a battery that is fully discharged, 100%
 *          represents a battery that is fully charged. 
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
BleStatus BLEBATTERYSERVICE_SetBatteryLevel(
						U8 batteryLevel);



#endif //(BLE_SUPPORT_BATTERY_SERVICE == 1)
#endif //__BLEBATTERY_SERVICE_H
