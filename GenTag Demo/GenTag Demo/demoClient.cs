using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using C1Lib;
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
using SiritReader;


[assembly: CLSCompliant(true)]
//[assembly: SecurityPermission(SecurityAction.RequestMinimum, Execution = true)]
namespace GentagDemo
{
    public partial class demoClient : Form
    {
        bool readerRunning;

        static demoClient mF;

        Graph graph = new Graph();

        nmeaInterpreter gpsInterpreter;

        string DeviceUID;

        NativeMethods tagReader = new NativeMethods();

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

            NativeMethods.GetDeviceUniqueID(AppData, appDataSize, 1, dUID, out SizeOut);

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
            tagReader.TagReceived += new NativeMethods.TagReceivedEventHandler(tagReceived);
            tagReader.VarioSensSettingsReceived += new NativeMethods.VarioSensSettingsReceivedEventHandler(receiveVarioSensSettings);
            tagReader.ReaderError += new NativeMethods.ReaderErrorHandler(receiveReaderError);
            tagReader.VarioSensLogReceived += new NativeMethods.VarioSensReadLogHandler(writeViolations);

            ImageList myImageList = new ImageList();
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.blank.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.cancel.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.ok.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.question.GetHbitmap()));
            treeView1.ImageList = myImageList;

            graph.Visible = false;
            graph.Size = new Size(240, 230);
            graph.Location = new Point(0, 0);
            VarioSens.Controls.Add(graph);

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

        // the variable that describes whether it's looping looking for wine bottles or general tags
        private bool wineLoop;
        private bool petLoop;

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void readerClick(object sender, EventArgs e)
        {
            if (readerRunning == false)
            {
                readerRunning = true;
                Cursor.Current = Cursors.WaitCursor;

                if (sender == readLogButton)
                    new Thread(new ThreadStart(launchReadVSLog)).Start();
                else if ((sender == readIDButton) || (sender == wineButton) || (sender == petButton))
                {
                    if (sender == wineButton)
                    {
                        wineLoop = true;
                        petLoop = false;
                    }
                    else if (sender == petButton)
                    {
                        wineLoop = false;
                        petLoop = true;
                    }
                    else
                    {
                        petLoop = false;
                        wineLoop = false;
                    }
                    new Thread(new ThreadStart(tagReader.readTagID)).Start();
                    new Thread(new ThreadStart(doLookup)).Start();
                }
                else if (sender == setValueButton)
                    new Thread(new ThreadStart(launchSetVSSettings)).Start();
                else if (sender == readValueButton)
                    new Thread(new ThreadStart(launchGetVSSettings)).Start();
                else if (sender == readPatientButton)
                    new Thread(new ThreadStart(readPatientData)).Start();
                else if (sender == medicationButton)
                    new Thread(new ThreadStart(readDrugData)).Start();
            }
            else
            {
                Cursor.Current = Cursors.Default;
                readerRunning = false;
                tagReader.running = false;
            }
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void receiveReaderError(string errorMessage)
        {
            setWaitCursor(false);
            readerRunning = false;
            MessageBox.Show(errorMessage);
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void tagReceived(string tagID)
        {
            Button button = wineLoop ? wineButton : (petLoop ? petButton : readIDButton);
            Hashtable cache = wineLoop ? cachedWineLookups : (petLoop ? cachedPetLookups : cachedIDLookups);

            lock (tagQueue.SyncRoot)
            {
                if (wineLoop || petLoop)
                {
                    if (!tagQueue.Contains(tagID))
                        tagQueue.Enqueue(tagID);
                }
                // either it's in the queue waiting to be processed or it has been processed and is cached
                else
                {
                    if (!tagQueue.Contains(tagID) && !cache.Contains(tagID))
                    {
                        tagQueue.Enqueue(tagID);
                        updateTreeView1(tagID, @"No description yet", false, true);
                    }
                }
            }

            // flash the panel to signal the user that a tag was read
            Color backup = getButtonColor(button);
            setButtonColor(button, Color.Green);
            Thread.Sleep(150);
            setButtonColor(button, backup);
        }

        #region VarioSens functions
        private void receiveVarioSensSettings(Single upper, Single lower, short period, short logMode, short batteryCheckInterval)
        {
            setWaitCursor(false);
            if (logMode < 0)
            {
                mF.setTextBox(getSetStatusBox, "Error reading tag");
                return;
            }
            mF.setTextBox(getSetStatusBox, "");
            mF.setTextBox(loLimitTextBox, lower.ToString(CultureInfo.CurrentUICulture));
            mF.setTextBox(hiLimitTextBox, upper.ToString(CultureInfo.CurrentUICulture));
            mF.setTextBox(intervalTextBox, period.ToString(CultureInfo.CurrentUICulture));
            mF.setComboBoxIndex(mF.logModeComboBox, logMode - 1);
            mF.setTextBox(batteryCheckIntervalTextBox, batteryCheckInterval.ToString(CultureInfo.CurrentUICulture));
        }

        private void launchReadVSLog()
        {
            tagReader.readLog();
            readerRunning = false;
            setWaitCursor(false);
        }

        private void launchSetVSSettings()
        {
            int mode;
            int errorCode = -1;

            if (getComboBoxIndex(logModeComboBox) == 0)
                mode = 1;
            else
                mode = 2;

            for (int i = 50; i >= 0; --i)
            {
                errorCode = tagReader.setVSSettings(mode,
                    float.Parse(getTextBox(hiLimitTextBox), CultureInfo.CurrentUICulture),
                    float.Parse(getTextBox(loLimitTextBox), CultureInfo.CurrentUICulture),
                    int.Parse(getTextBox(intervalTextBox), CultureInfo.CurrentUICulture),
                    int.Parse(getTextBox(batteryCheckIntervalTextBox), CultureInfo.CurrentUICulture));

                if (errorCode != 0)
                {
                    string eC = "";
                    switch (errorCode)
                    {
                        case -1:
                            eC = "Cannot est. comm";
                            break;
                        case -2:
                            eC = "Cannot enable reader";
                            break;
                        case -3:
                            eC = "Cannot init reader";
                            break;
                        case -4:
                            eC = "Cannot get log settings";
                            break;
                        case -5:
                            eC = "Cannot set log timer";
                            break;
                        case -6:
                            eC = "Cannot set log mode";
                            break;
                        case -7:
                            eC = "Cannot start logging";
                            break;
                    }
                    setTextBox(getSetStatusBox, eC + " (" + i + "/50)");
                }
                else
                {
                    setTextBox(getSetStatusBox, "Success");
                    break;
                }
            }
            readerRunning = false;
            setWaitCursor(false);
        }

        private void launchGetVSSettings()
        {
            tagReader.running = true;
            tagReader.getVSSettings();
            readerClick(readValueButton, new EventArgs());
        }

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
                mF.Invoke(new NativeMethods.writeViolationsDelegate(writeViolations),
                    new object[] { upperTempLimit, lowerTempLimit, len, recordPeriod, dateTime, logMode, temperatures });
                return;
            }
            mF.textBox9.Text = @"";
            mF.textBox1.Text = upperTempLimit.ToString(CultureInfo.CurrentUICulture);
            mF.textBox2.Text = lowerTempLimit.ToString(CultureInfo.CurrentUICulture);
            mF.textBox3.Text = recordPeriod.ToString(CultureInfo.CurrentUICulture);
            mF.listBox1.Items.Clear();
            mF.graph.Clear();

            for (int i = 0; i < dateTime.Length; i++)
            {
                if (logMode[i] == 1)
                {
                    mF.listBox1.Items.Add(temperatures[i].ToString("F", CultureInfo.CurrentUICulture));
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
                catch (Exception)
                {
                    throw;
                }
            }
        }

        #endregion


        private Hashtable cachedWineLookups = new Hashtable();

        private Hashtable cachedIDLookups = new Hashtable();

        private Hashtable cachedPetLookups = new Hashtable();

        private Hashtable IDsCurrentlyBeingLookedUp = new Hashtable();

        private Hashtable wineIDsCurrentlyBeingLookedUp = new Hashtable();

        private Hashtable petIDsCurrentlyBeingLookedUp = new Hashtable();

        private Queue tagQueue = new Queue();

        private bool wineChanged;

        private void doLookup()
        {
            Hashtable cache = wineLoop ? cachedWineLookups : (petLoop ? cachedPetLookups : cachedIDLookups);

            while ((readerRunning) || (tagQueue.Count > 0))
            {
                string currentTag;

                lock (tagQueue.SyncRoot)
                {
                    if (tagQueue.Count > 0)
                        currentTag = (string)tagQueue.Dequeue();
                    else
                        currentTag = null;
                }
                if (string.IsNullOrEmpty(currentTag)) // if there was no string returned
                {
                    Thread.Sleep(250);
                    continue;
                }
                else if (cache.ContainsKey(currentTag)) // else if the tag is already looked up
                {
                    wineChanged = true;
                    if (wineLoop)
                        displayBottle((org.dyndns.criusWine.wineBottle)cache[currentTag]);
                    if (petLoop)
                        displayPet((petWS.petInfo)cache[currentTag]);

                }
                else // hashtable does not contain info on this tag
                {
                    // if a request for this tag has already been made
                    if (IDsCurrentlyBeingLookedUp.ContainsValue(currentTag) && !wineLoop && !petLoop)
                        continue;
                    if (wineIDsCurrentlyBeingLookedUp.ContainsValue(currentTag) && wineLoop)
                        continue;
                    if (petIDsCurrentlyBeingLookedUp.ContainsValue(currentTag) && petLoop)
                        continue;
                    // use the web service to retrieve a description  
                    try
                    {
                        AsyncCallback cb = new AsyncCallback(receiveNewItem);
                        IAsyncResult handle;

                        if (wineLoop)
                        {
                            // set the current bottle to be looked up
                            wineChanged = false;
                            // get the bottle info and cache it
                            org.dyndns.criusWine.wineWS ws = new org.dyndns.criusWine.wineWS();
                            ws.Timeout = 300000;
                            handle = ws.BeginretrieveBottleInformation(currentTag, DeviceUID, 0, 0, cb, ws);
                            wineIDsCurrentlyBeingLookedUp[handle] = currentTag;
                        }
                        else if (petLoop)
                        {
                            petWS.petWS ws = new petWS.petWS();
                            ws.Timeout = 300000;
                            handle = ws.BeginretrievePetInformation(currentTag, DeviceUID, 0, 0, cb, ws);
                            petIDsCurrentlyBeingLookedUp[handle] = currentTag;
                        }
                        else
                        {

                            // get the tag info and cache it
                            org.dyndns.crius.GetDatesWS ws = new org.dyndns.crius.GetDatesWS();
                            ws.Timeout = 300000;
                            handle = ws.BegingetItem(currentTag, DeviceUID, 0, 0, cb, ws);
                            IDsCurrentlyBeingLookedUp[handle] = currentTag;
                        }

                    }
                    catch (SoapException ex)
                    {
                        MessageBox.Show("Unexpected SOAP exception: " + ex.Message);
                    }
                }
                Thread.Sleep(250);
            }
        }


        private void receiveNewItem(IAsyncResult ar)
        {
            Hashtable cache = wineLoop ? cachedWineLookups : (petLoop ? cachedPetLookups : cachedIDLookups);

            try
            {
                if (wineLoop)
                {
                    org.dyndns.criusWine.wineWS ws = (org.dyndns.criusWine.wineWS)ar.AsyncState;
                    org.dyndns.criusWine.wineBottle bottle = ws.EndretrieveBottleInformation(ar);
                    lock (cache.SyncRoot)
                    {
                        cache[bottle.rfidNum] = bottle;
                    }
                    if (!wineChanged)
                    {
                        displayBottle(bottle);
                    }
                }
                else if (petLoop)
                {
                    petWS.petWS ws = (petWS.petWS)ar.AsyncState;
                    petWS.petInfo newPet = ws.EndretrievePetInformation(ar);
                    lock (cache.SyncRoot)
                    {
                        cache[newPet.rfidNum] = newPet;
                    }
                    if (!wineChanged)
                    {
                        displayPet(newPet);
                    }
                }
                else
                {
                    org.dyndns.crius.GetDatesWS ws = (org.dyndns.crius.GetDatesWS)ar.AsyncState;
                    org.dyndns.crius.GetDates info = ws.EndgetItem(ar);

                    // display the tag info
                    updateTreeView1(info.name, info.desc, info.authenticated, false);
                    lock (cache.SyncRoot)
                    {
                        cache[info.name] = info;
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
            catch (SocketException)
            {
                Thread.Sleep(2000);
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

        private void rescheduleLookup(IAsyncResult ar)
        {
            AsyncCallback cb = new AsyncCallback(receiveNewItem);
            IAsyncResult handle;

            string currentTag = (string)IDsCurrentlyBeingLookedUp[ar];

            if (wineLoop)
            {
                // set the current bottle to be looked up
                //currentRfidNum = currentTag;
                // get the bottle info and cache it
                org.dyndns.criusWine.wineWS ws = new org.dyndns.criusWine.wineWS();
                ws.Timeout = 300000;
                handle = ws.BeginretrieveBottleInformation(currentTag, DeviceUID, 0, 0, cb, ws);

            }
            else if (petLoop)
            {
                petWS.petWS ws = new petWS.petWS();
                ws.Timeout = 300000;                
                handle = ws.BeginretrievePetInformation(currentTag, DeviceUID, 0, 0, cb, ws);
            }
            else
            {
                // get the tag info and cache it
                org.dyndns.crius.GetDatesWS ws = new org.dyndns.crius.GetDatesWS();
                ws.Timeout = 300000;
                handle = ws.BegingetItem(currentTag, DeviceUID, 0, 0, cb, ws);
            }
            IDsCurrentlyBeingLookedUp[handle] = currentTag;
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void displayBottle(org.dyndns.criusWine.wineBottle bottle)
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
        private void displayPet(petWS.petInfo newPet)
        {
            // display the bottle info
            if (newPet.exists)
            {
                setLabel(petOwnerLabel, newPet.owner);
                setLabel(petContactInfoLabel, newPet.contactInfo);
                setLabel(petPhonenumberLabel, newPet.phoneNumber);
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

        private delegate void updateTreeView1Delegate(string currentTag, string rfidDescr, bool isAuthenticated, bool isNew);

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void updateTreeView1(string currentTag, string rfidDescr, bool isAuthenticated, bool isNew)
        {
            if (this.InvokeRequired)
            {
                Invoke(new updateTreeView1Delegate(updateTreeView1),
                    new object[] { currentTag, rfidDescr, isAuthenticated, isNew });
                return;
            }
            treeView1.BeginUpdate();

            bool exists = false;

            foreach (TreeNode tn in treeView1.Nodes)
            {
                if (tn.Text == currentTag)
                {
                    tn.Nodes.Clear();
                    TreeNode tN = new TreeNode(rfidDescr);
                    tn.SelectedImageIndex = tn.ImageIndex = isNew ? 3 : (isAuthenticated ? 2 : 1);
                    tn.Nodes.Add(tN);
                    exists = true;
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
                cachedIDLookups.Clear();
                cachedWineLookups.Clear();
                tagQueue.Clear();
                IDsCurrentlyBeingLookedUp.Clear();
                wineIDsCurrentlyBeingLookedUp.Clear();
            }
            else if (e.KeyCode == System.Windows.Forms.Keys.Q)
            {
                readerRunning = false;
                tagReader.running = false;
                Application.Exit();
            }
            else if (e.KeyCode == System.Windows.Forms.Keys.F2)
            {
                if (tabControl1.TabPages[tabControl1.SelectedIndex].Name == "authPage")
                    readerClick(readIDButton, new EventArgs());
                else if (tabControl1.TabPages[tabControl1.SelectedIndex].Name == "winePage")
                    readerClick(wineButton, new EventArgs());
            }
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
                readLogButton.Text = GentagDemo.Properties.Resources.readLogButtonInit;
            }
            else if (obj == setValueButton)
            {
                setValueButton.Text = GentagDemo.Properties.Resources.setValueButtonInit;
            }
            else if (obj == readValueButton)
            {
                setValueButton.Text = GentagDemo.Properties.Resources.readValueButtonInit;
            }
            else if (obj == setValueButton)
            {
                readIDButton.Text = GentagDemo.Properties.Resources.readIDButtonInit;
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
                org.dyndns.crius.GetDates values = ws.getItem(textBox4.Text, DeviceUID, 0, 0);
                rfidDescr = values.desc;
            }
            catch (WebException ex)
            {
                MessageBox.Show(@"Problem connecting to web service: " + ex.Message);
            }

            updateTreeView1(textBox4.Text, rfidDescr, false, false);

            textBox4.Enabled = true;

            Cursor.Current = Cursors.Default;
        }

        private string patientID;

        private void readPatientData()
        {
            try
            {
                patientID = NativeMethods.readOneTagID();
                setWaitCursor(false);
                org.dyndns.crius.GetDatesWS ws = new org.dyndns.crius.GetDatesWS();
                ws.Timeout = 30000;
                org.dyndns.crius.patientInfo values = ws.getPatientInfo(patientID);
                setTextBox(patientNameBox, values.name);
                setTextBox(patientDescriptionBox, values.description);
                setPhoto(patientPhoto, values.image);
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
                readerRunning = false;
            }
        }

        private string drugID;

        private void readDrugData()
        {
            try
            {
                drugID = NativeMethods.readOneTagID();
                setWaitCursor(false);
                org.dyndns.crius.GetDatesWS ws = new org.dyndns.crius.GetDatesWS();

                byte[] bA = ws.getPicture(drugID, true);
                setPhoto(drugPhoto, bA);
                bool drugInteraction = ws.checkInteraction(patientID, drugID);

                if (drugInteraction == true)
                    MessageBox.Show(Properties.Resources.DrugInteractionWarningMessage);
            }
            catch (WebException ex)
            {
                MessageBox.Show("Problem connecting to web service: " + ex.Message);
            }
            catch (NotSupportedException ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                readerRunning = false;
                setWaitCursor(false);
            }
        }

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
                        trackingCheckBox.Checked = false;
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
            setProgressBar(queueSizeBar, queueSize);
        }

        private void gpsNmea_DateTimeChanged(DateTime dT)
        {
            setTextBox(timeTextBox, dT.ToString("t", CultureInfo.CurrentUICulture));
        }
        private void gpsNmea_FixObtained()
        {
            setTextBox(statusTextBox, @"Fixed");
        }
        private void gpsNmea_FixLost()
        {
            setTextBox(statusTextBox, @"Searching");
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

    }
}