/*
 * K22_System.c
 *
 *  Created on: Nov 13, 2014
 *      Author: B45087
 */


#include <string.h>
#include <mqx.h>
#include <bsp.h>
#include <mutex.h>
#include <timer.h>
#include "BleTypes.h"
#include "BleSystem.h"

/***************************************************************************\
 * EXTERN data
\****************************************************************************/
extern MQX_FILE_PTR flash_file;



/***************************************************************************\
 * RAM data
\***************************************************************************/
extern MUTEX_STRUCT stack_mutex, database_mutex;
extern MUTEX_ATTR_STRUCT stack_mutexattr, database_mutexattr;
MQX_TICK_STRUCT tick;
_timer_id timer_id1;
#define TIMER_TASK_PRI  2
#define TIMER_STACK_SIZE 1000

/***************************************************************************\
 * ROM data
\***************************************************************************/


/*****************************************************************************
 *	Local functions Prototype
 *****************************************************************************/
#define SYSTEM_MemSet memset
#define SYSTEM_MemCpy memcpy
#define SYSTEM_MemCmp memcmp

/***************************************************************************\
 * External Functions
\***************************************************************************/




/***************************************************************************\
 * Synchronisation Operation
\***************************************************************************/
#if (BLE_STACK_SKIP_MUTEX_FUNCTIONS == 0)
/** Lock the stack mutex.
 *
 * SYSTEM_StackMutexPend()
 *	The BLE stack uses a MUTEX to prevent internal interface reentrency and 
 *	dead lock, the BLE stack calls this function to acquire the MUTEX
 *	this function SHALL be implemented.
 *	It could be implemented using a mutex or perhaps a semaphore
 *
 * @todo implement this function
 * @todo Mutex or semaphore
 *
 * @see SYSTEM_StackMutexPost()
 *
 * @return none
 *	
 * @author Alexandre GIMARD
 */
void SYSTEM_StackMutexPend(void){
	// Add here specific code to pend the stack Mutex
	//>
	_mutex_lock(&stack_mutex);

} 

/** Unlock the stack mutex.
 *
 * SYSTEM_StackMutexPost()
 *	The BLE stack uses a MUTEX to prevent internal interface reentrency and 
 *	dead lock, the BLE stack calls this function to acquire the MUTEX
 *	this function SHALL be implemented.
 *	It could be implemented using a mutex or perhaps a semaphore
 *
 * @todo implement this function
 *
 * @see SYSTEM_StackMutexPend()
 *
 * @return none
 *	
 * @author Alexandre GIMARD
 */
void SYSTEM_StackMutexPost(void){
	// Add here specific code to post the stack Mutex
	//>
    _mutex_unlock(&stack_mutex);
}

/** Lock the Attribute database mutex.
 *
 * SYSTEM_DatabaseMutexPend()
 *	The BLE stack uses a MUTEX to prevent internal/external multiple access 
 *	to the Attribute protocol database, the BLE stack calls this function to 
 *  acquire the MUTEX. This function SHALL be implemented.
 *  Note that the User application does not call this function directly, but
 *  call the application interface ATT_Server_SecureDatabaseAccess().
 *
 *	It could be implemented using a mutex or perhaps a semaphore
 *
 * @see SYSTEM_DatabaseMutexPost()
 *
 * @return none
 *	
 * @author Alexandre GIMARD
 */
void SYSTEM_DatabaseMutexPend(void){
	// Add here specific code to pend the database Mutex
	//>
	_mutex_lock(&database_mutex);
}


/** Unlock the stack mutex.
 *
 * SYSTEM_DatabaseMutexPost()
*	The BLE stack uses a MUTEX to prevent internal/external multiple access 
 *	to the Attribute protocol database, the BLE stack calls this function to 
 *  release the MUTEX. This function SHALL be implemented.
 *  Note that the User application does not call this function directly, but
 *  call the application interface ATT_Server_ReleaseDatabaseAccess().
 *
 *	It could be implemented using a mutex or perhaps a semaphore
 *
 * @todo implement this function
 *
 * @see SYSTEM_DatabaseMutexPend()
 *
 * @return none
 *	
 * @author Alexandre GIMARD
 */
