


#ifndef __HMC5983_H
#define __HMC5983_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "stm32f4xx_hal.h"
#include "SPI.h"
	 

class Hmc5983Ctrl
{
	public:
	  void Init(SpiCtrl *pSpi);
	  void DeInit(void);
	  void ReadMagneticFloat(float mc[3]);
	  void ReadMagnetic(int16_t mc[3]);
	protected:
		void ReadRegister(uint8_t addr,void *data,int size);
	  void WriteRegister(uint8_t addr,void *data,int size);
	private:
		SpiCtrl *pSpi;
};


#ifdef __cplusplus
}
#endif

#endif 
