#ifndef __ATTCLIENT_H__
#define __ATTCLIENT_H__
/****************************************************************************
 *
 * File:          attclient.h
 *
 * Description:   ATT API: all that is specific to the attribute client role.
 * 
 * Created:       August, 2008
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

/**
 * @file attclient.h
 *
 * ATT API: all that is specific to the attribute client role.
 *
 * @author xavier.boniface@alpwise.com
 * @author Alexandre GIMARD
 */

#include "ATT/AttTypes.h"

// Includes common ATT definitions. 
#include "ATT/AttBase.h"

#if (ATTRIBUTE_PROTOCOL == 1)

/**
 * @name APIcallback
 *
 *@{
 */


/**
 * @brief 'data' field of the ATT's API client callback for
 * 
 */
typedef struct
{
  AttHandle  handle;
  U8        *value;
  U16        valueLen;
} AttHandleValue;

/**
 * @brief 'data' field of the ATT's API client callback for
 * 
 */
typedef struct
{
  U8        *value;
  U16        valueLen;
} AttValue;

/**
 * @brief 'data' field of the ATT's API client callback for
 * 
 */
typedef struct
{
  U8        valueLen;
  U8        *rspData;
  U8		rspDataLen;
} AttReadByTypeRspData,AttReadByGroupRspData;


///**
// * @brief Parameters of the ATT callback.
// *
// * Appart from the ATTEVT_SEND_CNF event, all client events are indications or
// * responses from a peer attribute server.
// * The 'from' and 'channel' parameters tell which attribute server this is and
// * through which path the indication/response was received.
// *
// * The 'data' field is only relevant if status==ATTSTATUS_SUCCESS.
// * (this comment is especially related to ResumeSendProcess(). @todo See if we really never
// * need to fill data in case we receive a true << Error Response >> PDU.
// */
//typedef struct
//{
//        AttEvent  event;
//        AttStatus status;
//  const AttDevice from;     /**< Which device hosts the attribute server which sent this. */
//
//  /**
//   * Analysis: data needed for all events:
//   * M means mandatory
//   * O means optional i.e. could be filled with some relevant information but is not required
//   *   by the upper layer.
//   * - means not relevant
//   *
//   *                       | Fields                                         |
//   *      Event            |------------------------------------------------|
//   *                       | status |handle | value | valueLen | uuid | cnf |
//   * ----------------------|------------------------------------------------|
//   * Read Info Rsp         |    -   |   X   |   -   |     -    |   X  |  -  |
//   * Read Info Complete    |    X   |   -   |   -   |     -    |   -  |  -  |
//   * Read by type Rsp      |    -   |   X   |   X   |     X    |   -  |  -  |
//   * Read by type Complete |    X   |   -   |   -   |     -    |   O  |  -  |
//   * Read Rsp              |    X   |   O   |   X   |     X    |   -  |  -  |
//   * Write Rsp             |    X   |   O   |   -   |     -    |   -  |  -  |
//   * HVI                   |    X   |   X   |   X   |     X    |   -  |  X  |
//   */
//  union {
//    /* Specific data types for the different event types here. */
//    AttHandleValue       handleValue;
//    AttHandleUuid        handleUuid;
//  } data;
//
//} AttClientCallbackParms;
//
///**
// * Prototype of the callback to send events to the API (same format for server and client).
// */
//typedef void (*AttClientCallback)(AttClientCallbackParms *parms);


typedef void * AttClient;


typedef struct {
	
	AttDevice	remoteDeviceHandle;
	AttClient	clientAttHandle;

	AttOpCode	opcode;

	union {
		struct {
			AttHandle	startingHandle;
			AttHandle	endingHandle;
		} FindInformationReq;

		struct {
			AttHandle	startingHandle;
			AttHandle	endingHandle;
			AttUuid		*uuid;
		} ReadByTypeReq, ReadByGroupReq;

		struct {
			AttHandle	startingHandle;
			AttHandle	endingHandle;
			AttUuid		*uuid;
			U8			*value;  // a set of [2..(ATTMTU-1)/2] handles
			U8			valueLen;
		} FindbyTypeValueReq;

		struct {
			AttHandle	handle;
		} ReadReq;

		struct {
			AttHandle	*handles;
			U8			numHandles;  // a set of [2..(ATTMTU-1)/2] handles
		} ReadMultipleReq;

		struct {
			AttHandle	handle;
			U8			*value;  // a set of [2..(ATTMTU-1)/2] handles
			U16			valueLen;
		} WriteReq, WriteCmd;

		
		struct {
			AttHandle	handle;
			U8			*value;  // a set of [2..(ATTMTU-1)/2] handles
			U16			valueLen;
			U16			offset;
		} PrepareWriteReq;

		struct{
			AttHandle	handle;
			U16			offset;
		} ReadBlobReq;

		struct {
			AttFlags	flag;
		} ExecuteWriteReq;

	} Parms;

	//BleNode		node;
	void*		node[2];

} AttCommand;

