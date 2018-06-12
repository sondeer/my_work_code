

#ifndef __POWER_H
#define __POWER_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"


class PowerCtrl
{
	public:
		void Init(void);
	  float Get5VVoltage(void);
	  float GetMainVoltage(void);
	private:
		uint16_t adcBuff[16];
    float mainVoltage , v5Voltage;
};


#ifdef __cplusplus
}
#endif

#endif 
