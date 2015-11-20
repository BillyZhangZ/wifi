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


#define ADK_INTERNAL 1

#include "fwk.h"
#include "ADK.h"
#include "btHFP.h"
#include "HCI.h"
#include "fio.h"
#include "msi.h"

#include "74hc595.h"

static MQX_FILE_PTR RDA5876_BT_DEV; 

//definition for rda5876 init params
const static uint32_t rdabt_rf_init_12[][2] = 
{ 
    {0x3f,0x0000}, //;page 0
    {0x01,0x1FFF}, //;
    {0x06,0x07F7}, //;padrv_set,increase the power.
    {0x08,0x01E7}, //;
    {0x09,0x0520}, //;
    {0x0B,0x03DF}, //;filter_cap_tuning<3:0>1101
    {0x0C,0x85E8}, //;
    {0x0F,0x0DBC}, //; 0FH,16'h1D8C; 0FH,16'h1DBC;adc_clk_sel=1 20110314 ;adc_digi_pwr_reg<2:0>=011; 
    {0x12,0x07F7}, //;padrv_set,increase the power. 
    {0x13,0x0327}, //;agpio down pullen . 
    {0x14,0x0CCC}, //;h0CFE; bbdac_cm 00=vdd/2. 
    {0x15,0x0526}, //;Pll_bypass_ontch:1,improve ACPR.
    {0x16,0x8918}, //;add div24 20101126
    {0x18,0x8800}, //;add div24 20101231
    {0x19,0x10C8}, //;pll_adcclk_en=1 20101126
    {0x1A,0x9128}, //;Mdll_adcclk_out_en=0
    {0x1B,0x80C0}, //;1BH,16'h80C2
    {0x1C,0x361f}, //;
    {0x1D,0x33fb}, //;Pll_cp_bit_tx<3:0>1110;13D3
    {0x1E,0x303f}, //;Pll_lpf_gain_tx<1:0> 00;304C
    {0x23,0x2222}, //;txgain
    {0x24,0x359d}, //;
    {0x27,0x0011}, //;
    {0x28,0x124f}, //;
    {0x39,0xA5FC}, //; 
    {0x3f,0x0001}, //;page 1
    {0x00,0x043F}, //;agc
    {0x01,0x467F}, //;agc
    {0x02,0x28FF}, //;agc//20110323;82H,16'h68FF;agc
    {0x03,0x67FF}, //;agc
    {0x04,0x57FF}, //;agc
    {0x05,0x7BFF}, //;agc
    {0x06,0x3FFF}, //;agc
    {0x07,0x7FFF}, //;agc
    {0x18,0xf3f5}, //;
    {0x19,0xf3f5}, //;
    {0x1A,0xe7f3}, //;
    {0x1B,0xf1ff}, //;
    {0x1C,0xffff}, //;
    {0x1D,0xffff}, //;
    {0x1E,0xffff}, //;
    {0x1F,0xFFFF}, //;padrv_gain;9FH,16'hFFEC;padrv_gain20101103
    {0x23,0x4224}, //;ext32k
    {0x24,0x0110},
    {0x25,0x43E1}, //;ldo_vbit:110,2.04v
    {0x26,0x4bb5}, //;reg_ibit:100,reg_vbit:101,1.2v,reg_vbit_deepsleep:110,750mV
    {0x32,0x0079}, //;TM mod
    {0x3f,0x0000}, //;page 0
};
 
 
 const static uint32_t RDA5876_ENABLE_SPI[][2] =
{
    {0x40240000,0x2004f39c},                               
};
 
const static uint32_t RDA5876_DISABLE_SPI[][2] = 
{
    {0x40240000,0x2000f29c},
};
 
