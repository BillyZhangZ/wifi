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

/**
 * Per-module definition of the current module for debug logging.  Must be defined
 * prior to first inclusion of aj_debug.h.
 * The corresponding flag dbgAJSVCAPP is defined in the containing sample app.
 */
#define AJ_MODULE AJSVCAPP
#include <aj_debug.h>

#include "ControlPanelProvided.h"
#include "ControlPanelGenerated.h"
#include "alljoyn/controlpanel/Common/BaseWidget.h"

#ifndef snprintf
#include <stdio.h>
#endif

#ifndef NDEBUG
extern AJ_EXPORT uint8_t dbgAJSVCAPP;
#endif

/* Temperature */
static char currentTemperatureBuff[15] = "0 Degree";
static char* currentTemperatureString = currentTemperatureBuff;
static uint32_t currentTemperature = 0;
static uint32_t previousTemperature = 0;

/* Humidity */
static char currentHumidityBuff[15] = "0 W";
static char* currentHumidityString = currentHumidityBuff;
static uint32_t currentHumidity = 0;
static uint32_t previousHumidity = 0;

/* PM2.5 */
static char currentPM2_5Buff[15] = "0 W";
static char* currentPM2_5String = currentPM2_5Buff;
static uint32_t currentPM2_5 = 0;
static uint32_t previousPM2_5 = 0;

/* Acceleration */
static char currentAccelerationBuff[10] = "0 W";
static char* currentAccelerationString = currentAccelerationBuff;
static uint32_t currentAcceleration = 0;
static uint32_t previousAcceleration = 0;

/* Rotation */
static char currentRotationBuff[10] = "0 W";
static char* currentRotationString = currentRotationBuff;
static uint32_t currentRotation = 0;
static uint32_t previousRotation = 0;

/* Magnetic */
static char currentMagneticBuff[10] = "0 W";
static char* currentMagneticString = currentMagneticBuff;
static uint32_t currentMagnetic = 0;
static uint32_t previousMagnetic = 0;

static char notificationText[51] = "Notification text goes here";
static char* notificationString = notificationText;
static uint16_t sendANotification = 0;
static uint16_t signalsToSend = 0;
static uint8_t eventsToSend = 0;

static const char* notificationActionObjPath = NULL;

/*********** get/set Used in ControlPanelGenerated.c ***********/
/* Temperature */
void* getCurrentTemperatureString()
{
    currentTemperatureBuff[snprintf(currentTemperatureString, sizeof(currentTemperatureBuff), "%d C", currentTemperature)] = '\0';
    return &currentTemperatureString;
}
void setCurrentTemperatureString(char const* temp)
{
    //do nothing
}

/* Humidity */
void* getCurrentHumidityString()
{
    currentHumidityBuff[snprintf(currentHumidityString, sizeof(currentHumidityBuff), "%d %%RH", currentHumidity)] = '\0';
    return &currentHumidityString;
}
void setCurrentHumidityString(char const* humidity)
{
    //do nothing
}

/* PM2.5 */
void* getCurrentPM2_5String()
{
    currentPM2_5Buff[snprintf(currentPM2_5String, sizeof(currentPM2_5Buff), "%d ug", currentPM2_5)] = '\0';
    return &currentPM2_5String;
}
void setCurrentPM2_5String(char const* PM2_5)
{
    //do nothing
}

/* Acceleration */
void* getCurrentAccelerationString()
{
    currentAccelerationBuff[snprintf(currentAccelerationString, sizeof(currentAccelerationBuff), "%d Degree", currentAcceleration)] = '\0';
    return &currentAccelerationString;
}
void setCurrentAccelerationString(char const* Acc)
{
    //do nothing
}

/* Rotation */
void* getCurrentRotationString()
{
    currentRotationBuff[snprintf(currentRotationString, sizeof(currentRotationBuff), "%d Degree", currentRotation)] = '\0';
    return &currentRotationString;
}
void setCurrentRotationString(char const* Acc)
{
    //do nothing
}

/* Magnetic */
void* getCurrentMagneticString()
{
    currentMagneticBuff[snprintf(currentMagneticString, sizeof(currentMagneticBuff), "%d Degree", currentMagnetic)] = '\0';
    return &currentMagneticString;
}
void setCurrentMagneticString(char const* Acc)
{
    //do nothing
}


