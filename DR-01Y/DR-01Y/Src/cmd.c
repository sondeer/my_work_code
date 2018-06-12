
#include "cmd.h"
#include <string.h>

#define ULTRASONIC_SENSOR      0x01
#define RGBLED                 0x08
#define TONE                   0x22
#define COMMON_COMMONCMD       0x3c
#define ENCODER_BOARD          0x3D

#define ISKYU_RGBLED        0x64
#define ISKYU_RGBLED_COLOR  0x65
#define ISKYU_JOYSTICK      0x66
#define ISKYU_JOYSTICK_A    0x67
#define ISKYU_RGBLED_MANUAL 0x68
#define ISKYU_BUTTON        0x70
#define ISKYU_MOTION        0x71
#define ISKYU_LINE          0x72
#define ISKYU_OBS_ON_OFF    116

#define ISKYU_GYRO          0x78

#define ISKYU_ROTATE_ANGLE     0x80
#define ISKYU_MOVE_DISTANCE    0x81
#define ISKYU_ROTATE_ANGLE_A   0x82
#define ISKYU_MOVE_DISTANCE_A  0x83
#define ISKYU_SET_AUTOMATIC_SPD 0x84

#define GET 1
#define RUN 2
#define RESET 4
#define START 5


typedef struct
{
	BootPacketStruct sendPacket[8];
	int size[8];
	int head;
	int tail;
}SendPacketBuff;

static SendPacketBuff sendBuff;


static uint8_t _dataBuffer[DATA_BUFF_SIZE];
static int _dataInBuff;
static void CmdAnalysisPacket(BootPacketStruct *p_Packet);
static void AddAckPacket(void);
//static int _dataPacketNum;


static int readSensor(BootPacketStruct *p_Packet);
static int runModule(BootPacketStruct *p_Packet);


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

            for (; findIndex < _dataInBuff - 1; findIndex++) {
                // Debug_Printf("%x ",this->_dataBuff[findIndex]);
                if (_dataBuffer[findIndex] == 0xff && _dataBuffer[findIndex + 1] == 0x55)
                    break;
            }

            //Debug_Printf("\nfindIndex = %d\n",findIndex);
            if (findIndex >= _dataInBuff - 1) {
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
  int size = sendBuff.size[sendBuff.tail];
	memcpy(buff,sendBuff.sendPacket + sendBuff.tail,size);
	sendBuff.tail ++;
	if(sendBuff.tail == 8)
		sendBuff.tail = 0;
	return size;
}

int CmdPacketCheck(BootPacketStruct *p_Packet,int length)
{
	  if (length < 4)
        return 0;
		
		int dataLength = p_Packet->dataLength;

    if (dataLength > 16)
        return 1;

    if (dataLength + 3 > length)
        return 0;
		
    return  dataLength + 3;
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
	switch(p_Packet->action)
	{
		case GET:
      {
        readSensor(p_Packet);
      }
      break;
    case RUN:
      {
        runModule(p_Packet);
				AddAckPacket();
      }
      break;
    case RESET:
      {
     
      }
      break;
    case START:
      {

      }
      break;
	}
}

int readSensor(BootPacketStruct *p_Packet)
{
	switch(p_Packet->device)
	{
		case ULTRASONIC_SENSOR:
			break;
		case ISKYU_BUTTON:
			break;
    case ISKYU_MOTION:
			break;
    case ISKYU_LINE:
			break;
    case ISKYU_OBS_ON_OFF :
			break;
    case ISKYU_GYRO:
			break;
	}
	return 0;
}

