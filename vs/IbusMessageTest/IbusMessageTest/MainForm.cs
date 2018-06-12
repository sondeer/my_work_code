using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using System.IO.Ports;
using System.Net;
using GMap.NET.MapProviders;
using GMap.NET;
using GMap.NET.WindowsForms;
using GMap.NET.WindowsForms.Markers;
using System.Net.Sockets;

namespace IbusMessageTest
{
    public partial class MainForm : Form   
    {
        private GMapOverlay mapOverlayMarker = new GMapOverlay();
        private bool connectedPort = false;
        private bool threadClose = false;
        private SerialPort com;
        private TcpClient tcpClient;
        private NetworkStream tcpClientStream;

        private System.Windows.Forms.Timer tmr;
        public MainForm()
        {
            InitializeComponent();
            
        }
        private void MainForm_Load(object sender, EventArgs e)
        {
            this.com = new SerialPort();
            
            this.portComboBox.Items.AddRange(SerialPort.GetPortNames());
            this.portComboBox.Items.Add("TCP");
            this.portComboBox.Items.Add("UDP");
            this.portComboBox.SelectedIndex = 0;
            this.baudComboBox.Items.AddRange(new string[] { "9600", "19200", "38400", "57600", "115200" });
            this.baudComboBox.SelectedIndex = 4;        
            
            Thread ibusThread = new Thread(new ThreadStart(RunIbusThread));
            ibusThread.Start();

            tmr = new System.Windows.Forms.Timer();
            tmr.Interval = 50;
            tmr.Tick += new EventHandler(this.timeOverEvent);
            tmr.Start();

            myMap.Manager.Mode = AccessMode.ServerAndCache;
            myMap.MapProvider = GMapProviders.GoogleChinaSatelliteMap;
            myMap.Zoom = 16;
            myMap.Overlays.Add(mapOverlayMarker);
            myMap.Position = new PointLatLng(30.5908367476714,114.273437708616);

        }

        private unsafe void RunIbusThread()
        {
            while(!threadClose)
            {
                if(connectedPort)
                {
                    if (this.com.IsOpen)
                    {
                        int num = com.BytesToRead;
                        if (num > 0)
                        {
                            byte[] buff = new byte[num];
                            com.Read(buff, 0, num);
                            fixed (byte* p = buff)
                            {
                                IbusMessageClass.Update(p, num);
                            }
                        }
                        num = IbusMessageClass.Update_ticks(10);
                        if (num > 0)
                        {
                            byte[] buff = new byte[num];
                            fixed (byte* p = buff)
                            {
                                num = IbusMessageClass.Get_data_readysend(p, num);
                            }
                            this.com.Write(buff, 0, num);
                        }

                    }
                    else if (this.tcpClient.Connected)
                    {
                        byte[] buff = new byte[256];
                        int num = 0;
                        try
                        {
                            num = tcpClientStream.Read(buff, 0, 256);
                        }
                        catch(Exception ex)
                        {

                        }
                        if (num > 0)
                        {
                            fixed (byte* p = buff)
                            {
                                IbusMessageClass.Update(p, num);
                            }
                        }

                        num = IbusMessageClass.Update_ticks(10);
                        if (num > 0)
                        {
                            fixed (byte* p = buff)
                            {
                                num = IbusMessageClass.Get_data_readysend(p, 256);
                            }
                            try
                            {
                                this.tcpClientStream.Write(buff, 0, num);
                            }
                            catch (Exception ex)
                            {

                            }                        
                        }
                    }
                }
                
                Thread.Sleep(10);
            }
        }

