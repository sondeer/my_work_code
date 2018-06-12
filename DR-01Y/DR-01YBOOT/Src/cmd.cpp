
#include "cmd.h"
#include "appimage.h"
#include <string.h>


#define PACKET_HEADER   0xEA
#define GET_PACKET_LENGTH(a)  ((a) & 0x1f)
#define GET_PACKET_CMD(a)     (uint8_t)(((a) >> 5) & 0x07)
#define GEN_PACKET_CMDLENGTH(a,b) (uint8_t)(((a) << 5) | (b))

#define CMD_ACK               0
#define CMD_GOTO_APP          1
#define CMD_SET_ADDR          2
#define CMD_SET_DATA          3
#define CMD_GET_VER           4
#define CMD_CHECK_AND_PRO     5  
#define CMD_SET_FIRMWARE_INF  6
#define CMD_EXTEND            7


#define CMD_EXT_DATA_NOACK   0
#define CMD_EXT_DATA_HASACK  1
#define CMD_EXT_LEAK_PACKET  2

typedef struct
{
	BootPacketStruct sendPacket[8];
	int head;
	int tail;
}SendPacketBuff;

SendPacketBuff sendBuff;

extern AppImage appImg;

CmdHandle::CmdHandle()
{
	this->_dataInBuff = 0;
	sendBuff.head = sendBuff.tail = 0;
	_dataSendAckFlag = true;
}

void CmdHandle::Update(uint8_t *buff,int size)
{
    int nread = size;
    int n;

    while (nread > 0) {
        int ret = 0;
        n = DATA_BUFF_SIZE - _dataInBuff;
        //Debug_Printf("_dataInBuff = %d\n",_dataInBuff);
        if (nread > n) {
            memcpy(this->_dataBuffer + _dataInBuff, buff + size - nread, n);
            _dataInBuff += n;
            nread -= n;
        }
        else {
            memcpy(this->_dataBuffer + _dataInBuff, buff + size - nread, nread);
            _dataInBuff += nread;
            nread = 0;
        }

        int findIndex = 0;

        while (1) {

            for (; findIndex < _dataInBuff; findIndex++) {
                // Debug_Printf("%x ",this->_dataBuff[findIndex]);
                if (this->_dataBuffer[findIndex] == PACKET_HEADER)
                    break;
            }

            //Debug_Printf("\nfindIndex = %d\n",findIndex);
            if (findIndex == _dataInBuff) {
                break;
            }
            BootPacketStruct *p_Packet = (BootPacketStruct *) (this->_dataBuffer + findIndex);
            ret = PacketCheck(p_Packet, _dataInBuff - findIndex);
            if (ret == 0) {
                //Debug_Printf("find failed %d\n", ret);
                break;
            }
            else if (ret < 4) {
                //Debug_Printf("find failed %d\n", ret);
                findIndex += ret;
            }
            else {
                //Debug_Printf("ret = %d\n",ret);
                //  if(p_IbusMessage->destAddress == PLANE_ADDR)
                AnalysisPacket(p_Packet);
                findIndex += ret;
            }
        }
        if (findIndex == 0)
            return;

        int leftNum = _dataInBuff - findIndex;
        for (int i = 0; i < leftNum; i++) {
            this->_dataBuffer[i] = this->_dataBuffer[findIndex + i];
        }
        _dataInBuff = leftNum;
    }
}

int  CmdHandle::GetSendData(uint8_t *buff)
{
	if(sendBuff.head == sendBuff.tail)
		return 0;
	
	int length = GET_PACKET_LENGTH(sendBuff.sendPacket[sendBuff.tail].cmdAndLength) + 4;
	memcpy(buff,sendBuff.sendPacket + sendBuff.tail,length);
	sendBuff.tail ++;
	if(sendBuff.tail == 8)
		sendBuff.tail = 0;
	return length;
}

int CmdHandle::PacketCheck(BootPacketStruct *p_Packet,int length)
{
	  if (length < 4)
        return 0;
		
		int dataLength = GET_PACKET_LENGTH(p_Packet->cmdAndLength);

    if (dataLength > 16)
        return 1;

    if (dataLength + 4 > length)
        return 0;
		
    uint8_t *p_ckSum = (uint8_t *) (p_Packet->data + dataLength);

    if (*p_ckSum == CheckSum(p_Packet,dataLength + 3))
        return dataLength + 4;
    else
        return 1;
}

uint8_t CmdHandle::CheckSum(void *buff,int size)
{
	uint8_t sum = 0;
	uint8_t *p = (uint8_t *)buff;
	for(int i = 0 ; i < size ; i ++)
	{
		sum ^= p[i] ;
	}
	return sum;
}

