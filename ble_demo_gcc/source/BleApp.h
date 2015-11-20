#ifndef __BLEAPP_H
#define __BLEAPP_H
/****************************************************************************
 *
 * File:          BleApp.h
 *
 * Description:   Contains main Low energy Application function
 * 					declaration.
 *
 * Created:       March, 2010
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.1 $
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
 * Constant
\***************************************************************************/

/***************************************************************************\
 * types
\***************************************************************************/
typedef U16 AppRequest;
typedef U16 AppState;

/***************************************************************************\
 * External Functions
\***************************************************************************/

BleStatus 	BLEAPP_Init(void); 
AppRequest	BLEAPP_Scheduling(void); 
BleStatus   BLEAPP_ReadyToGoToSleep( void );

#endif //__BLEAPP_H
