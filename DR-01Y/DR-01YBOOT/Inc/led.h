
#ifndef __LED_H
#define __LED_H

#include "stm32f0xx_hal.h"

#define MAX_NUM_SIZE 16
#define DATA_BUFF_SIZE 256


class LedCtrl 
{
	public:
		LedCtrl();
	  void Init(void);
	  void Update(void);
	  void SetLedState(int state);
	  void SetFlashTime(int mTime);
	
	private:
		uint32_t lastFlashTime;
	  int flashTimeOut;
};

#endif