const static uint32_t RDA5876_PSKEY_RF[][2] =
{
    {0x40240000,0x2004f39c}, //;SPI2_CLK_EN PCLK_SPI2_EN
    {0x800000C0,0x00000021}, //;CHIP_PS PSKEY: Total number
    {0x800000C4,0x003F0000},
    {0x800000C8,0x00414003},
    {0x800000CC,0x004225BD},
    {0x800000D0,0x004908E4},
    {0x800000D4,0x0043B074},
    {0x800000D8,0x0044D01A},
    {0x800000DC,0x004A0600},
    {0x800000E0,0x0054A020},
    {0x800000E4,0x0055A020},
    {0x800000E8,0x0056A542},
    {0x800000EC,0x00574C18},
    {0x800000F0,0x003F0001},
    {0x800000F4,0x00410900},
    {0x800000F8,0x0046033F},
    {0x800000FC,0x004C0000},
    {0x80000100,0x004D0015},
    {0x80000104,0x004E002B},
    {0x80000108,0x004F0042},
    {0x8000010C,0x0050005A},
    {0x80000110,0x00510073},
    {0x80000114,0x0052008D},
    {0x80000118,0x005300A7},
    {0x8000011C,0x005400C4},
    {0x80000120,0x005500E3},
    {0x80000124,0x00560103},
    {0x80000128,0x00570127},
    {0x8000012C,0x0058014E},
    {0x80000130,0x00590178},
    {0x80000134,0x005A01A1},
    {0x80000138,0x005B01CE},
    {0x8000013C,0x005C01FF},
    {0x80000140,0x003F0000},
    {0x80000144,0x00000000}, //;PSKEY: Page 0
    {0x80000040,0x10000000}, 
};

static const uint32_t RDA5876_DCCAL[][2]=
{
    {0x00000030,0x00000129},
    {0x00000030,0x0000012b}
};
 
static const uint32_t RDA5876_PSKEY_MISC[][2] =
{
    {0x800004ec,0xfa8dFFFF}, //disable 3m edr, enable edr    
    {0x800004f0,0x83793f98}, //enable ssp
    {0x80000070,0x00012080},
    {0x80000074,0x0f025010}, //sleep
    {0x8000007c,0xb530b530},
    {0x80000084,0x9098C007},
    {0x800000a0,0x00000000}, //g_host_wake_event1 g_host_wake_event2=0
    {0x800000a4,0x00000000},
    {0x800000a8,0x0Bbaba30}, //min power level
    //3200000
    //{0x80000060,0x0030d400},
    //{0x80000064,0x0030d400},
    //921600
    //{0x80000060,0x000e1000},
    //{0x80000064,0x000e1000},
    //806400
    //{0x80000060,0x000c4e00},
    //{0x80000064,0x000c4e00},
    //115200 Baud rate
    {0x80000060,0x0001c200},
    {0x80000064,0x0001c200},
    //200000
    //{0x80000060,0x00030d40},
    //{0x80000064,0x00030d40},
    //230400
    //{0x80000060,0x00038400},
    //{0x80000064,0x00038400},
    //300000
    //{0x80000060,0x000493e0},
    //{0x80000064,0x000493e0},
    //345600
    //{0x80000060,0x00054600},
    //{0x80000064,0x00054600},
    //460800
    //{0x80000060,0x00070800},
    //{0x80000064,0x00070800},
    //3000000
    //{0x80000060,0x002dc6c0},
    //{0x80000064,0x002dc6c0},
    {0x80000040,0x0702b300},  
}; 
 
