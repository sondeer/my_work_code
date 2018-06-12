
#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f0xx_hal.h"

#define LEFT_MOTOR      0
#define RIGHT_MOTOR     1

void MotorUpdate(void);
void MotorInit(void);
void SetOneMotorPwm(int id,int pwm);
void SetMotorPwm(int pwm1,int pwm2);
void SetOneMotorSpeed(int id,float speed);
void SetMotorSpeed(float speed1,float speed2);
int  GetOneMotorPwm(int id);
void GetMotorPwm(int *pwm1,int *pwm2);
float GetOneMotorSpeed(int id);
void GetMotorSpeed(float *speed1,float *speed2);
float GetCurAverageSpeed(void);
void SetSpeedPid(float p,float i,float d);

#endif
