using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace IbusMessageTest
{
    struct MissionItemStruct
    {
        public int total;
        public int seq;
        public int command;
        public double x;
        public double y;
        public float z;
        public float param1;
        public float param2;
        public float param3;
        public float param4;
    };
    class IbusMessageClass
    {
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void Update(byte*buff,int size);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Set_retry_rates(int times, int timeOut);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Update_ticks(int timeMs);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern int Get_data_readysend(byte* buff, int size);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Get_ibus_ret(int id);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Set_plane_time(uint time);

        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Set_arm_disarm(int armflag);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Set_plane_home(double lat, double lng, float alt);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Set_plane_mode(int mode);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Set_plane_land();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Set_plane_rtl();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Set_plane_takeoff(float alt);

        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Send_write_tastitem(ref MissionItemStruct item);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Send_read_tastitem(int pointIndex);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Get_ret_tastitem(int id, ref MissionItemStruct item);

        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Send_rc_remote_code(int code);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Send_rc_mode(int mode);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Send_rc_set_threshold(int threshold);

        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Send_tastitem_takeoff(UInt16 allPoint, UInt16 pointIndex, float alt);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Send_tastitem_land(UInt16 allPoint, UInt16 pointIndex);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Send_tastitem_rtl(UInt16 allPoint, UInt16 pointIndex);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Send_tastitem_circle(UInt16 allPoint, UInt16 pointIndex, double lat, double lon, float alt, Int16 r, UInt16 times);
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Send_tastitem_waypoint(UInt16 allPoint, UInt16 pointIndex, double lat, double lon, float alt, UInt16 delayS);

        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_attitude_roll();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_attitude_pitch();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_attitude_yaw();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_attitude_rollspeed();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_attitude_pitchspeed();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_attitude_yawspeed();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Get_sysstate_flymode();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Get_sysstate_subflymode();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Get_sysstate_stars();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern double Get_sysstate_homelat();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern double Get_sysstate_homelon();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_sysstate_homealt();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern uint Get_sysstate_datetime();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern uint Get_sysstate_stateflag();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern double Get_position_lat();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern double Get_position_lon();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_position_alt();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_position_relative_alt();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_position_vx();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_position_vy();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_position_vz();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_position_hdg();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Get_battery_remainingCapacity();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_battery_voltage();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_battery_current();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Get_battery_remainingTime();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Get_battery_remainingPercent();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Get_camera_cameramode();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_camera_camerapitch();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Get_camera_remainingnum();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Get_camera_remainingTime();
        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern float Get_camera_remainingCapacity();

        [DllImport("IbusMessageLib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int Get_rc_mode();
    }
}
