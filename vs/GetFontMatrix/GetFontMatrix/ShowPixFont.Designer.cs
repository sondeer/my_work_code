namespace GetFontMatrix
{
    partial class ShowPixFont
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
        #region 组件设计器生成的代码

        /// <summary> 
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.showPictureBox = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.showPictureBox)).BeginInit();
            this.SuspendLayout();
            // 
            // showPictureBox
            // 
            this.showPictureBox.Location = new System.Drawing.Point(0, 0);
            this.showPictureBox.Name = "showPictureBox";
            this.showPictureBox.Size = new System.Drawing.Size(100, 50);
            this.showPictureBox.TabIndex = 0;
            this.showPictureBox.TabStop = false;
            // 
            // ShowPixFont
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.Controls.Add(this.showPictureBox);
            this.Name = "ShowPixFont";
            this.Size = new System.Drawing.Size(194, 183);
            ((System.ComponentModel.ISupportInitialize)(this.showPictureBox)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PictureBox showPictureBox;




    }
}
