/****************************************************************************
 *
 * File:          BleRadio.c
 *
 * Description:   Contains interfaces for specific Radio initialisation
 *					routines
 * 
 * Created:       October, 2010
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.1 $
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
#include "BleRadio.h"

#include "BleHci.h"


#if (BLECONTROLLER_NEED_SPECIFIC_INIT == 1)

/**
 * @file K22_EM9301_Radio.c
 *
 * 
 */

/***************************************************************************
 *	constant definition
 ***************************************************************************/
#if (BLE_EM9301_LOADPATCH == 1)
// Parameter for the HCI commands sent during initialization
U8 	hciCommandParms[50];

// Number of ROW [0-63]
#define NUMBER_OF_ROWS			64

// Number of Bytes Per ROW
#define ROW_SIZE				48

// Number of Sectors [0-17]
#define NUMBER_OF_SECTORS		2

signed char currentRow;
signed char currentSector;

unsigned char patchLoadingState;

#define PATCH_LOAD_STATE_NOTSTARTED             0x00u
#define PATCH_LOAD_STATE_ENTERINGISPMODE        0x01u
//#define PATCH_LOAD_STATE_LOADING                0x02u
#define PATCH_LOAD_STATE_EXITINGISPMODE         0x04u
//#define PATCH_LOAD_STATE_DONE                   0x08u

#else //(BLE_EM9301_LOADPATCH == 1)
// Parameter for the HCI commands sent during initialization
// if the patch loading is disabled save some memory because needed buffer
// is smaller
U8 	hciCommandParms[6];
#endif //(BLE_EM9301_LOADPATCH == 1)

/** em9301PublicAddress 
 * The Public address is by default the EM-Microelectronics Public address
 * If an application need to use public address(es), the addresses must be 
 * issued by the IEEE 802-2001. Several devices cannot share and use the
 * same public address. 
 * This address is set during the stack initialisation, so it can be
 * overwrittren by using BleRadio_SetPublicAddress(BD_ADDR *addr);
 * before the stack initialisation.
 */
BD_ADDR em9301PublicAddress = { 0x0C, 0xF3, 0xEE, 0x00, 0x00, 0x00}; 

/***************************************************************************\
 *	Local functions Prototype
\***************************************************************************/

#if (BLE_EM9301_LOADPATCH == 1)
static void SendCommand(void);
#endif //(BLE_EM9301_LOADPATCH == 1)

/****************************************************************************
 *	APPLICATION INTERFACE functions declaration
 ****************************************************************************/

/** Start the RADIO specific initialisation
 *
 * BLERADIO_Init()
 *	This function is called  during the HCI layer initialisation 
 *  ( BLEMGMT_Init() )in order to run some specific radio initialisation
 * procedures.
 *
 *  BLECONTROLLER_NEED_SPECIFIC_INIT shall be enabled.
 * 
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation have successfully
 *	 started.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed to start
 * it will result to a failure to the BLEMGMT_Init() function.
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLERADIO_Init(){

#if (BLE_EM9301_LOADPATCH == 1)
    // If enabled, init the patch Loading state machine
	// start by uploading the last sector and the last row
	currentRow = NUMBER_OF_ROWS - 1;
	currentSector = NUMBER_OF_SECTORS - 1;
    
    patchLoadingState = PATCH_LOAD_STATE_NOTSTARTED;
#endif //(BLE_EM9301_LOADPATCH == 1)

	// it is the responsibility of this layer to check
	// that we have (need answer) to radio command if not
	// radio initialization failed
    
	// send the HCI RESET command, it does not takes any parameters
	_time_delay(100);
	return BLEHCI_SendRadioCommand(0x0C03/*BLEHCI_CMD_RESET*/, 0x00 , (U8 *) 0);
}

/* Handle all the HCI events received during the Radio Initialization
 *
 * BLERADIO_HCIEventHandler()
 *	This function handle all the HCI events received by the STACK during
 *  the radio initialization procedure, between BLERADIO_Init and
 * BLEHCI_RadioIsInitalised
 *
 *  BLECONTROLLER_NEED_SPECIFIC_INIT shall be enabled.
 *
 * @param  eventCode: the HCI event OpCode
 * @param  parmsLen: the HCI event Parameter Lengh
 * @param  parms: a pointer to the HCI event Parameter
 * 
 * @return nothing
 *
 * @author Alexandre GIMARD
 */
