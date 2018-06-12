#include "stdafx.h"
#include "CmdHandle.h"


#define PACKET_HEADER   0xEA
#define GET_PACKET_LENGTH(a)  (a & 0x1f)
#define GET_PACKET_CMD(a)     ((a >> 5) & 0x03)
#define GEN_PACKET_CMDLENGTH(cmd,len) (unsigned char)((cmd << 5) | len)

#define CMD_ACK               0
#define CMD_GOTO_APP          1
#define CMD_SET_ADDR          2
#define CMD_SET_DATA          3
#define CMD_GET_VER           4
#define CMD_CHECK_AND_PRO     5  
#define CMD_SET_FIRMWARE_INF  6
#define CMD_EXTEND            7

CmdHandle::CmdHandle()
{
	this->serialNum = 0;
}


CmdHandle::~CmdHandle()
{
}

int CmdHandle::GenerateGotoAppPacket(void *buff)
{
	BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
	p_bootPacket->header = PACKET_HEADER;
	p_bootPacket->serialNum = ++serialNum;
	p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_GOTO_APP, 0);
	p_bootPacket->data[0] = CheckSum(buff, 3);

	return 4;
}

int CmdHandle::GenerateSetAddrPacket(void *buff, unsigned long addr)
{
	BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
	p_bootPacket->header = PACKET_HEADER;
	p_bootPacket->serialNum = ++serialNum;
	p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_SET_ADDR, 4);
	memcpy(p_bootPacket->data, &addr, 4);
	p_bootPacket->data[4] = CheckSum(buff, 3 + 4);
	return 8;
}

int CmdHandle::GenerateSetDataPacket(void *buff, int sNum, void *data, int length)
{
	BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
	p_bootPacket->header = PACKET_HEADER;
	p_bootPacket->serialNum = sNum;
	this->serialNum = sNum;
	p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_SET_DATA, length);
	memcpy(p_bootPacket->data, data, length);
	p_bootPacket->data[length] = CheckSum(buff, 3 + length);
	return length + 4;
}

int CmdHandle::GenerateGetVerPacket(void *buff)
{
	BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
	p_bootPacket->header = PACKET_HEADER;
	p_bootPacket->serialNum = ++serialNum;
	p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_GET_VER, 0);
	p_bootPacket->data[0] = CheckSum(buff, 3);

	return 4;
}

int CmdHandle::GenerateCkAndProPacket(void *buff, void *data, unsigned long num)
{
	unsigned long *p = (unsigned long *)data;
	unsigned long ckSum = 0;
	for (int i = 0; i < num / 4; i++)
	{
		ckSum ^= p[i];
	}

	BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
	p_bootPacket->header = PACKET_HEADER;
	p_bootPacket->serialNum = ++ serialNum;
	p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_CHECK_AND_PRO, 8);
	memcpy(p_bootPacket->data, &num, 4);
	memcpy(p_bootPacket->data + 4, &ckSum, 4);
	p_bootPacket->data[8] = CheckSum(buff, 3 + 8);

	return 12;
}

int CmdHandle::GenerateSetInfPacket(void *buff, void *data, unsigned long length, unsigned long addr, unsigned long ver)
{
	unsigned long *p = (unsigned long *)data;
	unsigned long ckSum = 0;
	for (int i = 0; i < length / 4; i++)
	{
		ckSum ^= p[i];
	}

	BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
	p_bootPacket->header = PACKET_HEADER;
	p_bootPacket->serialNum = ++ serialNum;
	p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_CHECK_AND_PRO, 16);
	memcpy(p_bootPacket->data, &addr, 4);
	memcpy(p_bootPacket->data + 4, &length, 4);
	memcpy(p_bootPacket->data + 8, &ver, 4);
	memcpy(p_bootPacket->data + 12, &ckSum, 4);
	p_bootPacket->data[16] = CheckSum(buff, 3 + 16);
	return 20;
}

bool CmdHandle::CheckAckPacket(void *data, int length)
{
	unsigned char *p = (unsigned char *)data;
	if (p[0] != PACKET_HEADER)
		return false;
	if (length < 5)
		return false;
	BootPacketStruct *p_bootPacket = (BootPacketStruct *)data;
	int dataLength = GET_PACKET_LENGTH(p_bootPacket->cmdAndLength);
	if (dataLength > 16)
		return false;
	if (dataLength + 4 > length)
		return false;
	if (p_bootPacket->data[dataLength] != CheckSum(data, dataLength + 3))
		return false;

	if (GET_PACKET_CMD(p_bootPacket->cmdAndLength) == CMD_ACK && p_bootPacket->serialNum == serialNum && p_bootPacket->data[0] == 0)
		return true;
	return false;
}

unsigned long CmdHandle::GetSoftVersion(void *data, int length)
{
	unsigned char *p = (unsigned char *)data;
	if (p[0] != PACKET_HEADER)
		return 0;
	if (length < 8)
		return 0;
	BootPacketStruct *p_bootPacket = (BootPacketStruct *)data;
	int dataLength = GET_PACKET_LENGTH(p_bootPacket->cmdAndLength);
	if (dataLength > 16)
		return 0;
	if (dataLength + 4 > length)
		return 0;
	if (p_bootPacket->data[dataLength] != CheckSum(data, dataLength + 3))
		return 0;

	if (GET_PACKET_CMD(p_bootPacket->cmdAndLength) == CMD_GET_VER && p_bootPacket->serialNum == serialNum)
		return *(unsigned long *)p_bootPacket->data;
	return 0;
}

unsigned char CmdHandle::CheckSum(void *buff, int size)
{
	unsigned char sum = 0;
	unsigned char *p = (unsigned char *)buff;
	for (int i = 0; i < size; i++)
	{
		sum ^= p[i];
	}
	return sum;
}
