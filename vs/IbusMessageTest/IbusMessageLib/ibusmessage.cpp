
#include "ibusmessage.h"
#include "stdafx.h"
#include "debug.h"
#include <time.h>

#define IBUS_HEADER   0xE0
#define PLANE_ADDR    0x01
#define APP_ADDR      0x02
#define RC_ADDR       0x00

#define IBUS_MSG_ID_RC_MSG      0x01
#define IBUS_MSG_ID_SYS_STATE   0x02
#define IBUS_MSG_ID_ATTITUDE    0x03
#define IBUS_MSG_ID_POSITION    0x04
#define IBUS_MSG_ID_BATTERY     0x05
#define IBUS_MSG_ID_CAMERA      0x06
#define IBUS_MSG_ID_ACK         0x10
#define IBUS_MSG_ID_STR         0x11

#define IBUS_MSG_ID_APP_HEARTBEAT       0x40
#define IBUS_MSG_ID_CMD                 0x41
#define IBUS_MSG_ID_TAST_ITEM           0x42
#define IBUS_MSG_ID_READ_TAST           0x43
#define IBUS_MSG_ID_SET_TIME            0x44
#define IBUS_MSG_ID_ARMINT              0x45
#define IBUS_MSG_ID_SET_HOME            0x46
#define IBUS_MSG_ID_SET_MODE            0x47
#define IBUS_MSG_ID_SET_TAKEOFF         0x48
#define IBUS_MSG_ID_SET_CRADLECAMERA    0x49
#define IBUS_MSG_ID_SET_RTLALT          0x4a
#define IBUS_MSG_ID_SET_FENCE           0x4b
#define IBUS_MSG_ID_START_TAST          0x4c

#define IBUS_MSG_ID_BUFF_STATE  0x7f

#define IBUS_MSG_ID_REMOTE_CODE         0x01
#define IBUS_MSG_ID_RC_MODE             0x02
#define IBUS_MSG_ID_SET_THRESHOLD       0x03


#define  CAMERA_START_VIDEO_CMD                 0xee         //开始录像
#define  CAMERA_STOP_VIDEO_CMD                  0xef         //停止录像
#define  CAMERA_START_PHOTOS_CMD                0xce         //拍照
#define  CAMERA_SETUP_VIDEO_RES_CMD             0xa1       
#define  CAMERA_SETUP_VIDEO_QUALITY_CMD         0xa2
#define  CAMERA_SETUP_DUAL_FILES_CMD            0xa3
#define  CAMERA_SETUP_TIME_LAPSE_CMD            0xa4
#define  CAMERA_SETUP_LOOP_RECORD_CMD           0xa5
#define  CAMERA_SETUP_VIDEO_STAMP_CMD           0xa7
#define  CAMERA_SETUP_EIS_CMD                   0x6b
#define  CAMERA_SETUP_PHOTO_SIZE_CMD            0x81
#define  CAMERA_SETUP_SELF_TIMER_CMD            0x87
#define  CAMERA_SETUP_BURST_RATE_CMD            0x88
#define  CAMERA_SETUP_PHOTO_STAMP_CMD           0x82
#define  CAMERA_SETUP_LONG_EXPOSURE_CMD         0x6d
#define  CAMERA_SETUP_SHARPNESS_CMD             0x83
#define  CAMERA_SETUP_WHITE_BALANCE_CMD         0x84
#define  CAMERA_SETUP_EV_CMD                    0x85
#define  CAMERA_SETUP_ISO_CMD                   0x86
#define  CAMERA_SETUP_SCENE_MODE_CMD            0x78
#define  CAMERA_SETUP_EFFECT_CMD                0x79
#define  CAMERA_SETUP_METER_CMD                 0x7a
#define  CAMERA_SETUP_STAMP_DISP_CMD            0x61
#define  CAMERA_SETUP_TV_MODE_CMD               0x70
#define  CAMERA_SETUP_LIGHT_FREQ_CMD            0x71
#define  CAMERA_SETUP_FORMAT_CMD                0x62
#define  CAMERA_SETUP_DEFAULT_CMD               0x63
#define  CAMERA_SETUP_SET_YM_CMD                0x72
#define  CAMERA_SETUP_SET_DH_CMD                0x73
#define  CAMERA_SETUP_SET_MS_CMD                0x74
#define  CAMERA_SETUP_SET_TIMELAPSE_CMD         0x75

#define  CAMERA_AUTO_PHOTOS_TIME_CMD            5         //自动拍照参数设置时间
#define  CAMERA_AUTO_PHOTOS_NUM_CMD             6         //自动拍照参数设置张数
#define  CAMERA_STOP_PHOTOS_CMD                 7         //停止连续拍照


#define  CAMERA_FORMAT_CMD                      8         //格式化

#define  CAMERA_PITCH_SET_POINT                 0xfe        //PITCH俯仰角设置
#define  CAMERA_PITCH_SET_SPEED                 0xff        //PITCH俯仰角设置


