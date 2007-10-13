namespace iSpectrum
{
    partial class Calibration
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
            this.Option_P2 = new System.Windows.Forms.RadioButton();
            this.Option_P1 = new System.Windows.Forms.RadioButton();
            this.Command_Exit = new System.Windows.Forms.Button();
            this.Text_E2 = new System.Windows.Forms.TextBox();
            this.Text_Ch2 = new System.Windows.Forms.TextBox();
            this.Text_E1 = new System.Windows.Forms.TextBox();
            this.Text_Ch1 = new System.Windows.Forms.TextBox();
            this.Command_CAL = new System.Windows.Forms.Button();
            this.Label6 = new System.Windows.Forms.Label();
            this.Label5 = new System.Windows.Forms.Label();
            this.Label3 = new System.Windows.Forms.Label();
            this.Label2 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // Option_P2
            // 
            this.Option_P2.BackColor = System.Drawing.SystemColors.Control;
            this.Option_P2.Cursor = System.Windows.Forms.Cursors.Default;
            this.Option_P2.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Option_P2.Location = new System.Drawing.Point(21, 81);
            this.Option_P2.Name = "Option_P2";
            this.Option_P2.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Option_P2.Size = new System.Drawing.Size(110, 18);
            this.Option_P2.TabIndex = 3;
            this.Option_P2.TabStop = true;
            this.Option_P2.Text = "Peak 2";
            this.Option_P2.UseVisualStyleBackColor = false;
            // 
            // Option_P1
            // 
            this.Option_P1.BackColor = System.Drawing.SystemColors.Control;
            this.Option_P1.Cursor = System.Windows.Forms.Cursors.Default;
            this.Option_P1.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Option_P1.Location = new System.Drawing.Point(21, 10);
            this.Option_P1.Name = "Option_P1";
            this.Option_P1.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Option_P1.Size = new System.Drawing.Size(110, 14);
            this.Option_P1.TabIndex = 0;
            this.Option_P1.TabStop = true;
            this.Option_P1.Text = "Peak 1";
            this.Option_P1.UseVisualStyleBackColor = false;
            // 
            // Command_Exit
            // 
            this.Command_Exit.BackColor = System.Drawing.SystemColors.Control;
            this.Command_Exit.Cursor = System.Windows.Forms.Cursors.Default;
            this.Command_Exit.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Command_Exit.Location = new System.Drawing.Point(121, 166);
            this.Command_Exit.Name = "Command_Exit";
            this.Command_Exit.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Command_Exit.Size = new System.Drawing.Size(75, 26);
            this.Command_Exit.TabIndex = 7;
            this.Command_Exit.Text = "Exit";
            this.Command_Exit.UseVisualStyleBackColor = false;
            this.Command_Exit.Click += new System.EventHandler(this.Command_Exit_Click);
            // 
            // Text_E2
            // 
            this.Text_E2.AcceptsReturn = true;
            this.Text_E2.BackColor = System.Drawing.SystemColors.Window;
            this.Text_E2.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.Text_E2.ForeColor = System.Drawing.SystemColors.WindowText;
            this.Text_E2.Location = new System.Drawing.Point(117, 126);
            this.Text_E2.MaxLength = 0;
            this.Text_E2.Name = "Text_E2";
            this.Text_E2.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Text_E2.Size = new System.Drawing.Size(78, 21);
            this.Text_E2.TabIndex = 5;
            this.Text_E2.Text = "0";
            this.Text_E2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.Text_E2_KeyPress);
            this.Text_E2.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Text_E2_KeyDown);
            // 
            // Text_Ch2
            // 
            this.Text_Ch2.AcceptsReturn = true;
            this.Text_Ch2.BackColor = System.Drawing.SystemColors.Window;
            this.Text_Ch2.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.Text_Ch2.ForeColor = System.Drawing.SystemColors.WindowText;
            this.Text_Ch2.Location = new System.Drawing.Point(117, 102);
            this.Text_Ch2.MaxLength = 0;
            this.Text_Ch2.Name = "Text_Ch2";
            this.Text_Ch2.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Text_Ch2.Size = new System.Drawing.Size(78, 21);
            this.Text_Ch2.TabIndex = 4;
            this.Text_Ch2.Text = "0";
            this.Text_Ch2.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.Text_Ch2_KeyPress);
            this.Text_Ch2.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Text_Ch2_KeyDown);
            // 
            // Text_E1
            // 
            this.Text_E1.AcceptsReturn = true;
            this.Text_E1.BackColor = System.Drawing.SystemColors.Window;
            this.Text_E1.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.Text_E1.ForeColor = System.Drawing.SystemColors.WindowText;
            this.Text_E1.Location = new System.Drawing.Point(117, 54);
            this.Text_E1.MaxLength = 0;
            this.Text_E1.Name = "Text_E1";
            this.Text_E1.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Text_E1.Size = new System.Drawing.Size(78, 21);
            this.Text_E1.TabIndex = 2;
            this.Text_E1.Text = "0";
            this.Text_E1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.Text_E1_KeyPress);
            this.Text_E1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Text_E1_KeyDown);
            // 
            // Text_Ch1
            // 
            this.Text_Ch1.AcceptsReturn = true;
            this.Text_Ch1.BackColor = System.Drawing.SystemColors.Window;
            this.Text_Ch1.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.Text_Ch1.ForeColor = System.Drawing.SystemColors.WindowText;
            this.Text_Ch1.Location = new System.Drawing.Point(117, 30);
            this.Text_Ch1.MaxLength = 0;
            this.Text_Ch1.Name = "Text_Ch1";
            this.Text_Ch1.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Text_Ch1.Size = new System.Drawing.Size(78, 21);
            this.Text_Ch1.TabIndex = 1;
            this.Text_Ch1.Text = "0";
            this.Text_Ch1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.Text_Ch1_KeyPress);
            this.Text_Ch1.KeyDown += new System.Windows.Forms.KeyEventHandler(this.Text_Ch1_KeyDown);
            // 
            // Command_CAL
            // 
            this.Command_CAL.BackColor = System.Drawing.SystemColors.Control;
            this.Command_CAL.Cursor = System.Windows.Forms.Cursors.Default;
            this.Command_CAL.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Command_CAL.Location = new System.Drawing.Point(23, 166);
            this.Command_CAL.Name = "Command_CAL";
            this.Command_CAL.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Command_CAL.Size = new System.Drawing.Size(76, 26);
            this.Command_CAL.TabIndex = 6;
            this.Command_CAL.Text = "Calibrate";
            this.Command_CAL.UseVisualStyleBackColor = false;
            this.Command_CAL.Click += new System.EventHandler(this.Command_CAL_Click);
            // 
            // Label6
            // 
            this.Label6.BackColor = System.Drawing.SystemColors.Control;
            this.Label6.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label6.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label6.Location = new System.Drawing.Point(85, 126);
            this.Label6.Name = "Label6";
            this.Label6.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label6.Size = new System.Drawing.Size(70, 18);
            this.Label6.TabIndex = 19;
            this.Label6.Text = "KeV";
            // 
            // Label5
            // 
            this.Label5.BackColor = System.Drawing.SystemColors.Control;
            this.Label5.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label5.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label5.Location = new System.Drawing.Point(45, 102);
            this.Label5.Name = "Label5";
            this.Label5.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label5.Size = new System.Drawing.Size(110, 18);
            this.Label5.TabIndex = 17;
            this.Label5.Text = "ADC Channel";
            // 
            // Label3
            // 
            this.Label3.BackColor = System.Drawing.SystemColors.Control;
            this.Label3.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label3.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label3.Location = new System.Drawing.Point(85, 54);
            this.Label3.Name = "Label3";
            this.Label3.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label3.Size = new System.Drawing.Size(70, 18);
            this.Label3.TabIndex = 15;
            this.Label3.Text = "KeV";
            // 
            // Label2
            // 
            this.Label2.BackColor = System.Drawing.SystemColors.Control;
            this.Label2.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label2.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label2.Location = new System.Drawing.Point(45, 30);
            this.Label2.Name = "Label2";
            this.Label2.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label2.Size = new System.Drawing.Size(110, 18);
            this.Label2.TabIndex = 13;
            this.Label2.Text = "ADC Channel";
            // 
            // Calibration
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(222, 210);
            this.Controls.Add(this.Option_P2);
            this.Controls.Add(this.Option_P1);
            this.Controls.Add(this.Command_Exit);
            this.Controls.Add(this.Text_E2);
            this.Controls.Add(this.Text_Ch2);
            this.Controls.Add(this.Text_E1);
            this.Controls.Add(this.Text_Ch1);
            this.Controls.Add(this.Command_CAL);
            this.Controls.Add(this.Label6);
            this.Controls.Add(this.Label5);
            this.Controls.Add(this.Label3);
            this.Controls.Add(this.Label2);
            this.Name = "Calibration";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Calibration";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Calibration_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.RadioButton Option_P2;
        public System.Windows.Forms.RadioButton Option_P1;
        public System.Windows.Forms.Button Command_Exit;
        public System.Windows.Forms.TextBox Text_E2;
        public System.Windows.Forms.TextBox Text_Ch2;
        public System.Windows.Forms.TextBox Text_E1;
        public System.Windows.Forms.TextBox Text_Ch1;
        public System.Windows.Forms.Button Command_CAL;
        public System.Windows.Forms.Label Label6;
        public System.Windows.Forms.Label Label5;
        public System.Windows.Forms.Label Label3;
        public System.Windows.Forms.Label Label2;
    }
}