#ifndef __BLESYSTEM_H
#define __BLESYSTEM_H

/****************************************************************************
 *
 * File:          BleSystem.h
 *
 * Description:   Contains interfaces routines for SYSTEM service needed by the BLE
 *					Stack.
 *				  These routines should be filled by the stack implementer's
 * 
 * Created:       June, 2008
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.26 $
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

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

/***************************************************************************\
 * External Functions
\***************************************************************************/

#if (BLE_STACK_SKIP_MUTEX_FUNCTIONS == 0)
/* Lock the stack critical section.
 *
 * SYSTEM_StackMutexPend()
 *	The BLE stack uses a critical section to prevent internal interface 
 *	reentrency and dead lock, the BLE stack calls this function to acquire
 *	the critical section
 *	this function SHALL be implemented.
 *	It could be implemented using a mutex or perhaps a semaphore
 *
 * BLE_STACK_SKIP_MUTEX_FUNCTIONS shall be DISABLED
 *
 * see SYSTEM_StackMutexPost()
 *
 * return none
 */
void SYSTEM_StackMutexPend(void);
#else //(BLE_STACK_SKIP_MUTEX_FUNCTIONS == 0)
#define SYSTEM_StackMutexPend(s) {};
#endif //(BLE_STACK_SKIP_MUTEX_FUNCTIONS == 0)

#if (BLE_STACK_SKIP_MUTEX_FUNCTIONS == 0)    
/* Unlock the stack critical section.
 *
 * SYSTEM_StackMutexPost()
 *	The BLE stack uses a critical section to prevent internal interface 
 *	reentrency and dead lock, the BLE stack calls this function to release
 *	the critical section
 *	this function SHALL be implemented.
 *	It could be implemented using a mutex or perhaps a semaphore
 *
 * BLE_STACK_SKIP_MUTEX_FUNCTIONS shall be DISABLED
 *
 * see SYSTEM_StackMutexPend()
 *
 * return none
 */
void SYSTEM_StackMutexPost(void);
#else //(BLE_STACK_SKIP_MUTEX_FUNCTIONS == 0)
#define SYSTEM_StackMutexPost(s) {};
#endif //(BLE_STACK_SKIP_MUTEX_FUNCTIONS == 0)
 
#if (BLE_STACK_SKIP_MUTEX_FUNCTIONS == 0)    
/* Lock the Attribute database mutex.
 *
 * SYSTEM_DatabaseMutexPend()
 *	The BLE stack uses a MUTEX to prevent internal/external multiple access 
 *	to the Attribute protocol database, the BLE stack calls this function to 
 *  acquire the MUTEX. This function SHALL be implemented.
 *  Note that the User application does not call this function directly, but
 *  call the application interface ATT_SERVER_SecureDatabaseAccess().
 * Only suitable if ATTRIBUTE_PROTOCOL is enabled (1)
 *
 *	It could be implemented using a mutex or perhaps a semaphore
 *
 * BLE_STACK_SKIP_MUTEX_FUNCTIONS shall be DISABLED
 *
 * see SYSTEM_DatabaseMutexPost()
 *
 * return none
 */
void SYSTEM_DatabaseMutexPend(void);
#else //(BLE_STACK_SKIP_MUTEX_FUNCTIONS == 0)
#define SYSTEM_DatabaseMutexPend(s) {};
#endif //(BLE_STACK_SKIP_MUTEX_FUNCTIONS == 0)
    
#if (BLE_STACK_SKIP_MUTEX_FUNCTIONS == 0)
/* Unlock the stack mutex.
 *
 * SYSTEM_DatabaseMutexPost()
 *	The BLE stack uses a MUTEX to prevent internal/external multiple access 
 *	to the Attribute protocol database, the BLE stack calls this function to 
 *  release the MUTEX. This function SHALL be implemented.
 *  Note that the User application does not call this function directly, but
 *  call the application interface ATT_SERVER_ReleaseDatabaseAccess().
 * Only suitable if ATTRIBUTE_PROTOCOL is enabled (1)
 *
 *	It could be implemented using a mutex or perhaps a semaphore
 *
 * BLE_STACK_SKIP_MUTEX_FUNCTIONS shall be DISABLED
 *
 * see SYSTEM_DatabaseMutexPend()
 *
 * return none
 */
void SYSTEM_DatabaseMutexPost(void);
#else //(BLE_STACK_SKIP_MUTEX_FUNCTIONS == 0)
#define SYSTEM_DatabaseMutexPost(s) {};
#endif //(BLE_STACK_SKIP_MUTEX_FUNCTIONS == 0)
    
