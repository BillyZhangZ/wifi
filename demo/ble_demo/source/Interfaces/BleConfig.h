#ifndef __BLECONFIG_H
#define __BLECONFIG_H
/****************************************************************************
 *
 * File:          BleConfig.h
 *
 * Description:   configuration option for BLE stack
 * 
 * Created:       June, 2008
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.54 $
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

#include "BLECustomConfig.h"


/***************************************************************************\
 * BLE Stack VERSION INFORMATION Constant
\***************************************************************************/
#ifdef BLESTCK_VERSION
#error "BLESTCK_VERSION should not be already defined"
#endif

// The Current stack version is 1.0.7.00 ( 0xMMmmRRrr)
#define BLESTCK_VERSION								0x01000700 


/***************************************************************************\
 * TRANSPORT Constants and configuration options
\***************************************************************************/
/** Transport used is UART */
#define BLETRANSPORT_UART                        0

/** Transport used is CUSTOM */
#define BLETRANSPORT_CUSTOM                      1


/** BLETRANSPORT_USED allows the user to choose the transport used 
 * by the BLE stack.
 *
 * This enables the specific transport API for UART or custom.
 * The default configuration is UART.
 * Override this definition in the CustomConfig file to change the value.
 * The different possible values are BLETRANSPORT_UART or BLETRANSPORT_CUSTOM.
 * Default value is BLETRANSPORT_UART.
 */
#ifndef BLETRANSPORT_USED
#define BLETRANSPORT_USED                        BLETRANSPORT_UART
#endif //BLETRANSPORT_USED

#if (BLETRANSPORT_USED != BLETRANSPORT_UART) && (BLETRANSPORT_USED != BLETRANSPORT_CUSTOM)
#error "BLETRANSPORT_USED shall be defined as BLETRANSPORT_UART or BLETRANSPORT_CUSTOM, check your custom config"
#endif

/***************************************************************************\
 * BLUETOOTH LOW ENERGY CONTROLLER Constants and configuration options
\***************************************************************************/
// The stack does not embeds any controller specific tweaks and specific facilities 
#define BLECONTROLLER_DEFAULT                    0

// The stack embeds tweaks and specific facilities for the EM-Microlectronic Marin
// Low-energy controller
#define BLECONTROLLER_EM                         1


/** BLECONTROLLER_USED allows the user to choose the Bluetooth Low energy controller used 
 * by the BLE stack.
 *
 * This enables specific code and optimisation for given controller
 * Override this definition in the CustomConfig file to change the value.
 * The different possible values are BLECONTROLLER_EM or BLECONTROLLER_DEFAULT.
 * Default value is BLECONTROLLER_DEFAULT.
 */
#ifndef BLECONTROLLER_USED
#define BLECONTROLLER_USED                       BLECONTROLLER_DEFAULT
#endif //BLECONTROLLER_USED

/** BLECONTROLLER_NEED_SPECIFIC_INIT give access to interface used by the HCI layer to
 * call specific radio initialisation
 *
 * The BLERADIO_Init() function is called during the HCI LAYER initialisation.
 * the BLERADIO_HciEventHandler() routes the HCI events during radio initialisation.
 * the BLEHCI_RadioInitialized() shall be called after specific radio initialisation code
 *
 * Note that the RADIO initialisation shall send the HCC_RESET 
 * command (0x0C03), because the stack initialisation will not send it if 
 * BLECONTROLLER_NEED_SPECIFIC_INIT is enabled (1).
 */
#ifndef BLECONTROLLER_NEED_SPECIFIC_INIT
#define BLECONTROLLER_NEED_SPECIFIC_INIT         0
#endif //BLECONTROLLER_NEED_SPECIFIC_INIT

/***************************************************************************\
 * HOST MICROCONTROLLER Constants and configuration options
\***************************************************************************/
/*---------------------------------
 * processor used with the BLE STACK 
 */
#define BLEPROCESSOR_UNKNOWNTYPE                 0
#define BLEPROCESSOR_LITTLEENDIAN                1
#define BLEPROCESSOR_BIGENDIAN                   2

