/*
 * Copyright (c) 2014, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 * of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "mqx.h"
#include <bsp.h> 
#include <fio.h>
#include <lwmsgq.h>
#include "fsl_smart_plug_cmd.h"
#include "ControlPanelProvided.h"
//#include "iwcfg.h"

#define UART_CHANNEL   "ittya:"           // ittya
 
#define MAGIC_NUM 2
#define CMD_HEAD_NUM 8
#define CMD_BODY_NUM 8

static uint8_t command_rxd_buf[16];
static uint8_t command_txd_buf[16];
static MQX_FILE_PTR uart_rx_dev = NULL, uart_tx_dev = NULL;

typedef struct {
  void *  data;     /* Point to date*/
  uint32_t len; /* date length,  number of bytes */
  uint32_t type;
  uint8_t cmd_cls;  /* Command class, [0]: class, [1]: opt */
  uint8_t cmd_opc;
} msg_t;

/* The granularity of message queue is one message. Its size is the multiplier of _mqx_max_type. Get that multiplier */
#define TX_TASKQ_GRANM ((sizeof(msg_t) - 1) / sizeof(_mqx_max_type) + 1)
_mqx_max_type  uart_tx_taskq[10 * TX_TASKQ_GRANM * sizeof(_mqx_max_type)]; /* prepare message queue for 10 events */

extern DatePropertyValue rtc_date;
extern void Custom_Api_PowerUpDown(uint8_t power_up);
extern int AJ_ath_driver_reset(void);
extern int AJ_ResetWiFi(void);
extern uint32_t AJ_iwcfg_set_power (uint32_t dev_num, uint32_t pow_val, uint32_t flags );

static uint8_t wifi_state[2] = 0;
static bool  UI_data_flag = FALSE;

void set_wifi_init_state(uint8_t state)
{
  wifi_state[0] = 0x01;   // wifi initialize done
  wifi_state[1] = state;
}

void set_wifi_init_respond(void)
{
  command_send(CMD_ACK, cls_shiftor_opc(SMTPLG_CMD_CLS_WIFI_CTL, SMTPLG_CMD_OP_WIFI_INIT), wifi_state, 2);
}

/* 1: Sleep mode, 0: normal mode */
uint32_t set_wifi_sleep_mode(uint8_t pwr_mode)        
{
  if (pwr_mode != 0 && pwr_mode != 1){
    printf("Invalid power mode\n");
    return -1;
  }

  return(AJ_iwcfg_set_power(BSP_DEFAULT_ENET_DEVICE, pwr_mode,0));
}

void set_mcu_lpmode(uint8_t power)
{
  if (power ==0){
    
     //printf("\nSet mcu enter sleep mode.\n");
         
     printf ("%s\n", _lpm_set_operation_mode (LPM_OPERATION_MODE_SLEEP) == 0 ? "OK" : "ERROR");
    //_lpm_set_operation_mode (LPM_OPERATION_MODE_SLEEP);     //LPM_OPERATION_MODE_SLEEP   LPM_OPERATION_MODE_WAIT
  }
  else if(power == 1){
  /* Return to RUN mode */
      _lpm_set_operation_mode (LPM_OPERATION_MODE_HSRUN);   // HSRUN or no ?
  }
}

/* 1: Power up mode, 0: Power down mode */
void set_wifi_power_up(uint8_t power_up)
{
  Custom_Api_PowerUpDown(power_up);
  
  if(power_up == 0){
    _time_delay(200);
    set_mcu_lpmode(0);
  }

  if(power_up == 1){
    set_mcu_lpmode(1);
    _time_delay(200);
    AJ_ResetWiFi();
    _time_delay(200);
  }
}

uint16_t cls_shiftor_opc(uint8_t cls, uint8_t opc)
{
  uint16_t  code_date = cls;

  code_date <<= 8;
  code_date |= opc;
  
  return code_date;
}

