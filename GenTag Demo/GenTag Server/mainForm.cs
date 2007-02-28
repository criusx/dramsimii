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

[assembly: SecurityPermission(
   SecurityAction.RequestMinimum, Execution = true)]
[assembly: PermissionSet(
   SecurityAction.RequestOptional, Unrestricted=true)]

namespace GenTag_Server
{
    public partial class mainForm : Form
    {
        string imagename;

        string drugImagename;
        
        public mainForm()
        {
            InitializeComponent();
        }

        private void closeButton_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void browseButton_Click(object sender, EventArgs e)
        {
            try
            {
                FileDialog fldlg = new OpenFileDialog();

                fldlg.InitialDirectory = @"C:\";

                fldlg.Filter = "Image File (*.jpg;*.bmp;*.gif)|*.jpg;*.bmp;*.gif";

                if (fldlg.ShowDialog() == DialogResult.OK)
                {

                    imagename = fldlg.FileName;

                    Bitmap newimg = new Bitmap(imagename);

                    pictImg.SizeMode = PictureBoxSizeMode.StretchImage;

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

        private void saveButton_Click(object sender, EventArgs e)
        {
            //use filestream object to read the image.

            //read to the full length of image to a byte array.

            //add this byte as an oracle parameter and insert it into database.

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

                    OracleCommand cmnd;

                    string query;

                    query = "insert into patientData(id,interaction,description,picture) values('" + idBox.Text + "','" + interactionBox.Text + "','" + descriptionBox.Text + "',:BlobParameter )";

                    OracleParameter blobParameter = new OracleParameter();

                    blobParameter.OracleDbType = OracleDbType.Blob;

                    blobParameter.ParameterName = "BlobParameter";

                    blobParameter.Value = blob;

                    cmnd = new OracleCommand(query, conn);

                    cmnd.Parameters.Add(blobParameter);

                    cmnd.ExecuteNonQuery();

                    MessageBox.Show("added to blob field");

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
            interactionBox.Clear();
            descriptionBox.Clear();            
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
             interactionBox.Text = dr.GetString(1);
             descriptionBox.Text = dr.GetString(2);
            
             OracleBlob inBlob = dr.GetOracleBlob(3);
           // MemoryStream s = new MemoryStream(inBlob.);
            pictImg.Image = Image.FromStream(inBlob);
            pictImg.Refresh();
        }

        private void interactionSave_Click(object sender, EventArgs e)
        {
            if (interactionID.Text.Length < 16)
            {
                MessageBox.Show(interactionID.Text + " isn't long enough");
                return;
            }

            string MyConString = @"User Id=" + usernameBox.Text + @";" +
                        @"Password=" + passwordBox.Text + @";" +
                        @"Data Source=" + dataSourceBox.Text;

            OracleConnection conn = new OracleConnection(MyConString);

            conn.Open();

            foreach (string s in interactionListBox.Items)
            {
                string query = "INSERT INTO INTERACTIONS VALUES('" + interactionID.Text + "','" + s + "')";

                OracleCommand cmnd = new OracleCommand(query, conn);
                
                try
                {
                    cmnd.ExecuteNonQuery(); 
                }
                catch (OracleException ex)
                {

                }               

                cmnd.Dispose();
            }

            MessageBox.Show("Added");

            conn.Close();

            conn.Dispose();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (drugIDAddBox.Text.Length >= 16)
                interactionListBox.Items.Add(drugIDAddBox.Text);
        }

        private void interactionLoad_Click(object sender, EventArgs e)
        {
            if (interactionID.Text.Length < 16)
            {
                MessageBox.Show(interactionID.Text + " isn't long enough");
                return;
            }

            string MyConString = @"User Id=" + usernameBox.Text + @";" +
                        @"Password=" + passwordBox.Text + @";" +
                        @"Data Source=" + dataSourceBox.Text;

            OracleConnection conn = new OracleConnection(MyConString);

            interactionListBox.Items.Clear();

            conn.Open();

            string query = "SELECT DRUGID FROM INTERACTIONS WHERE PATIENTID='" + interactionID.Text + "'";

            OracleCommand cmnd = new OracleCommand(query, conn);

            OracleDataReader dr = cmnd.ExecuteReader();    

            while (dr.Read())
                interactionListBox.Items.Add(dr.GetString(0));

            conn.Close();

            conn.Dispose();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            interactionListBox.Items.Clear();
        }

        private void drugBrowseButton_Click(object sender, EventArgs e)
        {
            try
            {

                FileDialog fldlg = new OpenFileDialog();

                fldlg.InitialDirectory = @"C:\";

                fldlg.Filter = "Image File (*.jpg;*.bmp;*.gif)|*.jpg;*.bmp;*.gif";

                if (fldlg.ShowDialog() == DialogResult.OK)
                {

                    drugImagename = fldlg.FileName;

                    Bitmap newimg = new Bitmap(drugImagename);

                    drugPictureBox.SizeMode = PictureBoxSizeMode.CenterImage;

                    drugPictureBox.Image = (Image)newimg;

                }

                fldlg = null;

            }

            catch (System.ArgumentException ae)
            {

                drugImagename = " ";

                MessageBox.Show(ae.Message.ToString());

            }

            catch (Exception ex)
            {

                MessageBox.Show(ex.Message.ToString());

            }
        }

        private void drugClearButton_Click(object sender, EventArgs e)
        {
            drugPictureBox.Image = null;
            drugPictureBox.Refresh();
            drugIDBox.Clear();
            drugDescriptionBox.Clear();
        }

        private void drugSaveButton_Click(object sender, EventArgs e)
        {
            //use filestream object to read the image.

            //read to the full length of image to a byte array.

            //add this byte as an oracle parameter and insert it into database.

            try
            {

                //proceed only when the image has a valid path

                if (imagename != "")
                {
                    FileStream fls;

                    fls = new FileStream(@drugImagename, FileMode.Open, FileAccess.Read);

                    byte[] blob = new byte[fls.Length];

                    fls.Read(blob, 0, System.Convert.ToInt32(fls.Length));

                    fls.Close();

                    string MyConString = @"User Id=" + usernameBox.Text + @";" +
                        @"Password=" + passwordBox.Text + @";" +
                        @"Data Source=" + dataSourceBox.Text;

                    OracleConnection conn = new OracleConnection(MyConString);

                    conn.Open();

                    string query = "insert into drugData values ('" + drugIDBox.Text+ "'," + "'" + drugDescriptionBox.Text + "',:BlobParameter )";

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

        private void drugLoadButton_Click(object sender, EventArgs e)
        {
            string MyConString = @"User Id=" + usernameBox.Text + @";" +
                        @"Password=" + passwordBox.Text + @";" +
                        @"Data Source=" + dataSourceBox.Text;

            OracleConnection conn = new OracleConnection(MyConString);
            conn.Open();

            OracleDataAdapter adap = new OracleDataAdapter();

            //adap.SelectCommand = new OracleCommand("SELECT * from patientData WHERE (id='" + idBox.Text + "')");
            OracleCommand cmd = new OracleCommand("SELECT description,picture from DrugData WHERE (drugid='" + drugIDBox.Text + "')");
            cmd.Connection = conn;
            OracleDataReader dr = cmd.ExecuteReader();
            dr.Read();
            
            drugDescriptionBox.Text = dr.GetString(0);            

            OracleBlob inBlob = dr.GetOracleBlob(1);
            // MemoryStream s = new MemoryStream(inBlob.);
            drugPictureBox.Image = Image.FromStream(inBlob);
            drugPictureBox.Refresh();
        }        
    }
}