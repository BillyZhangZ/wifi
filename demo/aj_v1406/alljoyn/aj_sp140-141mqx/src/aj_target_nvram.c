/**
 * @file
 */
/******************************************************************************
 * Copyright 2013, Qualcomm Connected Experiences, Inc.
 *
 ******************************************************************************/
#define AJ_MODULE NVRAM

#include "aj_nvram.h"
#include <flashx.h>

#include "aj_debug.h"

/**
 * Turn on per-module debug printing by setting this variable to non-zero value
 * (usually in debugger).
 */
#ifndef NDEBUG
uint8_t dbgNVRAM = 0;
#endif

/*
 * Identifies an AJ NVRAM block
 */
static const char AJ_NV_SENTINEL[8] = "ALLJOYN";

#define INVALID_ID (0)
#define INVALID_DATA (0xFFFF)
#define INVALID_DATA_BYTE (0xFF)

/*
 * How much space in flash we request to allocate for NVRAM. This will be rounded up to a whole
 * number of sectors so the actual space allocated may be larger.
 */
#define AJ_NVRAM_REQUESTED  4096

typedef struct _NV_EntryHeader {
    uint16_t id;
    uint16_t sz;
} NV_EntryHeader;

#define ALIGN_SIZE(x) ((x + 7) & 0xFFF8)


static void NV_ErasePartition(uint32_t offset);

void AJ_NVRAM_Layout_Print(void);

void* AJ_Flash_GetBaseAddr();

/*
 * Flash device handle
 */
static MQX_FILE_PTR flash_hdl;
/*
 * Sector size
 */
static uint32_t NV_SectorSize;
/*
 * Size of each of the two partitions
 */
static uint32_t NV_PartitionSize;
/*
 * Base offset for active parition
 */
static uint32_t NV_Active;
/*
 * Base offset for backup parition
 */
static uint32_t NV_Backup;

static uint8_t* NV_BaseAddr;

static uint8_t NV_Busy;

static void NV_Dump(uint8_t* base, const char* tag)
{
    _mem_size pos = sizeof(AJ_NV_SENTINEL);

    AJ_InfoPrintf(("============ AJ NVRAM Map %s===========\n", tag));
    AJ_InfoPrintf(("0x%x\n", base));

    while (pos < NV_PartitionSize) {
        NV_EntryHeader* nvEntry = (NV_EntryHeader*)(base + pos);
        if (nvEntry->id == INVALID_DATA) {
            break;
        }
        AJ_InfoPrintf(("ID = %d, capacity = %d addr=0x%x\n", nvEntry->id, nvEntry->sz, nvEntry));
        pos += nvEntry->sz + sizeof(NV_EntryHeader);
    }
    AJ_InfoPrintf(("============ End ===========\n"));
}

/*
 * Swap out the active and passive partitions
 */
static void NV_SwapPartitions()
{
    uint32_t tmp = NV_Active;
    NV_Active = NV_Backup;
    NV_Backup = tmp;
    AJ_InfoPrintf(("Active partition base address is now 0x%x\n", NV_BaseAddr + NV_Active));
}

/*
 * Read from the active NVRAM section.
 *
 * @param dest  An offset relative to the start of the active NVRAM section
 * @param data  Buffer to receive the data
 * @param size  The size of the data to read
 */
static int32_t NV_Read(uint32_t src, void* buf, uint16_t size)
{
    uint8_t* base = NV_BaseAddr + NV_Active + src;
    memcpy(buf, base, size);
    return size;
}

/*
 * Write to the active NVRAM section.
 *
 * @param dest  An offset relative to the start of the active NVRAM section
 * @param data  The data to write
 * @param size  The size of the data to write
 */
static int32_t NV_Write(uint32_t dest, void* data, uint16_t size)
{
    _mqx_int num;

    fseek(flash_hdl, NV_Active + dest, IO_SEEK_SET);
    num = write(flash_hdl, data, size);
    if (num != size) {
        AJ_ErrPrintf(("Write NVRAM failed to dest %u\n", dest));
        return -1;
    }
    return num;
}
/*
 * Clear all data sets from the active partition
 */
