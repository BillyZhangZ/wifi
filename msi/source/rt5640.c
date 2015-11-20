/*HEADER**********************************************************************
*
* Copyright 2014 Freescale Semiconductor, Inc.
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This include file is used to provide information needed by
*   applications using the SAI I/O functions.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <i2c.h>
#include "74hc595.h"
#include "codec.h"
#include "rt5640.h"


#if BSP_K22FSH                              
#define I2C_DEVICE_POLLED "i2c0:"
#else
#error No communication device defined! Please check user_config.h and enable proper I2C device. 
#endif

#if BSPCFG_ENABLE_SAI
#define AUDIO_DEVICE "sai:"
#elif BSPCFG_ENABLE_II2S0
#define AUDIO_DEVICE "ii2s0:"
#else
#error No audio device defined! Please check user_config.h and enable either I2S or SAI device.
#endif

enum snd_soc_bias_level {
	SND_SOC_BIAS_OFF = 0,
	SND_SOC_BIAS_STANDBY = 1,
	SND_SOC_BIAS_PREPARE = 2,
	SND_SOC_BIAS_ON = 3,
};

struct reg_default {
	unsigned int reg;
	unsigned int def;
};

#if 1
static struct reg_default init_list[] = {
       //{RT5640_PR_BASE + 0x1c,	0x0d21},
	{RT5640_PR_BASE + 0x3d,	0x3600},
	{RT5640_PR_BASE + 0x12,	0x0aa8},
	{RT5640_PR_BASE + 0x14,	0x0aaa},
	{RT5640_PR_BASE + 0x20,	0x6110},
	{RT5640_PR_BASE + 0x21,	0xe0e0},
	{RT5640_PR_BASE + 0x23,	0x1804},
};
#define RT5640_INIT_REG_LEN ARRAY_SIZE(init_list)
#endif

#if 0 /* From rt AE */
static const struct reg_default rt5640_reg[] = {

    {0xFA, 0x3401},
//->    {0x63, 0xE8D8},       /* Power Management Control 3 */
                          /* bit 3 Slow VREF, bit 4 VREF2 Power Contorl Enable
                          bit 4 VREF2 Power Control enable
                          bit 6,7 Left/Right Headphone Amp Power Control enable
                          bit 11 MBIAS Bandgap Power enable
                          bit 13 MBIAS Power enable
                          bit 14 VREF1 Fast Mode Control , enable Slow VREF
                          bit 15 VREF1 Power enable.
                          */
                         
                          /* Power Management Control 1 */
   //-> {0x61, 0x9800},       
                          /* bit 11 Analog DACR1 Power Control enable
                             bit 12 Analog DACL1 Power Control enable
                             bit 15 I2S1 Digital Interface Power Control enable */
                    //{0x93, 0x3030},  
                    //{0x2A, 0x1414},  
//    {0x73, 0x0008},       /* ADC/DAC Clock Control 1 */
                          /* bit 3:2   10'b 32FS     */
    {0x6A, 0x001C},  
    {0x6C, 0x0D21},   
                           /* HP Amp Contorl 1 */
// ->    {0x8E, 0x001F},        
                           /* bit 0 HP Amp All Power On Control enable
                            * bit 1 Reserved 
                            * bit 2 Power on Soft Generator enable
                            * bit 3 Charge Pump Power Contorl enable
                            * bit 4 Enable HeadPhone Output
                            */
//->    {0x8F, 0x3100},        /* ? */
           //{0x91, 0x0E00},  
           //{0x45, 0xB000},  
           //{0x02, 0x4848},  
       //{0x48, 0xB800},  
       //{0x49, 0x1800},
    {0x6A, 0x003D},  
    {0x6C, 0x3600},  

//    {0x61, 0x9801},    // ?
    //{0x01, 0x4848},  
    //{0x6A, 0x001C},  
    //{0x6C, 0xFD21}, 

#if 0
      {0x6A, 0x006c},  
      {0x6C, 0x0}, 
      {0xd3, 0x0}, 
      {0xb0, 0xa080 },
#endif
   
} ;
#endif

