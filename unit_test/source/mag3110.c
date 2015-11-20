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

/****************************************************************************/
#include <mqx.h>
#include <bsp.h>

#include "i2c_common.h"

/**************************************************************************
  Local header files for this application
 **************************************************************************/
#include "mag3110.h"


/**************************************************************************
  Global variables
 **************************************************************************/



/**************************************************************************
  Global variables
 **************************************************************************/
int unit_test_mag3110(){
   //MQX_FILE_PTR         fd;
   uint8_t              mag3110_device_id;
   
   I2C_Init();
   I2C_ReadRegister8(I2C_mag3110_ADDRESS,I2C_mag3110_ID_REG,&mag3110_device_id);
   I2C_DeInit();
   printf ("The mag3110's device ID is 0x%x  \n",mag3110_device_id);

   if(mag3110_device_id != mag3110_ID_REG_VAL)
	   return -1;
   return 0;
}

                        
/* end file */


