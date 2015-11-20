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
#include "lwevent.h"

#include "74hc595.h"

/**************************************************************************
  Local header files for this application
 **************************************************************************/
#include "pm2p5.h"
#include "pm25table.h"
// formular  // v scale is 40 ,density scale is 250
// adc * 3.3 * 124 * 40 / 65535 / 100  => lookup table 
// 

#define PM2P5_LED_PWREN			(GPIO_PORT_C | GPIO_PIN2)  /* LCD_PWREN */
#define PM2P5_LED_MUX_GPIO 	     (LWGPIO_MUX_C2_GPIO)

#define ADC_A_IDX            0
#define ADC_B_IDX            1
#define ADC_AB_IDX           2
#define ADC_INPUT_CH         0 // 3 // pm2.5 => ADC0_DP0/ADC1_DP3(G1) , VBAT => ADC1_DP0/ADC0_DP3 (G2)

//#define ALWAYS_SAMPLING


/**************************************************************************
  Global variables
 **************************************************************************/



/**************************************************************************
  Global variables
 **************************************************************************/
ADC_MemMapPtr adc_ptr = NULL;  /* set adc0/1 register base */
LWGPIO_STRUCT pm2p5_ledgpio;

static void delay_us(int num)
{
  int n;
  for(n=1; n < num; n++) {
	#if (defined(__CWCC__) || defined(__IAR_SYSTEMS_ICC__) || defined(__GNUC__))
	  asm("nop");
	#elif defined (__CC_ARM)
	  __nop();
    #endif
  }
}

/*static*/ int adc_calibrate(ADC_MemMapPtr adc_ptr)
{
    uint16_t cal_var;
    struct {
        uint8_t  SC2;
        uint8_t  SC3;
    } saved_regs;

    if (adc_ptr == NULL)
        return -1; /* no such ADC peripheral exists */

    saved_regs.SC2 = adc_ptr->SC2;
    saved_regs.SC3 = adc_ptr->SC3;

    /* Enable Software Conversion Trigger for Calibration Process */
    adc_ptr->SC2 &= ~ADC_SC2_ADTRG_MASK;

    /* Initiate calibration */
    adc_ptr->SC3 |= ADC_SC3_CAL;

    /* Wait for conversion to complete */
    while (adc_ptr->SC2 & ADC_SC2_ADACT_MASK){}

    /* Check if calibration failed */
    if (adc_ptr->SC3 & ADC_SC3_CALF_MASK)  {
        /* Clear calibration failed flag */
        adc_ptr->SC3 |= ADC_SC3_CALF_MASK;

        /* calibration failed */
        return -1;
    }

    /* Calculate plus-side calibration values according
     * to Calibration function described in reference manual.   */
    /* 1. Initialize (clear) a 16b variable in RAM */
    cal_var  = 0x0000;
    /* 2. Add the following plus-side calibration results CLP0, CLP1,
     *    CLP2, CLP3, CLP4 and CLPS to the variable. */
    cal_var +=  adc_ptr->CLP0;
    cal_var +=  adc_ptr->CLP1;
    cal_var +=  adc_ptr->CLP2;
    cal_var +=  adc_ptr->CLP3;
    cal_var +=  adc_ptr->CLP4;
    cal_var +=  adc_ptr->CLPS;

    /* 3. Divide the variable by two. */
    cal_var = cal_var / 2;

    /* 4. Set the MSB of the variable. */
    cal_var += 0x8000;

    /* 5. Store the value in the plus-side gain calibration registers ADCPGH and ADCPGL. */
    adc_ptr->PG = cal_var;
#if 1 //ADC_HAS_DIFFERENTIAL_CHANNELS
    /* Repeat procedure for the minus-side gain calibration value. */
    /* 1. Initialize (clear) a 16b variable in RAM */
    cal_var  = 0x0000;

    /* 2. Add the following plus-side calibration results CLM0, CLM1,
     *    CLM2, CLM3, CLM4 and CLMS to the variable. */
    cal_var  = 0x0000;
    cal_var += adc_ptr->CLM0;
    cal_var += adc_ptr->CLM1;
    cal_var += adc_ptr->CLM2;
    cal_var += adc_ptr->CLM3;
    cal_var += adc_ptr->CLM4;
    cal_var += adc_ptr->CLMS;

    /* 3. Divide the variable by two. */
    cal_var = cal_var / 2;

    /* 4. Set the MSB of the variable. */
    cal_var += 0x8000;

    /* 5. Store the value in the minus-side gain calibration registers ADCMGH and ADCMGL. */
    adc_ptr->MG = cal_var;
#endif /* ADC_HAS_DIFFERENTIAL_CHANNELS */
    /* Clear CAL bit */
    adc_ptr->SC3 = saved_regs.SC3;
    adc_ptr->SC2 = saved_regs.SC2;

    return 0;
}


