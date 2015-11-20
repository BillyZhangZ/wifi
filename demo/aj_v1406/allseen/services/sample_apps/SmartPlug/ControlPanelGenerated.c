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

#include <alljoyn.h>
#include <alljoyn/controlpanel/Common/ConstraintList.h>
#include <alljoyn/controlpanel/ControlPanelService.h>
#include "ControlPanelProvided.h"

#ifndef NDEBUG
extern AJ_EXPORT uint8_t dbgAJSVCAPP;
#endif

const char MyDeviceRootContainerObjectPath[] = "/ControlPanel/MyDevice/rootContainer";
const char enMyDeviceRootContainerObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en";
const char enMyDeviceCurrentActivePowerPropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/CurrentActivePowerProperty";
const char enMyDeviceCurrentReactivePowerPropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/CurrentReactivePowerProperty";
const char enMyDeviceCurrentTotalPowerPropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/CurrentTotalPowerProperty";
const char enMyDeviceTotalActivePowerSumPropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/TotalActivePowerSumProperty";
const char enMyDeviceCurrentGridFreqPropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/CurrentGridFreqProperty";
const char enMyDeviceHistoryRunTimePropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/HistoryRunTimeProperty";
const char enMyDeviceControlsPlugContainerObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/controlsPlugContainer";
const char enMyDevicePlugModeObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/controlsPlugContainer/plugMode";
const char enMyDevicePlugstatusStringPropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/controlsPlugContainer/plugstatusStringProperty";
const char enMyDeviceControlsContainerObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/controlsContainer";
const char enMyDevicePlugSavePowerModeObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/controlsContainer/plugSavePowerMode";
const char enMyDevicePowerSaveStatusStringPropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/controlsContainer/powerSaveStatusStringProperty";
const char enMyDeviceWorkTimerContainerObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/WorkTimerContainer";
const char enMyDeviceWorkTimerObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/WorkTimerContainer/workTimer";
const char enMyDeviceStartFlipTimePropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/WorkTimerContainer/StartFlipTimeProperty";
const char enMyDeviceEndFlipTimePropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/WorkTimerContainer/EndFlipTimeProperty";
const char enMyDeviceRTCSetContainerObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/RTCSetContainer";
const char enMyDeviceRTCDatePropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/RTCSetContainer/RTCDateProperty";
const char enMyDeviceRTCTimePropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/RTCSetContainer/RTCTimeProperty";

ContainerWidget MyDeviceRootContainer;
PropertyWidget MyDeviceCurrentActivePowerProperty;
PropertyWidget MyDeviceCurrentReactivePowerProperty;
PropertyWidget MyDeviceCurrentTotalPowerProperty;
PropertyWidget MyDeviceTotalActivePowerSumProperty;
PropertyWidget MyDeviceCurrentGridFreqProperty;
PropertyWidget MyDeviceHistoryRunTimeProperty;
ContainerWidget MyDeviceControlsPlugContainer;
PropertyWidget MyDevicePlugMode;
PropertyWidget MyDevicePlugstatusStringProperty;
ContainerWidget MyDeviceControlsContainer;
PropertyWidget MyDevicePlugSavePowerMode;
PropertyWidget MyDevicePowerSaveStatusStringProperty;
ContainerWidget MyDeviceWorkTimerContainer;
PropertyWidget MyDeviceWorkTimer;
PropertyWidget MyDeviceStartFlipTimeProperty;
PropertyWidget MyDeviceEndFlipTimeProperty;
ContainerWidget MyDeviceRTCSetContainer;
PropertyWidget MyDeviceRTCDateProperty;
PropertyWidget MyDeviceRTCTimeProperty;

/*
 * Static variables used to fill widgets
 */
