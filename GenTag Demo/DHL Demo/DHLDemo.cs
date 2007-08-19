using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.CompilerServices;
using SiritReader;
using System.Globalization;
using System.IO;
using C1Lib;
using System.Net;
using Microsoft.WindowsCE.Forms;

namespace DHL_Demo
{
    public partial class dhlDemoForm : Form
    {
        nmeaInterpreter gpsInterpreter;

        string DeviceUID;

        Checkpoint checkpointDialog = new Checkpoint();

        public dhlDemoForm()
        {
            InitializeComponent();

            // generate the device's UID
            string AppString = DHL_Demo.Properties.Resources.titleString;

            byte[] AppData = new byte[AppString.Length];

            for (int count = 0; count < AppString.Length; count++)
                AppData[count] = (byte)AppString[count];

            int appDataSize = AppData.Length;

            byte[] dUID = new byte[20];

            uint SizeOut = 20;

            NativeMethods.GetDeviceUniqueID(AppData, appDataSize, 1, dUID, out SizeOut);

            for (int i = 0; i < 20; i++)
                DeviceUID += dUID[i].ToString("X", CultureInfo.CurrentUICulture);

            gpsInterpreter = new nmeaInterpreter(new CultureInfo("en-US"), DeviceUID);

            // setup event callbacks for nmea events
            gpsInterpreter.DateTimeChanged += new nmeaInterpreter.DateTimeChangedEventHandler(gpsNmea_DateTimeChanged);
            gpsInterpreter.FixObtained += new nmeaInterpreter.FixObtainedEventHandler(gpsNmea_FixObtained);
            gpsInterpreter.FixLost += new nmeaInterpreter.FixLostEventHandler(gpsNmea_FixLost);
            gpsInterpreter.PositionReceived += new nmeaInterpreter.PositionReceivedEventHandler(gpsNmea_PositionReceived);
            gpsInterpreter.SpeedReceived += new nmeaInterpreter.SpeedReceivedEventHandler(gpsNmea_SpeedReceived);
            gpsInterpreter.BearingReceived += new nmeaInterpreter.BearingReceivedEventHandler(gpsNmea_BearingReceived);
            gpsInterpreter.SatelliteReceived += new nmeaInterpreter.SatelliteReceivedEventHandler(gpsNmea_SatelliteReceived);
            gpsInterpreter.PDOPReceived += new nmeaInterpreter.PDOPReceivedEventHandler(gpsNmea_PDOPReceived);
            gpsInterpreter.HDOPReceived += new nmeaInterpreter.HDOPReceivedEventHandler(gpsNmea_HDOPReceived);
            gpsInterpreter.VDOPReceived += new nmeaInterpreter.VDOPReceivedEventHandler(gpsNmea_VDOPReceived);
            gpsInterpreter.NumSatsReceived += new nmeaInterpreter.NumberOfSatellitesInViewEventHandler(gpsNmea_NumSatsReceived);
            gpsInterpreter.QueueUpdated += new nmeaInterpreter.SetQueuedRequestsEventHandler(pendingQueueUpdate);
        }      

        #region GPS Event Handlers

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
            else
            {
                comPortsComboBox.SelectedIndex = comPortsComboBox.Items.IndexOf("COM7");
            }
        }



        [MethodImpl(MethodImplOptions.Synchronized)]
        private void connectGPSButton_Click(object sender, EventArgs e)
        {
            string errorMsg = null;
            for (int triesLeft = 5; triesLeft > 0; )
            {
                try
                {
                    if (gpsInterpreter.IsOpen())
                    {
                        gpsInterpreter.Close();
                        setPanel(panel1, Color.Red);
                        setPanel(panel2, Color.Red);
                        connectGPSButton.Text = "Connect";
                    }
                    else
                    {
                        gpsInterpreter.Open(comPortsComboBox.SelectedItem.ToString());
                        connectGPSButton.Text = "Disconnect";
                    }
                    errorMsg = null;
                    break;
                }
                catch (Exception ex)
                {
                    errorMsg = ex.Message;
                    triesLeft--;
                }
            }
            if (!string.IsNullOrEmpty(errorMsg))
                MessageBox.Show(errorMsg);
        }



        private void pendingQueueUpdate(int queueSize)
        {
            //setProgressBar(queueSizeBar, queueSize);
        }

        private void gpsNmea_DateTimeChanged(DateTime dT)
        {
            setTextBox(timeTextBox, dT.ToString("t", CultureInfo.CurrentUICulture));
        }
        private void gpsNmea_FixObtained()
        {
            setTextBox(statusTextBox, @"Fixed");
            setPanel(panel1, Color.Green);
            setPanel(panel2, Color.Green);
        }
        private void gpsNmea_FixLost()
        {
            setTextBox(statusTextBox, @"Searching");
            setPanel(panel1, Color.Yellow);
            setPanel(panel2, Color.Yellow);
        }

