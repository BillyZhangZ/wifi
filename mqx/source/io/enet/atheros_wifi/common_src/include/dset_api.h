#ifndef __dset_api_h__
#define __dset_api_h__

/**HEADER********************************************************************
* 
*
* Copyright (c) 2014    Qualcomm Atheros Inc.;
* All Rights Reserved
*
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
* $FileName: dset.h$
* $Version : 3.8.40.0$
* $Date    : May-28-2014$
*
* Comments:
*
*   This file contains the defines, externs and data
*   structure definitions required by application
*   programs in order to use the Ethernet packet driver.
*
*END************************************************************************/


/*--------------------------------------------------------------------------*/
/*                        
**                            CONSTANT DEFINITIONS
*/

#define			MAX_HOST_DSET_SIZE				8

#define			INVALID_DSET_ID					0

typedef  enum  __DSET_OP  {
		DSET_OP_CREATE  = 1,
		DSET_OP_OPEN  = 2,
		DSET_OP_READ  = 3,
		DSET_OP_WRITE  = 4,
		DSET_OP_COMMIT = 5,
		DSET_OP_CLOSE  = 6,
		DSET_OP_SIZE   = 7,
		DSET_OP_DELETE  = 8,
} DSET_OP;

/*--------------------------------------------------------------------------*/
/*                        
**                            TYPE DEFINITIONS
*/

typedef void  (*dset_callback_fn_t)(uint32_t status, void *pv);

typedef struct host_api_dset_struct {
   uint_32     dset_id;         /* dset id    */
   uint_32     media_id;
   uint_32     flags;
   uint_32     offset;          /* dset offset            */
   uint_32     length;          /* dset length            */
   uint_32     left_len;        /* for large read/write   */
   dset_callback_fn_t  cb;
   void		   *cb_args;
   uint_8      done_flag;
   uint_8      *data_ptr;        /* dset buffer address */
} HOST_DSET_HANDLE;

//typedef struct host_dset_item {
//   uint_32     dset_id;         /* dset id    */
//   uint_32     length;          /* dset length            */
//} HOST_DSET_ITEM;

/*--------------------------------------------------------------------------*/
/*                        
**                            PROTOTYPES AND GLOBAL EXTERNS
*/

extern HOST_DSET_HANDLE   dset_handles[];

HOST_DSET_HANDLE *dset_find_handle(uint32_t dset_id);

uint_32    api_host_dset_create(HOST_DSET_HANDLE **pDsetHandle, uint32_t dset_id, uint_32 media_id, uint32_t length, uint32_t flags, dset_callback_fn_t create_cb, void *callback_arg);
uint_32    api_host_dset_open(HOST_DSET_HANDLE **pDsetHandle, uint32_t dset_id, uint32_t flags, dset_callback_fn_t open_cb, void *callback_arg);
uint_32    api_host_dset_write(HOST_DSET_HANDLE *pDsetHandle, uint8_t *buffer, uint32_t length, uint32_t offset, uint32_t flags, dset_callback_fn_t write_cb, void *callback_arg);
uint_32    api_host_dset_read(HOST_DSET_HANDLE *pDsetHandle, uint8_t *buffer, uint32_t length, uint32_t offset, dset_callback_fn_t  read_cb, void *callback_arg);
uint_32    api_host_dset_commit(HOST_DSET_HANDLE *pDsetHandle, dset_callback_fn_t  read_cb, void *callback_arg);
uint_32    api_host_dset_close(HOST_DSET_HANDLE *pDsetHandle, dset_callback_fn_t  read_cb, void *callback_arg);
uint_32    api_host_dset_size(HOST_DSET_HANDLE *pDsetHandle, dset_callback_fn_t  read_cb, void *callback_arg);
uint_32    api_host_dset_delete(uint32_t dset_id, uint32_t flags, dset_callback_fn_t  read_cb, void *callback_arg);

#endif