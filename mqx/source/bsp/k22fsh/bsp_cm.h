/**HEADER***********************************************************************
*
* Copyright (c) 2011 Freescale Semiconductor;
* All Rights Reserved
*
********************************************************************************
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
********************************************************************************
*
* $FileName: bsp_cm.h$
* $Version : 3.8.1.0$
* $Date    : Sep-21-2012$
*
* Comments:
*   Clock manager Kinetis BSP specific definitions and function prototypes.
*
*       _bsp_initialize_hardware();
*       _bsp_set_clock_configuration();
*       _bsp_get_clock_configuration();
*       _bsp_get_clock();
*
*END***************************************************************************/

#ifndef __bsp_cm_h__
    #define __bsp_cm_h__


#ifndef PE_LDD_VERSION

#define CPU_BUS_CLK_HZ                  60000000U /* Initial value of the bus clock frequency in Hz */
#define CPU_CORE_CLK_HZ                 120000000U /* Initial value of the core/system clock frequency in Hz.  */

#define CPU_CLOCK_CONFIG_NUMBER         0x04U       /* Specifies number of defined clock configurations. */

#define CPU_BUS_CLK_HZ_CLOCK_CONFIG0    40000000U /* Value of the bus clock frequency in the clock configuration 0 in Hz. */
#define CPU_CORE_CLK_HZ_CLOCK_CONFIG0   80000000U /* Value of the core/system clock frequency in the clock configuration 0 in Hz. */

#define CPU_BUS_CLK_HZ_CLOCK_CONFIG1    8000000U /* Value of the bus clock frequency in the clock configuration 1 in Hz. */
#define CPU_CORE_CLK_HZ_CLOCK_CONFIG1   8000000U /* Value of the core/system clock frequency in the clock configuration 1 in Hz. */

#define CPU_BUS_CLK_HZ_CLOCK_CONFIG2    2000000U /* Value of the bus clock frequency in the clock configuration 2 in Hz. */
#define CPU_CORE_CLK_HZ_CLOCK_CONFIG2   2000000U /* Value of the core/system clock frequency in the clock configuration 2 in Hz. */

#define CPU_BUS_CLK_HZ_CLOCK_CONFIG3    60000000U /* Value of the bus clock frequency in the clock configuration 3 in Hz. */
#define CPU_CORE_CLK_HZ_CLOCK_CONFIG3   120000000U /* Value of the core/system clock frequency in the clock configuration 3 in Hz. */


#define CPU_XTAL_CLK_HZ                 24000000U /* Value of the external crystal or oscillator clock frequency in Hz */
#define CPU_XTAL32k_CLK_HZ              32768U /* Value of the external 32k crystal or oscillator clock frequency in Hz */
#define CPU_INT_SLOW_CLK_HZ             32768U /* Value of the slow internal oscillator clock frequency in Hz  */
#define CPU_INT_FAST_CLK_HZ             4000000U /* Value of the fast internal oscillator clock frequency in Hz  */

#define CPU_FAMILY_Kinetis             /* Specification of the core type of the selected cpu */
#define CPU_DERIVATIVE_MK22FN512VDC12   /* Name of the selected cpu derivative */
#define CPU_PARTNUM_MK22FN512VDC12      /* Part number of the selected cpu */
#define CPU_LITTLE_ENDIAN               /* The selected cpu uses little endian */

