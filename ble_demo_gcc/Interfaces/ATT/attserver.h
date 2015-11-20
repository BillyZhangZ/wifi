#ifndef __ATTSERVER_H__
#define __ATTSERVER_H__
/****************************************************************************
 *
 * File:          attserver.h
 *
 * Description:   ATT API: all that is specific to the attribute server role.
 * 
 * Created:       August, 2008
 * Version:       0.1
 *
 * CVS Revision : $Revision: 1.25 $
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

/**
 * @file attserver.h
 *
 * ATT API: all that is specific to the attribute server role.
 *
 * @author xavier.boniface@alpwise.com
 *
 */

#include "ATT/AttTypes.h"

#if (ATTRIBUTE_PROTOCOL == 1)
#if (ATT_ROLE_SERVER == 1)

// Includes common ATT definitions. 
#include "ATT/AttBase.h"

/***************************************************************************\
 *	MACRO and CONSTANT definition
\***************************************************************************/
/**
 *  A Constant of to bytes with 0x00 value that may be used as default 
 * client configuration value
 *  or a readable only value of 0x0000
 */
extern CONST_DECL U8   AttServerClientConfigDefaultValue[2];  
/***************************************************************************\
 *	Types definition
\***************************************************************************/
/**
 * AttPermissions:
 * Read/Write permissions constraint if an attribute.
 * The default permission (0x00) of an attribute is to be 
 * readable and writable.
 *
 * Usage: Bitfield:
 *
 *  Bit 0 = authentication required for remote read access.
 *  Bit 1 = encryption     required for remote read access.
 *  Bit 2 = authorization  required for remote read access.
 *  Bit 3 = read forbidden. No client may read this attribute.
 *
 *  Bit 4 = authentication required for remote write access
 *  Bit 5 = encryption     required for remote write access
 *  Bit 6 = authorization  required for remote write access.
 *  Bit 7 = write forbidden. No client may write this attribute.
 */
typedef U8	AttPermissions;
#define ATT_PERMISSIONS_NONE                			0x00u
//ATT_PERMISSIONS_ALLACCESS means that the Attribute is readable and writable
// without any constraints
#define ATT_PERMISSIONS_ALLACCESS              			0x00u

#define ATT_PERMISSIONS_READ_AUTHENTICATION 			0x01u
#define ATT_PERMISSIONS_READ_ENCRYPTION     			0x02u
#define ATT_PERMISSIONS_READ_AUTHORIZATION  			0x04u
#define ATT_PERMISSIONS_READ_FORBIDDEN      			0x08u

#define ATT_PERMISSIONS_WRITE_AUTHENTICATION			0x10u
#define ATT_PERMISSIONS_WRITE_ENCRYPTION    			0x20u
#define ATT_PERMISSIONS_WRITE_AUTHORIZATION 			0x40u
#define ATT_PERMISSIONS_WRITE_FORBIDDEN     			0x80u

//ATT_PERMISSIONS_NOACCESS means that the Attribute is not readable and not writable
#define ATT_PERMISSIONS_NOACCESS                		0x88u

/**
 * @brief data for ATTEVT_SERVER_FREE
 *
 * 'data' field of the ATT's API server callback for this event.
 *
 */
typedef struct
{
  AttAttribute *attribute;
  U8           *value;
  U8           *uuid128;   /**< NULL if the attribute had a 16-bit UUID. */
} AttFreeData;


/**
 * @brief data for ATTEVT_SERVER_READ_REQ
 *
 * 'data' field of the ATT's API server callback for this event.
 */
typedef struct
{
	/** The attribute at which the client is willing to read. 
	 */
	const AttAttribute	*attribute;

 	/** The offset at which the client is willing to read. 
	 */
	U16					offset;   

	/* Return parameter! Shall be filled by the application within the callback.
	 * Allowed return codes are:
	 * - ATTPDU_STATUS_SUCCESS
	 * - or any application error code (i.e. any code between
	 *   ATTPDU_STATUS_APPLICATION_ERROR_FIRST and
	 *   ATTPDU_STATUS_APPLICATION_ERROR_LAST.
	 *
	 * Note: ATTPDU_STATUS_SUCCESS is the default value if 'response' is not
	 * filled.
	 */
	AttPduStatus       *response;
} AttReadReqData;


/**
 * @brief data for ATTEVT_SERVER_WRITE_REQ
 *
 * 'data' field of the ATT's API server callback for this event.
 */
typedef struct
{
	/** The attribute at which the client is willing to write. 
	 */
	const AttAttribute *attribute;

	/** The offset at which the client is willing to write.  
	 */
	U16					offset;  

	/** The value at which the client is willing to write.  
	 */
	const U8           *value; 

	/** The amount of bytes that the client is
	 *   willing to write as from this offset.  
	 */
	U16					length;   


	/* Return parameter! Shall be filled by the application within the callback.
	 * Allowed return codes are:
	 * - ATTPDU_STATUS_SUCCESS
	 * - or any application error code (i.e. any code between
	 *   ATTPDU_STATUS_APPLICATION_ERROR_FIRST and
	 *   ATTPDU_STATUS_APPLICATION_ERROR_LAST.
	 *
	 * Note: ATTPDU_STATUS_SUCCESS is the default value if 'response' is not
	 * filled.
	 */
	AttPduStatus       *response;

} AttWriteReqData;

/**
 * @brief data for ATTEVT_SERVER_WRITE_COMPLETE
 *
 * 'data' field of the ATT's API server callback for this event.
 */
typedef struct
{
	/** The attribute at which the client is willing to write. 
	 */
	const AttAttribute *attribute;

	/** The offset at which the client is willing to write.  
	 */
	U16					offset;  

	/** The value at which the client is willing to write.  
	 */
	const U8           *value; 

	/** The amount of bytes that the client is
	 *   willing to write as from this offset.  
	 */
	U16					length;   

} AttWriteCompleteData;

/**
 * Different events sent upstream in the ATT server callback.
 */
