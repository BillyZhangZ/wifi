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
#include "sht25.h"


/**************************************************************************
  Global variables
 **************************************************************************/



/**************************************************************************
  Global variables
 **************************************************************************/

const uint16_t POLYNOMIAL = 0x131;

unsigned char sth_CRC8(uint8_t * data, uint8_t len, uint8_t checksum)
{
    uint8_t crc = 0;
    uint8_t i;
    //calculates 8-Bit checksum with given polynomial
    for (i = 0; i < len; ++i) { 
        crc ^= (data[i]);
        for (uint8_t bit = 8; bit > 0; --bit) {
            if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
            else crc = (crc << 1);
        }
    }
    if (crc != checksum) return CHECKSUM_ERROR;
    else return 0;
}


static void delay_us(int num)
{
  int n;
  for(n=1; n < num; n++) 
  {
	#if (defined(__CWCC__) || defined(__IAR_SYSTEMS_ICC__) || defined(__GNUC__))
	  asm("nop");
	#elif defined (__CC_ARM)
	  __nop();
    #endif
  }
}

static void delay_ms(int num)
{
	_time_delay(num);
}

//
int sht25_measurement(uint8_t type, uint8_t holdm,uint16_t *data)
{
	uint_32 result;
	uint8_t cmd;
	uint8_t buffer[3];
    uint_32 rx_request = 1;  /* fk the only set 1 then ack after receive */
	int i;
	int start_i = 0;
	I2C_STATISTICS_STRUCT stats;

	*data = 0;

	//1. set i2c address
	i2c_SetAddress(I2C_sht25_ADDRESS);

	//2. write command
	if ((type == MEASUREMENT_TYPE_T) && (holdm == HOLD_MASTER_MODE))
		cmd = T_MEASUREMENT_HOLDM_CMD;
	else if ((type == MEASUREMENT_TYPE_T) && (holdm == POLLING_MODE))
		cmd = T_MEASUREMENT_POLL_CMD;
	else if ((type == MEASUREMENT_TYPE_RH) && (holdm == HOLD_MASTER_MODE))
		cmd = RH_MEASUREMENT_HOLDM_CMD;
	else if ((type == MEASUREMENT_TYPE_RH) && (holdm == POLLING_MODE))
		cmd = RH_MEASUREMENT_POLL_CMD;
	else {
		printf("invalid type or hold mode\n");
	}

	buffer[0] =	(uint_8)(cmd & 0xFF);
	result = write(fd, buffer, 1);
	if (1 != result) {
		#ifdef I2C_COMMON_DEBUG
		 printf("_Write: Error - write to address %d\n",cmd);
		#endif
		return(-1);
	}

	result = fflush (fd);
	if (MQX_OK != result) {
		printf("flush err\n");
		return(-2);
	}

	// polling mode wait 20us and stop
	if ((cmd == T_MEASUREMENT_POLL_CMD) || (cmd == RH_MEASUREMENT_POLL_CMD)) {
		delay_ms(1);
		if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL)) return(-6);

		i = 0;
	    do { 
            delay_ms(10); //delay 10ms
			if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &rx_request) ){
    		#ifdef I2C_COMMON_DEBUG
	          printf("_ReadReg: Error - unable to set number of bytes requested.\n");
     		#endif
	    	return(-3);
	        }

			// if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL)) return(-3);
     
			result = 0;
	        /* try Read data */
	        result = read (fd, buffer, 1);
	        
			if (1 != result) {
				//check ack
			    //if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATISTICS, (pointer)&stats)) return(-5);
				// if (stats.TX_NAKS)
              
				// stop 
                if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL)) return(-6);

			    // repeat start
                //if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_REPEATED_START, NULL)) return(-2);
	        }
			else {
				start_i ++;
				break;
			}

            if(i++ >= DELAY_TIMEOUT) {
				printf("polling read timeout\n");
				return -1;
            }

		} while(1);
	}
	
    if ((cmd == T_MEASUREMENT_HOLDM_CMD) || (cmd == RH_MEASUREMENT_HOLDM_CMD)) {
		// hold mode in mqx i2c driver do =>  while (0 == (i2csr & I2C_S_IICIF_MASK));
	    /* Send repeated start */
        if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_REPEATED_START, NULL)) {
		#ifdef I2C_COMMON_DEBUG
		 printf("_ReadReg: Error - unable to send repeated start.\n");
		#endif
		return(-2);
        }
        delay_ms(10); //delay 10ms
    }

#if (0)
	// 3. read data 3 bytes
	/* Set read request (1), then after read send ack */
	for (i = start_i; i < 3 ; i++) {
   	    if ((i < 2) && (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &rx_request))) {
    		#ifdef I2C_COMMON_DEBUG
	          printf("_ReadReg: Error - unable to set number of bytes requested.\n");
     		#endif
	    	return(-3);
	    }
	    result = 0;
	    /* Read all data */
	    result = read (fd, buffer + i, 1);
	    if (1 != result) {
		    #ifdef I2C_COMMON_DEBUG
		     printf("_ReadReg: Error - otg transceiver not responding %d.\n",result);
		    #endif
		    return(-4);
	    }
    }