static const uint16_t MyDeviceRootContainerHints[] = { LAYOUT_HINT_VERTICAL_LINEAR, LAYOUT_HINT_HORIZONTAL_LINEAR };
static const char* const MyDeviceCurrentActivePowerPropertySignature = "s";
static const char* const MyDeviceCurrentActivePowerPropertyLabel[] = { "1: Current Active Power:" };
static const uint16_t MyDeviceCurrentActivePowerPropertyHints[] = { PROPERTY_WIDGET_HINT_TEXTVIEW };
static const char* const MyDeviceCurrentReactivePowerPropertySignature = "s";
static const char* const MyDeviceCurrentReactivePowerPropertyLabel[] = { "2: Current Reactive Power:" };
static const uint16_t MyDeviceCurrentReactivePowerPropertyHints[] = { PROPERTY_WIDGET_HINT_TEXTVIEW };
static const char* const MyDeviceCurrentTotalPowerPropertySignature = "s";
static const char* const MyDeviceCurrentTotalPowerPropertyLabel[] = { "3: Current Total Power:" };
static const uint16_t MyDeviceCurrentTotalPowerPropertyHints[] = { PROPERTY_WIDGET_HINT_TEXTVIEW };
static const char* const MyDeviceTotalActivePowerSumPropertySignature = "s";
static const char* const MyDeviceTotalActivePowerSumPropertyLabel[] = { "4: Active Power Sum:" };
static const uint16_t MyDeviceTotalActivePowerSumPropertyHints[] = { PROPERTY_WIDGET_HINT_TEXTVIEW };
static const char* const MyDeviceCurrentGridFreqPropertySignature = "s";
static const char* const MyDeviceCurrentGridFreqPropertyLabel[] = { "5: Grid frequency:" };
static const uint16_t MyDeviceCurrentGridFreqPropertyHints[] = { PROPERTY_WIDGET_HINT_TEXTVIEW };
static const char* const MyDeviceHistoryRunTimePropertySignature = "s";
static const char* const MyDeviceHistoryRunTimePropertyLabel[] = { "6: History Run Time:" };
static const uint16_t MyDeviceHistoryRunTimePropertyHints[] = { PROPERTY_WIDGET_HINT_TEXTVIEW };
static const uint16_t MyDeviceControlsPlugContainerHints[] = { LAYOUT_HINT_HORIZONTAL_LINEAR };
static const char* const MyDevicePlugModeSignature = "q";
static const char* const MyDevicePlugModeLabel[] = { "Plug Mode" };
static const uint16_t MyDevicePlugModeHints[] = { PROPERTY_WIDGET_HINT_SPINNER };
static ConstraintList MyDevicePlugModeConstraintList[2];
static const uint16_t MyDevicePlugModeConstraintValue0 = 1;
static const char* const MyDevicePlugModeDisplay0[] = { "ON" };
static const uint16_t MyDevicePlugModeConstraintValue1 = 0;
static const char* const MyDevicePlugModeDisplay1[] = { "OFF" };
static const char* const MyDevicePlugstatusStringPropertySignature = "s";
static const char* const MyDevicePlugstatusStringPropertyLabel[] = { "Status:" };
static const uint16_t MyDevicePlugstatusStringPropertyHints[] = { PROPERTY_WIDGET_HINT_TEXTVIEW };
static const uint16_t MyDeviceControlsContainerHints[] = { LAYOUT_HINT_HORIZONTAL_LINEAR };
static const char* const MyDevicePlugSavePowerModeSignature = "q";
static const char* const MyDevicePlugSavePowerModeLabel[] = { "Save Mode" };
static const uint16_t MyDevicePlugSavePowerModeHints[] = { PROPERTY_WIDGET_HINT_SPINNER };
static ConstraintList MyDevicePlugSavePowerModeConstraintList[2];
static const uint16_t MyDevicePlugSavePowerModeConstraintValue0 = 0;
static const char* const MyDevicePlugSavePowerModeDisplay0[] = { "OFF" };
static const uint16_t MyDevicePlugSavePowerModeConstraintValue1 = 1;
static const char* const MyDevicePlugSavePowerModeDisplay1[] = { "Auto" };
static const char* const MyDevicePowerSaveStatusStringPropertySignature = "s";
static const char* const MyDevicePowerSaveStatusStringPropertyLabel[] = { "Status:" };
static const uint16_t MyDevicePowerSaveStatusStringPropertyHints[] = { PROPERTY_WIDGET_HINT_TEXTVIEW };
static const uint16_t MyDeviceWorkTimerContainerHints[] = { LAYOUT_HINT_HORIZONTAL_LINEAR };
static const char* const MyDeviceWorkTimerSignature = "q";
static const char* const MyDeviceWorkTimerLabel[] = { "Work Time:" };
static const uint16_t MyDeviceWorkTimerHints[] = { PROPERTY_WIDGET_HINT_SPINNER };
static ConstraintList MyDeviceWorkTimerConstraintList[2];
static const uint16_t MyDeviceWorkTimerConstraintValue0 = 1;
static const char* const MyDeviceWorkTimerDisplay0[] = { "ON" };
static const uint16_t MyDeviceWorkTimerConstraintValue1 = 0;
static const char* const MyDeviceWorkTimerDisplay1[] = { "OFF" };
static const uint16_t MyDeviceStartFlipTimePropertyHints[] = { PROPERTY_WIDGET_HINT_TIMEPICKER };
static const uint16_t MyDeviceEndFlipTimePropertyHints[] = { PROPERTY_WIDGET_HINT_TIMEPICKER };
static const uint16_t MyDeviceRTCSetContainerHints[] = { LAYOUT_HINT_HORIZONTAL_LINEAR };
static const char* const MyDeviceRTCDatePropertyLabel[] = { "RTC Time:" };
static const uint16_t MyDeviceRTCDatePropertyHints[] = { PROPERTY_WIDGET_HINT_DATEPICKER };
static const uint16_t MyDeviceRTCTimePropertyHints[] = { PROPERTY_WIDGET_HINT_TIMEPICKER };