/** BLEPROCESSOR_TYPE allows the user to choose which kind of host is used
 *
 * This enables internal specific code and optimisation for a given 
 * microprossecor type
 * Override this definition in the CustomConfig file to change the value.
 * The different possible values are BLEPROCESSOR_BIGENDIAN, 
 *	BLEPROCESSOR_LITTLEENDIAN or BLEPROCESSOR_UNKNOWNTYPE
 * Default value is BLEPROCESSOR_UNKNOWNTYPE.
 */
#ifndef	BLEPROCESSOR_TYPE
#define	BLEPROCESSOR_TYPE                        BLEPROCESSOR_UNKNOWNTYPE
#endif	//BLEPROCESSOR_TYPE


/***************************************************************************\
 * COMPILER and PORTING configuration options
\***************************************************************************/
/** BLE_PROTECT_ISR_FUNCTION_CALL allows the stack code to protect the stack 
 * functions that are called both from the transport layer within Interrupt service 
 * routine (ISR) and from the Application Layer Interface (API).
 * If transport layer (for example) BLETRANSPORT_UartDataReceived is handled upon ISR
 * This option should be overridden to 1 to avoid reentrancy problem.
 * Default value is 0.
 */
#ifndef	BLE_PROTECT_ISR_FUNCTION_CALL
#define	BLE_PROTECT_ISR_FUNCTION_CALL            0
#endif	//BLE_PROTECT_ISR_FUNCTION_CALL


/** BLE_USE_RESTRICTED_LOCAL_MEMORY allows the stack code to implement mechanism
 * for restricted stack memory size environment.
 * if this option is overidden to 1, the stack code will use some local variables
 * used in the stack as global.
 * Default value is 0.
 */
#ifndef	BLE_USE_RESTRICTED_LOCAL_MEMORY
#define	BLE_USE_RESTRICTED_LOCAL_MEMORY          0
#endif	//BLE_USE_RESTRICTED_LOCAL_MEMORY


/** BLETYPES_ALREADY_DEFINED allows the stack code to 
 * define in "bletypes.h" the standard U8, U16 and U32 types.
 * If the system already define these types or if these type should be 
 * different that is defined in bletypes.h, set BLETYPES_ALREADY_DEFINED to
 * one and if needed redefine them in "BLECustomConfig.h"
 * Default value is 0 (means that the stack use its own defined type).
 */
#ifndef	BLETYPES_ALREADY_DEFINED
#define	BLETYPES_ALREADY_DEFINED                 0
#endif	//BLETYPES_ALREADY_DEFINED


/** BLE_USE_INTERNAL_MEMORY_FUNCTIONS allows the stack code to 
 * use its internal memory functions definition for SYSTEM_Mem*** 
 * (see declaration in "bleSystem.h") 
 * if the system already define these functions ,
 * set BLE_USE_INTERNAL_MEMORY_FUNCTIONS to	0 in "BLECustomConfig.h" and
 * implement these functions in the application code, it may save ROM memory.
 * Default value is 1 (means that the stack use its own defined functions).
 */
#ifndef	BLE_USE_INTERNAL_MEMORY_FUNCTIONS
#define	BLE_USE_INTERNAL_MEMORY_FUNCTIONS        1
#endif	//BLE_USE_INTERNAL_MEMORY_FUNCTIONS

/** BLE_STACK_SKIP_MUTEX_FUNCTIONS allows the stack code to 
 * not calling mutex function defined in "bleSystem.h".
 * In case on monotasking systems, these functions may be useless
 * and skipping them save code memory footprint.
 *
 * Default value is 0 (means that the implementer must defined them,
 even empty).
 */
#ifndef	BLE_STACK_SKIP_MUTEX_FUNCTIONS
#define	BLE_STACK_SKIP_MUTEX_FUNCTIONS           0
#endif	//BLE_STACK_SKIP_MUTEX_FUNCTIONS



