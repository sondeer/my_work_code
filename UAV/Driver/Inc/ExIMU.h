#ifndef __EXIMU_H
#define __EXIMU_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "stm32f4xx_hal.h"
#include "SPI.h"
	 

class ExImuCtrl
{
	public:
	  void Init(SpiCtrl *pSpi);
	  void DeInit(void);
	  void ReadExImu(float mc[7],int size);
	protected:
	private:
		SpiCtrl *pSpi;
};


#ifdef __cplusplus
}
#endif

#endif 