typedef U8 AttEvent;

/**
 * Events
 */

/**
 * An attribute has been fully removed from the database. The profile may now
 * free the associated memory if needed.
 *
 * @param status always is ATTSTATUS_SUCCESS
 * @param data is a: 'AttAttribute * attribute'
 */
#define ATTEVT_SERVER_FREE						1


/**
 * Notification that a client is willing to write an attribute value.
 *
 * Only issued if the 'write forbidden' permission bit is not set for this 
 * attribute.
 *
 * Shall be accepted or rejected by filling the 'response' field with either
 * ATTPDU_STATUS_SUCCESS to accept the writing, or any error code reserved to
 * the application to reject it; these are codes ranging from
 * ATTPDU_STATUS_APPLICATION_ERROR_FIRST to 
 * ATTPDU_STATUS_APPLICATION_ERROR_LAST.
 *
 * Note : The application AUTHORIZATION is also managed here, the 
 * ATTPDU_STATUS_INSUFFICIENT_AUTHORIZATION may be reported as error code
 *
 * Note: ATTPDU_STATUS_SUCCESS is the default value if 'response' is not
 * filled.
 *
 * @param status always is ATTSTATUS_SUCCESS
 * @param data is an "AttWriteReqData writeReq" structure.
 */
#define ATTEVT_SERVER_WRITE_REQ					2

/**
 * Notification that a client has successfully write an attribute value
 * it is sent to the higher layer when any of the ATT write procedure has
 * completed to write an attribute value
 *
 * @param status always is ATTSTATUS_SUCCESS
 * @param data is an "AttWriteCompleteData" structure.
 */
#define ATTEVT_SERVER_WRITE_COMPLETE			3

/**
 * Notification that a client is willing to read an attribute value.
 *
 * Only issued if the 'read forbidden' permission bit is not set for this 
 * attribute.
 *
 * Shall be accepted or rejected by filling the 'response' field with either
 * ATTPDU_STATUS_SUCCESS to accept the reading, or any error code reserved to
 * the application to reject it; these are codes ranging from
 * ATTPDU_STATUS_APPLICATION_ERROR_FIRST to 
 * ATTPDU_STATUS_APPLICATION_ERROR_LAST.
 *
 * Note : The application AUTHORIZATION is also managed here, the 
 * ATTPDU_STATUS_INSUFFICIENT_AUTHORIZATION may be reported as error code
 *
 * Note: ATTPDU_STATUS_SUCCESS is the default value if 'response' is not
 * filled.
 *
 * @param status always is ATTSTATUS_SUCCESS
 * @param data is an "AttReadReqData readReq" structure.
 */
#define ATTEVT_SERVER_READ_REQ					4

/**
 * Notification that a Handle Value Indication has been sent.
 *
 * @param status: if different than ATTSTATUS_SUCCESS, this means that the HVI
 * couldn't be sent.
 *
 * As many HVI_SENT events are issued as the number of HVIs that could succes-
 * sfully be sent.
 */
#define ATTEVT_SERVER_HVI_SENT					5



/**
 * @brief Notification that a Handle Value Confirmation was received.
 *
 * @param status: if different than ATTSTATUS_SUCCESS, this means that the
 * Handle Value Confirmation PDU is erroneous (too long. If too short, the
 * handle cannot even be read and the ATT cannot know which callback to
 * warn, so, it is just ignored).
 *
 */
#define ATTEVT_SERVER_HANDLE_VALUE_CONFIRMATION 6




/**
 * Parameters of the ATT callback.
 */
typedef struct
{
  AttEvent		event;
  AttStatus		status;
  AttDevice		peer;

  union {
    // Specific data types for the different event types here. 
	// In ATTEVT_SERVER_FREE event
	AttFreeData				free;
	// In ATTEVT_SERVER_WRITE_REQ event
	AttWriteReqData			writeReq;
	// In ATTEVT_SERVER_WRITE_COMPLETE event
	AttWriteCompleteData    writeComplete;
	// In ATTEVT_SERVER_READ_REQ event
	AttReadReqData			readReq;
	// In ATTEVT_SERVER_HANDLE_VALUE_CONFIRMATION event
	AttHandle				handle; 
  } parms; 

} AttServerCallbackParms;


/**
 * AttServerCallback
 * Prototype of the callback in which the SERVICE events wil be posted by
 * the stack
 *
 */
typedef void (*AttServerCallback)(AttServerCallbackParms *parms);


/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/

/**
 * ATT_SERVER_SecureDatabaseAccess
 * Prevents other service/profiles/remote device from accessing the  
 * database while the profile/application is doing it.
 *
 * Once done, ATT_SERVER_ReleaseDatabaseAccess() shall be called so that other
 * threads may access the database (e.g. a remote attribute client through
 *  PDUs).
 *
 * Calls to ATT_SERVER_SecureDatabaseAccess() / 
 * ATT_SERVER_ReleaseDatabaseAccess() shall surround all calls to the ATT
 * server API functions. A sequence of APIs may be called in the same
 * 'secure session'. This guarantees atomicity of the operations to the peer's
 * eyes.
 * Example:
 *   ATT_SERVER_SecureDatabaseAccess();
 *   ATT_SERVER_RegisterServiceAttribute(...);
 *   ATT_SERVER_AddCharacteristic(...);
 *   ATT_SERVER_ReleaseDatabaseAccess();
 *
 * @param none
 *
 * @return nothing
 *	
 * @author Alexandre GIMARD
 */
void ATT_SERVER_SecureDatabaseAccess(void);

/**
 * Release the database access previoulsy locked by 
 * ATT_SERVER_SecureDatabaseAccess.
 *
 * @see ATT_SERVER_SecureDatabaseAccess
 *
 * @param none
 *
 * @return nothing
 *	
 * @author Alexandre GIMARD
 */
void ATT_SERVER_ReleaseDatabaseAccess(void);



