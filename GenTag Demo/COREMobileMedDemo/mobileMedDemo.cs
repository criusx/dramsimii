using System;
using System.Collections;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Web.Services.Protocols;
using System.Windows.Forms;
using Microsoft.WindowsCE.Forms;
using Microsoft.WindowsMobile.Forms;
using RFIDReader;

namespace COREMobileMedDemo
{
    public partial class mobileMedDemo : Form
    {
        #region Enumerated Types
        private enum loopType { submitPhoto, submitPatientTemps, patientLookup, drugCheck, drugLookup, submitDose };
        #endregion

        #region Private Classes
        private class lookupInfo
        {
            public string tagID;
            public loopType whichLookup;
            public string extraTagValue;
            public byte[] extraArray;
            public float[] tempArray;
            public float dose;

            public lookupInfo(string tagValue, loopType lookupValue)
            {
                tagID = tagValue;
                whichLookup = lookupValue;
            }

            public lookupInfo(string tagValue, loopType lookupValue, string extraValue)
            {
                tagID = tagValue;
                whichLookup = lookupValue;
                extraTagValue = extraValue;
            }

            public lookupInfo(string tagValue, loopType lookupValue, byte[] image)
            {
                tagID = tagValue;
                whichLookup = lookupValue;
                extraArray = image;
            }

            /// <summary>
            /// Constructor for submitting a patient's temperatures
            /// </summary>
            /// <param name="tagValue"></param>
            /// <param name="lookupValue"></param>
            /// <param name="temperatures"></param>
            public lookupInfo(string tagValue, loopType lookupValue, float[] temperatures)
            {
                tagID = tagValue;
                whichLookup = lookupValue;
                tempArray = temperatures;
            }

            public lookupInfo(string tagValue, loopType lookupValue, string extraValue, byte[] array, float medDose)
            {
                tagID = tagValue;
                whichLookup = lookupValue;
                extraTagValue = extraValue;
                extraArray = array;
                dose = medDose;
            }

            public override bool Equals(object obj)
            {
                if (!(obj.GetType() == typeof(lookupInfo)))
                    return false;
                else
                    return this == (lookupInfo)obj;
            }

            public override int GetHashCode()
            {
                int result = 0;
                for (int i = 0; i < tagID.Length; ++i)
                    result ^= tagID[i];
                if (extraTagValue != null)
                    for (int j = 0; j < extraTagValue.Length; ++j)
                        result ^= extraTagValue[j];

                return result;
            }

            public static bool operator ==(lookupInfo inf1, lookupInfo inf2)
            {
                try
                {
                    if ((inf1.extraTagValue == null && inf2.extraTagValue != null) ||
                    (inf1.extraTagValue != null && inf2.extraTagValue == null))
                        return false;
                    else if (inf1.extraTagValue == null && inf2.extraTagValue == null)
                        return inf1.tagID.Equals(inf2.tagID, StringComparison.OrdinalIgnoreCase) &&
                        inf1.whichLookup == inf2.whichLookup;
                    else
                        return inf1.tagID.Equals(inf2.tagID, StringComparison.OrdinalIgnoreCase) &&
                            inf1.extraTagValue.Equals(inf2.extraTagValue, StringComparison.OrdinalIgnoreCase) &&
                            (inf1.whichLookup == inf2.whichLookup);
                }
                catch (NullReferenceException)
                { return false; }
            }

            public static bool operator !=(lookupInfo inf1, lookupInfo inf2)
            {
                try
                {
                    return !inf1.tagID.Equals(inf2.tagID, StringComparison.OrdinalIgnoreCase) ||
                    !inf1.extraTagValue.Equals(inf2.extraTagValue, StringComparison.OrdinalIgnoreCase) ||
                    (inf1.whichLookup != inf2.whichLookup);
                }
                catch (NullReferenceException)
                { return false; }
            }
        }
        #endregion

        #region Members
        /// <summary>
        /// The unique ID for this handset/PDA/Laptop, is based on the hardware configuration
        /// </summary>
        private string DeviceUID;

        /// <summary>
        /// The popup notification that tells users of important inforation
        /// Used to tell the user of updated software versions, patient interactions, reader errors, etc.
        /// </summary>
        Notification popupNotification;

        /// <summary>
        /// The endpoint for the web service
        /// </summary>
        COREMedDemoWS.COREMedDemoWS ws = new COREMedDemoWS.COREMedDemoWS();

