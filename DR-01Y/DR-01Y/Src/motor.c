
#include "motor.h"

#define MOTOR_MODE_PWM    0
#define MOTOR_MODE_SPEED  1
#define MOTOR_MODE_DIS    2

#define MOTOR_ENCODER     9
#define MOTOR_RATIO       48

struct PID_Struct
{
	float p,i,d;
	float oldError;
	float isum;
}motorPID[2];


#define MOTOR1_P()     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET)
#define MOTOR1_N()     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET)
#define MOTOR1_OFF()   HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET)

#define MOTOR2_P()     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_RESET)
#define MOTOR2_N()     HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_SET)
#define MOTOR2_OFF()   HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET);HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10,GPIO_PIN_RESET)

#define constrain(v,min,max) (v < min?min:(v>max?max:v))


static void InitVariable(void);
static void InitExit(void);
static void InitTim(void);
static void MotorSpeedUpdate(void);

static int motorCount[2];
static int motorPwm[2];
static float tagMotorSpeed[2];
static float curMotorSpeed[2];
static int motorMode;
static uint32_t lastUpdateTime;
static int lastMotorCount[2];


void InitVariable(void)
{
	motorCount[0] = motorCount[1] = 0;
	lastMotorCount[0] = lastMotorCount[1] = 0;
	motorMode = MOTOR_MODE_PWM;
	lastUpdateTime = 0;
	curMotorSpeed[0] = curMotorSpeed[1] = 0;
	tagMotorSpeed[0] = tagMotorSpeed[1] = 0;
	motorPwm[0] = motorPwm[1] = 0;
	SetSpeedPid(0.6,0.4,1);
}

void InitExit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
	
	
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void InitTim(void)
{
	TIM_HandleTypeDef htim;
	TIM_OC_InitTypeDef sConfig;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_TIM3_CLK_ENABLE();
	
	MOTOR1_OFF();
	MOTOR2_OFF();
	
	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_TIM3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	htim.Instance = TIM1;
  htim.Init.Prescaler = SystemCoreClock / (256 * 10000) - 1;
  htim.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim.Init.Period = 256;
  htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim.Init.RepetitionCounter = 0;
  htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&htim);
	
  sConfig.OCMode       = TIM_OCMODE_PWM1;
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  sConfig.Pulse = 0;
	HAL_TIM_PWM_ConfigChannel(&htim, &sConfig, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim,TIM_CHANNEL_2);
	
	
	htim.Instance = TIM3;
  HAL_TIM_Base_Init(&htim);
	
	HAL_TIM_PWM_ConfigChannel(&htim, &sConfig, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim,TIM_CHANNEL_3);
	
}

void MotorUpdate(void)
{
	int32_t disTime = HAL_GetTick() - lastUpdateTime;
	if(disTime >= 5)
	{
		lastUpdateTime = HAL_GetTick();
		curMotorSpeed[0] += ((motorCount[0] - lastMotorCount[0]) * 1000 * 60.0f / ( disTime * MOTOR_ENCODER * MOTOR_RATIO) - curMotorSpeed[0]) / 2.0f;
		curMotorSpeed[1] += ((motorCount[1] - lastMotorCount[1]) * 1000 * 60.0f / ( disTime * MOTOR_ENCODER * MOTOR_RATIO) - curMotorSpeed[1]) / 2.0f;
		lastMotorCount[0] = motorCount[0];
		lastMotorCount[1] = motorCount[1];
		if(motorMode == MOTOR_MODE_SPEED)
		{
			MotorSpeedUpdate();
		}
	}
}

void MotorInit(void)
{
	InitVariable();
	InitExit();
	InitTim();
}

void MotorSpeedUpdate(void)
{
	float error;
	float out;
	
	for(int i = 0 ; i < 2 ; i ++)
	{
		error = tagMotorSpeed[i] - curMotorSpeed[i];
		motorPID[i].isum += error * motorPID[i].i;
		motorPID[i].isum = constrain(motorPID[i].isum,-255,255);
		out = motorPID[i].p * error + motorPID[i].isum + motorPID[i].d * (error - motorPID[i].oldError);
    motorPID[i].oldError = error;
		out = constrain(out,motorPwm[i] - 50 ,motorPwm[i] + 50);
		SetOneMotorPwm(i,out);
	}
	motorMode = MOTOR_MODE_SPEED;
}

void SetOneMotorPwm(int id,int pwm)
{
	motorMode = MOTOR_MODE_PWM;
	pwm = constrain(pwm,-255,255);
	if(id == 0)
	{ 
		motorPwm[0] = pwm;
		if(pwm < 0)
		{
			pwm = -pwm;
			MOTOR1_N();		
		}
		else 
		{
			MOTOR1_P();
		}
		TIM3->CCR3 = pwm;	
	}
	else
	{
		motorPwm[1] = pwm;
		if(pwm < 0)
		{
			pwm = -pwm;
			MOTOR2_N();
		}
		else 
		{
			MOTOR2_P();
		}
		TIM1->CCR2 = pwm;
	}
}

void SetMotorPwm(int pwm1,int pwm2)
{
	SetOneMotorPwm(LEFT_MOTOR,pwm1);
	SetOneMotorPwm(RIGHT_MOTOR,pwm2);
}

void SetOneMotorSpeed(int id,float speed)
{
	motorMode = MOTOR_MODE_SPEED;
	tagMotorSpeed[id] = speed;
}

void SetMotorSpeed(float speed1,float speed2)
{
	SetOneMotorSpeed(LEFT_MOTOR,speed1);
	SetOneMotorSpeed(RIGHT_MOTOR,speed2);
}

int GetOneMotorPwm(int id)
{
	return motorPwm[id];
}

void GetMotorPwm(int *pwm1,int *pwm2)
{
	*pwm1 = motorPwm[0];
	*pwm2 = motorPwm[1];
}

float GetOneMotorSpeed(int id)
{
	return curMotorSpeed[id];
}

void GetMotorSpeed(float *speed1,float *speed2)
{
	*speed1 = curMotorSpeed[0];
	*speed2 = curMotorSpeed[1];
}

void SetSpeedPid(float p,float i,float d)
{
	motorPID[0].p = motorPID[1].p = p;
	motorPID[0].i = motorPID[1].i = i;
	motorPID[0].d = motorPID[1].d = d;
	motorPID[0].oldError = motorPID[1].oldError = 0;
	motorPID[0].isum = motorPID[1].isum = 0;
}

float GetCurAverageSpeed(void)
{
	return (curMotorSpeed[0] + curMotorSpeed[1]) / 2;
}

void EXIT8_10_CallBack(void)
{
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_7) != RESET) 
  { 
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);
		
		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_6))
		{
			motorCount[0] --;
		}
		else 
		{
			motorCount[0] ++;
		}
  }
	
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_12) != RESET) 
  { 
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11))
		{
			motorCount[1] ++;
		}
		else 
		{
			motorCount[1] --;
		}
  }
}
