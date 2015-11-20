#ifndef __BLEGATT_H
#define __BLEGATT_H
/****************************************************************************
 *
 * File:          BleGatt.h
 *
 * Description:   Contains interfaces routines for GATT initialisation,
 *					de-initialisation and execution.
 * 
 * Created:       March, 2010
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.16 $
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
#include "ATT/att.h"

/** GENERIC_ATTRIBUTE_PROFILE defines if the local Device supports (set to 1) or 
 * does not support (set to 0) the GENERIC ATTRIBUTE Profile 
 * if the local device supports the ATTRIBUTE PROTOCOL CLIENT role as well as
 * GATT based profile this configuration shall be enabled (define to 1) in 
 * custom configuration file.
 * Default behavior is to not support it.
 */
#ifndef GENERIC_ATTRIBUTE_PROFILE
  #define GENERIC_ATTRIBUTE_PROFILE	0
#endif //ATTRIBUTE_PROTOCOL

#if ( (ATT_ROLE_CLIENT == 1) && (GENERIC_ATTRIBUTE_PROFILE == 0) )
// if attribute client is enabled then force the stack to supports GATT
// note that if ATT_CLIENT == 1 then ATTRIBUTE_PROTOCOL == 1
#error  "GENERIC_ATTRIBUTE_PROFILE shall be enabled if ATT_ROLE_CLIENT is enabled"
#endif //( (ATT_ROLE_CLIENT == 1) && (GENERIC_ATTRIBUTE_PROFILE == 0) ) 

#if (GENERIC_ATTRIBUTE_PROFILE == 1)
// Enable GATT only if ATT is supported
#if (ATTRIBUTE_PROTOCOL == 1)

// Enable feature only if ATT CLIENT is supported
#if (ATT_ROLE_CLIENT == 1)

/***************************************************************************\
 * CONSTANTS and MACROS definitions
\***************************************************************************/
/** BLEGATT_SUPPORT_ALL_SERVICE_DISCOVERY
 * define if the current GATT implementation supports (set to 1)
 * or not (set to 0) the All Sercive Discovery procedure ( find
 * all service)
 * default value is to not support All Sercive Discovery procedure
 */
#ifndef BLEGATT_SUPPORT_ALL_SERVICE_DISCOVERY
#define BLEGATT_SUPPORT_ALL_SERVICE_DISCOVERY					0
#endif //BLEGATT_SUPPORT_ALL_SERVICE_DISCOVERY

/** BLEGATT_SUPPORT_SINGLE_SERVICE_DISCOVERY
 * define if the current GATT implementation supports (set to 1)
 * or not (set to 0) the Single Sercive Discovery procedure ( find
 * a specific service)
 * default value is to not support Single Sercive Discovery procedure
 */
#ifndef BLEGATT_SUPPORT_SINGLE_SERVICE_DISCOVERY
#define BLEGATT_SUPPORT_SINGLE_SERVICE_DISCOVERY				0
#endif //BLEGATT_SUPPORT_SINGLE_SERVICE_DISCOVERY

/** BLEGATT_SUPPORT_RELATIONSHIP_DISCOVERY
 * define if the current GATT implementation supports (set to 1)
 * or not (set to 0) the Relationship Discovery procedure ( find
 * included services)
 * default value is to not support Relationship Discovery procedure
 */
#ifndef BLEGATT_SUPPORT_RELATIONSHIP_DISCOVERY
#define BLEGATT_SUPPORT_RELATIONSHIP_DISCOVERY					0 
#endif //BLEGATT_SUPPORT_RELATIONSHIP_DISCOVERY

/** BLEGATT_SUPPORT_SINGLE_CHARACTERISTIC_DISCOVERY
 * define if the current GATT implementation supports (set to 1)
 * or not (set to 0) the characteristic Discovery procedure by UUID
 *( find characteristic by UUID)
 * default value is to not support the characteristic Discovery procedure
 */
#ifndef BLEGATT_SUPPORT_SINGLE_CHARACTERISTIC_DISCOVERY
#define BLEGATT_SUPPORT_SINGLE_CHARACTERISTIC_DISCOVERY			0 
#endif //BLEGATT_SUPPORT_SINGLE_CHARACTERISTIC_DISCOVERY

/** BLEGATT_SUPPORT_ALL_CHARACTERISTIC_DISCOVERY
 * define if the current GATT implementation supports (set to 1)
 * or not (set to 0) the characteristic Discovery procedure 
 *( find all characteristic of a service)
 * default value is to not support the characteristic Discovery procedure
 */
#ifndef BLEGATT_SUPPORT_ALL_CHARACTERISTIC_DISCOVERY
#define BLEGATT_SUPPORT_ALL_CHARACTERISTIC_DISCOVERY			0 
#endif //BLEGATT_SUPPORT_ALL_CHARACTERISTIC_DISCOVERY

/** BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_HANDLE
 * define if the current GATT implementation supports (set to 1)
 * or not (set to 0) the characteristic value acquisition when the handle of 
 * the characteristic value is know
 *( find characteristic value by handle)
 * default value is to not support the characteristic read by handle procedure
 */
#ifndef BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_HANDLE
#define BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_HANDLE		0 
#endif //BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_HANDLE


/** BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_TYPE
 * define if the current GATT implementation supports (set to 1)
 * or not (set to 0) the characteristic value acquisition when the type of 
 * the characteristic value is know
 *( find characteristic value by type)
 * default value is to not support the characteristic read by type procedure
 */
#ifndef BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_TYPE
#define BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_TYPE		0 
#endif //BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_TYPE


/** BLEGATT_SUPPORT_READ_LONG_CHARACTERISTIC_VALUE
 * define if the current GATT implementation supports (set to 1)
 * or not (set to 0) the long characteristic readding 
 *( read long characteristic value)
 * default value is to not support it.
 */
#ifndef BLEGATT_SUPPORT_READ_LONG_CHARACTERISTIC_VALUE
#define BLEGATT_SUPPORT_READ_LONG_CHARACTERISTIC_VALUE			0 
#endif //BLEGATT_SUPPORT_READ_LONG_CHARACTERISTIC_VALUE

/** BLEGATT_SUPPORT_READ_MULTIPLE_VALUES
 * define if the current GATT implementation supports (set to 1)
 * or not (set to 0) the multiple values reading 
 *( read multiples characteristic values)
 * default value is to not support it.
 */
#ifndef BLEGATT_SUPPORT_READ_MULTIPLE_VALUES
#define BLEGATT_SUPPORT_READ_MULTIPLE_VALUES					0 
#endif //BLEGATT_SUPPORT_READ_MULTIPLE_VALUES

/** BLEGATT_SUPPORT_GET_CHARACTERISTIC_DESCRIPTOR_LIST
 * define if the current GATT implementation supports (set to 1)
 * or not (set to 0) the characteristic description acquisition 
 *( get characteristic descriptor)
 * default value is to not support it.
 */
#ifndef BLEGATT_SUPPORT_GET_CHARACTERISTIC_DESCRIPTOR_LIST
#define BLEGATT_SUPPORT_GET_CHARACTERISTIC_DESCRIPTOR_LIST		0 
#endif //BLEGATT_SUPPORT_GET_CHARACTERISTIC_DESCRIPTOR

/** BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_WITHOUT_RESPONSE
 * define if the current GATT implementation supports (set to 1)
 * or not (set to 0) the write characteristic value without response 
 * procedure 
 * default value is to not support it.
 */
