
#include "flash.h"

FlashWriter::FlashWriter(void)
{
}

FlashWriter::FlashWriter(uint32_t beginAddr,uint32_t size)
{
	
}

int FlashWriter::GetPage(uint32_t addr)
{
	return addr & 0xfffff800;  
}

int FlashWriter::WriteFlash(uint32_t addr,void*data,int size)
{
	uint32_t writeAddr = addr;
	uint32_t *p_data = (uint32_t *)data;
	for(int i = 0 ; i < size ; i += 4)
	{
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, writeAddr + i, *p_data) != HAL_OK)
			return -1;
		p_data ++;
	}
	return size;
}

int FlashWriter::EraseFlashPage(uint32_t pageAddr,int num)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError = 0;
	
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.PageAddress = pageAddr;
  EraseInitStruct.NbPages     = num;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		return PAGEError;
	}
	return 0;
}

void FlashWriter::EnableCtlFlash(void)
{
	HAL_FLASH_Unlock();
}
  
void FlashWriter::DisableCtlFlash(void)
{
	HAL_FLASH_Lock();
}
