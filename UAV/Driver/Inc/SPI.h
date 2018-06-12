

#ifndef __SPI_H
#define __SPI_H

#ifdef __cplusplus
 extern "C" {
#endif

#define SpiID1   0
#define SpiID2   1
#define SpiID3   2
#define SpiID4   3
	 
#include "stm32f4xx_hal.h"
	 

class SpiCtrl
{
	public:
		SpiCtrl(int spiNum);
	  void Init(int baud);
	  void DeInit(void);
	  void TransmitReceive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);
	private:
		bool inited;
	  int  spiNum;
	  SPI_HandleTypeDef hSpi;
};


#ifdef __cplusplus
}
#endif

#endif 
