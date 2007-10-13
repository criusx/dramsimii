namespace iSpectrum
{
    partial class Form1
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.Menu_iGEM = new System.Windows.Forms.MenuStrip();
            this.DdToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.LoadActiveSpectrumToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.LoadStaticSpectrumToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.SaveActiveSpectrumToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ClearActiveSpectrumToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ClearStaticSpectrumToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ExitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.CalibrateToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.FWHMToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ConfigureToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.graphToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToJPEGToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.HelpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.IndexToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.AboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.Button_LSS = new System.Windows.Forms.Button();
            this.Button_LAS = new System.Windows.Forms.Button();
            this.Button_SAS = new System.Windows.Forms.Button();
            this.Button_CAS = new System.Windows.Forms.Button();
            this.Button_Stop = new System.Windows.Forms.Button();
            this.Button_Start = new System.Windows.Forms.Button();
            this.Label_CiS = new System.Windows.Forms.Label();
            this.Check_DisStatic = new System.Windows.Forms.CheckBox();
            this.Check_DisLive = new System.Windows.Forms.CheckBox();
            this.Check_EenergyD = new System.Windows.Forms.CheckBox();
            this.Frame3 = new System.Windows.Forms.GroupBox();
            this.TextBox_Counts = new System.Windows.Forms.TextBox();
            this.TextBox_Time = new System.Windows.Forms.TextBox();
            this.Label11 = new System.Windows.Forms.Label();
            this.Label10 = new System.Windows.Forms.Label();
            this.checkBoxCT = new System.Windows.Forms.CheckBox();
            this.checkBoxTC = new System.Windows.Forms.CheckBox();
            this.Check_SpectrumS = new System.Windows.Forms.CheckBox();
            this.Frame2 = new System.Windows.Forms.GroupBox();
            this.TextBox_YMin = new System.Windows.Forms.TextBox();
            this.TextBox_YMax = new System.Windows.Forms.TextBox();
            this.Check_AutoScale = new System.Windows.Forms.CheckBox();
            this.Option_Log = new System.Windows.Forms.RadioButton();
            this.Option_Linear = new System.Windows.Forms.RadioButton();
            this.Label6 = new System.Windows.Forms.Label();
            this.Label5 = new System.Windows.Forms.Label();
            this.Frame1 = new System.Windows.Forms.GroupBox();
            this.Check_XFull = new System.Windows.Forms.CheckBox();
            this.TextBox_XMin = new System.Windows.Forms.TextBox();
            this.TextBox_XMax = new System.Windows.Forms.TextBox();
            this.Label8 = new System.Windows.Forms.Label();
            this.Label7 = new System.Windows.Forms.Label();
            this.Label_ESC = new System.Windows.Forms.Label();
            this.Label_SC = new System.Windows.Forms.Label();
            this.Label_FWHM = new System.Windows.Forms.Label();
            this.Label31 = new System.Windows.Forms.Label();
            this.Label_TC = new System.Windows.Forms.Label();
            this.Label_CPS = new System.Windows.Forms.Label();
            this.Label4 = new System.Windows.Forms.Label();
            this.Label_14 = new System.Windows.Forms.Label();
            this.Labell_CiS = new System.Windows.Forms.Label();
            this.Label1 = new System.Windows.Forms.Label();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.label3 = new System.Windows.Forms.Label();
            this.button_setAxis = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label12 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.Label_Time = new System.Windows.Forms.Label();
            this.timerspectrum = new System.Windows.Forms.Timer(this.components);
            this.timerCollect = new System.Windows.Forms.Timer(this.components);
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.timerHVRamp = new System.Windows.Forms.Timer(this.components);
            this.Button_CSS = new System.Windows.Forms.Button();
            this.Menu_iGEM.SuspendLayout();
            this.Frame3.SuspendLayout();
            this.Frame2.SuspendLayout();
            this.Frame1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // Menu_iGEM
            // 
            this.Menu_iGEM.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.DdToolStripMenuItem,
            this.CalibrateToolStripMenuItem,
            this.FWHMToolStripMenuItem,
            this.ConfigureToolStripMenuItem,
            this.graphToolStripMenuItem,
            this.HelpToolStripMenuItem});
            this.Menu_iGEM.Location = new System.Drawing.Point(0, 0);
            this.Menu_iGEM.Name = "Menu_iGEM";
            this.Menu_iGEM.Size = new System.Drawing.Size(1016, 24);
            this.Menu_iGEM.TabIndex = 1;
            this.Menu_iGEM.Text = "MenuStrip1";
            // 
            // DdToolStripMenuItem
            // 
            this.DdToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.LoadActiveSpectrumToolStripMenuItem,
            this.LoadStaticSpectrumToolStripMenuItem,
            this.SaveActiveSpectrumToolStripMenuItem,
            this.ClearActiveSpectrumToolStripMenuItem,
            this.ClearStaticSpectrumToolStripMenuItem,
            this.ExitToolStripMenuItem});
            this.DdToolStripMenuItem.Name = "DdToolStripMenuItem";
            this.DdToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.DdToolStripMenuItem.Text = "&File";
            // 
            // LoadActiveSpectrumToolStripMenuItem
            // 
            this.LoadActiveSpectrumToolStripMenuItem.Name = "LoadActiveSpectrumToolStripMenuItem";
            this.LoadActiveSpectrumToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
            this.LoadActiveSpectrumToolStripMenuItem.Text = "Load As &Active Spectrum";
            this.LoadActiveSpectrumToolStripMenuItem.Click += new System.EventHandler(this.LoadActiveSpectrumToolStripMenuItem_Click);
            // 
            // LoadStaticSpectrumToolStripMenuItem
            // 
            this.LoadStaticSpectrumToolStripMenuItem.Name = "LoadStaticSpectrumToolStripMenuItem";
            this.LoadStaticSpectrumToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
            this.LoadStaticSpectrumToolStripMenuItem.Text = "Load As S&tatic Spectrum";
            this.LoadStaticSpectrumToolStripMenuItem.Click += new System.EventHandler(this.LoadStaticSpectrumToolStripMenuItem_Click);
            // 
            // SaveActiveSpectrumToolStripMenuItem
            // 
            this.SaveActiveSpectrumToolStripMenuItem.Enabled = false;
            this.SaveActiveSpectrumToolStripMenuItem.Name = "SaveActiveSpectrumToolStripMenuItem";
            this.SaveActiveSpectrumToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
            this.SaveActiveSpectrumToolStripMenuItem.Text = "&Save Active Spectrum";
            this.SaveActiveSpectrumToolStripMenuItem.Click += new System.EventHandler(this.SaveActiveSpectrumToolStripMenuItem_Click);
            // 
            // ClearActiveSpectrumToolStripMenuItem
            // 
            this.ClearActiveSpectrumToolStripMenuItem.Enabled = false;
            this.ClearActiveSpectrumToolStripMenuItem.Name = "ClearActiveSpectrumToolStripMenuItem";
            this.ClearActiveSpectrumToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
            this.ClearActiveSpectrumToolStripMenuItem.Text = "Clear Active Spectrum";
            this.ClearActiveSpectrumToolStripMenuItem.Click += new System.EventHandler(this.ClearActiveSpectrumToolStripMenuItem_Click);
            // 
            // ClearStaticSpectrumToolStripMenuItem
            // 
            this.ClearStaticSpectrumToolStripMenuItem.Enabled = false;
            this.ClearStaticSpectrumToolStripMenuItem.Name = "ClearStaticSpectrumToolStripMenuItem";
            this.ClearStaticSpectrumToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
            this.ClearStaticSpectrumToolStripMenuItem.Text = "Clear Static Spectrum";
            this.ClearStaticSpectrumToolStripMenuItem.Click += new System.EventHandler(this.clearStaticSpectrumToolStripMenuItem_Click);
            // 
            // ExitToolStripMenuItem
            // 
            this.ExitToolStripMenuItem.Name = "ExitToolStripMenuItem";
            this.ExitToolStripMenuItem.Size = new System.Drawing.Size(206, 22);
            this.ExitToolStripMenuItem.Text = "E&xit";
            this.ExitToolStripMenuItem.Click += new System.EventHandler(this.ExitToolStripMenuItem_Click);
            // 
            // CalibrateToolStripMenuItem
            // 
            this.CalibrateToolStripMenuItem.Name = "CalibrateToolStripMenuItem";
            this.CalibrateToolStripMenuItem.Size = new System.Drawing.Size(66, 20);
            this.CalibrateToolStripMenuItem.Text = "&Calibrate";
            this.CalibrateToolStripMenuItem.Click += new System.EventHandler(this.CalibrateToolStripMenuItem_Click);
            // 
            // FWHMToolStripMenuItem
            // 
            this.FWHMToolStripMenuItem.Name = "FWHMToolStripMenuItem";
            this.FWHMToolStripMenuItem.Size = new System.Drawing.Size(56, 20);
            this.FWHMToolStripMenuItem.Text = "F&WHM";
            this.FWHMToolStripMenuItem.Click += new System.EventHandler(this.FWHMToolStripMenuItem_Click);
            // 
            // ConfigureToolStripMenuItem
            // 
            this.ConfigureToolStripMenuItem.Name = "ConfigureToolStripMenuItem";
            this.ConfigureToolStripMenuItem.Size = new System.Drawing.Size(72, 20);
            this.ConfigureToolStripMenuItem.Text = "Co&nfigure";
            this.ConfigureToolStripMenuItem.Click += new System.EventHandler(this.ConfigureToolStripMenuItem_Click);
            // 
            // graphToolStripMenuItem
            // 
            this.graphToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.saveToJPEGToolStripMenuItem});
            this.graphToolStripMenuItem.Name = "graphToolStripMenuItem";
            this.graphToolStripMenuItem.Size = new System.Drawing.Size(51, 20);
            this.graphToolStripMenuItem.Text = "&Graph";
            // 
            // saveToJPEGToolStripMenuItem
            // 
            this.saveToJPEGToolStripMenuItem.Name = "saveToJPEGToolStripMenuItem";
            this.saveToJPEGToolStripMenuItem.Size = new System.Drawing.Size(140, 22);
            this.saveToJPEGToolStripMenuItem.Text = "&Save to JPEG";
            this.saveToJPEGToolStripMenuItem.Click += new System.EventHandler(this.saveToJPEGToolStripMenuItem_Click);
            // 
            // HelpToolStripMenuItem
            // 
            this.HelpToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.IndexToolStripMenuItem,
            this.AboutToolStripMenuItem});
            this.HelpToolStripMenuItem.Name = "HelpToolStripMenuItem";
            this.HelpToolStripMenuItem.Size = new System.Drawing.Size(44, 20);
            this.HelpToolStripMenuItem.Text = "&Help";
            // 
            // IndexToolStripMenuItem
            // 
            this.IndexToolStripMenuItem.Name = "IndexToolStripMenuItem";
            this.IndexToolStripMenuItem.Size = new System.Drawing.Size(107, 22);
            this.IndexToolStripMenuItem.Text = "Index";
            this.IndexToolStripMenuItem.Click += new System.EventHandler(this.IndexToolStripMenuItem_Click);
            // 
            // AboutToolStripMenuItem
            // 
            this.AboutToolStripMenuItem.Name = "AboutToolStripMenuItem";
            this.AboutToolStripMenuItem.Size = new System.Drawing.Size(107, 22);
            this.AboutToolStripMenuItem.Text = "About";
            this.AboutToolStripMenuItem.Click += new System.EventHandler(this.AboutToolStripMenuItem_Click);
            // 
            // Button_LSS
            // 
            this.Button_LSS.BackColor = System.Drawing.SystemColors.Control;
            this.Button_LSS.Cursor = System.Windows.Forms.Cursors.Default;
            this.Button_LSS.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Button_LSS.Location = new System.Drawing.Point(268, 28);
            this.Button_LSS.Name = "Button_LSS";
            this.Button_LSS.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Button_LSS.Size = new System.Drawing.Size(81, 36);
            this.Button_LSS.TabIndex = 3;
            this.Button_LSS.Text = "Load Static Spectrum";
            this.Button_LSS.UseVisualStyleBackColor = false;
            this.Button_LSS.Click += new System.EventHandler(this.Button_LSS_Click);
            // 
            // Button_LAS
            // 
            this.Button_LAS.BackColor = System.Drawing.SystemColors.Control;
            this.Button_LAS.Cursor = System.Windows.Forms.Cursors.Default;
            this.Button_LAS.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Button_LAS.Location = new System.Drawing.Point(172, 28);
            this.Button_LAS.Name = "Button_LAS";
            this.Button_LAS.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Button_LAS.Size = new System.Drawing.Size(81, 36);
            this.Button_LAS.TabIndex = 2;
            this.Button_LAS.Text = "Load Active Spectrum";
            this.Button_LAS.UseVisualStyleBackColor = false;
            this.Button_LAS.Click += new System.EventHandler(this.Button_LAS_Click);
            // 
            // Button_SAS
            // 
            this.Button_SAS.BackColor = System.Drawing.SystemColors.Control;
            this.Button_SAS.Cursor = System.Windows.Forms.Cursors.Default;
            this.Button_SAS.Enabled = false;
            this.Button_SAS.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Button_SAS.Location = new System.Drawing.Point(364, 28);
            this.Button_SAS.Name = "Button_SAS";
            this.Button_SAS.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Button_SAS.Size = new System.Drawing.Size(81, 36);
            this.Button_SAS.TabIndex = 4;
            this.Button_SAS.Text = "Save Active Spectrum";
            this.Button_SAS.UseVisualStyleBackColor = false;
            this.Button_SAS.Click += new System.EventHandler(this.Button_SAS_Click);
            // 
            // Button_CAS
            // 
            this.Button_CAS.BackColor = System.Drawing.SystemColors.Control;
            this.Button_CAS.Cursor = System.Windows.Forms.Cursors.Default;
            this.Button_CAS.Enabled = false;
            this.Button_CAS.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Button_CAS.Location = new System.Drawing.Point(460, 28);
            this.Button_CAS.Name = "Button_CAS";
            this.Button_CAS.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Button_CAS.Size = new System.Drawing.Size(89, 36);
            this.Button_CAS.TabIndex = 5;
            this.Button_CAS.Text = "Clear Active Spectrum";
            this.Button_CAS.UseVisualStyleBackColor = false;
            this.Button_CAS.Click += new System.EventHandler(this.Button_CAS_Click);
            // 
            // Button_Stop
            // 
            this.Button_Stop.BackColor = System.Drawing.SystemColors.Control;
            this.Button_Stop.Cursor = System.Windows.Forms.Cursors.Default;
            this.Button_Stop.Enabled = false;
            this.Button_Stop.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Button_Stop.Location = new System.Drawing.Point(92, 28);
            this.Button_Stop.Name = "Button_Stop";
            this.Button_Stop.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Button_Stop.Size = new System.Drawing.Size(65, 36);
            this.Button_Stop.TabIndex = 1;
            this.Button_Stop.Text = "Stop";
            this.Button_Stop.UseVisualStyleBackColor = false;
            this.Button_Stop.Click += new System.EventHandler(this.Button_Stop_Click);
            // 
            // Button_Start
            // 
            this.Button_Start.BackColor = System.Drawing.SystemColors.Control;
            this.Button_Start.Cursor = System.Windows.Forms.Cursors.Default;
            this.Button_Start.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Button_Start.Location = new System.Drawing.Point(12, 28);
            this.Button_Start.Name = "Button_Start";
            this.Button_Start.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Button_Start.Size = new System.Drawing.Size(65, 36);
            this.Button_Start.TabIndex = 0;
            this.Button_Start.Text = "Start";
            this.Button_Start.UseVisualStyleBackColor = false;
            this.Button_Start.Click += new System.EventHandler(this.Button_Start_Click);
            // 
            // Label_CiS
            // 
            this.Label_CiS.BackColor = System.Drawing.Color.Transparent;
            this.Label_CiS.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label_CiS.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label_CiS.Location = new System.Drawing.Point(160, 600);
            this.Label_CiS.Name = "Label_CiS";
            this.Label_CiS.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label_CiS.Size = new System.Drawing.Size(62, 19);
            this.Label_CiS.TabIndex = 82;
            this.Label_CiS.Text = "0";
            // 
            // Check_DisStatic
            // 
            this.Check_DisStatic.BackColor = System.Drawing.SystemColors.Control;
            this.Check_DisStatic.Checked = true;
            this.Check_DisStatic.CheckState = System.Windows.Forms.CheckState.Checked;
            this.Check_DisStatic.Cursor = System.Windows.Forms.Cursors.Default;
            this.Check_DisStatic.Enabled = false;
            this.Check_DisStatic.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Check_DisStatic.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.Check_DisStatic.Location = new System.Drawing.Point(12, 66);
            this.Check_DisStatic.Name = "Check_DisStatic";
            this.Check_DisStatic.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Check_DisStatic.Size = new System.Drawing.Size(134, 19);
            this.Check_DisStatic.TabIndex = 2;
            this.Check_DisStatic.Text = "Display static spectrum";
            this.Check_DisStatic.UseVisualStyleBackColor = false;
            this.Check_DisStatic.CheckedChanged += new System.EventHandler(this.Check_DisStatic_CheckedChanged);
            // 
            // Check_DisLive
            // 
            this.Check_DisLive.BackColor = System.Drawing.SystemColors.Control;
            this.Check_DisLive.Checked = true;
            this.Check_DisLive.CheckState = System.Windows.Forms.CheckState.Checked;
            this.Check_DisLive.Cursor = System.Windows.Forms.Cursors.Default;
            this.Check_DisLive.Enabled = false;
            this.Check_DisLive.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Check_DisLive.Location = new System.Drawing.Point(12, 101);
            this.Check_DisLive.Name = "Check_DisLive";
            this.Check_DisLive.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Check_DisLive.Size = new System.Drawing.Size(129, 19);
            this.Check_DisLive.TabIndex = 3;
            this.Check_DisLive.Text = "Display live spectrum";
            this.Check_DisLive.UseVisualStyleBackColor = false;
            this.Check_DisLive.CheckStateChanged += new System.EventHandler(this.Check_DisLive_CheckStateChanged);
            // 
            // Check_EenergyD
            // 
            this.Check_EenergyD.BackColor = System.Drawing.SystemColors.Control;
            this.Check_EenergyD.Cursor = System.Windows.Forms.Cursors.Default;
            this.Check_EenergyD.Enabled = false;
            this.Check_EenergyD.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Check_EenergyD.Location = new System.Drawing.Point(12, 45);
            this.Check_EenergyD.Name = "Check_EenergyD";
            this.Check_EenergyD.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Check_EenergyD.Size = new System.Drawing.Size(121, 19);
            this.Check_EenergyD.TabIndex = 1;
            this.Check_EenergyD.Text = "KeV Display Mode";
            this.Check_EenergyD.UseVisualStyleBackColor = false;
            this.Check_EenergyD.CheckedChanged += new System.EventHandler(this.Check_EenergyD_CheckedChanged);
            this.Check_EenergyD.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Check_EenergyD_MouseUp);
            // 
            // Frame3
            // 
            this.Frame3.BackColor = System.Drawing.Color.Transparent;
            this.Frame3.Controls.Add(this.TextBox_Counts);
            this.Frame3.Controls.Add(this.TextBox_Time);
            this.Frame3.Controls.Add(this.Label11);
            this.Frame3.Controls.Add(this.Label10);
            this.Frame3.Controls.Add(this.checkBoxCT);
            this.Frame3.Controls.Add(this.checkBoxTC);
            this.Frame3.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Frame3.Location = new System.Drawing.Point(344, 600);
            this.Frame3.Name = "Frame3";
            this.Frame3.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Frame3.Size = new System.Drawing.Size(124, 119);
            this.Frame3.TabIndex = 6;
            this.Frame3.TabStop = false;
            this.Frame3.Text = "Collection Control";
            // 
            // TextBox_Counts
            // 
            this.TextBox_Counts.AcceptsReturn = true;
            this.TextBox_Counts.BackColor = System.Drawing.SystemColors.Window;
            this.TextBox_Counts.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.TextBox_Counts.ForeColor = System.Drawing.SystemColors.WindowText;
            this.TextBox_Counts.Location = new System.Drawing.Point(39, 83);
            this.TextBox_Counts.MaxLength = 0;
            this.TextBox_Counts.Name = "TextBox_Counts";
            this.TextBox_Counts.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.TextBox_Counts.Size = new System.Drawing.Size(55, 20);
            this.TextBox_Counts.TabIndex = 1;
            this.TextBox_Counts.Text = "0";
            this.TextBox_Counts.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TextBox_Counts_KeyDown);
            this.TextBox_Counts.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextBox_Counts_KeyPress);
            // 
            // TextBox_Time
            // 
            this.TextBox_Time.AcceptsReturn = true;
            this.TextBox_Time.BackColor = System.Drawing.SystemColors.Window;
            this.TextBox_Time.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.TextBox_Time.ForeColor = System.Drawing.SystemColors.WindowText;
            this.TextBox_Time.Location = new System.Drawing.Point(39, 41);
            this.TextBox_Time.MaxLength = 0;
            this.TextBox_Time.Name = "TextBox_Time";
            this.TextBox_Time.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.TextBox_Time.Size = new System.Drawing.Size(55, 20);
            this.TextBox_Time.TabIndex = 0;
            this.TextBox_Time.Text = "0";
            this.TextBox_Time.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TextBox_Time_KeyDown);
            this.TextBox_Time.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextBox_Time_KeyPress);
            // 
            // Label11
            // 
            this.Label11.BackColor = System.Drawing.SystemColors.Control;
            this.Label11.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label11.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label11.Location = new System.Drawing.Point(16, 66);
            this.Label11.Name = "Label11";
            this.Label11.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label11.Size = new System.Drawing.Size(86, 19);
            this.Label11.TabIndex = 27;
            this.Label11.Text = "Total counts";
            // 
            // Label10
            // 
            this.Label10.BackColor = System.Drawing.SystemColors.Control;
            this.Label10.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label10.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label10.Location = new System.Drawing.Point(16, 24);
            this.Label10.Name = "Label10";
            this.Label10.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label10.Size = new System.Drawing.Size(97, 14);
            this.Label10.TabIndex = 2;
            this.Label10.Text = "Collection Time (s)";
            // 
            // checkBoxCT
            // 
            this.checkBoxCT.BackColor = System.Drawing.SystemColors.Control;
            this.checkBoxCT.Cursor = System.Windows.Forms.Cursors.Default;
            this.checkBoxCT.ForeColor = System.Drawing.SystemColors.ControlText;
            this.checkBoxCT.Location = new System.Drawing.Point(16, 43);
            this.checkBoxCT.Name = "checkBoxCT";
            this.checkBoxCT.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.checkBoxCT.Size = new System.Drawing.Size(97, 20);
            this.checkBoxCT.TabIndex = 28;
            this.checkBoxCT.UseVisualStyleBackColor = false;
            this.checkBoxCT.CheckedChanged += new System.EventHandler(this.checkBoxCT_CheckedChanged);
            // 
            // checkBoxTC
            // 
            this.checkBoxTC.BackColor = System.Drawing.SystemColors.Control;
            this.checkBoxTC.Cursor = System.Windows.Forms.Cursors.Default;
            this.checkBoxTC.ForeColor = System.Drawing.SystemColors.ControlText;
            this.checkBoxTC.Location = new System.Drawing.Point(16, 83);
            this.checkBoxTC.Name = "checkBoxTC";
            this.checkBoxTC.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.checkBoxTC.Size = new System.Drawing.Size(97, 20);
            this.checkBoxTC.TabIndex = 29;
            this.checkBoxTC.UseVisualStyleBackColor = false;
            this.checkBoxTC.CheckedChanged += new System.EventHandler(this.checkBoxTC_CheckedChanged);
            // 
            // Check_SpectrumS
            // 
            this.Check_SpectrumS.BackColor = System.Drawing.SystemColors.Control;
            this.Check_SpectrumS.Cursor = System.Windows.Forms.Cursors.Default;
            this.Check_SpectrumS.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Check_SpectrumS.Location = new System.Drawing.Point(12, 24);
            this.Check_SpectrumS.Name = "Check_SpectrumS";
            this.Check_SpectrumS.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Check_SpectrumS.Size = new System.Drawing.Size(129, 19);
            this.Check_SpectrumS.TabIndex = 0;
            this.Check_SpectrumS.Text = "Spectrum  Smoothing";
            this.Check_SpectrumS.UseVisualStyleBackColor = false;
            this.Check_SpectrumS.CheckedChanged += new System.EventHandler(this.Check_SpectrumS_CheckedChanged);
            // 
            // Frame2
            // 
            this.Frame2.BackColor = System.Drawing.SystemColors.Control;
            this.Frame2.Controls.Add(this.TextBox_YMin);
            this.Frame2.Controls.Add(this.TextBox_YMax);
            this.Frame2.Controls.Add(this.Check_AutoScale);
            this.Frame2.Controls.Add(this.Option_Log);
            this.Frame2.Controls.Add(this.Option_Linear);
            this.Frame2.Controls.Add(this.Label6);
            this.Frame2.Controls.Add(this.Label5);
            this.Frame2.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Frame2.Location = new System.Drawing.Point(12, 16);
            this.Frame2.Name = "Frame2";
            this.Frame2.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Frame2.Size = new System.Drawing.Size(142, 116);
            this.Frame2.TabIndex = 0;
            this.Frame2.TabStop = false;
            this.Frame2.Text = "Y Axis";
            // 
            // TextBox_YMin
            // 
            this.TextBox_YMin.AcceptsReturn = true;
            this.TextBox_YMin.BackColor = System.Drawing.SystemColors.Window;
            this.TextBox_YMin.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.TextBox_YMin.Enabled = false;
            this.TextBox_YMin.ForeColor = System.Drawing.SystemColors.WindowText;
            this.TextBox_YMin.Location = new System.Drawing.Point(6, 65);
            this.TextBox_YMin.MaxLength = 0;
            this.TextBox_YMin.Name = "TextBox_YMin";
            this.TextBox_YMin.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.TextBox_YMin.Size = new System.Drawing.Size(54, 20);
            this.TextBox_YMin.TabIndex = 4;
            this.TextBox_YMin.Text = "0";
            this.TextBox_YMin.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TextBox_YMin_KeyDown);
            this.TextBox_YMin.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextBox_YMin_KeyPress);
            // 
            // TextBox_YMax
            // 
            this.TextBox_YMax.AcceptsReturn = true;
            this.TextBox_YMax.BackColor = System.Drawing.SystemColors.Window;
            this.TextBox_YMax.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.TextBox_YMax.Enabled = false;
            this.TextBox_YMax.ForeColor = System.Drawing.SystemColors.WindowText;
            this.TextBox_YMax.Location = new System.Drawing.Point(6, 89);
            this.TextBox_YMax.MaxLength = 0;
            this.TextBox_YMax.Name = "TextBox_YMax";
            this.TextBox_YMax.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.TextBox_YMax.Size = new System.Drawing.Size(54, 20);
            this.TextBox_YMax.TabIndex = 3;
            this.TextBox_YMax.Text = "800";
            this.TextBox_YMax.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TextBox_YMax_KeyDown);
            this.TextBox_YMax.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextBox_YMax_KeyPress);
            // 
            // Check_AutoScale
            // 
            this.Check_AutoScale.BackColor = System.Drawing.SystemColors.Control;
            this.Check_AutoScale.Checked = true;
            this.Check_AutoScale.CheckState = System.Windows.Forms.CheckState.Checked;
            this.Check_AutoScale.Cursor = System.Windows.Forms.Cursors.Default;
            this.Check_AutoScale.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Check_AutoScale.Location = new System.Drawing.Point(6, 44);
            this.Check_AutoScale.Name = "Check_AutoScale";
            this.Check_AutoScale.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Check_AutoScale.Size = new System.Drawing.Size(97, 20);
            this.Check_AutoScale.TabIndex = 2;
            this.Check_AutoScale.Text = "Autoscale";
            this.Check_AutoScale.UseVisualStyleBackColor = false;
            this.Check_AutoScale.CheckStateChanged += new System.EventHandler(this.Check_AutoScale_CheckStateChanged);
            this.Check_AutoScale.CheckedChanged += new System.EventHandler(this.Check_AutoScale_CheckedChanged);
            // 
            // Option_Log
            // 
            this.Option_Log.BackColor = System.Drawing.SystemColors.Control;
            this.Option_Log.Cursor = System.Windows.Forms.Cursors.Default;
            this.Option_Log.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Option_Log.Location = new System.Drawing.Point(6, 28);
            this.Option_Log.Name = "Option_Log";
            this.Option_Log.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Option_Log.Size = new System.Drawing.Size(89, 19);
            this.Option_Log.TabIndex = 1;
            this.Option_Log.Text = "Logarithmic";
            this.Option_Log.UseVisualStyleBackColor = false;
            this.Option_Log.CheckedChanged += new System.EventHandler(this.Option_Log_CheckedChanged);
            // 
            // Option_Linear
            // 
            this.Option_Linear.BackColor = System.Drawing.SystemColors.Control;
            this.Option_Linear.Checked = true;
            this.Option_Linear.Cursor = System.Windows.Forms.Cursors.Default;
            this.Option_Linear.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Option_Linear.Location = new System.Drawing.Point(6, 13);
            this.Option_Linear.Name = "Option_Linear";
            this.Option_Linear.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Option_Linear.Size = new System.Drawing.Size(81, 19);
            this.Option_Linear.TabIndex = 0;
            this.Option_Linear.TabStop = true;
            this.Option_Linear.Text = "Linear";
            this.Option_Linear.UseVisualStyleBackColor = false;
            this.Option_Linear.CheckedChanged += new System.EventHandler(this.Option_Linear_CheckedChanged);
            // 
            // Label6
            // 
            this.Label6.BackColor = System.Drawing.SystemColors.Control;
            this.Label6.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label6.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label6.Location = new System.Drawing.Point(65, 67);
            this.Label6.Name = "Label6";
            this.Label6.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label6.Size = new System.Drawing.Size(63, 23);
            this.Label6.TabIndex = 17;
            this.Label6.Text = "Y Axis Min";
            // 
            // Label5
            // 
            this.Label5.BackColor = System.Drawing.SystemColors.Control;
            this.Label5.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label5.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label5.Location = new System.Drawing.Point(65, 89);
            this.Label5.Name = "Label5";
            this.Label5.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label5.Size = new System.Drawing.Size(63, 20);
            this.Label5.TabIndex = 16;
            this.Label5.Text = "Y Axis Max";
            // 
            // Frame1
            // 
            this.Frame1.BackColor = System.Drawing.SystemColors.Control;
            this.Frame1.Controls.Add(this.Check_XFull);
            this.Frame1.Controls.Add(this.TextBox_XMin);
            this.Frame1.Controls.Add(this.TextBox_XMax);
            this.Frame1.Controls.Add(this.Label8);
            this.Frame1.Controls.Add(this.Label7);
            this.Frame1.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Frame1.Location = new System.Drawing.Point(160, 16);
            this.Frame1.Name = "Frame1";
            this.Frame1.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Frame1.Size = new System.Drawing.Size(140, 89);
            this.Frame1.TabIndex = 1;
            this.Frame1.TabStop = false;
            this.Frame1.Text = "X Axis";
            // 
            // Check_XFull
            // 
            this.Check_XFull.BackColor = System.Drawing.SystemColors.Control;
            this.Check_XFull.Checked = true;
            this.Check_XFull.CheckState = System.Windows.Forms.CheckState.Checked;
            this.Check_XFull.Cursor = System.Windows.Forms.Cursors.Default;
            this.Check_XFull.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Check_XFull.Location = new System.Drawing.Point(8, 17);
            this.Check_XFull.Name = "Check_XFull";
            this.Check_XFull.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Check_XFull.Size = new System.Drawing.Size(81, 19);
            this.Check_XFull.TabIndex = 0;
            this.Check_XFull.Text = "Full scale";
            this.Check_XFull.UseVisualStyleBackColor = false;
            this.Check_XFull.CheckStateChanged += new System.EventHandler(this.Check_XFull_CheckStateChanged);
            this.Check_XFull.CheckedChanged += new System.EventHandler(this.Check_XFull_CheckedChanged);
            // 
            // TextBox_XMin
            // 
            this.TextBox_XMin.AcceptsReturn = true;
            this.TextBox_XMin.BackColor = System.Drawing.SystemColors.Window;
            this.TextBox_XMin.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.TextBox_XMin.Enabled = false;
            this.TextBox_XMin.ForeColor = System.Drawing.SystemColors.WindowText;
            this.TextBox_XMin.Location = new System.Drawing.Point(8, 38);
            this.TextBox_XMin.MaxLength = 0;
            this.TextBox_XMin.Name = "TextBox_XMin";
            this.TextBox_XMin.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.TextBox_XMin.Size = new System.Drawing.Size(48, 20);
            this.TextBox_XMin.TabIndex = 2;
            this.TextBox_XMin.Text = "0";
            this.TextBox_XMin.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TextBox_XMin_KeyDown);
            this.TextBox_XMin.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextBox_XMin_KeyPress);
            // 
            // TextBox_XMax
            // 
            this.TextBox_XMax.AcceptsReturn = true;
            this.TextBox_XMax.BackColor = System.Drawing.SystemColors.Window;
            this.TextBox_XMax.Cursor = System.Windows.Forms.Cursors.IBeam;
            this.TextBox_XMax.Enabled = false;
            this.TextBox_XMax.ForeColor = System.Drawing.SystemColors.WindowText;
            this.TextBox_XMax.Location = new System.Drawing.Point(8, 63);
            this.TextBox_XMax.MaxLength = 0;
            this.TextBox_XMax.Name = "TextBox_XMax";
            this.TextBox_XMax.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.TextBox_XMax.Size = new System.Drawing.Size(48, 20);
            this.TextBox_XMax.TabIndex = 1;
            this.TextBox_XMax.Text = "4095";
            this.TextBox_XMax.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TextBox_XMax_KeyDown);
            this.TextBox_XMax.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.TextBox_XMax_KeyPress);
            // 
            // Label8
            // 
            this.Label8.BackColor = System.Drawing.SystemColors.Control;
            this.Label8.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label8.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label8.Location = new System.Drawing.Point(60, 40);
            this.Label8.Name = "Label8";
            this.Label8.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label8.Size = new System.Drawing.Size(66, 20);
            this.Label8.TabIndex = 21;
            this.Label8.Text = "X Axis Min";
            // 
            // Label7
            // 
            this.Label7.BackColor = System.Drawing.SystemColors.Control;
            this.Label7.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label7.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label7.Location = new System.Drawing.Point(60, 64);
            this.Label7.Name = "Label7";
            this.Label7.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label7.Size = new System.Drawing.Size(66, 20);
            this.Label7.TabIndex = 20;
            this.Label7.Text = "X Axis Max";
            // 
            // Label_ESC
            // 
            this.Label_ESC.BackColor = System.Drawing.Color.Transparent;
            this.Label_ESC.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label_ESC.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label_ESC.Location = new System.Drawing.Point(160, 620);
            this.Label_ESC.Name = "Label_ESC";
            this.Label_ESC.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label_ESC.Size = new System.Drawing.Size(64, 17);
            this.Label_ESC.TabIndex = 89;
            this.Label_ESC.Text = "0";
            // 
            // Label_SC
            // 
            this.Label_SC.BackColor = System.Drawing.SystemColors.Control;
            this.Label_SC.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label_SC.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label_SC.Location = new System.Drawing.Point(8, 620);
            this.Label_SC.Name = "Label_SC";
            this.Label_SC.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label_SC.Size = new System.Drawing.Size(154, 20);
            this.Label_SC.TabIndex = 88;
            this.Label_SC.Text = "Selected Channel or Energy = ";
            // 
            // Label_FWHM
            // 
            this.Label_FWHM.BackColor = System.Drawing.Color.Transparent;
            this.Label_FWHM.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label_FWHM.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label_FWHM.Location = new System.Drawing.Point(160, 700);
            this.Label_FWHM.Name = "Label_FWHM";
            this.Label_FWHM.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label_FWHM.Size = new System.Drawing.Size(104, 19);
            this.Label_FWHM.TabIndex = 84;
            this.Label_FWHM.Text = "0";
            this.Label_FWHM.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Label31
            // 
            this.Label31.BackColor = System.Drawing.SystemColors.Control;
            this.Label31.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label31.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label31.Location = new System.Drawing.Point(8, 702);
            this.Label31.Name = "Label31";
            this.Label31.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label31.Size = new System.Drawing.Size(132, 19);
            this.Label31.TabIndex = 83;
            this.Label31.Text = "Full width half maximum = ";
            // 
            // Label_TC
            // 
            this.Label_TC.BackColor = System.Drawing.Color.Transparent;
            this.Label_TC.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label_TC.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label_TC.Location = new System.Drawing.Point(160, 680);
            this.Label_TC.Name = "Label_TC";
            this.Label_TC.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label_TC.Size = new System.Drawing.Size(68, 20);
            this.Label_TC.TabIndex = 81;
            this.Label_TC.Text = "0";
            // 
            // Label_CPS
            // 
            this.Label_CPS.BackColor = System.Drawing.Color.Transparent;
            this.Label_CPS.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label_CPS.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label_CPS.Location = new System.Drawing.Point(160, 640);
            this.Label_CPS.Name = "Label_CPS";
            this.Label_CPS.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label_CPS.Size = new System.Drawing.Size(64, 13);
            this.Label_CPS.TabIndex = 79;
            this.Label_CPS.Text = "0";
            // 
            // Label4
            // 
            this.Label4.BackColor = System.Drawing.SystemColors.Control;
            this.Label4.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label4.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label4.Location = new System.Drawing.Point(8, 661);
            this.Label4.Name = "Label4";
            this.Label4.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label4.Size = new System.Drawing.Size(116, 19);
            this.Label4.TabIndex = 76;
            this.Label4.Text = "Total Collection Time =";
            // 
            // Label_14
            // 
            this.Label_14.BackColor = System.Drawing.SystemColors.Control;
            this.Label_14.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label_14.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label_14.Location = new System.Drawing.Point(8, 640);
            this.Label_14.Name = "Label_14";
            this.Label_14.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label_14.Size = new System.Drawing.Size(108, 19);
            this.Label_14.TabIndex = 73;
            this.Label_14.Text = "Counts per Second =";
            // 
            // Labell_CiS
            // 
            this.Labell_CiS.BackColor = System.Drawing.SystemColors.Control;
            this.Labell_CiS.Cursor = System.Windows.Forms.Cursors.Default;
            this.Labell_CiS.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Labell_CiS.Location = new System.Drawing.Point(8, 600);
            this.Labell_CiS.Name = "Labell_CiS";
            this.Labell_CiS.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Labell_CiS.Size = new System.Drawing.Size(147, 19);
            this.Labell_CiS.TabIndex = 72;
            this.Labell_CiS.Text = "Counts in Selected Channel =";
            // 
            // Label1
            // 
            this.Label1.BackColor = System.Drawing.SystemColors.Control;
            this.Label1.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label1.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label1.Location = new System.Drawing.Point(8, 682);
            this.Label1.Name = "Label1";
            this.Label1.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label1.Size = new System.Drawing.Size(132, 20);
            this.Label1.TabIndex = 71;
            this.Label1.Text = "Total Number of Counts =";
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.BackColor = System.Drawing.SystemColors.Control;
            this.label3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label3.Font = new System.Drawing.Font("Arial Black", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.ForeColor = System.Drawing.Color.Black;
            this.label3.Location = new System.Drawing.Point(876, 32);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(129, 25);
            this.label3.TabIndex = 92;
            this.label3.Text = "iGEM On Line";
            // 
            // button_setAxis
            // 
            this.button_setAxis.Location = new System.Drawing.Point(220, 112);
            this.button_setAxis.Name = "button_setAxis";
            this.button_setAxis.Size = new System.Drawing.Size(51, 20);
            this.button_setAxis.TabIndex = 1;
            this.button_setAxis.Text = "Set";
            this.button_setAxis.UseVisualStyleBackColor = true;
            this.button_setAxis.Click += new System.EventHandler(this.button_setAxis_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.button_setAxis);
            this.groupBox1.Controls.Add(this.Frame2);
            this.groupBox1.Controls.Add(this.Frame1);
            this.groupBox1.Location = new System.Drawing.Point(488, 600);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(312, 136);
            this.groupBox1.TabIndex = 7;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Axis Control";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label12);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.Check_SpectrumS);
            this.groupBox2.Controls.Add(this.Check_EenergyD);
            this.groupBox2.Controls.Add(this.Check_DisStatic);
            this.groupBox2.Controls.Add(this.Check_DisLive);
            this.groupBox2.Location = new System.Drawing.Point(824, 600);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(156, 136);
            this.groupBox2.TabIndex = 8;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Display Control";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.ForeColor = System.Drawing.Color.Blue;
            this.label12.Location = new System.Drawing.Point(80, 116);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(34, 13);
            this.label12.TabIndex = 89;
            this.label12.Text = "(Blue)";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.ForeColor = System.Drawing.Color.Green;
            this.label2.Location = new System.Drawing.Point(78, 85);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(42, 13);
            this.label2.TabIndex = 88;
            this.label2.Text = "(Green)";
            // 
            // Label_Time
            // 
            this.Label_Time.BackColor = System.Drawing.Color.Transparent;
            this.Label_Time.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label_Time.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label_Time.Location = new System.Drawing.Point(160, 656);
            this.Label_Time.Name = "Label_Time";
            this.Label_Time.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label_Time.Size = new System.Drawing.Size(64, 24);
            this.Label_Time.TabIndex = 80;
            this.Label_Time.Text = "00:00:00";
            this.Label_Time.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // timerspectrum
            // 
            this.timerspectrum.Interval = 1000;
            this.timerspectrum.Tick += new System.EventHandler(this.timerspectrum_Tick);
            // 
            // timerCollect
            // 
            this.timerCollect.Interval = 1;
            this.timerCollect.Tick += new System.EventHandler(this.timerCollect_Tick);
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackColor = System.Drawing.Color.Transparent;
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(748, 24);
            this.pictureBox1.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(95, 46);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox1.TabIndex = 93;
            this.pictureBox1.TabStop = false;
            // 
            // timerHVRamp
            // 
            this.timerHVRamp.Interval = 500;
            this.timerHVRamp.Tick += new System.EventHandler(this.timerHVRamp_Tick);
            // 
            // Button_CSS
            // 
            this.Button_CSS.Enabled = false;
            this.Button_CSS.Location = new System.Drawing.Point(568, 28);
            this.Button_CSS.Name = "Button_CSS";
            this.Button_CSS.Size = new System.Drawing.Size(80, 36);
            this.Button_CSS.TabIndex = 94;
            this.Button_CSS.Text = "Clear Static Spectrum";
            this.Button_CSS.UseVisualStyleBackColor = true;
            this.Button_CSS.Click += new System.EventHandler(this.Button_CSS_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoValidate = System.Windows.Forms.AutoValidate.EnableAllowFocusChange;
            this.ClientSize = new System.Drawing.Size(1016, 741);
            this.Controls.Add(this.Button_CSS);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.Label_CiS);
            this.Controls.Add(this.Frame3);
            this.Controls.Add(this.Label_ESC);
            this.Controls.Add(this.Label_SC);
            this.Controls.Add(this.Label_FWHM);
            this.Controls.Add(this.Label31);
            this.Controls.Add(this.Label_TC);
            this.Controls.Add(this.Label_Time);
            this.Controls.Add(this.Label_CPS);
            this.Controls.Add(this.Label4);
            this.Controls.Add(this.Label_14);
            this.Controls.Add(this.Labell_CiS);
            this.Controls.Add(this.Label1);
            this.Controls.Add(this.Button_LSS);
            this.Controls.Add(this.Button_LAS);
            this.Controls.Add(this.Button_SAS);
            this.Controls.Add(this.Button_CAS);
            this.Controls.Add(this.Button_Stop);
            this.Controls.Add(this.Button_Start);
            this.Controls.Add(this.Menu_iGEM);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "  eV Products    iSpectrum";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Menu_iGEM.ResumeLayout(false);
            this.Menu_iGEM.PerformLayout();
            this.Frame3.ResumeLayout(false);
            this.Frame3.PerformLayout();
            this.Frame2.ResumeLayout(false);
            this.Frame2.PerformLayout();
            this.Frame1.ResumeLayout(false);
            this.Frame1.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        internal System.Windows.Forms.MenuStrip Menu_iGEM;
        internal System.Windows.Forms.ToolStripMenuItem DdToolStripMenuItem;
        internal System.Windows.Forms.ToolStripMenuItem LoadActiveSpectrumToolStripMenuItem;
        internal System.Windows.Forms.ToolStripMenuItem LoadStaticSpectrumToolStripMenuItem;
        internal System.Windows.Forms.ToolStripMenuItem SaveActiveSpectrumToolStripMenuItem;
        internal System.Windows.Forms.ToolStripMenuItem ClearActiveSpectrumToolStripMenuItem;
        internal System.Windows.Forms.ToolStripMenuItem ExitToolStripMenuItem;
        internal System.Windows.Forms.ToolStripMenuItem CalibrateToolStripMenuItem;
        internal System.Windows.Forms.ToolStripMenuItem FWHMToolStripMenuItem;
        internal System.Windows.Forms.ToolStripMenuItem ConfigureToolStripMenuItem;
        internal System.Windows.Forms.ToolStripMenuItem HelpToolStripMenuItem;
        internal System.Windows.Forms.ToolStripMenuItem IndexToolStripMenuItem;
        internal System.Windows.Forms.ToolStripMenuItem AboutToolStripMenuItem;
        public System.Windows.Forms.Button Button_LSS;
        public System.Windows.Forms.Button Button_LAS;
        public System.Windows.Forms.Button Button_SAS;
        public System.Windows.Forms.Button Button_CAS;
        public System.Windows.Forms.Button Button_Stop;
        public System.Windows.Forms.Button Button_Start;
        public System.Windows.Forms.Label Label_CiS;
        public System.Windows.Forms.CheckBox Check_DisStatic;
        public System.Windows.Forms.CheckBox Check_DisLive;
        public System.Windows.Forms.CheckBox Check_EenergyD;
        public System.Windows.Forms.GroupBox Frame3;
        public System.Windows.Forms.TextBox TextBox_Counts;
        public System.Windows.Forms.TextBox TextBox_Time;
        public System.Windows.Forms.Label Label11;
        public System.Windows.Forms.Label Label10;
        public System.Windows.Forms.CheckBox Check_SpectrumS;
        public System.Windows.Forms.GroupBox Frame2;
        public System.Windows.Forms.TextBox TextBox_YMin;
        public System.Windows.Forms.TextBox TextBox_YMax;
        public System.Windows.Forms.CheckBox Check_AutoScale;
        public System.Windows.Forms.RadioButton Option_Log;
        public System.Windows.Forms.RadioButton Option_Linear;
        public System.Windows.Forms.Label Label6;
        public System.Windows.Forms.Label Label5;
        public System.Windows.Forms.GroupBox Frame1;
        public System.Windows.Forms.CheckBox Check_XFull;
        public System.Windows.Forms.TextBox TextBox_XMin;
        public System.Windows.Forms.TextBox TextBox_XMax;
        public System.Windows.Forms.Label Label8;
        public System.Windows.Forms.Label Label7;
        public System.Windows.Forms.Label Label_ESC;
        public System.Windows.Forms.Label Label_SC;
        public System.Windows.Forms.Label Label_FWHM;
        public System.Windows.Forms.Label Label31;
        public System.Windows.Forms.Label Label_TC;
        public System.Windows.Forms.Label Label_CPS;
        public System.Windows.Forms.Label Label4;
        public System.Windows.Forms.Label Label_14;
        public System.Windows.Forms.Label Labell_CiS;
        public System.Windows.Forms.Label Label1;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
        //private GraphicsServer.GSNet.Charting.GSNetWinChart gsNetWinChart1;
        private System.Windows.Forms.Label label3;
        internal System.Windows.Forms.Button button_setAxis;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        public System.Windows.Forms.Label Label_Time;
        private System.Windows.Forms.Timer timerspectrum;
        private System.Windows.Forms.Timer timerCollect;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.CheckBox checkBoxCT;
        public System.Windows.Forms.CheckBox checkBoxTC;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.ToolStripMenuItem graphToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToJPEGToolStripMenuItem;
        private System.Windows.Forms.Timer timerHVRamp;
        private System.Windows.Forms.Button Button_CSS;
        private System.Windows.Forms.ToolStripMenuItem ClearStaticSpectrumToolStripMenuItem;
    }
}

