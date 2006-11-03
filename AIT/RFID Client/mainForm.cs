using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Text;
using System.IO.Ports;
using System.Windows.Forms;
using System.Net.Sockets;
using System.Net;
using System.Threading;
using System.Runtime.InteropServices;
using WJ.MPR.Reader;
using WJ.MPR.Util;
using System.Xml;
using RFIDProtocolLibrary;
using Bluetooth;
using OpenNETCF.Windows.Forms;
using OwnerDrawnListFWProject;
using ListItemNS;
using Microsoft.Win32;

namespace AIT
{
    public partial class mainForm : Form
    {
        [DllImport("coredll.dll")]
        private static extern bool QueryPerformanceCounter(out long lpPerformanceCount);
        [DllImport("coredll.dll")]
        private static extern bool QueryPerformanceFrequency(out long lpFrequency);

        static void Main()
        {
            Application.Run(new mainForm());
        }

        #region member variables

        private const int latLongArraySize = 128;

        private MPRReader Reader;
        private byte isScan;
        private Browser webBr;
        private ArrayList inventoryTags;
        
        private string gpsBuffer;

        private string ipPhoneNumber;
        private string tagServer;

        private double latitude;
        private double longitude;
        private char EorW;
        private char NorS;
        private bool fullOnce = false;
        private int latLongPtr = 0;
        private double[] latitudeValues = new double[latLongArraySize];
        private double[] longitudeValues = new double[latLongArraySize];
        private double avgLat = 0;
        private double avgLong = 0;
        private static byte[] mac = { 0x00, 0x03, 0x7a, 0x23, 0xc6, 0xd6 }; // mac address for the DAQ
        private RadiationSensor radSensor = new RadiationSensor(mac);
        private Object timerLock = new Object();
        
        private int selectedIndex;

        private CustomListBox custList;

        private CustomMenu custMenu;

        private ClientConnection cc;
        private static Mutex gpsBufLock = new Mutex();
        
        public delegate void GPSTextModifiedEventHandler(object sender, EventArgs e);

        public delegate void DrawItemEventHandler(object sender, DrawItemEventArgs e);

        public enum DrawItemState
        {
            None = 0,
            Selected = 1,
            Disabled = 4,
            Focus = 16
        }

        public event GPSTextModifiedEventHandler gpsMod;

        #endregion

        #region GUI Event Handlers

        /// <summary>
        /// The constructor for the class, sets a few things up
        /// </summary>
        public mainForm()
        {
            InitializeComponent();
            this.Text = Text + " v" + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString();
        }

        /// <summary>
        /// This event is fired whenever the DLL requests an inventory from the MPR.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Reader_InvPollEvent(object sender, EventArgs e)
        {
        }

