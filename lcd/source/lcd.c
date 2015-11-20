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

#include <mqx.h>
#include <bsp.h>
#include <spi.h>
//#include "lcd.h"
#include <fio.h>

#include "74hc595.h"

#include "st_font.h"

#define BSP_LCD_PIN_74HC595
#define BSP_LCD_SPI_CS_74HC595                         (1 << BSP_74HC595_SPI_S1)

#ifndef BSP_LCD_PIN_74HC595
/* pin mux setting */
#define BSP_LCD_PWREN			(GPIO_PORT_D | GPIO_PIN5)  /* LCD_PWREN */
#define BSP_LCD_RST				(GPIO_PORT_D | GPIO_PIN0)  /* LCD_nRST */
#define BSP_LCD_BL				(GPIO_PORT_D | GPIO_PIN1)  /* BLT_PWREN */
#define BSP_LCD_CS				(GPIO_PORT_D | GPIO_PIN4)  /* SPI1_PCS0 */
#endif
#define BSP_LCD_A0				(GPIO_PORT_D | GPIO_PIN7)  /* SPI1_MISO (LCD_C/D)  SPI1.SIN */

#ifndef BSP_LCD_PIN_74HC595
#define BSP_LCD_PWREN_MUX_GPIO  (LWGPIO_MUX_D5_GPIO)
#define BSP_LCD_RST_MUX_GPIO 	(LWGPIO_MUX_D0_GPIO)
#define BSP_LCD_BL_MUX_GPIO 	(LWGPIO_MUX_D1_GPIO)
#define BSP_LCD_CS_MUX_GPIO 	(LWGPIO_MUX_D4_GPIO)
#endif
#define BSP_LCD_A0_MUX_GPIO 	(LWGPIO_MUX_D7_GPIO)


#define ON 			1
#define OFF			0
#define NOR			0
#define INV			1
#define SAVE			1
#define WAKE			0

#define LCD_POWER_ON		0x2F
#define LCD_POWER_OFF		0x28

#define LCD_DISPLAY_ON		0xAF
#define LCD_DISPLAY_OFF		0xAE

#define LCD_DISPMOD_INV		0xA7
#define LCD_DISPMOD_NOR		0xA6

#define LCD_SCANDIR_INV		0xA1
#define LCD_SCANDIR_NOR		0xA0

#define LCD_ALLPIXEL_ON		0xA5
#define LCD_ALLPIXEL_OFF	0xA4

#define LCD_SOFTWARE_RESET	0xE2

#define MAX_COLUME 16   /* 0 - 15 */
#define MAX_ROW 4       /* 0 - 3 */

#define CMD_A0     0
#define DATA_A0     1

#define LCD_DISP_START_COLUME   127 // 131

typedef struct _console_t_
{
    int row,col;  /* */
    uint8_t content[MAX_ROW][MAX_COLUME + 1];  /* +1 place \0 */
    MQX_FILE_PTR spifd;
} console_t;

static const uint8_t page_idx[] = {3*2,2*2,1*2,0*2} ;  /* index is display row */
//static const uint8_t page_idx[] = {0*2,1*2,2*2,3*2} ;  /* index is display row */
static console_t console;


static void LCD_gpio_init(MQX_FILE_PTR spifd);
static void LCD_send_byte(MQX_FILE_PTR spifd, uint8_t RS, uint8_t RW, uint8_t data);
static void LCD_send_character(MQX_FILE_PTR spifd, uint8_t page, uint8_t addr, uint8_t *character);

static void LCD_set_regcommand(MQX_FILE_PTR spifd, uint8_t regcommand);
static void LCD_set_startline(MQX_FILE_PTR spifd, uint8_t startline);
static void LCD_set_pageaddress(MQX_FILE_PTR spifd, uint8_t pageaddress);
static void LCD_set_columnaddr_high(MQX_FILE_PTR spifd, uint8_t columnaddr_h);
static void LCD_set_columnaddr_low(MQX_FILE_PTR spifd, uint8_t columnaddr_l);
static void LCD_write_ramdata(MQX_FILE_PTR spifd, uint8_t ramdata);