void CmdHandle::AnalysisPacket(BootPacketStruct *p_Packet)
{	
	//uint8_t dataLength = GET_PACKET_LENGTH(p_Packet->cmdAndLength);
	uint8_t cmd = GET_PACKET_CMD(p_Packet->cmdAndLength);
	switch(cmd)
	{
		case CMD_ACK:
			break;
    case CMD_GOTO_APP:			
	
			if(appImg.GoToApp(1) == -1)
			{
				uint8_t nack = 1;
				AddSendPacket(CMD_ACK,p_Packet->serialNum,&nack,1);
			}
			break;
    case CMD_SET_ADDR:
		{
			uint32_t addr = 0;
			memcpy(&addr,p_Packet->data,4);
			if(appImg.SetWriteAddr(addr) == -1)
			{
				uint8_t nack = 1;
				AddSendPacket(CMD_ACK,p_Packet->serialNum,&nack,1);
			}
			else 
			{
				uint8_t ack = 0;
				AddSendPacket(CMD_ACK,p_Packet->serialNum,&ack,1);
			}
		}
			break;
    case CMD_SET_DATA:
		  {
			   appImg.SetDataBuff(p_Packet->serialNum,p_Packet->data,16);
				 if(_dataSendAckFlag)
				 {
				   uint8_t ack = 0;
				   AddSendPacket(CMD_ACK,p_Packet->serialNum,&ack,1);
				 }
				 else 
				 {
					 DeleteLeakPacketFlag(p_Packet->serialNum);
				 }
			}
			break;
    case CMD_GET_VER:
		  {
			  uint32_t ver[3] = {0,0,0};
				ver[0] = appImg.GetSoftVer();
				ver[1] = appImg.GetHardVer();
				ver[2] = 1;
		    AddSendPacket(CMD_GET_VER,p_Packet->serialNum,(uint8_t*)ver,9);
		  }
			break;
    case CMD_CHECK_AND_PRO: 
		{
			uint32_t ckSum = 0,dataNum = 0;
			memcpy(&dataNum,p_Packet->data,4);
			memcpy(&ckSum,p_Packet->data + 4,4);
			if(appImg.CheckBuffData(dataNum) != ckSum)
			{
				uint8_t nack = 1;
				AddSendPacket(CMD_ACK,p_Packet->serialNum,&nack,1);
			}
			else 
			{
				appImg.WriteBuffFlash(dataNum);
				uint8_t ack = 0;
				AddSendPacket(CMD_ACK,p_Packet->serialNum,&ack,1);
			}
		}
			break;
		case CMD_SET_FIRMWARE_INF:
		  {
			   FirmwareInfStruct firmwareInf;
				 memcpy(&firmwareInf.appBeginAddr,p_Packet->data,4);
				 memcpy(&firmwareInf.appImageLength,p_Packet->data + 4,4);
				 memcpy(&firmwareInf.appVersion,p_Packet->data + 8,4);
				 memcpy(&firmwareInf.appCheckSum,p_Packet->data + 12,4);
				 //firmwareInf.appBeginAddr = *(uint32_t*)p_Packet->data;
				 //firmwareInf.appImageLength = *(uint32_t*)(p_Packet->data + 4);
	       //firmwareInf.appVersion = *(uint32_t*)(p_Packet->data + 8);
	       //firmwareInf.appCheckSum = *(uint32_t*)(p_Packet->data + 12);
				 appImg.WriteFirmwareInf(&firmwareInf);
				 if(appImg.IsAppCheckRight() != 0)
				 {
					 uint8_t nack = 1;
				   AddSendPacket(CMD_ACK,p_Packet->serialNum,&nack,1);
				 }
				 else
				 {
					 uint8_t ack = 0;
				   AddSendPacket(CMD_ACK,p_Packet->serialNum,&ack,1);
				 }
			}
			break;
		case CMD_EXTEND:
		{
			AnalysisExtPacket(p_Packet);
		}
		  break;
	}
}

void CmdHandle::AnalysisExtPacket(BootPacketStruct *p_Packet)
{
	switch(p_Packet->data[0])
	{
		case CMD_EXT_DATA_NOACK:
		{
			InitLeakPacketFlag(p_Packet->data[1]);
		  uint8_t ack = 0;
			_dataSendAckFlag = false;
			AddSendPacket(CMD_ACK,p_Packet->serialNum,&ack,1);
		}
			break;
		case CMD_EXT_DATA_HASACK:
		{
			_dataSendAckFlag = true;
			uint8_t ack = 0;
			AddSendPacket(CMD_ACK,p_Packet->serialNum,&ack,1);
		}
			break;
		case CMD_EXT_LEAK_PACKET:
		{
			uint8_t buff[16];
			buff[0] = CMD_EXT_LEAK_PACKET;
			int length = GetLeakPacketIndex(buff + 1,15);
			AddSendPacket(CMD_EXTEND,p_Packet->serialNum,buff,length + 1);
		}
			break;
	}
}

int  CmdHandle::AddSendPacket(uint8_t cmd,uint8_t sNum,uint8_t *data,int length)
{
	BootPacketStruct *p_Packet = sendBuff.sendPacket + sendBuff.head;
	sendBuff.head ++;
	if(sendBuff.head == 8)
		sendBuff.head = 0;
	p_Packet->header = PACKET_HEADER;
	p_Packet->serialNum = sNum;
	p_Packet->cmdAndLength = GEN_PACKET_CMDLENGTH(cmd,length) ;
	memcpy(p_Packet->data,data,length);
	p_Packet->data[length] = CheckSum(p_Packet,length + 3);
	
	return length + 4;
}

void CmdHandle::InitLeakPacketFlag(int num)
{
	int div8 = (num + 7 )/ 8;
	_dataPacketNum = num;
	if(div8 > 16)
		div8 = 16;
	for(int i = 0 ; i < div8 ; i ++)
	{
		_leakPacketFlag[i] = 0;
	}
}

void CmdHandle::DeleteLeakPacketFlag(int index)
{
	int byteIndex = index / 8;
	int bitIndex = index % 8;
	_leakPacketFlag[byteIndex] |=  (1 << bitIndex);
}

int CmdHandle::GetLeakPacketIndex(uint8_t *buff,int size)
{
	int num = 0;
	int div8 = (_dataPacketNum + 7 )/ 8;
	
	for(int i = 0 ; i < div8 ; i ++)
	{
		if(_leakPacketFlag[i] != 0xff)
		{
			for(int j = 0 ; j < 8 ; j ++)
			{
				int index = i * 8 + j;
				if(index >= _dataPacketNum || num == size)
					return num;
				if(!(_leakPacketFlag[i] & ( 1 << j)))
				{
					buff[num ++] = index;
				}
			}
		}
	}
	return num;
}

