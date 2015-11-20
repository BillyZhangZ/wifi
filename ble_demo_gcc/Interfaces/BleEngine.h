#ifndef __BLEENGINE_H
#define __BLEENGINE_H
/****************************************************************************
 *
 * File:          BleEngine.h
 *
 * Description:   Contains interfaces routines for Stack initialisation,
 *					de-initialisation and execution.
 * 
 * Created:       June, 2008
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.30 $
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

/*---------------------------------
 * BLE Stack Events
 */

/**
 * BleEvent
 *	Event received in the BleCallback callbacks registered during the call
 *	of BLEMGMT_RegisterHandler() function.
 */
typedef U8 BleEvent;

/**
 *	Event followed the call of BLEMGMT_Init().
 *	The status field will indicates completion of the initialisation process
 *	- BLESTATUS_SUCCESS indicates that the BLE stack has been successfully
 *		initialized.
 *	- BLESTATUS_FAILED indicates that the BLE stack could not be
 *		initialized.
 *	if BLE_PARMS_CHECK is enabled
 *	- BLESTATUS_INVALID_PARMS indicates that the BLE stack has not be
 *		initialized cause an invalid configuration:
 *		The Controller does not support SCANNER or ADVERTISER role:
 *		reconsider BLE_ROLE_ADVERTISER or BLE_ROLE_SCANNER to optimize.
 */
#define BLEEVENT_INITIALIZATION_RSP							0x01

/**
 *	Event followed the reception of a HARDWARE error event by the controller.
 *  Upon reception of This event the stack will automatically De-init, It is
 * Up to the application to re-init the stack.
 *	The status field is always BLESTATUS_FAILED
 *	The parms field indicates the Hardware Error event error code as U8 *.
 */
#define BLEEVENT_HARDWARE_ERROR								0x02


#if (BLE_SECURITY == 1)
/**
 * Event reported when a remote device ( MASTER on the connection ) 
 * has started a pairing procedure.
 * Depending of the SM_IO_CAPABILITIES a passkey may be requested after
 * upon the BLEEVENT_PASSKEY_REQUEST event.
 *
 *	The status field is always BLESTATUS_SUCCESS.
 *  The param field containing a pointer to a BlePairingInfo structure 
 *  indicating the connection handle on which the Pairing procedure has
 *  started.
 *  BlePairingInfo *info = (BlePairingInfo *) param;
 *  Note that the pointer is valid only during the callback.
 *  During this event, BLESMP_ConfirmPairing may be called to accept the
 *  pairing request; otherwise, the pairing request is automatically rejected.
 */
#define	BLEEVENT_PAIRING_REQUEST							0x20

/**
 * Event reported when a Passkey need to be provided in order to continue 
 * the pairing procedure.
 *
 *	The status field is always BLESTATUS_SUCCESS.
 *  The param field containing a pointer to a BlePairingInfo structure 
 *  indicating the connection handle on which the Pairing procedure has
 *  started.
 *  BlePairingInfo *info = (BlePairingInfo *) param;
 *  Note that the pointer is valid only during the callback
 *  A passkey shall be 6 characters in length, with each character
 *  value in the range 0x30-0x39 ['0'..'9']. 
 */
#define	BLEEVENT_PASSKEY_REQUEST							0x21

/**
 * Event reported when a Passkey need to be displayed in order to continue 
 * the pairing procedure.
 *
 *	The status field is always BLESTATUS_SUCCESS.
 *  The param field containing a pointer to a BlePairingInfo structure 
 *  indicating the connection handle on which the Pairing procedure has
 *  started as well that the passkey to be displayed..
 *  BlePairingInfo *info = (BlePairingInfo *) param;
 *  Note that the pointer is valid only during the callback
 *  A passkey to display is 6 characters in length, with each character
 *  value in the range 0x30-0x39 ['0'..'9']. 
 */
#define BLEEVENT_PASSKEY_DISPLAY							0x22

/**
 * Event followed the call of BLESMP_InitiatePairing() or a 
 * BLEEVENT_PAIRING_REQUEST event, it indicates the completion of the
 * pairing operation.
 *
 *	The status field indicates status of the operation
 *	- BLESTATUS_SUCCESS indicates that the operation has succeded.
 *	- BLESTATUS_FAILED indicates that the operation has failed. 
 *
 *  The param field containing a pointer to a BlePairingInfo structure 
 *  indicating the connection handle on which the Pairing procedure has
 *  completed. In case of failure, the reason field is valid and contains 
 *  a SmErrorCode describing the reason of the paring failure.
 *  BlePairingInfo *info = (BlePairingInfo *) param;
 *  Note that the pointer is valid only during the callback
 */
