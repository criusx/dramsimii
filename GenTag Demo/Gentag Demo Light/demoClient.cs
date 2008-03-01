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
using System.Diagnostics;
using Microsoft.Win32;


[assembly: CLSCompliant(true)]
namespace GentagDemo
{
    public partial class demoClient : Form
    {
        #region Private Classes
        private class lookupInfo
        {
            public string tagID;
            public loopType whichLookup;
            public string extraTagValue;

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
                    for (int j = 0; j < extraTagValue.Length; ++j )
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

        #region Enumerated Types
        private enum loopType { authentication, patient, med, assay, interaction, detect, none };
        #endregion

        #region Members
        private static demoClient mF;

        private Notification popupNotification;

        private nmeaInterpreter gpsInterpreter;

        private string DeviceUID;

        private Reader tagReader;

        private TextWriter debugOut;

        /// <summary>
        /// The waiting cursor which allows the user to know that the reader is running
        /// </summary>
        private RFIDReadCursor.RFIDReadWaitCursor waitCursor;

        /// <summary>
        /// The thread which does a check to see if this is the newest version, synchronously
        /// </summary>
        private Thread versionCheckThread;

        /// <summary>
        /// The dialog box which allows the user to select
        /// </summary>
        private assayResultsChooser assayResultsDialog;

        /// <summary>
        /// The RFID of the patient currently being looked up
        /// </summary>
        private string currentPatientID;

        private TimeSpan assayTimer;

        /// <summary>
        /// The variable that describes what type of tag it's searching for
        /// </summary>
        private loopType loop;

        //private Hashtable wineBottleCache = new Hashtable();

        private Hashtable authenticationCache = new Hashtable();

        //private Hashtable petCache = new Hashtable();

        private Hashtable patientCache = new Hashtable();

        private Hashtable assayCache = new Hashtable();

        private Hashtable itemsCurrentlyBeingLookedUp = new Hashtable();

        // new tags are placed into this queue by the reader thread
        private Queue<lookupInfo> incomingTagQueue = new Queue<lookupInfo>();

        // tags are moved from incoming to lookup queues via this queue
        private Queue<lookupInfo> pendingTagQueue = new Queue<lookupInfo>();

        // tags are stored here if they are not able to be looked up currently
        private Queue<lookupInfo> lookupQueue = new Queue<lookupInfo>();

        // the maximum number of lookup threads which may run concurrently
        private const int maxLookupThreads = 2;

        // the number of lookups currently in-flight
        private int pendingLookups;

        // various web services are below
        private medWS.COREMedDemoWS COREMedDemoWebService;

        private authenticationWS.AuthenticationWebService authenticationWebService;

        private assayWS.AssayWS assayWebService;

        private int assayItemChosen = -1;

        private string sessionID;

        private System.Windows.Forms.Timer assayCountdownTimer;

        private DateTime assayEndTime = new DateTime();

        private manualTag manualDialog;

        /// <summary>
        /// Timer to retrieve tags sent by the reader thread
        /// </summary>
        private System.Threading.Timer tagSearchTimer;

        private TimerCallback tagSearchCallback;

        private AutoResetEvent tagSearchAutoEvent;

        private bool tagSearchReading;

        private string newTagID = "";

        private tagTypes newTagType = tagTypes.none;

        /// <summary>
        /// The reader indicates that it is done reading now
        /// </summary>
        //private bool doneReading;
        #endregion

