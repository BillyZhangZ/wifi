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

//////////////////////////////////////////////////////
const char* getNotificationString();
const char* getNotificationActionObjPath();
uint16_t isThereANotificationToSend();

void* getCurrentTemperatureString();
void setCurrentTemperatureString(char const* temp);
void* getCurrentHumidityString();
void setCurrentHumidityString(char const* humidity);
void* getCurrentPM2_5String();
void setCurrentPM2_5String(char const* PM2_5);
void* getCurrentAccelerationString();
void setCurrentAccelerationString(char const* Acc);
void* getCurrentRotationString();
void setCurrentRotationString(char const* Acc);
void* getCurrentMagneticString();
void setCurrentMagneticString(char const* Acc);

/*************** API for SET Sensor Infomation ****************/
void setCurrentTemperature(uint32_t temp);
void setCurrentHumidity(uint32_t humidity);
void setCurrentPM2_5(uint32_t pm2_5);
void setCurrentAcceleration(uint32_t acceleration);
void setCurrentRotation(uint32_t rotation);
void setCurrentMagnetic(uint32_t magnetic);


void onTurnFanOn(ExecuteActionContext* context, uint8_t accepted);
void onTurnFanOff(ExecuteActionContext* context, uint8_t accepted);

//void simulateTemperatureChange();
uint16_t checkForUpdatesToSend();
uint8_t checkForEventsToSend();

#endif /* CONTROLPANELPROVIDED_H_ */

