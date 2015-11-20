#ifndef __BLEGAP_H
#define __BLEGAP_H
/****************************************************************************
 *
 * File:          BleGap.h
 *
 * Description:   interface for the GAP profile
 * 
 * Created:       February, 2009
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.47 $
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
#include "BleEngine.h"

/***************************************************************************\
 *	CONFIGURATION
\***************************************************************************/
/** BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME
 * Enable ( set to 1) or Disable (set To 0) the ability for the local device
 * To retrieve the remote device name though a connection.
 * It enables the BLEGAP_GetRemoteDeviceName() API.
 * Enabling it increase consequently the final ROM and RAM size because it 
 * add a GATT client into the GAP in order to read the GAP Remote Device Name
 * characteristic.
 * BLE_CONNECTION_SUPPORT shall be enabled.
 * BLEGATT_SUPPORT_SINGLE_SERVICE_DISCOVERY shall be enabled.
 * BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_TYPE shall be enabled.
 * This option is ignored when one of these options are disabled.
 */
#ifndef BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME
#define BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME				0
#endif //BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME

#if (BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME == 1)
#include "BleGatt.h"
#endif //(BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME == 1)

#if (BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME == 1)
#if (ATT_ROLE_CLIENT == 0) 
#error ATT_ROLE_CLIENT shall be Enabled when BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME is enabled
#endif //(ATT_ROLE_CLIENT == 0) 
#endif //(BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME == 1)

#if (BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME == 1)
#if !( (BLE_CONNECTION_SUPPORT == 1) && \
	(BLEGATT_SUPPORT_SINGLE_SERVICE_DISCOVERY == 1) && \
	(BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_TYPE == 1) )
#error BLE_CONNECTION_SUPPORT and BLEGATT_SUPPORT_SINGLE_SERVICE_DISCOVERY and \
BLEGATT_SUPPORT_READ_CHARACTERISTIC_VALUE_BY_TYPE shall be Enabled when BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME\
is enabled
#endif //
#endif //(BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME == 1)
/***************************************************************************\
 * CONSTANTS and MACROS Definition
\***************************************************************************/


// The following constants are a representation of the Appendix A : 
// 11	APPENDIX A (NORMATIVE): TIMERS AND CONSTANTS of the GAP 
// specification


//////////////////////////////////////////////////////////////////////////////
// TIMERS
//////////////////////////////////////////////////////////////////////////////

// TGAP(lim_timeout) = 30.72s = 30720 
// CSA2 specification (27 december 2011) change this value to 180 seconds
// Maximum time to remain limited discoverable (advertising)	
// When in limited discoverable mode
#define TGAP_LIM_TIMEOUT									180000


// TGAP(gen_inquiry) = 10.24s = 10240
// Minimum time to perform scanning		
// Inquiry STATE shall last TGAP(gen_inquiry) or longer	
// when performing general or limited inquiry
#ifndef TGAP_GEN_INQUIRY
#define TGAP_GEN_INQUIRY									10240
#endif //TGAP_GEN_INQUIRY 

//////////////////////////////////////////////////////////////////////////////
// SCAN INTERVAL and WINDOWS
//////////////////////////////////////////////////////////////////////////////

// RECOMMENDED FAST FOR LIMITED AND GENERAL DISCOVERY AND FOR CONNECTION
// => When user initiated

// TGAP_FAST_SCAN_INT
// CSA3 specification (24 July 2012) change this value from 11.25 ms to
//  30 ms
// it also merge the discovery and conection scan intervals 
// i.e. TGAP_CONN_SCAN_INT and TGAP_GEN_INQUIRY_SCAN_INT to
// TGAP(fast_scan_int) = 30 ms = 48 = 0x0030				
// Scan interval in any discovery or connection establishment procedure 
// when user initiated
#ifndef TGAP_FAST_SCAN_INT
#define TGAP_FAST_SCAN_INT							        0x0030 
#endif //TGAP_FAST_SCAN_INT

// TGAP_FAST_SCAN_WIND
// CSA3 specification (24 July 2012) change this value from 11.25 ms to
//  30 ms
// it also merge the discovery and conection scan windows
// i.e. TGAP_CONN_SCAN_WIND and TGAP_GEN_INQUIRY_SCAN_WIND to
// TGAP(fast_scan_wind) = 30 ms = 48 = 0x0030				
// Scan windows in any discovery or connection establishment procedure 
// when user initiated
#ifndef TGAP_FAST_SCAN_WIND
#define TGAP_FAST_SCAN_WIND							        0x0030
#endif //TGAP_FAST_SCAN_WIND     

// RECOMMENDED SLOW FOR LIMITED AND GENERAL DISCOVERY AND FOR CONNECTION
// => When backgroung initiated

// TGAP_SLOW_SCAN_INT
// TGAP(scan_slow_int1) = 11.25 ms = 18 = 0x0012				
// Scan interval in any discovery or connection establishment procedure 
// when background scanning
#ifndef TGAP_SLOW_SCAN_INT
#define TGAP_SLOW_SCAN_INT							        0x0012
#endif //TGAP_SLOW_SCAN_INT  

// TGAP_SLOW_SCAN_WIND
// TGAP(scan_slow_wind1) = 1.28 s = 2048 = 0x0800				
// Scan window in any discovery or connection establishment procedure 
// when background scanning
#ifndef TGAP_SLOW_SCAN_WIND
#define TGAP_SLOW_SCAN_WIND							        0x0800
#endif //TGAP_SLOW_SCAN_WIND   

//////////////////////////////////////////////////////////////////////////////
// ADVERTISEMENT INTERVALs
//////////////////////////////////////////////////////////////////////////////

// RECOMMENDED FAST FOR LIMITED AND GENERAL DISCOVERY AND FOR CONNECTION
// => When user initiated


// For all limited discoverable, general discoverable mode and connectable
// mode the advertising interval minimum and maximum are recommended to 1.28s
// is 0x800
// Range: 0x0020 to 0x4000 (default is 0x800)
// Range: 20ms to 10.24 s (default is 1.28 s)

// TGAP_FAST_ADV_INT
// CSA3 specification (24 July 2012) change this value from 1.28 s to
//  60 ms
// TGAP(fast_adv_int) = 60 ms = 96 = 0x0060				
// Minimum to maximum advertisement interval in any discoverable or 
// connectable mode when user initiated
#ifndef TGAP_FAST_ADV_INT
//#define TGAP_FAST_ADV_INT	    							0x0800
//#define TGAP_FAST_ADV_INT									0x0060 
// Note that in our case, we specify 0x160 as recommended advertising interval
// it is because 0x160 is the minimum advertising interval when discoverable only
#define TGAP_FAST_ADV_INT									0x0160 
#endif //TGAP_FAST_ADV_INT


// RECOMMENDED SLOW FOR LIMITED AND GENERAL DISCOVERY AND FOR CONNECTION
// => When backgroung initiated

// TGAP_SLOW_ADV_INT
// TGAP(slow_adv_int) = 1025 ms = 1640 = 0x0668				
// Minimum to maximum advertisement interval in any discoverable or 
// connectable mode when user initiated
#ifndef TGAP_SLOW_ADV_INT
#define TGAP_SLOW_ADV_INT									0x0668
#endif //TGAP_SLOW_ADV_INT

//////////////////////////////////////////////////////////////////////////////
// CONNECTION PARAMETERS
//////////////////////////////////////////////////////////////////////////////

// TGAP(initial_conn_interval) = 30ms = 24 slots = 0x0028
// Minimum Link Layer connection interval
// When using Connection Establishment procedure
// Range: 0x0006 to 0x0C80
// Note that we choose to set this value to 50ms instead of recommended 
// 30ms in order to save power consumption
#ifndef TGAP_INITIAL_CONN_INTERVAL_MIN
//#define TGAP_INITIAL_CONN_INTERVAL_MIN					0x0190
#define TGAP_INITIAL_CONN_INTERVAL_MIN                    0x0028 
//#define TGAP_INITIAL_CONN_INTERVAL_MIN                    0x0018 
#endif //TGAP_INITIAL_CONN_INTERVAL_MIN

// TGAP(initial_conn_interval) = 50ms = 40 slots = 0x0028
// Minimum Link Layer connection interval
// When using Connection Establishment procedure
// Range: 0x0006 to 0x0C80
#ifndef TGAP_INITIAL_CONN_INTERVAL_MAX
//#define TGAP_INITIAL_CONN_INTERVAL_MAX					0x0190
#define TGAP_INITIAL_CONN_INTERVAL_MAX						0x0028
#endif //TGAP_INITIAL_CONN_INTERVAL_MAX

// TGAP(conn_est_latency) = 0 =  0x0000
// Link Layer connection slave latency
// When using Connection Establishment procedure
#ifndef TGAP_CONN_LATENCY
#define TGAP_CONN_LATENCY									0x0000
#endif //TGAP_CONN_LATENCY

