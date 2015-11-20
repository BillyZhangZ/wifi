#ifndef __BLETIMETYPES_H
#define __BLETIMETYPES_H
/****************************************************************************
 *
 * File:          BleTimeTypes.h
 *
 * Description:   Contains global types definition for time representation.
 * 
 * Created:       October, 2011
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


/***************************************************************************\
 *	Type definition
\***************************************************************************/


/** 
 * BleDateTime type
 * Structure to represent the date Time information
 * It contains the year, month, day, hours, minutes and seconds information
 */
typedef struct {

	/* year is the Year as defined by the Gregorian calendar. 
	 * Valid range 1582 to 9999.
	 * A value of 0 means that the year is not known
	 */
	U16			year;

	/* month is the Month of the year as defined by the Gregorian calendar.
	 * Valid range 1 (January) to 12 (December). 
	 * A value of 0 means that the month is not known
	 */
	U8			month;

	/* day is Day of the month as defined by the Gregorian calendar.
	 * Valid range 1 to 31. A value of 0 means that the day of month
	 * is not known. 
	 */
	U8			day;

	/* hours is Number of hours past midnight.
	 * Valid range 0 to 23.  
	 */
	U8			hours;

	/* minutes is Number of minutes since the start of the hour.
	 * Valid range 0 to 59.  
	 */
	U8			minutes;

	/* seconds is Number of seconds since the start of the minute.
	 * Valid range 0 to 59.  
	 */
	U8			seconds;

} BleDateTime;


/** 
 * BleDayOfWeek type
 * BleDayOfWeek is Day of the week.
 * Valid range 1 (Monday) to 7 (Sunday). 
 * A value of 0 means that the day of Week
 * is not known. 
 */
typedef U8 BleDayOfWeek;

/** 
 * BleTimeFraction256 type
 * BleTimeFraction256 is the 1/256 fraction of second.
 * A value of 0 means that the BleTimeFraction256
 * is not known. 
 */
typedef U8 BleTimeFraction256;

/** 
 * BleDayDateTime type
 * Structure to represent the day date Time information
 * It contains the day of week and the date time information
 */
typedef struct {

	BleDayOfWeek		dayOfWeek;
	BleDateTime			dateTime;

} BleDayDateTime;


/** 
 * BleTimeZone type
 * BleTimeZone defines the Time Zone from the Coordinated universal time (UTC)
 * valid range is -48 (UTC-12:00) to 56 (UTC+14:00)
 * BLETIMEZONE_UNKNOWN means that the timeZone is not known
 *
 *  +-------+-----------+    +-------+-----------+    +-------+-----------+
 *  | value |   means   |    | value |   means   |    | value |   means   |
 *  +-------+-----------+    +-------+-----------+    +-------+-----------+
 *  | -48   | UTC-12:00 |    | -04   | UTC-1:00  |    | +32   | UTC+8:00  |	
 *  | -44   | UTC-11:00 |    | -00   | UTC+0:00  |    | +35   | UTC+8:45  |	
 *  | -40   | UTC-10:00 |    | +04   | UTC+1:00  |    | +36   | UTC+9:00  |	
 *  | -38   | UTC-9:30  |    | +08   | UTC+2:00  |    | +38   | UTC+9:30  |	
 *  | -36   | UTC-9:00  |    | +12   | UTC+3:00  |    | +40   | UTC+10:00 |	
 *  | -32   | UTC-8:00  |    | +14   | UTC+3:30  |    | +42   | UTC+10:30 |	
 *  | -28   | UTC-7:00  |    | +16   | UTC+4:00  |    | +44   | UTC+11:00 |  	
 *  | -24   | UTC-6:00  |    | +18   | UTC+4:30  |    | +46   | UTC+11:30 |  	
 *  | -20   | UTC-5:00  |    | +20   | UTC+5:00  |    | +48   | UTC+12:00 |  	
 *  | -18   | UTC-4:30  |    | +22   | UTC+5:30  |    | +51   | UTC+12:45 |  	
 *  | -16   | UTC-4:00  |    | +23   | UTC+5:45  |    | +52   | UTC+13:00 |  	
 *  | -14   | UTC-3:30  |    | +24   | UTC+6:00  |    | +56   | UTC+14:00 |
 *  | -12   | UTC-3:00  |    | +26   | UTC+6:30  |    +-------+-----------+ 
 *  | -08   | UTC-2:00  |    | +28   | UTC+7:00  |    
 *  +-------+-----------+    +-------+-----------+    
 */
typedef S8 BleTimeZone;
#define BLETIMEZONE_UNKNOWN				-128


/** 
 * BleTimeDSTOffset type
 * BleTimeDSTOffset defines the current Dayligth Saving Time offset.
 * Available possible value are
 * BLETIMEDSTOFFSET_STANDARD: 		Standard Time
 * BLETIMEDSTOFFSET_HALFANHOUR: 	Half An Hour Daylight Time (+0.5h)
 * BLETIMEDSTOFFSET_ONEHOUR: 		Daylight Time (+1h)
 * BLETIMEDSTOFFSET_TWOHOURS: 		Double Daylight Time (+2h)
 * BLETIMEDSTOFFSET_UNKNOWN: 		the DST is not known
 */
typedef U8 BleTimeDSTOffset;

#define BLETIMEDSTOFFSET_STANDARD		0x00
#define BLETIMEDSTOFFSET_HALFANHOUR		0x02
#define BLETIMEDSTOFFSET_ONEHOUR		0x04
#define BLETIMEDSTOFFSET_TWOHOURS		0x08
#define BLETIMEDSTOFFSET_UNKNOWN		0xFF


/** 
 * BleTimeSource type
 * BleTimeSource defines the time update source.
 * Available possible value are
 *  +--------------------------------------+-----------------------+    
 *  | value                                |   means               |    
 *  +--------------------------------------+-----------------------+    
 *  | BLETIMESOURCE_UNKNWOWN               | Unknown               |    
 *  | BLETIMESOURCE_NETWORKTIMEPROTOCOL    | Network Time Protocol |    
 *  | BLETIMESOURCE_GPS                    | GPS                   |    
 *  | BLETIMESOURCE_RADIOSIGNAL            | Radio Time Signal     |    
 *  | BLETIMESOURCE_MANUAL                 | Manual                |    
 *  | BLETIMESOURCE_ATOMICCLOCK            | Atomic Clock          |    
 *  | BLETIMESOURCE_CELLULARNETWORK        | Cellular Network      |       
 *  +--------------------------------------+-----------------------+  
 */
typedef U8 BleTimeSource;

#define BLETIMESOURCE_UNKNWOWN					0
#define BLETIMESOURCE_NETWORKTIMEPROTOCOL		1
#define BLETIMESOURCE_GPS						2
#define BLETIMESOURCE_RADIOSIGNAL				3
#define BLETIMESOURCE_MANUAL					4
#define BLETIMESOURCE_ATOMICCLOCK				5
#define BLETIMESOURCE_CELLULARNETWORK			6				



#endif //__BLETIME_CLIENT_H
