#include "stdafx.h"
#include "CmdHandle.h"
#include "debug.h"


#define PACKET_HEADER   0xEA
#define GET_PACKET_LENGTH(a)  (a & 0x1f)
#define GET_PACKET_CMD(a)     ((a >> 5) & 0x07)
#define GEN_PACKET_CMDLENGTH(cmd,len) (unsigned char)((cmd << 5) | len)

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
#define CMD_EXT_SET          3
#define CMD_EXT_GET          4




#define SET_MOTOR_SPEED      1
#define SET_LED_PATTERN      4
#define SET_ROCKER_SPEED     6
#define SET_DEVICE_MODE      7
#define SET_AUDIO_INDEX      8
#define SET_LED_PATTERN_2    9
#define SET_THROWER_STATE    0x0a

#define SET_AUDIO_OFF        0x0b
#define SET_SEG_DISPLAY 0x0c
#define SET_BEEP_FREQ 0x0d
#define SET_LANGUAGE  0x0e
#define SET_APP_PAGE  0x0f

#define SET_PROGRAM_MODE 0x80



#define GET_DEVICE_MODE      7

#define GET_ULTRASONIC_DIS   0x10
#define GET_LINE_PATROL      0x11
#define GET_GYRO             0x12
#define GET_BATTERY_STATE    0x13 //获取电池状态
#define GET_KEY_STATE        0x14
#define GET_THROW_STATE        0x15


#define GET_TEMPERATURE 0x20
#define GET_LIGHT_SENSOR 0x21
#define GET_SOUND_SENSOR 0x22
#define GET_MOTION_SENSOR 0x23
#define GET_GAS_SENSOR 0x24
#define GET_FLAME_SENSOR 0x25

#define GET_ALL_STATE        0xff



CmdHandle::CmdHandle()
{
    this->serialNum = 0;
    this->_dataInBuff = 0;
    this->repeatFlag = false;
}


CmdHandle::~CmdHandle()
{
}


void CmdHandle::Update(unsigned char *buff, int size)
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
            BootPacketStruct *p_Packet = (BootPacketStruct *)(this->_dataBuffer + findIndex);
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

int CmdHandle::GetReSendPacket(unsigned char *buff)
{
   if(!repeatFlag)
      return 0;
   time10msCount ++;
   if(time10msCount >= 10)
   {
       time10msCount = 0;
       memcpy(buff,&lastSendPacket,sizeof(BootPacketStruct));
       return GET_PACKET_LENGTH(lastSendPacket.cmdAndLength) + 4;
   }
   return 0;
}

int CmdHandle::GenerateGotoAppPacket(void *buff)
{
	BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
	p_bootPacket->header = PACKET_HEADER;
	p_bootPacket->serialNum = ++serialNum;
	p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_GOTO_APP, 0);
	p_bootPacket->data[0] = CheckSum(buff, 3);
	hasLastPacket = false;

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
	hasLastPacket = false;
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
	hasLastPacket = false;
	return length + 4;
}

int CmdHandle::GenerateGetVerPacket(void *buff)
{
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_GET_VER, 0);
    p_bootPacket->data[0] = CheckSum(buff, 3);
    hasLastPacket = false;
    hasActiveData[12] = false;
    return 4;
}

int CmdHandle::GenerateCkAndProPacket(void *buff, void *data, unsigned long num)
{
	unsigned char *p = (unsigned char *)data;
	unsigned char ckSum[4] = { 0 ,0,0,0};
	for (int i = 0; i < num / 4; i++)
	{
		ckSum[0] ^= p[i * 4 + 0];
		ckSum[1] ^= p[i * 4 + 1];
		ckSum[2] ^= p[i * 4 + 2];
		ckSum[3] ^= p[i * 4 + 3];
	}

	BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
	p_bootPacket->header = PACKET_HEADER;
	p_bootPacket->serialNum = ++ serialNum;
	p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_CHECK_AND_PRO, 8);
	memcpy(p_bootPacket->data, &num, 4);
	memcpy(p_bootPacket->data + 4, ckSum, 4);
	p_bootPacket->data[8] = CheckSum(buff, 3 + 8);
	hasLastPacket = false;
	return 12;
}

