// IbusMessageLib.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "IbusMessageLib.h"
#include "ibusmessage.h"
#include "debug.h"


IbusMessage ibusMeg;
// 这是导出函数的一个示例。

IBUSMESSAGELIB_API void Update(void*buff,int size)
{
	//Debug_Printf("Update\n");
	ibusMeg.update((uint8_t*)buff, size);
}

IBUSMESSAGELIB_API void Set_retry_rates(int times, int timeOut)
{
	ibusMeg.set_retry_rates(times, timeOut);
}

IBUSMESSAGELIB_API int  Update_ticks(int timeMs)
{
	return ibusMeg.update_ticks(timeMs);
}

IBUSMESSAGELIB_API int  Get_data_readysend(void*buff, int size)
{
	return ibusMeg.get_data_readysend(buff, size);
}

IBUSMESSAGELIB_API int  Get_ibus_ret(int id)
{
	return ibusMeg.get_ibus_ret(id);
}

IBUSMESSAGELIB_API int  Set_plane_time(unsigned int time)
{
	return ibusMeg.set_plane_time(time);
}

IBUSMESSAGELIB_API int  Set_arm_disarm(int armflag)
{
	return ibusMeg.set_arm_disarm(armflag);
}

IBUSMESSAGELIB_API int  Set_plane_home(double lat, double lng, float alt)
{
	return ibusMeg.set_plane_home(lat, lng, alt);
}

IBUSMESSAGELIB_API int  Set_plane_mode(int mode)
{
	return ibusMeg.set_plane_mode(mode);
}

IBUSMESSAGELIB_API int  Set_plane_land(void)
{
	return ibusMeg.set_plane_land();
}

IBUSMESSAGELIB_API int  Set_plane_rtl(void)
{
	return ibusMeg.set_plane_rtl();
}

IBUSMESSAGELIB_API int  Set_plane_takeoff(float alt)
{
	return ibusMeg.set_plane_takeoff(alt);
}

IBUSMESSAGELIB_API int  Send_write_tastitem(mission_item_struct_t *item)
{
	return ibusMeg.send_write_tastitem(item);
}

IBUSMESSAGELIB_API int  Send_read_tastitem(int pointIndex)
{
	return ibusMeg.send_read_tastitem(pointIndex);
}

IBUSMESSAGELIB_API int  Get_ret_tastitem(int id, mission_item_struct_t *item)
{ 
	return ibusMeg.get_ret_tastitem(id, item);
}

IBUSMESSAGELIB_API int  Send_tastitem_takeoff(unsigned short allPoint, unsigned short pointIndex, float alt)
{
	return ibusMeg.send_tastitem_takeoff(allPoint, pointIndex,alt);
}

IBUSMESSAGELIB_API int  Send_tastitem_land(unsigned short allPoint, unsigned short pointIndex)
{
	return ibusMeg.send_tastitem_land(allPoint, pointIndex);
}

IBUSMESSAGELIB_API int  Send_tastitem_rtl(unsigned short allPoint, unsigned short pointIndex)
{
	return ibusMeg.send_tastitem_rtl(allPoint, pointIndex);
}

IBUSMESSAGELIB_API int  Send_tastitem_circle(unsigned short allPoint, unsigned short pointIndex, double lat, double lon, float alt, short r, unsigned short times)
{
	return ibusMeg.send_tastitem_circle(allPoint, pointIndex, lat,lon,alt,r,times);
}

IBUSMESSAGELIB_API int  Send_tastitem_waypoint(unsigned short allPoint, unsigned short pointIndex, double lat, double lon, float alt, unsigned short delayS)
{
	return ibusMeg.send_tastitem_waypoint(allPoint, pointIndex, lat, lon, alt, delayS);
}

IBUSMESSAGELIB_API int Send_rc_remote_code(int code)
{
	return ibusMeg.send_rc_remote_code(code);
}

IBUSMESSAGELIB_API int Send_rc_mode(int mode)
{
	return ibusMeg.send_rc_mode(mode);
}

IBUSMESSAGELIB_API int Send_rc_set_threshold(int threshold)
{
	return ibusMeg.send_rc_set_threshold(threshold);
}

IBUSMESSAGELIB_API float Get_attitude_roll(void)
{
	return ibusMeg.get_attitude_roll();
}

