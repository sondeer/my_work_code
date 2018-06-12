namespace ShowWave
{
    partial class ShowWaveForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            Iocomp.Classes.PlotChannelTrace plotChannelTrace1 = new Iocomp.Classes.PlotChannelTrace();
            Iocomp.Classes.PlotDataCursorXY plotDataCursorXY1 = new Iocomp.Classes.PlotDataCursorXY();
            Iocomp.Classes.PlotDataView plotDataView1 = new Iocomp.Classes.PlotDataView();
            Iocomp.Classes.PlotLabelBasic plotLabelBasic1 = new Iocomp.Classes.PlotLabelBasic();
            Iocomp.Classes.PlotLegendBasic plotLegendBasic1 = new Iocomp.Classes.PlotLegendBasic();
            Iocomp.Classes.PlotXAxis plotXAxis1 = new Iocomp.Classes.PlotXAxis();
            Iocomp.Classes.PlotYAxis plotYAxis1 = new Iocomp.Classes.PlotYAxis();
            this.plot1 = new Iocomp.Instrumentation.Plotting.Plot();
            this.panel1 = new System.Windows.Forms.Panel();
            this.baudRateComboBox = new System.Windows.Forms.ComboBox();
            this.startButton = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.portComboBox = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.saveDataButton = new System.Windows.Forms.Button();
            this.loadDataButton = new System.Windows.Forms.Button();
            this.clearDataButton = new System.Windows.Forms.Button();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // plot1
            // 
            this.plot1.LoadingBegin();
            plotChannelTrace1.Color = System.Drawing.Color.Red;
            plotChannelTrace1.Name = "ID0";
            plotChannelTrace1.TitleText = "ID0";
            this.plot1.Channels.Add(plotChannelTrace1);
            plotDataCursorXY1.Hint.Fill.Pen.Color = System.Drawing.SystemColors.InfoText;
            plotDataCursorXY1.Name = "Data-Cursor 1";
            plotDataCursorXY1.TitleText = "Data-Cursor 1";
            this.plot1.DataCursors.Add(plotDataCursorXY1);
            plotDataView1.Name = "Data-View 1";
            plotDataView1.TitleText = "Data-View 1";
            this.plot1.DataViews.Add(plotDataView1);
            this.plot1.Dock = System.Windows.Forms.DockStyle.Fill;
            plotLabelBasic1.DockOrder = 0;
            plotLabelBasic1.Name = "Label 1";
            plotLabelBasic1.TitleText = "Label 1";
            this.plot1.Labels.Add(plotLabelBasic1);
            plotLegendBasic1.DockOrder = 0;
            plotLegendBasic1.Name = "Legend 1";
            plotLegendBasic1.TitleText = "Legend 1";
            this.plot1.Legends.Add(plotLegendBasic1);
            this.plot1.Location = new System.Drawing.Point(0, 0);
            this.plot1.Name = "plot1";
            this.plot1.Size = new System.Drawing.Size(1257, 340);
            this.plot1.TabIndex = 1;
            plotXAxis1.DockOrder = 0;
            plotXAxis1.Name = "X-Axis 1";
            plotXAxis1.Title.Text = "X-Axis 1";
            this.plot1.XAxes.Add(plotXAxis1);
            plotYAxis1.DockOrder = 0;
            plotYAxis1.Name = "Y-Axis 1";
            plotYAxis1.Title.Text = "Y-Axis 1";
            this.plot1.YAxes.Add(plotYAxis1);
            this.plot1.LoadingEnd();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.clearDataButton);
            this.panel1.Controls.Add(this.loadDataButton);
            this.panel1.Controls.Add(this.saveDataButton);
            this.panel1.Controls.Add(this.baudRateComboBox);
            this.panel1.Controls.Add(this.startButton);
            this.panel1.Controls.Add(this.label2);
            this.panel1.Controls.Add(this.portComboBox);
            this.panel1.Controls.Add(this.label1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(1257, 39);
            this.panel1.TabIndex = 2;
            // 
            // baudRateComboBox
            // 
            this.baudRateComboBox.FormattingEnabled = true;
            this.baudRateComboBox.Location = new System.Drawing.Point(222, 9);
            this.baudRateComboBox.Name = "baudRateComboBox";
            this.baudRateComboBox.Size = new System.Drawing.Size(77, 20);
            this.baudRateComboBox.TabIndex = 5;
            // 
            // startButton
            // 
            this.startButton.Location = new System.Drawing.Point(334, 7);
            this.startButton.Name = "startButton";
            this.startButton.Size = new System.Drawing.Size(75, 23);
            this.startButton.TabIndex = 4;
            this.startButton.Text = "开始";
            this.startButton.UseVisualStyleBackColor = true;
            this.startButton.Click += new System.EventHandler(this.startButton_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(175, 13);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(41, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "波特率";
            // 
            // portComboBox
            // 
            this.portComboBox.FormattingEnabled = true;
            this.portComboBox.Location = new System.Drawing.Point(66, 9);
            this.portComboBox.Name = "portComboBox";
            this.portComboBox.Size = new System.Drawing.Size(61, 20);
            this.portComboBox.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(19, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "串口号";
            // 
            // saveDataButton
            // 
            this.saveDataButton.Location = new System.Drawing.Point(518, 7);
            this.saveDataButton.Name = "saveDataButton";
            this.saveDataButton.Size = new System.Drawing.Size(75, 23);
            this.saveDataButton.TabIndex = 6;
            this.saveDataButton.Text = "保存数据";
            this.saveDataButton.UseVisualStyleBackColor = true;
            this.saveDataButton.Click += new System.EventHandler(this.saveDataButton_Click);
            // 
            // loadDataButton
            // 
            this.loadDataButton.Location = new System.Drawing.Point(610, 7);
            this.loadDataButton.Name = "loadDataButton";
            this.loadDataButton.Size = new System.Drawing.Size(75, 23);
            this.loadDataButton.TabIndex = 7;
            this.loadDataButton.Text = "加载数据";
            this.loadDataButton.UseVisualStyleBackColor = true;
            this.loadDataButton.Click += new System.EventHandler(this.loadDataButton_Click);
            // 
            // clearDataButton
            // 
            this.clearDataButton.Location = new System.Drawing.Point(426, 7);
            this.clearDataButton.Name = "clearDataButton";
            this.clearDataButton.Size = new System.Drawing.Size(75, 23);
            this.clearDataButton.TabIndex = 8;
            this.clearDataButton.Text = "清除数据";
            this.clearDataButton.UseVisualStyleBackColor = true;
            this.clearDataButton.Click += new System.EventHandler(this.clearDataButton_Click);
            // 
            // ShowWaveForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1257, 340);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.plot1);
            this.Name = "ShowWaveForm";
            this.Text = "波形显示";
            this.Load += new System.EventHandler(this.ShowWaveForm_Load);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private Iocomp.Instrumentation.Plotting.Plot plot1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.ComboBox baudRateComboBox;
        private System.Windows.Forms.Button startButton;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox portComboBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button loadDataButton;
        private System.Windows.Forms.Button saveDataButton;
        private System.Windows.Forms.Button clearDataButton;
    }
}

