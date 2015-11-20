/**HEADER*******************************************************************
* 
* Copyright (c) 2014 Freescale Semiconductor;
* All Rights Reserved
*
**************************************************************************** 
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
****************************************************************************
*
* Comments:
*
*   This file contains main initialization for your application
*   and infinite loop
*
*END************************************************************************/

#if MQX_KERNEL_LOGGING
#include <klog.h>
#endif
#if MQX_USE_LOGS
#include <log.h>
#endif



#include "main.h"
#ifdef  UNIT_TEST_WIFIMODULE
#include "rtcs.h"
#endif
#include "shell.h"

#ifdef  UNIT_TEST_BASEBOARD
#include "usb_test.h"
#include "alc3261.h"
#include "em9301.h"
#include "fxas21002.h"
#include "mag3110.h"
#include "microsd.h"
#include "mma8653.h"
#include "rda5876.h"
#include "sht25.h"
#include "lcd.h"
#include "pm2p5.h"
#include "ir.h"
#include "aaa.h"
#endif

#ifdef  UNIT_TEST_WIFIMODULE
#include "qca4002.h"
#include "pin.h"
#include "adc_pin.h"
#include "aaa.h"
#endif

#define trace printf
#define SPI_CHANNEL "spi1:"

//#define trace printf_lcd
const SHELL_COMMAND_STRUCT Shell_commands[] = { 
   { "sh",        Shell_sh }, 
   { "help",      Shell_help }, 
#ifdef  UNIT_TEST_WIFIMODULE
   { "netstat",   Shell_netstat },  
   { "ipconfig",  Shell_ipconfig },
   { "iwconfig",  Shell_iwconfig },
   { "ping",      Shell_ping },      
   { "pinverify", Shell_pins_verify },      
#endif
   { "?",         Shell_command_list },     
   { "exit",      Shell_exit },  
   { NULL,        NULL }, 
};



TASK_TEMPLATE_STRUCT MQX_template_list[] =
{
/*  Task number,             Entry point,              Stack,                    Pri,   String,            Auto? */
   {MAIN_TASK,                Main_task,                1500,                     13,   "main",           MQX_AUTO_START_TASK},
   {0,                            0,                     0,                        0,     0,                 0,               }
};

#if 1
const char *device_mode[] =
{
    "SPI_DEVICE_MASTER_MODE",
    "SPI_DEVICE_SLAVE_MODE",
};

