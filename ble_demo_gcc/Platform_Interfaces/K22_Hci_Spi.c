/****************************************************************************
 *
 * File:          BleUart.c
 *
 * Description:   Contains routines for Transport specific initialisation,
 *					and de-initialisation.
 * 
 * Created:       June, 2008
 * Version:		  0.1
 *
 * CVS Revision : $Revision: 1.4 $
 * $Project$
 *
 * ALL RIGHTS RESERVED.
 *
 * Unpublished Confidential Information of ALPWISE.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of ALPWISE.
 * 
 * Use of this work is governed by a license granted ALPWISE.
 * This work contains confidential and proprietary information of
 * ALPWISE. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
#include <mqx.h>
#include <bsp.h>
#include <spi.h>
#include <mutex.h>

#include "BleTypes.h"
#include "BleTransport.h"
#include "BleSystem.h"

#include "spi_prv.h"
#include "spi_em9301_prv.h"
#include "spi_dspi_common.h"
#include "74hc595.h"

/***************************************************************************\
 * CONSTANT 
\***************************************************************************/

/*Length of Read Buffer*/
//#define EM9301_READ_BUFFER_SIZE			50

/***************************************************************************\
 * RAM data
\***************************************************************************/
MQX_FILE_PTR spi_dev;
MQX_FILE_PTR flash_file;
SPI_CS_CALLBACK_STRUCT	callback;
MUTEX_STRUCT stack_mutex, database_mutex;
MUTEX_ATTR_STRUCT stack_mutexattr, database_mutexattr;
U8 hciBuffer[50];

/***************************************************************************\
 * MACRO 
\***************************************************************************/
#define PRE_CONFIGURE_FLAG 0x01   //should check this in the cs callback


/***************************************************************************\
 * ROM data
\***************************************************************************/

/***************************************************************************
 *	Local functions Prototype
 ***************************************************************************/
static _mqx_int read_cs_callback(uint32_t pre_cfg_mask, void *user_data);
static _mqx_int write_cs_callback(uint32_t pre_cfg_mask, void *user_data);
static void taskRead(uint32_t para);

/***************************************************************************\
 * External Functions prototypes
\***************************************************************************/
extern void SYSTEM_Log(char *format,...);

/****************************************************************************\
 *	APPLICATION INTERCAFES functions definition
\****************************************************************************/

/** Initialise the Uart.
 *
 * BLEUART_Init()
 *	This function is called by the BLE stack (TRANSPORT layer) to initialise
 *	the UART layer.
 *  The user should uses this function to open and setup parameters for the
 *	UART line, eventually create Read Thread or setup RX and TX interrupts.
 *
 *	When this function succeeds, the UART layer shall be fully functional
 *
 *	This function is called during the BLESTCK_Init() process, failure here 
 *	will issue a failure in BLESTCK_Init()
 *
 * @todo implement this function
 *
 * @see BLESTCK_Init()
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates to the BLE stack that the UART have been
 *		successfully initialized
 *	- BLESTATUS_FAILED indicates to the BLE stack that the UART could not be
 *		initialized
 *	
 * @author Alexandre GIMARD
 */
extern const SPI_INIT_STRUCT _bsp_spi1_init;
BleStatus BLEUART_Init(void){
	// Add here specific code to execute during Stack Initialisation
	// in order to initialise the transport drivers
	//>
	_task_id	task_id;
	TASK_TEMPLATE_STRUCT	task_template;
    uint32_t para = 0x00;
    _io_spi_install("spi1:", &_bsp_spi1_init);
    spi_dev = fopen("spi1:", NULL);	     
	ioctl (spi_dev, IO_IOCTL_SPI_SET_TRANSFER_MODE, &para);
	
	task_template.TASK_TEMPLATE_INDEX = 0;
	task_template.TASK_ADDRESS = taskRead;
	task_template.TASK_STACKSIZE = 3000L;
	task_template.TASK_PRIORITY = 7;
	task_template.TASK_NAME = "SPI read";
	task_template.TASK_ATTRIBUTES = 0;
	task_template.CREATION_PARAMETER = (uint32)spi_dev;
	task_template.DEFAULT_TIME_SLICE = 0;
	task_id = _task_create_blocked(0, 0, (uint32)&task_template);
	if(task_id == 0)
	{
		#ifdef LOCAL_LOG
		SYSTEM_Log("Create read task failed\n");
		#endif
		return BLESTATUS_FAILED;
	}	
	_task_ready(_task_get_td(task_id));
	mux_74hc595_clear_bit(BSP_74HC595_0, BSP_74HC595_SPI_S0);
	mux_74hc595_clear_bit(BSP_74HC595_0, BSP_74HC595_SPI_S1);
	mux_74hc595_set_bit(BSP_74HC595_0, BSP_74HC595_VBLE_3V3);
#if 0
	/*initialize related mutex*/
	_mutatr_init(&stack_mutexattr);
        _mutex_init(&stack_mutex, &stack_mutexattr);
	_mutatr_init(&database_mutexattr);
	_mutex_init(&database_mutex, &database_mutexattr);

	/*open flash file for system interface*/
	flash_file = fopen("flashx:", NULL);
	if(flash_file == NULL)
	{
	#ifdef LOCAL_LOG
		SYSTEM_Log("open flash file error.\n");
	#endif
		return BLESTATUS_FAILED;
	}
#endif
	
	//<
	return BLESTATUS_SUCCESS;
}


