
#ifndef __ATTITUDECONTROL_H
#define __ATTITUDECONTROL_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "CalculationPID.h"

class AttitudeControl
{
	public:
		void Init(void);
    void RunAttitudeControlV(float throttle,float inAngle[],float targetAngle[],float inAngleSpeed[],float outSpeed[]);
	protected:
		float WrapAnglePI(float in);
	private:
		PidCtrl pidAngle[3];
    PidCtrl pidAngleV[3];
};

#ifdef __cplusplus
}
#endif

#endif 



