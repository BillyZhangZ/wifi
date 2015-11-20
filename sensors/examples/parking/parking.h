
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
#ifndef __PARKING_
#define __PARKING_

typedef enum {
    PARKING_EVENT_EMPTY   = 0,
    PARKING_EVENT_OCCUPIED,
}PARKINGEVENT;

typedef void *(*PARKINGCBF)(PARKINGEVENT);

int parking_set_parameter(unsigned int* occupiedduration, 
                          unsigned int* emptyduration,
                          unsigned int* parkingmagnetthreshold,
                          unsigned int* collectionrateint,
                          unsigned int* collectionratefrac,
                          PARKINGCBF    cb);

void Mag_task(uint32_t param);

#endif

