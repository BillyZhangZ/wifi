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

#include "74hc595.h"

/**************************************************************************
  Local header files for this application
 **************************************************************************/
#include "ir.h"

// os utility
#define utility_memcpy memcpy
#define utility_memset memset

#define flash_app_erase(x,y)
#define flash_app_memcpy_32bit  memcpy

uint32 test_recvKey = 0;


/* pin mux setting */
#define BSP_IR_PWM   			(GPIO_PORT_C | GPIO_PIN2)  /* IR carrier */
#define BSP_IR_RECV      		(GPIO_PORT_C | GPIO_PIN6)  /* IR capture */

#define BSP_IR_PWM_MUX_GPIO     (LWGPIO_MUX_C2_GPIO)
#define BSP_IR_PWM_MUX_FTM0_CH1 (4)                         /* ALT4 */
#define BSP_IR_RECV_MUX_GPIO 	(LWGPIO_MUX_C6_GPIO)

#define TX_TIMER_INT   INT_PIT1
#define RX_TIMER_INT   INT_PIT2


LWGPIO_STRUCT ir_pwm_pin, ir_recv_pin;

volatile uint8 isLearnMode = FALSE;
volatile uint8 isReceived = FALSE;
volatile uint8 recvState = IR_RX_IDLE;
ir_learn_data_struct recvData;
uint8 isLearnKey = FALSE;
ir_key_struct irData;
ir_learn_data_struct learnData;
ir_save_data_struct saveData;

ir_save_data_struct dummy_flash_saveData;
#define IR_FLASH_START_ADDR    (&dummy_flash_saveData) 

ir_key_struct defaultKey[IR_MAX_KEY][IR_MAX_MODE] =  //Mode 0 Hisense, Mode 1 Samsung
{
  {{900, 450, 0x00FDB04F}, {450, 450, 0xE0E040BF}}, //POWER
  {{900, 450, 0x00FD6897}, {450, 450, 0xE0E006F9}}, //UP
  {{900, 450, 0x00FDE817}, {450, 450, 0xE0E08679}}, //DOWN
  {{900, 450, 0x00FD9867}, {450, 450, 0xE0E0A659}}, //LEFT
  {{900, 450, 0x00FD18E7}, {450, 450, 0xE0E046B9}}, //RIGHT
  {{900, 450, 0x00FDA857}, {450, 450, 0xE0E016E9}}, //OK
  {{900, 450, 0x00FD48B7}, {450, 450, 0xE0E0807F}}, //SOURCE
  {{900, 450, 0x00FD52AD}, {450, 450, 0xE0E048B7}}, //P+
  {{900, 450, 0x00FDD22D}, {450, 450, 0xE0E008F7}}, //P-
  {{900, 450, 0x00FD22DD}, {450, 450, 0xE0E0E01F}}, //V+
  {{900, 450, 0x00FDC23D}, {450, 450, 0xE0E0D02F}}, //V-
  {{900, 450, 0x00FD708F}, {450, 450, 0xE0E0F00F}}, //MUTE
  {{900, 450, 0x00FD807F}, {450, 450, 0xE0E020DF}}, //ONE
  {{900, 450, 0x00FD40BF}, {450, 450, 0xE0E0A05F}}, //TWO
  {{900, 450, 0x00FDC03F}, {450, 450, 0xE0E0609F}}, //THREE
  {{900, 450, 0x00FD20DF}, {450, 450, 0xE0E010EF}}, //FOUR
  {{900, 450, 0x00FDA05F}, {450, 450, 0xE0E0906F}}, //FIVE
  {{900, 450, 0x00FD609F}, {450, 450, 0xE0E050AF}}, //SIX
  {{900, 450, 0x00FDE01F}, {450, 450, 0xE0E030CF}}, //SEVEN
  {{900, 450, 0x00FD10EF}, {450, 450, 0xE0E0B04F}}, //EIGHT
  {{900, 450, 0x00FD906F}, {450, 450, 0xE0E0708F}}, //NINE
  {{900, 450, 0x00FDD02F}, {450, 450, 0xE0E0C837}}, //RECALL
  {{900, 450, 0x00FD00FF}, {450, 450, 0xE0E08877}}, //ZERO
  {{900, 450, 0x00000000}, {450, 450, 0x00000000}}  //BACKSPACE
};