static const struct reg_default rt5640_reg[] = {
	{ 0x00, 0x000e },
	{ 0x01, 0xc8c8 },
	{ 0x02, 0xc8c8 },
	{ 0x03, 0xc8c8 },
	{ 0x04, 0x8000 },
	{ 0x0d, 0x0000 },
	{ 0x0e, 0x0000 },
	{ 0x0f, 0x0808 },
	{ 0x19, 0xafaf },
	{ 0x1a, 0xafaf },
	{ 0x1b, 0x0000 },
	{ 0x1c, 0x2f2f },
	{ 0x1d, 0x2f2f },
	{ 0x1e, 0x0000 },
	{ 0x27, 0x7060 },
	{ 0x28, 0x7070 },
	{ 0x29, 0x8080 },
	{ 0x2a, 0x5454 },
	{ 0x2b, 0x5454 },
	{ 0x2c, 0xaa00 },
	{ 0x2d, 0x0000 },
	{ 0x2e, 0xa000 },
	{ 0x2f, 0x0000 },
	{ 0x3b, 0x0000 },
	{ 0x3c, 0x007f },
	{ 0x3d, 0x0000 },
	{ 0x3e, 0x007f },
	{ 0x45, 0xe000 },
	{ 0x46, 0x003e },
	{ 0x47, 0x003e },
	{ 0x48, 0xf800 },
	{ 0x49, 0x3800 },
	{ 0x4a, 0x0004 },
	{ 0x4c, 0xfc00 },
	{ 0x4d, 0x0000 },
	{ 0x4f, 0x01ff },
	{ 0x50, 0x0000 },
	{ 0x51, 0x0000 },
	{ 0x52, 0x01ff },
	{ 0x53, 0xf000 },
	{ 0x61, 0x0000 },
	{ 0x62, 0x0000 },
	{ 0x63, 0x00c0 },
	//{ 0x63, 0xe8d8 },
	{ 0x64, 0x0000 },
	{ 0x65, 0x0000 },
	{ 0x66, 0x0000 },
	{ 0x6a, 0x0000 },
	{ 0x6c, 0x0000 },
	{ 0x70, 0x8000 },
	{ 0x71, 0x8000 },
	{ 0x72, 0x8000 },
	{ 0x73, 0x1114 },
	{ 0x74, 0x0c00 },
	{ 0x75, 0x1d00 },
	{ 0x80, 0x0000 },
	{ 0x81, 0x0000 },
	{ 0x82, 0x0000 },
	{ 0x83, 0x0000 },
	{ 0x84, 0x0000 },
	{ 0x85, 0x0008 },
	{ 0x89, 0x0000 },
	{ 0x8a, 0x0000 },
	{ 0x8b, 0x0600 },
	{ 0x8c, 0x0228 },
	{ 0x8d, 0xa000 },
	{ 0x8e, 0x0004 },
	//{ 0x8e, 0x001f },
	{ 0x8f, 0x1100 },
	//{ 0x8f, 0x3100 },
	{ 0x90, 0x0646 },
	{ 0x91, 0x0c00 },
	{ 0x92, 0x0000 },
	{ 0x93, 0x3000 },
	{ 0xb0, 0x2080 },
	{ 0xb1, 0x0000 },
	{ 0xb4, 0x2206 },
	{ 0xb5, 0x1f00 },
	{ 0xb6, 0x0000 },
	{ 0xb8, 0x034b },
	{ 0xb9, 0x0066 },
	{ 0xba, 0x000b },
	{ 0xbb, 0x0000 },
	{ 0xbc, 0x0000 },
	{ 0xbd, 0x0000 },
	{ 0xbe, 0x0000 },
	{ 0xbf, 0x0000 },
	{ 0xc0, 0x0400 },
	{ 0xc2, 0x0000 },
	{ 0xc4, 0x0000 },
	{ 0xc5, 0x0000 },
	{ 0xc6, 0x2000 },
	{ 0xc8, 0x0000 },
	{ 0xc9, 0x0000 },
	{ 0xca, 0x0000 },
	{ 0xcb, 0x0000 },
	{ 0xcc, 0x0000 },
	{ 0xcf, 0x0013 },
	{ 0xd0, 0x0680 },
	{ 0xd1, 0x1c17 },
	{ 0xd2, 0x8c00 },
	{ 0xd3, 0xaa20 },
	{ 0xd6, 0x0400 },
	{ 0xd9, 0x0809 },
	{ 0xfe, 0x10ec },
	{ 0xff, 0x6231 },
};

