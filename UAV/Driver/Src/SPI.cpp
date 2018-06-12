
#include "SPI.h"

SpiCtrl::SpiCtrl(int spiId)
{
	this->spiNum = spiId;
	this->inited = false;
}

void SpiCtrl::Init(int baud)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	
	if(this->spiNum == SpiID2)
	{
		__SPI2_CLK_ENABLE();
		__GPIOB_CLK_ENABLE();
		
		GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
	  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		
		this->hSpi.Instance = SPI2;
    this->hSpi.Init.Mode = SPI_MODE_MASTER;
    this->hSpi.Init.Direction = SPI_DIRECTION_2LINES;
    this->hSpi.Init.DataSize = SPI_DATASIZE_8BIT;
    this->hSpi.Init.CLKPolarity = SPI_POLARITY_HIGH;
    this->hSpi.Init.CLKPhase = SPI_PHASE_2EDGE;
    this->hSpi.Init.NSS = SPI_NSS_SOFT;
    this->hSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    this->hSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    this->hSpi.Init.TIMode = SPI_TIMODE_DISABLED;
    this->hSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    HAL_SPI_Init(&this->hSpi);
	}
	else if(this->spiNum == SpiID4)
	{
		__SPI4_CLK_ENABLE();
		__GPIOE_CLK_ENABLE();
		
		GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
	  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
		
		this->hSpi.Instance = SPI4;
    this->hSpi.Init.Mode = SPI_MODE_MASTER;
    this->hSpi.Init.Direction = SPI_DIRECTION_2LINES;
    this->hSpi.Init.DataSize = SPI_DATASIZE_8BIT;
    this->hSpi.Init.CLKPolarity = SPI_POLARITY_LOW;
    this->hSpi.Init.CLKPhase = SPI_PHASE_1EDGE;
    this->hSpi.Init.NSS = SPI_NSS_SOFT;
    this->hSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    this->hSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    this->hSpi.Init.TIMode = SPI_TIMODE_DISABLED;
    this->hSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    HAL_SPI_Init(&this->hSpi);
	}
}

void SpiCtrl::DeInit(void)
{
	if(this->spiNum == SpiID2)
	{
	  __SPI2_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
	}
	else if(this->spiNum == SpiID4)
	{
	  __SPI4_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2|GPIO_PIN_5|GPIO_PIN_6);
	}
}

void SpiCtrl::TransmitReceive(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
	HAL_SPI_TransmitReceive(&this->hSpi,pTxData,pRxData,Size,Timeout);
}
