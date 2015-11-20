
/**HEADER********************************************************************
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
 **************************************************************************
 *
 * Comments:
 *
 *END************************************************************************/

#ifndef __QCA4002_H_
#define __QCA4002_H_

#include <atheros_stack_offload.h>
#include "atheros_wifi.h"
#include "atheros_wifi_api.h"

extern void   *handle;

void atheros_driver_setup_init(void);
void HVAC_initialize_networking(void);
int qca4002_connect(char* ssid, char *securitymode, char *passwd);
int qca4002_is_connect(void);
int qca4002_ipconfig_static(char* ip_addr_string, char *mask_string, char *gw_string);
int_32 ath_inet_aton ( const char*        name, /* [IN] dotted decimal IP address */
                       A_UINT32*     ipaddr_ptr /* [OUT] binary IP address */
                      );

#endif
