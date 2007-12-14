using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using RFIDReader;
using System.Net;
using System.IO;
using System.IO.Ports;
using System.Net.Sockets;
using Microsoft.Win32;
using System.Resources;
using System.Runtime.CompilerServices;
using System.Globalization;
using System.Threading;
using System.Security.Permissions;
using TestPocketGraphBar;
using Microsoft.WindowsMobile.Status;
using GentagDemo;
using System.Web.Services.Protocols;
using RFIDReader;


[assembly: CLSCompliant(true)]
//[assembly: SecurityPermission(SecurityAction.RequestMinimum, Execution = true)]
namespace GentagDemo
{
    public partial class demoClient : Form
    {
        bool readerRunning;

        static demoClient mF;

        Graph graph = new Graph();

        Graph radGraph = new Graph();

        nmeaInterpreter gpsInterpreter;

        string DeviceUID;

        Reader tagReader = new Reader();

        

        public demoClient()
        {
            // run initializations provided by the designer
            InitializeComponent();

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

            ImageList myImageList = new ImageList();
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.blank.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.cancel.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.ok.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.question.GetHbitmap()));
            treeView1.ImageList = myImageList;

            // web service inits
            authorizationWebService = new authWS.GetDatesWS();
            petWebService = new petWS.petWS();
            wineWebService = new wineWS.wineWS();
            COREMedDemoWebService = new medWS.COREMedDemoWS();
            petWebService.Timeout = 300000;
            authorizationWebService.Timeout = 300000;
            wineWebService.Timeout = 300000;
            COREMedDemoWebService.Timeout = 30000;

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

        private enum loopType { wine, counterfeit, pet, patient, med, test, none };

