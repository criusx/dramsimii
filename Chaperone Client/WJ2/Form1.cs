using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
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
using RFIDProtocolLib;
//using InTheHand.Net.Bluetooth;
//using InTheHand.Net.Sockets;
//using InTheHand.Net;
//using Oracle.Lite.Data;
//using Oracle.DataAccess.Lite;


namespace WJ2
{
    public partial class Form1 : Form
    {
        [DllImport("coredll.dll")]
        private static extern bool QueryPerformanceCounter(out long lpPerformanceCount);

        static void Main()
        {
            Application.Run(new Form1());
        }

        #region Form1 Properties (variables)

        //private OracleConnection conn = new OracleConnection();
        //public const string authError = "[POL-3013]";

        private MPRReader Reader;
        private Browser webBr;
        private ArrayList inventoryTags;
        private int totalTags;
        private string gpsBuffer;
        private string utcTime;
        private string latitude;
        private string longitude;
        private string numberOfSatellites;
        private event EventHandler eh;
        private ClientConnection cc;
        private static Mutex gpsBufLock = new Mutex();
        private byte originItems;
        private Cursor cursor;
        public delegate void GPSTextModifiedEventHandler(object sender, EventArgs e);

        public event GPSTextModifiedEventHandler gpsMod;

        #endregion

        #region GUI Event Handlers
        