// declare
void ir_recv_pin_isr(void * pin);

// enable - high
static inline void ir_power_enable(uint8_t enable)
{
	if(enable)
        mux_74hc595_set_bit(BSP_74HC595_0,   BSP_74HC595_VIR_RCV_3V3);
	else
        mux_74hc595_clear_bit(BSP_74HC595_0,   BSP_74HC595_VIR_RCV_3V3);
}

void ir_init(void)
{
    // PTC2   BSP_IR_PWM
    if (!lwgpio_init(&ir_pwm_pin, BSP_IR_PWM, LWGPIO_DIR_OUTPUT /*LWGPIO_DIR_INPUT*/, LWGPIO_VALUE_NOCHANGE)) {
        printf("Initializing IR recvive GPIO as output failed.\n");
        //_task_block();
    }
    lwgpio_set_functionality(&ir_pwm_pin, BSP_IR_PWM_MUX_GPIO);

    // PTC6 BSP_IR_RECV
    if (!lwgpio_init(&ir_recv_pin, BSP_IR_RECV, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE)) {
        printf("Initializing IR recvive GPIO as input failed.\n");
        //_task_block();
    }
    lwgpio_set_functionality(&ir_recv_pin, BSP_IR_RECV_MUX_GPIO);
    //lwgpio_set_attribute(&btn1, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
    /* enable gpio functionality for given pin, react on both edge */
    if (!lwgpio_int_init(&ir_recv_pin, LWGPIO_INT_MODE_FALLING | LWGPIO_INT_MODE_RISING)) {
        printf("Initializing IR recvive GPIO for interrupt failed.\n");
        _task_block();
    }
    /* install gpio interrupt service routine */
    _int_install_isr(lwgpio_int_get_vector(&ir_recv_pin), ir_recv_pin_isr, (void *) &ir_recv_pin);

	/* set the interrupt level, and unmask the interrupt in interrupt controller*/
    _bsp_int_init(lwgpio_int_get_vector(&ir_recv_pin), 3, 0, TRUE /* FALSE */);
#if 0  /* don't enable interrupt at init */
    /* enable interrupt on GPIO peripheral module*/
    lwgpio_int_enable(&ir_recv_pin, TRUE);
#endif

    /*IR power*/
    /*Turn off IR power*/
    ir_power_enable(0);
  
    ir_carrier_init();
    // liutest carrier 38k
    //while(1){};
   
	ir_timer_init();
	// liutest pit1
	/*
	ir_carrier_disable();
    ir_tx_timer_enable();
    ir_tx_timer_int_enable();
    while(1){}; */

	// clare dumy IR_FLASH_START_ADDR
	memset(IR_FLASH_START_ADDR,0, sizeof(dummy_flash_saveData));

    ir_readkey();

    ir_carrier_disable();
    ir_rx_timer_disable();
    ir_tx_timer_disable();
}

#if (0)
void ir_carrier_init(void)
{
    // FTM0_CH1 // alt4
    lwgpio_set_functionality(&ir_pwm_pin, BSP_IR_PWM_MUX_FTM0_CH1);

    SIM_SCGC6|=SIM_SCGC6_FTM0_MASK;     // FTM0 clk

    FTM0_MODE |= FTM_MODE_WPDIS_MASK;   // write protect disable

    FTM0_C1SC |= FTM_CnSC_ELSB_MASK;    // ELSB=1
    FTM0_C1SC &= ~FTM_CnSC_ELSA_MASK; 
    FTM0_C1SC |= FTM_CnSC_MSB_MASK;     // MSB=1
    FTM0_SC = /* 0x0c */0x09;           // system clk 120M ,divide 2 = 60M
    FTM0_MODE &= ~1;                    // FTM0 enable
    FTM0_OUTMASK = 0XFD;                // FTM0 CH1 output
    FTM0_QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;// diable quadrature decoder
    FTM0_COMBINE = 0; //DECAPEN=0
    FTM0_CNTIN = 0;   // count initial value is zero
    FTM0_MOD = 1578;   // pwm clk = (MOD-CNTIN+1) * input clock cycle (1578.947368421053 - 1)
    FTM0_C1V = 526;    // 1/3 duty
    FTM0_CNT=0;
}  
#else
void ir_carrier_init(void)
{
    // FTM0_CH1 // alt4
    lwgpio_set_functionality(&ir_pwm_pin, BSP_IR_PWM_MUX_FTM0_CH1);

    SIM_SCGC6|=SIM_SCGC6_FTM0_MASK;     // FTM0 clk

    FTM0_MODE |= FTM_MODE_WPDIS_MASK;   // write protect disable
    FTM0_MOD = 1578;   // pwm clk = (MOD-CNTIN+1) * input clock cycle (1578.947368421053 - 1)

    FTM0_C1SC |= FTM_CnSC_ELSB_MASK;    // ELSB=1
    FTM0_C1SC &= ~FTM_CnSC_ELSA_MASK;
    FTM0_C1SC |= FTM_CnSC_MSB_MASK;     // MSB=1
    FTM0_C1V = (526);    // 1/3 duty

    FTM0_MODE &= ~1;                    // FTM0 enable
    FTM0_OUTMASK = 0XFD;                // FTM0 CH1 output
    FTM0_QDCTRL &= ~FTM_QDCTRL_QUADEN_MASK;// diable quadrature decoder
    FTM0_COMBINE = 0; //DECAPEN=0
    FTM0_CNTIN = 0;   // count initial value is zero

    FTM0_SC = /* 0x09*/ 0x08;           // system clk 120M ,divide 2 = 60M

    FTM0_CNT=0;
}  
#endif


