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

#include <spi.h>


/**************************************************************************
  Local header files for this application
 **************************************************************************/
#include "em9301.h"
#include "74hc595.h"
#include "spi_dspi_common.h"

/**************************************************************************
  Macros for this application
 **************************************************************************/ 
#define BSPCFG_ENABLE_SPI_STATS					  1 
#define PRE_CONFIGURE_FLAG                      0x01 
#define EM9301_COM_CHANNEL						"spi1:"
#define BSP_EM9301_MOSI_PIN						(GPIO_PORT_D | GPIO_PIN6)
#define BSP_EM9301_IRQ_PIN						(GPIO_PORT_E | GPIO_PIN1)
#define BSP_EM9301_IRQ_MUX_IRQ                  LWGPIO_MUX_E1_GPIO
#define HCI_BUF_SIZE                            50

/**************************************************************************
  Local variables
 **************************************************************************/
static uint32_t packetStore[HCI_BUF_SIZE>>2];
static uint8_t* packet = (uint8_t*)packetStore;

/**************************************************************************
  Local Functions declaration
 **************************************************************************/
static MQX_FILE_PTR ble_spi_init(char* identify);
static _mqx_int     ble_spi_deinit(MQX_FILE_PTR);
static void taskRead(_mqx_uint para);
static void	irq_int_handler(void* pin);
static _mqx_int read_cs_callback(uint32_t pre_cfg_mask, void *user_data);
static _mqx_int write_cs_callback(uint32_t pre_cfg_mask, void *user_data);
static _mqx_int wait_command_complete_evt(uint16_t cmd);

//HCI command APIs
static void btResetCommand(MQX_FILE_PTR dev);
static void btLESetEventMask(MQX_FILE_PTR dev);
static void btLEAdvertiseParams(MQX_FILE_PTR dev);
static void btEmSetPublicAddress(MQX_FILE_PTR dev);
static void btLESetAdvertisingPacket(MQX_FILE_PTR dev);
static void btLESetAdvertisEnable(MQX_FILE_PTR dev);

/**************************************************************************
  Global variables
 **************************************************************************/
LWSEM_STRUCT    irq_lwsem;
LWGPIO_STRUCT   SPI_IRQ_PIN;
uint8_t hciBuffer[HCI_BUF_SIZE];
SPI_CS_CALLBACK_STRUCT	callback;

 /**************************************************************************
   functions
 **************************************************************************/
 int unit_test_em9301(void)
 {

    MQX_FILE_PTR spi_device = ble_spi_init(EM9301_COM_CHANNEL);
    if(spi_device == NULL)
    {
        printf("Ble device init failed.\n");
        _task_block();
    }
    _time_delay(100);
    while(hciBuffer[0] == 0x00);
    if(hciBuffer[0] == 0x04 && hciBuffer[1]== 0xFF \
       && hciBuffer[2] == 0x01 && hciBuffer[3] == 0x01)
    {
        /*reset controller*/
        btResetCommand(spi_device);
        while(MQX_OK != wait_command_complete_evt(0x030C));
        
        /*set LE event mask*/
        btLESetEventMask(spi_device);
        while(MQX_OK != wait_command_complete_evt(0x0120));
        
        /*set public address*/
        btEmSetPublicAddress(spi_device);
        while(MQX_OK != wait_command_complete_evt(0x02FC));
        
        /*set Adv parameters*/
        btLEAdvertiseParams(spi_device);
        while(MQX_OK != wait_command_complete_evt(0x0620));
        
        /*send Adv data*/
        btLESetAdvertisingPacket(spi_device);
        while(MQX_OK != wait_command_complete_evt(0x0820));

        /*set Advertise enable*/
        btLESetAdvertisEnable(spi_device);
        while(MQX_OK != wait_command_complete_evt(0x0A20));
    
        return 0;
        
#if 0
        while(hciBuffer[1] == 0xFF);
        if(hciBuffer[4] == 0x03 && hciBuffer[5] == 0x0C && hciBuffer[6] == 0x00)
        {
            if (MQX_OK  == ble_spi_deinit(spi_device)) 
                return 0;
        }                   
#endif
        
    }
    _task_block();
 }



