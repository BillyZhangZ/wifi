#ifndef __ATT_H__
#define __ATT_H__
/****************************************************************************
 *
 * File:          Att.h
 *
 * Description:   API for the attribute protocol
 * 
 * Created:       June, 2008
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

/**
 * @file Att.h
 *
 * API for the attribute protocol.
 *
 * @author xavier.boniface@alpwise.com
 */
#include "ATT/AttConfig.h"

#if (ATTRIBUTE_PROTOCOL == 1)

/* Includes common to both the client and the server role. */
#include "ATT/AttBase.h"

/* Specific includes for each role. */
#if (ATT_ROLE_SERVER == 1)
  #include "ATT/attserver.h"
#endif //(ATT_ROLE_SERVER == 1)

#if (ATT_ROLE_CLIENT == 1)
  #include "ATT/attclient.h"
#endif //(ATT_ROLE_CLIENT == 1)


/****************************************************************************\
 *	APPLICATION INTERFACE functions definition
\****************************************************************************/

/** Ask if the given 128 bit UUID is masked with the BLUETOOTH BASE UUID
 *
 * ATT_is128bitsUUIDBtMasked():
 *	Ask if the given UUID if made using the Bluetooth mask UUID
 *  0000xxxx-0000-1000-8000-00805F9B34FB
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 *
 * @param uuid	A valid pointer to a 128bit UUID
 *
 * @return The status of the comparaison:
 *	- BLESTATUS_SUCCESS indicates that the UUID is a BLUETOOTH based one
 *	- BLESTATUS_FAILED indicates that the UUID is a 128 bit specific UUID
 *	
 * @author Alexandre GIMARD
 */
BleStatus ATT_is128bitsUUIDBtMasked( const U8 *uuid);

/** Ask if the given  UUID is equal to the given 16 bits UUID
 *
 * ATT_Are16bitsUUIDEquals():
 *	Ask if the givenUuid is equal to the given 16 bits UUID.
 *  The given UUID may be a 128 bit UUID or a 16 bit UUID, if it is a
 *  128 bits uuid then the function checks that is bluetooth based and then
 *  compare the 16 part together.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 *
 * @param givenUuid	A valid pointer to an UUID
 * @param uuid	A 16 bit UUID to compare with givenUuid
 *
 * @return The status of the comparaison:
 *	- BLESTATUS_SUCCESS indicates that the UUID are equals
 *	- BLESTATUS_FAILED indicates that the UUID are not equals
 *	
 * @author Alexandre GIMARD
 */
BleStatus ATT_Are16bitsUUIDEquals(AttUuid *givenUuid, U16 uuid);

/** Ask if the given  UUID is equal to the given 128 bits UUID
 *
 * ATT_Are128bitsUUIDEquals():
 *	Ask if the givenUuid is equal to the given 128 bits UUID.
 *  The given UUID may be a 128 bit UUID or a 16 bit UUID, if it is a
 *  16 bits uuid then the function checks that the given is bluetooth based
 *  and then compare the 16 part together.
 *
 * ATTRIBUTE_PROTOCOL shall be enabled
 *
 * @param givenUuid	A valid pointer to an UUID
 * @param uuid	A 128 bit UUID to compare with givenUuid
 *
 * @return The status of the comparaison:
 *	- BLESTATUS_SUCCESS indicates that the UUID are equals
 *	- BLESTATUS_FAILED indicates that the UUID are not equals
 *	
 * @author Alexandre GIMARD
 */
BleStatus ATT_Are128bitsUUIDEquals(AttUuid *givenUuid, const U8* uuid);

#endif //(ATTRIBUTE_PROTOCOL == 1) 

#endif /* __ATT_H__ */
