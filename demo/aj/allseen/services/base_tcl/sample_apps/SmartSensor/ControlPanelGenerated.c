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
const char enMyDeviceSensorInfoContainerObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/SensorInfoContainer";
const char enMyDeviceCurrentTemperaturePropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/SensorInfoContainer/CurrentTemperatureProperty";
const char enMyDeviceCurrentHumidityPropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/SensorInfoContainer/CurrentHumidityProperty";
const char enMyDeviceCurrentPM2_5PropertyObjectPath[] = "/ControlPanel/MyDevice/rootContainer/en/SensorInfoContainer/CurrentPM2_5Property";

ContainerWidget MyDeviceRootContainer;
ContainerWidget MyDeviceSensorInfoContainer;
PropertyWidget MyDeviceCurrentTemperatureProperty;
PropertyWidget MyDeviceCurrentHumidityProperty;
PropertyWidget MyDeviceCurrentPM2_5Property;

/*
 * Static variables used to fill widgets
 */
static const uint16_t MyDeviceRootContainerHints[] = { LAYOUT_HINT_VERTICAL_LINEAR, LAYOUT_HINT_HORIZONTAL_LINEAR };
static const uint16_t MyDeviceSensorInfoContainerHints[] = { LAYOUT_HINT_VERTICAL_LINEAR };
static const char* const MyDeviceCurrentTemperaturePropertySignature = "s";
static const char* const MyDeviceCurrentTemperaturePropertyLabel[] = { "1: Temperature:" };
static const uint16_t MyDeviceCurrentTemperaturePropertyHints[] = { PROPERTY_WIDGET_HINT_TEXTVIEW };
static const char* const MyDeviceCurrentHumidityPropertySignature = "s";
static const char* const MyDeviceCurrentHumidityPropertyLabel[] = { "2: Humidity:" };
static const uint16_t MyDeviceCurrentHumidityPropertyHints[] = { PROPERTY_WIDGET_HINT_TEXTVIEW };
static const char* const MyDeviceCurrentPM2_5PropertySignature = "s";
static const char* const MyDeviceCurrentPM2_5PropertyLabel[] = { "3: PM2.5:" };
static const uint16_t MyDeviceCurrentPM2_5PropertyHints[] = { PROPERTY_WIDGET_HINT_TEXTVIEW };


void WidgetsInit()
{
    initializeContainerWidget(&MyDeviceRootContainer);
    MyDeviceRootContainer.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceRootContainer.base, TRUE);

    MyDeviceRootContainer.base.optParams.bgColor = 0x1e90ff;
    MyDeviceRootContainer.base.optParams.hints = MyDeviceRootContainerHints;
    MyDeviceRootContainer.base.optParams.numHints = 2;

    initializeContainerWidget(&MyDeviceSensorInfoContainer);
    MyDeviceSensorInfoContainer.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceSensorInfoContainer.base, TRUE);

    MyDeviceSensorInfoContainer.base.optParams.bgColor = 0x200;
    MyDeviceSensorInfoContainer.base.optParams.hints = MyDeviceSensorInfoContainerHints;
    MyDeviceSensorInfoContainer.base.optParams.numHints = 1;

    initializePropertyWidget(&MyDeviceCurrentTemperatureProperty);
    MyDeviceCurrentTemperatureProperty.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceCurrentTemperatureProperty.base, TRUE);
    setBaseWritable(&MyDeviceCurrentTemperatureProperty.base, FALSE);

    MyDeviceCurrentTemperatureProperty.signature = MyDeviceCurrentTemperaturePropertySignature;
    MyDeviceCurrentTemperatureProperty.propertyType = SINGLE_VALUE_PROPERTY;
    MyDeviceCurrentTemperatureProperty.getValue = &getCurrentTemperatureString;

    MyDeviceCurrentTemperatureProperty.base.optParams.bgColor = 0x500;
    MyDeviceCurrentTemperatureProperty.base.optParams.label = MyDeviceCurrentTemperaturePropertyLabel;
    MyDeviceCurrentTemperatureProperty.base.optParams.hints = MyDeviceCurrentTemperaturePropertyHints;
    MyDeviceCurrentTemperatureProperty.base.optParams.numHints = 1;

    initializePropertyWidget(&MyDeviceCurrentHumidityProperty);
    MyDeviceCurrentHumidityProperty.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceCurrentHumidityProperty.base, TRUE);
    setBaseWritable(&MyDeviceCurrentHumidityProperty.base, FALSE);

    MyDeviceCurrentHumidityProperty.signature = MyDeviceCurrentHumidityPropertySignature;
    MyDeviceCurrentHumidityProperty.propertyType = SINGLE_VALUE_PROPERTY;
    MyDeviceCurrentHumidityProperty.getValue = &getCurrentHumidityString;

    MyDeviceCurrentHumidityProperty.base.optParams.bgColor = 0x500;
    MyDeviceCurrentHumidityProperty.base.optParams.label = MyDeviceCurrentHumidityPropertyLabel;
    MyDeviceCurrentHumidityProperty.base.optParams.hints = MyDeviceCurrentHumidityPropertyHints;
    MyDeviceCurrentHumidityProperty.base.optParams.numHints = 1;

    initializePropertyWidget(&MyDeviceCurrentPM2_5Property);
    MyDeviceCurrentPM2_5Property.base.numLanguages = 1;
    setBaseEnabled(&MyDeviceCurrentPM2_5Property.base, TRUE);
    setBaseWritable(&MyDeviceCurrentPM2_5Property.base, FALSE);

    MyDeviceCurrentPM2_5Property.signature = MyDeviceCurrentPM2_5PropertySignature;
    MyDeviceCurrentPM2_5Property.propertyType = SINGLE_VALUE_PROPERTY;
    MyDeviceCurrentPM2_5Property.getValue = &getCurrentPM2_5String;

    MyDeviceCurrentPM2_5Property.base.optParams.bgColor = 0x500;
    MyDeviceCurrentPM2_5Property.base.optParams.label = MyDeviceCurrentPM2_5PropertyLabel;
    MyDeviceCurrentPM2_5Property.base.optParams.hints = MyDeviceCurrentPM2_5PropertyHints;
    MyDeviceCurrentPM2_5Property.base.optParams.numHints = 1;

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

    case EN_MYDEVICE_SENSORINFOCONTAINER_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceSensorInfoContainer;

    case EN_MYDEVICE_SENSORINFOCONTAINER_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceSensorInfoContainer;

    case EN_MYDEVICE_SENSORINFOCONTAINER_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceSensorInfoContainer;

    case EN_MYDEVICE_SENSORINFOCONTAINER_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_CONTAINER;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceSensorInfoContainer;

    case EN_MYDEVICE_CURRENTTEMPERATUREPROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentTemperatureProperty;

    case EN_MYDEVICE_CURRENTTEMPERATUREPROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentTemperatureProperty;

    case EN_MYDEVICE_CURRENTTEMPERATUREPROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentTemperatureProperty;

    case EN_MYDEVICE_CURRENTTEMPERATUREPROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentTemperatureProperty;

    case EN_MYDEVICE_CURRENTTEMPERATUREPROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentTemperatureProperty;

    case EN_MYDEVICE_CURRENTHUMIDITYPROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentHumidityProperty;

    case EN_MYDEVICE_CURRENTHUMIDITYPROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentHumidityProperty;

    case EN_MYDEVICE_CURRENTHUMIDITYPROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentHumidityProperty;

    case EN_MYDEVICE_CURRENTHUMIDITYPROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentHumidityProperty;

    case EN_MYDEVICE_CURRENTHUMIDITYPROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentHumidityProperty;

    case EN_MYDEVICE_CURRENTPM2_5PROPERTY_GET_ALL_VALUES:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentPM2_5Property;

    case EN_MYDEVICE_CURRENTPM2_5PROPERTY_VERSION_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VERSION;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentPM2_5Property;

    case EN_MYDEVICE_CURRENTPM2_5PROPERTY_STATES_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_STATES;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentPM2_5Property;

    case EN_MYDEVICE_CURRENTPM2_5PROPERTY_OPTPARAMS_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_OPTPARAMS;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentPM2_5Property;

    case EN_MYDEVICE_CURRENTPM2_5PROPERTY_VALUE_PROPERTY:
        *widgetType = WIDGET_TYPE_PROPERTY;
        *propType = PROPERTY_TYPE_VALUE;
        *language = MYDEVICE_MYLANGUAGES_EN;
        return &MyDeviceCurrentPM2_5Property;

    default:
        return FALSE;
    }
}