/**************************************************************************
  Local Functions 
 **************************************************************************/                        
static MQX_FILE_PTR ble_spi_init(char* identify)
{
    _task_id read_task_id;
    TASK_TEMPLATE_STRUCT	task_template;
	_mqx_uint priority;
    MQX_FILE_PTR spi_dev;
    _mqx_uint para = 0x00;
    
    spi_dev = fopen(identify, NULL);	   	
	ioctl (spi_dev, IO_IOCTL_SPI_SET_TRANSFER_MODE, &para);	
	ioctl (spi_dev, IO_IOCTL_SPI_SET_ATTRIBUTES, &para);
	ioctl (spi_dev, IO_IOCTL_SPI_SET_DUMMY_PATTERN, &para);
    para = 1000000;
	ioctl (spi_dev, IO_IOCTL_SPI_SET_BAUD, &para);
    task_template.TASK_TEMPLATE_INDEX = 0;
	task_template.TASK_ADDRESS = taskRead;
	task_template.TASK_STACKSIZE = 1000L;
	_task_get_priority(_task_get_id_from_name("main"), &priority);
	task_template.TASK_PRIORITY = priority - 1;
	task_template.TASK_NAME = "SPI_READ";
	task_template.TASK_ATTRIBUTES = 0;
	task_template.CREATION_PARAMETER = (uint32)spi_dev;
	task_template.DEFAULT_TIME_SLICE = 0;
	read_task_id = _task_create_blocked(0, 0, (uint32)&task_template);
	if(read_task_id == 0)
	{
		return NULL;
	}
    _task_ready(_task_get_td(read_task_id));

    return spi_dev;
}


static _mqx_int ble_spi_deinit(MQX_FILE_PTR identify)
{
    if(MQX_OK != fclose(identify)){
		printf("Unable to close communication channel.\n");
		_task_block();
	}
    if(MQX_OK != mux_74hc595_clear_bit(BSP_74HC595_0, BSP_74HC595_VBLE_3V3))
    {
        printf("VBLE_3V3 set bit failed.\n");
        _task_block();
    }

    _bsp_int_disable(lwgpio_int_get_vector(&SPI_IRQ_PIN));
    _int_install_isr(lwgpio_int_get_vector(&SPI_IRQ_PIN), _int_get_default_isr(), NULL);
    if(MQX_OK != _task_destroy(_task_get_id_from_name("SPI_READ")))
    {
        printf("Destroy read task failed.\n");
        _task_block();    
    }
    if(MQX_OK != _lwsem_destroy(&irq_lwsem))
    {
        printf("Sem destroy failed.\n");
        _task_block();
    }
    return MQX_OK;
}

