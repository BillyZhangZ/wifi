/**HEADER********************************************************************
 * 
 * Copyright (c) 2015 Freescale Semiconductor;
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



/**************************************************************************
  Local header files for this application
 **************************************************************************/
#include "mag3110.h"

/**************************************************************************
  Macro
 **************************************************************************/

#define MAG3110_I2C_ADDRESS  0x0e    /* I2C dest address of MMA8653 */
#define MAG3110_STATUS 0x00
#define MAG3110_OUT_X 0x01 /* MSB first */
#define MAG3110_OUT_Y 0x03
#define MAG3110_OUT_Z 0x05
#define MAG3110_WHO_AM_I 0x07
#define MAG3110_OFF_X 0x09 /* MSB first */
#define MAG3110_OFF_Y 0x0b
#define MAG3110_OFF_Z 0x0d
#define MAG3110_DIE_TEMP 0x0f
#define MAG3110_CTRL_REG1 0x10
#define MAG3110_CTRL_REG2 0x11

#define MAG3110_STATUS_DRDY (BIT(2) | BIT(1) | BIT(0))

#define MAG3110_CTRL_DR_MASK (BIT(7) | BIT(6) | BIT(5))
#define MAG3110_CTRL_DR_SHIFT 5
#define MAG3110_CTRL_DR_DEFAULT 0

#define MAG3110_CTRL_TM BIT(1) /* trigger single measurement */
#define MAG3110_CTRL_AC BIT(0) /* continuous measurements */

#define MAG3110_CTRL_AUTO_MRST_EN BIT(7) /* magnetic auto-reset */
#define MAG3110_CTRL_RAW BIT(5) /* measurements not user-offset corrected */

#define MAG3110_DEVICE_ID 0xc4

#define MAG3110_DEFALUT_I2C_CLOCK 100000
#define MAG3110_MAX_I2C_CLOCK     400000

#define BIT(nr)			(1UL << (nr))

#define malloc _mem_alloc_system_zero
#define free   _mem_free
#define trace  printf


#define swab16(x) \
	((unsigned short)( \
		(((unsigned short)(x) & (unsigned short)0x00ffU) << 8) | \
		(((unsigned short)(x) & (unsigned short)0xff00U) >> 8) ))

/**************************************************************************
  type
 **************************************************************************/
struct mag3110dev{

    char          bus[20];
    unsigned int  speed;
    unsigned char crl_reg1;
    MQX_FILE_PTR  fd;
};

/**************************************************************************
  Global variables
 **************************************************************************/
static const int mag3110_samp_freq[8][2] = {
	{80, 0}, {40, 0}, {20, 0}, {10, 0}, {5, 0}, {2, 500000},
	{1, 250000}, {0, 625000}
};

/**************************************************************************
  local functioin 
 **************************************************************************/
static int sign_extend32(unsigned int value, int index) {
	unsigned char shift = 31 - index;
	return (int)(value << shift) >> shift;
}

static int mag3110_get_int_plus_micros_index(const int (*vals)[2], int n, int val, int val2) {
    while (n-- > 0)
        if (val == vals[n][0] && val2 == vals[n][1])
            return n;
    return -1;
}

static int mag3110_get_samp_freq_index(int val, int val2) {
	return mag3110_get_int_plus_micros_index(mag3110_samp_freq, 8, val, val2);
}


static int mag3110_i2c_write(MQX_FILE_PTR fd, unsigned char reg, unsigned char count, unsigned char *buf){

    unsigned char  *tempbuf;
    unsigned int    result;

    tempbuf = malloc(count + 1);
    if(tempbuf == NULL){
        trace("mag3110_i2c_write malloc tempbuf %d bytes failed! \n", count + 1);
        return -4;
    }

    tempbuf[0] = reg;
    memcpy(tempbuf + 1, buf, count);

    result = write(fd, tempbuf, count + 1); 
    if ((count + 1) != result) {
        free(tempbuf);
        return -1;
    } 

    result = fflush(fd);
    if (MQX_OK != result) {
        free(tempbuf);
        return -3;
    } 

    /* Stop I2C transfer */
    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_STOP, NULL)) {
        free(tempbuf);
        return -2;
    }

    free(tempbuf);

    return MQX_OK;
}


static int mag3110_i2c_read(MQX_FILE_PTR fd, unsigned char reg, unsigned char count, unsigned char *buf){

    unsigned char  buffer;
    unsigned int   result, param;

    buffer = reg;
    result = write(fd, &buffer, 1);
    if (1 != result) {
        return -1;
    }

    result = fflush(fd);
    if (MQX_OK != result){
        return -6;
    } 

    /* Send repeated start */
    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_REPEATED_START, NULL)) {
        return -2;
    }

    /* Set read request */
    param = count;
    if (I2C_OK != ioctl(fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param)){
        return -3;
    }

    result = 0;
    /* Read all data */ 
    result = read(fd, buf, count);
    if (count != result){
        return -4;
    }

    result = fflush (fd);
    if (MQX_OK != result){
        return -7;
    } 

    /* Stop I2C transfer */
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL)){
        return -5;
    }

    return MQX_OK;
};

