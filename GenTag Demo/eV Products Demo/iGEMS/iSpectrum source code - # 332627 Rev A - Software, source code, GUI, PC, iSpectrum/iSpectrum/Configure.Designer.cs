namespace iSpectrum
{
    partial class Configure
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
            this.CommandReset = new System.Windows.Forms.Button();
            this.TextLLD = new System.Windows.Forms.TextBox();
            this.TextVB = new System.Windows.Forms.TextBox();
            this.cmbGain = new System.Windows.Forms.ComboBox();
            this.cmdExit = new System.Windows.Forms.Button();
            this.cmbPeakingTime = new System.Windows.Forms.ComboBox();
            this.LabelCDT = new System.Windows.Forms.Label();
            this.LabelCSN = new System.Windows.Forms.Label();
            this.Label18 = new System.Windows.Forms.Label();
            this.LabelALLD = new System.Windows.Forms.Label();
            this.LabelAG = new System.Windows.Forms.Label();
            this.LabelAB = new System.Windows.Forms.Label();
            this.LabelAPT = new System.Windows.Forms.Label();
            this.LabelADT = new System.Windows.Forms.Label();
            this.LabelASN = new System.Windows.Forms.Label();
            this.Label10 = new System.Windows.Forms.Label();
            this.Label9 = new System.Windows.Forms.Label();
            this.LabelSDT = new System.Windows.Forms.Label();
            this.LabelSPT = new System.Windows.Forms.Label();
            this.LabelSSN = new System.Windows.Forms.Label();
            this.LabelSB = new System.Windows.Forms.Label();
            this.LabelSG = new System.Windows.Forms.Label();
            this.LabelSLLD = new System.Windows.Forms.Label();
            this.Label2 = new System.Windows.Forms.Label();
            this.Label1 = new System.Windows.Forms.Label();
            this.Label20 = new System.Windows.Forms.Label();
            this.Label25 = new System.Windows.Forms.Label();
            this.Label13 = new System.Windows.Forms.Label();
            this.Label22 = new System.Windows.Forms.Label();
            this.Label26 = new System.Windows.Forms.Label();
            this.Label33 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.errorProvider1 = new System.Windows.Forms.ErrorProvider(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).BeginInit();
            this.SuspendLayout();
            // 
            // CommandReset
            // 
            this.CommandReset.BackColor = System.Drawing.SystemColors.Control;
            this.CommandReset.CausesValidation = false;
            this.CommandReset.Cursor = System.Windows.Forms.Cursors.Default;
            this.CommandReset.ForeColor = System.Drawing.SystemColors.ControlText;
            this.CommandReset.Location = new System.Drawing.Point(115, 232);
            this.CommandReset.Name = "CommandReset";
            this.CommandReset.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.CommandReset.Size = new System.Drawing.Size(144, 27);
            this.CommandReset.TabIndex = 6;
            this.CommandReset.Text = "Reset to Factory Defaults";
            this.CommandReset.UseVisualStyleBackColor = false;
            this.CommandReset.Click += new System.EventHandler(this.CommandReset_Click);
            // 
            // TextLLD
            // 
            this.TextLLD.AcceptsReturn = true;
            this.TextLLD.BackColor = System.Drawing.SystemColors.Window;
            this.TextLLD.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.TextLLD.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.TextLLD.ForeColor = System.Drawing.SystemColors.WindowText;
            this.TextLLD.Location = new System.Drawing.Point(157, 185);
            this.TextLLD.MaxLength = 0;
            this.TextLLD.Name = "TextLLD";
            this.TextLLD.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.TextLLD.Size = new System.Drawing.Size(79, 21);
            this.TextLLD.TabIndex = 4;
            this.TextLLD.Enter += new System.EventHandler(this.TextLLD_Enter);
            this.TextLLD.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextLLD_KeyPress);
            this.TextLLD.Validating += new System.ComponentModel.CancelEventHandler(this.TextLLD_Validating);
            this.TextLLD.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TextLLD_KeyDown);
            // 
            // TextVB
            // 
            this.TextVB.AcceptsReturn = true;
            this.TextVB.BackColor = System.Drawing.SystemColors.Window;
            this.TextVB.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.TextVB.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.TextVB.ForeColor = System.Drawing.SystemColors.WindowText;
            this.TextVB.Location = new System.Drawing.Point(157, 157);
            this.TextVB.MaxLength = 0;
            this.TextVB.Name = "TextVB";
            this.TextVB.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.TextVB.Size = new System.Drawing.Size(79, 21);
            this.TextVB.TabIndex = 3;
            this.TextVB.Enter += new System.EventHandler(this.TextVB_Enter);
            this.TextVB.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextVB_KeyPress);
            this.TextVB.Validating += new System.ComponentModel.CancelEventHandler(this.TextVB_Validating);
            this.TextVB.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TextVB_KeyDown);
            // 
            // cmbGain
            // 
            this.cmbGain.BackColor = System.Drawing.SystemColors.Window;
            this.cmbGain.Cursor = System.Windows.Forms.Cursors.Default;
            this.cmbGain.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbGain.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.cmbGain.ForeColor = System.Drawing.SystemColors.WindowText;
            this.cmbGain.Items.AddRange(new object[] {
            "18",
            "36"});
            this.cmbGain.Location = new System.Drawing.Point(157, 131);
            this.cmbGain.Name = "cmbGain";
            this.cmbGain.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.cmbGain.Size = new System.Drawing.Size(87, 24);
            this.cmbGain.TabIndex = 2;
            this.cmbGain.SelectionChangeCommitted += new System.EventHandler(this.cmbGain_SelectionChangeCommitted);
            this.cmbGain.SelectedIndexChanged += new System.EventHandler(this.cmbGain_SelectedIndexChanged);
            // 
            // cmdExit
            // 
            this.cmdExit.BackColor = System.Drawing.SystemColors.Control;
            this.cmdExit.Cursor = System.Windows.Forms.Cursors.Default;
            this.cmdExit.ForeColor = System.Drawing.SystemColors.ControlText;
            this.cmdExit.Location = new System.Drawing.Point(320, 232);
            this.cmdExit.Name = "cmdExit";
            this.cmdExit.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.cmdExit.Size = new System.Drawing.Size(73, 27);
            this.cmdExit.TabIndex = 5;
            this.cmdExit.Text = "Exit";
            this.cmdExit.UseVisualStyleBackColor = false;
            this.cmdExit.Click += new System.EventHandler(this.cmdExit_Click);
            // 
            // cmbPeakingTime
            // 
            this.cmbPeakingTime.BackColor = System.Drawing.SystemColors.Window;
            this.cmbPeakingTime.Cursor = System.Windows.Forms.Cursors.Default;
            this.cmbPeakingTime.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbPeakingTime.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.cmbPeakingTime.ForeColor = System.Drawing.SystemColors.WindowText;
            this.cmbPeakingTime.Items.AddRange(new object[] {
            "1.2",
            "2.4"});
            this.cmbPeakingTime.Location = new System.Drawing.Point(157, 105);
            this.cmbPeakingTime.Name = "cmbPeakingTime";
            this.cmbPeakingTime.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.cmbPeakingTime.Size = new System.Drawing.Size(87, 24);
            this.cmbPeakingTime.TabIndex = 1;
            this.cmbPeakingTime.SelectionChangeCommitted += new System.EventHandler(this.cmbPeakingTime_SelectionChangeCommitted);
            this.cmbPeakingTime.SelectedIndexChanged += new System.EventHandler(this.cmbPeakingTime_SelectedIndexChanged);
            // 
            // LabelCDT
            // 
            this.LabelCDT.BackColor = System.Drawing.Color.Transparent;
            this.LabelCDT.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelCDT.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelCDT.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LabelCDT.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelCDT.Location = new System.Drawing.Point(158, 79);
            this.LabelCDT.Name = "LabelCDT";
            this.LabelCDT.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelCDT.Size = new System.Drawing.Size(108, 16);
            this.LabelCDT.TabIndex = 62;
            // 
            // LabelCSN
            // 
            this.LabelCSN.BackColor = System.Drawing.Color.Transparent;
            this.LabelCSN.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelCSN.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelCSN.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LabelCSN.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelCSN.Location = new System.Drawing.Point(157, 53);
            this.LabelCSN.Name = "LabelCSN";
            this.LabelCSN.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelCSN.Size = new System.Drawing.Size(87, 17);
            this.LabelCSN.TabIndex = 61;
            // 
            // Label18
            // 
            this.Label18.BackColor = System.Drawing.SystemColors.Control;
            this.Label18.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label18.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label18.Location = new System.Drawing.Point(155, 10);
            this.Label18.Name = "Label18";
            this.Label18.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label18.Size = new System.Drawing.Size(89, 32);
            this.Label18.TabIndex = 60;
            this.Label18.Text = "Current Settings";
            // 
            // LabelALLD
            // 
            this.LabelALLD.BackColor = System.Drawing.Color.Transparent;
            this.LabelALLD.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelALLD.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelALLD.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LabelALLD.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelALLD.Location = new System.Drawing.Point(272, 184);
            this.LabelALLD.Name = "LabelALLD";
            this.LabelALLD.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelALLD.Size = new System.Drawing.Size(63, 16);
            this.LabelALLD.TabIndex = 59;
            // 
            // LabelAG
            // 
            this.LabelAG.BackColor = System.Drawing.Color.Transparent;
            this.LabelAG.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelAG.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelAG.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LabelAG.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelAG.Location = new System.Drawing.Point(272, 130);
            this.LabelAG.Name = "LabelAG";
            this.LabelAG.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelAG.Size = new System.Drawing.Size(55, 16);
            this.LabelAG.TabIndex = 58;
            // 
            // LabelAB
            // 
            this.LabelAB.BackColor = System.Drawing.Color.Transparent;
            this.LabelAB.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelAB.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelAB.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LabelAB.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelAB.Location = new System.Drawing.Point(272, 156);
            this.LabelAB.Name = "LabelAB";
            this.LabelAB.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelAB.Size = new System.Drawing.Size(63, 16);
            this.LabelAB.TabIndex = 57;
            // 
            // LabelAPT
            // 
            this.LabelAPT.BackColor = System.Drawing.Color.Transparent;
            this.LabelAPT.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelAPT.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelAPT.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LabelAPT.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelAPT.Location = new System.Drawing.Point(272, 104);
            this.LabelAPT.Name = "LabelAPT";
            this.LabelAPT.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelAPT.Size = new System.Drawing.Size(55, 16);
            this.LabelAPT.TabIndex = 56;
            // 
            // LabelADT
            // 
            this.LabelADT.BackColor = System.Drawing.Color.Transparent;
            this.LabelADT.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelADT.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelADT.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LabelADT.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelADT.Location = new System.Drawing.Point(272, 78);
            this.LabelADT.Name = "LabelADT";
            this.LabelADT.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelADT.Size = new System.Drawing.Size(103, 16);
            this.LabelADT.TabIndex = 55;
            // 
            // LabelASN
            // 
            this.LabelASN.BackColor = System.Drawing.Color.Transparent;
            this.LabelASN.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelASN.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelASN.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LabelASN.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelASN.Location = new System.Drawing.Point(272, 52);
            this.LabelASN.Name = "LabelASN";
            this.LabelASN.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelASN.Size = new System.Drawing.Size(87, 17);
            this.LabelASN.TabIndex = 49;
            // 
            // Label10
            // 
            this.Label10.BackColor = System.Drawing.SystemColors.Control;
            this.Label10.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label10.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label10.Location = new System.Drawing.Point(385, 26);
            this.Label10.Name = "Label10";
            this.Label10.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label10.Size = new System.Drawing.Size(100, 13);
            this.Label10.TabIndex = 48;
            this.Label10.Text = "(Green)";
            // 
            // Label9
            // 
            this.Label9.BackColor = System.Drawing.SystemColors.Control;
            this.Label9.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label9.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label9.Location = new System.Drawing.Point(270, 26);
            this.Label9.Name = "Label9";
            this.Label9.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label9.Size = new System.Drawing.Size(103, 13);
            this.Label9.TabIndex = 47;
            this.Label9.Text = "(Blue)";
            // 
            // LabelSDT
            // 
            this.LabelSDT.BackColor = System.Drawing.Color.Transparent;
            this.LabelSDT.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelSDT.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelSDT.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LabelSDT.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelSDT.Location = new System.Drawing.Point(385, 78);
            this.LabelSDT.Name = "LabelSDT";
            this.LabelSDT.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelSDT.Size = new System.Drawing.Size(108, 16);
            this.LabelSDT.TabIndex = 46;
            // 
            // LabelSPT
            // 
            this.LabelSPT.BackColor = System.Drawing.Color.Transparent;
            this.LabelSPT.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelSPT.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelSPT.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LabelSPT.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelSPT.Location = new System.Drawing.Point(385, 104);
            this.LabelSPT.Name = "LabelSPT";
            this.LabelSPT.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelSPT.Size = new System.Drawing.Size(60, 16);
            this.LabelSPT.TabIndex = 45;
            // 
            // LabelSSN
            // 
            this.LabelSSN.BackColor = System.Drawing.Color.Transparent;
            this.LabelSSN.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelSSN.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelSSN.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LabelSSN.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelSSN.Location = new System.Drawing.Point(385, 52);
            this.LabelSSN.Name = "LabelSSN";
            this.LabelSSN.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelSSN.Size = new System.Drawing.Size(92, 17);
            this.LabelSSN.TabIndex = 44;
            // 
            // LabelSB
            // 
            this.LabelSB.BackColor = System.Drawing.Color.Transparent;
            this.LabelSB.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelSB.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelSB.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LabelSB.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelSB.Location = new System.Drawing.Point(385, 156);
            this.LabelSB.Name = "LabelSB";
            this.LabelSB.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelSB.Size = new System.Drawing.Size(68, 16);
            this.LabelSB.TabIndex = 43;
            // 
            // LabelSG
            // 
            this.LabelSG.BackColor = System.Drawing.Color.Transparent;
            this.LabelSG.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelSG.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelSG.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LabelSG.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelSG.Location = new System.Drawing.Point(385, 130);
            this.LabelSG.Name = "LabelSG";
            this.LabelSG.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelSG.Size = new System.Drawing.Size(60, 16);
            this.LabelSG.TabIndex = 42;
            // 
            // LabelSLLD
            // 
            this.LabelSLLD.BackColor = System.Drawing.Color.Transparent;
            this.LabelSLLD.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.LabelSLLD.Cursor = System.Windows.Forms.Cursors.Default;
            this.LabelSLLD.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.LabelSLLD.ForeColor = System.Drawing.SystemColors.ControlText;
            this.LabelSLLD.Location = new System.Drawing.Point(385, 184);
            this.LabelSLLD.Name = "LabelSLLD";
            this.LabelSLLD.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.LabelSLLD.Size = new System.Drawing.Size(68, 16);
            this.LabelSLLD.TabIndex = 41;
            // 
            // Label2
            // 
            this.Label2.BackColor = System.Drawing.SystemColors.Control;
            this.Label2.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label2.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label2.Location = new System.Drawing.Point(385, 10);
            this.Label2.Name = "Label2";
            this.Label2.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label2.Size = new System.Drawing.Size(124, 16);
            this.Label2.TabIndex = 40;
            this.Label2.Text = "Static Spectrum";
            // 
            // Label1
            // 
            this.Label1.BackColor = System.Drawing.SystemColors.Control;
            this.Label1.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label1.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label1.Location = new System.Drawing.Point(272, 10);
            this.Label1.Name = "Label1";
            this.Label1.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label1.Size = new System.Drawing.Size(103, 19);
            this.Label1.TabIndex = 39;
            this.Label1.Text = "Active Spectrum";
            // 
            // Label20
            // 
            this.Label20.BackColor = System.Drawing.SystemColors.Control;
            this.Label20.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label20.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label20.Location = new System.Drawing.Point(13, 80);
            this.Label20.Name = "Label20";
            this.Label20.Size = new System.Drawing.Size(128, 15);
            this.Label20.TabIndex = 38;
            this.Label20.Text = "Detector type =";
            // 
            // Label25
            // 
            this.Label25.BackColor = System.Drawing.SystemColors.Control;
            this.Label25.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label25.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label25.Location = new System.Drawing.Point(12, 108);
            this.Label25.Name = "Label25";
            this.Label25.Size = new System.Drawing.Size(160, 20);
            this.Label25.TabIndex = 37;
            this.Label25.Text = "Peaking time (uSec) =";
            // 
            // Label13
            // 
            this.Label13.BackColor = System.Drawing.SystemColors.Control;
            this.Label13.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label13.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label13.Location = new System.Drawing.Point(12, 54);
            this.Label13.Name = "Label13";
            this.Label13.Size = new System.Drawing.Size(136, 25);
            this.Label13.TabIndex = 36;
            this.Label13.Text = "iGem ID Number =";
            // 
            // Label22
            // 
            this.Label22.BackColor = System.Drawing.SystemColors.Control;
            this.Label22.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label22.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label22.Location = new System.Drawing.Point(11, 158);
            this.Label22.Name = "Label22";
            this.Label22.Size = new System.Drawing.Size(160, 22);
            this.Label22.TabIndex = 35;
            this.Label22.Text = "Negative bias(volts) =";
            // 
            // Label26
            // 
            this.Label26.BackColor = System.Drawing.SystemColors.Control;
            this.Label26.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label26.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label26.Location = new System.Drawing.Point(11, 132);
            this.Label26.Name = "Label26";
            this.Label26.Size = new System.Drawing.Size(168, 20);
            this.Label26.TabIndex = 34;
            this.Label26.Text = "Channel gain (mV/fC) =";
            // 
            // Label33
            // 
            this.Label33.Location = new System.Drawing.Point(0, 0);
            this.Label33.Name = "Label33";
            this.Label33.Size = new System.Drawing.Size(100, 25);
            this.Label33.TabIndex = 64;
            // 
            // label3
            // 
            this.label3.BackColor = System.Drawing.SystemColors.Control;
            this.label3.Cursor = System.Windows.Forms.Cursors.Default;
            this.label3.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label3.Location = new System.Drawing.Point(12, 188);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(160, 22);
            this.label3.TabIndex = 65;
            this.label3.Text = "Threshold (mv) =";
            // 
            // errorProvider1
            // 
            this.errorProvider1.BlinkRate = 200;
            this.errorProvider1.ContainerControl = this;
            // 
            // Configure
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(530, 272);
            this.Controls.Add(this.CommandReset);
            this.Controls.Add(this.TextLLD);
            this.Controls.Add(this.TextVB);
            this.Controls.Add(this.cmbGain);
            this.Controls.Add(this.cmdExit);
            this.Controls.Add(this.cmbPeakingTime);
            this.Controls.Add(this.LabelCDT);
            this.Controls.Add(this.LabelCSN);
            this.Controls.Add(this.Label18);
            this.Controls.Add(this.LabelALLD);
            this.Controls.Add(this.LabelAG);
            this.Controls.Add(this.LabelAB);
            this.Controls.Add(this.LabelAPT);
            this.Controls.Add(this.LabelADT);
            this.Controls.Add(this.LabelASN);
            this.Controls.Add(this.Label10);
            this.Controls.Add(this.Label9);
            this.Controls.Add(this.LabelSDT);
            this.Controls.Add(this.LabelSPT);
            this.Controls.Add(this.LabelSSN);
            this.Controls.Add(this.LabelSB);
            this.Controls.Add(this.LabelSG);
            this.Controls.Add(this.LabelSLLD);
            this.Controls.Add(this.Label2);
            this.Controls.Add(this.Label1);
            this.Controls.Add(this.Label20);
            this.Controls.Add(this.Label25);
            this.Controls.Add(this.Label13);
            this.Controls.Add(this.Label26);
            this.Controls.Add(this.Label33);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.Label22);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "Configure";
            this.Text = "Hardware Configure";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Configure_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.Button CommandReset;
        public System.Windows.Forms.TextBox TextLLD;
        public System.Windows.Forms.TextBox TextVB;
        public System.Windows.Forms.ComboBox cmbGain;
        public System.Windows.Forms.Button cmdExit;
        public System.Windows.Forms.ComboBox cmbPeakingTime;
        public System.Windows.Forms.Label LabelCDT;
        public System.Windows.Forms.Label LabelCSN;
        public System.Windows.Forms.Label Label18;
        public System.Windows.Forms.Label LabelALLD;
        public System.Windows.Forms.Label LabelAG;
        public System.Windows.Forms.Label LabelAB;
        public System.Windows.Forms.Label LabelAPT;
        public System.Windows.Forms.Label LabelADT;
        public System.Windows.Forms.Label LabelASN;
        public System.Windows.Forms.Label Label10;
        public System.Windows.Forms.Label Label9;
        public System.Windows.Forms.Label LabelSDT;
        public System.Windows.Forms.Label LabelSPT;
        public System.Windows.Forms.Label LabelSSN;
        public System.Windows.Forms.Label LabelSB;
        public System.Windows.Forms.Label LabelSG;
        public System.Windows.Forms.Label LabelSLLD;
        public System.Windows.Forms.Label Label2;
        public System.Windows.Forms.Label Label1;
        public System.Windows.Forms.Label Label20;
        public System.Windows.Forms.Label Label25;
        public System.Windows.Forms.Label Label13;
        public System.Windows.Forms.Label Label22;
        public System.Windows.Forms.Label Label26;
        public System.Windows.Forms.Label Label33;
        public System.Windows.Forms.Label label3;
        private System.Windows.Forms.ErrorProvider errorProvider1;
    }
}