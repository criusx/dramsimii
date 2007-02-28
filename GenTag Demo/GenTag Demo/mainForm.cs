using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using C1Lib;
//using Protocol;
using System.Net;
using System.IO;
using System.Net.Sockets;
using Microsoft.Win32;
using System.Resources;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Globalization;
using System.Threading;
using TestPocketGraphBar;


[assembly: CLSCompliant(true)]
namespace GenTag_Demo
{
    public partial class mainForm : Form
    {
        bool readerRunning;

        static mainForm mF;

        Graph graph;  

        public mainForm()
        {
            InitializeComponent();

            mF = this;

            readerRunning = false;

            graph = new Graph();
            graph.Visible = false;
            graph.Size = new Size(240, 230);
            graph.Location = new Point(0, 0);
            tabPage2.Controls.Add(graph);

            // Load Localized values
            readIDButton.Text = GenTag_Demo.Properties.Resources.readIDButtonInit;
            readLogButton.Text = GenTag_Demo.Properties.Resources.readLogButtonInit;
            manualIDButton.Text = GenTag_Demo.Properties.Resources.manualIDButtonInit;
            setValueButton.Text = GenTag_Demo.Properties.Resources.setValueButtonInit;
            readValueButton.Text = GenTag_Demo.Properties.Resources.readValueButtonInit;
            this.Text = GenTag_Demo.Properties.Resources.titleString;

            tabControl1.TabPages[0].Text = GenTag_Demo.Properties.Resources.tab2String;
            tabControl1.TabPages[1].Text = GenTag_Demo.Properties.Resources.tab1String;
            tabControl1.TabPages[2].Text = GenTag_Demo.Properties.Resources.tab3String;
            tabControl1.TabPages[3].Text = GenTag_Demo.Properties.Resources.tab4String;

            label6.Text = label1.Text = GenTag_Demo.Properties.Resources.hiLimitString;
            label5.Text = label2.Text = GenTag_Demo.Properties.Resources.loLimitString;
            label7.Text = label3.Text = GenTag_Demo.Properties.Resources.intervalString;
            label4.Text = GenTag_Demo.Properties.Resources.logString;

            textBox9.Text = @"";


            // Init the Registry
            RegistryKey regKey = Registry.LocalMachine;

            regKey = regKey.OpenSubKey(@"SOFTWARE", true);

            if (Array.IndexOf(regKey.GetSubKeyNames(), @"GenTag", 0) == -1)
                regKey.CreateSubKey(@"GenTag");
            regKey = regKey.OpenSubKey(@"GenTag", true);

            string[] settingKeys = regKey.GetSubKeyNames();

            //user ID
            if (Array.IndexOf(settingKeys, @"hostname", 0) == -1)
            {
                regKey.CreateSubKey(@"hostname");
                regKey.SetValue(@"hostname", @"129.2.99.117");
            }
            //hostName.Text = regKey.GetValue(@"hostname").ToString();
        }

        private void readIDClick(object sender, EventArgs e)
        {
            if (readerRunning == false)
            {
                readerRunning = true;
                new Thread(new ThreadStart(readID)).Start();
            }
            else
            {
                readerRunning = false;
            }
        }

        private delegate void updateTreeDelegate();

