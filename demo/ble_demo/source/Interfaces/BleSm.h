#ifndef __BLESM_H
#define __BLESM_H
/****************************************************************************
 *
 * File:          BleSm.h
 *
 * Description:   Contains Application interfaces routines for Security Manager.
 * 
 * Created:       January, 2009
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.21 $
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

/***************************************************************************
 *	Type definition
 ***************************************************************************/

/** Definition for the SmErrorCode type
 * This type is used to describe the reason of a PAIRING failure, it is 
 * reported during BLEEVENT_PAIRING_COMPLETE event when status is
 * BLESTATUS_FAILED in the reason field of the during the BlePairingInfo structure.
 */
typedef U8 SmErrorCode;
#define SMERROR_PASSKEYENTRY		0x01 //The user input of passkey failed or was cancelled
#define SMERROR_OOB					0x02 //The OOB data is not available	
#define SMERROR_AUTHENTIFICATIONREQ	0x03 //The pairing procedure cannot be performed as authentication requirements cannot be met due to IO capabilities of one or both devices
#define SMERROR_CONFIRMVALUE		0x04 //The confirm value does not match the calculated compare value
#define SMERROR_PAIRINGNOTSUPPORTED	0x05 //Pairing is not supported by the device
#define SMERROR_ENCRYPTIONKEYSIZE	0x06 //The resultant encryption key size is insufficient for the security requirements of this device
#define SMERROR_INVALIDCOMMAND		0x07 //The SMP command received is not supported on this device
#define SMERROR_UNKNOWREASON		0x08 //Pairing failed due to an unspecified reason
#define SMERROR_REPEATEDATTEMPTS	0x09 //Pairing or authentication procedure is disallowed because too little time has elapsed since last pairing request or security request 


/** Definition for the SmRemoteAuthCapabilities type
 * This type is used to describe the Authencation capabilities of the remote device,
 * it is reported during BLEEVENT_PAIRING_REQUEST event
 */
typedef U8 SmRemoteAuthCapabilities;
#define SMCAP_MITMREQ				0x01 //The remote device request MITM.
#define SMCAP_MITMCAPABLE			0x02 //The remote device is MITM capable according his IO capabilities.
#define SMCAP_OOBCAPABLE			0x04 //The remote device has some OOB data present.

/** Definition for the BlePairingInfo type
 * This type is used during the related PAIRING events:
 * BLEEVENT_PAIRING_REQUEST, BLEEVENT_PASSKEY_REQUEST,
 * BLEEVENT_PASSKEY_DISPLAY and BLEEVENT_PAIRING_COMPLETE
 * to carry specific event information.
 */
typedef struct {
	/** The connection handle involved in the pairing procedure, always 
	 * valid 
	 */
	U16 connHandle;

	union {
	/** The passkey to display
	 * Only valid during the BLEEVENT_PASSKEY_DISPLAY event 
	 */
	U8 passkeyDisplay[6];

	/** The reason of the pairing failure 
	 * Only valid during the BLEEVENT_PAIRING_COMPLETE event 
	 * when status is BLESTATUS_FAILED
	 */
	SmErrorCode  reason;

	/** The pairing capabilities of the remote device 
	 * Only valid during the BLEEVENT_PAIRING_REQUEST event 
	 */
	SmRemoteAuthCapabilities capabilities;

	}sm;
} BlePairingInfo;

/** Definition for the BleLinkSecurityProperties type
 * This type is a bitfield used to describe the link security Properties for 
 * a given link.
 * it can be retrieved thanks to the BLESMP_GetLinkSecurityPropertie(...) API
 */
typedef U8 BleLinkSecurityProperties;
#define CONNSTATE_NOSECURITY		0x00u
#define CONNSTATE_AUTHENTICATED		0x10u
#define CONNSTATE_ENCRYPTED			0x20u
#define CONNSTATE_BONDED			0x40u

