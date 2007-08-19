using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Microsoft.WindowsMobile.Forms;
using System.IO;
using System.Net;
using SiritReader;
using System.Collections;
using System.Net.Sockets;
using System.Threading;

namespace COREMobileMedDemo
{
    public partial class mobileMedDemo : Form
    {
        NativeMethods nativeMethods = new NativeMethods();

        public mobileMedDemo()
        {
            InitializeComponent();

            nativeMethods.VarioSensLogReceived += new NativeMethods.VarioSensReadLogHandler(VarioSensLogReceived);

            selfReference = this;

            patientNameLabel.Text = "";
            DOBLabel.Text = "";

        }

        medEntryForm meF = new medEntryForm();

        byte[] image;

        private string RFIDnum = "";

        static mobileMedDemo selfReference;

        private void menuItem1_Click(object sender, EventArgs e)
        {
            CameraCaptureDialog newImage = new CameraCaptureDialog();
            newImage.StillQuality = CameraCaptureStillQuality.High;
            newImage.Resolution = new Size(320, 240);
            newImage.Owner = this;
            newImage.Title = "Patient Photo";
            newImage.Mode = CameraCaptureMode.Still;

            if (newImage.ShowDialog() == DialogResult.OK && newImage.FileName.Length > 0)
            {
                FileStream fileStream = new FileStream(newImage.FileName, FileMode.Open, FileAccess.Read);

                image = new byte[fileStream.Length];

                fileStream.Read(image, 0, System.Convert.ToInt32(fileStream.Length));

                fileStream.Close();

                pictureBox1.Image = new Bitmap(newImage.FileName);

                System.IO.File.Delete(newImage.FileName);
            }
        }

