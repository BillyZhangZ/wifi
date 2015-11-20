/*
 * Copyright (c) 2014 Qualcomm Atheros, Inc.
 * All Rights Reserved.
 * Qualcomm Atheros Confidential and Proprietary.
 * $ATH_LICENSE_TARGET_C$
 */ 

/**************************************************************************
*
* $FileName: flash_agent.c$
* $Version : 3.6.20.0$
* $Date    : Jun-4-2010$
*
* Comments:
*
*   
*
*END************************************************************************/


#include "main.h"
#include "enet_wifi.h"

#include "mqx.h"
#include "bsp.h"
#include "enet.h"
#include <a_types.h>
#include <a_osapi.h>
#include <atheros_wifi.h>
#include <atheros_wifi_api.h>

uint_32 Crc16 (unsigned char *ptr,int count);
int_32 handle_request(request_t* request, uint_8 error);
A_INT32 get_version(version_t* version);
A_INT32 ath_driver_reset();

/********* Globals ************************/
request_t request;
response_t response;
extern _enet_handle handle;

static void init()
{
    uint_32 flags = 0;
   
    atheros_driver_setup_init();
    HVAC_initialize_networking();

#ifdef BSP_FRDM_K22F120M
    {
        /* change baudrate tp 38400 */
        uint_32 baudrate = 38400;
        ioctl(stdin, IO_IOCTL_SERIAL_SET_BAUD, (void *)&baudrate);
    }
#endif
    
    /* Clear or set flags */
    ioctl(stdin, IO_IOCTL_SERIAL_GET_FLAGS, &flags); 
    flags &= ~(IO_SERIAL_XON_XOFF | IO_SERIAL_TRANSLATION | IO_SERIAL_ECHO);
    ioctl(stdin, IO_IOCTL_SERIAL_SET_FLAGS, &flags);
}