void ir_carrier_enable(void)
{
    // enanble count
    //FTM0_SC &= (~FTM_SC_CLKS_MASK);
	FTM0_SC |=/* 0x09*/ 0x08; 
    lwgpio_set_functionality(&ir_pwm_pin, BSP_IR_PWM_MUX_FTM0_CH1);
}

void ir_carrier_disable(void)
{
    // disable count
    FTM0_SC &= (~FTM_SC_CLKS_MASK);
	lwgpio_set_value(&ir_pwm_pin,LWGPIO_VALUE_LOW);
    lwgpio_set_functionality(&ir_pwm_pin, BSP_IR_RECV_MUX_GPIO);
}

void ir_readkey(void)
{
  utility_memcpy(&saveData, (uint8*)IR_FLASH_START_ADDR, sizeof(ir_save_data_struct));
  
  if(saveData.mode >= IR_MAX_MODE)
  	saveData.mode = IR_MODE_0;
}

void ir_savekey(uint8 keyIndex)
{
  recvData.learned = TRUE;
  utility_memcpy(&saveData.learndata[keyIndex][saveData.mode], &recvData, sizeof(ir_learn_data_struct));
  flash_app_erase((void *)IR_FLASH_START_ADDR, sizeof(ir_save_data_struct));
  flash_app_memcpy_32bit((void *)IR_FLASH_START_ADDR, &saveData, sizeof(ir_save_data_struct));
}

void ir_sendkey(uint8 keyIndex)
{
  if(saveData.learndata[keyIndex][saveData.mode].learned == TRUE)
  {
    isLearnKey = TRUE;
    utility_memcpy(&learnData, &saveData.learndata[keyIndex][saveData.mode], sizeof(ir_learn_data_struct));
  }
  else
  {
    isLearnKey = FALSE;
  	irData.head = defaultKey[keyIndex][saveData.mode].head;
	irData.transition = defaultKey[keyIndex][saveData.mode].transition;
	irData.keycode = defaultKey[keyIndex][saveData.mode].keycode;
  }
  ir_tx_timer_enable();
  ir_tx_timer_int_enable();
}

void ir_set_mode(uint8 mode)
{
  saveData.mode = mode;
  printf(">>>>irMode = %d\n", saveData.mode);
  flash_app_erase((void *)IR_FLASH_START_ADDR, sizeof(ir_save_data_struct));
  flash_app_memcpy_32bit((void *)IR_FLASH_START_ADDR, &saveData, sizeof(ir_save_data_struct));
}

