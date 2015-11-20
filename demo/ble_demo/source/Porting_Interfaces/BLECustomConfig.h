/****************************************************************************
 *
 * File:          BLECustomConfig.h
 *
 * Description:   Contains stack specific configuration.
 * 
 * Created:       June, 2008
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

/**@file BLECustomConfig.h
 * @section BLECustomConfig.h description
 * 
 * This file allows the application to fine tune the STACK and PROFILES 
 * configuration to fit the most as possible with the application requested 
 * features.
 *
 * This file is included within the whole STACK and PROFILES prior any other 
 * header files, so configuration defined/overrided in this file will be   
 * supersede any other CORE STACK and PROFILES configuration.
 *
 * So it override:
 * - The CORE STACK configuration defined in bleConfig.h
 * - The ATTribute protocol configuration defined in attConfig.h
 * - Any SERVICE configuration defined in SERVICEs interfaces
 * - Any PROFILE configuration defined in PROFILEs interfaces
 *		( including GATT and GAP PROFILEs)
 * - maybe the STACK default types (U8, U16, ...) defined in bleTypes.h
 * 
 */

#define	CONST_DECL												const
#define UNUSED_PARAMETER(P)										(P = P)
#define ATT_SERVER_SUPPORT_PREPARE_EXECUTE_WRITE_REQ			1
#define ATT_SERVER_SUPPORT_READ_BLOB_REQ						1
#define BLE_SUPPORT_HEARTRATE_SERVICE               1
#define BLEHEARTRATE_SUPPORT_SENSOR               1
#define BLE_HEARTRATE_SERVICE_SUPPORT_BODYSENSORLOCATION               1
#define BLE_HEARTRATE_SERVICE_SUPPORT_ENERGYEXPENDED               1
#define BLE_SUPPORT_DEVICEINFORMATION_SERVICE               1
#define BLEDEVICEINFORMATION_SUPPORT_SYSTEMID               1
#define BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER               1
#define BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME               1
#define BLE_ROLE_SCANNER               1
#define BLE_ROLE_ADVERTISER               1
#define BLE_PARMS_CHECK               1
#define BLE_CONNECTION_SUPPORT               1
#define BLE_SECURITY               1
#define BLECONTROLLER_USED               1
#define BLE_ENABLE_VENDOR_SPECIFIC               1
#define BLECONTROLLER_NEED_SPECIFIC_INIT               1
#define ATTRIBUTE_PROTOCOL               1
#define BLE_PROTECT_ISR_FUNCTION_CALL               1
#define BLE_USE_RESTRICTED_LOCAL_MEMORY               1
#define BLE_USE_INTERNAL_MEMORY_FUNCTIONS				0
#define BLE_PRINT_DEBUG_TRACES				1
#define ATT_DEBUG_TRACES										1
#define SMP_DEBUG_TRACES										1
#if 0

/***************************************************************************\
 * PLATFORM Options
\***************************************************************************/
// Platform specific

#ifndef BLECUSTOMCONFIG_H_
#define BLECUSTOMCONFIG_H_

#define BLE_PROTECT_ISR_FUNCTION_CALL							1
#define BLE_USE_RESTRICTED_LOCAL_MEMORY							1
#define BLETYPES_ALREADY_DEFINED 								0
#define BLECONTROLLER_NEED_SPECIFIC_INIT						1
#define BLECONTROLLER_USED						 BLECONTROLLER_EM
#define BLE_EM9301_LOADPATCH									0

//place the constant declaration in code area
#define	CONST_DECL												const
#define UNUSED_PARAMETER(P)										(P = P)

/***************************************************************************\
 * CORE STACK Options
\***************************************************************************/

#define BLE_ROLE_SCANNER										1
#define	BLE_ROLE_ADVERTISER										1

#define	BLE_ADVERTSING_TX_POWER_SUPPORT							0
#define BLE_PARMS_CHECK											1
#define	BLEERROR_HANDLER										0
#define BLE_CONNECTION_SUPPORT									1
#define BLE_RANDOM_ADDRESSING_SUPPORT							0
#define BLE_SECURITY											0
#define BLE_SM_SIGNATURE_SUPPORT								0
#define BLE_WHITE_LIST_SUPPORT									0
#define BLE_ENABLE_TEST_COMMANDS								0
#define BLE_USE_HOST_CHANNEL_CLASSIFICATION						0
#define BLE_ENABLE_VENDOR_SPECIFIC								1
#define BLEL2CAP_ENABLE_API										0

#define SM_IO_CAPABILITIES  				SM_IO_NOINPUTNOOUTPUT

/***************************************************************************\
 * ATT Options
\***************************************************************************/

#define ATTRIBUTE_PROTOCOL										1
#define ATT_ROLE_CLIENT											1
#define ATT_SERVER_SUPPORT_PREPARE_EXECUTE_WRITE_REQ			1
#define ATT_SERVER_SUPPORT_READ_BLOB_REQ						1

/***************************************************************************\
 * GATT Options
\***************************************************************************/

#define GENERIC_ATTRIBUTE_PROFILE								1
#define BLEGATT_SUPPORT_SINGLE_SERVICE_DISCOVERY				1
#define BLEGATT_SUPPORT_ALL_CHARACTERISTIC_DISCOVERY			1
#define BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_TYPE		1
#define BLEGATT_SUPPORT_GET_CHARACTERISTIC_DESCRIPTOR_LIST		1
#define BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_VALUE				1

/***************************************************************************\
 * LOGGING Options
\***************************************************************************/

#define BLE_PRINT_DEBUG_TRACES									1
#define ATT_DEBUG_TRACES										1
#define SMP_DEBUG_TRACES										1
#if (BLE_PRINT_DEBUG_TRACES == 1)
#define BLEHCI_HANDLER											0
#endif //(BLE_PRINT_DEBUG_TRACES == 1)

/***************************************************************************\
 * SERVICES Options
\***************************************************************************/
//They will be defined in the Service custom configuration based on this one
#define BLE_SUPPORT_IMMEDIATEALERT_SERVICE						0
#define BLE_SUPPORT_HID_SERVICE									1
#define BLE_HIDSERVICE_SUPPORT_BOOTPROTOCOLMODE					1
#define BLE_SUPPORT_BATTERY_SERVICE								1
#define BLE_SUPPORT_DEVICEINFORMATION_SERVICE					1
#define BLEDEVICEINFORMATION_SUPPORT_PNPID						1
#define BLE_USE_INTERNAL_MEMORY_FUNCTIONS 0

/***************************************************************************\
 * PROFILES Options
\***************************************************************************/
//They will be defined in the Profile custom configuration based on this one
#define BLEFINDME_SUPPORT_TARGET								0
#define BLEALPWDATAEXCHANGE_SUPPORT_CLIENT						0
#define BLEALPWDATAEXCHANGECLIENT_SUPPORT_SAVINGINFORMATION		0
#define BLEALPWDATAEXCHANGE_SUPPORT_SERVER						0
#define BLEHID_SUPPORT_DEVICE									1


#endif /* BLECUSTOMCONFIG_H_ */

#endif