static void LCD_turn_backlight(MQX_FILE_PTR spifd, uint8_t on_off);
static void LCD_turn_display(MQX_FILE_PTR spifd, uint8_t on_off);
static void LCD_set_dispmod(MQX_FILE_PTR spifd, uint8_t inv_nor);
static void LCD_scan_direction(MQX_FILE_PTR spifd, uint8_t inv_nor);
static void LCD_save_power(MQX_FILE_PTR spifd, uint8_t save_wake);
static void LCD_power_control(MQX_FILE_PTR spifd, uint8_t on_off);
static void LCD_software_reset(MQX_FILE_PTR spifd);

static void LCD_clean_full_screen(MQX_FILE_PTR spifd);
static void LCD_display_string(MQX_FILE_PTR spifd, uint8_t row, uint8_t col, char *string);

static LWGPIO_STRUCT rstgpio, blgpio, a0gpio, pwrgpio;

/*
#if ! BSPCFG_ENABLE_SPI0
	#error This application requires BSPCFG_ENABLE_SPI0 defined non-zero in user_config.h. Please recompile kernel with this option.
#endif

#if ! BSPCFG_ENABLE_SPI1
	#error This application requires BSPCFG_ENABLE_SPI1 defined non-zero in user_config.h. Please recompile kernel with this option.
#endif
*/

const char *device_mode[] =
{
    "SPI_DEVICE_MASTER_MODE",
    "SPI_DEVICE_SLAVE_MODE",
};

const char *clock_mode[] =
{
    "SPI_CLK_POL_PHA_MODE0",
    "SPI_CLK_POL_PHA_MODE1",
    "SPI_CLK_POL_PHA_MODE2",
    "SPI_CLK_POL_PHA_MODE3"
};

// data - high
static inline void cmd_data_sel(uint8_t RS)
{
    /* lcd cmd & data selcect */
    if(RS)  /* data */
	lwgpio_set_value(&a0gpio, LWGPIO_VALUE_HIGH);
    else   /* command */ 
	lwgpio_set_value(&a0gpio, LWGPIO_VALUE_LOW);
}

// enable - high
static inline void power_enable(uint8_t enable)
{
#ifndef BSP_LCD_PIN_74HC595
	if(enable)
          lwgpio_set_value(&pwrgpio, LWGPIO_VALUE_HIGH);
	else
          lwgpio_set_value(&pwrgpio, LWGPIO_VALUE_LOW);
#else
	if(enable)
          mux_74hc595_set_bit(BSP_74HC595_0,   BSP_74HC595_LCD_PWREN);
	else
          mux_74hc595_clear_bit(BSP_74HC595_0,   BSP_74HC595_LCD_PWREN);
#endif
}

// on - low
static inline void blklight_enable(uint8_t on_off)
{
#ifndef BSP_LCD_PIN_74HC595
  switch(on_off)
  {
  case 1:
    lwgpio_set_value(&blgpio, LWGPIO_VALUE_LOW); //turn on back light
    break;
  case 0:
    lwgpio_set_value(&blgpio, LWGPIO_VALUE_HIGH); //turn off back light
    break;
  default:
    printf("the value don't be supportted!\n");
    break;
  }
#else
  if(on_off)
     mux_74hc595_set_bit(BSP_74HC595_0,   BSP_74HC595_BLT_PWREN);
  else
     mux_74hc595_clear_bit(BSP_74HC595_0,   BSP_74HC595_BLT_PWREN);
#endif
}