/** UNUSED_PARAMETER
 * A way to deal with some function not used parameter to avoid compiler
 * warning, it is most of the time defined as (P=P) or ((void)(P)) (which can 
 * also trigs some warning
 */
#ifndef UNUSED_PARAMETER
#define UNUSED_PARAMETER(P)                      ((void)(P))
#endif //UNUSED_PARAMETER


/** CONST_DECL 
 * The keyword to define where to place the "constant" variables declaration
 * the default behaviour is to place them using "const" keyword.
 */
#ifndef CONST_DECL
#define	CONST_DECL                               const
#endif //CONST_DECL 

/** BLE_FUNC_PTR 
 * The stack registers and uses internally function pointer to be called
 * by other internal layer, for example internal timers, Core stack callback
 * or Attribute Protocol callback may be registered internally by the stack
 * and Profiles.
 * These functions (passed as pointer) are defined local in a layer and
 * may be called by another layer. So by default they are prototped as 
 * "static" but some compiler does not accept to call function pointer that 
 * are prototyped as "static".
 */
#ifndef BLE_FUNC_PTR
#define	BLE_FUNC_PTR                             static
#endif //BLE_FUNC_PTR 


/***************************************************************************\
 *  HCI Number of packets configuration options
\***************************************************************************/

/** NUM_TX_DATA_PACKETS 
 * Defines the number of buffers used by the stack
 * for ACL transmition, 
 * minimum is 3: 1 for ATT, 1 for SMP, 1 for APP 
 * and one to be be available when the stack send
 * packet during the L2CAP callback
 */
#ifndef	NUM_TX_DATA_PACKETS
#define	NUM_TX_DATA_PACKETS                      4
#endif	//NUM_TX_DATA_PACKETS

/** NUM_RX_DATA_PACKETS 
 * Defines the number of buffers used by the stack
 * for ACL reception
 */
#ifndef	NUM_RX_DATA_PACKETS
#define	NUM_RX_DATA_PACKETS                      4
#endif //NUM_RX_DATA_PACKETS

/** NUM_EVENT_PACKETS 
 * Defines the number of buffers used by the stack 
 * for HCI events reception
 */
#ifndef	NUM_EVENT_PACKETS
#define	NUM_EVENT_PACKETS                        4
#endif //NUM_EVENT_PACKETS
#if (NUM_EVENT_PACKETS > 255) 
#error NUM_EVENT_PACKETS should be less than 255
#endif

/** NUM_COMMAND_PACKETS 
 * Defines the number of buffers used by the stack
 * for HCI command transmition
 */
#ifndef	NUM_COMMAND_PACKETS
#define	NUM_COMMAND_PACKETS                      2
#endif //NUM_COMMAND_PACKETS
#if (NUM_COMMAND_PACKETS > 255) 
#error NUM_COMMAND_PACKETS should be less than 255
#endif



/***************************************************************************\
 *  CORE STACK Constants and configuration options
\***************************************************************************/
/**
 * BLE_ROLE_ADVERTISER 
 * defines if the ADVERTISER role is enabled (1) or disabled (0) 
 * this role increases ROM and RAM size of the BLE application
 * By default this option is enabled (1)
 */
#ifndef BLE_ROLE_ADVERTISER
#define	BLE_ROLE_ADVERTISER                      1
#endif //BLE_ROLE_ADVERTISER


/**
 * BLE_ROLE_SCANNER
 * defines if the SCANNER role is enabled (1) or disabled (0) 
 * this role increases ROM and RAM size of the BLE application
 * By default this option is DISABLED (0)
 */
#ifndef BLE_ROLE_SCANNER
#define	BLE_ROLE_SCANNER                         0
#endif //BLE_ROLE_SCANNER


/*---------------------------------
 * Advertsing features
 */
