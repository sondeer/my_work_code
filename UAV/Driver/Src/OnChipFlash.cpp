
#include "OnChipFlash.h"

void OnChipFlash::Init(uint32_t sector,uint32_t dataBlockSize)
{
	this->sectorId = sector;
	this->dataBlockSize = dataBlockSize;
	
	
}

int OnChipFlash::ReadData(void *buff,int size)
{
}

int OnChipFlash::WriteData(void *buff,int size)
{
}