/**
 * 'data' type container for the ATTOPCODE_SEND_CNF client core events
 */
typedef struct
{
  AttOpCode      opCode; /* The request that was sent or could not be sent. */

} AttSendCnfData;


/**
 * 'data' type container for the ATTOPCODE_ERROR_RSP client core events
 */
typedef struct
{
  AttOpCode    opCode; /* The request that generated this error response. */
  AttHandle    handle; /* The attribute handle that generated this error
                        * response. */
  AttPduStatus status; /* The reason why the request has generated an error
                        * response. */
} AttErrorRspData;


/**
 * 'data' type container for the ATTOPCODE_FIND_INFORMATION_RSP client core
 * events.
 */
typedef struct
{
  U16          numResults; /* The number of (handle, uuid) pairs returned in
                            * this PDU. */

  AttFormat	   resultFormat;   /* The size of the UUIDs reported in the pairs (see
                            * 'results')( All UUIDs reported in one PDU shall
                            * have the same size) or a single list of handles */

  U8          *results;    /* The "Information Data" field of the PDU i.e.
                            * <numResults> (handle, UUID) pairs, the one after
                            * the other.
                            * The 2 bytes handle is coded in little endian
                            * format, followed by the UUID value. The UUID
                            * value is also coded in little endian format, on
                            * 2 or 16 bytes depending on uuidSize.
                            */
} AttFindInfoRspData;


/**
 * 'data' type container for the ATTOPCODE_FIND_INFORMATION_RSP client core
 * events.
 */
typedef struct
{
	/** The number of (handle, handle) pairs returned in this answer. 
	 */
	U16          numResults; 


	/** The pairs of (handle, handle) returned in this answer. 
	 */
	U8          *results;   

}AttFindByTypeValueRspData;

/**
 * 'data' type container for the ATTOPCODE_READ_BLOB_RSP client core events
 */
typedef struct
{
  AttHandle  handle;
  U16        offset;
  U8        *value;
  U16        valueLen;

} AttHandleOffsetValue;


/**
 * @brief Parameters of the AttClientCoreCallback for client core --> client
 * auto/direct communication.
 *
 * Different possible events for a client:
 *
 * |----------------------|--------------------------------------------------|
 * |      event:          |                     fields                       |
 * |                      |--------------------------------------------------|
 * |   ATTOPCODE_...      | status | peer & channel   | data                 |
 * |----------------------|--------------------------------------------------|
 * |  ...SEND_CNF         |  PDU   | to which device  | AttOpCode            |
 * |                      |  sent? | the PDU was sent |                      |
 * |----------------------|--------------------------------------------------|
 * |  ...ERROR_RSP        |Received| from which       | AttErrorRspData      | 
 * |.FIND_INFORMATION_RSP |   PDU  | device the       | AttFindInfoRspData   | 
 * |   ...READ_RSP        |   was  | PDU was sent     | AttHandleValue       |
 * |   ...WRITE_RSP       |  valid |                  | AttHandle            |
 * |   ...READ_BLOB_RSP   |    ?   |                  | AttHandleOffsetValue |
 * |  ...WRITE_BLOB_RSP   |        |                  | AttHandle            |
 * |.READ_MODIFY_WRITE_RSP|        |                  | AttHandleValue       |
 * | ..PREPARE_WRITE_RSP  |        |                  | AttHandleValue       |
 * | ..EXECUTE_WRITE_RSP  |        |                  | ??todo               |
 * | ...HANDLE_VALUE_IND  |        |                  | AttHandleValue       |
 * |----------------------|--------------------------------------------------|
 *
 */