MQX_FILE_PTR fd = NULL;

static int rt5640_set_bias_level(enum snd_soc_bias_level level)
{
	switch (level) {
	case SND_SOC_BIAS_STANDBY:
            rt5640_ModifyReg(RT5640_PWR_ANLG1,
                    RT5640_PWR_VREF1 | RT5640_PWR_MB | RT5640_PWR_BG | RT5640_PWR_VREF2,
                    RT5640_PWR_VREF1 | RT5640_PWR_MB | RT5640_PWR_BG | RT5640_PWR_VREF2);
            _time_delay(20);
            rt5640_ModifyReg(RT5640_PWR_ANLG1,
                    RT5640_PWR_FV1 | RT5640_PWR_FV2 | RT5640_PWR_HP_L | RT5640_PWR_HP_R,
                    RT5640_PWR_FV1 | RT5640_PWR_FV2 | RT5640_PWR_HP_L | RT5640_PWR_HP_R);

		break;

	case SND_SOC_BIAS_OFF:
		rt5640_WriteReg(RT5640_DEPOP_M1, 0x0004);
		rt5640_WriteReg(RT5640_DEPOP_M2, 0x1100);
		rt5640_ModifyReg(RT5640_DUMMY1, 0x1, 0);
		rt5640_WriteReg(RT5640_PWR_DIG1, 0x0000);
		rt5640_WriteReg(RT5640_PWR_DIG2, 0x0000);
		rt5640_WriteReg(RT5640_PWR_VOL, 0x0000);
		rt5640_WriteReg(RT5640_PWR_MIXER, 0x0000);
		rt5640_WriteReg(RT5640_PWR_ANLG1, 0x0000);
		rt5640_WriteReg(RT5640_PWR_ANLG2, 0x0000);
		break;

	default:
		break;
	}

	return 0;
}

static int  rt5640_init_regs(void){

    int index = 0;


    for(index = 0; index < sizeof(rt5640_reg) / sizeof (rt5640_reg[0]); index++){
        rt5640_WriteReg(rt5640_reg[index].reg, rt5640_reg[index].def);
    }

#if 1
    for(index = 0; index < sizeof(init_list) / sizeof (init_list[0]); index++){
        rt5640_WriteReg(init_list[index].reg, init_list[index].def);
    }
#endif

    return 0;
}


static _mqx_int rt5640_Init(void)
{
    uint32_t param;
    I2C_STATISTICS_STRUCT stats;
    if (fd == NULL)
    {
        fd = fopen (I2C_DEVICE_POLLED, NULL);
    }
    if (fd == NULL) 
    {
        printf ("ERROR: Unable to open I2C driver!\n");
        return(-9);
    }
    param = 100000;
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_BAUD, &param))
    {
        return(-1);
    }
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_MASTER_MODE, NULL))
    {
        return(-2);
    }
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL))
    {
        return(-3);
    }
    param = RT5640_I2C_ADDR;
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param))
    {
        return(-4);
    }

    /* Power on RT5640 */
    mux_74hc595_set_bit(BSP_74HC595_0, BSP_74HC595_AUD_PWREN);
    mux_74hc595_set_bit(BSP_74HC595_0, BSP_74HC595_AUD_LDO1_EN);
    
    /* Initiate start and send I2C bus address */
    param = fwrite (&param, 1, 0, fd);
    
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_GET_STATISTICS, (void *)&stats))
    {
        return(-5);
    }
    /* Stop I2C transfer */
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
    {
        return(-6);
    }
    /* Check ack (device exists) */
    if (I2C_OK == ioctl (fd, IO_IOCTL_FLUSH_OUTPUT, &param))
    {
        if ((param) || (stats.TX_NAKS)) 
        {
            return(-7);
        }
    }
    else
    {
        return(-8);
    }
    return(MQX_OK);
}