        private string readTagID()
        {
            if (C1Lib.C1.NET_C1_open_comm() != 1)
            {
                MessageBox.Show(GenTag_Demo.Properties.Resources.error1);
            }
            else if (C1Lib.C1.NET_C1_enable() != 1)
            {
                MessageBox.Show(GenTag_Demo.Properties.Resources.error2);
                C1Lib.C1.NET_C1_disable();
            }
            else
            {
                // wait while a tag is read
                while ((readerRunning == true) && (C1Lib.ISO_15693.NET_get_15693(0x00) == 0)) { }

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
            throw new System.IO.IOException("Unable to read tag");
        }

        private void readID()
        {
            Cursor.Current = Cursors.WaitCursor;

            try
            {
                string currentTag = readTagID();

                string rfidDescr = "";

                // use the web service to retrieve a description
                try
                {
                    org.dyndns.crius.GetDatesWS ws = new org.dyndns.crius.GetDatesWS();

                    try
                    {
                        rfidDescr += ws.getDescription(currentTag);
                    }
                    catch (WebException ex)
                    {
                        rfidDescr += @"No description found";
                        MessageBox.Show("Problem connecting to web service: " + ex.Message);
                    }
                }
                catch (SocketException ex)
                {
                    MessageBox.Show(ex.Message);
                    return;
                }

                updateTreeView1(currentTag, rfidDescr);
            }
            catch (System.IO.IOException ex)
            {
                MessageBox.Show(ex.Message);
            }

            readerRunning = false;
            Cursor.Current = Cursors.Default;
        }

        private delegate void updateTreeView1Delegate(string currentTag, string rfidDescr);

        private void updateTreeView1(string currentTag, string rfidDescr)
        {
            if (InvokeRequired)
            {
                Invoke(new updateTreeView1Delegate(updateTreeView1),
                    new object[] { currentTag, rfidDescr });
                return;
            }
            treeView1.BeginUpdate();

            bool exists = false;

            foreach (TreeNode tn in treeView1.Nodes)
            {
                Console.Out.WriteLine(tn.Text);
                if (tn.Text == currentTag)
                {
                    tn.Nodes.Clear();
                    tn.Nodes.Add(rfidDescr);
                    exists = true;
                }
            }
            if (!exists)
            {
                treeView1.Nodes.Add(currentTag).Nodes.Add(rfidDescr);
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
        protected static extern int getVarioSensLog(writeViolationsCB cb);

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
                    throw ee;
                }
            }
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void readLogClick(object sender, EventArgs e)
        {
            if (readerRunning == false)
            {
                readerRunning = true;
                new Thread(new ThreadStart(readLog)).Start();
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
                else if ((errorCode == -1) || (errorCode == -2))
                    readerRunning = false;
                Thread.Sleep(100);
            }

            switch (errorCode)
            {
                case -1:
                    MessageBox.Show(GenTag_Demo.Properties.Resources.error1);
                    break;
                case -2:
                    MessageBox.Show(GenTag_Demo.Properties.Resources.error2);
                    break;
                //case -3:
                //    mF.textBox9.Text = GenTag_Demo.Properties.Resources.error3;
                //    break;
                //case -4:
                //    mF.textBox9.Text = GenTag_Demo.Properties.Resources.error4;
                //    break;
                //case -5:
                //    mF.textBox9.Text = GenTag_Demo.Properties.Resources.error5;
                //    break;
                //case -6:
                //    mF.textBox9.Text = GenTag_Demo.Properties.Resources.error6;
                //    break;
                //case -7:
                //    mF.textBox9.Text = GenTag_Demo.Properties.Resources.error7;
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
                readLogButton.Text = GenTag_Demo.Properties.Resources.readLogButtonInit;
            }
            else if (obj == setValueButton)
            {
                setValueButton.Text = GenTag_Demo.Properties.Resources.setValueButtonInit;
            }
            else if (obj == readValueButton)
            {
                setValueButton.Text = GenTag_Demo.Properties.Resources.readValueButtonInit;
            }
            else if (obj == setValueButton)
            {
                readIDButton.Text = GenTag_Demo.Properties.Resources.readIDButtonInit;
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
        public static extern int setVarioSensSettings(float lowTemp, float hiTemp, int interval, int mode, int batteryCheckInterval);

        // set
        private void setVSSettingsClick(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;

            try
            {
                int mode;

                if (comboBox1.SelectedIndex == 0)
                    mode = 1;
                else
                    mode = 2;

                int eC = -1;
                for (int i = 0; i < 8; i++)
                {
                    if ((eC = setVarioSensSettings(float.Parse(textBox7.Text), float.Parse(textBox6.Text), int.Parse(textBox5.Text), mode, int.Parse(textBox8.Text))) == 0)
                        break;
                }

                if (eC != 0)
                    MessageBox.Show("Error");
            }
            catch (Exception ex)
            {
                throw ex;
            }

            Cursor.Current = Cursors.Default;
        }

        public delegate void writeVSSettingsCB(
            Single upperTempLimit,
            Single lowerTempLimit,
            short recordPeriod,
            short logMode,
            short batteryCheckInterval);

        [DllImport("VarioSens Lib.dll")]
        public static extern int getVarioSensSettings(writeVSSettingsCB cb);

        private void getCallback(Single upper, Single lower, short period, short logMode, short batteryCheckInterval)
        {
            mF.textBox7.Text = lower.ToString();
            mF.textBox6.Text = upper.ToString();
            mF.textBox5.Text = period.ToString();
            if (logMode == 1)
            {
                mF.comboBox1.SelectedIndex = 0;
            }
            else if (logMode == 2)
            {
                mF.comboBox1.SelectedIndex = 1;
            }
            mF.textBox8.Text = batteryCheckInterval.ToString();
        }

        // get
        private void button5_Click(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;

            writeVSSettingsCB mycb = new writeVSSettingsCB(getCallback);

            int errorCode = -1;

            for (int i = 0; i < 8; i++)
            {
                if ((errorCode = getVarioSensSettings(mycb)) == 0)
                    break;
            }

            if (errorCode != 0)
                MessageBox.Show("Error reading tag");

            Cursor.Current = Cursors.Default;
        }

        private void readPatientButton_Click(object sender, EventArgs e)
        {
            if (readerRunning == false)
            {
                readerRunning = true;
                new Thread(new ThreadStart(readPatientData)).Start();
            }
            else
            {
                readerRunning = false;
            }
        }

        private string patientID;

        private void readPatientData()
        {
            try
            {
                Cursor.Current = Cursors.WaitCursor;
                patientID = readTagID();
                org.dyndns.crius.GetDatesWS ws = new org.dyndns.crius.GetDatesWS();
                org.dyndns.crius.GetDates values = ws.getInfo(patientID, false);
                setTextBox(patientNameBox, values.name);
                setTextBox(patientDescriptionBox, values.desc);

                byte[] bA = ws.getPicture(patientID, false);
                setPhoto(patientPhoto, bA);
            }
            catch (WebException ex)
            {

            }
            finally
            {
                readerRunning = false;
                Cursor.Current = Cursors.Default;
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

        private void medicationButton_Click(object sender, EventArgs e)
        {
            if (readerRunning == false)
            {
                readerRunning = true;
                new Thread(new ThreadStart(readDrugData)).Start();
            }
            else
            {
                readerRunning = false;
            }
        }

        private string drugID;

        private void readDrugData()
        {
            Cursor.Current = Cursors.WaitCursor;
            drugID = readTagID();
            try
            {
                org.dyndns.crius.GetDatesWS ws = new org.dyndns.crius.GetDatesWS();
                org.dyndns.crius.GetDates values = ws.getInfo(drugID, true);
                byte[] bA = ws.getPicture(drugID, true);
                bool drugInteraction = ws.checkInteraction(patientID, drugID);

                //setPatientData(values.name, values.desc);
                setPhoto(drugPhoto, bA);

                if (drugInteraction == true)
                    MessageBox.Show("Warning, interaction between this patient and this drug");
            }
            catch (WebException ex)
            {                
                MessageBox.Show("Problem connecting to web service: " + ex.Message);
            }

            readerRunning = false;
            Cursor.Current = Cursors.Default;
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
            }
            catch (ArgumentException ex)
            {

            }
        }
    }
}