#ifndef BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_WITHOUT_RESPONSE
#define BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_WITHOUT_RESPONSE	0 
#endif //BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_WITHOUT_RESPONSE

/** BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_VALUE
 * define if the current GATT implementation supports (set to 1)
 * or not (set to 0) the write characteristic value with response 
 * procedure 
 * default value is to not support it.
 */
#ifndef BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_VALUE
#define BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_VALUE				0 
#endif //BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_VALUE

/** BLEGATT_SUPPORT_WRITE_LONG_CHARACTERISTIC_VALUE
 * define if the current GATT implementation supports (set to 1)
 * or not (set to 0) the write long characteristic value (len > ATT_MTU_BLE -3) 
 * with response procedure .
 * default value is to not support it.
 */
#ifndef BLEGATT_SUPPORT_WRITE_LONG_CHARACTERISTIC_VALUE
#define BLEGATT_SUPPORT_WRITE_LONG_CHARACTERISTIC_VALUE			0 
#endif //BLEGATT_SUPPORT_WRITE_LONG_CHARACTERISTIC_VALUE

/***************************************************************************\
 * Types definitions
\***************************************************************************/
/** 
 * BleGattEvent type
 * Define the different kind of events that could be received by the 
 * BleGattCallback, they are received in the BleGattCallbackParms->event
 */
typedef U8 BleGattEvent;

/** BLEGATTEVENT_PRIMARYSERVICEFOUND
 * Event received when a primary Service is found
 * The status field is Always BLESTATUS_SUCCESS  
 * The parms field indicates the Service information within a
 * BleGattServiceInformation pointer.
 */
#define BLEGATTEVENT_PRIMARYSERVICEFOUND					0

/** BLEGATTEVENT_PRIMARYSERVICESEARCHCOMPLETE
 * Event received when the primary Service discovery has completed
 * The status field indicates the status of the operation.
 * The parms field is not applicable.
 */
#define BLEGATTEVENT_PRIMARYSERVICESEARCHCOMPLETE			1

/** BLEGATTEVENT_INCLUDEDSERVICEFOUND
 * Event received when an included Service is found
 * The status field is Always BLESTATUS_SUCCESS  
 * The parms field indicates the Service information within a
 * BleGattServiceInformation pointer.
 */
#define BLEGATTEVENT_INCLUDEDSERVICEFOUND					2

/** BLEGATTEVENT_INCLUDEDSERVICESEARCHCOMPLETE
 * Event received when the included Service discovery has completed
 * The status field indicates the status of the operation.
 * The parms field is not applicable.
 */
#define BLEGATTEVENT_INCLUDEDSERVICESEARCHCOMPLETE			3

/** BLEGATTEVENT_CHARACTERISTICFOUND
 * Event received when a characteristic is found
 * The status field is Always BLESTATUS_SUCCESS  
 * The parms field indicates the characteristic information within a
 * BleGattCharacteristicInformation pointer.
 */
#define BLEGATTEVENT_CHARACTERISTICFOUND					4

/** BLEGATTEVENT_CHARACTERISTICSEARCHCOMPLETE
 * Event received when the characteristic discovery has completed
 * The status field indicates the status of the operation.
 * The parms field is not applicable.
 */
#define BLEGATTEVENT_CHARACTERISTICSEARCHCOMPLETE			5

/** BLEGATTEVENT_DESCRIPTORFOUND
 * Event received when a characteristic descriptor is found
 * The status field is Always BLESTATUS_SUCCESS  
 * The parms field indicates the characteristic descriptor information 
 * within a BleGattDescriptorInformation pointer.
 */
#define BLEGATTEVENT_DESCRIPTORFOUND						6

/** BLEGATTEVENT_DESCRIPTORSEARCHCOMPLETE
 * Event received when the characteristic descriptors discovery
 * has completed
 * The status field indicates the status of the operation.
 * The parms field is not applicable.
 */
#define BLEGATTEVENT_DESCRIPTORSEARCHCOMPLETE				7

/** BLEGATTEVENT_CHARACTERISTICVALUEREAD
 * Event received when the read characteristic value 
 * has completed
 * The status field indicates the status of the operation.
 * The parms field indicates the characteristic value information 
 * within a BleGattCharacteristicValueInformation pointer.
 * If the status is BLESTATUS_FAILED, then the errorCode field of the
 * BleGattCharacteristicValueInformation pointer contains the Attribute 
 * Protocol error code.
 */
#define BLEGATTEVENT_CHARACTERISTICVALUEREAD				8

/** BLEGATTEVENT_LONGCHARACTERISTICVALUEPART
 * Event received when the part of a characteristic long value 
 * is received
 * The status field is Always BLESTATUS_SUCCESS 
 * The parms field indicates the characteristic value part information
 * within a BleGattLongCharacteristicValueInformation pointer.
 */
#define BLEGATTEVENT_LONGCHARACTERISTICVALUEPART			9

/** BLEGATTEVENT_LONGCHARACTERISTICVALUECOMPLETE
 * Event received when the read characteristic long value 
 * has completed
 * The status field indicates the status of the operation.
 * The parms field indicate the information about the handle from which the 
 * operation is complete as well of the ATTRIBUTE PROTOCOL error code in case
 * of failure (status equal to BLESTATUS_FAILED) within a 
 * BleGattLongCharacteristicCompleteInformation pointer
 */
#define BLEGATTEVENT_LONGCHARACTERISTICVALUECOMPLETE		10

/** BLEGATTEVENT_WRITEWITHOUTRESPONSESENT
 * Event received when the write characteristic value without response
 * request has been sent
 * The status field indicates the status of the operation.
 * The parms field is not applicable.
 */
#define BLEGATTEVENT_WRITEWITHOUTRESPONSESENT				11

/** BLEGATTEVENT_CHARACTERISTICVALUEWRITECOMPLETE
 * Event received when the write characteristic value 
 * has completed
 * The status field indicates the status of the operation.
 * The parms field indicates the characteristic value information 
 * within a BleGattWriteRspInformation pointer. 
 * If the status is BLESTATUS_FAILED, then the errorCode field of the
 * BleGattWriteRspInformation pointer contains the Attribute Protocol
 * error code.
 */
#define BLEGATTEVENT_CHARACTERISTICVALUEWRITECOMPLETE		12

/** BLEGATTEVENT_LONGCHARACTERISTICVALUEWRITECOMPLETE
 * Event received when the write characteristic long value 
 * has completed
 * The status field indicates the status of the operation.
 * The parms field indicates the characteristic long value information 
 * within a BleGattWriteRspInformation pointer. 
 * If the status is BLESTATUS_FAILED, then the errorCode field of the
 * BleGattWriteRspInformation pointer contains the Attribute Protocol
 * error code.
 */
#define BLEGATTEVENT_LONGCHARACTERISTICVALUEWRITECOMPLETE	13

/** BLEGATTEVENT_NOTIFICATION
 * Event received when an handle Value Notification has been received by
 * the CLIENT to the remote SERVER
 * The status field is Always BLESTATUS_SUCCESS 
 * The parms field indicates the handle and value information 
 * within a BleGattNotificationInformation pointer. 
 */
#define BLEGATTEVENT_NOTIFICATION							14

