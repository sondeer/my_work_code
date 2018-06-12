namespace EKFTest
{
    partial class MainForm
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
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            Iocomp.Classes.PlotChannelTrace plotChannelTrace1 = new Iocomp.Classes.PlotChannelTrace();
            Iocomp.Classes.PlotDataCursorXY plotDataCursorXY1 = new Iocomp.Classes.PlotDataCursorXY();
            Iocomp.Classes.PlotDataView plotDataView1 = new Iocomp.Classes.PlotDataView();
            Iocomp.Classes.PlotLabelBasic plotLabelBasic1 = new Iocomp.Classes.PlotLabelBasic();
            Iocomp.Classes.PlotLegendBasic plotLegendBasic1 = new Iocomp.Classes.PlotLegendBasic();
            Iocomp.Classes.PlotXAxis plotXAxis1 = new Iocomp.Classes.PlotXAxis();
            Iocomp.Classes.PlotYAxis plotYAxis1 = new Iocomp.Classes.PlotYAxis();
            Iocomp.Classes.PlotChannelTrace plotChannelTrace2 = new Iocomp.Classes.PlotChannelTrace();
            Iocomp.Classes.PlotDataCursorXY plotDataCursorXY2 = new Iocomp.Classes.PlotDataCursorXY();
            Iocomp.Classes.PlotDataView plotDataView2 = new Iocomp.Classes.PlotDataView();
            Iocomp.Classes.PlotLabelBasic plotLabelBasic2 = new Iocomp.Classes.PlotLabelBasic();
            Iocomp.Classes.PlotLegendBasic plotLegendBasic2 = new Iocomp.Classes.PlotLegendBasic();
            Iocomp.Classes.PlotXAxis plotXAxis2 = new Iocomp.Classes.PlotXAxis();
            Iocomp.Classes.PlotYAxis plotYAxis2 = new Iocomp.Classes.PlotYAxis();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.plot1 = new Iocomp.Instrumentation.Plotting.Plot();
            this.plot2 = new Iocomp.Instrumentation.Plotting.Plot();
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.beginButton = new System.Windows.Forms.Button();
            this.imageList2 = new System.Windows.Forms.ImageList(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.plot1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.plot2);
            this.splitContainer1.Size = new System.Drawing.Size(1132, 554);
            this.splitContainer1.SplitterDistance = 255;
            this.splitContainer1.TabIndex = 0;
            // 
            // plot1
            // 
            this.plot1.LoadingBegin();
            plotChannelTrace1.Color = System.Drawing.Color.Red;
            plotChannelTrace1.Name = "Channel 1";
            plotChannelTrace1.TitleText = "Channel 1";
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
            this.plot1.Size = new System.Drawing.Size(1132, 255);
            this.plot1.TabIndex = 0;
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
            // plot2
            // 
            this.plot2.LoadingBegin();
            plotChannelTrace2.Color = System.Drawing.Color.Red;
            plotChannelTrace2.Name = "Channel 1";
            plotChannelTrace2.TitleText = "Channel 1";
            this.plot2.Channels.Add(plotChannelTrace2);
            plotDataCursorXY2.Hint.Fill.Pen.Color = System.Drawing.SystemColors.InfoText;
            plotDataCursorXY2.Name = "Data-Cursor 1";
            plotDataCursorXY2.TitleText = "Data-Cursor 1";
            this.plot2.DataCursors.Add(plotDataCursorXY2);
            plotDataView2.Name = "Data-View 1";
            plotDataView2.TitleText = "Data-View 1";
            this.plot2.DataViews.Add(plotDataView2);
            this.plot2.Dock = System.Windows.Forms.DockStyle.Fill;
            plotLabelBasic2.DockOrder = 0;
            plotLabelBasic2.Name = "Label 1";
            plotLabelBasic2.TitleText = "Label 1";
            this.plot2.Labels.Add(plotLabelBasic2);
            plotLegendBasic2.DockOrder = 0;
            plotLegendBasic2.Name = "Legend 1";
            plotLegendBasic2.TitleText = "Legend 1";
            this.plot2.Legends.Add(plotLegendBasic2);
            this.plot2.Location = new System.Drawing.Point(0, 0);
            this.plot2.Name = "plot2";
            this.plot2.Size = new System.Drawing.Size(1132, 295);
            this.plot2.TabIndex = 0;
            plotXAxis2.DockOrder = 0;
            plotXAxis2.Name = "X-Axis 1";
            plotXAxis2.Title.Text = "X-Axis 1";
            this.plot2.XAxes.Add(plotXAxis2);
            plotYAxis2.DockOrder = 0;
            plotYAxis2.Name = "Y-Axis 1";
            plotYAxis2.Title.Text = "Y-Axis 1";
            this.plot2.YAxes.Add(plotYAxis2);
            this.plot2.LoadingEnd();
            // 
            // imageList1
            // 
            this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList1.Images.SetKeyName(0, "");
            this.imageList1.Images.SetKeyName(1, "");
            this.imageList1.Images.SetKeyName(2, "");
            this.imageList1.Images.SetKeyName(3, "");
            this.imageList1.Images.SetKeyName(4, "");
            this.imageList1.Images.SetKeyName(5, "");
            this.imageList1.Images.SetKeyName(6, "");
            this.imageList1.Images.SetKeyName(7, "");
            this.imageList1.Images.SetKeyName(8, "");
            this.imageList1.Images.SetKeyName(9, "");
            this.imageList1.Images.SetKeyName(10, "");
            this.imageList1.Images.SetKeyName(11, "");
            this.imageList1.Images.SetKeyName(12, "");
            this.imageList1.Images.SetKeyName(13, "");
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.beginButton);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.splitContainer1);
            this.splitContainer2.Size = new System.Drawing.Size(1223, 554);
            this.splitContainer2.SplitterDistance = 87;
            this.splitContainer2.TabIndex = 1;
            // 
            // beginButton
            // 
            this.beginButton.Location = new System.Drawing.Point(8, 8);
            this.beginButton.Name = "beginButton";
            this.beginButton.Size = new System.Drawing.Size(75, 23);
            this.beginButton.TabIndex = 0;
            this.beginButton.Text = "开始";
            this.beginButton.UseVisualStyleBackColor = true;
            this.beginButton.Click += new System.EventHandler(this.beginButton_Click);
            // 
            // imageList2
            // 
            this.imageList2.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList2.ImageStream")));
            this.imageList2.TransparentColor = System.Drawing.Color.Transparent;
            this.imageList2.Images.SetKeyName(0, "");
            this.imageList2.Images.SetKeyName(1, "");
            this.imageList2.Images.SetKeyName(2, "");
            this.imageList2.Images.SetKeyName(3, "");
            this.imageList2.Images.SetKeyName(4, "");
            this.imageList2.Images.SetKeyName(5, "");
            this.imageList2.Images.SetKeyName(6, "");
            this.imageList2.Images.SetKeyName(7, "");
            this.imageList2.Images.SetKeyName(8, "");
            this.imageList2.Images.SetKeyName(9, "");
            this.imageList2.Images.SetKeyName(10, "");
            this.imageList2.Images.SetKeyName(11, "");
            this.imageList2.Images.SetKeyName(12, "");
            this.imageList2.Images.SetKeyName(13, "");
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1223, 554);
            this.Controls.Add(this.splitContainer2);
            this.Name = "MainForm";
            this.Text = "Form1";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private Iocomp.Instrumentation.Plotting.Plot plot1;
        private Iocomp.Instrumentation.Plotting.Plot plot2;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.Button beginButton;
        private System.Windows.Forms.ImageList imageList2;



    }
}

