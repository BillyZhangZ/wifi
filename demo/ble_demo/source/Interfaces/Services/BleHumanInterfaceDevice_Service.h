#ifndef __BLEHUMANINTERFACEDEVICE_SERVICE_H
#define __BLEHUMANINTERFACEDEVICE_SERVICE_H
/****************************************************************************
 *
 * File:          BleHumanInterfaceDevice_Service.h
 *
 * Description:   Contains routines declaration for BleHumanInterfaceDevice 
 *				Service.
 * 
 * Created:       October, 2011
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

#include "BleTypes.h"
#include "ATT/att.h"

/***************************************************************************\
 *	Type definition
\***************************************************************************/


/***************************************************************************\
 * OPTIONAL FEATURES
\***************************************************************************/
/**
 * BLE_SUPPORT_HID_SERVICE
*	Defines if the BLE local device enables access to the local  
 *	Human  Interface Device service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles to get access to the  
 * Human Device Interface Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_HID_SERVICE
#define BLE_SUPPORT_HID_SERVICE						0
#endif //BLE_SUPPORT_HID_SERVICE

#if (BLE_SUPPORT_HID_SERVICE == 1)

/**
 * BLE_HIDSERVICE_SUPPORT_BOOTPROTOCOLMODE
 *	Defines if the BLE local HID Service anable the support of the BOOT
 * protocol Mode.
 *  Supporting BOOT protocol Mode enables remote HID Host to change at 
 * runtime the current protocol mode from BOOT mode to REPORT mode (or vice-
 * versa). Also, it enable adding BOOT report types like BOOT MOUSE INPUT or
 * BOOT KEYBOARD INPUT or OUTPUT report.
 * Note that this service  implementation only event received in the register
 * callback is to notify from the PROTOCOL MODE change HID host request.
 * If BLE_HIDSERVICE_SUPPORT_BOOTPROTOCOLMODE is disabled (0) the callback is
 * ignored and can be set to (BleHIDServiceCallBack)0 during Service 
 * registration
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_HIDSERVICE_SUPPORT_BOOTPROTOCOLMODE
#define BLE_HIDSERVICE_SUPPORT_BOOTPROTOCOLMODE						0
#endif //BLE_HIDSERVICE_SUPPORT_BOOTPROTOCOLMODE

/***************************************************************************\
 * CONSTANTS
\***************************************************************************/



/** 
 * MAX_REPORT_LEN constant
 * Defines the max size of the report
 */
#define MAX_REPORT_LEN									23

/** 
 * MAX_REPORT_MAP_LEN constant
 * Defines the max size of the report map
 * 
 */
#define MAX_REPORT_MAP_LEN								23

/** 
 * MAX_BOOT_REPORT_LEN constant
 * Defines the boot report len
 * 
 */
#define MAX_BOOT_REPORT_LEN								23

/** 
 * BleHIDServiceEvent type
 * Define the different kind of events that could be received by the 
 * BleHIDServiceCallBack
 */
#define HID_INFORMATION_VALUE_LEN						4


/***************************************************************************\
 *	Type definition
\***************************************************************************/
/** 
 * BleHIDServiceEvent type
 * Define the different kind of events that could be received by the 
 * BleHIDServiceCallBack
 */
typedef U8 BleHIDServiceEvent;

/** BLEHIDSERVICE_EVENT_PROTOCOL_MODE_CHANGE
 * Event received when a remote host request to change the protocol mode
 * Note that this event is only received when 
 * BLE_HIDSERVICE_SUPPORT_BOOTPROTOCOLMODE is enabled.
 * The status field is always BLESTATUS_SUCCESS
 * The parms fields is a pointer to the report value within a 
 * BleHIDProtocolValueInformation pointer.
 */
#define BLEHIDSERVICE_EVENT_PROTOCOL_MODE_CHANGE					0xC2

/** BLEHIDSERVICE_EVENT_COMMAND_RECEIVED
 * Event received when a remote host have send a command to this HID device.
 * The status field is always BLESTATUS_SUCCESS
 * The parms fields is a pointer to the report value within a 
 * BleHIDCommandValueInformation pointer.
 */
#define BLEHIDSERVICE_EVENT_COMMAND_RECEIVED						0xC3

/** BLEHIDSERVICE_EVENT_REPORT_RECEIVED
 * Event received when a remote host have written a local output report
 * of this HID device.
 * The status field is always BLESTATUS_SUCCESS
 * The parms fields is a pointer to the report value within a 
 * BleHIDServiceReportValueInformation pointer.
 */
