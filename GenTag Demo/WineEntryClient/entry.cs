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
using System.Security.Permissions;
using System.Runtime.InteropServices;
using System.IO;
using System.Text.RegularExpressions;
using System.Net;

[assembly: CLSCompliant(true)]
[assembly: SecurityPermission(SecurityAction.RequestMinimum, Execution = true)]
namespace WineEntryClient
{
    public partial class entry : Form
    {
        private string DeviceUID = "FFFFFFFFFFFFFFFFFFFF";

        public entry()
        {
            InitializeComponent();
        }

        bool readerRunning = false;

        private void readButton_Click(object sender, EventArgs e)
        {
            if (readerRunning)
            {
                setButton(button1, "Get");
                readerRunning = false;
            }
            else
            {
                readerRunning = true;
                setButton(button1, "Stop");
                new Thread(new ThreadStart(readTagID)).Start();
            }
        }

        string tagID;

        private void readTagID()
        {
            if (C1Lib.C1.NET_C1_open_comm() == 0)
            {
                //throw new IOException(@"Unable to open comm to reader");
                MessageBox.Show(Properties.Resources.UnableToOpenCommToReader);
            }
            else if (C1Lib.C1.NET_C1_enable() != 1)
            {
                C1Lib.C1.NET_C1_disable();
                MessageBox.Show(Properties.Resources.UnableToEnableDevice);
                //throw new IOException(@"Unable to enable device");
            }
            else
            {
                // wait while a tag is read
                while ((readerRunning == true) && C1Lib.ISO_15693.NET_get_15693(0x00) == 0) { Thread.Sleep(50); }

                if (readerRunning == false)
                    return;

                StringBuilder newTag = new StringBuilder(C1Lib.ISO_15693.tag.id_length);

                for (int i = 0; i < C1Lib.ISO_15693.tag.id_length; i++)
                    newTag.Append(C1Lib.util.hex_value(C1Lib.ISO_15693.tag.tag_id[i]));

                C1Lib.C1.NET_C1_disable();
                C1Lib.C1.NET_C1_close_comm();

                tagID = newTag.ToString();
                setTextBox(idBox, tagID);
                readButton_Click(button1, new EventArgs());
            }
            //throw new IOException(@"Unable to read tag");
        }

        private string imagename;

        private void browseButton_Click(object sender, EventArgs e)
        {
            PictureBox pictureBox;
            if (sender == wineBrowseButton)
            {
                pictureBox = winePB;
            }
            else
            {
                return;
            }
            try
            {
                FileDialog fldlg = new OpenFileDialog();

                fldlg.Filter = "Image File (*.jpg;*.bmp;*.gif;*.png)|*.jpg;*.bmp;*.gif;*.png";

                if (fldlg.ShowDialog() == DialogResult.OK)
                {
                    imagename = fldlg.FileName;

                    Bitmap newimg = new Bitmap(imagename);

                    pictureBox.SizeMode = PictureBoxSizeMode.CenterImage;

                    pictureBox.Image = (Image)newimg;
                }

                fldlg = null;
            }
            catch (System.ArgumentException ex)
            {
                imagename = " ";
                MessageBox.Show(ex.Message.ToString());
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

        private delegate void setButtonDelegate(Button tB, string desc);

        private void setButton(Button tB, string val)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setButtonDelegate(setButton), new object[] { tB, val });
                return;
            }
            tB.Text = val;
        }
        #endregion

        private void saveButton_Click(object sender, EventArgs e)
        {
            statusLabel.Text = Properties.Resources.emptyString;

            for (int triesLeft = 5; triesLeft > 0; triesLeft--)
            {
                try
                {
                    //proceed only when the image has a valid path

                    if (!string.IsNullOrEmpty(imagename))
                    {
                        FileStream fls = new FileStream(@imagename, FileMode.Open, FileAccess.Read);

                        byte[] blob = new byte[fls.Length];

                        fls.Read(blob, 0, System.Convert.ToInt32(fls.Length));

                        fls.Close();

                        bool status = false;
                       if (sender == wineSaveButton)
                        {
                            wineWS.wineWS ws = new wineWS.wineWS();
                            status = ws.enterBottleInformation(new string[] { tagID }, wineTypeComboBox.Text, Convert.ToInt32(yearUpDown.Value), countryBox.Text, vineyardBox.Text, reviewBox.Text, blob);
                            ws.Dispose();
                        }
                        if (status)
                            statusLabel.Text = Properties.Resources.Success;
                        else
                            statusLabel.Text = Properties.Resources.failedWithReason;
                        return;
                    }
                }
                catch (Exception ex)
                {
                    statusLabel.Text = Properties.Resources.failedWithReason + ex.Message;
                }
            }
        }

        private void clearButton_Click(object sender, EventArgs e)
        {
            idBox.Clear();
           if (sender == wineClearButton)
            {             
                wineTypeComboBox.SelectedIndex = 0;
                yearUpDown.Value = DateTime.Now.Year;
                countryBox.SelectedIndex = 0;
                vineyardBox.Clear();
                reviewBox.Clear();
                winePB.Image = null;
            }
        }