/** BLEGATTEVENT_INDICATION
 * Event received when an handle Value Indication has been received by
 * the CLIENT to the remote SERVER
 * The status field is Always BLESTATUS_SUCCESS 
 * The parms field indicates the handle and value information 
 * within a BleGattIndicationInformation pointer. 
 */
#define BLEGATTEVENT_INDICATION								15

/** BLEGATTEVENT_CONFIRMATIONSENT
 * Event received when the handle Value Confirmation has been sent by
 * the CLIENT to the remote SERVER
 * The status field indicates the status of the operation.
 * The parms field is not applicable.
 */
#define BLEGATTEVENT_CONFIRMATIONSENT						16

/**
 * BleGattClient
 *	An unique identifier of a GENERIC ATTRIBUTE Profile CLIENT local
 *  Client. 
 *  It is returned by the BLEGATT_RegisterClient and used in the GENERIC 
 *  ATTRIBUTE Profile CLIENT Sub-Procedure
 */
typedef void * BleGattClient;


/**
 * BleGattCommand
 *	An unique memory block needed INTERNALY by the local GENERIC ATTRIBUTE 
 *  Profile CLIENT to execute Sub-Procedures. The command can be reused only 
 *  if a client ongoing sub-Procedure has safely completed.
 *  The GENERIC ATTRIBUTE Profile queues the commands so it is possible for 
 *  HIGHER layers to start multiple Sub-Procedures as long as multiple 
 *  BleGattCommand are used.
 */
typedef struct{

	/** 
	 *	The Following fields are for INTERNAL USE ONLY
	 * Do not address them directly
	 */
	AttCommand		attCommand;

	U8				procedure;
	U8				terminate;			// used by higher layer to terminate

	BleGattClient	gattClient;

	struct {
		AttUuid			uuidParam;		// Service disc, service disc by uuid, charac search, charac search by uuid
		U8				value[2];		// Service disc by uuid
		AttUuid			targetUuid;		// Charac search by uuid
		U16				valueLen;		// Write long
		AttHandle		handle;			// Write long, find included (included service found starting handle)
		AttHandle		ieHandle;		// Find included (included service found ending handle)
		AttHandle		iHandle;		// Find included,the included Attribute Handle needed if a 128 bit attribute is followed by a 16 bit attribute
		AttHandle		eHandle;		// the service ending handle in which we are looking for included attribute
		AttPduStatus	error;			// writelong characterictic
	} m;			// Some memory used internally
} BleGattCommand;


/**
 * BleGattServiceInformation
 *	The structure representing a remote ATTRIBUTE SERVER service information
 * it indicates:
 * startingHandle: the starting handle of the service in the remote ATTRIBUTE
 *	 SERVER database
 * endingHandle: the ending handle of the service in the remote ATTRIBUTE
 *	 SERVER database
 * serviceUuid : the service UUID
 */
typedef struct {

	AttHandle	startingHandle;
	AttHandle	endingHandle;
	AttUuid		serviceUuid;

} BleGattServiceInformation;

/**
 * BleGattCharacteristicInformation
 *	The structure representing a remote ATTRIBUTE SERVER characteristic 
 *  information
 * it indicates:
 * characteristicProperties: the characteristic Properties
 * characteristicDeclHandle: the characteristic declaration Handle in the
 *	 remote ATTRIBUTE SERVER database
 * valueHandle : the characteristic value handle in the
 *	 remote ATTRIBUTE SERVER database
 * valueUuid : the characteristic value UUID
 */
typedef struct {

	AttCharacteriticProperties		characteristicProperties;
	AttHandle						characteristicDeclHandle;
	AttHandle						valueHandle;
	AttUuid							valueUuid;

} BleGattCharacteristicInformation;

/**
 * BleGattDescriptorInformation
 *	The structure representing a remote ATTRIBUTE SERVER characteristic 
 *  descriptor information
 * it indicates:
 * descriptorHandle : the characteristic descriptor handle in the
 *	 remote ATTRIBUTE SERVER database
 * descriptorUuid : the characteristic descriptor UUID 
 */
typedef struct {

	AttHandle	descriptorHandle;
	AttUuid		descriptorUuid;

} BleGattDescriptorInformation;

/**
 * BleGattCharacteristicValueInformation
 *	The structure representing a remote ATTRIBUTE SERVER characteristic 
 *  value information
 * it indicates:
 * valueHandle : the characteristic value handle in the
 *	 remote ATTRIBUTE SERVER database
 * value : a pointer to the characteristic value
 * valueLen : the characteristic value length
 * valueUuid : the characteristic value UUID
 * errorCode : the ATTRIBUTE PROTOCOL Error code indicating why the remote
 * ATTRIBUTE SERVER characteristic value information is not filled
 */
typedef struct {

	AttHandle	valueHandle;
	U8*			value;
	U16			valueLen;
	AttUuid		valueUuid;
	AttPduStatus	errorCode;

} BleGattCharacteristicValueInformation;

/**
 * BleGattLongCharacteristicValueInformation
 *	The structure representing a part of a ATTRIBUTE SERVER characteristic 
 *  value.
 * it indicates:
 * valueHandle : the characteristic value handle in the
 *	 remote ATTRIBUTE SERVER database
 * value : a pointer to the characteristic value
 * offsetFrom : the starting offset of the part attribute value
 * offsetTo : the ending offset of the part attribute value
 */
typedef struct {

	AttHandle	valueHandle;
	U8*			value;
	U16			offsetFrom;
	U16			offsetTo;

} BleGattLongCharacteristicValueInformation;

/**
 * BleGattLongCharacteristicCompleteInformation
 *	The structure representing the answer to a ATTRIBUTE SERVER  
 *   characteristic value long characteristic Procedure. it is received in the
 *   callback during the BLEGATTEVENT_LONGCHARACTERISTICVALUECOMPLETE event.
 * it indicates:
 * valueHandle : the characteristic value handle in the
 *	 remote ATTRIBUTE SERVER database
 * errorCode : the ATTRIBUTE PROTOCOL Error code indicating why the remote
 *  ATTRIBUTE SERVER long characteristic value read procedure failed.
 * it is only applicable when the status of the operation is BLESTATUS_FAILED
 */
typedef struct {

	AttHandle		valueHandle;
	AttPduStatus	errorCode;

} BleGattLongCharacteristicCompleteInformation;

/**
 * BleGattWriteRspInformation
 *	The structure representing the answer to a ATTRIBUTE SERVER  
 *   characteristic value write Procedure.
 * it indicates:
 * valueHandle : the characteristic value handle in the
 *	 remote ATTRIBUTE SERVER database
 * errorCode : the ATTRIBUTE PROTOCOL Error code indicating why the remote
 *  ATTRIBUTE SERVER characteristic value write procedure failed.
 */
typedef struct {

	AttHandle		valueHandle;
	AttPduStatus	errorCode;

} BleGattWriteRspInformation;

/**
 * BleGattNotificationInformation,BleGattIndicationInformation
 *	The structure representing a remote ATTRIBUTE SERVER characteristic 
 *  value information received during an handle value indication or an 
 *  handle value notification.
 * it indicates:
 * valueHandle : the characteristic value handle in the
 *	 remote ATTRIBUTE SERVER database
 * value : a pointer to the characteristic value
 * valueLen : the characteristic value length
 */
typedef struct {

	AttHandle	valueHandle;
	U8*			value;
	U16			valueLen;

}BleGattNotificationInformation, BleGattIndicationInformation;

