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

///////////////////////////Smart Plug///////////////////////////////
#include "fsl_smart_plug_cmd.h"
static char plugstatusText[] = "Plug Power OFF";
static char* plugstatusString = plugstatusText;
static uint16_t currentPlugMode = 1, currentPlugModeStatus = 0;
static uint16_t previousPlugMode = 0, previousPlugModeStatus = 0;

static char powersavestatusText[] = "PowerSave OFF ";
static char* powersavestatusString = powersavestatusText;
static uint16_t currentPowerSaveMode = 0;
static uint16_t previousPowerSaveMode = 0;

/* current Active Power */
static char currentActPowerBuff[15] = "0.0 W";
static char* currentActPowerString = currentActPowerBuff;
static uint32_t currentActPowerInt = 0, currentActPowerDec = 0;
static uint32_t previousActPowerInt = 0, previousActPowerDec = 0;

/* current Reactive Power */
static char currentReactPowerBuff[15] = "0.0 W";
static char* currentReactPowerString = currentReactPowerBuff;
static uint32_t currentReactPowerInt = 0, currentReactPowerDec = 0;
static uint32_t previousReactPowerInt = 0, previousReactPowerDec = 0;

/* current Total Power */
static char currentTotalpowerBuff[15] = "0.0 W";
static char* currentTotalpowerString = currentTotalpowerBuff;
static uint32_t currentTotalPowerInt = 0, currentTotalPowerDec = 0;
static uint32_t previousTotalPowerInt = 0, previousTotalPowerDec = 0;

/* Total Active Power Sum */
static char totalActpowerSumBuff[15] = "0.0 KW";
static char* totalActpowerSumString = totalActpowerSumBuff;
static uint32_t totalActpowerSumInt = 0, totalActpowerSumDec = 0;
static uint32_t previousActpowerSumInt = 0, previousActpowerSumDec = 0;

/* Grid Frequence */
static char gridfreqBuff[10] = "50 Hz";
static char* gridfreqString = gridfreqBuff;
static uint16_t gridfreq = 50, previousGridFreq = 0;

/* Work history run time*/
static char historyRunTimeBuff[10] = "0H 0M";
static char* historyRunTimeString = historyRunTimeBuff;
static uint16_t historyRunTime = 0, prevHistoryRunTime = 0, historyRunHour = 0, historyRunMin = 0;

static char notificationText[51] = "Notification text goes here";
static char* notificationString = notificationText;
static uint16_t sendANotification = 0;
static uint16_t signalsToSend = 0;

static const char* notificationActionObjPath = NULL;


TimePropertyValue fliptime, previousFliptime;
TimePropertyValue end_fliptime, previous_end_fliptime;
DatePropertyValue rtc_date, previous_rtc_date;
TimePropertyValue rtc_time, previous_rtc_time;

static uint16_t workTimerONOFF = 0, previous_workTimerONOFF = 0;
////////////////////////////////////////////////////////////////


/********************************** For Smart Plug ***********************************/