        /// <summary>
        /// Interface to the tag reader class
        /// </summary>
        Reader tagReader = new Reader();

        /// <summary>
        /// The dialog box that allows staff to give specified amounts of medication
        /// to the patient
        /// </summary>
        medEntryForm medicineEntryForm = new medEntryForm();

        /// <summary>
        /// A byte array of the patient record being entered
        /// </summary>
        byte[] newPatientImage;

        /// <summary>
        /// The RFID number of the patient whose photo is being entered
        /// </summary>
        string newPatientRFIDNum = string.Empty;

        /// <summary>
        /// A reference to the single instance of this class, for static functions
        /// </summary>
        static mobileMedDemo selfReference;

        /// <summary>
        /// the periodicity of sampling temperatures, will be written to the tag's settings and to the web service
        /// </summary> 
        static int periodicity = 60;

        /// <summary>
        /// Information about the current patient
        /// </summary>
        COREMedDemoWS.patientRecord currentPatient;

        /// <summary>
        /// The RFID number of the current medication
        /// </summary>
        string medicationRFIDNum = string.Empty;

        // time to submit the rfid number and image to the web service
        COREMedDemoWS.patientRecord patientRecord = new COREMedDemoWS.patientRecord();

        /// <summary>
        /// Dialog box that lets the user take a picture
        /// </summary>
        CameraCaptureDialog cameraCaptureDialogBox = new CameraCaptureDialog();

        /// <summary>
        /// Callback for when web services complete
        /// </summary>
        AsyncCallback lookupCallback;

        /// <summary>
        /// List of handles of in-flight web services
        /// </summary>
        Hashtable itemsCurrentlyBeingLookedUp = new Hashtable();

        /// <summary>
        /// The output file for debug output, prints diagnostic messages
        /// </summary>
        TextWriter debugOut;
        #endregion

        /// <summary>
        /// Initializes variables, sets up the GUI components
        /// </summary>
        public mobileMedDemo()
        {
            InitializeComponent();

            lookupCallback = new AsyncCallback(receiveResult);

            selfReference = this;

            patientNameLabel.Text = string.Empty;

            DOBLabel.Text = string.Empty;

            // set the version
            versionLabel.Text = Properties.Resources.Version + " " + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString() + " " + Properties.Resources.Alpha;

            try
            {
                DeviceUID = Reader.getDeviceUniqueID(COREMobileMedDemo.Properties.Resources.titleString);
            }
            catch (MissingMethodException)
            {
                DeviceUID = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
            }

            // initialize the debug stream
            for (int i = 500; i >= 0; i--)
            {
                try
                {
                    debugOut = new StreamWriter("debug" + (i - 500).ToString(CultureInfo.InvariantCulture) + ".txt", false);
                    break;
                }
                catch (IOException)
                {
                }
                if (i == 0)
                {
                    MessageBox.Show("Cannot write debug file, quitting.");
                    this.Close();
                }
            }

            cameraCaptureDialogBox.StillQuality = CameraCaptureStillQuality.High;
            cameraCaptureDialogBox.Resolution = new Size(320, 240);
            cameraCaptureDialogBox.Owner = this;
            cameraCaptureDialogBox.Title = "Patient Photo";
            cameraCaptureDialogBox.Mode = CameraCaptureMode.Still;
        }

