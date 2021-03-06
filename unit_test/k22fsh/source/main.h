#ifndef __main_h_
#define __main_h_
#include <mqx.h>
#include <bsp.h>


#define APPLICATION_HAS_SHELL 


#ifdef __USB_OTG__ 
#include "otgapi.h" 
#include "devapi.h" 
#else 
#include "hostapi.h" 
#endif 


#define MAIN_TASK   1


#define RTCS_DHCP 1




#define RTCS_PPP 0

    

extern void Main_task (uint_32);



/* PPP device must be set manually and 
** must be different from the default IO channel (BSP_DEFAULT_IO_CHANNEL) 
*/
#define PPP_DEVICE      "ittyb:" 

/*
** Define PPP_DEVICE_DUN only when using PPP to communicate
** to Win9x Dial-Up Networking over a null-modem
** This is ignored if PPP_DEVICE is not #define'd
*/
#define PPP_DEVICE_DUN  1

/*
** Define the local and remote IP addresses for the PPP link
** These are ignored if PPP_DEVICE is not #define'd
*/
#define PPP_LOCADDR     IPADDR(192,168,0,216)
#define PPP_PEERADDR    IPADDR(192,168,0,217)

/*
** Define a default gateway
*/
#define GATE_ADDR       IPADDR(192,168,0,1)

#endif /* __main_h_ */