_mqx_int CodecDeinit(void){

    return rt5640_DeInit();
}

static _mqx_int rt5640_DeInit(void){

    /* Power down RT5640 */
    mux_74hc595_clear_bit(BSP_74HC595_0, BSP_74HC595_AUD_PWREN);
    mux_74hc595_clear_bit(BSP_74HC595_0, BSP_74HC595_AUD_LDO1_EN);

    if(fd != NULL){
        fclose(fd);
        fd = NULL;
    }
    return 0;
}
/*FUNCTION****************************************************************
* 
* Function Name    : rt5640_WriteReg
* Returned Value   : MQX error code
* Comments         :
*    Writes a value to the entire register. All
*    bit-fields of the register will be written.
*
*END*********************************************************************/

static _mqx_int rt5640_WriteReg(uint16_t reg, uint16_t reg_val){

    _mqx_int ret = MQX_OK;

    if(reg & RT5640_PR_BASE){

      ret = _rt5640_WriteReg(RT5640_PRIV_INDEX, reg - RT5640_PR_BASE);
      if(ret == MQX_OK)
          ret = _rt5640_WriteReg(RT5640_PRIV_DATA,  reg_val);

    }
    else{
        ret = _rt5640_WriteReg((uint8_t)reg, reg_val);
    }
    return ret;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _rt5640_WriteReg
* Returned Value   : MQX error code
* Comments         :
*    Writes a value to the entire register. All
*    bit-fields of the register will be written.
*
*END*********************************************************************/
static _mqx_int _rt5640_WriteReg(uint8_t reg, uint16_t reg_val)
{
    uint8_t buffer[3];
    uint32_t result;
    buffer[0] = reg;
    buffer[1] =	(uint8_t)((reg_val >> 8) & 0xFF);
    buffer[2] =	(uint8_t)(reg_val & 0xFF);
    result = write(fd, buffer, 3); 
    if (3 != result)
    {
        return(-1);
    } 
    result = fflush (fd);
    if (MQX_OK != result)
    {
        return(-3);
    } 
    /* Stop I2C transfer */
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
    {
        return(-2);
    }
    result = 0;
    return(MQX_OK);
}

/*FUNCTION****************************************************************
* 
* Function Name    : rt5640_ReadReg
* Returned Value   : MQX error code
* Comments         :
*    Reads value of register. 
*
*END*********************************************************************/
static _mqx_int rt5640_ReadReg(uint16_t reg, uint16_t *dest_ptr){

    _mqx_int ret = MQX_OK;

    if(reg & RT5640_PR_BASE){

        ret = _rt5640_WriteReg(RT5640_PRIV_INDEX, reg - RT5640_PR_BASE);
        if(ret == MQX_OK)
            ret = _rt5640_ReadReg(RT5640_PRIV_DATA,  dest_ptr);
    }
    else{
        ret = _rt5640_ReadReg((uint8_t)reg, dest_ptr);
    }

    //printf("reg 0x%02x value is 0x%04x\n", reg, *dest_ptr);

    return ret;
}

/*FUNCTION****************************************************************
* 
* Function Name    : _rt5640_ReadReg
* Returned Value   : MQX error code
* Comments         :
*    Reads value of register. 
*
*END*********************************************************************/
static _mqx_int _rt5640_ReadReg(uint8_t reg, uint16_t *dest_ptr)
{
    uint8_t buffer[2];
    uint32_t result, param;
    buffer[0] = reg;
    result = write(fd, buffer, 1);
    if (1 != result)
    {
        return(-1);
    }
    result = fflush (fd);
    if (MQX_OK != result)
    {
        return(-6);
    } 
    /* Send repeated start */
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_REPEATED_START, NULL))
    {
        return(-2);
    }
    /* Set read request */
    param = 2;
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
    {
        return(-3);
    }
    result = 0;
    /* Read all data */ 
    result = read (fd, buffer, 2);
    if (2 != result)
    {
        return(-4);
    }
    result = fflush (fd);
    if (MQX_OK != result)
    {
        return(-7);
    } 
    *dest_ptr = (buffer[1] & 0xFFFF) | ((buffer[0] & 0xFFFF) << 8);
    /* Stop I2C transfer */
    if (I2C_OK != ioctl (fd, IO_IOCTL_I2C_STOP, NULL))
    {
        return(-5);
    }
    return (MQX_OK);
}

