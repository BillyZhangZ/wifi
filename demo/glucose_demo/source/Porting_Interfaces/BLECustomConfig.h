/**@file BLECustomConfig.h
 * @section BLECustomConfig.h description
 * 
 * This file allows the application to fine tune the STACK and PROFILES
 * configuration to fit the most as possible with the application requested
 * features.
 *
 * This file is included within the whole STACK and PROFILES prior any other
 * header files, so configuration defined/overrided in this file will be
 * supersede any other CORE STACK and PROFILES configuration.
 *
 * So it override:
 * - The CORE STACK configuration defined in bleConfig.h
 * - The ATTribute protocol configuration defined in attConfig.h
 * - Any SERVICE configuration defined in SERVICEs interfaces
 * - Any PROFILE configuration defined in PROFILEs interfaces
 *		( including GATT and GAP PROFILEs)
 * - Maybe the STACK default types (U8, U16, ...) defined in bleTypes.h
 *
 */
#define	CONST_DECL												const
#define UNUSED_PARAMETER(P)										(P = P)
#define ATT_SERVER_SUPPORT_PREPARE_EXECUTE_WRITE_REQ			1
#define ATT_SERVER_SUPPORT_READ_BLOB_REQ						1
#define BLE_SUPPORT_GLUCOSE_SERVICE               1
#define BLEGLUCOSE_SUPPORT_SENSOR               1
#define BLE_GLUCOSESERVICE_SUPPORT_CONCENTRATION_MEASUREMENT               1
#define BLE_GLUCOSESERVICE_SUPPORT_TIMEOFFSET_MEASUREMENT               1
#define BLE_GLUCOSESERVICE_SUPPORT_LOWBATTERYDETECTIONDURINGMEASUREMENT_FEATURE               1
#define BLE_GLUCOSESERVICE_SUPPORT_SENSORMALFUNCTIONDETECTION_FEATURE               1
#define BLE_GLUCOSESERVICE_SUPPORT_GENERALDEVICEFAULT_FEATURE               1
#define BLE_GLUCOSESERVICE_SUPPORT_TIMEFAULT_FEATURE               1
//#define BLE_GLUCOSESERVICE_SUPPORT_SENSOR_STATUS_ANNUNCIATION_MEASUREMENT 1
#define BLE_SUPPORT_DEVICEINFORMATION_SERVICE               1
#define BLEDEVICEINFORMATION_SUPPORT_SYSTEMID               1
#define BLEDEVICEINFORMATION_SUPPORT_MODELNUMBER               1
#define BLEDEVICEINFORMATION_SUPPORT_MANUFACTURERNAME               1
#define BLE_ROLE_SCANNER               1
#define BLE_ROLE_ADVERTISER               1
#define BLE_PARMS_CHECK               1
#define BLE_CONNECTION_SUPPORT               1
#define BLE_SECURITY               1
#define BLECONTROLLER_USED               1
#define BLE_ENABLE_VENDOR_SPECIFIC               1
#define BLECONTROLLER_NEED_SPECIFIC_INIT               1
#define ATTRIBUTE_PROTOCOL               1
#define BLE_PROTECT_ISR_FUNCTION_CALL               1
#define BLE_USE_RESTRICTED_LOCAL_MEMORY               1
#define BLE_USE_INTERNAL_MEMORY_FUNCTIONS				0
#define BLE_PRINT_DEBUG_TRACES				1
#define ATT_DEBUG_TRACES										1
#define SMP_DEBUG_TRACES										1
#define BLE_RANDOM_ADDRESSING_SUPPORT							1
//#define BLE_SM_DYNAMIC_IO_CAPABILITIES							1