/** De-Initialise the Uart.
 *
 * BLEUART_Deinit()
 *	This function is called by the BLE stack (TRANSPORT layer) to de-initialise
 *	the UART layer. Eventual thread shall be terminated here.
 *	When this function succeed, the UART layer shall be fully de-initialised
 *
 *	This function is called during the BLESTCK_Deinit() process, failure here 
 *	will issue a failure in BLESTCK_Deinit()
 *
 * @todo implement this function
 *
 * @see BLESTCK_Deinit()
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates to the BLE stack that the UART have been
 *		successfully initialized
 *	- BLESTATUS_FAILED indicates to the BLE stack that the UART could not be
 *		initialized
 *	
 * @author Alexandre GIMARD
 */
BleStatus BLEUART_Deinit(void){
	// Add here specific code to execute during Stack De-Initialisation
	// in order to de-initialise the transport drivers
	//>
    if(MQX_OK != fclose(spi_dev))
    {
    #ifdef LOCAL_LOG
        SYSTEM_Log("Unable to close communication channel\n");
    #endif        
        return BLESTATUS_FAILED;
    }
	//<
	return BLESTATUS_SUCCESS;
}


/** Send data through Uart.
 *
 * BLEUART_Send()
 *	This function is called by the BLE stack (TRANSPORT layer) to send data
 *	through the UART layer. 
 *	When the function succeed, the UART layer shall have send all the data
 *	It is to the uart layer to retry to sent the data, if the function fails
 *  it result to a transport error and stack de-initialisation.
 *  Note that BLEUART_Send is always called after BLEUART_TXStarts() and
 *  before BLEUART_TXEnds().
 *
 * @todo implement this function
 *
 * @param data : pointer to received data 
 * @param dataLen : len of the received data contained in the data pointer
 *
 *
 * @return The status of the operation:
 *	- BLESTATUS_SUCCESS indicates to the BLE stack that the data have been
 *		successfully sent
 *	- BLESTATUS_FAILED indicates to the BLE stack that the data have not 
 *		 been sent, it result to a transport error
 */
BleStatus BLEUART_Send(U8* uartData , U8 uartDataLen){

	// Add here specific code to execute during Stack Process
	// in order to send data trough transport
	//>
	#if 0
	U8 i = 0;
	U8* dataPtr = uartData;
	SYSTEM_Log("--Tx:");
	for(; i < uartDataLen; i++)
		SYSTEM_Log(" %02x", dataPtr[i]);
	SYSTEM_Log("\n");
#endif
	if(IO_ERROR == fwrite(uartData, 1, uartDataLen, spi_dev)){
		SYSTEM_Log("Wrire ERROR\n");
		return BLESTATUS_FAILED;
	}
	//<
	return BLESTATUS_SUCCESS;
}

/** Give information to the UART layer that the stack will starts a transmit
 * process.
 *
 * BLEUART_TXStarts()
 *	This function is called by the BLE stack (TRANSPORT layer) to inform
 *	the UART layer that a packet will be transmitted. 
 *	As a packet can be transmitted into several BLEUART_Send() calls, the 
 *  stack  inform the UART layer that a transmittion starts. Then the stack 
 *  will call one or several BLEUART_Send() and then  BLEUART_TXEnds API.
 *  it is usefull for UARt line that are not full duplex or with a specific
 *  flow control and requiere to know when transmition starts.
 * 	Otherwise the function should be empty.
 *  Note that BLEUART_Send is always called after BLEUART_TXStarts() and
 *  before BLEUART_TXEnds().
 * 
 * @return none
 */
void BLEUART_TXStarts( void ){

	// Add here specific code to execute during Stack Process
	// in order to run specific code before sending data
	//>
	__asm("cpsid i");
    callback.CALLBACK = write_cs_callback;
	callback.USERDATA = spi_dev;
    ioctl(spi_dev, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback);	
	//<
}

/** Give information to the UART layer that the stack has ends a transmit
 * process.
 *
 * BLEUART_TXEnds()
 *	This function is called by the BLE stack (TRANSPORT layer) to inform
 *	the UART layer that a packet has been transmitted. 
 *	As a packet can be transmitted into several BLEUART_Send() calls, the 
 *  stack  inform the UART layer that a transmittion ends. 
 *  it is usefull for UART line that are not full duplex or with a specific
 *  flow control and requiere to know when transmition ends.
 * 	Otherwise the function should be empty.
 *  Note that BLEUART_Send is always called after BLEUART_TXStarts() and
 *  before BLEUART_TXEnds().
 * 
 * @return none
 */