#define BLEEVENT_PAIRING_COMPLETE							0x23
#endif //(BLE_SECURITY == 1)

#if (BLE_RANDOM_ADDRESSING_SUPPORT == 1)
/**
 * Event followed the call of BLESMP_GenerateRandomAddress().
 *
 *	The status field indicates status of the operation
 *	- BLESTATUS_SUCCESS indicates that the operation has succeed, the 
 *  param field containing a pointer to an BD_ADDR indicating the new  
 *  random address.
 *  BD_ADDR *addr = (BD_ADDR *) param;
 *  Note that the pointer is valid until the next
 *	BLESMP_GenerateRandomAddress() call.
 *	In other cases this parameter is zero.
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 */
#define BLEEVENT_GENERATE_RANDOM_ADDRESS_RSP				0x24
#endif //(BLE_RANDOM_ADDRESSING_SUPPORT == 1)

#if (BLE_ENABLE_VENDOR_SPECIFIC == 1)
/**
 * Event that may follow the sent of a vendor specific command.
 *
 *	The param field containing a pointer to a BleHciVendorCommandComplete
 *  structure indicating the opcode of the command which is complete and
 *  the return parameters.
 *  All the BleHciVendorCommandComplete values are only valid during
 *	the callback.
 *	The status field is always BLESTATUS_SUCCESS 
 */
#define BLEEVENT_VENDOR_COMMAND_COMPLETE					0x25

/**
 * Event that may follow the sent of a vendor specific command.
 *
 *	The param field containing a pointer to a BleHciVendorCommandStatus
 *  structure indicating the opcode of the command which is complete.
 *  All the BleHciVendorCommandStatus values are only valid during
 *	the callback.
 *	The status field is always BLESTATUS_SUCCESS 
 */
#define BLEEVENT_VENDOR_COMMAND_STATUS						0x26

/**
 * Event indicating that a vendor specific event have been received
 *	by the stack (event code 0xFF).
 *
 *	The param field containing a pointer to a BleHciVendorEvent
 *  structure indicating the parameter length and the
 *  parameters of the event.
 *  All the BleHciVendorEvent values are only valid during
 *	the callback.
 *	The status field is always BLESTATUS_SUCCESS 
 */
#define BLEEVENT_VENDOR_EVENT								0x27
#endif //(BLE_ENABLE_VENDOR_SPECIFIC == 1)


/*---------------------------------
 * BLE Stack Callback
 */

/**
 * BleCallback
 *	This callback receives the BLE stack events. Each of these events can be
 *	associated with a defined status and parameter. The callback is executed
 *	during the stack context, please be careful to not doing heavy process
 *	in this function.
 */
typedef void (*BleCallback)(BleEvent event, BleStatus status, void *param);

/**
 * BleHandler
 *  Handler to provide in order to register a callback during the call of 
 *  BLEMGMT_RegisterHandler() function.
 */
typedef struct{
	/** The address of a callback function that will receive the BLE stack
	 *  event.
	 */
	BleCallback callback;

	//for internal use only
	void* internal[2];

} BleHandler;

/**
 * BleHardwareInformation
 * Structure used to describe the current Hardware information
 * manufacturer name and manufacturer revsion
 */
typedef struct{
	/** The Bluetooth Low Energy Controller manufacturer name.
	 * It is one of the defined Bluetooth SIG company ID in assigned numbers 
	 */
	U16		manufacturerName;

	/** The Bluetooth Low Energy Controller current LMP firmware version.
	 * It is manufacturer dependant
	 */
	U16		lmpSubVersion;

} BleHardwareInformation;

/***************************************************************************\
 * External Functions
\***************************************************************************/



/* Initialise the whole BLE stack, including transport, radio ...
 *
 * BLEMGMT_Init():
 *	This function is used to initialise the BLE stack. It shall be the entry
 *  point of an application using it and it shall be called before any other
 *	stack interfaces.
 *  It initializes:
 *	- Some global context and internal states.
 *	- The transport layer
 *	- The radio layer
 *	- The Core layer (BLE-HCI, L2CAP...)
 *
 * return: The status of the operation:
 *	- BLESTATUS_PENDING indicates that the BLE stack initialisation
 *		has been successfully started, application will received the 
 *		BLEEVENT_INITIALIZATION_RSP event with the completion status.
 *
 *	- BLESTATUS_FAILED indicates that the BLE stack could not be
 *		initialized
 *	
 */
