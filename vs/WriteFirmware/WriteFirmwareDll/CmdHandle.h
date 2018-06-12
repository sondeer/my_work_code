#pragma once

#pragma pack(1)

#define MAX_NUM_SIZE 16
#define DATA_BUFF_SIZE 256

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
	void Update(unsigned char *buff, int size);
	int GetReSendPacket(unsigned char *buff);
	int GenerateGotoAppPacket(void *buff);
	int GenerateSetAddrPacket(void *buff,unsigned long addr);
	int GenerateSetDataPacket(void *buff, int sNum, void *data, int length);
	int GenerateGetVerPacket(void *buff);
	int GenerateCkAndProPacket(void *buff, void *data,unsigned long num);
	int GenerateSetInfPacket(void *buff, void *data, unsigned long length, unsigned long addr,unsigned long ver);
	int GenerateSetNoAckPacket(void *buff, int num);
	int GenerateGetLeakPacket(void *buff);
	int CheckAckPacket(void *data, int length);
	unsigned long GetSoftVersion(void *data, int length);

	int GetSoftVersion(unsigned long *softVer,unsigned long *halVer = NULL,char *inbootflag = NULL); //获取版本号 softVer软件版本 halVer硬件版本  inbootflag是否处于可升级状态（如果不处于）
	int CheckAckPacket(void);
	int GetLeakPacket(unsigned char *buff, int *num);

    /*****************add by llj  SETDATA*********************/
    int SetMotorSpeed(void *buff,   char left_motor_speed , char right_motor_speed);//设置电机的速度
    int SetDeviceMode(void *buff,   unsigned  char mode ,unsigned  char obstacle_avoidance_flag );//设置小车的模式,obstacle_avoidance_flag表示是否打开了避障 1打开 0没有开
    int SetRockerSpeed(void *buff,   unsigned  char rocker_speed);//设置摇杆的速度
    int SetLedPattern(void *buff,   unsigned  char *led_buff);//设置LED点阵屏数据  由于一次设置不完，所以需要分两次设置
    int SetLedPattern2(void *buff,   unsigned  char *led_buff);
    int SetAudioIndex(void *buff,   unsigned  int audio_index);
    int SetThrowerState(void *buff,  unsigned  char port_num,unsigned int freq, unsigned  int compare);
    int SetAudioOnOff(void *buff,char flag);
    int SetSegDisplay(void *buff,char port,short value,char pointIndex);
    int SetBeepFreq(void *buff,unsigned short freq,unsigned short time);//freq  频率 time 持续的时间
    int SetEnterProgram(void *buff);//先获取数据，在发生进入编程模式，然后在升级
    int SetLanguage(void *buff,char lag);
    int SetAppPage(void *buff,char pag);
    
    /*****************add by llj  GETDATA*********************/
    int GetDeviceMode(void *buff);//获取设备模式
    int GetUltrasonicDis(void *buff);//获取超声波距离
    int GetLinePatrol(void *buff);//获取循线传感器状态
    int GetBatteryState(void *buff);//获取循线传感器状态
    int GetKeyState(void *buff);//获取按钮状态
    int GetThrowState(void *buff,unsigned char port_num);   //获取投石车状态
    int GetTemperature(void *buff,unsigned char port_num );
    int GetLightSensor(void *buff,unsigned char port_num );
    int GetSoundSensor(void *buff,unsigned char port_num );
    int GetMotionSensor(void *buff,unsigned char port_num );
    int GetGasSensor(void *buff,unsigned char port_num );
    int GetFlameSensor(void *buff,unsigned char port_num );
    int GetAllState(void *buff);    /********* 获所有状态 *********/
    /*****************add by llj  handle return*********************/
    int GetUltDisReturn(unsigned char *dis);//处理查询获取到的超声波传感器距离
    int GetlineReturn(unsigned char *line_state1,unsigned char *line_state2,unsigned char *line_state3,unsigned char *line_state4);//处理查询获取到的巡线传感器状态
    int GetBatteryStateReturn(unsigned char *battery_charger_flag,unsigned char *battery_uv_flag,unsigned char *battery_volatge);//battery_charger_flags表示充电状态，battery_uv_flag表示欠压标志battery_volatge表示电池电压乘以5
    int GetModeReturn(unsigned char *system_mode);//battery_charger_flags表示充电状态，battery_uv_flag表示欠压标志battery_volatge表示电池电压乘以5
    int GetKeyStateReturn(unsigned char *key_state);//battery_charger_flags表示充电状态，battery_uv_flag表示欠压标志battery_volatge表示电池电压乘以5
    int GetThrowStateReturn(unsigned char *throw_state);//battery_charger_flags表示充电状态，battery_uv_flag表示欠压标志battery_volatge表示电池电压乘以5
    int GetTemperatureReturn(short *temp,short *humidity );
    int GetLightSensorReturn(short *light);
    int GetSoundSensorReturn(short *sound);
    int GetMotionSensorReturn(short *motion);
    int GetGasSensorReturn(short *gas);
    int GetFlameSensorReturn(short *flame);

protected:
	int  PacketCheck(BootPacketStruct *p_Packet, int length);
	unsigned char CheckSum(void *buff, int size);
	void AnalysisPacket(BootPacketStruct *p_Packet);
	void AnalysisExtPacket(BootPacketStruct *p_Packet);
	void AnalysisExtGetPacket(BootPacketStruct *p_Packet);
	void SetNeedAckPacket(BootPacketStruct *p_Packet);
	
private:
	bool repeatFlag;
	int  time10msCount;
	unsigned char serialNum;
	unsigned char _dataBuffer[DATA_BUFF_SIZE];
	BootPacketStruct lastReceivePacket;
	BootPacketStruct lastSendPacket;
	unsigned char  retDis;
	unsigned char retLinestate;
	unsigned char retBatterystate;
	unsigned char retSystem_mode;
	unsigned char retKey_state;
	unsigned char retThrow_state;
	unsigned long retSoftVer,retHalVer;
	char   retInBootFlag;
	short retTemperature;
	short retTumidity;
	short retLight;
	short retSound;
	short retMotion;
	short retGas;
	short retFlame;
	bool  hasActiveData[16];
	bool  hasLastPacket;
	int _dataInBuff;
};