        private void connectButton_Click(object sender, EventArgs e)
        {
            if (connectedPort == false)
            {
                if(this.portComboBox.Text == "TCP")
                {
                    this.tcpClient = new TcpClient();
                    string[] ipstr = this.ipEndpointTextBox.Text.Split(':');
                    if(ipstr.Length != 2)
                    {
                        MessageBox.Show("ip地址输入有误");
                    }

                    tcpClient.Connect(ipstr[0], Convert.ToInt32(ipstr[1]));
                    this.tcpClientStream = tcpClient.GetStream();
                }
                else
                {
                    this.portComboBox.Enabled = false;
                    this.baudComboBox.Enabled = false;                  

                    this.com.PortName = this.portComboBox.Text;
                    this.com.BaudRate = Int32.Parse(this.baudComboBox.Text);
                    this.com.Parity = Parity.None;
                    this.com.DataBits = 8;
                    this.com.StopBits = StopBits.One;
                    this.com.DtrEnable = true;
                    this.com.RtsEnable = true;
                    this.com.Open();
                }
                this.connectButton.Text = "断开连接";
                connectedPort = true;
                this.panel3.Enabled = true;
            }
            else
            {
                connectedPort = false;
                this.panel3.Enabled = false;
                if (this.portComboBox.Text == "TCP")
                {
                    tcpClient.Close();
                }
                else
                {
                    this.com.Close();
                }               
                this.portComboBox.Enabled = true;
                this.baudComboBox.Enabled = true;
                this.connectButton.Text = "连接";                
            }
            
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            threadClose = true;
            if(connectedPort)
            {
                if (this.portComboBox.Text == "TCP")
                {
                    tcpClient.Close();
                }
                else
                {
                    this.com.Close();
                }
            }
        
        }

        private void timeOverEvent(object sender, EventArgs e)
        {
            if (connectedPort == false)
                return;
            this.showRollLabel.Text = IbusMessageClass.Get_attitude_roll().ToString();
            this.showPitchLabel.Text = IbusMessageClass.Get_attitude_pitch().ToString();
            this.showYawLabel.Text = IbusMessageClass.Get_attitude_yaw().ToString();

            this.showFlymodeLabel.Text = IbusMessageClass.Get_sysstate_flymode().ToString();
            this.showSubflymodeLabel.Text = IbusMessageClass.Get_sysstate_subflymode().ToString();
            this.showStarsLabel.Text = IbusMessageClass.Get_sysstate_stars().ToString();
            this.showHomelatLabel.Text = IbusMessageClass.Get_sysstate_homelat().ToString();
            this.showHomelonLabel.Text = IbusMessageClass.Get_sysstate_homelon().ToString();
            this.showHomealtLabel.Text = IbusMessageClass.Get_sysstate_homealt().ToString();
            this.showStateflagLabel.Text = Convert.ToString(IbusMessageClass.Get_sysstate_stateflag(),16);
            uint temp = IbusMessageClass.Get_sysstate_datetime();
            string dataTimeStr;
            dataTimeStr = (temp % 60).ToString().PadLeft(2,'0');
            temp /= 60;

            dataTimeStr = (temp % 60).ToString().PadLeft(2, '0') + ':' + dataTimeStr;
            temp /= 60;

            dataTimeStr = (temp % 24).ToString().PadLeft(2, '0') + ':' + dataTimeStr;
            temp /= 24;

            dataTimeStr = (temp % 31).ToString().PadLeft(2, '0') + ' ' + dataTimeStr;
            temp /= 31;

            dataTimeStr = (temp % 12).ToString().PadLeft(2, '0') + '-' + dataTimeStr;
            temp /= 12;
            temp += 2000;

            dataTimeStr = temp.ToString().PadLeft(2, '0') + '-' + dataTimeStr;
            this.showDateTimeLabel.Text = dataTimeStr;

            this.showLatLabel.Text = IbusMessageClass.Get_position_lat().ToString();
            this.showLonLabel.Text = IbusMessageClass.Get_position_lon().ToString();
            this.showAltLabel.Text = IbusMessageClass.Get_position_alt().ToString();
            this.showRelativealtLabel.Text = IbusMessageClass.Get_position_relative_alt().ToString();

            this.showCapacityLabel.Text = IbusMessageClass.Get_battery_remainingCapacity().ToString();
            this.showVoltageLabel.Text = IbusMessageClass.Get_battery_voltage().ToString();
            this.showCurrentLabel.Text = IbusMessageClass.Get_battery_current().ToString();
            this.showPercentLabel.Text = IbusMessageClass.Get_battery_remainingPercent().ToString();
            /*    IbusMessageClass.Get_camera_cameramode();
                IbusMessageClass.Get_camera_camerapitch();
                IbusMessageClass.Get_camera_remainingnum();
                IbusMessageClass.Get_camera_remainingTime();
                IbusMessageClass.Get_camera_remainingCapacity();*/
            this.showRcModeLabel.Text = IbusMessageClass.Get_rc_mode().ToString();
        }

