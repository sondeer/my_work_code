#include "MS5611.h"
#include <string.h>


#define SetMs5611High()  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_SET)
#define SetMs5611Low()   HAL_GPIO_WritePin(GPIOD,GPIO_PIN_8,GPIO_PIN_RESET)


// Command byte define
#define CMD_RESET         0x1E
#define CMD_CD1_256       0x40
#define CMD_CD1_512       0x42
#define CMD_CD1_1024      0x44
#define CMD_CD1_2048      0x46
#define CMD_CD1_4096      0x48
#define CMD_CD2_256       0x50
#define CMD_CD2_512       0x52
#define CMD_CD2_1024      0x54
#define CMD_CD2_2048      0x56
#define CMD_CD2_4096      0x58
#define CMD_ADC_READ      0x00
#define CMD_PROM_READ     0xA0
#define PROM_SIZE         8

#define CMD_CD1           0x40
#define CMD_CD2           0x50

void Ms5611Ctrl::Init(SpiCtrl *pSpi)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__GPIOD_CLK_ENABLE();

	SetMs5611High();
	GPIO_InitStruct.Pin   = GPIO_PIN_8;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct); 	
  
	this->pSpi = pSpi;
	this->convertionPrecision = 8;
	this->Reset();
}

void Ms5611Ctrl::DeInit(void)
{
}

void Ms5611Ctrl::Reset(void)
{
  uint32_t i;
    
  this->WriteCmd(CMD_RESET);
      	
  HAL_Delay(20);
	
	uint16_t*p = &this->calibRef.fData;
  for (i=0; i<PROM_SIZE; i++)
  { 
    this->ReadRegister(CMD_PROM_READ + (i*2), p ++, 2);
  }
	this->ConvertionTemperature();
	HAL_Delay(10);
	this->GetTemperature();
	this->ConvertionPressure();
	HAL_Delay(10);
}

void Ms5611Ctrl::ReadRegister(uint8_t cmd,void *data,int size)
{
	uint8_t  txBuffer[8] = {0,0,0,0,0,0,0,0};
	uint8_t  rxBuffer[8];
	txBuffer[0] = cmd;
	SetMs5611Low();
	this->pSpi->TransmitReceive(txBuffer, rxBuffer, size + 1,1);
	SetMs5611High();
	
	uint8_t *p = (uint8_t *)data;
	
	for(int i = 0 ; i < size ; i ++)
	{
		p[i] = rxBuffer[size - i];
	}
}

void Ms5611Ctrl::WriteCmd(uint8_t cmd)
{
	uint8_t  rxBuffer[8];
	SetMs5611Low();
	this->pSpi->TransmitReceive(&cmd,rxBuffer,1,1);
	SetMs5611High();
}

void Ms5611Ctrl::SetPrecision(int precision)
{
	switch(precision)
	{
		case 256:
			this->convertionPrecision = 0;
			break;
		case 512:
			this->convertionPrecision = 2;
			break;
		case 1024:
			this->convertionPrecision = 4;
			break;
		case 2048:
			this->convertionPrecision = 6;
			break;
		default:
			this->convertionPrecision = 8;
			break;
	}
}

void Ms5611Ctrl::ConvertionTemperature(void)
{
	this->WriteCmd(CMD_CD2 | this->convertionPrecision);
}

void Ms5611Ctrl::ConvertionPressure(void)
{
	this->WriteCmd(CMD_CD1 | this->convertionPrecision);
}
	uint32_t D2;
int  Ms5611Ctrl::GetTemperature(void)
{
	//uint32_t D2;
	this->ReadRegister(CMD_ADC_READ, &D2, 3);
    
  this->dT = D2 - ((int)calibRef.tRef << 8);
  this->temperature = 2000 + ((this->dT * calibRef.tSens) >> 23); 
	return  this->temperature;
}

	 uint32_t D1,tmp,off2,sens2,t2;
	 int64_t  off, sens; 

int  Ms5611Ctrl::GetPressure(void)
{
//	 uint32_t D1,tmp,off2,sens2,t2;
//	 int64_t  off, sens;

	 this->ReadRegister(CMD_ADC_READ, &D1, 3);
    
   off  = ((int64_t)this->calibRef.pOff << 16) + ((this->calibRef.tCO * this->dT) >> 7);
   sens = ((int64_t)this->calibRef.pSens << 15) + ((this->calibRef.tCS * this->dT) >> 8);
	
   if (this->temperature < 2000)
   {
      t2    = ((int64_t)dT * dT) >> 31;
      tmp   = (this->temperature - 2000);
      tmp  *= tmp;
      off2  = (5 * tmp)>>1;
      sens2 = (5 * tmp)>>2;
      
      if (this->temperature< -1500)
      {
        tmp   = (this->temperature + 1500);
        tmp  *= tmp;
        off2  = off2 + (7 * tmp);
        sens2 = sens2 + ((11 * tmp)>>1);
      }
   }
   else
   {
      t2    = 0;
      off2  = 0;
      sens2 = 0;
   }
    
   this->temperature -= t2;
   off  -= off2;
   sens -= sens2; 
    
   this->pressure = (((D1 * sens)>>21) - off)>>15;     
   return this->pressure;		
}

float Ms5611Ctrl::GetPressureKF(float press,float dt)
{
	static float X[2] = {101000,0};
  static float P[4] = {1,1,1,1};
	const float Q[2] = {0.01f,0.01f};
	const float R = 10;
	float A[4] = {1,0,0,1};
  float K[2] ;
	float temp[4],temp1[4] = {1,0,0,1} ;
  float sum ;
	
	A[1] = dt;
	
	for( int i = 0 ; i < 2 ; i ++)
	{
		for(int j = 0 ; j < 2 ; j ++)
		{
			sum = 0;
			for( int k = 0 ; k < 2 ; k ++)
			{
				sum += A[ i * 2 + k ] * P[k * 2 + j];
			}
			temp[i * 2 + j] = sum;
		}
	}
	
	for( int i = 0 ; i < 2 ; i ++)
	{
		for(int j = 0 ; j < 2 ; j ++)
		{
			sum = 0;
			for( int k = 0 ; k < 2 ; k ++)
			{
				sum += temp[ i * 2 + k ] * A[j * 2 + k];
			}
			P[i * 2 + j] = sum;
		}
	}
	
	for ( int i = 0 ; i < 2 ; i ++)
	{
		P[i * 2 + i] += Q[i];
	}

	X[0] += X[1] * dt;

	float disPress = press - X[0];
	
	for( int i = 0 ; i < 2 ; i ++)
	{
		K[i] = P[i] / ( P[0] + R);
		X[i] += K[i] * disPress;
		temp1[i * 2] -= K[i];
	}
	
	for( int i = 0 ; i < 2 ; i ++)
	{
		for(int j = 0 ; j < 2 ; j ++)
		{
			sum = 0;
			for( int k = 0 ; k < 2 ; k ++)
			{
				sum += temp1[ i * 2 + k ] * P[k * 2 + j];
			}
			temp[i * 2 + j] = sum;
		}
	}
	
	for(int i = 0 ; i < 4 ; i ++)
	    P[i] = temp[i];
	
	return X[0];
}

