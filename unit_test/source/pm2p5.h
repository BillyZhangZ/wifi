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
 *   This file contains mass storage class application types and definitions.
 *
 *END************************************************************************/

#ifndef _PM2P5_H_
#define _PM2P5_H_

// adc
#define ADC_CFG2_ADLSTS_20             ADC_CFG2_ADLSTS(0)
#define ADC_CFG2_ADLSTS_12             ADC_CFG2_ADLSTS(1)
#define ADC_CFG2_ADLSTS_6              ADC_CFG2_ADLSTS(2)
#define ADC_CFG2_ADLSTS_2              ADC_CFG2_ADLSTS(3)
#define ADC_CFG2_ADLSTS_DEFAULT       (ADC_CFG2_ADLSTS_20)

#define ADC_SC3_AVGS_4                 ADC_SC3_AVGS(0x00)
#define ADC_SC3_AVGS_8                 ADC_SC3_AVGS(0x01)
#define ADC_SC3_AVGS_16                ADC_SC3_AVGS(0x02)
#define ADC_SC3_AVGS_32                ADC_SC3_AVGS(0x03)
#define ADC_SC3_AVGE                   ADC_SC3_AVGE_MASK
#define ADC_SC3_ADCO                   ADC_SC3_ADCO_MASK
#define ADC_SC3_CALF                   ADC_SC3_CALF_MASK
#define ADC_SC3_CAL                    ADC_SC3_CAL_MASK

#define ADC_SC2_REFSEL_VREF            ADC_SC2_REFSEL(0x00)
#define ADC_SC2_REFSEL_VALT            ADC_SC2_REFSEL(0x01)
#define ADC_SC2_REFSEL_VBG             ADC_SC2_REFSEL(0x02)

#define ADC_SC1_ADCH_DISABLED          ADC_SC1_ADCH(0x1F)

#define ADC_CFG1_ADIV_1                ADC_CFG1_ADIV(0x00)
#define ADC_CFG1_ADIV_2                ADC_CFG1_ADIV(0x01)
#define ADC_CFG1_ADIV_4                ADC_CFG1_ADIV(0x02)
#define ADC_CFG1_ADIV_8                ADC_CFG1_ADIV(0x03)

#define ADC_CFG1_ADICLK_BUSCLK         ADC_CFG1_ADICLK(0x00)
#define ADC_CFG1_ADICLK_BUSCLK2        ADC_CFG1_ADICLK(0x01)
#define ADC_CFG1_ADICLK_ALTCLK         ADC_CFG1_ADICLK(0x02)
#define ADC_CFG1_ADICLK_ADACK          ADC_CFG1_ADICLK(0x03)


//// ADCSC1 (register)

// Conversion Complete (COCO) mask
#define COCO_COMPLETE     ADC_SC1_COCO_MASK
#define COCO_NOT          0x00

// ADC interrupts: enabled, or disabled.
#define AIEN_ON           ADC_SC1_AIEN_MASK
#define AIEN_OFF          0x00

// Differential or Single ended ADC input
#define DIFF_SINGLE       0x00
#define DIFF_DIFFERENTIAL ADC_SC1_DIFF_MASK

//// ADCCFG1
// Power setting of ADC
#define ADLPC_LOW         ADC_CFG1_ADLPC_MASK
#define ADLPC_NORMAL      0x00

// Clock divisor
#define ADIV_1            0x00
#define ADIV_2            0x01
#define ADIV_4            0x02
#define ADIV_8            0x03

// Long samle time, or Short sample time
#define ADLSMP_LONG       ADC_CFG1_ADLSMP_MASK
#define ADLSMP_SHORT      0x00

// How many bits for the conversion?  8, 12, 10, or 16 (single ended).
#define MODE_8            0x00
#define MODE_12           0x01
#define MODE_10           0x02
#define MODE_16           0x03

// ADC Input Clock Source choice? Bus clock, Bus clock/2, "altclk", or the 
//                                ADC's own asynchronous clock for less noise
#define ADICLK_BUS        0x00
#define ADICLK_BUS_2      0x01
#define ADICLK_ALTCLK     0x02
#define ADICLK_ADACK      0x03

//// ADCCFG2

// Select between B or A channels
#define MUXSEL_ADCB       ADC_CFG2_MUXSEL_MASK
#define MUXSEL_ADCA       0x00

// Ansync clock output enable: enable, or disable the output of it
#define ADACKEN_ENABLED   ADC_CFG2_ADACKEN_MASK
#define ADACKEN_DISABLED  0x00

// High speed or low speed conversion mode
#define ADHSC_HISPEED     ADC_CFG2_ADHSC_MASK
#define ADHSC_NORMAL      0x00

