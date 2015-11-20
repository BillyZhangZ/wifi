/**
 * @file
 */
/******************************************************************************
 * Copyright 2013-2014, Qualcomm Connected Experiences, Inc.
 *
 ******************************************************************************/
#include <stdlib.h>

#include "aj_wifi_ctrl.h"
#include "aj_target.h"
#include "aj_util.h"
#include <mqx.h>
#include "qca_includes.h"
#include "aj_malloc.h"

#if BSP_K22FSH                                 
#include <sensors.h>
#endif
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

static const AJ_HeapConfig heapConfig[] = {
    { 8,    4, 1 },
    { 16,   4, 1 },
    { 32,   2, 1 },
    { 64,   2, 1 },
    { 128,  2, 1 },
    { 256,  2, 1 },
    { 384,  2, 1 },
    { 512,  2, 0 },
    { 4096, 1, 0 }
};
#define WSL_HEAP_WORD_COUNT (7172 / 4)
static uint32_t heap[WSL_HEAP_WORD_COUNT];

void AJ_PoolAllocInit(void)
{
    //prepare the heap
    size_t heapSz;

    heapSz = AJ_PoolRequired(heapConfig, ArraySize(heapConfig));
    if (heapSz > sizeof(heap)) {
        AJ_Printf("Heap space is too small %d required %d\n", sizeof(heap), heapSz);
        return;
    }
    AJ_PoolInit(heap, heapSz, heapConfig, ArraySize(heapConfig));
    AJ_PoolDump();
}

void* AJ_Malloc(size_t sz)
{
    void* mem = AJ_PoolAlloc(sz);
    if (!mem) {
        AJ_Printf("!!!Failed to malloc %d bytes\n", sz);
    }
    return mem;
}
void AJ_Free(void* mem)
{
    if ((mem > (void*)&heap) && (mem < (void*)&heap[WSL_HEAP_WORD_COUNT])) {
        AJ_PoolFree(mem);
    }
}

void* AJ_Realloc(void* ptr, size_t size)
{
    return AJ_PoolRealloc(ptr, size);
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

uint16_t AJ_ByteSwap16(uint16_t x)
{
    return ((x >> 8) & 0x00FF) | ((x << 8 & 0xFF00));
}

uint32_t AJ_ByteSwap32(uint32_t x)
{
    return ((x >> 24) & 0x000000FF) | ((x >> 8) & 0x0000FF00) |
           ((x << 24) & 0xFF000000) | ((x << 8) & 0x00FF0000);
}

uint64_t AJ_ByteSwap64(uint64_t x)
{
    return ((x >> 56) & 0x00000000000000FF) | ((x >> 40) & 0x000000000000FF00) |
           ((x << 56) & 0xFF00000000000000) | ((x << 40) & 0x00FF000000000000) |
           ((x >> 24) & 0x0000000000FF0000) | ((x >>  8) & 0x00000000FF000000) |
           ((x << 24) & 0x0000FF0000000000) | ((x <<  8) & 0x000000FF00000000);
}
#ifndef NDEBUG

int _AJ_DbgEnabled(char* module)
{
#ifdef ER_DEBUG_ALL
    return TRUE;
#else
    return FALSE;
#endif
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


int AJ_Sensors_Fetch(AJ_SENSOR_TYPE type, void *buf){

    int ret = -1;

    switch(type){
        case AJ_SENSOR_TEMPERATURE:
            {
                unsigned int *pTemp = (unsigned int *)buf;

#if BSP_K22FSH                                 
                ret = sensors_read_temperature_as_celsius(pTemp);
#endif

            }
            break;
        case AJ_SENSOR_HUMIDITY:
            {
                unsigned int *pHumidity = (unsigned int *)buf;
                
#if BSP_K22FSH                                 
                ret = sensors_read_humidity(pHumidity);
#endif
            }
            break;
         case AJ_SENSOR_PM2_5:
            {
                unsigned int *pPM2_5 = (unsigned int *)buf;
                
#if BSP_K22FSH                                 
                ret = sensors_read_PM2_5(pPM2_5);
#endif
            }
            break;
        default:
            break;
    }

    return ret;
}


#endif