/**
 * ATT_SERVER_RegisterServiceAttribute
 * Start a new empty service page i.e. add a service attribute to the
 *        database.
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (return ATTSTATUS_INVALID_PARM otherwise)
 *
 * @param [in] valueLen: the length of the value of this attribute. Remmember
 *        that the value for service attributes is a UUID, so, length shall be
 *        2 for 16-bit UUIDs, or 16 for 128-bit UUIDs. 
 *        ATTPDU_SIZEOF_16_BIT_UUID and ATTPDU_SIZEOF_128_BIT_UUID constant 
 *		  should be used to avoid any problems.
 *
 * @param [in] value: the value (the service UUID), in little endian format.
 *        Until the ATTEVT_SERVER_FREE is received, the <valueLen> bytes at
 *        this location shall not be modified any more.
 *
 * @param [in] callback: the callback through which the profile application
 *        wants to receive all events related to this attribute and to all
 *        attributes which will be added to this service:
 *          - ATTEVT_SERVER_FREE
 *          - ATTEVT_SERVER_WRITE_REQ
 *			- ATTEVT_SEREVR_WRITE_COMPLETE
 *          - ATTEVT_SERVER_HVI_SENT
 *          - ATTEVT_SERVER_HANDLE_VALUE_CONFIRMATION
 *        May be NULL if the profile is not interested in any of these events.
 * 
 * @param [in] serviceAttribute: a memory block allocated by the profile/
 *        application for use by the ATT to store the attribute. Shall not be
 *         currently in the database already (this is from the time an
 *        ATT_SERVER_RegisterServiceAttribute() 
 *        is performed, until the corresponding ATTEVT_SERVER_FREE is
 *        received).
 *
 * @return The status of this operation:
 *		- ATTSTATUS_SUCCESS means that the service attribute could be added.
 *      - ATTSTATUS_DATABASE_FROZEN (only in case ATT_SERVER_HANDLES_ARE_FREEZABLE) if
 *        ATT_SERVER_FreezeDatabase() has already been called, in which case
 *        more change is accepted in the database.
 *      - ATTSTATUS_BUSY if there is at least one link layer connection, in
 *        which case the database's structure shall not be modified.
 *		- ATTSTATUS_DATABASE_FULL if there is no room in the handle's space to
 *        APPEND this attribute at the back of the database.
 *      - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *        asserted otherwise if ATT_DEBUG == 1) if valueLen is not 2 or 16, or
 *        any pointer parameter is NULL.
 *		  ( always asserted) if database is not locked.
 *		- ATTSSTATUS_FAILED: if the service memory is already in the database.
 *
 * @author Alexandre GIMARD
 */
AttStatus ATT_SERVER_RegisterServiceAttribute(
  U16                  valueLen,
  U8                  *value,    
  AttServerCallback    callback,
  AttServiceAttribute *memory
  );


/**
 * @brief Remove an attribute from the database.
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (asserted if ATT_DEBUG ==1).
 *
 * This causes an ATTEVT_SERVER_FREE event to be received in the associated 
 * service's callback during this function call.
 * 
 * If the attribute is a service attribute which was REGISTERED (using
 * ATT_SERVER_RegisterServiceAttribute() rather than just
 * ATT_SERVER_AddAttribute()), all attributes which belong to this service are
 * removed (causing as many ATTEVT_SERVER_FREE events), and the service,
 * is free'd at last).
 * 
 * @param attribute The attribute to remove.
 *
 * @return The status of this operation:
 *		- ATTSTATUS_SUCCESS means that the service attribute could be removed.
 *      - ATTSTATUS_DATABASE_FROZEN (only in case ATT_SERVER_HANDLES_ARE_FREEZABLE) if
 *        ATT_SERVER_FreezeDatabase() has already been called, in which case
 *        more change is accepted in the database.
 *      - ATTSTATUS_BUSY if there is at least one link layer connection, in
 *        which case the database's structure shall not be modified.
 *      - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *        asserted otherwise if ATT_DEBUG == 1) if the attribute is not in the
 *        database.
 *
 * @author Alexandre GIMARD
 */
AttStatus ATT_SERVER_RemoveAttribute(AttAttribute *attribute);

/**
 * @brief Add a characteristic to a service page i.e. following a given service
 *        attribute.
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (return ATTSTATUS_INVALID_PARM otherwise)
 *
 * @param [in] characteriticProperty: The Characteristic Properties bit
 *        field determines how the Characteristic Value can be used, or how
 *        the characteristic descriptors (see Section 3.3.3) can be accessed.
 *        If the property is set, the action described is permitted. Multiple
 *        Characteristic Properties can be set.
 *
 * @param [in] characteristic: A memory Block used by the stack to store the
 *        characteristic needed memory.
 *
 * @param [in] uuid: The characteristic value attribute's type (aka. uuid): a 
 *		16 or 128-bits UUID.
 *   	Note 1: if this is a 128-bit uuid, the uuid->value.uuid128 pointer on  
 *   	the UUID shall be kept unchanged at this location until the 
 *   	corresponding ATTEVT_SERVER_FREE is released.
 *   	Note 2: this may be a service UUID, but it will not be tagged in the
 *   	database as being the start of a 'page'). Hence, for example, removing 
 *   	it won't cause any following attribute to be removed as well (see
 *   	ATT_SERVER_RemoveAttribute()).
 * 
 * @param [in] valueRemoteAccessPermissions: Whether the characteristic value  
 *        may be read/written by peer devices.
 *
 * @param [in] valueLen: the length of the value of this characteristic value.
 *
 * @param [in] value: the value in little endian format.
 *        Until the ATTEVT_SERVER_FREE is received, the <valueLen> bytes at
 *        this location shall not be modified any more (unless
 *        ATT_Server_WriteAttribute() is called).
 *
 * @param [in] isVariableValueLengh: defines if the characteristic value size 
 *		  is fixed (set to 0) or variable (set to 1). it is usefull in case of
 *		  the attribute is writable to define if the remote is allowed to write 
 *        this kind of value Length
 *
 * @param [in] maxValueLen: in case of the characteristic value size is variable 
 *        the maximum size allowed for the attribute.
 *
 * @param [in] serviceAttribute: the service to which this 
 *        characteristic belongs. Shall have been previously registered with 
 *        ATT_SERVER_RegisterServiceAttribute().
 * 
 * @param [in] valueAttribute: a memory block allocated by the profile/application for
 *        use by the ATT to store the characteristic value attribute memory. 
 *         Shall not be currently in the
 *        database already (this is from ATT_SERVER_Addcharacteristic()
 *        is performed, until the corresponding ATTEVT_SERVER_FREE is
 *        received).
 *
 * @return The status of this operation:
 *		- ATTSTATUS_SUCCESS means that the service attribute has been added.
 *      - ATTSTATUS_DATABASE_FROZEN (only in case ATT_SERVER_HANDLES_ARE_FREEZABLE) if
 *        ATT_SERVER_FreezeDatabase() has already been called, in which case
 *        more change is accepted in the database.
 *      - ATTSTATUS_BUSY if there is at least one link layer connection, in
 *        which case the database's structure shall not be modified.
 *		- ATTSTATUS_DATABASE_FULL if there is no room in the handle's space to
 *        APPEND this attribute AFTER the last attribute currently belonging
 *        to this service (might be the service attribute itself).
 *      - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *        asserted otherwise if ATT_DEBUG == 1) if the UUID is not a 16 or 128
 *        bit one, or any pointer parameter is NULL, or the serviceAttribute
 *        is invalid or not registered or memory is already in the database.
 *
 * @author Alexandre GIMARD
 */