        public demoClient()
        {
            // run initializations provided by the designer
            InitializeComponent();

            mF = this;

            tagReader = new Reader();

            softInputPanel.Enabled = false;

            // assayResultsDialog
            assayResultsDialog = new assayResultsChooser();

            //assayResultsDialog.Height = System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Height;

            try
            {
                DeviceUID = Reader.getDeviceUniqueID(GentagDemo.Properties.Resources.titleString);
            }
            catch (MissingMethodException)
            {
                DeviceUID = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";
            }

            // assayCountdownTimer
            assayCountdownTimer = new System.Windows.Forms.Timer();

            assayCountdownTimer.Interval = 1000;

            assayCountdownTimer.Tick += new EventHandler(assayCountdownTimer_Tick);

            //doneReading = false;

            // resize the client depending on the resolution
            if (this.ClientSize.Height > 240)
            {
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
            }

            // intialize threads
            versionCheckThread = new Thread(versionCheck);
            versionCheckThread.IsBackground = true;
            versionCheckThread.Start();
            
            tagSearchReading = true;

            tagSearchCallback = new TimerCallback(tagSearchTimer_Tick);

            tagSearchAutoEvent = new AutoResetEvent(false);            

            tagSearchTimer = new System.Threading.Timer(tagSearchCallback, tagSearchAutoEvent, 50, 500);

            // load the manual tag input dialog
            manualDialog = new manualTag();
            manualDialog.Hide();

            // set the version
            versionLabel.Text = Properties.Resources.Version + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString() + Properties.Resources.Alpha;

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
            gpsInterpreter.AltitudeReceived +=new nmeaInterpreter.AltitudeReceivedEventHandler(gpsNmea_AltitudeReceived);

            // setup event callbacks for tag reading events
            tagReader.TagReceived += new EventHandler<TagReceivedEventArgs>(receiveTag);
            tagReader.VarioSensSettingsReceived += new EventHandler<VarioSensSettingsEventArgs>(receiveVarioSensSettings);
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

            //initStatusPage();

            ///////////////////////////////////////////////////
            /////// For McKesson Demo
            ///////////////////////////////////////////////////
            //trackingCheckBox.Enabled = false;
            //scanCOMPortButton.Enabled = false;
            //connectGPSButton.Enabled = false;
            //readButton.Enabled = false;
            //writeButton.Enabled = false;

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

        /// <summary>
        /// Function called when a button is clicked that should initiate reading of a tag.
        /// Different tags require different reading methods.
        /// </summary>
        /// <param name="sender">The button or other object whose event was fired.</param>
        /// <param name="e">Parameters related to this event.</param>
        private void readerClick(object sender, EventArgs e)
        {
            if (!tagReader.Running)
            {
                // read tag ID functions
                if (sender == medicationButton || sender == readIDButton || sender == readPatientButton || sender == assayReadButton)
                {
                    if (sender == readIDButton)
                        loop = loopType.authentication;
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
                // other functions
                if (sender == detectTagTypeButton)
                {
                    loop = loopType.detect;
                    tagReader.ReadThreadType = readType.detectMany;
                }
                else if (sender == readButton)
                    tagReader.ReadThreadType = readType.readTagMemory;
                else if (sender == writeButton)
                    tagReader.ReadThreadType = readType.writeTagMemory;

                //TODO: send values for setVarioSensSettings and writeTagMemory
                try
                {
                    tagReader.Running = true;

                    setWaitCursor(true);
                }
                catch (IOException ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
            else
            {                
                setWaitCursor(false);
                tagReader.Running = false;
            }
        }

        /// <summary>
        /// Removes tags from the queue and possibly sends them off to the network
        /// Will move tags out of the incoming queue to the pending queue to clear incoming tags out
        /// Then moves unique tags from the pending queue to either the lookup queue, begins processing
        /// or throws them away if the tag is already being looked up or pending
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        void tagSearchTimer_Tick(object sender)
        {
            if (tagSearchReading && Monitor.TryEnter(pendingTagQueue))
            {
                try
                {
                    // move the messages from the incoming queue to the pending queue
                    // the should be done quickly to avoid slowing down the reader thread
                    int newTags;
                    loopType type = loopType.none;
                    lock (incomingTagQueue)
                    {
                        newTags = incomingTagQueue.Count;
                        while (incomingTagQueue.Count > 0)
                        {
                            type = incomingTagQueue.Peek().whichLookup;
                            pendingTagQueue.Enqueue(incomingTagQueue.Dequeue());
                        }
                    }

                    switch (type)
                    {
                        case loopType.assay:
                        case loopType.interaction:
                        case loopType.med:
                        case loopType.patient:
                            setWaitCursor(false);
                            break;
                    }

                    // move from the pending queue to the lookup queue, this should eliminate duplicates
                    // it's also OK for this to wait longer to acquire the lock
                    // also OK to switch threads in by updating the tree view
                    while (pendingTagQueue.Count > 0)
                        if (!lookupQueue.Contains(pendingTagQueue.Peek()))
                        {
                            if (pendingTagQueue.Peek().whichLookup == loopType.authentication)
                                updateTreeView1(pendingTagQueue.Peek().tagID, GentagDemo.Properties.Resources.NoDescriptionYet, authenticatedType.unknown);
                            lookupQueue.Enqueue(pendingTagQueue.Dequeue());
                        }
                        else
                            pendingTagQueue.Dequeue();

                    // indicate that there were new tags found
                    if (newTags > 0)
                        waitCursor.Blink();

                    startScheduledLookups();

                    
                }
                finally
                {
                    Monitor.Exit(pendingTagQueue);
                    if (!tagSearchReading)
                    {
                        tagSearchAutoEvent.Set();
                    }
                }
            }
            else
                Debug.WriteLine("had to wait");
        }


        /// <summary>
        /// Update the GUI for the auth page and detect page
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void updateGUI_Tick(object sender, EventArgs e)
        {
            // update the status labels
            setLabel(queuedLookupsLabel, lookupQueue.Count.ToString(CultureInfo.CurrentCulture));
            setLabel(pendingLookupsLabel, pendingLookups.ToString(CultureInfo.CurrentCulture));

            if (loop == loopType.detect)
            {
             //use this loop to also update the detect screen
                switch (newTagType)
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

                setLabel(detectTagIDLabel, newTagID);
            }
        }
                
        private void startScheduledLookups()
        {
            // then go through the lookup queue to see how many pending requests can be
            // sent as real requests
            while ((lookupQueue.Count > 0) && (pendingLookups < maxLookupThreads))
            {
                lookupInfo currentTag = null;
                // get the next tag
                if (lookupQueue.Count > 0)
                {
                    currentTag = lookupQueue.Dequeue();
                }
                else
                    break;

                if (string.IsNullOrEmpty(currentTag.tagID)) // if there was no string returned
                    continue;

                // then begin a lookup                
                if (!itemsCurrentlyBeingLookedUp.ContainsValue(currentTag) && (pendingLookups < maxLookupThreads))
                {
                        for (int i = scheduleLookup(currentTag.tagID, currentTag.whichLookup, currentPatientID); i > 0; i--)
                            System.Threading.Interlocked.Increment(ref pendingLookups);
                }
            }
        }

        /// <summary>
        /// The callback for when a tag is received by the reader thread
        /// </summary>
        /// <param name="tagID"></param>        
        private void receiveTag(object sender, TagReceivedEventArgs args)
        {
            switch (loop)
            {
                case loopType.assay:
                case loopType.med:
                case loopType.patient:                    
                case loopType.authentication:
                    {
                        lookupInfo newLookupInfo = new lookupInfo(args.Tag, loop, currentPatientID);

                        lock (incomingTagQueue)
                        {
                            if (!incomingTagQueue.Contains(newLookupInfo))
                                incomingTagQueue.Enqueue(newLookupInfo);
                        }
                    }
                    break;
                case loopType.detect:
                    {
                        newTagID = args.Tag;
                        newTagType = args.Type;
                    }
                    break;
            }
            //Thread.Sleep(500);
            Debug.WriteLine("Tag: " + args.Tag);

        }

        /// <summary>
        /// Creates the appropriate web service and does the lookup
        /// </summary>
        /// <param name="tagID">the RFID tag string to lookup</param>
        /// <returns>The number of requests that were sent</returns>
        private int scheduleLookup(string tagID, loopType whichLoop, string extraTag)
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
                        case loopType.authentication:
                            {
                                // if it has already been added, do nothing
                                if (authenticationCache.ContainsKey(tagID))
                                {
                                    result = 0;
                                }
                                else
                                {
                                    updateTreeView1(tagID, Properties.Resources.NoDescriptionYet, authenticatedType.unknown);

                                    lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                    {
                                        lookupInfo newInfo = new lookupInfo(tagID, loopType.authentication);
                                        // get the tag info and cache it
                                        IAsyncResult handle = authenticationWebService.BegingetItem(tagID, DeviceUID, gpsInterpreter.getLatitude(), gpsInterpreter.getLongitude(), cb, newInfo);

                                        itemsCurrentlyBeingLookedUp[handle] = newInfo;
                                    }
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

                                    currentPatientID = tagID;

                                    lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                    {
                                        lookupInfo newInfo = new lookupInfo(tagID, loopType.patient);

                                        IAsyncResult handle = COREMedDemoWebService.BegingetPatientRecord(tagID, DeviceUID, cb, newInfo);

                                        itemsCurrentlyBeingLookedUp[handle] = newInfo;
                                    }

                                }
                                break;
                            }
                        case loopType.med:
                            {
                                // patient and med RFID cannot be the same
                                if (string.IsNullOrEmpty(currentPatientID) || string.Compare(currentPatientID, tagID, true,CultureInfo.CurrentCulture) == 0)
                                    break;

                                result = 2;

                                setTextBox(drugNameBox, Properties.Resources.PleaseWait);

                                lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                {
                                    lookupInfo newInfo = new lookupInfo(tagID, loopType.med);

                                    IAsyncResult handleA = COREMedDemoWebService.BegingetDrugInfo(tagID, DeviceUID, cb, newInfo);

                                    itemsCurrentlyBeingLookedUp[handleA] = newInfo;

                                    newInfo = new lookupInfo(tagID, loopType.interaction, currentPatientID);

                                    IAsyncResult handleB = COREMedDemoWebService.BegincheckInteraction(currentPatientID, tagID, cb, newInfo);

                                    itemsCurrentlyBeingLookedUp[handleB] = newInfo;
                                }

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

                                    lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                    {
                                        lookupInfo newInfo = new lookupInfo(tagID, loopType.assay);
                                        IAsyncResult handle = assayWebService.BeginretrieveAssayInformation(tagID, DeviceUID, cb, newInfo);
                                        // to allow association of a handle back to a tag id
                                        itemsCurrentlyBeingLookedUp[handle] = newInfo;
                                    }
                                }
                                break;
                            }
                        case loopType.interaction:
                            {
                                if (string.IsNullOrEmpty(tagID) || string.IsNullOrEmpty(extraTag))
                                    break;

                                lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                {
                                    lookupInfo newInfo = new lookupInfo(tagID, loopType.interaction, extraTag);
                                    IAsyncResult handle = COREMedDemoWebService.BegincheckInteraction(extraTag, tagID, cb, newInfo);

                                    itemsCurrentlyBeingLookedUp[handle] = newInfo;
                                }

                                break;
                            }
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
                        case loopType.authentication:
                            {
                                authenticationWS.itemInfo info = authenticationWebService.EndgetItem(ar);

                                if (info.retryNeeded)
                                    throw new WebException();

                                // display the tag info
                                if (info != null && info.RFIDNum != null)
                                {

                                    updateTreeView1(lookupInformation.tagID, info.description, info.authenticated ? authenticatedType.yes : authenticatedType.no);

                                    lock (authenticationCache.SyncRoot)
                                    { authenticationCache[info.RFIDNum] = info; }
                                }
                            }
                            break; 

                        case loopType.med:
                            {
                                medWS.drugInfo drugInf = COREMedDemoWebService.EndgetDrugInfo(ar);

                                if (drugInf.retryNeeded)
                                    throw new WebException();

                                if (drugInf != null)
                                    displayDrug(drugInf);
                            }
                            break;

                        case loopType.interaction:
                            {
                                medWS.errorReport er = COREMedDemoWebService.EndcheckInteraction(ar);

                                if (er.retryNeeded)
                                    throw new WebException();

                                if (er.errorCode > 0)
                                    notify(Properties.Resources.Alert, er.errorMessage, true);
                            }
                            break;

                        case loopType.patient:
                            {
                                medWS.patientRecord patientInfo = COREMedDemoWebService.EndgetPatientRecord(ar);

                                if (patientInfo.retryNeeded)
                                    throw new WebException();

                                if (patientInfo.RFIDnum != null)
                                {
                                    lock (patientCache.SyncRoot)
                                    { patientCache[patientInfo.RFIDnum] = patientInfo; }
                                }

                                if (patientInfo.RFIDnum == currentPatientID)
                                    displayPatient(patientInfo);
                            }
                            break;

                        case loopType.assay:
                            {
                                assayWS.assayInfo assayRecord = assayWebService.EndretrieveAssayInformation(ar);

                                if (assayRecord.needRetry)
                                    throw new WebException();


                                displayAssay(assayRecord);

                                if (assayRecord.rfidNum != null)
                                {
                                    lock (assayCache.SyncRoot)
                                    { assayCache[assayRecord.rfidNum] = assayRecord; }
                                }
                            }
                            break;

                        default:
                            break;
                    }

                    // free up some space in the available lookups and schedule a waiting lookup
                    System.Threading.Interlocked.Decrement(ref pendingLookups);
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
        /// Performs a lookup again if the previous lookup has failed
        /// </summary>
        /// <param name="ar"></param>
        private void rescheduleLookup(IAsyncResult ar)
        {
            lookupInfo itemInfo = (lookupInfo)ar.AsyncState;
            scheduleLookup(itemInfo.tagID, itemInfo.whichLookup, itemInfo.extraTagValue);
        }

        /// <summary>
        /// The destructor for the class, ensures that the tag reader quits and that the debug text file is closed
        /// </summary>
        ~demoClient()
        {            
            Dispose(false);
        }

        //protected virtual void Dispose(bool disposing)
        //{
        //    if (!disposing)
        //    {
        //        tagReader.Running = false;
        //        tagSearchReading = false;

        //        if (tagSearchTimer != null)
        //        {
        //            tagSearchAutoEvent.WaitOne(500, false);
        //            tagSearchTimer.Dispose();
        //        }
        //        debugOut.Dispose();
        //    }
        //}

        private void receiveTagContents(object sender, TagContentsEventArgs args)
        {
            setWaitCursor(false);
            setTextBox(readWriteTB, args.Contents);
        }

        private void writeTagMemory()
        {
            tagReader.writeTag(System.Text.Encoding.Unicode.GetBytes(getTextBox(readWriteTB)));
        }

        private void doneWriting(object sender, FinishedWritingStatusEventArgs args)
        {
            setLabel(readWriteStatusLabel, Properties.Resources.Status + args.Status);
            setWaitCursor(false);
        }

        #region Display Results
        void displayTagType(object sender, TagTypeEventArgs args)
        {
        }

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

                        assayTimerLabel.Text = string.Empty;

                        sessionID = null;

                        assayResultsDialog.setImages(null);

                        assayResultsDialog.setAfterMessage(null);

                        setPhoto(assayImagePictureBox, (System.Drawing.Image)null);
                    }
                }
                else
                {
                    assayMessageLabel.Text = string.Empty;

                    assayTimerLabel.Text = string.Empty;

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
                if (drug != null)
                {
                    if (drug.exists && drug.picture != null)
                    {
                        setPhoto(drugPhoto, drug.picture);
                        setTextBox(drugNameBox, drug.name);
                    }
                    else if (!drug.exists)
                    {
                        setPhoto(drugPhoto, (System.Drawing.Image)null);
                        setTextBox(drugNameBox, Properties.Resources.NotFound);
                    }
                }
                else
                {
                    setPhoto(drugPhoto, (System.Drawing.Image)null);
                    setTextBox(drugNameBox, string.Empty);
                }
            }
            catch (Exception)
            {
                setPhoto(drugPhoto, (System.Drawing.Image)null);
                setTextBox(drugNameBox, string.Empty);
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
                setTextBox(patientNameBox, string.Empty);
                setTextBox(patientDescriptionBox, string.Empty);
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
                    setTextBox(patientNameBox, string.Empty);
                    setTextBox(patientDescriptionBox, GentagDemo.Properties.Resources.patientInfoNotFound);
                    setPhoto(patientPhoto, (Image)null);
                }
            }
            else
            {
                setTextBox(patientNameBox, GentagDemo.Properties.Resources.NotFound);
                setTextBox(patientDescriptionBox, string.Empty);
                setPhoto(patientPhoto, (Image)null);
            }
        }

