#include "HMC5983.h"
#include <string.h>

#define MAG_ADDRESS 0x3C
#define MAG_DATA_REGISTER 0x03
#define ConfigRegA           0x00
#define ConfigRegB           0x01
#define magGain              0x20
#define PositiveBiasConfig   0x11
#define NegativeBiasConfig   0x12
#define NormalOperation      0x10
#define ModeRegister         0x02
#define ContinuousConversion 0x00
#define SingleConversion     0x01

#define SampleAveraging_1    0x00
#define SampleAveraging_2    0x01
#define SampleAveraging_4    0x02
#define SampleAveraging_8    0x03

#define DataOutputRate_0_75HZ 0x00
#define DataOutputRate_1_5HZ  0x01
#define DataOutputRate_3HZ    0x02
#define DataOutputRate_7_5HZ  0x03
#define DataOutputRate_15HZ   0x04
#define DataOutputRate_30HZ   0x05
#define DataOutputRate_75HZ   0x06

#define SetHmc5983High()  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_SET)
#define SetHmc5983Low()   HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_RESET)


void Hmc5983Ctrl::Init(SpiCtrl *pSpi)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	__GPIOD_CLK_ENABLE();

	SetHmc5983High();
	GPIO_InitStruct.Pin   = GPIO_PIN_11;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  	
	this->pSpi = pSpi;
  uint8_t rValue[3] = {SampleAveraging_8 << 5 | DataOutputRate_75HZ << 2 | NormalOperation,0,ContinuousConversion};
	this->WriteRegister(ConfigRegA,rValue,3);
}

void Hmc5983Ctrl::DeInit(void)
{
}

void Hmc5983Ctrl::ReadMagneticFloat(float mc[3])
{
	int16_t buff[3];
	
	ReadMagnetic(buff);
	
	for(int i = 0 ; i < 3 ; i ++)
	{
		mc[i] = buff[i] / 1370.0f;
	}
}

void Hmc5983Ctrl::ReadMagnetic(int16_t mc[3])
{
	uint8_t buff[8] = {0,0,0,0,0,0,0,0};
	uint8_t rdbuff[8];
	buff[0] = 0x03 | ( 1 << 6) | ( 1 << 7);
	SetHmc5983Low();
	this->pSpi->TransmitReceive(buff,rdbuff,7,1);
	SetHmc5983High();
	
  mc[0] = rdbuff[1] << 8 | rdbuff[2];
  mc[2] = rdbuff[3] << 8 | rdbuff[4];
  mc[1] = rdbuff[5] << 8 | rdbuff[6];
}

void Hmc5983Ctrl::ReadRegister(uint8_t addr,void *data,int size)
{
	uint8_t buff[8] = {0,0,0,0,0,0,0,0};
	uint8_t rdbuff[8];
	buff[0] = addr | ( 1 << 6) | ( 1 << 7);
	SetHmc5983Low();
	this->pSpi->TransmitReceive(buff,rdbuff,size + 1,1);
	SetHmc5983High();
	
	memcpy(data,rdbuff + 1,size);
}

void Hmc5983Ctrl::WriteRegister(uint8_t addr,void *data,int size)
{
  uint8_t buff[8] = {0,0,0,0,0,0,0,0};
	uint8_t rdbuff[8];
	buff[0] = addr | ( 1 << 6) | ( 0 << 7);
	memcpy(buff + 1,data,size);
	SetHmc5983Low();
	this->pSpi->TransmitReceive(buff,rdbuff,size + 1,1);
	SetHmc5983High();
}