AttStatus ATT_SERVER_AddCharacteristic(
  AttCharacteriticProperties		characteriticProperty,
  Att16BitCharacteristicAttribute	*characteristic,
  AttUuid							*uuid,
  AttPermissions					valueRemoteAccessPermissions,
  U16								valueLen,
  U8								*value,
  U8								isVariableValueLengh,
  U16								maxValueLen,
  AttServiceAttribute				*serviceAttribute,
  AttAttribute						*valueAttribute
  );

/** ATT_SERVER_AddCharacteristicUserDescription
 * Add a characteristic Format descriptor to a specified
 * characteristic
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (return ATTSTATUS_INVALID_PARM otherwise)
 *
 * @param characteristic: the characteristic handle for which this descriptor
 *         applies
 *
 * @param userDescription: a valid pointer to the user description descriptor
 *			value, this pointer shall be alive until the characteristic is available
 *			in the database.
 *
 * @param userDescriptionLen: the length of data in the userDescription memory,
 *			it shall not exceed the maximum defined value for an attribute
 *			( ATT_MAX_VALUE_LENGTH )
 *
 * @param attribute: a memory block allocated by the application used by the
 *		   ATT to manage the descriptor. It shall be left allocated during all
 *		   the life time of the service/characteristic.
 *
 * @return The status of this operation:
 *		- ATTSTATUS_SUCCESS means that the characteristic have been added.
 *      - ATTSTATUS_DATABASE_FROZEN (only in case ATT_SERVER_HANDLES_ARE_FREEZABLE) if
 *        ATT_SERVER_FreezeDatabase() has already been called, in which case
 *        more change is accepted in the database.
 *      - ATTSTATUS_BUSY if there is at least one link layer connection, in
 *        which case the database's structure shall not be modified.
 *		- ATTSTATUS_DATABASE_FULL if there is no room in the handle's space to
 *        APPEND this attribute AFTER the last attribute currently belonging
 *        to this service (might be the service attribute itself).
 *      - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *        asserted otherwise if ATT_DEBUG == 1) if the UUID is not a 16 or 128
 *        bit one, or any pointer parameter is NULL, or the serviceAttribute
 *        is invalid or not registered or memory is already in the database.
 *
 * @author Alexandre GIMARD
 */
AttStatus ATT_SERVER_AddCharacteristicUserDescription(
  Att16BitCharacteristicAttribute				 *characteristic,
  U16						 userDescriptionLen,
  U8						 *userDescription,
  AttAttribute				 *attribute
  );


/** ATT_SERVER_AddCharacteristicFormat
 * Add a characteristic Format descriptor to a specified
 * characteristic
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (return ATTSTATUS_INVALID_PARM otherwise)
 *
 * @param characteristic: the characteristic handle for which this descriptor
 *         applies
 *
 * @param format: A pointer to a memory block containing the value of the 
 *		  format descriptor, this memory block sahhl be allocated by the
 *		  application and shall stay valid still the characteristic is 
 *		  available in the ATTTRIBUTE database, the format is a seven bit 
 *		  length data block.
 *
 * @param attribute: a memory block allocated by the application used by the
 *		   ATT to manage the descriptor. It shall be left allocated during all
 *		   the life time of the service/characteristic.
 *
 * @return The status of this operation:
 *		- ATTSTATUS_SUCCESS means that the characteristic have been added.
 *      - ATTSTATUS_DATABASE_FROZEN (only in case ATT_SERVER_HANDLES_ARE_FREEZABLE) if
 *        ATT_SERVER_FreezeDatabase() has already been called, in which case
 *        more change is accepted in the database.
 *      - ATTSTATUS_BUSY if there is at least one link layer connection, in
 *        which case the database's structure shall not be modified.
 *		- ATTSTATUS_DATABASE_FULL if there is no room in the handle's space to
 *        APPEND this attribute AFTER the last attribute currently belonging
 *        to this service (might be the service attribute itself).
 *      - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *        asserted otherwise if ATT_DEBUG == 1) if the UUID is not a 16 or 128
 *        bit one, or any pointer parameter is NULL, or the serviceAttribute
 *        is invalid or not registered or memory is already in the database.
 *
 * @author Alexandre GIMARD
 */
AttStatus ATT_SERVER_AddCharacteristicFormat(
  Att16BitCharacteristicAttribute				 *characteristic,
  U8						 *format,
  AttAttribute				 *attribute
  );

