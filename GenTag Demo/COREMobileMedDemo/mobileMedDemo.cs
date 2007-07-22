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
using SiritReader;

namespace COREMobileMedDemo
{
    public partial class mobileMedDemo : Form
    {
        NativeMethods nativeMethods = new NativeMethods();

        public mobileMedDemo()
        {
            InitializeComponent();

            nativeMethods.VarioSensLogReceived +=new NativeMethods.VarioSensReadLogHandler(VarioSensLogReceived);

            selfReference = this;
            
        }

        byte[] image;

        string RFIDnum;

        static mobileMedDemo selfReference;

        private void menuItem1_Click(object sender, EventArgs e)
        {
            CameraCaptureDialog newImage = new CameraCaptureDialog();
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
            // only want VS tags so we can grab temp data
            RFIDnum = textBox1.Text = NativeMethods.readOneVSTagID();
            if (RFIDnum.Length > 0)
            {
                for (int i = 50; i >= 0; --i)
                {
                    if (nativeMethods.setVSSettings(1, 30.0f, 29.0f, 60, 360) == 0)
                        break;
                }
                MessageBox.Show("Please rescan the tag.", "Could not reset temp logging");
            }
            Cursor.Current = Cursors.Default;
        }



        private void button4_Click(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.WaitCursor;
            nativeMethods.readLog();
            Cursor.Current = Cursors.Default;
        }

        private static void VarioSensLogReceived(
            Single upperTempLimit,
            Single lowerTempLimit,
            Int32 len,
            short recordPeriod,
            int[] dateTime,
            Byte[] logMode,
            Single[] temperatures)
        {
            //if (mF.InvokeRequired)
            //{
            //    mF.Invoke(new NativeMethods.writeViolationsDelegate(writeViolations),
            //        new object[] { upperTempLimit, lowerTempLimit, len, recordPeriod, dateTime, logMode, temperatures });
            //    return;
            //}
            //mF.textBox9.Text = @"";
            //mF.textBox1.Text = upperTempLimit.ToString(CultureInfo.CurrentUICulture);
            //mF.textBox2.Text = lowerTempLimit.ToString(CultureInfo.CurrentUICulture);
            //mF.textBox3.Text = recordPeriod.ToString(CultureInfo.CurrentUICulture);
            //mF.listBox1.Items.Clear();
            //mF.graph.Clear();

            DateTime now = DateTime.Now;

            for (int i = 0; i < dateTime.Length; i++)
            {
                if (logMode[i] == 1)
                {                    
                    now = now - new TimeSpan(0, 0, 60);
                    ListViewItem lvi = new ListViewItem(new string[] {now.ToShortTimeString(),temperatures[i].ToString("0.0")});
                    selfReference.listView1.Items.Add(lvi);
                    //mF.listBox1.Items.Add(temperatures[i].ToString("F", CultureInfo.CurrentUICulture));
                    //mF.graph.Add(i, temperatures[i]);
                }
                else if (logMode[i] == 2)
                {
                    return;

                    //UInt32 time_t = Convert.ToUInt32(dateTime[i]);

                    //DateTime convertedValue = origin + new TimeSpan(time_t * TimeSpan.TicksPerSecond);
                    //if (System.TimeZone.CurrentTimeZone.IsDaylightSavingTime(convertedValue) == true)
                    //{
                    //    System.Globalization.DaylightTime daylightTime = System.TimeZone.CurrentTimeZone.GetDaylightChanges(convertedValue.Year);
                    //    convertedValue = convertedValue + daylightTime.Delta;
                    //}
                    //mF.listBox1.Items.Add(temperatures[i].ToString("F", CultureInfo.CurrentCulture) + " C" + convertedValue.ToString());

                }
            }
            //if (logMode[0] == 1)
            //{
            //    try
            //    {
            //        if (mF.listBox1.Items.Count > 1)
            //        {
            //            mF.graph.Visible = true;
            //            mF.graph.BringToFront();
            //        }
            //    }
            //    catch (Exception)
            //    {
            //        throw;
            //    }
            //}
        }
      
    }
}