void SYSTEM_DatabaseMutexPost(void){
	// Add here specific code to post the stack Mutex
	//>
    _mutex_unlock(&database_mutex);
}
#endif //(BLE_STACK_SKIP_MUTEX_FUNCTIONS == 0)
 /***************************************************************************\
 * Timer Operation
 \***************************************************************************/

/** The stack request a timer creation
 *
 * SYSTEM_TimerCreate()
 *	The BLE stack need one timer in order to calulates timeout or wachdog.
 *  It uses this function to create the timer, timers are internally
 *  multiplexed, so it need only one.
 *
 * @todo implement this function
 *
 * @see SYSTEM_TimerCancel() , SYSTEM_TimerGetRemain()
 *
 * @param func : pointer to a function to call when the timer expires
 * @param param : param to pass in func when the timer expire
 * @param millisecond : time to wait before the timer expiration
 *
 * @return none
 *	
 * @author Alexandre GIMARD
 */
void SYSTEM_CreateTimer(void *func, void *param, U32 millisecond){
	// Add here specific code to create the stack timer
	//> 
    _timer_create_component(TIMER_TASK_PRI, TIMER_STACK_SIZE);
    timer_id1 = _timer_start_periodic_every(func, param, TIMER_ELAPSED_TIME_MODE, millisecond);
}


/** The stack request its timer destruction
 *
 * SYSTEM_CancelTimer()
 *	The BLE stack no more need timer. it request destruction of the timer
 * created with SYSTEM_CreateTimer().
 *
 * @todo implement this function
 *
 * @see SYSTEM_CreateTimer() , SYSTEM_TimerGetRemain()
 *
 * @return none
 *	
 * @author Alexandre GIMARD
 */
void SYSTEM_CancelTimer(void){
	// Add here specific code to cancel the stack timer
	//>
    _timer_cancel(timer_id1);
}

/** The stack request the timer remainting time
 *
 * SYSTEM_TimerGetRemain()
 *	The BLE stack need the remaining time of the timer created by
 *  SYSTEM_CreateTimer() in order to do some internal things
 *
 * @todo implement this function
 *
 * @see SYSTEM_CreateTimer() , SYSTEM_CancelTimer()
 *
 * @return the time remaining in millisecond.
 *	
 * @author Alexandre GIMARD
 */
U32 SYSTEM_TimerGetRemain(void){
	// Add here specific code to get the millisecond
	// remaining in the the stack timer
	//>	
	//**

}


/***************************************************************************\
 * Security Operation
\***************************************************************************/
//#if (BLE_SECURITY == 1)


#define SLOT_SIZE	128
U8 memoryDump[SLOT_SIZE];

/** The stack request the system to store an information in persistent memory
 *
 * SYSTEM_SetPersistentInformation()
 *	The BLE stack need to save a specific security information with a specified remote
 * device in the persistent memory.
 *
 * @param addr : the remote address for which security information should be stored,
 *			IMPORTANT NOTE: if BD_ADDR is 00:00:00:00:00:00, all the entry of the 
 *			persistent memory shall be updated.
 * @param infoType : the type of the information to store
 * @param infoValue : a valid pointer to the information to save
 * @param infoLen : the length the information to save
 *
 * @return the status of the operation
 *	
 * @author Alexandre GIMARD
 */