typedef struct
{
  AttOpCode  event;   /**< ATTOPCODE_SEND_CNF or an _IND, _REQ or _RSP,
                       *   or ATTOPCODE_TIMEOUT. */
  AttStatus  status;  /**< ATTSTATUS_SUCCESS or ATTSTATUS_FAILED: 
                       *    - for ATTOPCODE_SEND_CNF: whether the sending
                       *      succeeded or failed.
                       *    - otherwise: whether the received PDU was well
                       *      formatted or not.
                       *    - for ATTOPCODE_TIMEOUT: always success.      */
  AttDevice  peer;    /**< The attribute server to which we sent/which sent
                       *   the PDU. */
  AttCommand *command;

  union {
    /*                Data types for the event:                             */
	AttSendCnfData				sendCnfData;      /* ATTOPCODE_SEND_CNF            */
    AttErrorRspData				errorRspData;     /* ATTOPCODE_ERROR_RSP           */
    AttFindInfoRspData			findInfoRspData;  /* ATTOPCODE_FIND_INFORMATION_RSP*/
	AttFindByTypeValueRspData	findByTypeValueRspData;  /* ATTOPCODE_FIND_BY_TYPE_VALUE_RSP*/
    AttHandleValue				handleValue;		/* ATTOPCODE_READ_MODIFY_WRITE_RSP,
														* ATTOPCODE_PREPARE_WRITE_RSP
														* ATTOPCODE_HANDLE_VALUE_IND and
														* ATTOPCODE_RELIABLE_
														  HANDLE_VALUE_IND    */
    AttOpCode					opCode;		/* ATTOPCODE_TIMEOUT (the opcode of	the command which timed out). */
											/* ATTOPCODE_CONN_FAILURE (the opcode of the command which failed). */


	AttValue					readRspData;		/* ATTOPCODE_READ_RSP */
	AttReadByTypeRspData		readByTypeRspData;
	AttReadByGroupRspData		readByGroupRspData;

  } Parms;

} AttClientCoreCallbackParms;

/**
 * Prototype of the callback to send events.
 */
typedef void (*AttClientCoreCallback)(AttClientCoreCallbackParms *clientCallbackParms);


typedef struct {
	
	/** The callback on which the ATT events will be returned */
	AttClientCoreCallback callback;

	/** Internal use only*/
	void *internal[2];

} AttClientHandler;


/** register an attribute Client.
 *
 * ATT_CLIENT_Register():
 * @param handler	The handler describing the client to register
 *
 * @return :
 *	- A valid Attribute protocol client upon SUCCESS or 0 in case of failure
 *	
 * @author Alexandre GIMARD
 */
AttClient ATT_CLIENT_Register(AttClientHandler *handler);

/** Deregister an attribute Client.
 *
 * ATT_CLIENT_Deregister():
 * @param client: a registered Attrubute client
 *
 * @return :
 *	- Always BLESTATUS_SUCCESS
 *	
 * @author Alexandre GIMARD
 */
BleStatus ATT_CLIENT_Deregister(AttClient client);

/* Query for client registration.
 *
 * ATT_CLIENT_IsRegistered():
 * @param client: an Attrubute client
 *
 * @return :
 *	- 0 if the client is not registered
 *	- an other value otherwise
 *	
 * @author Alexandre GIMARD
 */
U8 ATT_CLIENT_IsRegistered(AttClient client);


/* Send an Attribute Protocol Command.
 *
 * ATT_CLIENT_SendCommand():
 * Used to send the specified command through the Attribute Protocol,
 * The given command fields indicates the remote device to which the command
 * is destinated, the local client responsible of this command,the command
 * opcode and the command parameters.
 *
 * @param command : the command to send, the remote device, the local client
 * and the command opcode fields are mandatory.
 * The command pointer shall be not null and available until the command has
 * completed ( succeed, failed or timeouted)
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation is in Progress, the event
 *	ATTOPCODE_SEND_CNF will be sent to the client callback once the command 
 *  have been sent.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function has failed because
 *		an invalid parameter.
 *
 * @author Alexandre GIMARD
 */
BleStatus ATT_CLIENT_SendCommand(AttCommand *command);


#endif //(ATTRIBUTE_PROTOCOL == 1) 

#endif // __ATTCLIENT_H__