void AJ_NVRAM_Clear()
{
    AJ_InfoPrintf(("AJ_NVRAM_Clear\n"));
    NV_ErasePartition(NV_Active);
    NV_ErasePartition(NV_Backup);
    /*
     * Write the sentinel string
     */
    if (NV_Write(0, (void *)AJ_NV_SENTINEL, sizeof(AJ_NV_SENTINEL)) != sizeof(AJ_NV_SENTINEL)) {
        AJ_ErrPrintf(("AJ_NVRAM_Clear failed to reset NVRAM\n"));
    }
}
void AJ_NVRAM_Init()
{
    char sentinel[8];
    uint32_t numSectors;
    _mqx_uint err;

    flash_hdl = fopen("flashx:", NULL);
    if (!flash_hdl) {
        AJ_ErrPrintf(("Failed to open flash device\n"));
        return;
    }

    err = ioctl(flash_hdl, FLASH_IOCTL_GET_SECTOR_SIZE, &NV_SectorSize);
    if (err != MQX_OK) {
        AJ_ErrPrintf(("Failed to get sector size\n"));
        goto CloseAndExit;
    }

    err = ioctl(flash_hdl, FLASH_IOCTL_GET_NUM_SECTORS, &numSectors);
    if (err != MQX_OK) {
        AJ_ErrPrintf(("Failed to get num sectors\n"));
        goto CloseAndExit;
    }

    NV_PartitionSize = ((NV_SectorSize - 1 + AJ_NVRAM_REQUESTED) / NV_SectorSize) * NV_SectorSize;
    /*
     * We need space for two partitions
     */
    if ((NV_PartitionSize * 2) > (numSectors * NV_SectorSize)) {
        AJ_ErrPrintf(("AJ_NVRAM_Init: not enough flash to allocated requested NVRAM (%d)\n", AJ_NVRAM_REQUESTED));
        goto CloseAndExit;
    }

    /*
     * We put the NVRAM Active and Backup partitions at the end of flash memory
     */
    NV_Active = (numSectors * NV_SectorSize) - 2 * NV_PartitionSize;
    //NV_Backup = NV_Active + NV_SectorSize;
    NV_Backup = NV_Active + NV_PartitionSize;
    NV_BaseAddr = AJ_Flash_GetBaseAddr();

    /*
     * Figure out where the active and backup partitions are.
     */
    NV_Read(0, sentinel, sizeof(sentinel));
    if (strcmp(AJ_NV_SENTINEL, sentinel) != 0) {
        /*
         * Try swapping Active and Backup
         */
        NV_SwapPartitions();
        NV_Read(0, sentinel, sizeof(sentinel));
        if (strcmp(AJ_NV_SENTINEL, sentinel) != 0) {
            AJ_NVRAM_Clear();
        }
    }
    return;

CloseAndExit:
    fclose(flash_hdl);
    flash_hdl = 0;
}

static NV_EntryHeader* NV_FindEntry(uint16_t id, uint8_t* base)
{
    size_t pos = sizeof(AJ_NV_SENTINEL);
    NV_EntryHeader* nvEntry;

    if (base == NULL) {
        base = NV_BaseAddr + NV_Active;
    }
    while (pos < NV_PartitionSize) {
        nvEntry = (NV_EntryHeader*)(base + pos);
        if (nvEntry->id == id) {
            return nvEntry;
        }
        pos += nvEntry->sz + sizeof(NV_EntryHeader);
    }
    return NULL;
}

/*
 * Erase all segments from the active partition
 */
