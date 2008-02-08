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
using System.Threading;
using System.Web.Services.Protocols;
using System.Windows.Forms;
using NMEA;
using RFIDReader;
using TestPocketGraphBar;
using System.Text;


[assembly: CLSCompliant(true)]
//[assembly: SecurityPermission(SecurityAction.RequestMinimum, Execution = true)]
namespace GentagDemo
{
    public partial class demoClient : Form
    {
        static demoClient mF;

        Graph graph = new Graph();

        Graph radGraph = new Graph();

        nmeaInterpreter gpsInterpreter;

        string DeviceUID;

        Reader tagReader;

        TextWriter debugOut;

        private RFIDReadCursor.RFIDReadWaitCursor userControl11;

        Thread versionCheckThread;

        assayResultsChooser assayResultsDialog;

        public demoClient()
        {
            // run initializations provided by the designer
            InitializeComponent();

            tagReader = new Reader();

            // assayResultsDialog
            assayResultsDialog = new assayResultsChooser();

            assayResultsDialog.Height = System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Height;

            DeviceUID = Reader.getDeviceUniqueID(GentagDemo.Properties.Resources.titleString);
            
            // assayCountdownTimer
            assayCountdownTimer = new System.Windows.Forms.Timer();

            assayCountdownTimer.Interval = 1000;

            assayCountdownTimer.Tick += new EventHandler(assayCountdownTimer_Tick);

            // open comm briefly to preload the assemblies
            Reader.initialize();

            this.ClientSize = new Size(System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Width,System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Width);
            tabControl1.Size = this.ClientSize;
            foreach (TabPage tP in tabControl1.TabPages)
            {
                tP.Size = new Size(this.ClientSize.Width,Convert.ToInt32(Convert.ToSingle(this.ClientSize.Height) / 240F) * tP.Size.Height);
                foreach (Control childControl in tP.Controls)
                {

                    if (childControl.GetType() == typeof(PictureBox) || childControl.GetType() == typeof(TreeView))
                    {
                        // special case
                        if (childControl == wineAuthPictureBox)
                            continue;
                        int newValue = this.ClientSize.Height - 240 + childControl.Height;
                        childControl.Height = newValue;
                        //childControl.Size = new Size(childControl.Size.Width, this.ClientSize.Height / 240 * childControl.Size.Height);
                    }
                    //childControl.Location = new Point(childControl.Location.X, this.ClientSize.Height / 240 * childControl.Location.Y);
                    //childControl.Location.Y = this.ClientSize.Height / 240 * childControl.Location.Y;
                }
            }

            // intialize threads
            versionCheckThread = new Thread(versionCheck);
            versionCheckThread.IsBackground = true;
            versionCheckThread.Start();

            // set the version
            versionLabel.Text = "Version " + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString();

            // initialize the debug stream
            debugOut = new StreamWriter("debug.txt", false);

            // userControl11
            int systemWidth = System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Width;
            int systemHeight = System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Height;
            this.userControl11 = new RFIDReadCursor.RFIDReadWaitCursor();
            this.userControl11.Location = new System.Drawing.Point((systemWidth - userControl11.Size.Width)/2, (systemHeight - userControl11.Size.Height)/2);
            this.userControl11.Name = "userControl11";
            this.userControl11.Size = new System.Drawing.Size(72, 72);



            // generate the device's UID
            string AppString =  Reader.getDeviceUniqueID(GentagDemo.Properties.Resources.titleString);

            gpsInterpreter = new nmeaInterpreter(CultureInfo.CurrentCulture, DeviceUID);

            mF = this;

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

            ImageList myImageList = new ImageList();
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.blank.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.cancel.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.ok.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.question.GetHbitmap()));
            authTreeView.ImageList = myImageList;

            // web service inits
            const int timeout = 15000;
            petWebService = new petWS.petWS();
            wineWebService = new wineWS.wineWS();
            COREMedDemoWebService = new medWS.COREMedDemoWS();
            authenticationWebService = new authenticationWS.AuthenticationWebService();
            assayWebService = new assayWS.AssayWS();
            petWebService.Timeout = timeout;
            wineWebService.Timeout = timeout;
            COREMedDemoWebService.Timeout = timeout;
            authenticationWebService.Timeout = timeout;
            assayWebService.Timeout = timeout;
            authenticationWebService.ConnectionGroupName = "authentication";
            wineWebService.ConnectionGroupName = "wine";
            petWebService.ConnectionGroupName = "pet";
            COREMedDemoWebService.ConnectionGroupName = "med";
            assayWebService.ConnectionGroupName = "assay";
            authenticationWebService.EnableDecompression = true;