/*FUNCTION****************************************************************
* 
* Function Name    : rt5640_ModifyReg
* Returned Value   : MQX error code
* Comments         :
*    Modifies value of register. Bits to set to zero are defined by first 
*	 mask, bits to be set to one are defined by second mask.
*
*END*********************************************************************/
static _mqx_int rt5640_ModifyReg(uint16_t reg, uint16_t clr_mask, uint16_t set_mask)
{
    uint16_t reg_val = 0;
    if (MQX_OK != rt5640_ReadReg(reg, &reg_val))
    {
        return(-1);
    }
    reg_val &= ~clr_mask;
    reg_val |= set_mask;
    if (MQX_OK != rt5640_WriteReg(reg, reg_val))
    {
        return(-2);
    }
    return(MQX_OK);	
}

/*FUNCTION****************************************************************
* 
* Function Name    : InitCodec
* Returned Value   : MQX error code
* Comments         :
*     
*
*END*********************************************************************/
_mqx_int CodecInit()
{
    MQX_FILE_PTR i2s_ptr = NULL;
    _mqx_uint errcode = 0;
    uint32_t fs_freq = 44100;
    uint32_t mclk_freq = fs_freq * CLK_MULT;
#if BSPCFG_ENABLE_SAI
    uint8_t mode = (I2S_TX_MASTER | I2S_RX_SLAVE);
#else
    uint8_t mode = I2S_MODE_MASTER;
#endif

    uint16_t reg_val = 0;

    i2s_ptr = fopen(AUDIO_DEVICE, "w");
    
    if (i2s_ptr == NULL)
    {
        printf("\n  InitCodec error: Unable to open audio device.");
        return(0xDEAD);
    }
    
    ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MODE_MASTER, &mode);
    ioctl(i2s_ptr, IO_IOCTL_I2S_SET_MCLK_FREQ, &mclk_freq);
    ioctl(i2s_ptr, IO_IOCTL_I2S_SET_FS_FREQ, &fs_freq);
    _time_delay(50);

    errcode = rt5640_Init();
    if (errcode != MQX_OK)
    {
        if (fclose(i2s_ptr) != MQX_OK)
        {
            printf("\n InitCodec error: Unable to close audio device.");
        }
        return (errcode);
    }

    /* 5640 identify */
    rt5640_ReadReg(RT5640_VENDOR_ID2, &reg_val);
    if (reg_val != RT5640_DEVICE_ID) {
        printf("Device with ID register %x is not rt5640/39\n", reg_val);
        return -1;
    }

    /* Reset device */
    rt5640_WriteReg(RT5640_RESET, 0);

    /* Init regs */
    rt5640_init_regs();

    /* bias ? */
    rt5640_set_bias_level(SND_SOC_BIAS_STANDBY);


    rt5640_ModifyReg(RT5640_DUMMY1, 0x0301, 0x0301);
    rt5640_ModifyReg(RT5640_MICBIAS, 0x0030, 0x0030);
    /* Select Control for I2S2 ADC Channel: Bypass DSP */
    rt5640_ModifyReg(RT5640_DSP_PATH2, 0xfc00, 0x0c00);  

    rt5640_ReadReg(RT5640_RESET, &reg_val);
    switch ( reg_val & RT5640_ID_MASK) {
        case RT5640_ID_5640:
            printf("RT5640 is identified \n");
            break;
        case RT5640_ID_5642:
            printf("RT5642 is identified \n");
            break;
        case RT5640_ID_5639:
            printf("RT5639 is identified \n");
            break;
    }


    /* Enable I2S1 */
    rt5640_ModifyReg(RT5640_PWR_DIG1,
            RT5640_PWR_I2S1 | RT5640_PWR_DAC_L1 | RT5640_PWR_DAC_R1,
            RT5640_PWR_I2S1 | RT5640_PWR_DAC_L1 | RT5640_PWR_DAC_R1);



    /* Clock Config */
    /* Sys Clock From MCLK -> 256FS */
    reg_val = RT5640_SCLK_SRC_MCLK;         
    rt5640_ModifyReg(RT5640_GLB_CLK,
            RT5640_SCLK_SRC_MASK, 
            reg_val);


    reg_val = 0;
    /* Digital Interface Control */
    reg_val  |= RT5640_I2S_MS_S;      /* Slave Mode */
    reg_val  |= RT5640_I2S_BP_NOR;    /* BCLK Polarity Control as Normal  */
    reg_val  |= RT5640_I2S_DL_16;     /* I2S1 Data Length Selection: 16bit */ 
    reg_val  |= RT5640_I2S_DF_I2S;    /* I2S1 PCM Data Format Selection: I2S format */
    rt5640_ModifyReg(RT5640_I2S1_SDP,
            RT5640_I2S_MS_MASK | RT5640_I2S_BP_MASK | RT5640_I2S_DL_MASK | RT5640_I2S_DF_MASK, 
            reg_val);


    reg_val = 0;
    /* ADC/DAC Clock Control 1*/
    //reg_val |= RT5640_I2S_BCLK_MS1_32;
    reg_val |= RT5640_I2S_PD1_1;      /* I2S Clock Pre-Divider 1  000'b: /1 */
    reg_val |= RT5640_DAC_OSR_32;
    rt5640_ModifyReg(RT5640_ADDA_CLK1, 
            //RT5640_I2S_BCLK_MS1_MASK | 
            RT5640_I2S_PD1_MASK | RT5640_DAC_OSR_MASK,
            reg_val);


