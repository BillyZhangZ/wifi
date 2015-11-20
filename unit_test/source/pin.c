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

/**************************************************************************
 **************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <shell.h>

#include <stdlib.h>

/**************************************************************************
  Local header files for this application
 **************************************************************************/

#define MAX_WIFI_MODULE_PINS   47

typedef struct _wifi_module_pins{
    int          needverify;
    unsigned     port;
    char        *desc;
} WIFI_MODULE_PINS, *WIFI_MODULE_PINS_PTR;

/**************************************************************************
  Global variables
 **************************************************************************/

static WIFI_MODULE_PINS wifi_module_pins[MAX_WIFI_MODULE_PINS + 1] = {
    {0,   -1,                        NULL},
    {0,   -1,                       "Ground"}, /* 1 */
    {1, (GPIO_PORT_D | GPIO_PIN0),  "SPI_CS_WIFIHM1"}, /* 2 */
    {1, (GPIO_PORT_C | GPIO_PIN10), "CHIP_PWD_L"}, /* 3 */
    {0,   -1,                       "USB_DN_KF"}, /* 4 */
    {0,   -1,                       "USB_DP_KF"}, /* 5 */
    {1, (GPIO_PORT_D | GPIO_PIN4),  "SPI1_CS0_EXT"}, /* 6 */
    {1, (GPIO_PORT_D | GPIO_PIN7),  "SPI1_SIN_EXT"}, /* 7 */
    {1, (GPIO_PORT_D | GPIO_PIN6),  "SPI1_SOUT_EXT"}, /* 8 */
    {1, (GPIO_PORT_D | GPIO_PIN5),  "SPI1_CLK_EXT"}, /* 9 */
    {0, (GPIO_PORT_A | GPIO_PIN0),  "JTAG_TCLK_MCU"}, /* 10 */
    {1, (GPIO_PORT_A | GPIO_PIN1),  "JTAG_TDI_MCU"}, /* 11 */
    {1, (GPIO_PORT_E | GPIO_PIN0),  "I2C_DATA_Host"}, /* 12 */
    {1, (GPIO_PORT_E | GPIO_PIN1),  "I2C_CLK_Host"}, /* 13 */
    {0,   -1,                       "USB_DM_MCU"}, /* 14 */
    {0,   -1,                       "USB_DP_MCU"}, /* 15 */
    {0,   -1,                       "ADC0_DP0"}, /* 16 */
    {0,   -1,                       "ADC0_DP3"}, /* 17 */
    {0,   -1,                       "DAC0_OUT"}, /* 18 */
    {0,   -1,                       "Ground"}, /* 19 */
    {0, (GPIO_PORT_B | GPIO_PIN16), "UART0_RXD_MCU"}, /* 20 */
    {1, (GPIO_PORT_A | GPIO_PIN2),  "JTAG_TDO_MCU"}, /* 21 */
    {1, (GPIO_PORT_A | GPIO_PIN5),  "JTAG_TRSTb_MCU"}, /* 22 */
    {1, (GPIO_PORT_A | GPIO_PIN4),  "EZP_CS_b"}, /* 23 */
    {1, (GPIO_PORT_A | GPIO_PIN12), "I2S0_TXD0"}, /* 24 */
    {1, (GPIO_PORT_B | GPIO_PIN1),  "I2C1_DATA_MCU"}, /* 25 */
    {1, (GPIO_PORT_B | GPIO_PIN0),  "I2C1_CLK_MCU"}, /* 26 */
    {0,   -1,                       "VDD_MCU"}, /* 27 */
    {0,   -1,                       "RESET_b"}, /* 28 */
    {0,   -1,                       "Ground"}, /* 29 */
    {1, (GPIO_PORT_B | GPIO_PIN2),  "UART0_RTS_MCU"}, /* 30 */
    {1, (GPIO_PORT_B | GPIO_PIN3),  "UART0_CTS_MCU"}, /* 31 */
    {0, (GPIO_PORT_A | GPIO_PIN3),  "JTAG_TMS_MCU"}, /* 32 */
    {1, (GPIO_PORT_B | GPIO_PIN18), "I2S0_TX_BCLK"}, /* 33 */
    {0, (GPIO_PORT_B | GPIO_PIN17), "UART0_TXD_MCU"}, /* 34 */
    {0,   -1,                       "VDD33"}, /* 35 */
    {0,   -1,                       "SPIM_CS#"}, /* 36 */
    {1, (GPIO_PORT_C | GPIO_PIN0),  "MCU_GPIO5"}, /* 37 */
    {1, (GPIO_PORT_C | GPIO_PIN3),  "UART1_RXD_MCU"}, /* 38 */
    {1, (GPIO_PORT_B | GPIO_PIN19), "I2S0_TX_FS"}, /* 39 */
    {1, (GPIO_PORT_C | GPIO_PIN4),  "UART1_TXD_MCU"}, /* 40 */
    {1, (GPIO_PORT_C | GPIO_PIN2),  "UART1_CTS_MCU"}, /* 41 */
    {1, (GPIO_PORT_C | GPIO_PIN1),  "UART1_RTS_MCU"}, /* 42 */
    {1, (GPIO_PORT_C | GPIO_PIN5),  "I2S0_RXD0"}, /* 43 */
    {1, (GPIO_PORT_C | GPIO_PIN6),  "I2S0_RX_BCLK"}, /* 44 */
    {1, (GPIO_PORT_C | GPIO_PIN8),  "I2S0_MCLK"}, /* 45 */
    {1, (GPIO_PORT_C | GPIO_PIN7),  "I2S0_RX_FS"}, /* 46 */
    {0,   -1,                       "Ground"}, /* 47 */
};