uint_32 Crc16 (unsigned char *ptr,int count)
{   
	int crc, i;

    crc = 0;
    while (--count >= 0) 
	{
		crc = crc ^ (int)*ptr++ << 8;
		for (i = 0; i < 8; ++i)
			if (crc & 0x8000)
				crc = crc << 1 ^ 0x1021;
			else
				crc = crc << 1;
    }
    return (crc & 0xFFFF);
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : ath_driver_reset()
* Returned Value  : A_OK - successful completion or
*		   A_ERROR - failed.
* Comments		  : Handles sriver reset functionality
*
*END*-----------------------------------------------------------------*/
A_INT32 ath_driver_reset()
{
       	A_INT32 error;
	ATH_IOCTL_PARAM_STRUCT ath_param;
	A_UINT32 value;

        reset_atheros_driver_setup_init();
        
        ath_param.cmd_id = ATH_CHIP_STATE;
        ath_param.length = sizeof(value);

        /*Bring the driver down*/
        value = 0;
        ath_param.data = &value;
        error = ENET_mediactl (handle,ENET_MEDIACTL_VENDOR_SPECIFIC,&ath_param);

        if(error != A_OK){                
            return A_ERROR;
        }
    
        /*Bring the driver up*/
        value = 1;
        ath_param.data = &value;
        error = ENET_mediactl (handle,ENET_MEDIACTL_VENDOR_SPECIFIC, &ath_param);

        if(error == A_OK){
                return A_OK;
        }else{
               
                return A_ERROR;
        }
}


/*FUNCTION*-------------------------------------------------------------
*
* Function Name   : get_version()
* Returned Value  : A_OK version is retrieved successfully else ERROR CODE
* Comments        : gets driver,firmware version.
*
*END*-----------------------------------------------------------------*/
A_INT32 get_version(version_t* version)
{    
    ATH_IOCTL_PARAM_STRUCT inout_param;
    A_INT32 error;

    /*Reset the driver to boot in normal mode*/
    ath_driver_reset();

    inout_param.cmd_id = ATH_GET_VERSION;
    inout_param.data = version;
    inout_param.length = 16;

    error = ENET_mediactl (handle,ENET_MEDIACTL_VENDOR_SPECIFIC, &inout_param);
    if (A_OK != error)
    {
        return error;
    }
    return A_OK;
}




void Flash_Agent_Task(uint_32 val)
{
  	uint_32 i =0;
	uint_8 c;
	uint_16 calcCrc = 0;
	uint_16 size;
	
	init();
	
	while(1)
    {
      	size = read(stdin,&c,1);
      	
      	if(size > 0 )
      	{
      		if(i == sizeof(request)-1)
      		{
      			/*Received full size packet,check CRC and prepare response*/
      			((uint_8 *)&request)[i]=c;
      			i++;
     		   
      			calcCrc = Crc16((uint_8*)&request,sizeof(request) - 2);
                        calcCrc = SHORT_LE_TO_HOST(calcCrc);
      			if(calcCrc != request.crc)
      			{
      				handle_request(&request, 1);
      			}
      			else
      			{
      				handle_request(&request, 0);
      			}      		      			
      			i = 0;      			      			
      		}
      		else
      		{
      			((uint_8 *)&request)[i]=c;
      			i++;
      		}
      		size = 0;
      	}
    }
}



int_32 handle_request(request_t* request, uint_8 error)
{
	ATH_PROGRAM_FLASH_STRUCT flashMsg;
	ATH_IOCTL_PARAM_STRUCT inout_param;
	version_t version;
		 	 
	if(error)
	{
		/*CRC error, indicate to the other side*/
		response.cmdId = SHORT_BE_TO_HOST(CRC_ERROR);
		write(stdout,(char*)&response,sizeof(response));
		return 0;
	}
	request->cmdId = SHORT_LE_TO_HOST(request->cmdId);
	switch (request->cmdId)
	{
		case PROGRAM:
			response.cmdId = PROGRAM_RESULT;
			response.cmdId = SHORT_BE_TO_HOST(response.cmdId);					
                        flashMsg.load_addr = LONG_LE_TO_HOST(request->addr);
                        flashMsg.length = SHORT_LE_TO_HOST(request->length);
                        flashMsg.result = 0;
                        flashMsg.buffer = request->buffer;
                        
                        inout_param.cmd_id = ATH_PROGRAM_FLASH;
                        inout_param.data = &flashMsg;
                        inout_param.length = sizeof(flashMsg);
                        
                        error = ENET_mediactl (handle,ENET_MEDIACTL_VENDOR_SPECIFIC,&inout_param);
                        if (0 != error)
                        {
                            return error;
                        }
			response.result = ((ATH_PROGRAM_FLASH_STRUCT*)inout_param.data)->result;
			response.result = LONG_BE_TO_HOST(response.result);			
			write(stdout,(char*)&response,sizeof(response));
			
			break;
		
		case EXECUTE:
			response.cmdId = EXECUTE_RESULT;
            response.cmdId = SHORT_BE_TO_HOST(response.cmdId);
			flashMsg.load_addr = LONG_LE_TO_HOST(request->addr);
	        flashMsg.length = SHORT_LE_TO_HOST(request->length);
            flashMsg.result = LONG_LE_TO_HOST(request->option);
            flashMsg.buffer = NULL;
    
	        inout_param.cmd_id = ATH_EXECUTE_FLASH;
            inout_param.data = &flashMsg;
            inout_param.length = sizeof(flashMsg);
    
	        error = ENET_mediactl (handle,ENET_MEDIACTL_VENDOR_SPECIFIC,&inout_param);
            if (0 != error)
	        {
	            return error;
            }
			response.result = ((ATH_PROGRAM_FLASH_STRUCT*)inout_param.data)->result;
		
			write(stdout,(char*)&response,sizeof(response));
			break;
		
		case VERSION_NUM:
			response.cmdId = VERSION_NUM;

			response.result = VERSION;
			response.cmdId = SHORT_BE_TO_HOST(response.cmdId);
			response.result = LONG_BE_TO_HOST(response.result);
			write(stdout,(char*)&response,sizeof(response));
			break;
		
		case FW_VERSION:      
			response.cmdId = FW_VERSION;
			response.cmdId = A_BE2CPU16(response.cmdId);	
			if(get_version(&version) != A_OK){

			    response.result = 1;
			}else{
			    response.result = 0;
			    response.version.host_ver   = LONG_BE_TO_HOST(version.host_ver);
			    response.version.target_ver = LONG_BE_TO_HOST(version.target_ver);
			    response.version.wlan_ver   = LONG_BE_TO_HOST(version.wlan_ver);
			    response.version.abi_ver    = LONG_BE_TO_HOST(version.abi_ver);
			}

			write(stdout,(char*)&response,sizeof(response));
			break;
				
		default:
			response.cmdId = UNKNOWN_CMD;
                        response.cmdId = SHORT_BE_TO_HOST(response.cmdId);
			write(stdout,(char*)&response,sizeof(response));
			break;
	}
	return 0;
}

void WifiConnected(int val)
{	
	//HVAC_SetOutput(HVAC_ALIVE_OUTPUT, (val)? TRUE:FALSE);	
	printf("Wifi Connected %d\n",val);
}