static int mag3110_request(struct mag3110dev *dev){
    int ret, tries = 150;
    unsigned char value;

    value = dev->crl_reg1 | MAG3110_CTRL_TM;
    /* trigger measurement */
    ret = mag3110_i2c_write(dev->fd, MAG3110_CTRL_REG1, 1, (unsigned char *)&value);
    if (ret < 0)
        return ret;

    while (tries-- > 0) {
        ret = mag3110_i2c_read(dev->fd, MAG3110_STATUS, 1, (unsigned char *)&value);
        if (ret < 0)
            return ret;
        /* wait for data ready */
        if ((value & MAG3110_STATUS_DRDY) == MAG3110_STATUS_DRDY)
            break;
        _time_delay(20);
    }

    if (tries < 0) {
        trace("mag3110_request data not ready \n");
        return -1;
    }

    return 0;
}


/**************************************************************************
  Global functioin 
 **************************************************************************/

int mag3110_get_magnet(void *handle, int *x, int *y, int *z){

    int ret = -1;
    unsigned short valuearray[3];

    struct mag3110dev *dev = (struct mag3110dev *)handle;
    if(dev == NULL){
        trace("mag3110_get_magnet handle is NULL! \n");
        return -1;
    }

    ret = mag3110_request(dev);
    if(ret < 0)
        return ret;

    ret = mag3110_i2c_read(dev->fd, MAG3110_OUT_X, sizeof(valuearray), (unsigned char *)valuearray);
    if(ret < 0)
        return ret;

    *x = sign_extend32(swab16(valuearray[0]), 15);
    *y = sign_extend32(swab16(valuearray[1]), 15);
    *z = sign_extend32(swab16(valuearray[2]), 15);

    return 0;
}

int mag3110_get_temperature(void *handle, int *temp){

    int value;
    int ret;

    struct mag3110dev *dev = (struct mag3110dev *)handle;
    if(dev == NULL){
        trace("mag3110_get_temperature handle is NULL! \n");
        return -1;
    }

    ret = mag3110_i2c_read(dev->fd, MAG3110_DIE_TEMP, 1, (unsigned char *)&value);
    if(ret < 0)
        return ret;

    *temp = sign_extend32(value, 7);
    return 0;
}

int mag3110_update_sample_frequency(void *handle, int val, int val2){

    int rate;

    struct mag3110dev *dev = (struct mag3110dev *)handle;
    if(dev == NULL){
        trace("mag3110_update_sample_frequency handle is NULL! \n");
        return -1;
    }

    rate = mag3110_get_samp_freq_index(val, val2);
    if (rate < 0)
        return -1;

    dev->crl_reg1 &= ~MAG3110_CTRL_DR_MASK;
    dev->crl_reg1 |= rate << MAG3110_CTRL_DR_SHIFT;
    return mag3110_i2c_write(dev->fd, MAG3110_CTRL_REG1, 1, (unsigned char *)&dev->crl_reg1);
}

int mag3110_get_sample_frequency(void *handle, int *val, int *val2){

    int i;

    struct mag3110dev *dev = (struct mag3110dev *)handle;
    if(dev == NULL){
        trace("mag3110_get_sample_frequency handle is NULL! \n");
        return -1;
    }

    i = dev->crl_reg1 >> MAG3110_CTRL_DR_SHIFT;
    *val  = mag3110_samp_freq[i][0];
    *val2 = mag3110_samp_freq[i][1];

    return 0;
}

int mag3110_update_user_offset_correction(void *handle, MAG3110_CHANNEL channel, int value){

    int user_offset_correction;

    struct mag3110dev *dev = (struct mag3110dev *)handle;
    if(dev == NULL){
        trace("mag3110_update_user_offset_correction handle is NULL! \n");
        return -1;
    }

    if (value < -10000 || value > 10000)
        return -1;

    user_offset_correction = swab16(value << 1);

    return mag3110_i2c_write(dev->fd, MAG3110_OFF_X + 2 * channel, 2,  (unsigned char *)&user_offset_correction);
}

int mag3110_get_user_offset_correction(void *handle, MAG3110_CHANNEL channel, int *value){

    int ret;
    int user_offset_correction;

    struct mag3110dev *dev = (struct mag3110dev *)handle;
    if(dev == NULL){
        trace("mag3110_get_user_offset_correction handle is NULL! \n");
        return -1;
    }

    ret = mag3110_i2c_read(dev->fd, MAG3110_OFF_X + 2 * channel, 2, (unsigned char *)&user_offset_correction);
    if (ret < 0)
        return ret;

    *value = sign_extend32(user_offset_correction >> 1, 14);
    return 0;
}