#if 0 /* Avoid noise */
    rt5640_ModifyReg(RT5640_PWR_MIXER,
            RT5640_PWR_OM_L | RT5640_PWR_OM_R,
            RT5640_PWR_OM_L | RT5640_PWR_OM_R);
#endif

    /* Dig inf 1 -> Sto DAC mixer  
     * DACL1 -> Stereo_DAC_Mixer_L  DACR1 -> Stereo_DAC_Mixer_R   */
    reg_val = 0x1414;
    rt5640_WriteReg(RT5640_STO_DAC_MIXER, reg_val);
                    
    /* DACL1 -> OUTMIXL */
    reg_val = 0x1fe;  /* Unmute DACL1 */
    rt5640_WriteReg(RT5640_OUT_L3_MIXER, reg_val);
    /* DACR1 -> OUTMIXR */
    reg_val = 0x1fe;  /* Unmute DACR1 */
    rt5640_WriteReg(RT5640_OUT_R3_MIXER, reg_val);

    /* DACL1 -> SPKMIXL */
    /* mute   : 1'b,  bit 1 OUTMIXL, bit2 DACL2, bit4 INL, bit5 RECMIXL,
     * unmute : 0'b   bit 3 DACL1 */
    reg_val = 0x0036; 
    rt5640_WriteReg(RT5640_SPK_L_MIXER, reg_val);
    /* DACR1 -> SPKMIXR */
    /* mute   : 1'b,  bit 1 OUTMIXR, bit2 DACR2, bit4 INR, bit5 RECMIXR,
     * unmute : 0'b   bit 3 DACLR */
    reg_val = 0x0036; 
    rt5640_WriteReg(RT5640_SPK_R_MIXER, reg_val);



    /* OUTMIX L/R -> HPVOL */
    /* bit 14: Unmute for Left Headphone Output Port, bit 6:Unmute for Right Headphone Output Port */
    /* MAX VOl for HP */
    reg_val = 0x8080; 
    rt5640_WriteReg(RT5640_HP_VOL, reg_val);


    /* SPKMIX L/R -> SPKVOLL */
    /* bit 14: Unmute for Left Speaker Volume Channel, bit 6:Unmute for Right Speaker Volume Channel */
    /* MAX VOl for Speaker */
    reg_val = 0x8080; 
    rt5640_WriteReg(RT5640_SPK_VOL, reg_val);


    /* bit 13: Unmute Control for HPOVOL to HPOMIX  Left and Right */
    reg_val = 0xc000; 
    rt5640_WriteReg(RT5640_HPO_MIXER, reg_val);

    /* bit 12: 0'b Unmute Control for SPKVOL L to SPOLMIX  Left 
     * bit 15  1'b mute DACR1   to SPOLMIX
     * bit 14  1'b mute DACL1   to SPOLMIX
     * bit 13  1'b mute SPKVOLR to SPOLMIX
     * bit 11  1'b mute BST1    to SPOLMIX  */
    reg_val = 0xe800; 
    rt5640_WriteReg(RT5640_SPO_L_MIXER, reg_val);

    /* bit 12: 0'b Unmute Control for SPKVOL R to SPOLMIX Right
     * bit 13  1'b mute DCAR1   to SPORMIX
     * bit 11  1'b mute BST1    to SPORMIX  */
    reg_val = 0x2800; 
    rt5640_WriteReg(RT5640_SPO_R_MIXER, reg_val);


