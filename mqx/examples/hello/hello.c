/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 1989-2008 ARC International
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the source for the hello example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h> 
#include <fio.h>


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


/* Task IDs */
#define HELLO_TASK 5
//#define SPI_CHANNEL  "spi0:"
#define SPI_CHANNEL  "spi1:"


extern void hello_task(uint32_t);


const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{ 
    /* Task Index,   Function,   Stack,  Priority, Name,     Attributes,          Param, Time Slice */
    { HELLO_TASK,   hello_task, 1500,   8,        "hello",  MQX_AUTO_START_TASK, 0,     0 },
    { 0 }
};



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
////#if 0
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
////#else
    printf("Slave write back to master\n");
    for(i = 0; i < 32; i++)
      recvBuf[i] = i+1;
    if(32 == fwrite(recvBuf, 1, 32, spifd))
      printf("\n OK");
    else
      printf("Error\n");
    fflush(spifd);
    /****/
    printf("Getting statistics: ");
    result = ioctl(spifd, IO_IOCTL_SPI_GET_STATS, &stats);
    if(SPI_OK == result) {
        printf("\n");
        printf("Rx packets:   %d\n", stats.RX_PACKETS);
        printf("Tx packets:   %d\n", stats.TX_PACKETS);
    }
//#endif
}
/*TASK*-----------------------------------------------------
* 
* Task Name    : hello_task
* Comments     :
*    This task prints " Hello World "
*
*END*-----------------------------------------------------*/
void hello_task
    (
        uint32_t initial_data
    )
{
    (void)initial_data; /* disable 'unused variable' warning */

    test_master_spi();
    //test_slave_spi();
    
    
    
    _task_block();
}

/* EOF */