void ir_timer_init(void)
{
  /* SIM_SCGC6: PIT=1 */
  SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;                                   
  /* PIT_MCR: MDIS=0,FRZ=0 */
  PIT_MCR = 0x00U;                                   
  /* PIT_TFLG0: TIF=1 */
  PIT_TFLG1 = PIT_TFLG_TIF_MASK;                                                              
  /* PIT_TFLG1: TIF=1 */
  PIT_TFLG2 = PIT_TFLG_TIF_MASK;                                   
  /* PIT_LDVAL0: TSV=0xEF */
  PIT_LDVAL1 = PIT_LDVAL_TSV(/*0xEF*/0x257);        /*  9.958333333333333 us */
  /* PIT_LDVAL1: TSV=0x20F0A3 */
  PIT_LDVAL2 = PIT_LDVAL_TSV(/*0x20F0A3*/0x6DDD00);   /* 120ms for NEC, intr prior recv pin same as rx timer  ,rev C no IR_RECEIVER  */
#if 0
  /* NVIC_IPR5: PRI_22=0x80 */
  NVIC_IPR5 = (uint32_t)((NVIC_IPR5 & (uint32_t)~(uint32_t)(
               NVIC_IP_PRI_22(0x7F)
              )) | (uint32_t)(
               NVIC_IP_PRI_22(0x80)
              ));
  /* NVIC_ISER: SETENA|=0x00400000 */
  NVIC_ISER |= NVIC_ISER_SETENA(0x00400000);
#else
    if(_int_install_isr(TX_TIMER_INT, ir_tx_timer_isr, NULL) == NULL) {
		printf("install ir tx timer isr failed\n");
        return ;
    } /* Endif */
    _bsp_int_init(TX_TIMER_INT, 3, 0, TRUE /*FALSE*/);

    if(_int_install_isr(RX_TIMER_INT, ir_rx_timer_isr, NULL) == NULL) {
		printf("install ir rx timer isr failed\n");
        return ;
    } /* Endif */
    _bsp_int_init(RX_TIMER_INT, 3, 0, TRUE /*FALSE*/);
#endif
  /* PIT_TCTRL0: TIE=0,TEN=0 */
  PIT_TCTRL1 &= (uint32_t)~(uint32_t)(
                 PIT_TCTRL_TIE_MASK |
                 PIT_TCTRL_TEN_MASK |
                 0xFFFFFFF8U
                );
  /* PIT_TCTRL1: CHN=0,TIE=0,TEN=0 */
  PIT_TCTRL2 = 0x00U;                                   
}

void ir_tx_timer_int_enable(void)
{
  PIT_TCTRL1 |= PIT_TCTRL_TIE_MASK;  
}

void ir_tx_timer_int_disable(void)
{
  PIT_TCTRL1 &= ~PIT_TCTRL_TIE_MASK;           
}

void ir_tx_timer_enable(void)
{
  PIT_TCTRL1 |= PIT_TCTRL_TEN_MASK;  
}

void ir_tx_timer_disable(void)
{
  PIT_TCTRL1 &= ~PIT_TCTRL_TEN_MASK;           
}

void ir_rx_timer_int_enable(void)
{
  PIT_TCTRL2 |= PIT_TCTRL_TIE_MASK;  
}

void ir_rx_timer_int_disable(void)
{
  PIT_TCTRL2 &= ~PIT_TCTRL_TIE_MASK;           
}

void ir_rx_timer_enable(void)
{
  PIT_TCTRL2 |= PIT_TCTRL_TEN_MASK;  
}

void ir_rx_timer_disable(void)
{
  PIT_TCTRL2 &= ~PIT_TCTRL_TEN_MASK;           
}