        private void gpsNmea_PositionReceived(string latitude, string longitude)
        {
            setTextBox(latitudeTextBox, latitude);
            setTextBox(longitudeTextBox, longitude);
        }
        private void gpsNmea_SpeedReceived(double speed)
        {
            setTextBox(speedTextBox, speed.ToString(CultureInfo.CurrentUICulture));
        }
        private void gpsNmea_BearingReceived(double bearing)
        {
            setTextBox(directionTextBox, bearing.ToString(CultureInfo.CurrentUICulture));
        }

        private int satNumber;
        private void gpsNmea_SatelliteReceived(int pseudoRandomCode, int azimuth, int elevation, int signalToNoiseRatio, bool firstMessage)
        {
            if (firstMessage)
            {
                satNumber = 0;
                setLabel(satLabel1, "");
                setLabel(satLabel2, "");
                setLabel(satLabel3, "");
                setLabel(satLabel4, "");
                setLabel(satLabel5, "");
                setLabel(satLabel6, "");
                setLabel(satLabel7, "");
                setLabel(satLabel8, "");
                setProgressBar(progressBar1, 0);
                setProgressBar(progressBar2, 0);
                setProgressBar(progressBar3, 0);
                setProgressBar(progressBar4, 0);
                setProgressBar(progressBar5, 0);
                setProgressBar(progressBar6, 0);
                setProgressBar(progressBar7, 0);
                setProgressBar(progressBar8, 0);
            }
            switch (satNumber)
            {
                case 0:
                    setLabel(satLabel1, pseudoRandomCode.ToString(CultureInfo.CurrentUICulture));
                    setProgressBar(progressBar1, signalToNoiseRatio);
                    break;
                case 1:
                    setLabel(satLabel2, pseudoRandomCode.ToString(CultureInfo.CurrentUICulture));
                    setProgressBar(progressBar2, signalToNoiseRatio);
                    break;
                case 2:
                    setLabel(satLabel3, pseudoRandomCode.ToString(CultureInfo.CurrentUICulture));
                    setProgressBar(progressBar3, signalToNoiseRatio);
                    break;
                case 3:
                    setLabel(satLabel4, pseudoRandomCode.ToString(CultureInfo.CurrentUICulture));
                    setProgressBar(progressBar4, signalToNoiseRatio);
                    break;
                case 4:
                    setLabel(satLabel5, pseudoRandomCode.ToString(CultureInfo.CurrentUICulture));
                    setProgressBar(progressBar5, signalToNoiseRatio);
                    break;
                case 5:
                    setLabel(satLabel6, pseudoRandomCode.ToString(CultureInfo.CurrentUICulture));
                    setProgressBar(progressBar6, signalToNoiseRatio);
                    break;
                case 6:
                    setLabel(satLabel7, pseudoRandomCode.ToString(CultureInfo.CurrentUICulture));
                    setProgressBar(progressBar7, signalToNoiseRatio);
                    break;
                case 7:
                    setLabel(satLabel8, pseudoRandomCode.ToString(CultureInfo.CurrentUICulture));
                    setProgressBar(progressBar8, signalToNoiseRatio);
                    break;
            }
            satNumber = (satNumber + 1) % 8;
        }

        private void gpsNmea_PDOPReceived(double value)
        {

        }

        private void gpsNmea_HDOPReceived(double value)
        {
            setTextBox(hdopTextBox, value.ToString(CultureInfo.CurrentUICulture));
        }

        private void gpsNmea_VDOPReceived(double value)
        {
            setTextBox(vdopTextBox, value.ToString(CultureInfo.CurrentUICulture));
        }

        private void gpsNmea_NumSatsReceived(int value)
        {
            setTextBox(satellitesUsedTextBox, value.ToString(CultureInfo.CurrentUICulture));
        }

        #endregion

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

        private delegate Color getPanelDelegate(Panel p);

        private Color getPanel(Panel p)
        {
            if (this.InvokeRequired)
            {
                return (Color)this.Invoke(new getPanelDelegate(getPanel), new object[] { p });
            }
            return p.BackColor;
        }

        private delegate Color getButtonColorDelegate(Button p);

        private Color getButtonColor(Button p)
        {
            if (this.InvokeRequired)
            {
                return (Color)this.Invoke(new getButtonColorDelegate(getButtonColor), new object[] { p });
            }
            return p.BackColor;
        }

        private delegate void setPanelDelegate(Panel p, Color c);

