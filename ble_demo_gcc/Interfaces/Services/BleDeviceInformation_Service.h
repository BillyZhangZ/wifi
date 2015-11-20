#ifndef __BLEDEVICEINFORMATION_H
#define __BLEDEVICEINFORMATION_H
/****************************************************************************
 *
 * File:          BleDeviceInformation_Service.h
 *
 * Description:   Contains interfaces definitions for The BLE DEVICE
 *					 INFORMATION Service.
 *				  This implementation is compliant with DIS specification
 *                version 1.1
 * 
 * Created:       December, 2010
 * Version:		  1.1
 *
 * CVS Revision : $Revision: 1.7 $
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
 *	Macro and constant definition
\***************************************************************************/
/***************************************************************************\
 * OPTIONAL FEATURES
\***************************************************************************/
/**
 * BLE_SUPPORT_DEVICEINFORMATION_SERVICE
 * Defines if the BLE local device enables access to the local DEVICE   
 * INFORMATION service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles or directly an Application to 
 * get access to the DEVICE INFORMATION Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_DEVICEINFORMATION_SERVICE
#define BLE_SUPPORT_DEVICEINFORMATION_SERVICE								0
#endif //BLE_SUPPORT_DEVICEINFORMATION_SERVICE

/***************************************************************************\
 * OPTIONAL FEATURES FOR DEVICE INFORMATION SERVICE
\***************************************************************************/
#if (BLE_SUPPORT_DEVICEINFORMATION_SERVICE == 1 )
/**
 * BLEDEVICEINFORMATION_SUPPORT_SYSTEMID
 * Defines if the BLE DEVICE INFORMATION Service
 * implementation hosts the System ID characteristic within the DEVICE
 * INFORMATION Service.
 * When enabled (set to 1) it enables the BLEDEVICEINFORMATION_SetSystemId API.
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLEDEVICEINFORMATION_SUPPORT_SYSTEMID
#define BLEDEVICEINFORMATION_SUPPORT_SYSTEMID							0
#endif //BLEDEVICEINFORMATION_SUPPORT_SYSTEMID

/**
 * BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER
 * Defines if the BLE DEVICE INFORMATION Service
 * implementation hosts the Model Number characteristic within the 
 * DEVICE INFORMATION Service.
 * When enabled (set to 1) it enables the BLEDEVICEINFORMATION_SetModelNumber
 * API.
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER
#define BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER						0
#endif //BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER

/**
 * BLEDEVICEINFORMATION_SUPPORT_SERIALNUMBER
 * Defines if the BLE DEVICE INFORMATION Service
 * implementation hosts the Serial Number characteristic within the 
 * DEVICE INFORMATION Service.
 * When enabled (set to 1) it enables the BLEDEVICEINFORMATION_SetSerialNumber
 * API.
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLEDEVICEINFORMATION_SUPPORT_SERIALNUMBER
#define BLEDEVICEINFORMATION_SUPPORT_SERIALNUMBER						0
#endif //BLEDEVICEINFORMATION_SUPPORT_SERIALNUMBER


/**
 * BLEDEVICEINFORMATION_SUPPORT_FIRMWAREREVISION
 * Defines if the BLE DEVICE INFORMATION Service
 * implementation hosts the Firmware Revision characteristic within the 
 * DEVICE INFORMATION Service.
 * When enabled (set to 1) it enables the 
 * BLEDEVICEINFORMATION_SetFirmwareRevision API.
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLEDEVICEINFORMATION_SUPPORT_FIRMWAREREVISION
#define BLEDEVICEINFORMATION_SUPPORT_FIRMWAREREVISION					0
#endif //BLEDEVICEINFORMATION_SUPPORT_FIRMWAREREVISION

/**
 * BLEDEVICEINFORMATION_SUPPORT_HARDWAREREVISION
 * Defines if the BLE DEVICE INFORMATION Service
 * implementation hosts the Hardware Revision characteristic within the 
 * DEVICE INFORMATION Service.
 * When enabled (set to 1) it enables the 
 * BLEDEVICEINFORMATION_SetHardwareRevision API.
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLEDEVICEINFORMATION_SUPPORT_HARDWAREREVISION
#define BLEDEVICEINFORMATION_SUPPORT_HARDWAREREVISION					0
#endif //BLEDEVICEINFORMATION_SUPPORT_HARDWAREREVISION


/**
 * BLEDEVICEINFORMATION_SUPPORT_SOFTWAREREVISION
 * Defines if the BLE DEVICE INFORMATION Service
 * implementation hosts the Software Revision characteristic within the 
 * DEVICE INFORMATION Service.
 * When enabled (set to 1) it enables the 
 * BLEDEVICEINFORMATION_SetSoftwareRevision API.
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLEDEVICEINFORMATION_SUPPORT_SOFTWAREREVISION
#define BLEDEVICEINFORMATION_SUPPORT_SOFTWAREREVISION					0
#endif //BLEDEVICEINFORMATION_SUPPORT_SOFTWAREREVISION

/**
 * BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME
 * Defines if the BLE DEVICE INFORMATION Service
 * implementation hosts the Manufacturer Name characteristic within the 
 * DEVICE INFORMATION Service.
 * When enabled (set to 1) it enables the 
 * BLEDEVICEINFORMATION_SetManufacturerName API.
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME
#define BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME					0
#endif //BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME

/**
 * BLEDEVICEINFORMATION_SUPPORT_REGISTRATIONDATA
 * Defines if the BLE DEVICE INFORMATION Service
 * implementation hosts the Registration Certificate Data characteristic 
 * within the DEVICE INFORMATION Service.
 * When enabled (set to 1) it enables the 
 * BLEDEVICEINFORMATION_SetRegistrationData API.
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLEDEVICEINFORMATION_SUPPORT_REGISTRATIONDATA
#define BLEDEVICEINFORMATION_SUPPORT_REGISTRATIONDATA					0
#endif //BLEDEVICEINFORMATION_SUPPORT_REGISTRATIONDATA

/**
 * BLEDEVICEINFORMATION_SUPPORT_PNPID
 * Defines if the BLE DEVICE INFORMATION Service
 * implementation hosts the PnP ID characteristic 
 * within the DEVICE INFORMATION Service.
 * When enabled (set to 1) it enables the 
 * BLEDEVICEINFORMATION_SetPnPId API.
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLEDEVICEINFORMATION_SUPPORT_PNPID
#define BLEDEVICEINFORMATION_SUPPORT_PNPID								0
#endif //BLEDEVICEINFORMATION_SUPPORT_PNPID

//  The Device information Profile version 1.1 does not impose any more to 
// support at least one of the characteristic.
//  It is possible that none of the characteristics below are included. 

/*#if ( (BLEDEVICEINFORMATION_SUPPORT_SYSTEMID == 0) && \
	(BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER == 0) && \
	(BLEDEVICEINFORMATION_SUPPORT_SERIALNUMBER == 0) && \
	(BLEDEVICEINFORMATION_SUPPORT_FIRMWAREREVISION == 0) && \
	(BLEDEVICEINFORMATION_SUPPORT_HARDWAREREVISION == 0) && \
	(BLEDEVICEINFORMATION_SUPPORT_SOFTWAREREVISION == 0) && \
	(BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME == 0) && \
	(BLEDEVICEINFORMATION_SUPPORT_REGISTRATIONDATA == 0) )
#error At least one optional characteristic shall be supported within the DEVICE INFORMATION Service.
#endif //((...))*/