/** ATT_SERVER_AddCharacteristicExtendedProperties
 * Add a characteristic Extended Properties config descriptor to a specified
 * characteristic
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (return ATTSTATUS_INVALID_PARM otherwise)
 *
 * @param characteristic: the characteristic handle for which this descriptor
 *         applies
 *
 * @param extProperties: the initial value of the configuration
 *
 * @param attribute: a memory block allocated by the application used by the
 *		   ATT to manage the descriptor. It shall be left allocated during all
 *		   the life time of the service/characteristic.
 *
 * @return The status of this operation:
 *		- ATTSTATUS_SUCCESS means that the characteristic have been added.
 *      - ATTSTATUS_DATABASE_FROZEN (only in case ATT_SERVER_HANDLES_ARE_FREEZABLE) if
 *        ATT_SERVER_FreezeDatabase() has already been called, in which case
 *        more change is accepted in the database.
 *      - ATTSTATUS_BUSY if there is at least one link layer connection, in
 *        which case the database's structure shall not be modified.
 *		- ATTSTATUS_DATABASE_FULL if there is no room in the handle's space to
 *        APPEND this attribute AFTER the last attribute currently belonging
 *        to this service (might be the service attribute itself).
 *      - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *        asserted otherwise if ATT_DEBUG == 1) if the UUID is not a 16 or 128
 *        bit one, or any pointer parameter is NULL, or the serviceAttribute
 *        is invalid or not registered or memory is already in the database.
 *
 * @author Alexandre GIMARD
 */
AttStatus ATT_SERVER_AddCharacteristicExtendedProperties(
  Att16BitCharacteristicAttribute	*characteristic,
  U16								 extProperties,
  AttAttribute						*attribute
  );

/** ATT_SERVER_AddCharacteristicClientConfig
 * Add a characteristic server config descriptor to a specified
 * characteristic
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (return ATTSTATUS_INVALID_PARM otherwise)
 *
 * @param characteristic: the characteristic handle for which this descriptor
 *         applies
 *
 * @param defaultValue: the default initial value of the configuration, it is 
 *        a constant conataining the default value, if te default value for 
 *        this configuration is 0x0000, this parameter may point to
 *        AttServerClientConfigDefaultValue
 *
 * @param instantiatedValueMemory: A valid pointer to an Array that will 
 *        contain internal Attribute protocol data, in order to save instances
 *        of the value.
 *        The size of the array must be BLE_MAX_NUM_CONNECTION * 2
 *
 * @param isValuePersistent: a variable indicating if the instance of the 
 *        attribute value must be saved into non volatilte memory in order to be
 *        restored upon reconnection.
 *        - if 1 the value is saved in persistent memory if local and remote 
 *          devices are bonded together.
 *        - if 0 the value is not saved in persistent memory.
 *
 * @param infoType: a constant indicating in which infotype the information is
 *        saved in persistent memory.
 *        This parameter is ignored if isValuePersistent is equal to zero.
 *
 * @param attribute: A memory block allocated by the application used by the
 *		   ATT to manage the Attribute. It shall be left allocated during all
 *		   the life time of the service/characteristic.
 *
 * @return The status of this operation:
 *		- ATTSTATUS_SUCCESS means that the characteristic have been added.
 *      - ATTSTATUS_DATABASE_FROZEN (only in case ATT_SERVER_HANDLES_ARE_FREEZABLE) if
 *        ATT_SERVER_FreezeDatabase() has already been called, in which case
 *        more change is accepted in the database.
 *      - ATTSTATUS_BUSY if there is at least one link layer connection, in
 *        which case the database's structure shall not be modified.
 *		- ATTSTATUS_DATABASE_FULL if there is no room in the handle's space to
 *        APPEND this attribute AFTER the last attribute currently belonging
 *        to this service (might be the service attribute itself).
 *      - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *        asserted otherwise if ATT_DEBUG == 1) if the UUID is not a 16 or 128
 *        bit one, or any pointer parameter is NULL, or the serviceAttribute
 *        is invalid or not registered or memory is already in the database.
 *
 * @author Alexandre GIMARD
 */
AttStatus ATT_SERVER_AddCharacteristicClientConfig(
	Att16BitCharacteristicAttribute		*characteristic,
    const U8                            *defaultValue,
    U8                                  *instantiatedValueMemory,
    U8                                  isValuePersistent,
    U8                                  infoType,
	AttInstantiatedAttribute    		*attribute );


/** ATT_SERVER_AddCharacteristicServerConfig
 * Add a specfic characteristic server config descriptor to a specified
 * characteristic
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (return ATTSTATUS_INVALID_PARM otherwise)
 *
 * @param characteristic: the characteristic handle for which this descriptor
 *         applies
 *
 * @param serverConfig: the value of the configuration
 *
 * @param attribute: a memory block allocated by the application used by the
 *		   ATT to manage the descriptor. It shall be left allocated during all
 *		   the life time of the service/characteristic.
 *
 * @return The status of this operation:
 *		- ATTSTATUS_SUCCESS means that the characteristic have been added.
 *      - ATTSTATUS_DATABASE_FROZEN (only in case ATT_SERVER_HANDLES_ARE_FREEZABLE) if
 *        ATT_SERVER_FreezeDatabase() has already been called, in which case
 *        more change is accepted in the database.
 *      - ATTSTATUS_BUSY if there is at least one link layer connection, in
 *        which case the database's structure shall not be modified.
 *		- ATTSTATUS_DATABASE_FULL if there is no room in the handle's space to
 *        APPEND this attribute AFTER the last attribute currently belonging
 *        to this service (might be the service attribute itself).
 *      - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *        asserted otherwise if ATT_DEBUG == 1) if the UUID is not a 16 or 128
 *        bit one, or any pointer parameter is NULL, or the serviceAttribute
 *        is invalid or not registered or memory is already in the database.
 *
 * @author Alexandre GIMARD
 */