void BLERADIO_HCIEventHandler(	U8 eventCode, U8  parmsLen, U8 *parms ){
	U16 opcode;

	if( ( (eventCode == 0x0E) || (eventCode == 0x10) ) ){
		// In case of an answer to the HCI_RESET event 
		// (HARDWARE ERROR event or COMMAND COMPLETE EVENT)
		// Cancel the timer
	}

	UNUSED_PARAMETER( parmsLen );
	switch (eventCode){
		case 0x0E:
			opcode = LE_TO_U16(parms + 1);
			if( opcode == 0x0C03){ //RESET
				// means reset has been acknolgedged

#if (BLE_EM9301_LOADPATCH == 1)                
                // depending of the patch loading state
                if( patchLoadingState == PATCH_LOAD_STATE_EXITINGISPMODE ){
                    //we are exited from ISP mode, it is time to continue the init
                    //patchLoadingState = PATCH_LOAD_STATE_DONE
#endif //(BLE_EM9301_LOADPATCH == 1)                                             
                    // Just for the demonstration purpose
                    // and illustrate ho wto change the EM9301 Public 
                    // Bluetooth device address
                    // change the local bluetooth device address there, 
                    // it is useless because it change the address to the default
                    // one. but it case the application needs to  set another IEEE
                    // registered address the place to do it, is there.
                   
                    // to 0C:F3:EE:00:00:00.
                    hciCommandParms[5] = em9301PublicAddress.addr[0];
                    hciCommandParms[4] = em9301PublicAddress.addr[1];
                    hciCommandParms[3] = em9301PublicAddress.addr[2];
                    hciCommandParms[2] = em9301PublicAddress.addr[3];
                    hciCommandParms[1] = em9301PublicAddress.addr[4];
                    hciCommandParms[0] = em9301PublicAddress.addr[5];

                    if( BLEHCI_SendRadioCommand(0xFC02, 6, (U8 *)&hciCommandParms[0]) != BLESTATUS_SUCCESS ){
                       BLEHCI_RadioIsInitialised( BLESTATUS_FAILED );
                    }
#if (BLE_EM9301_LOADPATCH == 1)                    
				} else if( patchLoadingState == PATCH_LOAD_STATE_NOTSTARTED ){
                    //we have not yet loading the patch, start the 
                    // loading procedure now
                    // try to enter in ISP mode by sending  HCI command 
                    // HCI_EM_Write_Data with parameters (address=0x1FFE, data=0x0000).
                    patchLoadingState = PATCH_LOAD_STATE_ENTERINGISPMODE;
                    
                    hciCommandParms[0] = 0xFE;
                    hciCommandParms[1] = 0x1F;
                    hciCommandParms[2] = 0x00;
                    hciCommandParms[3] = 0x00;                    

                    if( BLEHCI_SendRadioCommand(0xFC00, 4, (U8 *)&hciCommandParms[0]) != BLESTATUS_SUCCESS ){
                       BLEHCI_RadioIsInitialised( BLESTATUS_FAILED );
                    }                    
                } else {
                }
#endif //(BLE_EM9301_LOADPATCH == 1)     
			} 
#if (BLE_EM9301_LOADPATCH == 1)             
            else if( opcode == 0xFC00){ // EM_WRITE_DATA
                // To complete the ISP mode entering send a RESET command
                // expected answer => hardware error event reason 80
                // To complete the ISP mode exiting send a RESET command
                // expected answer => reset command complete
                BLEHCI_SendRadioCommand(0x0C03/*BLEHCI_CMD_RESET*/, 0x00 , (U8 *) 0);
                
            } else if( opcode == 0xFC80){ // EM_WRITE_PROGRAM
				SendCommand();
                
}
#endif //(BLE_EM9301_LOADPATCH == 1)             
            else if( opcode == 0xFC02){ // EM_CHANGE_BD_ADDRESS
				// Reset has been sent successfully, now 
				// activate/enable the Transition to BLE Sleep mode 
				hciCommandParms[0] = 0x01; //0x00 is disable
				if(  BLEHCI_SendRadioCommand(0xFC06, 1, (U8*) &hciCommandParms[0]) != BLESTATUS_SUCCESS ){
				   	BLEHCI_RadioIsInitialised( BLESTATUS_FAILED );
				}
			} else if (opcode == 0xFC06) { // EM_POWER_MODE
				// means transition to BLE SLEEP mode is activated
                // this application does not do everything elese,
                // however some other RADIO specific operation can be done there
                // like change the TX power level, ...
				BLEHCI_RadioIsInitialised( BLESTATUS_SUCCESS);
			}
		break;

		case 0x10:
		{
			//reason = *parms;
#if (BLE_EM9301_LOADPATCH == 1)             
			if( *parms == 0x80 ){
				// we are now in ISP mode
				// Patch can be uploaded
                //patchLoadingState = PATCH_LOAD_STATE_LOADING
				SendCommand();
			} else {
			 	// Another failure
				BLEHCI_RadioIsInitialised( BLESTATUS_FAILED );
			}
#else //(BLE_EM9301_LOADPATCH == 1)  
            //failure during initialization
            BLEHCI_RadioIsInitialised( BLESTATUS_FAILED );            
#endif //(BLE_EM9301_LOADPATCH == 1)             
            
		}
		break;
	}
}

