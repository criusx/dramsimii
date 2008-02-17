using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using RFIDReader;

namespace GentagDemo
{
    public partial class demoClient : Form
    {
        void receiveVarioSensSettings(object sender, VarioSensSettingsEventArgs args)
        {
            MessageBox.Show(Properties.Resources.DisabledVarioSens);
            //throw new Exception("Implement this");
        }

        void writeViolations(object sender, VarioSensLogEventArgs args)
        {
            MessageBox.Show(Properties.Resources.DisabledVarioSens);
            //throw new Exception("Implement this");
        }

        void launchReadVSLog()
        {
            MessageBox.Show(Properties.Resources.DisabledVarioSens);
            //throw new Exception("Implement this");
        }

        void launchSetVSSettings()
        {
            MessageBox.Show(Properties.Resources.DisabledVarioSens);
            //throw new Exception("Implement this");
        }

        void launchGetVSSettings()
        {
            MessageBox.Show(Properties.Resources.DisabledVarioSens);
            //throw new Exception("Implement this");
        }

        void radScan()
        {
            MessageBox.Show(Properties.Resources.DisabledVarioSens);
            //throw new Exception("Implement this");
        }


    }
}
