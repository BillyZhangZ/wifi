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
#include <fio.h>
#include <i2c.h>

#include "adc_pin.h"
#include "glucose_app.h"

#include "BleGlucose_Sensor.h"



 /**************************************************************************
 *
 * MARCOS:
 *
 *************************************************************************/
#define GPIO_PARER_TEST         (GPIO_PORT_A | GPIO_PIN1)   
//#define GPIO_I2C0_INT           (GPIO_PORT_B | GPIO_PIN16)
#define GPIO_I2C0_INT           (GPIO_PORT_C | GPIO_PIN6)
#define GPIO_ASC1               (GPIO_PORT_A | GPIO_PIN2)
#define GPIO_ASC2               (GPIO_PORT_A | GPIO_PIN5)
//#define GPIO_PERI_PWR_CR        (GPIO_PORT_B | GPIO_PIN17)
#define GPIO_PERI_PWR_CR        (GPIO_PORT_C | GPIO_PIN5)


//ADC channels

#define  ADC0_TEMP				(ADC0_SOURCE_AD4B)
#define  ADC0_TEMP_REF			(ADC0_SOURCE_AD15)
#define  ADC0_VOL_M				(ADC0_SOURCE_ADPM0)
#define  ADC1_CUR_M				(ADC1_SOURCE_AD7B)
#define  ADC0_VREF_DEFAULT		(3300)

//temp related
#define  TEMPRREF 				(250)

//glucose related
#define CORRECT_CODE			(0x20)/*this variable should be changed according to test paper*/
#define MEASURE_TYPE_GLUCOSE	(0x01)
#define MEASURE_TYPE_UA			(0x02)
#define MEASURE_TYPE_KET		(0x03)

#define GLUCOSE_COMPEN_VOLT		(400)

typedef struct{
	uint16_t temp;
	uint16_t volt;
}temp_table;

const temp_table tempTab[] = {
{0,1881},{5,1870},{10,1858},{15,1847},{20,1835},{25,1823},{30,1811},{35,1799},
{40,1787},{45,1775},{50,1763},{55,1750},{60,1738},{65,1725},{70,1713},{75,1700},
{80,1687},{85,1674},{90,1661},{95,1648},{100,1635},{105,1622},{110,1608},{115,1595},
{120,1582},{125,1568},{130,1555},{135,1541},{140,1528},{145,1514},{150,1500},
{155,1487},{160,1473},{165,1459},{170,1445},{175,1432},{180,1418},{185,1404},
{190,1390},{195,1376},{200,1363},{205,1349},{210,1336},{215,1321},{220,1308},
{225,1294},{230,1281},{235,1266},{240,1253},{245,1239},{250,1225},{255,1212},
{260,1198},{265,1185},{270,1170},{275,1158},{280,1143},{285,1131},{290,1116},
{295,1105},{300,1090},{305,1079},{310,1066},{315,1053},{320,1041},{325,1028},
{330,1015},{335,1003},{340,991},{345,978},{350,966},{355,954},{360,942},{365,930},
{370,918},{375,906},{380,895},{385,883},{390,872},{395,861},{400,849},{405,838},
{410,827},{415,816},{420,805},{425,795},{430,784},{435,774}
};

#define SM_I2C_DEVICE_POLLED "i2c0:"  
#define HT66F_ADDR			(0xE0>>1) //slave address, any value?
#define HT66F_WORD_ADDR		(0x00) //word address, any value?

 /**************************************************************************
 *
 * VARIABLES:
 *
 *************************************************************************/
MQX_FILE_PTR i2c0_dev;
LWSEM_STRUCT paper_int_sem, notify_ble;
LWGPIO_STRUCT  TEST_PAPER_PIN, PERI_PWR_CR, ASC1_PIN, ASC2_PIN, I2C_INIT;
 

uint16_t g_sample_value[5]; //ADC samples
uint16_t compare_2s_value; //Read ADC at 2S
uint16_t compare_5s_value; //Read ADC at 5S

//data to send to HT66F
uint8_t g_data_to_HT66F[10];
uint8_t g_data_from_HT66F[10];

static uint8_t index_record;
#define MAX_NUMBER_OF_RECORDS                                    100
extern BleGlucoseServiceMeasurement measurementRecords[MAX_NUMBER_OF_RECORDS];;
 
  /**************************************************************************
 *
 * INTERNAL FUNCTIONS:
 *
 *************************************************************************/

