

#ifndef __CALCULATIONPID_H
#define __CALCULATIONPID_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
	 
typedef struct 
{
	float errorSum; 
	float errorOld; 
	float minSum;
	float maxSum;
	float Kp; 
	float Ki;
	float Kd;
	float Kfilter;
	float oldValue;
}PID_Struct;	 

#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

class PidCtrl
{
	public:
    float CalculationPid(float error,float dt = 1);
		PID_Struct pid;	
};


#ifdef __cplusplus
}
#endif

#endif 