        /// <summary>
        /// Called when an asynchronous web request returns, either by finishing or timing out
        /// </summary>
        /// <param name="ar">The result of the web request</param>
        private void receiveResult(IAsyncResult ar)
        {
            try
            {
                bool containsKey = false;
                lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                {
                    containsKey = itemsCurrentlyBeingLookedUp.ContainsKey(ar);
                }
                // if items were cleared out, do not display information for pending requests
                if (containsKey)
                {
                    lookupInfo lookupInformation = (lookupInfo)ar.AsyncState;

                    switch (lookupInformation.whichLookup)
                    {
                        case loopType.patientLookup:
                            {
                                currentPatient = ws.EndgetPatientRecord(ar);
                                if (currentPatient.retryNeeded)
                                    throw new WebException();
                                else if (currentPatient.exists)
                                {
                                    setPhoto(patientPhotoPB, currentPatient.image);
                                    setLabel(patientNameLabel, currentPatient.lastName + ", " + currentPatient.firstName + " " + currentPatient.middleName);
                                    setLabel(medicineEntryForm.patientName, currentPatient.lastName + ", " + currentPatient.firstName + " " + currentPatient.middleName);
                                    setLabel(DOBLabel, (new DateTime(currentPatient.DOB) - new DateTime(1970, 1, 1)).ToString());
                                    setButtonEnabled(readMedButton, true);
                                }
                                else
                                {
                                    setPhoto(patientPhotoPB, (Image)null);
                                    setLabel(patientNameLabel, Properties.Resources.NotFound);
                                    setLabel(medicineEntryForm.patientName, string.Empty);
                                    setLabel(DOBLabel, string.Empty);
                                }
                            }
                            break;

                        case loopType.submitPhoto:
                            {
                                COREMedDemoWS.errorReport eR = ws.EndenterPatientPhoto(ar);
                                if (eR.errorCode == 0)
                                    notify("Success", "Patient photo successfully tied to patient record.", false);
                                else
                                    notify("Failure", eR.errorMessage, true);
                            }
                            break;

                        case loopType.drugLookup:
                            {
                                COREMedDemoWS.drugInfo dI = ws.EndgetDrugInfo(ar); // catch webexception

                                if (!dI.exists)
                                {
                                    setPhoto(medPB, (Image)null);
                                    setPhoto(medicineEntryForm.medicinePicture, (Image)null);
                                    
                                    setLabel(medicineEntryForm.medicationName, string.Empty);
                                    medicationRFIDNum = string.Empty;
                                    notify("Warning", "Not a known drug", true);

                                }
                                else
                                {
                                    setPhoto(medPB, dI.picture);
                                    setPhoto(medicineEntryForm.medicinePicture, dI.picture);

                                    setLabel(medicineEntryForm.medicationName, dI.name);
                                }
                            }
                            break;

                        case loopType.drugCheck:
                            {
                                COREMedDemoWS.errorReport errorReport = ws.EndcheckInteraction(ar);
                                if (errorReport.retryNeeded)
                                    throw new WebException();
                                if (!errorReport.success)
                                    notify("Warning", errorReport.errorMessage, true);
                                else
                                {
                                    if (showDialog(medicineEntryForm) == DialogResult.OK)
                                    {
                                        // then submit dosage to db
                                        scheduleLookup(new lookupInfo(patientRecord.RFIDnum, loopType.submitDose, medicationRFIDNum, null, Convert.ToSingle(medicineEntryForm.dosageValue)));
                                    }
                                }
                            }
                            break;

                        case loopType.submitDose:
                            {
                                bool success = ws.EndregisterDoseGiven(ar);
                                if (!success)
                                    notify("Warning", "This dose was not allowed", true);
                                else
                                    notify("Notice", "This dose has been recorded", false);
                            }
                            break;

                        case loopType.submitPatientTemps:
                            {
                                COREMedDemoWS.errorReport errorReport = ws.EndlogPatientVitals(ar);
                                if (errorReport.retryNeeded)
                                    throw new WebException();
                                else if (errorReport.errorCode != 0)
                                    notify("Warning", errorReport.errorMessage, true);
                            }
                            break;

                        default:
                            break;
                    }
                }
            }
            catch (WebException e)
            {
                debugOut.WriteLine(e.ToString() + Properties.Resources.at + e.StackTrace);
                rescheduleLookup(ar);
            }
            catch (ArgumentNullException e)
            {
                debugOut.WriteLine(e.ToString() + Properties.Resources.at + e.StackTrace);
                rescheduleLookup(ar);
            }
            catch (MemberAccessException e)
            {
                debugOut.WriteLine(e.ToString() + Properties.Resources.at + e.StackTrace);
                MessageBox.Show(Properties.Resources.Unexpected);
            }
            catch (SocketException e)
            {
                debugOut.WriteLine(e.ToString() + Properties.Resources.at + e.StackTrace);
                rescheduleLookup(ar);
            }
            catch (IOException e)
            {
                debugOut.WriteLine(e.ToString() + Properties.Resources.at + e.StackTrace);
                return;
            }
            catch (InvalidCastException e)
            {
                debugOut.WriteLine(e.ToString() + Properties.Resources.at + e.StackTrace);
            }
            catch (InvalidOperationException e)
            {
                debugOut.WriteLine(e.ToString() + Properties.Resources.at + e.StackTrace);
            }
            catch (IndexOutOfRangeException e)
            {
                debugOut.WriteLine(e.ToString() + Properties.Resources.at + e.StackTrace);
            }
            catch (NullReferenceException e)
            {
                debugOut.WriteLine(e.ToString() + Properties.Resources.at + e.StackTrace);
                rescheduleLookup(ar);
            }
            finally
            {
                // unmark this item as being looked up
                lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                {
                    itemsCurrentlyBeingLookedUp.Remove(ar);
                }
            }
        }

