
#ifndef __RGBLED_H
#define __RGBLED_H

#include "stm32f0xx_hal.h"


void RgbLedInit(void);
void WriteRgbLed(int index,int rgb);
void ShowRgbLed(void);
void SetAllRgbLed(int rgb);
void ClearAllRgbLed(void);

#endif