void WidgetsInit()
{
    initializeContainerWidget(&MyDeviceRootContainer);
    MyDeviceRootContainer.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceRootContainer.base, TRUE);

    MyDeviceRootContainer.base.optParams.bgColor = 0x1e90ff;
    MyDeviceRootContainer.base.optParams.hints = MyDeviceRootContainerHints;
    MyDeviceRootContainer.base.optParams.numHints = 2;

    initializePropertyWidget(&MyDeviceCurrentActivePowerProperty);
    MyDeviceCurrentActivePowerProperty.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceCurrentActivePowerProperty.base, TRUE);
    setBaseWritable(&MyDeviceCurrentActivePowerProperty.base, FALSE);

    MyDeviceCurrentActivePowerProperty.signature = MyDeviceCurrentActivePowerPropertySignature;
    MyDeviceCurrentActivePowerProperty.propertyType = SINGLE_VALUE_PROPERTY;
    MyDeviceCurrentActivePowerProperty.getValue = &getCurrentActivePowerString;

    MyDeviceCurrentActivePowerProperty.base.optParams.bgColor = 0x500;
    MyDeviceCurrentActivePowerProperty.base.optParams.label = MyDeviceCurrentActivePowerPropertyLabel;
    MyDeviceCurrentActivePowerProperty.base.optParams.hints = MyDeviceCurrentActivePowerPropertyHints;
    MyDeviceCurrentActivePowerProperty.base.optParams.numHints = 1;

    initializePropertyWidget(&MyDeviceCurrentReactivePowerProperty);
    MyDeviceCurrentReactivePowerProperty.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceCurrentReactivePowerProperty.base, TRUE);
    setBaseWritable(&MyDeviceCurrentReactivePowerProperty.base, FALSE);

    MyDeviceCurrentReactivePowerProperty.signature = MyDeviceCurrentReactivePowerPropertySignature;
    MyDeviceCurrentReactivePowerProperty.propertyType = SINGLE_VALUE_PROPERTY;
    MyDeviceCurrentReactivePowerProperty.getValue = &getCurrentReactivePowerString;

    MyDeviceCurrentReactivePowerProperty.base.optParams.bgColor = 0x500;
    MyDeviceCurrentReactivePowerProperty.base.optParams.label = MyDeviceCurrentReactivePowerPropertyLabel;
    MyDeviceCurrentReactivePowerProperty.base.optParams.hints = MyDeviceCurrentReactivePowerPropertyHints;
    MyDeviceCurrentReactivePowerProperty.base.optParams.numHints = 1;

    initializePropertyWidget(&MyDeviceCurrentTotalPowerProperty);
    MyDeviceCurrentTotalPowerProperty.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceCurrentTotalPowerProperty.base, TRUE);
    setBaseWritable(&MyDeviceCurrentTotalPowerProperty.base, FALSE);

    MyDeviceCurrentTotalPowerProperty.signature = MyDeviceCurrentTotalPowerPropertySignature;
    MyDeviceCurrentTotalPowerProperty.propertyType = SINGLE_VALUE_PROPERTY;
    MyDeviceCurrentTotalPowerProperty.getValue = &getCurrentTotalPowerString;

    MyDeviceCurrentTotalPowerProperty.base.optParams.bgColor = 0x500;
    MyDeviceCurrentTotalPowerProperty.base.optParams.label = MyDeviceCurrentTotalPowerPropertyLabel;
    MyDeviceCurrentTotalPowerProperty.base.optParams.hints = MyDeviceCurrentTotalPowerPropertyHints;
    MyDeviceCurrentTotalPowerProperty.base.optParams.numHints = 1;

    initializePropertyWidget(&MyDeviceTotalActivePowerSumProperty);
    MyDeviceTotalActivePowerSumProperty.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceTotalActivePowerSumProperty.base, TRUE);
    setBaseWritable(&MyDeviceTotalActivePowerSumProperty.base, FALSE);

    MyDeviceTotalActivePowerSumProperty.signature = MyDeviceTotalActivePowerSumPropertySignature;
    MyDeviceTotalActivePowerSumProperty.propertyType = SINGLE_VALUE_PROPERTY;
    MyDeviceTotalActivePowerSumProperty.getValue = &getTotalActivePowerSumString;

    MyDeviceTotalActivePowerSumProperty.base.optParams.bgColor = 0x500;
    MyDeviceTotalActivePowerSumProperty.base.optParams.label = MyDeviceTotalActivePowerSumPropertyLabel;
    MyDeviceTotalActivePowerSumProperty.base.optParams.hints = MyDeviceTotalActivePowerSumPropertyHints;
    MyDeviceTotalActivePowerSumProperty.base.optParams.numHints = 1;

    initializePropertyWidget(&MyDeviceCurrentGridFreqProperty);
    MyDeviceCurrentGridFreqProperty.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceCurrentGridFreqProperty.base, TRUE);
    setBaseWritable(&MyDeviceCurrentGridFreqProperty.base, FALSE);

    MyDeviceCurrentGridFreqProperty.signature = MyDeviceCurrentGridFreqPropertySignature;
    MyDeviceCurrentGridFreqProperty.propertyType = SINGLE_VALUE_PROPERTY;
    MyDeviceCurrentGridFreqProperty.getValue = &getCurrentGridFreqString;

    MyDeviceCurrentGridFreqProperty.base.optParams.bgColor = 0x500;
    MyDeviceCurrentGridFreqProperty.base.optParams.label = MyDeviceCurrentGridFreqPropertyLabel;
    MyDeviceCurrentGridFreqProperty.base.optParams.hints = MyDeviceCurrentGridFreqPropertyHints;
    MyDeviceCurrentGridFreqProperty.base.optParams.numHints = 1;

    initializePropertyWidget(&MyDeviceHistoryRunTimeProperty);
    MyDeviceHistoryRunTimeProperty.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceHistoryRunTimeProperty.base, TRUE);
    setBaseWritable(&MyDeviceHistoryRunTimeProperty.base, FALSE);

    MyDeviceHistoryRunTimeProperty.signature = MyDeviceHistoryRunTimePropertySignature;
    MyDeviceHistoryRunTimeProperty.propertyType = SINGLE_VALUE_PROPERTY;
    MyDeviceHistoryRunTimeProperty.getValue = &getHistoryRunTimeString;

    MyDeviceHistoryRunTimeProperty.base.optParams.bgColor = 0x500;
    MyDeviceHistoryRunTimeProperty.base.optParams.label = MyDeviceHistoryRunTimePropertyLabel;
    MyDeviceHistoryRunTimeProperty.base.optParams.hints = MyDeviceHistoryRunTimePropertyHints;
    MyDeviceHistoryRunTimeProperty.base.optParams.numHints = 1;

    initializeContainerWidget(&MyDeviceControlsPlugContainer);
    MyDeviceControlsPlugContainer.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceControlsPlugContainer.base, TRUE);

    MyDeviceControlsPlugContainer.base.optParams.bgColor = 0x200;
    MyDeviceControlsPlugContainer.base.optParams.hints = MyDeviceControlsPlugContainerHints;
    MyDeviceControlsPlugContainer.base.optParams.numHints = 1;

    initializePropertyWidget(&MyDevicePlugMode);
    MyDevicePlugMode.base.numLanguages = 1;
    setBaseEnabled(&MyDevicePlugMode.base, TRUE);
    setBaseWritable(&MyDevicePlugMode.base, TRUE);

    MyDevicePlugMode.signature = MyDevicePlugModeSignature;
    MyDevicePlugMode.propertyType = SINGLE_VALUE_PROPERTY;
    MyDevicePlugMode.getValue = &getCurrentPlugMode;

    MyDevicePlugMode.base.optParams.bgColor = 0xffd700;
    MyDevicePlugMode.base.optParams.label = MyDevicePlugModeLabel;
    MyDevicePlugMode.base.optParams.hints = MyDevicePlugModeHints;
    MyDevicePlugMode.base.optParams.numHints = 1;

    MyDevicePlugMode.optParams.numConstraints = 2;
    MyDevicePlugMode.optParams.constraintList = MyDevicePlugModeConstraintList;
    MyDevicePlugMode.optParams.constraintList[0].value = &MyDevicePlugModeConstraintValue0;
    MyDevicePlugMode.optParams.constraintList[0].display = MyDevicePlugModeDisplay0;
    MyDevicePlugMode.optParams.constraintList[1].value = &MyDevicePlugModeConstraintValue1;
    MyDevicePlugMode.optParams.constraintList[1].display = MyDevicePlugModeDisplay1;

    initializePropertyWidget(&MyDevicePlugstatusStringProperty);
    MyDevicePlugstatusStringProperty.base.numLanguages = 1;
    setBaseEnabled(&MyDevicePlugstatusStringProperty.base, TRUE);
    setBaseWritable(&MyDevicePlugstatusStringProperty.base, FALSE);

    MyDevicePlugstatusStringProperty.signature = MyDevicePlugstatusStringPropertySignature;
    MyDevicePlugstatusStringProperty.propertyType = SINGLE_VALUE_PROPERTY;
    MyDevicePlugstatusStringProperty.getValue = &getPlugStatusString;

    MyDevicePlugstatusStringProperty.base.optParams.bgColor = 0x500;
    MyDevicePlugstatusStringProperty.base.optParams.label = MyDevicePlugstatusStringPropertyLabel;
    MyDevicePlugstatusStringProperty.base.optParams.hints = MyDevicePlugstatusStringPropertyHints;
    MyDevicePlugstatusStringProperty.base.optParams.numHints = 1;

    initializeContainerWidget(&MyDeviceControlsContainer);
    MyDeviceControlsContainer.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceControlsContainer.base, TRUE);

    MyDeviceControlsContainer.base.optParams.bgColor = 0x200;
    MyDeviceControlsContainer.base.optParams.hints = MyDeviceControlsContainerHints;
    MyDeviceControlsContainer.base.optParams.numHints = 1;

    initializePropertyWidget(&MyDevicePlugSavePowerMode);
    MyDevicePlugSavePowerMode.base.numLanguages = 1;
    setBaseEnabled(&MyDevicePlugSavePowerMode.base, TRUE);
    setBaseWritable(&MyDevicePlugSavePowerMode.base, TRUE);

    MyDevicePlugSavePowerMode.signature = MyDevicePlugSavePowerModeSignature;
    MyDevicePlugSavePowerMode.propertyType = SINGLE_VALUE_PROPERTY;
    MyDevicePlugSavePowerMode.getValue = &getCurrentPowerSaveMode;

    MyDevicePlugSavePowerMode.base.optParams.bgColor = 0xffd700;
    MyDevicePlugSavePowerMode.base.optParams.label = MyDevicePlugSavePowerModeLabel;
    MyDevicePlugSavePowerMode.base.optParams.hints = MyDevicePlugSavePowerModeHints;
    MyDevicePlugSavePowerMode.base.optParams.numHints = 1;

    MyDevicePlugSavePowerMode.optParams.numConstraints = 2;
    MyDevicePlugSavePowerMode.optParams.constraintList = MyDevicePlugSavePowerModeConstraintList;
    MyDevicePlugSavePowerMode.optParams.constraintList[0].value = &MyDevicePlugSavePowerModeConstraintValue0;
    MyDevicePlugSavePowerMode.optParams.constraintList[0].display = MyDevicePlugSavePowerModeDisplay0;
    MyDevicePlugSavePowerMode.optParams.constraintList[1].value = &MyDevicePlugSavePowerModeConstraintValue1;
    MyDevicePlugSavePowerMode.optParams.constraintList[1].display = MyDevicePlugSavePowerModeDisplay1;

    initializePropertyWidget(&MyDevicePowerSaveStatusStringProperty);
    MyDevicePowerSaveStatusStringProperty.base.numLanguages = 1;
    setBaseEnabled(&MyDevicePowerSaveStatusStringProperty.base, TRUE);
    setBaseWritable(&MyDevicePowerSaveStatusStringProperty.base, FALSE);

    MyDevicePowerSaveStatusStringProperty.signature = MyDevicePowerSaveStatusStringPropertySignature;
    MyDevicePowerSaveStatusStringProperty.propertyType = SINGLE_VALUE_PROPERTY;
    MyDevicePowerSaveStatusStringProperty.getValue = &getPowerSaveStatusString;

    MyDevicePowerSaveStatusStringProperty.base.optParams.bgColor = 0x500;
    MyDevicePowerSaveStatusStringProperty.base.optParams.label = MyDevicePowerSaveStatusStringPropertyLabel;
    MyDevicePowerSaveStatusStringProperty.base.optParams.hints = MyDevicePowerSaveStatusStringPropertyHints;
    MyDevicePowerSaveStatusStringProperty.base.optParams.numHints = 1;

    initializeContainerWidget(&MyDeviceWorkTimerContainer);
    MyDeviceWorkTimerContainer.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceWorkTimerContainer.base, TRUE);

    MyDeviceWorkTimerContainer.base.optParams.bgColor = 0x200;
    MyDeviceWorkTimerContainer.base.optParams.hints = MyDeviceWorkTimerContainerHints;
    MyDeviceWorkTimerContainer.base.optParams.numHints = 1;

    initializePropertyWidget(&MyDeviceWorkTimer);
    MyDeviceWorkTimer.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceWorkTimer.base, TRUE);
    setBaseWritable(&MyDeviceWorkTimer.base, TRUE);

    MyDeviceWorkTimer.signature = MyDeviceWorkTimerSignature;
    MyDeviceWorkTimer.propertyType = SINGLE_VALUE_PROPERTY;
    MyDeviceWorkTimer.getValue = &getWorkTimerONOFF;

    MyDeviceWorkTimer.base.optParams.bgColor = 0xffd700;
    MyDeviceWorkTimer.base.optParams.label = MyDeviceWorkTimerLabel;
    MyDeviceWorkTimer.base.optParams.hints = MyDeviceWorkTimerHints;
    MyDeviceWorkTimer.base.optParams.numHints = 1;

    MyDeviceWorkTimer.optParams.numConstraints = 2;
    MyDeviceWorkTimer.optParams.constraintList = MyDeviceWorkTimerConstraintList;
    MyDeviceWorkTimer.optParams.constraintList[0].value = &MyDeviceWorkTimerConstraintValue0;
    MyDeviceWorkTimer.optParams.constraintList[0].display = MyDeviceWorkTimerDisplay0;
    MyDeviceWorkTimer.optParams.constraintList[1].value = &MyDeviceWorkTimerConstraintValue1;
    MyDeviceWorkTimer.optParams.constraintList[1].display = MyDeviceWorkTimerDisplay1;

    initializePropertyWidget(&MyDeviceStartFlipTimeProperty);
    MyDeviceStartFlipTimeProperty.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceStartFlipTimeProperty.base, TRUE);
    setBaseWritable(&MyDeviceStartFlipTimeProperty.base, TRUE);
    MyDeviceStartFlipTimeProperty.propertyType = TIME_VALUE_PROPERTY;
    MyDeviceStartFlipTimeProperty.getValue = &getStartFlipTimevar;

    MyDeviceStartFlipTimeProperty.base.optParams.bgColor = 0x500;
    MyDeviceStartFlipTimeProperty.base.optParams.hints = MyDeviceStartFlipTimePropertyHints;
    MyDeviceStartFlipTimeProperty.base.optParams.numHints = 1;

    initializePropertyWidget(&MyDeviceEndFlipTimeProperty);
    MyDeviceEndFlipTimeProperty.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceEndFlipTimeProperty.base, TRUE);
    setBaseWritable(&MyDeviceEndFlipTimeProperty.base, TRUE);
    MyDeviceEndFlipTimeProperty.propertyType = TIME_VALUE_PROPERTY;
    MyDeviceEndFlipTimeProperty.getValue = &getEndFlipTimevar;

    MyDeviceEndFlipTimeProperty.base.optParams.bgColor = 0x500;
    MyDeviceEndFlipTimeProperty.base.optParams.hints = MyDeviceEndFlipTimePropertyHints;
    MyDeviceEndFlipTimeProperty.base.optParams.numHints = 1;

    initializeContainerWidget(&MyDeviceRTCSetContainer);
    MyDeviceRTCSetContainer.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceRTCSetContainer.base, TRUE);

    MyDeviceRTCSetContainer.base.optParams.bgColor = 0x200;
    MyDeviceRTCSetContainer.base.optParams.hints = MyDeviceRTCSetContainerHints;
    MyDeviceRTCSetContainer.base.optParams.numHints = 1;

    initializePropertyWidget(&MyDeviceRTCDateProperty);
    MyDeviceRTCDateProperty.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceRTCDateProperty.base, TRUE);
    setBaseWritable(&MyDeviceRTCDateProperty.base, TRUE);
    MyDeviceRTCDateProperty.propertyType = DATE_VALUE_PROPERTY;
    MyDeviceRTCDateProperty.getValue = &getRTCDatevar;

    MyDeviceRTCDateProperty.base.optParams.bgColor = 0x500;
    MyDeviceRTCDateProperty.base.optParams.label = MyDeviceRTCDatePropertyLabel;
    MyDeviceRTCDateProperty.base.optParams.hints = MyDeviceRTCDatePropertyHints;
    MyDeviceRTCDateProperty.base.optParams.numHints = 1;

    initializePropertyWidget(&MyDeviceRTCTimeProperty);
    MyDeviceRTCTimeProperty.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceRTCTimeProperty.base, TRUE);
    setBaseWritable(&MyDeviceRTCTimeProperty.base, TRUE);
    MyDeviceRTCTimeProperty.propertyType = TIME_VALUE_PROPERTY;
    MyDeviceRTCTimeProperty.getValue = &getRTCTimevar;

    MyDeviceRTCTimeProperty.base.optParams.bgColor = 0x500;
    MyDeviceRTCTimeProperty.base.optParams.hints = MyDeviceRTCTimePropertyHints;
    MyDeviceRTCTimeProperty.base.optParams.numHints = 1;

    return;
}

