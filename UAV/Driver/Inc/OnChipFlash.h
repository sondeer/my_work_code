
#ifndef __ONCHIPFLASH_H
#define __ONCHIPFLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"


class OnChipFlash
{
	public:
		void Init(uint32_t sector,uint32_t dataBlockSize);
	  int  ReadData(void *buff,int size);
	  int  WriteData(void *buff,int size);
	protected:
	private:
	  uint32_t sectorId;
	  uint32_t sectorSize;
	  uint32_t dataBlockSize;
	  uint32_t sectorBeginAddr;
};

#ifdef __cplusplus
}
#endif

#endif 