        enum authenticatedType { unknown, yes, no };

        private delegate void updateTreeView1Delegate(string currentTag, string rfidDescr, authenticatedType authType);

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void updateTreeView1(string currentTag, string rfidDescr, authenticatedType authType)
        {
            if (authTreeView.InvokeRequired)
            {
                authTreeView.BeginInvoke(new updateTreeView1Delegate(updateTreeView1),
                    new object[] { currentTag, rfidDescr, authType });
            }
            else
            {
                bool exists = false;

                foreach (TreeNode currentTreeNode in authTreeView.Nodes)
                {
                    if (string.Compare(currentTreeNode.Text,currentTag,StringComparison.OrdinalIgnoreCase) == 0)
                    {
                        exists = true;
                        if (authType == authenticatedType.no || authType == authenticatedType.yes)
                        {
                            authTreeView.BeginUpdate();
                            // if it exists and wants to be Yes/No, then update this entry
                            currentTreeNode.Nodes.Clear();
                            TreeNode tN = new TreeNode(rfidDescr);
                            currentTreeNode.SelectedImageIndex = currentTreeNode.ImageIndex = authType == authenticatedType.yes ? 2 : 1;
                            currentTreeNode.Nodes.Add(tN);
                            authTreeView.EndUpdate();
                        }
                    }
                }
                //if (!exists && (authType == authenticatedType.yes || authType == authenticatedType.no))
                //    MessageBox.Show("adding yes/no and there is no existing item");
                if (!exists)
                {
                    if (authType == authenticatedType.unknown)
                    {
                        authTreeView.BeginUpdate();
                        TreeNode superTn = new TreeNode(currentTag);
                        superTn.SelectedImageIndex = superTn.ImageIndex = 3;

                        TreeNode subTn = new TreeNode(rfidDescr);
                        subTn.SelectedImageIndex = subTn.ImageIndex = 0;

                        superTn.Nodes.Add(subTn);
                        authTreeView.Nodes.Add(superTn);
                        authTreeView.EndUpdate();
                    }
                }
            }

        }
        #endregion