// TGAP(conn_est_min_ce_len) = 0 =  0x0000
// Link Layer connection minimum length
// When using Connection Establishment procedure
#ifndef TGAP_CONN_MIN_LENGTH
#define TGAP_CONN_MIN_LENGTH								0x0000
#endif //TGAP_CONN_MIN_LENGTH

// TGAP(conn_est_max_ce_len) = 0 =  0x0000
// Link Layer connection maximum length
// When using Connection Establishment procedure
#ifndef TGAP_CONN_MAX_LENGTH
#define TGAP_CONN_MAX_LENGTH								0x0000
#endif //TGAP_CONN_MAX_LENGTH

// TGAP(conn_est_superv_timeout) = 20 s = 2000 = 0x07D0
// Link Layer connection supervision timeout
// When using Connection Establishment procedure
#ifndef TGAP_CONN_SUPERV_TIMEOUT
//#define TGAP_CONN_SUPERV_TIMEOUT							0x0C80	// Max
//#define TGAP_CONN_SUPERV_TIMEOUT							0x07D0
#define TGAP_CONN_SUPERV_TIMEOUT							0x03E8 //speed-up to 10s
#endif //TGAP_CONN_SUPERV_TIMEOUT

//////////////////////////////////////////////////////////////////////////////

/***************************************************************************\
 *	TYPES definition
\***************************************************************************/

/** 
 * BleGapRole type
 * The main Role of the local Device
 */
typedef U8 BleGapRole;
#define BLEGAPROLE_BROADCASTER								1
#define BLEGAPROLE_OBSERVER									2
#define BLEGAPROLE_PERIPHERAL								3
#define BLEGAPROLE_CENTRAL									4


/** 
 * BleGapMode type
 * Used to define the current Accessibility mode ( CONNECTABILITY / 
 * DISCOVERABILITY ) of the local Device.
 */
typedef U8 BleGapMode;
// Not Connectable: The Local device cannot accept incoming connection.
#define BLEMODE_NOTCONNECTABLE								0x00
// Connectable: The Local device can be connected by any remote device.
#define BLEMODE_CONNECTABLE									0x01
// The Connectable mask of the current BleGapMode.
#define BLEMODE_CONNECTABLE_MASK							0x01

// Not discoverable : The local device is in the not-discoverable mode.
#define BLEMODE_NOTDISCOVERABLE								0x00
// Discoverable : The local device may be discovered by a remote device.
#define BLEMODE_DISCOVERABLE								0x10
// Limited discoverable : The local device may be discovered during 
// TGAP_LIM_TIMEOUT millisecond, after it falls in the not-discoverable mode.
#define BLEMODE_LIMITEDDISCOVERABLE							0x20
// The Discoverale mask of the current BleGapMode.
#define BLEMODE_DISCOVERABLE_MASK							0x30


#if	(BLE_ROLE_SCANNER == 1)
/** 
 * BleInquiryMode type
 * Used to define the desired Inquiry mode for an Inquiry (LIMITED or
 * GENERAL ), used in BLEGAP_StartInquiry() API.
 */
typedef U8 BleInquiryMode;
// The desired inquiry mode is limited inquiry. The inquiry reports only 
// remote devices in limited discoverable mode.
#define BLEINQUIRYMODE_LIMITED								0x01
// The desired inquiry mode is general inquiry. The inquiry reports general
// and limited discoverable devices.
#define BLEINQUIRYMODE_GENERAL								0x02


/** 
 * BleGapDuplicateFilteringMode type
 * Used to define the desired duplicate filtering mode for an Obervation
 * procedure (enabled or disabled ), used in BLEGAP_StartObserveProcedure() 
 * API and BLEGAP_StartObserveProcedureWithInterval() API.
 */
typedef U8 BleGapDuplicateFilteringMode;
// The desired BleGapDuplicateFilteringMode mode is disabled. The remote 
// device's  broadcast packet will be reported each broadcast interval
#define BLEGAPDUPLICATEFILTERING_OFF						0x00
// The desired BleGapDuplicateFilteringMode mode is enabled. The remote 
// device's  broadcast packet will be only once if the data does not change,
// and each time data change.
#define BLEGAPDUPLICATEFILTERING_ON							0x01


/** 
 * AdDataBitfield type
 * It is a bitfield used to define which field is included in the AD data
 * it is OR'ED using the BLEGAPADDATABIT_* definition
 */
typedef U32 BleGapAdDataBitField;

//Flags
#define BLEGAPADDATABIT_FLAG								0x00000001u
//Incomplete List of 16-bit Service Class UUIDs
#define BLEGAPADDATABIT_SERVICEUUID16_MORE					0x00000002u
//Complete List of 16-bit Service Class UUIDs
#define BLEGAPADDATABIT_SERVICEUUID16_FULL					0x00000004u
//Incomplete List of 128-bit Service Class UUIDs 
#define BLEGAPADDATABIT_SERVICEUUID128_MORE					0x00000020u
//Complete List of 128-bit Service Class UUIDs
#define BLEGAPADDATABIT_SERVICEUUID128_FULL					0x00000040u
//Shortened Local Name
#define BLEGAPADDATABIT_SHORTNAME							0x00000080u
//Complete Local Name
#define BLEGAPADDATABIT_COMPLETENAME						0x00000100u
//Tx Power Level
#define BLEGAPADDATABIT_TXPOWERLEVEL						0x00000200u
//Slave Connection Interval Range
#define BLEGAPADDATABIT_SLAVECONNINTERVAL					0x00020000u
//List of 16-bit Service Solicitation UUIDs
#define BLEGAPADDATABIT_SERVICEUUID16SOLICITATION			0x00080000u
//List of 128-bit Service Solicitation UUIDs
#define BLEGAPADDATABIT_SERVICEUUID128SOLICITATION			0x00100000u
//Service Data
#define BLEGAPADDATABIT_SERVICEDATA							0x00200000u
//Public Target Address
#define BLEGAPADDATABIT_PUBLICTARGETADDRESS					0x00400000u
//Random Target Address
#define BLEGAPADDATABIT_RANDOMTARGETADDRESS					0x00800000u
//Appearance
#define BLEGAPADDATABIT_APPEARANCE							0x01000000u
//Manufacturer Specific Data
#define BLEGAPADDATABIT_VENDORSPECIFIC						0x10000000u

#endif	//(BLE_ROLE_SCANNER == 1)

/** 
 * AdDataType type
 * The type of device specific data that can be included in the AD data
 */
typedef U8 BleGapAdDataType;

//Flags
#define BLEGAPADDATATYPE_FLAG								0x01 		
//Incomplete List of 16-bit Service Class UUIDs
#define BLEGAPADDATATYPE_SERVICEUUID16_MORE					0x02
//Complete List of 16-bit Service Class UUIDs
#define BLEGAPADDATATYPE_SERVICEUUID16_FULL					0x03
//Incomplete List of 128-bit Service Class UUIDs 
#define BLEGAPADDATATYPE_SERVICEUUID128_MORE				0x06
//Complete List of 128-bit Service Class UUIDs
#define BLEGAPADDATATYPE_SERVICEUUID128_FULL				0x07
//Shortened Local Name
#define BLEGAPADDATATYPE_SHORTNAME							0x08
//Complete Local Name
#define BLEGAPADDATATYPE_COMPLETENAME						0x09
//Tx Power Level
#define BLEGAPADDATATYPE_TXPOWERLEVEL						0x0A
//Slave Connection Interval Range
#define BLEGAPADDATATYPE_SLAVECONNINTERVAL					0x12
//List of 16-bit Service Solicitation UUIDs
#define BLEGAPADDATATYPE_SERVICEUUID16SOLICITATION			0x14
//List of 128-bit Service Solicitation UUIDs
#define BLEGAPADDATATYPE_SERVICEUUID128SOLICITATION			0x15
//Service Data
#define BLEGAPADDATATYPE_SERVICEDATA						0x16
//Public Target Address
#define BLEGAPADDATATYPE_PUBLICTARGETADDRESS				0x17
//Random Target Address
#define BLEGAPADDATATYPE_RANDOMTARGETADDRESS				0x18
//Appearance
#define BLEGAPADDATATYPE_APPEARANCE							0x19
//Manufacturer Specific Data
#define BLEGAPADDATATYPE_VENDORSPECIFIC						0xFF

/** 
 * BleGapAppearance type
 * Used to define the possible values for the appearance to set in the
 * BLEGAP_SetAppearence API.
 * These values are defined in the Bluetooth Assigned number
 */