#if (BLE_SECURITY == 1)
#if (BLE_SM_DYNAMIC_IO_CAPABILITIES == 1)
/** Definition for the BleSmpIoCapability type
 * This type is used to define the available IO capabilities
 * Possible values are: (defined in bleconfig.h)
 * #define SM_IO_DISPLAY_ONLY			0
 * #define SM_IO_DISPLAY_YESNO			1
 * #define SM_IO_KEYBOARD_ONLY			2
 * #define SM_IO_NOINPUTNOOUTPUT		3
 * #define SM_IO_KEYBOARD_DYSPLAY		4
 */
typedef U8 BleSmpIoCapability;
#endif //(BLE_SM_DYNAMIC_IO_CAPABILITIES == 1)
#endif //(BLE_SECURITY == 1)

#if (BLE_RANDOM_ADDRESSING_SUPPORT == 1)
/** Definition for the BleSmpRandomAddressType type
 * This type is used to define the available Random adresses Types
 * like STATIC, RANDOM RESOLVABLE or RANDOM NOT RESOLVABLE
 */
typedef U8 BleSmpRandomAddressType;

#define BLESM_RANDOMADDRESS_STATIC			            1
#define BLESM_RANDOMADDRESS_PRIVATE_NOTRESOLVABLE	    2
#define BLESM_RANDOMADDRESS_PRIVATE_RESOLVABLE		    3


#endif //(BLE_RANDOM_ADDRESSING_SUPPORT == 1)
/****************************************************************************
 *	APPLICATION INTERFACE functions definition
 ****************************************************************************/

#if (BLE_RANDOM_ADDRESSING_SUPPORT == 1)
/* Used by the application to generate and set a new random address for the 
 *  local device.
 *
 * BLESMP_GenerateAndSetRandomAddress():
 *	This function is used by the application layer to generate a new 
 *  private address in order to use as the local device private address in an
 *  advertising process, a scanning process or a connection process.
 * 
 * The type of the RANDOM ADDRESS type is depending of the application cycle
 *  - A RANDOM STATIC must be regenerated and change at each device power up
 *      (stack init), it is not resolvable, it is means that when the device 
 *      address is changing the remote bonded device are no more considered as
 *      bonded, persitent memory is cleared.
 *      It is targeted for device that does not restart/ power up.
 *  - A RANDOM PRIVATE NON RESOLVABLE must be regenerated and change every 15 
 *      minutes by the application, it is also not resolvable, it is means 
 *      that when the device address is changing the remote bonded device are  
 *      no more considered as bonded, persitent memory is cleared.
 *      It is better for privacy, but not for persitent operations, it is 
 *      targeted for device that does not bond with remote device.
 *  - A RANDOM PRIVATE RESOLVABLE must be regenerated and change every 15 
 *      minutes by the application, it resolvable, it is means that when
 *      the device address is changing the remote bonded device are able to 
 *      "resolve the new addess" and then recognize the device
 *      It is better for privacy, but cost some time and power to resolve the
 *      address.
 *  
 *  BLE_RANDOM_ADDRESSING_SUPPORT shall be enabled.
 *
 * param randomAddressType: The type of PRIVATE Address that the application
 *  wants to generate and set. It could be a
 * - RANDOM STATIC
 * - RANDOM PRIVATE RESOLVABLE 
 * - RANDOM PRIVATE NON RESOLVABLE 
 *
 * return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation succeeded, the 
 *		BLEEVENT_GENERATE_RANDOM_ADDRESS_RSP event will be received in the 
 *		global callback with the status of the operation, and if succeed, the
 *		random address generated.
 *
 *	- BLESTATUS_BUSY indicates that the operation failed because an other 
 *	 operation is already in progress, retry when the current operation is
 *	 complete.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it can due to
 *		an active connection using random addressing is in progress.
 */
BleStatus BLESMP_GenerateAndSetRandomAddress(
                        BleSmpRandomAddressType randomAddressType);

#endif /*(BLE_RANDOM_ADDRESSING_SUPPORT == 1)*/



#if (BLE_SECURITY == 1)

#if (BLE_SM_DYNAMIC_IO_CAPABILITIES == 1)

