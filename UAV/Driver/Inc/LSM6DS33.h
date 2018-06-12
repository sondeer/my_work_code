


#ifndef __LSM6DS33_H
#define __LSM6DS33_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "stm32f4xx_hal.h"
#include "SPI.h"

typedef struct 
{
	int16_t temperature;
	int16_t g[3];
	int16_t a[3];
}LSM6DS33_DATAStruct;

typedef struct 
{
	float temperature;
	float g[3];
	float a[3];
}LSM6DS33_FDATAStruct;	 

class Lsm6ds33Ctrl
{
	public:
	  void Init(SpiCtrl *pSpi);
	  void DeInit(void);
	  int  MPU_Get_AllData(LSM6DS33_DATAStruct *data);
	  int  MPU_GetAngularAcceleration(LSM6DS33_FDATAStruct *data);
	  void GyroCalibration(int num);
	  void DynamicGyroCalibration(float value[3],float rate);
	protected:
		void InitLsm6ds33(void);
	  uint8_t ReadRegister(uint8_t addr);
	  void ReadRegister(uint8_t addr,void *data,int size);
	  void WriteRegister(uint8_t addr,void *data,int size);
	  void WriteRegister(uint8_t addr,uint8_t data);
	private:
		SpiCtrl *pSpi;
    float  adToRadian;
	  float  adToAcceleration;
	  float  gOffset[3];
};


#ifdef __cplusplus
}
#endif

#endif 