typedef U16 BleGapAppearance;
#define BLEGAPAPPEARANCE_UNKNOWN							0x0000
#define BLEGAPAPPEARANCE_GENERIC_PHONE						0x0040
#define BLEGAPAPPEARANCE_GENERIC_COMPUTER					0x0080
#define BLEGAPAPPEARANCE_GENERIC_WATCH						0x00C0
#define BLEGAPAPPEARANCE_WATCH_SPORTWATCH					0x00C1
#define BLEGAPAPPEARANCE_GENERIC_CLOCK						0x00FF
#define BLEGAPAPPEARANCE_GENERIC_DISPLAY					0x0140
#define BLEGAPAPPEARANCE_GENERIC_REMOTECONTROL				0x0180
#define BLEGAPAPPEARANCE_GENERIC_EYEGLASSES					0x01C0
#define BLEGAPAPPEARANCE_GENERIC_TAG						0x0200
#define BLEGAPAPPEARANCE_GENERIC_KEYRING					0x0240
#define BLEGAPAPPEARANCE_GENERIC_MEDIAPLAYER				0x0280
#define BLEGAPAPPEARANCE_GENERIC_BARCODESCANNER				0x02C0
#define BLEGAPAPPEARANCE_GENERIC_THERMOMETER				0x0300
#define BLEGAPAPPEARANCE_THERMOMETER_EAR					0x0301
#define BLEGAPAPPEARANCE_GENERIC_HEARTRATE					0x0340
#define BLEGAPAPPEARANCE_HEARTRATE_BELT						0x0341
#define BLEGAPAPPEARANCE_GENERIC_BLOODPRESSURE				0x0380
#define BLEGAPAPPEARANCE_BLOODPRESSURE_ARM					0x0381
#define BLEGAPAPPEARANCE_BLOODPRESSURE_WRIST				0x0382
#define BLEGAPAPPEARANCE_GENERIC_HID						0x03C0
#define BLEGAPAPPEARANCE_HID_KEYBOARD						0x03C1
#define BLEGAPAPPEARANCE_HID_MOUSE							0x03C2
#define BLEGAPAPPEARANCE_HID_JOYSTICK						0x03C3
#define BLEGAPAPPEARANCE_HID_GAMEPAD						0x03C4
#define BLEGAPAPPEARANCE_HID_DIGITIZERTABLET				0x03C5
#define BLEGAPAPPEARANCE_HID_CARDREADER						0x03C6
#define BLEGAPAPPEARANCE_HID_DIGITALPEN						0x03C7
#define BLEGAPAPPEARANCE_HID_BARCODESCANNER					0x03C8

/** 
 * BleGapEvent type
 * Define the different kind of events that could be received by the 
 * BleGapCallBack, within each event a differnet status and parameter is
 * defined
 */
typedef U8 BleGapEvent;


#if	(BLE_ROLE_SCANNER == 1)
/** BLEGAP_EVENT_INQUIRYRESULT
 * Event received after the call of the BLEGAP_StartInquiry() API. It 
 * indicates that a remote device has been found.
 * The remote device is providing useful information in the Inquiry Result
 * such as name, service supported....
 * This event is received when the local device recieve an ADVERTISEMENT
 * Packet.
 * BLE_ROLE_SCANNER shall be enabled.
 * The status field is always BLESTATUS_SUCCESS.
 * The parms field indicates the inquiry results though the 
 * BleGapInquiryResult Type
 */
#define BLEGAP_EVENT_INQUIRYRESULT							0x5F

/** BLEGAP_EVENT_INQUIRYRESULT_EXT
 * Event received after the call of the BLEGAP_StartInquiry() API. It 
 * may follow BLEGAP_EVENT_INQUIRYRESULT depending if the remote device
 * has more information to transmit using the Inquiry Packet.
 * This event is received when the local device recieve a SCAN RESPONSE
 * Packet.
 * BLE_ROLE_SCANNER shall be enabled.
 * The status field is always BLESTATUS_SUCCESS.
 * The parms field indicates the inquiry results though the 
 * BleGapInquiryResult Type
 */
#define BLEGAP_EVENT_INQUIRYRESULT_EXT						0x60

/** BLEGAP_EVENT_INQUIRYCOMPLETE
 * Event indicating that The Inquiry is Complete.
 * An inquiry can be complete because the inquiry process has completed after
 * the TGAP_GEN_INQUIRY time, or because the user have call the 
 * BLEGAP_CancelInquiry() API.
 * BLE_ROLE_SCANNER shall be enabled.
 * The status field is always BLESTATUS_SUCCESS.
 * The parms field is always NULL
 */
#define BLEGAP_EVENT_INQUIRYCOMPLETE						0x61
#endif	//(BLE_ROLE_SCANNER == 1)

/** BLEGAP_EVENT_MODECHANGE
 * Event indicating that the Accessibilty mode have change for the local 
 * Device.
 * it could follow a BLEGAP_SetMode() API call.
 * The status field indicates the status of the operation. (BLESTATUS_SUCCESS
 * or BLESTATUS_FAILED) 
 * The parms field is always NULL.
 * In order to retrieve the new Accessibilty mode, BLEGAP_GetMode() APi may be
 * issued.
 */
#define BLEGAP_EVENT_MODECHANGE								0x62

#if (BLE_CONNECTION_SUPPORT == 1)
/** BLEGAP_EVENT_CONNECTED
 * Event indicating that a connection have completed.
 * for a CENTRAL,it follows a BLEGAP_Connect() API call.
 * For a PERIPHERAL, it means that a remote device have created the 
 * connection.
 * BLE_CONNECTION_SUPPORT shall be enabled.
 * The status field indicates the status of the operation. (BLESTATUS_SUCCESS
 * or BLESTATUS_FAILED) 
 * The parms field indicates the connection information though the
 * BleGapConnectionInformation Type
 */
#define BLEGAP_EVENT_CONNECTED								0x63

/** BLEGAP_EVENT_DISCONNECTED
 * Event indicating that the connection have been disconnected.
 * BLE_CONNECTION_SUPPORT shall be enabled.
 * The status field indicates the status of the operation. (BLESTATUS_SUCCESS
 * or BLESTATUS_FAILED) 
 * The parms field indicates the connection information though the
 * BleGapConnectionInformation Type
 */
#define BLEGAP_EVENT_DISCONNECTED							0x64

/** BLEGAP_EVENT_CONNECTIONUPDATED
 * Event indicating that the connection have been updated.
 * BLE_CONNECTION_SUPPORT shall be enabled.
 * The status field indicates the status of the operation.
 * The parms field indicates the connection information though the
 * BleGapConnectionInformation Type
 */
#define BLEGAP_EVENT_CONNECTIONUPDATED						0x65

#if	(BLE_ROLE_SCANNER == 1)
/** BLEGAP_EVENT_CONNECTIONCANCELED
 * Event indicating that the connection creation have been cancelled.
 * BLE_CONNECTION_SUPPORT shall be enabled.
 * BLE_ROLE_SCANNER shall be enabled.
 * The status field indicates the status of the operation. (BLESTATUS_SUCCESS
 * or BLESTATUS_FAILED) 
 * The parms field is always NULL
 */
#define BLEGAP_EVENT_CONNECTIONCANCELED						0x66
#endif //(BLE_ROLE_SCANNER == 1)

#endif //(BLE_CONNECTION_SUPPORT == 1)

#if (BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME == 1)
/** BLEGAP_EVENT_NAMEINFORMATION
 * Event indicating that the name Information have been received.
 * it follows a BLEGAP_GetRemoteDeviceName() API call.
 * BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME shall be enabled.
 * The status field indicates the status of the operation. (BLESTATUS_SUCCESS
 * or BLESTATUS_FAILED) 
 * The parms field indicates the name information though the
 * BleGapNameInformation Type
 */
#define BLEGAP_EVENT_NAMEINFORMATION						0x67
#endif //(BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME == 1)

#if	(BLE_ROLE_SCANNER == 1)
/** BLEGAP_EVENT_OBSERVEMODE_DATA
 * Event received after the call of the BLEGAP_StartObserveProcedure() API. It 
 * indicates that data has been observed over the air.
 * BLE_ROLE_SCANNER shall be enabled.
 * The status field is always BLESTATUS_SUCCESS.
 * The parms field indicates the inquiry results though the BleObserveModeData
 * Type.
 */
#define BLEGAP_EVENT_OBSERVEMODE_DATA						0x68
#endif	//(BLE_ROLE_SCANNER == 1)

#if	(BLE_ROLE_ADVERTISER == 1)
/** BLEGAP_EVENT_STARTBROADCAST_RSP
 * Event received after the call of the BLEGAP_StartBroadcastProcedure() API. It 
 * indicates if the broadcast procedure have started.
 * BLE_ROLE_ADVERTISER shall be enabled.
 * The status field indicates the status of the operation.
 * The parms field is always NULL
 */
