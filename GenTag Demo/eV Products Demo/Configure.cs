using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.ComponentModel;
using System.IO;

namespace eV_Products_Demo
{
    public partial class Configure : Form
    {
        public Configure(eVDemo mF)
        {
            InitializeComponent();
            this.mF_Form = mF;
            changed = false;
            lldvld = false;
            biasvld = false;
        }

        private eVDemo mF_Form;
        private bool changed;
        private string bias;
        private string lld;
        private bool lldvld;
        private bool biasvld;

        public string obias;
        public string olld;
        public string ogain;
        public string opt;

        private void cmdExit_Click(object sender, EventArgs e)
        {
            if (changed)
                Savechanged();
            if (lldvld == false && biasvld == false)
            {
                mF_Form.BringToFront();// prevent the MainForm Disappear after some MessageBox is shown.
                this.Hide();
            }
        }

        private void Configure_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = true;// for cancel the Form close so that all data in Config can be kept.
            if (changed)
                Savechanged();
            if (lldvld == false && biasvld == false)
            {
                mF_Form.BringToFront();// prevent the MainForm Disappear after some MessageBox is shown.
                this.Hide();
            }
        }

        private void CommandReset_Click(object sender, EventArgs e)
        {
            if (mF_Form.SetDefault())
            {
                MessageBox.Show("All parameters have been reset to the default factory values!");
            }
            changed = true;
        }

        private void cmbGain_SelectionChangeCommitted(object sender, EventArgs e)
        {
            if (this.CanSelect)
            {
                changed = true;
                if (!mF_Form.SetGain(this.cmbGain.Text))
                {
                    MessageBox.Show("Set Gain Error");
                    cmbGain.Text = ogain;
                    changed = false;
                }
                else
                {
                    mF_Form.Activedata.dGain = this.cmbGain.Text;
                }
            }
        }

        private void Savechanged()
        {
            string FILE_NAME = this.LabelCSN.Text + ".ser";

            changed = false;
            if (File.Exists(FILE_NAME))
            {
                using (StreamWriter sw = File.CreateText(FILE_NAME))
                {

                    sw.WriteLine("Detector Type = " + this.LabelCDT.Text);
                    sw.WriteLine("Peaking time = " + this.cmbPeakingTime.Text);
                    sw.WriteLine("Channel Gain = " + this.cmbGain.Text);
                    try
                    {
                        if (double.Parse(this.TextVB.Text) < 0 || double.Parse(this.TextVB.Text) > 2000)
                        {
                            MessageBox.Show("The valid range of bias voltage is 0-2000 Volts");
                            this.TextVB.Text = "0";
                        }
                        sw.WriteLine("Detector Bias = " + this.TextVB.Text);
                    }
                    catch (Exception)
                    {
                        MessageBox.Show("The valid range of bias voltage is 0-2000 Volts");
                    }
                    try
                    {
                        if (double.Parse(this.TextLLD.Text) < 0 || double.Parse(this.TextLLD.Text) > 2499)
                        {
                            MessageBox.Show("The threshold has been set to 0");
                            this.TextLLD.Text = "0";
                        }
                        sw.WriteLine("Threshold = " + this.TextLLD.Text);
                    }
                    catch (Exception)
                    {
                        MessageBox.Show("The valid range of Threshold is 0-2499 mV");
                    }
                    sw.Close();
                }
            }
            else
                MessageBox.Show("Can't find the *.ser file of this detector");
        }

        private void cmbPeakingTime_SelectionChangeCommitted(object sender, EventArgs e)
        {
            if (this.CanSelect)
            {
                changed = true;
                if (!mF_Form.SetPT(this.cmbPeakingTime.Text))
                {
                    MessageBox.Show("Set Peak Time Error");
                    cmbPeakingTime.Text = opt;
                    changed = false;
                }
                else
                {
                    mF_Form.Activedata.sPeakT = this.cmbPeakingTime.Text;
                }
            }
        }

        private void TextVB_Enter(object sender, EventArgs e)
        {
            bias = this.TextVB.Text;
        }

        private void TextVB_Validating(object sender, CancelEventArgs e)
        {
            if (bias != this.TextVB.Text)
            {
                try
                {
                    if (double.Parse(this.TextVB.Text) >= 0 && double.Parse(this.TextVB.Text) <= 2000)
                    {
                        changed = true;
                        if (!mF_Form.SetBias(this.TextVB.Text))
                        {
                            MessageBox.Show("Set Bias Error");
                            TextVB.Text = obias;
                            changed = false;
                        }
                        else
                        {
                            mF_Form.Activedata.sBias = this.TextVB.Text;
                        }
                        biasvld = false;
                        errorProvider1.SetError(TextVB, "");
                    }
                    else
                    {
                        errorProvider1.SetError(TextVB, "The valid range of bias voltage is 0-2000 Volts");
                        biasvld = true;
                        e.Cancel = true;
                    }
                }
                catch (Exception)
                {
                    MessageBox.Show("The valid range of bias voltage is 0-2000 Volts");
                    TextVB.Text = "0";
                    e.Cancel = true;
                }
            }
        }

        private void TextLLD_Validating(object sender, CancelEventArgs e)
        {
            if (lld != TextLLD.Text)
            {
                try
                {
                    if (double.Parse(this.TextLLD.Text) >= 0 && double.Parse(this.TextLLD.Text) <= 2499)
                    {
                        changed = true;
                        if (!mF_Form.SetLLD(this.TextLLD.Text))
                        {
                            MessageBox.Show("Set LLD Error");
                            TextLLD.Text = olld;
                            changed = false;
                        }
                        else
                        {
                            mF_Form.Activedata.sLLD = this.TextLLD.Text;
                        }
                        errorProvider1.SetError(TextLLD, "");
                        lldvld = false;
                    }
                    else
                    {
                        errorProvider1.SetError(TextLLD, "The valid range of Threshold is 0-2499 mV");
                        lldvld = true;
                        e.Cancel = true;
                    }
                }
                catch (Exception)
                {
                    MessageBox.Show("The valid range of Threshold is 0-2499 mV");
                    TextLLD.Text = "0";
                    e.Cancel = true;
                }
            }
        }

        private void TextLLD_Enter(object sender, EventArgs e)
        {
            lld = this.TextLLD.Text;
        }

        private void TextVB_KeyDown(object sender, KeyEventArgs e)
        {
            mF_Form.nonNumberEntered = false;
            if (e.KeyCode < Keys.D0 || e.KeyCode > Keys.D9)
            {
                if (e.KeyCode < Keys.NumPad0 || e.KeyCode > Keys.NumPad9)
                    if (e.KeyCode != Keys.Back)
                        mF_Form.nonNumberEntered = true;
            }
        }

        private void TextVB_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (mF_Form.nonNumberEntered == true)
                e.Handled = true;
        }

        private void TextLLD_KeyDown(object sender, KeyEventArgs e)
        {
            mF_Form.nonNumberEntered = false;
            if (e.KeyCode < Keys.D0 || e.KeyCode > Keys.D9)
            {
                if (e.KeyCode < Keys.NumPad0 || e.KeyCode > Keys.NumPad9)
                    if (e.KeyCode != Keys.Back)
                        mF_Form.nonNumberEntered = true;
            }
        }

        private void TextLLD_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (mF_Form.nonNumberEntered == true)
                e.Handled = true;
        }

        private void cmbPeakingTime_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void cmbGain_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
    }
}