/* CPU frequencies in clock configuration 0 */
#define CPU_CLOCK_CONFIG_0              0x00U /* Clock configuration 0 identifier */
#define CPU_CORE_CLK_HZ_CONFIG_0        80000000UL /* Core clock frequency in clock configuration 0 */
#define CPU_BUS_CLK_HZ_CONFIG_0         40000000UL /* Bus clock frequency in clock configuration 0 */
#define CPU_FLEXBUS_CLK_HZ_CONFIG_0     20000000UL /* Flexbus clock frequency in clock configuration 0 */
#define CPU_FLASH_CLK_HZ_CONFIG_0       26670000UL /* FLASH clock frequency in clock configuration 0 */
#define CPU_USB_CLK_HZ_CONFIG_0         48000000UL /* USB clock frequency in clock configuration 0 */
#define CPU_PLL_FLL_CLK_HZ_CONFIG_0     80000000UL /* PLL/FLL clock frequency in clock configuration 0 */
#define CPU_MCGIR_CLK_HZ_CONFIG_0       0UL /* MCG internal reference clock frequency in clock configuration 0 */
#define CPU_OSCER_CLK_HZ_CONFIG_0       24000000UL /* System OSC external reference clock frequency in clock configuration 0 */
#define CPU_ERCLK32K_CLK_HZ_CONFIG_0    32768UL /* External reference clock 32k frequency in clock configuration 0 */
#define CPU_MCGFF_CLK_HZ_CONFIG_0       15625UL /* MCG fixed frequency clock */

/* CPU frequencies in clock configuration 1 */
#define CPU_CLOCK_CONFIG_1              0x01U /* Clock configuration 1 identifier */
#define CPU_CORE_CLK_HZ_CONFIG_1        8000000UL /* Core clock frequency in clock configuration 1 */
#define CPU_BUS_CLK_HZ_CONFIG_1         8000000UL /* Bus clock frequency in clock configuration 1 */
#define CPU_FLEXBUS_CLK_HZ_CONFIG_1     8000000UL /* Flexbus clock frequency in clock configuration 1 */
#define CPU_FLASH_CLK_HZ_CONFIG_1       8000000UL /* FLASH clock frequency in clock configuration 1 */
#define CPU_USB_CLK_HZ_CONFIG_1         48000000UL /* USB clock frequency in clock configuration 1 */
#define CPU_PLL_FLL_CLK_HZ_CONFIG_1     80000000UL /* PLL/FLL clock frequency in clock configuration 1 */
#define CPU_MCGIR_CLK_HZ_CONFIG_1       0UL /* MCG internal reference clock frequency in clock configuration 1 */
#define CPU_OSCER_CLK_HZ_CONFIG_1       24000000UL /* System OSC external reference clock frequency in clock configuration 1 */
#define CPU_ERCLK32K_CLK_HZ_CONFIG_1    32768UL /* External reference clock 32k frequency in clock configuration 1 */
#define CPU_MCGFF_CLK_HZ_CONFIG_1       15625UL /* MCG fixed frequency clock */

/* CPU frequencies in clock configuration 2 */
#define CPU_CLOCK_CONFIG_2              0x02U /* Clock configuration 2 identifier */
#define CPU_CORE_CLK_HZ_CONFIG_2        2000000UL /* Core clock frequency in clock configuration 2 */
#define CPU_BUS_CLK_HZ_CONFIG_2         2000000UL /* Bus clock frequency in clock configuration 2 */
#define CPU_FLEXBUS_CLK_HZ_CONFIG_2     2000000UL /* Flexbus clock frequency in clock configuration 2 */
#define CPU_FLASH_CLK_HZ_CONFIG_2       500000UL /* FLASH clock frequency in clock configuration 2 */
#define CPU_USB_CLK_HZ_CONFIG_2         0UL /* USB clock frequency in clock configuration 2 */
#define CPU_PLL_FLL_CLK_HZ_CONFIG_2     0UL /* PLL/FLL clock frequency in clock configuration 2 */
#define CPU_MCGIR_CLK_HZ_CONFIG_2       2000000UL /* MCG internal reference clock frequency in clock configuration 2 */
#define CPU_OSCER_CLK_HZ_CONFIG_2       24000000UL /* System OSC external reference clock frequency in clock configuration 2 */
#define CPU_ERCLK32K_CLK_HZ_CONFIG_2    32768UL /* External reference clock 32k frequency in clock configuration 2 */
#define CPU_MCGFF_CLK_HZ_CONFIG_2       16384UL /* MCG fixed frequency clock */