AttStatus ATT_SERVER_AddCharacteristicServerConfig(
  Att16BitCharacteristicAttribute	*characteristic,
  U16								 config,
  AttAttribute						*attribute
  );

/** ATT_SERVER_AddCharacteristicSpecificDescriptor
 * Add a specfic characteristic specific descriptor to a specified
 * characteristic
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (return ATTSTATUS_INVALID_PARM otherwise)
 *
 * @param characteristic: the characteristic handle for which this descriptor
 *         applies
 *
 * @param [in] uuid: The characteristic descriptor attribute's type (aka. uuid): a 
 *		16 or 128-bits UUID.
 *   	Note 1: if this is a 128-bit uuid, the uuid->value.uuid128 pointer on  
 *   	the UUID shall be kept unchanged at this location until the 
 *   	corresponding ATTEVT_SERVER_FREE is released.
 *   	Note 2: this may be a service UUID, but it will not be tagged in the
 *   	database as being the start of a 'page'). Hence, for example, removing 
 *   	it won't cause any following attribute to be removed as well (see
 *   	ATT_SERVER_RemoveAttribute()).
 * 
 * @param [in] remoteAccessPermissions: Whether the characteristic descriptor  
 *        may be read/written by peer devices.
 *
 * @param [in] valueLen: the length of the value of this characteristic descriptor.
 *
 * @param [in] value: the value in little endian format.
 *        Until the ATTEVT_SERVER_FREE is received, the <valueLen> bytes at
 *        this location shall not be modified any more (unless
 *        ATT_Server_WriteAttribute() is called).
 *
 * @param [in] isVariableValueLengh: defines if the characteristic descriptor
 *		  value size  is fixed (set to 0) or variable (set to 1). it is usefull 
 *		  in case of the attribute is writable to define if the remote is allowed 
 *        to write this kind of value Length
 *
 * @param [in] maxValueLen: in case of the characteristic descriptor value size 
 *        is variable the maximum size allowed for the attribute.
 *
 * @param attribute: a memory block allocated by the application used by the
 *		  ATT to manage the descriptor. It shall be left allocated during all
 *		  the life time of the service/characteristic.
 *
 * @return The status of this operation:
 *		- ATTSTATUS_SUCCESS means that the characteristic have been added.
 *      - ATTSTATUS_DATABASE_FROZEN (only in case ATT_SERVER_HANDLES_ARE_FREEZABLE) if
 *        ATT_SERVER_FreezeDatabase() has already been called, in which case
 *        more change is accepted in the database.
 *      - ATTSTATUS_BUSY if there is at least one link layer connection, in
 *        which case the database's structure shall not be modified.
 *		- ATTSTATUS_DATABASE_FULL if there is no room in the handle's space to
 *        APPEND this attribute AFTER the last attribute currently belonging
 *        to this service (might be the service attribute itself).
 *      - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *        asserted otherwise if ATT_DEBUG == 1) if the UUID is not a 16 or 128
 *        bit one, or any pointer parameter is NULL, or the serviceAttribute
 *        is invalid or not registered or memory is already in the database.
 *
 * @author Alexandre GIMARD
 */
AttStatus ATT_SERVER_AddCharacteristicSpecificDescriptor(
	Att16BitCharacteristicAttribute		*characteristic,
	AttUuid             *uuid,
	AttPermissions      remoteAccessPermissions,
	U16                 valueLen,
	U8                  *value,
	U8					isVariableValueLengh,
	U16					maxValueLen,
	AttAttribute		*attribute );


/** ATT_SERVER_AddServiceRelashionship
 * Add a service inclusion in the given Service
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (return ATTSTATUS_INVALID_PARM otherwise)
 *
 * @param [in] serviceAttribute: A valid pointer to the service in which the  
 *		included service will be included
 *
 * @param [in] includedServiceAttribute: A valid pointer to the service to 
 *		include into the "serviceAttribute" 
 * 
 * @param [in] includeAttribute: A memory block allocated by the application 
 *		  used by the ATT to manage the included service attribute. 
 *		  It shall be left allocated during all the life time of the
 *		  service/characteristic.
 *
 * @param [in] includeValue: A memory block allocated by the application 
 *		  used by the ATT to manage the included service attribute value. 
 *		  It shall be left allocated during all the life time of the
 *		  service/characteristic.
 *		  The size of the memory block depend of the included service:
 *		  - If the included service is a 16 bit UUID then the memory block
 *		  shall have a length of 6 bytes.
 *		  - If the included service is a 128 bit UUID (or it UUID size is not
 *		  known )then the memory block shall have a length a 20 bytes
 *
 * @return The status of this operation:
 *		- ATTSTATUS_SUCCESS means that the service inclusion have been added.
 *      - ATTSTATUS_DATABASE_FROZEN (only in case ATT_SERVER_HANDLES_ARE_FREEZABLE) if
 *        ATT_SERVER_FreezeDatabase() has already been called, in which case
 *        more change is accepted in the database.
 *      - ATTSTATUS_BUSY if there is at least one link layer connection, in
 *        which case the database's structure shall not be modified.
 *		- ATTSTATUS_DATABASE_FULL if there is no room in the handle's space to
 *        APPEND this attribute AFTER the last attribute currently belonging
 *        to this service (might be the service attribute itself).
 *      - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *        asserted otherwise if ATT_DEBUG == 1) if the UUID is not a 16 or 128
 *        bit one, or any pointer parameter is NULL, or the serviceAttribute
 *        is invalid or not registered or memory is already in the database.
 *
 * @author Alexandre GIMARD
 */
AttStatus ATT_SERVER_AddServiceRelashionship(
	AttServiceAttribute		*serviceAttribute,
	AttServiceAttribute		*includedServiceAttribute,
	AttAttribute			*includeAttribute,
	U8						*includeValue );

