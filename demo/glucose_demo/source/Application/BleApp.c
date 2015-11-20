 /****************************************************************************
 *
 * File:          BleApp.c
 *
 * Description:   Contains main Low energy Application.
 * 
 * Created:       June, 2013
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.0 $
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
#include "BleApp.h"

// Low energy core stack inclusion
#include "BleTypes.h"
#include "BleSystem.h"
#include "BleEngine.h"
#include "BleGap.h"

#if (BLE_SECURITY == 1)
#include "BleSm.h"
#endif //(BLE_SECURITY == 1)

#if (BLE_ENABLE_VENDOR_SPECIFIC == 1)
#include "BleHci.h"
#endif //(BLE_ENABLE_VENDOR_SPECIFIC == 1)

#include <mqx.h>
#include <bsp.h>
#include <mutex.h>

#include "BleGlucose_Sensor.h"


//extern void BLESMP_ConfirmPasskey(U16 connHandle,U8 Passkey[6]);
/***************************************************************************\
 * MACROS and CONSTANTS definition
\***************************************************************************/
#define APPREQUEST_NOREQUEST            0x0000
#define APPREQUEST_NOTIFYTORUN          0x0001
#define APPREQUEST_INITPROFILES         0x0002
#define APPREQUEST_STARTDISCOVERABLE    0x0004
#define APPREQUEST_REGISTERGAPDEVICE    0x0008
#define APPREQUEST_PAIRING              0x0010
#define APPREQUEST_LINKUP               0x0020
#define APPREQUEST_DISCONNECT           0x0040

#define APPREQUEST_EM9301SLEEP          0x0080
#define APPREQUEST_EM9301IDLE           0x0100
#define APPREQUEST_RANDOM_ADDRESS       0x0200
//Example
#define APPREQUEST_SETVALUE             0x0400

#define APPSTATE_IDLE                   0x0000
#define APPSTATE_INITIALIZED            0x0001
#define APPSTATE_DISCOVERABLE           0x0002
#define	APPSTATE_CONNECTED              0x0004
#define	APPSTATE_LINKED                 0x0008
#define APPSTATE_BONDED                 0x0010
#define APPSTATE_SLEEP                  0x0020

/***************************************************************************\
 * ROM data
\***************************************************************************/


/***************************************************************************\
 * RAM data
\***************************************************************************/
// The current appRequest
AppRequest  appRequest;

// The current appState
AppState    appState;

// The handler for stack callback / events
BleHandler  handler;

// The Connection Handle with the server
U16         currentConnHandle;

// The remote device ADDRESS:
BD_ADDR     remoteAddr;

// Indicate if a pairing is in progress
U8          pairingInProgress;

// Device Name
const U8 bleLocalDeviceName[] = "Glucose_lu";

#if (BLEGLUCOSE_SUPPORT_SENSOR == 1)

#define MAX_NUMBER_OF_RECORDS                                    100
//Define glucose measurement records table
BleGlucoseServiceMeasurement glucoseMeasureSet;
BleGlucoseServiceMeasurement measurementRecords[MAX_NUMBER_OF_RECORDS];
#endif //(BLEGLUCOSE_SUPPORT_SENSOR == 1)

#if (BLE_SUPPORT_DEVICEINFORMATION_SERVICE == 1)
//Change device information as you want
const U8 bleDeviceInfoManufacturerName[] = "ALPWISE";
const U8 bleDeviceInfoModelNumber[] = "1.0";
#endif //(BLE_SUPPORT_DEVICEINFORMATION_SERVICE == 1)


/***************************************************************************\
 *	Local functions Prototype
\***************************************************************************/
static void BleApp_CoreCallback(BleEvent event, BleStatus status, void* param);
static void BleApp_GapCallback(BleGapEvent event, BleStatus status, void *parms);


BleStatus BLEAPP_Init(void); 
AppRequest BLEAPP_Scheduling(void); 
BleStatus BLEAPP_ReadyToGoToSleep( void );
void AppDisconnect(void);
void BLEMGMT_NotifyToRun(void);


void setGlucoseValue(void);
/***************************************************************************\
 * External Functions
\***************************************************************************/
//extern void LED_SetState(LedId led, unsigned char state);

