using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Microsoft.VisualBasic.CompilerServices; // for String to int conv

namespace iSpectrum
{
    public partial class FWHM : Form
    {
        public FWHM(Form1 mF)
        {
            InitializeComponent();
            this.mF_Form = mF;
        }
        private Form1 mF_Form;

        private void Command_CAL_Click(object sender, EventArgs e)
        {
            if (mF_Form.paintROI())
            {
                int chl = 0;
                int chr = 0;
                int arg1 = 0;
                int arg2 = 0;
                try
                {
                    chl = Conversions.ToInteger(this.Text_Ch1.Text);
                    chr = Conversions.ToInteger(this.Text_Ch2.Text);
                    mF_Form.chl = chl;
                    mF_Form.chr = chr;
                }
                catch (Exception)
                {
                    MessageBox.Show("Invalid channel number selected!");
                }
                if ( chr < 4095)
                {
                    double fwhm = 0;
                    // if in KEV Disaplay, convert to channel.
                    if (mF_Form.Check_EenergyD.Enabled == true && mF_Form.Check_EenergyD.Checked == true)
                    {
                        double dtemp1;
                        double dtemp2;
                        if (mF_Form.Activedata.SeriesIndex >= 0)
                        {
                            dtemp1 = mF_Form.Activedata.factor;
                            dtemp2 = mF_Form.Activedata.intercept;
                        }
                        else
                        {
                            dtemp1 = mF_Form.Staticdata.factor;
                            dtemp2 = mF_Form.Staticdata.intercept;
                        }
                        arg1 = Conversions.ToInteger((chl - dtemp2) / dtemp1);
                        arg2 = Conversions.ToInteger((chr - dtemp2) / dtemp1);
                        mF_Form.chl = chl;
                        mF_Form.chr = chr;
                    }
                    else
                    {
                        arg1 = chl;
                        arg2 = chr;
                    }
                    try 
                    {
                        if (mF_Form.Activedata.SeriesIndex >= 0)
                        {
                            fwhm = mF_Form.Activedata.calcFWHM(arg1, arg2);
                            mF_Form.fwhm = fwhm;
                        }
                        if (mF_Form.Check_EenergyD.Enabled == true && mF_Form.Check_EenergyD.Checked == true)
                        {
                            mF_Form.Label_FWHM.Text = (fwhm * mF_Form.Activedata.factor).ToString("0.0") + " KeV";
                            this.lblFWHM.Text = (fwhm * mF_Form.Activedata.factor).ToString("0.0") + " KeV";
                        }
                        else
                        {
                            mF_Form.Label_FWHM.Text = fwhm.ToString("0.0") + " Channels";
                            this.lblFWHM.Text = fwhm.ToString("0.0") + " Channels";
                        }
                    }
                    catch (Exception)
                    {
                        MessageBox.Show("Calibration calculation Failed");
                    }
                }
                else
                    MessageBox.Show("Channel number must be less than 4095");
            }
        }

        private void FWHM_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.mF_Form.SetChartCursor();
            mF_Form.BringToFront();// prevent the MainForm Disappear after some MessageBox is shown.
            e.Cancel = true; // just hide, not close. the Cancel property of Main Form must set false before close the program
            this.Hide();
        }

        private void Command_Exit_Click(object sender, EventArgs e)
        {
            this.mF_Form.SetChartCursor();
            mF_Form.BringToFront();// prevent the MainForm Disappear after some MessageBox is shown.
            this.Hide();
        }

        private void Text_Ch1_KeyDown(object sender, KeyEventArgs e)
        {
            mF_Form.nonNumberEntered = false;
            if (e.KeyCode < Keys.D0 || e.KeyCode > Keys.D9)
            {
                if (e.KeyCode < Keys.NumPad0 || e.KeyCode > Keys.NumPad9)
                    if (e.KeyCode != Keys.Back)
                        if (e.KeyValue != 190 && e.KeyCode != Keys.Decimal)
                            mF_Form.nonNumberEntered = true;
            }
        }

        private void Text_Ch1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (mF_Form.nonNumberEntered == true)
                e.Handled = true;
        }

        private void Text_Ch2_KeyDown(object sender, KeyEventArgs e)
        {
            mF_Form.nonNumberEntered = false;
            if (e.KeyCode < Keys.D0 || e.KeyCode > Keys.D9)
            {
                if (e.KeyCode < Keys.NumPad0 || e.KeyCode > Keys.NumPad9)
                    if (e.KeyCode != Keys.Back)
                        if (e.KeyValue != 190 && e.KeyCode != Keys.Decimal)
                            mF_Form.nonNumberEntered = true;
            }
        }

        private void Text_Ch2_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (mF_Form.nonNumberEntered == true)
                e.Handled = true;
        }
    }
}