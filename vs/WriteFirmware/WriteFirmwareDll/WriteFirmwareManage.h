#pragma once

class WriteFirmwareManage
{
public:
	WriteFirmwareManage();
	~WriteFirmwareManage();
	unsigned long Update(unsigned char *buff, int size, unsigned char *outBuff,int *outLength, unsigned long timeMil);
	//int GetDataToSend(void *buff,int size);
	unsigned long GetDeviceSoftVer(void);
	int BeginWrite(void);
	void Init(void);
private:
	unsigned long waitTimeBase;
	int waitTimeOut;
	int processState;
	int processPercentage;
	unsigned char sendBuff[32];
	int sendBuffLength;
	unsigned long deviceSoftVer;
	unsigned long fileBaseAddr;
	unsigned char fileDataBuff[0x40000];
	int fileDataLength;

	int division2kNum;
	int index2k;

	int division16Num;
	int index16;

	unsigned char leakPacket[16];
	int leakNum;
	int leadIndex;

	bool beginFlag;
};