        private void setPlaneTimeButton_Click(object sender, EventArgs e)
        {
            DateTime nowTime = DateTime.Now;
            uint timeSeconds = (uint)(((((((nowTime.Year - 2000) * 12) + nowTime.Month) * 31 + nowTime.Day) * 24 + nowTime.Hour) * 60 + nowTime.Minute) * 60 + nowTime.Second);
            int id = IbusMessageClass.Set_plane_time(timeSeconds);
            int ret = 0;
            if(id < 0)
            {
                MessageBox.Show("同步时间错误");
            }

            do
            {
              ret = IbusMessageClass.Get_ibus_ret(id);
                Thread.Sleep(5);
            } while (ret > 0);
            if(ret < 0)
            {
                MessageBox.Show("同步时间错误:" + ret.ToString());
            }
            else if(ret == 0)
            {
                MessageBox.Show("同步时间成功:" + ret.ToString());
            }
        }

        private void myMap_MouseDown(object sender, MouseEventArgs e)
        {
            if(e.Button == MouseButtons.Left)
            {
                PointLatLng mouseLatLng = myMap.FromLocalToLatLng(e.X, e.Y);
                GMapMarker mapMarker = new GMarkerGoogle(mouseLatLng, GMarkerGoogleType.green);
                mapMarker.ToolTipText = (mapOverlayMarker.Markers.Count + 1).ToString();
                mapOverlayMarker.Markers.Add(mapMarker);
                if(mapOverlayMarker.Markers.Count >= 2)
                {
                    List<PointLatLng> listPoint = new List<PointLatLng>();
                    listPoint.Add(mapOverlayMarker.Markers[mapOverlayMarker.Markers.Count - 2].Position);
                    listPoint.Add(mouseLatLng);
                    GMapRoute gMapRoute = new GMapRoute(listPoint,"");
                    gMapRoute.Stroke.Color = Color.Yellow;
                    gMapRoute.Stroke.Width = 2;

                    mapOverlayMarker.Routes.Add(gMapRoute);
                }
                mouseLatLng = MyMap.gcj_To_Gps84(mouseLatLng);
                this.showMapLatLable.Text = mouseLatLng.Lat.ToString();
                this.showMapLonLabel.Text = mouseLatLng.Lng.ToString();
            }
        }

        private void cleanMarkerButton_Click(object sender, EventArgs e)
        {
            mapOverlayMarker.Markers.Clear();
            mapOverlayMarker.Routes.Clear();
        }

        private void setHomeButton_Click(object sender, EventArgs e)
        {
            int id,ret;
            if (mapOverlayMarker.Markers.Count > 0)
            {
                PointLatLng pos = MyMap.gcj_To_Gps84(mapOverlayMarker.Markers[0].Position);
                id = IbusMessageClass.Set_plane_home(pos.Lat, pos.Lng,0);
            }
            else
            {
                id = IbusMessageClass.Set_plane_home(0, 0, 0);
            }

            if (id < 0)
            {
                MessageBox.Show("设置home点错误");
            }

            do
            {
                ret = IbusMessageClass.Get_ibus_ret(id);
                Thread.Sleep(5);
            } while (ret > 0);

            if (ret < 0)
            {
                MessageBox.Show("设置home点错误:" + ret.ToString());
            }
            else if (ret == 0)
            {
                MessageBox.Show("设置home点成功:" + ret.ToString());
            }
        }