BleStatus SYSTEM_SetPersistentInformation(BD_ADDR *addr, U8 infoType, U8* infoValue, U8 InfoLen){

	U8 i;
	U8 offset;

	#if 1

	// This implementation save the information in a RAW format like
	// [Address][Infotype][infotypeLen][infoValue][Infotype][infotypeLen][infoValue]

#if (BLE_PRINT_DEBUG_TRACES == 1)
	SYSTEM_Log("\n[SYSTEM] Try to SET infotype [%d] information in FLASH \n", infoType);
#endif //(BLE_PRINT_DEBUG_TRACES == 1)

	
	// Dump persistent memory slot into RAM
	/*APIs in MQX*/
	fseek(flash_file, 0, IO_SEEK_END);
	fseek(flash_file, -SLOT_SIZE, IO_SEEK_END);
	ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
	if(read(flash_file, memoryDump, SLOT_SIZE) == 0)
	{
		return BLESTATUS_FAILED;
	}
	/*
	FLASH_ReadData(SLOT_ADDRESS,memoryDump,SLOT_SIZE );
	*/
	// first check that the memory has already been written
	// The module memory contains 0xFF after erasure
	if( (memoryDump[0] == 0xFF) && (memoryDump[1] == 0xFF) && (memoryDump[2] == 0xFF)
		&& (memoryDump[3] == 0xFF) && (memoryDump[4] == 0xFF) && (memoryDump[5] == 0xFF)){
		// new memory
		SYSTEM_MemSet(memoryDump, 0, SLOT_SIZE);
		SYSTEM_MemCpy(memoryDump, addr->addr, 6);

	}

	// Now we got the memory dump parse it
	if( (addr->addr[0] == 0) && (addr->addr[1] == 0) && (addr->addr[2] == 0)
		&& (addr->addr[3] == 0) && (addr->addr[4] == 0) && (addr->addr[5] == 0)){
		// All entries should be updated

	} else if( SYSTEM_MemCmp(memoryDump, addr->addr, 6) == 0){
		// The first 6 bytes in memory Dump are BDADDR	
		// The existing entry should be updated

	} else {
		// This is a new entry
		// As in this implementation, we save only one record
		// Clear All the previous entry to not have mixed keys
		SYSTEM_MemSet(memoryDump, 0, SLOT_SIZE);
		SYSTEM_MemCpy(memoryDump, addr->addr, 6);
	}

	// Parse  the tuples [Infotype][infotypeLen][infoValue] to see if the infotype is
	// already in the persistent memory
	offset = 6;
	while (1) {
		U8 memInfoType    = memoryDump[offset];
		U8 memInfoTypeLen = memoryDump[offset+1];
		if( memInfoType == infoType ){
			// OK it was already present in the database
			// Our implementation erase it
			// And it will be above added into the database
			// Erase it means shift the memory dump for memInfoTypeLen + 2 byte
			for(i = offset+memInfoTypeLen+2; i < SLOT_SIZE; i++){
				memoryDump[i-memInfoTypeLen-2] = memoryDump[i];
			}
			// fill the last non initialized part (SLOT_SIZE - memInfoTypeLen - 2) -> SLOT_SIZE 
			// with 0
			SYSTEM_MemSet(&memoryDump[SLOT_SIZE - memInfoTypeLen - 2],
				0,
				memInfoTypeLen + 2 );

		} else if( memInfoType == 0 ) {
			// it's means that we reach the end of the records
			// check that we have enough room
			if( (offset + InfoLen + 2) > SLOT_SIZE ){
#if (BLEERROR_HANDLER == 1)
				BleError_CatchException(BLEERROR_LEVEL_CRASH,
					BLEERROR_TYPE_SYSTEM, BLEERROR_CODE_GENERAL, 0);
#endif //(BLEERROR_HANDLER == 1)
				return BLESTATUS_FAILED;
			}
			//OK, so we can add our record here (at offset)
			memoryDump[offset] = infoType;
			memoryDump[offset+1] = InfoLen;
			SYSTEM_MemCpy(memoryDump+offset+2, infoValue, InfoLen);
			break; // out of the while
		} else {
			// we found an infotype that is not corresponding to the given one
			offset += memInfoTypeLen + 2;
			if( offset >= SLOT_SIZE ){
#if (BLEERROR_HANDLER == 1)
				// The memory is already full of other InfoType
				BleError_CatchException(BLEERROR_LEVEL_CRASH,
					BLEERROR_TYPE_SYSTEM, BLEERROR_CODE_GENERAL, 0);
#endif //(BLEERROR_HANDLER == 1)
				return BLESTATUS_FAILED;
			}
		}
	}

	// Dump Back the RAM content into persistent information
	//FLASH_WriteData(SLOT_ADDRESS,memoryDump,SLOT_SIZE);
	//return BLESTATUS_SUCCESS;

	/*write APIs in MQX*/
	fseek(flash_file, 0, IO_SEEK_END);
	fseek(flash_file, -SLOT_SIZE, IO_SEEK_END);
	if(write(flash_file, memoryDump, SLOT_SIZE) == 0)
	{
		return BLESTATUS_FAILED;
	}
	/*
	if(FLASH_WriteData(SLOT_ADDRESS,memoryDump,SLOT_SIZE) == 0)
	{
		return BLESTATUS_FAILED;
	}*/

	#endif
	return BLESTATUS_SUCCESS;
}