        private void loadButton_Click(object sender, EventArgs e)
        {
            statusLabel.Text = null;
            int triesLeft = 5;
            while (triesLeft > 0)
            {
                try
                {
                    wineWS.wineBottle bottle =
                        (new wineWS.wineWS()).retrieveBottleInformation(tagID, DeviceUID, (float)0.0, (float)0.0);

                    if (bottle.exists)
                    {
                        wineTypeComboBox.SelectedItem = bottle.type;
                        yearUpDown.Value = Convert.ToDecimal(bottle.year);
                        countryBox.SelectedItem = bottle.origin;
                        vineyardBox.Text = bottle.vineyard;
                        reviewBox.Text = bottle.review;
                        winePB.Image = new Bitmap(new MemoryStream(bottle.image));
                        statusLabel.Text = Properties.Resources.Success;
                    }
                    else
                    {
                        statusLabel.Text = Properties.Resources.InvalidRFID;
                    }
                    return;
                }
                catch (Exception ex)
                {
                    statusLabel.Text = Properties.Resources.failedWithReason + ex.Message;
                    triesLeft--;
                }
            }
        }

        private void idBox_TextChanged(object sender, EventArgs e)
        {
            idBox.Text = Regex.Replace(idBox.Text, "[^0-9a-fA-F]+", "");
            idBox.SelectionStart = idBox.Text.Length;
            tagID = idBox.Text;
            if (idBox.Text.Length < 16)
            {
                return;
            }
            else
            {
                if (idBox.Text.Length > 16)
                    idBox.Text = idBox.Text.Substring(0, 16);
                unneededTextBox.Text = idBox.Text.Substring(0, 4);
                // convert the string to a byte array
                string remiainingBytes = idBox.Text.Substring(4);
                byte[] stringAsBytes = new byte[6];

                for (int i = 0; i < 12; i += 2)
                {
                    stringAsBytes[i / 2] = byte.Parse(remiainingBytes.Substring(i, 2), System.Globalization.NumberStyles.HexNumber);
                }

                
                byte[] buffer = new byte[8];
                buffer[0] = (byte)((stringAsBytes[0] & 0xFC) >> 2);
                buffer[4] = (byte)((stringAsBytes[3] & 0xFC) >> 2);
                buffer[1] = (byte)(((stringAsBytes[0] & 0x03) << 4) | ((stringAsBytes[1] & 0xF0) >> 4));
                buffer[5] = (byte)(((stringAsBytes[3] & 0x03) << 4) | ((stringAsBytes[4] & 0xF0) >> 4));
                buffer[2] = (byte)(((stringAsBytes[1] & 0x0F) << 2) | ((stringAsBytes[2] & 0xC0) >> 6));
                buffer[6] = (byte)(((stringAsBytes[4] & 0x0F) << 2) | ((stringAsBytes[5] & 0xC0) >> 6));
                buffer[3] = (byte)(stringAsBytes[2] & 0x3F);
                buffer[7] = (byte)(stringAsBytes[5] & 0x3F);

                char[] result = new char[buffer.Length];

                for (int i = 0; i < buffer.Length; i++)
                {
                    result[i] = sixbit2char(buffer[i]);
                }

                StringBuilder sb = new StringBuilder(result.Length);
                foreach (char ch in result)
                    sb.Append(ch);

                base64TextBox.Text = sb.ToString();
            }
        }

        private char sixbit2char(byte b)
        {
            char[] lookupTable = new char[64]{
                'A','B','C','D','E','F','G','H','I','J','K','L','M',
                'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
                'a','b','c','d','e','f','g','h','i','j','k','l','m',
                'n','o','p','q','r','s','t','u','v','w','x','y','z',
                '0','1','2','3','4','5','6','7','8','9','+','/'
            };
            
            if ((b >= 0) && (b <= 63))
            {
                return lookupTable[(int)b];
            }
            else
            {
                //should not happen;
                return ' ';
            }
        }

        private void textBox_Click(object sender, EventArgs e)
        {
            ((TextBox)sender).SelectionStart = 0;
            ((TextBox)sender).SelectionLength = ((TextBox)sender).Text.Length;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (idBox.Text.Length < 16)
            {
                MessageBox.Show(Properties.Resources.rfidTagLengthError);
                return;
            }
            if (descriptionTextBox.Text.Length < 1)
            {
                MessageBox.Show(Properties.Resources.emptyDescriptionError);
                return;
            }
            int retryCount = 5;
            while (retryCount > 0)
            {
                try
                {
                    Cursor.Current = Cursors.WaitCursor;
                    authWS.GetDatesWS ws = new authWS.GetDatesWS();
                    if (!ws.setItem(idBox.Text, DeviceUID, descriptionTextBox.Text, authCheckBox.Checked))
                        MessageBox.Show(Properties.Resources.unableToSendAuth);
                    Cursor.Current = Cursors.Default;
                    break;
                }
                catch (WebException ex)
                {

                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message.ToString());
                }
                finally
                {
                    retryCount--;
                }                
            }
        }
        
    }
}