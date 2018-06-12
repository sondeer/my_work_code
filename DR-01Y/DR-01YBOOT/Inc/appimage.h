
#ifndef __APPIMAGE_H
#define __APPIMAGE_H

#include "stm32f0xx_hal.h"

typedef struct 
{
	uint32_t activeFlag;
	uint32_t appBeginAddr;
	uint32_t appImageLength;
	uint32_t appVersion;
	uint32_t appCheckSum;
	uint32_t infCheckSum;
}FirmwareInfStruct; 

class AppImage
{
	public:
		AppImage();
	  int  GoToApp(uint8_t arg);
	  int  SetWriteAddr(uint32_t addr);
	  int  SetDataBuff(int index,uint8_t *data,int size);
	  uint32_t CheckBuffData(uint32_t num);
	  int  IsAppCheckRight(void); 
	  int  WriteBuffFlash(uint32_t length);
	  int  WriteFirmwareInf(FirmwareInfStruct *pInf);
	  uint32_t GetSoftVer(void);
	  uint32_t GetHardVer(void);
	private:
		uint8_t dataBuff[2048];
	  uint32_t writeAddr;
	  FirmwareInfStruct *p_FirmwareInf;
};

#endif