static inline void lcd_hw_reset()
{
#ifndef BSP_LCD_PIN_74HC595	
    if(!lwgpio_init(&rstgpio, BSP_LCD_RST, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        _time_delay (200L);
        _task_block();
    }
    lwgpio_set_functionality(&rstgpio,BSP_LCD_RST_MUX_GPIO);
    lwgpio_set_value(&rstgpio, LWGPIO_VALUE_LOW);
    _time_delay(100L);
    lwgpio_set_value(&rstgpio, LWGPIO_VALUE_HIGH);
#else
    _time_delay(50L);  
    mux_74hc595_clear_bit(BSP_74HC595_0,   BSP_74HC595_LCD_nRST);
    _time_delay(50L);
    mux_74hc595_set_bit(BSP_74HC595_0,   BSP_74HC595_LCD_nRST);
    _time_delay(50L);
#endif

}

#ifdef BSP_LCD_SPI_CS_74HC595                         
static _mqx_int set_lcd_spi_cs (uint32_t cs_mask, void *user_data) {

    if (cs_mask & BSP_SDCARD_SPI_CS) {
        if(BSP_LCD_SPI_CS_74HC595 & (1 <<  BSP_74HC595_SPI_S0)){
            mux_74hc595_set_bit(BSP_74HC595_0,   BSP_74HC595_SPI_S0);
        }
        else{
            mux_74hc595_clear_bit(BSP_74HC595_0,   BSP_74HC595_SPI_S0);
        }

        if(BSP_LCD_SPI_CS_74HC595 & (1 <<  BSP_74HC595_SPI_S1)){
            mux_74hc595_set_bit(BSP_74HC595_0,   BSP_74HC595_SPI_S1);
        }
        else{
            mux_74hc595_clear_bit(BSP_74HC595_0,   BSP_74HC595_SPI_S1);
        }

    }
    return MQX_OK;
}
#endif

static void LCD_gpio_init(MQX_FILE_PTR spifd)
{
	/* lcd power enable */
#ifndef BSP_LCD_PIN_74HC595
	if(!lwgpio_init(&pwrgpio, BSP_LCD_PWREN, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        _time_delay (200L);
        _task_block();
    }
    lwgpio_set_functionality(&pwrgpio, BSP_LCD_PWREN_MUX_GPIO);	
#endif
    power_enable(ON);

    /* lcd reset (low) */
#ifndef BSP_LCD_PIN_74HC595
    if(!lwgpio_init(&rstgpio, BSP_LCD_RST, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        _time_delay (200L);
        _task_block();
    }
    lwgpio_set_functionality(&rstgpio,BSP_LCD_RST_MUX_GPIO);
#endif
    lcd_hw_reset();

    /* lcd backlight (low) */
#ifndef BSP_LCD_PIN_74HC595
    if(!lwgpio_init(&blgpio, BSP_LCD_BL, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        _time_delay (200L);
        _task_block();
    }
    lwgpio_set_functionality(&blgpio, BSP_LCD_BL_MUX_GPIO);
	//lwgpio_set_value(&blgpio, LWGPIO_VALUE_LOW);
#endif
    blklight_enable(ON);
#if 1 /* move to each time when select c/d */	
    /* lcd cmd & data selcect */
    if(!lwgpio_init(&a0gpio, BSP_LCD_A0, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        _time_delay (200L);
        _task_block();
    }
    lwgpio_set_functionality(&a0gpio, BSP_LCD_A0_MUX_GPIO);	
    // cs pin
#endif    
}

static void LCD_gpio_deinit(MQX_FILE_PTR spifd)
{
#if 1  
    // reinit spi MISO
    /* Configure GPIOD 4~7 for SPI1 peripheral function     */
    PORT_MemMapPtr  pctl;
    pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;

    pctl->PCR[4] = PORT_PCR_MUX(7) | PORT_PCR_DSE_MASK;     /* SPI1.PCS0   */
    pctl->PCR[5] = PORT_PCR_MUX(7) | PORT_PCR_DSE_MASK;     /* SPI1.SCK    */
    pctl->PCR[6] = PORT_PCR_MUX(7) | PORT_PCR_DSE_MASK;     /* SPI1.SOUT   */
    pctl->PCR[7] = PORT_PCR_MUX(7) | PORT_PCR_DSE_MASK;     /* SPI1.SIN    */
#endif  
    /* move to lcd_deinit
    blklight_enable(OFF);
    power_enable(OFF);
    */
}

static void LCD_send_byte(MQX_FILE_PTR spifd, uint8_t RS,uint8_t RW,uint8_t data)
{
        _mqx_int result;
	
	/*
	if(RS) 
		lwgpio_set_value(&a0gpio, LWGPIO_VALUE_HIGH);
	else
		lwgpio_set_value(&a0gpio, LWGPIO_VALUE_LOW);
        */
	cmd_data_sel(RS);

	result = fwrite(&data, 1, 1, spifd);

	fflush(spifd);

	if(result != 1)
        {
           printf("LCD send byte error\n");
        }
}

static void LCD_send_character(MQX_FILE_PTR spifd, uint8_t page, uint8_t addr, uint8_t *character)
{
	uint8_t i;
	
   	LCD_send_byte(spifd, 0, 0, page);
	LCD_send_byte(spifd, 0, 0, addr);
	for(i=0; i<8; i++)
	{
		LCD_send_byte(spifd, 0, 0, i);
   		LCD_send_byte(spifd, 1, 0, character[i]);
	}
	
	LCD_send_byte(spifd, 0, 0, ++page);
	LCD_send_byte(spifd, 0, 0, addr);
	for(i=0; i<8; i++)
	{
		LCD_send_byte(spifd, 0, 0, i);
   		LCD_send_byte(spifd, 1, 0, character[i+8]);
	}
}

static void LCD_set_regcommand(MQX_FILE_PTR spifd, uint8_t regcommand)
{
  _mqx_int result;
  
  // lwgpio_set_value(&a0gpio, LWGPIO_VALUE_LOW); //select register command
  cmd_data_sel(CMD_A0);
  
  result = fwrite(&regcommand, 1, 1, spifd);
  
  fflush(spifd);
  
  if(result != 1)
  {
     printf("LCD set command error!\n");
  }
}

static void LCD_set_startline(MQX_FILE_PTR spifd, uint8_t startline)
{
  if (startline > 63) 
  {
    printf("the value don't be supportted!\n");
    return;
  }
  
  LCD_set_regcommand(spifd, (startline | 0x40));
}

static void LCD_set_pageaddress(MQX_FILE_PTR spifd, uint8_t pageaddress)
{
  if (pageaddress > 15) 
  {
    printf("the value don't be supportted!\n");
    return;
  }
  
  LCD_set_regcommand(spifd, (pageaddress | 0xB0));
}

static void LCD_set_columnaddr_high(MQX_FILE_PTR spifd, uint8_t columnaddr_h)
{
  if (columnaddr_h > 15) 
  {
    printf("the value don't be supportted!\n");
    return;
  }
  
  LCD_set_regcommand(spifd, (columnaddr_h | 0x10));
}

static void LCD_set_columnaddr_low(MQX_FILE_PTR spifd, uint8_t columnaddr_l)
{
  if (columnaddr_l > 15) 
  {
    printf("the value don't be supportted!\n");
    return;
  }
  
  LCD_set_regcommand(spifd, (columnaddr_l | 0x00));
}

static void LCD_write_ramdata(MQX_FILE_PTR spifd, uint8_t ramdata)
{
  _mqx_int result;
  
  //lwgpio_set_value(&a0gpio, LWGPIO_VALUE_HIGH); //select register data
  /* data */
  cmd_data_sel(DATA_A0);
  
  result = fwrite(&ramdata, 1, 1, spifd);
  
  fflush(spifd);
  
  if(result != 1)
  {
     printf("LCD write ram error!\n");
  }
}

static void LCD_clean_full_screen(MQX_FILE_PTR spifd)
{
  uint8_t i;
  
  LCD_power_control(spifd, ON);
  
  LCD_set_regcommand(spifd, 0xE2);
  
  LCD_scan_direction(spifd, NOR);
  LCD_set_dispmod(spifd, NOR);
  //LCD_save_power(spifd, WAKE);
  LCD_set_startline(spifd, 0);
  
  //LCD_set_regcommand(spifd, 0xA5);
  //LCD_set_regcommand(spifd, 0xA4);
  LCD_set_regcommand(spifd, 0xA3);
  
  
  LCD_turn_display(spifd,ON);
  
  for(i = 0 ; i < MAX_ROW; i++)
    LCD_display_string(spifd, i, 0, "                ");
  
}

static void LCD_display_char(MQX_FILE_PTR spifd, uint8_t page_no, uint8_t column, uint8_t *character)
{
  int8_t i ,j;
  
  LCD_set_startline(spifd, 0);
  //LCD_set_startline(spifd, page_no * 8);
  LCD_set_pageaddress(spifd, page_no);
  //LCD_set_columnaddr_high(spifd, column_high);
  j = column;
  for(i = 0; i < 8; i++) {
     LCD_set_columnaddr_high(spifd, ((j >> 4) & 0xf));
     LCD_set_columnaddr_low(spifd, (j & 0xf));
     LCD_write_ramdata(spifd, character[7 - i]);
     j--;
  }
  
  //LCD_set_startline(spifd, (page_no + 1) * 8);
  LCD_set_pageaddress(spifd, page_no + 1);
  //LCD_set_columnaddr_high(spifd, column_high);
  j = column;
  for(i=0; i < 8; i++) {
     LCD_set_columnaddr_high(spifd, ((j >> 4) & 0xf));
     LCD_set_columnaddr_low(spifd, (j & 0xf));
     LCD_write_ramdata(spifd, character[7 - i + 8]);
     j--;
  }
}

static void LCD_display_string(MQX_FILE_PTR spifd, uint8_t row, uint8_t col, char *string)
{
  uint8_t cnt, page_no, colume;
  uint8_t len = strlen(string);
  
  // verify row,col
  if((col >= MAX_COLUME) || (row >= MAX_ROW)) {
    printf("Invalid row and col for display\n");
    return;
  }
  
  /*if(col + len > MAX_COLUME) {
    len = MAX_COLUME - col;
  }*/
  
  page_no = page_idx[row];
  for(cnt = 0; cnt < len; cnt++) {
    colume  = LCD_DISP_START_COLUME - (col +  cnt) * 8;
    LCD_display_char(spifd, page_no, colume, (uint8 *)(st_font16x8[string[cnt]]));
  }
}

static void LCD_turn_backlight(MQX_FILE_PTR spifd, uint8_t on_off)
{
	blklight_enable(on_off);
}

static void LCD_turn_display(MQX_FILE_PTR spifd, uint8_t on_off)
{
  switch(on_off)
  {
  case 1:
    LCD_set_regcommand(spifd, LCD_DISPLAY_ON); //turn on display
    break;
  case 0:
    LCD_set_regcommand(spifd, LCD_DISPLAY_OFF); //turn off display
    break;
  default:
    printf("the value don't be supportted!\n");
    break;
  }
}

static void LCD_set_dispmod(MQX_FILE_PTR spifd, uint8_t inv_nor)
{
  switch(inv_nor)
  {
  case 1:
    LCD_set_regcommand(spifd, LCD_DISPMOD_INV); //setting display inverse mode
    break;
  case 0:
    LCD_set_regcommand(spifd, LCD_DISPMOD_NOR); //setting display normal mode
    break;
  default:
    printf("the value don't be supportted!\n");
    break;
  }
}

static void LCD_scan_direction(MQX_FILE_PTR spifd, uint8_t inv_nor)
{
  switch(inv_nor)
  {
  case 1:
    LCD_set_regcommand(spifd, LCD_SCANDIR_INV); //setting display scan direction is inverse mode
    break;
  case 0:
    LCD_set_regcommand(spifd, LCD_SCANDIR_NOR); //setting display scan direction is normal mode
    break;
  default:
    printf("the value don't be supportted!\n");
    break;
  }
}

//save power = display off + all pixel on
static void LCD_save_power(MQX_FILE_PTR spifd, uint8_t save_wake)
{
  switch(save_wake)
  {
  case 1:
    LCD_set_regcommand(spifd, LCD_DISPLAY_OFF); 
    LCD_set_regcommand(spifd, LCD_ALLPIXEL_ON);
    break;
  case 0:
    LCD_set_regcommand(spifd, LCD_ALLPIXEL_OFF);
    LCD_set_regcommand(spifd, LCD_DISPLAY_ON);
    break;
  default:
    printf("the value don't be supportted!\n");
    break;
  }
}

static void LCD_power_control(MQX_FILE_PTR spifd, uint8_t on_off)
{
  switch(on_off)
  {
  case 1: 
	  //power_enable(1);
  _time_delay(100);
    LCD_set_regcommand(spifd, LCD_POWER_ON);
    break;
  case 0: 
    LCD_set_regcommand(spifd, LCD_POWER_OFF); 
    //power_enable(0);
    break;
  default: 
    printf("the value don't be supportted!\n");
    break;
  }
}

static void LCD_software_reset(MQX_FILE_PTR spifd)
{
  LCD_set_regcommand(spifd, LCD_SOFTWARE_RESET);
}

void lcd_open(/*int param*/) 
{
    MQX_FILE_PTR           	spifd;
    uint32_t                para, result;
    SPI_STATISTICS_STRUCT  	stats;
#ifdef BSP_LCD_SPI_CS_74HC595                         
    SPI_CS_CALLBACK_STRUCT callback;
#endif

    spifd = fopen("spi1:", NULL);

    if(NULL == spifd) {
        printf("Error opening SPI driver!\n");
        _time_delay(200L);
        _task_block();
    }

    console.spifd = spifd;

#ifdef BSP_LCD_SPI_CS_74HC595
    callback.CALLBACK = set_lcd_spi_cs;
    callback.USERDATA = NULL;
    if (SPI_OK != ioctl (spifd, IO_IOCTL_SPI_SET_CS_CALLBACK, &callback))
    {
        printf ("Setting CS callback failed.\n");
        _task_block();
    }
#endif

    /* Display baud rate */
    printf("Current baud rate ... ");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_BAUD, &para)) {
        printf("%d Hz\n", para);
    }
    else {
        printf("ERROR\n");
    }

    /* Set a different rate */
    para = 1000000;
    printf("Changing the baud rate to %d Hz ... ", para);
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_BAUD, &para)) {
        printf("OK\n");
    }
    else {
        printf("ERROR\n");
    }

    /* Display baud rate */
    printf("Current baud rate ... ");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_BAUD, &para)) {
        printf("%d Hz\n", para);
    }
    else {
        printf("ERROR\n");
    }

    /* Set clock mode */
    para = SPI_CLK_POL_PHA_MODE0;
    printf("Setting clock mode to %s ... ", clock_mode[para]);
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_MODE, &para)) {
        printf("OK\n");
    }
    else {
        printf("ERROR\n");
    }

    /* Get clock mode */
    printf("Getting clock mode ... ");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_MODE, &para)) {
        printf("%s\n", clock_mode[para]);
    }
    else {
        printf("ERROR\n");
    }

    /* Set big endian */
    para = SPI_DEVICE_BIG_ENDIAN;
    printf("Setting endian to %s ... ", para == SPI_DEVICE_BIG_ENDIAN ? "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_ENDIAN, &para)) {
        printf("OK\n");
    }
    else {
        printf("ERROR\n");
    }

    /* Get endian */
    printf("Getting endian ... ");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_ENDIAN, &para)) {
        printf("%s\n", para == SPI_DEVICE_BIG_ENDIAN ? "SPI_DEVICE_BIG_ENDIAN" : "SPI_DEVICE_LITTLE_ENDIAN");
    }
    else {
        printf("ERROR\n");
    }

    /* Set transfer mode */
    para = SPI_DEVICE_MASTER_MODE;
    printf("Setting transfer mode to %s ... ", device_mode[para]);
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_SET_TRANSFER_MODE, &para)) {
        printf("OK\n");
    }
    else {
        printf("ERROR\n");
    }

    /* Get transfer mode */
    printf("Getting transfer mode ... ");
    if(SPI_OK == ioctl(spifd, IO_IOCTL_SPI_GET_TRANSFER_MODE, &para)) {
        printf("%s\n", device_mode[para]);
    }
    else {
        printf("ERROR\n");
    }

    /* Clear statistics */
    printf("Clearing statistics ... ");
    result = ioctl(spifd, IO_IOCTL_SPI_CLEAR_STATS, NULL);
    if(SPI_OK == result) {
        printf("OK\n");
    }
    else if(MQX_IO_OPERATION_NOT_AVAILABLE == result) {
        printf("not available, define BSPCFG_ENABLE_SPI_STATS\n");
    }
    else {
        printf("ERROR\n");
    }

    /* Get statistics */
    printf("Getting statistics: ");
    result = ioctl(spifd, IO_IOCTL_SPI_GET_STATS, &stats);
    if(SPI_OK == result) {
        printf("\n");
        printf("Rx packets:   %d\n", stats.RX_PACKETS);
        printf("Tx packets:   %d\n", stats.TX_PACKETS);
    }
    else if(MQX_IO_OPERATION_NOT_AVAILABLE == result) {
        printf("not available, define BSPCFG_ENABLE_SPI_STATS\n");
    }
    else {
        printf("ERROR\n");
    }
    printf("\n");
}

