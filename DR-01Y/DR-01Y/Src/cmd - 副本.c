
#include "cmd.h"
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

#define CMD_EXT_SET          3
#define CMD_EXT_GET          4
#define CMD_EXT_ROCKER       5


#define SET_MOTOR_SPEED      1
#define SET_MOTOR_PWM        2
#define SET_LED_RGB          3
#define SET_LED_PATTERN      4
#define SET_BUZZER_PWM       5
#define SET_ROCKER_SPEED     6
#define SET_DEVICE_MODE      7


#define GET_MOTOR_SPEED      1
#define GET_MOTOR_PWM        2
#define GET_LED_RGB          3
#define GET_LED_TUAN         4
#define GET_BUZZER_PWM       5
#define GET_ROCKER_SPEED     6
#define GET_DEVICE_MODE      7

#define GET_ULTRASONIC_DIS   0x10
#define GET_LINE_PATROL      0x11
#define GET_GYRO             0x12


typedef struct
{
	BootPacketStruct sendPacket[8];
	int head;
	int tail;
}SendPacketBuff;

static SendPacketBuff sendBuff;


static uint8_t _dataBuffer[DATA_BUFF_SIZE];
static int _dataInBuff;
//static int _dataPacketNum;


static int  CmdPacketCheck(BootPacketStruct *p_Packet,int length);
static uint8_t CmdCheckSum(void *buff,int size);
static void CmdAnalysisPacket(BootPacketStruct *p_Packet);
static void CmdAnalysisExtPacket(BootPacketStruct *p_Packet);
static int  CmdAddSendPacket(uint8_t cmd,uint8_t sNum,uint8_t *data,int length);
//static void CmdInitLeakPacketFlag(int num);
//static void CmdDeleteLeakPacketFlag(int index);
//static int  CmdGetLeakPacketIndex(uint8_t *buff,int size);	
static uint8_t CmdAnalysisSetPacket(BootPacketStruct *p_Packet);
static void CmdAnalysisGetPacket(BootPacketStruct *p_Packet);

void CmdInit(void)
{
	_dataInBuff = 0;
	sendBuff.head = sendBuff.tail = 0;
}

void CmdUpdate(uint8_t *buff,int size)
{
    int nread = size;
    int n;

    while (nread > 0) {
        int ret = 0;
        n = DATA_BUFF_SIZE - _dataInBuff;
        //Debug_Printf("_dataInBuff = %d\n",_dataInBuff);
        if (nread > n) {
            memcpy(_dataBuffer + _dataInBuff, buff + size - nread, n);
            _dataInBuff += n;
            nread -= n;
        }
        else {
            memcpy(_dataBuffer + _dataInBuff, buff + size - nread, nread);
            _dataInBuff += nread;
            nread = 0;
        }

        int findIndex = 0;

        while (1) {

            for (; findIndex < _dataInBuff; findIndex++) {
                // Debug_Printf("%x ",this->_dataBuff[findIndex]);
                if (_dataBuffer[findIndex] == PACKET_HEADER)
                    break;
            }

            //Debug_Printf("\nfindIndex = %d\n",findIndex);
            if (findIndex == _dataInBuff) {
                break;
            }
            BootPacketStruct *p_Packet = (BootPacketStruct *) (_dataBuffer + findIndex);
            ret = CmdPacketCheck(p_Packet, _dataInBuff - findIndex);
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
                CmdAnalysisPacket(p_Packet);
                findIndex += ret;
            }
        }
        if (findIndex == 0)
            return;

        int leftNum = _dataInBuff - findIndex;
        for (int i = 0; i < leftNum; i++) {
            _dataBuffer[i] = _dataBuffer[findIndex + i];
        }
        _dataInBuff = leftNum;
    }
}

int  CmdGetSendData(uint8_t *buff)
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

int CmdPacketCheck(BootPacketStruct *p_Packet,int length)
{
	  if (length < 4)
        return 0;
		
		int dataLength = GET_PACKET_LENGTH(p_Packet->cmdAndLength);

    if (dataLength > 16)
        return 1;

    if (dataLength + 4 > length)
        return 0;
		
    uint8_t *p_ckSum = (uint8_t *) (p_Packet->data + dataLength);

    if (*p_ckSum == CmdCheckSum(p_Packet,dataLength + 3))
        return dataLength + 4;
    else
        return 1;
}

