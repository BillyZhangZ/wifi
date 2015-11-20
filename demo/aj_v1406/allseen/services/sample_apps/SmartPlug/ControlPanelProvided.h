/******************************************************************************
 * Copyright (c) 2013 - 2014, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#ifndef CONTROLPANELPROVIDED_H_
#define CONTROLPANELPROVIDED_H_

#include "alljoyn.h"
#include "alljoyn/controlpanel/Common/DateTimeUtil.h"
#include "ControlPanelGenerated.h"

const char* getNotificationString();
const char* getNotificationActionObjPath();

uint16_t isThereANotificationToSend();

uint16_t checkForUpdatesToSend();

////////////////////////For Smart Plug////////////////////////////////
void* getCurrentPlugMode();
void setCurrentPlugMode(uint16_t newMode);
void* getPlugStatusString();
void setPlugStatusString(const char* newStatusString);

void* getCurrentPowerSaveMode();
void* getPowerSaveStatusString();

void* getCurrentActivePowerString();
void setCurrentActivePowerString(char const* power);

void* getCurrentReactivePowerString();
void setCurrentReactivePowerString(char const* power);

void* getCurrentTotalPowerString();
void setCurrentTotalPowerString(char const* power);

void* getTotalActivePowerSumString();
void setTotalActivePowerSumString(char const* power);

void* getCurrentGridFreqString();
void setCurrentGridFreqString(char const* gridFreq);

void* getHistoryRunTimeString();
void setHistoryRunTimeString(char const* histruntime);

#if 0

void setCurrentActivePower(uint8_t * Actpower, int len);

void setCurrentReactivePower(uint8_t * Reactpower, int len);

void setCurrentTotalPower(uint8_t * Totalpower, int len);

void setTotalActivePowerSum(uint8_t * Actpowersum, int len);
#else

void setPowerValue(uint8_t * power_buf, int len, int type);

#endif

void setActualPlugONOFFStatus(uint8_t * actualStatus, int len);


void setCurrentGridFreq(uint8_t * freq_str, int len);

void setHistoryRunTime(uint8_t * hisTime_str, int len);

void* getWorkTimerONOFF();
void setWorkTimerONOFF(uint16_t onoff);

void* getStartFlipTimevar();

void setStartFlipTimevar(TimePropertyValue *newValue);

void* getEndFlipTimevar();

void setEndFlipTimevar(TimePropertyValue *newValue);

void* getRTCDatevar();

void setRTCDatevar(DatePropertyValue *newValue);

void* getRTCTimevar();

void setRTCTimevar(TimePropertyValue *newValue);
#endif /* CONTROLPANELPROVIDED_H_ */