        /// <summary>
        /// Event Fired when Main Form is loaded.
        /// All init stuff goes here
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Form1_Load(object sender, System.EventArgs e)
        {
            // setup the custom listbox
            custList = new CustomListBox();
            custList.Width = this.Width;
            custList.Height = 250;
            custList.Location = new Point(0, 0);
            custList.MouseUp += new MouseEventHandler(inventoryListBox_SelectedIndexChanged);

            this.tabPage1.Controls.Add(this.custList);

            //setup the custom menu
            custMenu = new CustomMenu();
            custMenu.Location = new Point(5, 20);
            custMenu.Width = 100;
            custMenu.BackColor = Color.LightSkyBlue;
            custMenu.Add("default");
            custMenu.Height += 2;
            custMenu.MouseUp += new MouseEventHandler(inventoryItemOptionMenuClicked);

            this.tabPage1.Controls.Add(custMenu);


            comboBox1.SelectedIndex = 0;

            gpsMod += new GPSTextModifiedEventHandler(GPSTextModifiedEvent);
            Reader = new MPRReader();
            Reader.DisconnectEvent += new EventHandler(Reader_DisconnectEvent);
            Reader.InvPollEvent += new EventHandler(Reader_InvPollEvent);
            Reader.InvTimerEnabledChanged += new EventHandler(Reader_InvTimerEnabledChanged);
            Reader.ManufacturingInformationChanged += new EventHandler(Reader_ManufacturingInformationChanged);
            Reader.StatusEvent += new WJ.MPR.Util.StringEventHandler(Reader_StatusEvent);
            Reader.TagAdded += new TagEventHandler(Reader_TagAdded);
            Reader.TagRemoved += new TagEventHandler(Reader_TagRemoved);
            Reader.Class1InventoryEnabled = true;
            Reader.Class0InventoryEnabled = true;
            Reader.PersistTime = TimeSpan.MaxValue;

            webBr = new Browser();
            webBr.Visible = false;
            invTimerBox.SelectedIndex = 0;
            cc = new ClientConnection();
            inventoryTags = new ArrayList();

            NorS = 'N';
            EorW = 'E';

            latitude = 38.991671;
            longitude = -76.901808;

            latBox.Text = latitude.ToString() + NorS;
            longBox.Text = longitude.ToString() + EorW;
            satBox.Text = "5";
            utcBox.Text = "15:23:44.323";

            cc.HostName = hostnameBox.Text;

            string[] COMPorts = System.IO.Ports.SerialPort.GetPortNames();
			Array.Sort(COMPorts);
            cbComPort.Items.Clear();
			btComPort.Items.Clear();
            foreach (string sCOMPort in COMPorts)
            {
                cbComPort.Items.Add(sCOMPort);
                btComPort.Items.Add(sCOMPort);
            }
			
            if (cbComPort.Items.Count > 0)
            {
                cbComPort.SelectedIndex = 0;
                cbComPort.Enabled = true;
            }
            if (btComPort.Items.Count > 0)
            {
                btComPort.SelectedIndex = 0;
                btComPort.Enabled = true;
            }


            RegistryKey regKey = Registry.LocalMachine;

            regKey = regKey.OpenSubKey(@"SOFTWARE", true);
            
            if (Array.IndexOf(regKey.GetSubKeyNames(), @"AIT", 0) == -1)
                regKey.CreateSubKey(@"AIT");
            regKey = regKey.OpenSubKey(@"AIT",true);

            string[] settingKeys = regKey.GetSubKeyNames();

            //user ID
            if (Array.IndexOf(settingKeys, @"userid", 0) == -1)
            {
                regKey.CreateSubKey(@"userid");
                regKey.SetValue(@"userid", @"test");
            }
            userIDBox.Text = regKey.GetValue(@"userid").ToString();

            // password
            if (Array.IndexOf(settingKeys, @"password", 0) == -1)
            {
                regKey.CreateSubKey(@"password");
                regKey.SetValue(@"password", @"test2000");
            }
            passwordBox.Text = regKey.GetValue(@"password").ToString();

            // hostname
            if (Array.IndexOf(settingKeys, @"hostname", 0) == -1)
            {
                regKey.CreateSubKey(@"hostname");
                regKey.SetValue(@"hostname", @"192.168.2.3");
            }
            cc.HostName = hostnameBox.Text = regKey.GetValue(@"hostname").ToString();

            // port
            if (Array.IndexOf(settingKeys, @"port", 0) == -1)
            {
                regKey.CreateSubKey(@"port");
                regKey.SetValue(@"port", RFIDProtocolLibrary.Packet.port.ToString());
            }
            portBox.Text = regKey.GetValue(@"port").ToString();
            cc.Port = int.Parse(portBox.Text);

            // tagserver
            if (Array.IndexOf(settingKeys, @"tagserver", 0) == -1)
            {
                regKey.CreateSubKey(@"tagserver");
                regKey.SetValue(@"tagserver", @"192.168.2.7");
            }
            tagServer = tagServerBox.Text = regKey.GetValue(@"tagserver").ToString();

            // ip phone address
            if (Array.IndexOf(settingKeys, @"ipphone", 0) == -1)
            {
                regKey.CreateSubKey(@"ipphone");
                regKey.SetValue(@"ipphone", @"192.168.2.3");
            }
            ipPhoneNumber = ipPhoneBox.Text = regKey.GetValue(@"ipphone").ToString();

        }