int adcc_init(int adc_num)
{
    //PORT_MemMapPtr pctl = NULL;
    //uint8_t gpio_port;

	/* led pin init */
    if(!lwgpio_init(&pm2p5_ledgpio, PM2P5_LED_PWREN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE)) {
        printf("Initializing GPIO with associated pins failed.\n");
        _time_delay (200L);
        _task_block();
    }
    lwgpio_set_functionality(&pm2p5_ledgpio, PM2P5_LED_MUX_GPIO);
    //lwgpio_set_value(&pm2p5_ledgpio, LWGPIO_VALUE_LOW);
    lwgpio_set_value(&pm2p5_ledgpio, LWGPIO_VALUE_HIGH);
    
	if(adc_num == 0) {
		/* SIM_SCGC6: ADC0=1 */
		adc_ptr = ADC0_BASE_PTR;
		SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;
	}
	else {
		adc_ptr = ADC1_BASE_PTR;
		SIM_SCGC6 |= SIM_SCGC6_ADC1_MASK;
	}

#ifdef ADC_HW_TRIGER
    SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;                 /* enable PIT */

    SIM_SOPT7 |= SIM_SOPT7_ADC1ALTTRGEN_MASK;
    SIM_SOPT7 |= (SIM_SOPT7_ADC1PRETRGSEL_MASK);     /* Pre Trigger B */
    SIM_SOPT7 &= ~(SIM_SOPT7_ADC1TRGSEL_MASK); 
    SIM_SOPT7 |= SIM_SOPT7_ADC1TRGSEL(7);            /* PIT trigger 3 */
    PIT_MCR_REG(PIT_BASE_PTR) = 0;
#endif

	// pin mux config : spec P216 ADC0_DP0/ADC1_DP3 is default
	/* set pin's multiplexer to analog
    gpio_port = ADC_SIG_PORTB | 4;
    pctl = (PORT_MemMapPtr) PORTB_BASE_PTR;
    pctl->PCR[gpio_port & 0x1F] &= ~PORT_PCR_MUX_MASK; 
	*/

    // Initialize ADC0/1 =================================================================================
    //averages (4 h/w)   // bus 60M /8/2 = 3.75M ADC clock
    ADC_CFG1_REG(adc_ptr) = ADLPC_NORMAL | ADC_CFG1_ADIV(ADIV_8) /*| ADLSMP_LONG*/ | ADC_CFG1_MODE(MODE_16)
      | ADC_CFG1_ADICLK(/*ADICLK_BUS_2*/ADICLK_BUS);

    // do calibration
    if(adc_calibrate(adc_ptr))
        printf("adc-%x calibrate failed\n",(int)adc_ptr);

    /* channel A/B selected */
#ifdef ADC_HW_TRIGER
    ADC_CFG2_REG(adc_ptr)  = MUXSEL_ADCB | ADACKEN_ENABLED | ADHSC_HISPEED | ADC_CFG2_ADLSTS(ADLSTS_20);
#else
	ADC_CFG2_REG(adc_ptr)  = MUXSEL_ADCA /*| ADACKEN_ENABLED */| ADHSC_HISPEED | ADC_CFG2_ADLSTS(ADLSTS_20);
#endif
	
    //ADC_CV1_REG(adc_ptr) = 0x0;
    //ADC_CV2_REG(adc_ptr) = 0x0;
        
    //ADC_SC1_REG(adc_ptr,0) =  AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH(ADC_INPUT_CH);   /* SC1 A */
    //ADC_SC1_REG(adc_ptr,1) =  AIEN_ON | DIFF_SINGLE | ADC_SC1_ADCH(ADC_INPUT_CH);   /* SC1 B */

#ifdef ADC_HW_TRIGER
    ADC_SC2_REG(adc_ptr) = ADTRG_HW /*ADTRG_SW*/ | ACFE_DISABLED | ACFGT_GREATER | ACREN_DISABLED
                     | DMAEN_DISABLED /* DMAEN_ENABLED*/| ADC_SC2_REFSEL(REFSEL_EXT);
#else
    ADC_SC2_REG(adc_ptr) = ADTRG_SW /*| ACFE_DISABLED | ACFGT_GREATER | ACREN_DISABLED
                     | DMAEN_DISABLED*/ | ADC_SC2_REFSEL(REFSEL_EXT);
#endif

    ADC_SC3_REG(adc_ptr) = /*ADC_SC3_CALF_MASK |*/ CAL_OFF | ADCO_SINGLE /*| AVGE_ENABLED | ADC_SC3_AVGS(AVGS_4)*/;  /* averages 4 h/w */
}