/** The stack request the system to retrieve an information from persistent memory
 *
 * SYSTEM_GetPersistentInformation()
 *	The BLE stack need to retrieve a specific security information 
 *	from the persistent memory.
 * Important note: The persistent information SHALL have been previously set by
 * SYSTEM_SetPersistentInformation() API, if the informat                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           ion has been never set this 
 * function shall answer BLESTATUS_FAILED.
 *
 * @param addr : the remote address for which security information shall be retrieved
 * @param infoType : the type of the information to retrieve
 * @param infoValue : the value of the information retrieved
 * @param infoLen : a the length the information retrieved
 *
 * @return the status of the operation
 *	
 * @author Alexandre GIMARD
 */
BleStatus SYSTEM_GetPersistentInformation(BD_ADDR *addr, U8 infoType, U8 **infoValue,U8 *infoLen){

	#if 1
	U8 offset;

	// This implementation save the information in a RAW format like
	// [Address][Infotype][infotypeLen][infoValue][Infotype][infotypeLen][infoValue]

#if (BLE_PRINT_DEBUG_TRACES == 1)
	SYSTEM_Log("\n[SYSTEM] Try to GET infotype [%d] information in FLASH \n", infoType);
#endif //(BLE_PRINT_DEBUG_TRACES == 1)

	// Dump persistent memory slot into RAM
	//FLASH_ReadData(SLOT_ADDRESS,memoryDump,SLOT_SIZE );

	/*
	if(FLASH_ReadData(SLOT_ADDRESS,memoryDump,SLOT_SIZE) == 0)
		return BLESTATUS_FAILED;
	*/
	/*read flash APIs in MQX*/
	fseek(flash_file, 0, IO_SEEK_END);
	fseek(flash_file, -SLOT_SIZE, IO_SEEK_END);
	ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
	if(read(flash_file, memoryDump, SLOT_SIZE) == 0)
		return BLESTATUS_FAILED;

	if( SYSTEM_MemCmp(memoryDump, addr->addr, 6) != 0){
		// The first 6 bytes in memory Dump are BDADDR	
		// The given Bluetooth Address is not the one saved in persistent memory
		return BLESTATUS_FAILED;
	} 

	// Here the address is correct
	// Parse  the tuples [Infotype][infotypeLen][infoValue] to see if the infotype is
	// in the persistent memory
	offset = 6;
	while (1) {
		U8 memInfoType    = memoryDump[offset];
		U8 memInfoTypeLen = memoryDump[offset+1];
		if( memInfoType == infoType ){
			// OK it was already present in the database
			*infoValue = (U8 *) &memoryDump[offset+2];
			*infoLen = memoryDump[offset+1];
	return BLESTATUS_SUCCESS;

		} else if( memInfoType == 0 ) {
			// it's means that we reach the end of the records
			// without found any records corresponding to the given infoType
				return BLESTATUS_FAILED;
		} else {
			// we found an infotype that is not corresponding to the given one
			offset += memInfoTypeLen + 2;
			if( offset >= SLOT_SIZE ){
				// we reach the end of the slot without founding anything
				return BLESTATUS_FAILED;
			}
		}
	}
    //unreachable:
	#endif
	return BLESTATUS_SUCCESS;
}

/** The stack request the system to remove an information from persistent memory
 *
 * SYSTEM_RemovePersistentInformation()
 *	 The BLE stack need to remove a specific persistent information 
 *	from the persistent memory. It request the remove a range a infotype
 *   The stack call this function when the Application wants to unbond with a
 *  remote device or when it detect that profile information is no more valid
 *   Also, an application may delete the linkUp information for a given
 *  profile by calling this function.
 *
 *  addr :  A valid pointer to the remote address for which persistent 
 *		    information shall be remove
 *  infoTypeFrom : the starting range of the infoType to delete, this 
 *          infoTypeFrom is included into the range to delete
 *  infoTypeTo : the ending range of the infoType to delete, this 
 *          infoTypeTo is included into the range to delete
 *
 * return the status of the operation
 */