int mag3110_get_real_rate(int *val1, int *val2){
    int n = 8;
    while(n--){
        if( (mag3110_samp_freq[n][0] * 1000000 + mag3110_samp_freq[n][1]) >=
            (*val1                   * 1000000 + *val2                  )){
            break;
        }
    }
    
    *val1 = mag3110_samp_freq[n][0];
    *val2 = mag3110_samp_freq[n][1];

    return 0;
}

void *mag3110_init(char *i2cbus, unsigned int speed){

    I2C_STATISTICS_STRUCT stats;
    unsigned int          param;
    int                   ret;
    unsigned char         regval;
    struct mag3110dev *dev = malloc(sizeof(struct mag3110dev));

    if (dev == NULL){
        trace("mag3110_init malloc failed \n");
    }

    do{
        dev->fd = fopen (i2cbus, NULL);

        if (dev->fd == NULL) {
            trace("mag3110_init unable to open I2C bus %s !\n", i2cbus);
            free(dev);
            break;
        }

        strncpy(dev->bus, i2cbus, sizeof(dev->bus));

        if(speed > MAG3110_MAX_I2C_CLOCK){
            param = MAG3110_DEFALUT_I2C_CLOCK;
        }
        else
            param = speed;

        if (I2C_OK != ioctl(dev->fd, IO_IOCTL_I2C_SET_BAUD, &param)) {
            fclose(dev->fd);
            free(dev);
            break;
        }
        dev->speed = param;

        if (I2C_OK != ioctl(dev->fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL)) {
            fclose(dev->fd);
            free(dev);
            break;
        }

        if (I2C_OK != ioctl(dev->fd, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL)) {
            fclose(dev->fd);
            free(dev);
            break;
        }

        param = MAG3110_I2C_ADDRESS;
        if (I2C_OK != ioctl(dev->fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param)) {
            fclose(dev->fd);
            free(dev);
            break;
        }

        /* Initiate start and send I2C bus address */
        param = fwrite (&param, 1, 0, dev->fd);
        if (I2C_OK != ioctl(dev->fd, IO_IOCTL_I2C_GET_STATISTICS, (void *)&stats)) {
            fclose(dev->fd);
            free(dev);
            break;
        }
        /* Stop I2C transfer */
        if (I2C_OK != ioctl(dev->fd, IO_IOCTL_I2C_STOP, NULL)) {
            fclose(dev->fd);
            free(dev);
            break;
        }

        /* Check ack (device exists) */
        if (I2C_OK == ioctl (dev->fd, IO_IOCTL_FLUSH_OUTPUT, &param)) {
            if ((param) || (stats.TX_NAKS)) {
                fclose(dev->fd);
                free(dev);
                break;
            }
        }
        else {
            fclose(dev->fd);
            free(dev);
            break;
        }

        /* identify */
        ret = mag3110_i2c_read(dev->fd, MAG3110_WHO_AM_I, 1, &regval);
        if(ret == MQX_OK){
            if (regval != MAG3110_DEVICE_ID) {
                trace("Device with ID register 0x%x is not mag3110 \n", regval);
                fclose(dev->fd);
                free(dev);
                break;
            }
        }

        /* init CRL_REG1 restore it to zero */
        /* DR[2:0] = 0, OS[1:0] = 0, 80HZ, 
         * FR:0, full 16bit values are read;
         * TM:0, Normal operation  based on AC condition;
         * AC:0, STANDBY mode;
         * */
        regval = 0; 
        if(mag3110_i2c_write(dev->fd, MAG3110_CTRL_REG1, 1, &regval) != MQX_OK){
            fclose(dev->fd);
            free(dev);
            break;
        }

        dev->crl_reg1 = regval;

        /* init CRL_REG2 */
        /* AUTO_MRST_EN: 1, Automatic magnetic sensor resets enabled;
         * RAW         : 0, Normal mode: data values are corrected by the user offset register values;
         * Mag_RST     : 0, 0: Reset cycle not active;
         * */
        regval = MAG3110_CTRL_AUTO_MRST_EN; 
        if((mag3110_i2c_write(dev->fd, MAG3110_CTRL_REG2, 1, (unsigned char *)&regval) != MQX_OK)){
            fclose(dev->fd);
            free(dev);
            break;
        }

    } while(0);

    return dev;
}

int mag3110_deinit(void *handle){

    struct mag3110dev *dev = (struct mag3110dev *)handle;
    if(dev == NULL){
        trace("mag3110_deinit handle is NULL! \n");
        return -1;
    }

    fclose(dev->fd);
    free(dev);
    return 0;
}
                        
/* end file */


