#ifndef __BLEALPWDATAEXCHANGE_SERVICE_H
#define __BLEALPWDATAEXCHANGE_SERVICE_H
/****************************************************************************
 *
 * File:          BleAlpwDataExchange_Service.h
 *
 * Description:   Contains routines declaration for Alpwise Data Exchange Service.
 * 
 * Created:       march, 2011
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.6 $
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
 * BLE_SUPPORT_ALPWDATAEXCHANGE_SERVICE
*	Defines if the BLE local device enables access to the local immediate  
 *	Alert service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles to get access to the immediate 
 * Alert Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_ALPWDATAEXCHANGE_SERVICE
#define BLE_SUPPORT_ALPWDATAEXCHANGE_SERVICE								0
#endif //BLE_SUPPORT_ALPWDATAEXCHANGE_SERVICE


/***************************************************************************\
 * CONSTANTS
\***************************************************************************/
/* BLEINFOTYPE_BLEALPWDATAEXCHANGE_CLIENTCONFIG 
 * The Unique descriptor ID for a two Bytes length information to save
 * in persistent memory representing the client config for a given remote
 * bonded device.
 * It is the Unique identifier passed in the infotype fields when the PROFILE
 * call the system APIs SYSTEM_SetPersistentInformation(BD_ADDR addr,
 * U8 infoType, U8* infoValue,U8 InfoLen) and 
 * SYSTEM_GetPersistentInformation(BD_ADDR addr, U8 infoType, U8 **infoValue,
 * U8 *InfoLen). 
 */
#define BLEINFOTYPE_BLEALPWDATAEXCHANGE_CLIENTCONFIG				0x6F

/***************************************************************************\
 *	Type definition
\***************************************************************************/
/** 
 * BleAlpwDataExchangeEvent type
 * Define the different kind of events that could be received by the 
 * BleAlpwDataExchangeCallBack
 */
typedef U8 BleAlpwDataExchangeEvent;

/** BLEALPWDATAEXCHANGE_EVENT_RXDATA
 * Event received when a new data is received from the CLIENT
 * The status field indicate the status of the operation (BLESTATUS_SUCCESS or
 * BLESTATUS_FAILED).
 * The parms field indicates the data information within the 
 * BleAlpwDataExchangeRxData type.
 */
#define BLEALPWDATAEXCHANGE_EVENT_RXDATA								0xA6

/** BLEALPWDATAEXCHANGE_EVENT_TXCOMPLETE
 * Event received when the data has been transmitted (or not) to the CLIENT
 * The status field indicate the status of the operation (BLESTATUS_SUCCESS or
 * BLESTATUS_FAILED).
 * The parms field is not used.
 */
#define BLEALPWDATAEXCHANGE_EVENT_TXCOMPLETE							0xA2

/** 
 * BleAlpwDataExchangeServerRxData type
 * Define what is received by the 
 * BleAlpwDataExchangeCallBack during BLEALPWDATAEXCHANGE_EVENT_RXDATA event
 * it indicates:
 * connHandle : The connection from which the data is coming
 * data : The data received
 * dataLen : The data length received
 */
typedef struct{

	U16 connHandle;
	U8	*rxData;
	U8	rxDataLen;

} BleAlpwDataExchangeServerRxData;

/**
 * BleAlpwDataExchangeCallBack
 *	This callback receives the ALPWDATAEXCHANGE SERVICE events. 
 *  Each events may be assiociated with specified satus and parameters.
 *  The callback is executed during the stack context,  be careful to
 *	 not doing heavy process in this function.
 */
typedef void (*BleAlpwDataExchangeCallBack)(BleAlpwDataExchangeEvent event,
									BleStatus status,
									void *parms); 

/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/


#endif //__BLEALPWDATAEXCHANGE_SERVICE_H