/** Used by the application to set the Local IO capability
 *
 * BLESMP_SetIOCapability():
 *	This function is used by the application layer to dynamicaly set the 
 * IO capability of the local device.
 *	Note that in case of DYNAMIC IO mode the default value for the device  
 * IO capabilty is set to SM_IO_CAPABILITIES; it means that the local device 
 * IO capabilty is SM_IO_CAPABILITIES if the function is not called.
 *  
 *  BLE_SECURITY shall be enabled.
 *  BLE_SM_DYNAMIC_IO_CAPABILITIES shall be enabled.
 *
 * @param ioCapability: the Io capabilty to set for the local device
 *
 * @return always BLESTATUS_SUCCESS
 *		
 * @author Alexandre GIMARD
 */
BleStatus BLESMP_SetIOCapability(BleSmpIoCapability ioCapability);
#endif //(BLE_SM_DYNAMIC_IO_CAPABILITIES == 1)

#if (BLE_SM_DYNAMIC_ENCRYPTION_KEY_SIZE == 1)
/** Used by the application to set the Maximum encryption key size
 *
 * BLESMP_SetMaximumEncrytionKeySize():
 *	This function is used by the application layer to dynamicaly set the 
 * maximum encrytion keySize supported by the local device.
 *	Note that in case of DYNAMIC encryption key size mode the default value
 *  for the device maximum encryption key size is set to 16; it means that  
 *  the local device.
 *  
 *  BLE_SECURITY shall be enabled.
 *  BLE_SM_DYNAMIC_ENCRYPTION_KEY_SIZE shall be enabled.
 *
 * @param keySize: the maximum encryption key size set for the local device
 *
 * @return: The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeds.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function has failed because
 *		an invalid parameter (KeySize must be between 7 and 16 bytes)
 *		
 * @author Alexandre GIMARD
 */
BleStatus BLESMP_SetMaximumEncrytionKeySize(U8 keySize);
#endif //(BLE_SM_DYNAMIC_ENCRYPTION_KEY_SIZE == 1)

/* Used by the application to initiate a pairing process
 *
 * BLESMP_InitiatePairing():
 *	This function is used by the application layer when the local device would
 *  initiate a pairing procedure with the remote device connected with the
 *  given connection handle. The local device will be then named as INITIATOR 
 *	and the remote device as RESPONDER
 *
 *  This function will launch a Simple Secure pairing procedure and, depending:
 *   - If any Out of Band data is set (BLESMP_SetOobData() ).
 *	 - Of the SM_IO_CAPABILITIES defined in the configuration.
 *  the application will receive BLEEVENT_PASSKEY_REQUEST event if a passkey should
 *  be provided, BLEEVENT_PASSKEY_DISPLAY is a passkey should be displayed in order
 *  to be entered by the peer device or no event.
 *
 *  Upon the Pairing process completion with success or not the 
 *	BLEEVENT_PAIRING_COMPLETE event will be notified to the application.
 *
 *
 *  BLE_SECURITY shall be enabled.
 *
 * see BLESMP_SetOobData
 * see BLESMP_ConfirmPasskey
 * see BLESMP_RejectPasskey
 *
 *  connHandle: The Local identifier of the Link layer connection that is
 *		intended to be authenticated. The connection handle was previously
 *		reported in the BLEEVENT_LINKLAYER_CONNECTED event or 
 *		BLEGAP_EVENT_CONNECTED event.
 *  bond : Set to 1 if the local device (called here the INITIATOR of the 
 *		pairing process) requests the link to be bonded and the security 
 *		information retrieved and saved. 
 *		This is a negotiation with the RESPONDER, if the RESPONDER does not
 *		require a bonded link, the link will not be bonded.
 *		Information of the bonding state of the link will be reported in the
 *		BLEEVENT_PAIRING_COMPLETE event.
 *		Important note: This flag is ignored when the ConnHandle describes a NON 
 *		RESOLVABLE or RESOLVABLE RANDOM ADRESS (as this implementation does not 
 *		resolve RESOLVABLE RANDOM ADRESS) and so the devices will not be bonded.
 *
 * return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation succeeded, the 
 *		BLEEVENT_PAIRING_COMPLETE event will be received in the 
 *		global callback with the status of the operation.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it can due to 
 *		the connhandle that does not describes an ongoing connection or the
 *		local device are already in a pairing process. 
 */
