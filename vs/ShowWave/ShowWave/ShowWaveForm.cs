using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;

namespace ShowWave
{
    public partial class ShowWaveForm : Form
    {
        private struct IdAndData
        {
            public byte id;
            public float data;
        }

        private SerialPort com;
        private System.Windows.Forms.Timer tmr;
        private byte []dataBuff;
        private int dataIndex;
        private int xCount;
        public ShowWaveForm()
        {
            InitializeComponent();
        }

        private void ShowWaveForm_Load(object sender, EventArgs e)
        {
            this.com = new SerialPort();
            tmr = new System.Windows.Forms.Timer();
            tmr.Interval = 1;
            tmr.Tick += new EventHandler(this.timeOverEvent);

            this.portComboBox.Items.AddRange(SerialPort.GetPortNames());
            this.baudRateComboBox.Items.AddRange(new string[] { "9600", "19200", "38400", "57600", "115200" });
            if (this.portComboBox.Items.Count > 0)
            {
                this.portComboBox.SelectedIndex = 0;
            }
            this.baudRateComboBox.SelectedIndex = 4;

            this.plot1.YAxes[0].Min = -100.0f;
            this.plot1.YAxes[0].Span = 200.0f;
            this.plot1.YAxes[0].ScaleDisplay.TextFormatting.Precision = 4;
            this.plot1.XAxes[0].ScaleDisplay.TextFormatting.Precision = 3;
            this.plot1.XAxes[0].Span = 1000.0f;

            dataBuff = new byte[2048];
            dataIndex = 0;
            xCount = 0;
        }

        private List<IdAndData> FindIdAndData()
        {
            List<IdAndData> listData = new List<IdAndData>();
/*
            IdAndData idData1 = new IdAndData();
            idData1.id = 0;
            idData1.data = 3;
            listData.Add(idData1);

            idData1.id = 1;
            idData1.data = 4;
            listData.Add(idData1);
*/
            for (int i = 0; i < dataIndex; i ++)
            {
                if(dataBuff[i] == 0xAA)
                {
                    if(dataIndex >= (i + 8) && dataIndex >= (i + dataBuff[i + 2] * 4 + 4))
                    {
                        int num = dataBuff[i + 2] ;
                        byte id = dataBuff[i + 1];
                        int packetSize = num * 4 + 4;
                        int sum = 0;
                        for (int j = i + 1; j < i + packetSize - 1; j ++)
                        {
                            sum += dataBuff[j];
                        }
                        if (dataBuff[i + packetSize - 1] != (sum & 0x000000ff))
                            continue;
                        for(int j = 0; j < num; j++)
                        {
                            IdAndData idData = new IdAndData();
                            idData.id = id ++;
                            idData.data = BitConverter.ToSingle(dataBuff, i + 3 + j * 4);
                            listData.Add(idData);
                        }
                        i = i + packetSize - 1;
                    }
                    else
                    {
                        for(int j = 0; j < dataIndex - i; j ++)
                        {
                            dataBuff[j] = dataBuff[i + j];
                        }
                        dataIndex = dataIndex - i;
                        return listData;
                    }
                }
            }
            dataIndex = 0;
            return listData;
        }

        private Color FindColor(int id)
        {
            Color color = System.Drawing.Color.White;
            switch (id)
            {
                case 0:
                    color = System.Drawing.Color.Red;
                    break;
                case 1:
                    color = System.Drawing.Color.Green;
                    break;
                case 2:
                    color = System.Drawing.Color.Blue;
                    break;
                case 3:
                    color = System.Drawing.Color.RoyalBlue;
                    break;
                case 4:
                    color = System.Drawing.Color.GreenYellow;
                    break;
                case 5:
                    color = System.Drawing.Color.Brown;
                    break;
                case 6:
                    color = System.Drawing.Color.Wheat;
                    break;
                case 7:
                    color = System.Drawing.Color.Chartreuse;
                    break;
                case 8:
                    color = System.Drawing.Color.DarkGoldenrod;
                    break;
                case 9:
                    color = System.Drawing.Color.DeepSkyBlue;
                    break;
                default:
                    color = System.Drawing.Color.White;
                    break;
            }
            return color;
        }