void ir_tx_timer_isr(void * param)
{
  static uint32 pitCounter = 0;
  static uint8 sendState = IR_TX_IDLE;
  static uint8 bitIndex = 0;

#if 0
  // test  add
  PIT_TFLG1 |= PIT_TFLG_TIF_MASK;
  if(bitIndex == 0) {
    lwgpio_set_value(&ir_pwm_pin,LWGPIO_VALUE_LOW);
  }
  else
    lwgpio_set_value(&ir_pwm_pin,LWGPIO_VALUE_HIGH);
  bitIndex = ~ bitIndex;
  return;
#endif  

    pitCounter++;
  
    switch(sendState)
    {
      case IR_TX_IDLE:
		ir_carrier_enable();
		pitCounter = 0;
		sendState = IR_TX_HEAD;
	    break;		
      case IR_TX_HEAD:
		if((isLearnKey && (pitCounter == learnData.head)) || (!isLearnKey && (pitCounter == irData.head)))
	    {
		  ir_carrier_disable();
		  pitCounter = 0;
		  sendState = IR_TX_TRANSITION;
	    }
	    break;
	  case IR_TX_TRANSITION:
	    if((isLearnKey && (pitCounter == learnData.transition)) || (!isLearnKey && (pitCounter == irData.transition)))
	    {
		  ir_carrier_enable();
		  pitCounter = 0;
		  bitIndex = 0;
		  sendState = IR_TX_DATA_HIGH;
	    }
	    break;
	  case IR_TX_DATA_HIGH:
	    if(isLearnKey && (pitCounter == learnData.data[bitIndex]))
	    {
		  ir_carrier_disable();
		  pitCounter = 0;
		  bitIndex++;
		  if(learnData.data[bitIndex] == 0)
		    sendState = IR_TX_STOP;
		  else
		    sendState = IR_TX_DATA_LOW;
	    }
		else if(!isLearnKey && (pitCounter == 56))
	    {
		  ir_carrier_disable();
		  pitCounter = 0;
		  sendState = IR_TX_DATA_LOW;
	    }
        break;
	  case IR_TX_DATA_LOW:
	    if(isLearnKey && (pitCounter == learnData.data[bitIndex]))
	    {
		  ir_carrier_enable();
		  pitCounter = 0;
		  bitIndex++;
		  if(learnData.data[bitIndex] == 0)
		    sendState = IR_TX_STOP;
		  else
		    sendState = IR_TX_DATA_HIGH;
	    }
		else if(!isLearnKey && (((((irData.keycode>>(31-bitIndex))&0x0001) == 1) && (pitCounter == 168)) || ((((irData.keycode>>(31-bitIndex))&0x0001) == 0) && (pitCounter == 56))))
	    {
	  	  ir_carrier_enable();
		  pitCounter = 0;
		  bitIndex++;
		  if(bitIndex < 32)
		    sendState = IR_TX_DATA_HIGH;
		  else
		    sendState = IR_TX_STOP;
	    }
        break;
	  case IR_TX_STOP:
#ifdef IR_RECEIVER
		if(isLearnKey || (!isLearnKey && (pitCounter == 56)))
#else
		if(pitCounter == 56)
#endif
		{
	      ir_carrier_disable();
		  ir_tx_timer_disable();
	      ir_tx_timer_int_disable();
		  pitCounter = 0;
		  bitIndex = 0;
		  sendState = IR_TX_IDLE;
		}
	    break;
    		
      default:
	    break;
    }

	PIT_TFLG1 |= PIT_TFLG_TIF_MASK;    
}

void ir_rx_timer_isr(void * param)
{
#if 0 // for rx clock 120ms clk test
  static uint8 bitIndex = 0; 
  // test  add rx time isr clock
  PIT_TFLG2 |= PIT_TFLG_TIF_MASK;
  if(bitIndex == 0) {
    lwgpio_set_value(&ir_pwm_pin,LWGPIO_VALUE_LOW);
  }
  else
    lwgpio_set_value(&ir_pwm_pin,LWGPIO_VALUE_HIGH);
  bitIndex = ~ bitIndex;
  return;
#else
         PIT_TFLG2 |= PIT_TFLG_TIF_MASK;
	ir_rx_timer_disable();
	ir_rx_timer_int_disable();
	recvState = IR_RX_IDLE;
#endif
	
	if((recvData.head > 300) && (recvData.head < 1200) && (recvData.transition > 300) && (recvData.transition < 1200))
	{
	  uint8 i;
	  uint32 recvKey = 0;
		
	  ir_set_received(TRUE);
#if 0
	  if(SIM_SDID >> 28 == 1) //KL1x Family;
        GPIOC_PDOR |= GPIO_PDOR_PDO(GPIO_PIN(9));
      else if(SIM_SDID >> 28 == 3) //KL3x Family;
        GPIOC_PDOR |= GPIO_PDOR_PDO(GPIO_PIN(5));
#endif
	  printf(">>>>recvData.head = %d\n", recvData.head);
	  printf(">>>>recvData.transition = %d\n", recvData.transition);
	  {
		for(i=0; i<MAX_BITINDEX; i++)
		{
		  printf(">>>>recvData.data[%d] = %d\n", i, recvData.data[i]);
		  
		  if((i%2 == 0) || (i > 63))
		    continue;
		  
		  if(recvData.data[i] > 112)
			recvKey |= 1<<(32-(i+1)/2);
		  else
			recvKey &= ~(1<<(32-(i+1)/2));
		}
      }
	  test_recvKey = recvKey; // added for test
	  printf(">>>>recvKey = 0x%x\n", recvKey);
	}

	//PIT_TFLG2 |= PIT_TFLG_TIF_MASK;
}