#if (BLE_RANDOM_ADDRESSING_SUPPORT == 0)
extern void BleRadio_SetPublicAddress(BD_ADDR *addr);
#endif //(BLE_RANDOM_ADDRESSING_SUPPORT == 0)

/***************************************************************************\
 *	Local functions definition
\***************************************************************************/
/***************************************************************************\
 * External variables
\***************************************************************************/
extern MUTEX_STRUCT stack_mutex, database_mutex;
extern MUTEX_ATTR_STRUCT stack_mutexattr, database_mutexattr;

/***************** Profile callbacks definition ****************************/


/****************** Stack callbacks definition *****************************/
static void BleApp_CoreCallback(BleEvent event, BleStatus status, void* param){
	  
	switch (event){
		case BLEEVENT_INITIALIZATION_RSP:
			{
				if(status != BLESTATUS_SUCCESS){
					// Failed to initialize completely the bluetooth low energy system
					// Init the appState machine
					appState 	= APPSTATE_IDLE;
					appRequest 	= APPREQUEST_NOREQUEST;
					
				} else{
					// Initialization is successful
					appState |= APPSTATE_INITIALIZED;
					
					// Depends if random addressing is supported, if yes then
					// Set a random address otherwise go ahead
#if (BLE_RANDOM_ADDRESSING_SUPPORT == 0)                
					appRequest 	|= APPREQUEST_REGISTERGAPDEVICE;
#else //(BLE_RANDOM_ADDRESSING_SUPPORT == 1)      
					appRequest 	|= APPREQUEST_RANDOM_ADDRESS;
#endif //(BLE_RANDOM_ADDRESSING_SUPPORT == 1)
				}
			}
			break;

#if (BLE_RANDOM_ADDRESSING_SUPPORT == 1)
		case BLEEVENT_GENERATE_RANDOM_ADDRESS_RSP:
			{
				// nice, go ahead
				appRequest 	|= APPREQUEST_REGISTERGAPDEVICE;
			}
			break;
#endif //(BLE_RANDOM_ADDRESSING_SUPPORT == 1)

		/**
		 * Vendor Specific event
		 */
        case BLEEVENT_VENDOR_COMMAND_COMPLETE:
			{
				BleHciVendorCommandComplete *event = (BleHciVendorCommandComplete*) param;
				if( (event->opcode == 0xFC03) && (*(event->parms) == 0x00) ){
					// OK Fine, the EM9301 is now in SLEEP mode
					// This application now wants to be in discoverable mode
					// so exit EM9301 from SLEEP mode.
                    appState |= APPSTATE_SLEEP;
					appRequest |= APPREQUEST_EM9301IDLE;
				}
			}
			break;

		case BLEEVENT_VENDOR_COMMAND_STATUS:
            {
                BleHciVendorCommandStatus *commandStatusEvent = (BleHciVendorCommandStatus*) param;
                if( commandStatusEvent->opcode == 0xFC03){
                    if(commandStatusEvent->status == 0x00){
                        // OK Fine, the EM9301 has accepted to go back in IDLE mode.
                        // Wait for the vendor event to be sure that the transistion is complete.
                    } 
                }
            }
            break;

		case BLEEVENT_VENDOR_EVENT:
			{
				BleHciVendorEvent *event = (BleHciVendorEvent*) param;
				if( (event->parmsLen > 0) && (event->parms[0] == 0x01) ){
					// OK Fine, the EM9301 is back to IDLE mode
                    appState &= ~APPSTATE_SLEEP;
                    // Start to be discoverable
					appRequest 	|= APPREQUEST_STARTDISCOVERABLE;
				}
			}
			break;

		/**
		 * Security events
		 */
#if (BLE_SECURITY == 1)
		case BLEEVENT_PAIRING_REQUEST:
            {
                BlePairingInfo *info = ((BlePairingInfo *) param);
                
                // Accept the pairing and accept bonding
                BLESMP_ConfirmPairing(info->connHandle, 1, 1);
                pairingInProgress = 1;
            }
            break;
	
		case BLEEVENT_PASSKEY_REQUEST:
			// not suitable in our implementation : SM_IO_CAPABILITIES=SM_IO_NOINPUTNOOUTPUT
			{
				BlePairingInfo *info = ((BlePairingInfo *) param);
				BLESMP_RejectPasskey(info->connHandle);
				BLESMP_ConfirmPasskey(info->connHandle,/*"123456*/"000000");
			}

			break;

		case BLEEVENT_PASSKEY_DISPLAY:
			// not suitable in our implementation : SM_IO_CAPABILITIES=SM_IO_NOINPUTNOOUTPUT
			{
				BlePairingInfo *info = ((BlePairingInfo *) param);
				SYSTEM_Log("Passkey to pair: %c%c%c",
						info->sm.passkeyDisplay[0],
						info->sm.passkeyDisplay[1],
						info->sm.passkeyDisplay[2]);
				SYSTEM_Log("%c%c%c\n",
						info->sm.passkeyDisplay[3],
						info->sm.passkeyDisplay[4],
						info->sm.passkeyDisplay[5]);
				BLESMP_ConfirmPasskey(info->connHandle,&(info->sm.passkeyDisplay[0]));
			}
			break;

		case BLEEVENT_PAIRING_COMPLETE:
            {
                BlePairingInfo *info = ((BlePairingInfo *) param);
                if(status == BLESTATUS_SUCCESS){
                    // PAIRING SUCCEED: go ahead, start to link with the remote server
                    BleLinkSecurityProperties securityProperties;

                    pairingInProgress = 0;
                    // Retrieve the link security state ( Authenticated and/or encrypted and/or bonded)
                    BLESMP_GetLinkSecurityProperties(info->connHandle, &securityProperties);
                    // Print the result
                    if( (securityProperties & CONNSTATE_BONDED) != 0x00 ){
                        appState 	|= APPSTATE_BONDED;
                    }



                } else {
                    //Look at failure reason
                    if(info->sm.reason == SMERROR_PAIRINGNOTSUPPORTED){
                        //The pairing failed because the remote device doesn't support pairing
                        //TODO: Put here the action to do in this case
                    }

                    // Pairing failed, disconnect the link
                    appRequest 	|= APPREQUEST_DISCONNECT;
                  
                    // The last attempts we does not match to 
                    // authenticated with the remote device.
                    // If the device is bonded ,unbond.
                    BLEGAP_GetBDAddr (currentConnHandle, &remoteAddr);
                    BLESMP_UnBond( &remoteAddr );
                }
            }
            break;
                
#endif //(BLE_SECURITY == 1)
		default:
            break;
	}
}