static void NV_ErasePartition(uint32_t partition)
{
    _mem_size pos = partition;
    _mem_size end = pos + NV_PartitionSize;

    //while (pos < end) 
    {
        _mqx_uint err = MQX_ERROR;
        int try;
        for (try = 0; (err != MQX_OK) && (try < 10); ++try)  {
            size_t i;
            uint32_t* check = (uint32_t*)(NV_BaseAddr + pos);

            err = MQX_OK;
            for (i = 0; i < (NV_PartitionSize / 4); ++i, ++check) {
                if (*check != 0xFFFFFFFF) {
                    err = MQX_ERROR;
                    break;
                }
            }
            if (err != MQX_OK) {
                fseek(flash_hdl, pos, IO_SEEK_SET);
                err = ioctl(flash_hdl, FLASH_IOCTL_ERASE_SECTOR, NULL);
                if ((err != MQX_OK) && (try > 1)) {
                    AJ_ErrPrintf(("ERASE_SECTOR failed try=%d\n", try));
                }
            }
        }
        pos += NV_SectorSize;
    }
}

static NV_EntryHeader* NV_Create(uint8_t* mem, uint16_t id, uint16_t sz)
{
    NV_EntryHeader* entry;

    AJ_ASSERT(sz > 0);

    entry = NV_FindEntry(INVALID_DATA, mem);
    /*
     * Check there is space for this entry
     */
    if (!entry || (sz + sizeof(NV_EntryHeader) > (NV_PartitionSize - ((uint8_t*)entry - mem)))) {
        AJ_ErrPrintf(("Error: Do not have enough NVRAM storage space.\n"));
        return NULL;
    }
    entry->id = id;
    entry->sz = sz;
    return entry;
}

static void DeleteEntry(uint8_t* mem, uint16_t id)
{
    NV_EntryHeader* entry = NV_FindEntry(id, mem);

    if (entry) {
        size_t sz = entry->sz + sizeof(NV_EntryHeader);
        size_t before = (uint8_t*)entry - mem;
        size_t after = NV_PartitionSize - (before + sz);
        /*
         * This should use memmove but memmove is broken
         */
        size_t i;
        uint32_t* dest = (uint32_t*)entry;
        uint32_t* src = dest + sz / 4;
        for (i = 0; i < after / 4; ++i) {
            *dest++ = *src++;
        }
        memset(mem + before + after, INVALID_DATA_BYTE, sz);
    }
}

AJ_Status AJ_NVRAM_Delete(uint16_t id)
{
    uint8_t* partitionBuf;

    if (!flash_hdl) {
        return AJ_ERR_INVALID;
    }
    if (NV_Busy) {
        return AJ_ERR_BUSY;
    }
    if (!NV_FindEntry(id, NULL)) {
        return AJ_ERR_NO_MATCH;
    }
    partitionBuf = AJ_Malloc(NV_PartitionSize);
    if (!partitionBuf) {
        AJ_ErrPrintf(("Failed to allocate partition buffer\n"));
        return AJ_ERR_RESOURCES;
    }
    NV_Read(0, partitionBuf, NV_PartitionSize);
    DeleteEntry(partitionBuf, id);
    NV_Write(0, partitionBuf, NV_PartitionSize);
    AJ_Free(partitionBuf);
    return AJ_OK;
}

