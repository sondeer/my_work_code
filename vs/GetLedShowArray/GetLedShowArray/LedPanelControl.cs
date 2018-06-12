using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GetLedShowArray
{
    public partial class LedPanelControl : UserControl
    {
        private byte[] byteArray = new byte[]{ 0,0,0,0,0,0,0,0,0};
        private Bitmap bitMap;

        public delegate void ArrayChangedEventHandler(object sender);
        public event ArrayChangedEventHandler ArrayChanged;
        public LedPanelControl()
        {
            InitializeComponent();
            this.SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
            this.SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            this.SetStyle(ControlStyles.UserPaint, true);
            CreateDrawBitMap();
        }

        public byte[]ByteArray
        {
            set { byteArray = value;
                CreateDrawBitMap();
                this.Invalidate();
            }
            get { return byteArray; }
        }

        public string ByteArrayString
        {
            get
            {
                string str = "";
                for(int i = 0; i < 8; i ++)
                {
                    if(i != 7)
                        str = str + "0x" + Convert.ToString(byteArray[i], 16).PadLeft(2,'0').ToUpper() + ",";
                    else
                        str = str + "0x" + Convert.ToString(byteArray[i], 16).PadLeft(2,'0').ToUpper();
                }
                return str;

            }
        }

        private void CreateDrawBitMap()
        {
            bitMap = new Bitmap(this.Width, this.Height);
            Graphics gsP = Graphics.FromImage(bitMap);

            float w = this.Width / 8.0f;
            float h = this.Height / 8.0f;
            for (int i = 0; i < 8; i++)
                for (int j = 0; j < 8; j++)
                {
                    if ((byteArray[i] & (1 << j)) == 0)
                        gsP.DrawImage(global::GetLedShowArray.Properties.Resources.gray, w * j, w * i, w, h);
                    else
                        gsP.DrawImage(global::GetLedShowArray.Properties.Resources.yellow, w * j, w * i, w, h);
                }

            gsP.Dispose();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            Graphics gsP = e.Graphics;
            if (bitMap != null)
                gsP.DrawImage(bitMap, 0, 0);
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            float w = this.Width / 8.0f;
            float h = this.Height / 8.0f;

            int i = (int)(e.X / w);
            int j = (int)(e.Y / h);

            byteArray[j] = (byte)(byteArray[j] ^ (1 << i));
            if (ArrayChanged != null)
                ArrayChanged(this);
            CreateDrawBitMap();
            this.Invalidate();
        }

        protected override void OnResize(EventArgs e)
        {
            CreateDrawBitMap();
            this.Invalidate();
        }

        public void ClearAllLed()
        {
            for(int i = 0; i < 8; i ++)
            {
                byteArray[i] = 0;
            }
            if (ArrayChanged != null)
                ArrayChanged(this);
            CreateDrawBitMap();
            this.Invalidate();
        }

    }
}