void command_send(uint32_t msg_type, uint16_t code, void * dataPtr, uint32_t len)
{
   msg_t msg;
   msg.type = msg_type;
   msg.cmd_opc =  code & 0x00ff;              /* code == cmd_cls | cmd_opc*/
   msg.cmd_cls = (code & 0xff00)>>8;
   msg.data = dataPtr;
   msg.len = len;
   
   if(LWMSGQ_FULL == _lwmsgq_send(uart_tx_taskq, (_mqx_max_type_ptr) &msg, 0)){
    printf("Could not inform to uart_tx_task 2\n");
  }
}

int smtplg_cmd_handle(uint8_t * cmd_hdr_p, int len)  /* len represent data body length*/
{ 
  SMTPLG_Header * plg_head_p = (SMTPLG_Header *) (cmd_hdr_p);
  SMTPLG_Body * plg_body_p = NULL;
  uint32_t power, i, zpos = 0;
  int ret = 1;

  if(len > 0)     /* Have data body */
    plg_body_p =(SMTPLG_Body *)(cmd_hdr_p + CMD_HEAD_NUM);

  if((plg_head_p->CLA) == SMTPLG_CMD_CLS_WIFI_CTL){
    switch(plg_head_p->INS){
    case SMTPLG_CMD_OP_WIFI_INIT:
         UI_data_flag = TRUE;
         set_wifi_init_respond();
         break;
      
    case SMTPLG_CMD_OP_WIFI_LP_MODE :  
         set_wifi_power_up(0);
         break;

    case SMTPLG_CMD_OP_WIFI_NORMAL_MODE :  
         set_wifi_power_up(1);
         break;
    default :
         break;
    }
  }
  else if((plg_head_p->CLA) == SMTPLG_CMD_CLS_APP){
    //printf("__Evanguo: dispatch_cmd CLS APP, op code 0x%x\n", (plg_head_p->CLA));
 
    switch(plg_head_p->INS){
      case SMTPLG_CMD_OP_GET_PWR_STS :  
           if(( plg_body_p != NULL)&&(plg_head_p->P2 == 0x90)){
             setActualPlugONOFFStatus(plg_body_p->Data, len);    //set actual plug ON/OFF status
           }
           break;
     case  SMTPLG_CMD_OP_SET_PWR_ONOFF :
           if(plg_head_p->P2 == 0x90){   /* SET Power ON/OFF finished */
	         command_send(CMD_GET, cls_shiftor_opc(SMTPLG_CMD_CLS_APP, SMTPLG_CMD_OP_GET_PWR_STS), NULL, 0);
           }
	   break;   
      case SMTPLG_CMD_OP_GET_ACT_PWR :   
           //printf("__Evanguo: dispatch_cmd SMTPLG_CMD_OP_GET_ACT_PWR APP, op code 0x%x\n", plg_head_p->INS);

           if(( plg_body_p != NULL)&&(plg_head_p->P2 == 0x90)){
             setPowerValue(plg_body_p->Data, len, 1);    //set active power display value
           }
           break;
      case SMTPLG_CMD_OP_GET_REACT_PWR :   
           //printf("__Evanguo: dispatch_cmd SMTPLG_CMD_OP_GET_REACT_PWR APP, op code 0x%x\n", plg_head_p->INS);

           if(( plg_body_p != NULL)&&(plg_head_p->P2 == 0x90)){
             setPowerValue(plg_body_p->Data, len, 2);   //set reactive power display value
           }
           break;
      case SMTPLG_CMD_OP_GET_TOTAL_PWR :   
           //printf("__Evanguo: dispatch_cmd SMTPLG_CMD_OP_GET_TOTAL_PWR APP, op code 0x%x\n", plg_head_p->INS);

           if(( plg_body_p != NULL)&&(plg_head_p->P2 == 0x90)){
             setPowerValue(plg_body_p->Data, len, 3);   //set total power display value
           }
           break;
      case SMTPLG_CMD_OP_GET_TOTAL_ACT_PWR_SUM :   
           //printf("__Evanguo: dispatch_cmd SMTPLG_CMD_OP_GET_TOTAL_ACT_PWR_SUM APP, op code 0x%x\n", plg_head_p->INS);

           if(( plg_body_p != NULL)&&(plg_head_p->P2 == 0x90)){
             setPowerValue(plg_body_p->Data, len, 4);   //set active power sum display value
           }
           break;
      case SMTPLG_CMD_OP_GET_GRID_FREQ :
           if(( plg_body_p != NULL)&&(plg_head_p->P2 == 0x90)){
             setCurrentGridFreq(plg_body_p->Data, len);
           }
           break;
      case SMTPLG_CMD_OP_GET_HIST_TIME :
           if(( plg_body_p != NULL)&&(plg_head_p->P2 == 0x90)){
	         setHistoryRunTime(plg_body_p->Data, len);
           }
           break;
      default: break;
    }	
  }
  else{
    ret = -1;
  }	
  return ret;
}