IBUSMESSAGELIB_API float Get_attitude_pitch(void)
{
	return ibusMeg.get_attitude_pitch();
}

IBUSMESSAGELIB_API float Get_attitude_yaw(void)
{
	return ibusMeg.get_attitude_yaw();
}

IBUSMESSAGELIB_API float Get_attitude_rollspeed(void)
{
	return ibusMeg.get_attitude_rollspeed();
}

IBUSMESSAGELIB_API float Get_attitude_pitchspeed(void)
{
	return ibusMeg.get_attitude_pitchspeed();
}

IBUSMESSAGELIB_API float Get_attitude_yawspeed(void)
{
	return ibusMeg.get_attitude_yawspeed();
}

IBUSMESSAGELIB_API int   Get_sysstate_flymode(void)
{
	return ibusMeg.get_sysstate_flymode();
}

IBUSMESSAGELIB_API int   Get_sysstate_subflymode(void)
{
	return ibusMeg.get_sysstate_subflymode();
}

IBUSMESSAGELIB_API int   Get_sysstate_stars(void)
{
	return ibusMeg.get_sysstate_stars();
}

IBUSMESSAGELIB_API double Get_sysstate_homelat(void)
{
	return ibusMeg.get_sysstate_homelat();
}

IBUSMESSAGELIB_API double Get_sysstate_homelon(void)
{
	return ibusMeg.get_sysstate_homelon();
}

IBUSMESSAGELIB_API float Get_sysstate_homealt(void)
{
	return ibusMeg.get_sysstate_homealt();
}

IBUSMESSAGELIB_API unsigned int  Get_sysstate_datetime(void)
{
	return ibusMeg.get_sysstate_datetime();
}

IBUSMESSAGELIB_API unsigned int Get_sysstate_stateflag(void)
{
	return ibusMeg.get_sysstate_stateflag();
}

IBUSMESSAGELIB_API double Get_position_lat(void)
{
	return ibusMeg.get_position_lat();
}

IBUSMESSAGELIB_API double Get_position_lon(void)
{
	return ibusMeg.get_position_lon();
}

IBUSMESSAGELIB_API float  Get_position_alt(void)
{
	return ibusMeg.get_position_alt();
}

IBUSMESSAGELIB_API float  Get_position_relative_alt(void)
{
	return ibusMeg.get_position_relative_alt();
}

IBUSMESSAGELIB_API float  Get_position_vx(void)
{
	return ibusMeg.get_position_vx();
}

IBUSMESSAGELIB_API float  Get_position_vy(void)
{
	return ibusMeg.get_position_vy();
}

IBUSMESSAGELIB_API float  Get_position_vz(void)
{
	return ibusMeg.get_position_vz();
}

IBUSMESSAGELIB_API float  Get_position_hdg(void)
{
	return ibusMeg.get_position_hdg();
}

IBUSMESSAGELIB_API int    Get_battery_remainingCapacity(void)
{
	return ibusMeg.get_battery_remainingCapacity();
}

IBUSMESSAGELIB_API float  Get_battery_voltage(void)
{
	return ibusMeg.get_battery_voltage();
}

IBUSMESSAGELIB_API float  Get_battery_current(void)
{
	return ibusMeg.get_battery_current();
}

IBUSMESSAGELIB_API int    Get_battery_remainingTime(void)
{
	return ibusMeg.get_battery_remainingTime();
}

IBUSMESSAGELIB_API int    Get_battery_remainingPercent(void)
{
	return ibusMeg.get_battery_remainingPercent();
}

IBUSMESSAGELIB_API int    Get_camera_cameramode(void)
{
	return ibusMeg.get_camera_cameramode();
}

IBUSMESSAGELIB_API float  Get_camera_camerapitch(void)
{
	return ibusMeg.get_camera_camerapitch();
}

IBUSMESSAGELIB_API int    Get_camera_remainingnum(void)
{
	return ibusMeg.get_camera_remainingnum();
}

IBUSMESSAGELIB_API int    Get_camera_remainingTime(void)
{
	return ibusMeg.get_camera_remainingTime();
}

IBUSMESSAGELIB_API float  Get_camera_remainingCapacity(void)
{
	return ibusMeg.get_camera_remainingCapacity();
}


IBUSMESSAGELIB_API int    Get_rc_mode(void)
{
	return ibusMeg.get_rc_mode();
}