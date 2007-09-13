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
using Microsoft.WindowsMobile.Status;
using GentagDemo;
using System.Web.Services.Protocols;
using SiritReader;


[assembly: CLSCompliant(true)]
//[assembly: SecurityPermission(SecurityAction.RequestMinimum, Execution = true)]
namespace GentagDemo
{
    public partial class wineClient : Form
    {
        bool readerRunning;

        static wineClient mF;

        string DeviceUID;

        NativeMethods tagReader = new NativeMethods();

        public wineClient()
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

            mF = this;

            // setup event callbacks for tag reading events
            tagReader.TagReceived += new NativeMethods.TagReceivedEventHandler(tagReceived);
            tagReader.ReaderError += new NativeMethods.ReaderErrorHandler(receiveReaderError);

            ImageList myImageList = new ImageList();
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.blank.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.cancel.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.ok.GetHbitmap()));
            myImageList.Images.Add(Image.FromHbitmap(GentagDemo.Properties.Resources.question.GetHbitmap()));
            treeView1.ImageList = myImageList;
        }

        // the variable that describes whether it's looping looking for wine bottles or general tags
        private bool wineLoop;

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void readerClick(object sender, EventArgs e)
        {
            if (readerRunning == false)
            {
                readerRunning = true;
                setWaitCursor(true);

                if ((sender == readIDButton) || (sender == wineButton))
                {
                    if (sender == wineButton)
                    {
                        wineLoop = true;
                    }
                    else
                    {
                        wineLoop = false;
                    }
                    new Thread(new ThreadStart(tagReader.readTagID)).Start();
                    new Thread(new ThreadStart(doLookup)).Start();
                }

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

        Queue<string> tagQueue = new Queue<string>();

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void tagReceived(string tagID)
        {
            Button button = wineLoop ? wineButton : readIDButton;
            Hashtable cache = wineLoop ? cachedWineLookups : cachedIDLookups;

            lock (tagQueue)
            {
                if (wineLoop)
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




        private Hashtable cachedWineLookups = new Hashtable();

        private Hashtable cachedIDLookups = new Hashtable();

        private Hashtable IDsCurrentlyBeingLookedUp = new Hashtable();

        private Hashtable wineIDsCurrentlyBeingLookedUp = new Hashtable();

        private bool wineChanged;

        private void doLookup()
        {
            Hashtable cache = wineLoop ? cachedWineLookups : cachedIDLookups;

            while ((readerRunning) || (tagQueue.Count > 0))
            {
                string currentTag;

                lock (tagQueue)
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
                }
                else // hashtable does not contain info on this tag
                {
                    // if a request for this tag has already been made
                    if (IDsCurrentlyBeingLookedUp.ContainsValue(currentTag) && !wineLoop)
                        continue;
                    if (wineIDsCurrentlyBeingLookedUp.ContainsValue(currentTag) && wineLoop)
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
                            org.dyndns.criusWine.WineWS ws = new org.dyndns.criusWine.WineWS();
                            ws.Timeout = 300000;
                            handle = ws.BeginretrieveBottleInformation(currentTag, DeviceUID, 0, 0, cb, ws);
                            wineIDsCurrentlyBeingLookedUp[handle] = currentTag;
                        }
                        else
                        {
                            // get the tag info and cache it
                            org.dyndns.criusWine.WineWS ws = new org.dyndns.criusWine.WineWS();
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
            Hashtable cache = wineLoop ? cachedWineLookups : cachedIDLookups;

            try
            {
                if (wineLoop)
                {
                    org.dyndns.criusWine.WineWS ws = (org.dyndns.criusWine.WineWS)ar.AsyncState;
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
                else
                {
                    org.dyndns.criusWine.WineWS ws = (org.dyndns.criusWine.WineWS)ar.AsyncState;
                    org.dyndns.criusWine.InfoPacket info = ws.EndgetItem(ar);

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
                // get the bottle info and cache it
                org.dyndns.criusWine.WineWS ws = new org.dyndns.criusWine.WineWS();
                ws.Timeout = 300000;
                handle = ws.BeginretrieveBottleInformation(currentTag, DeviceUID, 0, 0, cb, ws);

            }
            else
            {
                // get the tag info and cache it
                org.dyndns.criusWine.WineWS ws = new org.dyndns.criusWine.WineWS();
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
            else if (e.KeyCode == System.Windows.Forms.Keys.F2)
            {
                if (tabControl1.TabPages[tabControl1.SelectedIndex].Name == "authPage")
                    readerClick(readIDButton, new EventArgs());
                else if (tabControl1.TabPages[tabControl1.SelectedIndex].Name == "winePage")
                    readerClick(wineButton, new EventArgs());
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