BleStatus BLESMP_InitiatePairing( U16 connHandle , U8 bond);

/* Used by the application to accept or reject the pairing request
 *
 * BLESMP_ConfirmPairing():
 *	This function is used by the application layer to accept or reject a pairing request
 * following a BLEEVENT_PAIRING_REQUEST event.
 * This function shall be call in the BLEEVENT_PAIRING_REQUEST event context,
 * otherwise, it failed
 * connHandle: the connHandle that identify the connection to
 *		Accept / reject  the pairing
 * accept: zero means that the pairing request is rejected by the application
 *		non-zero means the pairing request is accepted by the application
 * bond: valid only in case of accept is non zero, set to 1 to request the link to
 *		be bonded, Information of the bonding state of the link will be reported in the
 *		BLEEVENT_PAIRING_COMPLETE event.
 *
 *  BLE_SECURITY shall be enabled.
 *
 */
BleStatus BLESMP_ConfirmPairing(U16 connHandle, U8 accept, U8 bond);

#if (BLE_SM_OUTOFBAND_PAIRING == 1)
/* Used by the application to use Out Of Band data as pairing information
 *
 * BLESMP_SetOobData():
 *	This function is used by the application layer to set some Out of band 
 *  data in order to use it as pairing information.
 *  When the application need to use Out of Band data, it shall call this
 *  function before any call to BLESMP_InitiatePairing ( as Pairing initiator)
 *  function or before/during the BLEEVENT_PAIRING_REQUEST event if it act as
 *  pairing responder. 
 *  Take care that the pairing will failed if the OOB data is set and the
 *  remote device does not have any OOB data set and vice-versa.
 *  This Out Of band data will be used for every pairing until the call to 
 *  BLESMP_ClearOobData() to clear the Out Of Band Data or BLESMP_SetOobData()
 *	to set new Out Of Band data.
 *  
 *  If no Out of band data is registered, the normal simple pairing behavior is
 *  used.
 *  Note that the Out Of Band data length SHALL be 16 bytes, the application is
 *	responsible to fill correctly the oobData.
 *
 *  BLE_SECURITY shall be enabled.
 *  BLE_SM_OUTOFBAND_PAIRING shall be enabled.
 *
 * see BLESMP_ClearOobData
 *
 *  oobData: A valid pointer containing the 16 bytes length Out of band data.
 *		The pointer shall be valid until the next call to BLESMP_ClearOobData() or
 *		BLESMP_SetOobData().
 *
 * return None
 */
void BLESMP_SetOobData(U8 *oobData);


/* Used by the application to clear any Out Of band data
 *
 * BLESMP_ClearOobData():
 *	This function is used by the application layer to clear previously set
 *  Out of band data in order to not use it anymore as pairing information.
 *  
 *  BLE_SECURITY shall be enabled.
 *  BLE_SM_OUTOFBAND_PAIRING shall be enabled.
 *
 * see BLESMP_SetOobData
 *
 * return None
 */
void BLESMP_ClearOobData(void);
#endif //(BLE_SM_OUTOFBAND_PAIRING == 1)

#if ( (BLE_SM_DYNAMIC_IO_CAPABILITIES == 1) || ((BLE_SM_DYNAMIC_IO_CAPABILITIES == 0) && (SM_IO_CAPABILITIES != SM_IO_NOINPUTNOOUTPUT)))
/** Used by the application to set the passkey or to confirm the passkey 
 *
 * BLESMP_ConfirmPasskey():
 *	This function is used by the application layer to provide to the stack the
 * Bluetooth Passkey following a BLEEVENT_PAIRING_REQUEST event, or to confirm the
 * passkey upon the BLEEVENT_PASSKEY_DISPLAY event.
 * In both case the passkey shall be 6 characters in length, with each character
 *  value in the range 0x30-0x39 ['0'..'9']. 
 * If the passkey is "012345" then Passkey[0] = 0;
 * If this function is not called after either BLEEVENT_PASSKEY_REQUEST or BLEEVENT_PASSKEY_DISPLAY
 * it is ignored.
 * A BLEEVENT_PAIRING_COMPLETE event will be received upon the pairing completion.
 * 
 * @param connHandle the connHandle that identify the connection to
 *	confirm the passkey
 * @param Passkey the passkey to provide or confirm
 *
 *  BLE_SECURITY shall be enabled.
 *  BLE_SM_DYNAMIC_IO_CAPABILITIES enabled OR if disabled,
 *	  SM_IO_CAPABILITIES shall be different of SM_IO_NOINPUTNOOUTPUT
 *
 * @return none
 *
 * @author Alexandre GIMARD
 */
