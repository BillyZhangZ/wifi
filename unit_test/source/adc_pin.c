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


/**************************************************************************
  Local header files for this application
 **************************************************************************/
#include "adc_pin.h"

//adc
#include "74hc595.h"

#include "pm2p5.h"

// extern from pm2p5.c
extern int adc_calibrate(ADC_MemMapPtr adc_ptr);

// adc
static int adcc_pin_init(int adc_num)
{
	ADC_MemMapPtr adc_ptr = NULL;  /* set adc0/1 register base */

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
    return 0;
}

int32_t adc_pin_measure(int adc_num,int input_ch)
{
	ADC_MemMapPtr adc_ptr = NULL;  /* set adc0/1 register base */
	//
	uint16_t val = 0;

	if(adc_num == 0) {
		/* SIM_SCGC6: ADC0=1 */
		adc_ptr = ADC0_BASE_PTR;
		SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;
	}
	else {
		adc_ptr = ADC1_BASE_PTR;
		SIM_SCGC6 |= SIM_SCGC6_ADC1_MASK;
	
		mux_74hc595_clear_bit(BSP_74HC595_0,   BSP_74HC595_SPI_S0);
        mux_74hc595_set_bit(BSP_74HC595_0,     BSP_74HC595_SPI_S1);
	}

    if((ADC_SC2_REG(adc_ptr) & ADC_SC2_ADACT_MASK) == 0) {
		/* set once conversion mode */
		ADC_SC3_REG(adc_ptr) = (ADC_SC3_REG(adc_ptr) & (~ADC_SC3_ADCO_MASK) | ADCO_SINGLE);
		/* set sw triger */
		ADC_SC2_REG(adc_ptr) = (ADC_SC2_REG(adc_ptr) & (~ADC_SC2_ADTRG_MASK) | ADTRG_SW);

		ADC_SC1_REG(adc_ptr,0) = input_ch;

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

/**************************************************************************
  Global variables
 **************************************************************************/

 /* dac0_enable only enable the clock to DAC0. WIll need this before */
void dac0_clk_enable (void )
{
    SIM_SCGC6 |= SIM_SCGC6_DAC0_MASK ; //Allow clock to enable DAC0
}

void VREF_Init(void)
{
    SIM_SCGC4 |= SIM_SCGC4_VREF_MASK ;
    VREF_SC = 0x81 ;// Enable Vrefo and select internal mode
    //VREF_SC = 0x82; // Tight-regulation mode buffer enabled is reconmended over low buffered mode
    while (!(VREF_SC & VREF_SC_VREFST_MASK)  ){} // wait till the VREFSC is stable
}

int SET_DACx_BUFFER( DAC_MemMapPtr dacx_base_ptr, byte dacindex, int buffval)
{
    int temp = 0 ; 
    
    // initialize all 16 words buffer with a the value buffval
    DAC_DATL_REG(dacx_base_ptr, dacindex)  =   (buffval&0x0ff); 
    DAC_DATH_REG(dacx_base_ptr, dacindex)  =   (buffval&0xf00) >>8;                                
    temp =( DAC_DATL_REG(dacx_base_ptr, dacindex)|( DAC_DATH_REG(dacx_base_ptr, dacindex)<<8)); 
    
    //if(temp != buffval){ while(1){} }
    
    return temp ; 
}// end of SET_DACx_BUFFER

void DACx_register_reset_por_values (DAC_MemMapPtr dacx_base_ptr)
{
    unsigned char dacbuff_index = 0 ;   

    for (dacbuff_index=0; dacbuff_index<16;dacbuff_index++){
        SET_DACx_BUFFER( dacx_base_ptr, dacbuff_index, DACx_DAT_RESET); 
    }
   
     DAC_SR_REG(dacx_base_ptr) = DACx_SR_RESET ;
     DAC_C0_REG(dacx_base_ptr) = DACx_C0_RESET ;
     DAC_C1_REG(dacx_base_ptr) = DACx_C1_RESET;
     DAC_C2_REG(dacx_base_ptr) = DACx_C2_RESET;
} //end of DACx_register_reset_por_values

void DAC12_buffered (DAC_MemMapPtr dacx_base_ptr, byte WatermarkMode, byte BuffMode, byte Vreference, byte TrigMode, byte BuffInitPos,byte BuffUpLimit){

    DAC_C0_REG(dacx_base_ptr) = (
        DAC_BFB_PTR_INT_DISABLE |             
        DAC_BFT_PTR_INT_DISABLE |             
        DAC_BFWM_INT_DISABLE  |             
        DAC_HP_MODE    |             
        DAC_SW_TRIG_STOP |            
        TrigMode |  
        Vreference |     
        DAC_ENABLE    // DAC enalbed
        );  
    
    if ( Vreference == DAC_SEL_VREFO ) {
        VREF_Init();
    }// end of if


    DAC_C1_REG(dacx_base_ptr) = ( 
        DAC_BF_ENABLE  |  //Buffer Enabled            
        WatermarkMode |  // set 1, 2, 3,or 4 word(s) mode           
        BuffMode        // set traversal mode, normal, swing, or onetime   
    ) ;

    DAC_C2_REG(dacx_base_ptr) = BuffInitPos | BuffUpLimit;

}//end of DAC12_buffered 

void DAC12_Buff_Init_Plus256( DAC_MemMapPtr dacx_base_ptr)
{
  
     int data = 0; 
     byte i =  0 ;
     //Uncomment the follows to test for buffer mode
     data = 0 ;  
     // for loop: Initializes the buffer words so that next buffer has an increment of 256 except last one (255)  
     for (i=0 ;i < 16 ; i++){
      
      if(i == 15){//Last buffer.The last word buffer (16th word) will have the value (16*0xff)-1 = 4096-1=4095=0xFFF. 
       data+= 255; 
      }
      else{ //Not last buffer.The next word buffer will have increment of 0xFF from previous word buffer.         
       data+= 256;             
      }  //end of if-else statement
      
      SET_DACx_BUFFER( dacx_base_ptr, i, data);
     
     }// end of for loop
}//end of DAC12_Buff_Init_Plus256

void DAC12_Buff_Init_value( DAC_MemMapPtr dacx_base_ptr,unsigned short val)
{
  
     byte i =  0 ;
     //Uncomment the follows to test for buffer mode
     // for loop: Initializes the buffer words so that next buffer has an increment of 256 except last one (255)  
     for (i=0 ;i < 16 ; i++){
      SET_DACx_BUFFER( dacx_base_ptr, i, val);
     
     }// end of for loop
}//end of DAC12_Buff_Init_Plus256

void DAC12_SoftTrigBuffInit(DAC_MemMapPtr dacx_base_ptr,byte BuffMode, byte Vreference, byte TrigMode, byte BuffInitPos, byte BuffUpLimit)
{ 
    //Initilized DAC12  
	DAC12_buffered( dacx_base_ptr,DAC_BFWM_1WORD, BuffMode, Vreference, TrigMode,BuffInitPos, BuffUpLimit) ;
	DAC12_Buff_Init_Plus256(dacx_base_ptr);//init buffer to with 256 increment with following values word 0(=256), Word 1 (=256+256) .... to word 15 (=4096)

} //end of DAC12_SoftTrigBuff

void DAC12_SoftwareTriggerLoop(void)
{
    int j = 0 ;

    while (1){     
      DAC_C0_REG(DAC0_BASE_PTR) |= DAC_SW_TRIG_NEXT; //SW trigger to advance to DAC0's next word buffer
      //DAC_C0_REG(DAC1_BASE_PTR) |= DAC_SW_TRIG_NEXT; //SW trigger to advance to DAC1's next word buffer
      for (j=0;j<10000;j++){}  // random delay
   }
}

/**************************************************************************
  Global variables
 **************************************************************************/
#define SAMPLE_TIMES   10


// dac 1.2v to adc check value
#define CHECK_VALID_MAX_VAL    24823
#define CHECK_VALID_MIN_VAL    19859

int unit_test_adc_pin(){
	unsigned int  tmp = 0 ,i;

    dac0_clk_enable();
    DACx_register_reset_por_values (DAC0_BASE_PTR); //reset DAC0 value to default reset value;
    DAC12_SoftTrigBuffInit(DAC0_BASE_PTR, DAC_BF_SWING_MODE, DAC_SEL_VREFO,DAC_SEL_SW_TRIG,DAC_SET_PTR_AT_BF(0),DAC_SET_PTR_UP_LIMIT(15));
    DAC12_Buff_Init_value(DAC0_BASE_PTR,4095);   // 1.2v

    // DAC12_SoftwareTriggerLoop();
 
    // ADC0 init    
	adcc_pin_init(0);   

	// start sample adc0_dp0
	// input_ch 0 => pm2.5 => ADC0_DP0/ADC1_DP3(G1) , 
	tmp = 0;
	i = SAMPLE_TIMES;
	do {
		tmp += adc_pin_measure(0,0);
		_time_delay(10);
		i--;
	}
	while (i);
        tmp = tmp/SAMPLE_TIMES;
	printf("ADC0_DP0 =0x%x\n",tmp);
	// check
#if defined(CHECK_VALID_MAX_VAL) && defined(CHECK_VALID_MIN_VAL)
	if ((tmp < CHECK_VALID_MIN_VAL) || (tmp > CHECK_VALID_MAX_VAL)) {
		printf("DAC0,ADC0_DP0 error\n");
		return -1;
	}
#endif

    // start sample adc0_dp0 
    // input_ch 3 => VBAT => ADC1_DP0/ADC0_DP3 (G2)
    // mux_74hc595_clear_bit(BSP_74HC595_0,   BSP_74HC595_SPI_S0);
    // mux_74hc595_set_bit(BSP_74HC595_0,     BSP_74HC595_SPI_S1);
	tmp = 0;
	i = SAMPLE_TIMES;
	do {
		tmp += adc_pin_measure(0,3);
		_time_delay(10);
		i--;
	}
	while (i);
        tmp = tmp/SAMPLE_TIMES;
	printf("ADC0_DP3 =0x%x\n",tmp);
#if defined(CHECK_VALID_MAX_VAL) && defined(CHECK_VALID_MIN_VAL)
	if ((tmp < CHECK_VALID_MIN_VAL) || (tmp > CHECK_VALID_MAX_VAL)) {
		printf("DAC0,ADC0_DP3 error\n");
		return -1;
	}
#endif


    return 0;
}

                        
/* end file */


