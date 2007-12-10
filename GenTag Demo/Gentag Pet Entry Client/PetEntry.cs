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
using ReaderFull;

[assembly: CLSCompliant(true)]
[assembly: SecurityPermission(SecurityAction.RequestMinimum, Execution = true)]
namespace Gentag_Pet_Entry_Client
{
    public partial class PetEntry : Form
    {
        private string DeviceUID = "FFFFFFFFFFFFFFFFFFFF";

        private ChangePasswordForm passwordDialog;

        public PetEntry()
        {
            InitializeComponent();

            tagReader = new Reader();

            passwordDialog = new ChangePasswordForm();

            tagReader.TagReceived += new Reader.TagReceivedEventHandler(tagReader_TagReceived);
            tagReader.ReaderError += new Reader.ReaderErrorHandler(tagReader_ReaderError);
        }

        void tagReader_ReaderError(string errorMessage)
        {
            MessageBox.Show(errorMessage);
        }

        void tagReader_TagReceived(string tagID)
        {
            setTextBox(idBox, tagID);
            readButton_Click(this, new EventArgs());
        }

        bool readerRunning = false;

        private void readButton_Click(object sender, EventArgs e)
        {
            if (readerRunning)
            {
                setButton(retrieveTagButton, "Get");
                readerRunning = false;
            }
            else
            {
                readerRunning = true;
                setButton(retrieveTagButton, "Stop");
                new Thread(new ThreadStart(tagReader.readTagID)).Start();
            }
        }

        Reader tagReader;

        string tagID;

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

                    setPhoto(imagePB, newimg);

                    //pictureBox.Image = (Image)newimg;
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


        private void clearButton_Click(object sender, EventArgs e)
        {
            idBox.Clear();
            if (sender == clearButton)
            {

            }
        }