void lcd_close()
{
    uint32_t result;
    LCD_gpio_deinit(console.spifd);
    _time_delay(50);
    
    /* Close the SPI */
    if(console.spifd != NULL) {
        result = (uint32_t)fclose (console.spifd);
	if(result)  {
	   printf("Error closing SPI, returned: 0x%08x\n", result);
	}
        console.spifd = NULL;
    }
}
void lcd_init(/*int param*/)
{
    static int init_flag = 0;
    printf ("lcd init start -\n");
    /* Open the SPI driver */
    /*
	if(param == 0)
    	spifd = fopen("spi0:", NULL);
	else
       	spifd = fopen("spi1:", NULL);
	else
        printf("invalid spi port\n");
    */
   lcd_open(/*int param*/);
   if(console.spifd == NULL) {
     memset(&console,0,sizeof(console_t));
     printf("lcd open failed\n");
   }
      
   
   if(init_flag == 0) {
     init_flag = 1;
   }
   else {
     return;
   }
    LCD_gpio_init(console.spifd);
#if 1
    LCD_clean_full_screen(console.spifd);
#else
    LCD_power_control(console.spifd, ON);
    LCD_set_regcommand(console.spifd, 0xE2);
  
    LCD_scan_direction(console.spifd, NOR);
    LCD_set_dispmod(console.spifd, NOR);
    LCD_save_power(console.spifd, WAKE);
    LCD_turn_display(console.spifd,ON);
    
    LCD_set_regcommand(console.spifd, 0xA5);
    LCD_set_regcommand(console.spifd, 0xA4);
  
    LCD_set_startline(console.spifd, 0);
#endif  
    printf ("lcd init end +\n");
}

