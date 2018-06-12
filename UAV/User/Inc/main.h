


#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
	 
enum SysStatusEnum
{
	PowerOff,
	PowerOn,
	Inited,
	CalibrationRc,
	Unlocked,
	Locked,
	PowerLow
};
	
//int main(void);
	 
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