        /// <summary>
        /// Creates the appropriate web service and does the lookup
        /// </summary>
        /// <param name="tagID">the RFID tag string to lookup</param>
        /// <returns>The number of requests that were sent</returns>
        private int scheduleLookup(lookupInfo currentLookup)
        {
            int result = 1;

            try
            {
                if (itemsCurrentlyBeingLookedUp.ContainsValue(currentLookup.tagID))
                {
                    result = 0;
                }
                else
                {
                    IAsyncResult handle;

                    switch (currentLookup.whichLookup)
                    {
                        case loopType.submitPhoto:

                            patientRecord.image = currentLookup.extraArray;
                            patientRecord.RFIDnum = currentLookup.tagID;
                            lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                            {
                                handle = ws.BeginenterPatientPhoto(patientRecord, lookupCallback, currentLookup);
                                itemsCurrentlyBeingLookedUp[handle] = currentLookup;
                            }
                            break;

                        case loopType.drugLookup:

                            lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                            {
                                handle = ws.BegingetDrugInfo(currentLookup.tagID, DeviceUID, lookupCallback, currentLookup);
                                itemsCurrentlyBeingLookedUp[handle] = currentLookup;
                            }
                            break;

                        case loopType.drugCheck:

                            lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                            {
                                handle = ws.BegincheckInteraction(currentLookup.extraTagValue, currentLookup.tagID, lookupCallback, currentLookup);
                                itemsCurrentlyBeingLookedUp[handle] = currentLookup;
                            }
                            break;

                        case loopType.submitDose:

                            lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                            {
                                // done inside the lock in case the request finishes synchronously (rare but happens)
                                handle = ws.BeginregisterDoseGiven(currentLookup.tagID, currentLookup.extraTagValue, "000", Convert.ToInt32(currentLookup.dose), lookupCallback, currentLookup);
                                itemsCurrentlyBeingLookedUp[handle] = currentLookup;
                            }
                            break;

                        case loopType.patientLookup:

                            lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                            {
                                handle = ws.BegingetPatientRecord(currentLookup.tagID, DeviceUID, lookupCallback, currentLookup);
                                itemsCurrentlyBeingLookedUp[handle] = currentLookup;
                            }
                            break;

                        case loopType.submitPatientTemps:

                            lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                            {
                                handle = ws.BeginlogPatientVitals(currentLookup.tagID, currentLookup.tempArray, periodicity, lookupCallback, currentLookup);
                                itemsCurrentlyBeingLookedUp[handle] = currentLookup;
                            }
                            break;

                        default:
                            notify(Properties.Resources.Error, Properties.Resources.Error, true);
                            break;
                    }
                }
            }
            catch (SoapException)
            {
                //MessageBox.Show(Properties.Resources.UnexpectedSOAPException + ex.Message);
            }
            return result;
        }


        /// <summary>
        /// Performs a lookup again if the previous lookup has failed
        /// </summary>
        /// <param name="ar"></param>
        private void rescheduleLookup(IAsyncResult ar)
        {
            lookupInfo itemInfo = (lookupInfo)ar.AsyncState;
            scheduleLookup(itemInfo);
        }

        /// <summary>
        /// This will take a photo of the patient, load it for the operator to see and also load the image into a byte array
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void takePhoto(object sender, EventArgs e)
        {
            try
            {
                if (cameraCaptureDialogBox.ShowDialog() == DialogResult.OK && cameraCaptureDialogBox.FileName.Length > 0)
                {
                    FileStream fileStream = new FileStream(cameraCaptureDialogBox.FileName, FileMode.Open, FileAccess.Read);

                    newPatientImage = new byte[fileStream.Length];

                    fileStream.Read(newPatientImage, 0, System.Convert.ToInt32(fileStream.Length));

                    fileStream.Close();

                    newPatientPhotoPB.Image = new Bitmap(cameraCaptureDialogBox.FileName);

                    System.IO.File.Delete(cameraCaptureDialogBox.FileName);
                }
            }
            catch (Exception ex)
            { }
        }

