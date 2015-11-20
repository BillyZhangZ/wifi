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
#include <fio.h>
#include <i2c.h>

#include "74hc595.h"



#include "alc3261.h"


#if BSP_K22FSH                              
#define I2C_DEVICE_POLLED "i2c0:"
#else
#error No communication device defined! Please check user_config.h and enable proper I2C device. 
#endif

#define RT5640_I2C_ADDR                         0x1C

#define RT5640_DEVICE_ID                        0x6231
#define RT5640_VENDOR_ID2			0xff

/*FUNCTION****************************************************************
* 
* Function Name    : _rt5640_ReadReg
* Returned Value   : MQX error code
* Comments         :
*    Reads value of register. 
*
*END*********************************************************************/
static int rt5640_ReadReg(MQX_FILE_PTR fd, uint8_t reg, uint16_t *dest_ptr)
{
    uint8_t buffer[2];
    uint32_t result, param;
    buffer[0] = reg;
    result = write(fd, buffer, 1);
    if (1 != result)
    {
        return(-1);
    }
    result = fflush (fd);
    if (MQX_OK != result)
    {
        return(-6);
    } 
    /* Send repeated start */
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_REPEATED_START, NULL))
    {
        return(-2);
    }
    /* Set read request */
    param = 2;
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
    {
        return(-3);
    }
    result = 0;
    /* Read all data */ 
    result = read (fd, buffer, 2);
    if (2 != result)
    {
        return(-4);
    }
    result = fflush (fd);
    if (MQX_OK != result)
    {
        return(-7);
    } 
    *dest_ptr = (buffer[1] & 0xFFFF) | ((buffer[0] & 0xFFFF) << 8);
    /* Stop I2C transfer */
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
    {
        return(-5);
    }
    return (MQX_OK);
}


int unit_test_alc3261(void){
    MQX_FILE_PTR         fd = NULL;
    uint32_t param;
    uint16_t reg_val;
    int      ret;
    I2C_STATISTICS_STRUCT stats;

    if (fd == NULL) {
        fd = fopen (I2C_DEVICE_POLLED, NULL);
    }
    if (fd == NULL) {
        printf ("ERROR: Unable to open I2C driver!\n");
        ret = -10;
        goto clean1;
    }
    param = 100000;
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_BAUD, &param)) {
        ret = -1;
        goto clean2;
    }
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL)) {
        ret = -2;
        goto clean2;
    }
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL)) {
        ret = -3;
        goto clean2;
    }
    param = RT5640_I2C_ADDR;
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param)) {
        ret = -4;
        goto clean2;
    }

    /* Power on RT5640 */
    mux_74hc595_set_bit(BSP_74HC595_0, BSP_74HC595_AUD_PWREN);
    mux_74hc595_set_bit(BSP_74HC595_0, BSP_74HC595_AUD_LDO1_EN);

    _time_delay(300);

    /* Initiate start and send I2C bus address */
    param = fwrite (&param, 1, 0, fd);

    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATISTICS, (void *)&stats)) {
        ret = -5;
        goto clean3;
    }
    /* Stop I2C transfer */
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL)) {
        ret = -6;
        goto clean3;
    }
    /* Check ack (device exists) */
    if (I2C_OK == ioctl (fd, IO_IOCTL_FLUSH_OUTPUT, &param)) {
        if ((param) || (stats.TX_NAKS)) {
            printf("check ack -7 \n");
            ret = -7;
            goto clean3;
        }
    }
    else {
        ret = -8;
        goto clean3;
    }

    /* 5640 identify */
    ret = rt5640_ReadReg(fd, RT5640_VENDOR_ID2, &reg_val);
    if(ret == MQX_OK){
        if (reg_val != RT5640_DEVICE_ID) {
            printf("Device with ID register %x is not rt5640/39\n", reg_val);
            ret = -9;
            goto clean3;
        }
    }

clean3:
    /* Power off RT5640 */
    mux_74hc595_clear_bit(BSP_74HC595_0, BSP_74HC595_AUD_PWREN);
    mux_74hc595_clear_bit(BSP_74HC595_0, BSP_74HC595_AUD_LDO1_EN);
clean2:
    fclose(fd);
clean1:
    return(ret);
}

/* end file */

