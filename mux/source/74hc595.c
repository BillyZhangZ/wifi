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
*END************************************************************************/

 
#include <mqx.h>
#include <bsp.h>
#include "74hc595.h"

#define MAX_74HC595_COUNT   6

typedef struct _mux_74hc595 {

    LWGPIO_STRUCT   ds;
    LWGPIO_STRUCT   shcp;
    LWGPIO_STRUCT   stcp;

    LWSEM_STRUCT    sem;
    uint32_t        count;
    unsigned short  outputvalue;

}MUX_74HC595, *MUX_74HC595_PTR;


static MUX_74HC595_PTR g74hc595[MAX_74HC595_COUNT];

static int _mux_74hc595_set_newoutputvalue(uint32_t index, uint32_t newoutputvalue){

    MUX_74HC595_PTR  mux74hc595     = g74hc595[index];
    uint32_t         shiftbitindex  = mux74hc595->count * 8;


    lwgpio_set_value(&mux74hc595->stcp, LWGPIO_VALUE_LOW); 

    do {

        lwgpio_set_value(&mux74hc595->shcp, LWGPIO_VALUE_LOW); 

        if(newoutputvalue & (1 << (shiftbitindex - 1))){
            lwgpio_set_value(&mux74hc595->ds, LWGPIO_VALUE_HIGH); 
        }
        else{
            lwgpio_set_value(&mux74hc595->ds, LWGPIO_VALUE_LOW); 
        }

        lwgpio_set_value(&mux74hc595->shcp, LWGPIO_VALUE_HIGH); 

        shiftbitindex--;

    }while(shiftbitindex != 0);

    lwgpio_set_value(&mux74hc595->stcp, LWGPIO_VALUE_HIGH); 

    mux74hc595->outputvalue = newoutputvalue;

    return 0;
}

int mux_74hc595_init(uint32_t index,
                     uint32_t count,
                     uint32_t shcppinid, uint32_t shcppinmux,
                     uint32_t stcppinid, uint32_t stcppinmux,
                     uint32_t dspinid,   uint32_t dspinmux
                     )
{

    if(g74hc595[index] != NULL){
        printf("mux_74hc595_init %d is not null \n");
        return -1;
    }

    if(index >= MAX_74HC595_COUNT){
        printf("mux_74hc595_init index more than max value \n");
        return -1;
    }

    if(count > 4){
        printf("mux_74hc595_init only support max count is 4 \n");
        return -1;

    }

    g74hc595[index] = _mem_alloc_system(sizeof(MUX_74HC595));

    if(g74hc595[index] == NULL){
        printf("mux_74hc595_init %d alloc mem failed \n", index);
        return -1;
    }

    if(MQX_OK != _lwsem_create(&g74hc595[index]->sem, 1)){

        printf("mux_74hc595_init %d _lwsem_create failed \n", index);
        _mem_free(g74hc595[index]);
        return -1;
    }

    _lwsem_wait(&g74hc595[index]->sem);

    lwgpio_init(&g74hc595[index]->shcp,
                shcppinid,
                LWGPIO_DIR_OUTPUT,
                LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(&g74hc595[index]->shcp, shcppinmux);

    lwgpio_init(&g74hc595[index]->stcp,
                stcppinid,
                LWGPIO_DIR_OUTPUT,
                LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(&g74hc595[index]->stcp, stcppinmux);

    lwgpio_init(&g74hc595[index]->ds,
                dspinid,
                LWGPIO_DIR_OUTPUT,
                LWGPIO_VALUE_NOCHANGE);
    lwgpio_set_functionality(&g74hc595[index]->ds, dspinmux);

    g74hc595[index]->count = count;

    _mux_74hc595_set_newoutputvalue(index, 0);
    g74hc595[index]->outputvalue = 0;

    _lwsem_post(&g74hc595[index]->sem);


    return  0;

}

int mux_74hc595_deinit(uint32_t index){

    
    if(g74hc595[index] != NULL){
        _lwsem_destroy(&g74hc595[index]->sem);
        _mem_free(g74hc595[index]);
        return 0;
    }
    else{
        return -1;
    }
}


int mux_74hc595_set_bit(uint32_t index, uint32_t bitindex){

    MUX_74HC595_PTR  mux74hc595;
    uint32_t         newoutputvalue;

    if(g74hc595[index] == NULL){
        printf("mux_74hc595_set_bit %d is null \n", index);
        return -1;
    }

    mux74hc595 = g74hc595[index];

    if(bitindex >= mux74hc595->count * 8){
        printf("mux_74hc595_set_bit bitindex %d more than max %d \n", bitindex, mux74hc595->count * 8 - 1);
        return -1;
    }

    _lwsem_wait(&mux74hc595->sem);

    if(!( (mux74hc595->outputvalue) & ( 1 << bitindex) )){
        newoutputvalue = mux74hc595->outputvalue | (1 << bitindex);
        _mux_74hc595_set_newoutputvalue(index, newoutputvalue);
        mux74hc595->outputvalue = newoutputvalue;
    }

    _lwsem_post(&mux74hc595->sem);

    return 0;
}

int mux_74hc595_clear_bit(uint32_t index, uint32_t bitindex){

    MUX_74HC595_PTR  mux74hc595;
    uint32_t         newoutputvalue;

    if(g74hc595[index] == NULL){
        printf("mux_74hc595_set_bit %d is null \n", index);
        return -1;
    }

    mux74hc595 = g74hc595[index];

    if(bitindex >= mux74hc595->count * 8){
        printf("mux_74hc595_set_bit bitindex %d more than max %d \n", bitindex, mux74hc595->count * 8 - 1);
        return -1;
    }

    _lwsem_wait(&mux74hc595->sem);

    if((mux74hc595->outputvalue) & ( 1 << bitindex) ){
        newoutputvalue = mux74hc595->outputvalue & ~(1 << bitindex);
        _mux_74hc595_set_newoutputvalue(index, newoutputvalue);
        mux74hc595->outputvalue = newoutputvalue;
    }

    _lwsem_post(&mux74hc595->sem);

    return 0;

}