static void taskRead(_mqx_uint para)
{
    MQX_FILE_PTR dev_file = (MQX_FILE_PTR)para;
    SPI_CS_CALLBACK_STRUCT callback;
    uint8_t tmp, *buf;
    
    _lwsem_create(&irq_lwsem, 0);
    mux_74hc595_set_bit(BSP_74HC595_0, BSP_74HC595_VBLE_3V3);
    mux_74hc595_clear_bit(BSP_74HC595_0, BSP_74HC595_SPI_S0);
	mux_74hc595_clear_bit(BSP_74HC595_0, BSP_74HC595_SPI_S1);
    /*RST pin configuration, last high for at least 1ms*/
	mux_74hc595_set_bit(BSP_74HC595_0, BSP_74HC595_BLE_RST);
	_time_delay(1);
	mux_74hc595_clear_bit(BSP_74HC595_0, BSP_74HC595_BLE_RST);
    
    lwgpio_init(&SPI_IRQ_PIN, BSP_EM9301_IRQ_PIN, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(&SPI_IRQ_PIN, BSP_EM9301_IRQ_MUX_IRQ);
    lwgpio_set_attribute(&SPI_IRQ_PIN, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
    lwgpio_int_init(&SPI_IRQ_PIN, LWGPIO_INT_MODE_RISING);
    _int_install_isr(lwgpio_int_get_vector(&SPI_IRQ_PIN), irq_int_handler, &SPI_IRQ_PIN);
    _bsp_int_init(lwgpio_int_get_vector(&SPI_IRQ_PIN), BSP_DSPI_INT_LEVEL, 0, TRUE);
    lwgpio_int_enable(&SPI_IRQ_PIN, TRUE);
    
    for(;;)
    {
        _lwsem_wait(&irq_lwsem);
        callback.CALLBACK = read_cs_callback;
		callback.USERDATA = dev_file;
        ioctl(dev_file, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback);
		buf = hciBuffer;
        while(lwgpio_get_value(&SPI_IRQ_PIN))
		{
			if(IO_ERROR != fread(&tmp, 1, 1,dev_file))
				*buf++ = tmp;
		}
		fflush(dev_file);    
    
    }

}

static _mqx_int read_cs_callback(uint32_t pre_cfg_mask, void *user_data)
{		
    //uint8_t byteReceievd;
	//MQX_FILE_PTR    dev = (MQX_FILE_PTR)user_data; 
	VDSPI_REG_STRUCT_PTR    dspi_ptr;  
	LWGPIO_STRUCT   em9301_spi_mosi;
	if(pre_cfg_mask & PRE_CONFIGURE_FLAG)
	{
		dspi_ptr = _bsp_get_dspi_base_address(1);    
		dspi_ptr->MCR = (dspi_ptr->MCR & ~(uint32_t)DSPI_MCR_PCSIS_MASK) | DSPI_MCR_PCSIS(0xFF);
		lwgpio_init(&em9301_spi_mosi, BSP_EM9301_MOSI_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);
		lwgpio_set_functionality(&em9301_spi_mosi, 1);
		lwgpio_set_value(&em9301_spi_mosi, 0);		
		/*Assert CS pin*/ 	
		dspi_ptr->MCR |= DSPI_MCR_MSTR_MASK;	
		dspi_ptr->MCR = (dspi_ptr->MCR & ~(uint32_t)DSPI_MCR_PCSIS_MASK) | DSPI_MCR_PCSIS(~0x01);
		lwgpio_set_functionality(&em9301_spi_mosi, 7);	
	}
    return MQX_OK;
}

static _mqx_int write_cs_callback(uint32_t pre_cfg_mask, void *user_data)
{
	VDSPI_REG_STRUCT_PTR    dspi_ptr;
	LWGPIO_STRUCT  em9301_spi_mosi;
	if(pre_cfg_mask & PRE_CONFIGURE_FLAG)
	{
		dspi_ptr = _bsp_get_dspi_base_address(1);
		dspi_ptr->MCR = (dspi_ptr->MCR & ~(uint32_t)DSPI_MCR_PCSIS_MASK) | DSPI_MCR_PCSIS(0xFF);
		/*MOSI set pin*/
		lwgpio_init(&em9301_spi_mosi, BSP_EM9301_MOSI_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);
		lwgpio_set_functionality(&em9301_spi_mosi, 1);
		lwgpio_set_value(&em9301_spi_mosi, 1);
		/*Assert CS pin*/
		dspi_ptr->MCR = (dspi_ptr->MCR & ~(uint32_t)DSPI_MCR_PCSIS_MASK) | DSPI_MCR_PCSIS(~0x01);
		lwgpio_set_functionality(&em9301_spi_mosi, 7);
	}	
	return MQX_OK;
}

static void	irq_int_handler(void* pin)
{
	lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR)pin);
	_lwsem_post(&irq_lwsem);
}

static _mqx_int wait_command_complete_evt(uint16 cmd)
{
  if(hciBuffer[1] == 0x0E && hciBuffer[4] == (cmd >> 8) && hciBuffer[5] == (cmd & 0xFF))
    return MQX_OK;
  else
    return MQX_ERROR;
}


/***************************************
**Definition for HCI cmd wrapper and sent
APIs here, not a complete HCI operation
set,just for our need to verify EM9301 
HCI functions***************************
****************************************/