AJ_NV_DATASET* AJ_NVRAM_Open(uint16_t id, char* mode, uint16_t capacity)
{
    AJ_Status status = AJ_OK;
    NV_EntryHeader* entry;
    AJ_NV_DATASET* handle = NULL;
    uint8_t* partitionBuf = NULL;

    if (!flash_hdl) {
        return NULL;
    }
    if (NV_Busy) {
        return NULL;
    }
    if (!id) {
        AJ_ErrPrintf(("Error: Zero is an invalid id\n"));
        goto OPEN_ERR_EXIT;
    }
    if (!mode || mode[1] || (*mode != 'r') && (*mode != 'w')) {
        AJ_ErrPrintf(("Error: Access mode must be \"r\" or \"w\"\n"));
        goto OPEN_ERR_EXIT;
    }
    if (*mode == AJ_NV_DATASET_MODE_READ) {
        entry = NV_FindEntry(id, NULL);
        if (!entry) {
            AJ_ErrPrintf(("Error: data set (id = %d) doesn't exist\n", id));
            goto OPEN_ERR_EXIT;
        }
    } else {
        if (capacity == 0) {
            AJ_ErrPrintf(("The capacity cannot be 0.\n"));
            goto OPEN_ERR_EXIT;
        }
        partitionBuf = AJ_Malloc(NV_PartitionSize);
        if (!partitionBuf) {
            AJ_ErrPrintf(("Failed to allocate partition buffer\n"));
            goto OPEN_ERR_EXIT;
        }
        NV_Read(0, partitionBuf, NV_PartitionSize);
        if (AJ_NVRAM_Exist(id)) {
            DeleteEntry(partitionBuf, id);
        }
        entry = NV_Create(partitionBuf, id, ALIGN_SIZE(capacity));
        if (!entry) {
            goto OPEN_ERR_EXIT;
        }
    }

    handle = (AJ_NV_DATASET*)AJ_Malloc(sizeof(AJ_NV_DATASET));
    if (!handle) {
        AJ_ErrPrintf(("AJ_NVRAM_Open() error: OutOfMemory. \n"));
        goto OPEN_ERR_EXIT;
    }

    handle->id = id;
    handle->curPos = 0;
    handle->capacity = entry->sz;
    handle->mode = *mode;
    handle->inode = (void*)entry;
    handle->internal = partitionBuf;
    NV_Busy = TRUE;
    return handle;

OPEN_ERR_EXIT:
    if (handle) {
        AJ_Free(handle);
    }
    if (partitionBuf) {
        AJ_Free(partitionBuf);
    }
    AJ_ErrPrintf(("AJ_NVRAM_Open() fails: status = %d. \n", status));
    return NULL;
}

size_t AJ_NVRAM_Write(void* ptr, uint16_t size, AJ_NV_DATASET* handle)
{
    if (!flash_hdl) {
        return -1;
    }
    if (!handle || handle->mode == AJ_NV_DATASET_MODE_READ) {
        AJ_ErrPrintf(("AJ_NVRAM_Write() error: The access mode does not allow write.\n"));
        return -1;
    }
    if (size > (handle->capacity - handle->curPos)) {
        AJ_ErrPrintf(("AJ_NVRAM_Write() error: No space to write %d bytes\n", size));
        return -1;
    }
    if (size > 0) {
        NV_EntryHeader* entry = (NV_EntryHeader*)handle->inode;
        memcpy((uint8_t*)entry + sizeof(NV_EntryHeader) + handle->curPos, ptr, size); 
        handle->curPos += size;
    }
    return size;
}

size_t AJ_NVRAM_Read(void* ptr, uint16_t size, AJ_NV_DATASET* handle)
{
    if (!flash_hdl) {
        return -1;
    }
    if (!handle || handle->mode == AJ_NV_DATASET_MODE_WRITE) {
        AJ_ErrPrintf(("AJ_NVRAM_Read() error: The access mode does not allow read.\n"));
        return -1;
    }
    size = min(size, handle->capacity -  handle->curPos);
    if (size > 0) {
        NV_EntryHeader* entry = (NV_EntryHeader*)handle->inode;
        if (handle->curPos == handle->capacity) {
            AJ_ErrPrintf(("AJ_NVRAM_Read() error: No data to read\n"));
            return -1;
        }
        memcpy(ptr, (uint8_t*)entry + sizeof(NV_EntryHeader) + handle->curPos, size); 
        handle->curPos += size;
    }
    return size;
}

