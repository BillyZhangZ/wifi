#ifndef __BLENETWORKAVAILABILITY_SERVICE_H
#define __BLENETWORKAVAILABILITY_SERVICE_H
/****************************************************************************
 *
 * File:          BleNetworkAvailability_Service.h
 *
 * Description:   Contains routines declaration for Network Availability Service.
 * 
 * Created:      July, 2013
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.2 $
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
 * CONFIGURATION
\***************************************************************************/
/**
 * BLE_SUPPORT_NETWORKAVAILABILITY_SERVICE
*	Defines if the BLE local device enables access to the local Network 
 *	Availability service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles to get access to the network
 * Availability Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_NETWORKAVAILABILITY_SERVICE
#define BLE_SUPPORT_NETWORKAVAILABILITY_SERVICE							0
#endif //BLE_SUPPORT_NETWORKAVAILABILITY_SERVICE

/***************************************************************************\
 * OPTIONAL FEATURES
\***************************************************************************/

/***************************************************************************\
 * CONSTANTS
\***************************************************************************/
#define BLEINFOTYPE_NETWORKAVAILABILITYSERVICE_CLIENTCONFIG				0x69

/***************************************************************************\
 *	Type definition
\***************************************************************************/
/** BleNetworkAvailabilityServiceNetworkAvailability  type 
* Define the different kind of network value that could be used by user
* There are two values: 
*  0 : Network is not available
*  1 : Network is available
*/
typedef U8 BleNetworkAvailabilityServiceNetworkAvailability;

/** BLENETWORKAVAILABILITYSERVICE_NETWORKAVAILABILITY_NONETWORKAVAILABLE  
 * The network value is 0, so there is no network available
 */
#define BLENETWORKAVAILABILITYSERVICE_NETWORKAVAILABILITY_NONETWORKAVAILABLE              0

/** BLENETWROKAVAILABILITYSERVICE_NETWORKAVAILABILITY_ONEORMORENETWORKAVAILABLE 
 * The network value is 1, it means the network is available.
 */
#define BLENETWROKAVAILABILITYSERVICE_NETWORKAVAILABILITY_ONEORMORENETWORKAVAILABLE       1

/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
#if (BLE_SUPPORT_NETWORKAVAILABILITY_SERVICE == 1)
/**
 * BLENETWORKAVAILABILITY_SetNetworkAvailability()
 *	This function is used to localy Set the Network value
 *  Note that upon success, the networkLevel can be considered to be set, No
 *  callback event will be reported in the registered.
 *
 * BLE_SUPPORT_NETWORKAVAILABILITY_SERVICE shall be enabled.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, merely because
 * Network Availability Service is not registered by any profile.
 *
 * @author My Huong NHAN
 */
BleStatus BLENETWORKAVAILABILITYSERVICE_SetNetworkAvailability( 
                  BleNetworkAvailabilityServiceNetworkAvailability networkValue );
#endif //(BLE_SUPPORT_NETWORKAVAILABILITY_SERVICE == 1)

#endif //__BLENETWORKAVAILABILITY_SERVICE_H
