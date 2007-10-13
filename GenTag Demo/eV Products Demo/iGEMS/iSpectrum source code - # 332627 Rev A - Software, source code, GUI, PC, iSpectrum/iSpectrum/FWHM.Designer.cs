namespace iSpectrum
{
    partial class FWHM
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
            this.Option_RE = new System.Windows.Forms.RadioButton();
            this.Option_RB = new System.Windows.Forms.RadioButton();
            this.Command_Exit = new System.Windows.Forms.Button();
            this.Text_Ch2 = new System.Windows.Forms.TextBox();
            this.Text_Ch1 = new System.Windows.Forms.TextBox();
            this.Command_CAL = new System.Windows.Forms.Button();
            this.Label5 = new System.Windows.Forms.Label();
            this.Label3 = new System.Windows.Forms.Label();
            this.Label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.lblFWHM = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // Option_RE
            // 
            this.Option_RE.BackColor = System.Drawing.SystemColors.Control;
            this.Option_RE.Cursor = System.Windows.Forms.Cursors.Default;
            this.Option_RE.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Option_RE.Location = new System.Drawing.Point(33, 95);
            this.Option_RE.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.Option_RE.Name = "Option_RE";
            this.Option_RE.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Option_RE.Size = new System.Drawing.Size(147, 24);
            this.Option_RE.TabIndex = 1;
            this.Option_RE.TabStop = true;
            this.Option_RE.Text = "ROI End";
            this.Option_RE.UseVisualStyleBackColor = false;
            // 
            // Option_RB
            // 
            this.Option_RB.BackColor = System.Drawing.SystemColors.Control;
            this.Option_RB.Cursor = System.Windows.Forms.Cursors.Default;
            this.Option_RB.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Option_RB.Location = new System.Drawing.Point(33, 16);
            this.Option_RB.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.Option_RB.Name = "Option_RB";
            this.Option_RB.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Option_RB.Size = new System.Drawing.Size(147, 19);
            this.Option_RB.TabIndex = 0;
            this.Option_RB.TabStop = true;
            this.Option_RB.Text = "ROI Begin";
            this.Option_RB.UseVisualStyleBackColor = false;
            // 
            // Command_Exit
            // 
            this.Command_Exit.BackColor = System.Drawing.SystemColors.Control;
            this.Command_Exit.Cursor = System.Windows.Forms.Cursors.Default;
            this.Command_Exit.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Command_Exit.Location = new System.Drawing.Point(164, 236);
            this.Command_Exit.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.Command_Exit.Name = "Command_Exit";
            this.Command_Exit.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Command_Exit.Size = new System.Drawing.Size(100, 35);
            this.Command_Exit.TabIndex = 3;
            this.Command_Exit.Text = "Exit";
            this.Command_Exit.UseVisualStyleBackColor = false;
            this.Command_Exit.Click += new System.EventHandler(this.Command_Exit_Click);
            // 
            // Text_Ch2
            // 
            this.Text_Ch2.AcceptsReturn = true;
            this.Text_Ch2.BackColor = System.Drawing.SystemColors.Window;
            this.Text_Ch2.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.Text_Ch2.ForeColor = System.Drawing.SystemColors.WindowText;
            this.Text_Ch2.Location = new System.Drawing.Point(161, 127);
            this.Text_Ch2.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.Text_Ch2.MaxLength = 0;
            this.Text_Ch2.Name = "Text_Ch2";
            this.Text_Ch2.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Text_Ch2.Size = new System.Drawing.Size(103, 22);
            this.Text_Ch2.TabIndex = 30;
            this.Text_Ch2.Text = "0";
            this.Text_Ch2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.Text_Ch2_KeyPress);
            this.Text_Ch2.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Text_Ch2_KeyDown);
            // 
            // Text_Ch1
            // 
            this.Text_Ch1.AcceptsReturn = true;
            this.Text_Ch1.BackColor = System.Drawing.SystemColors.Window;
            this.Text_Ch1.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.Text_Ch1.ForeColor = System.Drawing.SystemColors.WindowText;
            this.Text_Ch1.Location = new System.Drawing.Point(161, 45);
            this.Text_Ch1.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.Text_Ch1.MaxLength = 0;
            this.Text_Ch1.Name = "Text_Ch1";
            this.Text_Ch1.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Text_Ch1.Size = new System.Drawing.Size(103, 22);
            this.Text_Ch1.TabIndex = 26;
            this.Text_Ch1.Text = "0";
            this.Text_Ch1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.Text_Ch1_KeyPress);
            this.Text_Ch1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Text_Ch1_KeyDown);
            // 
            // Command_CAL
            // 
            this.Command_CAL.BackColor = System.Drawing.SystemColors.Control;
            this.Command_CAL.Cursor = System.Windows.Forms.Cursors.Default;
            this.Command_CAL.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Command_CAL.Location = new System.Drawing.Point(33, 236);
            this.Command_CAL.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.Command_CAL.Name = "Command_CAL";
            this.Command_CAL.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Command_CAL.Size = new System.Drawing.Size(101, 35);
            this.Command_CAL.TabIndex = 2;
            this.Command_CAL.Text = "Calculate";
            this.Command_CAL.UseVisualStyleBackColor = false;
            this.Command_CAL.Click += new System.EventHandler(this.Command_CAL_Click);
            // 
            // Label5
            // 
            this.Label5.BackColor = System.Drawing.SystemColors.Control;
            this.Label5.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label5.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label5.Location = new System.Drawing.Point(65, 123);
            this.Label5.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.Label5.Name = "Label5";
            this.Label5.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label5.Size = new System.Drawing.Size(147, 24);
            this.Label5.TabIndex = 29;
            this.Label5.Text = "ADC Channel";
            // 
            // Label3
            // 
            this.Label3.BackColor = System.Drawing.SystemColors.Control;
            this.Label3.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label3.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label3.Location = new System.Drawing.Point(87, 67);
            this.Label3.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.Label3.Name = "Label3";
            this.Label3.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label3.Size = new System.Drawing.Size(67, 24);
            this.Label3.TabIndex = 27;
            this.Label3.Text = "Or KeV";
            // 
            // Label2
            // 
            this.Label2.BackColor = System.Drawing.SystemColors.Control;
            this.Label2.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label2.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label2.Location = new System.Drawing.Point(65, 43);
            this.Label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.Label2.Name = "Label2";
            this.Label2.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label2.Size = new System.Drawing.Size(147, 24);
            this.Label2.TabIndex = 25;
            this.Label2.Text = "ADC Channel";
            // 
            // label1
            // 
            this.label1.BackColor = System.Drawing.SystemColors.Control;
            this.label1.Cursor = System.Windows.Forms.Cursors.Default;
            this.label1.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label1.Location = new System.Drawing.Point(87, 147);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.label1.Size = new System.Drawing.Size(61, 24);
            this.label1.TabIndex = 36;
            this.label1.Text = "Or KeV";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(30, 191);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(155, 17);
            this.label4.TabIndex = 37;
            this.label4.Text = "Full width half maximum";
            // 
            // lblFWHM
            // 
            this.lblFWHM.AutoSize = true;
            this.lblFWHM.Location = new System.Drawing.Point(192, 191);
            this.lblFWHM.Name = "lblFWHM";
            this.lblFWHM.Size = new System.Drawing.Size(104, 17);
            this.lblFWHM.TabIndex = 38;
            this.lblFWHM.Text = "                        ";
            // 
            // FWHM
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(308, 297);
            this.Controls.Add(this.lblFWHM);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.Option_RE);
            this.Controls.Add(this.Option_RB);
            this.Controls.Add(this.Command_Exit);
            this.Controls.Add(this.Text_Ch2);
            this.Controls.Add(this.Text_Ch1);
            this.Controls.Add(this.Command_CAL);
            this.Controls.Add(this.Label5);
            this.Controls.Add(this.Label3);
            this.Controls.Add(this.Label2);
            this.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.Name = "FWHM";
            this.Text = "FWHM";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FWHM_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.RadioButton Option_RE;
        public System.Windows.Forms.RadioButton Option_RB;
        public System.Windows.Forms.Button Command_Exit;
        public System.Windows.Forms.TextBox Text_Ch2;
        public System.Windows.Forms.TextBox Text_Ch1;
        public System.Windows.Forms.Button Command_CAL;
        public System.Windows.Forms.Label Label5;
        public System.Windows.Forms.Label Label3;
        public System.Windows.Forms.Label Label2;
        public System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label4;
        public System.Windows.Forms.Label lblFWHM;
    }
}