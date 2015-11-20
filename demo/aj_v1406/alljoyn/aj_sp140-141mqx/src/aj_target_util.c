/**
 * @file
 */
/******************************************************************************
 * Copyright 2013, Qualcomm Connected Experiences, Inc.
 *
 ******************************************************************************/
#include <stdlib.h>
#include <qca_includes.h>

#include "aj_wifi_ctrl.h"
#include "aj_target.h"
#include "aj_util.h"
#include <mqx.h>

/*
 * max time to suspend WiFi
 */
#define MAX_SUSPEND 10 * 1000

#if defined BSP_LED2 && defined BSP_LED3
#define LED_HOT   BSP_LED2
#define LED_COOL  BSP_LED3
#endif

void AJ_Sleep(uint32_t time)
{
#ifdef SUSPEND_WIFI_WHILE_SLEEPING
    /*
     * if we're sleeping for over 200 msec, put WIFI to sleep to conserve power
     */
    if (time > 200) {
        /*
         * but don't sleep for more than MAX_SUSPEND otherwise we could
         * lose association with the AP
         */
        uint32_t mintime = min(time, MAX_SUSPEND);
        AJ_SuspendWifi(mintime);
    }
#endif
    _time_delay(time);
}

uint32_t AJ_GetElapsedTime(AJ_Time* timer, uint8_t cumulative)
{
    uint32_t elapsed;
    TIME_STRUCT now;

    _time_get_elapsed(&now);

    elapsed = (1000 * (now.SECONDS - timer->seconds)) + (uint32_t)(now.MILLISECONDS - timer->milliseconds);
    if (!cumulative) {
        timer->seconds = now.SECONDS;
        timer->milliseconds = now.MILLISECONDS;
    }
    return elapsed;
}
void AJ_InitTimer(AJ_Time* timer)
{
    TIME_STRUCT now;
    _time_get_elapsed(&now);
    timer->seconds = now.SECONDS;
    timer->milliseconds = now.MILLISECONDS;
}
void* AJ_Malloc(size_t sz)
{
    void* mem = A_MALLOC(sz, MALLOC_ID_CONTEXT);
    if (!mem) {
        AJ_Printf("!!!Failed to malloc %d bytes\n", sz);
    }
    return mem;
}

void AJ_Free(void* mem)
{
    if (mem) {
        A_FREE(mem, MALLOC_ID_CONTEXT);
    }
}

uint8_t AJ_StartReadFromStdIn()
{
    /* Not implemented */
    return FALSE;
}

uint8_t AJ_StopReadFromStdIn()
{
    /* Not implemented */
    return FALSE;
}

char* AJ_GetCmdLine(char* buf, size_t num)
{
    /* Not implemented */
    return NULL;
}

void AJ_Reboot(void)
{
    A_UINT32 read_value;

    read_value = SCB_AIRCR;
    read_value &= ~SCB_AIRCR_VECTKEY_MASK;
    read_value |= SCB_AIRCR_VECTKEY(0x05FA);
    read_value |= SCB_AIRCR_SYSRESETREQ_MASK;

    _int_disable();
    SCB_AIRCR = read_value;
    while(1);  /* Ensure completion of memory access */
}

int AJ_ChangeTemperature(int step){

#if defined BSP_LED2 && defined BSP_LED3
    static MQX_FILE_PTR hotport = NULL, coolport = NULL;

    static const uint_32 hotport_set[] = {
        LED_HOT | GPIO_PIN_STATUS_0,
        GPIO_LIST_END
    };

    static const uint_32 coolport_set[] = {
        LED_COOL | GPIO_PIN_STATUS_0,
        GPIO_LIST_END
    };

    if(hotport == NULL)
        hotport = fopen("gpio:write", (char_ptr) &hotport_set);

    if (coolport == NULL)
        coolport = fopen("gpio:write", (char_ptr) &coolport_set);

    if(step > 0){
        if (hotport){
            ioctl(hotport, GPIO_IOCTL_WRITE_LOG0, NULL);    
            _time_delay(300 * step);
            ioctl(hotport, GPIO_IOCTL_WRITE_LOG1, NULL);    
        }

    }
    else{
        if (hotport){
            ioctl(coolport, GPIO_IOCTL_WRITE_LOG0, NULL);    
            _time_delay(300 * (-step));
            ioctl(coolport, GPIO_IOCTL_WRITE_LOG1, NULL);    
        }
    }
#endif
    return 0;
}

static AJMODEBUTTONCB  ajModeCB;

AJMODEBUTTONCB AJ_Get_ModeButton_CB(void){
    return ajModeCB;
}

int AJ_Register_ModeButton_CB(AJMODEBUTTONCB cb){
    ajModeCB = cb;
    return 0;
}

#ifndef NDEBUG

int _AJ_DbgEnabled(char* module)
{
    return FALSE;
}

#endif
#define Tarteg_SWAP16(v) (((v) >> 8) | ((v) << 8))
#define Tarteg_SWAP32(v) (((v) >> 24) | (((v) & 0xFF0000) >> 8) | (((v) & 0x00FF00) << 8) | ((v) << 24))
#define Tarteg_SWAP64(v) (((v) >> 56) |\
					(((v) & 0x00ff000000000000) >> 40) |\
					(((v) & 0x0000ff0000000000) >> 24) |\
					(((v) & 0x000000ff00000000) >> 8)|\
					(((v) & 0x00000000ff000000) << 8)|\
					(((v) & 0x0000000000ff0000) << 24)|\
					(((v) & 0x000000000000ff00) << 40)|\
					(((v) << 56)))



uint16_t AJ_ByteSwap16(uint16_t x)
{
    return Tarteg_SWAP16(x);
}

uint32_t AJ_ByteSwap32(uint32_t x)
{
    return Tarteg_SWAP32(x);
}

uint64_t AJ_ByteSwap64(uint64_t x)
{
    return Tarteg_SWAP64(x);
}
void* AJ_Realloc(void* ptr, size_t size)
{
    AJ_Free(ptr);
    return AJ_Malloc(size);
}

