using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using RFIDReader;
using System.Collections;
using System.Runtime.CompilerServices;
using System.Globalization;
using System.Threading;
using System.Net.Sockets;
using System.Net;
using System.IO;
using System.Web.Services.Protocols;

namespace GentagPet
{
    public partial class gentagPet : Form
    {
        bool readerRunning;

        string DeviceUID;

        Reader tagReader = new Reader();


        public gentagPet()
        {
            InitializeComponent();

            // generate the device's UID
            string AppString = GentagPet.Properties.Resources.appTitle;

            byte[] AppData = new byte[AppString.Length];

            for (int count = 0; count < AppString.Length; count++)
                AppData[count] = (byte)AppString[count];

            int appDataSize = AppData.Length;

            byte[] dUID = new byte[20];

            uint SizeOut = 20;

            Reader.GetDeviceUniqueID(AppData, appDataSize, 1, dUID, out SizeOut);

            for (int i = 0; i < 20; i++)
                DeviceUID += dUID[i].ToString("X", CultureInfo.CurrentUICulture);

            // setup callbacks
            // setup event callbacks for tag reading events
            tagReader.TagReceived += new Reader.TagReceivedEventHandler(tagReceived);
            tagReader.ReaderError += new Reader.ReaderErrorHandler(receiveReaderError);

        }

        private void readerClick(object sender, EventArgs e)
        {

        }

        private void menuItem2_Click(object sender, EventArgs e)
        {
            if (readerRunning == false)
            {
                readerRunning = true;
                menuItem2.Text = "Stop";
                setWaitCursor(true);

                new Thread(new ThreadStart(tagReader.readTagID)).Start();
            }
            else
            {
                menuItem2.Text = "Lookup";
                stopReading();
            }
        }

        private Hashtable cachedPetLookups = new Hashtable();

        private Hashtable petIDsCurrentlyBeingLookedUp = new Hashtable();

        private void stopReading()
        {
            setWaitCursor(false);
            readerRunning = false;
            tagReader.running = false;
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void receiveReaderError(string errorMessage)
        {
            setWaitCursor(false);
            readerRunning = false;
            MessageBox.Show(errorMessage);
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void tagReceived(string tagID, bool doneReading)
        {
            if (string.IsNullOrEmpty(tagID)) // if there was no string returned
                return;

            AsyncCallback cb = new AsyncCallback(receiveNewItem);
            Color oldColor = this.BackColor;
            this.BackColor = Color.Gainsboro;

            try
            {
                if (cachedPetLookups.ContainsKey(tagID))
                    displayPet((petWS.petInfo)cachedPetLookups[tagID]);
                else if (petIDsCurrentlyBeingLookedUp.ContainsValue(tagID))
                    return;
                else
                {
                    petWS.petWS ws = new petWS.petWS();
                    ws.Timeout = 300000;
                    //IAsyncResult handle = ws.BeginretrievePetInformation(tagID, DeviceUID, gpsInterpreter.getLatitude(), gpsInterpreter.getLongitude(), cb, ws);
                    IAsyncResult handle = ws.BeginretrievePetInformation(tagID, DeviceUID, 0.0F, 0.0F, cb, ws);
                    lock (petIDsCurrentlyBeingLookedUp.SyncRoot)
                    { petIDsCurrentlyBeingLookedUp[handle] = tagID; }
                }

            }
            catch (SoapException ex)
            {
                MessageBox.Show("Unexpected SOAP exception: " + ex.Message);
            }


            // flash the panel to signal the user that a tag was read

            Thread.Sleep(150);
            this.BackColor = oldColor;

        }


        private void receiveNewItem(IAsyncResult ar)
        {
            try
            {

                // try to cast as petInfo
                petWS.petWS ws = (petWS.petWS)ar.AsyncState;
                petWS.petInfo newPet = ws.EndretrievePetInformation(ar);
                displayPet(newPet);
                lock (cachedPetLookups.SyncRoot)
                {
                    cachedPetLookups[newPet.rfidNum] = newPet;
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
                rescheduleLookup(ar);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.ToString());
            }
        }

        private void rescheduleLookup(IAsyncResult ar)
        {
            AsyncCallback cb = new AsyncCallback(receiveNewItem);

                lock (petIDsCurrentlyBeingLookedUp.SyncRoot)
                {
                    string currentTag = (string)petIDsCurrentlyBeingLookedUp[ar];
                    petIDsCurrentlyBeingLookedUp.Remove(ar);
                    petWS.petWS ws = new petWS.petWS();
                    ws.Timeout = 300000;
                    //IAsyncResult handle = ws.BeginretrievePetInformation(currentTag, DeviceUID, gpsInterpreter.getLatitude(), gpsInterpreter.getLongitude(), cb, ws);
                    IAsyncResult handle = ws.BeginretrievePetInformation(currentTag, DeviceUID, 0.0F, 0.0F, cb, ws);
                    petIDsCurrentlyBeingLookedUp[handle] = currentTag;
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
                setLabel(petOwnerLabel, GentagPet.Properties.Resources.blank);
                setLabel(petContactInfoLabel, GentagPet.Properties.Resources.blank);
                setLabel(petPhonenumberLabel, GentagPet.Properties.Resources.blank);
                setLabel(petBreedLabel, GentagPet.Properties.Resources.blank);
                setPhoto(petPB, (System.Drawing.Image)null);
            }
        }
        #region Safe Accessors and Mutators

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
                if (pB.Image != null)
                    pB.Image.Dispose();
                pB.Image = bA;
                pB.Refresh();
            }
            catch (ArgumentException ex)
            {

            }
        }

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
                if (pB.Image != null)
                    pB.Image.Dispose();
                pB.Image = new Bitmap(new MemoryStream(bA));
                pB.Refresh();
            }
            catch (ArgumentException ex)
            {

            }
        }

        #endregion

    }
}