#define BLEGAP_EVENT_STARTBROADCAST_RSP						0x69

/** BLEGAP_EVENT_STOPBROADCAST_RSP
 * Event received after the call of the BLEGAP_StopBroadcastProcedure() API. It 
 * indicates if the broadcast procedure have stopped.
 * BLE_ROLE_ADVERTISER shall be enabled.
 * The status field indicates the status of the operation.
 * The parms field is always NULL
 */
#define BLEGAP_EVENT_STOPBROADCAST_RSP						0x6A

/** BLEGAP_EVENT_SETBROADCASTVALUE_RSP
 * Event received after the call of the BLEGAP_SetBroadcastValue API. It 
 * indicates if the broadcast value have been set.
 * BLE_ROLE_ADVERTISER shall be enabled.
 * The status field indicates the status of the operation.
 * The parms field is always NULL
 */
#define BLEGAP_EVENT_SETBROADCASTVALUE_RSP					0x6B
#endif	//(BLE_ROLE_ADVERTISER == 1)

#if (BLE_CONNECTION_SUPPORT == 1)
/** BLEGAP_EVENT_UPDATECONNECTION_RSP
 * Event received after the call of the BLEGAP_UpdateConnectionInterval API.  
 * It indicates if the update has started (depending of the status field).
 * BLE_CONNECTION_SUPPORT shall be enabled.
 * The status field indicates the status of the operation.
 * The parms field is always NULL
 */
#define BLEGAP_EVENT_UPDATECONNECTION_RSP					0x6C

/** BLEGAP_EVENT_LOCALNAMEUPADTED
 * Event indicating that the local name Information have been updated by a 
 * remote device.
 * BLE_CONNECTION_SUPPORT shall be enabled.
 * The status field indicates the status of the operation. (BLESTATUS_SUCCESS
 * or BLESTATUS_FAILED) 
 * The parms field indicates the name information though the
 * BleGapNameInformation Type, the connHandle Field indicating the remote
 * device that have written the name
 */
#define BLEGAP_EVENT_LOCALNAMEUPADTED						0x6D

#endif //(BLE_CONNECTION_SUPPORT == 1)

/**
 * BleGapCallBack
 *	This callback receives the BLE GAP events. 
 *  Each of these events can be associated with a defined status and 
 *  parameters.
 *	The callback is executed during the stack context,  be careful to not  
 *	running heavy process in this function.
 */
typedef void (*BleGapCallBack)(BleGapEvent event,
							   BleStatus status,
							   void *parms);

#if	(BLE_ROLE_SCANNER == 1)
/**
 * BleGapInquiryResult
 *	The BleGapInquiryResult type is received as parameter in the GAP callback
 * during the BLEGAP_EVENT_INQUIRYRESULT or BLEGAP_EVENT_INQUIRYRESULT_EXT 
 * event, it contains some information about the remote device found during
 * the INQUIRY procedure
 */
typedef struct{
	/**
	 *	Indicates the remote device address type 
	 *	
	 *	BLEADDRESS_PUBLIC = Public address
	 *	BLEADDRESS_RANDOM = Random address
	 */
	BleAddressType	advAddressType;

	/**
	 *	Device address of the remote device 
	 */
	BD_ADDR			*advAddress;

	/**
	 * The Remote Device discoverability and connectability option
	 * only applicable during BLEGAP_EVENT_INQUIRYRESULT event.
	 */
	BleGapMode		remoteMode;

	/**
	 * rssi value of the advertising packets;
	 * rssi value is from -127 to 20;
	 */
	S8				rssi;

	/**
	 * Data from advertising packets;
	 */
	U8				*adData;

	/**
	 * Indicates size of data in octets from advertising packet
	 * from the advertiser.
	 */
	U8				adDataLen;

} BleGapInquiryResult;
#endif	//(BLE_ROLE_SCANNER == 1)

#if (BLE_CONNECTION_SUPPORT == 1)
/**
 * BleGapConnectionInformation
 *	The BleGapConnectionInformation type is received as parameter in the GAP 
 *  callback during the BLEGAP_EVENT_CONNECTED event, 
 *  BLEGAP_EVENT_DISCONNECTED event and BLEGAP_EVENT_CONNECTIONUPDATED it contains
 *  some information about the current connection.
 */
typedef struct {

	/**
	 * Local identifier of the connection
	 */
	U16 connHandle;

	/**
	 * The local connection role in this Link layer connection,
	 * BLECONNROLE_MASTER or BLECONNROLE_SLAVE
	 */
	BleConnRole connRole; 

	/**
	 *	Indicates the remote device address type
	 *	
	 *	BLEADDRESS_PUBLIC = Public address
	 *	BLEADDRESS_RANDOM = Random address
	 */
	BleAddressType	remoteAddressType;

	/**
	 *	The remote device address
	 */
	BD_ADDR remoteAddress;

	/** 
	 * Indicates the reason of the disconnection.
	 * Refer to the BleDisconnectionReason type description for the list of 
	 * possible value
	 * valid only during a BLEGAP_EVENT_DISCONNECTED event.
	 */
	BleDisconnectionReason  disconnectionReason;

	/**
	 * the current Link layer connection interval
	 * Defines connection interval in the following manner: 
	 * connection Interval = connInterval * 1.25 ms
	 * Range: 0x0006 to 0x0C80
	 */
	U16 connInterval;

	/** 
	 * Slave latency for the connection in number of connection events.
	 * Defines the slave latency parameter in the following manner:
	 * Slave connection Latency = connLatency (as number of LL connection
	 * events).
	 * Range: 0x0000 to 0x0C80
	 * Valid only during either a BLEGAP_EVENT_CONNECTED event when 
	 * role is BLECONNROLE_SLAVE or a 
	 * BLEGAP_EVENT_CONNECTIONUPDATED event.
	 */
	U16 connLatency;
	
	/** 
	 * Defines the connection timeout parameter in the following manner:
	 * connection timeout = connTimeout * 10 ms 
	 * Range: 0x000A to 0x01F4
	 * Valid only during either a BLEGAP_EVENT_CONNECTED event when 
	 * role is BLECONNROLE_SLAVE or a 
	 * BLEGAP_EVENT_CONNECTIONUPDATED event.
	 */
	U16 connSupervisionTimeout;

} BleGapConnectionInformation;

/**
 * BleGapNameInformation
 *	The BleGapNameInformation type is received as parameter in the GAP 
 * callback during the BLEGAP_EVENT_NAMEINFORMATION event,
 * it contains some information about the name of the rmeote device.
 */
typedef struct{
	/**
	 * The connection handle identifying the remote device
	 */
	U16 connHandle;

	/**
	 * The pointer containing the name of the remote device
	 */
	U8  *name;

	/**
	 * The nameLen in the name pointer
	 */
	U16  nameLen;

} BleGapNameInformation;

#endif //(BLE_CONNECTION_SUPPORT == 1)


#if	(BLE_ROLE_SCANNER == 1)
/**
 * BleObserveModeData
 *	The BleObserveModeData type is received as parameter in the GAP 
 * callback during the BLEGAP_EVENT_OBSERVEMODE_DATA event,
 * it contains some information about the data received during the OBSERVE
 * procedure.
 */
typedef struct{
    /**
	 *	Indicates the remote device ( broadcaster) address type used in the 
	 *  broadcasted packet
	 *	
	 *	BLEADDRESS_PUBLIC = Public address
	 *	BLEADDRESS_RANDOM = Random address
	 */
	BleAddressType	remoteAddressType;


	/**
	 * Indicates the remote device ( broadcaster) address type used by the
     * remote broadcasted.
	 */
	BD_ADDR         remoteAddress;

	/**
	 * reserved, do not use
	 */
	U8   reserved;	

	/**
	 * The length of the observed data in the observeData pointer.
	 */
	U8              observeDataLen;	
    
    /**
	 * The pointer containing the broadcasted data.
	 */
	U8              *observeData;

	/**
	 * RSSI value of the received observed packet in dBm.
     * The RSSI value represent a signed integer (from -127 to 127)
     *
     * - Values between -127 to +20 are indicating the RSSI value in dBm.
     * - if RSSI is equal to 127 means that RSSI value is not available.
     * - values between 21 to 126 are reserved for future use.
     *
	 */
	S8	            rssi;
} BleObserveModeData;
#endif	//(BLE_ROLE_SCANNER == 1)


/****************************************************************************
 *	APPLICATION INTERFACE functions definition
 ****************************************************************************/