        /// <summary>
        /// The constructor for the class, sets a few things up
        /// </summary>
        public Form1()
        {
            InitializeComponent();
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
            gpsMod += new GPSTextModifiedEventHandler(GPSTextModifiedEvent);
            Reader = new MPRReader();
            Reader.DisconnectEvent += new EventHandler(Reader_DisconnectEvent);
            Reader.InvPollEvent += new EventHandler(Reader_InvPollEvent);
            Reader.InvTimerEnabledChanged += new EventHandler(Reader_InvTimerEnabledChanged);
            Reader.ManufacturingInformationChanged += new EventHandler(Reader_ManufacturingInformationChanged);
            Reader.StatusEvent += new WJ.MPR.Util.StringEventHandler(Reader_StatusEvent);
            Reader.TagAdded += new TagEventHandler(Reader_TagAdded);
            Reader.TagRemoved += new TagEventHandler(Reader_TagRemoved);
            webBr = new Browser();
            webBr.Visible = false;
            waypointBox.SelectedIndex = 0;
            cc = new ClientConnection();
            inventoryTags = new ArrayList();
            //cursor = new ();
           
            string[] COMPorts = System.IO.Ports.SerialPort.GetPortNames();
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
                btComPort.SelectedIndex = 1;
                btComPort.Enabled = true;
            }

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
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void startInventoryButton_Click(object sender, EventArgs e)
        {
            Reader.Class1InventoryEnabled = true;
            Reader.Class0InventoryEnabled = true;
            Reader.PersistTime = new TimeSpan(0, 5, 5, 0);
            readerConnect(false);
            Reader.InvTimerEnabled = !Reader.InvTimerEnabled;
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
                    startInventoryButton.Text = "Stop Inventory";
                    startInventoryButton.BackColor = Color.Red;
                    startInventoryButton.ForeColor = Color.White;
                    invProgressBar.Value = 0;
                    inventoryTags.Clear();
                    invProgressBar.Show();
                    waypointBox.Hide();
                    inventoryListBox.Items.Clear();
                    Reader.ClearInventory();
                }
                else /// stopping an inventory loop
                {
                    startInventoryButton.Text = "Start Inventory";
                    startInventoryButton.BackColor = Color.LightGreen;
                    startInventoryButton.ForeColor = Color.Black;
                    invProgressBar.Hide();
                    waypointBox.Show();

                    //for (int i = 0; i < inventoryTags.Count; ++i)
                    //    inventoryListBox.Items.Insert(i, inventoryTags[i]);
                    //return;
                    try
                    {
                        cc = new ClientConnection();
                        // inventory has ended, now go submit manifest
                        // connect to the server
                        cc.Connect("66.171.240.43", 1555);

                        // handshake
                        cc.SendConnectPacket();
                        cc.WaitForConnectResponsePacket();
                        if (latitude == null)
                            latitude = "00.000";
                        if (longitude == null)
                            longitude = "00.000";
                        //cc.SendQueryPacket(new QueryRequest())

                        byte locL = byte.Parse(waypointBox.SelectedIndex.ToString());
                        if (locL == 0)
                        {
                            originItems = (byte)inventoryTags.Count;
                        }
                        else
                        {
                            if (inventoryTags.Count != originItems)
                            {
                                locL = 2;
                            }
                        }
                        // correlate names with tag IDs and add to the list
                        for (int i = 0; i < inventoryTags.Count; ++i)
                        {   
                            cc.SendQueryPacket(new QueryRequest(new RFID(inventoryTags[i].ToString()), latitude, longitude, locL));

                            QueryResponse qr = cc.WaitForQueryResponsePacket();
                            inventoryListBox.Items.Insert(i, qr.ShortDesc);
                        }
                        cc.Close();
                    }
                    catch (Exception ex)
                    {

                    }

                    return;


                    //for (int i = 0; i < inventoryTags.Count; ++i)
                    //    inventoryListBox.Items.Insert(i, inventoryTags[i]);
                    
                    // old server code
                    // form the packet
//                     Byte[] packet = new Byte[16];
//                     try
//                     {
//                         Socket conn2 =
//                             new Socket(AddressFamily.InterNetwork,
//                             SocketType.Stream,
//                             ProtocolType.Tcp);
//                         IPAddress server = IPAddress.Parse("192.168.1.100");
//                         int port = 2000;
//                         IPEndPoint endpoint = new IPEndPoint(server, port);
//                         conn2.Connect(endpoint);
//                         if (conn2.Connected)
//                         {
//                             conn2.SendTo(packet, endpoint);
//                         }
//                         conn2.Close();
//                     }
//                     catch (SocketException ex)
//                     {
//                         MessageBox.Show(ex.Message);
//                     }
//                     string InsertString =
//                         "INSERT INTO "
//                         + dbTableBox.Text
//                         + "VALUES ('"
//                         + Tag.TagID + "', "
//                         + locationBox1.SelectedValue.ToString() + "', "
//                         + ownerBox1.SelectedValue.ToString() + "')";
// 
// 
//                     command.CommandText = InsertString;
//                     OracleCommand myCommand = new OracleCommand(InsertString);
// 
//                     conn.Open();
//                     myCommand.ExecuteNonQuery();
//                     conn.Close();
                 }
            }
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
        /// Event fired when the connect button is pressed
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void connectBtn_Click(object sender, EventArgs e)
        {
            /*conn.ConnectionString = "Data Source=(DESCRIPTION="
                + "(ADDRESS_LIST=(ADDRESS=(PROTOCOL=TCP)(HOST="
                + hostnameBox.Text + ")(PORT="
                + portBox.Text + ")))"
                + "(CONNECT_DATA=(SERVER=DEDICATED)(SID=LN1)));"
                + "User Id=" + userIDBox.Text + ";Password=" + passwordBox.Text + ";";*/
            /* "Data Source=Oracle9i"
             + ";Integrated Security=false"
             + ";Server="
             + hostnameBox.Text
             + ";User ID="
             + userIDBox.Text
             + ";Password="
             + passwordBox.Text; */
            try
            {
                //conn.Open();
                //conn.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message.ToString());
            }
            finally
            {
                //conn.Dispose();
            }

        }

