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

#ifndef CONTROL_GENERATED_H_
#define CONTROL_GENERATED_H_

#include <alljoyn.h>
#include <alljoyn/controlpanel/Widgets/ActionWidget.h>
#include <alljoyn/controlpanel/Widgets/PropertyWidget.h>
#include <alljoyn/controlpanel/Widgets/ContainerWidget.h>
#include <alljoyn/controlpanel/Widgets/LabelWidget.h>
#include <alljoyn/controlpanel/Widgets/DialogWidget.h>
#include <alljoyn/controlpanel/Common/HttpControl.h>
#include <alljoyn/services_common/ServicesCommon.h>

/**
 * Defines and objectPaths
 */

#define GET_WIDGET_VALUE_CASES \
case EN_MYDEVICE_ROOTCONTAINER_GET_VALUE: \
case EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_GET_VALUE: \
case EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_GET_VALUE: \
case EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_GET_VALUE: \
case EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_GET_VALUE: \
case EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_GET_VALUE: \
case EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_GET_VALUE: \
case EN_MYDEVICE_CONTROLSPLUGCONTAINER_GET_VALUE: \
case EN_MYDEVICE_PLUGMODE_GET_VALUE: \
case EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_GET_VALUE: \
case EN_MYDEVICE_CONTROLSCONTAINER_GET_VALUE: \
case EN_MYDEVICE_PLUGSAVEPOWERMODE_GET_VALUE: \
case EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_GET_VALUE: \
case EN_MYDEVICE_WORKTIMERCONTAINER_GET_VALUE: \
case EN_MYDEVICE_WORKTIMER_GET_VALUE: \
case EN_MYDEVICE_STARTFLIPTIMEPROPERTY_GET_VALUE: \
case EN_MYDEVICE_ENDFLIPTIMEPROPERTY_GET_VALUE: \
case EN_MYDEVICE_RTCSETCONTAINER_GET_VALUE: \
case EN_MYDEVICE_RTCDATEPROPERTY_GET_VALUE: \
case EN_MYDEVICE_RTCTIMEPROPERTY_GET_VALUE: \


#define GET_WIDGET_ALL_VALUE_CASES \
case EN_MYDEVICE_ROOTCONTAINER_GET_ALL_VALUES: \
case EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_GET_ALL_VALUES: \
case EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_GET_ALL_VALUES: \
case EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_GET_ALL_VALUES: \
case EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_GET_ALL_VALUES: \
case EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_GET_ALL_VALUES: \
case EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_GET_ALL_VALUES: \
case EN_MYDEVICE_CONTROLSPLUGCONTAINER_GET_ALL_VALUES: \
case EN_MYDEVICE_PLUGMODE_GET_ALL_VALUES: \
case EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_GET_ALL_VALUES: \
case EN_MYDEVICE_CONTROLSCONTAINER_GET_ALL_VALUES: \
case EN_MYDEVICE_PLUGSAVEPOWERMODE_GET_ALL_VALUES: \
case EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_GET_ALL_VALUES: \
case EN_MYDEVICE_WORKTIMERCONTAINER_GET_ALL_VALUES: \
case EN_MYDEVICE_WORKTIMER_GET_ALL_VALUES: \
case EN_MYDEVICE_STARTFLIPTIMEPROPERTY_GET_ALL_VALUES: \
case EN_MYDEVICE_ENDFLIPTIMEPROPERTY_GET_ALL_VALUES: \
case EN_MYDEVICE_RTCSETCONTAINER_GET_ALL_VALUES: \
case EN_MYDEVICE_RTCDATEPROPERTY_GET_ALL_VALUES: \
case EN_MYDEVICE_RTCTIMEPROPERTY_GET_ALL_VALUES: \


#define GET_ROOT_VALUE_CASES \
case MYDEVICE_ROOT_CONTROLPANEL_ROOTCONTAINER_GET_VALUE: \


#define GET_ROOT_ALL_VALUE_CASES \
case MYDEVICE_ROOT_CONTROLPANEL_ROOTCONTAINER_GET_ALL_VALUES: \