int32_t adc_measure_pm2p5()
{
	//
	uint16_t val = 0;
    if((ADC_SC2_REG(adc_ptr) & ADC_SC2_ADACT_MASK) == 0) {
		/* set once conversion mode */
		ADC_SC3_REG(adc_ptr) = (ADC_SC3_REG(adc_ptr) & (~ADC_SC3_ADCO_MASK) | ADCO_SINGLE);
		/* set sw triger */
		ADC_SC2_REG(adc_ptr) = (ADC_SC2_REG(adc_ptr) & (~ADC_SC2_ADTRG_MASK) | ADTRG_SW);

		ADC_SC1_REG(adc_ptr,0) = ADC_INPUT_CH;

		while(!(ADC_SC1_REG(adc_ptr,0) & ADC_SC1_COCO_MASK)){
			//printf("adc calculating...\n");
			//_time_delay(1);
		}

		val = (uint16_t) (ADC_R_REG(adc_ptr,0));
    }
    else {
      printf("Conversion in progress...\n");
      _time_delay(50);
      val = (uint16_t) (ADC_R_REG(adc_ptr,0));
    }
    return val;
}

#define PIT_CLK                BSP_BUS_CLOCK
//#define START_SAMPLE_FREQ     3571 // 3571.428571428571  
//#define STOP_SAMPLE_FREQ     25000
#define SAMPLE_WIDTH_TIME_US        320
#define START_SAMPLE_TIME_US        280 //40//280
#define STOP_SAMPLE_TIME_US         (SAMPLE_WIDTH_TIME_US - START_SAMPLE_TIME_US)// 40
#define SAMPLE_PERIOD_TIME_MS     9
#define SAMPLE_PERIOD_TIME_US     (1000 - START_SAMPLE_TIME_US - STOP_SAMPLE_TIME_US)

#define WAIT_START_SAMPLE_STATE   0
#define WAIT_STOP_SAMPLE_STATE    1 
#define WAIT_NEXT_START_STATE     2

#define SAMPLE_LED_HIGH     LWGPIO_VALUE_LOW
#define SAMPLE_LED_LOW      LWGPIO_VALUE_HIGH


VQPIT_REG_STRUCT_PTR  sample_qpit_ptr = NULL;
LWEVENT_STRUCT sample_event;
uint32_t sample_state;
int sample_value = 0;

static void SAMPLE_TIMER_ISR(void * p);

static int sample_timer_init()
{
    SIM_SCGC6 |= SIM_SCGC6_PIT_MASK; // Enable PIT Module Clock
    sample_qpit_ptr = (VQPIT_REG_STRUCT_PTR)PIT_BASE_PTR;
    sample_qpit_ptr->MCR = 0/* QPIT_MCR_FRZ*/;
    printf("PIT reg 0x%x\n",PIT_BASE_PTR);

    if(_int_install_isr(SAMPLE_INT, SAMPLE_TIMER_ISR, NULL) == NULL) {
		printf("install sample timer isr failed\n");
        return -1;
    } /* Endif */

	/* create lwevent group */
    if (_lwevent_create(&sample_event, 0) != MQX_OK) { /* 0 - manual clear */
        return -1;
    }

    _bsp_int_init(SAMPLE_INT, 2, 0, TRUE /*FALSE*/);

	return 0;
}

