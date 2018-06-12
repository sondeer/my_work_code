
#include "main.h"
#include "led.h"
#include "SPI.h"
#include "HMC5983.h"
#include "MS5611.h"
#include "LSM6DS33.h"
#include "Motor.h"
#include "usb_device.h"
#include "AttitudeCalculationEKF.h"
#include "AttitudeControl.h"
#include "power.h"
#include "RcCtrl.h"
#include "cmsis_os.h"
#include "ExIMU.h"


extern void Timer2ms_Callback(void const *arg);
osTimerDef (Timer2ms, Timer2ms_Callback);   

extern void Timer10ms_Callback(void const *arg);
osTimerDef (Timer10ms, Timer10ms_Callback);     

void RcThread (void const *arg);                          
osThreadDef (RcThread, osPriorityNormal, 1, 256);  

void LedThread (void const *arg);                          
osThreadDef (LedThread, osPriorityNormal, 1, 256);  

SysStatusEnum sysStatus;
bool       powerLowFlag;

extern uint32_t os_time;

uint32_t HAL_GetTick(void) { 
  return os_time; 
}

void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

}

SpiCtrl spiCtrl2(SpiID2);
SpiCtrl spiCtrl4(SpiID4);
Hmc5983Ctrl hmc5983; 
Ms5611Ctrl  ms5611;
Lsm6ds33Ctrl lsm6ds33;
MotorCtrl    motor;
PowerCtrl    power;
AttitudeCalculationEKF attitudeEkf;
AttitudeControl attitudeCtrl;
RcCtrl  rcCtrl;
ExImuCtrl  exImu;

int readMcTime = 0;
int pidTime = 0;
float  tempTest;
float  pressTest; 

float throttleTest,throttleTestOld,rcRollTest,rcPitchTest,rcYawTest;
float outAngle[6];
float testTagetAngle[3] = {0,0,0};
float sendFloat[16];
float outSpeed[4];
float oldAngle[3];
float aOutTest[3];
float powerTest,power5Vtest;
float vibrationFrequencyTest;

