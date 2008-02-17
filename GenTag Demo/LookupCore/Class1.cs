using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using NMEA;
using RFIDReader;
using System.IO;
using System.Collections;
using System.Runtime.CompilerServices;

namespace LookupCore
{
    public class LookupCore
    {
        #region Members
        static LookupCore mF;

        string DeviceUID;

        Reader tagReader;

        TextWriter debugOut;

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
        /// The web service endpoints
        /// </summary>
        petWS.petWS petWebService;
        wineWS.wineWS wineWebService;
        medWS.COREMedDemoWS COREMedDemoWebService;
        authenticationWS.AuthenticationWebService authenticationWebService;
        assayWS.AssayWS assayWebService;
        #endregion

        public LookupCore()
        {
            tagReader = new Reader();

            DeviceUID = Reader.getDeviceUniqueID(GentagDemo.Properties.Resources.titleString);

            for (int i = 5; i > 0; i--)
            {
                try
                {
                    debugOut = new StreamWriter("debug.txt" + (i - 5).ToString(), false);
                    break;
                }
                catch (IOException)
                {
                }
            }

            // setup event callbacks for tag reading events
            tagReader.TagReceived += new EventHandler<TagReceivedEventArgs>(receiveTag);
            tagReader.VarioSensSettingsReceived += new EventHandler<VarioSensSettingsEventArgs>(receiveVarioSensSettings);
            tagReader.ReaderError += new EventHandler<ReaderErrorEventArgs>(receiveReaderError);
            tagReader.VarioSensLogReceived += new EventHandler<VarioSensLogEventArgs>(writeViolations);
            tagReader.ReturnTagContents += new EventHandler<TagContentsEventArgs>(receiveTagContents);
            tagReader.DoneWriting += new EventHandler<FinishedWritingStatusEventArgs>(doneWriting);
            tagReader.TagTypeDetected += new EventHandler<TagTypeEventArgs>(displayTagType);

            // web service inits
            const int timeout = 10000;
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
        }

        ~LookupCore()
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
                    else if (ar.AsyncState.GetType() == typeof(wineWS.wineWS))
                    {
                        // try to cast as wineBottle
                        wineWS.wineWS ws = (wineWS.wineWS)ar.AsyncState;
                        wineWS.wineBottle bottle = ws.EndretrieveBottleInformation(ar);

                        if (bottle.retryNeeded)
                            throw new WebException();

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

        private void receiveReaderError(object sender, ReaderErrorEventArgs args)
        {
            stopReading();

            showMessage(args.Reason);
        }        
    }
}
