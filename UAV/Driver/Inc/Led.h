


#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"

#define LED_ON     GPIO_PIN_RESET
#define LED_OFF    GPIO_PIN_SET

#define LED_PORT_CLK_ENABLE()          __GPIOD_CLK_ENABLE()	 

#define LED_PORT       GPIOD
#define REDLED_PIN     GPIO_PIN_5 
#define GREENLED_PIN   GPIO_PIN_4 
#define BLUELED_PIN    GPIO_PIN_3 
	 

class LedCtrl
{
	public:
		void LedInit(void);
	  void SetRedLed(int state);
	  void SetGreenLed(int state);
	  void SetBlueLed(int state);
	  void ToggleRedLed(void);
	  void ToggleGreenLed(void);
	  void ToggleBlueLed(void);
};

extern LedCtrl led;

#ifdef __cplusplus
}
#endif

#endif 