/***************************************************************************\
 * Memory Operation
\***************************************************************************/

/* Copy n bytes from the "from" address to the "to" address 
 *
 * SYSTEM_MemCpy()
 *	This function copy n bytes from the "from" address to the
 *  "to" address.
 *  The bytes are copied in the "to" buffer as the the "to" first byte
 *   will take the value from the "from" first byte and the "to" n-ieme
 *   byte will take the value from the n-ieme "from " byte.
 *
 *  The stack uses for this function its internal function definition if
 *  BLE_USE_INTERNAL_MEMORY_FUNCTIONS is set to 1
 *  If the system has already this kind of mechanism set 
 *  BLE_USE_INTERNAL_MEMORY_FUNCTIONS to 0 in "BLECustomConfig.h" and
 *  implement these functions in the application code, it may save ROM memory.
 *
 *  to: Pointer to the destination address
 *  from: Pointer to the source address
 *  n: number of bytes to copy
 *
 * return A pointer on the "to" address
 *	
 */
void* SYSTEM_MemCpy(void *to, const void *from, U16 n);

/* Copy n bytes from the "from" address to the "to" address 
 *
 * SYSTEM_ReverseMemCpy()
 *	This function copy n bytes from the "from" address to the
 *  "to" address.
 *  The bytes are copied in the "to" buffer as the the "to" first byte
 *   will take the value from the n-ieme "from" byte and the "to" n-ieme
 *   byte will take the value from the first "from " byte.
 *   
 *  The stack uses for this function its internal function definition if
 *  BLE_USE_INTERNAL_MEMORY_FUNCTIONS is set to 1
 *  If the system has already this kind of mechanism set 
 *  BLE_USE_INTERNAL_MEMORY_FUNCTIONS to 0 in "BLECustomConfig.h" and
 *  implement these functions in the application code, it may save ROM memory.
 *
 *  to: Pointer to the destination address
 *  from: Pointer to the source address
 *  n: number of bytes to copy
 *
 * return A pointer on the "to" address
 *	
 */
void* SYSTEM_ReverseMemCpy(void *to, const void *from, U16 n);


/* Compare n bytes between the "a1" address and the "a2" address 
 *
 * SYSTEM_MemCmp()
 *	This function compares n bytes between the "a1" address and the
 *  "a2" address.
 *
 *  The stack uses for this function its internal function definition if
 *  BLE_USE_INTERNAL_MEMORY_FUNCTIONS is set to 1
 *  If the system has already this kind of mechanism set 
 *  BLE_USE_INTERNAL_MEMORY_FUNCTIONS to 0 in "BLECustomConfig.h" and
 *  implement these functions in the application code, it may save ROM memory.
 *
 *  a1: Pointer to the first address
 *  a2: Pointer to the second address
 *  n: number of bytes to compare
 *
 * return  0 if the n bytes are the same
 *			a positive value if the n bytes are different
 */
U16 SYSTEM_MemCmp(const void *a1, const void *a2, U16 n);


/* Set n bytes between from a given address to a given value.
 *
 * SYSTEM_MemSet()
 *	This function set n bytes between from a given address (addr) to a
 *  given value (value).
 *
 *  The stack uses for this function its internal function definition if
 *  BLE_USE_INTERNAL_MEMORY_FUNCTIONS is set to 1
 *  If the system has already this kind of mechanism set 
 *  BLE_USE_INTERNAL_MEMORY_FUNCTIONS to 0 in "BLECustomConfig.h" and
 *  implement these functions in the application code, it may save ROM memory.
 *
 *  addr: Pointer to the address
 *  value: value to set
 *  n: number of bytes to set
 *
 * return  addr is returned
 */
void *SYSTEM_MemSet(void *addr, U8 value, U16 n);

/***************************************************************************\
 * Timer Operation
\***************************************************************************/

/* The stack request a timer creation
 *
 * SYSTEM_TimerCreate()
 *	The BLE stack needs one timer in order to calculate timeout or watchdog.
 *  It uses this function to create the timer; timers are internally
 *  multiplexed, so it needs only one.
 *
 * see SYSTEM_TimerCancel() , SYSTEM_TimerGetRemain()
 *
 *  func : pointer to a function to call when the timer expires
 *  param : parameter to pass in func when the timer expire
 *  millisecond : time to wait before the timer expiration
 *
 * return none
 */
void SYSTEM_CreateTimer(void *func, void *param, U32 millisecond);


/* The stack request its timer destruction
 *
 * SYSTEM_CancelTimer()
 *	The BLE stack no more needs timer. It requests destruction of the timer
 * created with SYSTEM_CreateTimer().
 *
 * see SYSTEM_CreateTimer() , SYSTEM_TimerGetRemain()
 *
 * return none
 */
