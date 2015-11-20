/**
 * @file
 */
/******************************************************************************
 * Copyright 2013-2014, Qualcomm Connected Experiences, Inc.
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
uint8_t dbgNVRAM = 1;
#endif

/*
 * Identifies an AJ NVRAM block
 */
static const char AJ_NV_SENTINEL[8] = "ALLJOYN";

#define DELETED_ENTRY (0)
#define UNUSED_ENTRY  (0xFFFF)

/*
 * How much space in flash we request to allocate for NVRAM. This will be rounded up to a whole
 * number of sectors so the actual space allocated may be larger.
 */
#define AJ_NVRAM_REQUESTED  AJ_NVRAM_SIZE

typedef struct _NV_EntryHeader {
    uint16_t id;
    uint16_t sz;
} NV_EntryHeader;

#define ALIGN_SIZE(x) ((x + 7) & 0xFFF8)


static void NV_ErasePartition();

void AJ_NVRAM_Layout_Print(void);

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
static uint32_t NV_Offset;

static uint8_t* NV_BaseAddr;

/*
 * Indicates an read or write operation is in progress
 */
static uint8_t NV_Busy;

/*
 * Indicates if there are deleted entries
 */
static uint8_t NV_IsCompact;

#define NV_PartitionAddr (NV_BaseAddr + NV_Offset)

void* Flash_GetBaseAddr()
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

uint32_t AJ_NVRAM_GetSize(void)
{
    uint32_t size = sizeof(AJ_NV_SENTINEL);
    _mem_size pos = sizeof(AJ_NV_SENTINEL);

    while (pos < NV_PartitionSize) {
        NV_EntryHeader* nvEntry = (NV_EntryHeader*)(NV_PartitionAddr + pos);
        if (nvEntry->id == UNUSED_ENTRY) {
            break;
        }
        size += nvEntry->sz + sizeof(NV_EntryHeader);
        pos += nvEntry->sz + sizeof(NV_EntryHeader);
    }
    return size;
}

static void NV_Dump(uint8_t* base, const char* tag)
{
    _mem_size pos = sizeof(AJ_NV_SENTINEL);

    AJ_InfoPrintf(("============ AJ NVRAM Map %s===========\n", tag));
    AJ_InfoPrintf(("0x%x\n", base));

    while (pos < NV_PartitionSize) {
        NV_EntryHeader* nvEntry = (NV_EntryHeader*)(base + pos);
        if (nvEntry->id == UNUSED_ENTRY) {
            break;
        }
        AJ_InfoPrintf(("ID = %d, capacity = %d addr=0x%x\n", nvEntry->id, nvEntry->sz, nvEntry));
        pos += nvEntry->sz + sizeof(NV_EntryHeader);
    }
    AJ_InfoPrintf(("============ End ===========\n"));
}

/*
 * Returns the sector base address and offset for an address
 */
static uint8_t* SectorBaseAddr(uint8_t* addr, size_t* offset)
{
    size_t pos = addr - NV_PartitionAddr;
    size_t sec = pos / NV_SectorSize;

    *offset = pos % NV_SectorSize;
    return NV_PartitionAddr + (sec * NV_SectorSize);
}

static int32_t NV_Read(uint32_t src, void* buf, uint16_t size)
{
    uint8_t* base = NV_PartitionAddr + src;
    memcpy(buf, base, size);
    return size;
}

static int32_t NV_Write(uint32_t dest, void* data, uint16_t size)
{
    _mqx_int num;

    fseek(flash_hdl, dest, IO_SEEK_SET);
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
    NV_ErasePartition();
    /*
     * Write the sentinel string
     */
    if (NV_Write(NV_Offset, (void *)AJ_NV_SENTINEL, sizeof(AJ_NV_SENTINEL)) != sizeof(AJ_NV_SENTINEL)) {
        AJ_ErrPrintf(("AJ_NVRAM_Clear failed to reset NVRAM\n"));
    }
}

