
#include "Led.h"

LedCtrl led;

void LedCtrl::LedInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	LED_PORT_CLK_ENABLE();
	
	GPIO_InitStruct.Pin   = REDLED_PIN | GREENLED_PIN | BLUELED_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

	//SetRedLed(LED_ON);
	//SetGreenLed(LED_ON);
	//SetBlueLed(LED_ON);
}

void LedCtrl::SetRedLed(int state)
{
	HAL_GPIO_WritePin(LED_PORT,REDLED_PIN,(GPIO_PinState)state);
}

void LedCtrl::SetGreenLed(int state)
{
	HAL_GPIO_WritePin(LED_PORT,GREENLED_PIN,(GPIO_PinState)state);
}

void LedCtrl::SetBlueLed(int state)
{
	HAL_GPIO_WritePin(LED_PORT,BLUELED_PIN,(GPIO_PinState)state);
}

void LedCtrl::ToggleRedLed(void)
{
	HAL_GPIO_TogglePin(LED_PORT,REDLED_PIN);
}

void LedCtrl::ToggleGreenLed(void)
{
	HAL_GPIO_TogglePin(LED_PORT,GREENLED_PIN);
}

void LedCtrl::ToggleBlueLed(void)
{
	HAL_GPIO_TogglePin(LED_PORT,BLUELED_PIN);
}

