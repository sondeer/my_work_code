#pragma once

#pragma pack(1)

#define MAX_NUM_SIZE 16

typedef struct {
	unsigned char header;
	unsigned char serialNum;
	unsigned char cmdAndLength;
	unsigned char data[MAX_NUM_SIZE + 1];
}BootPacketStruct;

#pragma pack()

class CmdHandle
{
public:
	CmdHandle();
	~CmdHandle();
	int GenerateGotoAppPacket(void *buff);
	int GenerateSetAddrPacket(void *buff,unsigned long addr);
	int GenerateSetDataPacket(void *buff, int sNum, void *data, int length);
	int GenerateGetVerPacket(void *buff);
	int GenerateCkAndProPacket(void *buff, void *data,unsigned long num);
	int GenerateSetInfPacket(void *buff, void *data, unsigned long length, unsigned long addr,unsigned long ver);
	bool CheckAckPacket(void *data, int length);
	unsigned long GetSoftVersion(void *data, int length);

	unsigned char CheckSum(void *buff, int size);
private:
	unsigned char serialNum;
};