        /// <summary>
        /// Send the photo and RFID number to the server to be linked together
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void submitPatientPhoto(object sender, EventArgs e)
        {
            if (newPatientImage == null || string.IsNullOrEmpty(newPatientRFIDNum))
                MessageBox.Show("Must have both image and bracelet ID first");
            else
                scheduleLookup(new lookupInfo(newPatientRFIDNum, loopType.submitPhoto, newPatientImage));
        }

        /// <summary>
        /// Read the patient's ID and store it, only choose VarioSens tags
        /// Also set the tag to begin logging
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void getPatientBraceletID(object sender, EventArgs e)
        {
            ((Button)sender).Enabled = false;

            patientVitalsListView.Items.Clear();
            
            // only want VS tags so we can grab temp data       
            try
            {
                newPatientRFIDNum = newPatientRFIDNumTB.Text = RFIDReader.Reader.readOneVSTagID();
            }
            catch (NullReferenceException ex)
            {
                notify("Warning", ex.Message,true);
            }

            ((Button)sender).Enabled = true;
        }

        /// <summary>
        /// Read the patient's VS tag, get the ID, get the log of temperatures recorded
        /// Submit the recorded temperatures, retrieve the patient's information
        /// Then enable the med button if the patient was successfully retrieved
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void readPatientTagClick(object sender, EventArgs e)
        {
            try
            {
                ((Button)sender).Enabled = false;

                setWaitCursor(true);

                VarioSensLogEventArgs result = tagReader.readLog(true, periodicity);

                newPatientRFIDNum = result.tagID;

                scheduleLookup(new lookupInfo(newPatientRFIDNum, loopType.patientLookup));

                if (result.Temperature.Length > 0)
                {
                    scheduleLookup(new lookupInfo(result.tagID, loopType.submitPatientTemps, result.Temperature));

                    // then display the temperatures
                    DateTime now = DateTime.Now;
                    patientVitalsListView.BeginUpdate();

                    for (int i = 0; i < result.RecordDate.Length; i++)
                    {
                        if (result.LogMode[i] == 1)
                        {
                            now = now - new TimeSpan(0, 0, periodicity);
                            float currentTemp = 9 / 5 * (result.Temperature[i]) + 32;
                            ListViewItem lvi = new ListViewItem(new string[] { now.ToShortTimeString(), currentTemp.ToString("0.0") });
                            if (currentTemp > 100)
                                lvi.BackColor = Color.Red;
                            patientVitalsListView.Items.Add(lvi);

                        }
                        // violation mode
                        else if (result.LogMode[i] == 2)
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

                    patientVitalsListView.EndUpdate();
                }
            }
            catch (NullReferenceException ex)
            {
                notify("Warning", ex.Message, false);
            }
            finally
            {
                setWaitCursor(false);
                ((Button)sender).Enabled = true;
            }            
        }
       
        private void clearDemo(object sender, EventArgs e)
        {
            patientVitalsListView.Clear();
            patientNameLabel.Text = string.Empty;
            setPhoto(medPB, (Image)null);
            setPhoto(medicineEntryForm.medicinePicture, (Image)null);
            setLabel(medicineEntryForm.medicationName, string.Empty);
            medicationRFIDNum = string.Empty;
            DOBLabel.Text = string.Empty;
            setPhoto(patientPhotoPB, (Image)null);
            setLabel(patientNameLabel, Properties.Resources.NotFound);
            setLabel(medicineEntryForm.patientName, string.Empty);
            setLabel(DOBLabel, string.Empty);
            setPhoto(newPatientPhotoPB, (Image)null);
            setTextBox(newPatientRFIDNumTB, string.Empty);
        }        

        private void readMedButton_Click(object sender, EventArgs e)
        {
            try
            {
                setWaitCursor(true);
                ((Button)sender).Enabled = false;
                medicationRFIDNum = Reader.readOneTagIDDLL();
                setWaitCursor(false);
                scheduleLookup(new lookupInfo(medicationRFIDNum, loopType.drugLookup));
                scheduleLookup(new lookupInfo(medicationRFIDNum, loopType.drugCheck, newPatientRFIDNum));
            }
            catch (NullReferenceException)
            { }
            finally
            {
                setWaitCursor(false); 
                ((Button)sender).Enabled = true;
            }
        }

        private void exitButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void disableLoggingButton_Click(object sender, EventArgs e)
        {
            try
            {
                tagReader.disableVS();
                notify("Notice", "Successfully disabled VarioSens tag.", false);
            }
            catch (NullReferenceException ex)
            {
                notify("Error",ex.Message,true);
            }
        }
    }
}