        private void setWayPointButton_Click(object sender, EventArgs e)
        {
            int id, ret;
            int wpCount = mapOverlayMarker.Markers.Count;

            if (wpCount == 0)
                return;

            for(int i = 0; i < wpCount; i ++)
            {
                PointLatLng pos = MyMap.gcj_To_Gps84(mapOverlayMarker.Markers[i].Position);
                id = IbusMessageClass.Send_tastitem_waypoint((ushort)wpCount, (ushort)(i+1),pos.Lat, pos.Lng, 10,0);

                if (id < 0)
                {
                    MessageBox.Show("设置航点点错误");
                }

                do
                {
                    ret = IbusMessageClass.Get_ibus_ret(id);
                    Thread.Sleep(5);
                } while (ret > 0);

                if (ret < 0)
                {
                    MessageBox.Show("设置航点错误:" + ret.ToString());
                }
                else if (ret == 0)
                {
                  //  MessageBox.Show("设置航点成功:" + ret.ToString());
                }
            }
            
        }

        private void armDisarmPlaneButton_Click(object sender, EventArgs e)
        {
            int id = IbusMessageClass.Set_arm_disarm(1);
            int ret = 0;
            if (id < 0)
            {
                MessageBox.Show("解锁失败");
            }

            do
            {
                ret = IbusMessageClass.Get_ibus_ret(id);
                Thread.Sleep(5);
            } while (ret > 0);
            if (ret < 0)
            {
                MessageBox.Show("解锁失败:" + ret.ToString());
            }
            else if (ret == 0)
            {
                MessageBox.Show("解锁成功:" + ret.ToString());
            }
        }

        private void setRtlButton_Click(object sender, EventArgs e)
        {
            int id = IbusMessageClass.Set_plane_rtl();
            int ret = 0;
            if (id < 0)
            {
                MessageBox.Show("返航命令发送失败");
            }

            do
            {
                ret = IbusMessageClass.Get_ibus_ret(id);
                Thread.Sleep(5);
            } while (ret > 0);
            if (ret < 0)
            {
                MessageBox.Show("返航命令发送失败:" + ret.ToString());
            }
            else if (ret == 0)
            {
                MessageBox.Show("返航命令发送成功:" + ret.ToString());
            }
        }

        private void setLandButton_Click(object sender, EventArgs e)
        {
            int id = IbusMessageClass.Set_plane_land();
            int ret = 0;
            if (id < 0)
            {
                MessageBox.Show("着陆命令发送失败");
            }

            do
            {
                ret = IbusMessageClass.Get_ibus_ret(id);
                Thread.Sleep(5);
            } while (ret > 0);
            if (ret < 0)
            {
                MessageBox.Show("着陆命令发送失败:" + ret.ToString());
            }
            else if (ret == 0)
            {
                MessageBox.Show("着陆命令发送成功:" + ret.ToString());
            }
        }

        private void setTakeoffButton_Click(object sender, EventArgs e)
        {
            int id = IbusMessageClass.Set_plane_takeoff(Convert.ToSingle(this.takeoffTextBox.Text));
            int ret = 0;
            if (id < 0)
            {
                MessageBox.Show("自动起飞失败");
            }

            do
            {
                ret = IbusMessageClass.Get_ibus_ret(id);
                Thread.Sleep(5);
            } while (ret > 0);
            if (ret < 0)
            {
                MessageBox.Show("自动起飞失败:" + ret.ToString());
            }
            else if (ret == 0)
            {
                MessageBox.Show("自动起飞成功:" + ret.ToString());
            }
        }