        /// <summary>
        /// Event fired when the clear button is pressed, gives the user the option to not
        /// clear the inventory after all with a messagebox
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void clearBtn_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Really clear inventory?", "Inventory will be removed", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation, MessageBoxDefaultButton.Button1) == DialogResult.Yes)
            {
                inventoryTags.Clear();
                inventoryListBox.Items.Clear();
                Reader.ClearInventory();
            }
        }

        /// <summary>
        /// Event fired when the user clicks on a new item
        /// Pulls up a built-in web browser that shows the web page
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void inventoryListBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            //RFIDTag a a.ToByteArray
            
            string selectedTag = inventoryTags[inventoryListBox.SelectedIndex].ToString();
            string lastTwo = "0x" + selectedTag.Substring(selectedTag.Length - 2);
            int lastTwoI = Convert.ToInt32(lastTwo,16);            
            string location = waypointBox.SelectedItem.ToString() == "A" ? "a_pda" : "b_pda";
            
            webBr.setUrlAndShow(new Uri("http://tbk.ece.umd.edu/" + lastTwoI.ToString() + location +".html"));
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
            catch (System.InvalidOperationException ex)
            {
                MessageBox.Show(ex.Message.ToString());
            }
            catch (System.ArgumentOutOfRangeException ex)
            {
                MessageBox.Show(ex.Message.ToString());
            }
            catch (System.ArgumentException ex)
            {
                MessageBox.Show(ex.Message.ToString());
            }
            catch (System.UnauthorizedAccessException ex)
            {
                MessageBox.Show(ex.Message.ToString());
            }
            catch (System.IO.IOException ex)
            {
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

                string utcTimeL = code.Substring(7, 71);
                utcTime = utcTimeL.Substring(0, 2) + ":"
                    + utcTimeL.Substring(2, 2) + ":"
                    + utcTimeL.Substring(4, 6);
                string lat = utcTimeL.Substring(11);
                latitude = lat.Substring(0, 2) + "° "
                    + lat.Substring(2, 2) + "\'" + lat.Substring(5, 2) + "."
                    + lat.Substring(7, 2);
                string NorS = lat.Substring(10);
                latitude += NorS.Substring(0, 1);
                string longitudeL = NorS.Substring(2);
                if (longitudeL[0] == '0')
                    longitude = longitudeL.Substring(1, 2);
                else
                    longitude = longitudeL.Substring(0, 3);
                longitude += "° "
                    + longitudeL.Substring(3, 2) + "\'"
                    + longitudeL.Substring(6, 2) + "."
                    + longitudeL.Substring(8, 2);

                string EorW = longitudeL.Substring(11);
                longitude += EorW.Substring(0, 1);
                string satsUsed = "";
                if (EorW[1] == ',')
                    satsUsed = EorW.Substring(2, 1);
                else
                    satsUsed = EorW.Substring(2, 2);
                string comma = ",";
                if (latitude.LastIndexOf(comma) < 0)
                {
                    numberOfSatellites = satsUsed;
                    satBox.Text = numberOfSatellites;                    
                    latBox.Text = latitude;
                    longBox.Text = longitude;
                }
                else
                {
                    satBox.Text = "0";
                    latBox.Text = "00° 00' 00.00";
                    longBox.Text = "00° 00' 00.00";
                }
                utcBox.Text = utcTime;
            }
            catch (ArgumentOutOfRangeException ex)
            {
            }
        }

        #endregion

        private void mapButton_Click(object sender, EventArgs e)
        {
            //long freq = 0;
            //QueryPerformanceCounter(out freq);
            string latitudeL = latitude;
            string longitudeL = longitude;
            float lat = float.Parse(latitudeL.Substring(0, 2))
                + float.Parse(latitudeL.Substring(4, 2)) / 60
                + float.Parse(latitudeL.Substring(7, 4)) / 3600;
            float lon = float.Parse(longitudeL.Substring(0, 2))
                + float.Parse(longitudeL.Substring(4, 2)) / 60
                + float.Parse(longitudeL.Substring(7, 4)) / 3600;
            
            webBr.setUrlAndShow(new Uri("http://terraserver.microsoft.com/image.aspx?PgSrh:NavLon=-" + lon.ToString() + "&PgSrh:NavLat=" + lat.ToString()));
        }

    }
}
