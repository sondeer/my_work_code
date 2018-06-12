
#include "Motor.h"
#include "math.h"

void MotorCtrl::Init(void)
{
	TIM_OC_InitTypeDef sConfig;	
	GPIO_InitTypeDef   GPIO_InitStruct;

	__HAL_RCC_TIM1_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
	
  GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	
  TimHandle.Instance = TIM1;
  TimHandle.Init.Prescaler = SystemCoreClock / MAX_COUNT / FREQ_PWM - 1;    
  TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
  TimHandle.Init.Period = MAX_COUNT;
  TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_PWM_Init(&TimHandle);

  sConfig.OCMode = TIM_OCMODE_PWM1;
  sConfig.Pulse =  0;//DEF_PWM_COUNT;
  sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode = TIM_OCFAST_DISABLE;
	
  HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1);
  HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_2);
  HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_3);
  HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_4);
	
	HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_4);
}

void MotorCtrl::SetSpeed(int index,float speed)
{
	if(index >= 4 || index < 0)
		return ;
	
	int speedI = DEF_PWM_COUNT + speed * DEF_PWM_COUNT;
	if(speedI > DEF_PWM_COUNT * 2)
		speedI = DEF_PWM_COUNT * 2;
	else if(speedI < DEF_PWM_COUNT)
		speedI = DEF_PWM_COUNT;
	*(&TimHandle.Instance->CCR1 + index)= speedI;
}

void MotorCtrl::StopAllMotor(void)
{
	TimHandle.Instance->CCR1 = 0;
	TimHandle.Instance->CCR2 = 0;
	TimHandle.Instance->CCR3 = 0;
	TimHandle.Instance->CCR4 = 0;
}
