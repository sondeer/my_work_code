namespace WriteFirmware
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
            this.label1 = new System.Windows.Forms.Label();
            this.portComboBox = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.filepathTextBox = new System.Windows.Forms.TextBox();
            this.loadButton = new System.Windows.Forms.Button();
            this.writeButton = new System.Windows.Forms.Button();
            this.progressBar = new System.Windows.Forms.ProgressBar();
            this.label3 = new System.Windows.Forms.Label();
            this.bootPathTextBox = new System.Windows.Forms.TextBox();
            this.loadBootButton = new System.Windows.Forms.Button();
            this.saveButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(41, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "端口号";
            // 
            // portComboBox
            // 
            this.portComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.portComboBox.FormattingEnabled = true;
            this.portComboBox.Location = new System.Drawing.Point(59, 5);
            this.portComboBox.Name = "portComboBox";
            this.portComboBox.Size = new System.Drawing.Size(64, 20);
            this.portComboBox.TabIndex = 1;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(129, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(47, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "app文件";
            // 
            // filepathTextBox
            // 
            this.filepathTextBox.Location = new System.Drawing.Point(175, 5);
            this.filepathTextBox.Name = "filepathTextBox";
            this.filepathTextBox.Size = new System.Drawing.Size(259, 21);
            this.filepathTextBox.TabIndex = 3;
            // 
            // loadButton
            // 
            this.loadButton.Location = new System.Drawing.Point(438, 4);
            this.loadButton.Name = "loadButton";
            this.loadButton.Size = new System.Drawing.Size(43, 23);
            this.loadButton.TabIndex = 4;
            this.loadButton.Text = "加载";
            this.loadButton.UseVisualStyleBackColor = true;
            this.loadButton.Click += new System.EventHandler(this.loadButton_Click);
            // 
            // writeButton
            // 
            this.writeButton.Location = new System.Drawing.Point(488, 4);
            this.writeButton.Name = "writeButton";
            this.writeButton.Size = new System.Drawing.Size(43, 23);
            this.writeButton.TabIndex = 5;
            this.writeButton.Text = "烧写";
            this.writeButton.UseVisualStyleBackColor = true;
            this.writeButton.Click += new System.EventHandler(this.writeButton_Click);
            // 
            // progressBar
            // 
            this.progressBar.Location = new System.Drawing.Point(15, 63);
            this.progressBar.Name = "progressBar";
            this.progressBar.Size = new System.Drawing.Size(516, 23);
            this.progressBar.Step = 1;
            this.progressBar.TabIndex = 6;
            this.progressBar.Visible = false;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(13, 39);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(53, 12);
            this.label3.TabIndex = 7;
            this.label3.Text = "boot文件";
            // 
            // bootPathTextBox
            // 
            this.bootPathTextBox.Location = new System.Drawing.Point(72, 35);
            this.bootPathTextBox.Name = "bootPathTextBox";
            this.bootPathTextBox.Size = new System.Drawing.Size(360, 21);
            this.bootPathTextBox.TabIndex = 8;
            // 
            // loadBootButton
            // 
            this.loadBootButton.Location = new System.Drawing.Point(438, 34);
            this.loadBootButton.Name = "loadBootButton";
            this.loadBootButton.Size = new System.Drawing.Size(43, 23);
            this.loadBootButton.TabIndex = 9;
            this.loadBootButton.Text = "加载";
            this.loadBootButton.UseVisualStyleBackColor = true;
            this.loadBootButton.Click += new System.EventHandler(this.loadBootButton_Click);
            // 
            // saveButton
            // 
            this.saveButton.Location = new System.Drawing.Point(488, 34);
            this.saveButton.Name = "saveButton";
            this.saveButton.Size = new System.Drawing.Size(43, 23);
            this.saveButton.TabIndex = 10;
            this.saveButton.Text = "保存";
            this.saveButton.UseVisualStyleBackColor = true;
            this.saveButton.Click += new System.EventHandler(this.saveButton_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(542, 98);
            this.Controls.Add(this.saveButton);
            this.Controls.Add(this.loadBootButton);
            this.Controls.Add(this.bootPathTextBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.progressBar);
            this.Controls.Add(this.writeButton);
            this.Controls.Add(this.loadButton);
            this.Controls.Add(this.filepathTextBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.portComboBox);
            this.Controls.Add(this.label1);
            this.Name = "MainForm";
            this.Text = "烧写固件";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox portComboBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox filepathTextBox;
        private System.Windows.Forms.Button loadButton;
        private System.Windows.Forms.Button writeButton;
        private System.Windows.Forms.ProgressBar progressBar;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox bootPathTextBox;
        private System.Windows.Forms.Button loadBootButton;
        private System.Windows.Forms.Button saveButton;
    }
}

