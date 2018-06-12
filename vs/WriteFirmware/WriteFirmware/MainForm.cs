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
using System.Threading;
using System.IO;
using System.IO.Pipes;

namespace WriteFirmware
{
    public partial class MainForm : Form
    {
        private SerialPort com;
        private bool beginWrite;
        private int progressCount;
        private UInt32 progressState;
        private System.Windows.Forms.Timer tmr;
        private FirmwareManage firmwareManage;

        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            //BluetoothCtrl.DetecteBluetoothDevice();
            this.firmwareManage = new FirmwareManage();
            this.com = new SerialPort();
            this.portComboBox.Items.AddRange(SerialPort.GetPortNames());
            this.beginWrite = false;
            this.progressCount = 0;

            tmr = new System.Windows.Forms.Timer();
            tmr.Interval = 10;
            tmr.Tick += new EventHandler(timerCallBack);
            tmr.Start();
        }

        private void loadButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "HEX文件(*.hex)|*.hex|BIN文件(*.bin)|*.bin";
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                this.filepathTextBox.Text = openFileDialog.FileName;
            }
        }

        private void writeButton_Click(object sender, EventArgs e)
        {
            Thread writeThread = new Thread(new ThreadStart(runWriteFirmware));

            if(this.portComboBox.Text == "")
            {
                MessageBox.Show("请选择一个有效端口号");
                return;
            }
            if(this.filepathTextBox.Text == "")
            {
                MessageBox.Show("请选择一个文件");
                return;
            }

            if (this.filepathTextBox.Text.Contains(".bin") || this.filepathTextBox.Text.Contains(".BIN"))
            {
                firmwareManage.HexOrBin = FirmwareManage.FileType.BinFileType;
            }
            firmwareManage.LoadHexFile(this.filepathTextBox.Text);

            this.com.PortName = this.portComboBox.Text;
            this.com.BaudRate = 115200;
            try
            {
                this.com.Open();
                this.firmwareManage.ComPort = com;
                if (!File.Exists(this.filepathTextBox.Text))
                {
                    MessageBox.Show("文件不存在");
                    return;
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show(ex.Message);
                return;
            }
            
            writeThread.Start();

        }

        private void runWriteFirmware()
        {
            progressCount = 0;
            this.beginWrite = true;
            /*while (this.beginWrite)
            {
                progressCount++;
                Thread.Sleep(100);
            }*/
            int secNum = this.firmwareManage.SectionNum;
            if (secNum < 1)
                return;
            int dataLength;
            UInt32 beginAddr;
            byte[] dataBuff = new byte[0x10000];
            this.firmwareManage.GetSectionData(0, out beginAddr, dataBuff, out dataLength);
            UInt32 ver = this.firmwareManage.GetSoftVersion(50);
            /*int division2k = dataLength >> 11;

           for(int i = 0; i < division2k; i ++)
           {
               if(this.firmwareManage.FirmwareSetAddr((UInt32)(beginAddr + i * 0x800)) != 0)
               {
                   MessageBox.Show("设置地址错误");
                   this.com.Close();
                   return;
               }

               for(int j = 0; j < 0x80; j ++)
               {
                   if(this.firmwareManage.WriteDataToDevice(dataBuff, i * 0x800 + j * 16,16) != 0)
                   {
                       MessageBox.Show("设置数据错误");
                       this.com.Close();
                       return;
                   }
               }

               if(this.firmwareManage.FirmwareCkAndPro(dataBuff, i * 0x800,0x800) != 0)
               {
                   MessageBox.Show("数据段校验错误");
                   this.com.Close();
                   return;
               }
               progressCount = (i + 1) * 100 / (division2k + 1);
           }

           int leftData = dataLength & 0x7ff;
           if(leftData != 0)
           {
               int division16 = leftData / 16;
               int leftData16 = leftData & 0x0f;

               if (this.firmwareManage.FirmwareSetAddr((UInt32)(beginAddr + division2k * 0x800)) != 0)
               {
                   MessageBox.Show("设置地址错误");
                   this.com.Close();
                   return;
               }

               for (int j = 0; j < division16; j++)
               {
                   if (this.firmwareManage.WriteDataToDevice(dataBuff, division2k * 0x800 + j * 16, 16) != 0)
                   {
                       MessageBox.Show("设置数据错误");
                       this.com.Close();
                       return;
                   }
               }

               if (leftData16 > 0)
               {
                   if (this.firmwareManage.WriteDataToDevice(dataBuff, division2k * 0x800 + division16 * 16, leftData16) != 0)
                   {
                       MessageBox.Show("设置数据错误");
                       this.com.Close();
                       return;
                   }
               }

               if (this.firmwareManage.FirmwareCkAndPro(dataBuff, division2k * 0x800, leftData) != 0)
               {
                   MessageBox.Show("数据段校验错误");
                   this.com.Close();
                   return;
               }
           }

           if (this.firmwareManage.FirmwareSetInf(dataBuff, (UInt32)dataLength,beginAddr, 0x01000000) != 0)
           {
               MessageBox.Show("设置固件信息错误");
               this.com.Close();
               return;
           }

   */

           FirmwareManage.WriteFirmwareInit();
           while (beginWrite)
            {
                byte[] buff = new byte[256];
                byte[] outBuff = new byte[32];
                int outLength = 0;
                int length = this.com.BytesToRead;
                if (length > 0)
                {
                    if (length > 256)
                        length = 256;
                    this.com.Read(buff, 0, length);
                }

                progressState = FirmwareManage.WriteFirmwareUpdate(buff, length, outBuff,out outLength, (UInt32)Environment.TickCount);
                progressCount = (int)progressState & 0x0000ffff;

                if (outLength > 0)
                    this.com.Write(outBuff,0, outLength);

                if ((progressState >> 16) == 15)
                {
                    Thread.Sleep(100);
                    beginWrite = false;
                    break;
                }
                    
       //         Thread.Sleep(1);
            }

            this.com.Close();
        }

        private void timerCallBack(object sender, EventArgs e)
        {
            if(this.beginWrite)
            {
                this.progressBar.Visible = true;
                this.progressBar.Value = progressCount;
                //if (progressCount == 100)
                 //   this.beginWrite = false;

                if (progressState == 0x00020000)
                {
                  //  if(MessageBox.Show("是否烧写","是否用版本号" + FirmwareManage.WriteFirmwareGetDeviceSoftVer().ToString() + "替换为版本号" + FirmwareManage.HexFileGetFileVer().ToString(), MessageBoxButtons.YesNoCancel) == DialogResult.Yes)
                 //   {
                       FirmwareManage.WriteFirmwareBeginWrite();
                  //  }
                  //  else
                   // {
                   //     this.beginWrite = false;
                   // }
                }
               
            }
            else
            {
                this.progressBar.Visible = false;
            }
        }

        private void loadBootButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "HEX文件(*.hex)|*.hex|BIN文件(*.bin)|*.bin";
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                this.bootPathTextBox.Text = openFileDialog.FileName;
            }
        }

        private void saveButton_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "BIN文件(*.bin)|*.bin";

            if (this.filepathTextBox.Text == "" || this.bootPathTextBox.Text == "")
            {
                MessageBox.Show("请选择有效文件");
            }

            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                FileStream fileStream = new FileStream(saveFileDialog.FileName, FileMode.OpenOrCreate);
                if (this.bootPathTextBox.Text.Contains(".bin") || this.bootPathTextBox.Text.Contains(".BIN"))
                {
                    firmwareManage.HexOrBin = FirmwareManage.FileType.BinFileType;
                }
                firmwareManage.LoadHexFile(this.bootPathTextBox.Text);

                int dataLength;
                UInt32 beginAddr;
                byte[] dataBuff = new byte[0x10000];
                this.firmwareManage.GetSectionData(0, out beginAddr, dataBuff, out dataLength);
                fileStream.Write(dataBuff, 0, dataLength);
                while (fileStream.Length < 0x3800)
                    fileStream.WriteByte(0xff);

                this.firmwareManage.LoadHexFile(this.filepathTextBox.Text);
                dataLength = this.firmwareManage.GetFirmwareInf(dataBuff);
                fileStream.Write(dataBuff, 0, dataLength);

                
                while (fileStream.Length < 0x4000)
                    fileStream.WriteByte(0xff);

                
                int secNum = this.firmwareManage.SectionNum;
                for (int i = 0; i < secNum; i++)
                {
                    this.firmwareManage.GetSectionData(i, out beginAddr, dataBuff, out dataLength);
                    fileStream.Write(dataBuff, 0, dataLength);
                }
                fileStream.Close();
            }
        }
    }
}
