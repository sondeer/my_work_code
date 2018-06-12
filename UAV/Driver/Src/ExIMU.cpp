
#include "ExIMU.h"
#include <string.h>

#define SetCSHigh()  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET)
#define SetCSLow()   HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_RESET)

void ExImuCtrl::Init(SpiCtrl *pSpi)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__GPIOE_CLK_ENABLE();

	SetCSHigh();
	GPIO_InitStruct.Pin   = GPIO_PIN_4;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  	
	this->pSpi = pSpi;
}

void ExImuCtrl::DeInit(void)
{
}

void ExImuCtrl::ReadExImu(float mc[7],int size)
{
	uint8_t wrbuff[32] = {0};
	uint8_t rdbuff[32];

	SetCSLow();
	this->pSpi->TransmitReceive(wrbuff,(uint8_t*)rdbuff,size * 4 + 1,1);
	SetCSHigh();
	
	memcpy(mc,rdbuff + 1,size * 4);
}