            graph.Visible = false;
            radGraph.Visible = false;
            graph.Size = new Size(240, 230);
            radGraph.Size = new Size(240, 230);
            graph.Location = new Point(0, 0);
            radGraph.Location = new Point(0, 0);
            VarioSens.Controls.Add(graph);
            radPage.Controls.Add(radGraph);



            //////////////////////////////////////////////////////////////////////////
            // for wine demo only
            //////////////////////////////////////////////////////////////////////////
            //tabControl1.TabPages.RemoveAt(1);
            //tabControl1.TabPages.RemoveAt(1);
            //tabControl1.TabPages.RemoveAt(3);
            //tabControl1.TabPages.RemoveAt(3);
            //tabControl1.TabPages.RemoveAt(2);
            //////////////////////////////////////////////////////////////////////////


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
                    MessageBox.Show("Version " + newVersion.ToString() + " is available. You are running " + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString() + ".");
            }
            catch (Exception)
            {
            }
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

        private enum loopType { wine, counterfeit, pet, patient, med, test, assay, none };

        // the variable that describes whether it's looping looking for wine bottles or general tags
        private loopType loop;


        private void readerClick(object sender, EventArgs e)
        {
            if (tagReader.Running == false)
            {
                setWaitCursor(true);

                if (sender == medicationButton || sender == readIDButton || sender == wineButton || sender == petButton || sender == readPatientButton || sender == readTestButton || sender == assayReadButton)
                {
                    if (sender == wineButton)
                        loop = loopType.wine;
                    else if (sender == petButton)
                        loop = loopType.pet;
                    else if (sender == readIDButton)
                        loop = loopType.counterfeit;
                    else if (sender == readPatientButton)
                        loop = loopType.patient;
                    else if (sender == medicationButton)
                        loop = loopType.med;
                    else if (sender == readTestButton)
                        loop = loopType.test;
                    else if (sender == assayReadButton)
                        loop = loopType.assay;

                    // whether it is a loop lookup or a single item lookup
                    if (sender == readPatientButton || sender == medicationButton || sender == readTestButton || sender == assayReadButton)
                        tagReader.ReadThreadType = readType.readOne;
                    else
                        tagReader.ReadThreadType = readType.readMany;

                }
                else if (sender == readLogButton)
                    tagReader.ReadThreadType = readType.readVarioSensLog;
                else if (sender == detectTagTypeButton)
                    tagReader.ReadThreadType = readType.detectMany;
                else if (sender == readButton)
                    tagReader.ReadThreadType = readType.readTagMemory;
                else if (sender == writeButton)
                    tagReader.ReadThreadType = readType.writeTagMemory;
                else if (sender == setValueButton)
                    tagReader.ReadThreadType = readType.setVarioSensSettings;
                else if (sender == readValueButton)
                    tagReader.ReadThreadType = readType.getVarioSensSettings;
                else if (sender == radScanButton)
                    tagReader.ReadThreadType = readType.radiationScan;

                //TODO: send values for setVarioSensSettings and writeTagMemory
                tagReader.Running = true;
            }
            else
            {
                tagReader.Running = false;
                stopReading();
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
            MessageBox.Show(args.Reason);            
        }

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
        /// The web service endpoints
        /// </summary>
        petWS.petWS petWebService;
        wineWS.wineWS wineWebService;
        medWS.COREMedDemoWS COREMedDemoWebService;
        authenticationWS.AuthenticationWebService authenticationWebService;
        assayWS.AssayWS assayWebService;


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
                                    if (handle == null)
                                        MessageBox.Show("Null handle");
                                    lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                    { itemsCurrentlyBeingLookedUp.Add(handle, tagID); }

                                    updateTreeView1(tagID, Properties.Resources.NoDescriptionYet, false, true, true);
                                }
                                break;
                            }
                        case loopType.pet:
                            {
                                if (petCache.ContainsKey(tagID))
                                {
                                    result = 0;
                                    displayPet((petWS.petInfo)petCache[tagID]);
                                }
                                else
                                {
                                    IAsyncResult handle = petWebService.BeginretrievePetInformation(tagID, DeviceUID, gpsInterpreter.getLatitude(), gpsInterpreter.getLongitude(), cb, petWebService);
                                    lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                    { itemsCurrentlyBeingLookedUp[handle] = tagID; }
                                }

                                break;
                            }
                        case loopType.wine:
                            {
                                if (wineBottleCache.ContainsKey(tagID))
                                {
                                    result = 0;
                                    displayBottle((wineWS.wineBottle)wineBottleCache[tagID]);
                                }
                                else
                                {
                                    mostRecentWineID = tagID;

                                    IAsyncResult handle = wineWebService.BeginretrieveBottleInformation(tagID, DeviceUID, gpsInterpreter.getLatitude(), gpsInterpreter.getLongitude(), cb, wineWebService);
                                    lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                    { itemsCurrentlyBeingLookedUp[handle] = tagID; }
                                }
                                break;
                            }
                        case loopType.test:
                            {
                                if (petCache.ContainsKey(tagID))
                                {
                                    result = 0;
                                    displayTest((petWS.petInfo)petCache[tagID]);
                                }
                                else
                                {
                                    IAsyncResult handle = petWebService.BeginretrievePetInformation(tagID, DeviceUID, gpsInterpreter.getLatitude(), gpsInterpreter.getLongitude(), cb, petWebService);
                                    lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                    { itemsCurrentlyBeingLookedUp[handle] = tagID; }
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
                                    IAsyncResult handle = COREMedDemoWebService.BegingetPatientRecord(tagID, cb, COREMedDemoWebService);
                                    lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                    { itemsCurrentlyBeingLookedUp[handle] = tagID; }
                                    currentPatientID = tagID;
                                }
                                break;
                            }
                        case loopType.med:
                            {
                                if (string.IsNullOrEmpty(currentPatientID))
                                    break;

                                result = 2;

                                IAsyncResult handle = COREMedDemoWebService.BegingetDrugInfo(tagID, cb, COREMedDemoWebService);
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
                                    IAsyncResult handle = assayWebService.BeginretrieveAssayInformation(tagID, DeviceUID, cb, assayWebService);
                                    // to allow association of a handle back to a tag id
                                    lock (itemsCurrentlyBeingLookedUp.SyncRoot)
                                    { itemsCurrentlyBeingLookedUp[handle] = tagID; }
                                }
                                break;
                            }
                        default:
                            MessageBox.Show("error");
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

        private Hashtable retryCount = new Hashtable();

        private string mostRecentWineID;

        private string currentPatientID;

        private TimeSpan assayTimer;

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
                    else if (ar.AsyncState.GetType() == typeof(wineWS.wineWS))
                    {
                        // try to cast as wineBottle
                        wineWS.wineWS ws = (wineWS.wineWS)ar.AsyncState;
                        wineWS.wineBottle bottle = ws.EndretrieveBottleInformation(ar);
                        lock (wineBottleCache.SyncRoot)
                        {
                            wineBottleCache[bottle.rfidNum] = bottle;
                        }
                        if (bottle.rfidNum == mostRecentWineID)
                        {
                            displayBottle(bottle);
                        }
                    }
                    else if (ar.AsyncState.GetType() == typeof(petWS.petWS))
                    {
                        // try to cast as petInfo
                        petWS.petWS ws = (petWS.petWS)ar.AsyncState;
                        petWS.petInfo newPet = ws.EndretrievePetInformation(ar);

                        if (newPet != null && newPet.exists == true && newPet.rfidNum != null)
                        {
                            // may be the test result
                            if (newPet.owner == "anthrax")
                                displayTest(newPet);
                            else
                                displayPet(newPet);
                            lock (petCache.SyncRoot)
                            {
                                petCache[newPet.rfidNum] = newPet;
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

                            if (drugInf != null && drugInf.exists)
                                displayDrug(drugInf);
                        }
                        catch (InvalidCastException e)
                        {
                            debugOut.WriteLine(e.ToString() + "@" + e.StackTrace);
                            try
                            {
                                bool alert = ws.EndcheckInteraction(ar);
                                if (alert)
                                    MessageBox.Show(Properties.Resources.DrugInteractionWarningMessage);
                            }
                            catch (InvalidCastException ex)
                            {
                                debugOut.WriteLine(ex.ToString() + "@" + ex.StackTrace);
                                medWS.patientRecord info = ws.EndgetPatientRecord(ar);

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
                            debugOut.WriteLine(e.ToString() + "@" + e.StackTrace);
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
                debugOut.WriteLine(e.ToString() + "@" + e.StackTrace);
                rescheduleLookup(ar);
            }
            catch (ArgumentNullException e)
            {
                debugOut.WriteLine(e.ToString() + "@" + e.StackTrace);
                rescheduleLookup(ar);
            }
            catch (MemberAccessException e)
            {
                debugOut.WriteLine(e.ToString() + "@" + e.StackTrace);
                MessageBox.Show("unexpected: ");
            }
            catch (SocketException e)
            {
                debugOut.WriteLine(e.ToString() + "@" + e.StackTrace);
                Thread.Sleep(2000);
            }
            catch (IOException e)
            {
                debugOut.WriteLine(e.ToString() + "@" + e.StackTrace);
                return;
            }
            catch (InvalidCastException e)
            {
                debugOut.WriteLine(e.ToString() + "@" + e.StackTrace);
            }
            catch (InvalidOperationException e)
            {
                debugOut.WriteLine(e.ToString() + "@" + e.StackTrace);
            }
            catch (IndexOutOfRangeException e)
            {
                debugOut.WriteLine(e.ToString() + "@" + e.StackTrace);
            }
            catch (NullReferenceException e)
            {
                debugOut.WriteLine(e.ToString() + "@" + e.StackTrace);
                rescheduleLookup(ar);
            }
            catch (Exception e)
            {
                debugOut.WriteLine(e.ToString() + "@" + e.StackTrace);
            }
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

            if (ar.AsyncState.GetType() == typeof(wineWS.wineWS))
            {
                scheduleLookup(currentTag, loopType.wine);
            }
            else if (ar.AsyncState.GetType() == typeof(petWS.petWS))
            {
                scheduleLookup(currentTag, loopType.pet);
            }
            else if (ar.AsyncState.GetType() == typeof(authenticationWS.AuthenticationWebService))
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

                if (info.exists)
                {
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
                    assayMessageLabel.Text = "Assay not found.";

                    assayTimerLabel.Text = GentagDemo.Properties.Resources.emptyString;

                    sessionID = null;

                    assayResultsDialog.setImages(null);

                    assayResultsDialog.setAfterMessage(null);

                    setPhoto(assayImagePictureBox,(System.Drawing.Image) null);
                }
                assayPage.ResumeLayout();
            }
        }

        private delegate void displayBottleDelegate(wineWS.wineBottle bottle);

        /// <summary>
        /// Displays the wine bottle
        /// </summary>
        /// <param name="bottle">Wine bottle information provided by the web service</param>
        private void displayBottle(wineWS.wineBottle bottle)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new displayBottleDelegate(displayBottle), new object[] {bottle});
            }
            else
            {
                winePage.SuspendLayout();

                if (bottle.exists)
                {
                    wineCountryLabel.Text = bottle.origin;
                    wineYearLabel.Text = bottle.year.ToString(CultureInfo.CurrentUICulture);
                    wineTypeLabel.Text = bottle.type;
                    wineVineyardLabel.Text = bottle.vineyard;
                    wineReviewTextBox.Text = bottle.review;
                    setPhoto(winePictureBox, bottle.image);
                    
                    if (bottle.authenticated)
                        setPhoto(wineAuthPictureBox, Image.FromHbitmap(GentagDemo.Properties.Resources.ok.GetHbitmap()));
                    else
                        setPhoto(wineAuthPictureBox, Image.FromHbitmap(GentagDemo.Properties.Resources.cancel.GetHbitmap()));
                }
                else
                {
                    setPhoto(winePictureBox, (System.Drawing.Image)null);
                    setPhoto(wineAuthPictureBox, Image.FromHbitmap(GentagDemo.Properties.Resources.cancel.GetHbitmap()));

                    wineCountryLabel.Text = GentagDemo.Properties.Resources.emptyString;
                    wineYearLabel.Text = GentagDemo.Properties.Resources.emptyString;
                    wineTypeLabel.Text = GentagDemo.Properties.Resources.emptyString;
                    wineVineyardLabel.Text = GentagDemo.Properties.Resources.emptyString;
                    wineReviewTextBox.Text = GentagDemo.Properties.Resources.emptyString;
                }

                winePage.ResumeLayout();
            }
        }
        
        private void displayDrug(medWS.drugInfo drug)
        {
            try
            {
                if (drug != null && drug.exists && drug.picture != null)
                    setPhoto(drugPhoto, drug.picture);
                else
                    setPhoto(drugPhoto, (System.Drawing.Image)null);
            }
            catch (Exception)
            {
                setPhoto(drugPhoto, (System.Drawing.Image)null);
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
                setTextBox(patientDescriptionBox, GentagDemo.Properties.Resources.patientInfoNotFound);
                setPhoto(patientPhoto, (Image)null);
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
                setTextBox(patientDescriptionBox, GentagDemo.Properties.Resources.patientInfoNotFound);
                setPhoto(patientPhoto, (Image)null);
            }
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void displayPet(petWS.petInfo newPet)
        {
            // display the bottle info
            if (newPet.exists)
            {
                setLabel(petOwnerLabel, newPet.owner);
                setLabel(petContactInfoLabel, newPet.contactInfo);
                setLabel(petPhonenumberLabel, newPet.homePhone);
                setLabel(petBreedLabel, newPet.breed);
                setPhoto(petPB, newPet.image);
            }
            else
            {
                setLabel(petOwnerLabel, GentagDemo.Properties.Resources.emptyString);
                setLabel(petContactInfoLabel, GentagDemo.Properties.Resources.emptyString);
                setLabel(petPhonenumberLabel, GentagDemo.Properties.Resources.emptyString);
                setLabel(petBreedLabel, GentagDemo.Properties.Resources.emptyString);
                setPhoto(petPB, (System.Drawing.Image)null);
            }
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void displayTest(petWS.petInfo newPet)
        {
            if (newPet != null && newPet.image != null && newPet.image.Length > 16)
                setPhoto(testDescriptionPictureBox, newPet.image);
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
                if (tabControl1.TabPages[tabControl1.SelectedIndex].Name == "authPage")
                    readerClick(readIDButton, new EventArgs());
                else if (tabControl1.TabPages[tabControl1.SelectedIndex].Name == "winePage")
                    readerClick(wineButton, new EventArgs());
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

            itemsCurrentlyBeingLookedUp.Clear();

            displayDrug(null);
            displayPatient(null);
        }

        private void manualLookupClick(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;

            textBox4.Enabled = false;

            authenticationWS.AuthenticationWebService ws = new authenticationWS.AuthenticationWebService();

            string rfidDescr = Properties.Resources.NoDescriptionFound;

            try
            {
                authenticationWS.itemInfo values = ws.getItem(textBox4.Text, DeviceUID, 0, 0);
                rfidDescr = values.description;
            }
            catch (WebException ex)
            {
                MessageBox.Show(Properties.Resources.ProblemConnectingToWebService + ex.Message);
            }

            updateTreeView1(textBox4.Text, rfidDescr, false, false, true);

            textBox4.Enabled = true;

            Cursor.Current = Cursors.Default;
        }

        private void trackingCheckBox_CheckStateChanged(object sender, EventArgs e)
        {
            gpsInterpreter.setTracking(trackingCheckBox.Checked);
        }

        private void wineReviewTextBox_TextChanged(object sender, EventArgs e)
        {
            wineReviewTextBox.Visible = false;
        }

        private void tabControl1_SelectedIndexChanged(object sender, EventArgs e)
        {
            stopReading();
        }

        private void textBox4_GotFocus(object sender, EventArgs e)
        {
            textBox4.Location = new Point(8, 0);
            textBox4.BringToFront();
            inputPanel1.Enabled = true;
        }

        private void textBox4_LostFocus(object sender, EventArgs e)
        {
            textBox4.Location = new Point(8, 193);
            inputPanel1.Enabled = false;
        }

        private void connectGPSButton_Click(object sender, EventArgs e)
        {
            try
            {
                if (gpsInterpreter.IsOpen())
                {
                    gpsInterpreter.Close();
                    setButtonText(connectGPSButton, "Connect");
                }
                else
                {
                    gpsInterpreter.Open(getComboBox(comPortsComboBox));
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
            // TODO:
        }

        private int assayItemChosen = -1;

        private string sessionID;

        private void assayRecordButton_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(sessionID) || string.IsNullOrEmpty(DeviceUID) || assayItemChosen < 0)
                return;

            int trials = 0;

            while (trials < 5)
            {
                try
                {
                    if (!assayWebService.submitAssayResult(sessionID, DeviceUID, assayItemChosen + 1))
                    {
                        MessageBox.Show(Properties.Resources.FailedToSubmit);
                    }
                    else
                    {
                        MessageBox.Show(Properties.Resources.ResultsRecorded);
                        assayItemChosen = -1;
                        break;
                    }
                }
                catch (WebException ex)
                {
                    debugOut.WriteLine(ex.StackTrace);

                    if (trials > 5)
                        MessageBox.Show(Properties.Resources.PossibleNetworkErrorPleaseTryAgain);                 
                }
                finally
                {
                    trials++;
                }
            }
            
        }


        System.Windows.Forms.Timer assayCountdownTimer;

        DateTime assayEndTime = new DateTime();

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
                System.Media.SystemSounds.Question.Play();
                if (DialogResult.OK == assayResultsDialog.ShowDialog())
                {
                    assayItemChosen = assayResultsDialog.SelectedIndex;
                }
            }
        }

        private void mainClearButton_Click(object sender, EventArgs e)
        {
            clearDemo();
        }


    }
}