static void dac_init(uint32_t mV);
static void power_peri(uint8_t enable);
static void output_volt_asc1(uint8_t enable);
static void output_volt_asc2(uint8_t enable);
static void wakeup_HT66F(void);
static void sleep_HT66F(void);
static void init_HT66F(void);
static void gpio_init(void);
static void	paper_int_handler(void* pin);
static void taskAdcSample(uint32_t para);
static uint16_t monitorTempValue(void);
static uint32_t monitorCurValue(void);
static uint32_t caclu_final_glucose(uint8_t test_type, uint8_t correct_code, int16_t temperature, uint16_t glu_current_value);


static uint32_t get_mqx_sys_time(BleDateTime* time);
 /**************************************************************************
 *
 * FUNCTIONS:
 *
 *************************************************************************/

/*1.create sample task
  2.Init DAC,GPIO,I2C
  3.AD
  4.FLASH
  5.IRQ handler
  6....BLE_APP_Init???
*/
_mqx_int glucose_init(void)
{

	_task_id	task_id;
	TASK_TEMPLATE_STRUCT	task_template;
	_mqx_uint priority;
	uint32_t para;
	/*init DAC,output 400mV, glucose mode*/
	para = 400;
	dac_init(para);
	/*GPIO*/
	gpio_init();

	/*create adc sample task*/
	task_template.TASK_TEMPLATE_INDEX = 0;
	task_template.TASK_ADDRESS = taskAdcSample;
	task_template.TASK_STACKSIZE = 1000L;
	_task_get_priority(_task_get_id_from_name("main"), &priority);
	task_template.TASK_PRIORITY = priority - 1;
	task_template.TASK_NAME = "ADC sample";
	task_template.TASK_ATTRIBUTES = 0;
	task_template.CREATION_PARAMETER = 0;
	task_template.DEFAULT_TIME_SLICE = 0;
	task_id = _task_create_blocked(0, 0, (uint32)&task_template);
	if(task_id == 0)
	{
		return MQX_ERROR;
	}
	_task_ready(_task_get_td(task_id));

#if 0
	/*flash*/
	flash_file = fopen("flashx:bank1", NULL);
	if(!flash_file)
	{
		return MQX_ERROR;
	}
#endif
	/*need to write init values into the flash*/
	//>--

	//<--
	return MQX_OK;
}


static void dac0_clk_enable (void )
{
    SIM_SCGC6 |= SIM_SCGC6_DAC0_MASK ; //Allow clock to enable DAC0
}

static void VREF_Init(void)
{
    SIM_SCGC4 |= SIM_SCGC4_VREF_MASK ;
    VREF_SC = 0x81 ;// Enable Vrefo and select internal mode
    //VREF_SC = 0x82; // Tight-regulation mode buffer enabled is reconmended over low buffered mode
    while (!(VREF_SC & VREF_SC_VREFST_MASK)  ){} // wait till the VREFSC is stable
}

static int SET_DACx_BUFFER( DAC_MemMapPtr dacx_base_ptr, byte dacindex, int buffval)
{
    int temp = 0 ; 
    // initialize all 16 words buffer with a the value buffval
    DAC_DATL_REG(dacx_base_ptr, dacindex)  =   (buffval&0x0ff); 
    DAC_DATH_REG(dacx_base_ptr, dacindex)  =   (buffval&0xf00) >>8;                                
    temp =( DAC_DATL_REG(dacx_base_ptr, dacindex)|( DAC_DATH_REG(dacx_base_ptr, dacindex)<<8)); 
    //if(temp != buffval){ while(1){} }
    return temp ; 
}

static void DACx_register_reset_por_values (DAC_MemMapPtr dacx_base_ptr)
{
    unsigned char dacbuff_index = 0 ;   

    for (dacbuff_index=0; dacbuff_index<16;dacbuff_index++){
        SET_DACx_BUFFER( dacx_base_ptr, dacbuff_index, DACx_DAT_RESET); 
    }   
     DAC_SR_REG(dacx_base_ptr) = DACx_SR_RESET ;
     DAC_C0_REG(dacx_base_ptr) = DACx_C0_RESET ;
     DAC_C1_REG(dacx_base_ptr) = DACx_C1_RESET;
     DAC_C2_REG(dacx_base_ptr) = DACx_C2_RESET;
} 

static void DAC12_buffered (DAC_MemMapPtr dacx_base_ptr, byte WatermarkMode, byte BuffMode, byte Vreference, byte TrigMode, byte BuffInitPos,byte BuffUpLimit){

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
}