#else
        rx_request = 3;
   	    if ((I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &rx_request))) {
    		#ifdef I2C_COMMON_DEBUG
	          printf("_ReadReg: Error - unable to set number of bytes requested.\n");
     		#endif
	    	return(-3);
	    }

	    result = 0;
	    /* Read all data */
	    result = read (fd, buffer , 3);
	    if (3 != result) {
		    #ifdef I2C_COMMON_DEBUG
		     printf("_ReadReg: Error - otg transceiver not responding.\n");
		    #endif
		    return(-4);
	    }

#endif

	// stop
	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL)) {
	 		return(-6);
 	}

	if(sth_CRC8(buffer,2,buffer[2]))
		return -1;

	(*data) = (uint16_t)(buffer[0] << 8 | buffer[1]);

	return 0;
}

int sht25_softrest()
{
    uint_32 result;
	uint8_t buffer[3];
	//1. set i2c address
	i2c_SetAddress(I2C_sht25_ADDRESS);
	
    //2. write cmd
	buffer[0] =	(uint_8)(SOFT_RESET_CMD & 0xFF);
	result = write(fd, buffer, 1);
	if (1 != result) {
		#ifdef I2C_COMMON_DEBUG
		 printf("_Write: Error - write to address %d\n",SOFT_RESET_CMD);
		#endif
		return(-1);
	}

	result = fflush (fd);
	if (MQX_OK != result) {
		printf("flush err\n");
		return(-2);
	}

	// stop
	if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL)) {
	 		return(-6);
 	}

    return 0;
}

float sht25_calcRH(uint16_t RH)
{
    float humidityRH; // variable for result
    RH &= ~0x0003;       // clear bits [1..0] (status bits)
    //-- calculate relative humidity [%RH] --
    humidityRH = -6.0 + 125.0/65536 * (float)RH; // RH= -6 + 125 * SRH/2^16
    return humidityRH;
}

float sht25_calcTC(uint16_t TC)
{
    float temperatureC; // variable for result
    TC &= ~0x0003;      // clear bits [1..0] (status bits)
    //-- calculate temperature [¡ãC] --
    temperatureC= -46.85 + 175.72/65536 *(float)TC; //T= -46.85 + 175.72 * ST/2^16
    return temperatureC;
}

int unit_test_sht25()
{
   int ret = 0;
   uint8_t  sht25_user_reg_val;
   uint16_t data;
   float sensors_val;
   int i = READ_SHT2x_TIMES;

   I2C_Init();
   I2C_ReadRegister8(I2C_sht25_ADDRESS,READ_USER_REG_CMD,&sht25_user_reg_val);
   printf ("The sth25's user reg value is 0x%x  \n",sht25_user_reg_val);
   sht25_user_reg_val = (sht25_user_reg_val & ~SHT2x_RES_MASK) | SHT2x_RES_12_14BIT | SHT2x_HEATER_ON;
   I2C_WriteRegister8(I2C_sht25_ADDRESS,WRITE_USER_REG_CMD,sht25_user_reg_val);
   I2C_ReadRegister8(I2C_sht25_ADDRESS,READ_USER_REG_CMD,&sht25_user_reg_val);
   printf ("The sth25's user reg value is 0x%x  after reconfig\n",sht25_user_reg_val);

   /*
   if(sht25_user_reg_val != sht25_user_REG_DEF_VAL) {
	   ret = -1;
	   goto sht25_testbreak;
   }*/

   // --- Set Resolution RH 10bit, Temp 13bit ---
   //sht25_user_reg_val = (sht25_user_reg_val & ~SHT2x_RES_MASK) | SHT2x_RES_12_14BIT;
   //I2C_WriteRegister8(I2C_sht25_ADDRESS,WRITE_USER_REG_CMD,sht25_user_reg_val);

   if(sht25_softrest()) {
	   ret = -2;
  	   goto sht25_testbreak;
   }

   delay_ms(50);

   do
   {
   // read RH
	if(sht25_measurement(MEASUREMENT_TYPE_RH,/*POLLING_MODE */ HOLD_MASTER_MODE ,&data)) {
	   ret = -3;
  	   goto sht25_testbreak;
	}
    sensors_val = sht25_calcRH(data);
    printf("measurement RH =%f\n",sensors_val);

	// read TC
	if(sht25_measurement(MEASUREMENT_TYPE_T,/*POLLING_MODE */HOLD_MASTER_MODE,&data)) {
	   ret = -4;
  	   goto sht25_testbreak;
	}
    sensors_val = sht25_calcTC(data);
    printf("measurement TC =%f\n",sensors_val);

	delay_ms(50);

	}
   while (--i);
	


sht25_testbreak:
	I2C_DeInit();
    if(ret)
		printf("sth25 test fail error %d\n",ret);

    return ret;
}


/* end file */

