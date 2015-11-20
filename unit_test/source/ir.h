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

#ifndef _IR_H_
#define _IR_H_

//#define IR_FLASH_START_ADDR		(FLASH_STORE_ADDR + 0x400) // we only test ,don't write to flash
#define IR_MAX_KEY				24
#define MAX_BITINDEX 			68


enum
{
	IR_MODE_0 = 0,
	IR_MODE_1,
	IR_MAX_MODE
};

enum 
{
   	IR_TX_IDLE = 0,
   	IR_TX_HEAD,
   	IR_TX_TRANSITION,
   	IR_TX_DATA_HIGH,
   	IR_TX_DATA_LOW,
   	IR_TX_STOP
};

enum 
{
   	IR_RX_IDLE = 0,
	IR_RX_HEAD,
   	IR_RX_TRANSITION,
   	IR_RX_DATA_HIGH,
   	IR_RX_DATA_LOW
};

typedef struct
{
   	uint16 head;
   	uint16 transition;
   	uint8 data[MAX_BITINDEX];
	uint8 learned;
} ir_learn_data_struct;

typedef struct
{
	uint8 mode;
   	ir_learn_data_struct learndata[IR_MAX_KEY][IR_MAX_MODE];
} ir_save_data_struct;

typedef struct
{
   	uint16 head;
   	uint16 transition;
   	uint32 keycode;
} ir_key_struct;

void ir_init(void);
void ir_carrier_init(void);
void ir_carrier_enable(void);
void ir_carrier_disable(void);
void ir_readkey(void);
void ir_savekey(uint8 keyIndex);
void ir_sendkey(uint8 keyIndex);
void ir_set_mode(uint8 mode);
void ir_timer_init(void);
void ir_tx_timer_int_enable(void);
void ir_tx_timer_int_disable(void);
void ir_tx_timer_enable(void);
void ir_tx_timer_disable(void);
void ir_rx_timer_int_enable(void);
void ir_rx_timer_int_disable(void);
void ir_rx_timer_enable(void);
void ir_rx_timer_disable(void);
void ir_tx_timer_isr(void *);
void ir_rx_timer_isr(void *);

void ir_recv_handle(void);
void ir_recv_pin_isr(void * pin);

void ir_set_learn_mode(uint8 learnMode);
void ir_set_received(uint8 received);
uint8 ir_is_learn_mode(void);
uint8 ir_is_received(void);
void ir_led_flicker(void);


int unit_test_ir();

#endif /* #ifndef _IR_H_ */
/* EOF */
