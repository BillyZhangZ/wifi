//------------------------------------------------------------------------------
// Copyright (c) 2011 Qualcomm Atheros, Inc.
// All Rights Reserved.
// Qualcomm Atheros Confidential and Proprietary.
// Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is
// hereby granted, provided that the above copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
// USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//------------------------------------------------------------------------------
//==============================================================================
// Author(s): ="Atheros"
//==============================================================================
/*
* $FileName: Shell_Task.c$
* $Version : 3.0.4.0$
* $Date    : Nov-21-2008$
*
* Comments:
*
*   
*
*END************************************************************************/



#include "main.h"
#include "mqx.h"

/*TASK*-----------------------------------------------------------------
*
* Function Name  : Shell_Task
* Returned Value : void
* Comments       :
*
*END------------------------------------------------------------------*/

#if DEMOCFG_ENABLE_SERIAL_SHELL

MQX_FILE_PTR rs232_dev = NULL;
    
#define SERIAL_CHANNEL "mttyb:"

void Uart_Init(int baudrate)
{   
    rs232_dev  = fopen( SERIAL_CHANNEL, (char const *)IO_SERIAL_NON_BLOCKING ); 
     if(!rs232_dev){
        printf("Error open uart device\n");
        _task_block();
    }
    
    if(ioctl(rs232_dev , IO_IOCTL_SERIAL_SET_BAUD, (pointer)&baudrate) != MQX_OK){
        printf("Set RDA5876_BT_DEV baudrate failed \n");
        return;
    }
    return;
}

uint8_t Uart_Read_Ch(void)
{
    int8_t ch = 0;
    read( rs232_dev, &ch,1);
    return ch;
}
void Shell_Task(uint_32 temp)
{ 
#if 0
   /* Run the shell on the serial port */
   for(;;)  {
      Shell(Shell_commands, NULL);
      printf("Shell exited, restarting...\n");
   }
#else
   //lunch configuration
   //wmiconfig --p 37dd5a0741
   //wmiconfig --wpa 2 CCMP CCMP 
   //wmiconfig --connect hdigroup
   //wmiconfig --ipstatic 10.96.15.249 255.255.255.0 10.96.15.254
   //wmiconfig --ip_http_client connect 111.13.101.208 /
   //wmiconfig --ip_http_client get /index.html
    char *argv[10];
    int ret_val = 0;
    
    //wait for RTCS ready
    _time_delay(2000);
    printf("start to connect hdigroup\n");
    //password
    argv[0]="wmiconfig";    argv[1]="--p";    argv[2]="37dd5a0741"; 
    ret_val = wmiconfig_handler(3, argv);
    if(ret_val != 0) printf("conifig password error\n");
    
    //wpa
    argv[0]="wmiconfig";    argv[1]="--wpa";    argv[2]="2"; argv[3]="CCMP"; argv[4]="CCMP"; 
    ret_val = wmiconfig_handler(5, argv);
    if(ret_val != 0) printf("conifig wpa error\n");
    
    //connect ap
    argv[0]="wmiconfig";    argv[1]="--connect";    argv[2]="hdigroup"; 
    ret_val = wmiconfig_handler(3, argv);
    if(ret_val != 0) printf("connect hdigroup ap  error\n");
    
    //config IP address
    argv[0]="wmiconfig";    argv[1]="--ipstatic";    argv[2]="10.96.15.249";    argv[3]="255.255.255.0";    argv[4]="10.96.15.254";
    ret_val = wmiconfig_handler(5,argv);
    if(ret_val != 0) printf("conifig static address error\n");
    
    _time_delay(3000);
#if 0
     //access baidu.com
    printf("trying to connect to baidu\n");
    argv[0]="wmiconfig";    argv[1]="--ip_http_client";  argv[2]="connect";   argv[3]="111.13.101.208"; 
    ret_val = wmiconfig_handler(4, argv);
    if(ret_val != 0) printf("access baidu.com error\n");
    
    _time_delay(5000);
    //fetch webpage
    printf("trying to get home page\n");
    argv[0]="wmiconfig";    argv[1]="--ip_http_client";  argv[2]="get";   argv[3]="/index.html"; 
    ret_val = wmiconfig_handler(4, argv);
    if(ret_val != 0) printf("access web page error\n");
#endif
    printf("connected to hdigroup AP\n");

    Uart_Init(115200);
    printf("start\n");
    char rec_buf[512+512+512] = {0};
    int rec_cnt = 0;
#define AT_MIN_LEN 5// AT+ \r \n
    while(1)
    {
        
        rec_buf[rec_cnt++] = Uart_Read_Ch();
        printf("%c",rec_buf[rec_cnt-1]);
        if(rec_cnt > AT_MIN_LEN)
        {
          if(rec_buf[0] == 'A' && rec_buf[0] == 'T' && rec_buf[0] == '+' && rec_buf[rec_cnt - 1] == '\r' && rec_buf[rec_cnt - 2] == '\n')
          {
              memcpy(argv[3], rec_buf + 3, rec_cnt - AT_MIN_LEN);
              
              printf("%s\n",argv[3]);
              argv[0]="wmiconfig";    argv[1]="--ip_http_client";  argv[2]="connect";   //argv[3]="111.13.101.208"; 
              ret_val = wmiconfig_handler(4, argv);
              if(ret_val != 0) printf("access baidu.com error\n");
              
              _time_delay(3000);
              //fetch webpage
              printf("trying to get home page\n");
              argv[0]="wmiconfig";    argv[1]="--ip_http_client";  argv[2]="get";   argv[3]="/index.html"; 
              ret_val = wmiconfig_handler(4, argv);
              if(ret_val != 0) printf("access web page error\n");
              printf("Test done\n");
              
              rec_cnt = 0;
              memset(rec_buf, 0, 512+512+512);
          }    
        }
        else //less than AT_MIN_LEN
        {
          
        }
       // printf("%s\n","get");
        //uint8_t ch = 0xaa;
        //write( rs232_dev, &ch, 1);
    }
#endif
}

#endif

/* EOF */