void uart_rx_task ( uint32_t initial_data )
{
  static uint32_t recv_cnt = 0;
  static uint8_t UART_CMD_MAGIC[MAGIC_NUM] = {MAGIC_NUMBER_1, MAGIC_NUMBER_2};
  uint8_t * buf_p = command_rxd_buf;
  int len = 0;
  uint8_t  uart_data;
  
  uart_rx_dev = fopen( UART_CHANNEL, NULL );

  if( uart_rx_dev == NULL ){
    /* device could not be opened */
    printf("__Evanguo: Open uart rx dev failed, uart_rx_task blocked\n");
    _task_block();
  }   

  printf("__Evanguo: Enter uart_rx_task\n");
  while(1){
    if(recv_cnt >= MAGIC_NUM){  /* recieved  available data transfer */	
      fread(buf_p + MAGIC_NUM, 1, 6, uart_rx_dev);  /* Wait for 6(CMD_HEAD_NUM - MAGIC_NUM) bytes data for command header exclude magic number*/
      recv_cnt = CMD_HEAD_NUM;
      /* Finished read CMD_HEAD_NUM bytes */
      len = buf_p[CMD_HEAD_NUM - 1];   /* The last byte, present LC */
      if(len > CMD_BODY_NUM) len = CMD_BODY_NUM;
      if(len > 0){
        fread(buf_p + CMD_HEAD_NUM, 1, len, uart_rx_dev);    /* Wait for 8 bytes data for command body*/
        recv_cnt += len;
      }
      smtplg_cmd_handle(buf_p, len);
      recv_cnt = 0;
    }
    else{                   /* Check Magic number data in the front of command header, recv_cnt = 0, 1 */
      fread(&uart_data, 1, 1, uart_rx_dev);
      if(uart_data == UART_CMD_MAGIC[recv_cnt]){ 
        buf_p[recv_cnt] = uart_data;
        recv_cnt ++;
      }
      else{
        recv_cnt = 0;
      }
    }
  }

  fclose(uart_rx_dev);
}

