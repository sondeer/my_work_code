namespace GetLedShowArray
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
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.cleanButton = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.ledPanelControl3 = new GetLedShowArray.LedPanelControl();
            this.ledPanelControl2 = new GetLedShowArray.LedPanelControl();
            this.ledPanelControl1 = new GetLedShowArray.LedPanelControl();
            this.SuspendLayout();
            // 
            // cleanButton
            // 
            this.cleanButton.Location = new System.Drawing.Point(0, 329);
            this.cleanButton.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.cleanButton.Name = "cleanButton";
            this.cleanButton.Size = new System.Drawing.Size(92, 25);
            this.cleanButton.TabIndex = 3;
            this.cleanButton.Text = "清空所有LED";
            this.cleanButton.UseVisualStyleBackColor = true;
            this.cleanButton.Click += new System.EventHandler(this.cleanButton_Click);
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(0, 365);
            this.textBox1.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(980, 163);
            this.textBox1.TabIndex = 4;
            // 
            // ledPanelControl3
            // 
            this.ledPanelControl3.Location = new System.Drawing.Point(660, 0);
            this.ledPanelControl3.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.ledPanelControl3.Name = "ledPanelControl3";
            this.ledPanelControl3.Size = new System.Drawing.Size(320, 320);
            this.ledPanelControl3.TabIndex = 2;
            // 
            // ledPanelControl2
            // 
            this.ledPanelControl2.Location = new System.Drawing.Point(330, 0);
            this.ledPanelControl2.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.ledPanelControl2.Name = "ledPanelControl2";
            this.ledPanelControl2.Size = new System.Drawing.Size(320, 320);
            this.ledPanelControl2.TabIndex = 1;
            // 
            // ledPanelControl1
            // 
            this.ledPanelControl1.Location = new System.Drawing.Point(0, 0);
            this.ledPanelControl1.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.ledPanelControl1.Name = "ledPanelControl1";
            this.ledPanelControl1.Size = new System.Drawing.Size(320, 320);
            this.ledPanelControl1.TabIndex = 0;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(993, 541);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.cleanButton);
            this.Controls.Add(this.ledPanelControl3);
            this.Controls.Add(this.ledPanelControl2);
            this.Controls.Add(this.ledPanelControl1);
            this.Margin = new System.Windows.Forms.Padding(2, 2, 2, 2);
            this.Name = "MainForm";
            this.Text = "获取图像数组";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private LedPanelControl ledPanelControl1;
        private LedPanelControl ledPanelControl2;
        private LedPanelControl ledPanelControl3;
        private System.Windows.Forms.Button cleanButton;
        private System.Windows.Forms.TextBox textBox1;
    }
}

