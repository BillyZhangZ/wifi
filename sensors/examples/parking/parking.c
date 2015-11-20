/**************************************************************************
* 
* Copyright (c) 2015 Freescale Semiconductor;
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

/***************************************************************************/
/* Header */
/***************************************************************************/
#include "main.h"
#include "mag3110.h"
#include "74hc595.h"
#include "parking.h"

/***************************************************************************/
/* Macro */
/***************************************************************************/
#define PARKINGDURATION             5000  /* ms */
#define PARKINGMAGNETTHRESHOLD     10000  /* mag3110 magnet value square */
#define PARKINGCOLLECTIONRATEINT       1  /* default rate is 1HZ */
#define PARKINGCOLLECTIONRATEFRAC      0  /* HZ frac * 1000000 */

#define QCA4002_DEBUG                  1

#if QCA4002_DEBUG
#include "qca4002.h"
#endif

/***************************************************************************/
/* Type */
/***************************************************************************/
#if QCA4002_DEBUG
struct mag3110_data{
    unsigned int x;
    unsigned int y;
    unsigned int z;
    unsigned int msd;
    unsigned int parkingflag;
};
#endif


struct _parkingparameter{
    unsigned char  init;
    unsigned int   occupiedduration;       /* as ms */
    unsigned int   emptyduration;          /* as ms */
    unsigned int   parkingmagnetthreshold; /* mag3110 magnet value square */
    unsigned int   collectionrateint;      /* as HZ int */
    unsigned int   collectionratefrac;     /* as HZ frac * 1000000 */
    unsigned int   mag3110rateint;         /* as HZ int */
    unsigned int   mag3110ratefrac;        /* as HZ frac * 1000000 */
    unsigned int   loopdelay;              /* as ms, from collectionrate */
    LWSEM_STRUCT   sem;
    PARKINGCBF     cb;
};

enum _parkingstate{
    PARKINGE = 0,
    PARKINGE2O,
    PARKINGO,
    PARKINGO2E,
    PARKINGUNKNOW
};

/***************************************************************************/
/* Local Variable */
/***************************************************************************/
static struct _parkingparameter parkingparameter;
static enum _parkingstate       parkingstate;

/***************************************************************************/
/* Globe Function */
/***************************************************************************/
static int parking_check_next_parameter(void         *mag3110dev,
                                        unsigned int *mag3110rateint, 
                                        unsigned int *mag3110ratefrac, 
                                        unsigned int *parkingmagnetthreshold, 
                                        unsigned int *durationcount,
                                        unsigned int *loopdelay,
                                        PARKINGCBF   *cb){
    int ret;

    /* re set parking paramter */
    _lwsem_wait(&parkingparameter.sem);
    *loopdelay              = parkingparameter.loopdelay;
    *durationcount          = parkingparameter.occupiedduration / parkingparameter.loopdelay;
    *parkingmagnetthreshold = parkingparameter.parkingmagnetthreshold;
    *cb                     = parkingparameter.cb;

    if((*mag3110rateint != parkingparameter.mag3110rateint) && (*mag3110ratefrac = parkingparameter.mag3110ratefrac)){
        ret = mag3110_update_sample_frequency(mag3110dev, parkingparameter.mag3110rateint, parkingparameter.mag3110ratefrac);
        if(ret != 0){
            _lwsem_post(&parkingparameter.sem);
            return ret;
        }
        *mag3110rateint         = parkingparameter.mag3110rateint;
        *mag3110ratefrac        = parkingparameter.mag3110ratefrac;
    }
    _lwsem_post(&parkingparameter.sem);
    return 0;

}
/***************************************************************************/
/* Globe Function */
/***************************************************************************/
int parking_set_parameter(unsigned int* occupiedduration, 
                          unsigned int* emptyduration,
                          unsigned int* parkingmagnetthreshold,
                          unsigned int* collectionrateint,
                          unsigned int* collectionratefrac,
                          PARKINGCBF    cb)
{
    if(parkingparameter.init){
        _lwsem_wait(&parkingparameter.sem);
        if(occupiedduration != NULL)
            parkingparameter.occupiedduration         = *occupiedduration;
        if(emptyduration != NULL)
            parkingparameter.emptyduration            = *emptyduration;
        if(parkingmagnetthreshold != NULL)
            parkingparameter.parkingmagnetthreshold   = *parkingmagnetthreshold;
        if((collectionrateint != NULL)  && (collectionratefrac != NULL)){
            parkingparameter.collectionrateint        = *collectionrateint;
            parkingparameter.collectionratefrac       = *collectionratefrac;
            parkingparameter.mag3110rateint           = *collectionrateint;
            parkingparameter.mag3110ratefrac          = *collectionratefrac;
            parkingparameter.loopdelay                =  1000000000 / (parkingparameter.collectionrateint * 1000000 +
                                                                       parkingparameter.collectionratefrac);
            mag3110_get_real_rate((int *)&parkingparameter.mag3110rateint, (int *)&parkingparameter.mag3110ratefrac);
        }

        parkingparameter.cb                           =  cb;
        _lwsem_post(&parkingparameter.sem);

        return 0;
    }else {
        return -1;
    }
}