void lcd_deinit()
{
   
    lcd_close();          
    /*  
    LCD_turn_display(console.spifd,OFF);
    _time_delay(50);
    LCD_power_control(console.spifd, OFF);
    _time_delay(50); 
    
    blklight_enable(OFF);
    power_enable(OFF);

    */
}

void lcd_printf(char * str)
{	
	int len = strlen(str);
	int i, rflag = 0;
       
        for (i = 0; i < len ; i++ ) {
		if (str[i] == '\n') {
			rflag = 1;
                        console.col = 0;
			break;
		}
		else {
		  console.content[/*console.row*/0][(console.col )] = str[i];
		  console.col++;
                  if(console.col >= MAX_COLUME) {
			  console.col = 0;
		  }
		}
        }

    // scroll
	if(rflag) { /* return */
		// for (i = 0 ; i < MAX_ROW -1; i++) {
                for (i = MAX_ROW -1 ; i > 0; i--) {
		   strncpy((char *)(console.content[i]), (char *)(console.content[i - 1]),16);
		}
		memset(console.content[0],0, MAX_COLUME + 1);
	}
	// refresh console
	for(i = 0; i < MAX_ROW; i++) {
           LCD_display_string (console.spifd, i,0,"                ");
	   LCD_display_string (console.spifd, i,0,(char *)(console.content[i]));
	}
}

char gbuff[32];
void printf_lcd(const char  *fmt_ptr, ...)
{
   va_list  ap;
   _mqx_int  result;

    lcd_init();
    PORT_MemMapPtr  pctl;
    pctl = (PORT_MemMapPtr)PORTD_BASE_PTR;
    if(!lwgpio_init(&a0gpio, BSP_LCD_A0, LWGPIO_DIR_OUTPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing GPIO with associated pins failed.\n");
        _time_delay (200L);
        _task_block();
    }
    lwgpio_set_functionality(&a0gpio, BSP_LCD_A0_MUX_GPIO);
  
   
   va_start(ap, fmt_ptr);
   result = vsnprintf (gbuff, 32, fmt_ptr, ap);

   va_end(ap);

   lcd_printf(gbuff);
   lcd_deinit();
   // pctl->PCR[7] = PORT_PCR_MUX(7) | PORT_PCR_DSE_MASK;     /* SPI1.SIN    */

}