uint8_t CmdCheckSum(void *buff,int size)
{
	uint8_t sum = 0;
	uint8_t *p = (uint8_t *)buff;
	for(int i = 0 ; i < size ; i ++)
	{
		sum ^= p[i] ;
	}
	return sum;
}

void CmdAnalysisPacket(BootPacketStruct *p_Packet)
{
	//uint8_t dataLength = GET_PACKET_LENGTH(p_Packet->cmdAndLength);
	uint8_t cmd = GET_PACKET_CMD(p_Packet->cmdAndLength);
	switch(cmd)
	{
		case CMD_ACK:
			break;
    case CMD_GOTO_APP:
			break;
    case CMD_SET_ADDR:
			break;
    case CMD_SET_DATA:
			break;
    case CMD_GET_VER:
			break;
    case CMD_CHECK_AND_PRO: 
			break;
		case CMD_SET_FIRMWARE_INF:
			break;
		case CMD_EXTEND:
		{
			CmdAnalysisExtPacket(p_Packet);
		}
		  break;
	}
}

void CmdAnalysisExtPacket(BootPacketStruct *p_Packet)
{
	switch(p_Packet->data[0])
	{
		case CMD_EXT_DATA_NOACK:
			break;
		case CMD_EXT_DATA_HASACK:
			break;
		case CMD_EXT_LEAK_PACKET:
			break;
		case CMD_EXT_SET :
		{
			uint8_t ack = CmdAnalysisSetPacket(p_Packet);
			CmdAddSendPacket(CMD_ACK,p_Packet->serialNum,&ack,1);
		}
			break;
    case CMD_EXT_GET :
			CmdAnalysisGetPacket(p_Packet);
			break;
    case CMD_EXT_ROCKER :
			break;
	}
}

uint8_t CmdAnalysisSetPacket(BootPacketStruct *p_Packet)
{
	uint8_t ret = 0;
	switch(p_Packet->data[1])
	{
		case SET_MOTOR_SPEED:
		{
			int8_t motorSpeed1 = p_Packet->data[2];
		  int8_t motorSpeed2 = p_Packet->data[3];
			ret = 0;
		  //SetMotorSpeed(motorSpeed1,motorSpeed2);
		}
			break;
		case SET_MOTOR_PWM:
		{
			int8_t motorPwm1 = p_Packet->data[2];
			int8_t motorPwm2 = p_Packet->data[3];
			ret = 0;
			//SetMotorPwm(motorPwm1,motorPwm2);
		}
			break;
		case SET_LED_RGB:
		{
			uint8_t ledIndex = p_Packet->data[2];
			uint8_t ledNum = p_Packet->data[3];
			for(int i = 0 ; i < ledNum ; i ++)
			{
				//SetLedRgb(i + ledNum,p_Packet->data[4 + i * 3],p_Packet->data[5 + i * 3],p_Packet->data[6 + i * 3]);
			}
			ret = 0;
		}
			break;
		case SET_LED_PATTERN:
		{
			uint8_t patternIndex = p_Packet->data[2];
			//SetLedPattern(patternIndex,p_Packet->data[3],p_Packet->data[4],p_Packet->data[5]);
			ret = 0;
		}
			break;
		case SET_BUZZER_PWM:
		{
			uint16_t buzzerPwm = p_Packet->data[2] | (p_Packet->data[3] << 8);
      //SetBuzzerPwm(buzzerPwm);	
      ret = 0;			
		}
			break;
		case SET_ROCKER_SPEED:
		{
			uint8_t rockerSpeed = p_Packet->data[2] ;
			//SetRockerSpeed(rockerSpeed);
			ret = 0;
		}
			break;
		case SET_DEVICE_MODE:
		{
			uint8_t mode = p_Packet->data[2] ;       //0:三轮车1:平衡车
			uint8_t autoFunc = p_Packet->data[3] ;   //0:普通模式1:壁障模式2:漫游模式3:巡线模式
			ret = 0;
		}
			break;
		default:
			ret = 1;
			break;
	}
	return ret;
}