#define SET_VALUE_CASES \
case MYDEVICE_ROOT_CONTROLPANEL_ROOTCONTAINER_SET_VALUE: \
case EN_MYDEVICE_ROOTCONTAINER_SET_VALUE: \
case EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_SET_VALUE: \
case EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_SET_VALUE: \
case EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_SET_VALUE: \
case EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_SET_VALUE: \
case EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_SET_VALUE: \
case EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_SET_VALUE: \
case EN_MYDEVICE_CONTROLSPLUGCONTAINER_SET_VALUE: \
case EN_MYDEVICE_PLUGMODE_SET_VALUE: \
case EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_SET_VALUE: \
case EN_MYDEVICE_CONTROLSCONTAINER_SET_VALUE: \
case EN_MYDEVICE_PLUGSAVEPOWERMODE_SET_VALUE: \
case EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_SET_VALUE: \
case EN_MYDEVICE_WORKTIMERCONTAINER_SET_VALUE: \
case EN_MYDEVICE_WORKTIMER_SET_VALUE: \
case EN_MYDEVICE_STARTFLIPTIMEPROPERTY_SET_VALUE: \
case EN_MYDEVICE_ENDFLIPTIMEPROPERTY_SET_VALUE: \
case EN_MYDEVICE_RTCSETCONTAINER_SET_VALUE: \
case EN_MYDEVICE_RTCDATEPROPERTY_SET_VALUE: \
case EN_MYDEVICE_RTCTIMEPROPERTY_SET_VALUE: \


#define ACTION_CASES \


#define GET_URL_CASES \


#define AJCPS_CONTROLLEE_GENERATED_OBJECTS \
    {  MyDeviceRootContainerObjectPath, ControlPanelInterfaces, AJ_OBJ_FLAG_HIDDEN | AJ_OBJ_FLAG_DISABLED | AJ_OBJ_FLAG_ANNOUNCED  }, \
    {  enMyDeviceRootContainerObjectPath, ContainerInterfaces  }, \
    {  enMyDeviceCurrentActivePowerPropertyObjectPath, PropertyInterfaces  }, \
    {  enMyDeviceCurrentReactivePowerPropertyObjectPath, PropertyInterfaces  }, \
    {  enMyDeviceCurrentTotalPowerPropertyObjectPath, PropertyInterfaces  }, \
    {  enMyDeviceTotalActivePowerSumPropertyObjectPath, PropertyInterfaces  }, \
    {  enMyDeviceCurrentGridFreqPropertyObjectPath, PropertyInterfaces  }, \
    {  enMyDeviceHistoryRunTimePropertyObjectPath, PropertyInterfaces  }, \
    {  enMyDeviceControlsPlugContainerObjectPath, ContainerInterfaces  }, \
    {  enMyDevicePlugModeObjectPath, PropertyInterfaces  }, \
    {  enMyDevicePlugstatusStringPropertyObjectPath, PropertyInterfaces  }, \
    {  enMyDeviceControlsContainerObjectPath, ContainerInterfaces  }, \
    {  enMyDevicePlugSavePowerModeObjectPath, PropertyInterfaces  }, \
    {  enMyDevicePowerSaveStatusStringPropertyObjectPath, PropertyInterfaces  }, \
    {  enMyDeviceWorkTimerContainerObjectPath, ContainerInterfaces  }, \
    {  enMyDeviceWorkTimerObjectPath, PropertyInterfaces  }, \
    {  enMyDeviceStartFlipTimePropertyObjectPath, PropertyInterfaces  }, \
    {  enMyDeviceEndFlipTimePropertyObjectPath, PropertyInterfaces  }, \
    {  enMyDeviceRTCSetContainerObjectPath, ContainerInterfaces  }, \
    {  enMyDeviceRTCDatePropertyObjectPath, PropertyInterfaces  }, \
    {  enMyDeviceRTCTimePropertyObjectPath, PropertyInterfaces  }, \


#define AJCPS_CONTROLLEE_GENERATED_OBJECTS_COUNT               21

#define MAX_NUM_LANGUAGES                                      1

#define MYDEVICE_MYLANGUAGES_EN 0

#define MYDEVICE_ROOT_CONTROLPANEL_ROOTCONTAINER_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 0, 0, AJ_PROP_GET)
#define MYDEVICE_ROOT_CONTROLPANEL_ROOTCONTAINER_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 0, 0, AJ_PROP_SET)
#define MYDEVICE_ROOT_CONTROLPANEL_ROOTCONTAINER_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 0, 0, AJ_PROP_GET_ALL)
#define MYDEVICE_ROOT_CONTROLPANEL_ROOTCONTAINER_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 0, 1, 0)