BleStatus BLEMGMT_Init(void);


/* De-initialise the whole BLE stack, including transport, radio ...
 *
 * BLEMGMT_Deinit():
 *	This function is used to de-initialise the BLE stack. It shutdown the
 *	BLE software
 *  It de-initializes:
 *	- Some global context and internal states.
 *	- The Core layer (BLE-HCI, L2CAP...)
 *	- The radio layer
 *	- The transport layer
 *
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the BLE stack has been successfully
 *		de-initialized
 *	- BLESTATUS_FAILED indicates that the BLE stack de-initialization has 
 *		failed
 *	
 */
BleStatus BLEMGMT_Deinit(void);

/* The stack itself request to be executed once
 *
 * BLEMGMT_NotifyToRun()
 *	The BLE stack requests to be executed. BLEMGMT_Run() shall be called.
 *  if the stack is executed in a separate thread, the thread shall be
 *  unlocked here.
 *
 * see BLEMGMT_Run()
 *
 * return none
 */
void BLEMGMT_NotifyToRun(void);

/* Run the BLE stack in order to execute its internal treatments
 *
 * BLEMGMT_Run():
 *	This function is used to run the BLE stack (may following a call by
 *	the stack of BLEMGMT_NotifyToRun() ).
 *
 *	It could be called by a thread or by a scheduler.
 *	if the BLE stack itself need to run it call the BLEMGMT_NotifyToRun()
 *	interface to inform application that it need to call BLEMGMT_Run().
 *
 *	This function shall not be called it the stack context, for example
 *	it shall not be called in the callback registered by 
 *  BLEMGMT_RegisterHandler() or in the system interface
 *	BLEMGMT_NotifyToRun(). 
 *
 * see BLEMGMT_NotifyToRun()
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation is successfull
 *	
 */
BleStatus BLEMGMT_Run(void);


/* Register a callback function to receive BleEvent
 *
 * BLEMGMT_RegisterHandler():
 *	This function is used to register a handler containing a callback
 *	function that will receive the stack events.
 *  You may register as many handlers as you want.
 *
 *	handler: A valid pointer to a BleHandler containing the address of a callback
 *  function that will receive the BLE stack event.
 *	The callback function must be declared as follow:
 *
 *		void myCallback(BleEvent event, BleStatus status, void *param);
 *
 * When the BLE stack wants to notify the application with an existing event
 *	it calls this function.
 * The handler pointer shall be valid until the call of BLEMGMT_DeRegisterHandler
 *
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation is successfull
 *  - BLESTATUS_FAILED indicates that the operation has failed
 */
BleStatus BLEMGMT_RegisterHandler(BleHandler *handler);


/* De-Register a registered callback function 
 *
 * BLEMGMT_DeRegisterHandler():
 *	This function is used to deregister a previously registered handler 
 *	
 * handler: A valid pointer to a registered BleHandler.
 * 
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation is successfull
 *  - BLESTATUS_FAILED indicates that the operation has failed
 */
BleStatus BLEMGMT_DeRegisterHandler(BleHandler *handler);

/* Get the ALPWISE core stack's Software version Information  
 *
 * BLEMGMT_GetCoreStackVersion():
 *	This function is used to Get the ALPWISE core stack's Software version 
 *  Information
 * 
 * @return: The core stack version in U32 format, 0x01020304 means that the
 *  used stack version is 1.2.52
 *
 * @author Alexandre GIMARD
 */
U32 BLEMGMT_GetCoreStackVersion(void);

/* Get the Bluetooth Low Energy controller version Information  
 *
 * BLEMGMT_GetControllerVersion():
 *	This function is used to Get the Bluetooth Low Energy controller 
 * manufacturer and version Information
 * 
 * @param information: a valid BleHardwareInformation pointer that will 
 *   receive, in case of success, the Bluetooth Low Energy controller 
 *   manufacturer and version Information values.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation is successfull
 *  - BLESTATUS_FAILED indicates that the operation has failed, it should be
 *		because the stack is not initialised
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEMGMT_GetControllerVersion(BleHardwareInformation **information);

#endif /*__BLEENGINE_H*/