void* IdentifyMsgOrPropId(uint32_t identifier, uint16_t* widgetType, uint16_t* propType, uint16_t* language)
{
    switch (identifier) {
    case EN_MYDEVICE_ROOTCONTAINER_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRootContainer;

    case EN_MYDEVICE_ROOTCONTAINER_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRootContainer;

    case EN_MYDEVICE_ROOTCONTAINER_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRootContainer;

    case EN_MYDEVICE_ROOTCONTAINER_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRootContainer;

    case EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentActivePowerProperty;

    case EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentActivePowerProperty;

    case EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentActivePowerProperty;

    case EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentActivePowerProperty;

    case EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentActivePowerProperty;

    case EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentReactivePowerProperty;

    case EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentReactivePowerProperty;

    case EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentReactivePowerProperty;

    case EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentReactivePowerProperty;

    case EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentReactivePowerProperty;

    case EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentTotalPowerProperty;

    case EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentTotalPowerProperty;

    case EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentTotalPowerProperty;

    case EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentTotalPowerProperty;

    case EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentTotalPowerProperty;

    case EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceTotalActivePowerSumProperty;

    case EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceTotalActivePowerSumProperty;

    case EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceTotalActivePowerSumProperty;

    case EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceTotalActivePowerSumProperty;

    case EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceTotalActivePowerSumProperty;

    case EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentGridFreqProperty;

    case EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentGridFreqProperty;

    case EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentGridFreqProperty;

    case EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentGridFreqProperty;

    case EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentGridFreqProperty;

    case EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceHistoryRunTimeProperty;

    case EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceHistoryRunTimeProperty;

    case EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceHistoryRunTimeProperty;

    case EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceHistoryRunTimeProperty;

    case EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceHistoryRunTimeProperty;

    case EN_MYDEVICE_CONTROLSPLUGCONTAINER_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceControlsPlugContainer;

    case EN_MYDEVICE_CONTROLSPLUGCONTAINER_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceControlsPlugContainer;

    case EN_MYDEVICE_CONTROLSPLUGCONTAINER_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceControlsPlugContainer;

    case EN_MYDEVICE_CONTROLSPLUGCONTAINER_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceControlsPlugContainer;

    case EN_MYDEVICE_PLUGMODE_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugMode;

    case EN_MYDEVICE_PLUGMODE_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugMode;

    case EN_MYDEVICE_PLUGMODE_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugMode;

    case EN_MYDEVICE_PLUGMODE_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugMode;

    case EN_MYDEVICE_PLUGMODE_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugMode;

    case EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugstatusStringProperty;

    case EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugstatusStringProperty;

    case EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugstatusStringProperty;

    case EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugstatusStringProperty;

    case EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugstatusStringProperty;

    case EN_MYDEVICE_CONTROLSCONTAINER_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceControlsContainer;

    case EN_MYDEVICE_CONTROLSCONTAINER_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceControlsContainer;

    case EN_MYDEVICE_CONTROLSCONTAINER_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceControlsContainer;

    case EN_MYDEVICE_CONTROLSCONTAINER_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceControlsContainer;

    case EN_MYDEVICE_PLUGSAVEPOWERMODE_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugSavePowerMode;

    case EN_MYDEVICE_PLUGSAVEPOWERMODE_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugSavePowerMode;

    case EN_MYDEVICE_PLUGSAVEPOWERMODE_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugSavePowerMode;

    case EN_MYDEVICE_PLUGSAVEPOWERMODE_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugSavePowerMode;

    case EN_MYDEVICE_PLUGSAVEPOWERMODE_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePlugSavePowerMode;

    case EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePowerSaveStatusStringProperty;

    case EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePowerSaveStatusStringProperty;

    case EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePowerSaveStatusStringProperty;

    case EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePowerSaveStatusStringProperty;

    case EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDevicePowerSaveStatusStringProperty;

    case EN_MYDEVICE_WORKTIMERCONTAINER_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceWorkTimerContainer;

    case EN_MYDEVICE_WORKTIMERCONTAINER_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceWorkTimerContainer;

    case EN_MYDEVICE_WORKTIMERCONTAINER_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceWorkTimerContainer;

    case EN_MYDEVICE_WORKTIMERCONTAINER_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceWorkTimerContainer;

    case EN_MYDEVICE_WORKTIMER_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceWorkTimer;

    case EN_MYDEVICE_WORKTIMER_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceWorkTimer;

    case EN_MYDEVICE_WORKTIMER_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceWorkTimer;

    case EN_MYDEVICE_WORKTIMER_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceWorkTimer;

    case EN_MYDEVICE_WORKTIMER_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceWorkTimer;

    case EN_MYDEVICE_STARTFLIPTIMEPROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceStartFlipTimeProperty;

    case EN_MYDEVICE_STARTFLIPTIMEPROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceStartFlipTimeProperty;

    case EN_MYDEVICE_STARTFLIPTIMEPROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceStartFlipTimeProperty;

    case EN_MYDEVICE_STARTFLIPTIMEPROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceStartFlipTimeProperty;

    case EN_MYDEVICE_STARTFLIPTIMEPROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceStartFlipTimeProperty;

    case EN_MYDEVICE_ENDFLIPTIMEPROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceEndFlipTimeProperty;

    case EN_MYDEVICE_ENDFLIPTIMEPROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceEndFlipTimeProperty;

    case EN_MYDEVICE_ENDFLIPTIMEPROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceEndFlipTimeProperty;

    case EN_MYDEVICE_ENDFLIPTIMEPROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceEndFlipTimeProperty;

    case EN_MYDEVICE_ENDFLIPTIMEPROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceEndFlipTimeProperty;

    case EN_MYDEVICE_RTCSETCONTAINER_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRTCSetContainer;

    case EN_MYDEVICE_RTCSETCONTAINER_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRTCSetContainer;

    case EN_MYDEVICE_RTCSETCONTAINER_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRTCSetContainer;

    case EN_MYDEVICE_RTCSETCONTAINER_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRTCSetContainer;

    case EN_MYDEVICE_RTCDATEPROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRTCDateProperty;

    case EN_MYDEVICE_RTCDATEPROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRTCDateProperty;

    case EN_MYDEVICE_RTCDATEPROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRTCDateProperty;

    case EN_MYDEVICE_RTCDATEPROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRTCDateProperty;

    case EN_MYDEVICE_RTCDATEPROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRTCDateProperty;

    case EN_MYDEVICE_RTCTIMEPROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRTCTimeProperty;

    case EN_MYDEVICE_RTCTIMEPROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRTCTimeProperty;

    case EN_MYDEVICE_RTCTIMEPROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRTCTimeProperty;

    case EN_MYDEVICE_RTCTIMEPROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRTCTimeProperty;

    case EN_MYDEVICE_RTCTIMEPROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceRTCTimeProperty;

    default:
        return FALSE;
    }
}

