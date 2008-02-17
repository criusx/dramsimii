using System;
using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.IO.Ports;
using System.Net;
using System.Net.Sockets;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading;
using System.Web.Services.Protocols;
using System.Windows.Forms;
using NMEA;
using RFIDReader;
using Microsoft.WindowsMobile.Status;
using Microsoft.WindowsCE.Forms;


[assembly: CLSCompliant(true)]
namespace GentagDemo
{
    public partial class demoClient : Form
    {
        #region Enumerated Types
        private enum loopType { counterfeit, patient, med, assay, none };
        #endregion

        #region Members
        private static demoClient mF;

        private Notification notification1 = null;

        private nmeaInterpreter gpsInterpreter;

        private string DeviceUID;

        private Reader tagReader;

        private TextWriter debugOut;

        private RFIDReadCursor.RFIDReadWaitCursor waitCursor;

        private Thread versionCheckThread;

        private assayResultsChooser assayResultsDialog;

        private string currentPatientID;

        private TimeSpan assayTimer;

        // the variable that describes whether it's looping looking for wine bottles or general tags
        private loopType loop;

        private Hashtable wineBottleCache = new Hashtable();

        private Hashtable counterfeitCache = new Hashtable();

        private Hashtable petCache = new Hashtable();

        private Hashtable patientCache = new Hashtable();

        private Hashtable assayCache = new Hashtable();

        private Hashtable itemsCurrentlyBeingLookedUp = new Hashtable();

        private Queue<string> lookupQueue = new Queue<string>();

        private const int maxLookupThreads = 4;

        // the number of lookups sent off already
        private int pendingLookups;

        private int queuedLookups;

        private medWS.COREMedDemoWS COREMedDemoWebService;

        private authenticationWS.AuthenticationWebService authenticationWebService;

        private assayWS.AssayWS assayWebService;

        private int assayItemChosen = -1;

        private string sessionID;

        System.Windows.Forms.Timer assayCountdownTimer;

        DateTime assayEndTime = new DateTime();

        manualTag manualDialog;
        #endregion

        public demoClient()
        {
            // run initializations provided by the designer
            InitializeComponent();

            mF = this;

            tagReader = new Reader();

            inputPanel1.Enabled = false;

            // assayResultsDialog
            assayResultsDialog = new assayResultsChooser();

            assayResultsDialog.Height = System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Height;

            DeviceUID = Reader.getDeviceUniqueID(GentagDemo.Properties.Resources.titleString);

            // assayCountdownTimer
            assayCountdownTimer = new System.Windows.Forms.Timer();

            assayCountdownTimer.Interval = 1000;

            assayCountdownTimer.Tick += new EventHandler(assayCountdownTimer_Tick);

            // resize the client depending on the resolution
            this.ClientSize = new Size(System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Width, System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Height);

            mainTabControl.Size = this.ClientSize;

            foreach (TabPage tP in mainTabControl.TabPages)
            {
                tP.Size = new Size(this.ClientSize.Width, Convert.ToInt32(Convert.ToSingle(this.ClientSize.Height) / 240F) * tP.Size.Height);
                foreach (Control childControl in tP.Controls)
                {
                    if (childControl.GetType() == typeof(PictureBox) || childControl.GetType() == typeof(TreeView))
                    {
                        childControl.Height = this.ClientSize.Height - 240 + childControl.Height;
                    }
                }
            }

            // intialize threads
            versionCheckThread = new Thread(versionCheck);
            versionCheckThread.IsBackground = true;
            versionCheckThread.Start();

            // load the manual tag input dialog
            manualDialog = new manualTag();
            manualDialog.Hide();

            // set the version
            versionLabel.Text = Properties.Resources.Version + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString() + Properties.Resources.Alpha;

            // initialize the debug stream
            for (int i = 50; i > 0; i--)
            {
                try
                {
                    debugOut = new StreamWriter("debug.txt" + (i - 50).ToString(), false);
                    break;
                }
                catch (IOException)
                {
                }
            }

            // initialize the custom wait cursor
            int systemWidth = System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Width;
            int systemHeight = System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Height;
            this.waitCursor = new RFIDReadCursor.RFIDReadWaitCursor();
            this.waitCursor.Location = new System.Drawing.Point((systemWidth - waitCursor.Size.Width) / 2, (systemHeight - waitCursor.Size.Height) / 2);
            this.waitCursor.Size = new System.Drawing.Size(72, 72);

            // initialize the nmea receiver
            gpsInterpreter = new nmeaInterpreter(CultureInfo.CurrentCulture, DeviceUID);

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

            // setup event callbacks for tag reading events
            tagReader.TagReceived += new EventHandler<TagReceivedEventArgs>(receiveTag);
            tagReader.VarioSensSettingsReceived += new EventHandler<VarioSensSettingsEventArgs>(receiveVarioSensSettings);
            tagReader.ReaderError += new EventHandler<ReaderErrorEventArgs>(receiveReaderError);
            tagReader.VarioSensLogReceived += new EventHandler<VarioSensLogEventArgs>(writeViolations);
            tagReader.ReturnTagContents += new EventHandler<TagContentsEventArgs>(receiveTagContents);
            tagReader.DoneWriting += new EventHandler<FinishedWritingStatusEventArgs>(doneWriting);
            tagReader.TagTypeDetected += new EventHandler<TagTypeEventArgs>(displayTagType);

            // initialize the image list in the treeview
            ImageList myImageList = new ImageList();
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.blank.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.cancel.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.ok.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.question.GetHbitmap()));
            authTreeView.ImageList = myImageList;