int main(void)
{
	float mcTest[3];
	LSM6DS33_FDATAStruct lsm6ds33Data;
	uint32_t statusTime;
	uint32_t subStatusTime[3] = {0,0,0};
	
	sysStatus = PowerOn;
	
	HAL_Init();
	SystemClock_Config();
  led.LedInit();
	motor.Init();
	spiCtrl2.Init(1000000);
	//spiCtrl4.Init(1000000);
  hmc5983.Init(&spiCtrl2);
	ms5611.Init(&spiCtrl2);
	lsm6ds33.Init(&spiCtrl2);
	power.Init();
	rcCtrl.Init();

	//exImu.Init(&spiCtrl4);
	attitudeCtrl.Init();
	
	MX_USB_DEVICE_Init();
	
	
	hmc5983.ReadMagneticFloat(mcTest);
  lsm6ds33.MPU_GetAngularAcceleration(&lsm6ds33Data);
  attitudeEkf.Init(lsm6ds33Data.a,lsm6ds33Data.g,mcTest);
	
	osKernelInitialize();
	
	osTimerId timer = osTimerCreate(osTimer(Timer2ms), osTimerPeriodic, NULL);
	osTimerStart (timer, 2);  
	
	timer = osTimerCreate(osTimer(Timer10ms), osTimerPeriodic, NULL);
	osTimerStart (timer, 10);  
	
  osThreadCreate (osThread (RcThread), NULL);         
	osThreadCreate (osThread (LedThread), NULL);  
	
	osKernelStart();
	
	sysStatus = Inited;
	statusTime = 0;
	while(1)
	{
		powerTest = power.GetMainVoltage();
		power5Vtest = power.Get5VVoltage();

		if(powerTest < 10.6f)
		{
			powerLowFlag = true;
		}
		else if(powerTest > 11.0f)
		{
			powerLowFlag = false;
		}
		
	  switch(sysStatus)
		{
			case PowerOff:
				break;
			case PowerOn:
				break;
			case Inited:
			  while(!rcCtrl.IsLinked());
	      {
          osDelay(10);
	      }
				
				if(rcCtrl.GetThrottle() > 0.9f)
	      {
	        sysStatus = CalibrationRc;
					statusTime = 0;
	      }
				else
				{
					sysStatus = Locked;
					statusTime = 0;
				}
				
				break;
			case CalibrationRc:
				if(rcCtrl.GetThrottle() < 0.1f)
	      {
	        sysStatus = Locked;
					statusTime = 0;
	      }
				break;
			case Unlocked:
		    if(rcCtrl.GetThrottle() < 0.00001f)	
			  {
					subStatusTime[0] ++;
					if(subStatusTime[0] > 1000)
					{
						sysStatus = Locked;
						statusTime = 0;
						subStatusTime[0] = 0;
					}
				}
				else
				{
					subStatusTime[0] = 0;
				}
				
				if(rcCtrl.GetThrottle() < 0.1f && rcCtrl.GetTargetPitch() > 0.60f && rcCtrl.GetTargetRoll() < -0.60f && rcCtrl.GetTargetYaw() > 0.60f)
				{
					subStatusTime[1] ++;
					if(subStatusTime[1] > 200)
					{
						sysStatus = Locked;
						statusTime = 0;
						subStatusTime[1] = 0;
					}
				}
				else
				{
					subStatusTime[1] = 0;
				}
				break;
			case Locked:
				if(rcCtrl.GetThrottle() < 0.1f && rcCtrl.GetTargetPitch() > 0.60f && rcCtrl.GetTargetRoll() > 0.60f && rcCtrl.GetTargetYaw() < -0.60f && !powerLowFlag)
				{
					subStatusTime[0] ++;
					if(subStatusTime[0] > 200)
					{
						sysStatus = Unlocked;
						statusTime = 0;
						subStatusTime[0] = 0;
					}
				}
				else
				{
					subStatusTime[0] = 0;
				}
				break;
			case PowerLow:
				break;
			default:
				break;
		}
		statusTime ++;
		osDelay(10);
	}
	return 0;
}
/*
void testFunc(float a[3])
{
	float temp;
	temp = a[0];
	a[0] = a[1];
	a[1] = a[2];
	a[2] = temp;
}
*/
LSM6DS33_FDATAStruct lsm6ds33Data;
void Timer2ms_Callback(void const *arg)
{
	static int readMcTime = 10;
	static float mcTest[3];
	

	
	if(readMcTime ++ >= 10)
  {
		hmc5983.ReadMagneticFloat(mcTest);
		readMcTime = 0;
	}
	
	if(readMcTime == 0)
	{
		pressTest = ms5611.GetPressureKF(ms5611.GetPressure(),0.02f);
		ms5611.ConvertionTemperature();
	}
	else if(readMcTime == 5)
	{
		tempTest = ms5611.GetTemperature();
		ms5611.ConvertionPressure();
	}
	
	lsm6ds33.MPU_GetAngularAcceleration(&lsm6ds33Data);
	attitudeEkf.CalcAttitudeAGM_EKF(lsm6ds33Data.a,lsm6ds33Data.g,mcTest,0.002f);
	attitudeEkf.GetAccelerationEf(aOutTest);
	vibrationFrequencyTest = attitudeEkf.GetVibrationFrequency();
	lsm6ds33.DynamicGyroCalibration(attitudeEkf.X_copy,0.000001f / (1 + vibrationFrequencyTest));
}

