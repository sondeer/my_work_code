
#ifndef __MOTOR_H
#define __MOTOR_H

#ifdef __cplusplus
 extern "C" {
#endif

	 
#include "stm32f4xx_hal.h"
	 
#define FREQ_PWM        400
#define MAX_COUNT       60000
#define DEF_PWM_COUNT   ( FREQ_PWM * MAX_COUNT / 1000)

class MotorCtrl
{	
	public:
		void Init(void);
    void SetSpeed(int index,float speed);
    void StopAllMotor(void);
	private:
		TIM_HandleTypeDef  TimHandle;
};
	

#ifdef __cplusplus
}
#endif

#endif 