#if	(BLE_ROLE_ADVERTISER == 1)
/**
 * BLE_ADVERTSING_TX_POWER_SUPPORT
 * BLE_ADVERTSING_TX_POWER_SUPPORT define if the Advertising TX power level
 * should be retrieved by the stack.
 * It enables some GAP API to fill Advertisement data. And stack APIs that 
 * retrieve the advertisement TX Power.
 * Defining the BLE_ADVERTSING_TX_POWER_SUPPORT to 1 (one) increase consequently 
 * the final ROM size.
 * By default, this option is enabled (1)
 */
#ifndef BLE_ADVERTSING_TX_POWER_SUPPORT
#define	BLE_ADVERTSING_TX_POWER_SUPPORT          1
#endif //BLE_ADVERTSING_TX_POWER_SUPPORT
#else
#undef BLE_ADVERTSING_TX_POWER_SUPPORT
#define	BLE_ADVERTSING_TX_POWER_SUPPORT          0

#endif //(BLE_ROLE_ADVERTISER == 1)


/*---------------------------------
 * Link layer connection support
 */

/**
 * BLE_CONNECTION_SUPPORT allows the local BLE device to support
 * the Link layer connection handling.
 * It enables the Interface to create an outgoing link layer connection, 
 * And mechanism to handle incoming connection.
 * Moreover, it enables the L2CAP layer needed to handle data over the link
 * layer connection.
 *
 * Note that in order to create an outgoing connection you should be in the
 * scanning mode so the BLE_ROLE_SCANNER should also be enabled.
 *
 * Be careful, defining the BLE_CONNECTION_SUPPORT to 1 (one) 
 * increase consequently the final ROM size. 
 *
 * Disabling this value lets you act as:
 * - A BROADCASTER. A broadcaster device is a BLE device that operates
 * only as an advertiser and uses only non-connectable advertising events
 * to broadcast data. A BLE device that is a broadcaster only may be
 * implemented without a receiver. To be a broadcaster BLE_ROLE_ADVERTISER
 * shall be set to 1.
 *
 * - An OBSERVER. An observer device is a BLE device that operates only as a
 * scanner using passive scanning to acquire data from broadcaster devices. 
 * A BLE device that is an observer only may be implemented without a 
 * transmitter. To be an observer BLE_ROLE_SCANNER shall be set to 1.
 *
 * Enabling this value lets you act as:
 * - A PERIPHERAL : A peripheral device is a BLE device that is capable of
 * operating as a slave in a link layer connection. Consequently, such a
 * device shall be able to operate as an advertiser using connectable 
 * advertising events to invite establishment of a link layer connection.
 * To be a peripheral BLE_ROLE_ADVERTISER shall be set to 1.
 *
 * - A CENTRAL : A central device is a BLE device that is capable of operating
 * as a master in a link layer connection. Consequently, such a device shall
 * be able to operate as a scanner and initiator to establish a link layer
 * connection. To be a central BLE_ROLE_SCANNER shall be set to 1.
 *
 * See below for a review of all roles according configuration definition:
 *
 *                          BLE_CONNECTION_SUPPORT    BLE_CONNECTION_SUPPORT
 *                       |           = 0           |           =1            |
 * ----------------------+-------------------------+-------------------------+
 * BLE_ROLE_ADVERTISER=0 |     NOT POSSIBLE        |   NOT POSSIBLE          |
 * BLE_ROLE_SCANNER=0    |                         |                         |
 * ----------------------+-------------------------+-------------------------+
 * BLE_ROLE_ADVERTISER=1 |    BROADCASTER ONLY     |   BROADCASTER AND       |
 * BLE_ROLE_SCANNER=0    |                         |   PERIPHERAL            |
 * ----------------------+-------------------------+-------------------------+
 * BLE_ROLE_ADVERTISER=0 |    OBSERVER ONLY        |   OBSERVER AND          |
 * BLE_ROLE_SCANNER=1    |                         |   CENTRAL               |
 * ----------------------+-------------------------+-------------------------+
 * BLE_ROLE_ADVERTISER=1 |    OBSERVER OR          |   ALL FOUR ROLES        |
 * BLE_ROLE_SCANNER=1    |    BROADCASTER          |                         |
 * ----------------------+-------------------------+-------------------------+
 *
 * the default value for BLE_CONNECTION_SUPPORT is ENABLED (1)
 */