#define EN_MYDEVICE_ROOTCONTAINER_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 1, 0, AJ_PROP_GET)
#define EN_MYDEVICE_ROOTCONTAINER_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 1, 0, AJ_PROP_SET)
#define EN_MYDEVICE_ROOTCONTAINER_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 1, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_ROOTCONTAINER_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 1, 1, 0)
#define EN_MYDEVICE_ROOTCONTAINER_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 1, 1, 1)
#define EN_MYDEVICE_ROOTCONTAINER_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 1, 1, 2)
#define EN_MYDEVICE_ROOTCONTAINER_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 1, 1, 3)

#define EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 2, 0, AJ_PROP_GET)
#define EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 2, 0, AJ_PROP_SET)
#define EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 2, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 2, 1, 0)
#define EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 2, 1, 1)
#define EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 2, 1, 2)
#define EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 2, 1, 3)
#define EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 2, 1, 4)
#define EN_MYDEVICE_CURRENTACTIVEPOWERPROPERTY_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 2, 1, 5)

#define EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 3, 0, AJ_PROP_GET)
#define EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 3, 0, AJ_PROP_SET)
#define EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 3, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 3, 1, 0)
#define EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 3, 1, 1)
#define EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 3, 1, 2)
#define EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 3, 1, 3)
#define EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 3, 1, 4)
#define EN_MYDEVICE_CURRENTREACTIVEPOWERPROPERTY_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 3, 1, 5)

#define EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 4, 0, AJ_PROP_GET)
#define EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 4, 0, AJ_PROP_SET)
#define EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 4, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 4, 1, 0)
#define EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 4, 1, 1)
#define EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 4, 1, 2)
#define EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 4, 1, 3)
#define EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 4, 1, 4)
#define EN_MYDEVICE_CURRENTTOTALPOWERPROPERTY_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 4, 1, 5)

#define EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 5, 0, AJ_PROP_GET)
#define EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 5, 0, AJ_PROP_SET)
#define EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 5, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 5, 1, 0)
#define EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 5, 1, 1)
#define EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 5, 1, 2)
#define EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 5, 1, 3)
#define EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 5, 1, 4)
#define EN_MYDEVICE_TOTALACTIVEPOWERSUMPROPERTY_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 5, 1, 5)

#define EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 6, 0, AJ_PROP_GET)
#define EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 6, 0, AJ_PROP_SET)
#define EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 6, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 6, 1, 0)
#define EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 6, 1, 1)
#define EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 6, 1, 2)
#define EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 6, 1, 3)
#define EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 6, 1, 4)
#define EN_MYDEVICE_CURRENTGRIDFREQPROPERTY_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 6, 1, 5)

#define EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 7, 0, AJ_PROP_GET)
#define EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 7, 0, AJ_PROP_SET)
#define EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 7, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 7, 1, 0)
#define EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 7, 1, 1)
#define EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 7, 1, 2)
#define EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 7, 1, 3)
#define EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 7, 1, 4)
#define EN_MYDEVICE_HISTORYRUNTIMEPROPERTY_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 7, 1, 5)

#define EN_MYDEVICE_CONTROLSPLUGCONTAINER_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 8, 0, AJ_PROP_GET)
#define EN_MYDEVICE_CONTROLSPLUGCONTAINER_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 8, 0, AJ_PROP_SET)
#define EN_MYDEVICE_CONTROLSPLUGCONTAINER_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 8, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_CONTROLSPLUGCONTAINER_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 8, 1, 0)
#define EN_MYDEVICE_CONTROLSPLUGCONTAINER_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 8, 1, 1)
#define EN_MYDEVICE_CONTROLSPLUGCONTAINER_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 8, 1, 2)
#define EN_MYDEVICE_CONTROLSPLUGCONTAINER_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 8, 1, 3)

#define EN_MYDEVICE_PLUGMODE_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 9, 0, AJ_PROP_GET)
#define EN_MYDEVICE_PLUGMODE_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 9, 0, AJ_PROP_SET)
#define EN_MYDEVICE_PLUGMODE_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 9, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_PLUGMODE_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 9, 1, 0)
#define EN_MYDEVICE_PLUGMODE_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 9, 1, 1)
#define EN_MYDEVICE_PLUGMODE_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 9, 1, 2)
#define EN_MYDEVICE_PLUGMODE_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 9, 1, 3)
#define EN_MYDEVICE_PLUGMODE_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 9, 1, 4)
#define EN_MYDEVICE_PLUGMODE_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 9, 1, 5)

