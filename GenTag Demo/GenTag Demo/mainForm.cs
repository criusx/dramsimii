using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using C1Lib;
using Protocol;

namespace GenTag_Demo
{
    public partial class mainForm : Form
    {
        public mainForm()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;

            //tempLabel.Text = outputBox.Text = "";

            if (C1Lib.C1.NET_C1_open_comm() != 1)
                return;
            if (C1Lib.C1.NET_C1_enable() != 1)
                return;

            // wait while a tag is read
            while (C1Lib.ISO_15693.NET_get_15693(0x00) == 0) { }
            
            //while (C1Lib.ISO_15693.NET_read_multi_15693(0x00, C1Lib.ISO_15693.tag.blocks) != 1) { }
                
            //string currentTag = C1Lib.util.to_str(C1Lib.ISO_15693.tag.read_buff, 256);

            string currentTag = "";

            for (int i = 0; i < C1Lib.ISO_15693.tag.id_length; i++)
                currentTag += C1Lib.ISO_15693.tag.tag_id[i];
            
            ClientConnection c = new ClientConnection(hostName.Text, Packet.port);

            Packet rfidReq = new Packet(PacketTypes.DescriptionRequest,currentTag);

            c.SendPacket(rfidReq);

            Packet rfidDesc = new Packet(PacketTypes.DescriptionResponse);

            c.GetPacket(rfidDesc);

            c.Close();

            treeView1.BeginUpdate();
            treeView1.Nodes.Add(currentTag);
            treeView1.Nodes[0].Nodes.Add(rfidDesc.ToString());
            treeView1.EndUpdate();
            

            //tempLabel.Text += "Joe";
            //System.Text.ASCIIEncoding encode = new System.Text.ASCIIEncoding();
            //byte[] bytes = encode.GetBytes(tempLabel.Text);

            //int block_no = (tempLabel.Text.Length + 1) / C1Lib.ISO_15693.tag.bytes_per_block;
            //while (C1Lib.ISO_15693.NET_write_multi_15693(0x00,block_no,bytes) != 1) {}

            C1Lib.C1.NET_C1_disable();
            C1Lib.C1.NET_C1_close_comm();

            Cursor.Current = Cursors.Default;

            return;
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
    }
}