#ifndef BLE_CONNECTION_SUPPORT
#define BLE_CONNECTION_SUPPORT                   1
#endif //BLE_CONNECTION_SUPPORT

#if (BLE_CONNECTION_SUPPORT == 1)
/**
 * BLE_NUM_MAX_CONNECTION 
 * Defines the maximum simultaneous link layer connections that can exist 
 * at a given time.
 * Increasing this value impacts the final RAM and ROM size.
 * It cannot be equal to zero if BLE_CONNECTION_SUPPORT is ENABLED
 *
 * the default value is 1
 */
#ifndef BLE_NUM_MAX_CONNECTION
#define BLE_NUM_MAX_CONNECTION                   1
#endif //BLE_NUM_MAX_CONNECTION

#if (BLE_NUM_MAX_CONNECTION > 255) 
#error BLE_NUM_MAX_CONNECTION should be less than 255
#endif

#if (BLE_NUM_MAX_CONNECTION == 0) 
#error BLE_NUM_MAX_CONNECTION should be greater than 0
#endif

#endif //(BLE_CONNECTION_SUPPORT==1)

/*---------------------------------
 * L2CAP Specifics
 */
/**
 * BLEL2CAP_ENABLE_API
 *	Define if the stack enables the L2CAP API defined in BleL2CAP.h
 * These API enble to register a specific L2CAP fixed channel
 * and Handle / send data over it.
 * It increase the ROM and RAM size.
 *
 * the default value is DISABLED (0)
 */
#ifndef BLEL2CAP_ENABLE_API
#define BLEL2CAP_ENABLE_API                      0
#endif //BLEL2CAP_ENABLE_API

/**
 * BLEL2CAP_MTU
 *	Define if the maximum transmition unit for the PDU in a BLEL2CAP
 * packet.
 *	Increasing this value impacts the final RAM size.
 *
 * the default value is 23
 */
#ifndef BLEL2CAP_MTU
#define	BLEL2CAP_MTU                             23
#endif //BLEL2CAP_MTU


/*---------------------------------
 * HCI Specifics
 */

/**
 * BLE_ENABLE_VENDOR_SPECIFIC
 *  Define if the HCI vendor specific command handling is enabled 
 * (BLE_ENABLE_VENDOR_SPECIFIC set to 1) or disabled 
 * (BLE_ENABLE_VENDOR_SPECIFIC set to 0).
 * 
 * Supporting this allows the application to use the 
 * BLEHCI_SendVendorSpecificCommand interface.
 *
 * As well the application can receive the vendor specific Event 
 * (event code 0xff) througt the BLEEVENT_VENDOR_SPECIFC_EVENT_RECEIVED
 * in the application callback. If the BLE_ENABLE_VENDOR_SPECIFIC is
 * disabled, the stack silently discard any vendor specific event.
 *
 * Enabling vendor specific command support increase final ROM memory
 * footprint.
 *
 * The default value is DISABLED (0)
 */
#ifndef BLE_ENABLE_VENDOR_SPECIFIC
#define BLE_ENABLE_VENDOR_SPECIFIC               0
#endif //BLE_ENABLE_VENDOR_SPECIFIC


/*---------------------------------
 * Other stuff
 */
/**
 * BLE_WHITE_LIST_SUPPORT
 *	Define if the white list management support is enabled 
 * (BLE_WHITE_LIST_SUPPORT set to 1) or disabled 
 * (BLE_WHITE_LIST_SUPPORT set to 0).
 * 
 * Supporting this allows the application to use the White List API by adding a 
 * remote device to a white list, clearing the white list and get the white list
 * free slot.
 *
 * If the application does not uses this features, it can not defined accurate 
 * filter policy rules using the white list.
 *
 * Enabling white List support increase final ROM memory footprint.
 *
 * The default value is DISABLED (0)
 */
