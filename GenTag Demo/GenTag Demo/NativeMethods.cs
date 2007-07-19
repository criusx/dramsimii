using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Runtime.CompilerServices;
using System.Windows.Forms;
using System.Threading;
using System.Globalization;

namespace GentagDemo
{
    class NativeMethods
    {
        #region Delegates
        public delegate void TagReceivedEventHandler(String tagID);
        public delegate void VarioSensSettingsReceivedEventHandler(Single hiLimit, Single loLimit, short period, short logMode, short batteryCheckInterval);
        public delegate void ReaderErrorHandler(string errorMessage);
        #endregion

        #region Events
        public event TagReceivedEventHandler TagReceived;
        public event VarioSensSettingsReceivedEventHandler VarioSensSettingsReceived;
        public event ReaderErrorHandler ReaderError;
        #endregion

        private static NativeMethods existingNativeMethods;

        public NativeMethods()
        {
            if (existingNativeMethods == null)
                existingNativeMethods = this;
            else
                throw new ArgumentException("Too many NativeMethods classes created");
        }

        [DllImport("coredll.dll")]
        internal extern static int GetDeviceUniqueID(
            [In, Out] byte[] appdata,
            int cbApplictionData,
            int dwDeviceIDVersion,
            [In, Out] byte[] deviceIDOuput,
            out uint pcbDeviceIDOutput);