static void  BleApp_GapCallback(BleGapEvent event, BleStatus status, void *parms){

	UNUSED_PARAMETER(status);
	
	switch(event){
 
	case BLEGAP_EVENT_CONNECTED:
        {
            // Link is now established
            BleGapConnectionInformation *connInfo = (BleGapConnectionInformation *) parms;
//            // Ligth on the LED1 on connection
//            LED_SetState(LED1,1);
            // Init the application state
            appState |= APPSTATE_CONNECTED;
            // Start a pairing procedure
            appRequest |= APPREQUEST_PAIRING;
            // Save the Connection Handle
            currentConnHandle = connInfo->connHandle;

            // change IO capabilities here?
//            if (BLESTATUS_SUCCESS != BLESMP_SetIOCapability(SM_IO_NOINPUTNOOUTPUT))
//            {
//
//            	return;
//            }

        }
        break;

	case BLEGAP_EVENT_DISCONNECTED:
		{
			// Device are now disconnected
			appState 	&= ~APPSTATE_CONNECTED;
			// Go ahead : be discoverable
			appRequest 	|= APPREQUEST_STARTDISCOVERABLE;
//			//Light off LED1 on deconnection
//			LED_SetState(LED1,0);
		}
		break;

	case BLEGAP_EVENT_MODECHANGE:
		{
			 U8 newMode = BLEGAP_GetMode();
				
			if( ((newMode & BLEMODE_DISCOVERABLE_MASK) == BLEMODE_DISCOVERABLE ) &&
				((newMode & BLEMODE_CONNECTABLE_MASK) == BLEMODE_CONNECTABLE  ) ){
					appState |= APPSTATE_DISCOVERABLE;
			}
			else {
				appState &= ~APPSTATE_DISCOVERABLE;
			}
		}
		break;
	}
}