IbusMessage::IbusMessage()
{
	for (int i = 0; i < ALL_IBUS_MSG_COUNT; i++)
	{
		ibusRetryBuff[i].timesCount = -1;
	}
	this->retryRates = 500;
	this->retryTimes = 3;
	this->sequence = 0;
       _dataInBuff = 0;
}

void IbusMessage::update(uint8_t buff[], int size)
{
  int nread = size;
  int n;

  while(nread > 0)
    {
      int ret = 0;
      n = DATA_BUFF_SIZE - _dataInBuff;
      //Debug_Printf("_dataInBuff = %d\n",_dataInBuff);
       if(nread > n)
         {
           memcpy(this->_dataBuff + _dataInBuff, buff + size - nread,n);
		   _dataInBuff += n;
		   nread -= n;
         }
       else
         {
			 memcpy(this->_dataBuff + _dataInBuff, buff + size - nread, nread);
			 _dataInBuff += nread;
			 nread = 0;
         }
  
       int findIndex = 0;

       while(1)
         {

          for(; findIndex < _dataInBuff ; findIndex ++)
             {
				// Debug_Printf("%x ",this->_dataBuff[findIndex]);
               if(this->_dataBuff[findIndex] == IBUS_HEADER)
                 break;
             }

		   //Debug_Printf("\nfindIndex = %d\n",findIndex);
           if(findIndex == _dataInBuff)
             {
               break;
             }
           ibus_message_t *p_IbusMessage = (ibus_message_t *)(this->_dataBuff + findIndex);
           ret = checkIbusPacket(p_IbusMessage,_dataInBuff - findIndex);
           if(ret  == -1)
             {
			   //Debug_Printf("find failed %d\n", ret);
               break;
             }
           else if(ret <= 5)
             {
				 Debug_Printf("find failed %d\n", ret);
                findIndex += ret;
             }
           else   
             {
			   //Debug_Printf("ret = %d\n",ret);
             //  if(p_IbusMessage->destAddress == PLANE_ADDR)
               analysisIbusPacket(p_IbusMessage);
               findIndex += ret;
             }
         }
       if(findIndex == 0)
         return;

       int leftNum = _dataInBuff - findIndex;
       for(int i = 0 ; i < leftNum ; i ++)
         {
           this->_dataBuff[i] = this->_dataBuff[findIndex + i];
         }
       _dataInBuff = leftNum;
    }
}

void IbusMessage::set_retry_rates(int times, int timeOut)
{
	this->retryRates = timeOut;
	this->retryTimes = times;
}

int IbusMessage::update_ticks(int timeMs)
{
	int readySendSize = 0;
	for (int i = 0; i < ALL_IBUS_MSG_COUNT; i++)
	{
		if (ibusRetryBuff[i].timesCount >= 0 )
		{
			ibusRetryBuff[i].retryTicks -= timeMs;
			if (ibusRetryBuff[i].retryTicks <= 0)
			{
				readySendSize += ibusRetryBuff[i].packetSize;
			}
		}
	}
	return readySendSize;
}

int  IbusMessage::get_data_readysend(void*buff, int size)
{
	int readySendSize = 0;
	uint8_t *p = (uint8_t *)buff;

	for (int i = 0; i < ALL_IBUS_MSG_COUNT; i++)
	{
		if (ibusRetryBuff[i].retryTicks <= 0)
		{
			if (ibusRetryBuff[i].timesCount > 0)
			{
				memcpy(p + readySendSize, &ibusRetryBuff[i].ibusPacket, ibusRetryBuff[i].packetSize);
				readySendSize += ibusRetryBuff[i].packetSize;
				ibusRetryBuff[i].timesCount--;
				ibusRetryBuff[i].retryTicks = ibusRetryBuff[i].retryRates;

				if (ibusRetryBuff[i].needACK == 0)
				{
					ibusRetryBuff[i].retCode = 0;
					ibusRetryBuff[i].timesCount = -1;
				}

				Debug_Printf("send type = %x\n", ibusRetryBuff[i].ibusPacket.type);
			}
			else if (ibusRetryBuff[i].timesCount == 0 && ibusRetryBuff[i].retCode == IBUS_RESULT_RUNNING)
			{
				ibusRetryBuff[i].retCode = IBUS_RESULT_TIMEOUT;
				ibusRetryBuff[i].timesCount = -1;
			}
		}

	}
	return readySendSize;
}

int  IbusMessage::get_ibus_ret(int id)
{
	if (id >= ALL_IBUS_MSG_COUNT || id < 0)
		return IBUS_RESULT_IDERROR;
	int ret = ibusRetryBuff[id].retCode;
	if (ret <= 0)
	{
		ibusRetryBuff[id].retCode = 0;
		ibusRetryBuff[id].timesCount = -1;
	}
	return ret;
}

int IbusMessage::set_plane_time(uint32_t time)
{
	return sendIbusMessage(IBUS_MSG_ID_SET_TIME, &time, 4, PLANE_ADDR);
}

int  IbusMessage::set_arm_disarm(int armflag)
{
	return sendIbusMessage(IBUS_MSG_ID_ARMINT, &armflag, 1, PLANE_ADDR);
}