AJ_Status AJ_NVRAM_Close(AJ_NV_DATASET* handle)
{
    AJ_Status status = AJ_OK;
    if (!flash_hdl) {
        return AJ_ERR_INVALID;
    }
    if (!handle) {
        AJ_ErrPrintf(("AJ_NVRAM_Close() error: Invalid handle. \n"));
        return AJ_ERR_INVALID;
    }
    if (handle->mode == AJ_NV_DATASET_MODE_WRITE) {
        NV_ErasePartition(NV_Backup);
        NV_SwapPartitions();
        if (NV_Write(0, (uint8_t*)handle->internal, NV_PartitionSize) != NV_PartitionSize) {
            AJ_ErrPrintf(("Failed to write - restoring original data partition\n"));
            NV_SwapPartitions();
            status = AJ_ERR_WRITE;
        } else {
            NV_ErasePartition(NV_Backup);
        }
        AJ_Free(handle->internal);
    }
    AJ_Free(handle);
    NV_Busy = FALSE;
    return status;
}

uint8_t AJ_NVRAM_Exist(uint16_t id)
{
    return flash_hdl && id && (NV_FindEntry(id, NULL) != NULL);
}

void AJ_NVRAM_Layout_Print()
{
    NV_Dump(NV_BaseAddr + NV_Active, "=");
}

/*******************************************************
 *                                                     *
 *     Direct flash memory access functions            *
 *                                                     *
 *******************************************************/

size_t AJ_Flash_GetSize()
{
    if (!flash_hdl) {
        return 0;
    } else {
        uint32_t numSectors;
        _mqx_uint err;

        err = ioctl(flash_hdl, FLASH_IOCTL_GET_NUM_SECTORS, &numSectors);
        if (err != MQX_OK) {
            AJ_ErrPrintf(("Failed to get num sectors\n"));
            return 0;
        } else {
            return (numSectors * NV_SectorSize) - (2 * NV_PartitionSize);
        }
    }
}
#if 1   //Add by Evanguo
uint32_t AJ_NVRAM_GetSizeRemaining(void)
{
    return AJ_NVRAM_SIZE - AJ_Flash_GetSize();
}
#endif
void* AJ_Flash_GetBaseAddr()
{
    if (!flash_hdl) {
        return NULL;
    }
    if (NV_BaseAddr) {
        return NV_BaseAddr;
    } else {
        _mem_size baseAddr;
        _mqx_uint err;

        fseek(flash_hdl, 0, IO_SEEK_SET);
        err = ioctl(flash_hdl, FLASH_IOCTL_GET_SECTOR_BASE, &baseAddr);
        if (err != MQX_OK) {
            AJ_ErrPrintf(("FLASH_IOCTL_GET_SECTOR_BASE failed.\n"));
            return NULL;
        } else {
            return (void*)baseAddr;
        }
    }
}

AJ_Status AJ_Flash_Clear()
{
    if (!flash_hdl) {
        return AJ_ERR_INVALID;
    } else {
        AJ_InfoPrintf(("AJ_Flash_Clear()\n"));
        _mqx_uint err;
        _mem_size pos = 0;
        _mem_size end = AJ_Flash_GetSize();
        uint8_t blank[8];
        memset(blank, INVALID_DATA_BYTE, sizeof(blank));

        while (pos < end) {
            fseek(flash_hdl, pos, IO_SEEK_SET);
            err = ioctl(flash_hdl, FLASH_IOCTL_ERASE_SECTOR, NULL);
            if (err != MQX_OK) {
                AJ_ErrPrintf(("Failed to erase sector\n"));
                return AJ_ERR_WRITE;
            }
            pos += NV_SectorSize;
        }
        fseek(flash_hdl, 0, IO_SEEK_SET);
        return AJ_OK;
    }
}

AJ_Status AJ_Flash_Write(uint32_t offset, void* data, size_t len)
{
    if (!flash_hdl) {
        return AJ_ERR_INVALID;
    } else {
        if (NV_Write(offset - NV_Active, data, len) != len) {
            return AJ_ERR_WRITE;
        } else {
            return AJ_OK;
        }
    }
}
