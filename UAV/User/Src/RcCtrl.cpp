
#include "RcCtrl.h"

void  RcCtrl::Init(void)
{
	this->sbus.Init();
	
	channelsNum[0] = 2;
	subValue[0] = 352;
	widthValue[0] = 1344;
	
	channelsNum[1] = 1;
	subValue[1] = 1024;
	widthValue[1] = 1000;
	
	channelsNum[2] = 0;
	subValue[2] = 1024;
	widthValue[2] = 1000;
	
	channelsNum[3] = 3;
	subValue[3] = 1024;
	widthValue[3] = 1000;
}

float RcCtrl::GetThrottle(void)
{
	return ((float)(this->sbus.GetChannelData(channelsNum[0]) )  - subValue[0] )/ widthValue[0];
}

float RcCtrl::GetTargetPitch(void)
{
	return ((float)(this->sbus.GetChannelData(channelsNum[1]) )  - subValue[1] )/ widthValue[1];
}

float RcCtrl::GetTargetRoll(void)
{
	return ((float)(this->sbus.GetChannelData(channelsNum[2]) )  - subValue[2] )/ widthValue[2];
}

float RcCtrl::GetTargetYaw(void)
{
	return ((float)(this->sbus.GetChannelData(channelsNum[3]) )  - subValue[3] )/ widthValue[3];
}

void  RcCtrl::Run2ms(uint32_t clock)
{
	this->sbus.SbusRun2ms(clock);
}

bool  RcCtrl::IsLinked(void)
{
	return this->sbus.IsLinked();
	return true;
}