#define EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 10, 0, AJ_PROP_GET)
#define EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 10, 0, AJ_PROP_SET)
#define EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 10, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 10, 1, 0)
#define EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 10, 1, 1)
#define EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 10, 1, 2)
#define EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 10, 1, 3)
#define EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 10, 1, 4)
#define EN_MYDEVICE_PLUGSTATUSSTRINGPROPERTY_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 10, 1, 5)

#define EN_MYDEVICE_CONTROLSCONTAINER_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 11, 0, AJ_PROP_GET)
#define EN_MYDEVICE_CONTROLSCONTAINER_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 11, 0, AJ_PROP_SET)
#define EN_MYDEVICE_CONTROLSCONTAINER_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 11, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_CONTROLSCONTAINER_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 11, 1, 0)
#define EN_MYDEVICE_CONTROLSCONTAINER_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 11, 1, 1)
#define EN_MYDEVICE_CONTROLSCONTAINER_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 11, 1, 2)
#define EN_MYDEVICE_CONTROLSCONTAINER_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 11, 1, 3)

#define EN_MYDEVICE_PLUGSAVEPOWERMODE_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 12, 0, AJ_PROP_GET)
#define EN_MYDEVICE_PLUGSAVEPOWERMODE_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 12, 0, AJ_PROP_SET)
#define EN_MYDEVICE_PLUGSAVEPOWERMODE_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 12, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_PLUGSAVEPOWERMODE_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 12, 1, 0)
#define EN_MYDEVICE_PLUGSAVEPOWERMODE_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 12, 1, 1)
#define EN_MYDEVICE_PLUGSAVEPOWERMODE_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 12, 1, 2)
#define EN_MYDEVICE_PLUGSAVEPOWERMODE_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 12, 1, 3)
#define EN_MYDEVICE_PLUGSAVEPOWERMODE_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 12, 1, 4)
#define EN_MYDEVICE_PLUGSAVEPOWERMODE_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 12, 1, 5)

#define EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 13, 0, AJ_PROP_GET)
#define EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 13, 0, AJ_PROP_SET)
#define EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 13, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 13, 1, 0)
#define EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 13, 1, 1)
#define EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 13, 1, 2)
#define EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 13, 1, 3)
#define EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 13, 1, 4)
#define EN_MYDEVICE_POWERSAVESTATUSSTRINGPROPERTY_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 13, 1, 5)

#define EN_MYDEVICE_WORKTIMERCONTAINER_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 14, 0, AJ_PROP_GET)
#define EN_MYDEVICE_WORKTIMERCONTAINER_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 14, 0, AJ_PROP_SET)
#define EN_MYDEVICE_WORKTIMERCONTAINER_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 14, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_WORKTIMERCONTAINER_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 14, 1, 0)
#define EN_MYDEVICE_WORKTIMERCONTAINER_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 14, 1, 1)
#define EN_MYDEVICE_WORKTIMERCONTAINER_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 14, 1, 2)
#define EN_MYDEVICE_WORKTIMERCONTAINER_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 14, 1, 3)

#define EN_MYDEVICE_WORKTIMER_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 15, 0, AJ_PROP_GET)
#define EN_MYDEVICE_WORKTIMER_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 15, 0, AJ_PROP_SET)
#define EN_MYDEVICE_WORKTIMER_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 15, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_WORKTIMER_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 15, 1, 0)
#define EN_MYDEVICE_WORKTIMER_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 15, 1, 1)
#define EN_MYDEVICE_WORKTIMER_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 15, 1, 2)
#define EN_MYDEVICE_WORKTIMER_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 15, 1, 3)
#define EN_MYDEVICE_WORKTIMER_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 15, 1, 4)
#define EN_MYDEVICE_WORKTIMER_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 15, 1, 5)

#define EN_MYDEVICE_STARTFLIPTIMEPROPERTY_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 16, 0, AJ_PROP_GET)
#define EN_MYDEVICE_STARTFLIPTIMEPROPERTY_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 16, 0, AJ_PROP_SET)
#define EN_MYDEVICE_STARTFLIPTIMEPROPERTY_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 16, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_STARTFLIPTIMEPROPERTY_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 16, 1, 0)
#define EN_MYDEVICE_STARTFLIPTIMEPROPERTY_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 16, 1, 1)
#define EN_MYDEVICE_STARTFLIPTIMEPROPERTY_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 16, 1, 2)
#define EN_MYDEVICE_STARTFLIPTIMEPROPERTY_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 16, 1, 3)
#define EN_MYDEVICE_STARTFLIPTIMEPROPERTY_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 16, 1, 4)
#define EN_MYDEVICE_STARTFLIPTIMEPROPERTY_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 16, 1, 5)

