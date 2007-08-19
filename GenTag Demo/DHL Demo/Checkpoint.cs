using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace DHL_Demo
{
    public enum deliveryOptions
    {
        Load,
        Deliver,
        Transfer,
        None
    }

    public partial class Checkpoint : Form
    {
        public Checkpoint()
        {
            InitializeComponent();
        }

        public deliveryOptions whichDeliveryOption
        {
            get
            {
                if (radioButton1.Checked)
                    return deliveryOptions.Load;
                else if (radioButton2.Checked)
                    return deliveryOptions.Deliver;
                else if (radioButton3.Checked)
                    return deliveryOptions.Transfer;
                else
                    return deliveryOptions.None;
            }

        }

        public void setInfo(string trackingNo, string email, string address)
        {
            addressLabel.Text = address;
            trackingLabel.Text = trackingNo;
            emailLabel.Text = email;
        }

        private void radioButton_Click(object sender, EventArgs e)
        {
            ((RadioButton)sender).Checked = true;
            if (sender != radioButton1)
                radioButton1.Checked = false;
            if (sender != radioButton2)
                radioButton2.Checked = false;
            if (sender != radioButton3)
                radioButton3.Checked = false;
        }
    }
}