/*FUNCTION*----------------------------------------------------------------
*
* Function Name  : MAG_task
* Returned Value : None
* Comments       :
*END*--------------------------------------------------------------------*/

void Mag_task(uint32_t param)
{ 
    int   ret, i;

    int   temperature;
    int   magnetX, magnetY, magnetZ;
    int   magnetXavg = 0, magnetYavg = 0, magnetZavg = 0;
    int   magMSD;
    void *mag3110dev = NULL;
    struct mag3110_data  *mag3110data = NULL;

    unsigned int lloopdelay;
    unsigned int ldurationcount;
    unsigned int lparkingmagnetthreshold;
    unsigned int lmag3110rateint, lmag3110ratefrac;
    PARKINGCBF   lcb;

#if QCA4002_DEBUG
    int   peersocket;
    SOCKADDR_T foreign_addr;
    A_UINT32  ip_address;

    /* init qca4002 */
    atheros_driver_setup_init();
    HVAC_initialize_networking();
    qca4002_connect("archLinux64", "wpa2", "haidong2014");
#endif

    /* init mag3110 */
    mag3110dev = mag3110_init("i2c0:", 100000);

    if(mag3110dev == NULL)
        return;

    /* reset mag3110 user offset correction */
    ret = mag3110_update_user_offset_correction(mag3110dev, MAG3110_CHANNEL_X, 0);
    if(ret != 0)
        return;

    ret = mag3110_update_user_offset_correction(mag3110dev, MAG3110_CHANNEL_Y, 0);
    if(ret != 0)
        return;

    ret = mag3110_update_user_offset_correction(mag3110dev, MAG3110_CHANNEL_Z, 0);
    if(ret != 0)
        return;



    /* read back mag3110 temperature */
    ret = mag3110_get_temperature(mag3110dev, &temperature);
    if(ret < 0)
        return;
    printf("mag3110 current temperature is %d \n", temperature);

    _lwsem_create(&parkingparameter.sem, 1);

    parkingparameter.occupiedduration         = PARKINGDURATION;
    parkingparameter.emptyduration            = PARKINGDURATION;
    parkingparameter.parkingmagnetthreshold   = PARKINGMAGNETTHRESHOLD;
    parkingparameter.collectionrateint        = PARKINGCOLLECTIONRATEINT;
    parkingparameter.collectionratefrac       = PARKINGCOLLECTIONRATEFRAC;
    parkingparameter.mag3110rateint           = PARKINGCOLLECTIONRATEINT;
    parkingparameter.mag3110ratefrac          = PARKINGCOLLECTIONRATEFRAC;
    parkingparameter.cb                       = NULL;

    parkingparameter.loopdelay                =  1000000000 / (parkingparameter.collectionrateint * 1000000 +
            parkingparameter.collectionratefrac);

    mag3110_get_real_rate((int *)&parkingparameter.mag3110rateint, (int *)&parkingparameter.mag3110ratefrac);

    parkingparameter.init                     =  1;


#if 0
    /* Start mag3110 magnet caculate base value flash LED for 3.2 seconds  800ms flash frequency */
    for(i = 0; i < 4; i++){
        mux_74hc595_clear_bit(BSP_74HC595_0,  BSP_74HC595_AMB_LED);
        _time_delay(400);
        mux_74hc595_set_bit(BSP_74HC595_0,  BSP_74HC595_AMB_LED);
        _time_delay(400);
    }
#endif

    /* mag3110 magnet caculate base value flash LED for 8 seconds 200ms flash frequency */
    /* set mag3110 output frequency 80 HZ  */
    ret = mag3110_update_sample_frequency(mag3110dev, 80, 0);
    if(ret != 0)
        return;


    for(i = 0; i < 40; i++){
        ret = mag3110_get_magnet(mag3110dev, &magnetX, &magnetY, &magnetZ);
        if(ret != 0)
           return;

        magnetXavg += magnetX;
        magnetYavg += magnetY;
        magnetZavg += magnetZ;

        mux_74hc595_clear_bit(BSP_74HC595_0,  BSP_74HC595_AMB_LED);
        _time_delay(100);

        mux_74hc595_set_bit(BSP_74HC595_0,  BSP_74HC595_AMB_LED);
        _time_delay(100);
    }

    mux_74hc595_clear_bit(BSP_74HC595_0,  BSP_74HC595_AMB_LED);

    magnetXavg /= 40;
    magnetYavg /= 40;
    magnetZavg /= 40;

    parkingstate = PARKINGE;





#if QCA4002_DEBUG
    if(qca4002_is_connect()){
        qca4002_ipconfig_static("192.168.12.90", "255.255.255.0", "192.168.12.1");
    }
    else{
        printf("qca4002 is not connected to a AP \n");
        return;
    }


    if((peersocket = t_socket((void*)handle, ATH_AF_INET, SOCK_STREAM_TYPE, 0)) == A_ERROR){
        printf("t_socket create failed \n");
        return;
    }


    ath_inet_aton("192.168.12.1", (A_UINT32*)&ip_address);

    memset(&foreign_addr, 0, sizeof(foreign_addr));
    foreign_addr.sin_addr   = ip_address;
    foreign_addr.sin_port   = 2390;
    foreign_addr.sin_family = ATH_AF_INET;

    if(t_connect((void*)handle, peersocket, (&foreign_addr), sizeof(foreign_addr)) == A_ERROR){
        printf("t_socket connect failed \n");
        return;
    }
    else{
        mux_74hc595_set_bit(BSP_74HC595_0,  BSP_74HC595_AMB_LED);
        printf("Connect to 192.168.12.1\n");
    }


    while((mag3110data = CUSTOM_ALLOC(sizeof(*mag3110data))) == NULL) {
        //Wait till we get a buffer
        /*Allow small delay to allow other thread to run*/
        _time_delay(1);
    }
#endif




    /* set mag3110 output frequency to default  */
    ret = mag3110_update_sample_frequency(mag3110dev, parkingparameter.mag3110rateint, parkingparameter.mag3110ratefrac);
    if(ret != 0)
        return;


    do{
        

        ret = mag3110_get_magnet(mag3110dev, &magnetX, &magnetY, &magnetZ);
        if(ret != 0)
            break;

        magMSD = (magnetX - magnetXavg) * (magnetX - magnetXavg) +
                 (magnetY - magnetYavg) * (magnetY - magnetYavg) +
                 (magnetZ - magnetZavg) * (magnetZ - magnetZavg);


        printf("%d,%d,%d  %d \n", magnetX, magnetY, magnetZ, magMSD); 

#if QCA4002_DEBUG
        memset(mag3110data, 0, sizeof(*mag3110data));
#endif

        switch(parkingstate){
            case  PARKINGE:
                parking_check_next_parameter( mag3110dev,
                                             &lmag3110rateint, 
                                             &lmag3110ratefrac, 
                                             &lparkingmagnetthreshold, 
                                             &ldurationcount, 
                                             &lloopdelay, 
                                             &lcb);
                if(magMSD > lparkingmagnetthreshold){
                    ldurationcount--;
                    parkingstate = PARKINGE2O;
                }
                else{
                    //magnetXavg = (magnetXavg * 9 / 10) + magnetX / 10;
                    //magnetYavg = (magnetYavg * 9 / 10) + magnetY / 10;
                    //magnetZavg = (magnetZavg * 9 / 10) + magnetZ / 10;
                }
#if QCA4002_DEBUG
                mag3110data->parkingflag = 0;
#endif
                break;
            case  PARKINGE2O:
                if(magMSD > lparkingmagnetthreshold){
                    ldurationcount--;
                    if(ldurationcount == 0){
                        if(lcb != NULL)
                            lcb(PARKING_EVENT_OCCUPIED);
                        printf("PARKINGO \n");
                        parkingstate = PARKINGO;
                    }
                }
                else{
                    parkingstate =  PARKINGE;
                }
#if QCA4002_DEBUG
                mag3110data->parkingflag = 0;
#endif
                break;
            case  PARKINGO:
                parking_check_next_parameter( mag3110dev,
                                             &lmag3110rateint, 
                                             &lmag3110ratefrac, 
                                             &lparkingmagnetthreshold, 
                                             &ldurationcount, 
                                             &lloopdelay, 
                                             &lcb);
                if(magMSD < lparkingmagnetthreshold){
                    ldurationcount--;
                    parkingstate = PARKINGO2E;
                }
#if QCA4002_DEBUG
                mag3110data->parkingflag = 1;
#endif
                break;
            case  PARKINGO2E:
                if(magMSD < lparkingmagnetthreshold){
                    ldurationcount--;
                    if(ldurationcount == 0){
                        if(lcb != NULL)
                            lcb(PARKING_EVENT_EMPTY);
                        parkingstate = PARKINGE;
                        printf("PARKINGE \n");
                    }
                }
                else{
                    parkingstate =  PARKINGO;
                }
#if QCA4002_DEBUG
                mag3110data->parkingflag = 1;
#endif
                break;
            default:
                printf("Unknow parkingstate:%d \n", parkingstate);
                break;
        }

#if QCA4002_DEBUG
        mag3110data->x   = magnetX;
        mag3110data->y   = magnetY;
        mag3110data->z   = magnetZ;
        mag3110data->msd = magMSD;

        ret = t_send((void*)handle, peersocket, (unsigned char*)mag3110data, sizeof(*mag3110data), 0);
        //printf("t_send %d bytes \n", ret);
#endif

        _time_delay(lloopdelay);
    }while(1);

    if(mag3110dev != NULL)
        mag3110_deinit(mag3110dev);

    return;
}



