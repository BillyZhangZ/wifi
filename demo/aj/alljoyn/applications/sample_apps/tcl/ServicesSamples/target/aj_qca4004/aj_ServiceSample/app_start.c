/*
  * Copyright (c) 2013 Qualcomm Atheros, Inc..
  * All Rights Reserved.
  * Qualcomm Atheros Confidential and Proprietary.
  */

#include "qcom_system.h"
#include <qcom/base.h>
void user_main(void);

void app_start(void)
{
    QCOM_SYS_START_PARA_t sysStartPara;

    sysStartPara.isMccEnabled = 0;
    sysStartPara.numOfVdev = 1;
    
    #if defined(ENABLED_MCC_MODE)
    sysStartPara.isMccEnabled = ENABLED_MCC_MODE;
    #endif

    #if defined(NUM_OF_VDEVS)
    sysStartPara.numOfVdev = NUM_OF_VDEVS;
    #endif

    qcom_sys_start(user_main, &sysStartPara);
}


