
#include "Power.h"

void PowerCtrl::Init(void)
{
	GPIO_InitTypeDef          GPIO_InitStruct;
	ADC_HandleTypeDef         AdcHandle;
	ADC_ChannelConfTypeDef    sConfig;
	DMA_HandleTypeDef         hdma_adc;
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_DMA2_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	hdma_adc.Instance = DMA2_Stream0;
  hdma_adc.Init.Channel  = DMA_CHANNEL_0;
  hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
  hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_adc.Init.MemDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_adc.Init.Mode = DMA_CIRCULAR;
  hdma_adc.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_adc.Init.FIFOMode = DMA_FIFOMODE_DISABLE;         
  hdma_adc.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
  hdma_adc.Init.MemBurst = DMA_MBURST_SINGLE;
  hdma_adc.Init.PeriphBurst = DMA_PBURST_SINGLE; 

  HAL_DMA_Init(&hdma_adc);
	
  AdcHandle.Instance = ADC1;
  AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV8;
  AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;
  AdcHandle.Init.ScanConvMode = ENABLE;
  AdcHandle.Init.ContinuousConvMode = ENABLE;
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;
  AdcHandle.Init.NbrOfDiscConversion = 0;
  AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.NbrOfConversion = 2;
  AdcHandle.Init.DMAContinuousRequests = ENABLE;
  AdcHandle.Init.EOCSelection = DISABLE;
	
	HAL_ADC_Init(&AdcHandle) ;
	
	sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  sConfig.Offset = 0; 
  HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) ;
	
	sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = 2;
	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) ;
	
	__HAL_LINKDMA(&AdcHandle, DMA_Handle, hdma_adc);
	HAL_ADC_Start_DMA(&AdcHandle,(uint32_t*)this->adcBuff,16);
	
	v5Voltage = mainVoltage = 0;
}

float PowerCtrl::Get5VVoltage(void)
{
	float sum = 0;
	for( int i = 0 ; i < 8 ; i ++)
	{
		sum += adcBuff[i * 2 + 1];
	}
	return sum * 3.3f * 2 / (8 * 4096);
}

float PowerCtrl::GetMainVoltage(void)
{
	float sum = 0;
	for( int i = 0 ; i < 8 ; i ++)
	{
		sum += adcBuff[i * 2];
	}
	return sum * 3.3f * 11 / (8 * 4096);
}
/*
void PowerCtrl::ADCRun1S(void)
{
	mainVoltage = GetMainVoltage();
	v5Voltage = Get5VVoltage();
}
*/