/**
 * BleGattCallbackParms
 *	The structure representing the events received by the BleGattCallback
 * it indicates:
 * event: the event Type, defined as BleGattEvent
 * status : the status associated with the event
 * connHandle : the connection handle identifying the remote SERVER
 * gattClient : the client involved in the event
 * command: the command used in the Sub-Procedure.
 * parms: a pointer to a structure defining information associated with
 *   the event.
 */
typedef struct {

	BleGattEvent	event;
	BleStatus		status;
	U16				connHandle;
	BleGattClient	gattClient;
	BleGattCommand	*command;
	void			*parms;

}BleGattCallbackParms;

/**
 * BleGattCallback
 *	This callback receives the GENERIC ATTRIBUTE Profile CLIENT profile events. 
 *  Each of these events is in the BleGattCallbackParms structure and so are 
 *  represented by an event type with a defined status and parameter.
 *	The callback is executed during the stack context, be careful to not doing 
 *	heavy process in this function.
 */
typedef void (*BleGattCallback)(BleGattCallbackParms *gattParms);

/**
 * BleGattHandler
 *	The handler passed during the BLEGATT_RegisterClient that indicates the 
 *  callback in which the the GENERIC ATTRIBUTE Profile CLIENT profile events
 *  for this client will be received.
 */
typedef struct {
	
	/** The callback on which the GATT events will be returned */
	BleGattCallback callback;

	/** Internal use only*/
	void *internal[2];

} BleGattHandler;

/***************************************************************************\
 * External Functions Prototype
\***************************************************************************/

/** Register an GENERIC ATTRIBUTE Profile CLIENT.
 *
 * BLEGATT_RegisterClient():
 *  This function is used to register an GENERIC ATTRIBUTE Profile CLIENT.
 *  it will defines a new GENERIC ATTRIBUTE Profile CLIENT that will be able
 *  to run GATT CLIENT procedures.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 *
 * @param handler	The handler describing the CLIENT to register
 *
 * @return :
 *	- A valid GENERIC ATTRIBUTE Profile client upon SUCCESS or 0 in case of failure
 *	
 * @author Alexandre GIMARD
 */
BleGattClient BLEGATT_RegisterClient(BleGattHandler *handler);