const char *clock_mode[] =
{
    "SPI_CLK_POL_PHA_MODE0",
    "SPI_CLK_POL_PHA_MODE1",
    "SPI_CLK_POL_PHA_MODE2",
    "SPI_CLK_POL_PHA_MODE3"
};
#define TEST_BUFFER_SIZE 32
static void test_master_spi()
{

    MQX_FILE_PTR spifd;
	uint32_t para, result;
	SPI_STATISTICS_STRUCT  	stats;
	spifd = fopen(SPI_CHANNEL, NULL);
    
    printf("spi master test!\n");

	if(NULL == spifd) {
        printf("Error opening SPI driver!\n");
        _time_delay(200L);
        _task_block();
    }
        /* Display baud rate */
    printf("Current baud rate ... ");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_BAUD, &para)) {
        printf("%d Hz\n", para);
    }
    else {
        printf("ERROR\n");
    }

    /* Set a different rate */
    para = 500000;
    printf("Changing the baud rate to %d Hz ... ", para);
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_BAUD, &para)) {
        printf("OK\n");
    }
    else {
        printf("ERROR\n");
    }

    /* Display baud rate */
    printf("Current baud rate ... ");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_BAUD, &para)) {
        printf("%d Hz\n", para);
    }
    else {
        printf("ERROR\n");
    }
    /* Set clock mode */
    para = SPI_CLK_POL_PHA_MODE0;
    printf("Setting clock mode to %s ... ", clock_mode[para]);
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_MODE, &para)) {
        printf("OK\n");
    }
    else {
        printf("ERROR\n");
    }
    /* Get clock mode */
    printf("Getting clock mode ... ");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_MODE, &para)) {
        printf("%s\n", clock_mode[para]);
    }
    else {
        printf("ERROR\n");
    }

    /* Set big endian */
    para = SPI_DEVICE_BIG_ENDIAN;
    printf("Setting endian to %s ... ", para == SPI_DEVICE_BIG_ENDIAN ? "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_ENDIAN, &para)) {
        printf("OK\n");
    }
    else {
        printf("ERROR\n");
    }
    /* Get endian */
    printf("Getting endian ... ");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_ENDIAN, &para)) {
        printf("%s\n", para == SPI_DEVICE_BIG_ENDIAN ? "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
    }
    else {
        printf("ERROR\n");
    }

    /* Set transfer mode */
    para = SPI_DEVICE_MASTER_MODE;
    printf("Setting transfer mode to %s ... ", device_mode[para]);
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_TRANSFER_MODE, &para)) {
        printf("OK\n");
    }
    else {
        printf("ERROR\n");
    }

    /* Get transfer mode */
    printf("Getting transfer mode ... ");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_TRANSFER_MODE, &para)) {
        printf("%s\n", device_mode[para]);
    }
    else {
        printf("ERROR\n");
    }

    /* Clear statistics */
    printf("Clearing statistics ... ");
    result = ioctl(spifd, IO_IOCTL_SPI_CLEAR_STATS, NULL);
    if(SPI_OK == result) {
        printf("OK\n");
    }
    else if(MQX_IO_OPERATION_NOT_AVAILABLE == result) {
        printf("not available, define BSPCFG_ENABLE_SPI_STATS\n");
    }
    else {
        printf("ERROR\n");
    }

    /* Get statistics */
    printf("Getting statistics: ");
    result = ioctl(spifd, IO_IOCTL_SPI_GET_STATS, &stats);
    if(SPI_OK == result) {
        printf("\n");
        printf("Rx packets:   %d\n", stats.RX_PACKETS);
        printf("Tx packets:   %d\n", stats.TX_PACKETS);
    }
    else if(MQX_IO_OPERATION_NOT_AVAILABLE == result) {
        printf("not available, define BSPCFG_ENABLE_SPI_STATS\n");
    }
    else {
        printf("ERROR\n");
    }
    printf("\n");


    while(1){
    /*master send 32 bytes to slave*/
    uint8_t sendBuf[TEST_BUFFER_SIZE], recvBuf[TEST_BUFFER_SIZE];
    uint32_t i;
    printf("\r\n Master transmit:\n");
    for(i = 0; i < TEST_BUFFER_SIZE; i++)
    {
        sendBuf[i] = i + 1;
        if ((i & 0x0F) == 0)
        {
            printf("\r\n    ");
        }
        printf(" %02X", sendBuf[i]);
    }
      
    if(TEST_BUFFER_SIZE == fwrite(sendBuf, 1, TEST_BUFFER_SIZE, spifd))
       printf("\r\nTransmit ok\n");
    else
       printf("Error\n");
    fflush(spifd);  
     /*wait till slave is ok*/
    _time_delay(1000);
      /*master recsive 32 bytes from slave*/
    if(TEST_BUFFER_SIZE == fread(recvBuf, 1, TEST_BUFFER_SIZE, spifd))
       printf("Receive ok\n");
    else
       printf("Error\n");
    printf("\r\n Master receive:");
    for (i = 0; i < TEST_BUFFER_SIZE; i++)
    {
        if(recvBuf[i] != sendBuf[i])
        {
          printf("Receive error!\n\n");
          break;
        }
    // Print 16 numbers in a line.
      if ((i & 0x0F) == 0)
      {
        printf("\r\n    ");
      }
      printf(" %02X", recvBuf[i]);
    }
      fflush(spifd);
    }
    
}
static void  test_slave_spi()
{
	MQX_FILE_PTR spifd;
	uint32_t para, result;
	SPI_STATISTICS_STRUCT  	stats;
	spifd = fopen(SPI_CHANNEL, NULL);

    printf("SPI slave read test!\n");
	if(NULL == spifd) {
        printf("Error opening SPI driver!\n");
        _time_delay(200L);
        _task_block();
    }

	/* Set transfer mode */
    para = SPI_DEVICE_SLAVE_MODE;
    printf("Setting transfer mode to %s ... ", device_mode[para]);
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_TRANSFER_MODE, &para)) {
        printf("OK\n");
    }
    else {
        printf("ERROR\n");
    }
    /* Get transfer mode */
    printf("Getting transfer mode ... ");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_TRANSFER_MODE, &para)) {
        printf("%s\n", device_mode[para]);
    }
    else {
        printf("ERROR\n");
    }
	
    /* Set clock mode */
    para = SPI_CLK_POL_PHA_MODE0;
    printf("Setting clock mode to %s ... ", clock_mode[para]);
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_MODE, &para)) {
        printf("OK\n");
    }
    else {
        printf("ERROR\n");
    }
    /* Get clock mode */
    printf("Getting clock mode ... ");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_MODE, &para)) {
        printf("%s\n", clock_mode[para]);
    }
    else {
        printf("ERROR\n");
    }

    /* Set big endian */
    para = SPI_DEVICE_BIG_ENDIAN;
    printf("Setting endian to %s ... ", para == SPI_DEVICE_BIG_ENDIAN ? "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_ENDIAN, &para)) {
        printf("OK\n");
    }
    else {
        printf("ERROR\n");
    }

    /* Get endian */
    printf("Getting endian ... ");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_ENDIAN, &para)) {
        printf("%s\n", para == SPI_DEVICE_BIG_ENDIAN ? "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
    }
    else {
        printf("ERROR\n");
    }

    /* Clear statistics */
    printf("Clearing statistics ... ");
    result = ioctl(spifd, IO_IOCTL_SPI_CLEAR_STATS, NULL);
    if(SPI_OK == result) {
        printf("OK\n");
    }
    else if(MQX_IO_OPERATION_NOT_AVAILABLE == result) {
        printf("not available, define BSPCFG_ENABLE_SPI_STATS\n");
    }
    else {
        printf("ERROR\n");
    }

    /* Get statistics */
    printf("Getting statistics: ");
    result = ioctl(spifd, IO_IOCTL_SPI_GET_STATS, &stats);
    if(SPI_OK == result) {
        printf("\n");
        printf("Rx packets:   %d\n", stats.RX_PACKETS);
        printf("Tx packets:   %d\n", stats.TX_PACKETS);
    }
    else if(MQX_IO_OPERATION_NOT_AVAILABLE == result) {
        printf("not available, define BSPCFG_ENABLE_SPI_STATS\n");
    }
    else {
        printf("ERROR\n");
    }
    printf("\n");

    uint8_t sendBuf[32], recvBuf[32];
    uint8_t i; 
    while(1)
    {
      printf("Slave receive from master...\n");   
      if(32 != fread(recvBuf, 1, 32, spifd))
      {
        printf("Error!\n");
      }
      fflush(spifd);
      for(i = 0; i < 32; i++)
      {
        printf(" %02x ", recvBuf[i]); 
      }
      printf("\n");
      printf("Statics:");
      result = ioctl(spifd, IO_IOCTL_SPI_GET_STATS, &stats);
      if(SPI_OK == result) {
          printf("\n");
          printf("Rx packets:   %d\n", stats.RX_PACKETS);
          printf("Tx packets:   %d\n", stats.TX_PACKETS);
      }
      printf("Slave write back to master\n");
      if(32 == fwrite(recvBuf, 1, 32, spifd))
        printf("\n OK");
      else
        printf("Error\n");
      fflush(spifd);
    }
}


