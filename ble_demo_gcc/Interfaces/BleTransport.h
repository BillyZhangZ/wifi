#ifndef __BLETRANSPORT_H
#define __BLETRANSPORT_H
/****************************************************************************
 *
 * File:          BleTransport.h
 *
 * Description:   Contains interfaces routines for TRANSPORT initialisation,
 *					de-initialisation and execution (read/write).
 * 
 * Created:       June, 2008
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.14 $
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



/***************************************************************************\
 * External Functions
\***************************************************************************/




#if (BLETRANSPORT_USED == BLETRANSPORT_UART)

/* Notify the Stack that some data have been received from the baseband
 * through the UART physical bus
 *
 * BLETRANSPORT_UartDataReceived():
 *	This function is used to notify the stack that data have been received
 *  by the transport from controller.
 *	When the function return the data buffer can be reused to read other data
 *
 *  uartData: Pointer to received data 
 *  uartDataLen: Length of the received data contained in the data pointer, 
 *				maximum 255 bytes can be notified at a time.
 *
 * return The status of the operation:
 *  - BLESTATUS_SUCCESS indicates that the operation is successful and
 *		that the data is well acquired at the stack level
 *	- BLESTATUS_FAILED indicates that the operation has failed, it could be 
 *		because the packet is bad-formatted.
 *  - BLESTATUS_NO_RESOURCES indicates that the operation has failed, because
 *		no internal buffers are available.
 */
BleStatus BLETRANSPORT_UartDataReceived(U8* uartData, U8 uartDataLen);


/* Initialise the UART transport layer.
 *
 * BLEUART_Init()
 *	This function is called by the BLE stack (TRANSPORT layer) to initialise
 *	the UART layer.
 *  The user should uses this function to open and setup parameters for the
 *	UART line, eventually create Read Thread or setup RX and TX interrupts.
 *
 *	When this function succeeds, the UART layer shall be fully functional
 *
 *	This function is called internally during the BLEMGMT_Init() process, failure here 
 *	will issue a failure in BLEMGMT_Init()
 *
 * see BLEMGMT_Init()
 *
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates to the BLE stack that the UART has been
 *		successfully initialized
 *	- BLESTATUS_FAILED indicates to the BLE stack that the UART could not be
 *		initialized
 */
BleStatus BLEUART_Init(void);


/* De-Initialise the UART transport layer.
 *
 * BLEUART_Deinit()
 *	This function is called by the BLE stack (TRANSPORT layer) to de-initialise
 *	the UART layer. Eventual thread shall be terminated here.
 *
 *	When this function succeeds, the UART layer shall be fully de-initialised
 *
 *	This function is called internally during the BLEMGMT_Deinit() process, failure here 
 *	will issue a failure in BLEMGMT_Deinit()
 *
 * see BLEMGMT_Deinit()
 *
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates to the BLE stack that the UART has been
 *		successfully de-initialized
 *	- BLESTATUS_FAILED indicates to the BLE stack that the UART could not be
 *		de-initialized
 */
BleStatus BLEUART_Deinit(void);


/* Send data through the UART transport layer.
 *
 * BLEUART_Send()
 *	This function is called by the BLE stack (TRANSPORT layer) to send data
 *	through the UART layer. 
 *	When the function succeeds, the UART layer shall have sent all the data
 *	It is to the UART layer to retry to send the data, if the function fails
 *  it result to a transport error and stack de-initialisation.
 *  Note that BLEUART_Send() is always called after BLEUART_TXStarts() and
 *  before BLEUART_TXEnds().
 *
 *  data: pointer to the buffer of data to send
 *  dataLen: length  of the buffer of data contained in the data pointer
 *
 *
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates to the BLE stack that the data have been
 *		successfully sent
 *	- BLESTATUS_FAILED indicates to the BLE stack that the data have not 
 *		 been sent, it result to a transport error
 */
BleStatus BLEUART_Send(U8* uartData , U8 uartDataLen);

/* Give information to the UART layer that the stack will starts a transmit
 * process.
 *
 * BLEUART_TXStarts()
 *	This function is called by the BLE stack (TRANSPORT layer) to inform
 *	the UART layer that a packet will be transmitted. 
 *	As a packet can be transmitted into several BLEUART_Send() calls, the 
 *  stack inform the UART layer that a transmission starts. Then the stack
 *  will call one or several BLEUART_Send() and then  BLEUART_TXEnds API. It
 *  is useful for UART line that are not full duplex or with a specific flow
 *  control and requires knowing when transmission starts.
 *  Otherwise, the function should be empty.
 *  Note that BLEUART_Send is always called after BLEUART_TXStarts() and 
 *  before BLEUART_TXEnds().
 * 
 * return none
 */
void BLEUART_TXStarts( void );

/* Give information to the UART layer that the stack has ends a transmit
 * process.
 *
 * BLEUART_TXEnds()
 *	This function is called by the BLE stack (TRANSPORT layer) to inform
 *	the UART layer that a packet has been transmitted. 
 *	As a packet can be transmitted into several BLEUART_Send() calls, the 
 *  stack  inform the UART layer that a transmission ends. 
 *  it is usefull for UART line that are not full duplex or with a specific
 *  flow control and requires knowing when transmission ends.
 * 	Otherwise the function should be empty.
 *  Note that BLEUART_Send is always called after BLEUART_TXStarts() and
 *  before BLEUART_TXEnds().

 * 
 * return none
 */
void BLEUART_TXEnds( void );

#endif //(BLETRANSPORT_USED == BLETRANSPORT_UART)

