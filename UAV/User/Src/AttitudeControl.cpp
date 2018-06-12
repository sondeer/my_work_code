
#include "AttitudeCalculationEKF.h"
#include "AttitudeControl.h"
#include <math.h>

extern AttitudeCalculationEKF attitudeEkf;

#define PIDMIX(array,k1, k2, k3) (array[0] * k1 + array[1] * k2 + array[2] * k3)
#define WRAP_ANGLE_PI(a)  (a )

void AttitudeControl::Init(void)
{
	for( int i = 0 ; i < 3 ; i ++)
	{
		pidAngle[i].pid.errorOld = 0;
		pidAngle[i].pid.errorSum = 0;
		
		pidAngle[i].pid.Kp = 8.00f;
	  pidAngle[i].pid.Ki = 0.00f;
		pidAngle[i].pid.minSum = -0.1f;
		pidAngle[i].pid.maxSum = 0.1f;
	  pidAngle[i].pid.Kd = 40.0f;
	  pidAngle[i].pid.Kfilter = 0.4f;
		pidAngle[i].pid.oldValue = 0.0f;
		
		pidAngleV[i].pid.errorOld = 0;
		pidAngleV[i].pid.errorSum = 0;
		
		pidAngleV[i].pid.Kp = 0.02f;
	  pidAngleV[i].pid.Ki = 0.0004f;
		pidAngleV[i].pid.minSum = -0.2f;
		pidAngleV[i].pid.maxSum = 0.2f;
	  pidAngleV[i].pid.Kd = 1.0f;
		pidAngleV[i].pid.Kfilter = 0.4f;
		pidAngleV[i].pid.oldValue = 0.0f;
	}
	pidAngle[2].pid.Kp = 8.0f;
	pidAngle[2].pid.Ki = 0.0f;
  pidAngle[2].pid.Kd = 40.0f;
	
	pidAngleV[2].pid.Kp = 0.2f;
	pidAngleV[2].pid.Ki = 0.0f;
  pidAngleV[2].pid.Kd = 5.0f;
}


extern float sendFloat[16];

void AttitudeControl::RunAttitudeControlV(float throttle,float inAngle[],float targetAngle[],float inAngleSpeed[],float outSpeed[])
{
	int i = 0;
	float targetAngleSpeed;
	float pidOut[3];
	float errorEf[3],errorBf[3];
	float throttleCompensation;
	
	errorEf[0] = WrapAnglePI(targetAngle[0] - inAngle[0]);
	errorEf[1] = WrapAnglePI(targetAngle[1] - inAngle[1]);
	errorEf[2] = WrapAnglePI(targetAngle[2] - inAngle[2]);
	
	attitudeEkf.ConversionEfToBf(errorEf,errorBf);
	
	for( i = 0 ; i < 3 ; i ++)
	{
		targetAngleSpeed = pidAngle[i].CalculationPid(errorBf[i]);
		sendFloat[i + 3] = targetAngleSpeed;
		pidOut[i] = pidAngleV[i].CalculationPid(targetAngleSpeed - inAngleSpeed[i]);
		
		sendFloat[i] = pidOut[i];
	}
	throttleCompensation = throttle * (1 + attitudeEkf.GetThrottleCompensation() / 10);
	outSpeed[0] =  throttleCompensation *(1 + constrain(PIDMIX(pidOut,1,-1, -1),-0.8f,0.8f));//( - pidOut[0] + pidOut[1] - pidOut[2]);
	outSpeed[1] =  throttleCompensation *(1 + constrain(PIDMIX(pidOut,-1,-1,1),-0.8f,0.8f));//( pidOut[0] + pidOut[1] + pidOut[2]);
	outSpeed[2] =  throttleCompensation *(1 + constrain(PIDMIX(pidOut,-1,1,-1),-0.8f,0.8f));//( pidOut[0] - pidOut[1] - pidOut[2]);
	outSpeed[3] =  throttleCompensation *(1 + constrain(PIDMIX(pidOut,1,1,1),-0.8f,0.8f));//( - pidOut[0] - pidOut[1] + pidOut[2]);
	
	//outSpeed[0] =  constrain(throttleCompensation + PIDMIX(pidOut,1,-1, -1),0.1f,throttleCompensation * 2);//( - pidOut[0] + pidOut[1] - pidOut[2]);
	//outSpeed[1] =  constrain(throttleCompensation + PIDMIX(pidOut,-1,-1,1),0.1f,throttleCompensation * 2);//( pidOut[0] + pidOut[1] + pidOut[2]);
	//outSpeed[2] =  constrain(throttleCompensation + PIDMIX(pidOut,-1,1,-1),0.1f,throttleCompensation * 2);//( pidOut[0] - pidOut[1] - pidOut[2]);
	//outSpeed[3] =  constrain(throttleCompensation + PIDMIX(pidOut,1,1,1),0.1f,throttleCompensation * 2);//( - pidOut[0] - pidOut[1] + pidOut[2]);
	
	
/*
	sum = 0;
  for( i = 0 ; i < 4 ; i ++)
	{
		outSpeed[i] = constrain(outSpeed[i],throttle * 1 / 4 , 7 * throttle / 4);
		sum += outSpeed[i];
	}
	
	if(sum != 0 && sum > (4 * throttle))
	{
	  for( i = 0 ; i < 4 ; i ++)
	  {
		 	outSpeed[i] = 4 * throttle * outSpeed[i] / sum;
			sendFloat[i + 4] = outSpeed[i];
	  }
	}*/
}

float AttitudeControl::WrapAnglePI(float in)
{
	if(in > PI)
	{
		return in - 2 * PI;
	}
	else if(in < - PI)
	{
		return in + 2 * PI;
	}
	else
		return in;
}


