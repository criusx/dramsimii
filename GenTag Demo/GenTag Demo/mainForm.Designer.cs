namespace GenTagDemo
{
    partial class mainForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(mainForm));
            this.readIDButton = new System.Windows.Forms.Button();
            this.readLogButton = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.checkBox1 = new System.Windows.Forms.CheckBox();
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.textBox9 = new System.Windows.Forms.TextBox();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.checkBox2 = new System.Windows.Forms.CheckBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.treeView2 = new System.Windows.Forms.TreeView();
            this.manualIDButton = new System.Windows.Forms.Button();
            this.textBox4 = new System.Windows.Forms.TextBox();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.getSetStatusBox = new System.Windows.Forms.TextBox();
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.readValueButton = new System.Windows.Forms.Button();
            this.setValueButton = new System.Windows.Forms.Button();
            this.textBox5 = new System.Windows.Forms.TextBox();
            this.textBox6 = new System.Windows.Forms.TextBox();
            this.textBox8 = new System.Windows.Forms.TextBox();
            this.textBox7 = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.tabPage5 = new System.Windows.Forms.TabPage();
            this.medicationButton = new System.Windows.Forms.Button();
            this.readPatientButton = new System.Windows.Forms.Button();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.drugPhoto = new System.Windows.Forms.PictureBox();
            this.patientPhoto = new System.Windows.Forms.PictureBox();
            this.patientDescriptionBox = new System.Windows.Forms.TextBox();
            this.patientNameBox = new System.Windows.Forms.TextBox();
            this.gpsPage = new System.Windows.Forms.TabPage();
            this.satellitesUsedTextBox = new System.Windows.Forms.TextBox();
            this.timeTextBox = new System.Windows.Forms.TextBox();
            this.hdopTextBox = new System.Windows.Forms.TextBox();
            this.vdopTextBox = new System.Windows.Forms.TextBox();
            this.statusTextBox = new System.Windows.Forms.TextBox();
            this.speedTextBox = new System.Windows.Forms.TextBox();
            this.satLabel8 = new System.Windows.Forms.Label();
            this.satLabel7 = new System.Windows.Forms.Label();
            this.satLabel6 = new System.Windows.Forms.Label();
            this.satLabel5 = new System.Windows.Forms.Label();
            this.satLabel4 = new System.Windows.Forms.Label();
            this.satLabel3 = new System.Windows.Forms.Label();
            this.satLabel2 = new System.Windows.Forms.Label();
            this.satLabel1 = new System.Windows.Forms.Label();
            this.label20 = new System.Windows.Forms.Label();
            this.label19 = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.directionTextBox = new System.Windows.Forms.TextBox();
            this.label17 = new System.Windows.Forms.Label();
            this.longitudeTextBox = new System.Windows.Forms.TextBox();
            this.label16 = new System.Windows.Forms.Label();
            this.latitudeTextBox = new System.Windows.Forms.TextBox();
            this.label15 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.comPortsComboBox = new System.Windows.Forms.ComboBox();
            this.connectGPSButton = new System.Windows.Forms.Button();
            this.scanCOMPortButton = new System.Windows.Forms.Button();
            this.progressBar8 = new System.Windows.Forms.ProgressBar();
            this.progressBar7 = new System.Windows.Forms.ProgressBar();
            this.progressBar6 = new System.Windows.Forms.ProgressBar();
            this.progressBar5 = new System.Windows.Forms.ProgressBar();
            this.progressBar4 = new System.Windows.Forms.ProgressBar();
            this.progressBar3 = new System.Windows.Forms.ProgressBar();
            this.progressBar2 = new System.Windows.Forms.ProgressBar();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.gpsSerialPort = new System.IO.Ports.SerialPort(this.components);
            this.listBox2 = new System.Windows.Forms.ListBox();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.tabPage5.SuspendLayout();
            this.gpsPage.SuspendLayout();
            this.SuspendLayout();
            // 
            // readIDButton
            // 
            this.readIDButton.Location = new System.Drawing.Point(2, 187);
            this.readIDButton.Name = "readIDButton";
            this.readIDButton.Size = new System.Drawing.Size(71, 27);
            this.readIDButton.TabIndex = 0;
            this.readIDButton.Click += new System.EventHandler(this.readerClick);
            // 
            // readLogButton
            // 
            this.readLogButton.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.readLogButton.Location = new System.Drawing.Point(2, 196);
            this.readLogButton.Name = "readLogButton";
            this.readLogButton.Size = new System.Drawing.Size(84, 18);
            this.readLogButton.TabIndex = 0;
            this.readLogButton.Click += new System.EventHandler(this.readerClick);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Controls.Add(this.tabPage5);
            this.tabControl1.Controls.Add(this.gpsPage);
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(240, 240);
            this.tabControl1.TabIndex = 5;
            // 
            // tabPage1
            // 
            this.tabPage1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(224)))), ((int)(((byte)(255)))));
            this.tabPage1.Controls.Add(this.pictureBox1);
            this.tabPage1.Controls.Add(this.readIDButton);
            this.tabPage1.Controls.Add(this.checkBox1);
            this.tabPage1.Controls.Add(this.treeView1);
            this.tabPage1.Location = new System.Drawing.Point(0, 0);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Size = new System.Drawing.Size(240, 217);
            this.tabPage1.Text = "tabPage1";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Left | System.Windows.Forms.AnchorStyles.Right)));
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(79, 125);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(154, 90);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            // 
            // checkBox1
            // 
            this.checkBox1.Enabled = false;
            this.checkBox1.Location = new System.Drawing.Point(0, 163);
            this.checkBox1.Name = "checkBox1";
            this.checkBox1.Size = new System.Drawing.Size(25, 18);
            this.checkBox1.TabIndex = 4;
            // 
            // treeView1
            // 
            this.treeView1.Location = new System.Drawing.Point(3, 3);
            this.treeView1.Name = "treeView1";
            this.treeView1.Size = new System.Drawing.Size(234, 118);
            this.treeView1.TabIndex = 2;
            // 
            // tabPage2
            // 
            this.tabPage2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(224)))), ((int)(((byte)(255)))));
            this.tabPage2.Controls.Add(this.textBox9);
            this.tabPage2.Controls.Add(this.listBox1);
            this.tabPage2.Controls.Add(this.checkBox2);
            this.tabPage2.Controls.Add(this.label4);
            this.tabPage2.Controls.Add(this.label3);
            this.tabPage2.Controls.Add(this.label2);
            this.tabPage2.Controls.Add(this.label1);
            this.tabPage2.Controls.Add(this.textBox3);
            this.tabPage2.Controls.Add(this.textBox2);
            this.tabPage2.Controls.Add(this.textBox1);
            this.tabPage2.Controls.Add(this.readLogButton);
            this.tabPage2.Location = new System.Drawing.Point(0, 0);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Size = new System.Drawing.Size(232, 214);
            this.tabPage2.Text = "tabPage2";
            // 
            // textBox9
            // 
            this.textBox9.Location = new System.Drawing.Point(10, 109);
            this.textBox9.Multiline = true;
            this.textBox9.Name = "textBox9";
            this.textBox9.ReadOnly = true;
            this.textBox9.Size = new System.Drawing.Size(76, 57);
            this.textBox9.TabIndex = 11;
            this.textBox9.Text = "textBox9";
            // 
            // listBox1
            // 
            this.listBox1.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.listBox1.Location = new System.Drawing.Point(93, 27);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(144, 184);
            this.listBox1.TabIndex = 6;
            // 
            // checkBox2
            // 
            this.checkBox2.Enabled = false;
            this.checkBox2.Location = new System.Drawing.Point(62, 172);
            this.checkBox2.Name = "checkBox2";
            this.checkBox2.Size = new System.Drawing.Size(25, 18);
            this.checkBox2.TabIndex = 4;
            // 
            // label4
            // 
            this.label4.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label4.Location = new System.Drawing.Point(93, 4);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(144, 20);
            this.label4.Tag = "";
            this.label4.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(2, 78);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(50, 20);
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(2, 51);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(50, 20);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(2, 24);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(50, 20);
            // 
            // textBox3
            // 
            this.textBox3.Location = new System.Drawing.Point(57, 78);
            this.textBox3.Name = "textBox3";
            this.textBox3.ReadOnly = true;
            this.textBox3.Size = new System.Drawing.Size(32, 21);
            this.textBox3.TabIndex = 1;
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(57, 51);
            this.textBox2.MaxLength = 3;
            this.textBox2.Name = "textBox2";
            this.textBox2.ReadOnly = true;
            this.textBox2.Size = new System.Drawing.Size(32, 21);
            this.textBox2.TabIndex = 1;
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(57, 24);
            this.textBox1.MaxLength = 3;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.Size = new System.Drawing.Size(32, 21);
            this.textBox1.TabIndex = 1;
            // 
            // tabPage3
            // 
            this.tabPage3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(224)))), ((int)(((byte)(255)))));
            this.tabPage3.Controls.Add(this.listBox2);
            this.tabPage3.Controls.Add(this.treeView2);
            this.tabPage3.Controls.Add(this.manualIDButton);
            this.tabPage3.Controls.Add(this.textBox4);
            this.tabPage3.Location = new System.Drawing.Point(0, 0);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(240, 217);
            this.tabPage3.Text = "tabPage3";
            // 
            // treeView2
            // 
            this.treeView2.Location = new System.Drawing.Point(0, 0);
            this.treeView2.Name = "treeView2";
            this.treeView2.Size = new System.Drawing.Size(27, 161);
            this.treeView2.TabIndex = 2;
            // 
            // manualIDButton
            // 
            this.manualIDButton.Location = new System.Drawing.Point(8, 167);
            this.manualIDButton.Name = "manualIDButton";
            this.manualIDButton.Size = new System.Drawing.Size(72, 20);
            this.manualIDButton.TabIndex = 1;
            this.manualIDButton.Click += new System.EventHandler(this.manualLookupClick);
            // 
            // textBox4
            // 
            this.textBox4.Location = new System.Drawing.Point(8, 193);
            this.textBox4.MaxLength = 25;
            this.textBox4.Name = "textBox4";
            this.textBox4.Size = new System.Drawing.Size(224, 21);
            this.textBox4.TabIndex = 0;
            // 
            // tabPage4
            // 
            this.tabPage4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(224)))), ((int)(((byte)(255)))));
            this.tabPage4.Controls.Add(this.getSetStatusBox);
            this.tabPage4.Controls.Add(this.comboBox1);
            this.tabPage4.Controls.Add(this.readValueButton);
            this.tabPage4.Controls.Add(this.setValueButton);
            this.tabPage4.Controls.Add(this.textBox5);
            this.tabPage4.Controls.Add(this.textBox6);
            this.tabPage4.Controls.Add(this.textBox8);
            this.tabPage4.Controls.Add(this.textBox7);
            this.tabPage4.Controls.Add(this.label8);
            this.tabPage4.Controls.Add(this.label7);
            this.tabPage4.Controls.Add(this.label6);
            this.tabPage4.Controls.Add(this.label9);
            this.tabPage4.Controls.Add(this.label5);
            this.tabPage4.Location = new System.Drawing.Point(0, 0);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Size = new System.Drawing.Size(240, 217);
            this.tabPage4.Text = "tabPage4";
            // 
            // getSetStatusBox
            // 
            this.getSetStatusBox.Location = new System.Drawing.Point(8, 153);
            this.getSetStatusBox.Name = "getSetStatusBox";
            this.getSetStatusBox.Size = new System.Drawing.Size(100, 21);
            this.getSetStatusBox.TabIndex = 14;
            // 
            // comboBox1
            // 
            this.comboBox1.Items.Add("All");
            this.comboBox1.Items.Add("Violations");
            this.comboBox1.Location = new System.Drawing.Point(105, 81);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(60, 22);
            this.comboBox1.TabIndex = 8;
            // 
            // readValueButton
            // 
            this.readValueButton.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular);
            this.readValueButton.Location = new System.Drawing.Point(161, 194);
            this.readValueButton.Name = "readValueButton";
            this.readValueButton.Size = new System.Drawing.Size(72, 20);
            this.readValueButton.TabIndex = 6;
            this.readValueButton.Text = "Get";
            this.readValueButton.Click += new System.EventHandler(this.readerClick);
            // 
            // setValueButton
            // 
            this.setValueButton.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular);
            this.setValueButton.Location = new System.Drawing.Point(7, 194);
            this.setValueButton.Name = "setValueButton";
            this.setValueButton.Size = new System.Drawing.Size(72, 20);
            this.setValueButton.TabIndex = 6;
            this.setValueButton.Text = "Set";
            this.setValueButton.Click += new System.EventHandler(this.readerClick);
            // 
            // textBox5
            // 
            this.textBox5.Location = new System.Drawing.Point(105, 54);
            this.textBox5.Name = "textBox5";
            this.textBox5.Size = new System.Drawing.Size(32, 21);
            this.textBox5.TabIndex = 5;
            // 
            // textBox6
            // 
            this.textBox6.Location = new System.Drawing.Point(105, 27);
            this.textBox6.MaxLength = 3;
            this.textBox6.Name = "textBox6";
            this.textBox6.Size = new System.Drawing.Size(60, 21);
            this.textBox6.TabIndex = 4;
            // 
            // textBox8
            // 
            this.textBox8.Location = new System.Drawing.Point(105, 109);
            this.textBox8.MaxLength = 3;
            this.textBox8.Name = "textBox8";
            this.textBox8.Size = new System.Drawing.Size(32, 21);
            this.textBox8.TabIndex = 3;
            // 
            // textBox7
            // 
            this.textBox7.Location = new System.Drawing.Point(105, 0);
            this.textBox7.MaxLength = 3;
            this.textBox7.Name = "textBox7";
            this.textBox7.Size = new System.Drawing.Size(60, 21);
            this.textBox7.TabIndex = 3;
            // 
            // label8
            // 
            this.label8.Location = new System.Drawing.Point(8, 81);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(100, 20);
            this.label8.Text = "logmode";
            // 
            // label7
            // 
            this.label7.Location = new System.Drawing.Point(8, 54);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(100, 20);
            this.label7.Text = "logperiod";
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(8, 27);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(100, 20);
            this.label6.Text = "hilimit";
            // 
            // label9
            // 
            this.label9.Location = new System.Drawing.Point(7, 109);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(100, 20);
            this.label9.Text = "batChiItv";
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(8, 4);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(100, 20);
            this.label5.Text = "lolimit";
            // 
            // tabPage5
            // 
            this.tabPage5.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(224)))), ((int)(((byte)(255)))));
            this.tabPage5.Controls.Add(this.medicationButton);
            this.tabPage5.Controls.Add(this.readPatientButton);
            this.tabPage5.Controls.Add(this.label11);
            this.tabPage5.Controls.Add(this.label10);
            this.tabPage5.Controls.Add(this.drugPhoto);
            this.tabPage5.Controls.Add(this.patientPhoto);
            this.tabPage5.Controls.Add(this.patientDescriptionBox);
            this.tabPage5.Controls.Add(this.patientNameBox);
            this.tabPage5.Location = new System.Drawing.Point(0, 0);
            this.tabPage5.Name = "tabPage5";
            this.tabPage5.Size = new System.Drawing.Size(232, 214);
            this.tabPage5.Text = "Patient";
            // 
            // medicationButton
            // 
            this.medicationButton.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.medicationButton.Location = new System.Drawing.Point(61, 135);
            this.medicationButton.Name = "medicationButton";
            this.medicationButton.Size = new System.Drawing.Size(66, 20);
            this.medicationButton.TabIndex = 4;
            this.medicationButton.Text = "Medication";
            this.medicationButton.Click += new System.EventHandler(this.readerClick);
            // 
            // readPatientButton
            // 
            this.readPatientButton.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.readPatientButton.Location = new System.Drawing.Point(8, 135);
            this.readPatientButton.Name = "readPatientButton";
            this.readPatientButton.Size = new System.Drawing.Size(48, 20);
            this.readPatientButton.TabIndex = 4;
            this.readPatientButton.Text = "Patient";
            this.readPatientButton.Click += new System.EventHandler(this.readerClick);
            // 
            // label11
            // 
            this.label11.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label11.Location = new System.Drawing.Point(7, 158);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(100, 12);
            this.label11.Text = "Patient Description";
            // 
            // label10
            // 
            this.label10.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label10.Location = new System.Drawing.Point(133, 135);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(100, 12);
            this.label10.Text = "Patient Name";
            // 
            // drugPhoto
            // 
            this.drugPhoto.Location = new System.Drawing.Point(121, 54);
            this.drugPhoto.Name = "drugPhoto";
            this.drugPhoto.Size = new System.Drawing.Size(112, 54);
            this.drugPhoto.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            // 
            // patientPhoto
            // 
            this.patientPhoto.Location = new System.Drawing.Point(8, 8);
            this.patientPhoto.Name = "patientPhoto";
            this.patientPhoto.Size = new System.Drawing.Size(107, 121);
            this.patientPhoto.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            // 
            // patientDescriptionBox
            // 
            this.patientDescriptionBox.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.patientDescriptionBox.Location = new System.Drawing.Point(8, 174);
            this.patientDescriptionBox.Multiline = true;
            this.patientDescriptionBox.Name = "patientDescriptionBox";
            this.patientDescriptionBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.patientDescriptionBox.Size = new System.Drawing.Size(225, 40);
            this.patientDescriptionBox.TabIndex = 0;
            // 
            // patientNameBox
            // 
            this.patientNameBox.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.patientNameBox.Location = new System.Drawing.Point(133, 151);
            this.patientNameBox.Name = "patientNameBox";
            this.patientNameBox.Size = new System.Drawing.Size(100, 19);
            this.patientNameBox.TabIndex = 0;
            // 
            // gpsPage
            // 
            this.gpsPage.Controls.Add(this.satellitesUsedTextBox);
            this.gpsPage.Controls.Add(this.timeTextBox);
            this.gpsPage.Controls.Add(this.hdopTextBox);
            this.gpsPage.Controls.Add(this.vdopTextBox);
            this.gpsPage.Controls.Add(this.statusTextBox);
            this.gpsPage.Controls.Add(this.speedTextBox);
            this.gpsPage.Controls.Add(this.satLabel8);
            this.gpsPage.Controls.Add(this.satLabel7);
            this.gpsPage.Controls.Add(this.satLabel6);
            this.gpsPage.Controls.Add(this.satLabel5);
            this.gpsPage.Controls.Add(this.satLabel4);
            this.gpsPage.Controls.Add(this.satLabel3);
            this.gpsPage.Controls.Add(this.satLabel2);
            this.gpsPage.Controls.Add(this.satLabel1);
            this.gpsPage.Controls.Add(this.label20);
            this.gpsPage.Controls.Add(this.label19);
            this.gpsPage.Controls.Add(this.label18);
            this.gpsPage.Controls.Add(this.directionTextBox);
            this.gpsPage.Controls.Add(this.label17);
            this.gpsPage.Controls.Add(this.longitudeTextBox);
            this.gpsPage.Controls.Add(this.label16);
            this.gpsPage.Controls.Add(this.latitudeTextBox);
            this.gpsPage.Controls.Add(this.label15);
            this.gpsPage.Controls.Add(this.label14);
            this.gpsPage.Controls.Add(this.label13);
            this.gpsPage.Controls.Add(this.label12);
            this.gpsPage.Controls.Add(this.comPortsComboBox);
            this.gpsPage.Controls.Add(this.connectGPSButton);
            this.gpsPage.Controls.Add(this.scanCOMPortButton);
            this.gpsPage.Controls.Add(this.progressBar8);
            this.gpsPage.Controls.Add(this.progressBar7);
            this.gpsPage.Controls.Add(this.progressBar6);
            this.gpsPage.Controls.Add(this.progressBar5);
            this.gpsPage.Controls.Add(this.progressBar4);
            this.gpsPage.Controls.Add(this.progressBar3);
            this.gpsPage.Controls.Add(this.progressBar2);
            this.gpsPage.Controls.Add(this.progressBar1);
            this.gpsPage.Location = new System.Drawing.Point(0, 0);
            this.gpsPage.Name = "gpsPage";
            this.gpsPage.Size = new System.Drawing.Size(240, 217);
            this.gpsPage.Text = "GPS";
            // 
            // satellitesUsedTextBox
            // 
            this.satellitesUsedTextBox.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.satellitesUsedTextBox.Location = new System.Drawing.Point(72, 50);
            this.satellitesUsedTextBox.Name = "satellitesUsedTextBox";
            this.satellitesUsedTextBox.ReadOnly = true;
            this.satellitesUsedTextBox.Size = new System.Drawing.Size(59, 19);
            this.satellitesUsedTextBox.TabIndex = 14;
            // 
            // timeTextBox
            // 
            this.timeTextBox.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.timeTextBox.Location = new System.Drawing.Point(72, 86);
            this.timeTextBox.Name = "timeTextBox";
            this.timeTextBox.ReadOnly = true;
            this.timeTextBox.Size = new System.Drawing.Size(59, 19);
            this.timeTextBox.TabIndex = 14;
            // 
            // hdopTextBox
            // 
            this.hdopTextBox.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.hdopTextBox.Location = new System.Drawing.Point(88, 121);
            this.hdopTextBox.Name = "hdopTextBox";
            this.hdopTextBox.ReadOnly = true;
            this.hdopTextBox.Size = new System.Drawing.Size(43, 19);
            this.hdopTextBox.TabIndex = 14;
            // 
            // vdopTextBox
            // 
            this.vdopTextBox.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.vdopTextBox.Location = new System.Drawing.Point(88, 156);
            this.vdopTextBox.Name = "vdopTextBox";
            this.vdopTextBox.ReadOnly = true;
            this.vdopTextBox.Size = new System.Drawing.Size(43, 19);
            this.vdopTextBox.TabIndex = 14;
            // 
            // statusTextBox
            // 
            this.statusTextBox.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.statusTextBox.Location = new System.Drawing.Point(72, 191);
            this.statusTextBox.Name = "statusTextBox";
            this.statusTextBox.ReadOnly = true;
            this.statusTextBox.Size = new System.Drawing.Size(59, 19);
            this.statusTextBox.TabIndex = 14;
            // 
            // speedTextBox
            // 
            this.speedTextBox.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.speedTextBox.Location = new System.Drawing.Point(7, 191);
            this.speedTextBox.Name = "speedTextBox";
            this.speedTextBox.ReadOnly = true;
            this.speedTextBox.Size = new System.Drawing.Size(59, 19);
            this.speedTextBox.TabIndex = 14;
            // 
            // satLabel8
            // 
            this.satLabel8.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.satLabel8.Location = new System.Drawing.Point(137, 194);
            this.satLabel8.Name = "satLabel8";
            this.satLabel8.Size = new System.Drawing.Size(19, 17);
            this.satLabel8.Text = "#";
            this.satLabel8.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // satLabel7
            // 
            this.satLabel7.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.satLabel7.Location = new System.Drawing.Point(137, 168);
            this.satLabel7.Name = "satLabel7";
            this.satLabel7.Size = new System.Drawing.Size(19, 17);
            this.satLabel7.Text = "#";
            this.satLabel7.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // satLabel6
            // 
            this.satLabel6.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.satLabel6.Location = new System.Drawing.Point(137, 142);
            this.satLabel6.Name = "satLabel6";
            this.satLabel6.Size = new System.Drawing.Size(19, 17);
            this.satLabel6.Text = "#";
            this.satLabel6.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // satLabel5
            // 
            this.satLabel5.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.satLabel5.Location = new System.Drawing.Point(137, 116);
            this.satLabel5.Name = "satLabel5";
            this.satLabel5.Size = new System.Drawing.Size(19, 17);
            this.satLabel5.Text = "#";
            this.satLabel5.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // satLabel4
            // 
            this.satLabel4.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.satLabel4.Location = new System.Drawing.Point(137, 90);
            this.satLabel4.Name = "satLabel4";
            this.satLabel4.Size = new System.Drawing.Size(19, 17);
            this.satLabel4.Text = "#";
            this.satLabel4.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // satLabel3
            // 
            this.satLabel3.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.satLabel3.Location = new System.Drawing.Point(137, 64);
            this.satLabel3.Name = "satLabel3";
            this.satLabel3.Size = new System.Drawing.Size(19, 17);
            this.satLabel3.Text = "#";
            this.satLabel3.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // satLabel2
            // 
            this.satLabel2.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.satLabel2.Location = new System.Drawing.Point(137, 38);
            this.satLabel2.Name = "satLabel2";
            this.satLabel2.Size = new System.Drawing.Size(19, 17);
            this.satLabel2.Text = "#";
            this.satLabel2.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // satLabel1
            // 
            this.satLabel1.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.satLabel1.Location = new System.Drawing.Point(137, 12);
            this.satLabel1.Name = "satLabel1";
            this.satLabel1.Size = new System.Drawing.Size(19, 17);
            this.satLabel1.Text = "#";
            this.satLabel1.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // label20
            // 
            this.label20.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label20.Location = new System.Drawing.Point(72, 35);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(59, 14);
            this.label20.Text = "Satellites";
            this.label20.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // label19
            // 
            this.label19.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label19.Location = new System.Drawing.Point(72, 71);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(59, 14);
            this.label19.Text = "Time";
            this.label19.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // label18
            // 
            this.label18.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label18.Location = new System.Drawing.Point(88, 106);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(43, 14);
            this.label18.Text = "HDOP";
            this.label18.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // directionTextBox
            // 
            this.directionTextBox.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.directionTextBox.Location = new System.Drawing.Point(72, 16);
            this.directionTextBox.Name = "directionTextBox";
            this.directionTextBox.ReadOnly = true;
            this.directionTextBox.Size = new System.Drawing.Size(59, 19);
            this.directionTextBox.TabIndex = 14;
            // 
            // label17
            // 
            this.label17.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label17.Location = new System.Drawing.Point(88, 141);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(43, 14);
            this.label17.Text = "VDOP";
            this.label17.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // longitudeTextBox
            // 
            this.longitudeTextBox.Font = new System.Drawing.Font("Tahoma", 7F, System.Drawing.FontStyle.Regular);
            this.longitudeTextBox.Location = new System.Drawing.Point(7, 157);
            this.longitudeTextBox.Name = "longitudeTextBox";
            this.longitudeTextBox.ReadOnly = true;
            this.longitudeTextBox.Size = new System.Drawing.Size(75, 18);
            this.longitudeTextBox.TabIndex = 14;
            // 
            // label16
            // 
            this.label16.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label16.Location = new System.Drawing.Point(72, 176);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(59, 14);
            this.label16.Text = "Status";
            this.label16.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // latitudeTextBox
            // 
            this.latitudeTextBox.Font = new System.Drawing.Font("Tahoma", 7F, System.Drawing.FontStyle.Regular);
            this.latitudeTextBox.Location = new System.Drawing.Point(7, 122);
            this.latitudeTextBox.Name = "latitudeTextBox";
            this.latitudeTextBox.ReadOnly = true;
            this.latitudeTextBox.Size = new System.Drawing.Size(75, 18);
            this.latitudeTextBox.TabIndex = 14;
            // 
            // label15
            // 
            this.label15.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label15.Location = new System.Drawing.Point(7, 176);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(59, 14);
            this.label15.Text = "Speed";
            this.label15.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // label14
            // 
            this.label14.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label14.Location = new System.Drawing.Point(72, 1);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(59, 14);
            this.label14.Text = "Direction";
            this.label14.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // label13
            // 
            this.label13.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label13.Location = new System.Drawing.Point(7, 142);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(75, 14);
            this.label13.Text = "Longitude";
            this.label13.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // label12
            // 
            this.label12.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.label12.Location = new System.Drawing.Point(7, 107);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(75, 14);
            this.label12.Text = "Latitude";
            this.label12.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // comPortsComboBox
            // 
            this.comPortsComboBox.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.comPortsComboBox.Location = new System.Drawing.Point(7, 27);
            this.comPortsComboBox.Name = "comPortsComboBox";
            this.comPortsComboBox.Size = new System.Drawing.Size(59, 20);
            this.comPortsComboBox.TabIndex = 9;
            // 
            // connectGPSButton
            // 
            this.connectGPSButton.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.connectGPSButton.Location = new System.Drawing.Point(7, 50);
            this.connectGPSButton.Name = "connectGPSButton";
            this.connectGPSButton.Size = new System.Drawing.Size(59, 20);
            this.connectGPSButton.TabIndex = 8;
            this.connectGPSButton.Text = "Connect";
            this.connectGPSButton.Click += new System.EventHandler(this.connectGPSButton_Click);
            // 
            // scanCOMPortButton
            // 
            this.scanCOMPortButton.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.scanCOMPortButton.Location = new System.Drawing.Point(7, 6);
            this.scanCOMPortButton.Name = "scanCOMPortButton";
            this.scanCOMPortButton.Size = new System.Drawing.Size(59, 20);
            this.scanCOMPortButton.TabIndex = 8;
            this.scanCOMPortButton.Text = "scanCOM";
            this.scanCOMPortButton.Click += new System.EventHandler(this.scanCOMPortButton_Click);
            // 
            // progressBar8
            // 
            this.progressBar8.Location = new System.Drawing.Point(162, 191);
            this.progressBar8.Maximum = 99;
            this.progressBar8.Name = "progressBar8";
            this.progressBar8.Size = new System.Drawing.Size(71, 20);
            // 
            // progressBar7
            // 
            this.progressBar7.Location = new System.Drawing.Point(162, 165);
            this.progressBar7.Maximum = 99;
            this.progressBar7.Name = "progressBar7";
            this.progressBar7.Size = new System.Drawing.Size(71, 20);
            // 
            // progressBar6
            // 
            this.progressBar6.Location = new System.Drawing.Point(162, 139);
            this.progressBar6.Maximum = 99;
            this.progressBar6.Name = "progressBar6";
            this.progressBar6.Size = new System.Drawing.Size(71, 20);
            // 
            // progressBar5
            // 
            this.progressBar5.Location = new System.Drawing.Point(162, 113);
            this.progressBar5.Maximum = 99;
            this.progressBar5.Name = "progressBar5";
            this.progressBar5.Size = new System.Drawing.Size(71, 20);
            // 
            // progressBar4
            // 
            this.progressBar4.Location = new System.Drawing.Point(162, 87);
            this.progressBar4.Maximum = 99;
            this.progressBar4.Name = "progressBar4";
            this.progressBar4.Size = new System.Drawing.Size(71, 20);
            // 
            // progressBar3
            // 
            this.progressBar3.Location = new System.Drawing.Point(162, 61);
            this.progressBar3.Maximum = 99;
            this.progressBar3.Name = "progressBar3";
            this.progressBar3.Size = new System.Drawing.Size(71, 20);
            // 
            // progressBar2
            // 
            this.progressBar2.Location = new System.Drawing.Point(162, 35);
            this.progressBar2.Maximum = 99;
            this.progressBar2.Name = "progressBar2";
            this.progressBar2.Size = new System.Drawing.Size(71, 20);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(162, 9);
            this.progressBar1.Maximum = 99;
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(71, 20);
            // 
            // gpsSerialPort
            // 
            this.gpsSerialPort.BaudRate = 4800;
            this.gpsSerialPort.PortName = "COM7";
            this.gpsSerialPort.ReadBufferSize = 8192;
            this.gpsSerialPort.ReceivedBytesThreshold = 512;
            this.gpsSerialPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.gpsSerialPort_DataReceived);
            // 
            // listBox2
            // 
            this.listBox2.Location = new System.Drawing.Point(34, 8);
            this.listBox2.Name = "listBox2";
            this.listBox2.Size = new System.Drawing.Size(199, 156);
            this.listBox2.TabIndex = 3;
            // 
            // mainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 320);
            this.Controls.Add(this.tabControl1);
            this.KeyPreview = true;
            this.Location = new System.Drawing.Point(0, 0);
            this.MinimizeBox = false;
            this.Name = "mainForm";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.mainForm_KeyDown);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.tabPage3.ResumeLayout(false);
            this.tabPage4.ResumeLayout(false);
            this.tabPage5.ResumeLayout(false);
            this.gpsPage.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button readIDButton;
        private System.Windows.Forms.Button readLogButton;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.CheckBox checkBox2;
        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.Button manualIDButton;
        private System.Windows.Forms.TextBox textBox4;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TreeView treeView2;
        private System.Windows.Forms.Button readValueButton;
        private System.Windows.Forms.Button setValueButton;
        private System.Windows.Forms.TextBox textBox5;
        private System.Windows.Forms.TextBox textBox6;
        private System.Windows.Forms.TextBox textBox7;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.ComboBox comboBox1;
        private System.Windows.Forms.TextBox textBox8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox textBox9;
        private System.Windows.Forms.TabPage tabPage5;
        private System.Windows.Forms.Button medicationButton;
        private System.Windows.Forms.Button readPatientButton;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.PictureBox patientPhoto;
        private System.Windows.Forms.TextBox patientDescriptionBox;
        private System.Windows.Forms.TextBox patientNameBox;
        private System.Windows.Forms.PictureBox drugPhoto;
        private System.Windows.Forms.TextBox getSetStatusBox;
        private System.Windows.Forms.CheckBox checkBox1;
        private System.Windows.Forms.TabPage gpsPage;
        private System.Windows.Forms.ProgressBar progressBar7;
        private System.Windows.Forms.ProgressBar progressBar6;
        private System.Windows.Forms.ProgressBar progressBar5;
        private System.Windows.Forms.ProgressBar progressBar4;
        private System.Windows.Forms.ProgressBar progressBar3;
        private System.Windows.Forms.ProgressBar progressBar2;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.ComboBox comPortsComboBox;
        private System.Windows.Forms.Button connectGPSButton;
        private System.Windows.Forms.Button scanCOMPortButton;
        private System.Windows.Forms.ProgressBar progressBar8;
        private System.Windows.Forms.TextBox speedTextBox;
        private System.Windows.Forms.TextBox directionTextBox;
        private System.Windows.Forms.TextBox longitudeTextBox;
        private System.Windows.Forms.TextBox latitudeTextBox;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox hdopTextBox;
        private System.Windows.Forms.TextBox vdopTextBox;
        private System.Windows.Forms.TextBox statusTextBox;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.TextBox satellitesUsedTextBox;
        private System.Windows.Forms.TextBox timeTextBox;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Label label19;
        private System.IO.Ports.SerialPort gpsSerialPort;
        private System.Windows.Forms.Label satLabel8;
        private System.Windows.Forms.Label satLabel7;
        private System.Windows.Forms.Label satLabel6;
        private System.Windows.Forms.Label satLabel5;
        private System.Windows.Forms.Label satLabel4;
        private System.Windows.Forms.Label satLabel3;
        private System.Windows.Forms.Label satLabel2;
        private System.Windows.Forms.Label satLabel1;
        private System.Windows.Forms.ListBox listBox2;
    }
}

