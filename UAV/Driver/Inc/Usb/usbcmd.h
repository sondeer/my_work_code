


#ifndef __USBCMD_H
#define __USBCMD_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"


void DoUsbCmd(void *buff,int len);
void LoadParameter(void);

#ifdef __cplusplus
}
#endif

#endif