        private void timeOverEvent(object sender, EventArgs e)
        {
            if (!com.IsOpen)
                return;
            int reciveNum = (int)com.BytesToRead;
            if (reciveNum <= 0)
                return;
            if (reciveNum > 2048 - dataIndex)
                reciveNum = 2048 - dataIndex;
            byte[] buffTemp = new byte[reciveNum];
            com.Read(buffTemp, 0, reciveNum);
            Array.Copy(buffTemp, 0, dataBuff, dataIndex, reciveNum);
            dataIndex += reciveNum;
            List<IdAndData> listData = FindIdAndData();
            for(int i = 0; i < listData.Count; i ++)
            {
                if (listData[i].id == 0)
                    xCount++;
                if (listData[i].id > 100)
                    return;
                while(listData[i].id >= plot1.Channels.Count)
                {
                    Iocomp.Classes.PlotChannelTrace plotChannelTrace = new Iocomp.Classes.PlotChannelTrace();
                    plotChannelTrace = new Iocomp.Classes.PlotChannelTrace();
                    plotChannelTrace.Name = "ID" + listData[i].id.ToString();
                    plotChannelTrace.TitleText = "ID" + listData[i].id.ToString();
                    plotChannelTrace.Color = FindColor(listData[i].id);
                    this.plot1.Channels.Add(plotChannelTrace);
                }
                if(listData[i].data <= 10000 && listData[i].data >= -10000)
                this.plot1.Channels[listData[i].id].AddXY(xCount, listData[i].data);
            }
        }

        private void startButton_Click(object sender, EventArgs e)
        {
            try
            {
                if (com.IsOpen)
                {
                    tmr.Stop();
                    com.Close();
                    this.startButton.Text = "开始";
                    this.portComboBox.Enabled = true;
                    this.baudRateComboBox.Enabled = true;
                    this.saveDataButton.Enabled = true;
                    this.loadDataButton.Enabled = true;                    
                }
                else
                {
                    if (this.portComboBox.Text == "")
                    {
                        MessageBox.Show(this, "请选择一个串口", "串口名无效");
                        return;
                    }

                    this.com.PortName = this.portComboBox.Text;
                    this.com.BaudRate = Int32.Parse(this.baudRateComboBox.Text);
                    this.com.Parity = Parity.None;
                    this.com.DataBits = 8;
                    this.com.StopBits = StopBits.One;
                    this.com.DtrEnable = true;
                    this.com.RtsEnable = true;
                    this.com.Open();

                    //this.com.ReadExisting();
                    this.startButton.Text = "停止";
                    this.portComboBox.Enabled = false;
                    this.baudRateComboBox.Enabled = false;
                    this.saveDataButton.Enabled = false;
                    this.loadDataButton.Enabled = false;
                    tmr.Start();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.Message, "错误");
                if (com.IsOpen)
                    com.Close();
            }
        }

        private void saveDataButton_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.FileName = DateTime.Now.ToString("yyyy-MM-dd hh_mm_ss");
            if (saveFileDialog.ShowDialog() == DialogResult.OK && saveFileDialog.FileName != "")
            {
                /*Stream fileStream = saveFileDialog.OpenFile();
                StreamWriter streamWriter = new StreamWriter(fileStream);
                streamWriter.WriteLine(this.plot1.Channels.Count.ToString());
                //fileStream.Seek(0,SeekOrigin.End);
                for (int i = 0 ; i < this.plot1.Channels.Count; i ++)
                {
                    this.plot1.Channels[i].SaveDataToStream(fileStream, out streamWriter);
                }
                streamWriter.Close();
                fileStream.Close();*/
                this.plot1.SaveDataToFile(saveFileDialog.FileName);
            }
        }

        private void loadDataButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            //openFileDialog.FileName = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss");
            if (openFileDialog.ShowDialog() == DialogResult.OK && openFileDialog.FileName != "")
            {
                Stream fileStream = openFileDialog.OpenFile();
                StreamReader streamReader = new StreamReader(fileStream);
                string strFirst = streamReader.ReadLine();
                int num = strFirst.Split(new char[] { '\t'}).Length / 2;
                for (int i = 0; i < num; i++)
                {
                    if(i >= plot1.Channels.Count)
                    {
                        Iocomp.Classes.PlotChannelTrace plotChannelTrace = new Iocomp.Classes.PlotChannelTrace();
                        plotChannelTrace = new Iocomp.Classes.PlotChannelTrace();
                        plotChannelTrace.Name = "ID" + i.ToString();
                        plotChannelTrace.TitleText = "ID" + i.ToString();
                        plotChannelTrace.Color = FindColor(i);
                        this.plot1.Channels.Add(plotChannelTrace);
                    }
                   // this.plot1.Channels[i].LoadDataFromStream(fileStream);
                }
                streamReader.Close();
                fileStream.Close();
                this.plot1.LoadDataFromFile(openFileDialog.FileName);
                this.plot1.YAxes[0].Min = -1000.0f;
                this.plot1.YAxes[0].Span = 2000.0f;
                this.plot1.YAxes[0].ScaleDisplay.TextFormatting.Precision = 4;
                this.plot1.XAxes[0].ScaleDisplay.TextFormatting.Precision = 3;
                this.plot1.XAxes[0].Min = 0;
                this.plot1.XAxes[0].Span = 10000.0f;
            }
        }

        private void clearDataButton_Click(object sender, EventArgs e)
        {
            this.plot1.Channels.Reset();
            dataIndex = 0;
            xCount = 0;
        }
    }
}
