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

#ifndef __ADC_PIN__H_
#define __ADC_PIN__H_

int unit_test_adc_pin();

// about dac register
/* DAXx registers reset values*/
#define DACx_DAT_RESET 0
#define DACx_SR_RESET  2
#define DACx_C0_RESET  0
#define DACx_C1_RESET  0
#define DACx_C2_RESET  15 //0x0f

/* DACx_C0 bits definition*/ 

#define DAC_DISABLE   0x00
#define DAC_ENABLE    DAC_C0_DACEN_MASK

#define DAC_SEL_VREFO  0x00
#define DAC_SEL_VDDA   DAC_C0_DACRFS_MASK 

#define DAC_SEL_PDB_HW_TRIG  0x00
#define DAC_SEL_SW_TRIG  DAC_C0_DACTRGSEL_MASK 

#define DAC_SW_TRIG_STOP 0x00
#define DAC_SW_TRIG_NEXT  DAC_C0_DACSWTRG_MASK 

#define DAC_HP_MODE  0x00
#define DAC_LP_MODE  DAC_C0_LPEN_MASK 

#define DAC_BFWM_INT_DISABLE  0x00
#define DAC_BFWM_INT_ENABLE   DAC_C0_DACBWIEN_MASK

#define DAC_BFT_PTR_INT_DISABLE 0x00
#define DAC_BFT_PTR_INT_ENABLE DAC_C0_DACBTIEN_MASK

#define DAC_BFB_PTR_INT_DISABLE 0x00
#define DAC_BFB_PTR_INT_ENABLE DAC_C0_DACBBIEN_MASK

/* DACx_C1 bits definition*/ 
#define DAC_DMA_DISABLE  0x00
#define DAC_DMA_ENABLE DAC_C1_DMAEN_MASK 

#define DAC_BFWM_1WORD DAC_C1_DACBFWM(0)  
#define DAC_BFWM_2WORDS DAC_C1_DACBFWM(1) 
#define DAC_BFWM_3WORDS DAC_C1_DACBFWM(2)  
#define DAC_BFWM_4WORDS DAC_C1_DACBFWM(3) 

#define DAC_BF_NORMAL_MODE DAC_C1_DACBFMD(0)
#define DAC_BF_SWING_MODE DAC_C1_DACBFMD(1) 
#define DAC_BF_ONE_TIME_MODE DAC_C1_DACBFMD(2)

#define DAC_BF_DISABLE 0x00
#define DAC_BF_ENABLE DAC_C1_DACBFEN_MASK 

/* DACx_C2 bits definition*/ 
#define DAC_SET_PTR_AT_BF(x) DAC_C2_DACBFRP(x)
#define DAC_SET_PTR_UP_LIMIT(x) DAC_C2_DACBFUP(x)

#define Clear_DACBFWMF  0x03
#define Clear_DACBFRPTF 0x05
#define Clear_DACBFRPBF 0x06



#endif

/* EOF */
