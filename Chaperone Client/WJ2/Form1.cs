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
using Bluetooth;

namespace WJ2
{
    public partial class Form1 : Form
    {
        [DllImport("coredll.dll")]
        private static extern bool QueryPerformanceCounter(out long lpPerformanceCount);
		[DllImport("coredll.dll")]
		private static extern bool SipShowIM(int dwFlag);


        static void Main()
        {
            Application.Run(new Form1());
        }

        #region Form1 Properties (variables)

        //private OracleConnection conn = new OracleConnection();
        //public const string authError = "[POL-3013]";

        private const int latLongArraySize = 128;

        private MPRReader Reader;
        private byte isScan;
        private Browser webBr;
        private ArrayList inventoryTags;
        private int totalTags;
        private string gpsBuffer;
        private string utcTime;
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
        private static byte[] mac = { 0x00, 0x03, 0x7a, 0x23, 0xc6, 0xd6 };
        private RadiationSensor radSensor = new RadiationSensor(mac);
        bool firstScan = true;
		private string serverAddress;
		private string phoneNumber;
        //private string numberOfSatellites;
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
            invTimerBox.SelectedIndex = 0;
            cc = new ClientConnection();
            inventoryTags = new ArrayList();
            //cursor = new ();
            Reader.Class1InventoryEnabled = true;
            Reader.Class0InventoryEnabled = true;
            Reader.PersistTime = TimeSpan.MaxValue;

            NorS = 'N';
            EorW = 'E';
            latitude = 0;
            longitude = 0;
            
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
        /// Also fired when the inventory timer runs out
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void startInventoryButton_Click(object sender, EventArgs e)
        {
            if (!Reader.IsConnected)
                readerConnect(false);

            if (sender == scanButton)
                isScan = 1;
            else
                isScan = 0;
            
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
                    startInventoryButton.Text = "Stop";
                    startInventoryButton.BackColor = Color.Red;
                    startInventoryButton.ForeColor = Color.White;
                    invProgressBar.Value = 0;
                    inventoryTags.Clear();
                    invProgressBar.Show();
                    invTimerBox.Hide();
                    inventoryListBox.Items.Clear();
                    Reader.ClearInventory();
                }
                else /// stopping an inventory loop
                {
                    startInventoryButton.Text = "Wait";
					startInventoryButton.Enabled = false;
                    startInventoryButton.BackColor = Color.LightGreen;
                    startInventoryButton.ForeColor = Color.Black;
                    invProgressBar.Hide();
                    invTimerBox.Show();

                    try
                    {
                        cc = new ClientConnection();

                        // inventory has ended, now go submit manifest

                        // connect to the server
                        cc.Connect(hostnameBox.Text, 1555);

                        // handshake
                        cc.SendConnectPacket();
                        cc.WaitForConnectResponsePacket();

                        // send all the RFIDs to the server
                        for (int i = 0; i < inventoryTags.Count; ++i)
                        {
                            cc.SendQueryPacket(new QueryRequest(new RFID(inventoryTags[i].ToString())));

                            //QueryResponse qr = cc.WaitForQueryResponsePacket();

                            //inventoryListBox.Items.Insert(i, qr.ShortDesc);

                        }
                        cc.SendInfoPacket(new InfoPacket(latitude.ToString("N15").Substring(0,15) + NorS.ToString(), longitude.ToString("N15").Substring(0,15) + EorW.ToString(), isScan));

                        cc.SendCommitPacket();

                        //cc.SendSetPhoneNumberPacket(new SetPhoneNumberRequest("00000000"));
                        cc.Close();
                    }
                    catch (Exception ex)
                    {
                        Console.Out.WriteLine(ex.ToString());

                        inventoryListBox.Items.Clear();

                        for (int i = 0; i < inventoryTags.Count; ++i)
                            inventoryListBox.Items.Insert(i, inventoryTags[i].ToString());
                    }
                    finally
                    {
                        startInventoryButton.Text = "M/R";
                        startInventoryButton.Enabled = true;
                    }
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
            string selectedTag = inventoryTags[inventoryListBox.SelectedIndex].ToString();
            selectedTag = selectedTag.Replace(" ","");
            string lastTwo = "0x" + selectedTag.Substring(selectedTag.Length - 2);
            int lastTwoI = Convert.ToInt32(lastTwo,16);
			selectedTag = selectedTag.Substring(0, selectedTag.Length - 2)+  lastTwoI.ToString();
            
            
            webBr.setUrlAndShow(new Uri("http://" + "tbk.ece.umd.edu/pda_item.jsp?rfid=" + selectedTag));
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
                double latitudeL = double.Parse (fields[2].Substring(0,2)) 
                    + double.Parse(fields[2].Substring(2)) / 60.0;

                double longitudeL = double.Parse(fields[4].Substring(0,3)) +
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
                    + (latitude - System.Math.Floor(latitude)).ToString().Substring(2,2) + "."
                    + (latitude - System.Math.Floor(latitude)).ToString().Substring(4) + "' "
                    + NorS;

                longBox.Text = System.Math.Floor(longitude) + "° "
                    + (longitude - System.Math.Floor(longitude)).ToString().Substring(2,2) + "."
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
                + latitude.ToString() + NorS + "+" 
                + longitude.ToString() + EorW + "&t=h"));
        }
        
        
        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                if (!radTimer.Enabled)
                {
                    radSensor.connect();
                    radTimer.Interval = int.Parse(radUpDown.Value.ToString()) * 1000;
                    radTimer.Enabled = true;
                    button1.BackColor = Color.DarkMagenta;
                }
                else
                {
                    radSensor.disconnect();
                    radTimer.Enabled = false;
                    button1.BackColor = Color.Cyan;
                }
            }
            catch (Exception ex)
            {
                radTimer.Enabled = false;
                button1.BackColor = Color.Green;
            }
        }

        private void radTimer_Tick(object sender, EventArgs e)
        {
            try
            {
                radTimer.Enabled = false;
                if (radSensor.poll())
                {
                    phoneNumber = textBox1.Text;
                    serverAddress = hostnameBox.Text;
                    Thread alert = new Thread(new ThreadStart(sendAlert));
                    alert.Start();
                }
                else
                {
                    radTimer.Enabled = true;
                }

            }
            catch (Exception ex)
            {
                button1.BackColor = Color.Green;
                radSensor.disconnect();
            }
        }

		private void sendAlert()
		{
			try
			{
				// send warning packet
				ClientConnection cc = new ClientConnection();
				cc.Connect(serverAddress, 1555);
				cc.SendSetPhoneNumberPacket(new SetPhoneNumberRequest(phoneNumber));
				cc.SendRaiseAlertPacket(new RaiseAlertRequest());
				cc.Close();
				radTimer.Enabled = true;
			}
			catch (Exception ex)
			{
				try
				{
					cc.Close();
				}
				catch (Exception ex2)
				{
					radTimer.Enabled = false;
					button1.BackColor = Color.Green;
					radSensor.disconnect();
				}
			}
		}

		private void textBox1_GotFocus(object sender, EventArgs e)
		{
			SipShowIM(1);
		}

		private void textBox1_LostFocus(object sender, EventArgs e)
		{
			SipShowIM(0);
		}
    }
}