int CmdHandle::GenerateSetInfPacket(void *buff, void *data, unsigned long length, unsigned long addr, unsigned long ver)
{
	unsigned char *p = (unsigned char *)data;
	unsigned char ckSum[4] = { 0 ,0,0,0 };
	for (int i = 0; i < length / 4; i++)
	{
		ckSum[0] ^= p[i * 4 + 0];
		ckSum[1] ^= p[i * 4 + 1];
		ckSum[2] ^= p[i * 4 + 2];
		ckSum[3] ^= p[i * 4 + 3];
	}

	BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
	p_bootPacket->header = PACKET_HEADER;
	p_bootPacket->serialNum = ++ serialNum;
	p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_SET_FIRMWARE_INF, 16);
	memcpy(p_bootPacket->data, &addr, 4);
	memcpy(p_bootPacket->data + 4, &length, 4);
	memcpy(p_bootPacket->data + 8, &ver, 4);
	memcpy(p_bootPacket->data + 12, ckSum, 4);
	p_bootPacket->data[16] = CheckSum(buff, 3 + 16);
	hasLastPacket = false;
	return 20;
}

int CmdHandle::GenerateSetNoAckPacket(void *buff, int num)
{
	BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
	p_bootPacket->header = PACKET_HEADER;
	p_bootPacket->serialNum = ++serialNum;
	p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 2);
	p_bootPacket->data[0] = CMD_EXT_DATA_NOACK;
	p_bootPacket->data[1] = num;
	p_bootPacket->data[2] = CheckSum(buff, 3 + 2);
	hasLastPacket = false;
	return 6;
}

int CmdHandle::GenerateGetLeakPacket(void *buff)
{
	BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
	p_bootPacket->header = PACKET_HEADER;
	p_bootPacket->serialNum = ++serialNum;
	p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 1);
	p_bootPacket->data[0] = CMD_EXT_LEAK_PACKET;
	p_bootPacket->data[1] = CheckSum(buff, 3 + 1);
	hasLastPacket = false;
	return 5;
}


//add by llj
int CmdHandle::SetMotorSpeed(void *buff, char left_motor_speed , char right_motor_speed)//设置电机的速度
{
    if (left_motor_speed > 0) {
        left_motor_speed = left_motor_speed > 100.0 ? 100.0 : left_motor_speed;
    }else{
        left_motor_speed = left_motor_speed < -100.0 ? -100.0 : left_motor_speed;
    }
    if (right_motor_speed > 0) {
        right_motor_speed = right_motor_speed > 100.0 ? 100.0 : right_motor_speed;
    }else{
        right_motor_speed = right_motor_speed < -100.0 ? -100.0 : right_motor_speed;
    }
    
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 4);
    p_bootPacket->data[0] = CMD_EXT_SET;
    p_bootPacket->data[1] = SET_MOTOR_SPEED;
    p_bootPacket->data[2] = left_motor_speed;
    p_bootPacket->data[3] = right_motor_speed;
    p_bootPacket->data[4] = CheckSum(buff, 3 + 4);
    hasLastPacket = false;
    SetNeedAckPacket(p_bootPacket);
    return 8;//返回的APP需要发送的BUFF数据长度
}


int CmdHandle::SetDeviceMode(void *buff,   unsigned  char mode ,unsigned  char obstacle_avoidance_flag )//设置小车的模式
{
    //   if(mode > 4)
    //        mode = 0;
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 4);
    p_bootPacket->data[0] = CMD_EXT_SET;
    p_bootPacket->data[1] = SET_DEVICE_MODE;
    p_bootPacket->data[2] = 0;//用来确定平衡车或者三轮车，这里用不到
    p_bootPacket->data[3] = mode+(obstacle_avoidance_flag<<7);//
    p_bootPacket->data[4] = CheckSum(buff, 3 + 4);
    hasLastPacket = false;
    SetNeedAckPacket(p_bootPacket);
    return 8;//返回的APP需要发送的BUFF数据长度
}


int CmdHandle::SetRockerSpeed(void *buff,   unsigned  char rocker_speed)//设置摇杆的速度
{
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 3);
    p_bootPacket->data[0] = CMD_EXT_SET;
    p_bootPacket->data[1] = SET_ROCKER_SPEED;
    p_bootPacket->data[2] = rocker_speed;//
    p_bootPacket->data[3] = CheckSum(buff, 3 + 3);
    hasLastPacket = false;
    SetNeedAckPacket(p_bootPacket);
    return 7;//返回的APP需要发送的BUFF数据长度
}