#if 1
    /* Power up ? */
    /* depop parameters ? */
    rt5640_ModifyReg(RT5640_PR_BASE + RT5640_CHPUMP_INT_REG1, 
            (uint16_t)0x0700, 
            (uint16_t)0x0200);
    rt5640_ModifyReg(RT5640_DEPOP_M2, 
            RT5640_DEPOP_MASK, 
            RT5640_DEPOP_MAN);
    rt5640_ModifyReg(RT5640_DEPOP_M1, 
            RT5640_HP_CP_MASK | RT5640_HP_SG_MASK | RT5640_HP_CB_MASK,
            RT5640_HP_CP_PU | RT5640_HP_SG_DIS | RT5640_HP_CB_PU);
    rt5640_WriteReg(RT5640_PR_BASE + RT5640_HP_DCC_INT1,
            (uint16_t)0x9f00);
    /* headphone amp power on */
    rt5640_ModifyReg(RT5640_PWR_ANLG1,
            RT5640_PWR_FV1 | RT5640_PWR_FV2, 0);
    rt5640_ModifyReg(RT5640_PWR_ANLG1,
            RT5640_PWR_HA,
            RT5640_PWR_HA);
    rt5640_ModifyReg(RT5640_PWR_ANLG1,
            RT5640_PWR_FV1 | RT5640_PWR_FV2 ,
            RT5640_PWR_FV1 | RT5640_PWR_FV2);

    rt5640_ModifyReg(RT5640_DEPOP_M2,
            RT5640_DEPOP_MASK | RT5640_DIG_DP_MASK,
            RT5640_DEPOP_AUTO | RT5640_DIG_DP_EN);
    rt5640_ModifyReg(RT5640_CHARGE_PUMP,
            RT5640_PM_HP_MASK, RT5640_PM_HP_HV);

    rt5640_ModifyReg(RT5640_DEPOP_M3,
            RT5640_CP_FQ1_MASK | RT5640_CP_FQ2_MASK | RT5640_CP_FQ3_MASK,
            (RT5640_CP_FQ_192_KHZ << RT5640_CP_FQ1_SFT) |
            (RT5640_CP_FQ_12_KHZ << RT5640_CP_FQ2_SFT) |
            (RT5640_CP_FQ_192_KHZ << RT5640_CP_FQ3_SFT));

    rt5640_WriteReg(RT5640_PR_BASE +
            RT5640_MAMP_INT_REG2, 0x1c00);
    rt5640_ModifyReg(RT5640_DEPOP_M1,
            RT5640_HP_CP_MASK | RT5640_HP_SG_MASK,
            RT5640_HP_CP_PD | RT5640_HP_SG_EN);
    rt5640_ModifyReg(RT5640_PR_BASE +
            RT5640_CHPUMP_INT_REG1, 0x0700, 0x0400);



