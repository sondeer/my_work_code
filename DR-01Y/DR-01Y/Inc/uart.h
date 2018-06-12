
#ifndef __UART_H
#define __UART_H

#include "stm32f0xx_hal.h"

#define BUFF_SIZE    256


void UartInit(int baud);
int UartDataLength(void);
int UartReadData(void* buff,int size);
int UartWriteData(void* buff,int size);
void UartInttruptCallBack(void);
void UartBeginTransmitDMA(uint8_t *buff,int size);
   

#endif