/*****************HCI*******************/
#define HCI_OGF_Controller_and_Baseband 3
  #define HCI_CMD_Reset   0x0003

#define HCI_LE_OGF  0x08
  #define HCI_LE_Set_Event_Mask     0x0001
  #define HCI_LE_Read_Buffer_Size   0x0002
  #define HCI_LE_Set_Random_Address 0x0005
  #define HCI_LE_Set_Advertising_Parameters 0x0006
  #define HCI_LE_Set_Advertising_Data   0x0008
  #define HCI_LE_Set_Advertise_Enable   0x000A

#define EM_VENDOR_BASE_OGF   0x3F
  #define EM_SET_PUBLIC_ADDRESS 0x0002
  #define EM_SET_POWER_MODE   0x0003
  #define EM_SVLD             0x0004
  #define EM_SET_RF_POWER_LEVEL   0x0005
  #define EM_POWER_MODE_CONFIGURATION 0x0006
  #define EM_SET_UART_BAUD_RATE   0x0007
  #define EM_SET_DCDC_VOLTAGE     0x0008


#define HCI_OPCODE(ogf, ocf)	((((unsigned short)(ogf)) << 10) | (ocf))

typedef struct
{
	unsigned short	opcode;
	unsigned char	totalParamLen;
	unsigned char	params[];

}HCI_Cmd;

static uint8_t* hciCmdPacketStart(uint16_t ogf, uint16_t ocf)
{		
    HCI_Cmd* cmd = (HCI_Cmd*)packetStore;
    cmd->opcode = HCI_OPCODE(ogf, ocf);
    return packet + 3;
}
static uint8_t* hciCmdPacketAddU8(uint8_t* state, uint8_t val)
{
    *state++ = val;
    return state;
}
static uint8_t* hciCmdPacketAddU16(uint8_t* state, uint16_t val)
{
    *state++ = val;
    *state++ = val >> 8;
    return state;
}
static uint32_t hciCmdPacketFinish(uint8_t* state)
{		
    HCI_Cmd* cmd = (HCI_Cmd*)packetStore;
    uint32_t paramLen = state - packet - 3;
    cmd->totalParamLen = paramLen;
    return paramLen + 3;
}

static void btTxCmdPacket(const HCI_Cmd* cmd, MQX_FILE_PTR spi_dev)
{
    uint8_t typ = 0x01;
    fwrite(&typ, 1, 1, spi_dev);
    fwrite((uint8_t*)cmd, 1, 3 + cmd->totalParamLen, spi_dev);   
#if 0
    {
    	uint32_t i;
    	printf("Send CMD Len %d:", cmd->totalParamLen);
    	for(i = 0; i< 3+ cmd->totalParamLen;i++)
    			printf("%02x ",((uint8_t*)cmd)[i]);
    	printf("\n");
    }
#endif
}

static void btResetCommand(MQX_FILE_PTR dev)
{
    uint8_t* packetState;
    packetState = hciCmdPacketStart(HCI_OGF_Controller_and_Baseband, HCI_CMD_Reset);
    hciCmdPacketFinish(packetState);
    callback.CALLBACK = write_cs_callback;
    callback.USERDATA = dev;
    ioctl(dev, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback);  
    btTxCmdPacket((HCI_Cmd*)packetStore, dev);
    fflush(dev);
}
static void btLESetEventMask(MQX_FILE_PTR dev)
{
    uint8_t* packetState;
    packetState = hciCmdPacketStart(HCI_LE_OGF, HCI_LE_Set_Event_Mask);
    hciCmdPacketFinish(packetState);
    callback.CALLBACK = write_cs_callback;
    callback.USERDATA = dev;
    ioctl(dev, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback); 
    btTxCmdPacket((HCI_Cmd*)packetStore, dev);
    fflush(dev);
}
  