static const uint32_t RDA5876_TRAP[][2] = 
{ 
    {0x40180100,0x000068b0}, //inc power
    {0x40180120,0x000068f4},
    {0x40180104,0x000066b0}, //dec power
    {0x40180124,0x000068f4},
    {0x40180108,0x0001544c}, //esco w
    {0x40180128,0x0001568c},
    {0x80000100,0xe3a0704f}, //2ev3 ev3 hv3
    {0x4018010c,0x0000bae8}, //esco packet
    {0x4018012c,0x80000100},
    {0x40180114,0x0000f8c4}, //all rxon
    {0x40180134,0x00026948},
    {0x40180118,0x000130b8}, //qos PRH_CHN_QUALITY_MIN_NUM_PACKETS
    {0x40180138,0x0001cbb4},
    {0x40180014,0x00017354}, //auth after encry
    {0x40180034,0x00017368},
    {0x40180018,0x0001a338}, //auth after encry
    {0x40180038,0x00000014},
    {0x4018011c,0x00010318}, //lu added for earphone 09.27
    {0x4018013c,0x00001f04},
    {0x80000200,0xe59f5014}, //sco sniff
    {0x80000204,0xe5d55000},
    {0x80000208,0xe3550001},
    {0x8000020C,0x03a00000},
    {0x80000210,0x11a00006},
    {0x80000214,0xe3a06bb7},
    {0x80000218,0xe286ffa8},
    {0x8000021c,0x80000989},
    {0x80000000,0xea00007e},
    {0x40180004,0x0002de9c},
    {0x40180024,0x00032d14},
    {0x40180000,0x00006f71},
};

const uint32_t RDA5876_BAUDRATE_230400[][2] =
{
    //230400
    {0x80000060,0x00038400},
    {0x80000064,0x00038400},
    {0x80000040,0x00000300},	
}; 


const uint32_t RDA5876_BAUDRATE_345600[][2] =
{
    //345600
    {0x80000060,0x00054600},
    {0x80000064,0x00054600},
    {0x80000040,0x00000300},	
}; 

const uint32_t RDA5876_BAUDRATE_460800[][2] =
{
    //460800
    {0x80000060,0x00070800},
    {0x80000064,0x00070800},
    {0x80000040,0x00000300},	
};

const uint32_t RDA5876_BAUDRATE_921600[][2] =
{
    //921600
    {0x80000060,0x000e1000},
    {0x80000064,0x000e1000},
    {0x80000040,0x00000300},	
}; 

const uint32_t RDA5876_BAUDRATE_1843200[][2] =
{
    //1843200
    {0x80000060,0x001c2000},
    {0x80000064,0x001c2000},
    {0x80000040,0x00000300},	
}; 

const uint32_t RDA5876_BAUDRATE_3000000[][2] =
{
    //3000000
    {0x80000060,0x002dc6c0},
    {0x80000064,0x002dc6c0},
    {0x80000040,0x00000300},	
}; 

const uint32_t RDA5876_BAUDRATE_3686400[][2] =
{
    //3686400
    {0x80000060,0x00384000},
    {0x80000064,0x00384000},
    {0x80000040,0x00000300},	
}; 

const uint32_t RDA5876_BAUDRATE_7372800[][2] =
{
    //7372800
    {0x80000060,0x00708000},
    {0x80000064,0x00708000},
    {0x80000040,0x00000300},	
}; 

const uint32_t RDA5876_BAUDRATE_115200[][2] =
{
    //115200
    {0x80000060,0x0001c200},
    {0x80000064,0x0001c200},
    {0x80000040,0x00000300},	
};


/**************************************************************************
  Global variables
 **************************************************************************/
#define RDA5876_SERIAL_CHANNEL "mttyb:"
/************************************************************************************/