/** Initialise the current device with the given role
 *
 * BLEGAP_RegisterDevice():
 *	This function is used to register a Bluetooth Low Energy device with
 *	the specified role.
 * It shall be an entry point of any application using GAP and should be 
 * called after stack initialization.
 *
 * @param role: The role for the current device
 * @param callback: The callback that will receive the GAP events
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the local device has been successfully
 *  registered for the given role.
 *
 *	- BLESTATUS_FAILED indicates that the local device has failed to 
 *  register for the given role.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter, it may be du because the device was previously
 *		registered for an other role, or the given role is not supported
 *		according to the current configuration.
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_RegisterDevice(BleGapRole role, BleGapCallBack callback);


/* Get the local Bluetooth Device address
 *
 * BLEGAP_GetLocalBdAddr()
 *	This function is used to retrieve the local bluetooth Address 
 *
 *  localAddress: a valid pointer to a BD_ADDR structure that will point to the 
 *      localAddress value upon success.
 *
 *  localAddressType: a pointer to a BleAddressType that will receive the type 
 *      of the address upon success.
 *      May be null if the application is not interested to retreive this 
 *      information or PRIVATE ADDRESSING is not supported.
 *
 * return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded; the addr 
 *    pointer has received the local Bluetooth device Address.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed. It may be du
 *    because the stack is not initialized.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function has failed because
 *		an invalid parameter (addr pointer is null).
 */
BleStatus BLEGAP_GetLocalBdAddr(BD_ADDR         **localAddress, 
                                BleAddressType  *localAddressType);

/** Retreiving the current device role
 *
 * BLEGAP_GetCurrentRole():
 *	this function is used to retreiving the current Bluetooth Low Energy
 * device role previously registered with BLEGAP_RegisterDevice()
 *
 * @return The current role of the local device, or zero, if the device
 *	 is not registered
 *	
 * @author Alexandre GIMARD
 */
BleGapRole BLEGAP_GetCurrentRole( void );

/** Quickly switch to another GAP device role
 *
 * BLEGAP_SwitchRole():
 *	This function is used to quickly switch to another GAP device role
 * (CENTRAL, PERIPHERAL, OBSERVER, BROADCASTER)
 *  The stack allows the switch only if the current mode is allowing the 
 * switch. The switch is allowed mainly if the local device:
 *  - is not CONNECTED
 *  - is not DISCOVERABLE
 *  - is not INITIATING a connection
 *  - is not BROADCATING
 *  - is not ADVERTISING
 *
 * @param desiredRole: The desired new GAP role.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the role switch have succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the role switch have failed.
 *   It may be caused by:
 *      - The current role is the same that the desired role.
 *      - The Current mode is either CONNECTED, DISCOVERABLE,
 *        INITIATING a connection, BROADCATING or ADVERTISING.
 *      - Because the device was not previously registered for an other role
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter, it may be du becausethe given role is not 
 *		supported according to the current configuration.
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_SwitchRole( BleGapRole desiredRole );

#if	(BLE_ROLE_ADVERTISER == 1)
/** Set the current device accessibility mode
 *
 * BLEGAP_SetMode():
 *	this function is used to set for the local device the specified 
 * accessibility (connectivity and discoverabilty) mode.
 * When using this interface, the advertisement interval using when 
 * discoverable and/or connectable is set to the Bluetooth specification
 * recommended value TGAP_ADV_INTERVAL = 0x800 = 1.28s
 *
 * BLE_ROLE_ADVERTISER shall be enabled.
 *
 * @param mode: The bitfield of available BleGapMode
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the local device was already in the
 *  requested mode, thus no changes will be made and BLEGAP_EVENT_MODECHANGE  
 *  events will be NOT received in the callback.
 *
 *	- BLESTATUS_PENDING indicates that the local device has successfully
 *  started the procedure, BLEGAP_EVENT_MODECHANGE will be received in the
 * registered callback with the status of the operation.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du
 *	 because of invalid mode combinaison ( For example a broadcaster cannot
 *   be connectable) or because there is another operation in progress ( if 
 *	 a peripheral is broadcasting it cannot set mode)
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_SetMode( BleGapMode mode );
#define BLEGAP_SetMode( mode ) BLEGAP_SetModeWithIntervals( mode, TGAP_FAST_ADV_INT )

/** Set the current device accessibility mode
 *
 * BLEGAP_SetModeWithIntervals():
 *	this function is used to set for the local device the specified 
 * accessibility (connectivity and discoverabilty) mode and if discoverable 
 * and / or connectable with the given advertisement interval.
 *
 * BLE_ROLE_ADVERTISER shall be enabled.
 *
 * @param mode: The bitfield of available BleGapMode
 * @param advertisementInterval : The discoverable/connectable interval
 *		to be used if the mode is set to discoverable and/or connectable
 *		the advertisementInterval value is ignored otherwise.
 *		The value represents the number of 0.625 ms slots.
 *		The possible value is:
 *		 Range: 0x0020 to 0x4000 (recommended is 0x800)
 *		 Range: 20ms to 10.24 s (recommended is 60 ms)
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the local device has successfully
 *  started the procedure, BLEGAP_EVENT_MODECHANGE will be received in the
 * registered callback with the status of the operation.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du
 *	 because of invalid mode combinaison ( For example a broadcaster cannot
 *   be connectable) or because there is another operation in progress ( if 
 *	 a peripheral is broadcasting it cannot set mode)
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_SetModeWithIntervals( BleGapMode mode, 
									  U16 advertisementInterval);

/** Retreiving the current device accessibility mode
 *
 * BLEGAP_GetMode():
 *	this function is used to retreiving the current Bluetooth Low Energy
 * device accessibility (connectivity and discoverabilty) mode.
 *
 * BLE_ROLE_ADVERTISER shall be enabled.
 *
 * @return The current mode of the local device.
 *	
 * @author Alexandre GIMARD
 */
BleGapMode BLEGAP_GetMode( void );
#endif	//(BLE_ROLE_ADVERTISER == 1)