/****************************************************************************
 *	External functions declaration
 ****************************************************************************/
/** Set the local controller public address
 *
 * BleRadio_SetPublicAddress()
 *	This function is called externaly, before the stack initialisation, in 
 * order to set the value of the public address that the stack set during 
 * the radio initialisation.
 *
 *  BLECONTROLLER_NEED_SPECIFIC_INIT shall be enabled.
 * 
 * @return None
 *
 * @author Alexandre GIMARD
 */
void BleRadio_SetPublicAddress(BD_ADDR *addr){

    SYSTEM_MemCpy(&em9301PublicAddress, addr, 6); 
}

/****************************************************************************
 *	Internal functions declaration
 ****************************************************************************/
#if (BLE_EM9301_LOADPATCH == 1)
static void SendCommand(){

	if( currentSector < 0){
		// FINISH!!
		
#if (BLE_PRINT_DEBUG_TRACES == 1)
		SYSTEM_Log("\r\n");
#endif //(BLE_PRINT_DEBUG_TRACES == 1)

        // We have now finsh to load the patch 
        // it is now time to exiting ISP mode
       
		// try to exit in ISP mode by sending HCI command 
        // HCI_EM_Write_Data with parameters (address=0x1FFE, data=0x55AA).
        patchLoadingState = PATCH_LOAD_STATE_EXITINGISPMODE;
                    
        hciCommandParms[0] = 0xFE;
        hciCommandParms[1] = 0x1F;
        hciCommandParms[2] = 0xAA;
        hciCommandParms[3] = 0x55;                    
                  
        if( BLEHCI_SendRadioCommand(0xFC00, 4, (U8 *)&hciCommandParms[0]) != BLESTATUS_SUCCESS ){
            BLEHCI_RadioIsInitialised( BLESTATUS_FAILED );
        }                    	
	} else {

		hciCommandParms[0] =  currentRow;
		hciCommandParms[1] =  currentSector;

		SYSTEM_MemCpy( hciCommandParms + 2,  &EMPatchArray[(ROW_SIZE * currentRow) + ( NUMBER_OF_ROWS * ROW_SIZE * currentSector )], ROW_SIZE );

		//Once Prepared it will be sent
 		if(  BLEHCI_SendRadioCommand(0xFC80, 50, (U8*) &hciCommandParms[0]) == BLESTATUS_SUCCESS){
			if( currentRow == 0 ) {
				currentRow = NUMBER_OF_ROWS - 1;
#if (BLE_PRINT_DEBUG_TRACES == 1)
				SYSTEM_Log("Sector %d done\r\n", currentSector);
#endif //(BLE_PRINT_DEBUG_TRACES == 1)
				currentSector --;
			} else {
				currentRow--;
			}
		}
	}
}
#endif //(BLE_EM9301_LOADPATCH == 1)

#endif //(BLECONTROLLER_NEED_SPECIFIC_INIT == 1)