/**************************************************************************
  Local functions
 **************************************************************************/
static int unit_test_pin_set(int pinnumber, int pinvalue){

    MQX_FILE_PTR pin_port = NULL;

    if(wifi_module_pins[pinnumber].needverify){

        const unsigned int output_set[] = {
            wifi_module_pins[pinnumber].port | GPIO_PIN_STATUS_0,
            GPIO_LIST_END
        };

        pin_port = fopen("gpio:write", (char *) &output_set);

        if (pin_port) {
            ioctl(pin_port, pinvalue == 0 ? GPIO_IOCTL_WRITE_LOG0 : GPIO_IOCTL_WRITE_LOG1, NULL);    
            fclose(pin_port);
            printf("Set \"%s\" %s\n", wifi_module_pins[pinnumber].desc, pinvalue == 1 ? "high" : "low");
            return 0;
        }else{
            printf("Open gpio pin %d failed \n", pinnumber);
            return -1;
        }

    }else{
        printf("wifi module pin %d need not be verified \n", pinnumber);
        return -1;
    }

}


/**************************************************************************
  Global functions
 **************************************************************************/

int unit_test_pin(void){

    /* Change PIN23 EZP_CS_b to PTA4 */
    PORT_PCR_REG(PORTA_BASE_PTR, 4) = PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK;
    GPIO_PDDR_REG(PTA_BASE_PTR) |= 0x00000010; // PA4 as output
    GPIO_PDOR_REG(PTA_BASE_PTR) |= 0x00000010; // PA4 in high level

    return 0;
}

int Shell_pins_verify(int argc, char* argv[] ) {

    bool     print_usage, shorthelp = FALSE;
    int      pinnumber = -1;
    int      pinvalue  = -1;

    print_usage = Shell_check_help_request(argc, argv, &shorthelp );

    if (!print_usage)  {
        if(argc < 3) {
            print_usage = TRUE;
        }
        else {
            pinnumber = atoi(argv[1]);
            pinvalue  = atoi(argv[2]);

            if((pinvalue != 0) && (pinvalue != 1)){
                printf("pin value must set to 0 or 1 \n");
                return -1;
            }
            if(pinnumber > MAX_WIFI_MODULE_PINS){
                printf("pinnumber %d more than MAX_WIFI_MODULE_PINS %d \n", pinnumber, MAX_WIFI_MODULE_PINS);
                return -1;
            }
            return unit_test_pin_set(pinnumber, pinvalue);
        }
    }

    if (print_usage)  {
        if(shorthelp) {
            printf("%s <pinnumber> <0/1> \n", argv[0]);
        }else {
            printf("%s <pinnumber> <0/1> \n", argv[0]);
        }
    }
    return 0;
}

/* end file */