static uint8_t NV_CheckIsCompact()
{
    size_t pos = sizeof(AJ_NV_SENTINEL);
    uint8_t* base = NV_PartitionAddr;

    while (pos < NV_PartitionSize) {
        NV_EntryHeader* nvEntry = (NV_EntryHeader*)(base + pos);
        if (nvEntry->id == DELETED_ENTRY) {
            return FALSE;
        }
        if (nvEntry->id == UNUSED_ENTRY) {
            break;
        }
        pos += nvEntry->sz + sizeof(NV_EntryHeader);
    }
    return TRUE;
}

void AJ_NVRAM_Init()
{
    uint32_t numSectors;
    char sentinel[8];
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
     * We put the NVRAM partition at the end of flash memory so properties are preserved across
     * reflashing the application.
     */
    NV_Offset = (numSectors * NV_SectorSize) - NV_PartitionSize;
    NV_BaseAddr = Flash_GetBaseAddr();

    /*
     * Figure out if NVRAM has been initialized
     */
    NV_Read(0, sentinel, sizeof(sentinel));
    if (strcmp(AJ_NV_SENTINEL, sentinel) != 0) {
        AJ_NVRAM_Clear();
    } else {
        NV_IsCompact = NV_CheckIsCompact();
    }
    return;

CloseAndExit:
    fclose(flash_hdl);
    flash_hdl = 0;
}

