/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved
*
* Copyright (c) 1989-2008 ARC International;
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
*END************************************************************************/

#include "main.h"
#include "usb_classes.h"

#include <lwmsgq.h>
#include "msi.h"
#if MFI_ENABLED
#include "usb_mfi_task.h"


/**********************testing purpose****************/
LWGPIO_STRUCT *testPin1 = NULL;
LWGPIO_STRUCT *testPin2 = NULL;
LWGPIO_STRUCT *testPin3 = NULL;
LWGPIO_STRUCT *testPin4 = NULL;


uint8_t iAP2_Host_bfnHostInitialization(void);
/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : USB_mfi_task
* Returned Value : None
* Comments       :
*     First function called. This rouine just transfers control to host main
*END*--------------------------------------------------------------------*/

void USB_mfi_task(uint_32 param)
{ 
    USB_STATUS           error;
    uint8_t              bInitStatus;

#ifdef BSP_LED1
    testPin1 = _mem_alloc_system(sizeof (*testPin1));
    lwgpio_init(testPin1,
            BSP_LED1,
            LWGPIO_DIR_OUTPUT,
            LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(testPin1, BSP_LED1_MUX_GPIO);
#endif

#ifdef BSP_LED2
    testPin2 = _mem_alloc_system(sizeof (*testPin2));
    lwgpio_init(testPin2,
            BSP_LED2,
            LWGPIO_DIR_OUTPUT,
            LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(testPin2, BSP_LED2_MUX_GPIO);
#endif

#ifdef BSP_LED3
    testPin3 = _mem_alloc_system(sizeof (*testPin3));
    lwgpio_init(testPin3,
            BSP_LED3,
            LWGPIO_DIR_OUTPUT,
            LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(testPin3, BSP_LED3_MUX_GPIO);
#endif

#ifdef BSP_LED4
    testPin4 = _mem_alloc_system(sizeof (*testPin4));
    lwgpio_init(testPin4,
            BSP_LED4,
            LWGPIO_DIR_OUTPUT,
            LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(testPin4, BSP_LED4_MUX_GPIO);
#endif


    do
    {
        bInitStatus = iAP2_Host_bfnHostInitialization();
        _time_delay(10);
    }while(0 != bInitStatus);

    _task_destroy(USB_MFI_TASK);
}



void iAP2_EA_Callback(void *DataRxInfo, uint8_t bEAProtID, uint8_t bStatus){
    return;
}
#endif