        private void downloadData()
        {
            setLabel(statusLabel, "");

            // do some error checking on the input data
            if (idBox.Text.Length < 16)
            {
                setLabel(statusLabel, Properties.Resources.InvalidRFID);
                return;
            }

            petWS.petWS ws = new petWS.petWS();
            ws.Timeout = 45000;

            for (int triesLeft = 5; triesLeft > 0; triesLeft--)
            {
                try
                {
                    petWS.petInfo pI = ws.retrievePetInformation(getTextBox(idBox), DeviceUID, 0.0F, 0.0F, getTextBox(passwordTB));

                    if (pI.exists)
                    {
                        setLabel(statusLabel, Properties.Resources.Success);
                        // fill in the text boxes with extra info
                        setTextBox(ownerTB, pI.owner);
                        string[] addresses = pI.contactInfo.Split('\n');
                        if (addresses.Length > 0)
                            setTextBox(ownerAddress1TB, addresses[0]);
                        if (addresses.Length > 1)
                            setTextBox(ownerAddress2TB, addresses[1]);
                        if (addresses.Length > 2)
                            setTextBox(ownerAddress3TB, addresses[2]);
                        setTextBox(breedTB, pI.breed);
                        setTextBox(nameTB, pI.name);
                        setTextBox(homePhoneTB, pI.homePhone);
                        setTextBox(cellPhoneTB, pI.cellPhone);
                        setTextBox(workPhoneTB, pI.workPhone);
                        setTextBox(emailTB, pI.email);
                        setTextBox(dietTB, pI.preferredFood);
                        setTextBox(medicalNeedsTB, pI.medicalNeeds);
                        setTextBox(medicationsTB, pI.medications);
                        setTextBox(vetNameTB, pI.vetName);
                        setTextBox(vetPhoneTB, pI.vetPhone);
                        addresses = pI.vetAddress.Split('\n');
                        if (addresses.Length > 0)
                            setTextBox(vetAddress1TB, addresses[0]);
                        if (addresses.Length > 1)
                            setTextBox(vetAddress2TB, addresses[1]);
                        if (addresses.Length > 2)
                            setTextBox(vetAddress3TB, addresses[2]);
                        setTextBox(rewardTB, pI.reward.ToString());
                        if (pI.image != null && pI.image.Length > 0)
                            setPhoto(imagePB, pI.image);

                    }
                    else
                    {
                        setLabel(statusLabel, Properties.Resources.FailedNoReason);
                    }

                    break;
                }
                catch (Exception ex)
                {
                    setLabel(statusLabel, Properties.Resources.failedWithReason + ex.Message);
                    triesLeft--;
                }
            }
            enableButtons();
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

        private void textBox_Click(object sender, EventArgs e)
        {
            ((TextBox)sender).SelectionStart = 0;
            ((TextBox)sender).SelectionLength = ((TextBox)sender).Text.Length;
        }

        private void wineBrowseButton_Click(object sender, EventArgs e)
        {
            PictureBox pictureBox;
            if (sender == BrowseForImageButton)
            {
                pictureBox = imagePB;
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

        private void wineClearButton_Click(object sender, EventArgs e)
        {
            idBox.Clear();
            if (sender == clearButton)
            {
                imagePB.Image = null;
            }
            authCodeTB.Clear();
            passwordTB.Clear();
            ownerTB.Clear();
            ownerAddress1TB.Clear();
            ownerAddress2TB.Clear();
            ownerAddress3TB.Clear();
            breedTB.Clear();
            nameTB.Clear();
            homePhoneTB.Clear();
            cellPhoneTB.Clear();
            workPhoneTB.Clear();
            emailTB.Clear();
            dietTB.Clear();
            medicalNeedsTB.Clear();
            medicationsTB.Clear();
            vetNameTB.Clear();
            vetPhoneTB.Clear();
            vetAddress1TB.Clear();
            vetAddress2TB.Clear();
            vetAddress3TB.Clear();
            rewardTB.Clear();
        }

        private void uploadData()
        {
            setLabel(statusLabel,Properties.Resources.emptyString);

            // do some error checking on the input data
            if (idBox.Text.Length < 16)
            {
                setLabel(statusLabel,Properties.Resources.InvalidRFID);
                return;
            }

            // fill in the info on this pet
            petWS.petInfo info = new petWS.petInfo();

            info.owner = getTextBox(ownerTB);
            info.contactInfo = getTextBox(ownerAddress1TB) + "\n" + getTextBox(ownerAddress2TB) + "\n" + getTextBox(ownerAddress3TB);
            info.breed = getTextBox(breedTB);
            info.name = getTextBox(nameTB);
            info.homePhone = getTextBox(homePhoneTB);
            info.cellPhone = getTextBox(cellPhoneTB);
            info.workPhone = getTextBox(workPhoneTB);
            info.email = getTextBox(emailTB);
            info.preferredFood = getTextBox(dietTB);
            info.medicalNeeds = getTextBox(medicalNeedsTB);
            info.medications = getTextBox(medicationsTB);
            info.vetName = getTextBox(vetNameTB);
            info.vetPhone = getTextBox(vetPhoneTB);
            info.vetAddress = getTextBox(vetAddress1TB) + "\n" + getTextBox(vetAddress2TB) + "\n" + getTextBox(vetAddress3TB);
            info.tagCode = getTextBox(authCodeTB);
            info.password = getTextBox(passwordTB);
            info.rfidNum = getTextBox(idBox);

            // try to convert the reward into an int
            try
            {
                info.reward = int.Parse(getTextBox(rewardTB));
                if (info.reward < 0)
                    throw new Exception();
            }
            catch (Exception ex)
            {
                MessageBox.Show("invalid reward amount entered");
            }

            // try to read the image in
            if (!string.IsNullOrEmpty(imagename))
            {
                FileStream fls = new FileStream(@imagename, FileMode.Open, FileAccess.Read);

                info.image = new byte[fls.Length];

                fls.Read(info.image, 0, System.Convert.ToInt32(fls.Length));

                fls.Close();
            }

            petWS.petWS gentagPetWS = new petWS.petWS();
            gentagPetWS.Timeout = 45000;

            // then try several times to send it off to the web service
            for (int triesLeft = 5; triesLeft > 0; triesLeft--)
            {
                try
                {
                    petWS.errorMessage errorMessage = gentagPetWS.insertOrUpdatePetInformation(info);

                    gentagPetWS.Dispose();
                    if (errorMessage.success)
                        setLabel(statusLabel,Properties.Resources.Success);
                    else
                        setLabel(statusLabel,Properties.Resources.failedWithReason + errorMessage.errorMessage1);
                    break;

                }
                catch (Exception ex)
                {
                    setLabel(statusLabel,Properties.Resources.failedWithReason + ex.Message);
                }
            }
            enableButtons();
        }



        private void idBox_TextChanged_1(object sender, EventArgs e)
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

        private void idBox_Click(object sender, EventArgs e)
        {
            ((TextBox)sender).SelectionStart = 0;
            ((TextBox)sender).SelectionLength = ((TextBox)sender).Text.Length;
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked)
                passwordTB.PasswordChar = '\0';
            else
                passwordTB.PasswordChar = '*';
        }

        private void button2_Click_2(object sender, EventArgs e)
        {
            new Thread(new ThreadStart(uploadData)).Start();
            disableButtons();
        }



        private void wineLoadButton_Click_1(object sender, EventArgs e)
        {
            new Thread(new ThreadStart(downloadData)).Start();
            disableButtons();
        }

        private void disableButtons()
        {

            setButtonState(createUpdateButton, false);
            setButtonState(retrieveDataButton, false);
            setButtonState(retrieveTagButton, false);
            setButtonState(clearButton, false);
            setButtonState(BrowseForImageButton, false);
            setButtonState(retrieveTagButton, false);
        }

        private void enableButtons()
        {
            setButtonState(createUpdateButton, true);
            setButtonState(retrieveDataButton, true);
            setButtonState(retrieveTagButton, true);
            setButtonState(clearButton, true);
            setButtonState(BrowseForImageButton, true);
            setButtonState(retrieveTagButton, true);
        }

        #region Safe Accessors and Mutators

        private delegate void bringToFrontDelegate(Control c);

        private void bringToFront(Control c)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new bringToFrontDelegate(bringToFront), new object[] { c });
            }
            else
            {
                c.Visible = true;
                c.BringToFront();
            }
        }