#define EN_MYDEVICE_ENDFLIPTIMEPROPERTY_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 17, 0, AJ_PROP_GET)
#define EN_MYDEVICE_ENDFLIPTIMEPROPERTY_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 17, 0, AJ_PROP_SET)
#define EN_MYDEVICE_ENDFLIPTIMEPROPERTY_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 17, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_ENDFLIPTIMEPROPERTY_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 17, 1, 0)
#define EN_MYDEVICE_ENDFLIPTIMEPROPERTY_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 17, 1, 1)
#define EN_MYDEVICE_ENDFLIPTIMEPROPERTY_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 17, 1, 2)
#define EN_MYDEVICE_ENDFLIPTIMEPROPERTY_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 17, 1, 3)
#define EN_MYDEVICE_ENDFLIPTIMEPROPERTY_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 17, 1, 4)
#define EN_MYDEVICE_ENDFLIPTIMEPROPERTY_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 17, 1, 5)

#define EN_MYDEVICE_RTCSETCONTAINER_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 18, 0, AJ_PROP_GET)
#define EN_MYDEVICE_RTCSETCONTAINER_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 18, 0, AJ_PROP_SET)
#define EN_MYDEVICE_RTCSETCONTAINER_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 18, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_RTCSETCONTAINER_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 18, 1, 0)
#define EN_MYDEVICE_RTCSETCONTAINER_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 18, 1, 1)
#define EN_MYDEVICE_RTCSETCONTAINER_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 18, 1, 2)
#define EN_MYDEVICE_RTCSETCONTAINER_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 18, 1, 3)

#define EN_MYDEVICE_RTCDATEPROPERTY_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 19, 0, AJ_PROP_GET)
#define EN_MYDEVICE_RTCDATEPROPERTY_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 19, 0, AJ_PROP_SET)
#define EN_MYDEVICE_RTCDATEPROPERTY_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 19, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_RTCDATEPROPERTY_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 19, 1, 0)
#define EN_MYDEVICE_RTCDATEPROPERTY_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 19, 1, 1)
#define EN_MYDEVICE_RTCDATEPROPERTY_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 19, 1, 2)
#define EN_MYDEVICE_RTCDATEPROPERTY_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 19, 1, 3)
#define EN_MYDEVICE_RTCDATEPROPERTY_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 19, 1, 4)
#define EN_MYDEVICE_RTCDATEPROPERTY_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 19, 1, 5)

#define EN_MYDEVICE_RTCTIMEPROPERTY_GET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 20, 0, AJ_PROP_GET)
#define EN_MYDEVICE_RTCTIMEPROPERTY_SET_VALUE                  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 20, 0, AJ_PROP_SET)
#define EN_MYDEVICE_RTCTIMEPROPERTY_GET_ALL_VALUES             AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 20, 0, AJ_PROP_GET_ALL)
#define EN_MYDEVICE_RTCTIMEPROPERTY_VERSION_PROPERTY           AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 20, 1, 0)
#define EN_MYDEVICE_RTCTIMEPROPERTY_STATES_PROPERTY            AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 20, 1, 1)
#define EN_MYDEVICE_RTCTIMEPROPERTY_OPTPARAMS_PROPERTY         AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 20, 1, 2)
#define EN_MYDEVICE_RTCTIMEPROPERTY_SIGNAL_PROPERTIES_CHANGED  AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 20, 1, 3)
#define EN_MYDEVICE_RTCTIMEPROPERTY_VALUE_PROPERTY             AJ_ENCODE_PROPERTY_ID(AJCPS_OBJECT_LIST_INDEX, 20, 1, 4)
#define EN_MYDEVICE_RTCTIMEPROPERTY_SIGNAL_VALUE_CHANGED       AJ_ENCODE_MESSAGE_ID(AJCPS_OBJECT_LIST_INDEX, 20, 1, 5)



