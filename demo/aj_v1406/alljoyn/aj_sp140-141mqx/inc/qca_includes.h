#ifndef  __QCA_INCLUDES_H
#define  __QCA_INCLUDES_H
/******************************************************************************
 * Copyright 2013, Qualcomm Connected Experiences, Inc.
 *
 ******************************************************************************/
#include <psptypes.h>
#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include <message.h>
#include <string.h>

#include <stdlib.h>
#include "rtcs.h"
#include "ipcfg.h"
#include "atheros_wifi_api.h"
#include "atheros_wifi.h"
#include "atheros_stack_offload.h"
#include "enet_wifi.h"
#include "iwcfg.h"


#define ATH_REG_OP_READ 			(1)
#define ATH_REG_OP_WRITE 			(2)
#define ATH_REG_OP_RMW 				(3)

enum ath_private_ioctl_cmd {
	ATH_REG_QUERY = ATH_CMD_LAST,
	ATH_MEM_QUERY
};


typedef struct {
	uint_32 address;
	uint_32 value;
	uint_32 mask;
	uint_32 size;
	uint_32 operation;
} ATH_REGQUERY;

/********************** Function Prototypes **************************************/

void atheros_driver_setup(void);

char *inet_ntoa( A_UINT32 addr, char *res_str );
char *inet6_ntoa(char* addr, char * str);
void ath_udp_echo (int_32 argc, char_ptr argv[]);
char * print_ip6(IP6_ADDR_T * addr, char * str);
A_INT32 ipconfig_static(A_INT32 argc, char* argv[]);
A_INT32 ipconfig_dhcp(A_INT32 argc, char* argv[]);

uint_32 mystrtoul(const char* arg, const char* endptr, int base);
int_32 ath_inet_aton(const char*  name, A_UINT32* ipaddr_ptr);

extern _enet_handle    handle;

#endif