int  IbusMessage::set_plane_home(double lat, double lng, float alt)
{
	uint8_t buff[12];
	
	*(int*)(buff)= lat * 1E7;
	*(int*)(buff + 4) = lng * 1E7;
	*(int*)(buff + 8) = alt * 100;
	
	return sendIbusMessage(IBUS_MSG_ID_SET_HOME, buff, 12, PLANE_ADDR);
}

int  IbusMessage::set_plane_mode(int mode)
{
	return sendIbusMessage(IBUS_MSG_ID_SET_MODE, &mode, 1, PLANE_ADDR);
}

int  IbusMessage::set_plane_land(void)
{
	int mode = LAND;
	return sendIbusMessage(IBUS_MSG_ID_SET_MODE, &mode, 1, PLANE_ADDR);
}

int  IbusMessage::set_plane_rtl(void)
{
	int mode = RTL;
	return sendIbusMessage(IBUS_MSG_ID_SET_MODE, &mode, 1, PLANE_ADDR);
}

int  IbusMessage::set_plane_takeoff(float alt)
{
	float takeoff_alt = alt * 100;
	return sendIbusMessage(IBUS_MSG_ID_SET_TAKEOFF, &takeoff_alt, 4, PLANE_ADDR);
}

int  IbusMessage::set_plane_rtlaltitude(float alt)
{
	return this->sendIbusMessage(IBUS_MSG_ID_SET_RTLALT,&alt ,4 ,PLANE_ADDR);
}

int  IbusMessage::set_plane_fenceparam(uint8_t en, uint8_t enf, uint8_t ac, float altmax, float  rmax)
{
	uint8_t buff[16];
	buff[0] = en;
	buff[1] = enf;
	buff[2] = ac;
	*(float*)(buff + 3) = altmax;
	*(float*)(buff + 7) = rmax;
	return sendIbusMessage(IBUS_MSG_ID_SET_FENCE, buff,11,PLANE_ADDR);
}

int  IbusMessage::set_plane_runtast(uint8_t cmd, uint8_t index)
{
	uint8_t buff[8];
	buff[0] = cmd;
	buff[1] = index;
	return sendIbusMessage(IBUS_MSG_ID_START_TAST, buff, 2 ,PLANE_ADDR);
}

int  IbusMessage::send_write_tastitem(mission_item_struct_t *item)
{
	ibus_missionitem_struct_t missionData;

	missionData.seq = item->seq;
	missionData.command = item->command;
	missionData.x = item->x * 1E7;
	missionData.y = item->y * 1E7;
	missionData.z = item->z * 100;
	missionData.total = item->total;

	// command specific conversions from mavlink packet to mission command
	switch (item->command) {

	case MAV_CMD_NAV_WAYPOINT:                          // MAV ID: 16
														// delay at waypoint in seconds
		*(uint16_t*)missionData.variable = item->param[0];
		break;
	case MAV_CMD_NAV_LOITER_TURNS:                      // MAV ID: 18
	{
		missionData.variable[0] = item->param[0];              // param 1 is number of times to circle is held in low p1
		missionData.variable[1] = item->param[1];        // param 3 is radius in meters is held in high p1
		missionData.variable[2] = item->param[2];   // store radius in high byte of p1, num turns in low byte of p1
		break;
	}
	case MAV_CMD_NAV_RETURN_TO_LAUNCH:                  // MAV ID: 20
		break;
	case MAV_CMD_NAV_LAND:                              // MAV ID: 21
		break;
	case MAV_CMD_NAV_TAKEOFF:                           // MAV ID: 22
		break;

	default:
		// unrecognised command
		break;
	}
											   // if we got this far then it must have been succesful
	return sendIbusMessage(IBUS_MSG_ID_TAST_ITEM, &missionData, sizeof(ibus_missionitem_struct_t), PLANE_ADDR);;
}

int  IbusMessage::send_read_tastitem(int pointIndex)
{
	return sendIbusMessage(IBUS_MSG_ID_READ_TAST, &pointIndex,2, PLANE_ADDR);;
}

int  IbusMessage::get_ret_tastitem(int id, mission_item_struct_t *item)
{
	int ret = get_ibus_ret(id);

	if (ret == IBUS_RESULT_ACCEPTED)
	{
		*item = missionItem;
	}

	return ret;
}

int  IbusMessage::send_tastitem_takeoff(uint16_t allPoint, uint16_t pointIndex, float alt)
{
	ibus_missionitem_struct_t missionData;
	missionData.command = MAV_CMD_NAV_TAKEOFF;
	missionData.total = allPoint;
	missionData.seq = pointIndex;
	missionData.z = alt * 100;

	return sendIbusMessage(IBUS_MSG_ID_TAST_ITEM, &missionData, sizeof(ibus_missionitem_struct_t), PLANE_ADDR);
}

int  IbusMessage::send_tastitem_land(uint16_t allPoint, uint16_t pointIndex)
{
	ibus_missionitem_struct_t missionData;
	missionData.command = MAV_CMD_NAV_LAND;
	missionData.total = allPoint;
	missionData.seq = pointIndex;

	return sendIbusMessage(IBUS_MSG_ID_TAST_ITEM, &missionData, sizeof(ibus_missionitem_struct_t), PLANE_ADDR);
}

