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


#include "aaa.h"


#if BSP_K22FSH                              
#define I2C_DEVICE_POLLED "i2c0:"
#else
#error No communication device defined! Please check user_config.h and enable proper I2C device. 
#endif

#define AAA_I2C_ADDR                         0x11

int unit_test_aaa(void){
    MQX_FILE_PTR         fd = NULL;
    uint32_t param;
    uint16_t reg_val;
    int      ret;
    I2C_STATISTICS_STRUCT stats;
    int      retry = 200;

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
    param = AAA_I2C_ADDR;
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param)) {
        ret = -4;
        goto clean2;
    }

    ret = -11;

    do {
        /* Initiate start and send I2C bus address */
        param = fwrite (&param, 1, 0, fd);

        if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATISTICS, (void *)&stats)) {
            ret = -5;
            break;
        }
        /* Stop I2C transfer */
        if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL)) {
            ret = -6;
            break;
        }
        /* Check ack (device exists) */
        if (I2C_OK == ioctl (fd, IO_IOCTL_FLUSH_OUTPUT, &param)) {
            if ((param) || (stats.TX_NAKS)) {
                _time_delay(10);
                 ioctl (fd, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL);
            }
            else{
                ret = MQX_OK;
                break;
            }
        }
        else {
            ret = -8;
            break;
        }
    }while(--retry);

    if(ret < 0)
        printf("Can't find AAA device \n");

clean3:
clean2:
    fclose(fd);
clean1:
    return(ret);
}

/* end file */

