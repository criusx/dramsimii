using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Threading;
using System.Runtime.CompilerServices;
using System.Collections;
using System.Net;


namespace CORE_MedDemo
{
    public partial class medDemo : Form
    {
        public medDemo()
        {
            InitializeComponent();
        }

        bool readerRunning = false;

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void getIDButton_Click(object sender, EventArgs e)
        {
            if (!readerRunning)
            {
                readerRunning = true;
                setButton(getIDButton, "Stop");
                new Thread(new ThreadStart(readTagID)).Start();
            }
            else
            {
                readerRunning = false;
                setButton(getIDButton, "Get ID");
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
                setTextBox(braceletIDtextBox, tagID);
                getIDButton_Click(getIDButton, new EventArgs());
            }
            //throw new IOException(@"Unable to read tag");
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

        private void EntryBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == System.Windows.Forms.Keys.Enter)
            {
                if ((sender == allergyEntryBox || sender == button1) && (allergyEntryBox.Text.Length > 0))
                {
                    allergiesListBox.Items.Add(allergyEntryBox.Text);
                    allergyEntryBox.Text = "";
                    allergyEntryBox.Focus();
                }
                else if (currentMedEntryBox.Text.Length > 0)
                {
                    currentMedsListBox.Items.Add(currentMedEntryBox.Text);
                    currentMedEntryBox.Text = "";
                    currentMedEntryBox.Focus();
                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            EntryBox_KeyDown(sender, new KeyEventArgs(Keys.Enter));
        }

        private void ListBox_DoubleClick(object sender, EventArgs e)
        {
            if (((ListBox)sender).SelectedIndex >= 0)
                ((ListBox)sender).Items.RemoveAt(((ListBox)sender).SelectedIndex);
        }

        private void submitButton_Click(object sender, EventArgs e)
        {
            try
            {
                COREMedDemoWS.COREMedDemoWS ws = new COREMedDemoWS.COREMedDemoWS();
                COREMedDemoWS.patientRecord patient = new COREMedDemoWS.patientRecord();
                patient.RFIDnum = braceletIDtextBox.Text;
                patient.firstName = firstNameTextBox.Text;
                patient.middleName = middleNameTextBox.Text;
                patient.lastName = lastNameTextBox.Text;
                TimeSpan t = dateOfBirthDateTimePicker.Value.ToUniversalTime() - new DateTime(1970, 1, 1).ToUniversalTime();
                patient.DOB = (long)t.TotalMilliseconds;
                ArrayList allergyArray = new ArrayList(allergiesListBox.Items);
                patient.allergies = (string[])allergyArray.ToArray(typeof(string));
                ArrayList medicationsArray = new ArrayList(currentMedsListBox.Items);
                patient.medications = (string[])medicationsArray.ToArray(typeof(string));
                COREMedDemoWS.errorReport eR = ws.enterPatientRecord(patient);
                if (eR.errorCode != 0)
                    MessageBox.Show(eR.errorMessage);
            }
            catch (WebException ex)
            {
                MessageBox.Show(ex.Message);
            }
        }
    }
}