static int sample_timer_set(/*uint32_t pitClk,*/ uint32_t seconds, uint32_t mseconds,uint32_t useconds)
{
    uint32_t  period;
	uint32_t pitClk = PIT_CLK;
	VQPIT_REG_STRUCT_PTR  qpit_ptr = sample_qpit_ptr;

    if((seconds >= (0xffffffff/pitClk)) || (mseconds > 999) || (useconds > 999))
        return -1;

    period = seconds * pitClk;
    period += mseconds * (pitClk/1000);
    period += useconds * (pitClk/1000000);

    /* Disable timer and interrupt */
    qpit_ptr->TIMERS[SAMPLE_PIT_CH].TCTRL = 0;

    qpit_ptr->TIMERS[SAMPLE_PIT_CH].LDVAL = period - 1;

    /* Clear any pending interrupt */
    qpit_ptr->TIMERS[SAMPLE_PIT_CH].TFLG = QPIT_TFLG_TIF;

    /* Enable timer and interrupt , NVIC enable at timer init function */
    qpit_ptr->TIMERS[SAMPLE_PIT_CH].TCTRL = (QPIT_TCTRL_TIE|QPIT_TCTRL_TEN);

   //return period;
    return 0;
}

static int sample_timer_deinit()
{
    VQPIT_REG_STRUCT_PTR  qpit_ptr = sample_qpit_ptr;

    /* Stop the timer */
    qpit_ptr->TIMERS[SAMPLE_PIT_CH].TCTRL &= (~ QPIT_TCTRL_TIE);

    _bsp_int_disable(SAMPLE_INT);

    if(_lwevent_destroy(&sample_event) != MQX_OK) {
        return -1;
    }

    return 0;
}

static void SAMPLE_TIMER_ISR(void * p)
{
    VQPIT_REG_STRUCT_PTR  qpit_ptr = sample_qpit_ptr;
    qpit_ptr->TIMERS[SAMPLE_PIT_CH].TFLG |= QPIT_TFLG_TIF;
    qpit_ptr->TIMERS[SAMPLE_PIT_CH].TCTRL &= (~ QPIT_TCTRL_TIE); // ~(QPIT_TCTRL_TIE|QPIT_TCTRL_TEN)
	// todo
	if(WAIT_START_SAMPLE_STATE == sample_state){
		// set wait sample stop time
            sample_timer_set(0, 0,STOP_SAMPLE_TIME_US);
		sample_state = WAIT_STOP_SAMPLE_STATE;
		// set event ,could sample adc
#ifdef ALWAYS_SAMPLING
	   sample_value = adc_measure_pm2p5();
#else
		_lwevent_set(&sample_event, 1);
#endif
	}
        else if (WAIT_STOP_SAMPLE_STATE == sample_state) {
            sample_timer_set(0, SAMPLE_PERIOD_TIME_MS,SAMPLE_PERIOD_TIME_US);
		sample_state = WAIT_NEXT_START_STATE;
		// set led low
		lwgpio_set_value(&pm2p5_ledgpio, SAMPLE_LED_LOW);
		sample_value = 0;
        } 
	else if(WAIT_NEXT_START_STATE == sample_state) {
        sample_timer_set(0, 0 ,START_SAMPLE_TIME_US);
		sample_state = WAIT_START_SAMPLE_STATE;
		// set led high
		lwgpio_set_value(&pm2p5_ledgpio, SAMPLE_LED_HIGH);

		//_lwevent_set(&sample_event, 1);

	}
	else {
		printf("error sample state!\n");
	}

    /* Enable PIT interrupt neeed due to errata on Kinetis PIT */
    qpit_ptr->TIMERS[SAMPLE_PIT_CH].TCTRL |= QPIT_TCTRL_TIE;
}