extern const char MyDeviceRootContainerObjectPath[];
extern const char enMyDeviceRootContainerObjectPath[];
extern const char enMyDeviceCurrentActivePowerPropertyObjectPath[];
extern const char enMyDeviceCurrentReactivePowerPropertyObjectPath[];
extern const char enMyDeviceCurrentTotalPowerPropertyObjectPath[];
extern const char enMyDeviceTotalActivePowerSumPropertyObjectPath[];
extern const char enMyDeviceCurrentGridFreqPropertyObjectPath[];
extern const char enMyDeviceHistoryRunTimePropertyObjectPath[];
extern const char enMyDeviceControlsPlugContainerObjectPath[];
extern const char enMyDevicePlugModeObjectPath[];
extern const char enMyDevicePlugstatusStringPropertyObjectPath[];
extern const char enMyDeviceControlsContainerObjectPath[];
extern const char enMyDevicePlugSavePowerModeObjectPath[];
extern const char enMyDevicePowerSaveStatusStringPropertyObjectPath[];
extern const char enMyDeviceWorkTimerContainerObjectPath[];
extern const char enMyDeviceWorkTimerObjectPath[];
extern const char enMyDeviceStartFlipTimePropertyObjectPath[];
extern const char enMyDeviceEndFlipTimePropertyObjectPath[];
extern const char enMyDeviceRTCSetContainerObjectPath[];
extern const char enMyDeviceRTCDatePropertyObjectPath[];
extern const char enMyDeviceRTCTimePropertyObjectPath[];


extern ContainerWidget MyDeviceRootContainer;
extern PropertyWidget MyDeviceCurrentActivePowerProperty;
extern PropertyWidget MyDeviceCurrentReactivePowerProperty;
extern PropertyWidget MyDeviceCurrentTotalPowerProperty;
extern PropertyWidget MyDeviceTotalActivePowerSumProperty;
extern PropertyWidget MyDeviceCurrentGridFreqProperty;
extern PropertyWidget MyDeviceHistoryRunTimeProperty;
extern ContainerWidget MyDeviceControlsPlugContainer;
extern PropertyWidget MyDevicePlugMode;
extern PropertyWidget MyDevicePlugstatusStringProperty;
extern ContainerWidget MyDeviceControlsContainer;
extern PropertyWidget MyDevicePlugSavePowerMode;
extern PropertyWidget MyDevicePowerSaveStatusStringProperty;
extern ContainerWidget MyDeviceWorkTimerContainer;
extern PropertyWidget MyDeviceWorkTimer;
extern PropertyWidget MyDeviceStartFlipTimeProperty;
extern PropertyWidget MyDeviceEndFlipTimeProperty;
extern ContainerWidget MyDeviceRTCSetContainer;
extern PropertyWidget MyDeviceRTCDateProperty;
extern PropertyWidget MyDeviceRTCTimeProperty;


typedef struct {
    const char* sender;
    uint16_t numSignals;
    int32_t signals[MAX_NUM_LANGUAGES];
} SetValueContext;

typedef struct {
    uint8_t signalType;
    int32_t signalId;
} Signal;

typedef struct {
    uint16_t numSignals;
    Signal signals[MAX_NUM_LANGUAGES * 2];
} ExecuteActionContext;
/**
 *
 * @return
 */
void WidgetsInit();

/**
 * Set Value of a property.
 * @param replyMsg - reply message
 * @param propId - id of property being changed
 * @param context - setvaluecontext. can be used to send signals
 * @return status
 */
AJ_Status SetValueProperty(AJ_Message* replyMsg, uint32_t propId, void* context);

/**
 * Execute Action
 * @param msg - the msg for the response
 * @param msgId - the Action being executed
 * @param context - ExecuteActionContext used to send signals as a result of action
 * @return status
 */
AJ_Status ExecuteAction(AJ_Message* msg, uint32_t msgId, ExecuteActionContext* context);


/* Defines and functions for Tester App */
typedef struct {
    uint32_t msgId;
    uint16_t numParams;
    uint16_t param[3];
} CPSTest;

#define NUMBER_OF_TESTS //NUM_TESTS_GO_HERE

#define ALL_REPLY_CASES                //ALL_REPLIES_GO_HERE

void TestsInit(CPSTest* testsToRun);

void* IdentifyMsgOrPropId(uint32_t identifier, uint16_t* widgetType, uint16_t* propType, uint16_t* language);
void* IdentifyMsgOrPropIdForSignal(uint32_t identifier, uint8_t* isProperty);
uint8_t IdentifyRootMsgOrPropId(uint32_t identifier);
AJSVC_ServiceStatus GeneratedMessageProcessor(AJ_BusAttachment* bus, AJ_Message* msg, AJ_Status* msgStatus);

#endif /* CONTROL_GENERATED_H_ */