void  AppDisconnect(void){
    if((appState  &  APPSTATE_CONNECTED) !=0){ 
        appRequest |= APPREQUEST_DISCONNECT;
    }
}

/***************************************************************************\
 *	APPLICATION functions definition
 ***************************************************************************/
BleStatus BLEAPP_Init(void){
    BleStatus status;
#if (BLE_RANDOM_ADDRESSING_SUPPORT == 0)
    BD_ADDR localAddr = {0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};

    //Replace public address by user given one
    BleRadio_SetPublicAddress(&localAddr);
#endif //(BLE_RANDOM_ADDRESSING_SUPPORT == 0)

	//Init the mutex first 
	_mutatr_init(&stack_mutexattr);
    _mutex_init(&stack_mutex, &stack_mutexattr);
	_mutatr_init(&database_mutexattr);
	_mutex_init(&database_mutex, &database_mutexattr);
	// Init the appState machine
    appState 	= APPSTATE_IDLE;
	appRequest 	= APPREQUEST_NOREQUEST;

    pairingInProgress = 0;

//    // Ligth off the LED1 and LED2 on Init
//    LED_SetState(LED1,0);
//    LED_SetState(LED2,0);

	status = BLEMGMT_Init();
    if(status == BLESTATUS_PENDING){
	 	// The initialisation is ongoing,	
		// Register the stack callback to catch the init success.
		handler.callback = BleApp_CoreCallback;
		status = BLEMGMT_RegisterHandler(&handler);
	}
	return status; //BLESTATUS_FAILED or BLESTATUS_SUCCESS
} 

//Example


void BLEAPP_SetValue(U8 value)
{

    BleDateTime current_time;
	current_time.year = 2015;
	current_time.month = 1;
	current_time.day = 16;
	current_time.hours = 10;
	current_time.minutes = 26;
	current_time.seconds = 50;
	static U16 measureValue = 0x0156;
	static U16 measure_time = 0;
#if 1
	glucoseMeasureSet.sequenceNumberValue = 0;
	glucoseMeasureSet. baseTimeValue = current_time;
	glucoseMeasureSet.flagsValue.fields.concentrationPresent = 1;
	glucoseMeasureSet.concentrationValue = measureValue++;
	glucoseMeasureSet.flagsValue.fields.timeOffsetPresent = 1;
	glucoseMeasureSet.timeOffsetValue = 0x60;
#endif
    appRequest = APPREQUEST_SETVALUE;

}

BleStatus BLEAPP_ReadyToGoToSleep( void ){
  
    if(appRequest == APPREQUEST_NOREQUEST){
        if( (appState == APPSTATE_INITIALIZED)){
            return BLESTATUS_SUCCESS;
        }
        if(((appState & APPSTATE_INITIALIZED) != (AppRequest)0) &&
          (appState & APPSTATE_SLEEP) != (AppRequest)0){ 
            return BLESTATUS_SUCCESS;
        }   
   }
   return BLESTATUS_FAILED;
}

void	BLEMGMT_NotifyToRun(void){
	appRequest  |=  APPREQUEST_NOTIFYTORUN;
}

