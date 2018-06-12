


#ifndef __MS5611_H
#define __MS5611_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "stm32f4xx_hal.h"
#include "SPI.h"
	 

/* Exported types ------------------------------------------------------------*/
typedef struct {

  float  fData;                  // Factory data, not use
  float  pSens;                  // Pressure sensitivity 
  float  pOff;                   // Pressure offset
  float  tCS;                    // Temperature coefficient of pressure sensitivity
  float  tCO;                    // Temperature coefficient of pressure offset
  float  tRef;                   // Reference temperature
  float  tSens;                  // Temperature coefficient of the temperature
  float  crc;                    // Serial code and CRC, not use
  
} Calibration_Data_t;               // Calibration data struct declaration
	 
class Ms5611Ctrl
{
	public:
	  void Init(SpiCtrl *pSpi);
	  void DeInit(void);
	  void SetPrecision(int precision);
	  void ConvertionTemperature(void);
	  void ConvertionPressure(void);
	  float GetTemperature(void);
	  float GetPressure(void);
	  float GetPressureKF(float press,float dt);
	  void  SetPressureBase(void);
	  float GetHeight(void);
	protected:
    void Reset(void);
	  void ReadRegister(uint8_t cmd,void *data,int size);
	  void WriteCmd(uint8_t cmd);
	private:
		SpiCtrl *pSpi;
	  Calibration_Data_t  calibRef; 
	  uint8_t convertionPrecision;
	  float temperature;
	  float dT;
	  float pressure;
	  float pressureKF;
	  float pressureBase;
};


#ifdef __cplusplus
}
#endif

#endif 