void Timer10ms_Callback(void const *arg)
{
	 static float olfYawTagetAngle = 0;
	
   //exImu.ReadExImu(outAngle,6);
	
	 if(!rcCtrl.IsLinked())
	 {
		 motor.StopAllMotor();
		 return;
	 }
	 
	 switch(sysStatus)
	 {
			case CalibrationRc:
				motor.SetSpeed(0,1);
		    motor.SetSpeed(1,1);
		    motor.SetSpeed(2,1);
		    motor.SetSpeed(3,1);
				break;
			case Unlocked:
				if(rcCtrl.IsLinked())
	      {
		      attitudeEkf.GetOutAngle(outAngle);
					//exImu.ReadExImu(outAngle,6);
		      throttleTest += 0.2f * (rcCtrl.GetThrottle() * 0.7f + 0.1f - throttleTest);
					//throttleTest = rcCtrl.GetThrottle();
		      testTagetAngle[0] = rcCtrl.GetTargetRoll();
		      testTagetAngle[1] = rcCtrl.GetTargetPitch();
					float temp = rcCtrl.GetTargetYaw();
					if(temp > 0.01f)
					{
						testTagetAngle[2] += (temp - 0.01f) / 40;
					}
					else if(temp < -0.01f)
					{
						testTagetAngle[2] += (temp + 0.01f) / 40;
					}
					else if(olfYawTagetAngle > 0.01f || olfYawTagetAngle < -0.01f)
					{
						testTagetAngle[2] = outAngle[2];
					}
  	      olfYawTagetAngle = temp;
		
		      if(throttleTest <= 0.10001f)
		      {
		      	testTagetAngle[2] = outAngle[2];
						outSpeed[0] = throttleTest;
						outSpeed[1] = throttleTest;
						outSpeed[2] = throttleTest;
						outSpeed[3] = throttleTest;
		      }
		      else
					{
		        attitudeCtrl.RunAttitudeControlV(throttleTest,outAngle,testTagetAngle,outAngle + 3,outSpeed);
					}
		
					memcpy(sendFloat + 9,outAngle,6 * 4);
					
					UsbSendData(sendFloat,64);
					
		      motor.SetSpeed(0,outSpeed[0]);
		      motor.SetSpeed(1,outSpeed[1]);
		      motor.SetSpeed(2,outSpeed[2]);
		      motor.SetSpeed(3,outSpeed[3]);
	      }
				break;
			case Locked:
				motor.SetSpeed(0,0);
		    motor.SetSpeed(1,0);
		    motor.SetSpeed(2,0);
		    motor.SetSpeed(3,0);
				break;
			case PowerLow:
				break;
			default:
				break;
		}
}


void RcThread (void const *arg)
{
	while(1)
	{
		rcCtrl.Run2ms(HAL_GetTick());
	  osDelay(2);
	}
}  

void LedThread(void const *arg)
{
	while(1)
	{
		if(powerLowFlag)
		{
			led.ToggleRedLed();
		  led.SetBlueLed(LED_OFF);
			led.SetGreenLed(LED_OFF);
		}
		else
		{
	    switch(sysStatus)
		  {
			  case PowerOff:
			  	break;
			  case PowerOn:
				  break;
			  case Inited:
				  led.SetBlueLed(LED_ON);
				  led.SetRedLed(LED_OFF);
			    led.SetGreenLed(LED_OFF);
				  break;
			  case CalibrationRc:
				  led.ToggleBlueLed();
				  led.SetRedLed(LED_OFF);
			    led.ToggleGreenLed();
				  break;
			  case Unlocked:
				  led.ToggleGreenLed();
			    led.SetRedLed(LED_OFF);
			    led.SetBlueLed(LED_OFF);
				  break;
			  case Locked:
				  led.ToggleBlueLed();
			    led.SetRedLed(LED_OFF);
			    led.SetGreenLed(LED_OFF);
				  break;
			  case PowerLow:
				  led.ToggleRedLed();
			    led.SetBlueLed(LED_OFF);
			    led.SetGreenLed(LED_OFF);
				  break;
			  default:
			  	break;
		  }
	  }
		osDelay(500);
	}
}