void* IdentifyMsgOrPropIdForSignal(uint32_t identifier, uint8_t* isProperty)
{
    switch (identifier) {
    case EN_MYDEVICE_ROOTCONTAINER_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceRootContainer;

    case EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceCurrentActivePowerProperty;

    case EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDeviceCurrentActivePowerProperty;

    case EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceCurrentReactivePowerProperty;

    case EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDeviceCurrentReactivePowerProperty;

    case EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceCurrentTotalPowerProperty;

    case EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDeviceCurrentTotalPowerProperty;

    case EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceTotalActivePowerSumProperty;

    case EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDeviceTotalActivePowerSumProperty;

    case EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceCurrentGridFreqProperty;

    case EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDeviceCurrentGridFreqProperty;

    case EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceHistoryRunTimeProperty;

    case EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDeviceHistoryRunTimeProperty;

    case EN_MYDEVICE_CONTROLSPLUGCONTAINER_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceControlsPlugContainer;

    case EN_MYDEVICE_PLUGMODE_SIGNAL_PROPERTIES_CHANGED:
        return &MyDevicePlugMode;

    case EN_MYDEVICE_PLUGMODE_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDevicePlugMode;

    case EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDevicePlugstatusStringProperty;

    case EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDevicePlugstatusStringProperty;

    case EN_MYDEVICE_CONTROLSCONTAINER_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceControlsContainer;

    case EN_MYDEVICE_PLUGSAVEPOWERMODE_SIGNAL_PROPERTIES_CHANGED:
        return &MyDevicePlugSavePowerMode;

    case EN_MYDEVICE_PLUGSAVEPOWERMODE_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDevicePlugSavePowerMode;

    case EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDevicePowerSaveStatusStringProperty;

    case EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDevicePowerSaveStatusStringProperty;

    case EN_MYDEVICE_WORKTIMERCONTAINER_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceWorkTimerContainer;

    case EN_MYDEVICE_WORKTIMER_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceWorkTimer;

    case EN_MYDEVICE_WORKTIMER_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDeviceWorkTimer;

    case EN_MYDEVICE_STARTFLIPTIMEPROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceStartFlipTimeProperty;

    case EN_MYDEVICE_STARTFLIPTIMEPROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDeviceStartFlipTimeProperty;

    case EN_MYDEVICE_ENDFLIPTIMEPROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceEndFlipTimeProperty;

    case EN_MYDEVICE_ENDFLIPTIMEPROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDeviceEndFlipTimeProperty;

    case EN_MYDEVICE_RTCSETCONTAINER_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceRTCSetContainer;

    case EN_MYDEVICE_RTCDATEPROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceRTCDateProperty;

    case EN_MYDEVICE_RTCDATEPROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDeviceRTCDateProperty;

    case EN_MYDEVICE_RTCTIMEPROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceRTCTimeProperty;

    case EN_MYDEVICE_RTCTIMEPROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDeviceRTCTimeProperty;

    default:
        return FALSE;
    }
}