void uart_tx_task ( uint32_t initial_data )
{  
   SMTPLG_Header * plg_head_p = (SMTPLG_Header *) command_txd_buf;
   SMTPLG_Body * plg_body_p = (SMTPLG_Body *)(command_txd_buf + CMD_HEAD_NUM);  //Note, fix a bug,  not plg_head_p + CMD_HEAD_NUM;
   uint32_t len = 0;
   _mqx_uint res;
   msg_t msg;
   static uint8_t opt_cnt = SMTPLG_CMD_OP_GET_ACT_PWR;
   
   uart_tx_dev  = fopen( UART_CHANNEL, NULL );					   
   if( uart_tx_dev == NULL )
   {
     /* device could not be opened */
     printf("__Evanguo: Open uart tx dev failed, uart_tx_task blocked\n");
     _task_block();
   }
   
   if (MQX_OK != _lwmsgq_init(uart_tx_taskq, 10, TX_TASKQ_GRANM)) 
   {
     printf("lwmsgq_init connect_taskq failed\n");
     _task_block();
   }
   
   command_send(CMD_GET, cls_shiftor_opc(SMTPLG_CMD_CLS_APP, SMTPLG_CMD_OP_GET_PWR_STS), NULL, 0);
   
   rtc_date.fullYear = 2014;
   rtc_date.month = 1;
   rtc_date.mDay = 1;
   
   while(1){
     if((res = _lwmsgq_receive(&uart_tx_taskq, (_mqx_max_type *) &msg, LWMSGQ_RECEIVE_BLOCK_ON_EMPTY,  100 /* ticks */, 0)) == MQX_OK){  /* 1tick =5ms */
       plg_head_p->CLA = msg.cmd_cls;
       plg_head_p->INS = msg.cmd_opc;
       
       if(msg.type == CMD_SET){
         if(msg.len > 1){
          // if((plg_head_p->CLA == SMTPLG_CMD_CLS_APP)&&((plg_head_p->INS == SMTPLG_CMD_OP_SET_TIMER_BEGIN)||  \
		  	             //                               (plg_head_p->INS == SMTPLG_CMD_OP_SET_TIMER_END)||(plg_head_p->INS == SMTPLG_CMD_OP_SET_CUR_TIME)))
           { /* SET work time */
             plg_head_p->P1 = 0;
             plg_head_p->P2 = 0;
             plg_head_p->P3 = 0;
             plg_head_p->LC = (uint8_t)msg.len;
             if(plg_head_p->LC > CMD_BODY_NUM) 
               plg_head_p->LC = CMD_BODY_NUM;
                                           
             _mem_zero((void *)plg_body_p->Data, CMD_BODY_NUM);
             _mem_copy((void *)msg.data, (void *)plg_body_p->Data, plg_head_p->LC);
             //len = CMD_HEAD_NUM + plg_head_p->LC;
             len = CMD_HEAD_NUM + CMD_BODY_NUM;
           } 
         }
         else if(msg.len == 1){
           plg_head_p->P1 = *(uint8_t *)msg.data;   /* ON/OFF */
           plg_head_p->P2 = 0;
           plg_head_p->P3 = 0;
           plg_head_p->LC = 0;
           len = CMD_HEAD_NUM;
        }
       }
       else if(msg.type == CMD_GET){
           plg_head_p->P1 = 0;
           plg_head_p->P2 = 0;
           plg_head_p->P3 = 0;
           plg_head_p->LC = 0;
           len = CMD_HEAD_NUM;      
       }
       else if(msg.type == CMD_ACK){
         if((plg_head_p->CLA == SMTPLG_CMD_CLS_WIFI_CTL)&&(plg_head_p->INS == SMTPLG_CMD_OP_WIFI_INIT)){
           plg_head_p->P1 = *(uint8_t *)msg.data;   /* ON/OFF */
           plg_head_p->P2 = *((uint8_t *)msg.data + 1);
           plg_head_p->P3 = 0;
           plg_head_p->LC = 0;
           len = CMD_HEAD_NUM;
        }
       }
     }  
     else if(res == LWMSGQ_TIMEOUT){		
       //printf("__Evanguo: wait msg time out\n");
       msg.type == CMD_GET;
       plg_head_p->CLA = SMTPLG_CMD_CLS_APP;
       plg_head_p->INS = opt_cnt ++;	  
       plg_head_p->P1 = 0;
       plg_head_p->P2 = 0;
       plg_head_p->P3 = 0;
       plg_head_p->LC = 0;
       len = CMD_HEAD_NUM;
       if( opt_cnt > SMTPLG_CMD_OP_GET_HIST_TIME)
         opt_cnt = SMTPLG_CMD_OP_GET_ACT_PWR;
     }
     else{
       printf("__Evanguo: _lwmsgq_receive uart_tx_taskq failed\n");
     }
     plg_head_p->MAGIC1 = MAGIC_NUMBER_1;
     plg_head_p->MAGIC2 = MAGIC_NUMBER_2;

     if(UI_data_flag == TRUE){
       write( uart_tx_dev, (void *)plg_head_p, len);
       fflush( uart_tx_dev );  /* ioctl  IO_IOCTL_FLUSH_OUTPUT */
     }
     
   }

   fclose(uart_tx_dev);
}
/*******************************************************************************
 * EOF
 ******************************************************************************/

