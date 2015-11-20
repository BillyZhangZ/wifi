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
 *   This file contains mass storage class application types and definitions.
 *
 *END************************************************************************/
#ifndef __SHT25_H_
#define __SHT25_H_

#define I2C_sht25_ADDRESS     0x40    /* 7bit address for MQX i2c driver */

/* 8bit */
#define I2C_sht25_ADR_W       128     // sensor I2C address | write bit
#define I2C_sht25_ADR_R       129     // sensor I2C address | read bit

#define sht25_user_REG_DEF_VAL   0x20    // user register default value

#define T_MEASUREMENT_HOLDM_CMD    0xE3
#define RH_MEASUREMENT_HOLDM_CMD   0xE5
#define T_MEASUREMENT_POLL_CMD		   0xF3
#define RH_MEASUREMENT_POLL_CMD		   0xF5
#define WRITE_USER_REG_CMD         0xE6
#define READ_USER_REG_CMD          0xE7
#define SOFT_RESET_CMD             0xFE

#define MEASUREMENT_TYPE_T         0x0
#define MEASUREMENT_TYPE_RH        0x1

#define POLLING_MODE                    0x0
#define HOLD_MASTER_MODE                0x1

#define POLLING_WAIT_US           30  /* more than 20us */
#define DELAY_TIMEOUT             20

#define SHT2x_RES_12_14BIT 0x00 // RH=12bit, T=14bit
#define SHT2x_RES_8_12BIT  0x01 // RH= 8bit, T=12bit
#define SHT2x_RES_10_13BIT 0x80 // RH=10bit, T=13bit
#define SHT2x_RES_11_11BIT 0x81 // RH=11bit, T=11bit
#define SHT2x_RES_MASK     0x81 // Mask for res. bits (7,0) in user reg.

#define SHT2x_EOB_ON      0x40 // end of battery
#define SHT2x_EOB_MASK    0x40 // Mask for EOB bit(6) in user reg.

#define SHT2x_HEATER_ON   0x04 // heater on
#define SHT2x_HEATER_OFF  0x00 // heater off
#define SHT2x_HEATER_MASK 0x04 // Mask for Heater bit(2) in user reg.


//#error list
#define CHECKSUM_ERROR   -1



//void i2c_read_sht25_reg(MQX_FILE_PTR fd);


#endif   /* __audio_host_h__ */

/* EOF */
