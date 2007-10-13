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
    public partial class Calibration : Form
    {
        public Calibration(Form1 mF)
        {
            InitializeComponent();
            this.mF_Form = mF;
        }

        private Form1 mF_Form;
        private void Command_Exit_Click(object sender, EventArgs e)
        {
            this.mF_Form.SetChartCursor();
            mF_Form.BringToFront();// prevent the MainForm Disappear after some MessageBox is shown
            this.Hide();
        }

        private void Command_CAL_Click(object sender, EventArgs e)
        {
            try
            {
                Conversions.ToDouble(this.Text_E1.Text);
                Conversions.ToDouble(this.Text_E2.Text);
                Conversions.ToDouble(this.Text_Ch2.Text);
                Conversions.ToDouble(this.Text_Ch1.Text);
            }
            catch (Exception)
            {
                MessageBox.Show("ADC channel and KeV must be numeric");
                return;
            }
            if (Conversions.ToDouble(this.Text_E1.Text) >= 0 && Conversions.ToDouble(this.Text_E1.Text) <= 3000
                && Conversions.ToDouble(this.Text_E2.Text) >= 0 && Conversions.ToDouble(this.Text_E2.Text) <= 3000
                && Conversions.ToDouble(this.Text_Ch1.Text) >= 0 && Conversions.ToDouble(this.Text_Ch1.Text) < 4096
                && Conversions.ToDouble(this.Text_Ch2.Text) >= 0 && Conversions.ToDouble(this.Text_Ch2.Text) < 4096)
            {
                try
                {
                    this.mF_Form.ctoe =
                    (Conversions.ToDouble(this.Text_E1.Text) - Conversions.ToDouble(this.Text_E2.Text)) / (Conversions.ToDouble(this.Text_Ch1.Text) - Conversions.ToDouble(this.Text_Ch2.Text));
                }
                catch(Exception)
                {
                    MessageBox.Show("Calibration failed, check ADC channel and KeV values");
                }

                if (this.mF_Form.ctoe > 0)
                {
                    this.mF_Form.d = (Conversions.ToDouble(this.Text_E1.Text)) - (Conversions.ToDouble(this.Text_Ch1.Text)) * mF_Form.ctoe;

                    this.mF_Form.SetAxisX();

                    this.mF_Form.Check_EenergyD.Enabled = true;
                }
                else
                {
                    MessageBox.Show("Negative calibration factor, check ADC channel and KeV values");
                }
            }
            else
                MessageBox.Show("The valid range of ADC channel is 0-4095 and KeV is 0-3000keV");
        }
        
        private void Calibration_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.mF_Form.SetChartCursor();
            mF_Form.BringToFront();// prevent the MainForm Disappear after some MessageBox is shown
            e.Cancel = true; // just hide, not close. the Cancel property of Main Form must set false before close the program
            this.Hide();
        }

        private void Text_E1_KeyDown(object sender, KeyEventArgs e)
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

        private void Text_E1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (mF_Form.nonNumberEntered == true)
                e.Handled = true;
        }

        private void Text_E2_KeyDown(object sender, KeyEventArgs e)
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

        private void Text_E2_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (mF_Form.nonNumberEntered == true)
                e.Handled = true;
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

        private void Text_Ch1_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (mF_Form.nonNumberEntered == true)
                e.Handled = true;
        }
    }
}