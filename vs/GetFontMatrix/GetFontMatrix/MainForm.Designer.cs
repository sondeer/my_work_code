namespace GetFontMatrix
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
            this.fontComboBox = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.pixComboBox = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.inputTextBox = new System.Windows.Forms.TextBox();
            this.blackCheckBox = new System.Windows.Forms.CheckBox();
            this.isHanZiCheckBox = new System.Windows.Forms.CheckBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.saveButton = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.showPixFont = new GetFontMatrix.ShowPixFont();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.mapShowPixFont = new GetFontMatrix.ShowPixFont();
            this.panel4 = new System.Windows.Forms.Panel();
            this.saveMapArrayButton = new System.Windows.Forms.Button();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.mapHeightTextBox = new System.Windows.Forms.TextBox();
            this.mapWidthTextBox = new System.Windows.Forms.TextBox();
            this.loadMapButton = new System.Windows.Forms.Button();
            this.panel3 = new System.Windows.Forms.Panel();
            this.showPictureBox = new System.Windows.Forms.PictureBox();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.panel4.SuspendLayout();
            this.panel3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.showPictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // fontComboBox
            // 
            this.fontComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.fontComboBox.FormattingEnabled = true;
            this.fontComboBox.Location = new System.Drawing.Point(46, 5);
            this.fontComboBox.Name = "fontComboBox";
            this.fontComboBox.Size = new System.Drawing.Size(121, 20);
            this.fontComboBox.TabIndex = 0;
            this.fontComboBox.SelectedIndexChanged += new System.EventHandler(this.fontComboBox_SelectedIndexChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 12);
            this.label1.TabIndex = 1;
            this.label1.Text = "字体";
            // 
            // pixComboBox
            // 
            this.pixComboBox.FormattingEnabled = true;
            this.pixComboBox.Location = new System.Drawing.Point(224, 5);
            this.pixComboBox.Name = "pixComboBox";
            this.pixComboBox.Size = new System.Drawing.Size(48, 20);
            this.pixComboBox.TabIndex = 2;
            this.pixComboBox.SelectedIndexChanged += new System.EventHandler(this.pixComboBox_SelectedIndexChanged);
            this.pixComboBox.TextUpdate += new System.EventHandler(this.pixComboBox_TextUpdate);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(181, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 12);
            this.label2.TabIndex = 3;
            this.label2.Text = "像素";
            // 
            // inputTextBox
            // 
            this.inputTextBox.Dock = System.Windows.Forms.DockStyle.Top;
            this.inputTextBox.Location = new System.Drawing.Point(0, 30);
            this.inputTextBox.Multiline = true;
            this.inputTextBox.Name = "inputTextBox";
            this.inputTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.inputTextBox.Size = new System.Drawing.Size(840, 100);
            this.inputTextBox.TabIndex = 4;
            this.inputTextBox.TextChanged += new System.EventHandler(this.inputTextBox_TextChanged);
            // 
            // blackCheckBox
            // 
            this.blackCheckBox.AutoSize = true;
            this.blackCheckBox.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.blackCheckBox.Location = new System.Drawing.Point(286, 7);
            this.blackCheckBox.Name = "blackCheckBox";
            this.blackCheckBox.Size = new System.Drawing.Size(50, 16);
            this.blackCheckBox.TabIndex = 7;
            this.blackCheckBox.Text = "黑体";
            this.blackCheckBox.UseVisualStyleBackColor = true;
            this.blackCheckBox.CheckedChanged += new System.EventHandler(this.blackCheckBox_CheckedChanged);
            // 
            // isHanZiCheckBox
            // 
            this.isHanZiCheckBox.AutoSize = true;
            this.isHanZiCheckBox.Location = new System.Drawing.Point(350, 7);
            this.isHanZiCheckBox.Name = "isHanZiCheckBox";
            this.isHanZiCheckBox.Size = new System.Drawing.Size(48, 16);
            this.isHanZiCheckBox.TabIndex = 9;
            this.isHanZiCheckBox.Text = "汉字";
            this.isHanZiCheckBox.UseVisualStyleBackColor = true;
            this.isHanZiCheckBox.CheckedChanged += new System.EventHandler(this.isHanZiCheckBox_CheckedChanged);
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.inputTextBox);
            this.panel1.Controls.Add(this.panel2);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(3, 3);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(840, 136);
            this.panel1.TabIndex = 10;
            // 
            // panel2
            // 
            this.panel2.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.panel2.Controls.Add(this.saveButton);
            this.panel2.Controls.Add(this.label1);
            this.panel2.Controls.Add(this.blackCheckBox);
            this.panel2.Controls.Add(this.fontComboBox);
            this.panel2.Controls.Add(this.isHanZiCheckBox);
            this.panel2.Controls.Add(this.label2);
            this.panel2.Controls.Add(this.pixComboBox);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(840, 30);
            this.panel2.TabIndex = 10;
            // 
            // saveButton
            // 
            this.saveButton.Location = new System.Drawing.Point(417, 4);
            this.saveButton.Name = "saveButton";
            this.saveButton.Size = new System.Drawing.Size(75, 23);
            this.saveButton.TabIndex = 10;
            this.saveButton.Text = "保存字模";
            this.saveButton.UseVisualStyleBackColor = true;
            this.saveButton.Click += new System.EventHandler(this.saveButton_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(854, 559);
            this.tabControl1.TabIndex = 11;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.showPixFont);
            this.tabPage1.Controls.Add(this.panel1);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(846, 533);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "取字模";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // showPixFont
            // 
            this.showPixFont.AutoScroll = true;
            this.showPixFont.Dock = System.Windows.Forms.DockStyle.Fill;
            this.showPixFont.DrawFontHeight = 8;
            this.showPixFont.DrawFontWidth = 4;
            this.showPixFont.Location = new System.Drawing.Point(3, 139);
            this.showPixFont.Name = "showPixFont";
            this.showPixFont.ShowArray = null;
            this.showPixFont.ShowMapArray = null;
            this.showPixFont.Size = new System.Drawing.Size(840, 391);
            this.showPixFont.TabIndex = 8;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.mapShowPixFont);
            this.tabPage2.Controls.Add(this.panel4);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(846, 533);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "取图片";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // mapShowPixFont
            // 
            this.mapShowPixFont.AutoScroll = true;
            this.mapShowPixFont.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mapShowPixFont.DrawFontHeight = 8;
            this.mapShowPixFont.DrawFontWidth = 4;
            this.mapShowPixFont.Location = new System.Drawing.Point(229, 3);
            this.mapShowPixFont.Name = "mapShowPixFont";
            this.mapShowPixFont.ShowArray = null;
            this.mapShowPixFont.ShowMapArray = null;
            this.mapShowPixFont.Size = new System.Drawing.Size(614, 527);
            this.mapShowPixFont.TabIndex = 3;
            // 
            // panel4
            // 
            this.panel4.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.panel4.Controls.Add(this.saveMapArrayButton);
            this.panel4.Controls.Add(this.label6);
            this.panel4.Controls.Add(this.label5);
            this.panel4.Controls.Add(this.label4);
            this.panel4.Controls.Add(this.label3);
            this.panel4.Controls.Add(this.mapHeightTextBox);
            this.panel4.Controls.Add(this.mapWidthTextBox);
            this.panel4.Controls.Add(this.loadMapButton);
            this.panel4.Controls.Add(this.panel3);
            this.panel4.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel4.Location = new System.Drawing.Point(3, 3);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(226, 527);
            this.panel4.TabIndex = 2;
            // 
            // saveMapArrayButton
            // 
            this.saveMapArrayButton.Location = new System.Drawing.Point(14, 115);
            this.saveMapArrayButton.Name = "saveMapArrayButton";
            this.saveMapArrayButton.Size = new System.Drawing.Size(86, 23);
            this.saveMapArrayButton.TabIndex = 9;
            this.saveMapArrayButton.Text = "保存图片数组";
            this.saveMapArrayButton.UseVisualStyleBackColor = true;
            this.saveMapArrayButton.Click += new System.EventHandler(this.saveMapArrayButton_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(167, 81);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(29, 12);
            this.label6.TabIndex = 8;
            this.label6.Text = "像素";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(167, 53);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(29, 12);
            this.label5.TabIndex = 7;
            this.label5.Text = "像素";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(14, 81);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(29, 12);
            this.label4.TabIndex = 6;
            this.label4.Text = "高度";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(14, 53);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(29, 12);
            this.label3.TabIndex = 5;
            this.label3.Text = "宽度";
            // 
            // mapHeightTextBox
            // 
            this.mapHeightTextBox.Location = new System.Drawing.Point(61, 77);
            this.mapHeightTextBox.Name = "mapHeightTextBox";
            this.mapHeightTextBox.ReadOnly = true;
            this.mapHeightTextBox.Size = new System.Drawing.Size(100, 21);
            this.mapHeightTextBox.TabIndex = 4;
            // 
            // mapWidthTextBox
            // 
            this.mapWidthTextBox.Location = new System.Drawing.Point(61, 49);
            this.mapWidthTextBox.Name = "mapWidthTextBox";
            this.mapWidthTextBox.ReadOnly = true;
            this.mapWidthTextBox.Size = new System.Drawing.Size(100, 21);
            this.mapWidthTextBox.TabIndex = 3;
            // 
            // loadMapButton
            // 
            this.loadMapButton.Location = new System.Drawing.Point(14, 13);
            this.loadMapButton.Name = "loadMapButton";
            this.loadMapButton.Size = new System.Drawing.Size(75, 23);
            this.loadMapButton.TabIndex = 2;
            this.loadMapButton.Text = "加载图片";
            this.loadMapButton.UseVisualStyleBackColor = true;
            this.loadMapButton.Click += new System.EventHandler(this.loadMapButton_Click);
            // 
            // panel3
            // 
            this.panel3.AutoScroll = true;
            this.panel3.BackColor = System.Drawing.Color.Transparent;
            this.panel3.Controls.Add(this.showPictureBox);
            this.panel3.Location = new System.Drawing.Point(0, 238);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(226, 152);
            this.panel3.TabIndex = 1;
            // 
            // showPictureBox
            // 
            this.showPictureBox.Location = new System.Drawing.Point(0, 0);
            this.showPictureBox.Name = "showPictureBox";
            this.showPictureBox.Size = new System.Drawing.Size(100, 50);
            this.showPictureBox.TabIndex = 0;
            this.showPictureBox.TabStop = false;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(854, 559);
            this.Controls.Add(this.tabControl1);
            this.Name = "MainForm";
            this.Text = "获取字模";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.panel4.ResumeLayout(false);
            this.panel4.PerformLayout();
            this.panel3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.showPictureBox)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ComboBox fontComboBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox pixComboBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox inputTextBox;
        private System.Windows.Forms.CheckBox blackCheckBox;
        private ShowPixFont showPixFont;
        private System.Windows.Forms.CheckBox isHanZiCheckBox;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Button saveButton;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private ShowPixFont mapShowPixFont;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.Button saveMapArrayButton;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox mapHeightTextBox;
        private System.Windows.Forms.TextBox mapWidthTextBox;
        private System.Windows.Forms.Button loadMapButton;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.PictureBox showPictureBox;
    }
}

