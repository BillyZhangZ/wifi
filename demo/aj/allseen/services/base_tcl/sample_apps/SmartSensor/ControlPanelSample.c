/******************************************************************************
 * Copyright (c) 2014, AllSeen Alliance. All rights reserved.
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

#include <alljoyn/controlpanel/ControlPanelService.h>
#include "ControlPanelProvided.h"
#include "ControlPanelGenerated.h"
#include "ControlPanelSample.h"
#include <aj_link_timeout.h>
#include <alljoyn/services_common/PropertyStore.h>

#ifndef NDEBUG
extern AJ_EXPORT uint8_t dbgAJSVCAPP;
#endif

static AJ_Object AJCPS_ObjectList[] = {
    AJCPS_CONTROLLEE_GENERATED_OBJECTS
    { NULL }
};

AJ_Status Controlee_Init()
{
    AJ_Status status = AJCPS_Start(AJCPS_ObjectList, &GeneratedMessageProcessor, &IdentifyMsgOrPropId, &IdentifyMsgOrPropIdForSignal, &IdentifyRootMsgOrPropId);
    WidgetsInit();
    return status;
}

void Controlee_DoWork(AJ_BusAttachment* busAttachment)
{
    uint16_t sendUpdates = checkForUpdatesToSend();
    if (sendUpdates > 0) {

      if ((sendUpdates & (1 << 0)) != 0) {
            AJ_AlwaysPrintf(("##### Sending update signal: Temperature string field \n"));
            AJCPS_SendPropertyChangedSignal(busAttachment, EN_MYDEVICE_CURRENTTEMPERATUREPROPERTY_SIGNAL_VALUE_CHANGED, AJCPS_GetCurrentSessionId());
        }
        if ((sendUpdates & (1 << 1)) != 0) {
            AJ_AlwaysPrintf(("##### Sending update signal: Humidity string field \n"));
            AJCPS_SendPropertyChangedSignal(busAttachment, EN_MYDEVICE_CURRENTHUMIDITYPROPERTY_SIGNAL_VALUE_CHANGED, AJCPS_GetCurrentSessionId());
        }
        if ((sendUpdates & (1 << 2)) != 0) {
            AJ_AlwaysPrintf(("##### Sending update signal: PM2.5 string field \n"));
            AJCPS_SendPropertyChangedSignal(busAttachment, EN_MYDEVICE_CURRENTPM2_5PROPERTY_SIGNAL_VALUE_CHANGED, AJCPS_GetCurrentSessionId());
        }
#if 0
        if ((sendUpdates & (1 << 3)) != 0) {
            AJ_AlwaysPrintf(("##### Sending update signal: Acceleration string field \n"));
            AJCPS_SendPropertyChangedSignal(busAttachment, EN_MYDEVICE_CURRENTACCELERATIONPROPERTY_SIGNAL_VALUE_CHANGED, AJCPS_GetCurrentSessionId());
        }
        if ((sendUpdates & (1 << 4)) != 0) {
            AJ_AlwaysPrintf(("##### Sending update signal: Rotation string field \n"));
            AJCPS_SendPropertyChangedSignal(busAttachment, EN_MYDEVICE_CURRENTROTATIONPROPERTY_SIGNAL_VALUE_CHANGED, AJCPS_GetCurrentSessionId());
        }
        if ((sendUpdates & (1 << 5)) != 0) {
            AJ_AlwaysPrintf(("##### Sending update signal: Magnetic string field \n"));
            AJCPS_SendPropertyChangedSignal(busAttachment, EN_MYDEVICE_CURRENTMAGNETICPROPERTY_SIGNAL_VALUE_CHANGED, AJCPS_GetCurrentSessionId());
        }
#endif
    }
    return;
}

AJ_Status Controlee_Finish(AJ_BusAttachment* busAttachment)
{
    return AJ_OK;
}