#ifndef BLE_WHITE_LIST_SUPPORT
#define BLE_WHITE_LIST_SUPPORT                   0
#endif //BLE_WHITE_LIST_SUPPORT


/**
 * BLE_USE_HOST_CHANNEL_CLASSIFICATION
 *	Define if the HCI channels classification is enabled
 * (BLE_USE_HOST_CHANNEL_CLASSIFICATION set to 1) or disabled 
 * (BLE_USE_HOST_CHANNEL_CLASSIFICATION set to 0).
 * 
 * Enabling channel classification increase final ROM memory footprint.
 *
 * The default value is DISABLED (0)
 */
#ifndef BLE_USE_HOST_CHANNEL_CLASSIFICATION
#define BLE_USE_HOST_CHANNEL_CLASSIFICATION			0
#endif //BLE_USE_HOST_CHANNEL_CLASSIFICATION


/**
 * BLE_ENABLE_TEST_COMMANDS
 *	Define if the HCI test command support is enabled 
 * (BLE_ENABLE_TEST_COMMANDS set to 1) or disabled 
 * (BLE_ENABLE_TEST_COMMANDS set to 0).
 * 
 * Supporting this allows the application to use the test API by starting the 
 * receiver test, the transmiter test or stopping the tests.
 *
 * Enabling HCI test command support increase final ROM memory footprint.
 *
 * The default value is DISABLED (0)
 */
#ifndef BLE_ENABLE_TEST_COMMANDS
#define BLE_ENABLE_TEST_COMMANDS                 0
#endif //BLE_ENABLE_TEST_COMMANDS

/***************************************************************************\
 *  SECURITY Constants and configuration options
\***************************************************************************/

/**
 * BLE_SECURITY
 *	Define if the Security Manager support is enabled 
 * (BLE_SECURITY set to 1) or disabled 
 * (BLE_SECURITY set to 0).
 * 
 * Supporting this allows to use the Security Manager API and allows the
 * Application to use authentication and encryption for active connection.
 *
 * If you do not use security, the application will reject the remote pairing
 * request.
 * Note that the security is active only on connection; it is automatically 
 * disabled if BLE_CONNECTION_SUPPORT is DISABLED
 *
 * Enabling security support increase consequently final ROM and RAM
 * memory footprint.
 *
 * The default value is ENABLED (1)
 */
#if (BLE_CONNECTION_SUPPORT==0)
#undef BLE_SECURITY
#define BLE_SECURITY                             0
#endif

#ifndef BLE_SECURITY
#define BLE_SECURITY                             1
#endif //BLE_SECURITY


#define SM_IO_DISPLAY_ONLY                       0
#define SM_IO_DISPLAY_YESNO                      1
#define SM_IO_KEYBOARD_ONLY                      2
#define SM_IO_NOINPUTNOOUTPUT                    3
#define SM_IO_KEYBOARD_DYSPLAY                   4

#if (BLE_SECURITY == 1)

/**
 * BLE_SM_DYNAMIC_IO_CAPABILITIES
 *	Define if the device is able to change its security IO capability during 
 *	its lifetime.
 *  If ENABLED (set to 1) then the device IO capability is set by the 
 *   application during runtime thanks to BLESMP_SetIOCapability().
 *  If DISABLED ( set to 0) then the IO capability is STATIC and set during  
 *   compilation thanks to SM_IO_CAPABILITIES definition.
 *
 *  The default configuration is DISBALED (0) 
 */
#ifndef BLE_SM_DYNAMIC_IO_CAPABILITIES
#define BLE_SM_DYNAMIC_IO_CAPABILITIES           0
#endif //BLE_SM_DYNAMIC_IO_CAPABILITIES