// Long Sample Time selector: 20, 12, 6, or 2 extra clocks for a longer sample time
#define ADLSTS_20          0x00
#define ADLSTS_12          0x01
#define ADLSTS_6           0x02
#define ADLSTS_2           0x03

////ADCSC2

// Read-only status bit indicating conversion status
#define ADACT_ACTIVE       ADC_SC2_ADACT_MASK
#define ADACT_INACTIVE     0x00

// Trigger for starting conversion: Hardware trigger, or software trigger.
// For using PDB, the Hardware trigger option is selected.
#define ADTRG_HW           ADC_SC2_ADTRG_MASK
#define ADTRG_SW           0x00

// ADC Compare Function Enable: Disabled, or Enabled.
#define ACFE_DISABLED      0x00
#define ACFE_ENABLED       ADC_SC2_ACFE_MASK

// Compare Function Greater Than Enable: Greater, or Less.
#define ACFGT_GREATER      ADC_SC2_ACFGT_MASK
#define ACFGT_LESS         0x00

// Compare Function Range Enable: Enabled or Disabled.
#define ACREN_ENABLED      ADC_SC2_ACREN_MASK
#define ACREN_DISABLED     0x00

// DMA enable: enabled or disabled.
#define DMAEN_ENABLED      ADC_SC2_DMAEN_MASK
#define DMAEN_DISABLED     0x00

// Voltage Reference selection for the ADC conversions
// (***not*** the PGA which uses VREFO only).
// VREFH and VREFL (0) , or VREFO (1).

#define REFSEL_EXT         0x00
#define REFSEL_ALT         0x01
#define REFSEL_RES         0x02     /* reserved */
#define REFSEL_RES_EXT     0x03     /* reserved but defaults to Vref */

////ADCSC3

// Calibration begin or off
#define CAL_BEGIN          ADC_SC3_CAL_MASK
#define CAL_OFF            0x00

// Status indicating Calibration failed, or normal success
#define CALF_FAIL          ADC_SC3_CALF_MASK
#define CALF_NORMAL        0x00

// ADC to continously convert, or do a sinle conversion
#define ADCO_CONTINUOUS    ADC_SC3_ADCO_MASK
#define ADCO_SINGLE        0x00

// Averaging enabled in the ADC, or not.
#define AVGE_ENABLED       ADC_SC3_AVGE_MASK
#define AVGE_DISABLED      0x00

// How many to average prior to "interrupting" the MCU?  4, 8, 16, or 32
#define AVGS_4             0x00
#define AVGS_8             0x01
#define AVGS_16            0x02
#define AVGS_32            0x03

////PGA

// PGA enabled or not?
#define PGAEN_ENABLED      ADC_PGA_PGAEN_MASK
#define PGAEN_DISABLED     0x00 

// Chopper stabilization of the amplifier, or not.
#define PGACHP_CHOP        ADC_PGA_PGACHP_MASK 
#define PGACHP_NOCHOP      0x00

// PGA in low power mode, or normal mode.
#define PGALP_LOW          ADC_PGA_PGALP_MASK
#define PGALP_NORMAL       0x00

// Gain of PGA.  Selectable from 1 to 64.
#define PGAG_1             0x00
#define PGAG_2             0x01
#define PGAG_4             0x02
#define PGAG_8             0x03
#define PGAG_16            0x04
#define PGAG_32            0x05
#define PGAG_64            0x06

#define ADC_SIG_PORTA   (0x01 << 5)
#define ADC_SIG_PORTB   (0x02 << 5)
#define ADC_SIG_PORTC   (0x03 << 5)
#define ADC_SIG_PORTD   (0x04 << 5)
#define ADC_SIG_PORTE   (0x05 << 5)

// hw tirger with pit
#define PIT_MCR_MDIS      (1<<1)
#define PIT_MCR_FRZ       (1<<0)

#define PIT_TCTRL_TIE     (1<<1)
#define PIT_TCTRL_TEN     (1<<0)

#define PIT_TFLG_TIF      (1<<0)

#define READ_ADC_TIMES      30  /* test read times */
//#define ADC_HW_TRIGER    /* test it use pit 3 */


#define SAMPLE_INT                       INT_PIT1
#define SAMPLE_PIT_CH                     1 
#define _sample_timer_mask_int           _qpit_mask_int
#define _sample_timer_unmask_int         _qpit_unmask_int
#define _sample_timer_init_freq          _qpit_init_freq
#define _sample_clear_int                _qpit_clear_int


int unit_test_pm2p5();

#endif   /* _PM2P5_H_ */

/* EOF */