#if (BLE_CONNECTION_SUPPORT == 1)
#if	(BLE_ROLE_SCANNER == 1)
/** Create a connection with the remote device identified with the given
 * address and address type using the specified interval
 *
 * BLEGAP_ConnectWithIntervalsEx():
 *	this function is used to create a Bluetooth Low Energy connection
 * with the remote device identified with the given address and address type.
 * It uses the given scan Interval for scanning the remote device to connect
 * and the connection interval once the connection is created.
 *
 * BLE_CONNECTION_SUPPORT shall be enabled.
 * BLE_ROLE_SCANNER shall be enabled.
 *
 * @param addr: A valid pointer to the Bluetooth device address of the
 *			remote device
 * @param addrType: The Bluetooth device address type (PUBLIC or PRIVATE)
 *			of the remote device.
 * @param scanInterval: The desired scanning interval to scan the device 
 *			to connect. scannning Interval = scanInterval * 0.625 ms
 *				scannning Interval range: 2.5 ms to 10.24 s
 *				scanInterval range: 0x0004 to 0x4000
 * @param scanWindow: The desired scanning window to scan the device 
 *			to connect. scannning Window = scanWindow * 0.625 ms
 *				scannning Window range: 2.5 ms to 10.24 s
 *				scanWindow range: 0x0004 to 0x4000
 *		Note that the scanning Window shall be lesser or equal of the scanning
 *		interval.
 *		The device will scan during the scan Window every scan interval, so if
 *		the both intervals are equals, the device will scan continuously.
 * @param connectionInterval: The desired connection interval to use when
 *			devices are connected.
 *			Connection Interval = connectionInterval  * 1.25 ms
 *			connectionInterval range: 0x0006 to 0x0C80
 * @param connectionTimeout: The desired connection timeout to use when
 *			devices are connected and will detect the disconnection
 *			Connection Timeout = connectionTimeout  * 10 ms
 *          connectionTimeout range: 0x000A to 0x0C80
 *          recommended value is TGAP_CONN_SUPERV_TIMEOUT
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation have successfully
 *  started, BLEGAP_EVENT_CONNECTED will be received in the
 * registered callback with the status of the operation.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. 
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_ConnectWithIntervalsEx( BD_ADDR *addr, 
							  BleAddressType addrType,
							  U16 scanInterval,
							  U16 scanWindow,
							  U16 connectionInterval,
                              U16 connectionTimeout);

/** Create a connection with the remote device identified with the given
 * address and address type using the specified interval
 *
 * BLEGAP_ConnectWithIntervals():
 *	this function is used to create a Bluetooth Low Energy connection
 * with the remote device identified with the given address and address type.
 * It uses the given scan Interval for scanning the remote device to connect
 * and the connection interval once the connection is created.
 *
 * BLE_CONNECTION_SUPPORT shall be enabled.
 * BLE_ROLE_SCANNER shall be enabled.
 *
 * @param addr: A valid pointer to the Bluetooth device address of the
 *			remote device
 * @param addrType: The Bluetooth device address type (PUBLIC or PRIVATE)
 *			of the remote device.
 * @param scanInterval: The desired scanning interval to scan the device 
 *			to connect. scannning Interval = scanInterval * 0.625 ms
 *				scannning Interval range: 2.5 ms to 10.24 s
 *				scanInterval range: 0x0004 to 0x4000
 * @param scanWindow: The desired scanning window to scan the device 
 *			to connect. scannning Window = scanWindow * 0.625 ms
 *				scannning Window range: 2.5 ms to 10.24 s
 *				scanWindow range: 0x0004 to 0x4000
 *		Note that the scanning Window shall be lesser or equal of the scanning
 *		interval.
 *		The device will scan during the scan Window every scan interval, so if
 *		the both intervals are equals, the device will scan continuously.
 * @param connectionInterval: The desired connection interval to use when
 *			devices are connected.
 *			Connection Interval = connectionInterval  * 1.25 ms
 *			connectionInterval range: 0x0006 to 0x0C80
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation have successfully
 *  started, BLEGAP_EVENT_CONNECTED will be received in the
 * registered callback with the status of the operation.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. 
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_ConnectWithIntervals( BD_ADDR *addr, 
							  BleAddressType addrType,
							  U16 scanInterval,
							  U16 scanWindow,
							  U16 connectionInterval);
#define BLEGAP_ConnectWithIntervals(addr, addrType, scanInterval, scanWindow, connectionInterval) BLEGAP_ConnectWithIntervalsEx(addr, addrType, scanInterval, scanWindow, connectionInterval,TGAP_CONN_SUPERV_TIMEOUT)

/** Create a connection with the remote device identified with the given
 * address and address type
 *
 * BLEGAP_Connect():
 *	this function is used to create a Bluetooth Low Energy connection
 * with the remote device identified with the given address and address type.
 * Note that this function is a MACRO redefinition of BLEGAP_ConnectWithIntervals   
 * using the following value for intervals and windows:
 *	TGAP(fast_scan_int) = 30 ms as scan interval
 *	TGAP(fast_scan_wind) = 30 ms as scan window
 *  TGAP(initial_conn_interval) = 50 ms as connection interval
 *
 * BLE_CONNECTION_SUPPORT shall be enabled.
 * BLE_ROLE_SCANNER shall be enabled.
 *
 * @param addr: A valid pointer to the Bluetooth device address of the
 *			remote device
 * @param addrType: The Bluetooth device address type (PUBLIC or PRIVATE)
 *			of the remote device
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation have successfully
 *  started, BLEGAP_EVENT_CONNECTED will be received in the
 * registered callback with the status of the operation.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. 
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_Connect( BD_ADDR *addr, BleAddressType addrType);
#define BLEGAP_Connect(addr, addrType) BLEGAP_ConnectWithIntervalsEx(addr, addrType, TGAP_FAST_SCAN_INT, TGAP_FAST_SCAN_WIND, TGAP_INITIAL_CONN_INTERVAL_MIN,TGAP_CONN_SUPERV_TIMEOUT)

/** Cancel the connection creation
 *
 * BLEGAP_CancelCreateConnection():
 *	this function is used to cancel any Bluetooth Low Energy connection
 * creation in progress.
 *
 * BLE_CONNECTION_SUPPORT shall be enabled.
 * BLE_ROLE_SCANNER shall be enabled.
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation have successfully
 *  started, BLEGAP_EVENT_CONNECTIONCANCELED will be received in the
 * registered callback with the status of the operation.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du 
 *  because no connection creation are in progress 
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_CancelCreateConnection( void );

#endif //(BLE_ROLE_SCANNER == 1)

/** Terminate an ongoing connection.
 *
 * BLEGAP_Disconnect():
 *	this function is used to terminate an ongoing Bluetooth Low Energy 
 * connection with the specified connected remote device.
 *
 * BLE_CONNECTION_SUPPORT shall be enabled.
 *
 *  connHandle: The Local identifier of the Link layer connection that 
 *		is intended to be disconnected. The connection handle was previously
 *		reported in the BLEGAP_EVENT_CONNECTED event.
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation have successfully
 *  started, BLEGAP_EVENT_DISCONNECT will be received in the
 * registered callback with the status of the operation.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du 
 *  because no connection are ongoing with the remote address.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_Disconnect( U16 connHandle );

/** Try to update the connection interval
 *
 * BLEGAP_UpdateConnectionIntervalEx():
 *	This function is used to try to update the current connection interval 
 *  parameter. 
 *  It is usefull to decrease the connection interval when the
 *	application wants to exchange a lot of data ( pairing, profile LinkUp) but
 *  it consumes more power so it is usefull to increase the connection 
 *  interval to save power.
 *
 * BLE_CONNECTION_SUPPORT shall be enabled.
 *
 * @param connHandle: The Local identifier of the Link layer connection that 
 *		is intended to be disconnected. The connection handle was previously
 *		reported in the BLEGAP_EVENT_CONNECTED event.
 * @param connIntervalMin Defines minimum value for the connection 
 * 		interval in the following manner: 
 * 		Minimum Connection Interval = connIntervalmin  * 1.25 ms
 * 		connIntervalMin range: 0x0006 to 0x0C80
 * 		Values outside the range are reserved.
 *
 * @param connIntervalMax:  Defines maximum value for the connection 
 * 		interval in the following manner: 
 * 		Maximum Connection Interval = connIntervalMax * 1.25 ms
 * 		connIntervalMax range: 0x0006 to 0x0C80
 * 		Values outside the range are reserved.
 *
 * @param connLatency:  Slave latency for the connection in number of 
 *		connection events.
 * 		Defines the slave latency parameter in the following manner:
 * 		connection Latency = connLatency (as number of LL connection events).
 * 		connLatency range: 0x0000 to 0x01F3 [E3879] 
 * 		Values outside the range are reserved.
 *
 * @param connTimeout: Defines connection timeout parameter in the 
 *		following manner: 
 * 		connection Timeout = connTimeout * 10 ms
 * 		connTimeout range: 0x000A to 0x0C80
 * 		Values outside the range are reserved.
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation have successfully
 *		started, BLEGAP_EVENT_UPDATECONNECTION_RSP will be received in the
 *		registered callback with the status of the operation.
 *		Then, if succeed, BLEGAP_EVENT_CONNECTIONUPDATED will be received in 
 *		the callback with the new connection information.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du 
 *  because no connection are ongoing with the remote address.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_UpdateConnectionIntervalEx( U16 connHandle,
											U16 connectionIntervalMin,
											U16 connectionIntervalMax,
											U16 connLatency,
											U16 connTimeout);

/** Try to update the connection interval
 *
 * BLEGAP_UpdateConnectionInterval():
 *	This function is used to try to update the current connection interval 
 *  parameter. 
 *  It is usefull to decrease the connection interval when the
 *	application wants to exchange a lot of data ( pairing, profile LinkUp) but
 *  it consumes more power so it is usefull to increase the connection 
 *  interval to save power.
 * Note that this API is a macro redefinition of the 
 *  BLEGAP_UpdateConnectionIntervalEx using the given interval as minimum an
 *  maximum connection interval and GAP recommended values for Connection
 *  Latency and Connection supervision Timeout.
 *
 * BLE_CONNECTION_SUPPORT shall be enabled.
 *
 * @param connHandle: The Local identifier of the Link layer connection that 
 *		is intended to be disconnected. The connection handle was previously
 *		reported in the BLEGAP_EVENT_CONNECTED event.
 * @param connectionInterval: The desired connection interval to apply
 *			Connection Interval = connectionInterval  * 1.25 ms
 *			connectionInterval range: 0x0006 to 0x0C80
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation have successfully
 *		started, BLEGAP_EVENT_UPDATECONNECTION_RSP will be received in the
 *		registered callback with the status of the operation.
 *		Then, if succeed, BLEGAP_EVENT_CONNECTIONUPDATED will be received in 
 *		the callback with the new connection information.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du 
 *  because no connection are ongoing with the remote address.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_UpdateConnectionInterval( U16 connHandle, 
										  U16 connectionInterval );
#define BLEGAP_UpdateConnectionInterval( connHandle, interval ) BLEGAP_UpdateConnectionIntervalEx(connHandle, interval, interval, TGAP_CONN_LATENCY, TGAP_CONN_SUPERV_TIMEOUT);


#if (BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME == 1)
/** retrieve the remote Bluetooth Device Name
 *
 * BLEGAP_GetRemoteDeviceName():
 *	this function is used to retreive the remote Bluetooth Low Energy  
 * device friendly Name
 *
 * BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME shall be enabled.
 * BLE_CONNECTION_SUPPORT shall be enabled.
 *
 * @param connHandle: The Local identifier of the Link layer connection for 
 *		which the name is intended to be retrieved. The connection handle was 
 *		previously reported in the BLEGAP_EVENT_CONNECTED event.
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation successfully
 *  started, BLEGAP_EVENT_NAMEINFORMATION will be received in the callback
 * with the status of the operation and the name information.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du 
 * because the remote device does not support BLE GAP device name 
 * characteristic, or it does not exist any connection with the given
 * Bluetooth device
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_GetRemoteDeviceName( U16 connHandle );
#endif //(BLEGAP_SUPPORT_RETRIEVING_REMOTE_NAME == 1)

/** Get the current connection Handle for the connection with the given device
 *
 * BLEGAP_GetConnectionHandle()
 *	This function is used to retrieve the current connection handle that
 *  identifies the connection between the local device and the remote device
 *  with the given address.
 *
 * BLE_CONNECTION_SUPPORT shall be enabled.
 *
 * @param addr the device address that identify the connection to
 *	retrieve information
 * @param connHandle the connHandle pointer that
 *	will be filled with the connection handle information upon success
 *
 * @return The status of the operation:
 * 	- BLESTATUS_SUCCESS indicates that the connection have been found, the  
 *		connHandle have been filled with the know connection handle information
 *	- BLESTATUS_FAILED indicates that the operation has failed
 *		connHandle have not been filled with any connection handle information.
 *		It may be because no connection are ongoing with the given device.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_GetConnectionHandle( BD_ADDR *addr, U16 *connHandle);

/** Get the connected devices 
 *
 * BLEGAP_GetNumberOfActiveConnections()
 *	This function can be called by the application in order to retrieve the 
 *	number of active connections as well that the active connection handles
 *  of the connected devices.
 *
 * THIS FUNCTION IS USED INTERNALY ONLY BY THE STACK DO NOT USE 
 *
 * @param connHandle: A pointer to an array that will contains the 
 *  list of the connected handles.
 *  May be null if the calling layer is only interested to the connection 
 *  count.
 *  Otherwise the size of the array must be of BLE_NUM_MAX_CONNECTION in
 *   order to be able to cantains all the active connections
 *
 * @return The number of currently connected devices (from 0 to 
 *  BLE_NUM_MAX_CONNECTION )
 *
 * @author Alexandre GIMARD
 */
