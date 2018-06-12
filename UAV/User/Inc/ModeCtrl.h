


#ifndef __RCCTRL_H
#define __RCCTRL_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "Sbus.h"
	  
class RcCtrl
{
	public:
		void  Init(void);
    float GetThrottle(void);
	  float GetTargetPitch(void);
		float GetTargetRoll(void);
	  float GetTargetYaw(void);
	  void  Run2ms(uint32_t clock);
	  bool  IsLinked(void);
	private:
		SbusCtrl     sbus;
	  uint8_t      channelsNum[16];
	  uint16_t      subValue[16];
	  int16_t      widthValue[16];
};


#ifdef __cplusplus
}
#endif

#endif 



