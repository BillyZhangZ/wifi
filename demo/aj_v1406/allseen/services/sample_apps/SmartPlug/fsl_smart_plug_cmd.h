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

#if !defined(__FSL_SMART_PLUG_CMD_H__)
#define __FSL_SMART_PLUG_CMD_H__

#include <stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
 /* Command Class */
#define SMTPLG_CMD_CLS_WIFI_CTL                                 0x01
#define SMTPLG_CMD_CLS_APP                                      0x02

/* Command operation code */
/* WIFI Control */
#define SMTPLG_CMD_OP_WIFI_INIT	                                0x01
#define SMTPLG_CMD_OP_WIFI_RESET                                0x02
#define SMTPLG_CMD_OP_WIFI_PWRSAVE_MODE                         0x03
#define SMTPLG_CMD_OP_WIFI_LP_MODE                              0x04
#define SMTPLG_CMD_OP_WIFI_NORMAL_MODE	                        0x05
#define SMTPLG_CMD_OP_WIFI_SET_SSID                             0x06
#define SMTPLG_CMD_OP_WIFI_SET_PASSWD                           0x07
#define SMTPLG_CMD_OP_WIFI_SET_CONN_PASSWD                      0x08

/* APP */
#define SMTPLG_CMD_OP_SET_PWR_ONOFF                             0x01
#define SMTPLG_CMD_OP_SET_WIFI_RESET                            0x02
#define SMTPLG_CMD_OP_SET_PWRSAVE_MODE	                        0x03

#define SMTPLG_CMD_OP_GET_PWRSAVE_MODE	                        0x04
#define SMTPLG_CMD_OP_GET_PWR_STS                               0x05
#define SMTPLG_CMD_OP_GET_ACT_PWR                               0x06
#define SMTPLG_CMD_OP_GET_REACT_PWR                             0x07
#define SMTPLG_CMD_OP_GET_TOTAL_PWR                             0x08
#define SMTPLG_CMD_OP_GET_TOTAL_ACT_PWR_SUM                     0x09
#define SMTPLG_CMD_OP_GET_GRID_FREQ                             0x0A
#define SMTPLG_CMD_OP_GET_HIST_TIME                             0x0B

#define SMTPLG_CMD_OP_SET_TIMER_ONOFF                           0x0C
#define SMTPLG_CMD_OP_SET_TIMER_BEGIN                           0x0D
#define SMTPLG_CMD_OP_SET_TIMER_END                             0x0E
#define SMTPLG_CMD_OP_SET_CUR_TIME                              0x0F

#define SMTPLG_CMD_OP_GET_TIMER_BEGIN                           0x10   //no used ?
#define SMTPLG_CMD_OP_GET_TIMER_END                             0x11   //no used ?
#define SMTPLG_CMD_OP_SET_CONN_PASSWD                           0x12   //no used ?

#define SMTPLG_SUPPORTED_TIMER                                  1

/* Error code */
#define SMTPLG_CMD_EXEC_SUCCEE                                  0x90
#define SMTPLG_CMD_EXEC_FAIL                                    0x60

#define LC_MAX                                                  8
#define MAGIC_NUMBER_1                                          's'
#define MAGIC_NUMBER_2                                          'p'

/* Command structure */
#define SMTPLUG_COMM_PKG_HEADER_LEN            8
#define SMTPLUG_COMM_PKG_BODY_LEN              8
#define SMTPLUG_COMM_PKG_MAGIC_NUM_LEN         2

/* Smart plug command header */
typedef struct
{
  uint8_t MAGIC1;       /* Magic number */
  uint8_t MAGIC2;       /* Magic number */
  uint8_t CLA;  /* Command class */
  uint8_t INS;  /* Operation code */
  uint8_t P1;   /* Parameter 1 */
  uint8_t P2;   /* Parameter 2 */
  uint8_t P3;   /* Parameter 3 */
  uint8_t LC;   /* Data field length */
} SMTPLG_Header, SMTPLG_Responce;

/* Smart plug command body */
typedef struct 
{
  uint8_t Data[LC_MAX];  /* Data */
} SMTPLG_Body;

/* Command structure */
typedef struct
{
  SMTPLG_Header Header;
  SMTPLG_Body Body;
} SMTPLG_Command;

typedef struct
{
    uint8_t  hour;
    uint8_t  min;
    uint8_t  sec;
    uint8_t  onoff;
} SMTPLUG_TM;

typedef struct
{
    uint16_t  year;
    uint8_t  mon;
    uint8_t  mday;
    uint8_t  hour;
    uint8_t  min;
    uint8_t  sec;
} SMTPLUG_Date_Time;

typedef struct 
{
    SMTPLUG_TM begin;
    SMTPLUG_TM end;
    uint8_t onoff;
} SMTPLUG_Timer;

#define CMD_GET  0x01
#define CMD_SET  0x02
#define CMD_ACK  0x03

void command_send(uint32_t msg_type, uint16_t code, void * dataPtr, uint32_t len);

uint16_t cls_shiftor_opc(uint8_t cls, uint8_t opc);

uint32_t set_wifi_sleep_mode(uint8_t pwr_mode);

#endif /* __FSL_SMART_PLUG_CMD_H__*/
/*******************************************************************************
 * EOF
 ******************************************************************************/