        /// <summary>
        /// When a key is pressed this creates shortcuts to certain functions
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void mainForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == System.Windows.Forms.Keys.Back)
            {
                clearDemo(null, null);
            }
            else if (e.KeyCode == System.Windows.Forms.Keys.F1)
            {
                if (mainTabControl.TabPages[mainTabControl.SelectedIndex].Name == "patientPage")
                    readerClick(readPatientButton, new EventArgs());
            }
            else if (e.KeyCode == System.Windows.Forms.Keys.F2)
            {
                if (mainTabControl.TabPages[mainTabControl.SelectedIndex].Name == "authPage")
                    readerClick(readIDButton, new EventArgs());
                else if (mainTabControl.TabPages[mainTabControl.SelectedIndex].Name == "patientPage")
                    readerClick(medicationButton, new EventArgs());
                else if (mainTabControl.TabPages[mainTabControl.SelectedIndex].Name == "assayPage")
                    readerClick(assayReadButton, new EventArgs());
            }
        }

        /// <summary>
        /// Clears all the pages to their original state
        /// </summary>
        ///
        private void clearDemo(object sender, EventArgs e)
        {
            authTreeView.BeginUpdate();
            authTreeView.Nodes.Clear();
            authTreeView.EndUpdate();

            authenticationCache.Clear();
            //wineBottleCache.Clear();
            //petCache.Clear();
            patientCache.Clear();
            assayCache.Clear();

            lock (itemsCurrentlyBeingLookedUp.SyncRoot)
            {
                itemsCurrentlyBeingLookedUp.Clear();
            }
            while (pendingLookups > 0)
                System.Threading.Interlocked.Decrement(ref pendingLookups);
            lock (lookupQueue)
            {
                setLabel(pendingLookupsLabel, pendingLookups.ToString(CultureInfo.CurrentCulture));
                lookupQueue.Clear();
                setLabel(queuedLookupsLabel, lookupQueue.Count.ToString(CultureInfo.CurrentCulture));

            }
            setLabel(detectTagIDLabel, string.Empty);
            setLabel(detectTagTypeLabel, string.Empty);
            displayDrug(null);
            displayPatient(null);
            displayAssay(null);
        }


        private void trackingCheckBox_CheckStateChanged(object sender, EventArgs e)
        {
            gpsInterpreter.Tracking = trackingCheckBox.Checked;
        }


        private void tabControl1_SelectedIndexChanged(object sender, EventArgs e)
        {
            setWaitCursor(false);
            tagReader.Running = false;
        }

        private void manualLookupClick(object sender, EventArgs e)
        {
            softInputPanel.Enabled = true;

            if (manualDialog.ShowDialog() == DialogResult.Yes)
            {
                loop = loopType.authentication;
                receiveTag(this, new TagReceivedEventArgs(manualDialog.Value, tagTypes.iso15693));
            }

            softInputPanel.Enabled = false;
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
            // Init the Registry
            RegistryKey regKey = Registry.LocalMachine;

            regKey = regKey.OpenSubKey(@"System");
            regKey = regKey.OpenSubKey(@"CurrentControlSet");
            regKey = regKey.OpenSubKey(@"GPS Intermediate Driver");
            int intermediateDriverEnabled = (int)regKey.GetValue("IsEnabled");
            string gpsComPort = string.Empty;
            if (intermediateDriverEnabled == 1)
            {
                regKey = regKey.OpenSubKey(@"Multiplexer");
                gpsComPort = (string)regKey.GetValue("DriverInterface");
            }
            else
            {
                regKey = regKey.OpenSubKey(@"Drivers");
                string currentDriver = (string)regKey.GetValue("CurrentDriver");
                regKey = regKey.OpenSubKey(currentDriver);
                gpsComPort = (string)regKey.GetValue(@"CommPort");
                
            }

            Hashtable comPorts = new Hashtable();

            gpsComPort = gpsComPort.Substring(0,4);

            comPorts.Add(gpsComPort, true);
            

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

            Array.Sort(COMPorts);

            foreach (string s in COMPorts)
            {
                comPorts[s]= true;
            }

            comPortsComboBox.Items.Clear();
            
            foreach (string s in comPorts.Keys)
            {                
                comPortsComboBox.Items.Add(s);
                if (string.Compare(s, gpsComPort) == 0)
                    comPortsComboBox.SelectedItem = s;
             
            }            
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

            while (trials < 10)
            {
                try
                {
                    assayWS.assayResultResponse response = assayWebService.submitAssayResult(sessionID, DeviceUID, assayItemChosen + 1);

                    if (response.needRetry)
                    { }
                    else if (!response.success)
                    {
                        MessageBox.Show(Properties.Resources.FailedToSubmit);
                    }
                    else
                    {
                        notify(Properties.Resources.Notice, Properties.Resources.ResultsRecorded, false);
                        assayItemChosen = -1;
                        setButtonEnabled(assayRecordButton, false);
                        return;
                    }
                }
                catch (WebException ex)
                {
                    debugOut.WriteLine(ex.StackTrace);

                    if (trials > 10)
                        notify(Properties.Resources.ConnectionProblem, Properties.Resources.PossibleNetworkErrorPleaseTryAgain, false);
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
            setButtonEnabled(assayBeginButton, false);
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

        private void notification1_ResponseSubmitted(object sender, Microsoft.WindowsCE.Forms.ResponseSubmittedEventArgs e)
        {
            popupNotification.Visible = false;
        }

        private void initStatusPage()
        {
            try
            {
                SystemState systemState;

                systemState = new SystemState(SystemProperty.ConnectionsCount);
                systemState.Changed += new ChangeEventHandler(connectionCountChanged);
                ListViewItem.ListViewSubItem lvi = new ListViewItem.ListViewSubItem();
                lvi.Text = ((int)systemState.CurrentValue).ToString(CultureInfo.CurrentCulture);
                statusListView.Items[5].SubItems.Add(lvi);

                systemState = new SystemState(SystemProperty.ConnectionsCellularCount);
                systemState.Changed += new ChangeEventHandler(cellularConnectionCountChanged);
                lvi = new ListViewItem.ListViewSubItem();
                lvi.Text = ((int)systemState.CurrentValue).ToString(CultureInfo.CurrentCulture);
                statusListView.Items[6].SubItems.Add(lvi);

                systemState = new SystemState(SystemProperty.ConnectionsNetworkCount);
                systemState.Changed += new ChangeEventHandler(networkConnectionCountChanged);
                lvi = new ListViewItem.ListViewSubItem();
                lvi.Text = ((int)systemState.CurrentValue).ToString(CultureInfo.CurrentCulture);
                statusListView.Items[7].SubItems.Add(lvi);

                try
                {
                    systemState = new SystemState(SystemProperty.ActiveSyncStatus);
                    systemState.Changed += new ChangeEventHandler(activeSyncStatusChanged);
                    lvi = new ListViewItem.ListViewSubItem();
                    lvi.Text = ((ActiveSyncStatus)systemState.CurrentValue).ToString(CultureInfo.CurrentCulture);
                    statusListView.Items[0].SubItems.Add(lvi);
                }
                catch (NullReferenceException)
                { }
                catch (ArgumentOutOfRangeException)
                { }

                systemState = new SystemState(SystemProperty.Phone1xRttCoverage);
                systemState.Changed += new ChangeEventHandler(phoneCoverageChanged);
                lvi = new ListViewItem.ListViewSubItem();
                lvi.Text = Convert.ToBoolean(systemState.CurrentValue, CultureInfo.CurrentCulture).ToString(CultureInfo.CurrentCulture);
                statusListView.Items[1].SubItems.Add(lvi);

                systemState = new SystemState(SystemProperty.PhoneNoService);
                systemState.Changed += new ChangeEventHandler(phoneNoServiceChanged);
                lvi = new ListViewItem.ListViewSubItem();
                lvi.Text = Convert.ToBoolean(systemState.CurrentValue, CultureInfo.CurrentCulture).ToString();
                statusListView.Items[2].SubItems.Add(lvi);

                systemState = new SystemState(SystemProperty.PhoneRadioOff);
                systemState.Changed += new ChangeEventHandler(phoneRadioOffChanged);
                lvi = new ListViewItem.ListViewSubItem();
                lvi.Text = Convert.ToBoolean(systemState.CurrentValue, CultureInfo.CurrentCulture).ToString();
                statusListView.Items[3].SubItems.Add(lvi);

                systemState = new SystemState(SystemProperty.PhoneSearchingForService);
                systemState.Changed += new ChangeEventHandler(searchingForServiceChanged);
                lvi = new ListViewItem.ListViewSubItem();
                lvi.Text = Convert.ToBoolean(systemState.CurrentValue, CultureInfo.CurrentCulture).ToString();
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
            catch (TypeLoadException)
            {

            }
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
                    mF.notify(Properties.Resources.UpdateAvailable, Properties.Resources.Version + newVersion.ToString() + Properties.Resources.IsAvailableYouAreRunning + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString(), true);
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
            statusListView.Items[8].SubItems[1].Text = Convert.ToBoolean(args.NewValue, CultureInfo.CurrentCulture).ToString();
        }

        void phoneRadioOffChanged(object sender, ChangeEventArgs args)
        {
            statusListView.Items[3].SubItems[1].Text = Convert.ToBoolean(args.NewValue, CultureInfo.CurrentCulture).ToString();
        }

        void phoneNoServiceChanged(object sender, ChangeEventArgs args)
        {
            statusListView.Items[2].SubItems[1].Text = Convert.ToBoolean(args.NewValue, CultureInfo.CurrentCulture).ToString();
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
            statusListView.Items[7].SubItems[1].Text = ((int)args.NewValue).ToString(CultureInfo.CurrentCulture);
        }

        void cellularConnectionCountChanged(object sender, ChangeEventArgs args)
        {
            statusListView.Items[6].SubItems[1].Text = ((int)args.NewValue).ToString(CultureInfo.CurrentCulture);
        }

        void connectionCountChanged(object sender, ChangeEventArgs args)
        {
            statusListView.Items[5].SubItems[1].Text = ((int)args.NewValue).ToString(CultureInfo.CurrentCulture);
        }
        #endregion

    }
}