int CmdHandle::SetLedPattern(void *buff,   unsigned  char *led_buff)//设置LED点阵屏数据  由于一次设置不完，所以需要分两次设置
{
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 14);
    p_bootPacket->data[0] = CMD_EXT_SET;
    p_bootPacket->data[1] = SET_LED_PATTERN;
    //p_bootPacket->data[2] = rocker_speed;//
    memcpy(p_bootPacket->data+2, led_buff, 14);
    p_bootPacket->data[14] = CheckSum(buff, 3 + 14);
    hasLastPacket = false;
    SetNeedAckPacket(p_bootPacket);
    return 18;//返回的APP需要发送的BUFF数据长度
    
}
int CmdHandle::SetLedPattern2(void *buff,   unsigned  char *led_buff)//设置LED点阵屏数据  由于一次设置不完，所以需要分两次设置
{
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 14);
    p_bootPacket->data[0] = CMD_EXT_SET;
    p_bootPacket->data[1] = SET_LED_PATTERN_2;
    //p_bootPacket->data[2] = rocker_speed;//
    memcpy(p_bootPacket->data+2, led_buff, 14);
    p_bootPacket->data[14] = CheckSum(buff, 3 + 14);
    hasLastPacket = false;
    SetNeedAckPacket(p_bootPacket);
    return 18;//返回的APP需要发送的BUFF数据长度
}


int CmdHandle::SetAudioIndex(void *buff,   unsigned  int audio_index)//设置需要播放的声音序列
{
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 4);
    p_bootPacket->data[0] = CMD_EXT_SET;
    p_bootPacket->data[1] = SET_AUDIO_INDEX;
    p_bootPacket->data[2] = audio_index%256;//
    p_bootPacket->data[3] = audio_index/256;//
    p_bootPacket->data[4] = CheckSum(buff, 3 + 4);

    hasLastPacket = false;
    SetNeedAckPacket(p_bootPacket);
    return 8;//返回的APP需要发送的BUFF数据长度
}

int CmdHandle::SetThrowerState(void *buff,  unsigned  char port_num,unsigned int freq, unsigned  int compare)
{
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 7);
    p_bootPacket->data[0] = CMD_EXT_SET;
    p_bootPacket->data[1] = SET_THROWER_STATE;
    p_bootPacket->data[2] = port_num;//控制的端口
    p_bootPacket->data[3] = freq%256;//PWM频率
    p_bootPacket->data[4] = freq/256;//PWM频率
    p_bootPacket->data[5] = compare%256;
    p_bootPacket->data[6] = compare/256;
    p_bootPacket->data[7] = CheckSum(buff, 3 + 7);
    hasLastPacket = false;
    SetNeedAckPacket(p_bootPacket);
    
    return 11;//返回的APP需要发送的BUFF数据长度
    
}

int CmdHandle::SetAudioOnOff(void *buff,char flag)
	{
	  BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 3);
    p_bootPacket->data[0] = CMD_EXT_SET;
    p_bootPacket->data[1] = SET_AUDIO_OFF;
    p_bootPacket->data[2] = flag;
    p_bootPacket->data[3] = CheckSum(buff, 3 + 3);
    SetNeedAckPacket(p_bootPacket);
    return 3 + 4;
	}
	
int CmdHandle::SetSegDisplay(void *buff,char port,short value,char pointIndex)
	{
	  BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 6);
    p_bootPacket->data[0] = CMD_EXT_SET;
    p_bootPacket->data[1] = SET_SEG_DISPLAY;
    p_bootPacket->data[2] = port;
    p_bootPacket->data[3] = value & 0x00ff;
    p_bootPacket->data[4] = (value >> 8);
    p_bootPacket->data[5] = pointIndex;
    p_bootPacket->data[6] = CheckSum(buff, 6 + 3);
    SetNeedAckPacket(p_bootPacket);
    return 6 + 4;
	}
	
int CmdHandle::SetBeepFreq(void *buff,unsigned short freq,unsigned short time)
	{
		BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 6);
    p_bootPacket->data[0] = CMD_EXT_SET;
    p_bootPacket->data[1] = SET_BEEP_FREQ;
    p_bootPacket->data[2] = freq & 0x00ff;
    p_bootPacket->data[3] = (freq >> 8);
    p_bootPacket->data[4] = time & 0x00ff;
    p_bootPacket->data[5] = (time >> 8);
    p_bootPacket->data[6] = CheckSum(buff, 6 + 3);
    SetNeedAckPacket(p_bootPacket);
    return 6 + 4;
	}
	
