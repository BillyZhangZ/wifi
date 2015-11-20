/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 2004-2008 Embedded Access Inc.;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: coff.c$
* $Version : 3.5.9.0$
* $Date    : Jan-22-2010$
*
* Comments:
*
*   This file contains the COFF File decoder for the
*   Exec function of the RTCS Communication Library.
*
*END************************************************************************/
#include <a_config.h>
#include <a_types.h>
#include <a_osapi.h>

#include "dset.h"

HOST_DSET    host_dsets[MAX_DSET_SIZE];

HOST_DSET  *dset_find(uint_32 dset_id)
{
    uint_16   i;
    HOST_DSET  *pDset;

    for (i=0, pDset=host_dsets; i < MAX_DSET_SIZE; i++, pDset++)
    {
        if (pDset->dset_id == dset_id)
            return  pDset;
    }

    return NULL;
}

HOST_DSET  *dset_insert(uint_32 dset_id)
{
    uint_16   i;
    HOST_DSET  *pDset;

    for (i=0, pDset=host_dsets; i < MAX_DSET_SIZE; i++, pDset++)
    {
	if (pDset->dset_id == 0)
        {
            pDset->dset_id = dset_id;
            if (pDset->data_ptr)
            {
                A_FREE(pDset->data_ptr, MALLOC_ID_CONTEXT);
                pDset->data_ptr = NULL;
            }
            
            return  pDset;
        }
    }

    return NULL;
}

HOST_DSET  *dset_get(uint_32 dset_id, uint_32 length)
{
    uchar _PTR_  pbuf;

    HOST_DSET  *pDset;

    pDset = dset_find(dset_id);
    
    if (pDset == NULL)
    {
        pDset = dset_insert(dset_id);
        if (pDset == NULL)
            return pDset;
    }
    
    /* Free the data buffer and reallocate based on new length */
    if (pDset->data_ptr)
    {
        A_FREE(pDset->data_ptr, MALLOC_ID_CONTEXT);
        pDset->data_ptr = NULL;
    }
    
    pbuf = (uchar _PTR_)A_MALLOC(length, MALLOC_ID_CONTEXT);
    if (pbuf == NULL)
    {
       pDset->dset_id = 0;
       return  NULL;
    }

    pDset->data_ptr = pbuf;
    pDset->length = length;

    return pDset;
}

uint_32 dset_write(HOST_DSET *pDset, uint_8 *pData, uint_32 offset, uint_32 length)
{
    uint_32   wrt_len;

    if (offset + length > pDset->length)
        wrt_len = pDset->length - offset;
    else
        wrt_len = length;

    memcpy(pDset->data_ptr + offset, pData, wrt_len);

    return  wrt_len;
}

uint_32 dset_read(HOST_DSET *pDset, uint_8 *pData, uint_32 offset, uint_32 length)
{
    uint_32   wrt_len;

    if (offset + length > pDset->length)
        wrt_len = pDset->length - offset;
    else
        wrt_len = length;

    memcpy(pData, pDset->data_ptr + offset, wrt_len);

    return  wrt_len;
}

uint_32    dset_fill(uint_8 *pData, uint_32 max_dset_count)
{
    uint_16   i, count = 0;
    HOST_DSET  *pDset;
    HOST_DSET_ITEM  *pItem = (HOST_DSET_ITEM *)pData;

    for (i=0, pDset=host_dsets; i < MAX_DSET_SIZE  && count < max_dset_count; i++, pDset++)
    {
		if (pDset->dset_id != 0)
		{
            pItem->dset_id = pDset->dset_id;
            pItem->length =  pDset->length;
            pItem ++;
            count ++;
		}
    }

    return count;
}

uint_16   next_dset_ndx;

HOST_DSET  *dset_get_first()
{
    uint_16   i;
    HOST_DSET  *pDset;

    for (i=0, pDset=host_dsets; i < MAX_DSET_SIZE; i++, pDset++)
    {
		if (pDset->dset_id != 0)
        {
            next_dset_ndx = i + 1; 
            return  pDset;
        }
    }

    return NULL;
}

HOST_DSET  *dset_get_next()
{
    uint_16   i;
    HOST_DSET  *pDset;

    pDset = &host_dsets[next_dset_ndx];

    for (i = next_dset_ndx; i < MAX_DSET_SIZE; i++, pDset++)
    {
		if (pDset->dset_id != 0)
        {
            next_dset_ndx = i + 1; 
            return  pDset;
        }
    }

    return NULL;
}