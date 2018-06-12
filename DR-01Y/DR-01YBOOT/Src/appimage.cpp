
#include "appimage.h"
#include "flash.h"
#include <string.h>

#define RAM_BEGIN_ADDR  0x20000000
#define FIRMWAREINF_ADDR 0x08003800

#define POWER_STATE 0x200000C4

typedef void (*Function)();

AppImage::AppImage()
{
	this->p_FirmwareInf = (FirmwareInfStruct*)FIRMWAREINF_ADDR;
}

int  AppImage::GoToApp(uint8_t arg)
{
	uint8_t * power_state = (uint8_t * ) POWER_STATE;	
	
	if(IsAppCheckRight() != 0)
		return -1;
	memcpy((uint8_t*)RAM_BEGIN_ADDR,(uint8_t*)p_FirmwareInf->appBeginAddr,256);
		
	if(arg == 1) {	
		power_state[0] = 0x69;
		power_state[1] = 2;
		power_state[2] = 0x02^0xFF;
		power_state[3] = 0x96;
	}
	
	uint32_t mspAddr = *(uint32_t *)(RAM_BEGIN_ADDR);
  Function Jump_To_App = (Function)*(uint32_t *)(RAM_BEGIN_ADDR + 4);
	__disable_irq();
	__HAL_RCC_USART2_CLK_DISABLE();
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_DMA1_CLK_DISABLE();
  __set_MSP(mspAddr);
	__HAL_SYSCFG_REMAPMEMORY_SRAM();
	
	//HAL_NVIC_DisableIRQ(SysTick_IRQn);
	SysTick->CTRL &= ~(1);

		
  Jump_To_App();
	return 0;
}

int  AppImage::SetWriteAddr(uint32_t addr)
{
	this->writeAddr = addr;
	return 0;
}

int  AppImage::SetDataBuff(int index,uint8_t *data,int size)
{
	if(index >= 128)
		return -1;
	memcpy(this->dataBuff + index * 16 , data , size);
	return 0;
}

uint32_t AppImage::CheckBuffData(uint32_t num)
{
	uint32_t chsum = 0;
	uint32_t *p = (uint32_t *)dataBuff;
	
	for(int i = 0 ; i < num / 4 ; i ++)
	{
		chsum ^= p[i];
	}
	
	return chsum;
}

int  AppImage::IsAppCheckRight(void)
{
	uint32_t *p = (uint32_t*)p_FirmwareInf;
	uint32_t cksum = 0;
	
	if(p_FirmwareInf->activeFlag != 0x55aa5a5a)
		return -1;
	
	for(int i = 0 ; i < sizeof(FirmwareInfStruct) / 4 - 1; i ++)
	{
		cksum ^= p[i];
	}
	if(p_FirmwareInf->infCheckSum != cksum)
		return -1;
	
	p = (uint32_t*)p_FirmwareInf->appBeginAddr;
	
	cksum = 0;
	for(int i = 0 ; i < p_FirmwareInf->appImageLength / 4; i ++)
	{
		cksum ^= p[i];
	}
	if(p_FirmwareInf->appCheckSum != cksum)
		return -1;
	return 0;
}

int  AppImage::WriteBuffFlash(uint32_t length)
{
	FlashWriter flWter;
	flWter.EnableCtlFlash();
	flWter.EraseFlashPage(this->writeAddr,1);
	flWter.WriteFlash(this->writeAddr,this->dataBuff,length);
	flWter.DisableCtlFlash();
	return 0;
}

int  AppImage::WriteFirmwareInf(FirmwareInfStruct *pInf)
{
	uint32_t cksum = 0;
	uint32_t *p = (uint32_t *)pInf;
	pInf->activeFlag = 0x55aa5a5a;
	for(int i = 0 ; i < sizeof(FirmwareInfStruct) / 4 - 1; i ++)
	{
		cksum ^= p[i];
	}
	pInf->infCheckSum = cksum;
	FlashWriter flWter;
	flWter.EnableCtlFlash();
	flWter.EraseFlashPage((uint32_t)p_FirmwareInf,1);
	flWter.WriteFlash((uint32_t)p_FirmwareInf,pInf,sizeof(FirmwareInfStruct));
	flWter.DisableCtlFlash();
	return 0;
}

#define SOFTWARE_VERSION      0x80040C0
#define HARDWARE_VERSION			0x80040C4


uint32_t  AppImage::GetSoftVer(void)
{
	uint32_t * software_version = (uint32_t *)SOFTWARE_VERSION;
	// return this->p_FirmwareInf->appVersion;
	return *software_version;
}

uint32_t  AppImage::GetHardVer(void)
{
	uint32_t * hardware_version = (uint32_t *)HARDWARE_VERSION;
	// return this->p_FirmwareInf->appVersion;
	return *hardware_version;
}

