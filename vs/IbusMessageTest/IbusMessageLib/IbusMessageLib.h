// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 IBUSMESSAGELIB_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// IBUSMESSAGELIB_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef IBUSMESSAGELIB_EXPORTS
#define IBUSMESSAGELIB_API __declspec(dllexport)
#else
#define IBUSMESSAGELIB_API __declspec(dllimport)
#endif

#include "ibusmessage.h"

extern "C"
{
	IBUSMESSAGELIB_API void Update(void *buff,int size);
	IBUSMESSAGELIB_API void Set_retry_rates(int times, int timeOut);
	IBUSMESSAGELIB_API int  Update_ticks(int timeMs);
	IBUSMESSAGELIB_API int  Get_data_readysend(void*buff, int size);
	IBUSMESSAGELIB_API int  Get_ibus_ret(int id);
	IBUSMESSAGELIB_API int  Set_plane_time(unsigned int time);
	IBUSMESSAGELIB_API int  Set_arm_disarm(int armflag);
	IBUSMESSAGELIB_API int  Set_plane_home(double lat, double lng, float alt);
	IBUSMESSAGELIB_API int  Set_plane_mode(int mode);
	IBUSMESSAGELIB_API int  Set_plane_land(void);
	IBUSMESSAGELIB_API int  Set_plane_rtl(void);
	IBUSMESSAGELIB_API int  Set_plane_takeoff(float alt);

	IBUSMESSAGELIB_API int  Send_write_tastitem(mission_item_struct_t *item);
	IBUSMESSAGELIB_API int  Send_read_tastitem(int pointIndex);
	IBUSMESSAGELIB_API int  Get_ret_tastitem(int id, mission_item_struct_t *item);

	IBUSMESSAGELIB_API int  Send_tastitem_takeoff(unsigned short allPoint, unsigned short pointIndex, float alt);
	IBUSMESSAGELIB_API int  Send_tastitem_land(unsigned short allPoint, unsigned short pointIndex);
	IBUSMESSAGELIB_API int  Send_tastitem_rtl(unsigned short allPoint, unsigned short pointIndex);
	IBUSMESSAGELIB_API int  Send_tastitem_circle(unsigned short allPoint, unsigned short pointIndex, double lat, double lon, float alt,short r, unsigned short times);
	IBUSMESSAGELIB_API int  Send_tastitem_waypoint(unsigned short allPoint, unsigned short pointIndex, double lat, double lon, float alt, unsigned short delayS);


	IBUSMESSAGELIB_API int Send_rc_remote_code(int code);
	IBUSMESSAGELIB_API int Send_rc_mode(int mode);
	IBUSMESSAGELIB_API int Send_rc_set_threshold(int threshold);

	IBUSMESSAGELIB_API float Get_attitude_roll(void);
	IBUSMESSAGELIB_API float Get_attitude_pitch(void);
	IBUSMESSAGELIB_API float Get_attitude_yaw(void);
	IBUSMESSAGELIB_API float Get_attitude_rollspeed(void);
	IBUSMESSAGELIB_API float Get_attitude_pitchspeed(void);
	IBUSMESSAGELIB_API float Get_attitude_yawspeed(void);
	IBUSMESSAGELIB_API int   Get_sysstate_flymode(void);
	IBUSMESSAGELIB_API int   Get_sysstate_subflymode(void);
	IBUSMESSAGELIB_API int   Get_sysstate_stars(void);
	IBUSMESSAGELIB_API double Get_sysstate_homelat(void);         //1E7
	IBUSMESSAGELIB_API double Get_sysstate_homelon(void);               //1E7
	IBUSMESSAGELIB_API float Get_sysstate_homealt(void);          //1E2
	IBUSMESSAGELIB_API unsigned int  Get_sysstate_datetime(void);
	IBUSMESSAGELIB_API unsigned int Get_sysstate_stateflag(void);
	IBUSMESSAGELIB_API double Get_position_lat(void);        //1E7
	IBUSMESSAGELIB_API double Get_position_lon(void);            //1E7
	IBUSMESSAGELIB_API float  Get_position_alt(void);            //1E2
	IBUSMESSAGELIB_API float  Get_position_relative_alt(void);   //1E2
	IBUSMESSAGELIB_API float  Get_position_vx(void);
	IBUSMESSAGELIB_API float  Get_position_vy(void);
	IBUSMESSAGELIB_API float  Get_position_vz(void);
	IBUSMESSAGELIB_API float  Get_position_hdg(void);
	IBUSMESSAGELIB_API int    Get_battery_remainingCapacity(void);            //mAh
	IBUSMESSAGELIB_API float  Get_battery_voltage(void);                //mV
	IBUSMESSAGELIB_API float  Get_battery_current(void);              //mA
	IBUSMESSAGELIB_API int    Get_battery_remainingTime(void);        //s
	IBUSMESSAGELIB_API int    Get_battery_remainingPercent(void);      //%
	IBUSMESSAGELIB_API int    Get_camera_cameramode(void);
	IBUSMESSAGELIB_API float  Get_camera_camerapitch(void);
	IBUSMESSAGELIB_API int    Get_camera_remainingnum(void);
	IBUSMESSAGELIB_API int    Get_camera_remainingTime(void);        //s
	IBUSMESSAGELIB_API float  Get_camera_remainingCapacity(void);

	IBUSMESSAGELIB_API int    Get_rc_mode(void);
}