#if (BLETRANSPORT_USED == BLETRANSPORT_CUSTOM)

/* Initialise the Custom transport layer.
 *
 * CUSTOMTRAN_Init()
 *	This function is called by the BLE stack (TRANSPORT layer) to initialise
 *	the CUSTOM TRANSPORT layer.
 *  The user should uses this function to open and setup parameters for the
 *	Transport BUS, eventually create Read Thread or setup RX and TX interrupts.
 *
 *	When this function succeeds, the transport layer shall be fully functional.
 *
 *	This function is called during the BLEMGMT_Init() process, failure here 
 *	will issue a failure in BLEMGMT_Init()
 *
 * see BLEMGMT_Init()
 *
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates to the BLE stack that the custom transport 
 *		layer has been successfully initialized
 *	- BLESTATUS_FAILED indicates to the BLE stack that the custom transport 
 *		layer could not be initialized
 */
BleStatus CUSTOMTRAN_Init(void);


/* De-Initialise the Custom transport layer.
 *
 * CUSTOMTRAN_Deinit()
 *	This function is called by the BLE stack (TRANSPORT layer) to de-initialise
 *	the CUSTOM TRANSPORT layer.
 *
 *  The user should use this function to close and de-init the custom Transport BUS
 *  and eventually terminate Read Thread.
 *
 *	When this function succeeds, the transport layer may be shutdown.
 *
 *	This function is called during the BLEMGMT_Deinit() process, failure here 
 *	will issue a failure in BLEMGMT_Deinit()
 *
 * see BLEMGMT_Init()
 *
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates to the BLE stack that the custom transport 
 *		has been successfully de-initialized
 *	- BLESTATUS_FAILED indicates to the BLE stack that the custom transport 
 *		could not be de-initialized
 */
BleStatus CUSTOMTRAN_Deinit(void);

/* Send some BLE-HCI data Packet to the baseband.
 *
 * BleTransport_SendDataPacket():
 *	The BLE stack (TRANSPORT layer) calls this function to send a BLE-HCI data
 *	packet to the controller through the custom transport layer.
 *
 *	When the function succeeds, the custom transport layer shall have sent all
 *	the data.
 *
 *	It is to the custom transport layer to retry to send the data, if the 
 *	function fails it result to a transport error and stack de-initialisation.
 *
 *
 *  data: the pointer to to data packet to send
 *	dataLen : length of the buffer of data contained in the data pointer
 *
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates to the BLE stack that the BLE-HCI data
 *		packet has been successfully sent
 *	- BLESTATUS_FAILED indicates to the BLE stack that the BLE-HCI data
 *		packet has not been sent, it result to a transport error.
 */
BleStatus CUSTOMTRAN_SendDataPacket(U8* dataPacket, U16 dataPacketLen);


/* Send some BLE-HCI Command data Packet to the baseband
 *
 * BleTransport_SendCommandPacket():
 *	The BLE stack (TRANSPORT layer) calls this function to send a BLE-HCI
 *	command packet to the controller through the custom transport layer.
 *
 *	When the function succeeds, the custom transport layer shall have sent all
 *	the data.
 *
 *	It is to the custom transport layer to retry to send the data, if the 
 *	function fails it result to a transport error and stack de-initialisation.
 *
 *
 *  data: the pointer to to data packet to send
 *	dataLen : length of the buffer of data contained in the data pointer
 *
 *  bleHciCommandPacket the command packet to send
 *
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates to the BLE stack that the BLE-HCI command
 *		packet has been successfully sent
 *	- BLESTATUS_FAILED indicates to the BLE stack that the BLE-HCI command
 *		packet has not been sent, it result to a transport error.
 */
BleStatus CUSTOMTRAN_SendCommandPacket(U8* commandPacket, U8 commandPacketlen);


/* Notify the Stack that a complete HCI Event frame have been received from  
 * the baseband through the TRANSPORT physical bus.
 *
 * BLETRANSPORT_EventReceived():
 *	This function is used to notify the stack that a complete HCI event 
 *	frame have been received by the transport from controller.
 *	When the function return the data buffer can be reused to read other data
 *
 *  data: Pointer to the HCI event frame received. 
 *  dataLen: Length  of the received data contained in the data pointer, 
 *			a maximum of 255 bytes can be notified at a time.
 *
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation is successful and
 *		that the data are well acquired at the stack level
 *	- BLESTATUS_FAILED indicates that the operation has failed, because
 *		the packet is bad-formatted or no internal buffer are available.
 */
BleStatus BLETRANSPORT_EventReceived(U8* eventPacket, U8 eventPacketLen);

/* Notify the Stack that a complete HCI Data frame have been received from  
 * the baseband through the TRANSPORT physical bus.
 *
 * BLETRANSPORT_DataReceived():
 *	This function is used to notify the stack that a complete HCI data frame
 *	frame have been received by the transport from controller.
 *	When the function return the data buffer can be reused to read other data
 *
 *  data: Pointer to the HCI data frame received
 *  dataLen: Length  of the received data contained in the data pointer, 
 *			
 *
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation is successful and
 *		that the data are well acquired at the stack level
 *	- BLESTATUS_FAILED indicates that the operation has failed, because
 *		the packet is bad-formatted or no internal buffer are available.
 */
BleStatus BLETRANSPORT_DataReceived(U8* dataPacket, U16 dataPacketLen);

#endif //(BLETRANSPORT_USED == BLETRANSPORT_CUSTOM)

#endif /*__BLETRANSPORT_H*/
