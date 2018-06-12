
#ifndef __CMD_H
#define __CMD_H

#include "stm32f0xx_hal.h"

#define MAX_NUM_SIZE 16
#define DATA_BUFF_SIZE 256

typedef __packed struct {
    uint8_t header;
	  uint8_t serialNum;
	  uint8_t cmdAndLength;
	  uint8_t data[MAX_NUM_SIZE + 1];
}BootPacketStruct;

class CmdHandle 
{
	public:
		CmdHandle();
	  void Update(uint8_t *buff,int size);
	  int  GetSendData(uint8_t *buff);
	protected:
		int  PacketCheck(BootPacketStruct *p_Packet,int length);
		uint8_t CheckSum(void *buff,int size);
	  void AnalysisPacket(BootPacketStruct *p_Packet);
	  void AnalysisExtPacket(BootPacketStruct *p_Packet);
	  int  AddSendPacket(uint8_t cmd,uint8_t sNum,uint8_t *data,int length);
	  void InitLeakPacketFlag(int num);
	  void DeleteLeakPacketFlag(int index);
	  int  GetLeakPacketIndex(uint8_t *buff,int size);
	
	private:
		uint8_t _dataBuffer[DATA_BUFF_SIZE];
	  int _dataInBuff;
	
	  bool _dataSendAckFlag;
	  int _dataPacketNum;
	  uint8_t _leakPacketFlag[16];
};

#endif