        private delegate void setWaitCursorDelegate(bool set);

        private void setWaitCursor(bool set)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setWaitCursorDelegate(setWaitCursor), new object[] { set });
            }
            if (set == true)
                Cursor.Current = Cursors.WaitCursor;
            else
                Cursor.Current = Cursors.Default;
        }

        private delegate Color getPanelDelegate(Panel p);

        private Color getPanel(Panel p)
        {
            if (this.InvokeRequired)
            {
                return (Color)this.Invoke(new getPanelDelegate(getPanel), new object[] { p });
            }
            return p.BackColor;
        }

        private delegate Color getButtonColorDelegate(Button p);

        private Color getButtonColor(Button p)
        {
            if (this.InvokeRequired)
            {
                return (Color)this.Invoke(new getButtonColorDelegate(getButtonColor), new object[] { p });
            }
            return p.BackColor;
        }

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

        private delegate void setButtonDelegate(Button p, String s);

        private void setButton(Button p, String s)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setButtonDelegate(setButton), new object[] { p, s });
                return;
            }
            p.Text = s;
        }

        private delegate void setButtonColorDelegate(Button p, Color c);

        private void setButtonColor(Button p, Color c)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setButtonColorDelegate(setButtonColor), new object[] { p, c });
                return;
            }
            p.BackColor = c;
        }

        private delegate void setCheckBoxDelegate(CheckBox cB, bool check);

        private void setCheckBox(CheckBox cB, bool check)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setCheckBoxDelegate(setCheckBox), new object[] { cB, check });
                return;
            }
            cB.Checked = check;
        }

        private delegate bool getCheckBoxDelegate(CheckBox cB);

        private bool getCheckBox(CheckBox cB)
        {
            if (this.InvokeRequired)
            {
                return (bool)this.Invoke(new getCheckBoxDelegate(getCheckBox), new object[] { cB });
            }
            return cB.Checked;
        }

        private delegate int getComboBoxIndexDelegate(ComboBox cB);

        private int getComboBoxIndex(ComboBox cB)
        {
            if (this.InvokeRequired)
            {
                return (int)this.Invoke(new getComboBoxIndexDelegate(getComboBoxIndex), new object[] { cB });
            }
            return cB.SelectedIndex;
        }

        private delegate void setComboBoxIndexDelegate(ComboBox cB, int index);

        private void setComboBoxIndex(ComboBox cB, int index)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setComboBoxIndexDelegate(setComboBoxIndex), new object[] { cB, index });
                return;
            }
            try
            {
                cB.SelectedIndex = index;
            }
            catch (ArgumentOutOfRangeException)
            {
                cB.SelectedIndex = 0;
            }
        }

        private delegate void setLabelDelegate(Label tB, string desc);

        private void setLabel(Label tB, string val)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setLabelDelegate(setLabel), new object[] { tB, val });
                return;
            }
            tB.Text = val;
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

        private delegate void setProgressBarDelegate(ProgressBar pB, int val);

        private void setProgressBar(ProgressBar pB, int val)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setProgressBarDelegate(setProgressBar), new object[] { pB, val });
                return;
            }
            if (val > pB.Maximum)
                pB.Value = pB.Maximum;
            else if (val < pB.Minimum)
                pB.Value = pB.Minimum;
            else
                pB.Value = val;
        }

        private delegate string getTextBoxDelegate(TextBox tB);

        private string getTextBox(TextBox tB)
        {
            if (this.InvokeRequired)
            {
                return (string)this.Invoke(new getTextBoxDelegate(getTextBox), new object[] { tB });
            }
            return tB.Text;
        }

        private delegate void setPhotoDelegateB(PictureBox pB, Image bA);

        private void setPhoto(PictureBox pB, Image bA)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setPhotoDelegateB(setPhoto), new object[] { pB, bA });
                return;
            }
            try
            {
                pB.Image = bA;
                pB.Refresh();
            }
            catch (ArgumentException)
            {

            }
        }

        private delegate void setPhotoDelegate(PictureBox pB, byte[] bA);

        private void setPhoto(PictureBox pB, byte[] bA)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setPhotoDelegate(setPhoto), new object[] { pB, bA });
                return;
            }
            try
            {
                if (pB.Image != null)
                    pB.Image.Dispose();
                pB.Image = new Bitmap(new MemoryStream(bA));
                pB.Refresh();
            }
            catch (ArgumentException)
            {

            }
        }

        private delegate void setButtonStateDelegate(Button b, bool enabled);

        private void setButtonState(Button b, bool enabled)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setButtonStateDelegate(setButtonState), new object[] { b, enabled });
                return;
            }
            b.Enabled = enabled;
        }
        #endregion

        private void changePasswordButton_Click(object sender, EventArgs e)
        {
            passwordDialog.setRFIDNum(getTextBox(idBox));
            if (getTextBox(idBox).Length < 16)
            {
                MessageBox.Show("Please enter a valid RFID number first");
                return;
            }

            DialogResult result = passwordDialog.ShowDialog();
        }

    }
}