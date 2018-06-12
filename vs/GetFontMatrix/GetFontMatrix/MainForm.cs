using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace GetFontMatrix
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            System.Drawing.Text.InstalledFontCollection ifc = new System.Drawing.Text.InstalledFontCollection();
            FontFamily[] ffs = ifc.Families;
            foreach (FontFamily ff in ffs)
                //在这里的样式里表示Regular可能使用的字体   
                if (ff.IsStyleAvailable(FontStyle.Regular))
                    this.fontComboBox.Items.Add(ff.Name);
            this.fontComboBox.Text = "宋体";
            for (int i = 3; i <= 256; i++)
            {
                this.pixComboBox.Items.Add(i.ToString());
            }
            this.pixComboBox.Text = "16";

        }

        private void SetTextBoxFont()
        {
            if (this.pixComboBox.Text == "" || this.fontComboBox.Text == "")
                return;
            FontStyle fontStyle = FontStyle.Regular;
            int ziHao = Convert.ToInt32(this.pixComboBox.Text);

            if (this.blackCheckBox.Checked)
                fontStyle = FontStyle.Bold;
            this.inputTextBox.Font = new Font(this.fontComboBox.Text, (float)ziHao * 72 / 96, fontStyle);
            GetMatrix();
        }

        private void fontComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            this.SetTextBoxFont();
        }

        private void pixComboBox_TextUpdate(object sender, EventArgs e)
        {
            this.SetTextBoxFont();
        }

        private void pixComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            this.SetTextBoxFont();
        }

        private void blackCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            this.SetTextBoxFont();
        }

        private void GetMatrix()
        {
            int    pixNum = Convert.ToInt32(this.pixComboBox.Text);
        
            int fontHeight = pixNum;
            int fontWidth = pixNum;

            if (!this.isHanZiCheckBox.Checked)
                fontWidth /= 2;


            FontStyle fontStyle = FontStyle.Regular;

            if (this.blackCheckBox.Checked)
                fontStyle = FontStyle.Bold;
        
            int getColor;
            int rowByteNum = (fontWidth + 7) / 8;
            int oneFontByteNum = rowByteNum * fontHeight;

            Bitmap bitMap = new Bitmap(rowByteNum * 8, fontHeight);
            Graphics gs = Graphics.FromImage(bitMap);
            float dpi = gs.DpiX;

          

            byte[] byteBuff = new byte[oneFontByteNum * this.inputTextBox.Text.Length];
            Font font = new Font(this.fontComboBox.Text, (float)pixNum * 72 / dpi, fontStyle);

            SizeF temp = gs.MeasureString("人", font);
            temp = gs.MeasureString("0", font);

            int n = 0;

            foreach (Char c in this.inputTextBox.Text)
            {
                gs.FillRectangle(Brushes.Black, gs.ClipBounds);
                gs.DrawString(new string(c, 1), font, Brushes.White, -fontHeight * 3 / 16, 0);
                for (int i = 0; i < fontHeight; i++)
                    for (int j = 0; j < rowByteNum; j++)
                    {
                        int index = n * oneFontByteNum + i * rowByteNum + j;
                        int xBegine = j * 8;
                        byteBuff[index] = 0;
                        for (int k = 0; k < 8; k++)
                        {
                            getColor = bitMap.GetPixel(xBegine + k, i).B;
                            if (getColor != 0)
                            {
                                byteBuff[index] |= (byte)(1 << k);
                            }
                        }
                    }
                n++;
            }
            this.showPixFont.DrawFontHeight = fontHeight;
            this.showPixFont.DrawFontWidth  = fontWidth;
            this.showPixFont.ShowArray = byteBuff;
        }

        private void inputTextBox_TextChanged(object sender, EventArgs e)
        {
            GetMatrix();
        }

        private void isHanZiCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            GetMatrix();
        }

        private void saveButton_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                StreamWriter stream = new StreamWriter(saveFileDialog.FileName, false, Encoding.Default);
                int fontWidth = this.showPixFont.DrawFontWidth;
                int fontHeight = this.showPixFont.DrawFontHeight;
                int rowByteNum = (fontWidth + 7) / 8;
                int oneFontByteNum = rowByteNum * fontHeight;
                byte[] showArray = this.showPixFont.ShowArray;
                int fontNum;
                string arrayHead = fontWidth.ToString() + "x" + fontHeight.ToString();
                string strHead = arrayHead;
                int bytesPerRow = 16;
                int allRowNum = (oneFontByteNum + bytesPerRow - 1) / bytesPerRow;
                int lastRowByteNum = oneFontByteNum - (allRowNum - 1) * bytesPerRow;

                if (this.blackCheckBox.Checked)
                {
                    arrayHead += "Blod";
                    strHead += "Blod";
                }
                arrayHead += "_Table =";
                strHead += "_Str =";

                if (showArray == null)
                    fontNum = 0;
                else
                    fontNum = showArray.Length / oneFontByteNum;

                if(this.isHanZiCheckBox.Checked)
                    stream.WriteLine(strHead + "\"" + this.inputTextBox.Text + "\";");

                string lineStr = "";
                stream.WriteLine();
                stream.WriteLine(arrayHead);
                stream.WriteLine("{");

                for (int i = 0; i < fontNum; i++)
                {
                    for (int j = 0; j < allRowNum - 1; j++)
                    {
                        lineStr = "";
                        for (int k = 0; k < bytesPerRow; k++)
                        {
                            lineStr += "0x" + Convert.ToString(showArray[i * oneFontByteNum + j * bytesPerRow + k],16).PadLeft(2, '0') + ",";
                        }
                        stream.WriteLine(lineStr);
                    }
                       lineStr = "";
                        for (int k = 0; k < lastRowByteNum; k++)
                        {
                            lineStr += "0x" + Convert.ToString(showArray[i * oneFontByteNum + (allRowNum - 1) * bytesPerRow + k],16).PadLeft(2, '0') + ",";
                        }
                        lineStr += "//" + this.inputTextBox.Text.Substring(i,1);
                        stream.WriteLine(lineStr);
                        stream.WriteLine();
                }
                stream.WriteLine("};");
                stream.Close();
            }
        }

       

        void GetMapMatrix(Bitmap bitMap)
        {
            int mapWidth = bitMap.Width;
            int mapHeight = bitMap.Height;
            byte[] byteBuff = new byte[mapWidth * mapHeight];
            Color getColor;

            for (int i = 0; i < mapHeight; i++)
            {
                for (int j = 0; j < mapWidth; j++)
                {
                    getColor = bitMap.GetPixel(j, i);
                    byteBuff[i * mapWidth + j] = (byte)((getColor.R & 0xe0) | ((getColor.G >> 3) & 0x1c) | (getColor.B >> 6));
                }
            }
            this.mapShowPixFont.DrawFontHeight = mapHeight;
            this.mapShowPixFont.DrawFontWidth = mapWidth ;
            this.mapShowPixFont.ShowMapArray = byteBuff;

        }

        private void loadMapButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "(*.jpg,*.gif,*.bmp,*.png,*.jpeg)|*.JPG;*.GIF;*.BMP;*.PNG;*.JPEG";

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                Bitmap bitMap = new Bitmap(openFileDialog.FileName);
                this.showPictureBox.Size = bitMap.Size;
                this.showPictureBox.Image = bitMap;
                GetMapMatrix(bitMap);
                this.mapWidthTextBox.Text = bitMap.Width.ToString();
                this.mapHeightTextBox.Text = bitMap.Height.ToString();
            }
        }

        private void saveMapArrayButton_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                StreamWriter stream = new StreamWriter(saveFileDialog.FileName, false, Encoding.Default);
                int mapWidth = this.mapShowPixFont.DrawFontWidth;
                int mapHeight = this.mapShowPixFont.DrawFontHeight;
                int bytesPerRow = 16;
                int allRowNum = (mapWidth * mapHeight + bytesPerRow - 1) / bytesPerRow;
                int lastRowByteNum = mapWidth * mapHeight - (allRowNum - 1) * bytesPerRow;
                byte[] showArray = this.mapShowPixFont.ShowArray;

                stream.WriteLine("(" + mapWidth.ToString() + "," + mapHeight.ToString() + ")");
                stream.WriteLine();
                string lineStr ;
                stream.WriteLine("{");
                for (int i = 0; i < allRowNum - 1; i++)
                {
                    lineStr = "";
                    for (int j = 0; j < bytesPerRow; j++)
                    {
                        lineStr += "0x" + Convert.ToString(showArray[i * bytesPerRow + j], 16).PadLeft(2, '0') + ",";
                    }
                    stream.WriteLine(lineStr);
                }
                lineStr = "";
                for (int j = 0; j < lastRowByteNum; j++)
                {
                    lineStr += "0x" + Convert.ToString(showArray[(allRowNum - 1) * bytesPerRow + j], 16).PadLeft(2, '0') + ",";
                }
                stream.WriteLine(lineStr);

                stream.WriteLine("}");
                stream.Close();
            }
        }
    }
}
