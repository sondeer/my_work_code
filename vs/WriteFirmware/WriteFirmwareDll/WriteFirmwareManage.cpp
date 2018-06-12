#include "stdafx.h"
#include "WriteFirmwareManage.h"
#include "CmdHandle.h"
#include "HexFile.h"
#include "debug.h"


HexFile hexFile;
CmdHandle cmdHandle;

WriteFirmwareManage::WriteFirmwareManage()
{
	Init();
	waitTimeOut = 1000;
}


WriteFirmwareManage::~WriteFirmwareManage()
{
}

unsigned long WriteFirmwareManage::Update(unsigned char *buff, int size, unsigned char *outBuff, int *outLength, unsigned long timeMil)
{
	cmdHandle.Update(buff, size);
	*outLength = 0;

/*	hexFile.GetSectionData(0, &fileBaseAddr, (char*)fileDataBuff, &fileDataLength);
	*outLength = cmdHandle.GenerateCkAndProPacket(outBuff, fileDataBuff, 0x800);
	return 0;*/
	Debug_Printf("processState = %d\n", processState);
	switch (processState)
	{
	case 0:
		waitTimeBase = timeMil;
		*outLength = cmdHandle.GenerateGetVerPacket(outBuff);
		processState++;
		break;
	case 1:
		char inbootFlag;
		if (cmdHandle.GetSoftVersion(&deviceSoftVer,NULL, &inbootFlag) == 0)
		{
			if (inbootFlag == 1)
			{
				processState++;
				int num = hexFile.GetSectionNum();
				int len = 0;
				unsigned long addrTemp;

				fileDataLength = 0;

				hexFile.GetSectionData(0, &fileBaseAddr, (char*)fileDataBuff, &fileDataLength);
				for (int i = 1; i < num; i++)
				{
					hexFile.GetSectionData(i, &addrTemp, (char*)fileDataBuff + fileDataLength, &len);
					fileDataLength += len;
				}

				division2kNum = (fileDataLength + 0x7ff) >> 11;
				index2k = 0;
			}
			else
			{
				*outLength = cmdHandle.SetEnterProgram(outBuff);
				processState--;
			}
		}
		else if(timeMil - waitTimeBase >= waitTimeOut)
		{
			processState--;
		}
		break;
	case 2:
		if (beginFlag)
		{
			waitTimeBase = timeMil;
			*outLength = cmdHandle.GenerateSetAddrPacket(outBuff, fileBaseAddr + index2k * 0x800);
			processState++;
		}
		break;
	case 3:
		if (cmdHandle.CheckAckPacket() == 0)
		{
			processState++;
			if (index2k == division2kNum - 1)
			{
				division16Num = ((fileDataLength & 0x7ff) + 15) / 16;
			}
			else
			{
				division16Num = 128;
			}
			index16 = 0;
		}
		else if(timeMil - waitTimeBase >= waitTimeOut)
		{
			processState--;
		}
		break;
	case 4:
		waitTimeBase = timeMil;
		*outLength = cmdHandle.GenerateSetNoAckPacket(outBuff, division16Num);
		processState++;
		break;
	case 5:
		if (cmdHandle.CheckAckPacket() == 0)
		{
			processState++;
		}
		else if (timeMil - waitTimeBase >= waitTimeOut)
		{
			processState--;
		}
		break;
	case 6:
		if ((index2k == division2kNum - 1) && (index16 == division16Num - 1))
		{
			int sendLen = fileDataLength % 16;
			if (sendLen == 0)
				sendLen = 16;
			*outLength = cmdHandle.GenerateSetDataPacket(outBuff, index16, fileDataBuff + index2k * 0x800 + index16 * 16, sendLen);
		}
		else
		{
			*outLength = cmdHandle.GenerateSetDataPacket(outBuff, index16, fileDataBuff + index2k * 0x800 + index16 * 16, 16);
		}
		index16 ++;
		if (index16 == division16Num)
			processState++;
		break;
	case 7:
		waitTimeBase = timeMil;
		*outLength = cmdHandle.GenerateGetLeakPacket(outBuff);
		processState++;
		break;
	case 8:
		if (cmdHandle.GetLeakPacket(leakPacket,&leakNum) == 0)
		{
			if (leakNum > 0)
			{
				leadIndex = 0;
				processState++;
			}  
			else if (leakNum == 0)
			{
				processState = 10;
				processState++;				
			}
		}
		else if (timeMil - waitTimeBase >= waitTimeOut)
		{
			processState --;
		}
		break;
	case 9:
		if ((index2k == division2kNum - 1) && (leakPacket[leadIndex] == division16Num - 1))
		{
			int sendLen = fileDataLength % 16;
			if (sendLen == 0)
				sendLen = 16;
			*outLength = cmdHandle.GenerateSetDataPacket(outBuff, leakPacket[leadIndex], fileDataBuff + index2k * 0x800 + leakPacket[leadIndex] * 16, sendLen);
		}
		else
		{
			*outLength = cmdHandle.GenerateSetDataPacket(outBuff, leakPacket[leadIndex], fileDataBuff + index2k * 0x800 + leakPacket[leadIndex] * 16, 16);
		}
		leadIndex ++;

		if (leadIndex == leakNum)
			processState = 7;
		break;
	case 10:
		waitTimeBase = timeMil;
		if (index2k == division2kNum - 1)
		{
			*outLength = cmdHandle.GenerateCkAndProPacket(outBuff, fileDataBuff + index2k * 0x800, fileDataLength & 0x7ff);
		}
		else
		{
			*outLength = cmdHandle.GenerateCkAndProPacket(outBuff, fileDataBuff + index2k * 0x800, 0x800);
		}
		Debug_Printf("fileDataLength = %d\n", fileDataLength);
		processState++;
		break;
	case 11:
		if (cmdHandle.CheckAckPacket() == 0)
		{
			index2k++;
			processPercentage = index2k * 100 / division2kNum;
			if (index2k == division2kNum )
			{
				processState++;
			}
			else
			{
				processState = 2;
			}
		}
		else if(timeMil - waitTimeBase >= waitTimeOut)
		{
			processState--;
		}
		break;
	case 12:
		waitTimeBase = timeMil;
		*outLength = cmdHandle.GenerateSetInfPacket(outBuff, fileDataBuff, fileDataLength, fileBaseAddr, hexFile.GetFileVer());
		processState++;
		break;
	case 13:
		if (cmdHandle.CheckAckPacket() == 0)
		{
			processPercentage = 100;
			processState++;
		}
		else if (timeMil - waitTimeBase >= waitTimeOut)
		{
			processState--;
		}
		break;
	case 14:
		*outLength = cmdHandle.GenerateGotoAppPacket(outBuff);
		processState++;
		break;
	default:
		beginFlag = false;
		break;
	}

	return (processState << 16) | processPercentage;
}
/*
int WriteFirmwareManage::GetDataToSend(void *buff, int size)
{
	int sizeBuff = sendBuffLength;
	if(sendBuffLength > 0)
	   memcpy(buff, sendBuff, sendBuffLength);
	sendBuffLength = 0;
	return sizeBuff;
}
*/
unsigned long WriteFirmwareManage::GetDeviceSoftVer(void)
{
	return deviceSoftVer;
}

int WriteFirmwareManage::BeginWrite(void)
{
	beginFlag = true;
	return 0;
}

void WriteFirmwareManage::Init(void)
{
	processState = processPercentage = 0;
	beginFlag = false;
	waitTimeOut = 100;
	deviceSoftVer = 0;
}