void BLEUART_TXEnds( void ){
	
	// Add here specific code to execute during Stack Process
	// in order to run specific code after sending data
	//>
	fflush(spi_dev);	
	__asm("cpsie i");
	//<
}

// BLEUART_READ through BLETRANSPORT_UartDataReceived(*buffer,(U8)bytesRead);


/****************************************************************************\
 *	Local functions definition
\****************************************************************************/

static _mqx_int read_cs_callback(uint32_t pre_cfg_mask, void *user_data)
{		
	
	BleStatus				status;
	MQX_FILE_PTR					dev = (MQX_FILE_PTR)user_data; 
	SPI_DEV_DATA_STRUCT_PTR			dev_data = (SPI_DEV_DATA_STRUCT_PTR)(dev->DEV_DATA_PTR);

	U8 byteReceievd;
	VDSPI_REG_STRUCT_PTR               dspi_ptr;  
	LWGPIO_STRUCT  em9301_spi_mosi;
	if(pre_cfg_mask & PRE_CONFIGURE_FLAG)
	{
		dspi_ptr = _bsp_get_dspi_base_address(1);    
		lwgpio_init(&em9301_spi_mosi, BSP_EM9301_MOSI_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);
		lwgpio_set_functionality(&em9301_spi_mosi, 1);
		lwgpio_set_value(&em9301_spi_mosi, 0);		
		/*Assert CS pin*/ 	
		dspi_ptr->MCR |= DSPI_MCR_MSTR_MASK;	
		dspi_ptr->MCR = (dspi_ptr->MCR & ~(uint32_t)DSPI_MCR_PCSIS_MASK) | DSPI_MCR_PCSIS(~0x01);
		lwgpio_set_functionality(&em9301_spi_mosi, 7);	
	}
	else
	{
		if(dev_data->STATS.RX_PACKETS)
		{
			if(dev_data->STATS.RX_PACKETS >= 50)
			{
				
				status = BLETRANSPORT_UartDataReceived(hciBuffer, dev_data->STATS.RX_PACKETS);
				if(status == BLESTATUS_FAILED)
				{
					BLEUART_Deinit();
					return BLESTATUS_FAILED;					
				}			
			}
			byteReceievd = dev_data->STATS.RX_PACKETS;
			status = BLETRANSPORT_UartDataReceived(hciBuffer, byteReceievd);			
			if(SPI_OK != ioctl(dev, IO_IOCTL_SPI_CLEAR_STATS, 0))
				status = BLESTATUS_FAILED;
		}			
        
	}	
	return status;
}

static _mqx_int write_cs_callback(uint32_t pre_cfg_mask, void *user_data)
{
	VDSPI_REG_STRUCT_PTR               dspi_ptr;
	LWGPIO_STRUCT  em9301_spi_mosi;
	if(pre_cfg_mask & PRE_CONFIGURE_FLAG)
	{
		dspi_ptr = _bsp_get_dspi_base_address(1);
		/*MOSI set pin*/
		lwgpio_init(&em9301_spi_mosi, BSP_EM9301_MOSI_PIN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE);
		lwgpio_set_functionality(&em9301_spi_mosi, 1);
		lwgpio_set_value(&em9301_spi_mosi, 1);
		/*Assert CS pin*/
		dspi_ptr->MCR = (dspi_ptr->MCR & ~(uint32_t)DSPI_MCR_PCSIS_MASK) | DSPI_MCR_PCSIS(~0x01);
		lwgpio_set_functionality(&em9301_spi_mosi, 7);
	}
	else
	{
		//after configure need to call the callback here!!!		
	
	}	
	return MQX_OK;
}


/*TASK*-------------------------------------------------------------------
*
* Task Name : spi_read_task func
* Comments  :
*
*END*----------------------------------------------------------------------*/
static void taskRead(uint32_t para)
{

	MQX_FILE_PTR dev_file = (MQX_FILE_PTR)para;
	SPI_DRIVER_DATA_STRUCT_PTR driver_data;		
	SPI_CS_CALLBACK_STRUCT callback;
	uint8_t tmp, *buf;
	for(;;)
	{
	driver_data = (SPI_DRIVER_DATA_STRUCT_PTR)(dev_file->DEV_PTR->DRIVER_INIT_PTR);		
	callback.CALLBACK = write_cs_callback;
	callback.USERDATA = dev_file;
	ioctl(dev_file, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback);
	_lwsem_wait(&driver_data->IRQ_SEM);	
	callback.CALLBACK = read_cs_callback;
	callback.USERDATA = dev_file;
	ioctl(dev_file, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback);
	buf = hciBuffer;
	while(lwgpio_get_value(&driver_data->SPI_IRQ_PIN))
	{
		if(IO_ERROR != fread(&tmp, 1, 1,dev_file))
			*buf++ = tmp;
	}
	fflush(dev_file);
	#if 0
	uint8_t *start = hciBuffer;	
	SYSTEM_Log("--Rx:");
	while(start < buf){
		SYSTEM_Log(" %02X", *start++);
	}
	SYSTEM_Log("\n");
	#endif
	}
	
}
