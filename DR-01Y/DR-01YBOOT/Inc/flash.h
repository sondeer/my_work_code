

#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f0xx_hal.h"

class FlashWriter
{
public:
	FlashWriter(void);
	FlashWriter(uint32_t beginAddr,uint32_t size);
  int GetPage(uint32_t addr);
  int WriteFlash(uint32_t addr,void*data,int size);
	int EraseFlashPage(uint32_t pageAddr,int num);
  void EnableCtlFlash(void);
  void DisableCtlFlash(void);
private:
	uint32_t beginAddr;
  uint32_t flashSize;
};

#endif