#define BLEHIDSERVICE_EVENT_REPORT_RECEIVED							0xC4

/**
 * BleHIDServiceCallBack
 *	This callback receives the HID DEVICE Service events. 
 *  Each events may be assiociated with specified status and parameters.
 *  The callback is executed during the stack context,  be careful 
 *	 not to do heavy process in this function.
 */
typedef void (*BleHIDServiceCallBack)(
					BleHIDServiceEvent event,
					BleStatus status,
					void *parms); 


/** 
 * BleHIDProtocolMode type
 * Enumerate the possible value for the PROTOCOL MODE characteristic value
 * This type is used when receiving a Protocol Mode Change request event.
 */
typedef U8 BleHIDServiceProtocolMode;
// The protocol mode is the BOOT protocol mode.
#define BLEHIDSERVICE_PROTOCOLMODE_BOOT							0x00
// The protocol mode is the REPORT protocol mode.
#define BLEHIDSERVICE_PROTOCOLMODE_REPORT						0x01

/** 
 * BleHIDServiceCommandId type
 * Enumerate the possible value for the COMMAND available to be received 
 * during a BLEHIDSERVICE_EVENT_COMMAND_RECEIVED event.
 */
typedef U8 BleHIDServiceCommandId;
// Informs HID Device that HID Host is entering the Suspend State as 
// defined in (§7.4.2, Bluetooth HID Profile Specification) 
#define BLEHIDSERVICE_COMMANDID_SUSPEND							0x00
//Informs HID Device that HID Host is exiting the Suspend State as defined 
//in (§7.4.2, Bluetooth HID Profile Specification ) 
#define BLEHIDSERVICE_COMMANDID_EXITSUSPEND						0x01



/** 
 * BleHIDServiceReportType type
 * It defines the possible value for a report type.
 * This type is used when registering a report to give the report Type.
 */
typedef U8	BleHIDServiceReportType;
#define BLEHIDSERVICE_REPORTTYPE_REPORTINPUT					0x01
#define BLEHIDSERVICE_REPORTTYPE_REPORTOUTPUT					0x02
#define BLEHIDSERVICE_REPORTTYPE_REPORTFEATURE					0x03
#if (BLE_HIDSERVICE_SUPPORT_BOOTPROTOCOLMODE == 1)
#define BLEHIDSERVICE_REPORTTYPE_BOOTMOUSEINPUT					0x04
#define BLEHIDSERVICE_REPORTTYPE_BOOTKEYBOARDINPUT				0x05
#define BLEHIDSERVICE_REPORTTYPE_BOOTKEYBOARDOUTPUT				0x06
#endif //(BLE_HIDSERVICE_SUPPORT_BOOTPROTOCOLMODE == 1)

/** 
 * BleHIDServiceReportID type
 * Defines a report ID used when registering the report or set the
 * report value.
 */
typedef U8	BleHIDServiceReportID;

/** 
 * BleHIDServiceReport type
 * Defines a report structure used when registering the report.
 * In all the case the reportType field must be filled,
 * the report ID is filled if the report is a REPORT protocol mode one.
 *
 * The value and the valueLen is used when the report is writable (OUTPUT 
 * REPORT )by a remote device,
 * The valueBuffer field hosts the buffer in which the value will be 
 * written by the remote device.
 * The valueBufferLen hosts the exact number of bytes that must be written by 
 * the remote host in this output report (it must correspond to the report len).
 */
typedef struct {
	
	// The report type for this report it can be either
	// BOOT (MOUSE/KEYBOARD INPUT/OUTPUT) or REPORT(INPUT/OUTPUT/FEATURE)
	BleHIDServiceReportType				reportType;

	// In case of report Type is REPORT, the report ID,
	// This field is ignored in case of reportType is a BOOT REPORT
	BleHIDServiceReportID				reportID;

	// In case of report Type is an OUTPUT REPORT ( BOOT or REPORT), 
	// This field hosts the buffer in which the value will be 
	// written by the remote device.
	U8									*valueBuffer;

	// In case of report Type is an OUTPUT REPORT ( BOOT or REPORT), 
	// This field hosts the exact number of bytes that must be written by 
	// the remote host in this output report (it must correspond to the report
	// len).
	U16									valueBufferLen;

	/** 
	 *	The Following fields are for INTERNAL USE ONLY
	 * Do not address them directly
	 */
	Att16BitCharacteristicAttribute		reportCharacteristic;
	AttAttribute						reportAttribute;

	// If the report is an INPUT The Report Client Configuration attribute 
	// and the current client configuration for the active connection(s)
	AttInstantiatedAttribute			reportClientConfigAttribute;
    // The client configuration memory for the active connection(s)
    U8			                        reportClientConfigMemory[2*BLE_NUM_MAX_CONNECTION];

	AttAttribute						reportReferenceDescriptorAttribute;
	U8									reportReferenceDescriptorValue[2];

} BleHIDServiceReport;