/***************************************************************************\
 *	Type definition
\***************************************************************************/
#if ( BLEDEVICEINFORMATION_SUPPORT_PNPID == 1 )
/**
 * BleDeviceInformationPnpId
 * Defines the format of the PnpId to be passed as parameter of the 
 * BLEDEVICEINFORMATION_SetPnPId API.
 * It indicates:
 * vendorIdSource: Identifies the source of the Vendor ID field
 * vendorID: Identifies the product vendor from the namespace in the Vendor
 *           ID Source
 * productID: Manufacturer managed identifier for this product
 * productVersion: Manufacturer managed version for this product
 */
typedef struct{
	U8		vendorIdSource;
	U16		vendorId;
	U16		productId;
	U16		productVersion;
} BleDeviceInformationPnpId;

#endif //( BLEDEVICEINFORMATION_SUPPORT_PNPID == 1 )
/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/

/***************************************************************************\
 * OPTIONAL API functions definition DEVICE INFORMATION SERVICE
\***************************************************************************/
/** Register a BLE DEVICE INFORMATION Service.
 *
 * BleDeviceInformation_Register()
 *	This function is used to add the supported DEVICE INFORMATION
 * Service characteristics into the Attribute database.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed. ( DEVICE 
 * INFORMATION Service may be already present in the database)
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEDEVICEINFORMATION_Register( void );

#if (BLEDEVICEINFORMATION_SUPPORT_SYSTEMID == 1)
/** Set the current DEVICE INFORMATION System Id Value
 *
 * BLEDEVICEINFORMATION_SetSystemId()
 *	This function is used to set the current DEVICE INFORMATION System 
 * Id characteristic value.
 *
 * BLEDEVICEINFORMATION_SUPPORT_SYSTEMID shall be enabled.
 *
 * @param systemId: A valid pointer to the 8 octets long system ID,The pointer
 *                  shall not be persistent and can be reused when the
 *                  function returns.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEDEVICEINFORMATION_SetSystemId(
	U8 *systemId);
#endif //(BLEDEVICEINFORMATION_SUPPORT_SYSTEMID == 1)

#if (BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER == 1)
/** Set the current DEVICE INFORMATION Model Number Value
 *
 * BLEDEVICEINFORMATION_SetModelNumber()
 *	This function is used to set the current DEVICE INFORMATION  
 * Model Number characteristic value.
 *
 * BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER shall be enabled.
 *
 * @param modelNumber: A valid pointer to the Model Number representing the
 *			Model Number string. The pointer shall be valid during all the 
 *			rest of the life time of the application or until this API is 
 *			called again with a modelNumberLen set to 0.
 * @param modelNumberLen: The length of the string in the modelNumber 
 *			pointer.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEDEVICEINFORMATION_SetModelNumber(
	U8 *modelNumber, 
	U8 modelNumberLen);
#endif //(BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER == 1)

#if (BLEDEVICEINFORMATION_SUPPORT_SERIALNUMBER == 1)
/** Set the current DEVICE INFORMATION Serial Number Value
 *
 * BLEDEVICEINFORMATION_SetSerialNumber()
 *	This function is used to set the current DEVICE INFORMATION  
 * Serial Number characteristic value.
 *
 * BLEDEVICEINFORMATION_SUPPORT_SERIALNUMBER shall be enabled.
 *
 * @param serialNumber: A valid pointer to the Serial Number representing the
 *			Serial Number string. The pointer shall be valid during all the 
 *			rest of the life time of the application or until this API is 
 *			called again with a serialNumberLen set to 0.
 * @param serialNumberLen: The length of the string in the serialNumber 
 *			pointer.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEDEVICEINFORMATION_SetSerialNumber(
	U8 *serialNumber, 
	U8 serialNumberLen);
#endif //(BLEDEVICEINFORMATION_SUPPORT_SERIALNUMBER == 1)

#if (BLEDEVICEINFORMATION_SUPPORT_FIRMWAREREVISION == 1)
/** Set the current DEVICE INFORMATION Firmware Revision Value
 *
 * BLEDEVICEINFORMATION_SetFirmwareRevision()
 *	This function is used to set the current DEVICE INFORMATION  
 *  Firmware Revision characteristic value.
 *
 * BLEDEVICEINFORMATION_SUPPORT_FIRMWAREREVISION shall be enabled.
 *
 * @param firmwareRevision: A valid pointer to the Firmware Revision representing the
 *			Firmware Revision string. The pointer shall be valid during all the 
 *			rest of the life time of the application or until this API is 
 *			called again with a firmwareRevisionLen set to 0.
 * @param firmwareRevisionLen: The length of the string in the firmwareRevision 
 *			pointer.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEDEVICEINFORMATION_SetFirmwareRevision(
	U8 *firmwareRevision, 
	U8 firmwareRevisionLen);
#endif //(BLEDEVICEINFORMATION_SUPPORT_FIRMWAREREVISION == 1)

#if (BLEDEVICEINFORMATION_SUPPORT_HARDWAREREVISION == 1)
/** Set the current DEVICE INFORMATION Hardware Revision Value
 *
 * BLEDEVICEINFORMATION_SetHardwareRevision()
 *	This function is used to set the current DEVICE INFORMATION  
 *  Hardware Revision characteristic value.
 *
 * BLEDEVICEINFORMATION_SUPPORT_HARDWAREREVISION shall be enabled.
 *
 * @param hardwareRevision: A valid pointer to the Hardware Revision representing the
 *			Hardware Revision string. The pointer shall be valid during all the 
 *			rest of the life time of the application or until this API is 
 *			called again with a hardwareRevisionLen set to 0.
 * @param hardwareRevisionLen: The length of the string in the hardwareRevision 
 *			pointer.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEDEVICEINFORMATION_SetHardwareRevision(
	U8 *hardwareRevision,
	U8 hardwareRevisionLen);
#endif //(BLEDEVICEINFORMATION_SUPPORT_HARDWAREREVISION == 1)

#if (BLEDEVICEINFORMATION_SUPPORT_SOFTWAREREVISION == 1)
/** Set the current DEVICE INFORMATION Software Revision Value
 *
 * BLEDEVICEINFORMATION_SetSoftwareRevision()
 *	This function is used to set the current DEVICE INFORMATION  
 *  Software Revision characteristic value.
 *
 * BLEDEVICEINFORMATION_SUPPORT_SOFTWAREREVISION shall be enabled.
 *
 * @param softwareRevision: A valid pointer to the Software Revision representing the
 *			Software Revision string. The pointer shall be valid during all the 
 *			rest of the life time of the application or until this API is 
 *			called again with a softwareRevisionLen set to 0.
 * @param softwareRevisionLen: The length of the string in the softwareRevision 
 *			pointer.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEDEVICEINFORMATION_SetSoftwareRevision(
	U8 *softwareRevision,
	U8 softwareRevisionLen);
#endif //(BLEDEVICEINFORMATION_SUPPORT_SOFTWAREREVISION == 1)

#if (BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME == 1)
/** Set the current DEVICE INFORMATION Manufacturer Name Value
 *
 * BLEDEVICEINFORMATION_SetManufacturerName()
 *	This function is used to set the current DEVICE INFORMATION  
 *  Manufacturer Name characteristic value.
 *
 * BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME shall be enabled.
 *
 * @param manufacturerName: A valid pointer to the Manufacturer Name representing the
 *			Manufacturer Name string. The pointer shall be valid during all the 
 *			rest of the life time of the application or until this API is 
 *			called again with a manufacturerNameLen set to 0.
 * @param manufacturerNameLen: The length of the string in the manufacturerName 
 *			pointer.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEDEVICEINFORMATION_SetManufacturerName(
	U8 *manufacturerName,
	U8 manufacturerNameLen);
#endif //(BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME == 1)

#if (BLEDEVICEINFORMATION_SUPPORT_REGISTRATIONDATA == 1)
/** Set the current DEVICE INFORMATION Registration Certificate Data 
 *	Value
 *
 * BLEDEVICEINFORMATION_SetRegistrationData()
 *	This function is used to set the current DEVICE INFORMATION  
 *  Registration Certificate Data characteristic value.
 *
 * BLEDEVICEINFORMATION_SUPPORT_REGISTRATIONDATA shall be enabled.
 *
 * @param registrationData: A valid pointer to the Manufacturer Name representing the
 *			Registration Certificate Data string. The pointer shall be valid during 
 *			all the rest of the life time of the application or until this API is 
 *			called again with a registrationDataLen set to 0.
 * @param registrationDataLen: The length of the string in the registrationData 
 *			pointer.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEDEVICEINFORMATION_SetRegistrationData(
	U8 *registrationData,
	U8 registrationDataLen);
#endif //(BLEDEVICEINFORMATION_SUPPORT_REGISTRATIONDATA == 1)

#if (BLEDEVICEINFORMATION_SUPPORT_PNPID == 1)
/** Set the current DEVICE INFORMATION Registration Certificate Data 
 *	Value
 *
 * BLEDEVICEINFORMATION_SetPnpId()
 *	This function is used to set the current DEVICE INFORMATION  
 *  PnP Id characteristic value. The PnP_ID characteristic is a set of values
 *  that used to create a device ID value that is unique for this device.
 *  Included in the characteristic is a Vendor ID Source field, a Vendor ID
 *  field, a Product ID field and a Product Version field. These values are 
 *  used to identify all devices of a given type/model/version using numbers. 
 *
 * BLEDEVICEINFORMATION_SUPPORT_PNPID shall be enabled.
 *
 * @param pnpId: A valid pointer to the PnP ID representing the
 *			PnP ID Data.  The pointer is copied in the service context and can
 *			be freed when this APi returns.
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates that the operation succeeded.
 *
 *	- BLESTATUS_FAILED indicates that the operation has failed.
 *
 * @author Alexandre GIMARD
 */
BleStatus BLEDEVICEINFORMATION_SetPnpId(
	BleDeviceInformationPnpId *pnpId);
#endif //(BLEDEVICEINFORMATION_SUPPORT_PNPID == 1)

#endif //(BLE_SUPPORT_DEVICEINFORMATION_SERVICE == 1 )

#endif //__BLEDEVICEINFORMATION_H
