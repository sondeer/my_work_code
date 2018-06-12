using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Collections;
using System.Data;
using System.Drawing;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.IO;

namespace FilePatch
{
    public partial class MainForm : Form
    {
        bool patchStartFlag = false;
        System.Windows.Forms.Timer tmr;
        Queue<string> strQueue;

        public MainForm()
        {
            InitializeComponent();
            strQueue = new Queue<string>();
            tmr = new System.Windows.Forms.Timer();
            tmr.Interval = 10;
            tmr.Tick += new EventHandler(TimerEventHandler);
            tmr.Start();
        }

        private void srcButton_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog();
            folderBrowserDialog.Description = "请选择文件路径";
            folderBrowserDialog.SelectedPath = Environment.CurrentDirectory;

            if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                this.srcTextBox.Text = folderBrowserDialog.SelectedPath;
            }
        }

        private void disButton_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBrowserDialog = new FolderBrowserDialog();
            folderBrowserDialog.Description = "请选择文件路径";
            folderBrowserDialog.SelectedPath = Environment.CurrentDirectory;

            if (folderBrowserDialog.ShowDialog() == DialogResult.OK)
            {
                this.disTextBox.Text = folderBrowserDialog.SelectedPath;
            }
        }

        private void patchButton_Click(object sender, EventArgs e)
        {
            this.showStateTextBox.Clear();
            if (this.srcTextBox.Text == "")
            {
                this.showStateTextBox.AppendText("请选择一个源文件夹！\n");
                MessageBox.Show("请选择一个源文件夹！");
                return;
            }
            if (this.disTextBox.Text == "")
            {
                this.showStateTextBox.AppendText("请选择一个目标文件夹！\n");
                MessageBox.Show("请选择一个目标文件夹！");
                return;
            }

            string srcPath = this.srcTextBox.Text + "\\";
            string disPath = this.disTextBox.Text + "\\";
            if (!File.Exists(srcPath + "Filelist.txt"))
            {
                this.showStateTextBox.AppendText("没有找到Filelist.txt！\n");
                MessageBox.Show("没有找到Filelist.txt！");
                return;
            }

            Thread patchThread = new Thread(new ThreadStart(RunPatchThread));
            patchThread.Start();

        }

        private void RunPatchThread()
        {
            string srcPath = this.srcTextBox.Text + "\\";
            string disPath = this.disTextBox.Text + "\\";

            StreamReader streamReader = new StreamReader(srcPath + "Filelist.txt");
            int mod = -1;
            patchStartFlag = true;

            while (!streamReader.EndOfStream)
            {
                string str = streamReader.ReadLine();
                string dirPath;

                if (str == "")
                    continue;

                while (str[0] == ' ' || str[0] == '\n')
                {
                    if (str == "")
                        break;
                    str = str.Remove(0, 1);
                }

                if (str == "[modify]")
                {
                    this.strQueue.Enqueue("[modify]\n");
                    mod = 0;
                }
                else if (str == "[add]")
                {
                    this.strQueue.Enqueue("[add]\n");
                    mod = 1;
                }
                else if (str == "[remove]")
                {
                    this.strQueue.Enqueue("[remove]\n");
                    mod = 2;
                }
                else if (str == "")
                {

                }
                else
                {
                    switch (mod)
                    {
                        case 0:
                            if (!File.Exists(srcPath + str))
                            {
                                this.strQueue.Enqueue("Error:源文件" + srcPath + str + "丢失！\n");
                                continue;
                            }

                            if (!File.Exists(disPath + str))
                            {
                                this.strQueue.Enqueue("没有找到" + disPath + str + "\n");
                            }
                            else
                            {
                                File.Delete(disPath + str);
                            }
                            dirPath = disPath + str;
                            dirPath = dirPath.Remove(disPath.LastIndexOf('\\'));
                            if(!Directory.Exists(dirPath))
                               Directory.CreateDirectory(dirPath);
                            File.Copy(srcPath + str, disPath + str);
                            this.strQueue.Enqueue("修改"+ disPath + str + "成功\n");
                            break;
                        case 1:
                            if (!File.Exists(srcPath + str))
                            {
                                this.strQueue.Enqueue("Error:源文件" + srcPath + str + "丢失！\n");
                                continue;
                            }
                            if (File.Exists(disPath + str))
                            {
                                this.strQueue.Enqueue("存在" + disPath + str + "\n");
                                File.Delete(disPath + str);
                            }
                            dirPath = disPath + str;
                            dirPath = dirPath.Remove(disPath.LastIndexOf('\\'));
                            if (!Directory.Exists(dirPath))
                                Directory.CreateDirectory(dirPath);
                            File.Copy(srcPath + str, disPath + str);
                            this.strQueue.Enqueue("添加" + disPath + str + "成功\n");
                            break;
                        case 2:
                            if (!File.Exists(disPath + str))
                            {
                                this.strQueue.Enqueue("没有找到" + disPath + str + "\n");
                            }
                            else
                            {
                                this.strQueue.Enqueue("删除" + disPath + str + "成功\n");
                                File.Delete(disPath + str);
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            streamReader.Close();
            patchStartFlag = false;
        }

        private void TimerEventHandler(object sender, EventArgs e)
        {
            if(patchStartFlag)
            {
                this.patchButton.Enabled = false;
            }
            else
            {
                this.patchButton.Enabled = true;
            }

            while (this.strQueue.Count > 0)
                this.showStateTextBox.AppendText(this.strQueue.Dequeue());
        }
    }
}
