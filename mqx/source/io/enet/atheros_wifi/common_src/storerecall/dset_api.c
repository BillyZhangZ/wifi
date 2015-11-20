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
#include <wmi.h>
#include <atheros_wifi_api.h>

#include "dset_api.h"

HOST_DSET_HANDLE   dset_handles[MAX_HOST_DSET_SIZE];

uint_32 remote_dset_op(DSET_OP op, HOST_DSET_HANDLE *pDsetHandle);

HOST_DSET_HANDLE *dset_find_handle(uint32_t dset_id)
{
	uint_16	i;
	HOST_DSET_HANDLE  *pDsetHandle = dset_handles;

    for (i=0; i<MAX_HOST_DSET_SIZE; i++)
    {
		if (pDsetHandle->dset_id == dset_id)
			return pDsetHandle; 
    }
 	return NULL;
}

HOST_DSET_HANDLE *dset_insert_handle(uint32_t dset_id, uint32_t flags, dset_callback_fn_t cb, void *cb_args)
{
	uint_16	i;
	HOST_DSET_HANDLE  *pDsetHandle = dset_handles;

    for (i=0; i<MAX_HOST_DSET_SIZE; i++)
    {
		if (pDsetHandle->dset_id == INVALID_DSET_ID)
		{
			pDsetHandle->dset_id = dset_id;
			pDsetHandle->flags = flags;
			pDsetHandle->cb = cb;
            pDsetHandle->cb_args = cb_args;

			return pDsetHandle;
		}; 
    }
 	return NULL;
}

uint_32    api_host_dset_create(HOST_DSET_HANDLE **pDsetHandle, uint32_t dset_id, uint_32 media_id, uint32_t length, uint32_t flags, dset_callback_fn_t create_cb, void *callback_arg)
{
	uint_32		status;

	*pDsetHandle = dset_find_handle(dset_id);
    if (*pDsetHandle != NULL)
		return A_FALSE;

	*pDsetHandle = dset_insert_handle(dset_id, flags, create_cb, callback_arg);
	if (*pDsetHandle == NULL)
		return A_FALSE;

	(*pDsetHandle)->length = length;
	(*pDsetHandle)->media_id = media_id;

	status = remote_dset_op(DSET_OP_CREATE, *pDsetHandle);

	return  status;
}

uint_32    api_host_dset_open(HOST_DSET_HANDLE **ppDsetHandle, uint32_t dset_id, uint32_t flags, dset_callback_fn_t open_cb, void *callback_arg)
{
	uint_32		status;

	*ppDsetHandle = dset_find_handle(dset_id);
    if (*ppDsetHandle != NULL)
		return A_OK;

	*ppDsetHandle = dset_insert_handle(dset_id, flags, open_cb, callback_arg);
	if (*ppDsetHandle == NULL)
		return A_FALSE;

	status = remote_dset_op(DSET_OP_OPEN, *ppDsetHandle);

	return  status;
}

uint_32    api_host_dset_write(HOST_DSET_HANDLE *pDsetHandle, uint8_t *buffer, uint32_t length, uint32_t offset, uint32_t flags, dset_callback_fn_t write_cb, void *callback_arg)
{
	uint_32		status;

    pDsetHandle->offset = offset;
    pDsetHandle->length = length;
    pDsetHandle->flags = flags;
    pDsetHandle->cb = write_cb;
    pDsetHandle->cb_args = callback_arg;
	pDsetHandle->data_ptr = buffer;

	status = remote_dset_op(DSET_OP_WRITE, pDsetHandle);
	return status;
}

uint_32    api_host_dset_read(HOST_DSET_HANDLE *pDsetHandle, uint8_t *buffer, uint32_t length, uint32_t offset, dset_callback_fn_t  read_cb, void *callback_arg)
{
	uint_32		status;

    pDsetHandle->offset = offset;
    pDsetHandle->length = length;

    pDsetHandle->cb = read_cb;
    pDsetHandle->cb_args = callback_arg;
	pDsetHandle->data_ptr = buffer;

	status = remote_dset_op(DSET_OP_READ, pDsetHandle);
	return status;
}

uint_32    api_host_dset_commit(HOST_DSET_HANDLE *pDsetHandle, dset_callback_fn_t commit_cb, void *callback_arg)
{
	uint_32		status;

    pDsetHandle->cb = commit_cb;
    pDsetHandle->cb_args = callback_arg;
	status = remote_dset_op(DSET_OP_COMMIT, pDsetHandle);
	return status;
}

uint_32    api_host_dset_close(HOST_DSET_HANDLE *pDsetHandle, dset_callback_fn_t close_cb, void *callback_arg)
{
	uint_32		status;

    pDsetHandle->cb = close_cb;
    pDsetHandle->cb_args = callback_arg;

	status = remote_dset_op(DSET_OP_CLOSE, pDsetHandle);
	return status;
}

uint_32    api_host_dset_size(HOST_DSET_HANDLE *pDsetHandle, dset_callback_fn_t  size_cb, void *callback_arg)
{
	uint_32		status;

    pDsetHandle->cb = size_cb;
    pDsetHandle->cb_args = callback_arg;

	status = remote_dset_op(DSET_OP_SIZE, pDsetHandle);
	return status;
}

uint_32    api_host_dset_delete(uint32_t dset_id, uint32_t flags, dset_callback_fn_t  delete_cb, void *callback_arg)
{
	uint_32		status;
    HOST_DSET_HANDLE *pDsetHandle;

	pDsetHandle = dset_find_handle(dset_id);
    if (pDsetHandle == NULL)
	{
		pDsetHandle = dset_insert_handle(dset_id, flags, delete_cb, callback_arg);
		if (pDsetHandle == NULL)
			return A_FALSE;
	}
	else
	{	
		pDsetHandle->cb = delete_cb;
		pDsetHandle->cb_args = callback_arg;
		pDsetHandle->dset_id = dset_id;
		pDsetHandle->flags = flags;
	}

	status = remote_dset_op(DSET_OP_DELETE, pDsetHandle);
	return status;
}