void CmdAnalysisGetPacket(BootPacketStruct *p_Packet)
{
	uint8_t retBuff[16];
	retBuff[0] = p_Packet->data[0];
	retBuff[1] = p_Packet->data[1];
	switch(p_Packet->data[1])
	{
		case GET_MOTOR_SPEED:
		{
		  //retBuff[2] = motorSpeed1;
			//retBuff[3] = motorSpeed2;
			CmdAddSendPacket(CMD_EXTEND,p_Packet->serialNum,retBuff,4);
		}
			break;
    case GET_MOTOR_PWM:
		{
		  //retBuff[2] = motorPwm1;
			//retBuff[3] = motorPwm2;
			CmdAddSendPacket(CMD_EXTEND,p_Packet->serialNum,retBuff,4);
		}
			break;
    case GET_LED_RGB:
		{
			uint8_t ledIndex = p_Packet->data[2];
			uint8_t ledNum = p_Packet->data[3];
		  retBuff[2] = p_Packet->data[2];
	    retBuff[3] = p_Packet->data[3];
			
			for(int i = 0 ; i < ledNum ; i ++)
			{
				//SetLedRgb(i + ledNum,p_Packet->data[4 + i * 3],p_Packet->data[5 + i * 3],p_Packet->data[6 + i * 3]);
			}
			CmdAddSendPacket(CMD_EXTEND,p_Packet->serialNum,retBuff,4 + ledNum * 3);
		}
			break;
    case GET_LED_TUAN:
		{
			//retBuff[2] = patternIndex;
			CmdAddSendPacket(CMD_EXTEND,p_Packet->serialNum,retBuff,3);
		}
			break;
    case GET_BUZZER_PWM:		
		{
			//retBuff[2] = buzzerPwm & 0xff;
			//retBuff[3] = buzzerPwm >> 8;
			CmdAddSendPacket(CMD_EXTEND,p_Packet->serialNum,retBuff,4);
		}
			break;
    case GET_ROCKER_SPEED:
		{
			//retBuff[2] = rockerSpeed;
			CmdAddSendPacket(CMD_EXTEND,p_Packet->serialNum,retBuff,3);
		}
			break;
    case GET_DEVICE_MODE:
		{
			//retBuff[2] = mode;
			//retBuff[3] = autoFunc;
			CmdAddSendPacket(CMD_EXTEND,p_Packet->serialNum,retBuff,4);
		}	
			break;
    case GET_ULTRASONIC_DIS:
		{
			//retBuff[2] = dis & 0xff;  // mm低位
			//retBuff[3] = dis >> 8;    // mm高位
			CmdAddSendPacket(CMD_EXTEND,p_Packet->serialNum,retBuff,4);
		}	
			break;
    case GET_LINE_PATROL:
		{
			//retBuff[2] = patrol1;  // 巡线1
			//retBuff[3] = patrol2;    // 巡线2
			CmdAddSendPacket(CMD_EXTEND,p_Packet->serialNum,retBuff,4);
		}
			break;
    case GET_GYRO:
		{
			//memcpy(retBuff + 2 , gyro_x , 4);     //x轴角度
			//memcpy(retBuff + 6 , gyro_y , 4);     //y轴角度
			//memcpy(retBuff + 10 , gyro_z , 4);    //z轴角度
			CmdAddSendPacket(CMD_EXTEND,p_Packet->serialNum,retBuff,14);
		}
			break;
		default:
			break;
	}
}

int  CmdAddSendPacket(uint8_t cmd,uint8_t sNum,uint8_t *data,int length)
{
	BootPacketStruct *p_Packet = sendBuff.sendPacket + sendBuff.head;
	sendBuff.head ++;
	if(sendBuff.head == 8)
		sendBuff.head = 0;
	p_Packet->header = PACKET_HEADER;
	p_Packet->serialNum = sNum;
	p_Packet->cmdAndLength = GEN_PACKET_CMDLENGTH(cmd,length) ;
	memcpy(p_Packet->data,data,length);
	p_Packet->data[length] = CmdCheckSum(p_Packet,length + 3);
	
	return length + 4;
}