static void btLEAdvertiseParams(MQX_FILE_PTR dev)
{
    uint8_t* packetState;
    packetState = hciCmdPacketStart(HCI_LE_OGF, HCI_LE_Set_Advertising_Parameters);
    packetState = hciCmdPacketAddU16(packetState, 0x00A0);
    packetState = hciCmdPacketAddU16(packetState, 0x00A0);
    packetState = hciCmdPacketAddU8(packetState, 0x03);
    packetState = hciCmdPacketAddU8(packetState, 0x00);
    packetState = hciCmdPacketAddU8(packetState, 0x00);    
    packetState = hciCmdPacketAddU16(packetState, 0x0000);/*pub addr*/
    packetState = hciCmdPacketAddU16(packetState, 0x0000);/*pub addr*/
    packetState = hciCmdPacketAddU16(packetState, 0x0000);/*pub addr*/
    packetState = hciCmdPacketAddU8(packetState, 0x07);
    packetState = hciCmdPacketAddU8(packetState, 0x00);
    hciCmdPacketFinish(packetState);  
    callback.CALLBACK = write_cs_callback;
    callback.USERDATA = dev;
    ioctl(dev, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback);
    btTxCmdPacket((HCI_Cmd*)packetStore, dev);
    fflush(dev);
}

static void btEmSetPublicAddress(MQX_FILE_PTR dev)
{
    uint8_t* packetState;
    packetState = hciCmdPacketStart(EM_VENDOR_BASE_OGF, EM_SET_PUBLIC_ADDRESS);
    packetState = hciCmdPacketAddU8(packetState, 0x0F);
    packetState = hciCmdPacketAddU8(packetState, 0x0E);
    packetState = hciCmdPacketAddU8(packetState, 0x0D);
    packetState = hciCmdPacketAddU8(packetState, 0x0C);
    packetState = hciCmdPacketAddU8(packetState, 0x0B);
    packetState = hciCmdPacketAddU8(packetState, 0xBB);
    hciCmdPacketFinish(packetState);  
    callback.CALLBACK = write_cs_callback;
    callback.USERDATA = dev;
    ioctl(dev, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback); 
    btTxCmdPacket((HCI_Cmd*)packetStore, dev);
    fflush(dev);
    
}

static void btLESetAdvertisingPacket(MQX_FILE_PTR dev)
{
    uint8_t* packetState;
    packetState = hciCmdPacketStart(HCI_LE_OGF, HCI_LE_Set_Advertising_Data);
    packetState = hciCmdPacketAddU16(packetState, 0x0215);
    packetState = hciCmdPacketAddU16(packetState, 0x0601);
    packetState = hciCmdPacketAddU16(packetState, 0x0303);
    packetState = hciCmdPacketAddU16(packetState, 0x180D);
    packetState = hciCmdPacketAddU16(packetState, 0x090D);
    packetState = hciCmdPacketAddU16(packetState, 0x324B);
    packetState = hciCmdPacketAddU16(packetState, 0x5F32);
    packetState = hciCmdPacketAddU16(packetState, 0x5346);
    packetState = hciCmdPacketAddU16(packetState, 0x5F48);
    packetState = hciCmdPacketAddU16(packetState, 0x4C42);
    packetState = hciCmdPacketAddU16(packetState, 0x0045);
    hciCmdPacketFinish(packetState);  
    callback.CALLBACK = write_cs_callback;
    callback.USERDATA = dev;
    ioctl(dev, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback); 
    btTxCmdPacket((HCI_Cmd*)packetStore, dev);
    fflush(dev);
}
static void btLESetAdvertisEnable(MQX_FILE_PTR dev)
{
    uint8_t* packetState;
    packetState = hciCmdPacketStart(HCI_LE_OGF, HCI_LE_Set_Advertise_Enable);
    packetState = hciCmdPacketAddU8(packetState, 0x01);
    hciCmdPacketFinish(packetState);  
    callback.CALLBACK = write_cs_callback;
    callback.USERDATA = dev;
    ioctl(dev, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback); 
    btTxCmdPacket((HCI_Cmd*)packetStore, dev);
    fflush(dev);
}
/*****************HCI*******************/
/* end file */

