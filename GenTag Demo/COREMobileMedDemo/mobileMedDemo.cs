using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Microsoft.WindowsMobile.Forms;
using System.IO;
using System.Net;

namespace COREMobileMedDemo
{
    public partial class mobileMedDemo : Form
    {
        public mobileMedDemo()
        {
            InitializeComponent();
        }

        byte[] image;

        string RFIDnum;

        private void menuItem1_Click(object sender, EventArgs e)
        {
            CameraCaptureDialog newImage = new CameraCaptureDialog();
            //newImage.FileName = "patientImage.jpg";
            newImage.StillQuality = CameraCaptureStillQuality.High;
            newImage.Resolution = new Size(320,240);            
            newImage.Owner = this;
            newImage.Title = "Patient Photo";            
            newImage.Mode = CameraCaptureMode.Still;

            if (newImage.ShowDialog() == DialogResult.OK && newImage.FileName.Length > 0)
            {
                FileStream fileStream = new FileStream(newImage.FileName, FileMode.Open, FileAccess.Read);

                image = new byte[fileStream.Length];

                fileStream.Read(image, 0, System.Convert.ToInt32(fileStream.Length));

                fileStream.Close();

                pictureBox1.Image = new Bitmap(newImage.FileName);

                System.IO.File.Delete(newImage.FileName);
            }
        }

        private void menuItem2_Click(object sender, EventArgs e)
        {
            if (image == null || string.IsNullOrEmpty(RFIDnum))
                MessageBox.Show("Must have both image and bracelet ID first");
            else
            {
                try
                {
                    // time to submit the rfid number and image to the web service
                    COREMedDemoWS.patientRecord pR = new COREMedDemoWS.patientRecord();

                    pR.image = image;
                    pR.RFIDnum = RFIDnum;
                    COREMedDemoWS.COREMedDemoWS ws = new COREMedDemoWS.COREMedDemoWS();
                    COREMedDemoWS.errorReport eR = ws.enterPatientPhoto(pR);
                    if (eR.errorCode != 0)
                        MessageBox.Show(eR.errorMessage);
                }
                catch (WebException ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;
            RFIDnum = textBox1.Text = NativeMethods.readOneTagID();
            Cursor.Current = Cursors.Default;
        }
      
    }
}