        /// <summary>
        /// Event fired when
        /// </summary>
        /// <param name="Message"></param>
        private void Reader_StatusEvent(string Message)
        {
            if (InvokeRequired)
                Invoke(new StringEventHandler(Reader_StatusEvent), new object[] { Message });
            else
            {
                //Logit(Message);
            }
        }

        /// <summary>
        /// This event is fired when a the DLL loses contact with an MPR.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Reader_DisconnectEvent(object sender, EventArgs e)
        {
            lblOnline.ForeColor = Color.Red;
            lblOnline.Text = "Offline";
            invProgressBar.Value = 0;
            invProgressBar.Hide();
        }

        /// <summary>
        /// This event is fired when the DLL starts or stops an inventory loop.
        /// Also fired when the inventory timer runs out
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void startInventoryButton_Click(object sender, EventArgs e)
        {
            // continuous polling, start or stop
            if (invTimerBox.Text != "Manual" && sender == startInventoryButton)
            {
                // first do a manifest
                isScan = 0;

                // then enable the timer
                if (invTimer.Enabled == true)
                {
                    invTimerBox.Visible = true;
                    startInventoryButton.Text = "Man";
                    invTimer.Enabled = false;
                    Cursor.Current = Cursors.Default;
                }
                else
                {
                    invTimerBox.Visible = false;
                    startInventoryButton.Text = "Stop";
                    invTimer.Interval = 1000 * int.Parse(invTimerBox.Text.ToString().Substring(0, invTimerBox.Text.ToString().Length - 1));
                    invTimer.Enabled = true;
                    Cursor.Current = Cursors.WaitCursor;
                }
            }
            else
            {
                if (sender == scanButton)
                    isScan = 1;
                else
                    isScan = 0;

                if ((string)comboBox1.SelectedItem == "WJ")
                {
                    if (!Reader.IsConnected)
                        readerConnect(false);

                    Reader.InvTimerEnabled = !Reader.InvTimerEnabled;
                }
                else if ((string)comboBox1.SelectedItem == "Symbol")
                {
                    Cursor.Current = Cursors.WaitCursor;
                    scanButton.Enabled = false;
                    startInventoryButton.Enabled = false;

                    getInventorySymbol(sender);

                    EventArgs eA = new EventArgs();
                    Reader_InvTimerEnabledChanged(sender, eA);

                    scanButton.Enabled = true;
                    startInventoryButton.Enabled = true;
                    Cursor.Current = Cursors.Default;
                }
            }
        }

        private void getInventorySymbol(object sender)
        {
            inventoryTags.Clear();

            try
            {
                // go get the xml                
                XmlTextReader reader = new XmlTextReader(@"http://" + tagServer + @"/cgi-bin/dataProxy?oper=queryTags&invis=1&showLastRP=1&raw=1");
                reader.WhitespaceHandling = WhitespaceHandling.None;

                while (reader.Read())
                {
                    if (reader.Name == @"Tag")
                        try
                        {
                            reader.MoveToAttribute(0);
                            inventoryTags.Add(new RFID(reader.Value.ToString()));
                        }
                        catch (ArgumentException e)
                        {
                            Console.Out.WriteLine(e.Message);
                        }
                }

                reader.Close();

                XmlTextReader clearer = new XmlTextReader(@"http://" + tagServer + @"/cgi-bin/dataProxy?oper=queryEvents&raw=1&invis=1");
                clearer.Read();
                clearer.Close();

                XmlTextReader clearer2 = new XmlTextReader(@"http://" + tagServer + @"/cgi-bin/dataProxy?oper=purgeAllTags");
                clearer2.Read();
                clearer2.Close();
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
            }
        }       