        private void menuItem2_Click(object sender, EventArgs e)
        {
            if (image == null || string.IsNullOrEmpty(RFIDnum))
                MessageBox.Show("Must have both image and bracelet ID first");
            else
            {
                try
                {
                    // time to submit the rfid number and image to the web service
                    COREMedDemoWS.patientRecord pR = new COREMedDemoWS.patientRecord();

                    pR.image = image;
                    pR.RFIDnum = RFIDnum;
                    COREMedDemoWS.COREMedDemoWS ws = new COREMedDemoWS.COREMedDemoWS();
                    COREMedDemoWS.errorReport eR = ws.enterPatientPhoto(pR);
                    if (eR.errorCode != 0)
                        MessageBox.Show(eR.errorMessage);
                }
                catch (WebException ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
        }

        // the periodicity of sampling temperatures, will be written to the tag's settings and to the web service
        private static int periodicity = 60;

        private void button1_Click(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;
            patientVitalsListView.Items.Clear();
            // only want VS tags so we can grab temp data
            RFIDnum = textBox1.Text = NativeMethods.readOneVSTagID();
            if (RFIDnum.Length > 0)
            {
                int i = 50;
                for (; i >= 0; --i)
                {
                    if (nativeMethods.setVSSettings(1, 30.0f, 29.0f, periodicity, 360) == 0)
                        break;
                }
                if (i < 0)
                    MessageBox.Show("Please rescan the tag.", "Could not reset temp logging");
            }
            Cursor.Current = Cursors.Default;
        }

        private void readPatientTagClick(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;
            // get the tag id
            RFIDnum = NativeMethods.readOneVSTagID();
            if (RFIDnum.Length > 0)
            {
                // read the log
                nativeMethods.readLog();
                // then reset it
                int i = 50;
                for (; i >= 0; --i)
                {
                    if (nativeMethods.setVSSettings(1, 30.0f, 29.0f, periodicity, 360) == 0)
                        break;
                }
                // then do patient lookup
                AsyncCallback cb = new AsyncCallback(patientIDLookupFinished);
                COREMedDemoWS.COREMedDemoWS ws = new COREMedDemoWS.COREMedDemoWS();
                ws.Timeout = 300000;
                IAsyncResult ar = ws.BegingetPatientRecord(RFIDnum, cb, ws);

                currentlyBeingRetrievedPatientRecords[ar] = (string)RFIDnum.Clone();
                if (i < 0)
                    MessageBox.Show("Please rescan the tag.", "Could not reset temp logging");
            }
            Cursor.Current = Cursors.Default;
        }

        Hashtable currentlyBeingRecordedVitals = new Hashtable();

        Hashtable currentlyBeingRetrievedPatientRecords = new Hashtable();

        private static void VarioSensLogReceived(
            Single upperTempLimit,
            Single lowerTempLimit,
            Int32 len,
            short recordPeriod,
            int[] dateTime,
            Byte[] logMode,
            Single[] temperatures)
        {
            DateTime now = DateTime.Now;

            for (int i = 0; i < dateTime.Length; i++)
            {
                if (logMode[i] == 1)
                {
                    now = now - new TimeSpan(0, 0, periodicity);
                    float currentTemp = 9 / 5 * (temperatures[i]) + 32;
                    ListViewItem lvi = new ListViewItem(new string[] { now.ToShortTimeString(), currentTemp.ToString("0.0") });
                    if (currentTemp > 100)
                        lvi.BackColor = Color.Red;
                    selfReference.patientVitalsListView.Items.Add(lvi);

                }
                // violation mode
                else if (logMode[i] == 2)
                {
                    return;

                    //UInt32 time_t = Convert.ToUInt32(dateTime[i]);

                    //DateTime convertedValue = origin + new TimeSpan(time_t * TimeSpan.TicksPerSecond);
                    //if (System.TimeZone.CurrentTimeZone.IsDaylightSavingTime(convertedValue) == true)
                    //{
                    //    System.Globalization.DaylightTime daylightTime = System.TimeZone.CurrentTimeZone.GetDaylightChanges(convertedValue.Year);
                    //    convertedValue = convertedValue + daylightTime.Delta;
                    //}
                    //mF.listBox1.Items.Add(temperatures[i].ToString("F", CultureInfo.CurrentCulture) + " C" + convertedValue.ToString());

                }
            }
            // then record these to the db
            AsyncCallback cb = new AsyncCallback(selfReference.vitalsRecordedConfirmation);
            COREMedDemoWS.COREMedDemoWS ws = new COREMedDemoWS.COREMedDemoWS();
            ws.Timeout = 300000;
            IAsyncResult ar = ws.BeginlogPatientVitals(selfReference.RFIDnum, temperatures, periodicity, cb, ws);
            patientVitals pV = new patientVitals(selfReference.RFIDnum, temperatures);
            selfReference.currentlyBeingRecordedVitals[ar] = pV;
        }

        COREMedDemoWS.patientRecord currentPatient;

        private void patientIDLookupFinished(IAsyncResult ar)
        {
            try
            {
                COREMedDemoWS.COREMedDemoWS ws = (COREMedDemoWS.COREMedDemoWS)ar.AsyncState;
                COREMedDemoWS.patientRecord pR = currentPatient = ws.EndgetPatientRecord(ar);
                if (pR.exists)
                {
                    setPhoto(patientPhotoPB, pR.image);
                    setLabel(patientNameLabel, pR.lastName + ", " + pR.firstName + " " + pR.middleName);
                    setLabel(DOBLabel, (new DateTime(pR.DOB) - new DateTime(1970, 1, 1)).ToString());
                }
            }
            catch (WebException)
            {
                rescheduleLookup(ar, currentlyBeingRetrievedPatientRecords);
            }
            catch (ArgumentNullException)
            {
                rescheduleLookup(ar, currentlyBeingRetrievedPatientRecords);
            }
            catch (MemberAccessException)
            {
                MessageBox.Show("unexpected: ");
            }
            catch (SocketException)
            {
                Thread.Sleep(2000);
                rescheduleLookup(ar, currentlyBeingRetrievedPatientRecords);
            }
            catch (IOException)
            {
                //MessageBox.Show(ex.Message);
                return;
            }
            catch (InvalidCastException e)
            {
                MessageBox.Show(e.ToString());
            }
            catch (InvalidOperationException)
            {
                MessageBox.Show("Possible timeout error");
            }

            catch (IndexOutOfRangeException e)
            {
                MessageBox.Show(e.ToString());
            }
            catch (NullReferenceException e)
            {
                MessageBox.Show(e.ToString());
            }
        }

        private void vitalsRecordedConfirmation(IAsyncResult ar)
        {
            try
            {
                COREMedDemoWS.COREMedDemoWS ws = (COREMedDemoWS.COREMedDemoWS)ar.AsyncState;
                COREMedDemoWS.errorReport eR = ws.EndlogPatientVitals(ar);
                if (eR.errorCode != 0)
                    MessageBox.Show(eR.errorMessage);
            }
            catch (WebException)
            {
                rescheduleLookup(ar, currentlyBeingRecordedVitals);
            }
            catch (ArgumentNullException)
            {
                rescheduleLookup(ar, currentlyBeingRecordedVitals);
            }
            catch (MemberAccessException)
            {
                MessageBox.Show("unexpected: ");
            }
            catch (SocketException)
            {
                Thread.Sleep(2000);
                rescheduleLookup(ar, currentlyBeingRecordedVitals);
            }
            catch (IOException)
            {
                //MessageBox.Show(ex.Message);
                return;
            }
            catch (InvalidCastException e)
            {
                MessageBox.Show(e.ToString());
            }
            catch (InvalidOperationException)
            {
                MessageBox.Show("Possible timeout error");
            }

            catch (IndexOutOfRangeException e)
            {
                MessageBox.Show(e.ToString());
            }
            catch (NullReferenceException e)
            {
                MessageBox.Show(e.ToString());
            }
        }

        private void rescheduleLookup(IAsyncResult ar, Hashtable lookupTable)
        {
            if (lookupTable == currentlyBeingRetrievedPatientRecords)
            {
                AsyncCallback cb = new AsyncCallback(patientIDLookupFinished);
                string currentID;
                lock (currentlyBeingRetrievedPatientRecords)
                {
                    currentID = (string)currentlyBeingRetrievedPatientRecords[ar];
                    currentlyBeingRetrievedPatientRecords.Remove(ar);
                }
                COREMedDemoWS.COREMedDemoWS ws = new COREMedDemoWS.COREMedDemoWS();
                ws.Timeout = 300000;
                IAsyncResult handle = ws.BegingetPatientRecord(RFIDnum, cb, ws);

                lock (currentlyBeingRetrievedPatientRecords.SyncRoot)
                {
                    currentlyBeingRetrievedPatientRecords[handle] = currentID;
                    currentlyBeingRetrievedPatientRecords.Remove(ar);
                }
            }
            else if (lookupTable == currentlyBeingRecordedVitals)
            {
                AsyncCallback cb = new AsyncCallback(vitalsRecordedConfirmation);
                patientVitals pV = (patientVitals)currentlyBeingRecordedVitals[ar];

                COREMedDemoWS.COREMedDemoWS ws = new COREMedDemoWS.COREMedDemoWS();

                ws.Timeout = 300000;

                IAsyncResult handle = ws.BeginlogPatientVitals(pV.RFID, pV.temps, periodicity, cb, ws);

                lock (currentlyBeingRecordedVitals.SyncRoot)
                {
                    currentlyBeingRecordedVitals[handle] = pV;
                    currentlyBeingRecordedVitals.Remove(ar);
                }
            }
        }

        string MedRFIDNum;

        private void readMedIDClick(object sender, EventArgs e)
        {
            setWaitCursor(true);
            // get the tag id
            MedRFIDNum = NativeMethods.readOneTagID();
            setWaitCursor(false);
            setWaitCursor(true);
            try
            {
                if (MedRFIDNum.Length > 0)
                {
                    // then do patient lookup
                    COREMedDemoWS.COREMedDemoWS ws = new COREMedDemoWS.COREMedDemoWS();
                    ws.Timeout = 300000;

                    COREMedDemoWS.drugInfo dI = ws.getDrugInfo(MedRFIDNum); // catch webexception

                    if (!dI.exists)
                    {
                        MessageBox.Show("Not a known drug");
                        MedRFIDNum = "";
                        return;
                    }
                    setPhoto(medPB, dI.picture);


                    if (ws.checkInteraction(RFIDnum, MedRFIDNum))
                        MessageBox.Show("This medication is contraindicated for " + currentPatient.firstName + " " + currentPatient.lastName);
                    else
                    {
                        setLabel(meF.medicationName, dI.name);
                        setLabel(meF.patientName, currentPatient.lastName + ", " + currentPatient.firstName + " " + currentPatient.middleName);
                        setPhoto(meF.medicinePicture, dI.picture);
                        if (meF.ShowDialog() == DialogResult.OK)
                        {
                            // then submit dosage to db
                            if (!ws.registerDoseGiven(RFIDnum, MedRFIDNum, "000", decimal.ToInt32(meF.dosageValue)))
                                MessageBox.Show("This dose has been disallowed");
                        }
                    }

                }
            }
            catch (WebException ex)
            {
                MessageBox.Show("lookup error, please try again.");
            }
            setWaitCursor(false);
        }
    }
}