            // web service inits
            const int timeout = 10000;

            COREMedDemoWebService = new medWS.COREMedDemoWS();
            authenticationWebService = new authenticationWS.AuthenticationWebService();
            assayWebService = new assayWS.AssayWS();

            COREMedDemoWebService.Timeout = timeout;
            authenticationWebService.Timeout = timeout;
            assayWebService.Timeout = timeout;

            initStatusPage();

            // Init the Registry
            //RegistryKey regKey = Registry.LocalMachine;

            //regKey = regKey.OpenSubKey(@"SOFTWARE", true);

            //if (Array.IndexOf(regKey.GetSubKeyNames(), @"Gentag", 0) == -1)
            //    regKey.CreateSubKey(@"Gentag");
            //regKey = regKey.OpenSubKey(@"Gentag", true);

            //string[] settingKeys = regKey.GetSubKeyNames();

            ////user ID
            //if (Array.IndexOf(settingKeys, @"hostname", 0) == -1)
            //{
            //    regKey.CreateSubKey(@"hostname");
            //    regKey.SetValue(@"hostname", @"129.2.99.117");
            //}
            //hostName.Text = regKey.GetValue(@"hostname").ToString();
        }

        

        

        void displayTagType(object sender, TagTypeEventArgs args)
        {

            blinkCursor(true);

            switch (args.Type)
            {
                case tagTypes.INSIDE:
                    setLabel(detectTagTypeLabel, "INSIDE");
                    break;
                case tagTypes.iso14443a:
                    setLabel(detectTagTypeLabel, "ISO14443A");
                    break;
                case tagTypes.iso14443b:
                    setLabel(detectTagTypeLabel, "ISO14443B");
                    break;
                case tagTypes.iso14443bsr176:
                    setLabel(detectTagTypeLabel, "ISO14443B");
                    break;
                case tagTypes.iso14443bsri:
                    setLabel(detectTagTypeLabel, "ISO14443B");
                    break;
                case tagTypes.iso15693:
                    setLabel(detectTagTypeLabel, "ISO15693");
                    break;
                case tagTypes.iso18000:
                    setLabel(detectTagTypeLabel, "ISO18000");
                    break;
                case tagTypes.MiFareClassic:
                    setLabel(detectTagTypeLabel, "MiFare");
                    break;
                case tagTypes.MiFareDESFire:
                    setLabel(detectTagTypeLabel, "MiFare DESFire");
                    break;
                case tagTypes.MiFareUltraLight:
                    setLabel(detectTagTypeLabel, "MiFare UltraLight");
                    break;
                case tagTypes.felica:
                    setLabel(detectTagTypeLabel, "FelICa");
                    break;
            }

            setLabel(detectTagIDLabel, args.TagID);
            Thread.Sleep(250);

            blinkCursor(false);
        }

        ~demoClient()
        {
            stopReading();
            tagReader.Running = false;
            debugOut.Dispose();
            Dispose(false);
        }

        private void receiveTagContents(object sender, TagContentsEventArgs args)
        {
            setTextBox(readWriteTB, args.Contents);
            stopReading();
        }

        private void writeTagMemory()
        {
            tagReader.writeTag(System.Text.Encoding.Unicode.GetBytes(getTextBox(readWriteTB)));
        }

        private void doneWriting(object sender, FinishedWritingStatusEventArgs args)
        {
            setLabel(readWriteStatusLabel, Properties.Resources.Status + args.Status);
            stopReading();
        }

        private void readerClick(object sender, EventArgs e)
        {
            if (tagReader.Running == false)
            {
                setWaitCursor(true);

                if (sender == medicationButton || sender == readIDButton || sender == readPatientButton || sender == assayReadButton)
                {
                    if (sender == readIDButton)
                        loop = loopType.counterfeit;
                    else if (sender == readPatientButton)
                        loop = loopType.patient;
                    else if (sender == medicationButton)
                        loop = loopType.med;
                    else if (sender == assayReadButton)
                        loop = loopType.assay;

                    // whether it is a loop lookup or a single item lookup
                    if (sender == readPatientButton || sender == medicationButton || sender == assayReadButton)
                        tagReader.ReadThreadType = readType.readOne;
                    else
                        tagReader.ReadThreadType = readType.readMany;

                }
                if (sender == detectTagTypeButton)
                    tagReader.ReadThreadType = readType.detectMany;
                else if (sender == readButton)
                    tagReader.ReadThreadType = readType.readTagMemory;
                else if (sender == writeButton)
                    tagReader.ReadThreadType = readType.writeTagMemory;

                //TODO: send values for setVarioSensSettings and writeTagMemory
                tagReader.Running = true;
            }
            else
            {
                tagReader.Running = false;
                stopReading();
            }
        }

        /// <summary>
        /// The callback for when a tag is received by the reader thread
        /// </summary>
        /// <param name="tagID"></param>
        [MethodImpl(MethodImplOptions.Synchronized)]
        private void receiveTag(object sender, TagReceivedEventArgs args)
        {
            if (string.IsNullOrEmpty(args.Tag)) // if there was no string returned
                return;

            blinkCursor(true);

            lock (lookupQueue)
            {
                if (pendingLookups < maxLookupThreads)
                {
                    pendingLookups += scheduleLookup(args.Tag, loop);

                    setLabel(pendingLookupsLabel, pendingLookups.ToString(CultureInfo.CurrentCulture));
                }
                else
                {
                    // no need to add it to the queue if it already exists
                    if (!lookupQueue.Contains(args.Tag))
                    {
                        queuedLookups++;
                        setLabel(queuedLookupsLabel, lookupQueue.Count.ToString(CultureInfo.CurrentCulture));

                        if (loop == loopType.counterfeit)
                            updateTreeView1(args.Tag, Properties.Resources.NoDescriptionYet, false, true, false);
                        // queue it up for later and return
                        lookupQueue.Enqueue(args.Tag);
                    }
                }
            }

            // flash the panel to signal the user that a tag was read
            Thread.Sleep(100);
            blinkCursor(false);

            if (args.Done)
                stopReading();
        }

        /// <summary>
        /// Creates the appropriate web service and does the lookup
        /// </summary>
        /// <param name="tagID">the RFID tag string to lookup</param>
        /// <returns>true if the request was sent, false if it wasn't for some reason</returns>
        private int scheduleLookup(string tagID, loopType whichLoop)
        {
            AsyncCallback cb = new AsyncCallback(receiveResult);
            int result = 1;

            try
            {
                if (itemsCurrentlyBeingLookedUp.ContainsValue(tagID))
                {
                    result = 0;
                }
                else
                {
                    switch (whichLoop)
                    {
                        case loopType.counterfeit:
                            {
                                // if it has already been added, do nothing
                                if (counterfeitCache.ContainsKey(tagID))
                                {
                                    result = 0;
                                }
                                else
                                {
                                    // get the tag info and cache it
                                    IAsyncResult handle = authenticationWebService.BegingetItem(tagID, DeviceUID, gpsInterpreter.getLatitude(), gpsInterpreter.getLongitude(), cb, authenticationWebService);
                                    
                                    lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                    { itemsCurrentlyBeingLookedUp.Add(handle, tagID); }

                                    updateTreeView1(tagID, Properties.Resources.NoDescriptionYet, false, true, true);
                                }
                                break;
                            }
                        case loopType.patient:
                            {
                                if (patientCache.ContainsKey(tagID))
                                {
                                    result = 0;
                                    displayPatient((medWS.patientRecord)patientCache[tagID]);
                                }
                                else
                                {
                                    setTextBox(patientNameBox, Properties.Resources.PleaseWait);
                                    IAsyncResult handle = COREMedDemoWebService.BegingetPatientRecord(tagID, DeviceUID, cb, COREMedDemoWebService);
                                    lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                    { itemsCurrentlyBeingLookedUp[handle] = tagID; }
                                    currentPatientID = tagID;
                                }
                                break;
                            }
                        case loopType.med:
                            {
                                // patient and med RFID cannot be the same
                                if (string.IsNullOrEmpty(currentPatientID) || string.Compare(currentPatientID, tagID, true) == 0)
                                    break;

                                result = 2;

                                setTextBox(drugNameBox, Properties.Resources.PleaseWait);
                                IAsyncResult handle = COREMedDemoWebService.BegingetDrugInfo(tagID, DeviceUID, cb, COREMedDemoWebService);
                                lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                { itemsCurrentlyBeingLookedUp[handle] = tagID; }

                                handle = COREMedDemoWebService.BegincheckInteraction(currentPatientID, tagID, cb, COREMedDemoWebService);

                                lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                { itemsCurrentlyBeingLookedUp[handle] = currentPatientID; }

                                break;
                            }
                        case loopType.assay:
                            {
                                if (assayCache.ContainsKey(tagID))
                                {
                                    result = 0;
                                    displayAssay((assayWS.assayInfo)assayCache[tagID]);
                                }
                                else
                                {
                                    setLabel(assayMessageLabel, GentagDemo.Properties.Resources.PleaseWait);
                                    IAsyncResult handle = assayWebService.BeginretrieveAssayInformation(tagID, DeviceUID, cb, assayWebService);
                                    // to allow association of a handle back to a tag id
                                    lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                    { itemsCurrentlyBeingLookedUp[handle] = tagID; }
                                }
                                break;
                            }
                        default:
                            notify(Properties.Resources.Error,Properties.Resources.Error);
                            break;
                    }
                }
            }
            catch (SoapException ex)
            {
                MessageBox.Show("Unexpected SOAP exception: " + ex.Message);
            }
            return result;
        }

        private void receiveResult(IAsyncResult ar)
        {
            try
            {
                if (itemsCurrentlyBeingLookedUp.ContainsKey(ar) == true)
                {

                    if (ar.AsyncState.GetType() == typeof(authenticationWS.AuthenticationWebService))
                    {
                        // try to cast as a standard item lookup    
                        authenticationWS.AuthenticationWebService ws = (authenticationWS.AuthenticationWebService)ar.AsyncState;
                        authenticationWS.itemInfo info = ws.EndgetItem(ar);

                        if (info.retryNeeded)
                            throw new WebException();

                        // display the tag info
                        if (info != null)
                        {
                            updateTreeView1(info.RFIDNum, info.description, info.authenticated, false, true);

                            lock (counterfeitCache.SyncRoot)
                            {
                                counterfeitCache[info.RFIDNum] = info;
                            }
                        }
                    }
                    else if (ar.AsyncState.GetType() == typeof(medWS.COREMedDemoWS))
                    {
                        medWS.COREMedDemoWS ws = (medWS.COREMedDemoWS)ar.AsyncState;
                        // interaction check
                        try
                        {
                            medWS.drugInfo drugInf = ws.EndgetDrugInfo(ar);

                            if (drugInf.retryNeeded)
                                throw new WebException();

                            if (drugInf != null)
                                displayDrug(drugInf);
                        }
                        catch (InvalidCastException e)
                        {
                            try
                            {
                                medWS.errorReport er = ws.EndcheckInteraction(ar);

                                if (er.retryNeeded)
                                    throw new WebException();

                                if (er.errorCode > 0)
                                    MessageBox.Show(er.errorMessage);
                            }
                            catch (InvalidCastException ex)
                            {
                                medWS.patientRecord info = ws.EndgetPatientRecord(ar);

                                if (info.retryNeeded)
                                    throw new WebException();

                                if (info != null && info.exists == true)
                                {
                                    lock (patientCache.SyncRoot)
                                    { patientCache[info.RFIDnum] = info; }

                                    if (info.RFIDnum == currentPatientID)
                                        displayPatient(info);
                                }
                            }
                        }
                    }
                    else if (ar.AsyncState.GetType() == typeof(assayWS.AssayWS))
                    {
                        assayWS.AssayWS ws = (assayWS.AssayWS)ar.AsyncState;

                        assayWS.assayInfo info = ws.EndretrieveAssayInformation(ar);

                        if (info.needRetry)
                            throw new WebException();

                        if (info.exists)
                        {
                            displayAssay(info);

                            assayCache[info.rfidNum] = info;
                        }
                    }
                    // free up some space in the available lookups and schedule a waiting lookup
                    lock (lookupQueue)
                    {
                        pendingLookups--;
                        try
                        {
                            if (lookupQueue.Count > 0)
                            {
                                while ((pendingLookups < maxLookupThreads) && (lookupQueue.Count > 0))
                                {
                                    pendingLookups += scheduleLookup(lookupQueue.Dequeue(), loop);
                                }
                            }

                            setLabel(pendingLookupsLabel, pendingLookups.ToString(CultureInfo.CurrentCulture));

                            setLabel(queuedLookupsLabel, lookupQueue.Count.ToString(CultureInfo.CurrentCulture));
                        }
                        catch (InvalidOperationException e)
                        {
                            debugOut.WriteLine(e.ToString() + Properties.Resources.at + e.StackTrace);
                        }
                    }

                    // unmark this item as being looked up
                    lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                    {
                        itemsCurrentlyBeingLookedUp.Remove(ar);
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
                Thread.Sleep(2000);
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
            catch (Exception e)
            {
                debugOut.WriteLine(e.ToString() + Properties.Resources.at + e.StackTrace);
            }
        }

        private void stopReading()
        {
            setWaitCursor(false);
            blinkCursor(false);
        }

        private void receiveReaderError(object sender, ReaderErrorEventArgs args)
        {
            stopReading();
            notify(Properties.Resources.ProblemWithReader, args.Reason);
        }

        private void notify(string caption, string text)
        {
            try
            {
                if (notification1 != null)
                    notification1.Dispose();
            }
            catch (Exception)
            { }
            notification1 = new Notification();
            notification1.Caption = caption;
            notification1.Text = text;
            notification1.Critical = true;
            notification1.Visible = true;
        }

        /// <summary>
        /// Performs a lookup again if the previous lookup has failed
        /// </summary>
        /// <param name="ar"></param>
        private void rescheduleLookup(IAsyncResult ar)
        {
            string currentTag;

            lock (itemsCurrentlyBeingLookedUp.SyncRoot)
            {
                currentTag = (string)itemsCurrentlyBeingLookedUp[ar];
                itemsCurrentlyBeingLookedUp.Remove(ar);
            }

            if (ar.AsyncState.GetType() == typeof(authenticationWS.AuthenticationWebService))
            {
                scheduleLookup(currentTag, loopType.counterfeit);
            }
            else if (ar.AsyncState.GetType() == typeof(medWS.COREMedDemoWS))
            {
                scheduleLookup(currentTag, loopType.med);
            }
        }

        #region Display Results
        private delegate void displayAssayDelegate(assayWS.assayInfo info);

        /// <summary>
        /// Displays the assay photo, messages, result pictures and initializes the countdown timer
        /// </summary>
        /// <param name="info">This is the assayInfo object that contains all the parameters for the assay</param>
        private void displayAssay(assayWS.assayInfo info)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new displayAssayDelegate(displayAssay), new object[] { info });
            }
            else
            {
                assayPage.SuspendLayout();

                if (info != null && info.exists)
                {
                    if (info.exists)
                    {
                        assayRecordButton.Enabled = false;

                        assayBeginButton.Enabled = true;

                        assayMessageLabel.Text = info.beforeMessage;

                        assayTimer = new TimeSpan(0, 0, info.timer);

                        assayTimerLabel.Text = assayTimer.ToString();

                        sessionID = info.sessionID;

                        assayResultsDialog.setImages(info.resultImages);

                        assayResultsDialog.setAfterMessage(info.afterMessage);

                        setPhoto(assayImagePictureBox, info.descriptionImage);
                    }
                    else
                    {

                        assayMessageLabel.Text = Properties.Resources.AssayNotFound;

                        assayTimerLabel.Text = GentagDemo.Properties.Resources.emptyString;

                        sessionID = null;

                        assayResultsDialog.setImages(null);

                        assayResultsDialog.setAfterMessage(null);

                        setPhoto(assayImagePictureBox, (System.Drawing.Image)null);
                    }
                }
                else
                {
                    assayMessageLabel.Text = GentagDemo.Properties.Resources.emptyString;

                    assayTimerLabel.Text = GentagDemo.Properties.Resources.emptyString;

                    sessionID = null;

                    assayResultsDialog.setImages(null);

                    assayResultsDialog.setAfterMessage(null);

                    setPhoto(assayImagePictureBox, (System.Drawing.Image)null);
                }
                assayPage.ResumeLayout();
            }
        }

        private void displayDrug(medWS.drugInfo drug)
        {
            try
            {
                if (drug != null && drug.exists && drug.picture != null)
                {
                    setPhoto(drugPhoto, drug.picture);
                    setTextBox(drugNameBox, drug.name);
                }
                else
                {
                    setPhoto(drugPhoto, (System.Drawing.Image)null);
                    setTextBox(drugNameBox, Properties.Resources.emptyString);
                }
            }
            catch (Exception)
            {
                setPhoto(drugPhoto, (System.Drawing.Image)null);
                setTextBox(drugNameBox, GentagDemo.Properties.Resources.emptyString);
            }
        }

        private delegate void displayPatientDelegate(medWS.patientRecord patient);

        /// <summary>
        /// Displays the patient information, such as DOB, photo, allergies, medical history, etc.
        /// </summary>
        /// <param name="patient"></param>
        private void displayPatient(medWS.patientRecord patient)
        {
            if (patient == null || patient.RFIDnum == null || patient.image == null)
            {
                setTextBox(patientNameBox, GentagDemo.Properties.Resources.emptyString);
                setTextBox(patientDescriptionBox, GentagDemo.Properties.Resources.emptyString);
                setPhoto(patientPhoto, (Image)null);
                setButtonEnabled(medicationButton, false);
            }
            else if (patient.exists)
            {
                try
                {
                    setTextBox(patientNameBox, patient.lastName + ", " + patient.firstName + " " + patient.middleName);
                    DateTime dob = new DateTime(1970, 1, 1);
                    dob = dob.AddTicks(patient.DOB * 1000);
                    string description = dob.ToString() + System.Console.Out.NewLine;
                    if (patient.allergies != null)
                    {
                        description += "Allergies" + System.Console.Out.NewLine;
                        foreach (string s in patient.allergies)
                            description += s + System.Console.Out.NewLine;
                    }
                    if (patient.medications != null)
                    {
                        description += "Medications" + System.Console.Out.NewLine;
                        foreach (string s in patient.medications)
                            description += s + System.Console.Out.NewLine;
                    }
                    setTextBox(patientDescriptionBox, description);
                    setPhoto(patientPhoto, patient.image);
                    setButtonEnabled(medicationButton, true);
                }
                catch (Exception)
                {
                    setTextBox(patientNameBox, GentagDemo.Properties.Resources.emptyString);
                    setTextBox(patientDescriptionBox, GentagDemo.Properties.Resources.patientInfoNotFound);
                    setPhoto(patientPhoto, (Image)null);
                }
            }
            else
            {
                setTextBox(patientNameBox, GentagDemo.Properties.Resources.emptyString);
                setTextBox(patientDescriptionBox, GentagDemo.Properties.Resources.emptyString);
                setPhoto(patientPhoto, (Image)null);
            }
        }

        private delegate void updateTreeView1Delegate(string currentTag, string rfidDescr, bool isAuthenticated, bool isNew, bool shouldUpdateIfExists);

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void updateTreeView1(string currentTag, string rfidDescr, bool isAuthenticated, bool isNew, bool shouldUpdateIfExists)
        {
            if (this.InvokeRequired)
            {
                Invoke(new updateTreeView1Delegate(updateTreeView1),
                    new object[] { currentTag, rfidDescr, isAuthenticated, isNew, shouldUpdateIfExists });
                return;
            }
            authTreeView.BeginUpdate();

            bool exists = false;

            foreach (TreeNode tn in authTreeView.Nodes)
            {
                if (tn.Text == currentTag)
                {
                    exists = true;
                    if (shouldUpdateIfExists)
                    {
                        tn.Nodes.Clear();
                        TreeNode tN = new TreeNode(rfidDescr);
                        tn.SelectedImageIndex = tn.ImageIndex = isNew ? 3 : (isAuthenticated ? 2 : 1);
                        tn.Nodes.Add(tN);
                    }
                }
            }
            if (!exists)
            {
                TreeNode superTn = new TreeNode(currentTag);
                superTn.SelectedImageIndex = superTn.ImageIndex = isNew ? 3 : (isAuthenticated ? 2 : 1);

                TreeNode subTn = new TreeNode(rfidDescr);
                subTn.SelectedImageIndex = subTn.ImageIndex = 0;

                superTn.Nodes.Add(subTn);
                authTreeView.Nodes.Add(superTn);
            }
            authTreeView.EndUpdate();
        }
        #endregion


        private void mainForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == System.Windows.Forms.Keys.Back)
            {
                clearDemo();
            }
            else if (e.KeyCode == System.Windows.Forms.Keys.F2)
            {
                if (mainTabControl.TabPages[mainTabControl.SelectedIndex].Name == "authPage")
                    readerClick(readIDButton, new EventArgs());
            }
        }

        private void clearDemo()
        {
            authTreeView.BeginUpdate();
            authTreeView.Nodes.Clear();
            authTreeView.EndUpdate();

            counterfeitCache.Clear();
            wineBottleCache.Clear();
            petCache.Clear();
            patientCache.Clear();
            assayCache.Clear();

            itemsCurrentlyBeingLookedUp.Clear();
            setLabel(detectTagIDLabel, GentagDemo.Properties.Resources.emptyString);
            setLabel(detectTagTypeLabel, GentagDemo.Properties.Resources.emptyString);
            displayDrug(null);
            displayPatient(null);
            displayAssay(null);
        }


        private void trackingCheckBox_CheckStateChanged(object sender, EventArgs e)
        {
            gpsInterpreter.setTracking(trackingCheckBox.Checked);
        }


        private void tabControl1_SelectedIndexChanged(object sender, EventArgs e)
        {
            stopReading();
        }

        private void textBox4_GotFocus(object sender, EventArgs e)
        {
            inputPanel1.Enabled = true;
            if (manualDialog.ShowDialog() == DialogResult.Yes)
            {
                updateTreeView1(manualDialog.Value, Properties.Resources.NoDescriptionYet, false, true, false);
                TagReceivedEventArgs arg = new TagReceivedEventArgs(manualDialog.Value, true);
                receiveTag(this, arg);
                scheduleLookup(manualDialog.Value, loopType.counterfeit);
            }
            inputPanel1.Enabled = false;
        }

        private void connectGPSButton_Click(object sender, EventArgs e)
        {
            try
            {
                if (gpsInterpreter.IsOpen())
                {
                    if (gpsInterpreter.Close())
                        setButtonText(connectGPSButton, "Connect");
                }
                else
                {
                    if (gpsInterpreter.Open(getComboBox(comPortsComboBox)))
                        setButtonText(connectGPSButton, "Disconnect");
                }
            }
            catch (System.IO.IOException)
            {

            }
        }

        private void scanCOMPorts(object sender, EventArgs e)
        {
            string[] COMPorts = SerialPort.GetPortNames();
            Array.Sort(COMPorts);
            comPortsComboBox.Items.Clear();
            foreach (string s in COMPorts)
            {
                comPortsComboBox.Items.Add(s);
            }
            comPortsComboBox.SelectedIndex = 0;
        }

        private void exitButton_Click(object sender, EventArgs e)
        {
            //System.Diagnostics.Process.Start(System.Reflection.Assembly.GetExecutingAssembly().ManifestModule.FullyQualifiedName, "");
            this.Close();
        }

        private void assayClearButton_Click(object sender, EventArgs e)
        {
            assayTimerLabel.Text = assayTimer.ToString();
            setButtonEnabled(assayBeginButton, true);
        }

        private void assayRecordButton_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(sessionID) || string.IsNullOrEmpty(DeviceUID) || assayItemChosen < 0)
                return;

            int trials = 0;

            while (trials < 5)
            {
                try
                {
                    assayWS.assayResultResponse response = assayWebService.submitAssayResult(sessionID, DeviceUID, assayItemChosen + 1);
                    if (response.needRetry || !response.success)
                    {
                        MessageBox.Show(Properties.Resources.FailedToSubmit);
                    }
                    else
                    {
                        MessageBox.Show(Properties.Resources.ResultsRecorded);
                        assayItemChosen = -1;
                        setButtonEnabled(assayRecordButton, false);
                        break;
                    }
                }
                catch (WebException ex)
                {
                    debugOut.WriteLine(ex.StackTrace);

                    if (trials > 5)
                        notify(Properties.Resources.ConnectionProblem,Properties.Resources.PossibleNetworkErrorPleaseTryAgain);
                }
                finally
                {
                    trials++;
                }
            }
        }

        private void assayBeginButton_Click(object sender, EventArgs e)
        {
            assayEndTime = DateTime.Now.Add(assayTimer);
            assayCountdownTimer.Enabled = true;
        }

        void assayCountdownTimer_Tick(object sender, EventArgs e)
        {

            setLabel(assayTimerLabel, assayEndTime.Subtract(DateTime.Now).ToString());

            if (DateTime.Now >= assayEndTime)
            {
                assayCountdownTimer.Enabled = false;
                setButtonEnabled(assayBeginButton, false);
                System.Media.SystemSounds.Question.Play();
                if (DialogResult.OK == assayResultsDialog.ShowDialog())
                {
                    assayItemChosen = assayResultsDialog.SelectedIndex;
                    setButtonEnabled(assayRecordButton, true);
                }
            }
        }

        private void mainClearButton_Click(object sender, EventArgs e)
        {
            clearDemo();
        }

        private void notification1_ResponseSubmitted(object sender, Microsoft.WindowsCE.Forms.ResponseSubmittedEventArgs e)
        {
            notification1.Visible = false;
        }

        private void initStatusPage()
        {
            SystemState systemState;

            systemState = new SystemState(SystemProperty.ConnectionsCount);
            systemState.Changed += new ChangeEventHandler(connectionCountChanged);
            ListViewItem.ListViewSubItem lvi = new ListViewItem.ListViewSubItem();
            lvi.Text = ((int)systemState.CurrentValue).ToString();
            statusListView.Items[5].SubItems.Add(lvi);

            systemState = new SystemState(SystemProperty.ConnectionsCellularCount);
            systemState.Changed += new ChangeEventHandler(cellularConnectionCountChanged);
            lvi = new ListViewItem.ListViewSubItem();
            lvi.Text = ((int)systemState.CurrentValue).ToString();
            statusListView.Items[6].SubItems.Add(lvi);

            systemState = new SystemState(SystemProperty.ConnectionsNetworkCount);
            systemState.Changed += new ChangeEventHandler(networkConnectionCountChanged);
            lvi = new ListViewItem.ListViewSubItem();
            lvi.Text = ((int)systemState.CurrentValue).ToString();
            statusListView.Items[7].SubItems.Add(lvi);

            try
            {
                systemState = new SystemState(SystemProperty.ActiveSyncStatus);
                systemState.Changed += new ChangeEventHandler(activeSyncStatusChanged);
                lvi = new ListViewItem.ListViewSubItem();
                lvi.Text = ((ActiveSyncStatus)systemState.CurrentValue).ToString();
                statusListView.Items[0].SubItems.Add(lvi);
            }
            catch (NullReferenceException)
            { }
            catch (ArgumentOutOfRangeException)
            { }

            systemState = new SystemState(SystemProperty.Phone1xRttCoverage);
            systemState.Changed += new ChangeEventHandler(phoneCoverageChanged);
            lvi = new ListViewItem.ListViewSubItem();
            lvi.Text = Convert.ToBoolean(systemState.CurrentValue).ToString();
            statusListView.Items[1].SubItems.Add(lvi);

            systemState = new SystemState(SystemProperty.PhoneNoService);
            systemState.Changed += new ChangeEventHandler(phoneNoServiceChanged);
            lvi = new ListViewItem.ListViewSubItem();
            lvi.Text = Convert.ToBoolean(systemState.CurrentValue).ToString();
            statusListView.Items[2].SubItems.Add(lvi);

            systemState = new SystemState(SystemProperty.PhoneRadioOff);
            systemState.Changed += new ChangeEventHandler(phoneRadioOffChanged);
            lvi = new ListViewItem.ListViewSubItem();
            lvi.Text = Convert.ToBoolean(systemState.CurrentValue).ToString();
            statusListView.Items[3].SubItems.Add(lvi);

            systemState = new SystemState(SystemProperty.PhoneSearchingForService);
            systemState.Changed += new ChangeEventHandler(searchingForServiceChanged);
            lvi = new ListViewItem.ListViewSubItem();
            lvi.Text = Convert.ToBoolean(systemState.CurrentValue).ToString();
            statusListView.Items[8].SubItems.Add(lvi);

            try
            {
                systemState = new SystemState(SystemProperty.PhoneSignalStrength);
                systemState.Changed += new ChangeEventHandler(signalStrengthChanged);
                lvi = new ListViewItem.ListViewSubItem();
                lvi.Text = systemState.CurrentValue.ToString();
                statusListView.Items[4].SubItems.Add(lvi);
            }
            catch (NullReferenceException)
            { }

            systemState = new SystemState(SystemProperty.PowerBatteryStrength);
            systemState.Changed += new ChangeEventHandler(batteryStrengthChanged);

            BatteryLevel bl = (BatteryLevel)systemState.CurrentValue;
            string batteryLevel;
            switch (bl)
            {
                case BatteryLevel.VeryLow:
                    batteryLevel = "Very Low";
                    break;
                case BatteryLevel.VeryHigh:
                    batteryLevel = "Very High";
                    break;
                case BatteryLevel.Medium:
                    batteryLevel = "Medium";
                    break;
                case BatteryLevel.Low:
                    batteryLevel = "Low";
                    break;
                case BatteryLevel.High:
                    batteryLevel = "High";
                    break;
                default:
                    batteryLevel = "";
                    break;
            }
            lvi = new ListViewItem.ListViewSubItem();
            lvi.Text = batteryLevel;
            statusListView.Items[9].SubItems.Add(lvi);
        }

        private static void versionCheck()
        {
            try
            {
                HttpWebRequest request = (HttpWebRequest)WebRequest.Create("http://www.gentag.com/version.txt");
                HttpWebResponse response = (HttpWebResponse)request.GetResponse();
                Stream responseStream = response.GetResponseStream();
                string tempString = null;
                int count = 0;
                byte[] buf = new byte[8192];
                StringBuilder sb = new StringBuilder();


                do
                {
                    // fill the buffer with data
                    count = responseStream.Read(buf, 0, buf.Length);

                    // make sure we read some data
                    if (count != 0)
                    {
                        // translate from bytes to ASCII text
                        tempString = Encoding.ASCII.GetString(buf, 0, count);

                        // continue building the string
                        sb.Append(tempString);
                    }
                }
                while (count > 0); // any more data to read?

                Version newVersion = new Version(sb.ToString());

                if (newVersion > System.Reflection.Assembly.GetExecutingAssembly().GetName().Version)

                    mF.notify(Properties.Resources.UpdateAvailable, Properties.Resources.Version + newVersion.ToString() + Properties.Resources.IsAvailableYouAreRunning + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString());
            }
            catch (Exception)
            {
            }
        }

        #region Status Callbacks
        void batteryStrengthChanged(object sender, ChangeEventArgs args)
        {
            BatteryLevel bl = (BatteryLevel)args.NewValue;
            string batteryLevel;
            switch (bl)
            {
                case BatteryLevel.VeryLow:
                    batteryLevel = "Very Low";
                    break;
                case BatteryLevel.VeryHigh:
                    batteryLevel = "Very High";
                    break;
                case BatteryLevel.Medium:
                    batteryLevel = "Medium";
                    break;
                case BatteryLevel.Low:
                    batteryLevel = "Low";
                    break;
                case BatteryLevel.High:
                    batteryLevel = "High";
                    break;
                default:
                    batteryLevel = "";
                    break;
            }

            statusListView.Items[9].SubItems[1].Text = batteryLevel;
        }

       

        void signalStrengthChanged(object sender, ChangeEventArgs args)
        {
            try
            {
                statusListView.Items[4].SubItems[1].Text = args.NewValue.ToString();
                statusListView.Refresh();
            }
            catch (NullReferenceException)
            { }
        }

        void searchingForServiceChanged(object sender, ChangeEventArgs args)
        {
            statusListView.Items[8].SubItems[1].Text = Convert.ToBoolean(args.NewValue).ToString();
        }

        void phoneRadioOffChanged(object sender, ChangeEventArgs args)
        {
            statusListView.Items[3].SubItems[1].Text = Convert.ToBoolean(args.NewValue).ToString();
        }

        void phoneNoServiceChanged(object sender, ChangeEventArgs args)
        {
            statusListView.Items[2].SubItems[1].Text = Convert.ToBoolean(args.NewValue).ToString();
        }

        void phoneCoverageChanged(object sender, ChangeEventArgs args)
        {
            statusListView.Items[1].SubItems[1].Text = args.NewValue.ToString();
        }

        void activeSyncStatusChanged(object sender, ChangeEventArgs args)
        {
            try
            {
                statusListView.Items[0].SubItems[1].Text = ((ActiveSyncStatus)args.NewValue).ToString();
            }
            catch (NullReferenceException)
            { }
            catch (ArgumentOutOfRangeException)
            { }
        }

        void networkConnectionCountChanged(object sender, ChangeEventArgs args)
        {
            statusListView.Items[7].SubItems[1].Text = ((int)args.NewValue).ToString();
        }

        void cellularConnectionCountChanged(object sender, ChangeEventArgs args)
        {
            statusListView.Items[6].SubItems[1].Text = ((int)args.NewValue).ToString();
        }

        void connectionCountChanged(object sender, ChangeEventArgs args)
        {
            statusListView.Items[5].SubItems[1].Text = ((int)args.NewValue).ToString();
        }
        #endregion
    }
}