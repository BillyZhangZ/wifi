/**HEADER********************************************************************
 * 
 * Copyright (c) 2014 Freescale Semiconductor;
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
 * Comments:
 *
 *END************************************************************************/

#ifndef I2C_COMMON_H
#define I2C_COMMON_H

#include <mqx.h>
#include <bsp.h>
#include <i2c.h>
#include "psptypes_legacy.h"

#define I2C_COMMON_DEBUG
extern MQX_FILE_PTR fd;

//#define I2C_SHARED        /* for i2c bus share with more than one device in same times */

#ifdef I2C_SHARED
extern LWSEM_STRUCT  i2c_sem;

#define  NEW_I2C_SEM \
do { \
	    _lwsem_create((LWSEM_STRUCT_PTR)(&i2c_sem), 1);\
} while(0);

#define   GET_I2C_SEM \
do { \
	    _lwsem_wait((LWSEM_STRUCT_PTR)&i2c_sem); \
} while(0);

#define   PUT_I2C_SEM \
do { \
	    _lwsem_post((LWSEM_STRUCT_PTR)&i2c_sem); \
} while(0);

#define  DEL_I2C_SEM \
do { \
	   _lwsem_destroy((LWSEM_STRUCT_PTR)&i2c_sem);\
} while(0);
#else
#define  NEW_I2C_SEM
#define   GET_I2C_SEM 
#define   PUT_I2C_SEM
#define  DEL_I2C_SEM
#endif

/*****************************************************************************
******************************************************************************
* Public prototypes
******************************************************************************
*****************************************************************************/
_mqx_int I2C_Init(void);
//_mqx_int I2C_Init(unsigned char u8I2CSlaveAddress);
_mqx_int I2C_DeInit(void);

int I2C_WriteRegister8(unsigned char, unsigned char , unsigned char );
int I2C_ReadRegister8(unsigned char address,unsigned char reg,unsigned char * reg_val);

_mqx_int I2C_ReadRegister16(unsigned char address,uint16_t reg, uint_16_ptr dest_ptr);
_mqx_int I2C_WriteRegister16(unsigned char address,uint16_t reg, uint_16 reg_val);

int i2c_SetAddress(unsigned char u8I2CSlaveAddress);



#endif