void pm2p5_sample_start()
{
    adcc_init(0);
    sample_timer_init();
    // set led high
    lwgpio_set_value(&pm2p5_ledgpio, SAMPLE_LED_HIGH);
    sample_state = WAIT_START_SAMPLE_STATE;
    sample_timer_set(0, 0,START_SAMPLE_TIME_US);
}

float lookup_pm25table(int * input_adcs,int input_count)
{
        int  i, tmp = 0;
        float desity = 0.0;
        for (i = 0 ; i < input_count; i++) {
          tmp += input_adcs[i];
        }
        tmp /= input_count;
        desity = tmp;
        desity =  desity * 3.3 * 124 * 40 / 65535 / 100;
        tmp = desity + PM25_TABLE_ADJUSTING;
        // lookup table
        for(i = 0; i < (PM25_TABLE_SIZE -2); i++) {
          if ((tmp > pm25table[i][0]) &&  (tmp <= pm25table[i+ 1][0]))
            break;
        }
        desity = tmp;
        if(i < (PM25_TABLE_SIZE - 2)) {
          //desity = (desity - pm25table[i][0]) / (float)(pm25table[i + 1 ][0] - pm25table[i][0]) *
          //    (float)(pm25table[i + 1 ][1] - pm25table[i][1]) + (float)pm25table[i][1];
          desity = (desity - pm25table[i][0]);
          desity /= (float)(pm25table[i + 1 ][0] - pm25table[i][0]);
          desity *= (float)(pm25table[i + 1 ][1] - pm25table[i][1]);
          desity +=  (float)pm25table[i][1];
          
          //tmp = desity;
          //printf("%d\n",tmp);
          desity = desity / 250 / 10;
        }
        else {
          printf("pm2.5 exceeding\n");
          if (tmp <= pm25table[0][0])
            tmp = tmp - pm25table[0][1];
          else if(tmp > pm25table[0][PM25_TABLE_SIZE -1])
            tmp = tmp - pm25table[0][PM25_TABLE_SIZE -1];
          else 
            printf("lookup pm 25 table failed\n");
        }
       
        return  desity;
}

int unit_test_pm2p5()
{
    int ret = 0;
    int i = READ_ADC_TIMES;
    int pm_adc[READ_ADC_TIMES] = {0};
    int  tmp;
    int32_t microsec;
    int overflow;
    MQX_TICK_STRUCT start_tick,cur_tick,diff_tick;

	/* for test led clk
	do {
	  _time_delay(500);
	  printf(".\n");
	}while(1);
	*/

#if (ADC_INPUT_CH == 3)
    	adcc_init(0);
        mux_74hc595_clear_bit(BSP_74HC595_0,   BSP_74HC595_SPI_S0);
        mux_74hc595_set_bit(BSP_74HC595_0,     BSP_74HC595_SPI_S1);

    //mux_74hc595_clear_bit(BSP_74HC595_0,   BSP_74HC595_SPI_S1);
    //mux_74hc595_set_bit(BSP_74HC595_0,     BSP_74HC595_SPI_S0);
	do {
		_time_get_elapsed_ticks_fast(&start_tick);
		tmp = adc_measure_pm2p5();
		_time_get_elapsed_ticks_fast(&cur_tick);
		overflow = 0;
		microsec = _time_diff_microseconds(&cur_tick,&start_tick,&overflow);
		printf("read batt = %d, microsecs %d ,overflow %d\n",tmp,microsec,overflow);
	}
	while (1);
#else
   	pm2p5_sample_start();

#ifdef ALWAYS_SAMPLING
       do{
	if(sample_value != 0)
	   printf("read pm2.5 = %d\n",sample_value);
	}
	while(1);
#else
	do {
          _lwevent_wait_ticks(&sample_event, 1, FALSE, 0);
          _lwevent_clear(&sample_event, 1);
          // sampling
	  tmp = adc_measure_pm2p5();
	  pm_adc[READ_ADC_TIMES - i] = tmp;
        } while (i--);
    
        printf("current PM2.5 density = %f mg/m3\n",lookup_pm25table(pm_adc,READ_ADC_TIMES));
#endif
    sample_timer_deinit();
#endif
    return 0;
}

/* end file */

