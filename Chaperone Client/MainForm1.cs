using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using Microsoft.Win32;
// Reference the MPR DLL namespace
using WJ.MPR.Reader;
using WJ.MPR.Util;

namespace MPR_Examp_DLL_Proj
{
	/// <summary>
	/// Form1 is the Main Form of the MPR Example DLL Demo Program.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		#region Form1 Properties (variables)
		private MPRReader Reader = new MPRReader();
		FormReaderInfo ReaderInfoForm;
		FormMsgLog MsgLogForm;

		#region GUI objects
		/// <summary>
		/// Create a reference to the object which we'll use to access the MPR.
		/// </summary>
		private System.Windows.Forms.ListBox lbTags;
		private System.Windows.Forms.RadioButton rbAntA;
		private System.Windows.Forms.RadioButton rbAntB;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.NumericUpDown nudTxPower;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.GroupBox groupBox3;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Label label29;
		private System.Windows.Forms.NumericUpDown nudInvPeriod;
		private System.Windows.Forms.ComboBox cbPersistTime;
		private System.Windows.Forms.CheckBox cbClass1;
		private System.Windows.Forms.CheckBox cbClass0;
		private System.Windows.Forms.StatusBar statusBar1;
		private System.Windows.Forms.Button btnInventory;
		private System.Windows.Forms.ComboBox cbComPort;
		private System.Windows.Forms.Button btnConnect;
		private System.Windows.Forms.Label lblOnline;
		private System.Windows.Forms.Button btnFindCOMs;
		private System.Windows.Forms.ToolTip toolTip1;
		private System.Windows.Forms.MenuItem menuItem1;
		private System.Windows.Forms.MenuItem menuItem3;
		private System.Windows.Forms.MainMenu mainMenu;
		private System.Windows.Forms.MenuItem menuItemExit;
		private System.Windows.Forms.MenuItem menuItemAbout;
		private System.Windows.Forms.MenuItem menuItem5;
		private System.Windows.Forms.MenuItem menuItemInformation;
		private System.Windows.Forms.MenuItem menuItemLog;
		private System.Windows.Forms.StatusBarPanel sbpInventory;
		private System.Windows.Forms.StatusBarPanel sbpCOM;
		private System.Windows.Forms.StatusBarPanel sbpSep;
		private System.ComponentModel.IContainer components;
		#endregion
		#endregion

		#region Application Initialization
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

		/// <summary>
		/// Constructor for the Main Form
		/// </summary>
		public Form1()
		{
			InitializeComponent();

			// Create a new ReaderInfoForm, 
			// to display info of our Reader object.
			ReaderInfoForm = new FormReaderInfo(Reader);

			// Create a new Message Log Form,
			// to display serial port transaction details.
			MsgLogForm = new FormMsgLog();

			// Look in the registry to determine which COM ports are present on this computer
			FillCommDetails();
		}

		/// <summary>
		/// Event Fired when Main Form is loaded.
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void Form1_Load(object sender, System.EventArgs e)
		{
			// Assign Persist Time to 2 seconds, as a default
			cbPersistTime.SelectedText = "2 seconds";

			Reader.DisconnectEvent += new EventHandler(Reader_DisconnectEvent);
			Reader.InvPollEvent	+= new EventHandler(Reader_InvPollEvent);
			Reader.InvTimerEnabledChanged += new EventHandler(Reader_InvTimerEnabledChanged);
			Reader.ManufacturingInformationChanged += new EventHandler(Reader_ManufacturingInformationChanged);
			Reader.StatusEvent += new WJ.MPR.Util.StringEventHandler(Reader_StatusEvent);
			Reader.TagAdded += new TagEventHandler(Reader_TagAdded);
			Reader.TagRemoved += new TagEventHandler(Reader_TagRemoved);
		}

		#endregion

		#region Application Termination
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			// destroy the MPR and free up its resources.
			Reader.Dispose();