#if 0
    /* Enable HP VOL adjust */
    rt5640_ModifyReg(RT5640_PWR_VOL,
            RT5640_PWR_HV_L | RT5640_PWR_HV_R,
            RT5640_PWR_HV_L | RT5640_PWR_HV_R);
#endif

    rt5640_ModifyReg(RT5640_PWR_DIG1, RT5640_PWR_CLS_D, RT5640_PWR_CLS_D);
    rt5640_ModifyReg(RT5640_PR_BASE + RT5640_CLSD_INT_REG1, 0xf000, 0xf000);
    rt5640_ModifyReg(RT5640_SPK_VOL, RT5640_L_MUTE | RT5640_R_MUTE, 0);


    /* Unmute */
    _time_delay(5);
    rt5640_ModifyReg(RT5640_HP_VOL,
            RT5640_L_MUTE | RT5640_R_MUTE, 
            0);
    _time_delay(80);
    

#endif


    if (fclose(i2s_ptr) != MQX_OK)
    {
        printf("\n  InitCodec error: Unable to close audio device.");
    }
    
    return (0);
}

/*FUNCTION****************************************************************
* 
* Function Name    : SetupCodec
* Returned Value   : MQX error code
* Comments         :
*     
*
*END*********************************************************************/
static _mqx_int SetupCodec(MQX_FILE_PTR device)
{ 
    uint32_t mclk_freq;
    uint32_t fs_freq;
    uint32_t multiple;
    _mqx_int errcode = 0;
    uint16_t mask_ones, mask_zeros;
    uint8_t mode = 0;
    
    ioctl(device, IO_IOCTL_I2S_GET_MODE, &mode);
    ioctl(device, IO_IOCTL_I2S_GET_MCLK_FREQ, &mclk_freq);
    ioctl(device, IO_IOCTL_I2S_GET_FS_FREQ, &fs_freq);

    return(0);
}

int32_t CodecVolDown()
{
    uint16_t reg_val;
    uint8_t  vol;

    rt5640_ReadReg(RT5640_HP_VOL, &reg_val);

    vol = (reg_val & RT5640_L_VOL_MASK) >> RT5640_L_VOL_SFT;

    if(vol == 39){
        return -1;
    }
    else{
        vol  = vol > 35 ? 39 : vol + 4;
        rt5640_ModifyReg(RT5640_HP_VOL,
                RT5640_L_VOL_MASK | RT5640_R_VOL_MASK,
                (vol << RT5640_L_VOL_SFT) | (vol << RT5640_R_VOL_SFT) );

    }
    return 0;
}


int32_t CodecVolUp()
{		
    uint16_t reg_val;
    uint8_t  vol;

    rt5640_ReadReg(RT5640_HP_VOL, &reg_val);

    vol = (reg_val & RT5640_L_VOL_MASK) >> RT5640_L_VOL_SFT;

    if(vol == 0){
        return -1;
    }
    else{
        vol = vol > 4 ? vol - 4 : 0;
        rt5640_ModifyReg(RT5640_HP_VOL,
                RT5640_L_VOL_MASK | RT5640_R_VOL_MASK,
                (vol << RT5640_L_VOL_SFT) | (vol << RT5640_R_VOL_SFT) );

    }

    return 0;
}

int32_t CodecMute(void)
{
    rt5640_ModifyReg(RT5640_HP_VOL,
            RT5640_L_MUTE | RT5640_R_MUTE,
            RT5640_L_MUTE | RT5640_R_MUTE);
    return 0;
}

int32_t CodecUmute(void)
{
    rt5640_ModifyReg(RT5640_HP_VOL,
            RT5640_L_MUTE | RT5640_R_MUTE, 
            0);
    _time_delay(80);
    return 0;
}

/* EOF */