/** 
 * BleHIDServiceFlags type
 * It is a bitfield used to define the possible value for the 
 * flag field used when setting the local HID device information
 */
typedef U8 BleHIDServiceFlags;

/** BLEHIDSERVICE_FLAG_HOSTWAKEUP
 * The device is not designed to be capable of providing wake-up signal to
 * a HID host
 */
#define BLEHIDSERVICE_FLAG_HOSTWAKEUP								0x01

/** BLEHIDSERVICE_FLAG_HOSTWAKEUP
 * The device is normally connectable
 */
#define BLEHIDSERVICE_FLAG_CONNECTABLE								0x02

/** 
 * BleHIDServiceDeviceInfo type
 * Defines the device Information structure used to set the local
 * HID Device Information during the call of 
 * BLEHIDSERVICE_SetDeviceInformation API. 
 */
typedef struct {

	// 16-bit unsigned integer representing version number of base
	// USB HID Specification implemented by HID Device
	U16										hidSpecification;

	// Identifies which country the hardware is localized for. 
	// Most hardware is not localized and thus this value would be zero (0).
	// The avilable country codes are referenced in the USB HID Specification
	U8										countryCode;

	// Flags representing the Remote device capabilities
	// like Host wakeUp or connectability
	BleHIDServiceFlags						flags;

} BleHIDServiceDeviceInfo;

/**
 * BleHIDService
 * The main BleHumanInterfaceDevice DEVICE  structure
 */
typedef struct {
	/*
	 * The BLE HID Service, Characteristic, Attribute and value
	 */
	AttServiceAttribute				hIDService; // WARNING THIS MEMBER MUST NOT BE MOVED AS IT IS ASSUMED AS FIRST ELEMENT OF THIS STRUCT

#if (BLE_HIDSERVICE_SUPPORT_BOOTPROTOCOLMODE == 1)
	Att16BitCharacteristicAttribute	protocolModeCharacteristic;
	AttAttribute					protocolModeAttribute;
	U8								protocolModeValue[1];
#endif //(BLE_HIDSERVICE_SUPPORT_BOOTPROTOCOLMODE == 1)

	Att16BitCharacteristicAttribute	reportMapCharacteristic;
	AttAttribute					reportMapAttribute;

	Att16BitCharacteristicAttribute	hIDInformationCharacteristic;
	AttAttribute					hIDInformationAttribute;
	U8								hIDInformationValue[HID_INFORMATION_VALUE_LEN];

	Att16BitCharacteristicAttribute	hIDControlPointCharacteristic;
	AttAttribute					hIDControlPointAttribute;
	U8								hIDControlPointValue[1];

	BleHIDServiceReport				*reports;
	U8								numberOfReports;
} BleHIDService;


/** 
 * BleHIDServiceReportValueInformation type
 * Defines a report structure used when the application receive the 
 * BLEHIDSERVICE_EVENT_REPORT_RECEIVED event.
 * It decribes the report that is written and the new report value with the concerned service.
 */
typedef struct {
	// The service that receive the protocol mode change
	BleHIDService					*service;

	// The report that receive the write notification
	BleHIDServiceReport					*report;

	// This field hosts the buffer in which the value has been 
	// written by the remote device.
	U8									*value;

	// This field hosts the number of bytes that have been written by 
	// the remote host in this output report.
	U16									valueLen;

} BleHIDServiceReportValueInformation;


/** 
 * BleHIDProtocolValueInformation type
 * Defines a Protocol mode structure used when the application receive the 
 * BLEHIDSERVICE_EVENT_PROTOCOL_MODE_CHANGE event.
 * It decribes the new protocol mode and the service concerned by the protocol change.
 */
typedef struct {
	
	// The service that receive the protocol mode change
	BleHIDService					*service;

	// This field hosts the protocol mode value 
	// written by the remote device.
	BleHIDServiceProtocolMode		protocolMode;

} BleHIDProtocolValueInformation;