#if (ATT_SERVER_HANDLES_ARE_FREEZABLE == 1)
/**
 * @brief Flags that the database will not be modified any more.
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (return ATTSTATUS_INVALID_PARM otherwise).
 *
 * Remove the 'Attribute Handles Changed' attribute, indicating to any client
 * that the handles will not change on this device any more.
 * @todo Clarify in spec whether these handles are allowed to change at next
 * device's startup, or whether they shall remain as is for the device's lifetime.
 *
 * As from then, all following attempts to add or remove attributes (including
 * services) will be rejected (error code: ATTSTATUS_DATABASE_FROZEN). Read/
 * Write operations which affect only the attributes' VALUES (and not their
 * handle) will still succeed.
 *
 * @return
 *			- ATTSTATUS_SUCCESS if the database is now frozen.
 *      - ATTSTATUS_DATABASE_FROZEN if this function has already been called.
 *      - ATTSTATUS_BUSY if there is at least one link layer connection, in
 *        which case it is not allowed to freeze the database (because this
 *        causes the removal of the Attribute Handles Changed and the
 *        database's structure shall not be modified when there is a link
 *        layer connection.
 *      - ATTSTATUS_INTERNAL
 */
AttStatus ATT_SERVER_FreezeDatabase(void);
#endif //(ATT_SERVER_HANDLES_ARE_FREEZABLE == 1)


/**
 * @brief Read the value of an attribute in the attribute database.
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (asserted if ATT_DEBUG ==1).
 *
 * Note: the profile/application already knows the address of the buffer
 * holding the value, but the length may have changed.
 * 
 * @param attribute [in ]  the attribute. May be a service attribute (need
 *                         cast then).
 *
 * @param value     [out]: a pointer on the buffer holding the value. The
 *   value it points shall no more be read after
 * ATT_SERVER_ReleaseDatabaseAccess() has been called.
 *
 * @param length    [out]: the length of the value.
 * 
 * @return
 *   - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *     asserted otherwise if ATT_DEBUG == 1) if the attribute is NULL or could
 *     not be found in the database.
 *   - ATTSTATUS_SUCCESS otherwise.
 */
AttStatus ATT_SERVER_ReadAttributeValue(const AttAttribute   *attribute,
                                              U8            **value,
                                              U16            *length);


/**
 * @brief Read the value of an client instantiated attribute in the 
 * attribute database.
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (asserted if ATT_DEBUG ==1).
 *
 * Note: the profile/application already knows the address of the buffer
 * holding the value, but the length may have changed.
 * 
 * @param attribute [in ]  the instantiated attribute. 
 *
 * @param connHandle [in ]  the connection handle identifying the client. 
 *
 * @param value     [out]: a pointer on the buffer holding the value. The
 *   value it points shall no more be read after
 * ATT_SERVER_ReleaseDatabaseAccess() has been called.
 *
 * @param length    [out]: the length of the value.
 * 
 * @return
 *   - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *     asserted otherwise if ATT_DEBUG == 1) if the attribute is NULL or could
 *     not be found in the database.
 *   - ATTSTATUS_FAILED means that the connection handle does not represent a
 *      valid connection or the attribute is not instantiated
 *   - ATTSTATUS_SUCCESS otherwise.
 */
AttStatus ATT_SERVER_ReadInstantiatedAttributeValue( 
                        AttInstantiatedAttribute   *instantiatedAttribute,
                        U16                         connHandle,
						U8                        **value,
						U16                        *length);

/**
 * @brief Write the value of an client instantiated attribute in the 
 * attribute database.
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (asserted if ATT_DEBUG ==1).
 * 
 * @param attribute [in ]  the instantiated attribute. 
 *
 * @param connHandle [in ]  the connection handle identifying the client. 
 *
 * @param value     [out]: a pointer on the buffer holding the value. The
 *   value it points shall no more be read after
 * ATT_SERVER_ReleaseDatabaseAccess() has been called.
 *
 * @param length    [out]: the length of the value.
 * 
 * @return
 *   - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *     asserted otherwise if ATT_DEBUG == 1) if the attribute is NULL or could
 *     not be found in the database.
 *   - ATTSTATUS_FAILED means that the connection handle does not represent a
 *      valid connection or the attribute is not instantiated
 *   - ATTSTATUS_SUCCESS otherwise.
 */
AttStatus ATT_SERVER_WriteInstantiatedAttributeValue( 
                        AttInstantiatedAttribute   *instantiatedAttribute,
                        U16                         connHandle,
                        U8                         *value,
                        U16                         length);
/**
 * ATT_SERVER_WriteAttributeValue
 * Re-write the value of an attribute in the attribute database.
 *
 * Even if the buffer remains the same, this api shall be called to notify the
 * att that the value in it changed! (since the ATT needs to perform some
 * operations).
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (return ATTSTATUS_INVALID_PARM otherwise).
 * 
 * @param attribute [in ]: the attribute to write.
 *
 * @param value     [in ]: a pointer on the buffer holding the new value.
 *  - Until the ATTEVT_SERVER_FREE is received, the <valueLen> bytes at
 *        this location shall not be modified any more or released by the
 *		  application, the value is not copied inside the stack but referenced
 *		  with this buffer.
 *
 * @param length    [in ]: the length of the value, if the attribute is a fixed
 *		  length attribute, then shall be the same length that the previous length.
 * 
 * @return
 *   - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *     asserted otherwise if ATT_DEBUG == 1) 
 *   - ATTSTATUS_FAILED if the attribute is NULL or could not be found in the
 *     database.
 *   - ATTSTATUS_SUCCESS otherwise.
 *
 * @author Alexandre GIMARD
 */
AttStatus ATT_SERVER_WriteAttributeValue(      AttAttribute *attribute,
                                         const U8           *value,
                                               U16           length);

