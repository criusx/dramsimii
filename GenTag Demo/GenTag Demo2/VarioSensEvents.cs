using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace GentagDemo
{
    public partial class demoClient : Form
    {
        void receiveVarioSensSettings(Single upperLimit, Single lowerLimit, short recordPeriod, short logMode, short batteryCheckInterval)
        {
            throw new Exception("Implement this");
        }

        void writeViolations( Single upperTempLimit,
            Single lowerTempLimit,
            Int32 len,
            short recordPeriod,
            int[] dateTime,
            Byte[] logMode,
            Single[] temperatures)
        {
            throw new Exception("Implement this");
        }

        void launchReadVSLog()
        {
            throw new Exception("Implement this");
        }

        void launchSetVSSettings()
        {
            throw new Exception("Implement this");
        }

        void launchGetVSSettings()
        {
            throw new Exception("Implement this");
        }

        void radScan()
        {
            throw new Exception("Implement this");
        }


    }
}