int CmdHandle::SetEnterProgram(void *buff)
	{
		BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 2);
    p_bootPacket->data[0] = CMD_EXT_SET;
    p_bootPacket->data[1] = SET_PROGRAM_MODE;
    p_bootPacket->data[2] = CheckSum(buff, 2 + 3);
   
    return 2 + 4;
	}
	
int CmdHandle::SetLanguage(void *buff,char lag)
{
		BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 3);
    p_bootPacket->data[0] = CMD_EXT_SET;
    p_bootPacket->data[1] = SET_LANGUAGE;
    p_bootPacket->data[2] = lag;
    p_bootPacket->data[3] = CheckSum(buff, 3 + 3);
    
    SetNeedAckPacket(p_bootPacket);
    return 3 + 4;
}

int CmdHandle::SetAppPage(void *buff,char pag)
{
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 3);
    p_bootPacket->data[0] = CMD_EXT_SET;
    p_bootPacket->data[1] = SET_APP_PAGE;
    p_bootPacket->data[2] = pag;
    p_bootPacket->data[3] = CheckSum(buff, 3 + 3);
    
    SetNeedAckPacket(p_bootPacket);
    return 3 + 4;
}

/*****************add by llj  GETDATA*********************/
int CmdHandle::GetDeviceMode(void *buff)//获取设备模式
{
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 2);
    p_bootPacket->data[0] = CMD_EXT_GET;
    p_bootPacket->data[1] = GET_DEVICE_MODE;
    p_bootPacket->data[2] = CheckSum(buff, 3 + 2);
    hasLastPacket = false;
    hasActiveData[0] = false;
    return 6;//返回的APP需要发送的BUFF数据长度
}
int CmdHandle::GetUltrasonicDis(void *buff)//获取超声波距离
{
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 2);
    p_bootPacket->data[0] = CMD_EXT_GET;
    p_bootPacket->data[1] = GET_ULTRASONIC_DIS;
    p_bootPacket->data[2] = CheckSum(buff, 3 + 2);
    hasLastPacket = false;
    hasActiveData[1] = false;
    return 6;//返回的APP需要发送的BUFF数据长度
}
int CmdHandle::GetLinePatrol(void *buff)//获取循线传感器状态
{
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 2);
    p_bootPacket->data[0] = CMD_EXT_GET;
    p_bootPacket->data[1] = GET_LINE_PATROL;
    p_bootPacket->data[2] = CheckSum(buff, 3 + 2);
    hasLastPacket = false;
    hasActiveData[2] = false;
    return 6;//返回的APP需要发送的BUFF数据长度
}
int CmdHandle::GetBatteryState(void *buff)//获取电池状态
{
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 2);
    p_bootPacket->data[0] = CMD_EXT_GET;
    p_bootPacket->data[1] = GET_BATTERY_STATE;
    p_bootPacket->data[2] = CheckSum(buff, 3 + 2);
    hasLastPacket = false;
    hasActiveData[3] = false;
    return 6;//返回的APP需要发送的BUFF数据长度
    
}
int CmdHandle::GetKeyState(void *buff)//获取按键状态
{
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 2);
    p_bootPacket->data[0] = CMD_EXT_GET;
    p_bootPacket->data[1] = GET_KEY_STATE;
    p_bootPacket->data[2] = CheckSum(buff, 3 + 2);
    hasLastPacket = false;
    hasActiveData[4] = false;
    return 6;//返回的APP需要发送的BUFF数据长度
}

int CmdHandle::GetAllState(void *buff)
{
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 2);
    p_bootPacket->data[0] = CMD_EXT_GET;
    p_bootPacket->data[1] = GET_ALL_STATE;
    p_bootPacket->data[2] = CheckSum(buff, 3 + 2);
    hasLastPacket = false;
    for(int i = 0 ; i < 6 ; i ++)
       hasActiveData[i] = false;
    return 6;
}

//int CmdHandle::GetThrowState(void *buff)//获取投石车状态
//{
//    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
//    p_bootPacket->header = PACKET_HEADER;
//    p_bootPacket->serialNum = ++serialNum;
//    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 2);
//    p_bootPacket->data[0] = CMD_EXT_GET;
//    p_bootPacket->data[1] = GET_THROW_STATE;
//    p_bootPacket->data[2] = CheckSum(buff, 3 + 2);
//    hasLastPacket = false;
//    return 6;//返回的APP需要发送的BUFF数据长度
//}