/* CPU frequencies in clock configuration 3 */
#define CPU_CLOCK_CONFIG_3              0x03U /* Clock configuration 3 identifier */
#define CPU_CORE_CLK_HZ_CONFIG_3        120000000UL /* Core clock frequency in clock configuration 3 */
#define CPU_BUS_CLK_HZ_CONFIG_3         60000000UL /* Bus clock frequency in clock configuration 3 */
#define CPU_FLEXBUS_CLK_HZ_CONFIG_3     24000000UL /* Flexbus clock frequency in clock configuration 3 */
#define CPU_FLASH_CLK_HZ_CONFIG_3       24000000UL /* FLASH clock frequency in clock configuration 3 */
#define CPU_USB_CLK_HZ_CONFIG_3         48000000UL /* USB clock frequency in clock configuration 3 */
#define CPU_PLL_FLL_CLK_HZ_CONFIG_3     120000000UL /* PLL/FLL clock frequency in clock configuration 3 */
#define CPU_MCGIR_CLK_HZ_CONFIG_3       0UL /* MCG internal reference clock frequency in clock configuration 3 */
#define CPU_OSCER_CLK_HZ_CONFIG_3       24000000UL /* System OSC external reference clock frequency in clock configuration 3 */
#define CPU_ERCLK32K_CLK_HZ_CONFIG_3    32768UL /* External reference clock 32k frequency in clock configuration 3 */
#define CPU_MCGFF_CLK_HZ_CONFIG_3       15625UL /* MCG fixed frequency clock */


#endif /* PE_LDD_VERSION */


typedef enum {
    BSP_CLOCK_CONFIGURATION_0 = 0,      /*  80 MHZ PEE   */
    BSP_CLOCK_CONFIGURATION_1,          /*  8 MHz PEE   */
    BSP_CLOCK_CONFIGURATION_2,          /*  2 MHz BLPI  */
    BSP_CLOCK_CONFIGURATION_3,          /* 120 MHZ PEE   */
    BSP_CLOCK_CONFIGURATIONS,           /* Number of available clock configurations */
    BSP_CLOCK_CONFIGURATION_AUTOTRIM    = BSP_CLOCK_CONFIGURATION_1,
    BSP_CLOCK_CONFIGURATION_DEFAULT     = BSP_CLOCK_CONFIGURATION_0,
    BSP_CLOCK_CONFIGURATION_120MHZ      = BSP_CLOCK_CONFIGURATION_3,
    BSP_CLOCK_CONFIGURATION_80MHZ       = BSP_CLOCK_CONFIGURATION_0,
    BSP_CLOCK_CONFIGURATION_8MHZ        = BSP_CLOCK_CONFIGURATION_1,
    BSP_CLOCK_CONFIGURATION_2MHZ        = BSP_CLOCK_CONFIGURATION_2
} BSP_CLOCK_CONFIGURATION;

#ifdef __cplusplus
extern "C" {
#endif


void _bsp_initialize_hardware
(
    void
);

BSP_CLOCK_CONFIGURATION _bsp_get_clock_configuration
(
    void
);

uint16_t _bsp_set_operation_mode(LDD_TDriverOperationMode OperationMode);

uint16_t _bsp_set_clock_configuration
(
    /* [IN] runtime clock configuration */
    const BSP_CLOCK_CONFIGURATION clock_configuration
);


uint32_t _bsp_get_clock
(
    /* [IN] clock configuration */
    const BSP_CLOCK_CONFIGURATION   clock_configuration,
    /* [IN] clock source index */
    const CM_CLOCK_SOURCE           clock_source
);

void _bsp_low_level_init(void);


#ifdef __cplusplus
}
#endif


#endif /* __bsp_cm_h__ */
/* EOF */

