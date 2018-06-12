#include "LSM6DS33.h"
#include <string.h>
#include <math.h>

#define PI   3.141592653f

#define FUNC_CFG_ACCESS 0x01 

#define FIFO_CTRL1      0x06  
#define FIFO_CTRL2      0x07  
#define FIFO_CTRL3      0x08
#define FIFO_CTRL4      0x09 
#define FIFO_CTRL5      0x0A 
#define ORIENT_CFG_G    0x0B 

#define INT1_CTRL       0x0D 
#define INT2_CTRL       0x0E 
#define WHO_AM_I        0x0F 
#define CTRL1_XL        0x10
#define CTRL2_G         0x11
#define CTRL3_C         0x12
#define CTRL4_C         0x13
#define CTRL5_C         0x14
#define CTRL6_C         0x15
#define CTRL7_G         0x16
#define CTRL8_XL        0x17
#define CTRL9_XL        0x18
#define CTRL10_C        0x19
#define WAKE_UP_SRC     0x1B
#define TAP_SRC         0x1C
#define D6D_SRC         0x1D
#define STATUS_REG      0x1E 

#define OUT_TEMP_L      0x20
#define OUT_TEMP_H      0x21
#define OUTX_L_G        0x22
#define OUTX_H_G        0x23 
#define OUTY_L_G        0x24
#define OUTY_H_G        0x25
#define OUTZ_L_G        0x26 
#define OUTZ_H_G        0x27 
#define OUTX_L_XL       0x28
#define OUTX_H_XL       0x29 
#define OUTY_L_XL       0x2A
#define OUTY_H_XL       0x2B
#define OUTZ_L_XL       0x2C
#define OUTZ_H_XL       0x2D
#define FIFO_STATUS1    0x3A 
#define FIFO_STATUS2    0x3B 
#define FIFO_STATUS3    0x3C 
#define FIFO_STATUS4    0x3D
#define FIFO_DATA_OUT_L 0x3E 
#define FIFO_DATA_OUT_H 0x3F 
#define TIMESTAMP0_REG  0x40 
#define TIMESTAMP1_REG  0x41 
#define TIMESTAMP2_REG  0x42 
#define STEP_TIMESTAMP_L 0x49
#define STEP_TIMESTAMP_H 0x4A 
#define STEP_COUNTER_L   0x4B 
#define STEP_COUNTER_H   0x4C 

#define FUNC_SRC         0x53 
#define TAP_CFG          0x58 
#define TAP_THS_6D       0x59 
#define INT_DUR2         0x5A 
#define WAKE_UP_THS      0x5B 

#define SetLsm6ds33High()  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_9,GPIO_PIN_SET)
#define SetLsm6ds33Low()   HAL_GPIO_WritePin(GPIOD,GPIO_PIN_9,GPIO_PIN_RESET)


void Lsm6ds33Ctrl::Init(SpiCtrl *pSpi)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__GPIOD_CLK_ENABLE();

	SetLsm6ds33High();
	GPIO_InitStruct.Pin   = GPIO_PIN_9;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct); 	

  this->pSpi = pSpi;
	this->InitLsm6ds33();
	this->GyroCalibration(500);
}

void Lsm6ds33Ctrl::DeInit(void)
{
}

int  Lsm6ds33Ctrl::MPU_Get_AllData(LSM6DS33_DATAStruct *data)
{
	this->ReadRegister(OUT_TEMP_L,data,14);
	return 14;
}

int  Lsm6ds33Ctrl::MPU_GetAngularAcceleration(LSM6DS33_FDATAStruct *data)
{	
  LSM6DS33_DATAStruct intData;
	this->MPU_Get_AllData(&intData);
	
	for(int i = 0 ; i < 3 ; i ++)
	{
		data->a[i] = intData.a[i] * this->adToAcceleration;
		data->g[i] = intData.g[i] * this->adToRadian - gOffset[i];
		//gOffset[i] += data->g[i] / 1000;
	}
	return 7;
}

