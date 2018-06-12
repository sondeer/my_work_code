using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Globalization;
using GMap.NET.WindowsForms;
using GMap.NET;

namespace IbusMessageTest
{
    public partial class MyMap : GMapControl
    {
        /// <summary>
        /// custom map of GMapControl
        /// </summary>
        private const string BAIDU_LBS_TYPE = "bd09ll";       
        private const double pi = 3.1415926535897932384626;
        private const double a = 6378245.0;
        private const double ee = 0.00669342162296594323;


        //private int counter;
        //readonly Font DebugFont = new Font(FontFamily.GenericSansSerif, 14, FontStyle.Regular);

        /// <summary>
        /// any custom drawing here
        /// </summary>
        /// <param name="drawingContext"></param>
        protected override void OnPaintOverlays(System.Drawing.Graphics g)
        {
            base.OnPaintOverlays(g);

             //g.DrawString(string.Format(CultureInfo.InvariantCulture, "{0:0.0}", Zoom) + "z, " + MapProvider + ", refresh: " + counter++ + ", load: " + ElapsedMilliseconds + "ms, render: " + delta + "ms", DebugFont, Brushes.Blue, DebugFont.Height, DebugFont.Height + 20);

            //g.DrawString(ViewAreaPixel.Location.ToString(), DebugFontSmall, Brushes.Blue, DebugFontSmall.Height, DebugFontSmall.Height);

            //string lb = ViewAreaPixel.LeftBottom.ToString();
            //var lbs = g.MeasureString(lb, DebugFontSmall);
            //g.DrawString(lb, DebugFontSmall, Brushes.Blue, DebugFontSmall.Height, Height - DebugFontSmall.Height * 2);

            //string rb = ViewAreaPixel.RightBottom.ToString();
            //var rbs = g.MeasureString(rb, DebugFontSmall);
            //g.DrawString(rb, DebugFontSmall, Brushes.Blue, Width - rbs.Width - DebugFontSmall.Height, Height - DebugFontSmall.Height * 2);

            //string rt = ViewAreaPixel.RightTop.ToString();
            //var rts = g.MeasureString(rb, DebugFontSmall);
            //g.DrawString(rt, DebugFontSmall, Brushes.Blue, Width - rts.Width - DebugFontSmall.Height, DebugFontSmall.Height);
        }

        public static PointLatLng gps84_To_Gcj02(PointLatLng pos)
        {
            double lon = pos.Lng, lat = pos.Lat;
            if (outOfChina(pos))
            {
                  return pos;
            }
            double dLat = transformLat(lon - 105.0, lat - 35.0);
            double dLon = transformLon(lon - 105.0, lat - 35.0);
            double radLat = lat / 180.0 * pi;
            double magic = Math.Sin(radLat);
            magic = 1 - ee * magic * magic;
            double sqrtMagic = Math.Sqrt(magic);
            dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
            dLon = (dLon * 180.0) / (a / sqrtMagic * Math.Cos(radLat) * pi);
            double mgLat = lat + dLat;
            double mgLon = lon + dLon;
            return new PointLatLng(mgLat, mgLon);
        }  

        public static PointLatLng gcj_To_Gps84(PointLatLng pos)
        {
            double lon = pos.Lng, lat = pos.Lat;
            PointLatLng pos1 = transform(pos);
            double lontitude = lon * 2 - pos1.Lng;
            double latitude = lat * 2 - pos1.Lat;
            return new PointLatLng(latitude, lontitude);
        }  
        public static PointLatLng gcj02_To_Bd09(PointLatLng pos)
        {
            double gg_lon = pos.Lng, gg_lat = pos.Lat;
            double x = gg_lon, y = gg_lat;
            double z = Math.Sqrt(x * x + y * y) + 0.00002 * Math.Sin(y * pi);
            double theta = Math.Atan2(y, x) + 0.000003 * Math.Cos(x * pi);
            double bd_lon = z * Math.Cos(theta) + 0.0065;
            double bd_lat = z * Math.Sin(theta) + 0.006;
            return new PointLatLng(bd_lat, bd_lon);
        }  

 
       public static PointLatLng bd09_To_Gcj02(PointLatLng pos)
        {
            double bd_lon = pos.Lng, bd_lat = pos.Lat;
            double x = bd_lon - 0.0065, y = bd_lat - 0.006;
            double z = Math.Sqrt(x * x + y * y) - 0.00002 * Math.Sin(y * pi);
            double theta = Math.Atan2(y, x) - 0.000003 * Math.Cos(x * pi);
            double gg_lon = z * Math.Cos(theta);
            double gg_lat = z * Math.Sin(theta);
            return new PointLatLng(gg_lat, gg_lon);
        }  

        public static PointLatLng bd09_To_Gps84(PointLatLng pos)
        {
            double bd_lon = pos.Lng, bd_lat = pos.Lat;
            PointLatLng gcj02 = bd09_To_Gcj02(pos);
            PointLatLng map84 = gcj_To_Gps84(gcj02);
            return map84;
        }  
  
        public static bool outOfChina(PointLatLng pos)
        {
            double lon = pos.Lng, lat = pos.Lat;
            if (lon < 72.004 || lon > 137.8347)
                     return true;
            if (lat < 0.8293 || lat > 55.8271)
                     return true;
            return false;
        }  
 
        public static PointLatLng transform(PointLatLng pos)
        {
            double lon = pos.Lng, lat = pos.Lat;
            if (outOfChina(pos))
            {
                return pos;
            }
            double dLat = transformLat(lon - 105.0, lat - 35.0);
            double dLon = transformLon(lon - 105.0, lat - 35.0);
            double radLat = lat / 180.0 * pi;
            double magic = Math.Sin(radLat);
            magic = 1 - ee * magic * magic;
            double sqrtMagic = Math.Sqrt(magic);
            dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
            dLon = (dLon * 180.0) / (a / sqrtMagic * Math.Cos(radLat) * pi);
            double mgLat = lat + dLat;
            double mgLon = lon + dLon;
            return new PointLatLng(mgLat, mgLon);
        }  
        public static double transformLat(double x, double y)
        {
            double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * Math.Sqrt(Math.Abs(x));
            ret += (20.0 * Math.Sin(6.0 * x * pi) + 20.0 * Math.Sin(2.0 * x * pi)) * 2.0 / 3.0;
            ret += (20.0 * Math.Sin(y * pi) + 40.0 * Math.Sin(y / 3.0 * pi)) * 2.0 / 3.0;
            ret += (160.0 * Math.Sin(y / 12.0 * pi) + 320 * Math.Sin(y * pi / 30.0)) * 2.0 / 3.0;
            return ret;
        }  
        public static double transformLon(double x, double y)
        {
            double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * Math.Sqrt(Math.Abs(x));
            ret += (20.0 * Math.Sin(6.0 * x * pi) + 20.0 * Math.Sin(2.0 * x * pi)) * 2.0 / 3.0;
            ret += (20.0 * Math.Sin(x * pi) + 40.0 * Math.Sin(x / 3.0 * pi)) * 2.0 / 3.0;
            ret += (150.0 * Math.Sin(x / 12.0 * pi) + 300.0 * Math.Sin(x / 30.0 * pi)) * 2.0 / 3.0;
            return ret;
        }    
    }
}