int  IbusMessage::send_tastitem_rtl(uint16_t allPoint, uint16_t pointIndex)
{
	ibus_missionitem_struct_t missionData;
	missionData.command = MAV_CMD_NAV_RETURN_TO_LAUNCH;
	missionData.total = allPoint;
	missionData.seq = pointIndex;

	return sendIbusMessage(IBUS_MSG_ID_TAST_ITEM, &missionData, sizeof(ibus_missionitem_struct_t), PLANE_ADDR);
}

int  IbusMessage::send_tastitem_circle(uint16_t allPoint, uint16_t pointIndex, double lat, double lon, float alt, int16_t r, uint16_t times)
{
	ibus_missionitem_struct_t missionData;
	missionData.command = MAV_CMD_NAV_LOITER_TURNS;
	missionData.total = allPoint;
	missionData.seq = pointIndex;
	missionData.x = lat * 1E7;
	missionData.y = lon * 1E7;
	missionData.z = alt * 100;
	missionData.variable[0] = r > 0 ? r : -r;
	missionData.variable[1] = times;
	missionData.variable[2] = r > 0 ? 0: 1;

	return sendIbusMessage(IBUS_MSG_ID_TAST_ITEM, &missionData, sizeof(ibus_missionitem_struct_t), PLANE_ADDR);
}

int  IbusMessage::send_tastitem_waypoint(uint16_t allPoint, uint16_t pointIndex, double lat, double lon, float alt, uint16_t delayS)
{
	ibus_missionitem_struct_t missionData;
	missionData.command = MAV_CMD_NAV_WAYPOINT;
	missionData.total = allPoint;
	missionData.seq = pointIndex;
	missionData.x = lat * 1E7;
	missionData.y = lon * 1E7;
	missionData.z = alt * 100;
	*(uint16_t*)missionData.variable = delayS;

	return sendIbusMessage(IBUS_MSG_ID_TAST_ITEM, &missionData, sizeof(ibus_missionitem_struct_t), PLANE_ADDR);
}