static void DAC12_Buff_Init_Plus256( DAC_MemMapPtr dacx_base_ptr)
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

static void DAC12_Buff_Init_value( DAC_MemMapPtr dacx_base_ptr,unsigned short val)
{
  
     byte i =  0 ;
     //Uncomment the follows to test for buffer mode
     // for loop: Initializes the buffer words so that next buffer has an increment of 256 except last one (255)  
     for (i=0 ;i < 16 ; i++){
      SET_DACx_BUFFER( dacx_base_ptr, i, val);
     }
}

static void DAC12_SoftTrigBuffInit(DAC_MemMapPtr dacx_base_ptr,byte BuffMode, byte Vreference, byte TrigMode, byte BuffInitPos, byte BuffUpLimit)
{ 
    //Initilized DAC12  
	DAC12_buffered( dacx_base_ptr,DAC_BFWM_1WORD, BuffMode, Vreference, TrigMode,BuffInitPos, BuffUpLimit) ;
	DAC12_Buff_Init_Plus256(dacx_base_ptr);//init buffer to with 256 increment with following values word 0(=256), Word 1 (=256+256) .... to word 15 (=4096)
} 

static void dac_init(uint32_t mV)
{
	dac0_clk_enable();
	DACx_register_reset_por_values (DAC0_BASE_PTR); //reset DAC0 value to default reset value;
	DAC12_SoftTrigBuffInit(DAC0_BASE_PTR, DAC_BF_SWING_MODE, DAC_SEL_VREFO,DAC_SEL_SW_TRIG,DAC_SET_PTR_AT_BF(0),DAC_SET_PTR_UP_LIMIT(15));
	//DAC12_Buff_Init_value(DAC0_BASE_PTR,4095);    //1.2V ref
	DAC12_Buff_Init_value(DAC0_BASE_PTR, mV*4096/1200);
}