uint8_t IdentifyRootMsgOrPropId(uint32_t identifier)
{
    switch (identifier) {
    case MYDEVICE_ROOT_CONTROLPANEL_ROOTCONTAINER_VERSION_PROPERTY:
    case MYDEVICE_ROOT_CONTROLPANEL_ROOTCONTAINER_GET_ALL_VALUES:
        return TRUE;

    default:
        return FALSE;
    }
}

AJ_Status SetValueProperty(AJ_Message* replyMsg, uint32_t propId, void* context)
{
    AJ_Status status = AJ_ERR_UNEXPECTED;

    const char* variantSig;
    AJ_UnmarshalVariant(replyMsg, &variantSig);

    switch (propId) {
    case EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_VALUE_PROPERTY:
        {
            const char* newValue;
            status = unmarshalPropertyValue(&MyDeviceCurrentActivePowerProperty, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setCurrentActivePowerString(newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_VALUE_PROPERTY:
        {
            const char* newValue;
            status = unmarshalPropertyValue(&MyDeviceCurrentReactivePowerProperty, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setCurrentReactivePowerString(newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_VALUE_PROPERTY:
        {
            const char* newValue;
            status = unmarshalPropertyValue(&MyDeviceCurrentTotalPowerProperty, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setCurrentTotalPowerString(newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_VALUE_PROPERTY:
        {
            const char* newValue;
            status = unmarshalPropertyValue(&MyDeviceTotalActivePowerSumProperty, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setTotalActivePowerSumString(newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_VALUE_PROPERTY:
        {
            const char* newValue;
            status = unmarshalPropertyValue(&MyDeviceCurrentGridFreqProperty, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setCurrentGridFreqString(newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_VALUE_PROPERTY:
        {
            const char* newValue;
            status = unmarshalPropertyValue(&MyDeviceHistoryRunTimeProperty, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setHistoryRunTimeString(newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_PLUGMODE_VALUE_PROPERTY:
        {
            uint16_t newValue;
            status = unmarshalPropertyValue(&MyDevicePlugMode, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setCurrentPlugMode(newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_PLUGMODE_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_VALUE_PROPERTY:
        {
            const char* newValue;
            status = unmarshalPropertyValue(&MyDevicePlugstatusStringProperty, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setPlugStatusString(newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_PLUGSAVEPOWERMODE_VALUE_PROPERTY:
        {
            uint16_t newValue;
            status = unmarshalPropertyValue(&MyDevicePlugSavePowerMode, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setCurrentPowerSaveMode(newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_PLUGSAVEPOWERMODE_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_VALUE_PROPERTY:
        {
            const char* newValue;
            status = unmarshalPropertyValue(&MyDevicePowerSaveStatusStringProperty, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setPowerSaveStatusString(newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_WORKTIMER_VALUE_PROPERTY:
        {
            uint16_t newValue;
            status = unmarshalPropertyValue(&MyDeviceWorkTimer, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setWorkTimerONOFF(newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_WORKTIMER_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_STARTFLIPTIMEPROPERTY_VALUE_PROPERTY:
        {
            TimePropertyValue newValue;
            status = unmarshalPropertyValue(&MyDeviceStartFlipTimeProperty, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setStartFlipTimevar(&newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_STARTFLIPTIMEPROPERTY_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_ENDFLIPTIMEPROPERTY_VALUE_PROPERTY:
        {
            TimePropertyValue newValue;
            status = unmarshalPropertyValue(&MyDeviceEndFlipTimeProperty, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setEndFlipTimevar(&newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_ENDFLIPTIMEPROPERTY_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_RTCDATEPROPERTY_VALUE_PROPERTY:
        {
            DatePropertyValue newValue;
            status = unmarshalPropertyValue(&MyDeviceRTCDateProperty, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setRTCDatevar(&newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_RTCDATEPROPERTY_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_RTCTIMEPROPERTY_VALUE_PROPERTY:
        {
            TimePropertyValue newValue;
            status = unmarshalPropertyValue(&MyDeviceRTCTimeProperty, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setRTCTimevar(&newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_RTCTIMEPROPERTY_SIGNAL_VALUE_CHANGED;
        }
        break;


    }

    return status;
}

AJ_Status ExecuteAction(AJ_Message* msg, uint32_t msgId, ExecuteActionContext* context)
{
    AJ_Message reply;
    AJ_MarshalReplyMsg(msg, &reply);

    switch (msgId) {

    }

    return AJ_DeliverMsg(&reply);
}

void TestsInit(CPSTest* testsToRun)
{

}

AJSVC_ServiceStatus GeneratedMessageProcessor(AJ_BusAttachment* bus, AJ_Message* msg, AJ_Status* msgStatus)
{
    AJSVC_ServiceStatus AJSVC_ServiceStatus = AJSVC_SERVICE_STATUS_HANDLED;

    switch (msg->msgId) {
        GET_WIDGET_VALUE_CASES
        *msgStatus = AJ_BusPropGet(msg, AJCPS_GetWidgetProperty, NULL);
        break;

        GET_WIDGET_ALL_VALUE_CASES
        *msgStatus = AJCPS_GetAllWidgetProperties(msg, msg->msgId);
        break;

        GET_ROOT_VALUE_CASES
        *msgStatus = AJ_BusPropGet(msg, AJCPS_GetRootProperty, NULL);
        break;

        GET_ROOT_ALL_VALUE_CASES
        *msgStatus = AJCPS_GetAllRootProperties(msg, msg->msgId);
        break;

        SET_VALUE_CASES
        {
            SetValueContext context;
            context.sender = msg->sender;
            context.numSignals = 0;
            *msgStatus = AJ_BusPropSet(msg, SetValueProperty, &context);
            if (*msgStatus == AJ_OK && context.numSignals != 0) {
                uint16_t indx;
                for (indx = 0; indx < context.numSignals; indx++) {
                    *msgStatus = AJCPS_SendPropertyChangedSignal(bus, context.signals[indx], AJCPS_GetCurrentSessionId());
                }
            }
        }
        break;

        ACTION_CASES
        {
            ExecuteActionContext context;
            context.numSignals = 0;
            *msgStatus = ExecuteAction(msg, msg->msgId, &context);
            if (*msgStatus == AJ_OK && context.numSignals != 0) {
                uint16_t indx;
                for (indx = 0; indx < context.numSignals; indx++) {
                    if (context.signals[indx].signalType == SIGNAL_TYPE_DATA_CHANGED) {
                        *msgStatus = AJCPS_SendPropertyChangedSignal(bus, context.signals[indx].signalId, AJCPS_GetCurrentSessionId());
                    } else if (context.signals[indx].signalType == SIGNAL_TYPE_DISMISS) {
                        *msgStatus = AJCPS_SendDismissSignal(bus, context.signals[indx].signalId, AJCPS_GetCurrentSessionId());
                    }
                }
            }
        }
        break;

        GET_URL_CASES
        *msgStatus = AJCPS_SendRootUrl(msg, msg->msgId);
        break;

    case AJ_SIGNAL_SESSION_LOST:
        break;

    default:
        AJSVC_ServiceStatus = AJSVC_SERVICE_STATUS_NOT_HANDLED;
        break;
    }

    return AJSVC_ServiceStatus;
}
