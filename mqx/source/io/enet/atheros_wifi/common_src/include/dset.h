#ifndef __dset_h__
#define __dset_h__

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

#define			MAX_DSET_SIZE				16

/*--------------------------------------------------------------------------*/
/*                        
**                            TYPE DEFINITIONS
*/

typedef struct host_dset_struct {
   uint_32     dset_id;         /* dset id    */
   uint_32     length;          /* dset length            */
   uint_8     *data_ptr;        /* dset buffer address */
} HOST_DSET;

typedef struct host_dset_item {
   uint_32     dset_id;         /* dset id    */
   uint_32     length;          /* dset length            */
} HOST_DSET_ITEM;


/*--------------------------------------------------------------------------*/
/*                        
**                            PROTOTYPES AND GLOBAL EXTERNS
*/

HOST_DSET  *dset_find(uint_32 dset_id);
HOST_DSET  *dset_get(uint_32 dset_id, uint_32 length);
uint_32    dset_write(HOST_DSET *pDset, uint_8 *pData, uint_32 offset, uint_32 length);
uint_32    dset_read(HOST_DSET *pDset, uint_8 *pData, uint_32 offset, uint_32 length);
uint_32    dset_fill(uint_8 *pData, uint_32 max_dset_count);

HOST_DSET  *dset_get_first();
HOST_DSET  *dset_get_next();




#endif