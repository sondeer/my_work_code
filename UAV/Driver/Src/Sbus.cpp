#include "Sbus.h"
#include "main.h"
#include <string.h>


typedef __packed  struct
{
	uint8_t head;
	uint16_t data[16];
	uint8_t end[2];
}SbusDataStruct;

UART_HandleTypeDef UartHandle;
DMA_HandleTypeDef hdma_rx;

void SbusCtrl::Init()
{
  UART_Init();
	this->teleLinkFlag = false;
	this->channelNum = 0;
	this->receiveNum = 0;
}

void SbusCtrl::UART_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	__GPIOE_CLK_ENABLE();

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET); 
	   
  __UART8_CLK_ENABLE();
	
  GPIO_InitStruct.Pin       = GPIO_PIN_0;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;  //GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_UART8;
    
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
  UartHandle.Instance          = UART8;
  UartHandle.Init.BaudRate     = 100000;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_9B;
  UartHandle.Init.StopBits     = UART_STOPBITS_2;
  UartHandle.Init.Parity       = UART_PARITY_EVEN;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
	
	HAL_UART_Init(&UartHandle);
	
	__DMA1_CLK_ENABLE();
	
	hdma_rx.Instance = DMA1_Stream6;
  hdma_rx.Init.Channel = DMA_CHANNEL_5;
  hdma_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_rx.Init.Mode = DMA_CIRCULAR;
  hdma_rx.Init.Priority = DMA_PRIORITY_LOW;
  hdma_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  HAL_DMA_Init(&hdma_rx);

  __HAL_LINKDMA(&UartHandle,hdmarx,hdma_rx);
	
	HAL_UART_Receive_DMA(&UartHandle, (uint8_t *)aRxBuffer, RXBUFFERSIZE);
}

SbusDataStruct sbusData;
int SbusCtrl::SbusRun2ms(uint32_t clock)
{
	static uint32_t oldClock = 0;
	static int oldIndex = 0;
	static int lastReadIndex = - 1;
  int nowIndex = hdma_rx.Instance->NDTR;
	nowIndex = RXBUFFERSIZE - nowIndex;
	
	if(oldIndex != nowIndex)
	{
		oldIndex = nowIndex;
		oldClock = clock;
	}
	else if(clock - oldClock > 30)
	{
		this->teleLinkFlag = false;
	}
	else if(clock - oldClock > 2)
	{
		if(lastReadIndex >= 0)
		{
			
			int sbusNum;
			
		  if(nowIndex > lastReadIndex)
			{
				sbusNum = nowIndex - lastReadIndex;
				if(sbusNum < 35)
				memcpy(&sbusData,aRxBuffer + lastReadIndex,sbusNum);
			}
			else if(nowIndex < lastReadIndex)
			{
				sbusNum = nowIndex + RXBUFFERSIZE - lastReadIndex;
				if(sbusNum < 35)
				{
				  memcpy(&sbusData,aRxBuffer + lastReadIndex,RXBUFFERSIZE - lastReadIndex);
				  memcpy((uint8_t*)&sbusData + (RXBUFFERSIZE - lastReadIndex),aRxBuffer,nowIndex);
				}
			} 
			
			if(sbusData.head == 0x0f && sbusNum < 35)
			{
				for(int i = 0 ; i < 16 ; i ++)
				{
					int dataIndex = (i * 11) / 16;
					int iIndex = (i * 11) % 16;
					
					this->channelData[i] = ((sbusData.data[dataIndex] >> iIndex) | (sbusData.data[dataIndex + 1] << (16 - iIndex))) & 0x07ff;
				}
				channelNum = 16;
				if((sbusData.data[11] & 0x000f) == 0x0c)
				{
				  this->teleLinkFlag = false;
				}
				else
				{
					this->teleLinkFlag = true;
				}
			}
    }
		lastReadIndex = nowIndex;
		return channelNum;
	}
	return 0;
}

bool SbusCtrl::IsLinked(void)
{
	return this->teleLinkFlag;
}

uint16_t SbusCtrl::GetChannelData(int channel)
{
	if(channel > channelNum)
		return 0;
	return this->channelData[channel];
}