        private void readTastitemButton_Click(object sender, EventArgs e)
        {
            int id, ret,index = 1;
            int wpCount = 0;
            MissionItemStruct missionItemStruct = new MissionItemStruct();

            mapOverlayMarker.Markers.Clear();
            mapOverlayMarker.Routes.Clear();

            do
            {
                id = IbusMessageClass.Send_read_tastitem(index);

                if (id < 0)
                {
                    MessageBox.Show("设置航点点错误");
                }

                do
                {
                    ret = IbusMessageClass.Get_ret_tastitem(id, ref missionItemStruct);
                    Thread.Sleep(5);
                } while (ret > 0);

                if (ret < 0)
                {
                    MessageBox.Show("设置航点错误:" + ret.ToString());
                }
                else if (ret == 0)
                {
                    //  MessageBox.Show("设置航点成功:" + ret.ToString());
                    PointLatLng pos = new PointLatLng(missionItemStruct.x, missionItemStruct.y);
                    wpCount = missionItemStruct.total;
                    mapOverlayMarker.Markers.Add(new GMarkerGoogle(MyMap.gps84_To_Gcj02(pos), GMarkerGoogleType.green));
                    mapOverlayMarker.Markers[mapOverlayMarker.Markers.Count - 1].ToolTipText = mapOverlayMarker.Markers.Count.ToString();
                    if (mapOverlayMarker.Markers.Count >= 2)
                    {
                        List<PointLatLng> listPoint = new List<PointLatLng>();
                        listPoint.Add(mapOverlayMarker.Markers[mapOverlayMarker.Markers.Count - 2].Position);
                        listPoint.Add(mapOverlayMarker.Markers[mapOverlayMarker.Markers.Count - 1].Position);
                        
                        GMapRoute gMapRoute = new GMapRoute(listPoint, "");
                        gMapRoute.Stroke.Color = Color.Yellow;
                        gMapRoute.Stroke.Width = 2;

                        mapOverlayMarker.Routes.Add(gMapRoute); 
                    }
                    this.myMap.Refresh();
                }

            } while (index++ < wpCount);
        }

        private void portComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if(this.portComboBox.Text == "TCP" || this.portComboBox.Text == "UDP")
            {
                this.baudComboBox.Visible = false;
                this.ipEndpointTextBox.Visible = true;
                if (this.com.IsOpen)
                    this.com.Close();
            }
            else
            {
                this.baudComboBox.Visible = true;
                this.ipEndpointTextBox.Visible = false;
            }
        }

        private void circleButton_Click(object sender, EventArgs e)
        {
            Int16 r = Convert.ToInt16(this.radiusTextBox.Text);
            UInt16 n = Convert.ToUInt16(this.numTextBox.Text);
            int id;
            PointLatLng pos;
            if (mapOverlayMarker.Markers.Count > 0)
            {
                pos = MyMap.gcj_To_Gps84(mapOverlayMarker.Markers[0].Position);
                id = IbusMessageClass.Send_tastitem_circle(0,0,pos.Lat, pos.Lng, IbusMessageClass.Get_position_alt(),r,n);
            }
            else
            {
                id = IbusMessageClass.Send_tastitem_circle(0, 0, 0, 0, 0, r, n);
            }

            
            int ret = 0;
            if (id < 0)
            {
                MessageBox.Show("绕圈失败");
            }

            do
            {
                ret = IbusMessageClass.Get_ibus_ret(id);
                Thread.Sleep(5);
            } while (ret > 0);
            if (ret < 0)
            {
                MessageBox.Show("自动起飞失败:" + ret.ToString());
            }
            else if (ret == 0)
            {
                MessageBox.Show("自动起飞成功:" + ret.ToString());
            }
        }

        private void setRcModeButton_Click(object sender, EventArgs e)
        {
            int rcMode = Convert.ToInt32(this.rcModeTextBox.Text);
            int id = IbusMessageClass.Send_rc_mode(rcMode);
            int ret = 0;
            if (id < 0)
            {
                MessageBox.Show("设置遥控器模式失败");
            }

            do
            {
                ret = IbusMessageClass.Get_ibus_ret(id);
                Thread.Sleep(5);
            } while (ret > 0);
            if (ret < 0)
            {
                MessageBox.Show("设置遥控器模式失败:" + ret.ToString());
            }
            else if (ret == 0)
            {
                MessageBox.Show("设置遥控器模式成功:" + ret.ToString());
            }
        }
    }
}