U8 BLEGAP_GetNumberOfActiveConnections( U16 *connHandles );

/** Get the device address from a connection Handle
 *
 * BLEGAP_GetBDAddr()
 *	This function is used to retrieve the current device address that is
 *  identified by the connection handle.
 *
 * BLE_CONNECTION_SUPPORT shall be enabled.
 *
 * @param connHandle the connection handle that identify the connection to
 *	retrieve information
 * @param addr the BD_ADDR pointer that
 *	will be filled with the Device address information upon success
 *
 * @return The status of the operation:
 * 	- BLESTATUS_SUCCESS indicates that the connection have been found, the  
 *		connHandle have been filled with the know connection handle information
 *	- BLESTATUS_FAILED indicates that the operation has failed
 *		connHandle have not been filled with any connection handle information.
 *		It may be because no connection are ongoing with the given device.
 *
 * @author Arnaud DROUERE (based on BLEGAP_GetConnectionHandle function)
 */
BleStatus BLEGAP_GetBDAddr (U16 connHandle, BD_ADDR *addr);
#endif //(BLE_CONNECTION_SUPPORT == 1)

#if	(BLE_ROLE_SCANNER == 1)

/** Start an INQUIRY procedure.
 *
 * BLEGAP_StartInquiry():
 *	this function is used to start a Bluetooth Low Energy INQUIRY procedure
 * in order to found Bluetooth Low Energy devices in range
 *
 * BLE_ROLE_SCANNER shall be enabled.
 *
 * @param mode: The desired inquiry mode (LIMITED or GENERAL)
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation have successfully
 *  started, BLEGAP_EVENT_INQUIRYRESULT will be received in the callback
 * when devices are found and BLEGAP_EVENT_INQUIRYCOMPLETE will be received in
 * the callback when the INQUIRY procedure are complete.
 * BLEGAP_EVENT_INQUIRYRESULT_EXT may be also received after 
 * BLEGAP_EVENT_INQUIRYRESULT in case of extended information are received.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du 
 *  because an other inquiry is in progress.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter, it may be du because the mode is neither LIMITED nor
 *		GENERAL
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_StartInquiry( BleInquiryMode mode );

/** Cancel the ongoing INQUIRY procedure
 *
 * BLEGAP_CancelInquiry():
 *	this function is used to cancel any Bluetooth Low Energy INQUIRY
 * procedure in progress.
 *
 * BLE_ROLE_SCANNER shall be enabled.
 *
 * @return The status of the operation
 * - BLESTATUS_PENDING indicates that the operation have successfully
 *  started, BLEGAP_EVENT_INQUIRYCOMPLETE will be received in the
 * registered callback with the status of the operation.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du
 * because there is no inquiry operation in progress.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_CancelInquiry( void );

/** Parse an AD data field for the specific field
 *
 * This function is used to parse an AD Data field received during inquiry or 
 * Observation procedure.
 * The AD data field may contains Device specific information like Device 
 * Name, Service Supported or Appearance.
 * This function is used to retrieve the specific AD data Type value and 
 * value length.
 * This function cann be called during BLEGAP_EVENT_INQUIRYRESULT or
 *	BLEGAP_EVENT_INQUIRYRESULT_EXT with the BleGapInquiryResult *result adData
 * Field.
 *
 * BLE_ROLE_SCANNER shall be enabled.
 *
 * @param adData:[in] The AD data to parse, mainly returned during Inquiry as  
 *		adData field of BleGapInquiryResult.
 *
 * @param adDataLen:[in] The length of the AD data to parse, mainly returned  
 *		 during Inquiry as adDataLen field of BleGapInquiryResult
 *
 * @param blockType:[in] The type of AD Data the application is interested
 *		 to retrieve in the AD data to parse.
 *
 * @param block:[in-out] A pointer that will receive, in case of success 
 *		( function return value is BLESTATUS_SUCCESS), a pointer to the 
 *		adData value.
 *
 * @param blockLen:[in-out] A pointer that will receive, in case of success 
 *		( function return value is BLESTATUS_SUCCESS),the adData
 *		 value length.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation has successfully
 *    completed and the BlockType field is present in the given "adData".
 *	  The "block" pointer points the value of the AD Data blockType.
 *	  The "blockLen" contains the value of the AD Data blockType.
 *
 *	- BLESTATUS_FAILED indicates that the operation has successfully
 *    completed and the BlockType field is not present in the given 
 *    "adData".
 *
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_GetADDataBlock( U8 * adData, 
								U8 adDataLen, 
								BleGapAdDataType blockType,
								U8 **block, 
								U8 *blockLen);

/** Parse an AD data to get the included AD data types 
 *
 * BLEGAP_GetADDataTypes():
 *	This function is used to parse an AD Data field received during inquiry or 
 * Observation procedure in order to retrieve which AD data types are included
 * in this field.
 *
 * BLE_ROLE_SCANNER shall be enabled.
 *
 * @param adData:[in] The AD data to parse, mainly returned during Inquiry as  
 *		adData field of BleGapInquiryResult.
 *
 * @param adData:[in] The length of the AD data to parse, mainly returned  
 *		 during Inquiry as adDataLen field of BleGapInquiryResult
 *
 * @param adDataBitField:[in-out] A pointer that will receive, in case of success 
 *		( function return value is BLESTATUS_SUCCESS),the bitfields describing
 *		the AD data types are included in the given "adData".
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation has successfully
 *    completed and the adDataBitField contains the Ad data type appearing 
 *    in the given "adData".
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed to complete
 *     and the adDataBitField is null and not applicable.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_GetADDataTypes( U8 * adData, 
								U8 adDataLen,
								BleGapAdDataBitField *adDataBitField);

/** Start an OBSERVATION procedure.
 *
 * BLEGAP_StartObserveProcedureWithIntervals():
 *	this function is used to start a Bluetooth Low Energy OBSERVATION 
 * procedure in order to get broadcasted data by other Bluetooth Low
 * Energy devices in range. It use the given scan interval and window
 *
 * BLE_ROLE_SCANNER shall be enabled.
 *
 * @param duplicateFiltering : determine if the local Observer should enable 
 *                  (set to BLEGAPDUPLICATEFILTERING_ON) or disable (set to 
 *                  BLEGAPDUPLICATEFILTERING_OFF) duplicate filtering 
 *                  capability.
 *                  Enabling duplicate filtering means that the Application
 *                  will receive only once a broadcast packet if there is no 
 *                  change in its data. Disabling duplicate filtering means 
 *                  that the broadcast packet will be receive every broadcast 
 *                  interval.
 * @param scanInterval: The desired scanning interval to scan the devices 
 *			when observe. scannning Interval = scanInterval * 0.625 ms
 *				scannning Interval range: 2.5 ms to 10.24 s
 *				scanInterval range: 0x0004 to 0x4000
 * @param scanWindow: The desired scanning window to scan the devices 
 *			when observe. scannning Window = scanWindow * 0.625 ms
 *				scannning Window range: 2.5 ms to 10.24 s
 *				scanWindow range: 0x0004 to 0x4000
 *		Note that the scanning Window shall be lesser or equal of the scanning
 *		interval.
 *		The device will scan during the scan Window every scan interval, so if
 *		the both intervals are equals, the device will scan continuously.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation successfully
 *  started, BLEGAP_EVENT_OBSERVEMODE_DATA will be received in the callback
 * when the local device have observed some data.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du 
 *  because the role does not match with this procedure.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_StartObserveProcedureWithIntervals( 
                      BleGapDuplicateFilteringMode duplicateFiltering,
                      U16 scanInterval,
                      U16 scanWindow);

/** Start an OBSERVATION procedure.
 *
 * BLEGAP_StartObserveProcedure():
 *	this function is used to start a Bluetooth Low Energy OBSERVATION 
 * procedure in order to get broadcasted data by other Bluetooth Low
 * Energy devices in range.
 * Note that this function is a MACRO redefinition of 
 * BLEGAP_StartObserveProcedureWithIntervals using the following value for 
 * intervals and windows:
 *	TGAP(fast_scan_int) = 30 ms as scan interval
 *	TGAP(fast_scan_wind) = 30 ms as scan window
 *
 * BLE_ROLE_SCANNER shall be enabled.
 *
 * @param duplicateFiltering : determine if the local Observer should enable 
 *                  (set to BLEGAPDUPLICATEFILTERING_ON) or disable (set to 
 *                  BLEGAPDUPLICATEFILTERING_OFF) duplicate filtering 
 *                  capability.
 *                  Enabling duplicate filtering means that the Application
 *                  will receive only once a broadcast packet if there is no 
 *                  change in its data. Disabling duplicate filtering means 
 *                  that the broadcast packet will be receive every broadcast 
 *                  interval.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation successfully
 *  started, BLEGAP_EVENT_OBSERVEMODE_DATA will be received in the callback
 * when the local device have observed some data. The BLEGAP_GetADDataBlock()
 *  API may be used to parse the received data.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du 
 *  because the role does not match with this procedure.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_StartObserveProcedure( 
			    BleGapDuplicateFilteringMode duplicateFiltering );
#define BLEGAP_StartObserveProcedure(duplicateFiltering) BLEGAP_StartObserveProcedureWithIntervals(duplicateFiltering, TGAP_FAST_SCAN_INT, TGAP_FAST_SCAN_WIND)

/** Stop an OBSERVATION procedure.
 *
 * BLEGAP_StopObserveProcedure():
 *	this function is used to stop a Bluetooth Low Energy OBSERVATION 
 * procedure previously started by calling the BLEGAP_StartObserveProcedure()
 * or BLEGAP_StartObserveProcedureWithIntervals() APIs.
 *
 * BLE_ROLE_SCANNER shall be enabled.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the Observation procedure has 
 * succesfully stopped
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du 
 *  because there is no OBSERVATION procedure in progress.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_StopObserveProcedure( void );

#endif	//(BLE_ROLE_SCANNER == 1)


#if	(BLE_ROLE_ADVERTISER == 1)

/** Start an BROADCAST procedure.
 *
 * BLEGAP_StartBroadcastProcedure():
 *	this function is used to start a Bluetooth Low Energy BROADCAST 
 * procedure in order to broadcast data to other Bluetooth Low
 * Energy devices in range.
 *
 * BLE_ROLE_ADVERTISER shall be enabled.
 *
 * @param broadcastInterval: the desired interval between two broadcast
 *	packet in Radio slot. One radio slot is 0.625 millisecond
 *  Allowed range for broadcastInterval is 0x00A0 ( 100 ms) to 
 *  0x4000 ( 10240 ms).
 *  A moderate value should be 0x0800 ( 1.28 seconds)
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation successfully
 *  started, BLEGAP_EVENT_STARTBROADCAST_RSP will be received in the callback
 * with the status of the operation.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du 
 *  because the role does not match with this procedure.
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_StartBroadcastProcedure( U16 broadcastInterval );

/** Stop an BROADCAST procedure.
 *
 * BLEGAP_StopBroadcastProcedure():
 *	this function is used to stop a Bluetooth Low Energy BROADCAST 
 * procedure.
 *
 * BLE_ROLE_ADVERTISER shall be enabled.
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation successfully
 *  started, BLEGAP_EVENT_STOPBROADCAST_RSP will be received in the callback
 * with the status of the operation.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du 
 *  because no BROADCAST procedure is in progress
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_StopBroadcastProcedure( void );


/** Set the value to broadcast during the BROADCAST procedure.
 *
 * BLEGAP_SetBroadcastValue():
 *	this function is used to set the value to broadcast during the ongoing
 * Bluetooth Low Energy BROADCAST  procedure.
 *
 * BLE_ROLE_ADVERTISER shall be enabled.
 *
 * @param UUID: The 16-bits UUID for which the data is broadcasted.
 * @param value: A valid pointer to the value to broadcast.
 *		This pointer shall stay valid during all the BROADCAST procedure. 
 * @param len: The length of the value in the value pointer.The length shall
 *		be less or equal to 24 bytes.
 *
 * @return The status of the operation:
 *	- BLESTATUS_PENDING indicates that the operation successfully
 *  started, BLEGAP_EVENT_SETBROADCASTVALUE_RSP will be received in the 
 * callback with the status of the operation.
 *
 *	- BLESTATUS_FAILED indicates that the operation failed. It may be du 
 *  because another GAP operation is in progress or setting broadcast data
 *  is not allowed. 
 *
 *	if BLE_PARMS_CHECK is set to 1:
 *	- BLESTATUS_INVALID_PARMS indicates that the function failed because
 *		an invalid parameter (len is greater than 24).
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_SetBroadcastValue(U16 UUID, U8 *value, U8 len);

#endif	//(BLE_ROLE_ADVERTISER == 1)

#if	( (BLE_ROLE_ADVERTISER == 1) || ((BLE_CONNECTION_SUPPORT == 1) && (ATT_ROLE_SERVER == 1)) )
/** Set the local Bluetooth Device Appearance.
 *
 * BLEGAP_SetLocalAppearance():
 *	this function is used to set the local Bluetooth Low Energy Device
 * Appearance, this appearance will be populated when discoverable if the 
 * device is a peripheral or maybe read by a remote device when connected.
 * Setting the Appearance improve user experience
 *
 * Either BLE_ROLE_ADVERTISER shall be enabled or the device shall support
 * connection ( BLE_CONNECTION_SUPPORT set to 1)
 *
 * @param appearance: the appearance to set for the local device,
 *			the bluetooth specification defined value are defined by the
 *			BleGapAppearance type (BLEGAPAPPEARENCE_*)
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the name has been successfully set
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_SetLocalAppearance( BleGapAppearance appearance );

/** Set the local Bluetooth Device Name.
 *
 * BLEGAP_SetLocalBluetoothDeviceName():
 *	this function is used to set the local Bluetooth Low Energy Device
 * friendly name.
*
 * Either BLE_ROLE_ADVERTISER shall be enabled or the device shall support
 * connection ( BLE_CONNECTION_SUPPORT set to 1)
 *
 * @param name: A valid pointer to the Bluetooth device friendly name. 
 *		This pointer shall stay valid during all the name validity. before 
 *		freeing the name pointer the BLEGAP_SetLocalBluetoothDeviceName shall
 *		be call with a nameLen set to zero.
 * @param nameLen: The current length of the Bluetooth device friendly name 
 *		in the name pointer.
 * @param isWritable: define if the name can be write by a remote device. if
 *      set to zero (0) then the name is only readable by a remote device,
 *      if set to one (1), then a paired remote device (with or without 
 *      authentication) may write the local name.
 * @param maximumWritableLen: In case of the remote device is allowed to write
 *      the local name, defines the maximum len that can be written into the 
 *      name pointer by the remote device. This parameter is ignored if the 
 *      isWritable parameter is defined to 0;
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the name has been successfully set
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEGAP_SetLocalBluetoothDeviceNameEx(U8 *name, 
                        U8 nameLen, 
                        U8 isWritable, 
                        U16 maximumWritableLen);
                        
#define BLEGAP_SetLocalBluetoothDeviceName(name, nameLen) BLEGAP_SetLocalBluetoothDeviceNameEx(name, nameLen, 0, 0)

#endif	//( (BLE_ROLE_ADVERTISER == 1) || ((BLE_CONNECTION_SUPPORT == 1) && (ATT_ROLE_SERVER == 1)) )



#endif /*__BLEGAP_H*/