static int runModule(BootPacketStruct *p_Packet)
{
	switch(p_Packet->device)
	{
		case ENCODER_BOARD:
			if(p_Packet->port == 0)
			{
				uint8_t slot = p_Packet->data[0];
				uint8_t speed_value = p_Packet->data[1];
			}
			break;
	  case RGBLED:
		{
			uint8_t slot = p_Packet->data[0];
			uint8_t index = p_Packet->data[1];
			uint8_t r = p_Packet->data[2];
			uint8_t g = p_Packet->data[3];
			uint8_t b = p_Packet->data[4];
			
		}
			break;
		case ISKYU_RGBLED:
		{
			uint8_t slot = p_Packet->data[0];
			uint8_t index = p_Packet->data[1];
			uint8_t h = p_Packet->data[2];
			uint8_t s = p_Packet->data[3];
			uint8_t v = p_Packet->data[4];
			
		}
			break;
		case ISKYU_RGBLED_COLOR:
		{
			uint8_t h = p_Packet->data[0];
			uint8_t s = p_Packet->data[1];
			uint8_t v = p_Packet->data[2];
		}
			break;
		case ISKYU_RGBLED_MANUAL:
		{
			uint8_t manual_h = p_Packet->data[0];
      uint8_t manual_l = p_Packet->data[1];
		}
			break;
		case ISKYU_JOYSTICK:
		if(p_Packet->port == 0)
		{
			int16_t joy_x;
      int16_t joy_y;
			memcpy(&joy_x,p_Packet->data,2);
			memcpy(&joy_y,p_Packet->data + 2,2);
		}
			break;
    case ISKYU_JOYSTICK_A:
		{
			int16_t joy_x;
      int16_t joy_y;
			memcpy(&joy_x,p_Packet->data,2);
			memcpy(&joy_y,p_Packet->data + 2,2);
		}
      break;		
    case ISKYU_ROTATE_ANGLE :
			if(p_Packet->port == 0)
			{
				int16_t angleSpeed;
        int16_t tagAngle;
			  memcpy(&angleSpeed,p_Packet->data,2);
			  memcpy(&tagAngle,p_Packet->data + 2,2);
			}
			break;
    case ISKYU_MOVE_DISTANCE:
			if(p_Packet->port == 0)
			{
				int16_t tagSpeed;
        int16_t tagDis;
			  memcpy(&tagSpeed,p_Packet->data,2);
			  memcpy(&tagDis,p_Packet->data + 2,2);
			}
			break;
    case ISKYU_ROTATE_ANGLE_A:
			if(p_Packet->port == 0)
			{
				int16_t angleSpeed;
        int16_t tagAngle;
			  memcpy(&angleSpeed,p_Packet->data,2);
			  memcpy(&tagAngle,p_Packet->data + 2,2);
			}
			break;
    case ISKYU_MOVE_DISTANCE_A :
			if(p_Packet->port == 0)
			{
				int16_t tagSpeed;
        int16_t tagDis;
			  memcpy(&tagSpeed,p_Packet->data,2);
			  memcpy(&tagDis,p_Packet->data + 2,2);
			}
			break;
    case ISKYU_SET_AUTOMATIC_SPD:
		{
			int16_t speed;
			memcpy(&speed,p_Packet->data,2);
		}
      break;			
		case TONE :
		{
			int16_t hz,ms;
			memcpy(&hz,p_Packet->data,2);
			memcpy(&ms,p_Packet->data + 2,2);
		}
			break;
    case COMMON_COMMONCMD:
		{
			uint8_t cmd_data = p_Packet->data[0];
		}
			break;
		
	}
	return 0;
}

void AddAckPacket(void)
{
	BootPacketStruct *p_Packet = sendBuff.sendPacket + sendBuff.head;

	p_Packet->header1 = 0xff;
	p_Packet->header2 = 0x55;
	p_Packet->dataLength = 0x0d;
	p_Packet->idx = 0x0a;
	sendBuff.size[sendBuff.head] = 4;
	
	sendBuff.head ++;
	if(sendBuff.head == 8)
		sendBuff.head = 0;
}

int  CmdAddSendPacket(uint8_t cmd,uint8_t sNum,uint8_t *data,int length)
{
	BootPacketStruct *p_Packet = sendBuff.sendPacket + sendBuff.head;
	sendBuff.head ++;
	if(sendBuff.head == 8)
		sendBuff.head = 0;

	
	return length + 4;
}
