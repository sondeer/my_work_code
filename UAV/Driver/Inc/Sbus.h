
#ifndef __SBUS_h
#define __SBUS_h


#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"

#define RXBUFFERSIZE  128


class SbusCtrl
{
	public:
	  void Init(void);
	  int  SbusRun2ms(uint32_t clock);
	  bool IsLinked(void);
	  uint16_t GetChannelData(int channel);
	protected:
		void UART_Init(void);
	private:
    uint8_t aRxBuffer[RXBUFFERSIZE];
	  uint16_t channelData[16];
	  int    channelNum;
    int    receiveNum;
	  bool   teleLinkFlag;
};




	 
#ifdef __cplusplus
}
#endif

#endif