const char* getNotificationString()
{
    sendANotification = 0;
    return notificationString;
}

const char* getNotificationActionObjPath()
{
    const char* retObjPath = notificationActionObjPath;
    notificationActionObjPath = NULL;
    return retObjPath;
}

uint16_t isThereANotificationToSend()
{
    return sendANotification;
}

// -- for string properties -- //



// -- for widgets --//

/*************** API for SET Sensor Infomation ****************/
void setCurrentTemperature(uint32_t temp)
{
	currentTemperature = temp;
}

void setCurrentHumidity(uint32_t humidity)
{
	currentHumidity = humidity;
}

void setCurrentPM2_5(uint32_t pm2_5)
{
	currentPM2_5 = pm2_5;
}

void setCurrentAcceleration(uint32_t acceleration)
{
        currentAcceleration = acceleration; 
}

void setCurrentRotation(uint32_t rotation)
{
        currentRotation = rotation; 
}


void setCurrentMagnetic(uint32_t magnetic)
{
        currentMagnetic = magnetic; 
}

/**************************************************************/


void setTemperatureFieldUpdate() {
    signalsToSend |= 1 << 0;
}

void setHumidityFieldUpdate() {
    signalsToSend |= 1 << 1;
}

void setPM2_5FieldUpdate() {
    signalsToSend |= 1 << 2;
}

void setAccelerationFieldUpdate() {
    signalsToSend |= 1 << 3;
}

void setRotationFieldUpdate() {
    signalsToSend |= 1 << 4;
}

void setMagneticFieldUpdate() {
    signalsToSend |= 1 << 5;
}


void set80FReachedEvent() {
    eventsToSend |= 1 << 0;
}

void set60FReachedEvent() {
    eventsToSend |= 1 << 1;
}

void setTurnedOffEvent() {
    eventsToSend |= 1 << 2;
}

void setTurnedOnEvent() {
    eventsToSend |= 1 << 3;
}


static void addDismissSignal(ExecuteActionContext* context, int32_t dismissSignal)
{
    context->numSignals = 1;
    context->signals[0].signalId = dismissSignal;
    context->signals[0].signalType = SIGNAL_TYPE_DISMISS;
}

#if 0
void onTurnFanOn(ExecuteActionContext* context, uint8_t accepted)
{
    if (accepted) {
        setCurrentMode(3);
    }
    addDismissSignal(context, MYDEVICE_NOTIFICATION_ACTION_TURNFANON_SIGNAL_DISMISS);
}

void onTurnFanOff(ExecuteActionContext* context, uint8_t accepted)
{
    if (accepted) {
        setCurrentMode(preFanMode);
    }
    addDismissSignal(context, MYDEVICE_NOTIFICATION_ACTION_TURNFANOFF_SIGNAL_DISMISS);
}
#endif


