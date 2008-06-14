namespace GentagDemo
{
    partial class demoClient
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
            if (!disposing)
            {
                tagReader.Running = false;
                tagSearchReading = false;
                
                tagSearchAutoEvent.Close();

                if (tagSearchTimer != null)
                {
                    tagSearchAutoEvent.WaitOne(500, false);
                    tagSearchTimer.Dispose();
                }
                debugOut.Dispose();
            }
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
            System.Windows.Forms.Label label29;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(demoClient));
            System.Windows.Forms.Label label28;
            System.Windows.Forms.Label label30;
            System.Windows.Forms.Label label27;
            System.Windows.Forms.ListViewItem listViewItem1 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem2 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem3 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem4 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem5 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem6 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem7 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem8 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem9 = new System.Windows.Forms.ListViewItem();
            System.Windows.Forms.ListViewItem listViewItem10 = new System.Windows.Forms.ListViewItem();
            this.readIDButton = new System.Windows.Forms.Button();
            this.mainTabControl = new System.Windows.Forms.TabControl();
            this.introPage = new System.Windows.Forms.TabPage();
            this.versionLabel = new System.Windows.Forms.Label();
            this.label26 = new System.Windows.Forms.Label();
            this.pictureBox2 = new System.Windows.Forms.PictureBox();
            this.mainClearButton = new System.Windows.Forms.Button();
            this.exitButton = new System.Windows.Forms.Button();
            this.authPage = new System.Windows.Forms.TabPage();
            this.pendingLookupsLabel = new System.Windows.Forms.Label();
            this.queuedLookupsLabel = new System.Windows.Forms.Label();
            this.label24 = new System.Windows.Forms.Label();
            this.label22 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.manualLookupButton = new System.Windows.Forms.Button();
            this.authTreeView = new System.Windows.Forms.TreeView();
            this.detectPage = new System.Windows.Forms.TabPage();
            this.detectTagIDLabel = new System.Windows.Forms.Label();
            this.detectTagTypeLabel = new System.Windows.Forms.Label();
            this.detectTagTypeButton = new System.Windows.Forms.Button();
            this.patientPage = new System.Windows.Forms.TabPage();
            this.medicationButton = new System.Windows.Forms.Button();
            this.readPatientButton = new System.Windows.Forms.Button();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.drugPhoto = new System.Windows.Forms.PictureBox();
            this.patientPhoto = new System.Windows.Forms.PictureBox();
            this.patientDescriptionBox = new System.Windows.Forms.TextBox();
            this.drugNameBox = new System.Windows.Forms.TextBox();
            this.patientNameBox = new System.Windows.Forms.TextBox();
            this.assayPage = new System.Windows.Forms.TabPage();
            this.assayImagePictureBox = new System.Windows.Forms.PictureBox();
            this.assayTimerLabel = new System.Windows.Forms.Label();
            this.assayMessageLabel = new System.Windows.Forms.Label();
            this.assayResetButton = new System.Windows.Forms.Button();
            this.assayRecordButton = new System.Windows.Forms.Button();
            this.assayBeginButton = new System.Windows.Forms.Button();
            this.assayReadButton = new System.Windows.Forms.Button();
            this.gpsPage = new System.Windows.Forms.TabPage();
            this.trackingCheckBox = new System.Windows.Forms.CheckBox();
            this.satellitesUsedTextBox = new System.Windows.Forms.TextBox();
            this.timeTextBox = new System.Windows.Forms.TextBox();
            this.hdopTextBox = new System.Windows.Forms.TextBox();
            this.dateTextBox = new System.Windows.Forms.TextBox();
            this.statusTextBox = new System.Windows.Forms.TextBox();
            this.altitudeTextBox = new System.Windows.Forms.TextBox();
            this.speedTextBox = new System.Windows.Forms.TextBox();
            this.satLabel8 = new System.Windows.Forms.Label();
            this.satLabel7 = new System.Windows.Forms.Label();
            this.satLabel6 = new System.Windows.Forms.Label();
            this.satLabel5 = new System.Windows.Forms.Label();
            this.satLabel4 = new System.Windows.Forms.Label();
            this.satLabel3 = new System.Windows.Forms.Label();
            this.satLabel2 = new System.Windows.Forms.Label();
            this.label20 = new System.Windows.Forms.Label();
            this.label19 = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.directionTextBox = new System.Windows.Forms.TextBox();
            this.label17 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
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
            this.queueSizeBar = new System.Windows.Forms.ProgressBar();
            this.satLabel1 = new System.Windows.Forms.Label();
            this.readWritePage = new System.Windows.Forms.TabPage();
            this.readWriteTB = new System.Windows.Forms.TextBox();
            this.readWriteStatusLabel = new System.Windows.Forms.Label();
            this.label25 = new System.Windows.Forms.Label();
            this.writeButton = new System.Windows.Forms.Button();
            this.readButton = new System.Windows.Forms.Button();
            this.statusPage = new System.Windows.Forms.TabPage();
            this.statusListView = new System.Windows.Forms.ListView();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.softInputPanel = new Microsoft.WindowsCE.Forms.InputPanel(this.components);
            this.GUIUpdateTimer = new System.Windows.Forms.Timer();
            this.tempPage = new System.Windows.Forms.TabPage();
            this.button1 = new System.Windows.Forms.Button();
            label29 = new System.Windows.Forms.Label();
            label28 = new System.Windows.Forms.Label();
            label30 = new System.Windows.Forms.Label();
            label27 = new System.Windows.Forms.Label();
            this.mainTabControl.SuspendLayout();
            this.introPage.SuspendLayout();
            this.authPage.SuspendLayout();
            this.detectPage.SuspendLayout();
            this.patientPage.SuspendLayout();
            this.assayPage.SuspendLayout();
            this.gpsPage.SuspendLayout();
            this.readWritePage.SuspendLayout();
            this.statusPage.SuspendLayout();
            this.tempPage.SuspendLayout();
            this.SuspendLayout();
            // 
            // label29
            // 
            resources.ApplyResources(label29, "label29");
            label29.Name = "label29";
            // 
            // label28
            // 
            resources.ApplyResources(label28, "label28");
            label28.Name = "label28";
            // 
            // label30
            // 
            resources.ApplyResources(label30, "label30");
            label30.Name = "label30";
            // 
            // label27
            // 
            resources.ApplyResources(label27, "label27");
            label27.Name = "label27";
            // 
            // readIDButton
            // 
            resources.ApplyResources(this.readIDButton, "readIDButton");
            this.readIDButton.Name = "readIDButton";
            this.readIDButton.Click += new System.EventHandler(this.readerClick);
            // 
            // mainTabControl
            // 
            this.mainTabControl.Controls.Add(this.introPage);
            this.mainTabControl.Controls.Add(this.authPage);
            this.mainTabControl.Controls.Add(this.detectPage);
            this.mainTabControl.Controls.Add(this.patientPage);
            this.mainTabControl.Controls.Add(this.assayPage);
            this.mainTabControl.Controls.Add(this.gpsPage);
            this.mainTabControl.Controls.Add(this.readWritePage);
            this.mainTabControl.Controls.Add(this.statusPage);
            this.mainTabControl.Controls.Add(this.tempPage);
            resources.ApplyResources(this.mainTabControl, "mainTabControl");
            this.mainTabControl.Name = "mainTabControl";
            this.mainTabControl.SelectedIndex = 0;
            this.mainTabControl.SelectedIndexChanged += new System.EventHandler(this.tabControl1_SelectedIndexChanged);
            // 
            // introPage
            // 
            this.introPage.Controls.Add(label29);
            this.introPage.Controls.Add(label28);
            this.introPage.Controls.Add(this.versionLabel);
            this.introPage.Controls.Add(this.label26);
            this.introPage.Controls.Add(this.pictureBox2);
            this.introPage.Controls.Add(this.mainClearButton);
            this.introPage.Controls.Add(this.exitButton);
            resources.ApplyResources(this.introPage, "introPage");
            this.introPage.Name = "introPage";
            // 
            // versionLabel
            // 
            resources.ApplyResources(this.versionLabel, "versionLabel");
            this.versionLabel.Name = "versionLabel";
            // 
            // label26
            // 
            resources.ApplyResources(this.label26, "label26");
            this.label26.Name = "label26";
            // 
            // pictureBox2
            // 
            resources.ApplyResources(this.pictureBox2, "pictureBox2");
            this.pictureBox2.Name = "pictureBox2";
            // 
            // mainClearButton
            // 
            resources.ApplyResources(this.mainClearButton, "mainClearButton");
            this.mainClearButton.Name = "mainClearButton";
            this.mainClearButton.Click += new System.EventHandler(this.clearDemo);
            // 
            // exitButton
            // 
            resources.ApplyResources(this.exitButton, "exitButton");
            this.exitButton.Name = "exitButton";
            this.exitButton.Click += new System.EventHandler(this.exitButton_Click);
            // 
            // authPage
            // 
            this.authPage.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(224)))), ((int)(((byte)(255)))));
            this.authPage.Controls.Add(this.pendingLookupsLabel);
            this.authPage.Controls.Add(this.queuedLookupsLabel);
            this.authPage.Controls.Add(this.label24);
            this.authPage.Controls.Add(this.label22);
            this.authPage.Controls.Add(this.pictureBox1);
            this.authPage.Controls.Add(this.manualLookupButton);
            this.authPage.Controls.Add(this.readIDButton);
            this.authPage.Controls.Add(this.authTreeView);
            resources.ApplyResources(this.authPage, "authPage");
            this.authPage.Name = "authPage";
            // 
            // pendingLookupsLabel
            // 
            resources.ApplyResources(this.pendingLookupsLabel, "pendingLookupsLabel");
            this.pendingLookupsLabel.Name = "pendingLookupsLabel";
            // 
            // queuedLookupsLabel
            // 
            resources.ApplyResources(this.queuedLookupsLabel, "queuedLookupsLabel");
            this.queuedLookupsLabel.Name = "queuedLookupsLabel";
            // 
            // label24
            // 
            resources.ApplyResources(this.label24, "label24");
            this.label24.Name = "label24";
            // 
            // label22
            // 
            resources.ApplyResources(this.label22, "label22");
            this.label22.Name = "label22";
            // 
            // pictureBox1
            // 
            resources.ApplyResources(this.pictureBox1, "pictureBox1");
            this.pictureBox1.Name = "pictureBox1";
            // 
            // manualLookupButton
            // 
            resources.ApplyResources(this.manualLookupButton, "manualLookupButton");
            this.manualLookupButton.Name = "manualLookupButton";
            this.manualLookupButton.Click += new System.EventHandler(this.manualLookupClick);
            // 
            // authTreeView
            // 
            resources.ApplyResources(this.authTreeView, "authTreeView");
            this.authTreeView.Name = "authTreeView";
            this.authTreeView.PathSeparator = "\\";
            // 
            // detectPage
            // 
            this.detectPage.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(224)))), ((int)(((byte)(255)))));
            this.detectPage.Controls.Add(this.detectTagIDLabel);
            this.detectPage.Controls.Add(this.detectTagTypeLabel);
            this.detectPage.Controls.Add(this.detectTagTypeButton);
            resources.ApplyResources(this.detectPage, "detectPage");
            this.detectPage.Name = "detectPage";
            // 
            // detectTagIDLabel
            // 
            resources.ApplyResources(this.detectTagIDLabel, "detectTagIDLabel");
            this.detectTagIDLabel.Name = "detectTagIDLabel";
            // 
            // detectTagTypeLabel
            // 
            resources.ApplyResources(this.detectTagTypeLabel, "detectTagTypeLabel");
            this.detectTagTypeLabel.Name = "detectTagTypeLabel";
            // 
            // detectTagTypeButton
            // 
            resources.ApplyResources(this.detectTagTypeButton, "detectTagTypeButton");
            this.detectTagTypeButton.Name = "detectTagTypeButton";
            this.detectTagTypeButton.Click += new System.EventHandler(this.readerClick);
            // 
            // patientPage
            // 
            this.patientPage.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(224)))), ((int)(((byte)(255)))));
            this.patientPage.Controls.Add(this.medicationButton);
            this.patientPage.Controls.Add(this.readPatientButton);
            this.patientPage.Controls.Add(this.label11);
            this.patientPage.Controls.Add(label30);
            this.patientPage.Controls.Add(this.label10);
            this.patientPage.Controls.Add(this.drugPhoto);
            this.patientPage.Controls.Add(this.patientPhoto);
            this.patientPage.Controls.Add(this.patientDescriptionBox);
            this.patientPage.Controls.Add(this.drugNameBox);
            this.patientPage.Controls.Add(this.patientNameBox);
            resources.ApplyResources(this.patientPage, "patientPage");
            this.patientPage.Name = "patientPage";
            // 
            // medicationButton
            // 
            resources.ApplyResources(this.medicationButton, "medicationButton");
            this.medicationButton.Name = "medicationButton";
            this.medicationButton.Click += new System.EventHandler(this.readerClick);
            // 
            // readPatientButton
            // 
            resources.ApplyResources(this.readPatientButton, "readPatientButton");
            this.readPatientButton.Name = "readPatientButton";
            this.readPatientButton.Click += new System.EventHandler(this.readerClick);
            // 
            // label11
            // 
            resources.ApplyResources(this.label11, "label11");
            this.label11.Name = "label11";
            // 
            // label10
            // 
            resources.ApplyResources(this.label10, "label10");
            this.label10.Name = "label10";
            // 
            // drugPhoto
            // 
            resources.ApplyResources(this.drugPhoto, "drugPhoto");
            this.drugPhoto.Name = "drugPhoto";
            // 
            // patientPhoto
            // 
            resources.ApplyResources(this.patientPhoto, "patientPhoto");
            this.patientPhoto.Name = "patientPhoto";
            // 
            // patientDescriptionBox
            // 
            resources.ApplyResources(this.patientDescriptionBox, "patientDescriptionBox");
            this.patientDescriptionBox.Name = "patientDescriptionBox";
            this.patientDescriptionBox.ReadOnly = true;
            // 
            // drugNameBox
            // 
            resources.ApplyResources(this.drugNameBox, "drugNameBox");
            this.drugNameBox.Name = "drugNameBox";
            this.drugNameBox.ReadOnly = true;
            // 
            // patientNameBox
            // 
            resources.ApplyResources(this.patientNameBox, "patientNameBox");
            this.patientNameBox.Name = "patientNameBox";
            this.patientNameBox.ReadOnly = true;
            // 
            // assayPage
            // 
            this.assayPage.Controls.Add(this.assayImagePictureBox);
            this.assayPage.Controls.Add(this.assayTimerLabel);
            this.assayPage.Controls.Add(this.assayMessageLabel);
            this.assayPage.Controls.Add(label27);
            this.assayPage.Controls.Add(this.assayResetButton);
            this.assayPage.Controls.Add(this.assayRecordButton);
            this.assayPage.Controls.Add(this.assayBeginButton);
            this.assayPage.Controls.Add(this.assayReadButton);
            resources.ApplyResources(this.assayPage, "assayPage");
            this.assayPage.Name = "assayPage";
            // 
            // assayImagePictureBox
            // 
            resources.ApplyResources(this.assayImagePictureBox, "assayImagePictureBox");
            this.assayImagePictureBox.Name = "assayImagePictureBox";
            // 
            // assayTimerLabel
            // 
            resources.ApplyResources(this.assayTimerLabel, "assayTimerLabel");
            this.assayTimerLabel.Name = "assayTimerLabel";
            // 
            // assayMessageLabel
            // 
            resources.ApplyResources(this.assayMessageLabel, "assayMessageLabel");
            this.assayMessageLabel.Name = "assayMessageLabel";
            // 
            // assayResetButton
            // 
            resources.ApplyResources(this.assayResetButton, "assayResetButton");
            this.assayResetButton.Name = "assayResetButton";
            this.assayResetButton.Click += new System.EventHandler(this.assayClearButton_Click);
            // 
            // assayRecordButton
            // 
            resources.ApplyResources(this.assayRecordButton, "assayRecordButton");
            this.assayRecordButton.Name = "assayRecordButton";
            this.assayRecordButton.Click += new System.EventHandler(this.assayRecordButton_Click);
            // 
            // assayBeginButton
            // 
            resources.ApplyResources(this.assayBeginButton, "assayBeginButton");
            this.assayBeginButton.Name = "assayBeginButton";
            this.assayBeginButton.Click += new System.EventHandler(this.assayBeginButton_Click);
            // 
            // assayReadButton
            // 
            resources.ApplyResources(this.assayReadButton, "assayReadButton");
            this.assayReadButton.Name = "assayReadButton";
            this.assayReadButton.Click += new System.EventHandler(this.readerClick);
            // 
            // gpsPage
            // 
            this.gpsPage.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(224)))), ((int)(((byte)(255)))));
            this.gpsPage.Controls.Add(this.trackingCheckBox);
            this.gpsPage.Controls.Add(this.satellitesUsedTextBox);
            this.gpsPage.Controls.Add(this.timeTextBox);
            this.gpsPage.Controls.Add(this.hdopTextBox);
            this.gpsPage.Controls.Add(this.dateTextBox);
            this.gpsPage.Controls.Add(this.statusTextBox);
            this.gpsPage.Controls.Add(this.altitudeTextBox);
            this.gpsPage.Controls.Add(this.speedTextBox);
            this.gpsPage.Controls.Add(this.satLabel8);
            this.gpsPage.Controls.Add(this.satLabel7);
            this.gpsPage.Controls.Add(this.satLabel6);
            this.gpsPage.Controls.Add(this.satLabel5);
            this.gpsPage.Controls.Add(this.satLabel4);
            this.gpsPage.Controls.Add(this.satLabel3);
            this.gpsPage.Controls.Add(this.satLabel2);
            this.gpsPage.Controls.Add(this.label20);
            this.gpsPage.Controls.Add(this.label19);
            this.gpsPage.Controls.Add(this.label18);
            this.gpsPage.Controls.Add(this.directionTextBox);
            this.gpsPage.Controls.Add(this.label17);
            this.gpsPage.Controls.Add(this.label1);
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
            this.gpsPage.Controls.Add(this.queueSizeBar);
            this.gpsPage.Controls.Add(this.satLabel1);
            resources.ApplyResources(this.gpsPage, "gpsPage");
            this.gpsPage.Name = "gpsPage";
            // 
            // trackingCheckBox
            // 
            resources.ApplyResources(this.trackingCheckBox, "trackingCheckBox");
            this.trackingCheckBox.Name = "trackingCheckBox";
            this.trackingCheckBox.CheckStateChanged += new System.EventHandler(this.trackingCheckBox_CheckStateChanged);
            // 
            // satellitesUsedTextBox
            // 
            resources.ApplyResources(this.satellitesUsedTextBox, "satellitesUsedTextBox");
            this.satellitesUsedTextBox.Name = "satellitesUsedTextBox";
            this.satellitesUsedTextBox.ReadOnly = true;
            // 
            // timeTextBox
            // 
            resources.ApplyResources(this.timeTextBox, "timeTextBox");
            this.timeTextBox.Name = "timeTextBox";
            this.timeTextBox.ReadOnly = true;
            // 
            // hdopTextBox
            // 
            resources.ApplyResources(this.hdopTextBox, "hdopTextBox");
            this.hdopTextBox.Name = "hdopTextBox";
            this.hdopTextBox.ReadOnly = true;
            // 
            // dateTextBox
            // 
            resources.ApplyResources(this.dateTextBox, "dateTextBox");
            this.dateTextBox.Name = "dateTextBox";
            this.dateTextBox.ReadOnly = true;
            // 
            // statusTextBox
            // 
            resources.ApplyResources(this.statusTextBox, "statusTextBox");
            this.statusTextBox.Name = "statusTextBox";
            this.statusTextBox.ReadOnly = true;
            // 
            // altitudeTextBox
            // 
            resources.ApplyResources(this.altitudeTextBox, "altitudeTextBox");
            this.altitudeTextBox.Name = "altitudeTextBox";
            this.altitudeTextBox.ReadOnly = true;
            // 
            // speedTextBox
            // 
            resources.ApplyResources(this.speedTextBox, "speedTextBox");
            this.speedTextBox.Name = "speedTextBox";
            this.speedTextBox.ReadOnly = true;
            // 
            // satLabel8
            // 
            resources.ApplyResources(this.satLabel8, "satLabel8");
            this.satLabel8.Name = "satLabel8";
            // 
            // satLabel7
            // 
            resources.ApplyResources(this.satLabel7, "satLabel7");
            this.satLabel7.Name = "satLabel7";
            // 
            // satLabel6
            // 
            resources.ApplyResources(this.satLabel6, "satLabel6");
            this.satLabel6.Name = "satLabel6";
            // 
            // satLabel5
            // 
            resources.ApplyResources(this.satLabel5, "satLabel5");
            this.satLabel5.Name = "satLabel5";
            // 
            // satLabel4
            // 
            resources.ApplyResources(this.satLabel4, "satLabel4");
            this.satLabel4.Name = "satLabel4";
            // 
            // satLabel3
            // 
            resources.ApplyResources(this.satLabel3, "satLabel3");
            this.satLabel3.Name = "satLabel3";
            // 
            // satLabel2
            // 
            resources.ApplyResources(this.satLabel2, "satLabel2");
            this.satLabel2.Name = "satLabel2";
            // 
            // label20
            // 
            resources.ApplyResources(this.label20, "label20");
            this.label20.Name = "label20";
            // 
            // label19
            // 
            resources.ApplyResources(this.label19, "label19");
            this.label19.Name = "label19";
            // 
            // label18
            // 
            resources.ApplyResources(this.label18, "label18");
            this.label18.Name = "label18";
            // 
            // directionTextBox
            // 
            resources.ApplyResources(this.directionTextBox, "directionTextBox");
            this.directionTextBox.Name = "directionTextBox";
            this.directionTextBox.ReadOnly = true;
            // 
            // label17
            // 
            resources.ApplyResources(this.label17, "label17");
            this.label17.Name = "label17";
            // 
            // label1
            // 
            resources.ApplyResources(this.label1, "label1");
            this.label1.Name = "label1";
            // 
            // longitudeTextBox
            // 
            resources.ApplyResources(this.longitudeTextBox, "longitudeTextBox");
            this.longitudeTextBox.Name = "longitudeTextBox";
            this.longitudeTextBox.ReadOnly = true;
            // 
            // label16
            // 
            resources.ApplyResources(this.label16, "label16");
            this.label16.Name = "label16";
            // 
            // latitudeTextBox
            // 
            resources.ApplyResources(this.latitudeTextBox, "latitudeTextBox");
            this.latitudeTextBox.Name = "latitudeTextBox";
            this.latitudeTextBox.ReadOnly = true;
            // 
            // label15
            // 
            resources.ApplyResources(this.label15, "label15");
            this.label15.Name = "label15";
            // 
            // label14
            // 
            resources.ApplyResources(this.label14, "label14");
            this.label14.Name = "label14";
            // 
            // label13
            // 
            resources.ApplyResources(this.label13, "label13");
            this.label13.Name = "label13";
            // 
            // label12
            // 
            resources.ApplyResources(this.label12, "label12");
            this.label12.Name = "label12";
            // 
            // comPortsComboBox
            // 
            resources.ApplyResources(this.comPortsComboBox, "comPortsComboBox");
            this.comPortsComboBox.Name = "comPortsComboBox";
            // 
            // connectGPSButton
            // 
            resources.ApplyResources(this.connectGPSButton, "connectGPSButton");
            this.connectGPSButton.Name = "connectGPSButton";
            this.connectGPSButton.Click += new System.EventHandler(this.connectGPSButton_Click);
            // 
            // scanCOMPortButton
            // 
            resources.ApplyResources(this.scanCOMPortButton, "scanCOMPortButton");
            this.scanCOMPortButton.Name = "scanCOMPortButton";
            this.scanCOMPortButton.Click += new System.EventHandler(this.scanCOMPorts);
            // 
            // progressBar8
            // 
            resources.ApplyResources(this.progressBar8, "progressBar8");
            this.progressBar8.Maximum = 99;
            this.progressBar8.Name = "progressBar8";
            // 
            // progressBar7
            // 
            resources.ApplyResources(this.progressBar7, "progressBar7");
            this.progressBar7.Maximum = 99;
            this.progressBar7.Name = "progressBar7";
            // 
            // progressBar6
            // 
            resources.ApplyResources(this.progressBar6, "progressBar6");
            this.progressBar6.Maximum = 99;
            this.progressBar6.Name = "progressBar6";
            // 
            // progressBar5
            // 
            resources.ApplyResources(this.progressBar5, "progressBar5");
            this.progressBar5.Maximum = 99;
            this.progressBar5.Name = "progressBar5";
            // 
            // progressBar4
            // 
            resources.ApplyResources(this.progressBar4, "progressBar4");
            this.progressBar4.Maximum = 99;
            this.progressBar4.Name = "progressBar4";
            // 
            // progressBar3
            // 
            resources.ApplyResources(this.progressBar3, "progressBar3");
            this.progressBar3.Maximum = 99;
            this.progressBar3.Name = "progressBar3";
            // 
            // progressBar2
            // 
            resources.ApplyResources(this.progressBar2, "progressBar2");
            this.progressBar2.Maximum = 99;
            this.progressBar2.Name = "progressBar2";
            // 
            // progressBar1
            // 
            resources.ApplyResources(this.progressBar1, "progressBar1");
            this.progressBar1.Maximum = 99;
            this.progressBar1.Name = "progressBar1";
            // 
            // queueSizeBar
            // 
            resources.ApplyResources(this.queueSizeBar, "queueSizeBar");
            this.queueSizeBar.Maximum = 10;
            this.queueSizeBar.Name = "queueSizeBar";
            // 
            // satLabel1
            // 
            resources.ApplyResources(this.satLabel1, "satLabel1");
            this.satLabel1.Name = "satLabel1";
            // 
            // readWritePage
            // 
            this.readWritePage.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(224)))), ((int)(((byte)(255)))));
            this.readWritePage.Controls.Add(this.readWriteTB);
            this.readWritePage.Controls.Add(this.readWriteStatusLabel);
            this.readWritePage.Controls.Add(this.label25);
            this.readWritePage.Controls.Add(this.writeButton);
            this.readWritePage.Controls.Add(this.readButton);
            resources.ApplyResources(this.readWritePage, "readWritePage");
            this.readWritePage.Name = "readWritePage";
            // 
            // readWriteTB
            // 
            resources.ApplyResources(this.readWriteTB, "readWriteTB");
            this.readWriteTB.Name = "readWriteTB";
            // 
            // readWriteStatusLabel
            // 
            resources.ApplyResources(this.readWriteStatusLabel, "readWriteStatusLabel");
            this.readWriteStatusLabel.Name = "readWriteStatusLabel";
            // 
            // label25
            // 
            resources.ApplyResources(this.label25, "label25");
            this.label25.Name = "label25";
            // 
            // writeButton
            // 
            resources.ApplyResources(this.writeButton, "writeButton");
            this.writeButton.Name = "writeButton";
            this.writeButton.Click += new System.EventHandler(this.readerClick);
            // 
            // readButton
            // 
            resources.ApplyResources(this.readButton, "readButton");
            this.readButton.Name = "readButton";
            this.readButton.Click += new System.EventHandler(this.readerClick);
            // 
            // statusPage
            // 
            this.statusPage.Controls.Add(this.statusListView);
            resources.ApplyResources(this.statusPage, "statusPage");
            this.statusPage.Name = "statusPage";
            // 
            // statusListView
            // 
            this.statusListView.Columns.Add(this.columnHeader1);
            this.statusListView.Columns.Add(this.columnHeader2);
            listViewItem1.Text = resources.GetString("statusListView.Items");
            listViewItem2.Text = resources.GetString("statusListView.Items1");
            listViewItem3.Text = resources.GetString("statusListView.Items2");
            listViewItem4.Text = resources.GetString("statusListView.Items3");
            listViewItem5.Text = resources.GetString("statusListView.Items4");
            listViewItem6.Text = resources.GetString("statusListView.Items5");
            listViewItem7.Text = resources.GetString("statusListView.Items6");
            listViewItem8.Text = resources.GetString("statusListView.Items7");
            listViewItem9.Text = resources.GetString("statusListView.Items8");
            listViewItem10.Text = resources.GetString("statusListView.Items9");
            this.statusListView.Items.Add(listViewItem1);
            this.statusListView.Items.Add(listViewItem2);
            this.statusListView.Items.Add(listViewItem3);
            this.statusListView.Items.Add(listViewItem4);
            this.statusListView.Items.Add(listViewItem5);
            this.statusListView.Items.Add(listViewItem6);
            this.statusListView.Items.Add(listViewItem7);
            this.statusListView.Items.Add(listViewItem8);
            this.statusListView.Items.Add(listViewItem9);
            this.statusListView.Items.Add(listViewItem10);
            resources.ApplyResources(this.statusListView, "statusListView");
            this.statusListView.Name = "statusListView";
            this.statusListView.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader1
            // 
            resources.ApplyResources(this.columnHeader1, "columnHeader1");
            // 
            // columnHeader2
            // 
            resources.ApplyResources(this.columnHeader2, "columnHeader2");
            // 
            // GUIUpdateTimer
            // 
            this.GUIUpdateTimer.Enabled = true;
            this.GUIUpdateTimer.Interval = 1125;
            this.GUIUpdateTimer.Tick += new System.EventHandler(this.updateGUI_Tick);
            // 
            // tempPage
            // 
            this.tempPage.Controls.Add(this.button1);
            resources.ApplyResources(this.tempPage, "tempPage");
            this.tempPage.Name = "tempPage";
            // 
            // button1
            // 
            resources.ApplyResources(this.button1, "button1");
            this.button1.Name = "button1";
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // demoClient
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            resources.ApplyResources(this, "$this");
            this.Controls.Add(this.mainTabControl);
            this.KeyPreview = true;
            this.MinimizeBox = false;
            this.Name = "demoClient";
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.mainForm_KeyDown);
            this.mainTabControl.ResumeLayout(false);
            this.introPage.ResumeLayout(false);
            this.authPage.ResumeLayout(false);
            this.detectPage.ResumeLayout(false);
            this.patientPage.ResumeLayout(false);
            this.assayPage.ResumeLayout(false);
            this.gpsPage.ResumeLayout(false);
            this.readWritePage.ResumeLayout(false);
            this.statusPage.ResumeLayout(false);
            this.tempPage.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button readIDButton;
        private System.Windows.Forms.TabControl mainTabControl;
        private System.Windows.Forms.TabPage authPage;
        private System.Windows.Forms.TreeView authTreeView;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.TabPage patientPage;
        private System.Windows.Forms.Button medicationButton;
        private System.Windows.Forms.Button readPatientButton;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.PictureBox patientPhoto;
        private System.Windows.Forms.TextBox patientDescriptionBox;
        private System.Windows.Forms.TextBox patientNameBox;
        private System.Windows.Forms.PictureBox drugPhoto;
        private System.Windows.Forms.TabPage gpsPage;
        private System.Windows.Forms.ProgressBar progressBar7;
        private System.Windows.Forms.ProgressBar progressBar6;
        private System.Windows.Forms.ProgressBar progressBar5;
        private System.Windows.Forms.ProgressBar progressBar4;
        private System.Windows.Forms.ProgressBar progressBar3;
        private System.Windows.Forms.ProgressBar progressBar2;
        private System.Windows.Forms.ProgressBar queueSizeBar;
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
        private System.Windows.Forms.TextBox dateTextBox;
        private System.Windows.Forms.TextBox altitudeTextBox;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.TextBox satellitesUsedTextBox;
        private System.Windows.Forms.TextBox timeTextBox;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Label satLabel8;
        private System.Windows.Forms.Label satLabel7;
        private System.Windows.Forms.Label satLabel6;
        private System.Windows.Forms.Label satLabel5;
        private System.Windows.Forms.Label satLabel4;
        private System.Windows.Forms.Label satLabel3;
        private System.Windows.Forms.Label satLabel2;
        private System.Windows.Forms.Label satLabel1;
        private System.Windows.Forms.CheckBox trackingCheckBox;
        private System.Windows.Forms.ProgressBar progressBar1;
        private Microsoft.WindowsCE.Forms.InputPanel softInputPanel;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.Label pendingLookupsLabel;
        private System.Windows.Forms.Label queuedLookupsLabel;
        private System.Windows.Forms.Label label24;
        private System.Windows.Forms.TabPage readWritePage;
        private System.Windows.Forms.TextBox readWriteTB;
        private System.Windows.Forms.Label readWriteStatusLabel;
        private System.Windows.Forms.Label label25;
        private System.Windows.Forms.Button writeButton;
        private System.Windows.Forms.Button readButton;
        private System.Windows.Forms.TabPage detectPage;
        private System.Windows.Forms.Button detectTagTypeButton;
        private System.Windows.Forms.Label detectTagIDLabel;
        private System.Windows.Forms.Label detectTagTypeLabel;
        private System.Windows.Forms.TabPage introPage;
        private System.Windows.Forms.Button exitButton;
        private System.Windows.Forms.Label versionLabel;
        private System.Windows.Forms.Label label26;
        private System.Windows.Forms.PictureBox pictureBox2;
        private System.Windows.Forms.TabPage assayPage;
        private System.Windows.Forms.Button assayReadButton;
        private System.Windows.Forms.Label assayMessageLabel;
        private System.Windows.Forms.Button assayBeginButton;
        private System.Windows.Forms.Label assayTimerLabel;
        private System.Windows.Forms.Button assayRecordButton;
        private System.Windows.Forms.PictureBox assayImagePictureBox;
        private System.Windows.Forms.Button assayResetButton;
        private System.Windows.Forms.Button mainClearButton;
        private System.Windows.Forms.TextBox drugNameBox;
        private System.Windows.Forms.TabPage statusPage;
        private System.Windows.Forms.ListView statusListView;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.Button manualLookupButton;
        private System.Windows.Forms.Timer GUIUpdateTimer;
        private System.Windows.Forms.TextBox statusTextBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TabPage tempPage;
        private System.Windows.Forms.Button button1;        
        
    }
}

