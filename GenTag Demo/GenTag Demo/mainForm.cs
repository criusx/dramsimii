using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using C1Lib;
using System.Net;
using System.IO;
using System.IO.Ports;
using System.Net.Sockets;
using Microsoft.Win32;
using System.Resources;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Globalization;
using System.Threading;
using System.Security.Permissions;
using TestPocketGraphBar;
using Microsoft.WindowsMobile.Status;


[assembly: CLSCompliant(true)]
//[assembly: SecurityPermission(SecurityAction.RequestMinimum, Execution = true)]
namespace GentagDemo
{
    public partial class mainForm : Form
    {
        bool readerRunning;

        static mainForm mF;

        Graph graph;

        nmeaInterpreter gpsNmea;

        string DeviceUID;

        private CultureInfo cultureInfo;

        private Hashtable cachedIDLookups;

        private Queue tagQueue;

        [DllImport("coredll.dll")]
        private extern static int GetDeviceUniqueID(
            [In, Out] byte[] appdata,
            int cbApplictionData,
            int dwDeviceIDVersion,
            [In, Out] byte[] deviceIDOuput,
            out uint pcbDeviceIDOutput);

        public mainForm()
        {
            InitializeComponent();
           
            // generate the device's UID
            string AppString = GentagDemo.Properties.Resources.titleString;

            byte[] AppData = new byte[AppString.Length];

            for (int count = 0; count < AppString.Length; count++)
                AppData[count] = (byte)AppString[count];

            int appDataSize = AppData.Length;

            byte[] dUID = new byte[20];

            uint SizeOut = 20;

            GetDeviceUniqueID(AppData, appDataSize, 1, dUID, out SizeOut);

            for (int i = 0; i < 20; i++)
                DeviceUID += dUID[i].ToString("X");

            mF = this;

            cachedIDLookups = new Hashtable();

            tagQueue = new Queue();

            cultureInfo = new CultureInfo("en-US");

            gpsNmea = new nmeaInterpreter(cultureInfo);

            gpsNmea.DateTimeChanged += new nmeaInterpreter.DateTimeChangedEventHandler(gpsNmea_DateTimeChanged);
            gpsNmea.FixObtained += new nmeaInterpreter.FixObtainedEventHandler(gpsNmea_FixObtained);
            gpsNmea.FixLost += new nmeaInterpreter.FixLostEventHandler(gpsNmea_FixLost);
            gpsNmea.PositionReceived += new nmeaInterpreter.PositionReceivedEventHandler(gpsNmea_PositionReceived);
            gpsNmea.SpeedReceived += new nmeaInterpreter.SpeedReceivedEventHandler(gpsNmea_SpeedReceived);
            gpsNmea.BearingReceived += new nmeaInterpreter.BearingReceivedEventHandler(gpsNmea_BearingReceived);
            gpsNmea.SatelliteReceived += new nmeaInterpreter.SatelliteReceivedEventHandler(gpsNmea_SatelliteReceived);
            gpsNmea.PDOPReceived += new nmeaInterpreter.PDOPReceivedEventHandler(gpsNmea_PDOPReceived);
            gpsNmea.HDOPReceived += new nmeaInterpreter.HDOPReceivedEventHandler(gpsNmea_HDOPReceived);
            gpsNmea.VDOPReceived += new nmeaInterpreter.VDOPReceivedEventHandler(gpsNmea_VDOPReceived);
            gpsNmea.NumSatsReceived += new nmeaInterpreter.NumberOfSatellitesInViewEventHandler(gpsNmea_NumSatsReceived);



            ImageList myImageList = new ImageList();
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.blank.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.cancel.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.ok.GetHbitmap()));
            treeView1.ImageList = myImageList;

            graph = new Graph();
            graph.Visible = false;
            graph.Size = new Size(240, 230);
            graph.Location = new Point(0, 0);
            tabPage2.Controls.Add(graph);