/**
 * @brief Gives the handle of an attribute in the database.
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (asserted if ATT_DEBUG ==1).
 *
 * When an attribute is added in the database, it is given a handle
 * via which an attribute client can access (read/write...) this attribute.
 * The local application still always access any attribute through its
 * address (AttAttribute *), but it might be willing to know which handle the
 * attribute was given.
 *
 * @param attribute [in ] designs an attribute which has been added in the
 *                        database.
 *
 * @param handle    [out] the handle (in case the function succeeds).
 *
 * @return 
 *   - ATTSTATUS_INVALID_PARM (only checked if ATT_PARMS_CHECK == 1 ;
 *     asserted otherwise if ATT_DEBUG == 1) if 'attribute' == NULL, or
 *     could not be found in the database, or 'handle' == NULL.
 *   - ATTSTATUS_SUCCESS otherwise.
 *
 * @todo: replace by a 'Get Properties' (or '"Read" Properties' (?)) API?
 * parms: const *attribute               [in]
 *              *uuid                    [out]
 *              *remoteAccessPermissions [out]
 *              *handle                  [out]
 */
AttStatus ATT_SERVER_GetHandle(AttAttribute *attribute, AttHandle *handle);

#if (ATT_SERVER_SUPPORT_NOTIFICATION == 1)
/**
 * ATT_SERVER_SendNotification
 * Send a 'Handle Value Notification' to an attribute client.
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (return ATTSTATUS_INVALID_PARM otherwise)
 *
 * ATT_SERVER_SUPPORT_NOTIFICATION shall be ENABLED
 *
 * @param attribute  : the attribute which value should be sent.
 * @param destination: the connection handle to the device to which 
 *       this should be sent.
 *
 * 
 * @return The status of the operation:
 *	- ATTSTATUS_PENDING if the HVN could be placed in the send queue. 
 *  - ATTSTATUS_INVALID_PARM (only if ATT_PARMS_CHECK==1 ; asserted otherwise
 *     if ATT_DEBUG==1) if:
 *     - attribute cannot be found in the database, or
 *     - attribute can be found in the database but is not committed, or
 *     - channel is an invalid combination.
 *		(always asserted) 
 *	   - the database is not locked
 *  - ATTSTATUS_FAILED if the channel is closed at the level below, or if the
 *		packet can not be placed in the send queue.
 */
AttStatus ATT_SERVER_SendNotification(
            const AttAttribute   *attribute,
            const AttDevice       destination
          );
#endif //(ATT_SERVER_SUPPORT_NOTIFICATION == 1)


#if (ATT_SERVER_SUPPORT_INDICATION == 1)
/**
 * ATT_SERVER_SendIndication 
 * Send a 'Handle Value Indication' to an attribute client.
 *
 * This function shall be called while the database has been secured using
 * ATT_SERVER_SecureDatabaseAccess() (return ATTSTATUS_INVALID_PARM otherwise)
 *
 * ATT_SERVER_SUPPORT_INDICATION shall be ENABLED
 * 
 * @param attribute  : the attribute which value should be sent.
 * @param destination: the connection handle to the device to which 
 *       this should be sent.
 * @param callback    : the callback that will receive the ATTEVT_SERVER_HVI_SENT
 *
 * @return The status of the operation:
 *	- ATTSTATUS_PENDING if the HVI could be placed in the send queue. An
 *     ATTEVT_SERVER_HVI_SENT once the packet could finally be sent or not.
 *  - ATTSTATUS_INVALID_PARM (only if ATT_PARMS_CHECK==1 ; asserted otherwise
 *     if ATT_DEBUG==1) if:
 *     - attribute cannot be found in the database, or
 *     - attribute can be found in the database but is not committed, or
 *     - channel is an invalid combination.
 *		(always asserted) 
 *	   - the database is not locked
 *  - ATTSTATUS_FAILED if the channel is closed at the level below, or if the
 *		packet can not be placed in the send queue.
 */
AttStatus ATT_SERVER_SendIndication(
            const AttAttribute   *attribute,
            const AttDevice       destination,
			AttServerCallback	  callback
          );
#endif //(ATT_SERVER_SUPPORT_INDICATION == 1)

#if (ATT_BROWSE_API == 1)

/**
 * Browse the database.
 *
 * This function SHALL be called while the database has been secured with
 * ATT_SERVER_SecureDatabaseAccess(). Once all returned fields have been read/
 * accessed, the database access may be released with a call to
 * ATT_SERVER_ReleaseDatabaseAccess())
 *
 * @param: reference: the reference reference (we want the one which follows
 *                    this one). NULL to get the first one.
 *                    Can be an AttServiceAttribute* casted in AttAttribute*.
 *                    Assert if 'reference' is not in the database (in case
 *                    'reference' is != NULL).
 *
 * @param: serviceAttribute [out]: the service which the returned attribute
 *                                 belongs to.
 *
 * @param: uuid [out]: the returned attribute's uuid (size and value).
 *
 * @param: valueLen [out]: the returned attribute's value length.
 *
 * @param: value [out]: the returned attribute's value. *

 * @param: permissions [out]: the returned attribute's permissions.
 *
 * @param: handle [out]: the returned attribute's handle.
 *
 * @return The attribute which follows 'attribute' in the database. NULL if
 *         'attribute' is the last attribute in the database.
 *         Can be an AttServiceAttribute* casted in AttAttribute*.
 *
 * The caller shall NOT DIRECTLY MODIFY the attribute's values which are
 * returned.
 */

AttAttribute *ATT_SERVER_GetNext(const AttAttribute  *  reference,
                                 AttServiceAttribute * *serviceAttribute,
                                 AttUuid               *uuid,
                                 U16                   *valueLen,
                                 U8                  * *value,
								 AttPermissions		   *permissions,
                                 AttHandle             *handle
                                 );

AttAttribute *ATT_SERVER_GetAttributeFromHandle( AttHandle       handle  );

/**
 * Will be called by the ATT when the ATT's own attributes changed (written by
 * a peer client for example).
 */
void ATT_SERVER_AttAttributesChanged(void);

#endif //(ATT_BROWSE_API == 1)


#endif //(ATT_ROLE_SERVER == 1)
#endif //(ATTRIBUTE_PROTOCOL == 1) 

#endif // __ATTSERVER_H__