/**
 * BLE_SM_DYNAMIC_ENCRYPTION_KEY_SIZE
 *	Define if the device is able to change the encryption Key Size during its
 *	lifetime.
 *  If ENABLED (set to 1) then the device supported maximum encryption key 
 *   size is set by the application during runtime between 7 bytes and 16  
 *	 bytes thanks to the BLESMP_SetMaximumEncrytionKeySize() API. 
 *   Note that the default value at start-up is 16 bytes
 *  If DISABLED ( set to 0) then the local device maximum encryption key size
 *   is 16 bytes.
 *  Note: using a Key size of 16 bytes instead or 7 bytes does not add over 
 *   the air traffic or Baseband additional calculation, and it is better in 
 *	 term of security.
 *   It is why a value of 16 bytes is just fine and an application should not
 *   change this value and let this option to disabled.
 *   However some stack qualification test cases requires modifying this 
 *	 setting
 *
 *  The default configuration is DISBALED (0) 
 */
#ifndef BLE_SM_DYNAMIC_ENCRYPTION_KEY_SIZE
#define BLE_SM_DYNAMIC_ENCRYPTION_KEY_SIZE       0
#endif //BLE_SM_DYNAMIC_ENCRYPTION_KEY_SIZE


/**
 * SM_IO_CAPABILITIES
 *	Define the STATIC local System Input/Output capabilities used in Security 
 *  manager for the Pairing process.
 * When BLE_SM_DYNAMIC_IO_CAPABILITIES is DISABLED then SM_IO_CAPABILITIES is the 
 *  local device IO capabiltity for all the device lifeTime.
 * When BLE_SM_DYNAMIC_IO_CAPABILITIES is ENABLED then SM_IO_CAPABILITIES is the 
 *  default local device IO capabiltity before any change thanks to 
 *	BLESMP_SetIOCapability() .
 *
 * The default configuration is NO INPUT / NO OUTPUT 
 */
#ifndef SM_IO_CAPABILITIES
#define SM_IO_CAPABILITIES                       SM_IO_NOINPUTNOOUTPUT
#endif //SMP_IO_CAPABILITIES

/**
 * BLE_SM_DOUBLE_ROLE_BONDING
 *	This option defines if the local device request or distribute also the
 * master encryption keys in order to be also bonded when the role
 * (peripheral - central) change betwwen to bonded devices.
 *
 * If enabled, (set to 1) :
 *  If the local device is CENTRAL (MASTER), during bonding procedure it will  
 *  also try to distribute this LTK, DIV and RAND in order to not repeat the 
 *  BONDING if the local device is PERIPHERAL (SLAVE) in future connections 
 *  with the samed evice.
 *  Also, if the device is PERIPHERAL (SLAVE), during bonding procedure, it 
 *  will try to request the LTK, DIV and RAND from CENTRAL (MASTER).
 *
 * Enabling this option increase the ROM and over the AIR traffic during
 *  Pairing procedure.
 * If a device have mainly a static ROLE (either CENTRAL or PERIPHERAL), this
 *  option should be left DISABLED in order to save POWER.
 * The default configuration is DISABLED ( set to 0)
 */
#ifndef BLE_SM_DOUBLE_ROLE_BONDING
#define BLE_SM_DOUBLE_ROLE_BONDING               0
#endif //BLE_SM_DOUBLE_ROLE_BONDING

/**
 * BLE_SM_OUTOFBAND_PAIRING
 *	This option defines if the local device enable out of band mechanism in
 * order to exchange pairing information.
 * If ENABLED (set to 1), then the SMP API BLESMP_SetOobData and 
 * BLESMP_ClearOobData are enabled in order to manage Out of band data used 
 * in the pairing procedure.
 *
 * The default configuration is DISABLED ( set to 0)
 */
#ifndef BLE_SM_OUTOFBAND_PAIRING
#define BLE_SM_OUTOFBAND_PAIRING                 0
#endif //BLE_SM_OUTOFBAND_PAIRING


//For internal only
#ifndef BLE_SM_SIGNATURE_SUPPORT
#define BLE_SM_SIGNATURE_SUPPORT                 0
#endif //BLE_SM_SIGNATURE_SUPPORT

#endif //(BLE_SECURITY == 1)