            // Load Localized values
            readIDButton.Text = GentagDemo.Properties.Resources.readIDButtonInit;
            readLogButton.Text = GentagDemo.Properties.Resources.readLogButtonInit;
            manualIDButton.Text = GentagDemo.Properties.Resources.manualIDButtonInit;
            setValueButton.Text = GentagDemo.Properties.Resources.setValueButtonInit;
            readValueButton.Text = GentagDemo.Properties.Resources.readValueButtonInit;
            this.Text = GentagDemo.Properties.Resources.titleString;

            tabControl1.TabPages[0].Text = GentagDemo.Properties.Resources.tab2String;
            tabControl1.TabPages[1].Text = GentagDemo.Properties.Resources.tab1String;
            tabControl1.TabPages[2].Text = GentagDemo.Properties.Resources.tab3String;
            tabControl1.TabPages[3].Text = GentagDemo.Properties.Resources.tab4String;

            label6.Text = label1.Text = GentagDemo.Properties.Resources.hiLimitString;
            label5.Text = label2.Text = GentagDemo.Properties.Resources.loLimitString;
            label7.Text = label3.Text = GentagDemo.Properties.Resources.intervalString;
            label4.Text = GentagDemo.Properties.Resources.logString;

            textBox9.Text = @"";


            // Init the Registry
            RegistryKey regKey = Registry.LocalMachine;

            regKey = regKey.OpenSubKey(@"SOFTWARE", true);

            if (Array.IndexOf(regKey.GetSubKeyNames(), @"Gentag", 0) == -1)
                regKey.CreateSubKey(@"Gentag");
            regKey = regKey.OpenSubKey(@"Gentag", true);

            string[] settingKeys = regKey.GetSubKeyNames();

