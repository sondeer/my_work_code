
#include "rgbled.h"


static uint8_t ledDataBuff[290];
SPI_HandleTypeDef SpiHandle;

#define HIGH_DATA  0x78
#define LOW_DATA   0x60

void RgbLedInit(void)
{
	DMA_HandleTypeDef hdma;
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	__HAL_RCC_SPI2_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();
	
	GPIO_InitStruct.Pin       = GPIO_PIN_3;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_SPI2;

  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	SpiHandle.Instance               = SPI2;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  SpiHandle.Init.CRCPolynomial     = 7;
  SpiHandle.Init.DataSize          = SPI_DATASIZE_7BIT;
  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  SpiHandle.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
  SpiHandle.Init.CRCLength         = SPI_CRC_LENGTH_DATASIZE;
  SpiHandle.Init.Mode = SPI_MODE_MASTER;

  HAL_SPI_Init(&SpiHandle) ;
	
	
	hdma.Instance                 = DMA1_Channel5;
  hdma.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma.Init.MemInc              = DMA_MINC_ENABLE;
  hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma.Init.Mode                = DMA_NORMAL;
  hdma.Init.Priority            = DMA_PRIORITY_HIGH;
	hdma.XferHalfCpltCallback = NULL;

  HAL_DMA_Init(&hdma);
	SET_BIT(SPI2->CR2, SPI_CR2_TXDMAEN);
	SET_BIT(SPI2->CR1, SPI_CR1_SPE);
	
	ledDataBuff[0] = ledDataBuff[289] = 0;
	/*
	uint8_t buff[256];
	
	for(int i = 0 ; i < 256 ; i ++)
	{
		buff[i] = 0xF8;
	}
	
	for(int i = 24 ; i < 48 ; i ++)
	{
		buff[i] = 0xE0;
	}
//	buff[0] = 0;
	buff[255] = 0;
	while(1)
	{
		HAL_SPI_Transmit(&SpiHandle, buff, 256, 100);
		HAL_Delay(100);
	}*/
}

void WriteRgbLed(int index,int rgb)
{
	int dataIndex = 24 *  index + 1;
	for(int i = 0 ; i < 8 ; i ++)
	{
		if(rgb & (1 << (15 - i)))
			ledDataBuff[dataIndex + i] = HIGH_DATA;
		else 
			ledDataBuff[dataIndex + i] = LOW_DATA;
		
		if(rgb & (1 << (23 - i)))
			ledDataBuff[dataIndex + 8 + i] = HIGH_DATA;
		else 
			ledDataBuff[dataIndex + 8+ i] = LOW_DATA;
		
		if(rgb & (1 << (7 - i)))
			ledDataBuff[dataIndex + 16 + i] = HIGH_DATA;
		else 
			ledDataBuff[dataIndex + 16 + i] = LOW_DATA;
	}
}

void SetAllRgbLed(int rgb)
{
	for(int i = 0 ; i < 12 ; i ++)
	{
		WriteRgbLed(i,rgb);
	}
}

void ClearAllRgbLed(void)
{
	for(int i = 1 ; i < 289 ; i ++)
	{
		ledDataBuff[i] = LOW_DATA;
	}
}

void  ShowRgbLed(void)
{
  DMA1_Channel5->CCR &= ~DMA_CCR_EN;
	DMA1_Channel5->CNDTR = 290;
	DMA1_Channel5->CMAR = (uint32_t)ledDataBuff;
	DMA1_Channel5->CPAR = (uint32_t)&SPI2->DR;
	DMA1_Channel5->CCR |= DMA_CCR_EN;
}