void* IdentifyMsgOrPropIdForSignal(uint32_t identifier, uint8_t* isProperty)
{
    switch (identifier) {
    case EN_MYDEVICE_ROOTCONTAINER_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceRootContainer;

    case EN_MYDEVICE_SENSORINFOCONTAINER_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceSensorInfoContainer;

    case EN_MYDEVICE_CURRENTTEMPERATUREPROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceCurrentTemperatureProperty;

    case EN_MYDEVICE_CURRENTTEMPERATUREPROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDeviceCurrentTemperatureProperty;

    case EN_MYDEVICE_CURRENTHUMIDITYPROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceCurrentHumidityProperty;

    case EN_MYDEVICE_CURRENTHUMIDITYPROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDeviceCurrentHumidityProperty;

    case EN_MYDEVICE_CURRENTPM2_5PROPERTY_SIGNAL_PROPERTIES_CHANGED:
        return &MyDeviceCurrentPM2_5Property;

    case EN_MYDEVICE_CURRENTPM2_5PROPERTY_SIGNAL_VALUE_CHANGED:
        *isProperty = TRUE;
        return &MyDeviceCurrentPM2_5Property;

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
    case EN_MYDEVICE_CURRENTTEMPERATUREPROPERTY_VALUE_PROPERTY:
        {
            const char* newValue;
            status = unmarshalPropertyValue(&MyDeviceCurrentTemperatureProperty, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setCurrentTemperatureString(newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_CURRENTTEMPERATUREPROPERTY_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_CURRENTHUMIDITYPROPERTY_VALUE_PROPERTY:
        {
            const char* newValue;
            status = unmarshalPropertyValue(&MyDeviceCurrentHumidityProperty, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setCurrentHumidityString(newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_CURRENTHUMIDITYPROPERTY_SIGNAL_VALUE_CHANGED;
        }
        break;

    case EN_MYDEVICE_CURRENTPM2_5PROPERTY_VALUE_PROPERTY:
        {
            const char* newValue;
            status = unmarshalPropertyValue(&MyDeviceCurrentPM2_5Property, replyMsg, &newValue, ((SetValueContext*)context)->sender);
            if (status != AJ_OK) {
                return status;
            }
            setCurrentPM2_5String(newValue);
            ((SetValueContext*)context)->numSignals = 1;
            ((SetValueContext*)context)->signals[0] = EN_MYDEVICE_CURRENTPM2_5PROPERTY_SIGNAL_VALUE_CHANGED;
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
