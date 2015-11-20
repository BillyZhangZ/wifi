#ifndef __BLERADIO_H
#define __BLERADIO_H
/****************************************************************************
 *
 * File:          BleRadio.h
 *
 * Description:   Contains interfaces for specific Radio initialisation
 *					routines
 * 
 * Created:       October, 2010
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.9 $
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

/****************************************************************************
 *	APPLICATION INTERFACE functions definition
 ****************************************************************************/

#if (BLECONTROLLER_NEED_SPECIFIC_INIT==1)

/* Start the RADIO specific initialisation
 *
 * BLERADIO_Init()
 *	This function is called  during the HCI layer initialisation 
 *  ( BLEMGMT_Init() )in order to run some specific radio initialisation
 * procedures.
 * Note that the RADIO initialisation shall send the HCC_RESET 
 * command (0x0C03), because the stack initialisation will not send it if 
 * BLECONTROLLER_NEED_SPECIFIC_INIT is enabled (1).
 *
 *  BLECONTROLLER_NEED_SPECIFIC_INIT shall be enabled.
 * 
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation have successfully
 *	 started.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed to start
 * it will result to a failure to the BLEMGMT_Init() function.
 *
 */
BleStatus BLERADIO_Init( void );

/* Handle all the HCI events recieved during the Radio Initialisation
 *
 * BLERADIO_HCIEventHandler()
 *	This function handle all the HCI events received by the STACK during
 *  the radio initialisation procedure, between BLERADIO_Init and
 * BLEHCI_RadioIsInitalised
 *
 *  BLECONTROLLER_NEED_SPECIFIC_INIT shall be enabled.
 *
 *  eventCode: the HCI event OpCode
 *  parmsLen: the HCI event Parameter Lengh
 *  parms: a pointer to the HCI event Parameter
 * 
 * return nothing
 *
 */
void BLERADIO_HCIEventHandler(	U8 eventCode, U8  parmsLen, U8 *parms);

/* Send a specific HCI command during Radio Initialisation
 *
 * BLEHCI_SendRadioCommand():
 *	Send a specific command. when an application want to send a HCI
 *	command during initialisation, it can use this interface.
 *
 * BLECONTROLLER_NEED_SPECIFIC_INIT shall be enabled
 *
 *  opCode	The opcode of the command to send
 *  parmsLen  The HCI command Parameter Lengh
 *  parms     The HCI command Parameter
 *
 *
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded
 *	- BLESTATUS_FAILED indicates that the operation has failed, this interface
 *  may have not been call during radio initialisation.
 *	
 */
BleStatus BLEHCI_SendRadioCommand(U16 opCode, U8 parmsLen, U8* parms) ;

/** Notify the STACK that the radio has been initialised
 *
 * BLEHCI_RadioIsInitialised()
 *	This function shall be call by the Application when the radio 
 *  initialisation is done. 
 *
 *  BLECONTROLLER_NEED_SPECIFIC_INIT shall be enabled.
 * 
 *  status :   The status of the RADIO initialisation,
 *           BLESTATUS_SUCCESS means that the radio is successfully
 *             initialised, the STACK will continue its initialisation
 *             process.
 *           BLESTATUS_FAILED means that the radio is not initialised.
 *             It will result the CORE stack callback to receive the 
 *             BLEEVENT_INITIALIZATION_RSP with the status BLESTATUS_FAILED.
 *
 * return nothing
 */
void BLEHCI_RadioIsInitialised( BleStatus status );

#endif //(BLECONTROLLER_NEED_SPECIFIC_INIT == 1)


#endif /*__BLERADIO_H*/