int IbusMessage::send_camera_photo()   // 拍照
{
	uint8_t buff[8];
	buff[0] = CAMERA_START_PHOTOS_CMD;
	buff[1] = 0x06;
	buff[2] = 0x20;
	return sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_video()   // 录像
{
	uint8_t buff[8];
	buff[0] = CAMERA_START_VIDEO_CMD;
	buff[1] = 0x06;
	buff[2] = 0x10;
	return sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_pitch(float pitch)  // 调整角度
{
	uint8_t buff[8];
	buff[0] = CAMERA_PITCH_SET_POINT;
	*(int16_t*)(buff + 1) = pitch;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_video_res(int res)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_VIDEO_RES_CMD;
	buff[1] = 0x00;
	buff[2] = res;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_video_quality(int quality)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_VIDEO_QUALITY_CMD;
	buff[1] = 0x00;
	buff[2] = quality;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_dual_files(int dualfiles)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_DUAL_FILES_CMD;
	buff[1] = 0x00;
	buff[2] = dualfiles;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_time_lapse(int timelapse)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_TIME_LAPSE_CMD;
	buff[1] = 0x00;
	buff[2] = timelapse;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_loop_record(int looprecord)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_LOOP_RECORD_CMD;
	buff[1] = 0x00;
	buff[2] = looprecord;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_video_stamp(int videostamp)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_VIDEO_STAMP_CMD;
	buff[1] = 0x00;
	buff[2] = videostamp;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_eis(int eis)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_EIS_CMD;
	buff[1] = 0x03;
	buff[2] = eis;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_photo_size(int photosize)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_PHOTO_SIZE_CMD;
	buff[1] = 0x01;
	buff[2] = photosize;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_self_timer(int selftimer)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_SELF_TIMER_CMD;
	buff[1] = 0x01;
	buff[2] = selftimer;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_burst_rate(int burstrate)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_BURST_RATE_CMD;
	buff[1] = 0x01;
	buff[2] = burstrate;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_photo_stamp(int photostamp)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_PHOTO_STAMP_CMD;
	buff[1] = 0x02;
	buff[2] = photostamp;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_long_exposure(int longexposure)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_LONG_EXPOSURE_CMD;
	buff[1] = 0x03;
	buff[2] = longexposure;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_sharpness(int sharpness)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_SHARPNESS_CMD;
	buff[1] = 0x03;
	buff[2] = sharpness;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_white_balance(int whitebalance)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_WHITE_BALANCE_CMD;
	buff[1] = 0x04;
	buff[2] = whitebalance;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_ev(int ev)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_EV_CMD;
	buff[1] = 0x05;
	buff[2] = ev;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_iso(int iso)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_ISO_CMD;
	buff[1] = 0x06;
	buff[2] = iso;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_scene_mode(int scenemode)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_SCENE_MODE_CMD;
	buff[1] = 0x04;
	buff[2] = scenemode;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_effect(int effect)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_EFFECT_CMD;
	buff[1] = 0x04;
	buff[2] = effect;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_meter(int meter)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_METER_CMD;
	buff[1] = 0x05;
	buff[2] = meter;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_stamp_disp(int stampdisp)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_STAMP_DISP_CMD;
	buff[1] = 0x02;
	buff[2] = stampdisp;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_tv_mode(int tvmode)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_TV_MODE_CMD;
	buff[1] = 0x04;
	buff[2] = tvmode;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_light_freq(int lightfreq)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_LIGHT_FREQ_CMD;
	buff[1] = 0x04;
	buff[2] = lightfreq;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_format()
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_FORMAT_CMD;
	buff[1] = 0x02;
	buff[2] = 0x21;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_default()
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_DEFAULT_CMD;
	buff[1] = 0x02;
	buff[2] = 0x22;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_time_ymdhms(uint8_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_SET_YM_CMD;
	buff[1] = year;
	buff[2] = month;
	buff[3] = day;
	buff[4] = hour;
	buff[5] = minute;
	buff[6] = second;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,7,PLANE_ADDR);
}

int IbusMessage::send_camera_time_ym(uint8_t year, uint8_t month)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_SET_YM_CMD;
	buff[1] = year;
	buff[2] = month;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_time_dh(uint8_t day, uint8_t hour)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_SET_DH_CMD;
	buff[1] = day;
	buff[2] = hour;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_time_ms(uint8_t minute, uint8_t second)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_SET_MS_CMD;
	buff[1] = minute;
	buff[2] = second;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_photo_timelapse(int timelapse)
{
	uint8_t buff[8];
	buff[0] = CAMERA_SETUP_SET_TIMELAPSE_CMD;
	buff[1] = 0x05;
	buff[2] = timelapse;
	return this->sendIbusMessage(IBUS_MSG_ID_SET_CRADLECAMERA,buff,3,PLANE_ADDR);
}

int IbusMessage::send_camera_remote_code()
{
	return 0;
}


int IbusMessage::send_rc_remote_code(int code)
{
	uint8_t buff[8] = { code };
	int num = this->sendIbusMessageSize(IBUS_MSG_ID_REMOTE_CODE,buff,1,RC_ADDR);
	ibusRetryBuff[num].needACK = 0;
	return num;
}

int IbusMessage::send_rc_mode(int mode)
{
	uint8_t buff[8] = { mode };
	int num = this->sendIbusMessageSize(IBUS_MSG_ID_RC_MODE,buff,1,RC_ADDR);
	ibusRetryBuff[num].needACK = 0;
	return num;
}

int IbusMessage::send_rc_set_threshold(int threshold)
{
	uint8_t buff[8] = { threshold };
	int num = this->sendIbusMessageSize(IBUS_MSG_ID_SET_THRESHOLD,buff,1,RC_ADDR);
	ibusRetryBuff[num].needACK = 0;
	return num;
}


int IbusMessage::checkIbusPacket(ibus_message_t *p_IbusMessage,int length)
{
  uint16_t *pckSum ;
  if(length < 5)
     return -1;

  if(p_IbusMessage->length > 24)
    return 1;

  if(p_IbusMessage->length + 5 > length)
     return -1;
  pckSum = (uint16_t *)(p_IbusMessage->data + p_IbusMessage->length);

  if(*pckSum == dataCheckSum(p_IbusMessage))
    return p_IbusMessage->length + 5;
  else
    return 1;

  return -1;
}

uint16_t IbusMessage::dataCheckSum(ibus_message_t *p_IbusMessage)
{
  uint16_t ckSum = 0;
  int num = p_IbusMessage->length + 3;
  uint8_t *p = (uint8_t *)p_IbusMessage;

  for(int i = 0 ; i < num ; i ++)
    {
      ckSum += p[i];
    }
  ckSum ^= 0x4567;
  return ckSum;
}

void IbusMessage::analysisIbusPacket(ibus_message_t *p_IbusMessage)
{
	if (p_IbusMessage->type != IBUS_MSG_ID_BUFF_STATE)
	{
		
		//Debug_Printf("ibusPackedt:type = %d,length = %d,clock = %ld\n", p_IbusMessage->type,p_IbusMessage->length,clock());
	}
  
  //Debug_Printf("analysisIbusPacket:%d,%d,%d,%d,%d\n", p_IbusMessage->type, p_IbusMessage->length, p_IbusMessage->data[0], p_IbusMessage->data[1], p_IbusMessage->data[6], p_IbusMessage->data[7]);
 /* for (int i = 0; i < 24; i ++)
  { 
	  Debug_Printf("%d ", p_IbusMessage->data[i]);
  }*/

  //Debug_Printf("\n ");

  switch(p_IbusMessage->type)
  {
  case IBUS_MSG_ID_RC_MSG:
	  receiveRcMessage(p_IbusMessage);
      break;
  case IBUS_MSG_ID_SYS_STATE:
	  receiveSysstateMessage(p_IbusMessage);
	  break;
  case IBUS_MSG_ID_ATTITUDE:
	  receiveAttitudeMessage(p_IbusMessage);
      break;
  case IBUS_MSG_ID_POSITION:
	  receivePositionMessage(p_IbusMessage);
	  break;
  case IBUS_MSG_ID_BATTERY:
	  receiveBatteryMessage(p_IbusMessage);
	  break;
  case IBUS_MSG_ID_CAMERA:
	  receiveCameraMessage(p_IbusMessage);
	  break;
  case IBUS_MSG_ID_ACK:
	  receiveAckMessage(p_IbusMessage);
	  break;
  case IBUS_MSG_ID_TAST_ITEM:
	  receiveTastItemMessage(p_IbusMessage);
	  break;
  case IBUS_MSG_ID_STR:
	  receiveStringMessage(p_IbusMessage);
  case IBUS_MSG_ID_BUFF_STATE:
	  receiveRcStateMessage(p_IbusMessage);
      break;
  default:
	  //Debug_Printf("p_IbusMessage->type=%d\n", p_IbusMessage->type);
      break;
  }
}

void IbusMessage::receiveRcMessage(ibus_message_t *p_IbusMessage)
{
/*	int buff[16];
	this->rcData = *(ibus_rcdata_struct_t *)p_IbusMessage->data;
	buff[0] = p_IbusMessage->data[1] | ((p_IbusMessage->data[2] & 0x0f) << 8);
	buff[1] = ((p_IbusMessage->data[2] >> 4) & 0x0f) | (p_IbusMessage->data[3] << 4);
	buff[2] = p_IbusMessage->data[4] | ((p_IbusMessage->data[5] & 0x0f) << 8);
	buff[3] = ((p_IbusMessage->data[5] >> 4) & 0x0f) | (p_IbusMessage->data[6] << 4);

	buff[4] = p_IbusMessage->data[7] | ((p_IbusMessage->data[8] & 0x0f) << 8);
	buff[5] = ((p_IbusMessage->data[8] >> 4) & 0x0f) | (p_IbusMessage->data[9] << 4);
	buff[6] = p_IbusMessage->data[10] | ((p_IbusMessage->data[11] & 0x0f) << 8);
	
	buff[7] = rcData.d_ch1;
	buff[8] = rcData.d_ch2;
	buff[9] = rcData.d_ch3;
	buff[10] = rcData.d_ch4;
	buff[11] = rcData.d_ch5;
	buff[12] = rcData.d_ch6;
	buff[13] = rcData.d_ch7;
	buff[14] = rcData.d_ch8;
	
	
	for (int i = 0; i < 8; i++)
	{
		Debug_Printf("d_ch%d=%d,",i, buff[i]);
	}
	//Debug_Printf("\n");
	
	//this->torcdataData = *(ibus_torcdata_struct_t *)p_IbusMessage->data;

	//Debug_Printf("YAW=%d\n", torcdataData.yaw);*/
}

void IbusMessage::receiveAttitudeMessage(ibus_message_t *p_IbusMessage)
{
	this->attitudeData = *(ibus_attitude_struct_t *)p_IbusMessage->data;
}

void IbusMessage::receiveSysstateMessage(ibus_message_t *p_IbusMessage)
{
	this->sysstateData = *(ibus_sysstate_struct_t *)p_IbusMessage->data;
	/*for (int i = 0; i < p_IbusMessage->length; i++)
	{
		Debug_Printf("%x ", p_IbusMessage->data[i]);
	}
	Debug_Printf("\n");
	Debug_Printf("this->sysstateData.stars = %d\n", this->sysstateData.stars);*/
    //Debug_Printf("%x ", this->sysstateData.homelat);
}

void IbusMessage::receivePositionMessage(ibus_message_t *p_IbusMessage)
{
	this->positionData = *(ibus_position_struct_t *)p_IbusMessage->data;
}

void IbusMessage::receiveBatteryMessage(ibus_message_t *p_IbusMessage)
{
	this->batteryData = *(ibus_battery_struct_t *)p_IbusMessage->data;
}

void IbusMessage::receiveCameraMessage(ibus_message_t *p_IbusMessage)
{

}

void IbusMessage::receiveAckMessage(ibus_message_t *p_IbusMessage)
{
	ibus_ack_struct_t *p_ackstruct = (ibus_ack_struct_t *)p_IbusMessage->data;
	Debug_Printf("receive ack type:%d,exType:%d,ret:%d\n", p_ackstruct->type, p_ackstruct->exType, p_ackstruct->retCode);

	for (int i = 0; i < ALL_IBUS_MSG_COUNT; i++)
	{
		if (ibusRetryBuff[i].timesCount >= 0 && ibusRetryBuff[i].retCode == IBUS_RESULT_RUNNING && ibusRetryBuff[i].ibusPacket.type == p_ackstruct->type)
		{
			
			ibusRetryBuff[i].timesCount = 0;
			ibusRetryBuff[i].retCode = p_ackstruct->retCode;
		}
	}
}

void IbusMessage::receiveTastItemMessage(ibus_message_t *p_IbusMessage)
{
	ibus_missionitem_struct_t  *p_missionData = (ibus_missionitem_struct_t  *)p_IbusMessage->data;
	missionItem.seq = p_missionData->seq;
	missionItem.command = p_missionData->command;
	missionItem.x = p_missionData->x * 1E-7;
	missionItem.y = p_missionData->y * 1E-7;
	missionItem.z = p_missionData->z * 0.01f;
	missionItem.total = p_missionData->total;

	Debug_Printf("receiveTastItemMessage:%d,%d\n", p_missionData->seq, p_missionData->total);
	// command specific conversions from mavlink packet to mission command
	switch (p_missionData->command) {

	case MAV_CMD_NAV_WAYPOINT:                          // MAV ID: 16					
		missionItem.param[0] = *(uint16_t*)p_missionData->variable;
		break;
	case MAV_CMD_NAV_LOITER_TURNS:                      // MAV ID: 18
	{
		missionItem.param[0] = p_missionData->variable[0];              // param 1 is number of times to circle is held in low p1
		missionItem.param[1] = p_missionData->variable[1];        // param 3 is radius in meters is held in high p1
		missionItem.param[2] = p_missionData->variable[2];   // store radius in high byte of p1, num turns in low byte of p1
		break;
	}
	case MAV_CMD_NAV_RETURN_TO_LAUNCH:                  // MAV ID: 20
		break;
	case MAV_CMD_NAV_LAND:                              // MAV ID: 21
		break;
	case MAV_CMD_NAV_TAKEOFF:                           // MAV ID: 22
		break;

	default:
		// unrecognised command
		break;
	}

	for (int i = 0; i < ALL_IBUS_MSG_COUNT; i++)
	{
		if (ibusRetryBuff[i].timesCount >= 0 && ibusRetryBuff[i].retCode == IBUS_RESULT_RUNNING && ibusRetryBuff[i].ibusPacket.type == IBUS_MSG_ID_READ_TAST)
		{
			ibusRetryBuff[i].timesCount = 0;
			ibusRetryBuff[i].retCode = IBUS_RESULT_ACCEPTED;
		}
	}
}

void IbusMessage::receiveStringMessage(ibus_message_t *p_IbusMessage)
{
	Debug_Printf((char*)p_IbusMessage->data);
	Debug_Printf("\n");
}

void IbusMessage::receiveRcStateMessage(ibus_message_t *p_IbusMessage)
{
	memcpy(&rcstateData, p_IbusMessage->data, sizeof(rcstateData));
}

/*
void IbusMessage::sendAttitudeInf(void)
{
  ibus_attitude_struct_t attitudeDataStruct;
  const Vector3f &gyro = this->_ahrs->get_ins().get_gyro();

  attitudeDataStruct.roll = this->_ahrs->roll;           //0.01度
  attitudeDataStruct.pitch = this->_ahrs->pitch;          //0.01度
  attitudeDataStruct.yaw = this->_ahrs->yaw;            //0.01度
  attitudeDataStruct.rollspeed = gyro.x;      //0.01度/s
  attitudeDataStruct.pitchspeed = gyro.y;     //0.01度/s
  attitudeDataStruct.yawspeed = gyro.z;       //0.01度/s

  sendIbusMessage(IBUS_MSG_ID_ATTITUDE,&attitudeDataStruct,sizeof(ibus_attitude_struct_t),2);
}

void IbusMessage::sendSysstateInf(void)
{
  ibus_sysstate_struct_t sysstateDataStruct;
  const Location &home = _ahrs->get_home();
  time_t tloc = time(nullptr);
  struct tm *tm = localtime(&tloc);

  sysstateDataStruct.flymode = copter.get_control_mode();
  sysstateDataStruct.subflymode = 0;
  sysstateDataStruct.stars = this->_ahrs->get_gps().num_sats(0);
  sysstateDataStruct.homelat = home.lat;             //1E7
  sysstateDataStruct.homelon = home.lng;              //1E7
  sysstateDataStruct.homealt = home.alt;             //1E2
  sysstateDataStruct.year = tm->tm_year - 100;
  sysstateDataStruct.month = tm->tm_mon;
  sysstateDataStruct.day = tm->tm_mday;
  sysstateDataStruct.hour = tm->tm_hour;
  sysstateDataStruct.minute = tm->tm_min;
  sysstateDataStruct.second = tm->tm_sec;
  sysstateDataStruct.stateflag = 0;

  sendIbusMessage(IBUS_MSG_ID_SYS_STATE,&sysstateDataStruct,sizeof(ibus_sysstate_struct_t),2);
}

void IbusMessage::sendPositionInf(void)
{
  ibus_position_struct_t positionDataStruct;
  const Vector3f &vel = copter.inertial_nav.get_velocity();

  positionDataStruct.lat = copter.current_loc.lat;             //1E7
  positionDataStruct.lon = copter.current_loc.lng;             //1E7
  positionDataStruct.alt = _ahrs->get_home().alt + copter.current_loc.alt;             //1E2
  positionDataStruct.relative_alt = copter.current_loc.alt;
  positionDataStruct.vx = vel.x;
  positionDataStruct.vy = vel.y;
  positionDataStruct.vz = vel.z;
  positionDataStruct.hdg = _ahrs->yaw_sensor;

  sendIbusMessage(IBUS_MSG_ID_POSITION,&positionDataStruct,sizeof(ibus_position_struct_t),2);
}

void IbusMessage::sendBatteryInf(void)
{
  ibus_battery_struct_t batteryDataStruct;

  batteryDataStruct.remainingCapacity = 5800;             //mAh
  batteryDataStruct.voltage = copter.battery.voltage() * 1000;               //mV
  batteryDataStruct.current = copter.battery.current_amps() * 1000;             //mA
  batteryDataStruct.remainingTime = 1200;       //s
  batteryDataStruct.remainingPercent = copter.battery.capacity_remaining_pct();    //%

  sendIbusMessage(IBUS_MSG_ID_BATTERY,&batteryDataStruct,sizeof(ibus_battery_struct_t),2);
}

void IbusMessage::sendCameraInf(void)
{

}

void IbusMessage::sendTorcdataInf(void)
{
  ibus_torcdata_struct_t torcDataStruct;

  torcDataStruct.stars = this->_ahrs->get_gps().num_sats(0);
  torcDataStruct.remainingCapacity = 50;
  torcDataStruct.flymode = copter.get_control_mode();
  torcDataStruct.cameramode = 0;
  torcDataStruct.remainingnum = 1000;
  torcDataStruct.remainingTime = 6000;       //s
  torcDataStruct.relative_alt = copter.get_current_loc().alt;        //cm
  torcDataStruct.velocity = this->_ahrs->get_gps().ground_speed(0)*100;            //cm/s;
  torcDataStruct.yaw = _ahrs->yaw_sensor;                 //0.01度
  torcDataStruct.distorc = 100;       //cm
  torcDataStruct.camerapitch = 0;
  torcDataStruct.remainingflyTime = 1200;    //s

  sendIbusMessage(IBUS_MSG_ID_RC_MSG,&torcDataStruct,sizeof(ibus_torcdata_struct_t),0);
}

*/
int IbusMessage::findRetryBuffIndex(void)
{
	int ret = -1;
	for (int i = 0; i < ALL_IBUS_MSG_COUNT; i++)
	{
		if (ibusRetryBuff[i].timesCount < 0)
			return i;
		else if (ibusRetryBuff[i].retCode != IBUS_RESULT_RUNNING)
			ret = i;
	}
	return ret;
}

int IbusMessage::sendIbusMessage(uint8_t type,void *buff,uint8_t size,uint8_t desAddr)
{
	int index = findRetryBuffIndex();
	if (index < 0)
		return IBUS_RESULT_NOBUFF;

	ibus_retry_struct_t *p_sendPacket = ibusRetryBuff + index;
    uint16_t *pckSum = (uint16_t *)(p_sendPacket->ibusPacket.data + 24);
	p_sendPacket->ibusPacket.packetHead = IBUS_HEADER;
	p_sendPacket->ibusPacket.destAddress = desAddr;
	p_sendPacket->ibusPacket.srcAddress =  APP_ADDR;
	p_sendPacket->ibusPacket.type = type;
	p_sendPacket->ibusPacket.length = 24;//size;

	memcpy(p_sendPacket->ibusPacket.data + 20, &this->sequence, 4);
	this->sequence++;
    memcpy(p_sendPacket->ibusPacket.data,buff,size);
    *pckSum = dataCheckSum(&p_sendPacket->ibusPacket);

	uint8_t *p = (uint8_t *)&p_sendPacket->ibusPacket;

	p_sendPacket->retCode = IBUS_RESULT_RUNNING;
	p_sendPacket->packetSize = 29;//size + 5;
	p_sendPacket->retryTicks = 0;
	p_sendPacket->needACK = 1;
	p_sendPacket->retryRates = retryRates;
	p_sendPacket->timesCount = retryTimes;
    return 0;
}

int IbusMessage::sendIbusMessageSize(uint8_t type, void *buff, uint8_t size, uint8_t desAddr)
{
	int index = findRetryBuffIndex();
	if (index < 0)
		return IBUS_RESULT_NOBUFF;

	ibus_retry_struct_t *p_sendPacket = ibusRetryBuff + index;
	uint16_t *pckSum = (uint16_t *)(p_sendPacket->ibusPacket.data + size);
	p_sendPacket->ibusPacket.packetHead = IBUS_HEADER;
	p_sendPacket->ibusPacket.destAddress = desAddr;
	p_sendPacket->ibusPacket.srcAddress = APP_ADDR;
	p_sendPacket->ibusPacket.type = type;
	p_sendPacket->ibusPacket.length = size;//size;

	memcpy(p_sendPacket->ibusPacket.data, buff, size);
	*pckSum = dataCheckSum(&p_sendPacket->ibusPacket);

	uint8_t *p = (uint8_t *)&p_sendPacket->ibusPacket;

	p_sendPacket->retCode = IBUS_RESULT_RUNNING;
	p_sendPacket->packetSize = size + 5;
	p_sendPacket->retryTicks = 0;
	p_sendPacket->needACK = 1;
	p_sendPacket->retryRates = retryRates;
	p_sendPacket->timesCount = retryTimes;
	return 0;
}