void SYSTEM_CancelTimer(void);

/* The stack request the timer remainting time
 *
 * SYSTEM_TimerGetRemain()
 *	The BLE stack need the remaining time of the timer created by
 *  SYSTEM_CreateTimer() in order to do some internal things
 *
 * see SYSTEM_CreateTimer() , SYSTEM_CancelTimer()
 *
 * return the time remaining in millisecond.
 */
U32 SYSTEM_TimerGetRemain(void);

/***************************************************************************\
 * Security Operation
\***************************************************************************/
#if (BLE_SECURITY == 1)

#define BLEINFOTYPE_MLTK16				0x01	// a 16 Bytes length key
#define BLEINFOTYPE_SLTK16				0x02	// a 16 Bytes length key
#define BLEINFOTYPE_MDIV2				0x03	// a 2 Bytes length key
#define BLEINFOTYPE_MRAND8				0x04	// a 8 Bytes length key
#define BLEINFOTYPE_MPROP				0x05	// a single Byte length information
#define BLEINFOTYPE_SPROP				0x06	// a single Byte length information
#define BLEINFOTYPE_REMOTE_IRK			0x07	// a 16 Bytes length key
//#define BLEINFOTYPE_LOCAL_IRK			0x08	// a 16 Bytes length key
#define BLEINFOTYPE_ATTHANDLESCHANGED	0x11	// a single Byte length information
#define BLEINFOTYPE_SERVICECHANGECONFIG	0x12	// a single Byte length information

/** The stack request the system to store an information in persistent memory
 *
 * SYSTEM_SetPersistentInformation()
 *	The BLE stack need to save a specific security information with a specified remote
 * device in the persistent memory.
 *
 *  addr : A valid pointer to the remote address for which security 
 *		   information should be stored,
 *			IMPORTANT NOTE: if BD_ADDR is 00:00:00:00:00:00, all the entry of the 
 *			persistent memory shall be updated.
 *  infoType : the type of the information to store
 *  infoValue : a valid pointer to the information to save
 *  infoLen : the length the information to save
 *
 * return the status of the operation
 */
BleStatus SYSTEM_SetPersistentInformation(
	BD_ADDR *addr, 
	U8 infoType, 
	U8* infoValue,
	U8 InfoLen);


/** The stack request the system to retrieve an information from persistent memory
 *
 * SYSTEM_GetPersistentInformation()
 *	The BLE stack need to retrieve a specific security information 
 *	from the persistent memory.
 *
 *  addr :  A valid pointer to the remote address for which security 
 *		    information shall be retrieved
 *  infoType : the type of the information to retrieve
 *  infoValue : the value of the information retrieved
 *  infoLen : a the length the information retrieved
 *
 * return the status of the operation
 */
BleStatus SYSTEM_GetPersistentInformation(
	BD_ADDR *addr,
	U8 infoType, 
	U8 **infoValue,
	U8 *InfoLen);

/** The stack request the system to remove an information from persistent memory
 *
 * SYSTEM_RemovePersistentInformation()
 *	 The BLE stack need to remove a specific persistent information 
 *	from the persistent memory. It request to remove a range of infotype
 *   The stack call this function when the Application wants to unbond with a
 *  remote device or when it detect that profile information is no more valid
 *   Also, an application may delete the linkUp information for a given
 *  profile by calling this function.
 *
 *  addr :  A valid pointer to the remote address for which persistent 
 *		    information shall be retrieved
 *  infoTypeFrom : the starting range of the infoType to delete, this 
 *          infoTypeFrom is included into the range to delete
 *  infoTypeTo : the ending range of the infoType to delete, this 
 *          infoTypeTo is included into the range to delete
 *
 * return the status of the operation
 */
BleStatus SYSTEM_RemovePersistentInformation(
	BD_ADDR *addr, 
	U8 infoTypeFrom, 
	U8 infoTypeTo);

#endif //(BLE_SECURITY == 1)


/***************************************************************************\
 * debug Traces Operation
\***************************************************************************/
#if (BLE_PRINT_DEBUG_TRACES == 1)

/* The stack request the system to print some Debug Traces
 *
 * SYSTEM_Log()
 *	The BLE stack ha&ve some debug information to print.
 *
 *	BLE_PRINT_DEBUG_TRACES shall be enabled.
 *
 *  format: a list of printable arguments
 *
 * return the status of the operation
 */
void SYSTEM_Log(char *format,...);

#endif //(BLE_PRINT_DEBUG_TRACES == 1)
#endif /*__BLESYSTEM_H*/
