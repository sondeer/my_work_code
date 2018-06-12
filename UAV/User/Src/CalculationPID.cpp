
#include "CalculationPID.h"

float PidCtrl::CalculationPid(float error,float dt)
{
	float out;
	
	pid.errorSum += pid.Ki * error * dt;
	
	pid.errorSum = constrain(pid.errorSum,pid.minSum,pid.maxSum);
	pid.oldValue += pid.Kfilter * (pid.Kd * (error - pid.errorOld) - pid.oldValue) / dt;
	
  out = pid.Kp * error  + pid.errorSum + pid.oldValue;
  
  pid.errorOld = error;

  return out;
}