/** De-Register an GENERIC ATTRIBUTE Profile CLIENT.
 *
 * BLEGATT_DeRegisterClient():
 *  This function is used to Deregister a registered BleGattHandler
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 *
 * @param handler	A registered handler describing the CLIENT to deregister
 *
 * @return :
 *	- Always SUCCESS
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_DeRegisterClient(BleGattHandler *handler);

/** Ask if GENERIC ATTRIBUTE Profile CLIENT is registered.
 *
 * BLEGATT_IsRegisteredClient():
 *  This function is used to check if a given BleGattHandler is a registered
 *  GENERIC ATTRIBUTE Profile CLIENT.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 *
 * @param handler	A handler to check if it is registered
 *
 * @return :
 *	- BLESTATUS_SUCCESS indicates that the CLIENT is registered
 *
 *	- BLESTATUS_FAILED indicates that the CLIENT is not registered
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_IsRegisteredClient(BleGattHandler *handler);

#if (BLEGATT_SUPPORT_ALL_SERVICE_DISCOVERY == 1)
/** Discover all the services supported by the remote ATTRIBUTE SERVER
 *
 * BLEGATT_DiscoverAllPrimaryServices():
 *	This sub-procedure is used by a CLIENT to discover all the primary 
 * services on a ATTRIBUTE SERVER.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 * BLEGATT_SUPPORT_ALL_SERVICE_DISCOVERY shall be enabled
 *
 * @param client: the registered CLIENT to use in this procedure
 * @param connHandle: the connection handle that identify the ATTRIBUTE 
 *			SERVER holding the primary services to discover.
 * @param command: an unique GENERIC ATTRIBUTE Profile CLIENT command to
 *			execute this procedure
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation has started successfully;
 *     Once a primary service has been found in the remote SERVER, the 
 *     BLEGATTEVENT_PRIMARYSERVICEFOUND event will be received in the CLIENT 
 *     callback with the primary service information. The 
 *     BleGattCallbackParms->parms field indicates the service information as  
 *     a (BleGattServiceInformation *) structure.
 *	
 *     Once the operation is completed the 
 *     BLEGATTEVENT_PRIMARYSERVICESEARCHCOMPLETE event will be received in the 
 *     CLIENT callback. The BleGattCallbackParms->status field indicate the  
 *     status of the operation.
 *
 *     The command can be reused after a 
 *     BLEGATTEVENT_PRIMARYSERVICESEARCHCOMPLETE event but not after a 
 *     BLEGATTEVENT_PRIMARYSERVICEFOUND event.
 *
 *     It is permitted to end the sub-procedure early if a desired primary  
 *     service is found prior to discovering all the primary services on the
 *     SERVER, so the HIGHER layers may call BLEGATT_EndSubProcedure() during
 *     the BLEGATTEVENT_PRIMARYSERVICEFOUND event, it will end the discovery 
 *     safely and the BLEGATTEVENT_PRIMARYSERVICESEARCHCOMPLETE event will be
 *     received promptly.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it may be du 
 *     of an invalid connection handle.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter 
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_DiscoverAllPrimaryServices( BleGattClient client,
											 U16 connHandle,
											 BleGattCommand *command );
#endif //(BLEGATT_SUPPORT_SERVICE_DISCOVERY == 1)

#if (BLEGATT_SUPPORT_SINGLE_SERVICE_DISCOVERY == 1)
/** Discover if a specified service is supported by the remote SERVER
 *
 * BLEGATT_DiscoverPrimaryServiceByType():
 *	This sub-procedure is used by a CLIENT to discover if the specified 
 *  primary service is present in a ATTRIBUTE SERVER database.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 * BLEGATT_SUPPORT_SINGLE_SERVICE_DISCOVERY shall be enabled
 *
 * @param client: the registered CLIENT to use in this procedure
 * @param connHandle: the connection handle that identify the ATTRIBUTE 
 *			SERVER holding the primary service to discover.
 * @param type: the UUID of the service to discover.
 * @param command: an unique GENERIC ATTRIBUTE Profile CLIENT command to
 *			execute this procedure
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation has started successfully;
 *     Once the primary service has been found in the remote SERVER, the 
 *     BLEGATTEVENT_PRIMARYSERVICEFOUND event will be received in the CLIENT 
 *     callback with the primary service information. The 
 *     BleGattCallbackParms->parms field indicates the service information as  
 *     a (BleGattServiceInformation *) structure. This event will not be 
 *     received if the service in not available in the ATTRIBUTE SERVER 
 *     database.
 *	
 *     Once the operation is completed the 
 *     BLEGATTEVENT_PRIMARYSERVICESEARCHCOMPLETE event will be received in the 
 *     callback. The BleGattCallbackParms->status field indicate the status of 
 *     the operation.
 *
 *     The command can be reused after a 
 *     BLEGATTEVENT_PRIMARYSERVICESEARCHCOMPLETE event but not after a 
 *     BLEGATTEVENT_PRIMARYSERVICEFOUND event.
 *
 *     It is permitted to end the sub-procedure early if a desired primary  
 *     service is found prior to discovering all the primary services on the
 *     SERVER, so the HIGHER layers may call BLEGATT_EndSubProcedure() during
 *     the BLEGATTEVENT_PRIMARYSERVICEFOUND event, it will end the discovery 
 *     safely and the BLEGATTEVENT_PRIMARYSERVICESEARCHCOMPLETE event will be
 *     received promptly.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it may be du 
 *     of an invalid connection handle.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter 
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_DiscoverPrimaryServiceByType(BleGattClient client,
											   U16 connHandle,
											   AttUuid *type,
											   BleGattCommand *command );
#endif //(BLEGATT_SUPPORT_SINGLE_SERVICE_DISCOVERY == 1)

#if (BLEGATT_SUPPORT_RELATIONSHIP_DISCOVERY == 1)
/** Discover all the included services of the specified service
 *
 * BLEGATT_FindIncludedService():
 *	This sub-procedure is used by a CLIENT to discover all the included  
 * services of the specified service on a ATTRIBUTE SERVER database.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 * BLEGATT_SUPPORT_RELATIONSHIP_DISCOVERY shall be enabled
 *
 * @param client: the registered CLIENT to use in this procedure
 * @param connHandle: the connection handle that identify the ATTRIBUTE 
 *			SERVER holding the primary services to discover.
 * @param serviceStartingHandle: the starting handle of the service in which 
 *			the included services will be found.
 * @param serviceEndingHandle: the Ending handle of the service in which 
 *			the included services will be found.
 * @param command: an unique GENERIC ATTRIBUTE Profile CLIENT command to
 *			execute this procedure
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation has started successfully;
 *     Once an included service has been found in the remote SERVER, the 
 *     BLEGATTEVENT_INCLUDEDSERVICEFOUND event will be received in the CLIENT 
 *     callback with the included service information. The 
 *     BleGattCallbackParms->parms field indicates the service information as  
 *     a (BleGattServiceInformation *) structure.
 *	
 *     Once the operation is completed the 
 *     BLEGATTEVENT_INCLUDEDSERVICESEARCHCOMPLETE event will be received in the 
 *     CLIENT callback. The BleGattCallbackParms->status field indicate the  
 *     status of the operation.
 *
 *     The command can be reused after a 
 *     BLEGATTEVENT_INCLUDEDSERVICESEARCHCOMPLETE event but not after a 
 *     BLEGATTEVENT_INCLUDEDSERVICESEARCHCOMPLETE event.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it may be du 
 *     of an invalid connection handle.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter 
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_FindIncludedService(BleGattClient client, 
									  U16 connHandle, 
									  AttHandle serviceStartingHandle, 
									  AttHandle serviceEndingHandle, 
									  BleGattCommand *command );
#endif //(BLEGATT_SUPPORT_RELATIONSHIP_DISCOVERY == 1)

#if (BLEGATT_SUPPORT_ALL_CHARACTERISTIC_DISCOVERY == 1)
/** Discover all the Characteristic supported by a service in the remote 
 *    ATTRIBUTE SERVER
 *
 * BLEGATT_DiscoverAllCharacteristic():
 *    This sub-procedure is used by a CLIENT to discover all the 
 * characteristics of a specified service in a ATTRIBUTE SERVER database.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 * BLEGATT_SUPPORT_ALL_CHARACTERISTIC_DISCOVERY shall be enabled
 *
 * @param client: the registered CLIENT to use in this procedure
 * @param connHandle: the connection handle that identify the ATTRIBUTE 
 *			SERVER holding the characteristic to discover.
 * @param serviceStartingHandle: the starting handle of the service in which 
 *			the characteristics will be found.
 * @param serviceEndingHandle: the Ending handle of the service in which 
 *			the characteristics will be found
 * @param command: an unique GENERIC ATTRIBUTE Profile CLIENT command to
 *			execute this procedure
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation has started successfully;
 *     Once a characteristic has been found in the remote SERVER, the 
 *     BLEGATTEVENT_CHARACTERISTICFOUND event will be received in the CLIENT 
 *     callback with the characteristic information. The 
 *     BleGattCallbackParms->parms field indicates the characteristic  
 *     information as a (BleGattCharacteristicInformation *) structure.
 *	
 *     Once the operation is completed the 
 *     BLEGATTEVENT_CHARACTERISTICSEARCHCOMPLETE event will be received in the 
 *     CLIENT callback. The BleGattCallbackParms->status field indicate the  
 *     status of the operation.
 *
 *     The command can be reused after a 
 *     BLEGATTEVENT_CHARACTERISTICSEARCHCOMPLETE event but not after a 
 *     BLEGATTEVENT_CHARACTERISTICFOUND event.
 *
 *     It is permitted to end the sub-procedure early if a desired   
 *     characteristic is found prior to discovering all the characteristics 
 *     on the SERVER, so the HIGHER layers may call BLEGATT_EndSubProcedure() 
 *     during the BLEGATTEVENT_CHARACTERISTICFOUND event, it will end the  
 *     discovery safely and the BLEGATTEVENT_CHARACTERISTICSEARCHCOMPLETE 
 *     event will be received promptly.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it may be du 
 *     of an invalid connection handle.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter 
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_DiscoverAllCharacteristic(BleGattClient client, 
									  U16 connHandle, 
									  AttHandle serviceStartingHandle, 
									  AttHandle serviceEndingHandle, 
									  BleGattCommand *command );
#endif //(BLEGATT_SUPPORT_ALL_CHARACTERISTIC_DISCOVERY == 1)

#if (BLEGATT_SUPPORT_SINGLE_CHARACTERISTIC_DISCOVERY == 1)
/** Discover if a specified service host the specified characteristic in
 *   the remote ATTRIBUTE SERVER
 *
 * BLEGATT_DiscoverCharacteristicByType():
 *	This sub-procedure is used by a CLIENT to discover if the specified 
 *  specified service host the specified characteristic in the remote 
 *  ATTRIBUTE SERVER database.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 * BLEGATT_SUPPORT_SINGLE_CHARACTERISTIC_DISCOVERY shall be enabled
 *
 * @param client: the registered CLIENT to use in this procedure
 * @param connHandle: the connection handle that identify the ATTRIBUTE 
 *			SERVER holding the characteristic to discover.
 * @param serviceStartingHandle: the starting handle of the service in which 
 *			the characteristics will be found.
 * @param serviceEndingHandle: the Ending handle of the service in which 
 *			the characteristics will be found
 * @param type: the UUID of the characteristic to discover.
 * @param command: an unique GENERIC ATTRIBUTE Profile CLIENT command to
 *			execute this procedure
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation has started successfully;
 *     Once the characteristic has been found in the remote SERVER, the 
 *     BLEGATTEVENT_CHARACTERISTICFOUND event will be received in the CLIENT 
 *     callback with the characteristic information. The 
 *     BleGattCallbackParms->parms field indicates the characteristic   
 *     information as a (BleGattCharacteristicInformation *) structure. This
 *     event will not be received if the characteristic in not available 
 *     in the ATTRIBUTE SERVER database.
 *	
 *     Once the operation is completed the 
 *     BLEGATTEVENT_CHARACTERISTICSEARCHCOMPLETE event will be received in the 
 *     callback. The BleGattCallbackParms->status field indicate the status of 
 *     the operation.
 *
 *     The command can be reused after a 
 *     BLEGATTEVENT_CHARACTERISTICSEARCHCOMPLETE event but not after a 
 *     BLEGATTEVENT_CHARACTERISTICFOUND event.
 *
 *     It is permitted to end the sub-procedure early if a desired   
 *     characteristic  is found prior to discovering all the characteristics 
 *     on the SERVER, so the HIGHER layers may call BLEGATT_EndSubProcedure() during
 *     the BLEGATTEVENT_CHARACTERISTICFOUND event, it will end the discovery 
 *     safely and the BLEGATTEVENT_CHARACTERISTICSEARCHCOMPLETE event will be
 *     received promptly.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it may be du 
 *     of an invalid connection handle.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter 
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_DiscoverCharacteristicByType(BleGattClient client, 
									  U16 connHandle, 
									  AttHandle serviceStartingHandle, 
									  AttHandle serviceEndingHandle, 
									  AttUuid *type,
									  BleGattCommand *command );
#endif //(BLEGATT_SUPPORT_SINGLE_CHARACTERISTIC_DISCOVERY == 1)

#if (BLEGATT_SUPPORT_GET_CHARACTERISTIC_DESCRIPTOR_LIST == 1)
/** Discover all the characteristic descriptors supported by a characteristic  
 *   in the remote ATTRIBUTE SERVER
 *
 * BLEGATT_GetCharacteristicDescriptorList():
 *    This sub-procedure is used by a CLIENT to discover all the 
 *    characteristic descriptors of a specified characteristic in a ATTRIBUTE 
 *    SERVER database.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 * BLEGATT_SUPPORT_GET_CHARACTERISTIC_DESCRIPTOR_LIST shall be enabled
 *
 * @param client: the registered CLIENT to use in this procedure
 * @param connHandle: the connection handle that identify the ATTRIBUTE 
 *			SERVER holding the characteristic descriptors to discover.
 * @param characStartingHandle: the starting handle of the characteristic in  
 *			which the characteristic descriptors will be found.
 * @param characEndingHandle: the Ending handle of the characteristic in which 
 *			the characteristic descriptors will be found
 * @param command: an unique GENERIC ATTRIBUTE Profile CLIENT command to
 *			execute this procedure
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation has started successfully;
 *     Once a characteristic descriptor has been found in the remote SERVER,  
 *     the BLEGATTEVENT_DESCRIPTORFOUND event will be received in the CLIENT 
 *     callback with the characteristic descriptor information. The 
 *     BleGattCallbackParms->parms field indicates the characteristic   
 *     descriptor information as a (BleGattDescriptorInformation *) structure.
 *	
 *     Once the operation is completed the 
 *     BLEGATTEVENT_DESCRIPTORSEARCHCOMPLETE event will be received in the 
 *     CLIENT callback. The BleGattCallbackParms->status field indicate the  
 *     status of the operation.
 *
 *     The command can be reused after a 
 *     BLEGATTEVENT_DESCRIPTORSEARCHCOMPLETE event but not after a 
 *     BLEGATTEVENT_DESCRIPTORFOUND event.
 *
 *     It is permitted to end the sub-procedure early if a desired   
 *     characteristic descriptor is found prior to discovering all the 
 *     characteristic descriptor on the SERVER, so the HIGHER layers may call
 *     BLEGATT_EndSubProcedure() during the BLEGATTEVENT_DESCRIPTORFOUND
 *     event, it will end the discovery safely and the 
 *     BLEGATTEVENT_DESCRIPTORSEARCHCOMPLETE event will be received promptly.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it may be du 
 *     of an invalid connection handle.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter 
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_GetCharacteristicDescriptorList(BleGattClient client, 
									  U16 connHandle, 
									  AttHandle characStartingHandle, 
									  AttHandle characEndingHandle, 
									  BleGattCommand *command );
#endif //(BLEGATT_SUPPORT_GET_CHARACTERISTIC_DESCRIPTOR_LIST == 1)


#if (BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_HANDLE == 1)
/** Read the value of the characteristic specified by its handle   
 *   in the remote ATTRIBUTE SERVER
 *
 * BLEGATT_ReadCharacteristicValueByHandle():
 *    This sub-procedure is used by a CLIENT to read a Characteristic Value  
 *    from a SERVER when the CLIENT knows the Characteristic Value Handle
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 * BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_HANDLE shall be enabled
 *
 * @param client: the registered CLIENT to use in this procedure
 * @param connHandle: the connection handle that identify the ATTRIBUTE 
 *			SERVER holding the characteristic value to read.
 * @param valueHandle: the handle of the characteristic for  
 *			which the value should be read.
 * @param command: an unique GENERIC ATTRIBUTE Profile CLIENT command to
 *			execute this procedure
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation has started successfully;
 *     Once the value has been read in the remote SERVER, the 
 *     BLEGATTEVENT_CHARACTERISTICVALUEREAD event will be received in the  
 *     CLIENT callback with the status of the operation and the characteristic
 *     value information. 
 *     The BleGattCallbackParms->status field indicate the status of the 
 *     operation. 
 *     The BleGattCallbackParms->parms field indicates the value information as  
 *     a (BleGattCharacteristicValueInformation *) structure.
 *	   if BleGattCallbackParms->status is BLESTATUS_SUCCESS the valueHandle,
 *	   value, valueLen and command parms fields are valid during the callback.
 *	   if BleGattCallbackParms->status is BLESTATUS_FAILED the valueHandle,
 *	   errorCode and command parms fields are valid during the callback.
 *	
 *     The command can be reused after a BLEGATTEVENT_CHARACTERISTICVALUEREAD
 *     event.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it may be du 
 *     of an invalid connection handle.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter 
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_ReadCharacteristicValueByHandle(BleGattClient client, 
									  U16 connHandle, 
									  AttHandle valueHandle, 
									  BleGattCommand *command );
#endif //(BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_HANDLE == 1)

#if (BLEGATT_SUPPORT_READ_LONG_CHARACTERISTIC_VALUE == 1)
/** Read the value of the long characteristic specified by its handle   
 *   in the remote ATTRIBUTE SERVER
 *
 * BLEGATT_ReadLongCharacteristicValueByHandle():
 *    This sub-procedure is used to read a Characteristic Value from a SERVER  
 *    when the CLIENT knows the Characteristic Value Handle and the length of 
 *    the Characteristic Value is longer than can be sent in a single Read 
 *    Response Attribute Protocol message
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 * BLEGATT_SUPPORT_READ_LONG_CHARACTERISTIC_VALUE shall be enabled
 *
 * @param client: the registered CLIENT to use in this procedure
 * @param connHandle: the connection handle that identify the ATTRIBUTE 
 *			SERVER holding the characteristic long value to read.
 * @param valueHandle: the handle of the characteristic for  
 *			which the long value should be read.
 * @param command: an unique GENERIC ATTRIBUTE Profile CLIENT command to
 *			execute this procedure
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation has started successfully;
 *     Once the value has started to be read in the remote SERVER, the 
 *     BLEGATTEVENT_LONGCHARACTERISTICVALUEPART event will be received in the  
 *     CLIENT callback with a part of the characteristic long value 
 *     information. 
 *     The BleGattCallbackParms->parms field indicates the part value 
 *     information as a (BleGattLongCharacteristicValueInformation *) 
 *     structure.
 *
 *     Once the reading operation is completed (or if it failed) the 
 *     BLEGATTEVENT_LONGCHARACTERISTICVALUECOMPLETE event will be received in 
 *     the CLIENT callback. The BleGattCallbackParms->status field indicate   
 *     the status of the operation.
 *	
 *     The command can be reused after a 
 *     BLEGATTEVENT_LONGCHARACTERISTICVALUECOMPLETE event.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it may be du 
 *     of an invalid connection handle.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter 
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_ReadLongCharacteristicValueByHandle(
										BleGattClient client, 
										U16 connHandle, 
										AttHandle valueHandle, 
										BleGattCommand *command );
#endif //(BLEGATT_SUPPORT_READ_LONG_CHARACTERISTIC_VALUE == 1)

#if (BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_TYPE == 1)
/** Read the value of the characteristic specified by its type   
 *   in the remote ATTRIBUTE SERVER
 *
 * BLEGATT_ReadCharacteristicValueByType():
 *    This sub-procedure is used by a CLIENT to read a Characteristic Value  
 *    from a SERVER when the CLIENT knows the Characteristic UUID.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 * BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_TYPE shall be enabled
 *
 * @param client: the registered CLIENT to use in this procedure
 * @param connHandle: the connection handle that identify the ATTRIBUTE 
 *			SERVER holding the characteristic value to read.
 * @param serviceStartingHandle: the starting handle of the service in which 
 *			the characteristic value will be read.
 * @param serviceEndingHandle: the Ending handle of the service in which 
 *			the characteristic value will be read.
 * @param valueType: the UUID of the characteristic value to read.
 * @param command: an unique GENERIC ATTRIBUTE Profile CLIENT command to
 *			execute this procedure
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation has started successfully;
 *     Once the value has been read in the remote SERVER, the 
 *     BLEGATTEVENT_CHARACTERISTICVALUEREAD event will be received in the  
 *     CLIENT callback with the status of the operation and the characteristic
 *     value information. 
 *     The BleGattCallbackParms->status field indicate the status of the 
 *     operation. 
 *     The BleGattCallbackParms->parms field indicates the value information as  
 *     a (BleGattCharacteristicValueInformation *) structure.
 *	   if BleGattCallbackParms->status is BLESTATUS_SUCCESS the valueUuid,
 *	   value, valueLen and command parms fields are valid during the callback.
 *	   if BleGattCallbackParms->status is BLESTATUS_FAILED the valueUuid,
 *	   errorCode and command parms fields are valid during the callback.
 *	
 *     The command can be reused after a BLEGATTEVENT_CHARACTERISTICVALUEREAD
 *     event.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it may be du 
 *     of an invalid connection handle.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter 
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_ReadCharacteristicValueByType(
										BleGattClient client, 
										U16 connHandle, 
										AttHandle serviceStartingHandle, 
										AttHandle serviceEndingHandle, 
										AttUuid *valueType,
										BleGattCommand *command );
#endif //(BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_TYPE == 1)

#if (BLEGATT_SUPPORT_READ_MULTIPLE_VALUES == 1)
/** Read multiple values of characteristics specified by their handles   
 *   in the remote ATTRIBUTE SERVER
 *
 * BLEGATT_ReadMultipleValues():
 *    This sub-procedure is used to read multiple Characteristic Values 
 *    from a SERVER when the CLIENT knows the Characteristic Value Handles.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 * BLEGATT_SUPPORT_READ_MULTIPLE_VALUES shall be enabled
 *
 * @param client: the registered CLIENT to use in this procedure
 * @param connHandle: the connection handle that identify the ATTRIBUTE 
 *			SERVER holding the characteristics values to read.
 * @param handles: an array of (numHandles) handles specifying the 
 *          characteristics value handles.
 * @param numHandles: the number of handles contained in the handles array.
 * @param command: an unique GENERIC ATTRIBUTE Profile CLIENT command to
 *			execute this procedure
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation has started successfully;
 *     Once the value has been read in the remote SERVER, the 
 *     BLEGATTEVENT_CHARACTERISTICVALUEREAD event will be received in the  
 *     CLIENT callback with the status of the operation and the characteristic
 *     value information. 
 *     The BleGattCallbackParms->status field indicate the status of the 
 *     operation. 
 *     The BleGattCallbackParms->parms field indicates the value information as  
 *     a (BleGattCharacteristicValueInformation *) structure.
 *	   if BleGattCallbackParms->status is BLESTATUS_SUCCESS the
 *	   value, valueLen and command parms fields are valid during the callback.
 *	   if BleGattCallbackParms->status is BLESTATUS_FAILED the 
 *	   errorCode and command parms fields are valid during the callback.
 *	
 *     The command can be reused after a BLEGATTEVENT_CHARACTERISTICVALUEREAD
 *     event.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it may be du 
 *     of an invalid connection handle.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter 
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_ReadMultipleValues(
								BleGattClient client, 
								U16 connHandle, 
								AttHandle *handles, 
								U8	numHandles,
								BleGattCommand *command );
#endif //(BLEGATT_SUPPORT_READ_MULTIPLE_VALUES == 1)



#if (BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_WITHOUT_RESPONSE == 1)
/** Write the value of the characteristic specified by its handle   
 *   in the remote ATTRIBUTE SERVER
 *
 * BLEGATT_WriteCharacteristicValueWithoutResponse():
 *    This sub-procedure is used by a CLIENT to write a Characteristic Value
 *    from a SERVER when the CLIENT knows the Characteristic Value Handle.
 *    this procedure is considered as a non reliable write as the CLIENT does
 *    not have answer from the SERVER about the completion of the write 
 *    procedure.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 * BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_WITHOUT_RESPONSE shall be enabled
 *
 * @param client: the registered CLIENT to use in this procedure
 * @param connHandle: the connection handle that identify the ATTRIBUTE 
 *			SERVER holding the characteristic value to write.
 * @param valueHandle: the handle of the characteristic for  
 *			which the value should be written.
 * @param value: a valid pointer containing the value to write for in the  
 *          valueHandle in the remote ATTRIBUTE SERVER database. The pointer
 *          shall stay valid still the BLEGATTEVENT_WRITEWITHOUTRESPONSESENT
 *          is not received.
 * @param valueLen: the length of the value intented to be write in the  
 *          valueHandle in the remote ATTRIBUTE SERVER database.
 * @param command: an unique GENERIC ATTRIBUTE Profile CLIENT command to
 *			execute this procedure
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation has started successfully;
 *     Once the request has been sent to the remote SERVER, the 
 *     BLEGATTEVENT_WRITEWITHOUTRESPONSESENT event will be received in the  
 *     CLIENT callback with the status of the operation. 
 *	
 *     The command can be reused after a BLEGATTEVENT_WRITEWITHOUTRESPONSESENT
 *     event.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it may be du 
 *     of an invalid connection handle.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter 
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_WriteCharacteristicValueWithoutResponse(
								BleGattClient client, 
								U16 connHandle, 
								AttHandle valueHandle, 
								U8 *value, 
								U8 valueLen,
								BleGattCommand *command);
#endif //(BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_WITHOUT_RESPONSE == 1)

#if (BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_VALUE == 1)
/** Write reliably the value of the characteristic specified by its handle   
 *   in the remote ATTRIBUTE SERVER
 *
 * BLEGATT_WriteCharacteristicValue():
 *    This sub-procedure is used by a CLIENT to write a Characteristic Value
 *    from a SERVER when the CLIENT knows the Characteristic Value Handle.
 *    This procedure is considered as a reliable write as the CLIENT 
 *    have answer from the SERVER about the completion of the write 
 *    procedure.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 * BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_VALUE shall be enabled
 *
 * @param client: the registered CLIENT to use in this procedure
 * @param connHandle: the connection handle that identify the ATTRIBUTE 
 *			SERVER holding the characteristic value to write.
 * @param valueHandle: the handle of the characteristic for  
 *			which the value should be written.
 * @param value: a valid pointer containing the value to write for in the  
 *          valueHandle in the remote ATTRIBUTE SERVER database. The pointer
 *          shall stay valid still the 
 *          BLEGATTEVENT_CHARACTERISTICVALUEWRITECOMPLETE is not received.
 * @param valueLen: the length of the value intented to be write in the  
 *          valueHandle in the remote ATTRIBUTE SERVER database.
 * @param command: an unique GENERIC ATTRIBUTE Profile CLIENT command to
 *			execute this procedure
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation has started successfully;
 *     Once the operation is completed in the remote SERVER, the 
 *     BLEGATTEVENT_CHARACTERISTICVALUEWRITECOMPLETE event will be received in   
 *     the CLIENT callback with the status of the operation. 
 *
 *     The BleGattCallbackParms->status field indicate the status of the 
 *     operation. 
 *     The BleGattCallbackParms->parms field indicates the write information as  
 *     a (BleGattWriteRspInformation *) structure.
 *	   if BleGattCallbackParms->status is BLESTATUS_SUCCESS the valueHandle,
 *	   and command parms fields are valid during the callback.
 *	   if BleGattCallbackParms->status is BLESTATUS_FAILED the valueHandle,
 *	   errorCode and command parms fields are valid during the callback.	
 *
 *     The command can be reused after a 
 *     BLEGATTEVENT_CHARACTERISTICVALUEWRITECOMPLETE event.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it may be du 
 *     of an invalid connection handle.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter 
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_WriteCharacteristicValue(
							BleGattClient client, 
							U16 connHandle, 
							AttHandle valueHandle, 
							U8 *value, 
							U8 valueLen,
							BleGattCommand *command);
#endif //(BLEGATT_SUPPORT_WRITE_CHARACTERISTIC_VALUE == 1)

#if (BLEGATT_SUPPORT_WRITE_LONG_CHARACTERISTIC_VALUE == 1)
/** Write reliably the long value of the characteristic specified by its    
 *   handle in the remote ATTRIBUTE SERVER
 *
 * BLEGATT_WriteLongCharacteristicValue():
 *    This sub-procedure is used by a CLIENT to write a Characteristic 
 *    Long Value from a SERVER when the CLIENT knows the Characteristic 
 *    Value Handle.
 *    This procedure is considered also as a reliable write as the CLIENT 
 *    have answer from the SERVER about the completion of the write 
 *    procedure.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 * BLEGATT_SUPPORT_WRITE_LONG_CHARACTERISTIC_VALUE shall be enabled
 *
 * @param client: the registered CLIENT to use in this procedure
 * @param connHandle: the connection handle that identify the ATTRIBUTE 
 *			SERVER holding the characteristic long value to write.
 * @param valueHandle: the handle of the characteristic for  
 *			which the value should be written.
 * @param value: a valid pointer containing the long value to write for in the 
 *          valueHandle in the remote ATTRIBUTE SERVER database. The pointer
 *          shall stay valid still the 
 *          BLEGATTEVENT_LONGCHARACTERISTICVALUEWRITECOMPLETE is not received.
 * @param valueLen: the length of the value intented to be write in the  
 *          valueHandle in the remote ATTRIBUTE SERVER database.
 * @param command: an unique GENERIC ATTRIBUTE Profile CLIENT command to
 *			execute this procedure
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation has started successfully;
 *     Once the operation is completed in the remote SERVER, the 
 *     BLEGATTEVENT_LONGCHARACTERISTICVALUEWRITECOMPLETE event will be  
 *     received in the CLIENT callback with the status of the operation. 
 *
 *     The BleGattCallbackParms->status field indicate the status of the 
 *     operation. 
 *     The BleGattCallbackParms->parms field indicates the write information as  
 *     a (BleGattWriteRspInformation *) structure.
 *	   if BleGattCallbackParms->status is BLESTATUS_SUCCESS the valueHandle,
 *	   and command parms fields are valid during the callback.
 *	   if BleGattCallbackParms->status is BLESTATUS_FAILED the valueHandle,
 *	   errorCode and command parms fields are valid during the callback.	
 *
 *     The command can be reused after a 
 *     BLEGATTEVENT_LONGCHARACTERISTICVALUEWRITECOMPLETE event.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it may be du 
 *     of an invalid connection handle.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter 
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_WriteLongCharacteristicValue(
								BleGattClient client, 
								U16 connHandle, 
								AttHandle valueHandle, 
								U8 *value, 
								U16 valueLen,
								BleGattCommand *command);
#endif //(BLEGATT_SUPPORT_WRITE_LONG_CHARACTERISTIC_VALUE == 1)

/** Confim an ndictaion received by the remote ATTRIBUTE SERVER
 *
 * BLEGATT_ConfirmIndication():
 *    This function is used by a CLIENT to confirm an INDICATION received 
 *    from a SERVER. It should be called during the BLEGATTEVENT_INDICATION
 *    event received in the CLIENT callback.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 *
 * @param client: the registered CLIENT to use in this procedure
 * @param connHandle: the connection handle that identify the ATTRIBUTE 
 *			SERVER that send the INDICATION.
 * @param command: an unique GENERIC ATTRIBUTE Profile CLIENT command to
 *			execute this procedure
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation has started successfully;
 *     Once the confirmation is sent to the remote SERVER, the 
 *     BLEGATTEVENT_CONFIRMATIONSENT event will be  
 *     received in the CLIENT callback with the status of the operation. 	
 *
 *     The command can be reused after a 
 *     BLEGATTEVENT_CONFIRMATIONSENT event.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, it may be du 
 *     of an invalid connection handle.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter 
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGATT_ConfirmIndication(
							BleGattClient client, 
							U16 connHandle, 
							BleGattCommand *command);

/** Terminate the ongoing Sub-procedure
 *
 * BLEGATT_EndSubProcedure():
 *    It is permitted by the specification to end a discovery ( Service, 
 *    characteritic or characteritic descriptor) sub-procedure early if a desired   
 *    information is found prior to discovering all the items on the SERVER, so
 *    the HIGHER layers may call this function during the Sub-Procedure, it will
 *    end the discovery safely.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 * ATT_ROLE_CLIENT shall be enabled
 * GENERIC_ATTRIBUTE_PROFILE shall be enabled
 * BLE_CONNECTION_SUPPORT shall be enabled
 *
 * @param command: The unique GENERIC ATTRIBUTE Profile CLIENT command of the
 *			ongoing procedure
 *
 * @return none
 *	
 * @author Alexandre GIMARD
 */
void BLEGATT_EndSubProcedure(BleGattCommand *command);

#endif //(ATT_ROLE_CLIENT == 1)

#endif //(ATTRIBUTE_PROTOCOL == 1)

#endif //(GENERIC_ATTRIBUTE_PROFILE == 1)

#endif //__BLEGATT_H