        public delegate void writeViolationsCB(
            Single upperTempLimit,
            Single lowerTempLimit,
            Int32 len,
            short recordPeriod,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] int[] dateTime,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] Byte[] logMode,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] Single[] temperatures);

        [DllImport("VarioSens Lib.dll")]
        private static extern int getVarioSensLog(writeViolationsCB cb);

        [DllImport("VarioSens Lib.dll")]
        private static extern int setVarioSensSettings(float lowTemp, float hiTemp, int interval, int mode, int batteryCheckInterval);

        public delegate void writeVSSettingsCB(
            Single upperTempLimit,
            Single lowerTempLimit,
            short recordPeriod,
            short logMode,
            short batteryCheckInterval);

        [DllImport("VarioSens Lib.dll")]
        private static extern int getVarioSensSettings(writeVSSettingsCB cb);

        public delegate void writeViolationsDelegate(
            Single upperTempLimit,
            Single lowerTempLimit,
            Int32 len,
            short recordPeriod,
            int[] dateTime,
            Byte[] logMode,
            Single[] temperatures);

        private bool readerRunning;

        public bool running
        {
            get { return readerRunning; }
            set { readerRunning = value; }
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        public void readLog(writeViolationsCB mycb)
        {
            //writeViolationsCB mycb = new writeViolationsCB(writeViolations);
            readerRunning = true;
            int errorCode = 0;
            while (readerRunning == true)
            {
                if ((errorCode = getVarioSensLog(mycb)) == 0)
                    readerRunning = false;
                else if ((errorCode == -1) || (errorCode == -2) || (errorCode == -6))
                    readerRunning = false;
                Thread.Sleep(100);
            }

            switch (errorCode)
            {
                case -1:
                    MessageBox.Show(GentagDemo.Properties.Resources.error1);
                    break;
                case -2:
                    MessageBox.Show(GentagDemo.Properties.Resources.error2);
                    break;                
                default:
                    break;
            }

            // TODO: go do web service stuff

            //resetButtons(readLogButton);
        }

        private static DateTime origin = System.TimeZone.CurrentTimeZone.ToLocalTime(new DateTime(1970, 1, 1, 0, 0, 0));

        public int setVSSettings(int mode, float hiTemp, float loTemp, int interval, int batteryCheckInterval)
        {
            int errorCode = -1;
            try
            {
                errorCode = -1;

                while ((readerRunning == true) &&
                    ((errorCode = setVarioSensSettings(loTemp,hiTemp,interval,mode,batteryCheckInterval)) != 0))
                    Thread.Sleep(100);
            }
            catch (ArgumentException ex)
            {
                MessageBox.Show("Error parsing: " + ex.ToString());
            }
            catch (FormatException ex)
            {
                MessageBox.Show("A setting is not in a valid format: " + ex.ToString());
            }
            catch (OverflowException ex)
            {
                MessageBox.Show("A value is too large or small: " + ex.ToString());
            }

            readerRunning = false;
            return errorCode;
        }

        // callbacks must be static
        private static void getVSSettingsCB(Single upper, Single lower, short period, short logMode, short batteryCheckInterval)
        {
            // yet static methods must have a way to access existing instances
            existingNativeMethods.VarioSensSettingsReceived(upper, lower, period, logMode, batteryCheckInterval);
        }

        public void getVSSettings()
        {
            writeVSSettingsCB writeVSCB = new writeVSSettingsCB(getVSSettingsCB);

            int errorCode = -1;

            while ((readerRunning == true) && ((errorCode = getVarioSensSettings(writeVSCB)) != 0))
            {
                Thread.Sleep(100);
            }

            if (errorCode != 0)
                VarioSensSettingsReceived(0, 0, 0, 0, -1);

            readerRunning = false;
        }

        public void readTagID()
        {
            if (C1Lib.C1.NET_C1_open_comm() != 1)
            {
                ReaderError(GentagDemo.Properties.Resources.error1);
                return;
            }
            else if (C1Lib.C1.NET_C1_enable() != 1)
            {
                C1Lib.C1.NET_C1_disable();
                ReaderError(GentagDemo.Properties.Resources.error2);
                return;
            }
            string oldTag = "";

            readerRunning = true;

            while (readerRunning)
            {
                // wait while a tag is read
                while ((readerRunning == true) && (C1Lib.ISO_15693.NET_get_15693(0x00) == 0)) { Thread.Sleep(20); }

                if (readerRunning == false)
                    break;

                //while (C1Lib.ISO_15693.NET_read_multi_15693(0x00, C1Lib.ISO_15693.tag.blocks) != 1) { }


                //string rfidDescr = C1Lib.util.to_str(C1Lib.ISO_15693.tag.read_buff, 256);
                //rfidDescr += "\n";

                StringBuilder newTagBuilder = new StringBuilder(C1Lib.ISO_15693.tag.id_length);

                for (int i = 0; i < C1Lib.ISO_15693.tag.id_length; i++)
                    newTagBuilder.Append(C1Lib.util.hex_value(C1Lib.ISO_15693.tag.tag_id[i]));

                string newTag = newTagBuilder.ToString();

                if ((string.Compare(newTag, oldTag) != 0) && (newTag.Length == 16))
                {
                    TagReceived(newTag.ToString());
                    oldTag = newTag;
                }
                
            }
            C1Lib.C1.NET_C1_disable();
            C1Lib.C1.NET_C1_close_comm();
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        public static string readOneTagID()
        {
            if (C1Lib.C1.NET_C1_open_comm() != 1)
            {
                throw new NotSupportedException(GentagDemo.Properties.Resources.error1);
            }
            else if (C1Lib.C1.NET_C1_enable() != 1)
            {
                C1Lib.C1.NET_C1_disable();
                throw new NotSupportedException(GentagDemo.Properties.Resources.error2);
            }

            // wait while a tag is read
            while (C1Lib.ISO_15693.NET_get_15693(0x00) == 0) { Thread.Sleep(20); }

            StringBuilder newTag = new StringBuilder(C1Lib.ISO_15693.tag.id_length);

            for (int i = 0; i < C1Lib.ISO_15693.tag.id_length; i++)
                newTag.Append(C1Lib.util.hex_value(C1Lib.ISO_15693.tag.tag_id[i]));

            C1Lib.C1.NET_C1_disable();
            C1Lib.C1.NET_C1_close_comm();
            return newTag.ToString();
        }
    }
}
