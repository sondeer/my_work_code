
#ifndef __UART_H
#define __UART_H

#include "stm32f0xx_hal.h"

#define BUFF_SIZE    256

class UartDevice
{
public:
		UartDevice(int port);
	  void Init(int baud);
	  int DataLength(void);
	  int ReadData(void* buff,int size);
	  int WriteData(void* buff,int size);
	  void InttruptCallBack(void);
protected:
	  void BeginTransmitDMA(uint8_t *buff,int size);
    
private:
		UART_HandleTypeDef UartHandle;
		uint8_t rx_dataBuff[BUFF_SIZE];
    uint8_t tx_dataBuff[BUFF_SIZE];
	  int  tx_headIndex;
    int  tx_tailIndex;
	  int  rx_tailIndex;
};

#endif