#endif
/*TASK*-----------------------------------------------------------------
*
* Function Name  : Main_task
* Comments       :
*    This task initializes MFS and starts SHELL.
*
*END------------------------------------------------------------------*/

void Main_task(uint_32 initial_data)
{
#if MQX_USE_LOGS || MQX_KERNEL_LOGGING
   _mqx_uint                 log_result;
#endif 

   int              error;
      
    //printf("start unit test\n");
    test_master_spi();
    //test_slave_spi();
    
    while(1){};

//#ifdef  UNIT_TEST_BASEBOARD
//    // lcd test
//    //lcd_init();
//    printf_lcd("1234567890");
//    printf_lcd("abcdef\n");
//    printf_lcd("1234567890");
//    printf_lcd("ABCDEF\n");
//    printf_lcd("~!@#$%%^&*()-+=-|\n");
//    printf_lcd("GHIJKLMNOPQRSTUV");
//    //lcd_deinit();
//    //lcd_init();
//    printf_lcd("abcdefghijklmnop\n");
//    printf_lcd("qrstuvwxyz\n");
//    lcd_deinit();
//#endif
//
//   /*******************************
//   * 
//   * START YOUR CODING HERE
//   *
//   ********************************/   
//   do {
//
//#ifdef  UNIT_TEST_BASEBOARD
//     // hc595 test
//     /*
//     do {
//       mux_74hc595_set_bit(BSP_74HC595_0,   BSP_74HC595_AMB_LED);
//       _time_delay(1000);
//       mux_74hc595_clear_bit(BSP_74HC595_0,   BSP_74HC595_AMB_LED);
//       _time_delay(1000);
//     }while(1);
//     */
//       // added ir test
//       //unit_test_ir();
//     
//#if 0
//       trace("usb:S\n");
//       error = unit_test_usb_host();
//       if(error != 0){
//           trace("usb:E\n");
//           break;
//       }
//       trace("usb:OK\n");
//#endif
//
//       trace("alc3261:S\n");
//       error = unit_test_alc3261();
//       if(error != 0){
//           trace("alc3261:E\n");
//           break;
//       }
//       trace("alc3261:OK\n");
//
//#if 0       
//       // pm2.5 test
//       trace("pm2.5:S\n");
//       error = unit_test_pm2p5();
//       if(error != 0){
//           trace("pm2.5:E\n");
//           break;
//       }
//       trace("pm2.5:OK\n");
//#endif       
//       
//       trace("rda5876:S\n");
//       error = unit_test_rda5876();
//       if(error != 0){
//           trace("rda5876:E\n");
//           break;
//       }
//       trace("rda5876:OK\n");
//       trace("em9301:S\n");
//       error = unit_test_em9301();
//       if(error != 0){
//           trace("em9301:E\n");
//           break;
//       }
//       trace("em9301:OK\n");
//       
//       trace("mma8653:S\n");
//       error = unit_test_mma8653();
//       if(error != 0){
//           trace("mma8653:E\n");
//           break;
//       }
//       trace("mma8653:OK\n");
//
//       trace("mag3110:S\n");
//       error = unit_test_mag3110();
//       if(error != 0){
//           trace("mag3110:E\n");
//           //-break;
//       }
//       trace("mag3110:OK\n");
//
//       trace("sht25:S\n");
//       error = unit_test_sht25();
//       if(error != 0){
//           trace("sht25:E\n");
//           break;
//       }
//       trace("sht25:OK\n");
//
//       trace("fxas21002:S\n");
//       error = unit_test_fxas21002();
//       if(error != 0){
//           trace("fxas21002:E\n");
//           //-break;
//       }
//       trace("fxas21002:OK\n");
//
//       trace("microsd:S\n");
//       error = unit_test_microsd();
//       if(error != 0){
//           trace("microsd:E\n");
//           break;
//       }
//       trace("microsd:OK\n");
//#if 0
//       trace("AAA:S \n");
//       error = unit_test_aaa();
//       if(error != 0){
//           trace("AAA:E \n");
//           break;
//       }
//       trace("AAA:OK \n");
//#endif
//       trace("PASS!\n");
//#endif
//
//#ifdef  UNIT_TEST_WIFIMODULE
//       trace("QCA4002:S\n");
//       error = unit_test_qca4002();
//       if(error != 0){
//           trace("QCA4002:E\n");
//           _task_block();
//           break;
//       }
//       trace("QCA4002:OK\n");
//
//#if 1
//       trace("AAA:S \n");
//       error = unit_test_aaa();
//       if(error != 0){
//           trace("AAA:E \n");
//           _task_block();
//           break;
//       }
//       trace("AAA:OK \n");
//#endif
//
//       trace("usb:S\n");
//       error = unit_test_usb_host();
//       if(error != 0){
//           trace("usb:E\n");
//           _task_block();
//           break;
//       }
//       trace("usb:OK\n");
//
//       // dac0 loop to adc0_dp0 and adc0_dp3 test
//       trace("dac adc loopback test:S\n");
//       error = unit_test_adc_pin();
//       if(error != 0){
//           trace("dac adc loopback test:E\n");
//           _task_block();
//           break;
//       }
//       trace("dac adc loopback test:OK\n");
//
//       trace("PINS INIT:S \n");
//       error = unit_test_pin();
//       if(error != 0){
//           trace("PINS INIT:E \n");
//           _task_block();
//           break;
//       }
//       trace("PINS INIT:OK \n");
//#endif
//
//   }while(0);
//          
//   for (;;)   
//   { 
//      /* Run the shell */ 
//      Shell(Shell_commands, NULL); 
//      printf("Shell exited, restarting...\n"); 
//   } 
//
//   

}
/* EOF */
