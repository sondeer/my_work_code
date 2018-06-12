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
using System.Collections;

namespace EKFTest
{
    public partial class MainForm : Form
    {
        private WRHid wrHid;
        private Thread recThread;
        private Queue<byte[]> dataQueue;
        private System.Windows.Forms.Timer tim;
        private bool beginFlag = false;
        private int dataCount = 0;

        public MainForm()
        {
            InitializeComponent();
        }

        private void ReceiveThread()
        {
            while(true)
            {
                if (!beginFlag)
                    continue;
                try 
                { 
                    byte []recData = new byte[64];
                    if(this.wrHid.Read(recData,64) == 64)
                    {
                        dataQueue.Enqueue(recData);
                    }
                    else
                    {
                        this.wrHid.Close();
                        this.wrHid.Open(0x0027, 0x0395);
                    }
                }
                catch (Exception ex)
                {
                     try 
                     {
                         this.wrHid.Close();
                         this.wrHid.Open(0x0027, 0x0395);
                     }
                     catch (Exception ex1)
                     {

                     }
                }
                Thread.Sleep(1);
            }
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            this.wrHid = new WRHid();
            
            try 
            {
                this.wrHid.Open(0x0027, 0x0395);
            }
            catch (Exception ex)
            {
                
            }
            dataQueue = new Queue<byte[]>();
            recThread = new Thread(new ThreadStart(ReceiveThread));
            recThread.Start();

            tim = new System.Windows.Forms.Timer();
            tim.Interval = 10;
            tim.Tick += new EventHandler(Timer10ms);
            tim.Start();

            this.plot1.Channels[0].Name = "卡尔曼滤波Pitch角度";
            this.plot1.Channels[0].TitleText = "卡尔曼滤波Pitch角度";

            this.plot2.Channels[0].Name = "卡尔曼滤波Roll角度";
            this.plot2.Channels[0].TitleText = "卡尔曼滤波Roll角度";

            Iocomp.Classes.PlotChannelTrace plotChannelTrace = new Iocomp.Classes.PlotChannelTrace();
            plotChannelTrace.Name = "卡尔曼滤波Pitch角速度";
            plotChannelTrace.TitleText = "卡尔曼滤波Pitch角速度";
            plotChannelTrace.Color = System.Drawing.Color.Blue;
            this.plot1.Channels.Add(plotChannelTrace);

            plotChannelTrace = new Iocomp.Classes.PlotChannelTrace();
            plotChannelTrace.Name = "原始Pitch角速度";
            plotChannelTrace.TitleText = "原始Pitch角速度";
            plotChannelTrace.Color = System.Drawing.Color.Orange;
            this.plot1.Channels.Add(plotChannelTrace);

            plotChannelTrace = new Iocomp.Classes.PlotChannelTrace();
            plotChannelTrace.Name = "卡尔曼滤波Roll角速度";
            plotChannelTrace.TitleText = "卡尔曼滤波Roll角速度";
            plotChannelTrace.Color = System.Drawing.Color.Blue;
            this.plot2.Channels.Add(plotChannelTrace);

            plotChannelTrace = new Iocomp.Classes.PlotChannelTrace();
            plotChannelTrace.Name = "原始Roll角速度";
            plotChannelTrace.TitleText = "原始Roll角速度";
            plotChannelTrace.Color = System.Drawing.Color.Orange;
            this.plot2.Channels.Add(plotChannelTrace);

            this.plot1.YAxes[0].Min = -0.1f;
            this.plot1.YAxes[0].Span = 0.2f;
            this.plot1.YAxes[0].ScaleDisplay.TextFormatting.Precision = 4;
            this.plot1.XAxes[0].ScaleDisplay.TextFormatting.Precision = 3;
            this.plot1.XAxes[0].Span = 10.0f;

            this.plot2.YAxes[0].Min = -0.1f;
            this.plot2.YAxes[0].Span = 0.2f;
            this.plot2.YAxes[0].ScaleDisplay.TextFormatting.Precision = 4;
            this.plot2.XAxes[0].ScaleDisplay.TextFormatting.Precision = 3;
            this.plot2.XAxes[0].Span = 10.0f;
        }

        private void Timer10ms(object sender, EventArgs e)
        {
            while (dataQueue.Count > 0)
            {
                float floatData;
                byte[] temp = dataQueue.Dequeue();

                if (temp == null)
                    continue;

                floatData = BitConverter.ToSingle(temp,9 * 4);
                this.plot1.Channels[0].AddXY(dataCount * 0.002f, floatData);

                floatData = BitConverter.ToSingle(temp, 12 * 4);
                this.plot1.Channels[1].AddXY(dataCount * 0.002f, floatData);

                floatData = BitConverter.ToSingle(temp, 1 * 4);
                this.plot1.Channels[2].AddXY(dataCount * 0.002f, floatData);

                floatData = BitConverter.ToSingle(temp, 10 * 4);
                this.plot2.Channels[0].AddXY(dataCount * 0.002f, floatData);

                floatData = BitConverter.ToSingle(temp, 13 * 4);
                this.plot2.Channels[1].AddXY(dataCount * 0.002f, floatData);

                floatData = BitConverter.ToSingle(temp, 0 * 4);
                this.plot2.Channels[2].AddXY(dataCount * 0.002f, floatData);
                dataCount++;

            }
            
        }

        private void beginButton_Click(object sender, EventArgs e)
        {
            beginFlag = !beginFlag;
            if (beginFlag)
            {
                this.beginButton.Text = "停止";
                this.plot1.Channels[0].Clear();
                this.plot1.Channels[1].Clear();
                this.plot1.Channels[2].Clear();
                this.plot2.Channels[0].Clear();
                this.plot2.Channels[1].Clear();
                this.plot2.Channels[2].Clear();
                this.plot1.XAxes[0].Min = 0.0f;
                this.plot2.XAxes[0].Min = 0.0f;
                this.plot1.XAxes[0].Tracking.Enabled = true;
                this.plot2.XAxes[0].Tracking.Enabled = true;
                this.plot1.YAxes[0].Tracking.Enabled = true;
                this.plot2.YAxes[0].Tracking.Enabled = true;
                dataCount = 0;
            }
            else
            {
                this.beginButton.Text = "开始";
            }
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            this.recThread.Abort();
        }
    }
}
