using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GetFontMatrix
{
    public partial class ShowPixFont : UserControl
    {
        private int pixSize = 1;
        private Color backgroundColor = Color.Green;
        private Color foregroundColor = Color.CadetBlue;
        private Color gridColor = Color.Black;
        private Color fontGridColor = Color.Yellow;

        private int fontHeight = 8;
        private int fontWidth = 4;
        private byte[] showArray;
        private bool isShowMap = false;
        private Bitmap bitMap;

        public ShowPixFont()
        {
            InitializeComponent();
            this.showPictureBox.Size = this.Size;
        }

        public int DrawFontHeight
        {
            get { return fontHeight; }
            set { fontHeight = value; }
        }

        public int DrawFontWidth
        {
            get { return fontWidth; }
            set { fontWidth = value; }
        }

        public byte[] ShowArray
        {
            get { return showArray; }
            set 
            { 
                showArray = value;
                //
                int rowByteNum = (fontWidth + 7) / 8;
                int oneFontByteNum = rowByteNum * fontHeight;
                int fontNum;

                if (this.showArray == null)
                    fontNum = 0;
                else
                    fontNum = this.showArray.Length / oneFontByteNum;

               // if (fontNum * fontWidth * pixSize > this.showPanel.Width)
                this.showPictureBox.Width = fontNum * fontWidth * pixSize;
               // if (fontHeight * pixSize > this.showPanel.Height)
                this.showPictureBox.Height = fontHeight * pixSize;

                isShowMap = false;
                CreateDrawBitMap();
                //this.showPictureBox.Invalidate();
                this.showPictureBox.Image = bitMap;
            }
        }

        public byte[] ShowMapArray
        {
            get { return showArray; }
            set
            {
                this.showArray = value;
                this.showPictureBox.Width = fontWidth * pixSize;
                this.showPictureBox.Height = fontHeight * pixSize;
                isShowMap = true;
                CreateDrawBitMap();
                //this.showPictureBox.Invalidate();
                this.showPictureBox.Image = bitMap;
            }
        }

        private void CreateDrawBitMap()
        {
            int drawWidth = this.showPictureBox.Width;
            int drawHeight = this.showPictureBox.Height;

            if (isShowMap)
            {
                bitMap = new Bitmap(drawWidth, drawHeight);
                Graphics gs = Graphics.FromImage(bitMap);

                if (this.showArray == null)
                    return;

                gs.FillRectangle(Brushes.White, 0, 0, drawWidth, drawHeight);
                for (int i = 0; i < fontHeight; i++)
                {
                    int yPix = i * pixSize;
                    for (int j = 0; j < fontWidth; j++)
                    {
                        int xPix = j * pixSize;
                        int byteColor = showArray[i * fontWidth + j];
                        Color color = Color.FromArgb(byteColor & 0xe0, (byteColor & 0x1c) << 3, (byteColor & 0x03) << 6);
                        if (pixSize <= 2)
                        {
                            gs.FillRectangle(new SolidBrush(color), xPix, yPix, pixSize, pixSize);
                        }
                        else
                        {
                            gs.FillRectangle(new SolidBrush(color), xPix + 1, yPix + 1, pixSize - 1, pixSize - 1);
                        }

                    }
                }
                gs.Dispose();
            }
            else
            {
                int rowByteNum = (fontWidth + 7) / 8;
                int oneFontByteNum = rowByteNum * fontHeight;
                int fontNum;

                if (this.showArray == null)
                    fontNum = 0;
                else
                    fontNum = this.showArray.Length / oneFontByteNum;

                if (drawWidth == 0)
                    return;

                bitMap = new Bitmap(drawWidth, drawHeight);
                Graphics gs = Graphics.FromImage(bitMap);

                gs.FillRectangle(new SolidBrush(backgroundColor), 0, 0, drawWidth, drawHeight);
                Pen pen = new Pen(gridColor, 1);

                for (int i = 0; i < drawWidth; i += pixSize)
                {
                    gs.DrawLine(pen, i, 0, i, drawHeight);
                }
                for (int i = 0; i < drawHeight; i += pixSize)
                {
                    gs.DrawLine(pen, 0, i, drawWidth, i);
                }

                for (int i = 0; i < fontNum; i++)
                {
                    int fontBegin = i * oneFontByteNum;
                    int xPixBegin = i * fontWidth * pixSize;

                    for (int j = 0; j < fontHeight; j++)
                    {
                        int rowBegin = fontBegin + rowByteNum * j;
                        int yPix = j * pixSize;
                        for (int k = 0; k < rowByteNum; k++)
                        {
                            int index = rowBegin + k;
                            for (int l = 0; l < 8; l++)
                            {
                                int xPix = xPixBegin + (k * 8 + l) * pixSize;
                                if ((this.showArray[index] & (1 << l)) != 0)
                                {
                                    gs.FillRectangle(new SolidBrush(foregroundColor), xPix + 1, yPix + 1, pixSize - 1, pixSize - 1);
                                }
                            }
                        }
                    }
                    pen.Color = fontGridColor;
                    gs.DrawLine(pen, xPixBegin, 0, xPixBegin + fontWidth * pixSize, 0);
                    gs.DrawLine(pen, xPixBegin + fontWidth * pixSize, 0, xPixBegin + fontWidth * pixSize, fontHeight * pixSize);
                    gs.DrawLine(pen, xPixBegin + fontWidth * pixSize, fontHeight * pixSize, xPixBegin, fontHeight * pixSize);
                    gs.DrawLine(pen, xPixBegin, fontHeight * pixSize, xPixBegin, 0);

                    //pen.DashStyle = System.Drawing.Drawing2D.DashStyle.Dash;
                    gs.DrawLine(pen, xPixBegin, 0, xPixBegin + fontWidth * pixSize, fontHeight * pixSize);
                    gs.DrawLine(pen, xPixBegin + fontWidth * pixSize, 0, xPixBegin, fontHeight * pixSize);

                }
                gs.Dispose();

               
            }
        }

        private void showPanel_Paint(object sender, PaintEventArgs e)
        {
       /*     Graphics gsP = e.Graphics;
            Rectangle rectangle = e.ClipRectangle;

            if (this.bitMap == null)
                return;
            gsP.DrawImage(bitMap, rectangle, rectangle, GraphicsUnit.Pixel);
            // gs.DrawString("22678热门", font, Brushes.White, 0, 0);
            gsP.Dispose();*/
        }
    }
}