void Lsm6ds33Ctrl::GyroCalibration(int num)
{
	//int g[3] = {0,0,0};
	//LSM6DS33_DATAStruct intData;
	
	if(num == 0)
		return;
/*	
	for(int i = 0 ; i < num ; i ++)
	{
		HAL_Delay(2);
		this->MPU_Get_AllData(&intData);
		
		if(intData.g[0] == 0 && intData.g[1] == 0 && intData.g[2] == 0)
		{
			i --;
			continue;
		}
		
		for( int j = 0 ; j < 3 ; j ++)
		{
			g[j] += intData.g[j];
		}
	
	}
	
		for( int j = 0 ; j < 3 ; j ++)
		{
			gOffset[j] = g[j] * this->adToRadian / num;
		} */
		
		//gOffset[0] = 0.0656615f;
		//gOffset[1] = -0.14600f;
		//gOffset[2] = -0.05570f;
		
		gOffset[0] = 0.0624f;
		gOffset[1] = -0.128f;
		gOffset[2] = 0.02864f;
}

void Lsm6ds33Ctrl::DynamicGyroCalibration(float value[3],float rate)
{
	for(int i = 0 ; i < 3 ; i ++)
	{
		this->gOffset[i] += value[i] * rate;
	}
}

void Lsm6ds33Ctrl::InitLsm6ds33(void)
{
	uint8_t registerBuff[16];
	uint8_t id = this->ReadRegister(WHO_AM_I);

	while(id != 0x69)
	{
		HAL_Delay(100);
		id = this->ReadRegister(WHO_AM_I);
	}
	this->WriteRegister(FUNC_CFG_ACCESS,0x01);

	registerBuff[0] = (7 << 4) | (0 << 2) | (0 << 0) ;    // +-2g 
	registerBuff[1] = (7 << 4) | (1 << 2) | (0 << 0) ;    // +-500dps
	registerBuff[2] = 0x44 ;   
	registerBuff[3] = 0;                 
	registerBuff[4] = (3 << 5);             //Gyroscope + accelerometer
	registerBuff[5] = 0;
	registerBuff[6] = 0;
	registerBuff[7] = 0;
	registerBuff[8] = 0x38;
	registerBuff[9] = 0x38;
	
	this->WriteRegister(CTRL1_XL,registerBuff,10);
	//this->ReadRegister(CTRL1_XL,registerBuff,3);
	this->WriteRegister(FUNC_CFG_ACCESS,0x00);
	
	this->adToRadian = (17.50f * PI) / (1000 * 180);
	this->adToAcceleration = (0.061f * 9.81f) / 1000;
}

uint8_t Lsm6ds33Ctrl::ReadRegister(uint8_t addr)
{
	uint8_t buff[2];
	uint8_t rdbuff[2];
	buff[0] = addr | ( 1 << 7);
  SetLsm6ds33Low();
	this->pSpi->TransmitReceive(buff,rdbuff,2,1);
	SetLsm6ds33High();
	return rdbuff[1];
}

void Lsm6ds33Ctrl::ReadRegister(uint8_t addr,void *data,int size)
{
	uint8_t buff[16];
	uint8_t rdbuff[16];
	buff[0] = addr | ( 1 << 7);
  SetLsm6ds33Low();
	this->pSpi->TransmitReceive(buff,rdbuff,size + 1,1);
	SetLsm6ds33High();
	memcpy(data,rdbuff + 1,size);
}

void Lsm6ds33Ctrl::WriteRegister(uint8_t addr,void *data,int size)
{
  uint8_t buff[16] ;
	uint8_t rdbuff[16];
	buff[0] = addr | ( 0 << 7);
	memcpy(buff + 1,data,size);
	SetLsm6ds33Low();
	this->pSpi->TransmitReceive(buff,rdbuff,size + 1,1);
	SetLsm6ds33High();
}

void Lsm6ds33Ctrl::WriteRegister(uint8_t addr,uint8_t data)
{
	uint8_t buff[2] ;
	uint8_t rdbuff[2];
	buff[0] = addr | ( 0 << 7);
	buff[1] = data;
	SetLsm6ds33Low();
	this->pSpi->TransmitReceive(buff,rdbuff,2,1);
	SetLsm6ds33High();
}
