using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using C1Lib;
//using Protocol;
using System.Net;
using System.Net.Sockets;
using Microsoft.Win32;
using System.Resources;
using System.Runtime.InteropServices;
using System.Globalization;
using System.Threading;
using TestPocketGraphBar;

[assembly:CLSCompliant(true)]
namespace GenTag_Demo
{
    public partial class mainForm : Form
    {
        static mainForm mF;

        Graph graph;
        //private bgTreeView treeView2;

        //[DllImport("user32.dll")]
        //public extern static int SendMessage(IntPtr hwnd, uint msg, uint wParam, uint lParam);

        //[DllImport("coredll.dll")]
        //public static extern int SendMessage(IntPtr hWnd, uint Message, uint wParam, uint lParam);

        //[DllImport("user32.dll")]
        //public extern static int SendMessage(IntPtr hwnd, uint msg, uint wParam, LVBKIMAGE lParam);

        //private const int NOERROR = 0x0;
        //private const int S_OK = 0x0;
        //private const int S_FALSE = 0x1;
        //private const int LVM_FIRST = 0x1000;
        //private const int LVM_SETBKIMAGE = LVM_FIRST + 68;
        //private const int LVM_SETTEXTBKCOLOR = LVM_FIRST + 38;
        //private const int LVBKIF_SOURCE_URL = 0x02;
        //private const int LVBKIF_STYLE_TILE = 0x10;
        //private const uint CLR_NONE = 0xFFFFFFFF;

        //SendMessage(listBox1.Handle, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_GRADIENT, LVS_EX_GRADIENT);
  

        //private const int LVS_EX_GRADIENT   = 0x20000000;
        //private const int LVM_FIRST     = 0x1000;
        //private const int LVM_SETEXTENDEDLISTVIEWSTYLE = (LVM_FIRST + 54);

        //private const int LVS_EX_GRADIENT = 0x20000000;
        //private const int LVM_FIRST = 0x1000;
        //private const int LVM_SETEXTENDEDLISTVIEWSTYLE = (LVM_FIRST + 54);

        public mainForm()
        {
            InitializeComponent();            

            mF = this;
            
            graph = new Graph();
            graph.Visible = false;
            graph.Size = new Size(240, 230);
            graph.Location = new Point(0, 0);            
            tabPage2.Controls.Add(graph);

            // Load Localized values
            button1.Text = GenTag_Demo.Properties.Resources.button1String;
            button2.Text = GenTag_Demo.Properties.Resources.button2String;
            button3.Text = GenTag_Demo.Properties.Resources.button3String;
            button4.Text = GenTag_Demo.Properties.Resources.button4String;
            button5.Text = GenTag_Demo.Properties.Resources.button5String;
            this.Text = GenTag_Demo.Properties.Resources.titleString;

            tabControl1.TabPages[0].Text = GenTag_Demo.Properties.Resources.tab2String;
            tabControl1.TabPages[1].Text = GenTag_Demo.Properties.Resources.tab1String;
            tabControl1.TabPages[2].Text = GenTag_Demo.Properties.Resources.tab3String;
            tabControl1.TabPages[3].Text = GenTag_Demo.Properties.Resources.tab4String;

            label5.Text = label1.Text = GenTag_Demo.Properties.Resources.hiLimitString;
            label6.Text = label2.Text = GenTag_Demo.Properties.Resources.loLimitString;
            label7.Text = label3.Text = GenTag_Demo.Properties.Resources.intervalString;
            label4.Text = GenTag_Demo.Properties.Resources.logString;

            textBox9.Text = @"";


            // Init the Registry
            RegistryKey regKey = Registry.LocalMachine;

            regKey = regKey.OpenSubKey(@"SOFTWARE", true);

            if (Array.IndexOf(regKey.GetSubKeyNames(), @"GenTag", 0) == -1)
                regKey.CreateSubKey(@"GenTag");
            regKey = regKey.OpenSubKey(@"GenTag", true);

            string[] settingKeys = regKey.GetSubKeyNames();

            //user ID
            if (Array.IndexOf(settingKeys, @"hostname", 0) == -1)
            {
                regKey.CreateSubKey(@"hostname");
                regKey.SetValue(@"hostname", @"129.2.99.117");
            }
           //hostName.Text = regKey.GetValue(@"hostname").ToString();
        }

