
#include "uart.h"
#include <string.h>

static uint8_t rx_dataBuff[BUFF_SIZE];
static uint8_t tx_dataBuff[BUFF_SIZE];
static int  tx_headIndex;
static int  tx_tailIndex;
static int  rx_tailIndex;

void UartInit(int baud)
{
	UART_HandleTypeDef UartHandle;
	GPIO_InitTypeDef  GPIO_InitStruct;
  DMA_HandleTypeDef hdma_tx;
  DMA_HandleTypeDef hdma_rx;
	
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();
	
	tx_headIndex = tx_tailIndex = rx_tailIndex = 0;
	
	GPIO_InitStruct.Pin       = GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_USART2;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	UartHandle.Instance        = USART2;
  UartHandle.Init.BaudRate   = baud;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits   = UART_STOPBITS_1;
  UartHandle.Init.Parity     = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode       = UART_MODE_TX_RX;
	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	
	HAL_UART_Init(&UartHandle) ;
	
	hdma_rx.Instance                 = DMA1_Channel5;
  hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_rx.Init.Mode                = DMA_CIRCULAR;
  hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
	hdma_rx.XferHalfCpltCallback = NULL;

  HAL_DMA_Init(&hdma_rx);

  
	hdma_tx.Instance                 = DMA1_Channel4;
  hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  hdma_tx.Init.Mode                = DMA_NORMAL;
  hdma_tx.Init.Priority            = DMA_PRIORITY_LOW;
	hdma_tx.XferHalfCpltCallback = NULL;

  HAL_DMA_Init(&hdma_tx);
	
	HAL_NVIC_SetPriority(DMA1_Channel4_5_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);
	
	HAL_DMA_Start(&hdma_rx,(uint32_t)&USART2->RDR,(uint32_t)rx_dataBuff,BUFF_SIZE);
	SET_BIT(USART2->CR3, USART_CR3_DMAR);
	

	//HAL_DMA_Start_IT(&hdma_tx,(uint32_t)tx_dataBuff,(uint32_t)&USART1->TDR,BUFF_SIZE);
}

int UartDataLength(void)
{
	int rx_headIndex = BUFF_SIZE - DMA1_Channel5->CNDTR;
	int leftData = rx_headIndex - rx_tailIndex;
	if(leftData < 0)
		leftData += BUFF_SIZE;
	
	return leftData;
}

int UartReadData(void* buff,int size)
{
	int rx_headIndex = BUFF_SIZE - DMA1_Channel5->CNDTR;
	int leftData = rx_headIndex - rx_tailIndex;
	if(leftData < 0)
		leftData += BUFF_SIZE;
	else if(leftData == 0)
		return 0;
	
	if(leftData < size)
		size = leftData;
	
	
	if(rx_headIndex > rx_tailIndex)
	{
		memcpy(buff,rx_dataBuff + rx_tailIndex,size);
	}
	else
	{
		int leftSize = BUFF_SIZE -  rx_tailIndex;
		memcpy(buff,rx_dataBuff + rx_tailIndex,leftSize);
		memcpy((uint8_t*)buff + leftSize,rx_dataBuff,size - leftSize);
	}
	
	rx_tailIndex += size;
	
	if(rx_tailIndex >= BUFF_SIZE)
		rx_tailIndex -= BUFF_SIZE;
	
	return size;
}

int UartWriteData(void* buff,int size)
{
	if(DMA1_Channel4->CCR & DMA_CCR_EN)
	{
		int rvtx_tailIndex = tx_tailIndex - DMA1_Channel4->CNDTR;
		
		if(rvtx_tailIndex < 0 )
			rvtx_tailIndex += BUFF_SIZE;
		
		int leftfreeData = rvtx_tailIndex - tx_headIndex;
		
		if(leftfreeData < 0)
			leftfreeData += BUFF_SIZE;
		
		if(size > leftfreeData)
			size = leftfreeData;
		
		if(rvtx_tailIndex > tx_headIndex)
		{
			memcpy(tx_dataBuff + tx_headIndex,buff,size);
			tx_headIndex += size;
		}
		else
		{
			int leftSize = BUFF_SIZE -  tx_headIndex;
			if(leftSize > size)
			{
				memcpy(tx_dataBuff + tx_headIndex,buff,size);
			}
			else
			{
			  memcpy(tx_dataBuff + tx_headIndex,buff,leftSize);
			  memcpy(tx_dataBuff,(uint8_t*)buff + leftSize,size - leftSize);
			}
			tx_headIndex += size;
		}
	}
	else 
	{
		tx_tailIndex = tx_headIndex = 0;
		if(size > BUFF_SIZE)
			size = BUFF_SIZE;
		memcpy(tx_dataBuff,buff,size);
		UartBeginTransmitDMA(tx_dataBuff,size);
		SET_BIT(USART2->CR3, USART_CR3_DMAT);
		tx_tailIndex += size;
		tx_headIndex += size;
	}
	
	if(tx_tailIndex > BUFF_SIZE)
		tx_tailIndex -= BUFF_SIZE;
	if(tx_headIndex > BUFF_SIZE)
		tx_tailIndex -= BUFF_SIZE;
	
	return 0;
}

void UartInttruptCallBack(void)
{
	DMA1->IFCR = DMA_FLAG_GL4 | DMA_FLAG_TC4 | DMA_FLAG_HT4 | DMA_FLAG_TE4 ;
	
	if(tx_tailIndex == tx_headIndex)
	{
		DMA1_Channel4->CCR &= ~DMA_CCR_EN;
		CLEAR_BIT(USART2->CR3, USART_CR3_DMAT);
	}
	else if(tx_headIndex > tx_tailIndex)
	{
		UartBeginTransmitDMA(tx_dataBuff + tx_tailIndex,tx_headIndex - tx_tailIndex);
		SET_BIT(USART2->CR3, USART_CR3_DMAT);
		tx_tailIndex = tx_headIndex;
	}
	else 
	{
		UartBeginTransmitDMA(tx_dataBuff + tx_tailIndex,BUFF_SIZE - tx_tailIndex);
		SET_BIT(USART2->CR3, USART_CR3_DMAT);
		tx_tailIndex = 0;
	}
}

void UartBeginTransmitDMA(uint8_t *buff,int size)
{
	DMA1_Channel4->CCR &= ~DMA_CCR_EN;
	DMA1_Channel4->CNDTR = size;
	DMA1_Channel4->CMAR = (uint32_t)buff;
	DMA1_Channel4->CPAR = (uint32_t)&USART2->TDR;
	DMA1_Channel4->CCR |= (DMA_IT_TC | DMA_IT_TE);
	DMA1_Channel4->CCR |= DMA_CCR_EN;
	SET_BIT(USART2->CR3, USART_CR3_DMAT);
}