        // the variable that describes whether it's looping looking for wine bottles or general tags
        private loopType loop;

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void readerClick(object sender, EventArgs e)
        {
            if (readerRunning == false)
            {
                readerRunning = true;
                setWaitCursor(true);

                if (sender == readLogButton)
                    new Thread(new ThreadStart(launchReadVSLog)).Start();
                else if ((sender == medicationButton) || (sender == readIDButton) || (sender == wineButton) || (sender == petButton) || (sender == readPatientButton) || (sender == readTestButton))
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

                    new Thread(new ThreadStart(tagReader.readTagID)).Start();
                }
                else if (sender == setValueButton)
                    new Thread(new ThreadStart(launchSetVSSettings)).Start();
                else if (sender == readValueButton)
                    new Thread(new ThreadStart(launchGetVSSettings)).Start();
                else if (sender == radScanButton)
                    new Thread(new ThreadStart(radScan)).Start();
            }
            else
            {
                stopReading();
            }
        }

        private void stopReading()
        {
            setWaitCursor(false);
            readerRunning = false;
            tagReader.running = false;
            loop = loopType.none;
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void receiveReaderError(string errorMessage)
        {
            setWaitCursor(false);
            readerRunning = false;
            loop = loopType.none;
            MessageBox.Show(errorMessage);
        }

        private Hashtable cachedWineLookups = new Hashtable();

        private Hashtable cachedCounterfeitLookups = new Hashtable();

        private Hashtable cachedPetLookups = new Hashtable();

        private Hashtable cachedPatientLookups = new Hashtable();

        private Hashtable counterfeitIDsCurrentlyBeingLookedUp = new Hashtable();

        private Hashtable wineIDsCurrentlyBeingLookedUp = new Hashtable();

        private Hashtable petIDsCurrentlyBeingLookedUp = new Hashtable();

        private Hashtable patientsCurrentlyBeingLookedUp = new Hashtable();

        private Hashtable interactionsACurrentlyBeingLookedUp = new Hashtable();

        private Hashtable interactionsBCurrentlyBeingLookedUp = new Hashtable();

        private Hashtable drugsCurrentlyBeingLookedUp = new Hashtable();

        private Queue<string> lookupQueue = new Queue<string>();

        private const int maxLookupThreads = 5;

        // the number of lookups sent off already
        private int pendingLookups = 0;
                
        private int queuedLookups = 0;

        /// <summary>
        /// The callback for when a tag is received by the reader thread
        /// </summary>
        /// <param name="tagID"></param>
        [MethodImpl(MethodImplOptions.Synchronized)]
        private void receiveTag(string tagID)
        {
            if (string.IsNullOrEmpty(tagID)) // if there was no string returned
                return;

            // flash the correct button
            Button button = loop == loopType.counterfeit ? readIDButton : loop == loopType.pet ? petButton : loop == loopType.wine ? wineButton : loop == loopType.patient ? readPatientButton : medicationButton;

            Color backup = getButtonColor(button);
            setButtonColor(button, Color.Green);

            lock (lookupQueue)
            {
                if (pendingLookups < maxLookupThreads)
                {
                    if (sendLookup(tagID))
                        pendingLookups++;

                    setLabel(pendingLookupsLabel, pendingLookups.ToString());
                }
                else
                {
                    queuedLookups++;
                    setLabel(queuedLookupsLabel, lookupQueue.Count.ToString());

                    if (loop == loopType.counterfeit)
                        updateTreeView1(tagID, @"No description yet", false, true, false);
                    // queue it up for later and return
                    lookupQueue.Enqueue(tagID);
                }
            }

            // flash the panel to signal the user that a tag was read
            Thread.Sleep(150);
            setButtonColor(button, backup);
        }

        /// <summary>
        /// The web service endpoints
        /// </summary>
        authWS.GetDatesWS authorizationWebService;
        petWS.petWS petWebService;
        wineWS.wineWS wineWebService;
        medWS.COREMedDemoWS COREMedDemoWebService;
                                

        /// <summary>
        /// Creates the appropriate web service and does the lookup
        /// </summary>
        /// <param name="tagID">the RFID tag string to lookup</param>
        /// <returns>true if the request was sent, false if it wasn't for some reason</returns>
        private bool sendLookup(string tagID)
        {
            AsyncCallback cb = new AsyncCallback(receiveResult);
            bool result = true;

            try
            {
                switch (loop)
                {
                    case loopType.counterfeit:
                        {
                            // if it has already been added, do nothing
                            if (cachedCounterfeitLookups.ContainsKey(tagID))
                            {
                                result = false;
                            }
                            else
                            {
                                // get the tag info and cache it
                                IAsyncResult handle = authorizationWebService.BegingetItem(tagID, DeviceUID, gpsInterpreter.getLatitude(), gpsInterpreter.getLongitude(), cb, authorizationWebService);
                                lock (counterfeitIDsCurrentlyBeingLookedUp.SyncRoot)
                                { counterfeitIDsCurrentlyBeingLookedUp[handle] = tagID; }

                                updateTreeView1(tagID, @"No description yet", false, true, true);
                            }
                            break;
                        }
                    case loopType.pet:
                        {
                            if (cachedPetLookups.ContainsKey(tagID))
                            {
                                result = false;
                                displayPet((petWS.petInfo)cachedPetLookups[tagID]);
                            }
                            else if (petIDsCurrentlyBeingLookedUp.ContainsValue(tagID))
                            {
                                result = false;
                                break;
                            }
                            else
                            {                                
                                IAsyncResult handle = petWebService.BeginretrievePetInformation(tagID, DeviceUID, gpsInterpreter.getLatitude(), gpsInterpreter.getLongitude(), cb, petWebService);
                                lock (petIDsCurrentlyBeingLookedUp.SyncRoot)
                                { petIDsCurrentlyBeingLookedUp[handle] = tagID; }
                            }

                            break;
                        }
                    case loopType.wine:
                        {
                            if (cachedWineLookups.ContainsKey(tagID))
                            {
                                result = false;
                                displayBottle((wineWS.wineBottle)cachedWineLookups[tagID]);
                            }
                            else if (wineIDsCurrentlyBeingLookedUp.ContainsValue(tagID))
                            {
                                result = false;
                            }
                            else
                            {
                                mostRecentWineID = tagID;
                                
                                IAsyncResult handle = wineWebService.BeginretrieveBottleInformation(tagID, DeviceUID, gpsInterpreter.getLatitude(), gpsInterpreter.getLongitude(), cb, wineWebService);
                                lock (wineIDsCurrentlyBeingLookedUp.SyncRoot)
                                { wineIDsCurrentlyBeingLookedUp[handle] = tagID; }
                            }
                            break;
                        }
                    case loopType.patient:
                        {
                            stopReading();

                            if (cachedPatientLookups.ContainsKey(tagID))
                            {
                                result = false;
                                displayPatient((medWS.patientRecord)cachedPatientLookups[tagID]);
                            }
                            else if (patientsCurrentlyBeingLookedUp.ContainsValue(tagID))
                            {
                                result = false;
                            }
                            else
                            {                                
                                IAsyncResult handle = COREMedDemoWebService.BegingetPatientRecord(tagID, cb, COREMedDemoWebService);
                                lock (patientsCurrentlyBeingLookedUp.SyncRoot)
                                { patientsCurrentlyBeingLookedUp[handle] = tagID; }
                                currentPatientID = tagID;
                            }
                            break;
                        }
                    case loopType.test:
                        {
                            stopReading();
                            if (cachedPetLookups.ContainsKey(tagID))
                            {
                                result = false;
                                displayTest((petWS.petInfo)cachedPetLookups[tagID]);
                            }
                            else if (petIDsCurrentlyBeingLookedUp.ContainsValue(tagID))
                            {
                                result = false;
                            }
                            else
                            {                                
                                IAsyncResult handle = petWebService.BeginretrievePetInformation(tagID, DeviceUID, gpsInterpreter.getLatitude(), gpsInterpreter.getLongitude(), cb, petWebService);
                                lock (petIDsCurrentlyBeingLookedUp.SyncRoot)
                                { petIDsCurrentlyBeingLookedUp[handle] = tagID; }
                            }
                            break;
                        }
                    case loopType.med:
                        {
                            stopReading();
                            // do not cache med lookups, make sure this info is always current
                            if (interactionsACurrentlyBeingLookedUp.ContainsValue(tagID))
                            {
                                result = false;
                            }
                            else
                            {
                                if (string.IsNullOrEmpty(currentPatientID))
                                    break;                                

                                IAsyncResult handle = COREMedDemoWebService.BegingetDrugInfo(tagID, cb, COREMedDemoWebService);
                                lock (drugsCurrentlyBeingLookedUp.SyncRoot)
                                { drugsCurrentlyBeingLookedUp[handle] = tagID; }

                                handle = COREMedDemoWebService.BegincheckInteraction(currentPatientID, tagID, cb, COREMedDemoWebService);
                                lock (interactionsBCurrentlyBeingLookedUp.SyncRoot)
                                { interactionsBCurrentlyBeingLookedUp[handle] = currentPatientID; }
                                lock (interactionsACurrentlyBeingLookedUp.SyncRoot)
                                { interactionsACurrentlyBeingLookedUp[handle] = tagID; }
                            }
                            break;
                        }
                    default:

                        break;
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
                if (ar.AsyncState.GetType() == typeof(wineWS.wineWS))
                {
                    // try to cast as wineBottle
                    wineWS.wineWS ws = (wineWS.wineWS)ar.AsyncState;
                    wineWS.wineBottle bottle = ws.EndretrieveBottleInformation(ar);
                    lock (cachedWineLookups.SyncRoot)
                    {
                        cachedWineLookups[bottle.rfidNum] = bottle;
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
                    // may be the test result
                    if (newPet.owner == "anthrax")
                        displayTest(newPet);
                    else
                        displayPet(newPet);
                    lock (cachedPetLookups.SyncRoot)
                    {
                        cachedPetLookups[newPet.rfidNum] = newPet;
                    }

                }
                else if (ar.AsyncState.GetType() == typeof(authWS.GetDatesWS))
                {
                    // try to cast as a standard item lookup    
                    authWS.GetDatesWS ws = (authWS.GetDatesWS)ar.AsyncState;
                    authWS.GetDates info = ws.EndgetItem(ar);

                    // display the tag info
                    updateTreeView1(info.name, info.desc, info.authenticated, false, true);
                    string tagID = (string)counterfeitIDsCurrentlyBeingLookedUp[ar];
                    lock (cachedCounterfeitLookups.SyncRoot)
                    {
                        cachedCounterfeitLookups[tagID] = info;
                    }
                }
                else if (ar.AsyncState.GetType() == typeof(medWS.COREMedDemoWS))
                {
                    medWS.COREMedDemoWS ws = (medWS.COREMedDemoWS)ar.AsyncState;
                    // interaction check
                    if (interactionsACurrentlyBeingLookedUp.ContainsKey(ar))
                    {
                        lock (interactionsBCurrentlyBeingLookedUp.SyncRoot)
                        { interactionsBCurrentlyBeingLookedUp.Remove(ar); }
                        lock (interactionsACurrentlyBeingLookedUp.SyncRoot)
                        { interactionsACurrentlyBeingLookedUp.Remove(ar); }

                        bool alert = ws.EndcheckInteraction(ar);
                        if (alert)
                            MessageBox.Show(Properties.Resources.DrugInteractionWarningMessage);
                    }
                    // patient lookup
                    else if (patientsCurrentlyBeingLookedUp.ContainsKey(ar))
                    {
                        string tagID = (string)patientsCurrentlyBeingLookedUp[ar];
                        lock (patientsCurrentlyBeingLookedUp.SyncRoot)
                        { patientsCurrentlyBeingLookedUp.Remove(ar); }

                        medWS.patientRecord info = ws.EndgetPatientRecord(ar);

                        displayPatient(info);

                        lock (cachedPatientLookups.SyncRoot)
                        { cachedPatientLookups[tagID] = info; }
                    }
                    // drug lookup
                    else
                    {
                        lock (drugsCurrentlyBeingLookedUp.SyncRoot)
                        { drugsCurrentlyBeingLookedUp.Remove(ar); }

                        medWS.drugInfo info = ws.EndgetDrugInfo(ar);

                        displayDrug(info);
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
                            bool lastResult = false;
                            while ((lookupQueue.Count > 0) && (lastResult = sendLookup(lookupQueue.Dequeue())) == false)
                            { }
                            if (lastResult)
                                pendingLookups++;
                        }

                        setLabel(pendingLookupsLabel, pendingLookups.ToString());

                        setLabel(queuedLookupsLabel, lookupQueue.Count.ToString());
                    }
                    catch (InvalidOperationException e)
                    {
                        MessageBox.Show(e.ToString());
                    }

                }

            }
            catch (WebException)
            {
                rescheduleLookup(ar);
            }
            catch (ArgumentNullException)
            {
                rescheduleLookup(ar);
            }
            catch (MemberAccessException)
            {
                MessageBox.Show("unexpected: ");
            }
            catch (SocketException e)
            {
                Thread.Sleep(2000);
                MessageBox.Show(e.ToString());
            }
            catch (IOException e)
            {
                MessageBox.Show(e.Message);
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
                MessageBox.Show(e.ToString() + "@" + e.StackTrace);
                rescheduleLookup(ar);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString());
            }
            finally
            {

            }
        }

        /// <summary>
        /// Performs a lookup again if the previous lookup has failed
        /// </summary>
        /// <param name="ar"></param>
        private void rescheduleLookup(IAsyncResult ar)
        {
            AsyncCallback cb = new AsyncCallback(receiveResult);

            if (ar.AsyncState.GetType() == typeof(wineWS.wineWS))
            {
                lock (wineIDsCurrentlyBeingLookedUp.SyncRoot)
                {
                    // get the bottle info and cache it
                    string currentTag = (string)wineIDsCurrentlyBeingLookedUp[ar];
                    wineIDsCurrentlyBeingLookedUp.Remove(ar);
                    IAsyncResult handle = wineWebService.BeginretrieveBottleInformation(currentTag, DeviceUID, gpsInterpreter.getLatitude(), gpsInterpreter.getLongitude(), cb, wineWebService);
                    wineIDsCurrentlyBeingLookedUp[handle] = currentTag;
                }
            }
            else if (ar.AsyncState.GetType() == typeof(petWS.petWS))
            {
                lock (petIDsCurrentlyBeingLookedUp.SyncRoot)
                {
                    string currentTag = (string)petIDsCurrentlyBeingLookedUp[ar];
                    petIDsCurrentlyBeingLookedUp.Remove(ar);
                    IAsyncResult handle = petWebService.BeginretrievePetInformation(currentTag, DeviceUID, gpsInterpreter.getLatitude(), gpsInterpreter.getLongitude(), cb, petWebService);
                    petIDsCurrentlyBeingLookedUp[handle] = currentTag;
                }
            }
            else if (ar.AsyncState.GetType() == typeof(authWS.GetDatesWS))
            {
                lock (counterfeitIDsCurrentlyBeingLookedUp.SyncRoot)
                {
                    // get the tag info and cache it
                    string currentTag = (string)counterfeitIDsCurrentlyBeingLookedUp[ar];
                    counterfeitIDsCurrentlyBeingLookedUp.Remove(ar);
                    IAsyncResult handle = authorizationWebService.BegingetItem(currentTag, DeviceUID, gpsInterpreter.getLatitude(), gpsInterpreter.getLongitude(), cb, authorizationWebService);
                    counterfeitIDsCurrentlyBeingLookedUp[handle] = currentTag;
                }
            }
            else if (ar.AsyncState.GetType() == typeof(medWS.COREMedDemoWS))
            {
                lock (patientsCurrentlyBeingLookedUp.SyncRoot)
                {
                    // get the tag info and cache it
                    string currentTag = (string)patientsCurrentlyBeingLookedUp[ar];
                    patientsCurrentlyBeingLookedUp.Remove(ar);
                    IAsyncResult handle = COREMedDemoWebService.BegingetPatientRecord(currentTag, cb, COREMedDemoWebService);
                    patientsCurrentlyBeingLookedUp[handle] = currentTag;
                }
            }
        }


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
            setPhoto(drugPhoto, drug.picture);
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void displayPatient(medWS.patientRecord patient)
        {
            if (patient.exists)
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

            if (newPet.image != null && newPet.image.Length > 16)
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

        private void mainForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == System.Windows.Forms.Keys.Back)
            {
                treeView1.Nodes.Clear();
                cachedCounterfeitLookups.Clear();
                cachedWineLookups.Clear();
                cachedPetLookups.Clear();
                counterfeitIDsCurrentlyBeingLookedUp.Clear();
                wineIDsCurrentlyBeingLookedUp.Clear();
                petIDsCurrentlyBeingLookedUp.Clear();
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

            authWS.GetDatesWS ws = new authWS.GetDatesWS();

            string rfidDescr = @"No description found";

            try
            {
                authWS.GetDates values = ws.getItem(textBox4.Text, DeviceUID, 0, 0);
                rfidDescr = values.desc;
            }
            catch (WebException ex)
            {
                MessageBox.Show(@"Problem connecting to web service: " + ex.Message);
            }

            updateTreeView1(textBox4.Text, rfidDescr, false, false, true);

            textBox4.Enabled = true;

            Cursor.Current = Cursors.Default;
        }

        //private string patientID;

        //private void readPatientData()
        //{
        //    try
        //    {



        //    }
        //    catch (NotSupportedException ex)
        //    {
        //        MessageBox.Show(ex.Message);
        //    }
        //    catch (IOException ex)
        //    {
        //        MessageBox.Show(ex.Message);
        //    }
        //    catch (WebException ex)
        //    {
        //        MessageBox.Show("Problem connecting to web service: " + ex.Message);
        //    }
        //    finally
        //    {
        //        setWaitCursor(false);
        //        readerRunning = false;
        //    }
        //}

        private string drugID;

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
            Cursor.Current = Cursors.Default;
            readerRunning = false;
            tagReader.running = false;
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
    }
}