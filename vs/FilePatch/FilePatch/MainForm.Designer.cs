namespace FilePatch
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
            this.srcTextBox = new System.Windows.Forms.TextBox();
            this.srcButton = new System.Windows.Forms.Button();
            this.disButton = new System.Windows.Forms.Button();
            this.disTextBox = new System.Windows.Forms.TextBox();
            this.showStateTextBox = new System.Windows.Forms.TextBox();
            this.patchButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // srcTextBox
            // 
            this.srcTextBox.Location = new System.Drawing.Point(12, 12);
            this.srcTextBox.Name = "srcTextBox";
            this.srcTextBox.Size = new System.Drawing.Size(476, 21);
            this.srcTextBox.TabIndex = 0;
            // 
            // srcButton
            // 
            this.srcButton.Location = new System.Drawing.Point(511, 9);
            this.srcButton.Name = "srcButton";
            this.srcButton.Size = new System.Drawing.Size(75, 23);
            this.srcButton.TabIndex = 1;
            this.srcButton.Text = "源文件夹";
            this.srcButton.UseVisualStyleBackColor = true;
            this.srcButton.Click += new System.EventHandler(this.srcButton_Click);
            // 
            // disButton
            // 
            this.disButton.Location = new System.Drawing.Point(511, 49);
            this.disButton.Name = "disButton";
            this.disButton.Size = new System.Drawing.Size(75, 23);
            this.disButton.TabIndex = 3;
            this.disButton.Text = "目标文件夹";
            this.disButton.UseVisualStyleBackColor = true;
            this.disButton.Click += new System.EventHandler(this.disButton_Click);
            // 
            // disTextBox
            // 
            this.disTextBox.Location = new System.Drawing.Point(12, 52);
            this.disTextBox.Name = "disTextBox";
            this.disTextBox.Size = new System.Drawing.Size(476, 21);
            this.disTextBox.TabIndex = 2;
            // 
            // showStateTextBox
            // 
            this.showStateTextBox.Location = new System.Drawing.Point(12, 99);
            this.showStateTextBox.Multiline = true;
            this.showStateTextBox.Name = "showStateTextBox";
            this.showStateTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.showStateTextBox.Size = new System.Drawing.Size(718, 435);
            this.showStateTextBox.TabIndex = 4;
            this.showStateTextBox.WordWrap = false;
            // 
            // patchButton
            // 
            this.patchButton.Location = new System.Drawing.Point(615, 9);
            this.patchButton.Name = "patchButton";
            this.patchButton.Size = new System.Drawing.Size(115, 63);
            this.patchButton.TabIndex = 5;
            this.patchButton.Text = "开始Patch";
            this.patchButton.UseVisualStyleBackColor = true;
            this.patchButton.Click += new System.EventHandler(this.patchButton_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(750, 570);
            this.Controls.Add(this.patchButton);
            this.Controls.Add(this.showStateTextBox);
            this.Controls.Add(this.disButton);
            this.Controls.Add(this.disTextBox);
            this.Controls.Add(this.srcButton);
            this.Controls.Add(this.srcTextBox);
            this.Name = "MainForm";
            this.Text = "FilePatch";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox srcTextBox;
        private System.Windows.Forms.Button srcButton;
        private System.Windows.Forms.Button disButton;
        private System.Windows.Forms.TextBox disTextBox;
        private System.Windows.Forms.TextBox showStateTextBox;
        private System.Windows.Forms.Button patchButton;
    }
}