#define COUNTER_PER_10US    0x257 // 0xEF
void ir_recv_handle(void)
{
  static uint32 startCounter;
  static uint32 tempCounter;
  static uint8 bitIndex = 0;
  
  switch(recvState)
  {
    case IR_RX_IDLE:
	  ir_rx_timer_enable();
	  ir_rx_timer_int_enable();
	  startCounter = PIT_CVAL2;
	  tempCounter = startCounter;
	  recvData.head = 0;
	  recvData.transition = 0;
	  for(bitIndex = 0; bitIndex < MAX_BITINDEX; bitIndex++)
	  	recvData.data[bitIndex] = 0;
	  recvState = IR_RX_HEAD;
	  break;
	case IR_RX_HEAD:
	  if((tempCounter - PIT_CVAL2) < (COUNTER_PER_10US * 10))
	    tempCounter = PIT_CVAL2;
	  else
	  {
	    recvData.head = (startCounter - tempCounter) / COUNTER_PER_10US;
		recvData.transition = (tempCounter - PIT_CVAL2) / COUNTER_PER_10US;
	    startCounter = PIT_CVAL2;
		tempCounter = startCounter;
		bitIndex = 0;
	    recvState = IR_RX_DATA_HIGH;
	  }
	  break;
	case IR_RX_DATA_HIGH:
	  if((tempCounter - PIT_CVAL2) < (COUNTER_PER_10US * 10))
	    tempCounter = PIT_CVAL2;
	  else
	  {
	    recvData.data[bitIndex++] = (startCounter - tempCounter) / COUNTER_PER_10US;
		recvData.data[bitIndex++] = (tempCounter - PIT_CVAL2) / COUNTER_PER_10US;
	    startCounter = PIT_CVAL2;
		tempCounter = startCounter;
	  }
	  break;

	default:
	  break;
  }
}

void ir_recv_pin_isr(void * pin)  /* PTC6 */
{
	// portc6
	if (lwgpio_int_get_flag((LWGPIO_STRUCT_PTR) pin)){
    	if(ir_is_learn_mode() && !ir_is_received())
            ir_recv_handle();
		lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);

	}
	else {
		printf("portc intr 0x%x\n",PORTC_ISFR);
	}
}

uint8 ir_is_learn_mode(void)
{
  return isLearnMode;
}

void ir_set_learn_mode(uint8 learnMode)
{
  isLearnMode = learnMode;
  if(1 == learnMode)
        /*Turn on IR power*/
        ir_power_enable(1);
  else
        /*Turn off IR power*/
        ir_power_enable(0);
  printf(">>>>isLearnMode = %d\n", isLearnMode);
}

uint8 ir_is_received(void)
{
  return isReceived;
}

void ir_set_received(uint8 received)
{
  isReceived = received;
  printf(">>>>isReceived = %d\n", isReceived);
}

#if 0
void ir_led_flicker(void)
{
  static uint32 prev_time;
  
  if(ir_is_learn_mode() && !ir_is_received())
  {
    if((prev_time + 500) < GetCurrentTime())
	{
  	  prev_time = GetCurrentTime();
#if 0 // liufaq
	  if(SIM_SDID >> 28 == 1) //KL1x Family;
        GPIOC_PTOR |= GPIO_PDOR_PDO(GPIO_PIN(9));
      else if(SIM_SDID >> 28 == 3) //KL3x Family;
        GPIOC_PTOR |= GPIO_PDOR_PDO(GPIO_PIN(5));
#endif
    }
  }
}
#endif


int unit_test_ir()
{
	ir_init();

	// send key test
	//while(1) {
	//	printf("send key power\n");
	//ir_sendkey(0);
	//}
	

	// learn recv key test
	// ir_power_enable(1);
	ir_set_learn_mode(1);
	lwgpio_int_enable(&ir_recv_pin, TRUE);

    test_recvKey = 0;
	while(!ir_is_received()){
		_time_delay(500);
        printf("wait key...\n");
	}

	printf("recvived key index %x\n",test_recvKey); /* test key 1 is "e0e020df" ,ok */
    return 0;
}

/* end file */

