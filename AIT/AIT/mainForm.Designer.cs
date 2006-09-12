namespace AIT
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
            this.startInventoryButton = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.invTimerBox = new System.Windows.Forms.ComboBox();
            this.invProgressBar = new System.Windows.Forms.ProgressBar();
            this.clearBtn = new System.Windows.Forms.Button();
            this.reconcileButton = new System.Windows.Forms.Button();
            this.scanButton = new System.Windows.Forms.Button();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.numTags = new System.Windows.Forms.NumericUpDown();
            this.lblOnline = new System.Windows.Forms.Label();
            this.numericUpDown1 = new System.Windows.Forms.NumericUpDown();
            this.radioButton2 = new System.Windows.Forms.RadioButton();
            this.radioButton1 = new System.Windows.Forms.RadioButton();
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.cbComPort = new System.Windows.Forms.ComboBox();
            this.wjConnectButton = new System.Windows.Forms.Button();
            this.label17 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.domainUpDown4 = new System.Windows.Forms.DomainUpDown();
            this.domainUpDown2 = new System.Windows.Forms.DomainUpDown();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.connectBtn = new System.Windows.Forms.Button();
            this.label19 = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.dbTableLbl = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.dbNameLbl = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.ipPhoneBox = new System.Windows.Forms.TextBox();
            this.tagServerBox = new System.Windows.Forms.TextBox();
            this.dbTableBox = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.dbNameBox = new System.Windows.Forms.TextBox();
            this.portBox = new System.Windows.Forms.TextBox();
            this.hostnameBox = new System.Windows.Forms.TextBox();
            this.passwordBox = new System.Windows.Forms.TextBox();
            this.userIDBox = new System.Windows.Forms.TextBox();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.satBox = new System.Windows.Forms.TextBox();
            this.utcBox = new System.Windows.Forms.TextBox();
            this.longBox = new System.Windows.Forms.TextBox();
            this.latBox = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.btComPort = new System.Windows.Forms.ComboBox();
            this.label12 = new System.Windows.Forms.Label();
            this.mapButton = new System.Windows.Forms.Button();
            this.gpsButton = new System.Windows.Forms.Button();
            this.label11 = new System.Windows.Forms.Label();
            this.ownerBox1 = new System.Windows.Forms.ComboBox();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.ownerLbl = new System.Windows.Forms.Label();
            this.tabPage5 = new System.Windows.Forms.TabPage();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label16 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.radConnectButton = new System.Windows.Forms.Button();
            this.radUpDown = new System.Windows.Forms.NumericUpDown();
            this.gpsSerialPort = new System.IO.Ports.SerialPort(this.components);
            this.radTimer = new System.Windows.Forms.Timer();
            this.invTimer = new System.Windows.Forms.Timer();
            this.inputPanel1 = new Microsoft.WindowsCE.Forms.InputPanel();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.tabPage5.SuspendLayout();
            this.SuspendLayout();
            // 
            // startInventoryButton
            // 
            this.startInventoryButton.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.startInventoryButton.BackColor = System.Drawing.Color.LightGreen;
            this.startInventoryButton.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular);
            this.startInventoryButton.ForeColor = System.Drawing.Color.Black;
            this.startInventoryButton.Location = new System.Drawing.Point(1, 246);
            this.startInventoryButton.Name = "startInventoryButton";
            this.startInventoryButton.Size = new System.Drawing.Size(35, 20);
            this.startInventoryButton.TabIndex = 0;
            this.startInventoryButton.Text = "Man";
            this.startInventoryButton.Click += new System.EventHandler(this.startInventoryButton_Click);
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Controls.Add(this.tabPage5);
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(240, 294);
            this.tabControl1.TabIndex = 2;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.invTimerBox);
            this.tabPage1.Controls.Add(this.invProgressBar);
            this.tabPage1.Controls.Add(this.clearBtn);
            this.tabPage1.Controls.Add(this.reconcileButton);
            this.tabPage1.Controls.Add(this.scanButton);
            this.tabPage1.Controls.Add(this.startInventoryButton);
            this.tabPage1.Location = new System.Drawing.Point(0, 0);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Size = new System.Drawing.Size(240, 271);
            this.tabPage1.Text = "Inv";
            this.tabPage1.MouseUp += new System.Windows.Forms.MouseEventHandler(this.tabPage1_MouseUp);
            // 
            // invTimerBox
            // 
            this.invTimerBox.Font = new System.Drawing.Font("Tahoma", 8F, System.Drawing.FontStyle.Regular);
            this.invTimerBox.Items.Add("Manual");
            this.invTimerBox.Items.Add("20s");
            this.invTimerBox.Items.Add("30s");
            this.invTimerBox.Items.Add("40s");
            this.invTimerBox.Items.Add("60s");
            this.invTimerBox.Items.Add("80s");
            this.invTimerBox.Items.Add("120s");
            this.invTimerBox.Items.Add("240s");
            this.invTimerBox.Location = new System.Drawing.Point(127, 246);
            this.invTimerBox.Name = "invTimerBox";
            this.invTimerBox.Size = new System.Drawing.Size(69, 20);
            this.invTimerBox.TabIndex = 3;
            this.invTimerBox.SelectedIndexChanged += new System.EventHandler(this.invTimerBox_SelectedIndexChanged);
            // 
            // invProgressBar
            // 
            this.invProgressBar.Location = new System.Drawing.Point(139, 246);
            this.invProgressBar.Maximum = 5;
            this.invProgressBar.Name = "invProgressBar";
            this.invProgressBar.Size = new System.Drawing.Size(55, 20);
            this.invProgressBar.Visible = false;
            // 
            // clearBtn
            // 
            this.clearBtn.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.clearBtn.BackColor = System.Drawing.Color.Red;
            this.clearBtn.Font = new System.Drawing.Font("Tahoma", 7F, System.Drawing.FontStyle.Bold);
            this.clearBtn.Location = new System.Drawing.Point(203, 246);
            this.clearBtn.Name = "clearBtn";
            this.clearBtn.Size = new System.Drawing.Size(32, 20);
            this.clearBtn.TabIndex = 2;
            this.clearBtn.Text = "Clear";
            this.clearBtn.Click += new System.EventHandler(this.clearBtn_Click);
            // 
            // reconcileButton
            // 
            this.reconcileButton.BackColor = System.Drawing.Color.DeepPink;
            this.reconcileButton.Enabled = false;
            this.reconcileButton.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular);
            this.reconcileButton.ForeColor = System.Drawing.Color.Black;
            this.reconcileButton.Location = new System.Drawing.Point(85, 246);
            this.reconcileButton.Name = "reconcileButton";
            this.reconcileButton.Size = new System.Drawing.Size(35, 20);
            this.reconcileButton.TabIndex = 0;
            this.reconcileButton.Text = "Rec";
            this.reconcileButton.Click += new System.EventHandler(this.reconcileButton_Click);
            // 
            // scanButton
            // 
            this.scanButton.BackColor = System.Drawing.Color.Turquoise;
            this.scanButton.Font = new System.Drawing.Font("Tahoma", 9F, System.Drawing.FontStyle.Regular);
            this.scanButton.ForeColor = System.Drawing.Color.Black;
            this.scanButton.Location = new System.Drawing.Point(43, 246);
            this.scanButton.Name = "scanButton";
            this.scanButton.Size = new System.Drawing.Size(35, 20);
            this.scanButton.TabIndex = 0;
            this.scanButton.Text = "Scan";
            this.scanButton.Click += new System.EventHandler(this.startInventoryButton_Click);
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.numTags);
            this.tabPage2.Controls.Add(this.lblOnline);
            this.tabPage2.Controls.Add(this.numericUpDown1);
            this.tabPage2.Controls.Add(this.radioButton2);
            this.tabPage2.Controls.Add(this.radioButton1);
            this.tabPage2.Controls.Add(this.comboBox1);
            this.tabPage2.Controls.Add(this.cbComPort);
            this.tabPage2.Controls.Add(this.wjConnectButton);
            this.tabPage2.Controls.Add(this.label17);
            this.tabPage2.Controls.Add(this.label14);
            this.tabPage2.Controls.Add(this.label4);
            this.tabPage2.Controls.Add(this.label3);
            this.tabPage2.Controls.Add(this.label2);
            this.tabPage2.Controls.Add(this.label1);
            this.tabPage2.Controls.Add(this.domainUpDown4);
            this.tabPage2.Controls.Add(this.domainUpDown2);
            this.tabPage2.Location = new System.Drawing.Point(0, 0);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Size = new System.Drawing.Size(232, 268);
            this.tabPage2.Text = "Setup";
            // 
            // numTags
            // 
            this.numTags.Location = new System.Drawing.Point(123, 128);
            this.numTags.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.numTags.Name = "numTags";
            this.numTags.Size = new System.Drawing.Size(100, 22);
            this.numTags.TabIndex = 23;
            this.numTags.Value = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.numTags.ValueChanged += new System.EventHandler(this.numTags_ValueChanged);
            // 
            // lblOnline
            // 
            this.lblOnline.Location = new System.Drawing.Point(114, 223);
            this.lblOnline.Name = "lblOnline";
            this.lblOnline.Size = new System.Drawing.Size(70, 20);
            this.lblOnline.Text = "Offline";
            // 
            // numericUpDown1
            // 
            this.numericUpDown1.Location = new System.Drawing.Point(123, 18);
            this.numericUpDown1.Maximum = new decimal(new int[] {
            27,
            0,
            0,
            0});
            this.numericUpDown1.Minimum = new decimal(new int[] {
            15,
            0,
            0,
            0});
            this.numericUpDown1.Name = "numericUpDown1";
            this.numericUpDown1.Size = new System.Drawing.Size(100, 22);
            this.numericUpDown1.TabIndex = 17;
            this.numericUpDown1.Value = new decimal(new int[] {
            27,
            0,
            0,
            0});
            this.numericUpDown1.ValueChanged += new System.EventHandler(this.numericUpDown1_ValueChanged);
            // 
            // radioButton2
            // 
            this.radioButton2.Checked = true;
            this.radioButton2.Location = new System.Drawing.Point(17, 178);
            this.radioButton2.Name = "radioButton2";
            this.radioButton2.Size = new System.Drawing.Size(100, 20);
            this.radioButton2.TabIndex = 11;
            this.radioButton2.Text = "Ant A";
            this.radioButton2.CheckedChanged += new System.EventHandler(this.radioButton2_CheckedChanged);
            // 
            // radioButton1
            // 
            this.radioButton1.Location = new System.Drawing.Point(123, 178);
            this.radioButton1.Name = "radioButton1";
            this.radioButton1.Size = new System.Drawing.Size(100, 20);
            this.radioButton1.TabIndex = 11;
            this.radioButton1.Text = "Ant B";
            // 
            // comboBox1
            // 
            this.comboBox1.Items.Add("Symbol");
            this.comboBox1.Items.Add("Intermec");
            this.comboBox1.Items.Add("WJ");
            this.comboBox1.Location = new System.Drawing.Point(123, 156);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(100, 22);
            this.comboBox1.TabIndex = 6;
            // 
            // cbComPort
            // 
            this.cbComPort.Location = new System.Drawing.Point(123, 74);
            this.cbComPort.Name = "cbComPort";
            this.cbComPort.Size = new System.Drawing.Size(100, 22);
            this.cbComPort.TabIndex = 6;
            // 
            // wjConnectButton
            // 
            this.wjConnectButton.Location = new System.Drawing.Point(25, 223);
            this.wjConnectButton.Name = "wjConnectButton";
            this.wjConnectButton.Size = new System.Drawing.Size(72, 20);
            this.wjConnectButton.TabIndex = 5;
            this.wjConnectButton.Text = "Connect";
            this.wjConnectButton.Click += new System.EventHandler(this.wjConnectButton_Click);
            // 
            // label17
            // 
            this.label17.Location = new System.Drawing.Point(17, 158);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(100, 20);
            this.label17.Text = "Reader Selected";
            this.label17.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label14
            // 
            this.label14.Location = new System.Drawing.Point(50, 130);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(67, 20);
            this.label14.Text = "Num Tags";
            this.label14.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(50, 102);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(67, 20);
            this.label4.Text = "Baud Rate";
            this.label4.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(57, 74);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(60, 20);
            this.label3.Text = "COM Port";
            this.label3.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(7, 46);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(110, 20);
            this.label2.Text = "Inventory Duration";
            this.label2.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(57, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(60, 20);
            this.label1.Text = "RF Power";
            this.label1.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // domainUpDown4
            // 
            this.domainUpDown4.Location = new System.Drawing.Point(123, 102);
            this.domainUpDown4.Name = "domainUpDown4";
            this.domainUpDown4.ReadOnly = true;
            this.domainUpDown4.Size = new System.Drawing.Size(100, 22);
            this.domainUpDown4.TabIndex = 0;
            this.domainUpDown4.Text = "57600";
            // 
            // domainUpDown2
            // 
            this.domainUpDown2.Location = new System.Drawing.Point(123, 46);
            this.domainUpDown2.Name = "domainUpDown2";
            this.domainUpDown2.ReadOnly = true;
            this.domainUpDown2.Size = new System.Drawing.Size(100, 22);
            this.domainUpDown2.TabIndex = 0;
            this.domainUpDown2.Text = "1";
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.connectBtn);
            this.tabPage3.Controls.Add(this.label19);
            this.tabPage3.Controls.Add(this.label18);
            this.tabPage3.Controls.Add(this.dbTableLbl);
            this.tabPage3.Controls.Add(this.label8);
            this.tabPage3.Controls.Add(this.dbNameLbl);
            this.tabPage3.Controls.Add(this.label7);
            this.tabPage3.Controls.Add(this.label6);
            this.tabPage3.Controls.Add(this.ipPhoneBox);
            this.tabPage3.Controls.Add(this.tagServerBox);
            this.tabPage3.Controls.Add(this.dbTableBox);
            this.tabPage3.Controls.Add(this.label5);
            this.tabPage3.Controls.Add(this.dbNameBox);
            this.tabPage3.Controls.Add(this.portBox);
            this.tabPage3.Controls.Add(this.hostnameBox);
            this.tabPage3.Controls.Add(this.passwordBox);
            this.tabPage3.Controls.Add(this.userIDBox);
            this.tabPage3.Location = new System.Drawing.Point(0, 0);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(240, 271);
            this.tabPage3.Text = "Server";
            // 
            // connectBtn
            // 
            this.connectBtn.Location = new System.Drawing.Point(95, 236);
            this.connectBtn.Name = "connectBtn";
            this.connectBtn.Size = new System.Drawing.Size(75, 19);
            this.connectBtn.TabIndex = 4;
            this.connectBtn.Text = "Test";
            // 
            // label19
            // 
            this.label19.Location = new System.Drawing.Point(7, 210);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(82, 20);
            this.label19.Text = "IP Phone";
            this.label19.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label18
            // 
            this.label18.Location = new System.Drawing.Point(7, 184);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(82, 20);
            this.label18.Text = "Tag Server";
            this.label18.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // dbTableLbl
            // 
            this.dbTableLbl.Location = new System.Drawing.Point(7, 157);
            this.dbTableLbl.Name = "dbTableLbl";
            this.dbTableLbl.Size = new System.Drawing.Size(82, 20);
            this.dbTableLbl.Text = "Table Name";
            this.dbTableLbl.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label8
            // 
            this.label8.Location = new System.Drawing.Point(33, 107);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(56, 20);
            this.label8.Text = "Port";
            this.label8.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // dbNameLbl
            // 
            this.dbNameLbl.Location = new System.Drawing.Point(33, 130);
            this.dbNameLbl.Name = "dbNameLbl";
            this.dbNameLbl.Size = new System.Drawing.Size(56, 20);
            this.dbNameLbl.Text = "DB Name";
            this.dbNameLbl.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label7
            // 
            this.label7.Location = new System.Drawing.Point(33, 80);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(56, 20);
            this.label7.Text = "Server";
            this.label7.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(33, 53);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(56, 20);
            this.label6.Text = "Password";
            this.label6.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // ipPhoneBox
            // 
            this.ipPhoneBox.Location = new System.Drawing.Point(95, 207);
            this.ipPhoneBox.Name = "ipPhoneBox";
            this.ipPhoneBox.Size = new System.Drawing.Size(138, 21);
            this.ipPhoneBox.TabIndex = 0;
            this.ipPhoneBox.LostFocus += new System.EventHandler(this.textBox1_LostFocus);
            this.ipPhoneBox.GotFocus += new System.EventHandler(this.textBox1_GotFocus);
            // 
            // tagServerBox
            // 
            this.tagServerBox.Location = new System.Drawing.Point(95, 181);
            this.tagServerBox.Name = "tagServerBox";
            this.tagServerBox.Size = new System.Drawing.Size(138, 21);
            this.tagServerBox.TabIndex = 0;
            this.tagServerBox.LostFocus += new System.EventHandler(this.textBox1_LostFocus);
            this.tagServerBox.GotFocus += new System.EventHandler(this.textBox1_GotFocus);
            // 
            // dbTableBox
            // 
            this.dbTableBox.Location = new System.Drawing.Point(95, 155);
            this.dbTableBox.Name = "dbTableBox";
            this.dbTableBox.Size = new System.Drawing.Size(138, 21);
            this.dbTableBox.TabIndex = 0;
            this.dbTableBox.LostFocus += new System.EventHandler(this.textBox1_LostFocus);
            this.dbTableBox.GotFocus += new System.EventHandler(this.textBox1_GotFocus);
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(33, 26);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(56, 20);
            this.label5.Text = "UserID";
            this.label5.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // dbNameBox
            // 
            this.dbNameBox.Location = new System.Drawing.Point(95, 129);
            this.dbNameBox.Name = "dbNameBox";
            this.dbNameBox.Size = new System.Drawing.Size(138, 21);
            this.dbNameBox.TabIndex = 0;
            this.dbNameBox.LostFocus += new System.EventHandler(this.textBox1_LostFocus);
            this.dbNameBox.GotFocus += new System.EventHandler(this.textBox1_GotFocus);
            // 
            // portBox
            // 
            this.portBox.Location = new System.Drawing.Point(95, 103);
            this.portBox.Name = "portBox";
            this.portBox.Size = new System.Drawing.Size(138, 21);
            this.portBox.TabIndex = 0;
            this.portBox.LostFocus += new System.EventHandler(this.textBox1_LostFocus);
            this.portBox.GotFocus += new System.EventHandler(this.textBox1_GotFocus);
            // 
            // hostnameBox
            // 
            this.hostnameBox.Location = new System.Drawing.Point(95, 77);
            this.hostnameBox.Name = "hostnameBox";
            this.hostnameBox.Size = new System.Drawing.Size(138, 21);
            this.hostnameBox.TabIndex = 0;
            this.hostnameBox.LostFocus += new System.EventHandler(this.hostnameBox_LostFocus);
            this.hostnameBox.GotFocus += new System.EventHandler(this.textBox1_GotFocus);
            // 
            // passwordBox
            // 
            this.passwordBox.Location = new System.Drawing.Point(95, 51);
            this.passwordBox.Name = "passwordBox";
            this.passwordBox.PasswordChar = '*';
            this.passwordBox.Size = new System.Drawing.Size(138, 21);
            this.passwordBox.TabIndex = 0;
            this.passwordBox.LostFocus += new System.EventHandler(this.textBox1_LostFocus);
            this.passwordBox.GotFocus += new System.EventHandler(this.textBox1_GotFocus);
            // 
            // userIDBox
            // 
            this.userIDBox.Location = new System.Drawing.Point(95, 25);
            this.userIDBox.Name = "userIDBox";
            this.userIDBox.Size = new System.Drawing.Size(138, 21);
            this.userIDBox.TabIndex = 0;
            this.userIDBox.LostFocus += new System.EventHandler(this.textBox1_LostFocus);
            this.userIDBox.GotFocus += new System.EventHandler(this.textBox1_GotFocus);
            // 
            // tabPage4
            // 
            this.tabPage4.Controls.Add(this.satBox);
            this.tabPage4.Controls.Add(this.utcBox);
            this.tabPage4.Controls.Add(this.longBox);
            this.tabPage4.Controls.Add(this.latBox);
            this.tabPage4.Controls.Add(this.label13);
            this.tabPage4.Controls.Add(this.btComPort);
            this.tabPage4.Controls.Add(this.label12);
            this.tabPage4.Controls.Add(this.mapButton);
            this.tabPage4.Controls.Add(this.gpsButton);
            this.tabPage4.Controls.Add(this.label11);
            this.tabPage4.Controls.Add(this.ownerBox1);
            this.tabPage4.Controls.Add(this.label10);
            this.tabPage4.Controls.Add(this.label9);
            this.tabPage4.Controls.Add(this.ownerLbl);
            this.tabPage4.Location = new System.Drawing.Point(0, 0);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Size = new System.Drawing.Size(232, 268);
            this.tabPage4.Text = "GPS";
            // 
            // satBox
            // 
            this.satBox.Location = new System.Drawing.Point(71, 85);
            this.satBox.Name = "satBox";
            this.satBox.ReadOnly = true;
            this.satBox.Size = new System.Drawing.Size(100, 21);
            this.satBox.TabIndex = 12;
            // 
            // utcBox
            // 
            this.utcBox.Location = new System.Drawing.Point(71, 58);
            this.utcBox.Name = "utcBox";
            this.utcBox.ReadOnly = true;
            this.utcBox.Size = new System.Drawing.Size(100, 21);
            this.utcBox.TabIndex = 12;
            // 
            // longBox
            // 
            this.longBox.Location = new System.Drawing.Point(71, 31);
            this.longBox.Name = "longBox";
            this.longBox.ReadOnly = true;
            this.longBox.Size = new System.Drawing.Size(162, 21);
            this.longBox.TabIndex = 12;
            // 
            // latBox
            // 
            this.latBox.Location = new System.Drawing.Point(71, 4);
            this.latBox.Name = "latBox";
            this.latBox.ReadOnly = true;
            this.latBox.Size = new System.Drawing.Size(162, 21);
            this.latBox.TabIndex = 12;
            // 
            // label13
            // 
            this.label13.Location = new System.Drawing.Point(7, 85);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(58, 20);
            this.label13.Text = "Sat. Used";
            this.label13.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // btComPort
            // 
            this.btComPort.Location = new System.Drawing.Point(63, 218);
            this.btComPort.Name = "btComPort";
            this.btComPort.Size = new System.Drawing.Size(86, 22);
            this.btComPort.TabIndex = 8;
            // 
            // label12
            // 
            this.label12.Location = new System.Drawing.Point(7, 58);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(58, 20);
            this.label12.Text = "UTC Time";
            this.label12.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // mapButton
            // 
            this.mapButton.Location = new System.Drawing.Point(7, 112);
            this.mapButton.Name = "mapButton";
            this.mapButton.Size = new System.Drawing.Size(80, 20);
            this.mapButton.TabIndex = 5;
            this.mapButton.Text = "Map It!";
            this.mapButton.Click += new System.EventHandler(this.mapButton_Click);
            // 
            // gpsButton
            // 
            this.gpsButton.Location = new System.Drawing.Point(157, 246);
            this.gpsButton.Name = "gpsButton";
            this.gpsButton.Size = new System.Drawing.Size(80, 20);
            this.gpsButton.TabIndex = 5;
            this.gpsButton.Text = "GPSconnect";
            this.gpsButton.Click += new System.EventHandler(this.BTbutton_Click);
            // 
            // label11
            // 
            this.label11.Location = new System.Drawing.Point(7, 31);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(58, 20);
            this.label11.Text = "Longitude";
            this.label11.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // ownerBox1
            // 
            this.ownerBox1.Items.Add("1");
            this.ownerBox1.Items.Add("12");
            this.ownerBox1.Items.Add("123");
            this.ownerBox1.Items.Add("1234");
            this.ownerBox1.Items.Add("12345");
            this.ownerBox1.Location = new System.Drawing.Point(63, 246);
            this.ownerBox1.Name = "ownerBox1";
            this.ownerBox1.Size = new System.Drawing.Size(86, 22);
            this.ownerBox1.TabIndex = 2;
            // 
            // label10
            // 
            this.label10.Location = new System.Drawing.Point(7, 4);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(57, 20);
            this.label10.Text = "Latitude";
            this.label10.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // label9
            // 
            this.label9.Location = new System.Drawing.Point(0, 223);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(57, 20);
            this.label9.Text = "GPS Port";
            // 
            // ownerLbl
            // 
            this.ownerLbl.Location = new System.Drawing.Point(7, 251);
            this.ownerLbl.Name = "ownerLbl";
            this.ownerLbl.Size = new System.Drawing.Size(40, 20);
            this.ownerLbl.Text = "Owner";
            // 
            // tabPage5
            // 
            this.tabPage5.Controls.Add(this.textBox1);
            this.tabPage5.Controls.Add(this.label16);
            this.tabPage5.Controls.Add(this.label15);
            this.tabPage5.Controls.Add(this.radConnectButton);
            this.tabPage5.Controls.Add(this.radUpDown);
            this.tabPage5.Location = new System.Drawing.Point(0, 0);
            this.tabPage5.Name = "tabPage5";
            this.tabPage5.Size = new System.Drawing.Size(232, 268);
            this.tabPage5.Text = "Rad";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(133, 117);
            this.textBox1.MaxLength = 10;
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(80, 21);
            this.textBox1.TabIndex = 20;
            this.textBox1.Text = "3012330583";
            this.textBox1.LostFocus += new System.EventHandler(this.textBox1_LostFocus);
            this.textBox1.GotFocus += new System.EventHandler(this.textBox1_GotFocus);
            // 
            // label16
            // 
            this.label16.Location = new System.Drawing.Point(27, 117);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(100, 20);
            this.label16.Text = "Phone Number";
            this.label16.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label15
            // 
            this.label15.Location = new System.Drawing.Point(27, 79);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(100, 20);
            this.label15.Text = "Sensor Interval";
            this.label15.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // radConnectButton
            // 
            this.radConnectButton.BackColor = System.Drawing.Color.Cyan;
            this.radConnectButton.Location = new System.Drawing.Point(27, 222);
            this.radConnectButton.Name = "radConnectButton";
            this.radConnectButton.Size = new System.Drawing.Size(121, 20);
            this.radConnectButton.TabIndex = 19;
            this.radConnectButton.Text = "RAD-CZT Connect";
            this.radConnectButton.Click += new System.EventHandler(this.radConnectButton_Click);
            // 
            // radUpDown
            // 
            this.radUpDown.Increment = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.radUpDown.Location = new System.Drawing.Point(133, 77);
            this.radUpDown.Maximum = new decimal(new int[] {
            3000,
            0,
            0,
            0});
            this.radUpDown.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.radUpDown.Name = "radUpDown";
            this.radUpDown.Size = new System.Drawing.Size(47, 22);
            this.radUpDown.TabIndex = 18;
            this.radUpDown.Value = new decimal(new int[] {
            30,
            0,
            0,
            0});
            // 
            // gpsSerialPort
            // 
            this.gpsSerialPort.BaudRate = 4800;
            this.gpsSerialPort.PortName = "COM4";
            this.gpsSerialPort.ReceivedBytesThreshold = 1024;
            this.gpsSerialPort.WriteBufferSize = 512;
            this.gpsSerialPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort_DataReceived);
            // 
            // radTimer
            // 
            this.radTimer.Interval = 30000;
            this.radTimer.Tick += new System.EventHandler(this.radTimer_Tick);
            // 
            // invTimer
            // 
            this.invTimer.Tick += new System.EventHandler(this.invTimer_Tick);
            // 
            // mainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(240, 294);
            this.Controls.Add(this.tabControl1);
            this.Font = new System.Drawing.Font("Tahoma", 10F, System.Drawing.FontStyle.Italic);
            this.KeyPreview = true;
            this.MinimizeBox = false;
            this.Name = "mainForm";
            this.Text = "Chaperone GUI";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.tabPage3.ResumeLayout(false);
            this.tabPage4.ResumeLayout(false);
            this.tabPage5.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button startInventoryButton;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.DomainUpDown domainUpDown2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button wjConnectButton;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.DomainUpDown domainUpDown4;
        private System.Windows.Forms.ComboBox cbComPort;
        private System.Windows.Forms.Label lblOnline;
        private System.Windows.Forms.RadioButton radioButton1;
        private System.Windows.Forms.RadioButton radioButton2;
        private System.Windows.Forms.NumericUpDown numericUpDown1;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox portBox;
        private System.Windows.Forms.TextBox hostnameBox;
        private System.Windows.Forms.TextBox passwordBox;
        private System.Windows.Forms.TextBox userIDBox;
        private System.Windows.Forms.Button connectBtn;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.Label ownerLbl;
        private System.Windows.Forms.Label dbTableLbl;
        private System.Windows.Forms.Label dbNameLbl;
        private System.Windows.Forms.TextBox dbTableBox;
        private System.Windows.Forms.TextBox dbNameBox;
        private System.Windows.Forms.Button clearBtn;
        private System.Windows.Forms.ComboBox ownerBox1;
        private System.Windows.Forms.Button gpsButton;
        private System.Windows.Forms.ComboBox btComPort;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.ProgressBar invProgressBar;
        private System.Windows.Forms.TextBox longBox;
        private System.Windows.Forms.TextBox latBox;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox utcBox;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox satBox;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.NumericUpDown numTags;
        private System.Windows.Forms.Label label14;
        private System.IO.Ports.SerialPort gpsSerialPort;
        private System.Windows.Forms.Button mapButton;
        private System.Windows.Forms.ComboBox invTimerBox;
        private System.Windows.Forms.TabPage tabPage5;
        private System.Windows.Forms.Timer radTimer;
        private System.Windows.Forms.Button radConnectButton;
        private System.Windows.Forms.NumericUpDown radUpDown;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.TextBox textBox1;
		private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Button scanButton;
        private System.Windows.Forms.ComboBox comboBox1;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.TextBox tagServerBox;
        private System.Windows.Forms.Button reconcileButton;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.TextBox ipPhoneBox;
        private System.Windows.Forms.Timer invTimer;
        private Microsoft.WindowsCE.Forms.InputPanel inputPanel1;
    }
}

