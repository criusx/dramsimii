namespace eV_Products_Demo
{
    partial class eVDemo
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.zedGraphControl1 = new ZedGraph.ZedGraphControl();
            this.button1 = new System.Windows.Forms.Button();
            this.lookupButton = new System.Windows.Forms.Button();
            this.storeButton = new System.Windows.Forms.Button();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.button_setAxis = new System.Windows.Forms.Button();
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
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.smoothLineCheckBox = new System.Windows.Forms.CheckBox();
            this.antialiasCheck = new System.Windows.Forms.CheckBox();
            this.Check_EenergyD = new System.Windows.Forms.CheckBox();
            this.Frame3 = new System.Windows.Forms.GroupBox();
            this.TextBox_Counts = new System.Windows.Forms.TextBox();
            this.TextBox_Time = new System.Windows.Forms.TextBox();
            this.Label11 = new System.Windows.Forms.Label();
            this.Label10 = new System.Windows.Forms.Label();
            this.checkBoxCT = new System.Windows.Forms.CheckBox();
            this.checkBoxTC = new System.Windows.Forms.CheckBox();
            this.timerCollect = new System.Windows.Forms.Timer(this.components);
            this.pictureBox2 = new System.Windows.Forms.PictureBox();
            this.label2 = new System.Windows.Forms.Label();
            this.Label_TC = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.timerspectrum = new System.Windows.Forms.Timer(this.components);
            this.Label_FWHM = new System.Windows.Forms.Label();
            this.Label31 = new System.Windows.Forms.Label();
            this.Button_Start = new System.Windows.Forms.Button();
            this.timerHVRamp = new System.Windows.Forms.Timer(this.components);
            this.Label_Time = new System.Windows.Forms.Label();
            this.Label4 = new System.Windows.Forms.Label();
            this.Button_Stop = new System.Windows.Forms.Button();
            this.configureButton = new System.Windows.Forms.Button();
            this.label9 = new System.Windows.Forms.Label();
            this.countsPerSecondLabel = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.button2 = new System.Windows.Forms.Button();
            this.label12 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.Check_DisStatic = new System.Windows.Forms.CheckBox();
            this.Check_DisLive = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.Frame2.SuspendLayout();
            this.Frame1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.Frame3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).BeginInit();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // zedGraphControl1
            // 
            this.zedGraphControl1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            this.zedGraphControl1.IsAntiAlias = true;
            this.zedGraphControl1.Location = new System.Drawing.Point(130, 10);
            this.zedGraphControl1.Margin = new System.Windows.Forms.Padding(1);
            this.zedGraphControl1.Name = "zedGraphControl1";
            this.zedGraphControl1.Padding = new System.Windows.Forms.Padding(1);
            this.zedGraphControl1.ScrollGrace = 0;
            this.zedGraphControl1.ScrollMaxX = 0;
            this.zedGraphControl1.ScrollMaxY = 0;
            this.zedGraphControl1.ScrollMaxY2 = 0;
            this.zedGraphControl1.ScrollMinX = 0;
            this.zedGraphControl1.ScrollMinY = 0;
            this.zedGraphControl1.ScrollMinY2 = 0;
            this.zedGraphControl1.Size = new System.Drawing.Size(919, 524);
            this.zedGraphControl1.TabIndex = 0;
            // 
            // button1
            // 
            this.button1.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button1.Location = new System.Drawing.Point(11, 9);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(114, 37);
            this.button1.TabIndex = 1;
            this.button1.Text = "Get Tag";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.tagLookupClick);
            // 
            // lookupButton
            // 
            this.lookupButton.Enabled = false;
            this.lookupButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lookupButton.Location = new System.Drawing.Point(11, 52);
            this.lookupButton.Name = "lookupButton";
            this.lookupButton.Size = new System.Drawing.Size(114, 37);
            this.lookupButton.TabIndex = 1;
            this.lookupButton.Text = "Lookup Spectrum";
            this.lookupButton.UseVisualStyleBackColor = true;
            this.lookupButton.Click += new System.EventHandler(this.lookupClick);
            // 
            // storeButton
            // 
            this.storeButton.Enabled = false;
            this.storeButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.storeButton.Location = new System.Drawing.Point(11, 95);
            this.storeButton.Name = "storeButton";
            this.storeButton.Size = new System.Drawing.Size(114, 37);
            this.storeButton.TabIndex = 1;
            this.storeButton.Text = "Store Spectrum";
            this.storeButton.UseVisualStyleBackColor = true;
            this.storeButton.Click += new System.EventHandler(this.storeSpectrumClick);
            // 
            // pictureBox1
            // 
            this.pictureBox1.BackgroundImage = global::eV_Products_Demo.Properties.Resources.Gentag_logo;
            this.pictureBox1.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.pictureBox1.ErrorImage = global::eV_Products_Demo.Properties.Resources.Gentag_logo;
            this.pictureBox1.InitialImage = global::eV_Products_Demo.Properties.Resources.Gentag_logo;
            this.pictureBox1.Location = new System.Drawing.Point(13, 404);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(113, 68);
            this.pictureBox1.TabIndex = 3;
            this.pictureBox1.TabStop = false;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.button_setAxis);
            this.groupBox1.Controls.Add(this.Frame2);
            this.groupBox1.Controls.Add(this.Frame1);
            this.groupBox1.Location = new System.Drawing.Point(560, 541);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(312, 136);
            this.groupBox1.TabIndex = 8;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Axis Control";
            // 
            // button_setAxis
            // 
            this.button_setAxis.Location = new System.Drawing.Point(220, 112);
            this.button_setAxis.Name = "button_setAxis";
            this.button_setAxis.Size = new System.Drawing.Size(51, 20);
            this.button_setAxis.TabIndex = 1;
            this.button_setAxis.Text = "Set";
            this.button_setAxis.UseVisualStyleBackColor = true;
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
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label12);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.Check_DisStatic);
            this.groupBox2.Controls.Add(this.Check_DisLive);
            this.groupBox2.Controls.Add(this.smoothLineCheckBox);
            this.groupBox2.Controls.Add(this.antialiasCheck);
            this.groupBox2.Controls.Add(this.Check_EenergyD);
            this.groupBox2.Location = new System.Drawing.Point(878, 541);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(169, 136);
            this.groupBox2.TabIndex = 10;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Display Control";
            // 
            // smoothLineCheckBox
            // 
            this.smoothLineCheckBox.BackColor = System.Drawing.SystemColors.Control;
            this.smoothLineCheckBox.Checked = true;
            this.smoothLineCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.smoothLineCheckBox.Cursor = System.Windows.Forms.Cursors.Default;
            this.smoothLineCheckBox.ForeColor = System.Drawing.SystemColors.ControlText;
            this.smoothLineCheckBox.Location = new System.Drawing.Point(12, 44);
            this.smoothLineCheckBox.Name = "smoothLineCheckBox";
            this.smoothLineCheckBox.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.smoothLineCheckBox.Size = new System.Drawing.Size(129, 19);
            this.smoothLineCheckBox.TabIndex = 0;
            this.smoothLineCheckBox.Text = "Smooth Line";
            this.smoothLineCheckBox.UseVisualStyleBackColor = false;
            this.smoothLineCheckBox.CheckedChanged += new System.EventHandler(this.smoothLineChanged);
            // 
            // antialiasCheck
            // 
            this.antialiasCheck.BackColor = System.Drawing.SystemColors.Control;
            this.antialiasCheck.Checked = true;
            this.antialiasCheck.CheckState = System.Windows.Forms.CheckState.Checked;
            this.antialiasCheck.Cursor = System.Windows.Forms.Cursors.Default;
            this.antialiasCheck.ForeColor = System.Drawing.SystemColors.ControlText;
            this.antialiasCheck.Location = new System.Drawing.Point(12, 24);
            this.antialiasCheck.Name = "antialiasCheck";
            this.antialiasCheck.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.antialiasCheck.Size = new System.Drawing.Size(129, 19);
            this.antialiasCheck.TabIndex = 0;
            this.antialiasCheck.Text = "Antialiased";
            this.antialiasCheck.UseVisualStyleBackColor = false;
            this.antialiasCheck.CheckedChanged += new System.EventHandler(this.antialiasedChanged);
            // 
            // Check_EenergyD
            // 
            this.Check_EenergyD.BackColor = System.Drawing.SystemColors.Control;
            this.Check_EenergyD.Cursor = System.Windows.Forms.Cursors.Default;
            this.Check_EenergyD.Enabled = false;
            this.Check_EenergyD.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Check_EenergyD.Location = new System.Drawing.Point(12, 113);
            this.Check_EenergyD.Name = "Check_EenergyD";
            this.Check_EenergyD.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Check_EenergyD.Size = new System.Drawing.Size(121, 19);
            this.Check_EenergyD.TabIndex = 1;
            this.Check_EenergyD.Text = "KeV Display Mode";
            this.Check_EenergyD.UseVisualStyleBackColor = false;
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
            this.Frame3.Location = new System.Drawing.Point(430, 541);
            this.Frame3.Name = "Frame3";
            this.Frame3.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Frame3.Size = new System.Drawing.Size(124, 100);
            this.Frame3.TabIndex = 9;
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
            // 
            // timerCollect
            // 
            this.timerCollect.Interval = 1;
            this.timerCollect.Tick += new System.EventHandler(this.collectDataTick);
            // 
            // pictureBox2
            // 
            this.pictureBox2.BackgroundImage = global::eV_Products_Demo.Properties.Resources.EV_logo;
            this.pictureBox2.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.pictureBox2.ErrorImage = global::eV_Products_Demo.Properties.Resources.Gentag_logo;
            this.pictureBox2.InitialImage = global::eV_Products_Demo.Properties.Resources.Gentag_logo;
            this.pictureBox2.Location = new System.Drawing.Point(11, 478);
            this.pictureBox2.Name = "pictureBox2";
            this.pictureBox2.Size = new System.Drawing.Size(114, 56);
            this.pictureBox2.TabIndex = 3;
            this.pictureBox2.TabStop = false;
            // 
            // label2
            // 
            this.label2.BackColor = System.Drawing.SystemColors.Control;
            this.label2.Cursor = System.Windows.Forms.Cursors.Default;
            this.label2.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label2.Location = new System.Drawing.Point(31, 16);
            this.label2.Name = "label2";
            this.label2.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.label2.Size = new System.Drawing.Size(97, 14);
            this.label2.TabIndex = 2;
            this.label2.Text = "Total Count";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Label_TC
            // 
            this.Label_TC.BackColor = System.Drawing.SystemColors.Control;
            this.Label_TC.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label_TC.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label_TC.Location = new System.Drawing.Point(134, 16);
            this.Label_TC.Name = "Label_TC";
            this.Label_TC.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label_TC.Size = new System.Drawing.Size(97, 14);
            this.Label_TC.TabIndex = 2;
            this.Label_TC.Text = "0";
            this.Label_TC.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.BackColor = System.Drawing.SystemColors.Control;
            this.label3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label3.Font = new System.Drawing.Font("Arial Black", 11F);
            this.label3.ForeColor = System.Drawing.Color.Black;
            this.label3.Location = new System.Drawing.Point(430, 654);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(123, 24);
            this.label3.TabIndex = 93;
            this.label3.Text = "iGEM On Line";
            // 
            // timerspectrum
            // 
            this.timerspectrum.Interval = 1000;
            this.timerspectrum.Tick += new System.EventHandler(this.timerspectrum_Tick);
            // 
            // Label_FWHM
            // 
            this.Label_FWHM.BackColor = System.Drawing.Color.Transparent;
            this.Label_FWHM.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label_FWHM.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label_FWHM.Location = new System.Drawing.Point(134, 36);
            this.Label_FWHM.Name = "Label_FWHM";
            this.Label_FWHM.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label_FWHM.Size = new System.Drawing.Size(104, 14);
            this.Label_FWHM.TabIndex = 95;
            this.Label_FWHM.Text = "0";
            this.Label_FWHM.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Label31
            // 
            this.Label31.BackColor = System.Drawing.SystemColors.Control;
            this.Label31.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label31.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label31.Location = new System.Drawing.Point(-2, 35);
            this.Label31.Name = "Label31";
            this.Label31.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label31.Size = new System.Drawing.Size(132, 14);
            this.Label31.TabIndex = 94;
            this.Label31.Text = "Full Width Half Maximum";
            this.Label31.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Button_Start
            // 
            this.Button_Start.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Button_Start.Location = new System.Drawing.Point(11, 138);
            this.Button_Start.Name = "Button_Start";
            this.Button_Start.Size = new System.Drawing.Size(114, 37);
            this.Button_Start.TabIndex = 1;
            this.Button_Start.Text = "Read Spectrum";
            this.Button_Start.UseVisualStyleBackColor = true;
            this.Button_Start.Click += new System.EventHandler(this.Button_Start_Click);
            // 
            // timerHVRamp
            // 
            this.timerHVRamp.Interval = 500;
            this.timerHVRamp.Tick += new System.EventHandler(this.timerHVRamp_Tick);
            // 
            // Label_Time
            // 
            this.Label_Time.BackColor = System.Drawing.Color.Transparent;
            this.Label_Time.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label_Time.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label_Time.Location = new System.Drawing.Point(134, 57);
            this.Label_Time.Name = "Label_Time";
            this.Label_Time.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label_Time.Size = new System.Drawing.Size(64, 14);
            this.Label_Time.TabIndex = 97;
            this.Label_Time.Text = "00:00:00";
            this.Label_Time.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // Label4
            // 
            this.Label4.BackColor = System.Drawing.SystemColors.Control;
            this.Label4.Cursor = System.Windows.Forms.Cursors.Default;
            this.Label4.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Label4.Location = new System.Drawing.Point(12, 54);
            this.Label4.Name = "Label4";
            this.Label4.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Label4.Size = new System.Drawing.Size(116, 14);
            this.Label4.TabIndex = 96;
            this.Label4.Text = "Total Collection Time";
            this.Label4.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // Button_Stop
            // 
            this.Button_Stop.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Button_Stop.Location = new System.Drawing.Point(11, 181);
            this.Button_Stop.Name = "Button_Stop";
            this.Button_Stop.Size = new System.Drawing.Size(114, 37);
            this.Button_Stop.TabIndex = 1;
            this.Button_Stop.Text = "Stop";
            this.Button_Stop.UseVisualStyleBackColor = true;
            this.Button_Stop.Click += new System.EventHandler(this.Button_Stop_Click);
            // 
            // configureButton
            // 
            this.configureButton.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.configureButton.Location = new System.Drawing.Point(12, 224);
            this.configureButton.Name = "configureButton";
            this.configureButton.Size = new System.Drawing.Size(114, 37);
            this.configureButton.TabIndex = 1;
            this.configureButton.Text = "Configure";
            this.configureButton.UseVisualStyleBackColor = true;
            this.configureButton.Click += new System.EventHandler(this.configureButton_Click);
            // 
            // label9
            // 
            this.label9.BackColor = System.Drawing.SystemColors.Control;
            this.label9.Cursor = System.Windows.Forms.Cursors.Default;
            this.label9.ForeColor = System.Drawing.SystemColors.ControlText;
            this.label9.Location = new System.Drawing.Point(12, 73);
            this.label9.Name = "label9";
            this.label9.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.label9.Size = new System.Drawing.Size(116, 14);
            this.label9.TabIndex = 96;
            this.label9.Text = "Counts Per Second";
            this.label9.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // countsPerSecondLabel
            // 
            this.countsPerSecondLabel.BackColor = System.Drawing.Color.Transparent;
            this.countsPerSecondLabel.Cursor = System.Windows.Forms.Cursors.Default;
            this.countsPerSecondLabel.ForeColor = System.Drawing.SystemColors.ControlText;
            this.countsPerSecondLabel.Location = new System.Drawing.Point(134, 75);
            this.countsPerSecondLabel.Name = "countsPerSecondLabel";
            this.countsPerSecondLabel.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.countsPerSecondLabel.Size = new System.Drawing.Size(64, 14);
            this.countsPerSecondLabel.TabIndex = 97;
            this.countsPerSecondLabel.Text = "0";
            this.countsPerSecondLabel.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.Label4);
            this.groupBox3.Controls.Add(this.countsPerSecondLabel);
            this.groupBox3.Controls.Add(this.label2);
            this.groupBox3.Controls.Add(this.label9);
            this.groupBox3.Controls.Add(this.Label_TC);
            this.groupBox3.Controls.Add(this.Label_Time);
            this.groupBox3.Controls.Add(this.Label31);
            this.groupBox3.Controls.Add(this.Label_FWHM);
            this.groupBox3.Location = new System.Drawing.Point(209, 541);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(215, 100);
            this.groupBox3.TabIndex = 98;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Collection Stats";
            // 
            // button2
            // 
            this.button2.Font = new System.Drawing.Font("Microsoft Sans Serif", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button2.Location = new System.Drawing.Point(12, 267);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(114, 37);
            this.button2.TabIndex = 1;
            this.button2.Text = "Calibrate";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.ForeColor = System.Drawing.Color.Blue;
            this.label12.Location = new System.Drawing.Point(135, 84);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(34, 13);
            this.label12.TabIndex = 93;
            this.label12.Text = "(Blue)";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.ForeColor = System.Drawing.Color.Green;
            this.label1.Location = new System.Drawing.Point(127, 50);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(42, 13);
            this.label1.TabIndex = 92;
            this.label1.Text = "(Green)";
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
            this.Check_DisStatic.Location = new System.Drawing.Point(12, 61);
            this.Check_DisStatic.Name = "Check_DisStatic";
            this.Check_DisStatic.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Check_DisStatic.Size = new System.Drawing.Size(134, 19);
            this.Check_DisStatic.TabIndex = 90;
            this.Check_DisStatic.Text = "Display static spectrum";
            this.Check_DisStatic.UseVisualStyleBackColor = false;
            // 
            // Check_DisLive
            // 
            this.Check_DisLive.BackColor = System.Drawing.SystemColors.Control;
            this.Check_DisLive.Checked = true;
            this.Check_DisLive.CheckState = System.Windows.Forms.CheckState.Checked;
            this.Check_DisLive.Cursor = System.Windows.Forms.Cursors.Default;
            this.Check_DisLive.Enabled = false;
            this.Check_DisLive.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Check_DisLive.Location = new System.Drawing.Point(12, 81);
            this.Check_DisLive.Name = "Check_DisLive";
            this.Check_DisLive.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.Check_DisLive.Size = new System.Drawing.Size(129, 19);
            this.Check_DisLive.TabIndex = 91;
            this.Check_DisLive.Text = "Display live spectrum";
            this.Check_DisLive.UseVisualStyleBackColor = false;
            // 
            // eVDemo
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1059, 681);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.Frame3);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.pictureBox2);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.storeButton);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.configureButton);
            this.Controls.Add(this.Button_Stop);
            this.Controls.Add(this.Button_Start);
            this.Controls.Add(this.lookupButton);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.zedGraphControl1);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "eVDemo";
            this.Text = "Gentag / eV Products Demo";
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.eVDemo_KeyDown);
            this.Load += new System.EventHandler(this.eVDemo_Load);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.Frame2.ResumeLayout(false);
            this.Frame2.PerformLayout();
            this.Frame1.ResumeLayout(false);
            this.Frame1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.Frame3.ResumeLayout(false);
            this.Frame3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private ZedGraph.ZedGraphControl zedGraphControl1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button lookupButton;
        private System.Windows.Forms.Button storeButton;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.GroupBox groupBox1;
        internal System.Windows.Forms.Button button_setAxis;
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
        private System.Windows.Forms.GroupBox groupBox2;
        public System.Windows.Forms.CheckBox antialiasCheck;
        public System.Windows.Forms.CheckBox Check_EenergyD;
        public System.Windows.Forms.GroupBox Frame3;
        public System.Windows.Forms.TextBox TextBox_Counts;
        public System.Windows.Forms.TextBox TextBox_Time;
        public System.Windows.Forms.Label Label11;
        public System.Windows.Forms.Label Label10;
        public System.Windows.Forms.CheckBox checkBoxCT;
        public System.Windows.Forms.CheckBox checkBoxTC;
        public System.Windows.Forms.CheckBox smoothLineCheckBox;
        private System.Windows.Forms.Timer timerCollect;
        private System.Windows.Forms.PictureBox pictureBox2;
        public System.Windows.Forms.Label label2;
        public System.Windows.Forms.Label Label_TC;
        public System.Windows.Forms.Label label3;
        public System.Windows.Forms.Timer timerspectrum;
        public System.Windows.Forms.Label Label_FWHM;
        public System.Windows.Forms.Label Label31;
        private System.Windows.Forms.Timer timerHVRamp;
        public System.Windows.Forms.Button Button_Start;
        public System.Windows.Forms.Label Label_Time;
        public System.Windows.Forms.Label Label4;
        public System.Windows.Forms.Button Button_Stop;
        public System.Windows.Forms.Button configureButton;
        public System.Windows.Forms.Label label9;
        public System.Windows.Forms.Label countsPerSecondLabel;
        private System.Windows.Forms.GroupBox groupBox3;
        public System.Windows.Forms.Button button2;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label1;
        public System.Windows.Forms.CheckBox Check_DisStatic;
        public System.Windows.Forms.CheckBox Check_DisLive;
    }
}