static void gpio_init(void)
{
	/*peri_pwr_cr, set low to enable VDD1*/
    lwgpio_init(&PERI_PWR_CR, GPIO_PERI_PWR_CR, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(&PERI_PWR_CR, 1);
    lwgpio_set_value(&PERI_PWR_CR, LWGPIO_VALUE_LOW);/* set pin to 0 */

    /*ASC1*/
    lwgpio_init(&ASC1_PIN, GPIO_ASC1, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(&ASC1_PIN, 1);
    lwgpio_set_value(&ASC1_PIN, LWGPIO_VALUE_HIGH); /* set pin to 1 */

    /*ASC2*/
    lwgpio_init(&ASC2_PIN, GPIO_ASC2, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(&ASC2_PIN, 1);
    lwgpio_set_value(&ASC2_PIN, LWGPIO_VALUE_HIGH);/* set pin to 1 */

    /*I2C_INIT*/
    lwgpio_init(&I2C_INIT, GPIO_I2C0_INT, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(&I2C_INIT, 1);
    lwgpio_set_value(&I2C_INIT, LWGPIO_VALUE_HIGH); /*set int pin to 0, sleep HT669*/

}

static void power_peri(uint8_t enable)
{
	if(enable)
	{
		lwgpio_set_value(&ASC1_PIN, LWGPIO_VALUE_LOW);
	}
	else
	{
		lwgpio_set_value(&ASC1_PIN, LWGPIO_VALUE_HIGH);
	}
}

/*ASC1 pin: low to enable; high to disable*/
static void output_volt_asc1(uint8_t enable)
{
	if(enable)
	{
		lwgpio_set_value(&ASC1_PIN, LWGPIO_VALUE_LOW);
	}
	else
	{
		lwgpio_set_value(&ASC1_PIN, LWGPIO_VALUE_HIGH);
	}
}


/*ASC2 pin*/
static void output_volt_asc2(uint8_t enable)
{
	if(enable)
	{
		lwgpio_set_value(&ASC2_PIN, LWGPIO_VALUE_LOW);
	}
	else
	{
		lwgpio_set_value(&ASC2_PIN, LWGPIO_VALUE_HIGH);
	}
}


static void wakeup_HT66F(void)
{
	 lwgpio_set_value(&I2C_INIT, LWGPIO_VALUE_LOW);
}

static void sleep_HT66F(void)
{
	 lwgpio_set_value(&I2C_INIT, LWGPIO_VALUE_HIGH);
}

static void init_HT66F(void)
{

	uint32_t param;
	i2c0_dev = fopen(SM_I2C_DEVICE_POLLED, NULL);
	if(i2c0_dev == NULL)
		return;
    param = 7000;
    if (I2C_OK != ioctl (i2c0_dev, IO_IOCTL_I2C_SET_BAUD, &param)) {
    	return;
    }
    if (I2C_OK != ioctl (i2c0_dev, IO_IOCTL_I2C_SET_MASTER_MODE, NULL)) {
    	return;
    }
    if (I2C_OK != ioctl (i2c0_dev, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL)) {
    	return;
    }
    param = HT66F_ADDR;
    if (I2C_OK != ioctl (i2c0_dev, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param)) {
        return;
    }


}
static void	paper_int_handler(void* pin)
{
	lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR)pin);
	_lwsem_post(&paper_int_sem);
}

static uint16_t cacluTemp(uint32_t tempValue)
{
	uint32_t index;
	uint32_t len = sizeof(tempTab)/sizeof(tempTab[0]);
	if(tempValue >= tempTab[0].volt) return tempTab[0].temp;
	if(tempValue <= tempTab[len-1].volt) return tempTab[len-1].temp;
	for(index = 0; index < len; index++)
	{
		if(tempValue >= tempTab[index].volt)
			break;
	}
	return tempTab[index].temp;
}

static uint16_t monitorTempValue(void)
{
	LWADC_STRUCT TempRef, TempChannel;
	LWADC_VALUE  TempValue;
	int16_t	 compenValue;
	uint16_t tempValueDegree;

	/*calibrate TEMP_REF channel*/
	_lwadc_init_input(&TempRef, ADC0_TEMP_REF);
	_lwadc_set_attribute(&TempRef, LWADC_NUMERATOR, 3300);
    _lwadc_read_average(&TempRef, 10, &TempValue);

    /*get the compensation value*/
    compenValue = TEMPRREF - cacluTemp(TempValue);

    /*temp value*/
	_lwadc_init_input(&TempChannel, ADC0_TEMP);
    _lwadc_read_average(&TempChannel, 10, &TempValue);
    tempValueDegree = cacluTemp(TempValue) + compenValue;
    return tempValueDegree;

}

static uint8_t convertTempValue(uint16 temp_value)
{
	uint8_t sign_flag = 0;
	uint8_t temp_result;
	if(temp_value % 10 == 5){
		sign_flag = 1;
	}
	if(temp_value){
		temp_value = temp_value/10;
	}
	if(sign_flag)
		temp_result = (0x80 | temp_value);
	else
		temp_result = (uint8_t)temp_value;
	return temp_result;

}

static uint32_t measure_current(uint32_t channel, uint32_t sample_time)
{
	LWADC_STRUCT adc_struct;
	LWADC_VALUE	 value;
	_lwadc_init_input(&adc_struct, channel);
	_lwadc_read_average(&adc_struct, sample_time, &value);
	return value;
}
static uint32_t monitorCurValue(void)
{
	uint32_t max_value;
	output_volt_asc1(1);
	_time_delay(1000);
	output_volt_asc1(1);
	output_volt_asc2(1);//ASC1 & ASC2 for glucose; ASC1 for others
	uint32_t i, sum = 0;
	/*1s*/
	for(i = 0; i < 2; i++)
	{
		_time_delay(500);
		max_value = measure_current(ADC1_CUR_M, 10);
		if(max_value > g_sample_value[4])
			g_sample_value[4] = max_value;
	}
	/*2s*/
	compare_2s_value = max_value;
	/*2.5~4.5s*/
	for(i = 0; i < 5; i++)
	{
		_time_delay(500);
		max_value = measure_current(ADC1_CUR_M, 10);
		if(max_value > g_sample_value[4])
			g_sample_value[4] = max_value;
	}

	/*4.625s~5s*/
	for(i = 0; i < 4; i++)
	{
		_time_delay(125);
		g_sample_value[i] = measure_current(ADC1_CUR_M, 10);
		sum += g_sample_value[i];
	}
	/*5s*/
	compare_5s_value =  measure_current(ADC1_CUR_M, 10);

	return (sum>>2);
}

static uint32_t caclu_final_glucose(uint8_t test_type, uint8_t correct_code, int16_t temperature, uint16_t glu_current_value)
{
	uint8_t para, result;
	g_data_to_HT66F[0] = HT66F_WORD_ADDR;
	g_data_to_HT66F[1] = test_type;
	g_data_to_HT66F[2] = correct_code;
	g_data_to_HT66F[3] = (uint8_t)temperature;
	g_data_to_HT66F[4] = glu_current_value>>8;
	g_data_to_HT66F[5] = glu_current_value&0xFF;
	wakeup_HT66F();
	_time_delay(100);

	para = HT66F_ADDR;
	if(I2C_OK != ioctl(i2c0_dev, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &para))
		return MQX_ERROR;
	//send data here
	result = fwrite(g_data_to_HT66F, 1, 6, i2c0_dev );
	if(result)
		result = fflush(i2c0_dev);
    ioctl (i2c0_dev, IO_IOCTL_I2C_STOP, NULL);
    _time_delay(100);
    para = 2;
	if (I2C_OK != ioctl (i2c0_dev, IO_IOCTL_I2C_SET_RX_REQUEST, &para))
	{
		return MQX_ERROR;
	}
	result = fread(g_data_from_HT66F, 1, 2, i2c0_dev);
	result = fflush(i2c0_dev);
#if 0
    if(I2C_OK == ioctl (i2c0_dev, IO_IOCTL_I2C_STOP, NULL))
    {
    	 return MQX_ERROR;
    }
#endif
    /*error handling*/
#if 0
    while(g_data_from_HT66F[0] == 0xFF)
    {
    	//

    }
#endif
    sleep_HT66F();
    //power down HT66F
    power_peri(0);
    power_peri(1);
}


static uint32_t get_mqx_sys_time(BleDateTime* time)
{
#if 0
	uint32_t rtc_time;
	TIME_STRUCT ts;
	DATE_STRUCT tm;
	_rtc_get_time(&rtc_time);
	ts.SECONDS = rtc_time;
	ts.MILLISECONDS = 0;
	if(_time_to_date(&ts, &tm) == FALSE)
		return MQX_ERROR;
	(*time).year = tm.YEAR;
	(*time).month = tm.MONTH;
	(*time).day = tm.DAY;
	(*time).hours = tm.HOUR;
	(*time).minutes = tm.MINUTE;
	(*time).seconds = tm.SECOND;
#endif
#if 1
	(*time).year = 2015;
	(*time).month = 1;
	(*time).day = 16;
	(*time).hours = 10;
	(*time).minutes = 26;
	(*time).seconds = 50;
#endif
	return MQX_OK;

}

static void taskAdcSample(uint32_t para)
{
	uint16_t temp_value;
	uint32_t glucose_value;
	BleDateTime current_time;
	index_record = 0;
	/*init i2C HT66F chip*/
	init_HT66F();
	/*test paper GPIO6 in QCA, should be falling edge interupt*/
    lwgpio_init(&TEST_PAPER_PIN, GPIO_PARER_TEST, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(&TEST_PAPER_PIN, 1);
    lwgpio_set_attribute(&TEST_PAPER_PIN, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);//???
    lwgpio_int_init(&TEST_PAPER_PIN, LWGPIO_INT_MODE_FALLING);
    _int_install_isr(lwgpio_int_get_vector(&TEST_PAPER_PIN), paper_int_handler, &TEST_PAPER_PIN);
    _bsp_int_init(lwgpio_int_get_vector(&TEST_PAPER_PIN), 4, 0, TRUE);
    lwgpio_int_enable(&TEST_PAPER_PIN, TRUE);
    _lwsem_create(&paper_int_sem, 0);	
	_lwsem_create(&notify_ble, 0);
	for(;;)
	{
		_lwsem_wait(&paper_int_sem);
		temp_value = monitorTempValue();
		glucose_value = monitorCurValue();
		glucose_value = ((glucose_value - GLUCOSE_COMPEN_VOLT)*3)>>1;
		caclu_final_glucose(MEASURE_TYPE_GLUCOSE, CORRECT_CODE, convertTempValue(200), 800);
		get_mqx_sys_time(&current_time);
		measurementRecords[index_record++].flagsValue.glucoseMeasurementFlagsValue = 0x1F;
		measurementRecords[index_record++].sequenceNumberValue++;
		measurementRecords[index_record++]. baseTimeValue = current_time;
		measurementRecords[index_record++].timeOffsetValue = 10;
		measurementRecords[index_record++].concentrationValue = (uint16_t)(g_data_from_HT66F[0] | g_data_from_HT66F[1]<<8);
		measurementRecords[index_record++].sampleTypeValue = 1; //mol/L
		measurementRecords[index_record++].sampleLocationValue = 1;//finger
		BLEAPP_SetValue(0);

	}


}


/* end file */

