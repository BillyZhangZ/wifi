#ifndef __BLENEXTDSTCHANGE_SERVICE_H
#define __BLENEXTDSTCHANGE_SERVICE_H
/****************************************************************************
 *
 * File:          BleNextDSTChange_Service.h
 *
 * Description:   Contains routines declaration for Next DST Change Service.
 * 
 * Created:       November, 2011
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.3 $
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
#include "BleTimeTypes.h"

/***************************************************************************\
 *	Type definition
\***************************************************************************/

/***************************************************************************\
 * OPTIONAL FEATURES
\***************************************************************************/
/**
 * BLE_SUPPORT_NEXTDSTCHANGE_SERVICE
 *	Defines if the BLE local device enables access to the local  
 *	Next DST Change service and characteristic.
 * 
 * If enabled ( set to 1 ) it enables Profiles to get access to the  
 * Next DST Change Service and characteristic
 *
 * The default value for this option is disabled (0).
 */
#ifndef BLE_SUPPORT_NEXTDSTCHANGE_SERVICE
#define BLE_SUPPORT_NEXTDSTCHANGE_SERVICE								0
#endif //BLE_SUPPORT_NEXTDSTCHANGE_SERVICE

/***************************************************************************\
 * CONSTANTS
\***************************************************************************/


/***************************************************************************\
 *	Type definition
\***************************************************************************/
/** 
 * BleNextDSTChangeServiceEvent type
 * Define the different kind of events that could be received by the 
 * BleNextDSTChangeServiceCallBack
 */
typedef U8 BleNextDSTChangeServiceEvent;

/** BLENEXTDSTCHANGESERVICE_EVENT_TIMEWITHDSTREQUEST
 * Event received when the date of the Next DST change is requested from
 *  the server.
 * When the remote device request the time of the next DST change
 * the server shall answer with the exact date of the DST change. It should 
 * answer as soon as possible, because it exists a timeout.
 * The parms field is a BleNextDSTChangeTimeRequest structure, the 
 * TimeWithDST field shall represent the Time with the next DST change and the
 * next DST the next DSToffset when exiting of the callback.
 */
#define BLENEXTDSTCHANGESERVICE_EVENT_TIMEWITHDSTREQUEST			0xC0


/** 
 * BleNextDSTChangeServiceTimeRequest type
 * Defines the parameters received in the BleNextDSTChangeServiceCallBack during  
 * the BLENEXTDSTCHANGESERVICE_EVENT_TIMEREQUEST event.
 *
 * it contains:
 * timeWithDST: a valid currentDateTime pointer provided by the stack 
 *			where the application shall write the next dst change date time
 * nextDSTOffset : a valid BleTimeFraction256 pointer provided by the stack 
 *			where the application shall write, the next dst change
 */
typedef struct{
	BleDateTime						*timeWithDST;
	BleTimeDSTOffset				*nextDSTOffset;
} BleNextDSTChangeServiceTimeRequest;



/**
 * BleNextDSTChangeServiceCallBack
 *	This callback receives the NEXT DST CHANGE Service events. 
 *  Each events may be assiociated with specified status and parameters.
 *  The callback is executed during the stack context,  be careful to
 *	 not doing heavy process in this function.
 */
typedef void (*BleNextDSTChangeServiceCallBack)(
					BleNextDSTChangeServiceEvent event,
					BleStatus status,
					void *parms); 


/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/
#if (BLE_SUPPORT_NEXTDSTCHANGE_SERVICE == 1)
#endif //(BLE_SUPPORT_NEXTDSTCHANGE_SERVICE == 1)

#endif //__BLENEXTDSTCHANGE_SERVICE_H