        private void setPanel(Panel p, Color c)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setPanelDelegate(setPanel), new object[] { p, c });
                return;
            }
            p.BackColor = c;
        }

        private delegate void setButtonColorDelegate(Button p, Color c);

        private void setButtonColor(Button p, Color c)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setButtonColorDelegate(setButtonColor), new object[] { p, c });
                return;
            }
            p.BackColor = c;
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

        private delegate bool getCheckBoxDelegate(CheckBox cB);

        private bool getCheckBox(CheckBox cB)
        {
            if (this.InvokeRequired)
            {
                return (bool)this.Invoke(new getCheckBoxDelegate(getCheckBox), new object[] { cB });
            }
            return cB.Checked;
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
            catch (ArgumentOutOfRangeException)
            {
                cB.SelectedIndex = 0;
            }
        }

        private delegate void setLabelDelegate(Label tB, string desc);

        private void setLabel(Label tB, string val)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setLabelDelegate(setLabel), new object[] { tB, val });
                return;
            }
            tB.Text = val;
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

        private delegate void setProgressBarDelegate(ProgressBar pB, int val);

        private void setProgressBar(ProgressBar pB, int val)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setProgressBarDelegate(setProgressBar), new object[] { pB, val });
                return;
            }
            if (val > pB.Maximum)
                pB.Value = pB.Maximum;
            else if (val < pB.Minimum)
                pB.Value = pB.Minimum;
            else
                pB.Value = val;
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

        private delegate void setPhotoDelegateB(PictureBox pB, Image bA);

        private void setPhoto(PictureBox pB, Image bA)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setPhotoDelegateB(setPhoto), new object[] { pB, bA });
                return;
            }
            try
            {
                pB.Image = bA;
                pB.Refresh();
            }
            catch (ArgumentException)
            {

            }
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
            catch (ArgumentException)
            {

            }
        }
        #endregion


        // submit
        private void pictureBox1_Click(object sender, EventArgs e)
        {
            setWaitCursor(true);
            try
            {
                if (getPanel(panel1) != Color.Green)
                {
                    setWaitCursor(false);
                    MessageBox.Show("GPS fix not acquired");
                    return;
                }
                
            //    org.dyndns.crius.GetDatesWS ws = new org.dyndns.crius.GetDatesWS();
            //    ws.Timeout = 30000;
            //    org.dyndns.crius.patientInfo values = ws.getPatientInfo(patientID);
            //    setTextBox(patientNameBox, values.name);
            //    setTextBox(patientDescriptionBox, values.description);
            //    setPhoto(patientPhoto, values.image);
            }
            catch (NotSupportedException ex)
            {
                MessageBox.Show(ex.Message);
            }
            catch (IOException ex)
            {
                MessageBox.Show(ex.Message);
            }
            catch (WebException ex)
            {
                MessageBox.Show("Problem connecting to web service: " + ex.Message);
            }
            finally
            {
                setWaitCursor(false);
            }
        }

        string newPackageID;

        private void getRFIDButton_Click(object sender, EventArgs e)
        {
            setWaitCursor(true);
            try
            {
                rfidTB.Text = newPackageID = NativeMethods.readOneTagID();
            }
            catch (NotSupportedException ex)
            {
                MessageBox.Show(ex.Message);
            }
            catch (IOException ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                setWaitCursor(false);
            }
        }

        private void genTrackingNoButton_Click(object sender, EventArgs e)
        {
            Random rN = new Random();
            trackingNoTB.Text = "";
            for (int i = 0; i < 12; i++)
                trackingNoTB.Text += rN.Next(10);
        }

        private void dhlDemoForm_KeyDown(object sender, KeyEventArgs e)
        {
            if ((e.KeyCode == System.Windows.Forms.Keys.Up))
            {
                // Up
            }
            if ((e.KeyCode == System.Windows.Forms.Keys.Down))
            {
                // Down
            }
            if ((e.KeyCode == System.Windows.Forms.Keys.Left))
            {
                // Left
            }
            if ((e.KeyCode == System.Windows.Forms.Keys.Right))
            {
                // Right
            }
            if ((e.KeyCode == System.Windows.Forms.Keys.Enter))
            {
                // Enter
            }
            if ((e.KeyCode == System.Windows.Forms.Keys.F1))
            {
                // do scan on scan page
                tabControl1.SelectedIndex = 1;
                string RFIDNum = NativeMethods.readOneTagID();
                // go grab the info on this package

                // set the info for it
            }

        }

        private void pictureBox2_Click(object sender, EventArgs e)
        {
            dhlDemoForm_KeyDown(this, new KeyEventArgs(System.Windows.Forms.Keys.F1));
        }
    }
}