void BLESMP_ConfirmPasskey(U16 connHandle,U8 Passkey[6]);


/** Used by the application to reject the passkey provided 
 *  or to reject the passkey request.
 *
 * BLESMP_RejectPasskey():
 *	This function is used by the application layer to reject the
 * Bluetooth Passkey request following a BLEEVENT_PASSKEY_REQUEST event, or to 
 * reject the passkey provided during the BLEEVENT_PASSKEY_DISPLAY event.
 * 
 * A BLEEVENT_PAIRING_COMPLETE event will be received upon the pairing completion.
 * 
 * @param connHandle the connHandle that identify the connection to
 *	reject the passkey
 *
 *  BLE_SECURITY shall be enabled.
 *  BLE_SM_DYNAMIC_IO_CAPABILITIES enabled OR if disabled,
 *	  SM_IO_CAPABILITIES shall be different of SM_IO_NOINPUTNOOUTPUT
 *
 * @return none
 *
 * @author Alexandre GIMARD
 */
void BLESMP_RejectPasskey(U16 connHandle);
#endif //( (BLE_SM_DYNAMIC_IO_CAPABILITIES == 1) || ((BLE_SM_DYNAMIC_IO_CAPABILITIES == 0) && (SM_IO_CAPABILITIES != SM_IO_NOINPUTNOOUTPUT)))


/* Used to retrieve the current security properties for the given link
 *
 * BLESMP_GetLinkSecurityProperties():
 *	This function is used by the application layer to retrieve the 
 * current security properties for the given link. It is useful in order to
 * know if the link is encrypted, authenticated or bonded
 *
 *  BLE_SECURITY shall be enabled.
 *
 *
 * connHandle: The Local identifier of the Link layer connection for which the
 *		security properties shall be retieved, this connection handle has been
 *		reported in the BLEEVENT_LINKLAYER_CONNECTED event.
 * bleLinkSecurityProperties : A valid pointer in which the security properties
 * are written upon the return of the function
 *
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation has succeeded. The
 *     bleLinkSecurityProperties information is valid.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it can du to 
 *		the connhandle that does not describes a valid connection.
 */
BleStatus BLESMP_GetLinkSecurityProperties(	U16 connHandle,
						BleLinkSecurityProperties *bleLinkSecurityProperties);

/* Retrieve bonding status of a remote device
 *
 * BLESMP_IsDeviceBonded()
 *	This function is used to know if the remote device identified by the given
 *  address is bonded or not when the local device acts in the given role. 
 *
 *  addr : A valid Pointer to the bluetooth	device address to check
 *    bonding state
 *  role : The local device role
 *
 * return the status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the remote device is bonded.
 *
 *	- BLESTATUS_FAILED indicates that the remote device is not bonded.
 */
BleStatus BLESMP_IsDeviceBonded( BD_ADDR *addr, BleConnRole role);

/* Unbond (delete persistent memory information) for the given remote device
 *
 * BLESMP_UnBond()
 *	This function is used to unbond a remote device, it will remove all the
 *   persistent information saved for this device. 
 *
 *  addr : A valid Pointer to the remote device bluetooth
 *			device address
 *
 * return the status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the remote device is no more bonded.
 *
 *	- BLESTATUS_FAILED indicates that the remote device was not bonded.
 */
BleStatus BLESMP_UnBond( BD_ADDR *addr);

#endif /*(BLE_SECURITY == 1)*/

#endif /*__BLESM_H*/