BleStatus SYSTEM_RemovePersistentInformation(BD_ADDR *addr, U8 infoTypeFrom, U8 infoTypeTo){
	// This implementation save the information in a RAW format like
	// [Address][Infotype][infotypeLen][infoValue][Infotype][infotypeLen][infoValue]
	#if 1
#if (BLE_PRINT_DEBUG_TRACES == 1)
	SYSTEM_Log("\n[SYSTEM] Try to REMOVE infotype [%d-%d] information in FLASH \n",
	    infoTypeFrom, 
	    infoTypeTo);  
#endif //(BLE_PRINT_DEBUG_TRACES == 1)

	// Dump persistent memory slot into RAM

	/*
	//FLASH_ReadData(SLOT_ADDRESS,memoryDump,SLOT_SIZE );
	if(FLASH_ReadData(SLOT_ADDRESS,memoryDump,SLOT_SIZE ) == 0)
			return BLESTATUS_FAILED;
	*/
	/*Read flash APIs in MQX*/
	fseek(flash_file, 0, IO_SEEK_END);
	fseek(flash_file, -SLOT_SIZE, IO_SEEK_END);
	ioctl(flash_file, FLASH_IOCTL_ENABLE_SECTOR_CACHE, NULL);
	if(read(flash_file, memoryDump, SLOT_SIZE) == 0)
		return BLESTATUS_FAILED;

	if( SYSTEM_MemCmp(memoryDump, addr->addr, 6) != 0){
		// The first 6 bytes in memory Dump are BDADDR	
		// The given Bluetooth Address is not the one saved in persistent memory
		return BLESTATUS_FAILED;
	} 

	if( (infoTypeFrom == 0x01) && (infoTypeTo == 0xFF) ){
		// means that all the infotype need to be removed, it is a complete UNBOND
		// mainly call by the stack after an UNBOND
		SYSTEM_MemSet(memoryDump, 0, SLOT_SIZE);
		SYSTEM_MemCpy(memoryDump, addr->addr, 6);	
	} else {
	 	// Only a range of infotype need to be removed,
		// Mainly call by an application to remove a profile information
		// Parse  the tuples [Infotype][infotypeLen][infoValue] to see if the infotype is
		// already in the persistent memory
		U8 memInfoType;
		U8 offset;
		
		offset = 6;

		// Get the first InfoType
		memInfoType = memoryDump[offset];

		while ( memInfoType != 0 ) {
			// The infotype exist, get its len
			U8 memInfoTypeLen = memoryDump[offset+1];

			if( (memInfoType >= infoTypeFrom) && (memInfoType <= infoTypeTo)){
				// OK an infootype inside the given range is present in the database
				// We shall erase it
				// Erase it means shift the memory dump for memInfoTypeLen + 2 byte
				U8 i;

				for(i = offset+memInfoTypeLen+2; i < SLOT_SIZE; i++){
					memoryDump[i-memInfoTypeLen-2] = memoryDump[i];
				}
				// fill the last non initialized part (SLOT_SIZE - memInfoTypeLen - 2) -> SLOT_SIZE 
				// with 0
				SYSTEM_MemSet(&memoryDump[SLOT_SIZE - memInfoTypeLen - 2],
					0,
					memInfoTypeLen + 2 );
	
			} else {
				// we found an infotype that is not corresponding to the given range
				offset += memInfoTypeLen + 2;
			}

			// Get the next InfoType
			memInfoType = memoryDump[offset];
		}
	}

	// Dump Back the RAM content into persistent information
	/*
	if(FLASH_WriteData(SLOT_ADDRESS,memoryDump,SLOT_SIZE) == 1)
		return BLESTATUS_SUCCESS;
	else
		return BLESTATUS_FAILED;
	*/
//	FLASH_WriteData(SLOT_ADDRESS,memoryDump,SLOT_SIZE);
	/*write flash APIs in MQX*/
	fseek(flash_file, 0, IO_SEEK_END);
	fseek(flash_file, -SLOT_SIZE, IO_SEEK_END);
	if(write(flash_file, memoryDump, SLOT_SIZE) == 0)
	{
		return BLESTATUS_FAILED;
	}
#endif
	return BLESTATUS_SUCCESS;
}

//#endif //(BLE_SECURITY == 1)


/***************************************************************************\
 * Logging Operation
\***************************************************************************/
#if (BLE_PRINT_DEBUG_TRACES == 1)

void SYSTEM_Log(char *format,...){

	// Add here specific code to print output argumented string  
	// 
	//>	
	//UNUSED_PARAMETER(format);
	printf(format);
}

#endif //(BLE_PRINT_DEBUG_TRACES == 1)

