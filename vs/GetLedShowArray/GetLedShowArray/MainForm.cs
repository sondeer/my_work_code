using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GetLedShowArray
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void cleanButton_Click(object sender, EventArgs e)
        {
            ledPanelControl1.ClearAllLed();
            ledPanelControl2.ClearAllLed();
            ledPanelControl3.ClearAllLed();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            ledPanelControl1.ArrayChanged += new GetLedShowArray.LedPanelControl.ArrayChangedEventHandler(this.ledPanelControl_ArrayChanged);
            ledPanelControl2.ArrayChanged += new GetLedShowArray.LedPanelControl.ArrayChangedEventHandler(this.ledPanelControl_ArrayChanged);
            ledPanelControl3.ArrayChanged += new GetLedShowArray.LedPanelControl.ArrayChangedEventHandler(this.ledPanelControl_ArrayChanged);


        }

        private void ledPanelControl_ArrayChanged(object sender)
        {
            byte[] byteArray = ledPanelControl1.ByteArray;
            string str = "";
            str += ledPanelControl1.ByteArrayString;
            str += "," + ledPanelControl2.ByteArrayString;
            str += "," + ledPanelControl3.ByteArrayString;
            this.textBox1.Text = str;
        }
    }
}