static NV_EntryHeader* NV_FindEntry(uint16_t id)
{
    size_t pos = sizeof(AJ_NV_SENTINEL);
    uint8_t* base = NV_PartitionAddr;
    NV_EntryHeader* nvEntry;

    if (id == DELETED_ENTRY) {
        return NULL;
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

static void NV_EraseSector(uint8_t* addr)
{
    _mem_size pos = addr - NV_BaseAddr;
    _mqx_uint err = MQX_ERROR;
    int try;

    for (try = 0; (err != MQX_OK) && (try < 10); ++try)  {
        size_t i;
        uint32_t* check = (uint32_t*)(NV_BaseAddr + pos);

        err = MQX_OK;
        for (i = 0; i < (NV_SectorSize / 4); ++i, ++check) {
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
}

/*
 * Erase all segments from the active partition
 */
static void NV_ErasePartition()
{
    size_t pos;
    for (pos = 0; pos < NV_PartitionSize; pos += NV_SectorSize) {
        NV_EraseSector(NV_PartitionAddr + pos);
    }
    NV_IsCompact = TRUE;
}

/*
 * Use a sector buffer to compact the NVRAM partition
 */
static void NV_Compact()
{
    size_t pos = sizeof(AJ_NV_SENTINEL);
    uint8_t* base = NV_PartitionAddr;
    uint32_t offset = NV_Offset;
    uint8_t* sectorBuf;
    size_t room;
    uint8_t* p;

    AJ_InfoPrintf(("NV_Compact\n"));

    sectorBuf = AJ_Malloc(NV_SectorSize);
    if (!sectorBuf) {
        AJ_ErrPrintf(("Failed to allocate sector buffer\n"));
        return;
    }
    memcpy(sectorBuf, NV_PartitionAddr, sizeof(AJ_NV_SENTINEL));
    room = NV_SectorSize - pos;
    p = sectorBuf + pos;

    while (pos < NV_PartitionSize) {
        NV_EntryHeader* nvEntry = (NV_EntryHeader*)(base + pos);
        size_t sz = nvEntry->sz + sizeof(NV_EntryHeader);

        if (nvEntry->id == UNUSED_ENTRY) {
            /*
             * Clear flash to the end of NVRAM
             */
            memset(p, 0xFF, room);
            NV_Write(offset, sectorBuf, NV_SectorSize);
            offset += NV_SectorSize;
            while ((offset - NV_Offset) < NV_PartitionSize) {
                NV_EraseSector(NV_BaseAddr + offset);
                offset += NV_SectorSize;
            }
            break;
        }
        pos += sz;
        if (nvEntry->id == DELETED_ENTRY) {
            continue;
        }
        while (sz) {
            size_t cp = min(sz, room);
            memcpy(p, nvEntry, cp);
            room -= cp;
            if (room == 0) {
                NV_Write(offset, sectorBuf, NV_SectorSize);
                room = NV_SectorSize;
                p = sectorBuf;
                offset += NV_SectorSize;
            }
            p += sz;
            sz -= cp;
        }
    }
    AJ_Free(sectorBuf);
    NV_IsCompact = TRUE;
}

uint32_t AJ_NVRAM_GetSizeRemaining(void)
{
    if (!NV_IsCompact) {
        NV_Compact();
    }
    return NV_PartitionSize - AJ_NVRAM_GetSize();
}

static NV_EntryHeader* NV_Reserve(uint16_t sz)
{
    uint8_t try = 0;

    AJ_ASSERT(sz > 0);

    /*
     * Sanity check
     */
    if ((sz + sizeof(NV_EntryHeader)) > NV_PartitionSize) {
        AJ_ErrPrintf(("Size to big for NVRAM\n"));
        return NULL;
    }
    /*
     * Check there is space for this entry
     */
    while (TRUE) {
        if ((sz + sizeof(NV_EntryHeader) <= AJ_NVRAM_GetSizeRemaining())) {
            break;
        }
        if (++try == 2) {
            AJ_ErrPrintf(("Not enough NVRAM free space\n"));
            return NULL;
        }
    }
    return NV_FindEntry(UNUSED_ENTRY);
}

AJ_Status AJ_NVRAM_Delete(uint16_t id)

{
    NV_EntryHeader* entry;
    size_t offset;
    uint8_t* sectorBuf;
    uint8_t* sectorAddr;

    if (!flash_hdl) {
        return AJ_ERR_INVALID;
    }
    if (NV_Busy) {
        return AJ_ERR_BUSY;
    }
    entry = NV_FindEntry(id);
    if (!entry) {
        return AJ_ERR_NO_MATCH;
    }
    sectorBuf = AJ_Malloc(NV_SectorSize);
    if (!sectorBuf) {
        AJ_ErrPrintf(("Failed to allocate partition buffer\n"));
        return AJ_ERR_RESOURCES;
    }
    /*
     * Read the sector into RAM, zero out the entry id then write it back.
     */
    sectorAddr = SectorBaseAddr((uint8_t*)entry, &offset);
    memcpy(sectorBuf, sectorAddr, NV_SectorSize);
    entry = (NV_EntryHeader*)(sectorBuf + offset);
    entry->id = 0;
    NV_EraseSector(sectorAddr);
    NV_Write(sectorAddr - NV_BaseAddr, sectorBuf, NV_SectorSize);

    AJ_Free(sectorBuf);
    NV_IsCompact = FALSE;
    return AJ_OK;
}

AJ_NV_DATASET* AJ_NVRAM_Open(uint16_t id, char* mode, uint16_t capacity)
{
    AJ_Status status = AJ_OK;
    NV_EntryHeader* entry = NV_FindEntry(id);
    AJ_NV_DATASET* handle = NULL;

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
        if (!entry) {
            AJ_ErrPrintf(("Error: data set (id = %d) doesn't exist\n", id));
            goto OPEN_ERR_EXIT;
        }
        capacity = entry->sz;
    } else {
        if (capacity == 0) {
            AJ_ErrPrintf(("The capacity cannot be 0\n"));
            goto OPEN_ERR_EXIT;
        }
        capacity = ALIGN_SIZE(capacity);
        if (entry && (capacity > entry->sz)) {
            status = AJ_NVRAM_Delete(id);
            if (status != AJ_OK) {
                goto OPEN_ERR_EXIT;
            }
            entry = NULL;
        }
        if (!entry) {
            entry = NV_Reserve(capacity);
        }
        if (!entry) {
            goto OPEN_ERR_EXIT;
        }
    }

    handle = (AJ_NV_DATASET*)AJ_Malloc(sizeof(AJ_NV_DATASET));
    if (!handle) {
        AJ_ErrPrintf(("AJ_NVRAM_Open() error: OutOfMemory\n"));
        goto OPEN_ERR_EXIT;
    }

    handle->id = id;
    handle->curPos = 0;
    handle->capacity = capacity;
    handle->mode = *mode;
    handle->inode = (void*)entry;
    NV_Busy = TRUE;
    return handle;

OPEN_ERR_EXIT:

    if (handle) {
        AJ_Free(handle);
    }
    AJ_ErrPrintf(("AJ_NVRAM_Open() %s\n", AJ_StatusText(status)));
    return NULL;
}

size_t AJ_NVRAM_Write(const void* ptr, uint16_t size, AJ_NV_DATASET* handle)
{
    size_t wrote = size;
    const uint8_t* data = (const uint8_t*)ptr;
    uint8_t* sectorBuf;

    if (!flash_hdl) {
        return -1;
    }
    if (!handle || handle->mode == AJ_NV_DATASET_MODE_READ) {
        AJ_ErrPrintf(("AJ_NVRAM_Write() handle was opened for READ\n"));
        return -1;
    }
    if (size > (handle->capacity - handle->curPos)) {
        AJ_ErrPrintf(("AJ_NVRAM_Write() error: No space to write %d bytes\n", size));
        return -1;
    }
    sectorBuf = AJ_Malloc(NV_SectorSize);
    if (!sectorBuf) {
        AJ_ErrPrintf(("Failed to allocate sector buffer\n"));
        return -1;
    }
    do {
        size_t writeSz;
        size_t offset;
        uint8_t* sectorAddr;
        if (handle->curPos == 0) {
            sectorAddr = SectorBaseAddr(handle->inode, &offset);
        } else {
            sectorAddr = SectorBaseAddr(handle->inode + sizeof(NV_EntryHeader) + handle->curPos, &offset);
        }
        memcpy(sectorBuf, sectorAddr, NV_SectorSize);
        /*
         * If this is the first write copy in the entry header
         */
        if (handle->curPos == 0) {
            NV_EntryHeader hdr;
            hdr.sz = handle->capacity;
            hdr.id = handle->id;
            memcpy(sectorBuf + offset, &hdr, sizeof(hdr));
            offset += sizeof(hdr);
        }
        /*
         * How much room is there left in the sector?
         */
        writeSz = min(NV_SectorSize - offset, size);
        memcpy(sectorBuf + offset, data, writeSz);
        NV_EraseSector(sectorAddr);
        NV_Write(sectorAddr - NV_BaseAddr, sectorBuf, NV_SectorSize);
        handle->curPos += writeSz;
        data += writeSz;
        size -= writeSz;
    } while (size > 0);

    AJ_Free(sectorBuf);
    return wrote;
}

const void* AJ_NVRAM_Peek(AJ_NV_DATASET* handle)
{
    NV_EntryHeader* entry;

    if (!flash_hdl) {
        return NULL;
    }
    if (!handle || (handle->mode == AJ_NV_DATASET_MODE_WRITE)) {
        AJ_ErrPrintf(("AJ_NVRAM_Read() handle opened for WRITE\n"));
        return NULL;
    }
    entry = (NV_EntryHeader*)handle->inode;
    return (uint8_t*)entry + sizeof(NV_EntryHeader) + handle->curPos;
}

size_t AJ_NVRAM_Read(void* ptr, uint16_t size, AJ_NV_DATASET* handle)
{
    if (!flash_hdl) {
        return -1;
    }
    if (!handle || (handle->mode == AJ_NV_DATASET_MODE_WRITE)) {
        AJ_ErrPrintf(("AJ_NVRAM_Read() handle opened for WRITE\n"));
        return -1;
    }
    size = min(size, handle->capacity -  handle->curPos);
    if (size > 0) {
        NV_EntryHeader* entry = (NV_EntryHeader*)handle->inode;
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
    if ((handle->mode == AJ_NV_DATASET_MODE_WRITE) && (handle->curPos == 0)) {
        /*
         * This ensures that the entry is actually created
         */
        AJ_NVRAM_Write(NULL, 0, handle);
    }
    AJ_Free(handle);
    NV_Busy = FALSE;
    return status;
}

uint8_t AJ_NVRAM_Exist(uint16_t id)
{
    return flash_hdl && id && (NV_FindEntry(id) != NULL);
}

void AJ_NVRAM_Layout_Print()
{
    NV_Dump(NV_PartitionAddr, "=");
}
