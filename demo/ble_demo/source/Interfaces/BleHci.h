#ifndef __BLEHCI_H
#define __BLEHCI_H
/****************************************************************************
 *
 * File:          BleHci.h
 *
 * Description:   Contains Application interfaces routines for BLEHCI.
 * 
 * Created:       January, 2009
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

#include "BleTypes.h"


#if (BLE_ENABLE_VENDOR_SPECIFIC == 1)

/***************************************************************************
 *	Type definition
 ***************************************************************************/

/** Definition for the BleHciVendorCommandComplete type
 *
 *	This type is used during the reception of a 
 *  BLEEVENT_VENDOR_COMMAND_COMPLETE event by the application
 */
typedef struct {
	/** 
	 * The vendor specific command opcode for which the command complete 
	 * event have been received.
	 */
	U16 opcode;

	/** 
	 * The returns parameters of the command.
	 */
	U8	*parms;

} BleHciVendorCommandComplete;


/** Definition for the BleHciVendorCommandStatus type
 *
 *	This type is used during the reception of a 
 *  BLEEVENT_VENDOR_COMMAND_STATUS event by the application
 */
typedef struct {
	/** 
	 * The vendor specific command opcode for which the command status 
	 * event have been received.
	 */
	U16 opcode;

	/** 
	 * The status parameter of the command.
	 */
	U8	status;

} BleHciVendorCommandStatus;

/** Definition for the BleHciVendorEvent type
 *
 *	This type is used during the reception of a 
 *  BLEEVENT_VENDOR_EVENT event by the application
 */
typedef struct {
	/** 
	 * The event code of the vendor specific event received.
	 */
	//U8 eventCode; //always 0xFF

	/** 
	 * The parameters length.
	 */
	U8	parmsLen;

	/** 
	 * The parameters.
	 */
	U8	*parms;

} BleHciVendorEvent;

#endif //(BLE_ENABLE_VENDOR_SPECIFIC == 1)

/****************************************************************************
 *	APPLICATION INTERFACE functions definition
 ****************************************************************************/

#if (BLE_ENABLE_VENDOR_SPECIFIC == 1) 
/* Send a vendor specific HCI command.
 *
 * BLEHCI_SendVendorSpecificCommand():
 *	Send a vendor specific command. when an application to send a vendor
 *	specific command, it can use this interface.
 *	The opcode of the command to send shall have an OGF of 0x3F, and the 
 *  command should generate a command complete or a command status event.
 *
 *  Upon the controller answer to this command, the application callback
 *	will receive a BLEEVENT_VENDOR_COMMAND_COMPLETE event or a
 *	BLEEVENT_VENDOR_COMMAND_STATUS event.
 *
 * BLE_ENABLE_VENDOR_SPECIFIC shall be enabled
 *
 *  opCode	The opcode of the command to send, the OGF shall be 0x3F
 *					 otherwise the function fails
 *  parmsLen  The parameters data length
 *  parms     The parameters data
 *
 *
 *  The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded
 *	- BLESTATUS_FAILED indicates that the operation has failed
 */
BleStatus BLEHCI_SendVendorSpecificCommand(U16 opCode, U8 parmsLen, U8* parms);

#endif //(BLE_ENABLE_VENDOR_SPECIFIC == 1) 

#endif /*__BLEHCI_H*/