/**
 * BLE_RANDOM_ADDRESSING_SUPPORT
 *	Define if the random addressing support is enabled 
 * (BLE_RANDOM_ADDRESSING_SUPPORT set to 1) or disabled 
 * (BLE_RANDOM_ADDRESSING_SUPPORT set to 0).
 * 
 * Supporting this allow to use the random Addressing API and allow the
 * Application to generate a random address to use in advertisement, scanning
 * and connection.
 *
 * If you do not use random addressing, the application always uses the local 
 * device public hardware address.
 *
 * Enabling random addressing support increase final ROM and RAM memory
 * footprint.
 *
 * The default value is DISABLED (0)
 */
#ifndef BLE_RANDOM_ADDRESSING_SUPPORT
#define BLE_RANDOM_ADDRESSING_SUPPORT            0
#endif //BLE_RANDOM_ADDRESSING_SUPPORT


/***************************************************************************\
 *  DEBUG or LOGGING Constants and configuration options
\***************************************************************************/
/**
 * BLE_PARMS_CHECK allows the user to check at runtime if the parameter
 * of the interfaces are well formatted and / or are in the allowed values
 * range.
 * When the BLE_PARMS_CHECK is defined to zero, the stack does not check
 * the parameters values and assume that they are always corrects.
 * Be carrefull defining the BLE_PARMS_CHECK to 1 (one) increase consequently 
 * the final ROM size. 
 *
 * the default value is DISABLED (0)
 */
#ifndef BLE_PARMS_CHECK
#define BLE_PARMS_CHECK                          0
#endif //BLE_PARMS_CHECK

/**
 * BLEERROR_HANDLER allows the stack to control the errors by calling the 
 * BLEERROR_CatchException to prevent crashing
 * 
 * When the BLEERROR_HANDLER is ENABLED (0), the Stack includes code that check
 * pointer or data integrity. for example bad transport character received or 
 * null pointer resulting from bad configuration.
 * So the application will be notified when the stack has reach an error
 * Be carrefull defining the BLEERROR_HANDLER to 1 (one) increase consequently 
 * the final ROM size. 
 * the default value is DISABLED (0)
 */
#ifndef BLEERROR_HANDLER
#define	BLEERROR_HANDLER                         0
#endif //BLEERROR_HANDLER

/**
 * BLESCTK_USE_DEPRECATED_API allows the application to control if it uses
 * deprecated API.
 * It enables (set to 1) or disables (set to 0) the API / Types / or 
 *  definitions that are deprecated and that will be removed in next 
 *  ALPW-BLESDK versions.
 * Then by disabling this configuration (set to 0) an application can check 
 *  if it is compliant with next release.
 *
 * The default value is ENABLED (1), means deprecated API are still available
 *	in the code
 *
 */
#ifndef BLESCTK_USE_DEPRECATED_API
#define	BLESCTK_USE_DEPRECATED_API               1
#endif //BLESCTK_USE_DEPRECATED_API


/** BLE_PRINT_DEBUG_TRACES enabled the SYSTEM_Log() function in order to 
 * allow several stack layer to Print DEBUG TRACES
 * Default behavior is to not export traces.
 * The stack layer Debug trace must be ativated separatly (see ATT_DEBUG_TRACES
 * or SMP_DEBUG_TRACES).
 * But disabling BLE_PRINT_DEBUG_TRACES disables all the satck layer debug*
 * traces
 */
#ifndef BLE_PRINT_DEBUG_TRACES
#define BLE_PRINT_DEBUG_TRACES                   0
#endif //BLE_PRINT_DEBUG_TRACES

/** SMP_DEBUG_TRACES defines if the Security Maneger Layer exports some debug 
 * traces during Runtime.
 * The debug print are exported through the SYSTEM_Log() function
 * Default behavior is to not export traces.
 * Note that this option is ignored and overided when BLE_PRINT_DEBUG_TRACES is
 * disabled (set to 0)
 */
#ifndef SMP_DEBUG_TRACES
#define SMP_DEBUG_TRACES                         0 
#endif //SMP_DEBUG_TRACES

#endif /*__BLECONFIG_H*/
