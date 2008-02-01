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

        private RFIDReadCursor.UserControl1 userControl11;

        Thread versionCheckThread;

        

        public demoClient()
        {
            // run initializations provided by the designer
            InitializeComponent();

            tagReader = new Reader();

            // open comm briefly to preload the assemblies
            tagReader.initialize();

            // intialize threads
            versionCheckThread = new Thread(versionCheck);
            versionCheckThread.Start();          

            // initialize the debug stream
            debugOut = new StreamWriter("debug.txt", false);

            // userControl11
            this.userControl11 = new RFIDReadCursor.UserControl1();
            this.userControl11.Location = new System.Drawing.Point(76, 76);
            this.userControl11.Name = "userControl11";
            this.userControl11.Size = new System.Drawing.Size(72, 72);           

            

            // generate the device's UID
            string AppString = GentagDemo.Properties.Resources.titleString;

            byte[] AppData = new byte[AppString.Length];

            for (int count = 0; count < AppString.Length; count++)
                AppData[count] = (byte)AppString[count];

            int appDataSize = AppData.Length;

            byte[] dUID = new byte[20];

            uint SizeOut = 20;

            RFIDReader.Reader.GetDeviceUniqueID(AppData, appDataSize, 1, dUID, out SizeOut);

            for (int i = 0; i < 20; i++)
                DeviceUID += dUID[i].ToString("X", CultureInfo.CurrentUICulture);

            gpsInterpreter = new nmeaInterpreter(new CultureInfo("en-US"), DeviceUID);

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
            tagReader.TagReceived += new RFIDReader.Reader.TagReceivedEventHandler(receiveTag);
            tagReader.VarioSensSettingsReceived += new RFIDReader.Reader.VarioSensSettingsReceivedEventHandler(receiveVarioSensSettings);
            tagReader.ReaderError += new RFIDReader.Reader.ReaderErrorHandler(receiveReaderError);
            tagReader.VarioSensLogReceived += new RFIDReader.Reader.VarioSensReadLogHandler(writeViolations);
            tagReader.ReturnTagContents += new Reader.ReturnTagContentsHandler(receiveTagContents);
            tagReader.DoneWriting += new Reader.DoneWritingTagContentsHandler(doneWriting);
            tagReader.TagTypeDetected += new Reader.TagTypeDetectedHandler(displayTagType);

            ImageList myImageList = new ImageList();
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.blank.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.cancel.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.ok.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.question.GetHbitmap()));
            treeView1.ImageList = myImageList;

            // web service inits
            const int timeout = 15000;
            petWebService = new petWS.petWS();
            wineWebService = new wineWS.wineWS();
            COREMedDemoWebService = new medWS.COREMedDemoWS();
            authenticationWebService = new authenticationWS.AuthenticationWebService();
            petWebService.Timeout = timeout;
            wineWebService.Timeout = timeout;
            COREMedDemoWebService.Timeout = timeout;
            authenticationWebService.Timeout = timeout;
            authenticationWebService.ConnectionGroupName = "authentication";
            wineWebService.ConnectionGroupName = "wine";
            petWebService.ConnectionGroupName = "pet";
            COREMedDemoWebService.ConnectionGroupName = "med";
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

        void displayTagType(Reader.tagTypes tagType, string tagID)
        {
            
            blinkCursor(true);

            switch (tagType)
            {
                case Reader.tagTypes.INSIDE:
                    setLabel(detectTagTypeLabel, "INSIDE");
                    break;
                case Reader.tagTypes.iso14443a:
                    setLabel(detectTagTypeLabel, "ISO14443A");
                    break;
                case Reader.tagTypes.iso14443b:
                    setLabel(detectTagTypeLabel, "ISO14443B");
                    break;
                case Reader.tagTypes.iso14443bsr176:
                    setLabel(detectTagTypeLabel, "ISO14443B");
                    break;
                case Reader.tagTypes.iso14443bsri:
                    setLabel(detectTagTypeLabel, "ISO14443B");
                    break;
                case Reader.tagTypes.iso15693:
                    setLabel(detectTagTypeLabel, "ISO15693");
                    break;
                case Reader.tagTypes.iso18000:
                    setLabel(detectTagTypeLabel, "ISO18000");
                    break;
                case Reader.tagTypes.MiFareClassic:
                    setLabel(detectTagTypeLabel, "MiFare");
                    break;
                case Reader.tagTypes.MiFareDESFire:
                    setLabel(detectTagTypeLabel, "MiFare DESFire");
                    break;
                case Reader.tagTypes.MiFareUltraLight:
                    setLabel(detectTagTypeLabel, "MiFare UltraLight");
                    break;
                case Reader.tagTypes.felica:
                    setLabel(detectTagTypeLabel, "FelICa");
                    break;
            }

            setLabel(detectTagIDLabel, tagID);
            Thread.Sleep(250);

            blinkCursor(false);
        }

        ~demoClient()
        {
            stopReading(true);
            debugOut.Dispose();
            Dispose(false);
        }

        private void receiveTagContents(string contents)
        {
            setTextBox(readWriteTB, contents);
            stopReading(false);
        }        

        private void writeTagMemory()
        {            
            tagReader.writeTag(System.Text.Encoding.Unicode.GetBytes(getTextBox(readWriteTB)));
        }

        private void doneWriting(string status)
        {
            setLabel(readWriteStatusLabel, "Status: " + status);
            stopReading(false);
        }       

        private enum loopType { wine, counterfeit, pet, patient, med, test, none };

        // the variable that describes whether it's looping looking for wine bottles or general tags
        private loopType loop;

        Thread readerThread;

        
        private void readerClick(object sender, EventArgs e)
        {
            if (tagReader.running == false)
            {
                stopReading(true);
                setWaitCursor(true);                
                
                if ((sender == medicationButton) || (sender == readIDButton) || (sender == wineButton) || (sender == petButton) || (sender == readPatientButton) || (sender == readTestButton))
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

                    readerThread = new Thread(tagReader.readTagID);

                }
                else if (sender == readLogButton)
                    readerThread = new Thread(launchReadVSLog);
                else if (sender == detectTagTypeButton)
                    readerThread = new Thread(tagReader.detectTag);
                else if (sender == readButton)
                    readerThread = new Thread(tagReader.readTag);
                else if (sender == writeButton)
                    readerThread = new Thread(writeTagMemory);
                else if (sender == setValueButton)
                    readerThread = new Thread(launchSetVSSettings);
                else if (sender == readValueButton)
                    readerThread = new Thread(launchGetVSSettings);
                else if (sender == radScanButton)
                    readerThread = new Thread(radScan);

                readerThread.IsBackground = true;
                readerThread.Start();
            }
            else
            {
                stopReading(true);
            }
        }

        private void stopReading(bool forceStop)
        {            
            setWaitCursor(false);
            blinkCursor(false);
            tagReader.running = false;

            if (forceStop)
            {
                if (readerThread != null)
                {
                    if (readerThread.Join(1250) == false)
                    {
                        readerThread.Abort();
                    }
                    readerThread = null;
                }
            }
        }

        private void receiveReaderError(string errorMessage)
        {
            stopReading(false);
            MessageBox.Show(errorMessage);
        }
       
        private Hashtable wineBottleCache = new Hashtable();

        private Hashtable counterfeitCache = new Hashtable();

        private Hashtable petCache = new Hashtable();

        private Hashtable patientCache = new Hashtable();
        
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
        private void receiveTag(string tagID)
        {
            if (string.IsNullOrEmpty(tagID)) // if there was no string returned
                return;
            
            blinkCursor(true);

            lock (lookupQueue)
            {
                if (pendingLookups < maxLookupThreads)
                {
                    pendingLookups += scheduleLookup(tagID, loop);

                    setLabel(pendingLookupsLabel, pendingLookups.ToString(CultureInfo.CurrentCulture));
                }
                else
                {
                    // no need to add it to the queue if it already exists
                    if (!lookupQueue.Contains(tagID))
                    {
                        queuedLookups++;
                        setLabel(queuedLookupsLabel, lookupQueue.Count.ToString(CultureInfo.CurrentCulture));

                        if (loop == loopType.counterfeit)
                            updateTreeView1(tagID, @"No description yet", false, true, false);
                        // queue it up for later and return
                        lookupQueue.Enqueue(tagID);
                    }
                }
            }

            // flash the panel to signal the user that a tag was read
            Thread.Sleep(100);
            blinkCursor(false);
        }

        /// <summary>
        /// The web service endpoints
        /// </summary>
        petWS.petWS petWebService;
        wineWS.wineWS wineWebService;
        medWS.COREMedDemoWS COREMedDemoWebService;
        authenticationWS.AuthenticationWebService authenticationWebService;
                                

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
                                    { itemsCurrentlyBeingLookedUp.Add(handle,tagID); }

                                    updateTreeView1(tagID, @"No description yet", false, true, true);
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
                                stopReading(false);
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
                                stopReading(false);

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
                                stopReading(false);
                                
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
        [MethodImpl(MethodImplOptions.Synchronized)]
        private void displayBottle(wineWS.wineBottle bottle)
        {
            // display the bottle info
            if (bottle.exists)
            {
                setLabel(wineCountryLabel, bottle.origin);
                setLabel(wineYearLabel, bottle.year.ToString(CultureInfo.CurrentUICulture));
                setLabel(wineTypeLabel, bottle.type);
                setLabel(wineVineyardLabel, bottle.vineyard);
                setTextBox(wineReviewTextBox, bottle.review);
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
                setLabel(wineCountryLabel, GentagDemo.Properties.Resources.emptyString);
                setLabel(wineYearLabel, GentagDemo.Properties.Resources.emptyString);
                setLabel(wineTypeLabel, GentagDemo.Properties.Resources.emptyString);
                setLabel(wineVineyardLabel, GentagDemo.Properties.Resources.emptyString);
                setTextBox(wineReviewTextBox, GentagDemo.Properties.Resources.emptyString);
            }
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void displayDrug(medWS.drugInfo drug)
        {
            try
            {
                if (drug != null && drug.exists && drug.picture != null)
                    setPhoto(drugPhoto, drug.picture);
            }
            catch (Exception)
            {
                setPhoto(drugPhoto, (System.Drawing.Image)null);
            }
        }

        private void clearDrug()
        {
            setPhoto(drugPhoto, (System.Drawing.Image)null);
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void displayPatient(medWS.patientRecord patient)
        {
            if (patient == null || patient.RFIDnum == null || patient.image == null)
            {
                setTextBox(patientNameBox, @"");
                setTextBox(patientDescriptionBox, @"Patient not found");
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
                    setTextBox(patientNameBox, @"");
                    setTextBox(patientDescriptionBox, @"Patient not found");
                    setPhoto(patientPhoto, (Image)null);
                }
            }
            else
            {
                setTextBox(patientNameBox, @"");
                setTextBox(patientDescriptionBox, @"Patient not found");
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
            if (newPet != null &&newPet.image != null && newPet.image.Length > 16)
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
            treeView1.BeginUpdate();

            bool exists = false;

            foreach (TreeNode tn in treeView1.Nodes)
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
                treeView1.Nodes.Add(superTn);
            }
            treeView1.EndUpdate();
        }
        #endregion


        private void mainForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == System.Windows.Forms.Keys.Back)
            {
                treeView1.BeginUpdate();
                treeView1.Nodes.Clear();
                treeView1.EndUpdate();
                
                counterfeitCache.Clear();
                wineBottleCache.Clear();
                petCache.Clear();
                patientCache.Clear();

                itemsCurrentlyBeingLookedUp.Clear();

                clearDrug();
                displayPatient(null);
            }
            else if (e.KeyCode == System.Windows.Forms.Keys.F2)
            {
                if (tabControl1.TabPages[tabControl1.SelectedIndex].Name == "authPage")
                    readerClick(readIDButton, new EventArgs());
                else if (tabControl1.TabPages[tabControl1.SelectedIndex].Name == "winePage")
                    readerClick(wineButton, new EventArgs());
            }
        }

        private void manualLookupClick(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;

            textBox4.Enabled = false;

            authenticationWS.AuthenticationWebService ws = new authenticationWS.AuthenticationWebService();

            string rfidDescr = @"No description found";

            try
            {
                authenticationWS.itemInfo values = ws.getItem(textBox4.Text, DeviceUID, 0, 0);
                rfidDescr = values.description;
            }
            catch (WebException ex)
            {
                MessageBox.Show(@"Problem connecting to web service: " + ex.Message);
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
            stopReading(true);
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

    }
}