using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using C1Lib;
using Protocol;
using System.Net.Sockets;
using Microsoft.Win32;
using System.Resources;
using System.Runtime.InteropServices;

namespace GenTag_Demo
{
    public partial class mainForm : Form
    {
        public mainForm()
        {
            InitializeComponent();

            // Load Localized values
            button1.Text = GenTag_Demo.Properties.Resources.button1String;
            button2.Text = GenTag_Demo.Properties.Resources.button2String;
            this.Text = GenTag_Demo.Properties.Resources.titleString;


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


        [DllImport("VarioSens Lib.dll",CallingConvention = CallingConvention.Winapi)]
        static extern int multiply(int a, int b);

        public delegate void arrayCB(
            int errorCode,
            int len,
            float upperTempLimit,
            float lowerTempLimit, 
            int recordPeriod,             
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] byte[] logMode,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] uint[] dateTime,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 0)] float[] temperatures);

        [DllImport("VarioSens Lib.dll")]
        public static extern void getVarioSensLog(arrayCB cb);


        private static void callbackFunct(
            int errorCode,
            int len, 
            float upperTempLimit, 
            float lowerTempLimit, 
            int recordPeriod, 
            byte[] logMode,
            uint[] dateTime,
            float[] temperatures)
        {
            switch (errorCode)
            {
                case 0:
                    uint time_t = dateTime[0];
                    DateTime origin = System.TimeZone.CurrentTimeZone.ToLocalTime(new DateTime(1970, 1, 1, 0, 0, 0));
                    DateTime convertedValue = origin + new TimeSpan(time_t * TimeSpan.TicksPerSecond);
                    if (System.TimeZone.CurrentTimeZone.IsDaylightSavingTime(convertedValue) == true)
                    {
                        System.Globalization.DaylightTime daylightTime = System.TimeZone.CurrentTimeZone.GetDaylightChanges(convertedValue.Year);
                        convertedValue = convertedValue + daylightTime.Delta;
                    }
                    MessageBox.Show("Success" + convertedValue.ToString());
                    break;
                case -1:
                    MessageBox.Show("Error, no communication with Sirit card");
                    break;
                case -2:
                    MessageBox.Show("Error, cannot enable Sirit PNP RFID");
                    break;
                case -3:
                    MessageBox.Show("Error, cannot read tag");
                    break;
                case -4:
                    MessageBox.Show("Error, cannot retrieve log state from VarioSens card");
                    break;
                case -5:
                    MessageBox.Show("Error, cannot get violation data");
                    break;
                case -6:
                    MessageBox.Show("No violations recorded");
                    break;
                case -7:
                    MessageBox.Show("Error, not a VarioSens tag");
                    break;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            arrayCB mycb = new arrayCB(callbackFunct);

            getVarioSensLog(mycb);

            return;
            uint time_t = 1162857786;
            DateTime origin = System.TimeZone.CurrentTimeZone.ToLocalTime(new DateTime(1970, 1, 1, 0, 0, 0));
            DateTime convertedValue = origin + new TimeSpan(time_t * TimeSpan.TicksPerSecond);
            if (System.TimeZone.CurrentTimeZone.IsDaylightSavingTime(convertedValue) == true)
            {
                System.Globalization.DaylightTime daylightTime = System.TimeZone.CurrentTimeZone.GetDaylightChanges(convertedValue.Year);
                convertedValue = convertedValue + daylightTime.Delta;
            }

            int a = 4;
            int b = 5;
            int c = multiply(a, b);
            c++;

            return;
            
            
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
                //GetDatesWS.GetDatesWS ws = new GetDatesWS.GetDatesWS();

                rfidDescr = ws.getDescription(currentTag);                

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
            if ((e.KeyCode == System.Windows.Forms.Keys.Up))
            {
                // Up
            }
            if ((e.KeyCode == System.Windows.Forms.Keys.Down))
            {
                // Down
            }
            if ((e.KeyCode == System.Windows.Forms.Keys.Left))
            {
                // Left
            }
            if ((e.KeyCode == System.Windows.Forms.Keys.Right))
            {
                // Right
                treeView1.Nodes.Clear();
            }
            if ((e.KeyCode == System.Windows.Forms.Keys.Enter))
            {
                // Enter
                treeView1.Nodes.Clear();
            }
        }
    }
    public unsafe class varioSensReader
    {
        public struct tag_15693
        {
            bool program_option;
            bool read_security;
            bool selected;          
            bool addressed;
            byte sys_info_support;
            byte command_support;
            byte DSFID;
            byte AFI;
            byte bytes_per_block;
            byte blocks;
            byte ic_ref;
            fixed byte tag_id[10];
            int id_length;
            fixed byte read_buff[256];
            fixed byte security_buff[256];
            byte tag_type;
        };

        struct tempViolationData
        {
            byte logMode;

            // Log Mode 0
            //	temperature[0] - internal temperature
            //  temperature[1] - external temperature
            //  logCounter - Log Counter

            // Log Mode 1
            //	temperature[0] - Measurement 1
            //  temperature[1] - Measurement 2
            //  temperature[2] - Measurement 3

            // Log Mode 2
            //	temperature[0] - Measurement outside limits
            //  logCounter - Log Counter

            // Log Mode 3
            //	temperature[0] - First time measurement is outside or inside limits or extremum value
            //  logCounter - Log Counter

            fixed float temperature[3];

            // the time that the violation occurred
            uint vltionTime;
        };

        const uint VARIOSENS = 2;

        public struct variosens_log
        {
            //log settings
            // memory block 0x05
            int calTemp1;
            int calTemp2;
            int calTicks1;

            // memory block 0x06
            uint calVoltageLo;
            uint voltageTh;
            uint calVoltageHi;
            int calTicks2;

            // memory block 0x08
            uint UTCStartTime;

            // memory block 0x09
            ushort stndByTime;
            ushort logIntval;

            // memory block 0x0A
            byte logMode;
            byte batCheck;
            float upperTemp;
            float lowerTemp;
            byte storageMode;
            byte sensorFlag;

            // memory block 0x0B
            uint nextBlockAdd;
            uint numMemRpl;
            bool cardStopped;
            bool cardExternal;
            uint numMeasure;

            uint numDownloadMeas;

            byte PWLevel;
            uint password;

            //fixed tempViolationData vltionData[VIOLATIONDATALNTH];
        };

        #region dllImports
        //[DllImport("C1lib_PPC.dll")]
        extern int get_15693(tag_15693 *tag, byte AFI);
        extern variosens_log new_variosens_log();
        extern int VS_init(tag_15693 *tag);
        extern int VS_setLogMode(tag_15693 *tag, variosens_log *pVLog);
        extern int VS_setLogTimer(tag_15693 *tag, variosens_log *pVLog);
        extern int VS_startLog(tag_15693 *tag, uint UTCTime);
        extern int VS_getLogState(tag_15693 *tag, variosens_log *pVLog);
        extern int VS_set_passive(tag_15693 *tag);
        extern int VS_verifyPW(tag_15693 *tag, variosens_log *pVLog);
        extern int VS_setPW(tag_15693 *tag, variosens_log *pVLog);
        extern int VS_getLogData(tag_15693 *pTag, variosens_log *pVLog);
        extern int C1_open_comm();
        extern int C1_close_comm();
        extern int C1_set_comm();
        extern int C1_disable();
        extern int C1_enable();
        
        #endregion

        unsafe private void button2_Click(object sender, EventArgs e)
        {
            return;
            if (C1Lib.C1.NET_C1_open_comm() != 1)
            {
                C1Lib.C1.NET_C1_close_comm();
                return;
            }

            if (C1Lib.C1.NET_C1_enable() != 1)
            {
                C1Lib.C1.NET_C1_close_comm();
                return;
            }
            if (C1Lib.ISO_15693.NET_get_15693(0x00) == 0) { }
            if (C1Lib.ISO_15693.tag.tag_type == 2)
            {
                    for (uint i = 0; i < 0x0F; i++)
                    {
                        if (C1Lib.ISO_15693.TempSens.NET_TS_get_log_info() == 1)
                        {
                            if (C1Lib.ISO_15693.TempSens.NET_TS_get_log_data() == 1)
                            {
                                //string violations = "";
                                //for (uint i=0; i <C1Lib.ISO_15693.TempSens.log.)
                                //MessageBox.Show("Violations: " + )
                                MessageBox.Show("Current Temp: " + C1Lib.ISO_15693.TempSens.log.temperature[0]);
                                break;
                            }
                        }
                        
                    }
                }
                C1Lib.C1.NET_C1_disable();
                C1Lib.C1.NET_C1_close_comm();
            //}
        }
    };
}