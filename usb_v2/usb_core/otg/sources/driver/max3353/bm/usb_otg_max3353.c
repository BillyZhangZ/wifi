/**HEADER********************************************************************
* 
* Copyright (c) 2010, 2013 - 2014 Freescale Semiconductor;
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
* $FileName: usb_otg_max3353.c$
* $Version : 
* $Date    : 
*
* Comments : This file contains the implementation of the OTG functions using the MAX3353 circuit
*
*         
*****************************************************************************/
#include "usb.h"
#include "usb_otg_private.h"
#include "usb_otg_max3353.h"
#include "soc_i2c.h"
#include "adapter.h"

uint32_t g_u32ID=0;

/* Private functions prototypes *********************************************/
bool            max3353_WriteReg(uint8_t i2c_channel, uint8_t regAdd , uint8_t regValue);
bool            max3353_ReadReg(uint8_t i2c_channel, uint8_t regAdd , uint8_t* p_regValue);
static uint8_t  _otg_max3353_get_interrupts(uint8_t i2c_channel);
static uint8_t  max3353_Init(uint8_t i2c_channel);
extern void     _bsp_usb_otg_max3353_clear_pin_int_flag(void);
extern void     _bsp_usb_otg_max3353_set_pin_int(bool level ,bool enable);
extern void     Pause(void);
/* Private functions definitions *********************************************/
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : max3353_CheckifPresent
* Returned Value   :
* Comments         : Check if MAX3353 is present
*    
*
*END*----------------------------------------------------------------------*/
static uint8_t max3353_CheckifPresent
(
    uint8_t i2c_channel
)
{

    uint8_t  u8Counter;
    uint8_t  ch_read;
    for (u8Counter=0;u8Counter<4;u8Counter++)
    {
        g_u32ID=g_u32ID<<8;
        max3353_ReadReg(i2c_channel,u8Counter , &ch_read); 
        g_u32ID |= ch_read;
    }
    if (g_u32ID!=MAX3353_MID)
    {
        return (MAX3353_NOT_PRESENT);
    }

    g_u32ID=0;
    for (u8Counter=4;u8Counter<8;u8Counter++)
    {
        g_u32ID=g_u32ID<<8;
        max3353_ReadReg(i2c_channel,u8Counter , &ch_read);
        g_u32ID |= ch_read;
    }
    if (g_u32ID!=MAX3353_PID)
    {
        return (MAX3353_NOT_PRESENT);
    }
    return (MAX3353_OK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : max3353_Init
* Returned Value   :
* Comments         : Initialize max3353
*    
*
*END*----------------------------------------------------------------------*/
static uint8_t max3353_Init
(
    uint8_t i2c_channel
)
{
    if (max3353_CheckifPresent(i2c_channel))
    {
        return (MAX3353_NOT_PRESENT);
    }

    /* Enable Charge pump for VBUs detection */ 
    max3353_WriteReg(i2c_channel,MAX3353_REG_CTRL_2,0x80);

    /* Set Rising edge for VBUS detection */
    max3353_WriteReg(i2c_channel,MAX3353_REG_INT_EDGE,VBUS_VALID_ED_MASK);

    /* Activate ID (GND and float) & SESSION Interrupts */
    max3353_WriteReg(i2c_channel,MAX3353_REG_INT_MASK,ID_FLOAT_EN_MASK|ID_GND_EN_MASK\
    |VBUS_VALID_EN_MASK|SESSION_SESSEND_EN_MASK);

    return (MAX3353_OK);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : max3353_WriteReg
* Returned Value   :
* Comments         : Write data to max3353 register
*    
*
*END*----------------------------------------------------------------------*/
bool max3353_WriteReg
(
    uint8_t i2c_channel, 
    uint8_t regAdd , 
    uint8_t regValue
)
{
    /* Send I2C slave address to bus*/
    i2c_Start(i2c_channel);
    i2c_WriteByte(i2c_channel,(MAX3353_SLAVE_ADDR <<1) | 0);
    i2c_Wait(i2c_channel);

    /* Send register address of MAX3353 */
    i2c_WriteByte(i2c_channel, regAdd);
    i2c_Wait(i2c_channel);
    
    /* Send data to MAX3353*/
    i2c_WriteByte(i2c_channel, regValue);
    i2c_Wait(i2c_channel);

    /* Send stop signal */
    i2c_Stop(i2c_channel);
    Pause();
    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : max3353_ReadReg
* Returned Value   :
* Comments         : Read data from max3353 register
*    
*
*END*----------------------------------------------------------------------*/
bool max3353_ReadReg
(
    uint8_t     i2c_channel, 
    uint8_t     regAdd, 
    uint8_t*    p_regValue
)
{
    uint8_t result;
    uint32_t j;
    
    /* Send Slave Address */
    i2c_Start(i2c_channel);
    i2c_WriteByte(i2c_channel, (MAX3353_SLAVE_ADDR <<1) | 0);
    i2c_Wait(i2c_channel);
    
    /* Write Register Address */ 
    i2c_WriteByte(i2c_channel, regAdd);
    i2c_Wait(i2c_channel);

    /* Do a repeated start */
    i2c_RepeatedStart(i2c_channel);

    /* Send Slave Address */  
    i2c_WriteByte(i2c_channel, (MAX3353_SLAVE_ADDR << 1) | 0x01); 
    i2c_Wait(i2c_channel);

    /* Put in Rx Mode */
    i2c_SetRXMode(i2c_channel);

    /* Turn off ACK */
    i2c_GiveNACK(i2c_channel);

    /* Dummy read */
    result = i2c_ReadByte(i2c_channel);
    for (j=0; j<5000; j++){};  
    i2c_Wait(i2c_channel);
    
    /* Send stop signal */
    i2c_Stop(i2c_channel);

    result = i2c_ReadByte(i2c_channel);
    Pause();
    *p_regValue = result;
    return TRUE;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _otg_max3353_enable_disable
* Returned Value   :
* Comments         : Enable/disable MAX3353
*    
*
*END*----------------------------------------------------------------------*/
static void _otg_max3353_enable_disable
(
    uint8_t i2c_channel,
    bool    enable
)
{
    uint8_t max3353_data;

    max3353_Init(i2c_channel);
    if (enable)
    {
        max3353_data = 0x02;
        while (max3353_WriteReg(i2c_channel,MAX3353_REG_CTRL_1, max3353_data) == FALSE){}
        max3353_data = 0x1F; /* Enable interrupts */
        while (max3353_WriteReg(i2c_channel,MAX3353_REG_INT_MASK, max3353_data) == FALSE){}
        max3353_data = 0x00;  /* Enable module. */
    }
    else
    {
        max3353_data = 1;  /* Activate shutdown */
    }

    /* Enable/Disable module */
    while (max3353_WriteReg(i2c_channel,MAX3353_REG_CTRL_2, max3353_data) == FALSE){}

    /* Read the latch to clear any pending interrupts */
    (void)_otg_max3353_get_interrupts(i2c_channel);  
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _otg_max3353_get_status
* Returned Value   : unsigned char, meaning the status of MAX3353
* Comments         : Get MAX3353 status
*    
*
*END*----------------------------------------------------------------------*/
static uint8_t _otg_max3353_get_status
(
    uint8_t i2c_channel
)
{
    uint8_t status;
    uint8_t edge;
    uint8_t new_edge;

    while (max3353_ReadReg(i2c_channel,MAX3353_REG_STATUS, &status) == FALSE){}

    /* Handle here the edge detection in SessionValid and VBus valid */
    /* Read the current edge */
    while (max3353_ReadReg(i2c_channel,MAX3353_REG_INT_EDGE, &edge) == FALSE){}

    new_edge = (uint8_t)((~(status)) & 0x03);

    if (new_edge != edge)
    {      
        /* Write the new edges */
        while (max3353_WriteReg(i2c_channel,MAX3353_REG_INT_EDGE, new_edge) == FALSE){}
    }

    return status;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _otg_max3353_get_interrupts
* Returned Value   : unsigned char meaning interrupts
* Comments         : Read interrupts from MAX3353
*    
*
*END*----------------------------------------------------------------------*/
static uint8_t _otg_max3353_get_interrupts
(
    uint8_t i2c_channel
)
{
    uint8_t data;

    while (max3353_ReadReg(i2c_channel,MAX3353_REG_INT_LATCH, &data) == FALSE){}
    return data;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _otg_max3353_set_VBUS
* Returned Value   :
* Comments         : Set VBUS for MAX3353
*    
*
*END*----------------------------------------------------------------------*/
static void _otg_max3353_set_VBUS
(
    uint8_t i2c_channel,
    bool    enable
)
{
    uint8_t max3353_data;
    
    if (enable)
    {
        max3353_data = OTG_CTRL_2_VBUS_DRV;/* connect VBUS to the charge pump */
    }
    else
    {
        max3353_data = OTG_CTRL_2_VBUS_DISCHG;/* disconnect the charge pump and  activate the 5k pull down resistor */
    }
    
    /* Enable/Disable module */
    while (max3353_WriteReg(i2c_channel,MAX3353_REG_CTRL_2, max3353_data) == FALSE){}

}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _otg_max3353_set_pdowns
* Returned Value   :
* Comments         : Set pull-downs for MAX3353
*    
*
*END*----------------------------------------------------------------------*/
static void _otg_max3353_set_pdowns
(
    uint8_t i2c_channel,
    uint8_t bitfield
)
{
    uint8_t max3353_data;

    max3353_data =  OTG_CTRL_1_IRQ_PUSH_PULL;
    if (bitfield & OTG_CTRL_PDOWN_DP)
    {
        max3353_data |= OTG_CTRL_1_PDOWN_DP;
    }

    if (bitfield & OTG_CTRL_PDOWN_DM)
    {
        max3353_data |= OTG_CTRL_1_PDOWN_DM;
    }
    
    while (max3353_WriteReg(i2c_channel,MAX3353_REG_CTRL_1, max3353_data) == FALSE){}
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _usb_otg_max3353_isr
* Returned Value   :
* Comments         : 
*    
*
*END*----------------------------------------------------------------------*/
static void _usb_otg_max3353_isr
(
    usb_otg_max3353_call_struct_t * otg_max3353_call_ptr
) 
{
    usb_otg_state_struct_t * usb_otg_struct_ptr = ((usb_otg_max3353_call_struct_t *)otg_max3353_call_ptr)->otg_handle_ptr;
    _bsp_usb_otg_max3353_clear_pin_int_flag();
    OS_Event_set(usb_otg_struct_ptr->otg_isr_event, USB_OTG_MAX3353_ISR_EVENT);
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _usb_otg_max3353_init
* Returned Value   :
* Comments         : 
*    
*
*END*----------------------------------------------------------------------*/
uint8_t _usb_otg_max3353_init
(
    usb_otg_max3353_call_struct_t * otg_max3353_call_ptr
)
{
    usb_otg_state_struct_t *    usb_otg_struct_ptr  = ((usb_otg_max3353_call_struct_t *)otg_max3353_call_ptr)->otg_handle_ptr;
    uint8_t                     channel             = otg_max3353_call_ptr->channel;

    /* configure GPIO for I2C function */
    i2c_Init(channel);
    _bsp_usb_otg_max3353_set_pin_int(FALSE, TRUE);
    OS_Event_set(usb_otg_struct_ptr->otg_isr_event, USB_OTG_MAX3353_ISR_EVENT);
    _otg_max3353_enable_disable(channel,TRUE);
    OS_install_isr(otg_max3353_call_ptr->init_param_ptr->vector, (void (*)(void *))_usb_otg_max3353_isr, otg_max3353_call_ptr);
    OS_intr_init(otg_max3353_call_ptr->init_param_ptr->vector,otg_max3353_call_ptr->init_param_ptr->priority,0,TRUE);
    return USB_OK;
}

/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _usb_otg_max3353_get_status
* Returned Value   :
* Comments         : 
*    
*
*END*----------------------------------------------------------------------*/
uint8_t _usb_otg_max3353_get_status
(
    usb_otg_max3353_call_struct_t * otg_max3353_call_ptr
)
{
    uint8_t                     status;
    uint8_t                     channel             = otg_max3353_call_ptr->channel;
    usb_otg_state_struct_t *    usb_otg_struct_ptr  = otg_max3353_call_ptr->otg_handle_ptr; 
    usb_otg_status_t *          otg_status_ptr      = &usb_otg_struct_ptr->otg_status;

   (void)_otg_max3353_get_interrupts(channel);
    
    status = _otg_max3353_get_status(channel);
    /* check the status indications */         
    /* ID status update */
    otg_status_ptr->id = (uint8_t)((status & OTG_STAT_ID_FLOAT) ? TRUE : FALSE);
    /* V_BUS_VALID status update */
    otg_status_ptr->vbus_valid = (uint8_t)((status & OTG_STAT_VBUS_VALID) ? TRUE : FALSE);
    /* SESS_VALID status update */
    otg_status_ptr->sess_valid = (uint8_t)((status & OTG_STAT_SESS_VALID) ? TRUE : FALSE);
    /* SESS_END status update */
    otg_status_ptr->sess_end = (uint8_t)((status & OTG_STAT_SESS_END) ? TRUE : FALSE);  

    return USB_OK;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _usb_otg_max3353_get_status
* Returned Value   :
* Comments         : 
*    
*
*END*----------------------------------------------------------------------*/
uint8_t _usb_otg_max3353_set_vbus
(
    usb_otg_max3353_call_struct_t * otg_max3353_call_ptr, 
    bool enable
)
{
    uint8_t channel = otg_max3353_call_ptr->channel;
    _otg_max3353_set_VBUS(channel,enable);

    return USB_OK;
}
/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _usb_otg_max3353_set_pull_downs
* Returned Value   :
* Comments         : 
*    
*
*END*----------------------------------------------------------------------*/
uint8_t _usb_otg_max3353_set_pull_downs
(
    usb_otg_max3353_call_struct_t * otg_max3353_call_ptr, 
    uint8_t bitfield 
)
{
    uint8_t channel = otg_max3353_call_ptr->channel;
    _otg_max3353_set_pdowns(channel,bitfield);
    return USB_OK;
}
/* EOF */