uint16_t isThereANotificationToSend()
{
    return sendANotification;
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

void* getCurrentPlugMode()
{
    return &currentPlugMode;
}

void setCurrentPlugMode(uint16_t newMode)
{
    currentPlugMode = newMode;            //ON or OFF
}

void* getPlugStatusString()
{
    return &plugstatusString;
}

void setPlugStatusString(const char* newStatusString)
{
    strncpy(plugstatusString, newStatusString, sizeof(plugstatusText));
    plugstatusString[30] = '\0';
}

void* getCurrentPowerSaveMode()
{
    return &currentPowerSaveMode;
}

void setCurrentPowerSaveMode(uint16_t newMode)
{
    currentPowerSaveMode = newMode;
}
      
void* getPowerSaveStatusString()
{
    return &powersavestatusString;
}

void setPowerSaveStatusString(const char* newStatusString)
{
    strncpy(powersavestatusString, newStatusString, sizeof(powersavestatusText));
    powersavestatusString[20] = '\0';
}

void* getCurrentActivePowerString()
{
    currentActPowerBuff[snprintf(currentActPowerString, sizeof(currentActPowerBuff), "%d.%d W", currentActPowerInt, currentActPowerDec)] = '\0';
    return &currentActPowerString;
}


void setCurrentActivePowerString(char const* power)
{
	//do nothing
}

void* getCurrentReactivePowerString()
{
  currentReactPowerBuff[snprintf(currentReactPowerString, sizeof(currentReactPowerBuff), "%d.%d W", currentReactPowerInt, currentReactPowerDec)] = '\0';
  return &currentReactPowerString;						
}

void setCurrentReactivePowerString(char const* power)
{
     //do nothing
}


void* getCurrentTotalPowerString()
{
    currentTotalpowerBuff[snprintf(currentTotalpowerString, sizeof(currentTotalpowerBuff), "%d.%d W", currentTotalPowerInt, currentTotalPowerDec)] = '\0';
    return &currentTotalpowerString;

}

void setCurrentTotalPowerString(char const* power)
{
    //do nothing
}

void* getTotalActivePowerSumString()
{
    totalActpowerSumBuff[snprintf(totalActpowerSumString, sizeof(totalActpowerSumBuff), "%d.%d KWH", totalActpowerSumInt, totalActpowerSumDec)] = '\0';
    return &totalActpowerSumString;

}

void setTotalActivePowerSumString(char const* power)
{
    //do nothing
}

void* getCurrentGridFreqString()
{
    gridfreqBuff[snprintf(gridfreqString, sizeof(gridfreqBuff), "%d Hz", gridfreq)] = '\0';
    return &gridfreqString;
}

void setCurrentGridFreqString(char const* gridFreq)
{
    //do nothing
}

void* getHistoryRunTimeString()
{
    historyRunTimeBuff[snprintf(historyRunTimeString, sizeof(historyRunTimeBuff), "%dH %dM", historyRunHour, historyRunMin)] = '\0';
    return &historyRunTimeString;
}

void setHistoryRunTimeString(char const* histruntime)
{
    //do nothing
}

void* getWorkTimerONOFF()
{
    return &workTimerONOFF;
}

void setWorkTimerONOFF(uint16_t onoff)
{
    workTimerONOFF = onoff;            //ON or OFF
}

void* getStartFlipTimevar()
{
    return &fliptime;
}

void setStartFlipTimevar(TimePropertyValue *newValue)
{
    fliptime.hour = newValue->hour;
    fliptime.minute= newValue->minute;
    fliptime.second= newValue->second;
}

void* getEndFlipTimevar()
{
    return &end_fliptime;
}

void setEndFlipTimevar(TimePropertyValue *newValue)
{
    end_fliptime.hour = newValue->hour;
    end_fliptime.minute= newValue->minute;
    end_fliptime.second= newValue->second;
}

void* getRTCDatevar()
{
    return &rtc_date;
}

void setRTCDatevar(DatePropertyValue *newValue)
{
    rtc_date.mDay = newValue->mDay;
    rtc_date.month= newValue->month;
    rtc_date.fullYear= newValue->fullYear;
}

void* getRTCTimevar()
{
    return &rtc_time;
}

void setRTCTimevar(TimePropertyValue *newValue)
{
    rtc_time.hour = newValue->hour;
    rtc_time.minute= newValue->minute;
    rtc_time.second= newValue->second;
}



/***********************************************************/
void setPowerValue(uint8_t * power_buf, int len, int type)
{
     uint32_t temp[2] = 0, i = 0, j;
     
     if(len < 4) return;
     
     for(j = 0; i = j/4, j < len; j++){ /* temp[0] store Integer part, temp[0] store decimal part */
       temp[i] <<=8;
       temp[i] |= power_buf[j];
     }

     switch(type)
     { 
       case 1:
         currentActPowerInt = temp[0];
         currentActPowerDec = temp[1];
         break;
       case 2:
         currentReactPowerInt = temp[0];
         currentReactPowerDec = temp[1];
         break;
       case 3:
         currentTotalPowerInt = temp[0];
         currentTotalPowerDec = temp[1];
         break;
       case 4:
         totalActpowerSumInt = temp[0];
         totalActpowerSumDec = temp[1];
         break;
       default:
         break;
     }
}

void setActualPlugONOFFStatus(uint8_t * actualStatus, int len)
{
  if(len < 1) return;
  currentPlugModeStatus= actualStatus[0];
}

void setCurrentGridFreq(uint8_t * freq_str, int len)
{
  if(len < 1) return;
  gridfreq = freq_str[0];
}

void setHistoryRunTime(uint8_t * hisTime_str, int len)
{
  uint32_t temp = 0 , j;

  if(len < 4) return;
  for(j = 0; j < len; j++){
    temp <<=8;
    temp |= hisTime_str[j];
  }
  historyRunTime = temp;
  historyRunHour = historyRunTime / 60; 
  historyRunMin =historyRunTime % 60;
}
/***********************************************************/

void setPlugModeFieldUpdate() {
    signalsToSend |= 1 << 0;
}

void setPlugModeStatusFieldUpdate() {
    signalsToSend |= 1 << 1;
}

void setPowerSaveFieldUpdate() {
    signalsToSend |= 1 << 2;
}

void setPowerSaveStatusFieldUpdate() {
    signalsToSend |= 1 << 3;
}

void setCurrentActivePowerFieldUpdate() {
    signalsToSend |= 1 << 4;
}

void setCurrentReactivePowerFieldUpdate() {
    signalsToSend |= 1 << 5;
}

void setCurrentTotalPowerFieldUpdate() {
    signalsToSend |= 1 << 6;
}

void setTotalActivePowerSumFieldUpdate() {
    signalsToSend |= 1 << 7;
}

void setCurrentGridPreqFieldUpdate() {
    signalsToSend |= 1 << 8;
}

void setHistoryRunTimeFieldUpdate() {
    signalsToSend |= 1 << 9;
}


void setWorkTimerONOFFFieldUpdate(){
    signalsToSend |= 1 << 10;
}	

void setStartTimingFieldUpdate() {
    signalsToSend |= 1 << 11;
}


void setEndTimingFieldUpdate() {
    signalsToSend |= 1 << 12;
}


void setRTCDateFieldUpdate() {
    signalsToSend |= 1 << 13;
}

void setRTCTimeFieldUpdate() {
    signalsToSend |= 1 << 14;
}


boolean isTimeDiffernt(TimePropertyValue * timePtr1,TimePropertyValue * timePtr2)
{
  boolean ret = TRUE;
  if((timePtr1->second == timePtr2->second)&&(timePtr1->minute == timePtr2->minute)\
  	&&(timePtr1->hour == timePtr2->hour)){
    ret = FALSE;
  }
  return ret;
}
void timeCopy(TimePropertyValue * destTimePtr,TimePropertyValue *srcTimePtr)
{
  memcpy(destTimePtr, srcTimePtr, sizeof(TimePropertyValue));
}

boolean isDateDiffernt(DatePropertyValue * datePtr1,DatePropertyValue * datePtr2)
{
  boolean ret = TRUE;
  if((datePtr1->mDay == datePtr2->mDay)&&(datePtr1->month == datePtr2->month)\
  	&&(datePtr1->fullYear == datePtr2->fullYear)){
    ret = FALSE;
  }
  return ret;
}

void dateCopy(DatePropertyValue * destDatePtr,DatePropertyValue *srcDatePtr)
{
  memcpy(destDatePtr, srcDatePtr, sizeof(DatePropertyValue));
}


uint16_t checkForUpdatesToSend()
{
    // this needs to be the brain
    uint16_t cmd_code = 0;   
    static SMTPLUG_TM start_time_temp = 0, end_time_temp = 0;
    static SMTPLUG_Date_Time rtc_timer_temp = 0;

    signalsToSend = 0;
    // 0001 == need to update the plug status text field
    // 0010 == need to update the power save mode text field
    // 0100 == need to update the work timing text field
    

    //modeOrTargetTempChanged = 0;

    //AJ_AlwaysPrintf(("In checkForUpdatesToSend, currentMode=%d, targetTemp=%d, currentTemperature=%d, fanSpeed=%d, triggerAnUpdate=%d \n", currentMode, targetTemp, currentTemperature, fanSpeed, triggerAnUpdate));
    //AJ_AlwaysPrintf(("In checkForUpdatesToSend, curPlugMode=%d, curPowerSaveMode=%d, currentActPower=%d, currentTotalPower=%d, currentTotalPower=%d \n", currentPlugMode, currentPowerSaveMode, currentActPower, currentReactPower, currentTotalPower));
    //AJ_AlwaysPrintf(("totalActpowerSum=%d, gridfreq=%d, historyRunTime=%d \n", totalActpowerSum, gridfreq, historyRunTime));


    // check if the current plug mode has been changed & update accordingly
    if (currentPlugMode != previousPlugMode) {
      previousPlugMode = currentPlugMode;

      command_send(CMD_SET, cls_shiftor_opc(SMTPLG_CMD_CLS_APP, SMTPLG_CMD_OP_SET_PWR_ONOFF), &currentPlugMode, 1);
      setPlugModeFieldUpdate();
    }

    if (currentPlugModeStatus != previousPlugModeStatus) {
      previousPlugModeStatus = currentPlugModeStatus;
      setPlugModeStatusFieldUpdate();

      if(currentPlugModeStatus == 0){             //Turn off plug
        //Plug Turn on
        plugstatusText[snprintf(plugstatusString, sizeof(plugstatusText), "Plug Power OFF")] = '\0';
        notificationText[snprintf(notificationString, sizeof(notificationText), "Plug Power OFF")] = '\0';
        sendANotification = 1;
      }
      else if(currentPlugModeStatus == 1){       //Turn on plug
        //Plug Turn on
        plugstatusText[snprintf(plugstatusString, sizeof(plugstatusText), "Plug Power ON ")] = '\0';
        notificationText[snprintf(notificationString, sizeof(notificationText), "Plug Power ON")] = '\0';
        sendANotification = 1;
      }
    }

	
    // check if the current plug power save mode has been changed & update accordingly
    if (currentPowerSaveMode != previousPowerSaveMode) {
      previousPowerSaveMode = currentPowerSaveMode;

      command_send(CMD_SET, cls_shiftor_opc(SMTPLG_CMD_CLS_APP, SMTPLG_CMD_OP_SET_PWRSAVE_MODE), &currentPowerSaveMode, 1);
      setPowerSaveFieldUpdate();
      setPowerSaveStatusFieldUpdate();
      if(currentPowerSaveMode == 0){             //Turn off power save mode
       //Power save  Turn off
       set_wifi_sleep_mode(0);
        powersavestatusText[snprintf(powersavestatusString, sizeof(powersavestatusText), "PowerSave OFF ")] = '\0';
        notificationText[snprintf(notificationString, sizeof(notificationText), "PowerSave OFF ")] = '\0';
        sendANotification = 1;
      }
      else if(currentPowerSaveMode == 1){       //Turn on power save mode
        //Power save  Turn on
        set_wifi_sleep_mode(1);
        powersavestatusText[snprintf(powersavestatusString, sizeof(powersavestatusText), "PowerSave AUTO")] = '\0';
        notificationText[snprintf(notificationString, sizeof(notificationText), "PowerSave AUTO")] = '\0';
        sendANotification = 1;
      }
    }
    /* Update current Active Power display */
    if ((currentActPowerDec != previousActPowerDec)|| (currentActPowerInt != previousActPowerInt)) {
      previousActPowerDec = currentActPowerDec;
      previousActPowerInt = currentActPowerInt;
      setCurrentActivePowerFieldUpdate();
    }
    
    /* Update current Reactive Power display */
    if ((currentReactPowerDec != previousReactPowerDec)||(currentReactPowerInt != previousReactPowerInt)) {
      previousReactPowerDec = currentReactPowerDec;
      previousReactPowerInt = currentReactPowerInt;
      setCurrentReactivePowerFieldUpdate();
    }

    /* Update current Total Power display */
    if ((currentTotalPowerDec != previousTotalPowerDec)||(currentTotalPowerInt != previousTotalPowerInt)) {
      previousTotalPowerDec = currentTotalPowerDec;
      previousTotalPowerInt = currentTotalPowerInt;
      setCurrentTotalPowerFieldUpdate();
    }

    /* Update Total Active Power Sum display */
    if ((totalActpowerSumDec != previousActpowerSumDec)||(totalActpowerSumInt != previousActpowerSumInt)){
      previousActpowerSumDec = totalActpowerSumDec;
      previousActpowerSumInt = totalActpowerSumInt;
      setTotalActivePowerSumFieldUpdate();
    }

    /* Grid Frequence */
    if ((gridfreq != previousGridFreq)){
      previousGridFreq = gridfreq;
      setCurrentGridPreqFieldUpdate();
    }
    
     /* Work history run time*/
     if ((historyRunTime != prevHistoryRunTime)){
       prevHistoryRunTime = historyRunTime;
       setHistoryRunTimeFieldUpdate();
    }
    
    /* Set work timer ON/OFF */
    if(workTimerONOFF != previous_workTimerONOFF){	
      previous_workTimerONOFF = workTimerONOFF;

      command_send(CMD_SET, cls_shiftor_opc(SMTPLG_CMD_CLS_APP, SMTPLG_CMD_OP_SET_TIMER_ONOFF), &workTimerONOFF, 1);
      setWorkTimerONOFFFieldUpdate();
      if(workTimerONOFF == 0){             //Turn off plug
        //Turn off Work timer 
      }
      else if(workTimerONOFF == 1){
        //Turn on Work timer  
      }
    }

    /* Set work start time */
    if(isTimeDiffernt(&fliptime, &previousFliptime)){	
      timeCopy(&previousFliptime, &fliptime);

      start_time_temp.hour = fliptime.hour;
      start_time_temp.min = fliptime.minute;
      start_time_temp.sec = fliptime.second;
      command_send(CMD_SET, cls_shiftor_opc(SMTPLG_CMD_CLS_APP, SMTPLG_CMD_OP_SET_TIMER_BEGIN), &start_time_temp, sizeof(SMTPLUG_TM));
      setStartTimingFieldUpdate();
    }
    /* Set work end time */
    if(isTimeDiffernt(&end_fliptime, &previous_end_fliptime)){	
      timeCopy(&previous_end_fliptime, &end_fliptime);

      end_time_temp.hour = end_fliptime.hour;
      end_time_temp.min = end_fliptime.minute;
      end_time_temp.sec = end_fliptime.second;  
      command_send(CMD_SET, cls_shiftor_opc(SMTPLG_CMD_CLS_APP, SMTPLG_CMD_OP_SET_TIMER_END), &end_time_temp, sizeof(SMTPLUG_TM));
      setEndTimingFieldUpdate();
    }
    /* Set RTC adjuste date */
    if(isDateDiffernt(&rtc_date, &previous_rtc_date)){	
      dateCopy(&previous_rtc_date, &rtc_date);
      setRTCDateFieldUpdate();
    }
    /* Set RTC adjuste time */
    if(isTimeDiffernt(&rtc_time, &previous_rtc_time)){	
      timeCopy(&previous_rtc_time, &rtc_time);
      rtc_timer_temp.year = rtc_date.fullYear;
      rtc_timer_temp.mon = rtc_date.month;
      rtc_timer_temp.mday = rtc_date.mDay;
      rtc_timer_temp.hour = rtc_time.hour;
      rtc_timer_temp.min = rtc_time.minute;
      rtc_timer_temp.sec = rtc_time.second;

      command_send(CMD_SET, cls_shiftor_opc(SMTPLG_CMD_CLS_APP, SMTPLG_CMD_OP_SET_CUR_TIME), &rtc_timer_temp, sizeof(SMTPLUG_Date_Time));
      setRTCTimeFieldUpdate();
    }
	
    return signalsToSend;
}