        public delegate void arrayCB(
            Int32 errorCode,
            Int32 len,
            Single upperTempLimit,
            Single lowerTempLimit, 
            short recordPeriod,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] int[] dateTime,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] Byte[] logMode,            
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] Single[] temperatures);

        [DllImport("VarioSens Lib.dll")]
        protected static extern void getVarioSensLog(arrayCB cb);

        private static DateTime origin = System.TimeZone.CurrentTimeZone.ToLocalTime(new DateTime(1970, 1, 1, 0, 0, 0));

        private static void callbackFunct(
            Int32 errorCode,
            Int32 len, 
            Single upperTempLimit, 
            Single lowerTempLimit, 
            short recordPeriod,
            int[] dateTime,
            Byte[] logMode,            
            Single[] temperatures)
        {
            mF.textBox9.Text = @"";

            switch (errorCode)
            {
                case 0:
                    mF.textBox1.Text = upperTempLimit.ToString();
                    mF.textBox2.Text = lowerTempLimit.ToString();
                    mF.textBox3.Text = recordPeriod.ToString();
                    mF.listBox1.Items.Clear();

                    for (int i = 0; i < dateTime.Length; i++)
                    {
                        if (logMode[i] == 1)
                        {
                            mF.listBox1.Items.Add(temperatures[i].ToString("F"));
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
                            mF.listBox1.Items.Add(temperatures[i].ToString("F",CultureInfo.CurrentCulture) + " C" + convertedValue.ToString());
                            
                        }
                        
                    }
                    if (logMode[0] == 1)
                    {
                        try
                        {
                            mF.graph.Visible = true;
                            mF.graph.BringToFront();
                            //mF.button2.Visible =
                            //    mF.label1.Visible =
                            //    mF.label2.Visible =
                            //    mF.label3.Visible =
                            //    mF.label4.Visible =
                            //    mF.textBox1.Visible =
                            //    mF.textBox2.Visible =
                            //    mF.textBox3.Visible =
                            //    mF.textBox9.Visible =
                            //    mF.listBox1.Visible =
                            //    mF.checkBox2.Visible = false;

                            //mF.graph.LeftMargin = 20;
                            //mF.graph.AxisColor = Color.Black;
                            //mF.graph.MaxHeight = 200;
                            //mF.graph.Thick = 6;
                            //mF.graph.DisplayTimes = 10;
                            //mF.graph.
                            //mF.graph.DrawGraphs(new PaintEventArgs());
                        }
                        catch (Exception ee)
                        {
                            throw ee;
                        }
                    }
                        break;
                case -1:
                    mF.textBox9.Text = GenTag_Demo.Properties.Resources.error1;
                    break;
                case -2:
                    mF.textBox9.Text = GenTag_Demo.Properties.Resources.error2;
                    break;
                case -3:
                    mF.textBox9.Text = GenTag_Demo.Properties.Resources.error3;
                    break;
                case -4:
                    mF.textBox9.Text = GenTag_Demo.Properties.Resources.error4;
                    break;
                case -5:
                    mF.textBox9.Text = GenTag_Demo.Properties.Resources.error5;
                    break;
                case -6:
                    mF.textBox9.Text = GenTag_Demo.Properties.Resources.error6;
                    break;
                case -7:
                    mF.textBox9.Text = GenTag_Demo.Properties.Resources.error7;
                    break;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;
            checkBox1.Checked = false;

            //tempLabel.Text = outputBox.Text = "";

            if (C1Lib.C1.NET_C1_open_comm() != 1)
            {
                MessageBox.Show("Could not open reader");
                Cursor.Current = Cursors.Default;
                return;
            }
            if (C1Lib.C1.NET_C1_enable() != 1)
            {
                MessageBox.Show("Could not open reader");
                Cursor.Current = Cursors.Default;
                return;
            }

            // wait while a tag is read
            while (C1Lib.ISO_15693.NET_get_15693(0x00) == 0) { }
            
            //while (C1Lib.ISO_15693.NET_read_multi_15693(0x00, C1Lib.ISO_15693.tag.blocks) != 1) { }
                
            //string currentTag = C1Lib.util.to_str(C1Lib.ISO_15693.tag.read_buff, 256);
            
            StringBuilder newTag = new StringBuilder(C1Lib.ISO_15693.tag.id_length);

            for (int i = 0; i < C1Lib.ISO_15693.tag.id_length; i++)
                newTag.Append(C1Lib.ISO_15693.tag.tag_id[i]);

            string currentTag = newTag.ToString();

            C1Lib.C1.NET_C1_disable();
            C1Lib.C1.NET_C1_close_comm();

            string rfidDescr = "No description found";

            checkBox1.Checked = true;

            try
            {
                org.dyndns.crius.GetDatesWS ws = new org.dyndns.crius.GetDatesWS();
                
                try
                {
                    rfidDescr = ws.getDescription(currentTag);
                }
                catch (WebException ex)
                {
                    MessageBox.Show("Problem connecting to web service: " + ex.Message);
                }
                

                //ClientConnection c = new ClientConnection(hostName.Text, Packet.port);

                //Packet rfidReq = new Packet(PacketTypes.DescriptionRequest, currentTag);

                //c.SendPacket(rfidReq);

                //Packet rfidDesc = new Packet(PacketTypes.DescriptionResponse);

                //c.GetPacket(rfidDesc);

                //rfidDescr = rfidDesc.ToString();

                //Packet closePacket = new Packet(PacketTypes.CloseConnectionRequest);
                //c.SendPacket(closePacket);
                //c.Close();
                //c.Close();                
            }
            catch (SocketException ex)
            {
                MessageBox.Show(ex.Message);
            }

            treeView1.BeginUpdate();

            bool exists = false;

            foreach (TreeNode tn in treeView1.Nodes)
            {
                Console.Out.WriteLine(tn.Text);
                if (tn.Text == currentTag)
                {
                    tn.Nodes.Clear();
                    tn.Nodes.Add(rfidDescr);
                    exists = true;
                }
            }
            if (!exists)
            {
                treeView1.Nodes.Add(currentTag).Nodes.Add(rfidDescr);
            }
            treeView1.EndUpdate();

            Cursor.Current = Cursors.Default;

            MessageBox.Show("Tag ID: " + currentTag + "\nDesc: " + rfidDescr, "Found new tag", MessageBoxButtons.OK, MessageBoxIcon.Exclamation, MessageBoxDefaultButton.Button1);

            return;

            
            

            //tempLabel.Text += "Joe";
            //System.Text.ASCIIEncoding encode = new System.Text.ASCIIEncoding();
            //byte[] bytes = encode.GetBytes(tempLabel.Text);

            //int block_no = (tempLabel.Text.Length + 1) / C1Lib.ISO_15693.tag.bytes_per_block;
            //while (C1Lib.ISO_15693.NET_write_multi_15693(0x00,block_no,bytes) != 1) {}

            

            
            //for (byte i = 0x00; i < 0xFF; i++)
            //    if (C1Lib.ISO_15693.NET_read_single_15693(i) == 1)
            //        outputBox.Text += C1Lib.util.to_str(C1Lib.ISO_15693.tag.read_buff, 256);
            //byte[] Inventory = { 0x01, 0x01, 0x00 };
            //byte[] Inventory = { 0x0F, 0XFF, 0XBF, 0X04, 0X00, 0X60, 0X02, 0X1E, 0X84, 0X6A, 0X27, 0X01, 0X00, 0XC6, 0X0C };
            //byte[] Inventory = { 0x27, 0x01, 0x00 };
            //byte[] InventoryOut = new byte[12];
            //int err0 =  C1.NET_C1_config_15693();
            //ushort crc1 = CRC.calc(Inventory, Inventory.Length - 2);
            //Inventory[Inventory.Length - 2] = (byte)(crc1 >> 0x08);
            //Inventory[Inventory.Length - 1] = (byte)(crc1 & 0x00FF);
            //uint err = C1Lib.C1.NET_C1_transmit(Inventory, Inventory.Length, InventoryOut, InventoryOut.Length, true, false);
            //if (C1Lib.ISO_15693.tag.tag_type == C1Lib.ISO_15693.TEMPSENS)
            //{
            //    if (C1Lib.ISO_15693.TempSens.NET_TS_get_log_info() == 0)
            //        return;
            //    if (C1Lib.ISO_15693.TempSens.NET_TS_get_log_data() == 0)
            //        return;
            //    if (C1Lib.ISO_15693.TempSens.log.next_measurment_block == 0)
            //        return;
            //    else
            //    {
            //        tempBar.Value = (int)C1Lib.ISO_15693.TempSens.log.temperature[C1Lib.ISO_15693.TempSens.log.next_measurment_block];
            //        tempLabel.Text = tempBar.Value + " F";
            //    }
                //if (!C1Lib.ISO_15693.TempSens.NET_TS_clear())
                //    return;
                //if (!C1Lib.ISO_15693.TempSens.NET_TS_get_log_info())
                //    return;
                //C1Lib.ISO_15693.TempSens.log.period = 450;
                //C1Lib.ISO_15693.TempSens.log.low_temp_limit = 10;
                //C1Lib.ISO_15693.TempSens.log.high_temp_limit = 120;
            //}

            //C1Lib.C1.NET_C1_disable();
            //C1Lib.C1.NET_C1_close_comm();

            //Cursor.Current = Cursors.Default;

        }         

        private void mainForm_KeyDown(object sender, KeyEventArgs e)
        {            
            if (e.KeyCode == System.Windows.Forms.Keys.Back)
            {
                treeView1.Nodes.Clear();
            }
            else if (e.KeyCode == System.Windows.Forms.Keys.Q)
            {
                Application.Exit();                
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;

            checkBox2.Checked = false;

            arrayCB mycb = new arrayCB(callbackFunct);

            getVarioSensLog(mycb);

            checkBox2.Checked = true;

            // go do web service stuff

            Cursor.Current = Cursors.Default;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;

            textBox4.Enabled = false;

            org.dyndns.crius.GetDatesWS ws = new org.dyndns.crius.GetDatesWS();

            string rfidDescr = @"No description found";

            try
            {
                rfidDescr = ws.getDescription(textBox4.Text);
            }
            catch (WebException ex)
            {
                MessageBox.Show(@"Problem connecting to web service: " + ex.Message);
            }

            treeView2.BeginUpdate();

            bool exists = false;

            foreach (TreeNode tn in treeView2.Nodes)
            {
                if (tn.Text == textBox4.Text)
                {
                    tn.Nodes.Clear();
                    tn.Nodes.Add(rfidDescr);
                    exists = true;
                }
            }
            if (!exists)
            {
                treeView2.Nodes.Add(textBox4.Text).Nodes.Add(rfidDescr);
            }
            treeView1.EndUpdate();

            textBox4.Enabled = true;

            Cursor.Current = Cursors.Default;
        }       

        [DllImport("VarioSens Lib.dll")]
        public static extern int setVarioSensSettings(float lowTemp, float hiTemp, int interval, int mode, int batteryCheckInterval);
        
        // set
        private void button4_Click(object sender, EventArgs e)
        {
            try
            {
                int mode;

                if (comboBox1.SelectedIndex == 0)
                    mode = 1;
                else
                    mode = 2;
                int eC = setVarioSensSettings(float.Parse(textBox7.Text), float.Parse(textBox6.Text), int.Parse(textBox5.Text), mode, int.Parse(textBox8.Text));
                if (eC != 0)
                    MessageBox.Show("Error");
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        public delegate void arrayCB2(
            Single upperTempLimit,
            Single lowerTempLimit,
            short recordPeriod,
            short errorCode,
            short logMode,
            short batteryCheckInterval);

        [DllImport("VarioSens Lib.dll")]
        public static extern void getVarioSensSettings(arrayCB2 cb);

        private void getCallback(Single upper, Single lower, short period, short errorCode, short logMode, short batteryCheckInterval)
        {
            if (errorCode == 0)
            {
                mF.textBox7.Text = lower.ToString();
                mF.textBox6.Text = upper.ToString();
                mF.textBox5.Text = period.ToString();
                if (logMode == 1)
                {
                    mF.comboBox1.SelectedIndex = 0;
                }
                else if (logMode == 2)
                {
                    mF.comboBox1.SelectedIndex = 1;
                }
                mF.textBox8.Text = batteryCheckInterval.ToString();
            }
        }

        // get
        private void button5_Click(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;

            arrayCB2 mycb = new arrayCB2(getCallback);

            getVarioSensSettings(mycb);

            Cursor.Current = Cursors.Default;
        }
    }    
}