uint16_t checkForUpdatesToSend()
{
    
#if 0
        // check if the current plug mode has been changed & update accordingly
        if (currentPlugMode != previousPlugMode) {
          previousPlugMode = currentPlugMode;
          setPlugModeFieldUpdate();
          setPlugModeStatusFieldUpdate();
    
          //set_command_send(CMD_SET, cls_shiftor_opc(SMTPLG_CMD_CLS_APP, SMTPLG_CMD_OP_SET_PWR_ONOFF), &currentPlugMode, 1);
           if(currentPlugMode == 0){             //Turn off plug
    
            plugstatusText[snprintf(plugstatusString, sizeof(plugstatusText), "Plug Power OFF")] = '\0';
            notificationText[snprintf(notificationString, sizeof(notificationText), "Plug Power OFF")] = '\0';
            sendANotification = 1;
           }
           else if(currentPlugMode == 1){       //Turn on plug
    
            plugstatusText[snprintf(plugstatusString, sizeof(plugstatusText), "Plug Power ON ")] = '\0';
            notificationText[snprintf(notificationString, sizeof(notificationText), "Plug Power ON")] = '\0';
            sendANotification = 1;
    
           }
    
        }
        // check if the current plug power save mode has been changed & update accordingly
        if (currentPowerSaveMode != previousPowerSaveMode) {
          previousPowerSaveMode = currentPowerSaveMode;
          setPowerSaveFieldUpdate();
          setPowerSaveStatusFieldUpdate();
    
          //set_command_send(CMD_SET, cls_shiftor_opc(SMTPLG_CMD_CLS_APP, SMTPLG_CMD_OP_SET_PWRSAVE_MODE), &currentPowerSaveMode, 1);
           if(currentPowerSaveMode == 0){             //Turn off power save mode
           //set_wifi_sleep_mode(0);
            powersavestatusText[snprintf(powersavestatusString, sizeof(powersavestatusText), "PowerSave OFF ")] = '\0';
            notificationText[snprintf(notificationString, sizeof(notificationText), "PowerSave OFF ")] = '\0';
            sendANotification = 1;
           }
           else if(currentPowerSaveMode == 1){       //Turn on power save mode
        //set_wifi_sleep_mode(1);
            powersavestatusText[snprintf(powersavestatusString, sizeof(powersavestatusText), "PowerSave AUTO")] = '\0';
            notificationText[snprintf(notificationString, sizeof(notificationText), "PowerSave AUTO")] = '\0';
            sendANotification = 1;
           }
        }
    
        if (strcmp(currentActPowerBuff, previousActPowerBuff)) {
          strcpy(previousActPowerBuff, currentActPowerBuff);
          setCurrentActivePowerFieldUpdate();
        }  
        if (strcmp(currentReactPowerBuff, previousReactPowerBuff)) {
          strcpy(previousReactPowerBuff, currentReactPowerBuff);
          setCurrentReactivePowerFieldUpdate();
           }
        if(isTimeDiffernt(&fliptime, &previousFliptime)){   
          timeCopy(&previousFliptime, &fliptime);
          setStartTimingFieldUpdate();
    
          //set_command_send(CMD_SET, cls_shiftor_opc(SMTPLG_CMD_CLS_APP, SMTPLG_CMD_OP_SET_TIMER_BEGIN), &fliptime, sizeof(TimePropertyValue));
        }
    
        if(isTimeDiffernt(&end_fliptime, &previous_end_fliptime)){  
          timeCopy(&previous_end_fliptime, &end_fliptime);
          setEndTimingFieldUpdate();
    
          //set_command_send(CMD_SET, cls_shiftor_opc(SMTPLG_CMD_CLS_APP, SMTPLG_CMD_OP_SET_TIMER_END), &fliptime, sizeof(TimePropertyValue));
        }
        
        if(isDateDiffernt(&set_date, &previous_set_date)){  
          dateCopy(&previous_set_date, &set_date);
          setDateFieldUpdate();
    
          //set_command_send(CMD_SET, cls_shiftor_opc(SMTPLG_CMD_CLS_APP, SMTPLG_CMD_OP_SET_TIMER_END), &fliptime, sizeof(TimePropertyValue));
        }
 #endif  
    
            if( AJ_Sensors_Fetch(AJ_SENSOR_TEMPERATURE, &currentTemperature) == 0){
    
                if (currentTemperature != previousTemperature) {
                    previousTemperature = currentTemperature;
                    setTemperatureFieldUpdate();
    
                }
    
            }
    
            if( AJ_Sensors_Fetch(AJ_SENSOR_HUMIDITY, &currentHumidity) == 0){
    
                if (currentHumidity != previousHumidity) {
                    previousHumidity = currentHumidity;
                    setHumidityFieldUpdate();
    
                }
    
            }
        if( AJ_Sensors_Fetch(AJ_SENSOR_PM2_5, &currentPM2_5) == 0){
    
                if (currentPM2_5 != previousPM2_5) {
                    previousPM2_5 = currentPM2_5;
                    setPM2_5FieldUpdate();
    
                }
    
            }
    

    return signalsToSend;
}

uint8_t checkForEventsToSend()
{
    eventsToSend = 0;
 
#if 0  // masked for debug
    // 0x01 == need to send event 80F reached
    // 0x02 == need to send event 60F reached
    // 0x04 == need to send event mode turned off
    // 0x08 == need to send event mode turned on i.e. one of { auto, heat, cool, fan }

    if (currentTemperature > previousTemperature && currentTemperature == 80) {
        set80FReachedEvent();
    }
    if (currentTemperature < previousTemperature && currentTemperature == 60) {
        set60FReachedEvent();
    }
    if (currentMode != previousMode) {
        if (currentMode == 4) {
            setTurnedOffEvent();
        } else if (previousMode == 4) {
            setTurnedOnEvent();
        }
    }
#endif
    return eventsToSend;
}

