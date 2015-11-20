#ifndef __BLETXPOWERLEVEL_SERVICE_H
#define __BLETXPOWERLEVEL_SERVICE_H
/****************************************************************************
 *
 * File:          BleTxPowerLevel_Service.h
 *
 * Description:   Contains routines declaration for Tx Power Level Service.
 * 
 * Created:       march, 2011
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

/***************************************************************************\
 * CONSTANTS
\***************************************************************************/

/**
 * BLE_SUPPORT_TXPOWERLEVEL_SERVICE
*	Defines if the BLE local device enables access to the local  
 *	Tx Power Level service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles to get access to the  
 * Tx Power Level Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_TXPOWERLEVEL_SERVICE
#define BLE_SUPPORT_TXPOWERLEVEL_SERVICE							0
#endif //BLE_SUPPORT_TXPOWERLEVEL_SERVICE

/***************************************************************************\
 * OPTIONAL FEATURES
\***************************************************************************/
#if ( BLE_SUPPORT_TXPOWERLEVEL_SERVICE == 1 )

/** TXPOWERLEVEL_IS_FIXED_VALUE
 * Define if the transmit Power is fixed for a given connexion 
 * Or if it is adaptative (it may change during the life time of the
 * connection).
 * If set to a fixed vlaue ( set to 1 ) the Tx Power level will be retrieved
 * only once in the connection
 * if set to a variable value (set to 0) the current connection TX POWER 
 * LEVEL will be retrieved every the defined TXPOWERLEVEL_RETRIEVE_TIMER 
 * milliseconds.
 *
 * The default value for this option is fixed (1).
 */
#ifndef TXPOWERLEVEL_IS_FIXED_VALUE
#define TXPOWERLEVEL_IS_FIXED_VALUE									1
#endif //TXPOWERLEVEL_IS_FIXED_VALUE


/** TXPOWERLEVEL_RETRIEVE_TIMER
 * Define the number of milliseconds to wait before two retrieves.
 * If set to a fixed vlaue ( set to 1 ) the Tx Power level will be
 * retrieved.
 *
 * The default value for this option is fixed 3000ms .
 */
#ifndef TXPOWERLEVEL_RETRIEVE_TIMER
#define TXPOWERLEVEL_RETRIEVE_TIMER									3000
#endif //TXPOWERLEVEL_RETRIEVE_TIMER
/***************************************************************************\
 *	Type definition
\***************************************************************************/

/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
#endif //( BLE_SUPPORT_TXPOWERLEVEL_SERVICE == 1 )

#endif //__BLETXPOWERLEVEL_SERVICE_H