        /// <summary>
        /// This event is fired when the DLL starts or stops an inventory loop.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Reader_InvTimerEnabledChanged(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new EventHandler(Reader_InvTimerEnabledChanged), new object[] { sender, e });
            else
            {
                if (Reader.InvTimerEnabled) /// starting an inventory loop
                {
                    if (invTimer.Enabled == false)
                    {
                        if (isScan == 1)
                            startInventoryButton.Enabled = false;
                        else
                            scanButton.Enabled = false;
                    }

                    invProgressBar.Value = 0;
                    inventoryTags.Clear();
                    invProgressBar.Show();
                    invTimerBox.Hide();
                    custList.Clear();
                    custList.Refresh();
                    Reader.ClearInventory();
                }
                else /// stopping an inventory loop
                {
                    custList.Clear();

                    try
                    {
                        if (invTimer.Enabled == false)
                        {
                            scanButton.Enabled = false;
                            startInventoryButton.Enabled = false;

                            invProgressBar.Hide();
                            invTimerBox.Show();

                            if (sender == scanButton)
                                reconcileButton.Enabled = true;
                            else
                            {
                                startInventoryButton.Enabled = true;
                                scanButton.Enabled = true;
                            }
                        }

                        if (inventoryTags.Count > 0)
                        {
                            inventoryTags = cc.sendScan(inventoryTags, latitude, NorS, longitude, EorW, isScan);
                            foreach (ListItem a in inventoryTags)
                            {
                                custList.Add(a);
                            }

                        }
                    }
                    catch (SocketException ex)
                    {
                        Console.Out.WriteLine(ex.Message);
                    }
                    // for more serious errors, display this
                    catch (ArgumentException ex)
                    {
                        alertAndShowBox(ex);

                        if (invTimer.Enabled == false)
                        {
                            scanButton.Enabled = true;
                            startInventoryButton.Enabled = true;
                            reconcileButton.Enabled = false;
                        }
                    }
                    finally
                    {
                        foreach (object a in inventoryTags)
                        {
                            custList.Add(new ListItem(a.ToString(), "", -2)); // -2 for unknown status
                        }
                        custList.RefreshVal();
                        inventoryTags.Clear();
                    }
                }
            }
        }

        [Conditional("DEBUG")]
        private static void alertAndShowBox(ArgumentException ex)
        {
            Console.Out.WriteLine(ex.Message);

            MessageBox.Show(ex.Message);
        }        

        /// <summary>
        /// Event fired when the antenna radio button changes
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        {
            radioButton2.Checked = !radioButton1.Checked;
            Reader.ActiveAntenna = (byte)((radioButton2.Checked) ? 0 : 1);
        }

        /// <summary>
        /// Event fired when the DLL's copy of the Reader's
        /// Manufacturing Information has changed.  In other words,
        /// the DLL reads the information and it was different from what it was before.
        /// This should only happen when the first time the info is read, 
        /// or when a different MPR is connected.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Reader_ManufacturingInformationChanged(object sender, EventArgs e)
        {
            numericUpDown1.Minimum = Reader.TxPowerMin;
            numericUpDown1.Maximum = Reader.TxPowerMax < 27 ? Reader.TxPowerMax : 27;
            if (Reader.TxPower < 27)
                numericUpDown1.Value = Reader.TxPower;
            else
                numericUpDown1.Value = 27;
        }

        /// <summary>
        /// Event fired when the txPower button is changed
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void numericUpDown1_ValueChanged(object sender, EventArgs e)
        {
            Reader.TxPower = (byte)numericUpDown1.Value;
        }