            //user ID
            if (Array.IndexOf(settingKeys, @"hostname", 0) == -1)
            {
                regKey.CreateSubKey(@"hostname");
                regKey.SetValue(@"hostname", @"129.2.99.117");
            }
            //hostName.Text = regKey.GetValue(@"hostname").ToString();
        }

        private string readTagID()
        {
            if (C1Lib.C1.NET_C1_open_comm() != 1)
            {
                MessageBox.Show(GentagDemo.Properties.Resources.error1);
            }
            else if (C1Lib.C1.NET_C1_enable() != 1)
            {
                MessageBox.Show(GentagDemo.Properties.Resources.error2);
                C1Lib.C1.NET_C1_disable();
            }
            else
            {
                // wait while a tag is read
                while ((readerRunning == true) && (C1Lib.ISO_15693.NET_get_15693(0x00) == 0)) { Thread.Sleep(100); }

                if (readerRunning == false)
                    throw new IOException("Reading of tag stopped");

                //while (C1Lib.ISO_15693.NET_read_multi_15693(0x00, C1Lib.ISO_15693.tag.blocks) != 1) { }


                //string rfidDescr = C1Lib.util.to_str(C1Lib.ISO_15693.tag.read_buff, 256);
                //rfidDescr += "\n";

                StringBuilder newTag = new StringBuilder(C1Lib.ISO_15693.tag.id_length);

                for (int i = 0; i < C1Lib.ISO_15693.tag.id_length; i++)
                    newTag.Append(C1Lib.util.hex_value(C1Lib.ISO_15693.tag.tag_id[i]));

                C1Lib.C1.NET_C1_disable();
                C1Lib.C1.NET_C1_close_comm();

                return newTag.ToString();
            }
            throw new IOException(@"Unable to read tag");
        }

        private void readID()
        {
            setWaitCursor(true);            

            while (readerRunning == true)
            {
                setCheckBox(checkBox1, false);

                try
                {
                    string currentTag = readTagID();

                    countUp();
                    lock (cachedIDLookups.SyncRoot)
                    {
                        if (cachedIDLookups.ContainsKey(currentTag) == false)
                            lock (tagQueue.SyncRoot)
                            {
                                if (tagQueue.Contains(currentTag) == false)
                                    tagQueue.Enqueue(currentTag);
                            }
                    }
                }
                catch (System.IO.IOException ex)
                {
                    MessageBox.Show(ex.Message);
                    readerRunning = false;
                }
                catch (System.NotSupportedException xx)
                {
                    MessageBox.Show("not working");
                }

                setCheckBox(checkBox1, true);


                Thread.Sleep(100);
            }
            setWaitCursor(false);
        }

        
        private void doLookup()
        {            
            while (readerRunning == true)
            {
                string currentTag;

                lock (tagQueue.SyncRoot)
                {
                    if (tagQueue.Count > 0)
                        currentTag = (string)tagQueue.Dequeue();
                    else
                        currentTag = "";
                }
                if ((currentTag.Length < 2) || (cachedIDLookups.ContainsKey(currentTag) == true))
                    Thread.Sleep(250);
                else
                {
                    int triesLeft = 8;
                    string failMessage = "";

                    org.dyndns.crius.GetDatesWS ws = new org.dyndns.crius.GetDatesWS();

                    while (triesLeft > 0)
                    {
                        // use the web service to retrieve a description  
                        try
                        {
                            countUp2();
                            string description = ws.getDescription(currentTag);
                            updateTreeView1(currentTag, description, ws.isAuthenticated(currentTag, DeviceUID, currentLatitude, currentLongitude));
                            lock (cachedIDLookups.SyncRoot)
                            {
                                cachedIDLookups.Add(currentTag, description);
                            }
                            break;
                        }
                        catch (WebException ex)
                        {
                            failMessage = "Problem connecting to web service: " + ex.Message;
                            triesLeft--;
                            Thread.Sleep(2000);
                        }
                        catch (SocketException ex)
                        {
                            failMessage = ex.Message;
                            triesLeft--;
                            Thread.Sleep(2000);
                        }
                        catch (IOException ex)
                        {
                            //MessageBox.Show(ex.Message);
                            //return;
                        }
                    }
                    if (triesLeft == 0)
                    {
                        updateTreeView1(currentTag, @"No description found", false);
                        //MessageBox.Show(failMessage);
                    }
                }
            }
        }

        private delegate void incCounter();

        private void countUp()
        {
            if (this.InvokeRequired)
            {
                Invoke(new incCounter(countUp));
                return;
            }
            numericUpDown1.Value++;
        }

        private void countUp2()
        {
            if (this.InvokeRequired)
            {
                Invoke(new incCounter(countUp2));
                return;
            }
            numericUpDown2.Value++;
        }

        private delegate void updateTreeView1Delegate(string currentTag, string rfidDescr, bool isAuthenticated);

        private void updateTreeView1(string currentTag, string rfidDescr, bool isAuthenticated)
        {
            if (this.InvokeRequired)
            {
                Invoke(new updateTreeView1Delegate(updateTreeView1),
                    new object[] { currentTag, rfidDescr, isAuthenticated });
                return;
            }
            treeView1.BeginUpdate();

            bool exists = false;

            foreach (TreeNode tn in treeView1.Nodes)
            {
                //Console.Out.WriteLine(tn.Text);
                if (tn.Text == currentTag)
                {
                    tn.Nodes.Clear();
                    TreeNode tN = new TreeNode(rfidDescr);
                    tn.SelectedImageIndex = tn.ImageIndex = isAuthenticated ? 2 : 1;
                    tn.Nodes.Add(tN);
                    exists = true;
                }
            }
            if (!exists)
            {
                TreeNode superTn = new TreeNode(currentTag);
                superTn.SelectedImageIndex = superTn.ImageIndex = isAuthenticated ? 2 : 1; ;

                TreeNode subTn = new TreeNode(rfidDescr);
                subTn.SelectedImageIndex = subTn.ImageIndex = 0;

                superTn.Nodes.Add(subTn);
                treeView1.Nodes.Add(superTn);
            }
            treeView1.EndUpdate();
        }

        private void mainForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == System.Windows.Forms.Keys.Back)
            {
                treeView1.Nodes.Clear();
            }
            else if (e.KeyCode == System.Windows.Forms.Keys.Q)
            {
                Application.Exit();
            }
        }

        public delegate void writeViolationsCB(
            Single upperTempLimit,
            Single lowerTempLimit,
            Int32 len,
            short recordPeriod,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] int[] dateTime,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] Byte[] logMode,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] Single[] temperatures);

        [DllImport("VarioSens Lib.dll")]
        private static extern int getVarioSensLog(writeViolationsCB cb);

        private delegate void writeViolationsDelegate(
            Single upperTempLimit,
            Single lowerTempLimit,
            Int32 len,
            short recordPeriod,
            int[] dateTime,
            Byte[] logMode,
            Single[] temperatures);

        private static DateTime origin = System.TimeZone.CurrentTimeZone.ToLocalTime(new DateTime(1970, 1, 1, 0, 0, 0));

        private static void writeViolations(
            Single upperTempLimit,
            Single lowerTempLimit,
            Int32 len,
            short recordPeriod,
            int[] dateTime,
            Byte[] logMode,
            Single[] temperatures)
        {
            if (mF.InvokeRequired)
            {
                mF.Invoke(new writeViolationsDelegate(writeViolations),
                    new object[] { upperTempLimit, lowerTempLimit, len, recordPeriod, dateTime, logMode, temperatures });
                return;
            }
            mF.textBox9.Text = @"";
            mF.textBox1.Text = upperTempLimit.ToString();
            mF.textBox2.Text = lowerTempLimit.ToString();
            mF.textBox3.Text = recordPeriod.ToString();
            mF.listBox1.Items.Clear();
            mF.graph.Clear();

            for (int i = 0; i < dateTime.Length; i++)
            {
                if (logMode[i] == 1)
                {
                    mF.listBox1.Items.Add(temperatures[i].ToString("F"));
                    mF.graph.Add(i, temperatures[i]);
                }
                else if (logMode[i] == 2)
                {
                    UInt32 time_t = Convert.ToUInt32(dateTime[i]);

                    DateTime convertedValue = origin + new TimeSpan(time_t * TimeSpan.TicksPerSecond);
                    if (System.TimeZone.CurrentTimeZone.IsDaylightSavingTime(convertedValue) == true)
                    {
                        System.Globalization.DaylightTime daylightTime = System.TimeZone.CurrentTimeZone.GetDaylightChanges(convertedValue.Year);
                        convertedValue = convertedValue + daylightTime.Delta;
                    }
                    mF.listBox1.Items.Add(temperatures[i].ToString("F", CultureInfo.CurrentCulture) + " C" + convertedValue.ToString());

                }
            }
            if (logMode[0] == 1)
            {
                try
                {
                    if (mF.listBox1.Items.Count > 1)
                    {
                        mF.graph.Visible = true;
                        mF.graph.BringToFront();
                    }
                }
                catch (Exception ee)
                {
                    throw;
                }
            }
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void readerClick(object sender, EventArgs e)
        {
            if (readerRunning == false)
            {
                readerRunning = true;
                if (sender == readLogButton)
                    new Thread(new ThreadStart(readLog)).Start();
                else if (sender == readIDButton)
                {
                    new Thread(new ThreadStart(readID)).Start();
                    new Thread(new ThreadStart(doLookup)).Start();
                }
                else if (sender == setValueButton)
                    new Thread(new ThreadStart(setVSSettings)).Start();
                else if (sender == readValueButton)
                    new Thread(new ThreadStart(getVSSettings)).Start();
                else if (sender == readPatientButton)
                    new Thread(new ThreadStart(readPatientData)).Start();
                else if (sender == medicationButton)
                    new Thread(new ThreadStart(readDrugData)).Start();
            }
            else
            {
                readerRunning = false;
            }
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void readLog()
        {
            Cursor.Current = Cursors.WaitCursor;

            setButtons(readLogButton);

            writeViolationsCB mycb = new writeViolationsCB(writeViolations);

            int errorCode = 0;
            while (readerRunning == true)
            {
                if ((errorCode = getVarioSensLog(mycb)) == 0)
                    readerRunning = false;
                else if ((errorCode == -1) || (errorCode == -2) || (errorCode == -6))
                    readerRunning = false;
                Thread.Sleep(100);
            }

            switch (errorCode)
            {
                case -1:
                    MessageBox.Show(GentagDemo.Properties.Resources.error1);
                    break;
                case -2:
                    MessageBox.Show(GentagDemo.Properties.Resources.error2);
                    break;
                //case -3:
                //    mF.textBox9.Text = Gentag_Demo.Properties.Resources.error3;
                //    break;
                //case -4:
                //    mF.textBox9.Text = Gentag_Demo.Properties.Resources.error4;
                //    break;
                //case -5:
                //    mF.textBox9.Text = Gentag_Demo.Properties.Resources.error5;
                //    break;
                //case -6:
                //    mF.textBox9.Text = Gentag_Demo.Properties.Resources.error6;
                //    break;
                //case -7:
                //    mF.textBox9.Text = Gentag_Demo.Properties.Resources.error7;
                //    break;
                default:
                    break;
            }

            // TODO: go do web service stuff

            resetButtons(readLogButton);

            Cursor.Current = Cursors.Default;
        }
        #region Adjust Buttons
        private delegate void resetButtonsDelegate(object obj);

        private void resetButtons(object obj)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new resetButtonsDelegate(resetButtons), new object[] { obj });
                return;
            }
            // reset buttons depending what was passed in
            readIDButton.Enabled = true;
            setValueButton.Enabled = true;
            readValueButton.Enabled = true;
            readLogButton.Enabled = true;

            if (obj == readLogButton)
            {
                readLogButton.Text = GentagDemo.Properties.Resources.readLogButtonInit;
            }
            else if (obj == setValueButton)
            {
                setValueButton.Text = GentagDemo.Properties.Resources.setValueButtonInit;
            }
            else if (obj == readValueButton)
            {
                setValueButton.Text = GentagDemo.Properties.Resources.readValueButtonInit;
            }
            else if (obj == setValueButton)
            {
                readIDButton.Text = GentagDemo.Properties.Resources.readIDButtonInit;
            }
        }

        private delegate void setButtonsDelegate(object obj);

        private void setButtons(object obj)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setButtonsDelegate(setButtons), new object[] { obj });
                return;
            }

            ((Button)obj).Text = @"Stop";
            // reset buttons depending what was passed in
            if (obj != readLogButton)
            {
                readLogButton.Enabled = false;
            }
            if (obj != readIDButton)
            {
                readIDButton.Enabled = false;
            }
            if (obj != setValueButton)
            {
                setValueButton.Enabled = false;
            }
            if (obj != readValueButton)
            {
                readValueButton.Enabled = false;
            }
        }
        #endregion
        private void manualLookupClick(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;

            textBox4.Enabled = false;

            org.dyndns.crius.GetDatesWS ws = new org.dyndns.crius.GetDatesWS();

            string rfidDescr = @"No description found";

            try
            {
                rfidDescr = ws.getDescription(textBox4.Text);
            }
            catch (WebException ex)
            {
                MessageBox.Show(@"Problem connecting to web service: " + ex.Message);
            }

            treeView2.BeginUpdate();

            bool exists = false;

            foreach (TreeNode tn in treeView2.Nodes)
            {
                if (tn.Text == textBox4.Text)
                {
                    tn.Nodes.Clear();
                    tn.Nodes.Add(rfidDescr);
                    exists = true;
                }
            }
            if (!exists)
            {
                treeView2.Nodes.Add(textBox4.Text).Nodes.Add(rfidDescr);
            }
            treeView1.EndUpdate();

            textBox4.Enabled = true;

            Cursor.Current = Cursors.Default;
        }

        [DllImport("VarioSens Lib.dll")]
        private static extern int setVarioSensSettings(float lowTemp, float hiTemp, int interval, int mode, int batteryCheckInterval);

        private void setVSSettings()
        {
            Cursor.Current = Cursors.WaitCursor;

            try
            {
                int mode;

                if (getComboBoxIndex(comboBox1) == 0)
                    mode = 1;
                else
                    mode = 2;

                int errorCode = -1;

                while ((readerRunning == true) &&
                    ((errorCode = setVarioSensSettings(
                    float.Parse(getTextBox(textBox7)),
                    float.Parse(getTextBox(textBox6)),
                    int.Parse(getTextBox(textBox5)), mode,
                    int.Parse(getTextBox(textBox8)))) != 0))
                {
                    Thread.Sleep(100);
                }

                if (errorCode != 0)
                    setTextBox(getSetStatusBox, "Error writing tag");
            }
            catch (ArgumentException ex)
            {
                MessageBox.Show("Error parsing: " + ex.ToString());
            }
            catch (FormatException ex)
            {
                MessageBox.Show("A setting is not in a valid format: " + ex.ToString());
            }
            catch (OverflowException ex)
            {
                MessageBox.Show("A value is too large or small: " + ex.ToString());
            }

            readerRunning = false;
            Cursor.Current = Cursors.Default;
        }

        public delegate void writeVSSettingsCB(
            Single upperTempLimit,
            Single lowerTempLimit,
            short recordPeriod,
            short logMode,
            short batteryCheckInterval);

        [DllImport("VarioSens Lib.dll")]
        private static extern int getVarioSensSettings(writeVSSettingsCB cb);

        private void getVSSettingsCB(Single upper, Single lower, short period, short logMode, short batteryCheckInterval)
        {
            mF.setTextBox(textBox7, lower.ToString());
            mF.setTextBox(textBox6, upper.ToString());
            mF.setTextBox(textBox5, period.ToString());
            mF.setComboBoxIndex(mF.comboBox1, logMode - 1);
            mF.setTextBox(textBox8, batteryCheckInterval.ToString());
        }

        private void getVSSettings()
        {
            Cursor.Current = Cursors.WaitCursor;

            writeVSSettingsCB writeVSCB = new writeVSSettingsCB(getVSSettingsCB);

            int errorCode = -1;

            while ((readerRunning == true) && ((errorCode = getVarioSensSettings(writeVSCB)) != 0))
            {
                Thread.Sleep(100);
            }

            if (errorCode != 0)
                setTextBox(getSetStatusBox, "Error reading tag");

            readerRunning = false;
            Cursor.Current = Cursors.Default;
        }

        private string patientID;

        private void readPatientData()
        {
            Cursor.Current = Cursors.WaitCursor;

            try
            {
                patientID = readTagID();
                org.dyndns.crius.GetDatesWS ws = new org.dyndns.crius.GetDatesWS();
                ws.Timeout = 30000;
                org.dyndns.crius.GetDates values = ws.getInfo(patientID, false);
                setTextBox(patientNameBox, values.name);
                setTextBox(patientDescriptionBox, values.desc);

                byte[] bA = ws.getPicture(patientID, false);
                setPhoto(patientPhoto, bA);
            }
            catch (IOException ex)
            {
                MessageBox.Show(ex.Message);
            }
            catch (WebException ex)
            {

            }

            readerRunning = false;
            Cursor.Current = Cursors.Default;
        }

        private string drugID;

        private void readDrugData()
        {
            Cursor.Current = Cursors.WaitCursor;
            drugID = readTagID();
            try
            {
                org.dyndns.crius.GetDatesWS ws = new org.dyndns.crius.GetDatesWS();
                //org.dyndns.crius.GetDates values = ws.getInfo(drugID, true);
                byte[] bA = ws.getPicture(drugID, true);
                bool drugInteraction = ws.checkInteraction(patientID, drugID);

                //setPatientData(values.name, values.desc);
                setPhoto(drugPhoto, bA);

                if (drugInteraction == true)
                    MessageBox.Show("Warning! \nThis drug should not be taken by this patient.");
            }
            catch (WebException ex)
            {
                MessageBox.Show("Problem connecting to web service: " + ex.Message);
            }

            readerRunning = false;
            Cursor.Current = Cursors.Default;
        }

        #region Safe Accessors and Mutators

        private delegate void setWaitCursorDelegate(bool set);

        private void setWaitCursor(bool set)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setWaitCursorDelegate(setWaitCursor), new object[] { set });
            }
            if (set == true)
                Cursor.Current = Cursors.WaitCursor;
            else
                Cursor.Current = Cursors.Default;
        }

        private delegate void setCheckBoxDelegate(CheckBox cB, bool check);

        private void setCheckBox(CheckBox cB, bool check)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setCheckBoxDelegate(setCheckBox), new object[] { cB, check });
                return;
            }
            cB.Checked = check;
        }

        private delegate int getComboBoxIndexDelegate(ComboBox cB);

        private int getComboBoxIndex(ComboBox cB)
        {
            if (this.InvokeRequired)
            {
                return (int)this.Invoke(new getComboBoxIndexDelegate(getComboBoxIndex), new object[] { cB });
            }
            return cB.SelectedIndex;
        }

        private delegate void setComboBoxIndexDelegate(ComboBox cB, int index);

        private void setComboBoxIndex(ComboBox cB, int index)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setComboBoxIndexDelegate(setComboBoxIndex), new object[] { cB, index });
                return;
            }
            try
            {
                cB.SelectedIndex = index;
            }
            catch (ArgumentOutOfRangeException ex)
            {
                cB.SelectedIndex = 0;
            }
        }

        private delegate void setTextBoxDelegate(TextBox tB, string desc);

        private void setTextBox(TextBox tB, string val)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setTextBoxDelegate(setTextBox), new object[] { tB, val });
                return;
            }
            tB.Text = val;
        }

        private delegate string getTextBoxDelegate(TextBox tB);

        private string getTextBox(TextBox tB)
        {
            if (this.InvokeRequired)
            {
                return (string)this.Invoke(new getTextBoxDelegate(getTextBox), new object[] { tB });
            }
            return tB.Text;
        }

        private delegate void setPhotoDelegate(PictureBox pB, byte[] bA);

        private void setPhoto(PictureBox pB, byte[] bA)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setPhotoDelegate(setPhoto), new object[] { pB, bA });
                return;
            }
            try
            {
                pB.Image = new Bitmap(new MemoryStream(bA));
                pB.Refresh();
            }
            catch (ArgumentException ex)
            {

            }
        }
        #endregion

        private void scanCOMPortButton_Click(object sender, EventArgs e)
        {
            string[] COMPorts = System.IO.Ports.SerialPort.GetPortNames();

            Array.Sort(COMPorts);

            comPortsComboBox.Items.Clear();
            bool foundCom7 = false;
            if (COMPorts.Length > 0)
            {
                int i = 0;
                foreach (string sCOMPort in COMPorts)
                {
                    comPortsComboBox.Items.Add(sCOMPort);
                    if (sCOMPort == @"COM7")
                    {
                        foundCom7 = true;
                        comPortsComboBox.SelectedIndex = i;
                    }
                    i++;
                }
                comPortsComboBox.SelectedIndex = 0;
            }
            if (foundCom7 == false)
            {
                comPortsComboBox.Items.Add(@"COM7");
                comPortsComboBox.SelectedIndex = comPortsComboBox.Items.Count - 1;
            }
        }

        private void connectGPSButton_Click(object sender, EventArgs e)
        {
            string errorMsg = "";
            for (int triesLeft = 5; triesLeft > 0; )
            {
                try
                {
                    if (!gpsSerialPort.IsOpen)
                    {
                        gpsSerialPort.PortName = comPortsComboBox.SelectedItem.ToString();
                        gpsSerialPort.Open();
                        connectGPSButton.Text = "Disconnect";
                    }
                    else
                    {
                        gpsSerialPort.Close();
                        connectGPSButton.Text = "Connect";
                    }
                    errorMsg = "";
                    triesLeft = 0;
                }
                catch (Exception ex)
                {
                    connectGPSButton.Text = "Connect";
                    errorMsg = ex.Message;
                    triesLeft--;
                }
            }
            if (errorMsg.Length > 2)
                MessageBox.Show(errorMsg);
        }

        private void gpsSerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            if (InvokeRequired)
                Invoke(new SerialDataReceivedEventHandler(gpsSerialPort_DataReceived), new object[] { sender, e });
            else
            {
                string[] buffer = gpsSerialPort.ReadExisting().Split('$');
                gpsSerialPort.DiscardInBuffer();
                for (int i = 1; i < buffer.Length - 1; i++)
                    if (buffer[i].StartsWith(@"GPRMC") || buffer[i].StartsWith(@"GPGSV") || buffer[i].StartsWith(@"GPGSA"))
                        if (gpsNmea.Parse(@"$" + buffer[i].Substring(0, buffer[i].Length - 2)) == false)
                        {
                            progressBar1.Value++;
                            //MessageBox.Show(buffer[i]);
                        }
            }

        }

        private void gpsNmea_DateTimeChanged(DateTime dT)
        {
            timeTextBox.Text = dT.ToString("t");
        }
        private void gpsNmea_FixObtained()
        {
            statusTextBox.Text = @"Fixed";
        }
        private void gpsNmea_FixLost()
        {
            statusTextBox.Text = @"Searching";
        }

        private const double maximumHDOPValue = 7.0;

        private string currentLatitude = "0";

        private string currentLongitude = "0";

        private void gpsNmea_PositionReceived(string latitude, string longitude)
        {
            if (currentHDOPValue < maximumHDOPValue)
            {
                currentLatitude = latitudeTextBox.Text = latitude;
                currentLongitude = longitudeTextBox.Text = longitude;
            }
        }
        private void gpsNmea_SpeedReceived(double speed)
        {
            speedTextBox.Text = speed.ToString();
        }
        private void gpsNmea_BearingReceived(double bearing)
        {
            directionTextBox.Text = bearing.ToString();
        }

        private int satNumber;
        private void gpsNmea_SatelliteReceived(int pseudoRandomCode, int azimuth, int elevation, int signalToNoiseRatio, bool firstMessage)
        {
            if (firstMessage == true)
            {
                satNumber = 0;
                satLabel1.Text = satLabel2.Text = satLabel3.Text =
                    satLabel4.Text = satLabel5.Text = satLabel6.Text =
                    satLabel7.Text = satLabel8.Text = "#";
                progressBar1.Value = progressBar2.Value = progressBar3.Value =
                    progressBar4.Value = progressBar5.Value =
                    progressBar6.Value = progressBar7.Value =
                    progressBar8.Value = 0;
            }
            switch (satNumber)
            {
                case 0:
                    satLabel1.Text = pseudoRandomCode.ToString();
                    progressBar1.Value = signalToNoiseRatio;
                    break;
                case 1:
                    satLabel2.Text = pseudoRandomCode.ToString();
                    progressBar2.Value = signalToNoiseRatio;
                    break;
                case 2:
                    satLabel3.Text = pseudoRandomCode.ToString();
                    progressBar3.Value = signalToNoiseRatio;
                    break;
                case 3:
                    satLabel4.Text = pseudoRandomCode.ToString();
                    progressBar4.Value = signalToNoiseRatio;
                    break;
                case 4:
                    satLabel5.Text = pseudoRandomCode.ToString();
                    progressBar5.Value = signalToNoiseRatio;
                    break;
                case 5:
                    satLabel6.Text = pseudoRandomCode.ToString();
                    progressBar6.Value = signalToNoiseRatio;
                    break;
                case 6:
                    satLabel7.Text = pseudoRandomCode.ToString();
                    progressBar7.Value = signalToNoiseRatio;
                    break;
                case 7:
                    satLabel8.Text = pseudoRandomCode.ToString();
                    progressBar8.Value = signalToNoiseRatio;
                    break;
            }
            satNumber = (satNumber + 1) % 8;
        }

        private void gpsNmea_PDOPReceived(double value)
        {

        }

        private double currentHDOPValue = 15;

        private void gpsNmea_HDOPReceived(double value)
        {
            hdopTextBox.Text = value.ToString();
            currentHDOPValue = value;
        }

        private void gpsNmea_VDOPReceived(double value)
        {
            vdopTextBox.Text = value.ToString();
        }

        private void gpsNmea_NumSatsReceived(int value)
        {
            satellitesUsedTextBox.Text = value.ToString();
        }
    }
}