int CmdHandle::GetThrowState(void *buff,unsigned char port_num)//获取投石车状态
{
    BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 3);
    p_bootPacket->data[0] = CMD_EXT_GET;
    p_bootPacket->data[1] = GET_THROW_STATE;
    p_bootPacket->data[2] = port_num;
    p_bootPacket->data[3] = CheckSum(buff, 3 + 3);
    hasLastPacket = false;
    hasActiveData[5] = false;
    return 7;//返回的APP需要发送的BUFF数据长度
}

int CmdHandle::GetTemperature(void *buff,unsigned char port_num )
{
	  BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 3);
    p_bootPacket->data[0] = CMD_EXT_GET;
    p_bootPacket->data[1] = GET_TEMPERATURE;
    p_bootPacket->data[2] = port_num;
    p_bootPacket->data[3] = CheckSum(buff, 3 + 3);
    hasLastPacket = false;
    hasActiveData[6] = false;
    return 7;
}

int CmdHandle::GetLightSensor(void *buff,unsigned char port_num )
{
		BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 3);
    p_bootPacket->data[0] = CMD_EXT_GET;
    p_bootPacket->data[1] = GET_LIGHT_SENSOR;
    p_bootPacket->data[2] = port_num;
    p_bootPacket->data[3] = CheckSum(buff, 3 + 3);
    hasLastPacket = false;
    hasActiveData[7] = false;
    return 7;
}

int CmdHandle::GetSoundSensor(void *buff,unsigned char port_num )
{
		BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 3);
    p_bootPacket->data[0] = CMD_EXT_GET;
    p_bootPacket->data[1] = GET_SOUND_SENSOR;
    p_bootPacket->data[2] = port_num;
    p_bootPacket->data[3] = CheckSum(buff, 3 + 3);
    hasLastPacket = false;
    hasActiveData[8] = false;
    return 7;
}

int CmdHandle::GetMotionSensor(void *buff,unsigned char port_num )
{
		BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 3);
    p_bootPacket->data[0] = CMD_EXT_GET;
    p_bootPacket->data[1] = GET_MOTION_SENSOR;
    p_bootPacket->data[2] = port_num;
    p_bootPacket->data[3] = CheckSum(buff, 3 + 3);
    hasLastPacket = false;
    hasActiveData[9] = false;
    return 7;
}

int CmdHandle::GetGasSensor(void *buff,unsigned char port_num )
{
		BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 3);
    p_bootPacket->data[0] = CMD_EXT_GET;
    p_bootPacket->data[1] = GET_GAS_SENSOR;
    p_bootPacket->data[2] = port_num;
    p_bootPacket->data[3] = CheckSum(buff, 3 + 3);
    hasLastPacket = false;
    hasActiveData[10] = false;
    return 7;
}

int CmdHandle::GetFlameSensor(void *buff,unsigned char port_num )
{
		BootPacketStruct *p_bootPacket = (BootPacketStruct*)buff;
    p_bootPacket->header = PACKET_HEADER;
    p_bootPacket->serialNum = ++serialNum;
    p_bootPacket->cmdAndLength = GEN_PACKET_CMDLENGTH(CMD_EXTEND, 3);
    p_bootPacket->data[0] = CMD_EXT_GET;
    p_bootPacket->data[1] = GET_FLAME_SENSOR;
    p_bootPacket->data[2] = port_num;
    p_bootPacket->data[3] = CheckSum(buff, 3 + 3);
    hasLastPacket = false;
    hasActiveData[11] = false;
    return 7;
}