        /// <summary>
        /// Event fired when the clear button is pressed, gives the user the option to not
        /// clear the inventory after all with a messagebox
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void clearBtn_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Really clear inventory?", "Inventory will be cleared.", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation, MessageBoxDefaultButton.Button1) == DialogResult.Yes)
            {
                custList.Clear();
                Reader.ClearInventory();
            }
        }

        /// <summary>
        /// Event fired when the user clicks on a new item
        /// Pulls up a built-in web browser that shows the web page
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void inventoryListBox_SelectedIndexChanged(object sender, MouseEventArgs e)
        {
            custMenu.Hide();

            selectedIndex = custList.vScrollValue + (e.Y / custList.ItemHeight);

            if (selectedIndex >= custList.Items.Count)
                return;

            custList.SelectedIndex = -1;
            
            custMenu.Clear();
            
            if (((ListItem)custList.Items[selectedIndex]).missingAdded <= 0)
                custMenu.Add("Remove");
            else if (((ListItem)custList.Items[selectedIndex]).missingAdded == 1)
                custMenu.Add("Add");
            custMenu.Add("Show WWW");

            custMenu.Show(new Point(Screen.PrimaryScreen.Bounds.Right - 5 - custMenu.Width, e.Y));

            custMenu.RefreshVal();
            
            base.OnMouseUp(e);
        }

        /// <summary>
        /// Event fired when the user clicks on a new item
        /// Pulls up a built-in web browser that shows the web page
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void inventoryItemOptionMenuClicked(object sender, MouseEventArgs e)
        {
            if (!custMenu.ClientRectangle.Contains(e.X, e.Y))
                return;

            try
            {
                switch (custMenu.action)
                {
                    case -1: // remove
                        cc.removeItem(((ListItem)custList.Items[selectedIndex]).RFIDNum);
                        break;

                    case 1: // add
                        cc.addItem(((ListItem)custList.Items[selectedIndex]).RFIDNum);
                        break;

                    case 0:
                        string selectedTag = inventoryTags[selectedIndex].ToString().Replace(" ", "");

                        string lastTwo = "0x" + selectedTag.Substring(selectedTag.Length - 2);

                        int lastTwoI = Convert.ToInt32(lastTwo, 16);

                        selectedTag = selectedTag.Substring(0, selectedTag.Length - 2) + lastTwoI.ToString();

                        webBr.setUrlAndShow(new Uri("http://" + "tbk.ece.umd.edu/pda_item.jsp?rfid=" + selectedTag));
                        break;
                    default: break;
                }
            }
            catch (Exception ex)
            {
                Console.Out.WriteLine(ex.Message.ToString());
            }
        }


        /// <summary>
        /// Event fired when the GPS connect button is pressed
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void BTbutton_Click(object sender, EventArgs e)
        {
            try
            {
                if (!gpsSerialPort.IsOpen)
                {
                    gpsSerialPort.PortName = btComPort.SelectedItem.ToString();
                    gpsSerialPort.BaudRate = 4800;
                    gpsSerialPort.DataBits = 8;
                    gpsSerialPort.Parity = Parity.None;
                    gpsSerialPort.StopBits = StopBits.One;
                    gpsSerialPort.Open();
                    gpsButton.Text = "Disconnect";
                }
                else
                {
                    gpsSerialPort.Close();
                    gpsButton.Text = "GPSConnect";
                }
            }
            catch (Exception ex)
            {
                gpsButton.Text = "GPSConnect";
                MessageBox.Show(ex.Message.ToString());
            }
        }

        /// <summary>
        /// Event fired when the GPS serial port gets the threshold amount of data
        /// The data is immediately copied into another buffer and an event is called to process
        /// this data, then it is displayed
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void serialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            if (InvokeRequired)
                Invoke(new SerialDataReceivedEventHandler(serialPort_DataReceived), new object[] { sender, e });
            else
            {
                gpsBufLock.WaitOne();
                gpsBuffer += gpsSerialPort.ReadExisting();
                gpsBufLock.ReleaseMutex();
                gpsSerialPort.DiscardInBuffer();

                // fire off an event to signal the other thread to process this event
                GPSTextModifiedEventHandler handler = gpsMod;
                EventArgs a = new EventArgs();
                if (handler != null)
                {
                    handler(this, a);
                }
            }
        }

        /// <summary>
        /// Event fired when the GPS serial port processing function has modified data and is
        /// calling for an update
        /// </summary>
        /// <param name="o"></param>
        /// <param name="e"></param>
        private void GPSTextModifiedEvent(object o, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new GPSTextModifiedEventHandler(GPSTextModifiedEvent), new object[] { o, e });
            else
            {
                updateGPSBoxes();
            }
        }

        /// <summary>
        /// Event fired when the number of known tags changes, so that the progress bar will be accurate
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void numTags_ValueChanged(object sender, EventArgs e)
        {
            invProgressBar.Maximum = int.Parse(numTags.Value.ToString());
        }

        /// <summary>
        /// Event fired when the user clicks connect to connect to the WJ Reader
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void wjConnectButton_Click(object sender, EventArgs e)
        {
            if ((string)comboBox1.SelectedItem == "WJ")
                readerConnect(true);
        }

        #endregion

        #region Functions

        /// <summary>
        /// Event that fires when a new Tag is added to the Inventory.
        /// A Tag stays in the inventory as long as it has been read 
        /// within the Persist Time. 
        /// </summary>
        /// <param name="Tag"></param>
        private void Reader_TagAdded(RFIDTag Tag)
        {
            if (InvokeRequired)
                Invoke(new TagEventHandler(Reader_TagAdded), new object[] { Tag });
            else
            {
                inventoryTags.Add(Tag);
                invProgressBar.Value++;
            }
        }

        /// <summary>
        /// Event fired when a tag is removed from the Inventory.
        /// A tag is removed when it has not been read for at least
        /// PersistTime.
        /// </summary>
        /// <param name="Tag"></param>
        private void Reader_TagRemoved(RFIDTag Tag)
        {
            if (InvokeRequired)
                Invoke(new TagEventHandler(Reader_TagRemoved), new object[] { Tag });
            else
            {
                //ArrayList TagsToRemove = new ArrayList();
                //foreach (object ItemInList in lbTags.Items)
                //    if (ItemInList.ToString() == Tag.TagID)
                //        TagsToRemove.Add(ItemInList);
                //while (TagsToRemove.Count != 0)
                //{
                //lbTags.Items.Remove(TagsToRemove[0]);
                //    TagsToRemove.RemoveAt(0);
                //}
            }
        }

        /// <summary>
        /// Function that tries to connect to the WJ reader
        /// </summary>
        /// <param name="disconnect"></param>
        private void readerConnect(bool disconnect)
        {
            try
            {
                Reader.TxPower = byte.Parse(numericUpDown1.Value.ToString());
                if (Reader.IsConnected)
                {
                    if (disconnect == true)
                        Reader.Disconnect();
                    else
                        return;
                }

                Reader.Connect(cbComPort.Text, "57600");

                if (Reader.IsConnected)
                {
                    lblOnline.ForeColor = Color.Green;
                    lblOnline.Text = "Online";
                }
                else
                {
                    MessageBox.Show(string.Format("Could not connect to MPR at {0}.", cbComPort.Text));
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }



        /// <summary>
        /// Does the actual updating of the GPS information values, which will be put into boxes later
        /// </summary>
        private void updateGPSBoxes()
        {
            try
            {
                string startCode = "$GPGGA";

                gpsBufLock.WaitOne();

                int start = gpsBuffer.LastIndexOf(startCode);

                if (start < 0)
                {
                    gpsBufLock.ReleaseMutex();
                    return;
                }

                string code = gpsBuffer.Substring(start, 78);

                if (code.Length < 78)
                {
                    start = gpsBuffer.IndexOf(startCode);
                    code = gpsBuffer.Substring(start);
                    if (code.Length < 78)
                    {
                        gpsBufLock.ReleaseMutex();
                        return;
                    }
                }

                gpsBuffer = "";
                gpsBufLock.ReleaseMutex(); // release the lock

                char[] sep = { ',' };
                string[] fields = code.Split(sep);

                if (fields[1].Length == 0)
                    return;

                // set the time
                utcBox.Text = int.Parse(fields[1].Substring(0, 2)) % 12 + ":"
                    + fields[1].Substring(2, 2) + ":"
                    + fields[1].Substring(4, 2) + " "
                    + ((int.Parse(fields[1].Substring(0, 2)) / 12) == 1 ? "PM" : "AM");

                // calculate latitude, longitude, and add into running average
                double latitudeL = double.Parse(fields[2].Substring(0, 2))
                    + double.Parse(fields[2].Substring(2)) / 60.0;

                double longitudeL = double.Parse(fields[4].Substring(0, 3)) +
                    double.Parse(fields[4].Substring(3)) / 60.0;

                EorW = fields[5][0];
                NorS = fields[3][0];

                avgLat -= latitudeValues[latLongPtr];
                avgLat += latitudeL;
                latitudeValues[latLongPtr] = latitudeL;

                avgLong -= longitudeValues[latLongPtr];
                avgLong += longitudeL;
                longitudeValues[latLongPtr] = longitudeL;

                latLongPtr = (latLongPtr + 1) % latLongArraySize;

                if (latLongPtr == 0)
                {
                    fullOnce = true;
                }

                latitude = avgLat / ((fullOnce ? latLongArraySize : latLongPtr) * 1.0);
                longitude = avgLong / ((fullOnce ? latLongArraySize : latLongPtr) * 1.0);

                latBox.Text = System.Math.Floor(latitude) + "° "
                    + (latitude - System.Math.Floor(latitude)).ToString().Substring(2, 2) + "."
                    + (latitude - System.Math.Floor(latitude)).ToString().Substring(4) + "' "
                    + NorS;

                longBox.Text = System.Math.Floor(longitude) + "° "
                    + (longitude - System.Math.Floor(longitude)).ToString().Substring(2, 2) + "."
                    + (longitude - System.Math.Floor(longitude)).ToString().Substring(4) + "' "
                    + EorW;

                satBox.Text = fields[7].TrimStart('0');

                return;
            }
            catch (ArgumentOutOfRangeException ex)
            {
            }
        }

        #endregion

        private void mapButton_Click(object sender, EventArgs e)
        {
            //webBr.setUrlAndShow(new Uri("http://terraserver.microsoft.com/image.aspx?PgSrh:NavLon=-" + lon.ToString() + "&PgSrh:NavLat=" + lat.ToString()));
            
            webBr.setUrlAndShow(new Uri("http://maps.google.com/maps?f=q&hl=en&q="
                + latitude.ToString() + "+"
                + longitude.ToString() + "&t=h"));
        }


        private void radConnectButton_Click(object sender, EventArgs e)
        {
            try
            {
                if (!radTimer.Enabled)
                {
                    radSensor.connect();
                    radTimer.Interval = int.Parse(radUpDown.Value.ToString()) * 1000;
                    radTimer.Enabled = true;
                    radConnectButton.BackColor = Color.DarkMagenta;
                }
                else
                {
                    radSensor.disconnect();
                    radTimer.Enabled = false;
                    radConnectButton.BackColor = Color.Cyan;
                }
            }
            catch (Exception ex)
            {
                radTimer.Enabled = false;
                radConnectButton.BackColor = Color.Green;
            }
        }

        private void radTimer_Tick(object sender, EventArgs e)
        {
            try
            {
                radTimer.Enabled = false;
                if (radSensor.poll())
                {
                    cc.sendAlert();
                }
            }
            catch (Exception ex)
            {
                radConnectButton.BackColor = Color.Green;
                radSensor.disconnect();
            }
            finally
            {
                radTimer.Enabled = true;
            }
        }        

        private void textBox1_GotFocus(object sender, EventArgs e)
        {
            inputPanel1.Enabled = true;
        }

        private void textBox1_LostFocus(object sender, EventArgs e)
        {
            RegistryKey regKey = Registry.LocalMachine;

            regKey = regKey.OpenSubKey(@"SOFTWARE", true);

            if (Array.IndexOf(regKey.GetSubKeyNames(), @"AIT", 0) == -1)
                regKey.CreateSubKey(@"AIT");
            regKey = regKey.OpenSubKey(@"AIT", true);

            if (sender == portBox)
            {
                regKey.SetValue(@"port", portBox.Text);
                cc.Port = int.Parse(portBox.Text);
            }
            else if (sender == hostnameBox)
            {
                regKey.SetValue(@"hostname", hostnameBox.Text);
				cc.HostName = hostnameBox.Text;
            }
            else if (sender == userIDBox)
            {
                regKey.SetValue(@"userid", userIDBox.Text);
            }
            else if (sender == passwordBox)
            {
                regKey.SetValue(@"password", passwordBox.Text);
            }
            else if (sender == tagServerBox)
            {
                regKey.SetValue(@"tagserver", tagServerBox.Text);
                tagServer = tagServerBox.Text;
            }
            else if (sender == ipPhoneBox)
            {
                regKey.SetValue(@"phonenumber", ipPhoneBox.Text);
                ipPhoneNumber = ipPhoneBox.Text;
            }

            inputPanel1.Enabled = false;
        }

        private void tabPage1_MouseUp(object sender, MouseEventArgs e)
        {
            custMenu.Hide();
        }

        private void reconcileButton_Click(object sender, EventArgs e)
        {
            cc.Reconcile(ipPhoneBox.Text);

            startInventoryButton.Enabled = true;
            scanButton.Enabled = true;
            reconcileButton.Enabled = false;
        }

        private void hostnameBox_LostFocus(object sender, EventArgs e)
        {
            cc.HostName = hostnameBox.Text;
            textBox1_LostFocus(sender, e);
        }

        private void invTimer_Tick(object sender, EventArgs e)
        {
            Thread thr = new Thread(new ThreadStart(doDemo));
            thr.Start();
        }

        private void doDemo()
        {
            if (Monitor.TryEnter(timerLock) == false)
                return;
            else
            {
                long timeA, timeB;
                QueryPerformanceCounter(out timeA);
                getInventorySymbol(this);
                QueryPerformanceCounter(out timeB);

                if (inventoryTags.Count > 0)
                {
                    try
                    {
                        inventoryTags = cc.sendScan(inventoryTags, latitude, NorS, longitude, EorW, isScan);
                        
                        cc.Reconcile(ipPhoneNumber);
                    }
                    catch (SocketException ex)
                    {
                        Console.Out.WriteLine(ex.Message);
                    }
                    // for more serious errors, display this
                    catch (ArgumentException ex)
                    {                        
                        alertAndShowBox(ex);                        
                    }
                    finally
                    {
                        showInventoryItems(this, new EventArgs());
                    }
                    
                }
                if (isScan == 0)
                    isScan = 1;

                
                long freq;
                QueryPerformanceFrequency(out freq);

                //MessageBox.Show("Time Taken: " + ((timeB - timeA) / freq).ToString());
            }
            //Thread.Join(); // join to the clearer thread
            Monitor.Exit(timerLock);
        }

        private void showInventoryItems(object sender, EventArgs e)
        {
            if (InvokeRequired)
                Invoke(new EventHandler(showInventoryItems), new object[] { sender, e });
            else
            {
                custList.Clear();
                foreach (object a in inventoryTags)
                {
                    custList.Add(new ListItem(a.ToString(), "", -2)); // -2 for unknown status
                }
                custList.RefreshVal();
            }
        }



        private void invTimerBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if ((string)invTimerBox.SelectedItem == "Manual")
                scanButton.Enabled = true;
            else
                scanButton.Enabled = false;
        }

        
    }
}