static void *btHfpCallback(void *userparam, void *param){
    printf("btHfpCallback status is %d \n", (int)(param));
    return NULL;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : 
* Returned Value : 
* Comments       :
*END*--------------------------------------------------------------------*/


int_32  Shell_audio(int_32 argc, char_ptr argv[] ) {

    boolean              print_usage, shorthelp = FALSE;

    print_usage = Shell_check_help_request(argc, argv, &shorthelp );

    if (!print_usage)  {
        if(argc < 3) {
            print_usage = TRUE;
        }
        else {

            if(!strcmp(argv[1], "vol")){

                if(!strcmp(argv[2], "up")){
                    msi_snd_vol_up();
                }else
                if(!strcmp(argv[2], "down")){
                    msi_snd_vol_down();
                }else
                if(!strcmp(argv[2], "mute")){
                    msi_snd_mute();
                }else
                if(!strcmp(argv[2], "unmute")){
                    msi_snd_umute();
                }else
                    print_usage = TRUE;
                }
            } 
    }

    if (print_usage)  {
        if(shorthelp) {
            printf("%s vol  <command> \n", argv[0]);
        }else {
            printf("%s vol  <command> \n", argv[0]);
            printf("vol command: \n"
                    "  up:     vol up\n"
                    "  down:   vol down\n"
                    "  mute:   vol mute\n"
                    "  unmute: vol unmute\n"
                    "\n");
        }
    }
    return 0;
}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : 
* Returned Value : 
* Comments       :
*END*--------------------------------------------------------------------*/


int_32  Shell_bt(int_32 argc, char_ptr argv[] ) {

    boolean              print_usage, shorthelp = FALSE;

    print_usage = Shell_check_help_request(argc, argv, &shorthelp );

    if (!print_usage)  {
        if(argc < 3) {
            print_usage = TRUE;
        }
        else {

            if(!strcmp(argv[1], "ct")){

                BTAVRCPCTKEY key = AVRCPCTUNKNOW;

                if(!strcmp(argv[2], "next")){
                    key =  AVRCPCTNEXT;
                }else
                if(!strcmp(argv[2], "pre")){
                    key =  AVRCPCTPREVIOUS;
                }else
                if(!strcmp(argv[2], "up")){
                    key =  AVRCPCTVOLUP;
                }else
                if(!strcmp(argv[2], "down")){
                    key =  AVRCPCTVOLDOWN;
                }else
                if(!strcmp(argv[2], "play")){
                    key =  AVRCPCTPLAY;
                }else
                if(!strcmp(argv[2], "pause")){
                    key =  AVRCPCTPAUSE;
                }else
                if(!strcmp(argv[2], "stop")){
                    key =  AVRCPCTSTOP;
                }else
                    print_usage = TRUE;

                if(key != AVRCPCTUNKNOW) {
                    avrcpServicePassthroughHandle(key, AVRCPCTPRESS);
#if 0
                    _time_delay(60);
                    avrcpServicePassthroughHandle(key, AVRCPCTRELEASE);
#endif

                }
            } /* ct */
            else
            if(!strcmp(argv[1], "hfp")){

                if(!strcmp(argv[2], "call")){
                    if(argc == 4){
                        btHfpCall((uint8_t *) argv[3], btHfpCallback, NULL);
                    }
                } /* call */

                if(!strcmp(argv[2], "A")){
                    btHfpAnswer(btHfpCallback, NULL);
                } /* answer */

                if(!strcmp(argv[2], "H")){
                    btHfpReject(btHfpCallback, NULL);
                } /* Reject */

                if(!strcmp(argv[2], "L")){
                    btHfpLastNumReDial(btHfpCallback, NULL);
                } /* re dial last number */

                if(!strcmp(argv[2], "SU")){
                    btHfpSpeakerVolUpSync(btHfpCallback, NULL);
                } /* speaker vol up sync */

                if(!strcmp(argv[2], "SD")){
                    btHfpSpeakerVolDownSync(btHfpCallback, NULL);
                } /* speaker vol down sync */

                if(!strcmp(argv[2], "ND")){
                     btHfpNRECDisable(btHfpCallback, NULL);
                } /* NREC disable */
            } /* hfp */
            else
                print_usage = TRUE;
        }

    }

    if (print_usage)  {
        if(shorthelp) {
            printf("%s ct  <command> \n", argv[0]);
            printf("%s hfp <command> \n", argv[0]);
        }else {
            printf("%s ct  <command> \n", argv[0]);
            printf("ct command: \n"
                    "  next: next     track \n"
                    "  pre:  previous track \n"
                    "  up:   volume up \n"
                    "  down: volume down \n"
                    "  play: play track \n"
                    "  pause:pause track \n"
                    "  stop: stop track \n"
                    "\n");
            printf("%s hfp <command> \n", argv[0]);
            printf("hfp command: \n"
                    "  call \"phonenumber\" \n"
                    "  A     Answer call   \n"
                    "  H     Reject call   \n"
                    "  L     Redial last number   \n"
                    "  SU    Speaker vol up   \n"
                    "  SD    Speaker vol down   \n"
                    "  ND    NRECDisable   \n"
                    );
        }
    }
    return 0;
}


//////////// bt support (boring)

volatile static uint32_t btSSP = ADK_BT_SSP_DONE_VAL;
const char* btPIN = 0;
char btName[64];

#define  maxPairedDevices 4
static uint8_t numPairedDevices = 0;
static uint8_t savedMac[maxPairedDevices][BLUETOOTH_MAC_SIZE];
static uint8_t savedKey[maxPairedDevices][BLUETOOTH_LINK_KEY_SIZE];

static void adkBtSspF(const uint8_t* mac, uint32_t val){

  btSSP = val;
  printf("ssp with val %u\n", val);
}

static char adkBtConnectionRequest(const uint8_t* mac, uint32_t devClass, uint8_t linkType){	//return 1 to accept

    printf("Accepting connection from %02X:%02X:%02X:%02X:%02X:%02X LinkType:%s\n", 
               mac[5], mac[4], mac[3], mac[2], mac[1], mac[0],
               linkType == 0 ? "SCO" : (linkType == 1 ? "ACL" :(linkType == 2 ? "eSCO" : "Reserved"))
               );
    return 1;
}

static char adkBtLinkKeyRequest(const uint8_t* mac, uint8_t* buf){ //link key create

  uint8_t i, j;
  
  printf("Key request from %02X:%02X:%02X:%02X:%02X:%02X\n  -> ", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);

  for(i = 0; i < numPairedDevices; i++){
 
    for(j = 0; j < BLUETOOTH_MAC_SIZE && savedMac[i][j] == mac[j]; j++);
    if(j == BLUETOOTH_MAC_SIZE){ //match
    
        printf("{");
        for(j = 0; j < BLUETOOTH_LINK_KEY_SIZE; j++){
         
          printf(" %02X", savedKey[i][j]);
          buf[j] = savedKey[i][j];
        }
        printf(" }");
        return 1;
    }
  }
  printf("FAIL\n");
  return 0;
}

static void adkBtLinkKeyCreated(const uint8_t* mac, const uint8_t* buf){ 	//link key was just created, save it if you want it later

   uint8_t j;
   
   printf("Key created for %02X:%02X:%02X:%02X:%02X:%02X <- {", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
   
   for(j = 0; j < BLUETOOTH_LINK_KEY_SIZE; j++){
         
     printf(" %02X", buf[j]);
   }
   printf(" }\n");
    
   if(numPairedDevices < maxPairedDevices){
      
      for(j = 0; j < BLUETOOTH_LINK_KEY_SIZE; j++) savedKey[numPairedDevices][j] = buf[j];
      for(j = 0; j < BLUETOOTH_MAC_SIZE; j++) savedMac[numPairedDevices][j] = mac[j];
      numPairedDevices++;
      printf("saved to slot %d/%d\n", numPairedDevices, maxPairedDevices);
   }
   else{
      printf("out of slots...discaring\n");
   }
}

static char adkBtPinRequest(const uint8_t* mac, uint8_t* buf){		//fill buff with PIN code, return num bytes used (16 max) return 0 to decline

   uint8_t v, i = 0;

   printf("PIN request from %02X:%02X:%02X:%02X:%02X:%02X\n  -> ", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
   
   if(btPIN){
     printf(" -> using pin '%s'\n", btPIN);
     for(i = 0; btPIN[i]; i++) buf[i] = btPIN[i];
     return i;
   }
   else printf(" no PIN set. rejecting\n");
   return 0;
}

#define MAGIX	0xFA


static void btStart(){
    uint8_t                 mac[BT_MAC_SIZE];
    uint8_t          rda5876mac[BT_MAC_SIZE] = {0x76, 0x58, 0x11, 0x22, 0x2D, 0xAE};

    ADK_btEnable(adkBtConnectionRequest, adkBtLinkKeyRequest, adkBtLinkKeyCreated, adkBtPinRequest, NULL);


    numPairedDevices = 0;
    memset(savedMac, 0, sizeof savedMac);
    memset(savedKey, 0, sizeof savedKey);

    btPIN = "0000";
    if(ADK_adkbtLocalMac(mac)){
        if(!memcmp(mac, rda5876mac, BT_MAC_SIZE)){
            mac[0] =  SIM_UIDMH        & 0xFF;
            mac[1] = (SIM_UIDMH >> 8)  & 0xFF;
            mac[2] =  SIM_UIDML        & 0xFF;
            mac[3] = (SIM_UIDML >> 8)  & 0xFF;
            mac[4] = (SIM_UIDML >> 16) & 0xFF;
            mac[5] = (SIM_UIDML >> 24) & 0xFF;  
            ADKRDAChangeBDAddr((const uint8_t *)mac);
            memset(mac, 0,  BT_MAC_SIZE);
            if(ADK_adkbtLocalMac(mac)){
                printf("BT new MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
            }
        }
        snprintf(btName, sizeof btName, "FSLA:%02X:%02X:%02X:%02X:%02X:%02X", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
    }else{
        snprintf(btName, sizeof btName, "FSLA?");
    }

    printf("ADK: setting BT name '%s' and pin '%s'\n", btName, btPIN);
    if(!ADK_btSetDeviceClass(DEVICE_CLASS_SERVICE_AUDIO | DEVICE_CLASS_SERVICE_RENDERING |
                DEVICE_CLASS_SERVICE_INFORMATION | (DEVICE_CLASS_MAJOR_AV << DEVICE_CLASS_MAJOR_SHIFT) |
                (DEVICE_CLASS_MINOR_AV_PORTBL_AUDIO << DEVICE_CLASS_MINOR_AV_SHIFT))) printf("ADK: Failed to set device class\n");
    if(!ADK_btSetLocalName(btName)) printf("ADK: failed to set BT name\n");
    if(!ADK_btDiscoverable(1)) printf("ADK: Failed to set discoverable\n");
    if(!ADK_btConnectable(1)) printf("ADK: Failed to set connectable\n");
    ADK_btSetSspCallback(adkBtSspF);
}



/**************************************************************************
  Local functions implementation for this application
 **************************************************************************/
static void uart_write_data(uint8_t *data, uint16_t data_len)//send data to UART
{
	fwrite(data, 1, data_len, RDA5876_BT_DEV);
}

static void BT_RDA_write_memory(uint32_t addr, const uint32_t *data, uint8_t len, uint8_t memory_type)
{
    uint16_t num_to_send; 
    uint16_t i,j;
    uint8_t data_to_send[256]={0};
    uint32_t address_convert;
    data_to_send[0] = 0x01;
    data_to_send[1] = 0x02;
    data_to_send[2] = 0xfd;
    data_to_send[3] = (uint8_t)(len*4+6);
    data_to_send[4] = (memory_type+0x80);//add the event display
    data_to_send[5] = len;
    if(memory_type == 0x01)
    {
        address_convert = addr*4+0x200;
        data_to_send[6] = (uint8_t)address_convert;
        data_to_send[7] = (uint8_t)(address_convert>>8);
        data_to_send[8] = (uint8_t)(address_convert>>16);
        data_to_send[9] = (uint8_t)(address_convert>>24); 
    }
    else
    {
        data_to_send[6] = (uint8_t)addr;
        data_to_send[7] = (uint8_t)(addr>>8);
        data_to_send[8] = (uint8_t)(addr>>16);
        data_to_send[9] = (uint8_t)(addr>>24);
    }
    for(i=0;i<len;i++,data++)
    {
        j=10+i*4;
        data_to_send[j] = (uint8_t)(*data);
        data_to_send[j+1] = (uint8_t)((*data)>>8);
        data_to_send[j+2] = (uint8_t)((*data)>>16);
        data_to_send[j+3] = (uint8_t)((*data)>>24);
    }
    num_to_send = 4+data_to_send[3];

    uart_write_data(data_to_send, num_to_send);
}

static void BT_RDA_uart_write_array(const uint32_t buf[][2],uint16_t len,uint8_t type)
{
    uint32_t i;
    for(i=0;i<len;i++)
    {
        BT_RDA_write_memory(buf[i][0],&buf[i][1],1,type);
        _time_delay(1);
    } 
}

static void RDA5876_RfInit(void)
{
    BT_RDA_uart_write_array(RDA5876_ENABLE_SPI,sizeof(RDA5876_ENABLE_SPI)/sizeof(RDA5876_ENABLE_SPI[0]),0);
    BT_RDA_uart_write_array(rdabt_rf_init_12,sizeof(rdabt_rf_init_12)/sizeof(rdabt_rf_init_12[0]),1);
    _time_delay(5);
}
 
static void RDA5876_Pskey_RfInit(void)
{
    BT_RDA_uart_write_array(RDA5876_PSKEY_RF,sizeof(RDA5876_PSKEY_RF)/sizeof(RDA5876_PSKEY_RF[0]),0);
}
 
static void RDA5876_Dccal(void)
{
    BT_RDA_uart_write_array(RDA5876_DCCAL,sizeof(RDA5876_DCCAL)/sizeof(RDA5876_DCCAL[0]),1);
    BT_RDA_uart_write_array(RDA5876_DISABLE_SPI,sizeof(RDA5876_DISABLE_SPI)/sizeof(RDA5876_DISABLE_SPI[0]),0);
}
 

static void RDA5876_Trap(void)
{
    BT_RDA_uart_write_array(RDA5876_TRAP,sizeof(RDA5876_TRAP)/sizeof(RDA5876_TRAP[0]),0);
}
static void RDA5876_Pskey_Misc(void)
{
    BT_RDA_uart_write_array(RDA5876_PSKEY_MISC,sizeof(RDA5876_PSKEY_MISC)/sizeof(RDA5876_PSKEY_MISC[0]),0);
}
 
static void rdabt_poweron_init(void){

    /*configure BT_PWREN pin*/

#if 0
    /*Change PTD1 to outputs */
    GPIOD_PDDR |= GPIO_PDDR_PDD(GPIO_PIN(1));
    /*Set PTD1 to 0*/
    GPIOD_PDOR &= ~GPIO_PDOR_PDO(GPIO_PIN(1));

    delay_ms(20);
    /*Set PTD1 to 1 */
    GPIOD_PDOR |= GPIO_PDOR_PDO(GPIO_PIN(1));

    RDA5876_RfInit();
    RDA5876_Pskey_RfInit();

    /*Set PTD1 to 0*/
    GPIOD_PDOR &= ~GPIO_PDOR_PDO(GPIO_PIN(1));
	delay_ms(20);
    /*Set PTD1 to 1 */
    GPIOD_PDOR |= GPIO_PDOR_PDO(GPIO_PIN(1));
#endif

    mux_74hc595_set_bit(BSP_74HC595_0, BSP_74HC595_BT_PWREN);
    mux_74hc595_clear_bit(BSP_74HC595_0, BSP_74HC595_BT_PWREN);
    _time_delay(20);
    mux_74hc595_set_bit(BSP_74HC595_0, BSP_74HC595_BT_PWREN);


    RDA5876_RfInit();   
    RDA5876_Pskey_RfInit();

    mux_74hc595_clear_bit(BSP_74HC595_0, BSP_74HC595_BT_PWREN);
    _time_delay(20);
    mux_74hc595_set_bit(BSP_74HC595_0, BSP_74HC595_BT_PWREN);

    _time_delay(50);

    RDA5876_RfInit();   
    RDA5876_Pskey_RfInit();
    RDA5876_Dccal(); 
    RDA5876_Trap();
    RDA5876_Pskey_Misc(); 

}

/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : UART_task
* Returned Value : None
* Comments       :
*     First function called. This rouine just transfers control to host main
*END*--------------------------------------------------------------------*/

void UART_task(uint_32 param)
{ 
    /* Store mounting point used. A: is the first one, bit #0 assigned, Z: is the last one, bit #25 assigned */
    uint_32              fs_mountp = 0;
    struct FWKFUNCS      _fwkFuncs;

    //uint32_t             baudrate = 921600;
    //uint32_t             baudrate = 1843200;
    uint32_t             baudrate = 3000000;
    //uint32_t             baudrate = 3686400;

    _int_install_unexpected_isr();


    memset(&_fwkFuncs, 0, sizeof _fwkFuncs);
    // _fwkFuncs.dacFuncs.init_with_periodbuffer  =  msi_snd_init_with_periodbuffer;
    // _fwkFuncs.dacFuncs.init        =  msi_snd_init;
    // _fwkFuncs.dacFuncs.deinit      =  msi_snd_deinit;
    _fwkFuncs.taskpriority           =  10;
    _fwkFuncs.dacFuncs.setSampleRate =  msi_snd_set_format;
    _fwkFuncs.dacFuncs.tryAddBuffer  =  msi_snd_write;
    _fwkFuncs.dacFuncs.vol_up        =  msi_snd_vol_up;
    _fwkFuncs.dacFuncs.vol_down      =  msi_snd_vol_down;
    _fwkFuncs.dacFuncs.mute          =  msi_snd_mute;
    _fwkFuncs.dacFuncs.unmute        =  msi_snd_umute;
    _fwkFuncs.dacFuncs.flush         =  msi_snd_flush;

    msi_snd_init_with_periodbuffer(512, 2);

    ADK_adkInit(_fwkFuncs);


    RDA5876_BT_DEV = fopen(RDA5876_SERIAL_CHANNEL, NULL);
    if(!RDA5876_BT_DEV){
        printf("Error open uart device\n");
        _task_block();
    }


    rdabt_poweron_init();

    ioctl(RDA5876_BT_DEV, IO_IOCTL_FLUSH_INPUT, NULL);

    fclose(RDA5876_BT_DEV);

    RDA5876_BT_DEV = fopen(RDA5876_SERIAL_CHANNEL, NULL);

    //BT_RDA_uart_write_array(RDA5876_BAUDRATE_3686400, sizeof(RDA5876_BAUDRATE_3686400)/sizeof(RDA5876_BAUDRATE_3686400[0]), 0);
    //BT_RDA_uart_write_array(RDA5876_BAUDRATE_1843200, sizeof(RDA5876_BAUDRATE_1843200)/sizeof(RDA5876_BAUDRATE_1843200[0]), 0);
    BT_RDA_uart_write_array(RDA5876_BAUDRATE_3000000, sizeof(RDA5876_BAUDRATE_3000000)/sizeof(RDA5876_BAUDRATE_3000000[0]), 0);
    //BT_RDA_uart_write_array(RDA5876_BAUDRATE_921600, sizeof(RDA5876_BAUDRATE_921600)/sizeof(RDA5876_BAUDRATE_921600[0]), 0);

    if(ioctl(RDA5876_BT_DEV , IO_IOCTL_SERIAL_SET_BAUD, (pointer)&baudrate) != MQX_OK){
        printf("Set RDA5876_BT_DEV baudrate failed \n");
        return;
    }
    //uart_set_baudrate((UART_MemMapPtr)UART0_BASE_PTR, CPU_PLL_FLL_CLK_HZ_CONFIG_0,115200);

    if (ADK_btInit(RDA5876_BT_DEV , 2) >= 0) {
        btStart();
    }
    
}