int CmdHandle::CheckAckPacket(void *data, int length)
{
	unsigned char *p = (unsigned char *)data;
	if (p[0] != PACKET_HEADER)
		return - 1;
	if (length < 5)
		return - 2;
	BootPacketStruct *p_bootPacket = (BootPacketStruct *)data;
	int dataLength = GET_PACKET_LENGTH(p_bootPacket->cmdAndLength);
	if (dataLength > 16)
		return - 3;
	if (dataLength + 4 > length)
		return - 4;
	if (p_bootPacket->data[dataLength] != CheckSum(data, dataLength + 3))
		return - 5;

	if (GET_PACKET_CMD(p_bootPacket->cmdAndLength) == CMD_ACK && p_bootPacket->serialNum == serialNum)
		return  p_bootPacket->data[0];
	return - 6;
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

int CmdHandle::GetSoftVersion(unsigned long *softVer,unsigned long *halVer,char *inbootflag)
{
		if (hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_GET_VER)
		{
			if(softVer != NULL)
			  memcpy(softVer, lastReceivePacket.data, 4);
			if (halVer != NULL)
			  memcpy(halVer, lastReceivePacket.data + 4, 4);
			if (inbootflag != NULL)
			  *inbootflag = lastReceivePacket.data[8];
				return 0;
		}
		else if (hasActiveData[12])
		{
			if (softVer != NULL)
		      *softVer = retSoftVer;
			if (halVer != NULL)
			  *halVer = retHalVer;
			if (inbootflag != NULL)
			  *inbootflag = retInBootFlag;
				return 0;
		}
	return -1;
}


//add by llj
int CmdHandle::GetUltDisReturn(unsigned char *dis)//处理查询获取到的超声波传感器距离
{
    if((hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_EXTEND)&&(lastReceivePacket.data[1]==GET_ULTRASONIC_DIS))
    {
        *dis = lastReceivePacket.data[2];
        return 0;
    }
    else if(hasActiveData[1])
    {
        *dis = retDis;
        return 0;
    }
    return -1;
}


//add by llj
int CmdHandle::GetlineReturn(unsigned char *line_state1,unsigned char *line_state2,unsigned char *line_state3,unsigned char *line_state4)//处理查询获取到的巡线传感器状态
{
    if((hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_EXTEND)&&(lastReceivePacket.data[1]==GET_LINE_PATROL))
    {
        *line_state1 = lastReceivePacket.data[2]&0X01;
        *line_state2 = lastReceivePacket.data[2]&0X02;
        *line_state3 = lastReceivePacket.data[2]&0X04;
        *line_state4 = lastReceivePacket.data[2]&0X08;
        return 0;
    }
    else if(hasActiveData[2])
    {
    	  *line_state1 = retLinestate&0X01;
        *line_state2 = retLinestate&0X02;
        *line_state3 = retLinestate&0X04;
        *line_state4 = retLinestate&0X08;
    		return 0;
    }
    return -1;
}

//add by llj
int CmdHandle::GetBatteryStateReturn(unsigned char *battery_charger_flag,unsigned char *battery_uv_flag,unsigned char *battery_volatge)//battery_charger_flags表示充电状态，battery_uv_flag表示欠压标志battery_volatge表示电池电压乘以5
{
    if((hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_EXTEND)&&(lastReceivePacket.data[1]==GET_BATTERY_STATE))
    {
        *battery_charger_flag = lastReceivePacket.data[2]&0X80;
        *battery_uv_flag = lastReceivePacket.data[2]&0X40;
        *battery_volatge = lastReceivePacket.data[2]&0X3f;
        return 0;
    }
    else if(hasActiveData[3])
    {
    	  *battery_charger_flag = retBatterystate&0X80;
        *battery_uv_flag = retBatterystate&0X40;
        *battery_volatge = retBatterystate&0X3f;
        return 0;
    }
    return -1;
}

//add by llj
int CmdHandle::GetModeReturn(unsigned char *system_mode)//battery_charger_flags表示充电状态，battery_uv_flag表示欠压标志battery_volatge表示电池电压乘以5
{
    if((hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_EXTEND)&&(lastReceivePacket.data[1]==GET_DEVICE_MODE))
    {
        *system_mode = lastReceivePacket.data[2];
        return 0;
    }
    else if(hasActiveData[0])
    {
    	 *system_mode =retSystem_mode;
        return 0;
    }
    return -1;
}

//add by llj
int CmdHandle::GetKeyStateReturn(unsigned char *key_state)//battery_charger_flags表示充电状态，battery_uv_flag表示欠压标志battery_volatge表示电池电压乘以5
{
    if((hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_EXTEND)&&(lastReceivePacket.data[1]==GET_KEY_STATE))
    {
        *key_state = lastReceivePacket.data[2];
        return 0;
    }
    else if(hasActiveData[4])
    {
    	 *key_state = retKey_state;
        return 0;
    }
    return -1;
}

//add by llj
int CmdHandle::GetThrowStateReturn(unsigned char *throw_state)//battery_charger_flags表示充电状态，battery_uv_flag表示欠压标志battery_volatge表示电池电压乘以5
{
    if((hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_EXTEND)&&(lastReceivePacket.data[1]==GET_THROW_STATE))
    {
        *throw_state = lastReceivePacket.data[2];
        return 0;
    }
    else if(hasActiveData[5])
    {
    	 *throw_state = retThrow_state;
        return 0;
    }
    return -1;
}

int CmdHandle::GetTemperatureReturn(short *temp,short *humidity )
{
	    if((hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_EXTEND)&&(lastReceivePacket.data[1]==GET_TEMPERATURE))
    {
        *temp = lastReceivePacket.data[2] | (lastReceivePacket.data[3] << 8);
        *humidity = lastReceivePacket.data[4] | (lastReceivePacket.data[5] << 8);
        return 0;
    }
    else if(hasActiveData[6])
    {
    	  *temp = retTemperature;
        *humidity = retTumidity;
        return 0;
    }
    return -1;
}

int CmdHandle::GetLightSensorReturn(short *light)
{
		if((hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_EXTEND)&&(lastReceivePacket.data[1]==GET_LIGHT_SENSOR))
    {
        *light = lastReceivePacket.data[2] | (lastReceivePacket.data[3] << 8);
        return 0;
    }
    else if(hasActiveData[7])
    {
    	  *light = retLight;
        return 0;
    }
    return -1;
}

int CmdHandle::GetSoundSensorReturn(short *sound)
{
		if((hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_EXTEND)&&(lastReceivePacket.data[1]==GET_SOUND_SENSOR))
    {
        *sound = lastReceivePacket.data[2] | (lastReceivePacket.data[3] << 8);
        return 0;
    }
    else if(hasActiveData[8])
    {
    	  *sound = retSound;
        return 0;
    }
    return -1;
}

int CmdHandle::GetMotionSensorReturn(short *motion)
{
		if((hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_EXTEND)&&(lastReceivePacket.data[1]==GET_MOTION_SENSOR))
    {
        *motion = lastReceivePacket.data[2] | (lastReceivePacket.data[3] << 8);
        return 0;
    }
    else if(hasActiveData[9])
    {
    	  *motion = retMotion;
        return 0;
    }
    return -1;
}

int CmdHandle::GetGasSensorReturn(short *gas)
{
	 if((hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_EXTEND)&&(lastReceivePacket.data[1]==GET_GAS_SENSOR))
    {
        *gas = lastReceivePacket.data[2] | (lastReceivePacket.data[3] << 8);
        return 0;
    }
    else if(hasActiveData[9])
    {
    	  *gas = retGas;
        return 0;
    }
    return -1;
}

int CmdHandle::GetFlameSensorReturn(short *flame)
{
		if((hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_EXTEND)&&(lastReceivePacket.data[1]==GET_FLAME_SENSOR))
    {
        *flame = lastReceivePacket.data[2] | (lastReceivePacket.data[3] << 8);
        return 0;
    }
    else if(hasActiveData[9])
    {
    	  *flame = retFlame;
        return 0;
    }
    return -1;
}


int CmdHandle::CheckAckPacket(void)
{
	if (hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_ACK)
	{
		return lastReceivePacket.data[0];
	}
	return -1;
}

int CmdHandle::GetLeakPacket(unsigned char *buff, int *num)
{
	if (hasLastPacket && GET_PACKET_CMD(lastReceivePacket.cmdAndLength) == CMD_EXTEND && lastReceivePacket.data[0] == CMD_EXT_LEAK_PACKET)
	{
		int length = GET_PACKET_LENGTH(lastReceivePacket.cmdAndLength) - 1;
		if (length > 0)
		{
			memcpy(buff, lastReceivePacket.data + 1, length);
		}
		*num = length;
		return 0;
	}

	return -1;
}

int CmdHandle::PacketCheck(BootPacketStruct *p_Packet, int length)
{
	if (length < 4)
		return 0;

	int dataLength = GET_PACKET_LENGTH(p_Packet->cmdAndLength);

	if (dataLength > 16)
		return 1;

	if (dataLength + 4 > length)
		return 0;

	unsigned char *p_ckSum = (unsigned char *)(p_Packet->data + dataLength);

	if (*p_ckSum == CheckSum(p_Packet, dataLength + 3))
		return dataLength + 4;
	else
		return 1;
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

void CmdHandle::AnalysisPacket(BootPacketStruct *p_Packet)
{
    if (p_Packet->serialNum == serialNum)
    {
        memcpy(&lastReceivePacket, p_Packet, sizeof(BootPacketStruct));
        hasLastPacket = true;
    }
    
    switch(GET_PACKET_CMD(p_Packet->cmdAndLength))
    {
    	case CMD_ACK :
    	  if(p_Packet->serialNum == lastSendPacket.serialNum)
    	     repeatFlag = false;
    		break;
      case CMD_GOTO_APP:
      	break;
      case CMD_SET_ADDR :
      	break;
      case CMD_SET_DATA:
      	break;
      case CMD_GET_VER :
        hasActiveData[12] = true;
        memcpy(&this->retSoftVer,p_Packet->data,4);
        memcpy(&this->retHalVer,p_Packet->data + 4,4);
        this->retInBootFlag = p_Packet->data[8];
      	break;
      case CMD_CHECK_AND_PRO :
      	break;
      case CMD_SET_FIRMWARE_INF:
      	break;
      case CMD_EXTEND:
      	AnalysisExtPacket(p_Packet);
      	break;
    }
}

void CmdHandle::AnalysisExtPacket(BootPacketStruct *p_Packet)
	{
		switch(p_Packet->data[0])
		{
			case CMD_EXT_DATA_NOACK  :
				break;
      case CMD_EXT_DATA_HASACK :
      	break;
      case CMD_EXT_LEAK_PACKET  :
      	break;
      case CMD_EXT_SET         :
      	break;
      case CMD_EXT_GET   :
            AnalysisExtGetPacket(p_Packet);
      	break;
		}
	}


void CmdHandle::AnalysisExtGetPacket(BootPacketStruct *p_Packet)
	{
		switch(p_Packet->data[1])
		{
			case GET_DEVICE_MODE :
				retSystem_mode = p_Packet->data[2];
				hasActiveData[0] = true;
				break;
      case GET_ULTRASONIC_DIS :
      	retDis = p_Packet->data[2];
      	hasActiveData[1] = true;
      	break;
      case GET_LINE_PATROL :
      	retLinestate = p_Packet->data[2];
      	hasActiveData[2] = true;
      	break;
      case GET_GYRO:
      	break;
      case GET_BATTERY_STATE:
      	retBatterystate = p_Packet->data[2];
      	hasActiveData[3] = true;
        break;
      case GET_KEY_STATE  :
      	retKey_state = p_Packet->data[2];
      	hasActiveData[4] = true;
        break;
      case GET_THROW_STATE :
      	retThrow_state = p_Packet->data[2];
      	hasActiveData[5] = true;
      	break;
      case GET_TEMPERATURE:
      	retTemperature = p_Packet->data[2] | (p_Packet->data[3] << 8);
        retTumidity = p_Packet->data[4] | (p_Packet->data[5] << 8);
        hasActiveData[6] = true;
      	break;
      case GET_LIGHT_SENSOR :
        retLight = p_Packet->data[2] | (p_Packet->data[3] << 8);
        hasActiveData[7] = true;
      	break;
      case GET_SOUND_SENSOR :
      	retSound = p_Packet->data[2] | (p_Packet->data[3] << 8);
        hasActiveData[8] = true;
      	break;
      case GET_MOTION_SENSOR :
      	retMotion = p_Packet->data[2] | (p_Packet->data[3] << 8);
        hasActiveData[9] = true;
      	break;
      case GET_GAS_SENSOR :
      	retGas = p_Packet->data[2] | (p_Packet->data[3] << 8);
        hasActiveData[10] = true;
      	break;
      case GET_FLAME_SENSOR :
      	retFlame = p_Packet->data[2] | (p_Packet->data[3] << 8);
        hasActiveData[11] = true;
        break;
      case GET_ALL_STATE:
        retSystem_mode = p_Packet->data[2];
        retDis = p_Packet->data[3];
        retLinestate = p_Packet->data[4];
        retBatterystate = p_Packet->data[5];
        retKey_state = p_Packet->data[6];
        retThrow_state = p_Packet->data[7];
        for(int i = 0 ; i < 6 ; i ++)
          hasActiveData[i] = true;
      	break;
		}
	}

void CmdHandle::SetNeedAckPacket(BootPacketStruct *p_Packet)
{
   memcpy(&lastSendPacket,p_Packet,sizeof(BootPacketStruct));
   repeatFlag = true;
   time10msCount = 0;
}