			// destroy the Message Log Form.
			MsgLogForm.Dispose();

			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}
		#endregion

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.components = new System.ComponentModel.Container();
            this.lbTags = new System.Windows.Forms.ListBox();
            this.rbAntA = new System.Windows.Forms.RadioButton();
            this.rbAntB = new System.Windows.Forms.RadioButton();
            this.label2 = new System.Windows.Forms.Label();
            this.nudTxPower = new System.Windows.Forms.NumericUpDown();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.btnFindCOMs = new System.Windows.Forms.Button();
            this.cbComPort = new System.Windows.Forms.ComboBox();
            this.lblOnline = new System.Windows.Forms.Label();
            this.btnConnect = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.cbClass1 = new System.Windows.Forms.CheckBox();
            this.cbClass0 = new System.Windows.Forms.CheckBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label29 = new System.Windows.Forms.Label();
            this.nudInvPeriod = new System.Windows.Forms.NumericUpDown();
            this.cbPersistTime = new System.Windows.Forms.ComboBox();
            this.btnInventory = new System.Windows.Forms.Button();
            this.statusBar1 = new System.Windows.Forms.StatusBar();
            this.sbpCOM = new System.Windows.Forms.StatusBarPanel();
            this.sbpSep = new System.Windows.Forms.StatusBarPanel();
            this.sbpInventory = new System.Windows.Forms.StatusBarPanel();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.mainMenu = new System.Windows.Forms.MainMenu(this.components);
            this.menuItem1 = new System.Windows.Forms.MenuItem();
            this.menuItemExit = new System.Windows.Forms.MenuItem();
            this.menuItem5 = new System.Windows.Forms.MenuItem();
            this.menuItemInformation = new System.Windows.Forms.MenuItem();
            this.menuItemLog = new System.Windows.Forms.MenuItem();
            this.menuItem3 = new System.Windows.Forms.MenuItem();
            this.menuItemAbout = new System.Windows.Forms.MenuItem();
            ((System.ComponentModel.ISupportInitialize)(this.nudTxPower)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudInvPeriod)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.sbpCOM)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.sbpSep)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.sbpInventory)).BeginInit();
            this.SuspendLayout();
            // 
            // lbTags
            // 
            this.lbTags.Location = new System.Drawing.Point(8, 144);
            this.lbTags.Name = "lbTags";
            this.lbTags.Size = new System.Drawing.Size(504, 394);
            this.lbTags.TabIndex = 4;
            // 
            // rbAntA
            // 
            this.rbAntA.Checked = true;
            this.rbAntA.Location = new System.Drawing.Point(16, 24);
            this.rbAntA.Name = "rbAntA";
            this.rbAntA.Size = new System.Drawing.Size(80, 16);
            this.rbAntA.TabIndex = 5;
            this.rbAntA.TabStop = true;
            this.rbAntA.Text = "Antenna A";
            this.rbAntA.CheckedChanged += new System.EventHandler(this.rbAntA_CheckedChanged);
            // 
            // rbAntB
            // 
            this.rbAntB.Location = new System.Drawing.Point(16, 40);
            this.rbAntB.Name = "rbAntB";
            this.rbAntB.Size = new System.Drawing.Size(80, 16);
            this.rbAntB.TabIndex = 6;
            this.rbAntB.Text = "Antenna B";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(96, 16);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(88, 16);
            this.label2.TabIndex = 7;
            this.label2.Text = "Tx Power (dBm)";
            // 
            // nudTxPower
            // 
            this.nudTxPower.Location = new System.Drawing.Point(112, 32);
            this.nudTxPower.Name = "nudTxPower";
            this.nudTxPower.Size = new System.Drawing.Size(48, 20);
            this.nudTxPower.TabIndex = 8;
            this.nudTxPower.ValueChanged += new System.EventHandler(this.nudTxPower_ValueChanged);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.btnFindCOMs);
            this.groupBox1.Controls.Add(this.cbComPort);
            this.groupBox1.Controls.Add(this.lblOnline);
            this.groupBox1.Controls.Add(this.btnConnect);
            this.groupBox1.Location = new System.Drawing.Point(8, 0);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(304, 64);
            this.groupBox1.TabIndex = 9;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "COM Port";
            // 
            // btnFindCOMs
            // 
            this.btnFindCOMs.Location = new System.Drawing.Point(168, 24);
            this.btnFindCOMs.Name = "btnFindCOMs";
            this.btnFindCOMs.Size = new System.Drawing.Size(75, 32);
            this.btnFindCOMs.TabIndex = 6;
            this.btnFindCOMs.Text = "Find COMs";
            this.toolTip1.SetToolTip(this.btnFindCOMs, "Scan Windows for newly attached COM ports that may be attached to MPRs.");
            this.btnFindCOMs.Click += new System.EventHandler(this.btnFindCOMs_Click);
            // 
            // cbComPort
            // 
            this.cbComPort.Enabled = false;
            this.cbComPort.Location = new System.Drawing.Point(8, 30);
            this.cbComPort.Name = "cbComPort";
            this.cbComPort.Size = new System.Drawing.Size(72, 21);
            this.cbComPort.TabIndex = 5;
            this.cbComPort.Text = "None";
            this.cbComPort.SelectedIndexChanged += new System.EventHandler(this.cbComPort_SelectedIndexChanged);
            // 
            // lblOnline
            // 
            this.lblOnline.ForeColor = System.Drawing.Color.Red;
            this.lblOnline.Location = new System.Drawing.Point(256, 32);
            this.lblOnline.Name = "lblOnline";
            this.lblOnline.Size = new System.Drawing.Size(40, 16);
            this.lblOnline.TabIndex = 4;
            this.lblOnline.Text = "Offline";
            // 
            // btnConnect
            // 
            this.btnConnect.Location = new System.Drawing.Point(88, 24);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(72, 32);
            this.btnConnect.TabIndex = 3;
            this.btnConnect.Text = "Connect";
            this.toolTip1.SetToolTip(this.btnConnect, "Connect to an MPR at selected COM port.");
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.rbAntA);
            this.groupBox2.Controls.Add(this.rbAntB);
            this.groupBox2.Controls.Add(this.nudTxPower);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Location = new System.Drawing.Point(320, 0);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(192, 64);
            this.groupBox2.TabIndex = 10;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Acive Antenna";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.cbClass1);
            this.groupBox3.Controls.Add(this.cbClass0);
            this.groupBox3.Controls.Add(this.label8);
            this.groupBox3.Controls.Add(this.label29);
            this.groupBox3.Controls.Add(this.nudInvPeriod);
            this.groupBox3.Controls.Add(this.cbPersistTime);
            this.groupBox3.Controls.Add(this.btnInventory);
            this.groupBox3.Location = new System.Drawing.Point(8, 64);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(504, 72);
            this.groupBox3.TabIndex = 67;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Inventory Control";
            // 
            // cbClass1
            // 
            this.cbClass1.Checked = true;
            this.cbClass1.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbClass1.Location = new System.Drawing.Point(120, 40);
            this.cbClass1.Name = "cbClass1";
            this.cbClass1.Size = new System.Drawing.Size(88, 16);
            this.cbClass1.TabIndex = 75;
            this.cbClass1.Text = "ePC Class 1";
            this.cbClass1.CheckedChanged += new System.EventHandler(this.cbProtocol_CheckedChanged);
            // 
            // cbClass0
            // 
            this.cbClass0.Checked = true;
            this.cbClass0.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbClass0.Location = new System.Drawing.Point(120, 24);
            this.cbClass0.Name = "cbClass0";
            this.cbClass0.Size = new System.Drawing.Size(88, 16);
            this.cbClass0.TabIndex = 74;
            this.cbClass0.Text = "ePC Class 0";
            this.cbClass0.CheckedChanged += new System.EventHandler(this.cbProtocol_CheckedChanged);
            // 
            // label8
            // 
            this.label8.Location = new System.Drawing.Point(232, 16);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(96, 16);
            this.label8.TabIndex = 71;
            this.label8.Text = "Tag Persist Time";
            // 
            // label29
            // 
            this.label29.Location = new System.Drawing.Point(360, 16);
            this.label29.Name = "label29";
            this.label29.Size = new System.Drawing.Size(88, 16);
            this.label29.TabIndex = 70;
            this.label29.Text = "Inv Period (ms)";
            // 
            // nudInvPeriod
            // 
            this.nudInvPeriod.Increment = new decimal(new int[] {
            50,
            0,
            0,
            0});
            this.nudInvPeriod.Location = new System.Drawing.Point(360, 32);
            this.nudInvPeriod.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.nudInvPeriod.Name = "nudInvPeriod";
            this.nudInvPeriod.Size = new System.Drawing.Size(84, 20);
            this.nudInvPeriod.TabIndex = 69;
            this.nudInvPeriod.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.nudInvPeriod.Value = new decimal(new int[] {
            300,
            0,
            0,
            0});
            this.nudInvPeriod.ValueChanged += new System.EventHandler(this.nudInvPeriod_ValueChanged);
            // 
            // cbPersistTime
            // 
            this.cbPersistTime.ItemHeight = 13;
            this.cbPersistTime.Items.AddRange(new object[] {
            "None",
            "0.5 second",
            "1 second",
            "2 seconds",
            "5 seconds",
            "10 seconds",
            "1 minute",
            "Infinite"});
            this.cbPersistTime.Location = new System.Drawing.Point(232, 32);
            this.cbPersistTime.Name = "cbPersistTime";
            this.cbPersistTime.Size = new System.Drawing.Size(98, 21);
            this.cbPersistTime.TabIndex = 68;
            this.cbPersistTime.SelectedIndexChanged += new System.EventHandler(this.cbPersistTime_SelectedIndexChanged);
            // 
            // btnInventory
            // 
            this.btnInventory.BackColor = System.Drawing.Color.LightGreen;
            this.btnInventory.Location = new System.Drawing.Point(16, 24);
            this.btnInventory.Name = "btnInventory";
            this.btnInventory.Size = new System.Drawing.Size(88, 32);
            this.btnInventory.TabIndex = 67;
            this.btnInventory.Text = "Start Inventory";
            this.btnInventory.UseVisualStyleBackColor = false;
            this.btnInventory.Click += new System.EventHandler(this.btnInventory_Click);
            // 
            // statusBar1
            // 
            this.statusBar1.Location = new System.Drawing.Point(0, 539);
            this.statusBar1.Name = "statusBar1";
            this.statusBar1.Panels.AddRange(new System.Windows.Forms.StatusBarPanel[] {
            this.sbpCOM,
            this.sbpSep,
            this.sbpInventory});
            this.statusBar1.ShowPanels = true;
            this.statusBar1.Size = new System.Drawing.Size(518, 16);
            this.statusBar1.SizingGrip = false;
            this.statusBar1.TabIndex = 68;
            this.statusBar1.PanelClick += new System.Windows.Forms.StatusBarPanelClickEventHandler(this.statusBar1_PanelClick);
            // 
            // sbpCOM
            // 
            this.sbpCOM.AutoSize = System.Windows.Forms.StatusBarPanelAutoSize.Contents;
            this.sbpCOM.Name = "sbpCOM";
            this.sbpCOM.Text = "Offline";
            this.sbpCOM.Width = 47;
            // 
            // sbpSep
            // 
            this.sbpSep.AutoSize = System.Windows.Forms.StatusBarPanelAutoSize.Spring;
            this.sbpSep.BorderStyle = System.Windows.Forms.StatusBarPanelBorderStyle.None;
            this.sbpSep.Name = "sbpSep";
            this.sbpSep.Width = 439;
            // 
            // sbpInventory
            // 
            this.sbpInventory.Alignment = System.Windows.Forms.HorizontalAlignment.Right;
            this.sbpInventory.AutoSize = System.Windows.Forms.StatusBarPanelAutoSize.Contents;
            this.sbpInventory.Name = "sbpInventory";
            this.sbpInventory.Text = "Idle";
            this.sbpInventory.Width = 32;
            // 
            // mainMenu
            // 
            this.mainMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.menuItem1,
            this.menuItem5,
            this.menuItem3});
            // 
            // menuItem1
            // 
            this.menuItem1.Index = 0;
            this.menuItem1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.menuItemExit});
            this.menuItem1.Text = "File";
            // 
            // menuItemExit
            // 
            this.menuItemExit.Index = 0;
            this.menuItemExit.Text = "E&xit";
            this.menuItemExit.Click += new System.EventHandler(this.menuItemExit_Click);
            // 
            // menuItem5
            // 
            this.menuItem5.Index = 1;
            this.menuItem5.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.menuItemInformation,
            this.menuItemLog});
            this.menuItem5.Text = "&Reader";
            // 
            // menuItemInformation
            // 
            this.menuItemInformation.Index = 0;
            this.menuItemInformation.Shortcut = System.Windows.Forms.Shortcut.CtrlI;
            this.menuItemInformation.Text = "Reader &Information";
            this.menuItemInformation.Click += new System.EventHandler(this.menuItemInformation_Click);
            // 
            // menuItemLog
            // 
            this.menuItemLog.Index = 1;
            this.menuItemLog.Shortcut = System.Windows.Forms.Shortcut.CtrlL;
            this.menuItemLog.Text = "Show &Message Log";
            this.menuItemLog.Click += new System.EventHandler(this.menuItemLog_Click);
            // 
            // menuItem3
            // 
            this.menuItem3.Index = 2;
            this.menuItem3.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.menuItemAbout});
            this.menuItem3.Text = "&Help";
            // 
            // menuItemAbout
            // 
            this.menuItemAbout.Index = 0;
            this.menuItemAbout.Text = "&About";
            this.menuItemAbout.Click += new System.EventHandler(this.menuItemAbout_Click);
            // 
            // Form1
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(518, 555);
            this.Controls.Add(this.statusBar1);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.lbTags);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.MaximizeBox = false;
            this.Menu = this.mainMenu;
            this.Name = "Form1";
            this.Text = "MPR DLL Demo";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.nudTxPower)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.nudInvPeriod)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.sbpCOM)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.sbpSep)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.sbpInventory)).EndInit();
            this.ResumeLayout(false);

		}
		#endregion

		#region COM Port enumeration
		/// <summary>
		/// Fill the COM dropdown combobox with a list of valid comports
		/// </summary>
		private void FillCommDetails()
		{
			// Walk the Registry in search of COM Ports
			string[] COMPorts = FindCOMPortNames();	

			// Fill in the items of the COM Port combo box
			cbComPort.Items.Clear();
			foreach (string sCOMPort in COMPorts)
			{
				cbComPort.Items.Add(sCOMPort);
			}

			// If any COM ports were found, make the first in the list the default
			if (cbComPort.Items.Count > 0) 
			{
				cbComPort.SelectedIndex = 0;
				cbComPort.Enabled = true;
			}
		}


		private string[] FindCOMPortNames()
		{
			RegistryKey rkCOMPorts = Registry.LocalMachine.OpenSubKey(@"HARDWARE\DEVICEMAP\SERIALCOMM");

			if (rkCOMPorts == null)
				return new string[] { };

			Console.WriteLine();
			Console.WriteLine("COM Ports found:");

			string[] sCOMPortValueNames = rkCOMPorts.GetValueNames();
			string[] sCOMPortNames = new string[sCOMPortValueNames.Length];
			int cnt = 0;

			foreach (string sCPName in sCOMPortValueNames)
			{
				string sCPValue = rkCOMPorts.GetValue(sCPName).ToString();
				Console.WriteLine("{0} => {1}", sCPName, sCPValue);
				sCOMPortNames[cnt++] = sCPValue;
			}

			return sCOMPortNames;
		}
		#endregion

		#region Reader Event Handlers
		/// <summary>
		/// This event is fired when a the DLL loses contact with an MPR.
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void Reader_DisconnectEvent(object sender, EventArgs e)
		{
			lblOnline.ForeColor = Color.Red;
			lblOnline.Text = "Offline";
			sbpInventory.Text = "Offline";
		}

		/// <summary>
		/// This event is fired whenever the DLL requests an inventory from the MPR.
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void Reader_InvPollEvent(object sender, EventArgs e)
		{

		}

		/// <summary>
		/// This event is fired when the DLL starts or stops an inventory loop.
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void Reader_InvTimerEnabledChanged(object sender, EventArgs e)
		{
			if (InvokeRequired)
				Invoke(new EventHandler(Reader_InvTimerEnabledChanged), new object[] {sender, e});
			else 
			{
				if (Reader.InvTimerEnabled) 
				{
					btnInventory.Text = "Stop Inventory";
					btnInventory.BackColor = Color.Red;
					btnInventory.ForeColor = Color.White;
					sbpInventory.Text = "Inventory in Progress";
					Logit(string.Format("\n+++Inventory Started at {0}.", DateTime.Now));
				}
				else
				{
					btnInventory.Text = "Start Inventory";
					btnInventory.BackColor = Color.LightGreen;
					btnInventory.ForeColor = Color.Black;
					sbpInventory.Text = "Idle";
					Logit(string.Format("---Inventory Ended at {0}.\n", DateTime.Now));
				}
			}
		}

		/// <summary>
		/// Event fired when the DLL's copy of the Reader's
		/// Manufacturing Information has changed.  In other words,
		/// the DLL reads the information and it was different from what it was before.
		/// This should only happen when the first time the info is read, 
		/// or when a different MPR is connected.
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void Reader_ManufacturingInformationChanged(object sender, EventArgs e)
		{
			nudTxPower.Minimum = Reader.TxPowerMin;
			nudTxPower.Maximum = Reader.TxPowerMax;
			nudTxPower.Value = Reader.TxPower;
		}


		/// <summary>
		/// Event fired when
		/// </summary>
		/// <param name="Message"></param>
		private void Reader_StatusEvent(string Message)
		{
			if (InvokeRequired)
				Invoke(new StringEventHandler(Reader_StatusEvent), new object[] {Message});
			else 
			{
				Logit(Message);
			}
		}

		/// <summary>
		/// Add a string to the Message Log
		/// </summary>
		/// <param name="s"></param>
		public void Logit(string s)
		{
			MsgLogForm.AddLine(s);
		}


		/// <summary>
		/// Event that fires when a new Tag is added to the Inventory.
		/// A Tag stays in the inventory as long as it has been read 
		/// within the Persist Time. 
		/// </summary>
		/// <param name="Tag"></param>
		private void Reader_TagAdded(RFIDTag Tag)
		{

			if (InvokeRequired)
				Invoke(new TagEventHandler(Reader_TagAdded), new object[] {Tag});
			else
			{
				lbTags.Items.Add(Tag.TagID);
			}
		}

		/// <summary>
		/// Event fired when a tag is removed from the Inventory.
		/// A tag is removed when it has not been read for at least
		/// PersistTime.
		/// </summary>
		/// <param name="Tag"></param>
		private void Reader_TagRemoved(RFIDTag Tag)
		{
			if (InvokeRequired)
				Invoke(new TagEventHandler(Reader_TagRemoved), new object[] {Tag});
			else 
			{
				ArrayList TagsToRemove = new ArrayList();
				foreach(object ItemInList in lbTags.Items)
					if (ItemInList.ToString() == Tag.TagID)
						TagsToRemove.Add(ItemInList);
				while (TagsToRemove.Count != 0) 
				{
					lbTags.Items.Remove(TagsToRemove[0]);
					TagsToRemove.RemoveAt(0);
				}
			}
		}
		#endregion

		#region GUI Control Event Handlers

		private void rbAntA_CheckedChanged(object sender, System.EventArgs e)
		{
			rbAntB.Checked = !rbAntA.Checked;
			Reader.ActiveAntenna = (byte)((rbAntA.Checked)?0:1);
		}

		private void nudTxPower_ValueChanged(object sender, System.EventArgs e)
		{
			Reader.TxPower = (byte)nudTxPower.Value;
		}

		private void cbProtocol_CheckedChanged(object sender, System.EventArgs e)
		{
			if (Reader.InvTimerEnabled == true) 
			{
				Reader.ClearInventory();
				lbTags.Items.Clear();
			}
			Reader.Class1InventoryEnabled = cbClass1.Checked;
			Reader.Class0InventoryEnabled = cbClass0.Checked;
		}

		/// <summary>
		/// Convert a time span as a string to a TimeSpan structure.
		/// </summary>
		/// <param name="value">A string describing the time span.</param>
		/// <returns>A TimeSpan structure corresponding to the requested time span.</returns>
		public TimeSpan TimeSpanDropDownParse(string value) 
		{
			switch (value)
			{
				case "One Shot":
				case "None":
					return new TimeSpan(0,0,0,0,90);
				case "1 second":
					return new TimeSpan(0,0,0,1);
				case "2 seconds":
					return new TimeSpan(0,0,0,2);
				case "5 seconds":
					return new TimeSpan(0,0,0,5);
				case "10 seconds":
					return new TimeSpan(0,0,0,10);
				case "30 seconds":
					return new TimeSpan(0,0,0,30);
				case "1 minute":
					return new TimeSpan(0,0,1,0);
				case "5 minutes":
					return new TimeSpan(0,0,5,0);
				case "10 minutes":
					return new TimeSpan(0,0,10,0);
				case "30 minutes":
					return new TimeSpan(0,0,30,0);
				case "1 hour":
					return new TimeSpan(0,1,0,0);
				case "Infinite":
					return TimeSpan.MaxValue;
				default:
					return new TimeSpan(0,0,0,1);
			}
		}

		private void cbPersistTime_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			Reader.PersistTime = TimeSpanDropDownParse(cbPersistTime.Text);
		}

		private void btnConnect_Click(object sender, System.EventArgs e)
		{
			try
			{
				if (Reader.IsConnected)
					Reader.Disconnect();

				Reader.Connect(cbComPort.Text, "57600");

				if (Reader.IsConnected) 
				{
					lblOnline.ForeColor = Color.Green;
					lblOnline.Text = "Online";
					
					this.sbpCOM.Text = "Online";
				} 
				else
				{
					MessageBox.Show(string.Format("Could not connect to MPR at {0}.", cbComPort.Text));
				}
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.Message);
			}
		}

		private void btnInventory_Click(object sender, System.EventArgs e)
		{
			Reader.InvTimerEnabled = !Reader.InvTimerEnabled;		
		}

		private void btnFindCOMs_Click(object sender, System.EventArgs e)
		{
			FillCommDetails();		
		}

		private void menuItemExit_Click(object sender, System.EventArgs e)
		{
			Reader.Disconnect();
			Application.Exit();
		}

		private void menuItemInformation_Click(object sender, System.EventArgs e)
		{
			ReaderInfoForm.Show();
		}

		private void menuItemAbout_Click(object sender, System.EventArgs e)
		{
			MessageBox.Show(Application.CompanyName + "\n" +
				"RFID Multi-protocol Reader (MPR Series)\n" +
				"Microsoft Windows PC DLL Example Project Demo\n" + 
				"Copyright 2005\n" +
				Application.ProductName + " " + Application.ProductVersion, 
				"WJCI Multprotocol Reader (MPR)");
		}

		private void menuItemLog_Click(object sender, System.EventArgs e)
		{
			MsgLogForm.Show();			
		}
		#endregion

        private void statusBar1_PanelClick(object sender, StatusBarPanelClickEventArgs e)
        {

        }

        private void cbComPort_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void nudInvPeriod_ValueChanged(object sender, EventArgs e)
        {

        }
	}
}