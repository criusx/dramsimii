using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Net.Sockets;
using System.Threading;
using Oracle.DataAccess.Client;
using System.Security.Permissions;
using System.Runtime.InteropServices;
using System.IO;
using Oracle.DataAccess.Types;

namespace WineEntryClient
{
    public partial class wineEntry : Form
    {
        public wineEntry()
        {
            InitializeComponent();
        }

        bool readerRunning = false;

        private void readButton_Click(object sender, EventArgs e)
        {
            if (readerRunning)
                readerRunning = false;
            else
            {
                readerRunning = true;
                new Thread(new ThreadStart(readTagID)).Start();                
            }            
        }

        string tagID;

        private void readTagID()
        {
            if (C1Lib.C1.NET_C1_open_comm() == 0)
            {
                //throw new IOException(@"Unable to open comm to reader");
                MessageBox.Show(@"Unable to open comm to reader");
            }
            else if (C1Lib.C1.NET_C1_enable() != 1)
            {                
                C1Lib.C1.NET_C1_disable();
                MessageBox.Show(@"Unable to enable device");
                //throw new IOException(@"Unable to enable device");
            }
            else
            {
                // wait while a tag is read
                while ((readerRunning == true) && C1Lib.ISO_15693.NET_get_15693(0x00) == 0) { Thread.Sleep(50); }

                if (readerRunning == false)
                    return;
                    //throw new IOException("Reading of tag stopped");

                //while (C1Lib.ISO_15693.NET_read_multi_15693(0x00, C1Lib.ISO_15693.tag.blocks) != 1) { }


                //string rfidDescr = C1Lib.util.to_str(C1Lib.ISO_15693.tag.read_buff, 256);
                //rfidDescr += "\n";

                StringBuilder newTag = new StringBuilder(C1Lib.ISO_15693.tag.id_length);

                for (int i = 0; i < C1Lib.ISO_15693.tag.id_length; i++)
                    newTag.Append(C1Lib.util.hex_value(C1Lib.ISO_15693.tag.tag_id[i]));

                C1Lib.C1.NET_C1_disable();
                C1Lib.C1.NET_C1_close_comm();

                tagID = newTag.ToString();
                setTextBox(idBox, tagID);
            }
            //throw new IOException(@"Unable to read tag");
        }

        private string imagename;

        private void browseButton_Click(object sender, EventArgs e)
        {
            try
            {
                FileDialog fldlg = new OpenFileDialog();

                fldlg.Filter = "Image File (*.jpg;*.bmp;*.gif;*.png)|*.jpg;*.bmp;*.gif;*.png";

                if (fldlg.ShowDialog() == DialogResult.OK)
                {

                    imagename = fldlg.FileName;

                    Bitmap newimg = new Bitmap(imagename);

                    pictImg.SizeMode = PictureBoxSizeMode.CenterImage;

                    pictImg.Image = (Image)newimg;

                }

                fldlg = null;

            }

            catch (System.ArgumentException ae)
            {

                imagename = " ";

                MessageBox.Show(ae.Message.ToString());

            }

            catch (Exception ex)
            {

                MessageBox.Show(ex.Message.ToString());

            }
        }

        #region Safe Accessors and Mutators
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
        #endregion

        private void saveButton_Click(object sender, EventArgs e)
        {            
            try
            {

                //proceed only when the image has a valid path

                if (imagename != "")
                {

                    FileStream fls;

                    fls = new FileStream(@imagename, FileMode.Open, FileAccess.Read);

                    byte[] blob = new byte[fls.Length];

                    fls.Read(blob, 0, System.Convert.ToInt32(fls.Length));

                    fls.Close();

                    string MyConString = @"User Id=" + usernameBox.Text + @";" +
                        @"Password=" + passwordBox.Text + @";" +
                        @"Data Source=" + dataSourceBox.Text;

                    OracleConnection conn = new OracleConnection(MyConString);

                    conn.Open();

                    string query = "insert into winedescriptions " +
                        "(id,vineyard,year,type,review,label,country) values('" +
                        idBox.Text + "','" + vineyardBox.Text + "','" +
                        yearUpDown.Value.ToString() + "','" + wineTypeComboBox.SelectedText +
                        reviewBox.Text + "',:BlobParameter," + countryBox.SelectedText + ")";

                    OracleParameter blobParameter = new OracleParameter();

                    blobParameter.OracleDbType = OracleDbType.Blob;

                    blobParameter.ParameterName = "BlobParameter";

                    blobParameter.Value = blob;

                    OracleCommand cmnd = new OracleCommand(query, conn);

                    cmnd.Parameters.Add(blobParameter);

                    cmnd.ExecuteNonQuery();

                    cmnd.Dispose();

                    conn.Close();

                    conn.Dispose();
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void clearButton_Click(object sender, EventArgs e)
        {
            idBox.Clear();
            reviewBox.Clear();
        }

        private void loadButton_Click(object sender, EventArgs e)
        {
            string MyConString = @"User Id=" + usernameBox.Text + @";" +
                        @"Password=" + passwordBox.Text + @";" +
                        @"Data Source=" + dataSourceBox.Text;

            OracleConnection conn = new OracleConnection(MyConString);
            conn.Open();

            OracleDataAdapter adap = new OracleDataAdapter();

            //adap.SelectCommand = new OracleCommand("SELECT * from patientData WHERE (id='" + idBox.Text + "')");
            OracleCommand cmd = new OracleCommand("SELECT * from patientData WHERE (id='" + idBox.Text + "')");
            cmd.Connection = conn;
            OracleDataReader dr = cmd.ExecuteReader();

            dr.Read();
            idBox.Text = dr.GetString(0);
            //interactionBox.Text = dr.GetString(1);
            reviewBox.Text = dr.GetString(2);

            OracleBlob inBlob = dr.GetOracleBlob(3);
            // MemoryStream s = new MemoryStream(inBlob.);
            pictImg.Image = Image.FromStream(inBlob);
            pictImg.Refresh();
        }




    }
}