AppRequest	BLEAPP_Scheduling(void){
	//
	// INTERNAL APPLICATION REQUESTS
	//
    while ( (appRequest & APPREQUEST_NOTIFYTORUN) != (AppRequest)0 ) {
		// Clear this appRequest flags
		appRequest &= ~APPREQUEST_NOTIFYTORUN;
		// Do the stuff, note that BLEMGMT_Run() may lead to a 
		// BLEMGMT_NotifyToRun() so purge this request while it 
		// exists.
		BLEMGMT_Run();
	}

    if( (appRequest & APPREQUEST_RANDOM_ADDRESS) != (AppRequest)0 ){
#if (BLE_RANDOM_ADDRESSING_SUPPORT == 1)
        BleStatus status;
#endif //(BLE_RANDOM_ADDRESSING_SUPPORT == 1)

		// Clear this appRequest flags
		appRequest &= ~APPREQUEST_RANDOM_ADDRESS;
#if (BLE_RANDOM_ADDRESSING_SUPPORT == 1)
		// Generate a random address
		status = BLESMP_GenerateAndSetRandomAddress(BLESM_RANDOMADDRESS_STATIC);
		if(status != BLESTATUS_PENDING){
			BLEAPP_Init();
		} else {
			// Go ahead :
            // The operation is ongoing, wait for the 
            // BLEEVENT_GENERATE_RANDOM_ADDRESS_RSP event indicating
            // this operation completion.    
		}
#endif //(BLE_RANDOM_ADDRESSING_SUPPORT == 1)
	}

	if( (appRequest & APPREQUEST_REGISTERGAPDEVICE) != (AppRequest)0 ){
		BleStatus status;

		// Clear this appRequest flags
		appRequest &= ~APPREQUEST_REGISTERGAPDEVICE;

		// Register a GAP device
		status = BLEGAP_RegisterDevice(BLEGAPROLE_PERIPHERAL, &BleApp_GapCallback);
		if(status != BLESTATUS_SUCCESS){
			// Should not happen, reset
			BLEAPP_Init();
		} else {
            //Set Device name
            BLEGAP_SetLocalBluetoothDeviceName( (U8 *)bleLocalDeviceName, sizeof(bleLocalDeviceName));
			// Go ahead : init profiles
			appRequest 	|= APPREQUEST_INITPROFILES;
		}
	}

	if( (appRequest & APPREQUEST_STARTDISCOVERABLE) != (AppRequest)0 ) {
        // Clear this appRequest flags
		appRequest &= ~APPREQUEST_STARTDISCOVERABLE;
		// Do the stuff.
        // We are starting the broadcast only if we are 
		// initialized.
		if( (appState & APPSTATE_INITIALIZED) != 0 ){
			BleStatus status;
            
			status = BLEGAP_SetMode(BLEMODE_DISCOVERABLE | BLEMODE_CONNECTABLE);
			if ( status == BLESTATUS_FAILED){
				// We have an error here
				// reset the chip?
				BLEAPP_Init();
			} else if ( status == BLESTATUS_SUCCESS){
				// already in this state
				appState |= APPSTATE_DISCOVERABLE;
			} else {
			 	// Means pending wait for the BLEGAP_EVENT_MODECHANGE in 
				// the GAP callback
			}	
		}
	}

	if( (appRequest & APPREQUEST_INITPROFILES) != (AppRequest)0  ){
		// Clear this appRequest flags
		appRequest &= ~APPREQUEST_INITPROFILES;

#if (BLEGLUCOSE_SUPPORT_SENSOR == 1)
	// Register a local Glucose SENSOR with a record table as parameter
	BLEGLUCOSE_SENSOR_Register(&measurementRecords[0],MAX_NUMBER_OF_RECORDS);
#endif //(BLEGLUCOSE_SUPPORT_SENSOR == 1)

#if (BLE_SUPPORT_DEVICEINFORMATION_SERVICE == 1)
    BLEDEVICEINFORMATION_Register();

#if (BLEDEVICEINFORMATION_SUPPORT_SYSTEMID == 1)
    // The following API does not need application life-time validity pointer
    BLEDEVICEINFORMATION_SetSystemId((U8*)"12345678");
#endif //(BLEDEVICEINFORMATION_SUPPORT_SYSTEMID == 1)

#if (BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME == 1)
    // The following API need application life-time validity pointer
    BLEDEVICEINFORMATION_SetManufacturerName((U8*)bleDeviceInfoManufacturerName,7);
#endif //(BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME == 1)

#if (BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER == 1)
    // The following API  need application life-time validity pointer
    BLEDEVICEINFORMATION_SetModelNumber((U8*)bleDeviceInfoModelNumber,5);
#endif //(BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER == 1)
#endif //(BLE_SUPPORT_DEVICEINFORMATION_SERVICE == 1)


		// Go ahead : Be discoverable
		// For demonstration place EM9301 in SLEEP mode
		appRequest |= APPREQUEST_EM9301SLEEP;
	}

	if( (appRequest & APPREQUEST_EM9301SLEEP) != (AppRequest)0 ){
		U8 commandParms[1] = { 0x01 }; // SLEEP mode
		// Clear this appRequest flags
		appRequest &= ~APPREQUEST_EM9301SLEEP;
		// Place the EM9301 in the SLEEP mode as the application is in
		// BLE NON ACTIVE state
		// An application is in BLE ACTIVE mode when:
		//     - As central, it is connecting or connected.
		//     - As peripheral, it is discoverable and/or connectable or connected.
		if( BLEHCI_SendVendorSpecificCommand( 0xFC03, 0x01, commandParms ) == BLESTATUS_FAILED ){
		 	// Failed to go in SLEEP mode, should not happen
			// Go ahead : be discoverable
			appRequest 	|= APPREQUEST_STARTDISCOVERABLE;
		}
		// The answer of this transition will be notified by a command complete in the
		// BleApp_CoreCallback Core stack callback
	}

	if( (appRequest & APPREQUEST_EM9301IDLE) != (AppRequest)0 ){
		U8 commandParms[1] = { 0x00 }; // IDLE mode
		// Clear this appRequest flags
		appRequest &= ~APPREQUEST_EM9301IDLE;
		
		// Place the EM9301 in the IDLE mode as the application next step is
		// to want to go to an BLE ACTIVE state.
		// An application is in BLE ACTIVE mode when:
		//     - As central, it is connecting or connected.
		//     - As peripheral, it is discoverable and/or connectable or connected.
		if( BLEHCI_SendVendorSpecificCommand( 0xFC03, 0x01, commandParms ) == BLESTATUS_FAILED ){
		 	// Failed to go in ACTIVE mode, should not happen
			// Re-Init the stack;
			BLEMGMT_Deinit();
			BLEMGMT_Init();
		}
	}

	if( (appRequest & APPREQUEST_PAIRING) != (AppRequest)0  ) {
        BleStatus status;
                
        if((appState & APPSTATE_SLEEP) != 0){
            appRequest |= APPREQUEST_EM9301IDLE;
        }
        else{
            // Clear this appRequest flags
            appRequest &= ~APPREQUEST_PAIRING;
            // Do the stuff
            status = BLESMP_InitiatePairing(currentConnHandle, 1);
            if(status == BLESTATUS_PENDING ){
                // Pairing started, wait for completion...
                // in BLEEVENT_PAIRING_COMPLETE
                pairingInProgress = 1;
            } else {
                // Pairing failed
                if (pairingInProgress == 1){
                    //TODO: Put here the action to do if a pairing
                    // failed because of a pairing in progress
                }
            }
        }
	}

    if( (appRequest & APPREQUEST_LINKUP) != (AppRequest)0  ) {


        if((appState & APPSTATE_SLEEP) != 0){
            appRequest |= APPREQUEST_EM9301IDLE;
        }
        else{
            // Clear this appRequest flags
            appRequest &= ~APPREQUEST_LINKUP;
            // Do the stuff, if we are supporting CLIENT

        }
	}

	if( (appRequest & APPREQUEST_DISCONNECT) != (AppRequest)0  ) {        
        if((appState & APPSTATE_SLEEP) != 0){
            appRequest |= APPREQUEST_EM9301IDLE;
        }
        else{
            // Clear this appRequest flags
            appRequest &= ~APPREQUEST_DISCONNECT;
            // Do the stuff
            BLEGAP_Disconnect( currentConnHandle ); 
        }
	}
	//
	// INTERNAL APPLICATION PROCESSING
	//
    //Application example
    if( (appRequest & APPREQUEST_SETVALUE) != 0 ){
        appRequest &= ~APPREQUEST_SETVALUE;
    BLEGLUCOSESERVICE_SetGlucoseMeasurement(&glucoseMeasureSet);
        //Put here the appropriate code
	}

	// Return the Application Request
	return appRequest;
}


U8 Ble_Conneted(void)
{
	if((appState & APPSTATE_BONDED) != 0)
	//if((appState & APPSTATE_CONNECTED) != 0)
	{
		return 1;
	}
	return 0;
}

void setGlucoseValue(void)
{
    
    static U8 data = 0;
	if(Ble_Conneted())
	{        
        BLEAPP_SetValue(data++%100);

	}

}
