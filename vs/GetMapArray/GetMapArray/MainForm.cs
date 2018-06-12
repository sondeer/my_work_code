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

namespace GetMapArray
{
    public partial class MainForm : Form
    {
        Bitmap thisRawBitmap;

        public MainForm()
        {
            InitializeComponent();
        }

        private Bitmap convertBitmap(Bitmap rawBitmap)
        {
            Bitmap retBitmap = new Bitmap(rawBitmap.Width, rawBitmap.Height);

            for (int i = 0; i < rawBitmap.Width; i ++)
                for(int j = 0; j < rawBitmap.Height; j ++)
                {
                    Color getColor = rawBitmap.GetPixel(i, j); //Color.FromArgb(i, j, 0);// rawBitmap.GetPixel(i, j);
                    retBitmap.SetPixel(i, j, Color.FromArgb(getColor.R & 0xf1, getColor.G & 0xfc, getColor.B & 0xf1));
                    //retBitmap.SetPixel(i, j, Color.FromArgb(getColor.R & 0xc0, getColor.G & 0xc0, getColor.B & 0xc0));
                }
            return retBitmap;
        }

        private void loadButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            if(openFileDialog.ShowDialog() == DialogResult.OK)
            {
                thisRawBitmap = new Bitmap(openFileDialog.FileName);
                this.rawPictureBox.Size = thisRawBitmap.Size;
                this.rawPictureBox.Image = thisRawBitmap;
                this.treatedPictureBox.Image = convertBitmap(thisRawBitmap);
            }
        }

        private void ColorToArray(Color c, byte[] array)
        {
            /* array[0] = 0;
             array[1] = 0;
             for (int i = 0; i < 5; i++)
             {
                 if ((c.R & (1 << (7 - i))) != 0)
                     array[0] = (byte)(array[0] | (1 << i));
             }

             for (int i = 0; i < 3; i++)
             {
                 if ((c.G & (1 << (7 - i))) != 0)
                 {
                     array[0] = (byte)(array[0] | (1 << (i + 5)));
                 }
                 if ((c.G & (1 << (4 - i))) != 0)
                 {
                     array[1] = (byte)(array[1] | (1 << i));
                 }
             }

             for (int i = 0; i < 5; i++)
             {
                 if ((c.B & (1 << (7 - i))) != 0)
                 {
                     array[1] = (byte)(array[1] | (1 << (i + 3)));
                 }
             }
             */
            array[0] = (byte)((c.R & 0xf1) | (c.G >> 5));
            array[1] = (byte)(((c.G & 0x1c) << 3) | (c.B  >> 3));
        }

        private void saveButton_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            if (saveFileDialog.ShowDialog() == DialogResult.OK)
            {
                FileStream fileStream = new FileStream(saveFileDialog.FileName, FileMode.OpenOrCreate);
                byte[] colorByte = new byte[2];

                for (int i = 0; i < thisRawBitmap.Height; i ++)
                   for(int j = 0; j < thisRawBitmap.Width; j ++)
                    {
                        //for (int i = 0; i < 128; i++)
                       //     for (int j = 0; j < 160; j++)
                       //     {
                                Color c = thisRawBitmap.GetPixel(j, i);
                               // Color c = Color.FromArgb(i,j,0);
                        //colorByte[0] = (byte)(((c.R & 0xf1) >> 3) | ((c.G & 0xfc) << 3));
                        //colorByte[1] = (byte)((c.B & 0xf1) | ((c.G & 0xfc) >> 5));
                        ColorToArray(c, colorByte);
                        fileStream.Write(colorByte,0,2);
                    }
                fileStream.Close();
            }
        }
    }
}
