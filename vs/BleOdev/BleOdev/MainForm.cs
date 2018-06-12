using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

using InTheHand.Net;
using InTheHand.Net.Bluetooth;
using InTheHand.Net.Sockets;

namespace BleOdev
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            BluetoothClient cli = new BluetoothClient();
            BluetoothDeviceInfo[] test = cli.DiscoverDevices();
            BluetoothClient blueclient = new BluetoothClient();
            Guid mGUID1 = BluetoothService.Handsfree;       //蓝牙服务的uuid
            BluetoothAddress addr = BluetoothAddress.Parse("506583092654");
            cli.Connect(addr, mGUID1);      //开始配对  蓝牙4.0不需要setpin
            InitializeComponent();
        }
    }
}