/** 
 * BleHIDCommandValueInformation type
 * Defines a command structure used when the application receive the 
 * BLEHIDSERVICE_EVENT_COMMAND_RECEIVED event.
 * It decribes the command received and the service concerned by this command
 */
typedef struct {
	
	// The service that receives the command 
	BleHIDService					*service;

	// This field hosts the command value 
	// written by the remote device.
	BleHIDServiceCommandId			commandValue;

} BleHIDCommandValueInformation;

/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/

/** Set the local HID DEVICE Information.
 *
 * BLEHIDSERVICE_SetDeviceInformation()
 *	This function is used to set the local HID DEVICE informaion like, 
 * specification version supported, country code or flags.
 *
 * BLE_SUPPORT_HID_SERVICE shall be enabled.
 *
 * @param hidDevice : a valid pointer on a HID device  
 *
 * @param hidDeviceInformation : a valid BleHIDServiceDeviceInfo pointer  
 *   containing the device information, the pointer can be released after
 *	 exiting of the function
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, merely because
 * HID Service is not registered.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEHIDSERVICE_SetDeviceInformation( 
	BleHIDService					*hidDevice,
	BleHIDServiceDeviceInfo			*hidDeviceInformation
	);

/** Set the local HID DEVICE Report MAP (Descriptor) value.
 *
 * BLEHIDSERVICE_SetReportMapValue()
 *	This function is used to set the local HID DEVICE given report MAP value.
 * Note that still this function have not been called then the report MAP 
 * value length is 0.
 *
 * BLE_SUPPORT_HID_SERVICE shall be enabled.
 *
 * @param hidDevice : A valid pointer to a hid device service, the one we want set report map value
 *
 * @param reportMapValue : A valid pointer to the new value of the report MAP, 
 *		this pointer must be persistent still the report MAP value is 
 *		available as the service does not copy any data but use the pointer to 
 *		access to the report Map value.
 *		The pointer could be released afer a call of the function with a
 *		reportMapValueLen of 0.
 * @param reportMapValueLen : The number of bytes representing the report MAP 
 *		length into the reportMapValue pointer. The maximum value length 
 *		supported by the service is 511 bytes.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, merely because
 * HID Service is not registered.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEHIDSERVICE_SetReportMapValue( 
					BleHIDService* hidDevice,
					U8	*reportMapValue,
					U16	reportMapValueLen);

/** Set the local HID DEVICE Report value.
 *
 * BLEHIDSERVICE_SetReportValue()
 *	This function is used to set the local HID DEVICE given report value, 
 * The report for which the value is intented to be assigned is specify by its
 * report type, and if the type is any of the REPORT one (not a BOOT one), 
 * also by it sreport ID.
 * If the remote host have configured this report to send data automaticaly
 * upon change (notification), then the service will automaticaly send the 
 * data to the remote HOST device.
 * Note that still this function have not been called then the report value 
 * length is 0.
 *
 * BLE_SUPPORT_HID_SERVICE shall be enabled.
 *
 * @param hidDevice : A valid pointer to a hid device service, the one we want to set report
 *
 * @param type : The report type of the report for which the value must be
 *		 updated.
 * @param reportID : If the report type is a REPORT one (
 *		BLEHIDSERVICE_REPORTTYPE_REPORTINPUT, 
 *		BLEHIDSERVICE_REPORTTYPE_REPORTOUTPUT or
 *		BLEHIDSERVICE_REPORTTYPE_REPORTFEATURE) then the existing report ID of
 *      the report for which the value must be updated.
 *		Otherwise this field is ignored and can be set to 0.
 * @param reportValue : A valid pointer to the new value of the report, this 
 *		pointer must be persistent still the report value is available as the  
 *		service does not copy any data but use the pointer to access to the  
 *		report value.
 *		The pointer could be released afer a call of the function with a
 *		valueLen of 0.
 * @param reportValueLen : The number of bytes representing the report length  
 *		into the value pointer. The maximum value length supported by the 
 *		service is 511 bytes.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed, merely because
 * HID Service is not registered.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEHIDSERVICE_SetReportValue(BleHIDService* hidDevice,
								BleHIDServiceReportType		type,
								BleHIDServiceReportID		reportID,
								U8							*reportValue,
								U16							reportValueLen);



#endif //(BLE_SUPPORT_HID_SERVICE == 1)
#endif //__BLEHUMANINTERFACEDEVICE_SERVICE_H
