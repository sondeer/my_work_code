
#include "led.h"

LedCtrl::LedCtrl()
{
	flashTimeOut = 0;
}
void LedCtrl::Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct; 
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin       = GPIO_PIN_15;
  GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void LedCtrl::SetLedState(int state)
{
	flashTimeOut = 0;
	if(state == 0)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET);
	}
	else 
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET);
	}
}

void LedCtrl::Update(void)
{
	if(flashTimeOut == 0)
		return;
	
	if(HAL_GetTick() - lastFlashTime >= flashTimeOut)
	{
		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_15);
		lastFlashTime = HAL_GetTick();
	}
}

void LedCtrl::SetFlashTime(int mTime)
{
	flashTimeOut = mTime / 2;
	lastFlashTime = HAL_GetTick();
}
