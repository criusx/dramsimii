using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Threading;

namespace RFIDReader
{
    public class Reader
    {
        #region Delegates
        public delegate void TagReceivedEventHandler(String tagID);
        public delegate void VarioSensSettingsReceivedEventHandler(Single hiLimit, Single loLimit, short period, short logMode, short batteryCheckInterval);
        public delegate void ReaderErrorHandler(string errorMessage);
        public delegate void VarioSensReadLogHandler(
           Single upperTempLimit,
           Single lowerTempLimit,
           Int32 len,
           short recordPeriod,
           [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] int[] dateTime,
           [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] Byte[] logMode,
           [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] Single[] temperatures);
        public delegate void ReturnTagContentsHandler(string contents);
        public delegate void DoneWritingTagContentsHandler(string status);
        #endregion

        #region Events
        public event TagReceivedEventHandler TagReceived;
        public event VarioSensSettingsReceivedEventHandler VarioSensSettingsReceived;
        public event VarioSensReadLogHandler VarioSensLogReceived;
        public event ReaderErrorHandler ReaderError;
        public event ReturnTagContentsHandler ReturnTagContents;
        public event DoneWritingTagContentsHandler DoneWriting;
        #endregion

        public Reader()
        {
            if (existingNativeMethods == null)
                existingNativeMethods = this;
            else
                throw new ArgumentException("Too many NativeMethods classes created");
        }

        private static Reader existingNativeMethods;

        #region DLL Imports

        [DllImport("coredll.dll")]
        public extern static int GetDeviceUniqueID(
            [In, Out] byte[] appdata,
            int cbApplictionData,
            int dwDeviceIDVersion,
            [In, Out] byte[] deviceIDOuput,
            out uint pcbDeviceIDOutput);

        [DllImport("VarioSens Lib.dll")]
        private static extern int getVarioSensLog(VarioSensReadLogHandler cb);

        [DllImport("VarioSens Lib.dll")]
        private static extern int setVarioSensSettings(float lowTemp, float hiTemp, int interval, int mode, int batteryCheckInterval);

        [DllImport("VarioSens Lib.dll")]
        private static extern IntPtr getVarioSensTagID();

        [DllImport("VarioSens Lib.dll")]
        private static extern int getVarioSensSettings(writeVSSettingsCB cb);

        #endregion
        public delegate void writeVSSettingsCB(
            Single upperTempLimit,
            Single lowerTempLimit,
            short recordPeriod,
            short logMode,
            short batteryCheckInterval);

        public delegate void writeViolationsDelegate(
            Single upperTempLimit,
            Single lowerTempLimit,
            Int32 len,
            short recordPeriod,
            int[] dateTime,
            Byte[] logMode,
            Single[] temperatures);

        private static bool readerRunning;

        public bool running
        {
            get { return readerRunning; }
            set { readerRunning = value; }
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        unsafe public static string readOneVSTagID()
        {
            return new string((char*)getVarioSensTagID());
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        public void readLog()
        {
            //writeViolationsCB mycb = new writeViolationsCB(writeViolations);
            readerRunning = true;
            int errorCode = 0;
            while (readerRunning == true)
            {
                if ((errorCode = getVarioSensLog(VarioSensLogReceived)) == 0)
                    readerRunning = false;
                else if ((errorCode == -1) || (errorCode == -2) || (errorCode == -6))
                    readerRunning = false;
                Thread.Sleep(100);
            }

            switch (errorCode)
            {
                case -1:
                    ReaderError("Please ensure that the Sirit reader is completely inserted");
                    break;
                case -2:
                    ReaderError("Unable to communicate with Sirit reader");
                    break;
                default:
                    break;
            }
        }

        private static DateTime origin = System.TimeZone.CurrentTimeZone.ToLocalTime(new DateTime(1970, 1, 1, 0, 0, 0));

        public int setVSSettings(int mode, float hiTemp, float loTemp, int interval, int batteryCheckInterval)
        {
            int errorCode = -1;
            readerRunning = true;
            try
            {
                errorCode = -1;

                while ((readerRunning == true) &&
                    ((errorCode = setVarioSensSettings(loTemp, hiTemp, interval, mode, batteryCheckInterval)) != 0))
                    Thread.Sleep(100);
            }
            catch (ArgumentException ex)
            {
                ReaderError("Error parsing: " + ex.ToString());
            }
            catch (FormatException ex)
            {
                ReaderError("A setting is not in a valid format: " + ex.ToString());
            }
            catch (OverflowException ex)
            {
                ReaderError("A value is too large or small: " + ex.ToString());
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

        public void initialize()
        {
            if (C1Lib.C1.NET_C1_open_comm() == 1)
            {
                C1Lib.C1.NET_C1_close_comm();
            }
        }

        StringBuilder newTagBuilder = new StringBuilder(16);

        static private int retryCount = 25;

        public void readTagID()
        {
            string errorMessage = "";
            int n = retryCount;

            for (; n > 0; --n)
            {
                Thread.Sleep(15);
                if (C1Lib.C1.NET_C1_open_comm() != 1)
                {
                    errorMessage = "Please ensure that the Sirit reader is completely inserted";
                    continue;
                }
                else if (C1Lib.C1.NET_C1_enable() != 1)
                {
                    C1Lib.C1.NET_C1_disable();
                    errorMessage = "Unable to communicate with Sirit reader";
                    continue;
                }
                else // connection was successful
                {
                    break;
                }                
            }

            if (n == 0)
            {
                ReaderError(errorMessage);
                return;
            }

            string oldTag = "";

            readerRunning = true;

            while (readerRunning)
            {
                Random rnd = new Random();
                // wait while a tag is read
                while ((readerRunning == true) && (C1Lib.ISO_15693.NET_get_15693(0x00) == 0))
                {
                    Thread.Sleep(20);
                    //////////////////////////////////////////////////////////////////////////                    
                    //string basic = "";
                    //byte[] newTag0 = new byte[8];
                    //rnd.NextBytes(newTag0);

                    //for (int i = 0; i < 8; i++)
                    //{                        
                    //    newTagBuilder.Append(newTag0[i].ToString("X").PadLeft(2,'0'));
                    //}

                    //TagReceived(newTagBuilder.ToString());
                    //newTagBuilder.Remove(0, newTagBuilder.Length);
                    //////////////////////////////////////////////////////////////////////////

                }

                if (readerRunning == false)
                    break;

                // this code will read the contents of the tag
                //while (C1Lib.ISO_15693.NET_read_multi_15693(0x00, C1Lib.ISO_15693.tag.blocks) != 1) { }


                //string rfidDescr = C1Lib.util.to_str(C1Lib.ISO_15693.tag.read_buff, 256);
                //rfidDescr += "\n";

                for (int i = 0; i < C1Lib.ISO_15693.tag.id_length; i++)
                    newTagBuilder.Append(C1Lib.ISO_15693.tag.tag_id[i].ToString("X").PadLeft(2, '0'));

                string newTag = newTagBuilder.ToString();

                newTagBuilder.Remove(0, newTagBuilder.Length);

                if ((string.Compare(newTag, oldTag) != 0) && (newTag.Length == 16))
                {
                    TagReceived(newTag.ToString());
                    oldTag = newTag;
                }

            }
            C1Lib.C1.NET_C1_disable();
            C1Lib.C1.NET_C1_close_comm();
        }

        public void writeTag(byte[] outputBuffer)
        {
            if (outputBuffer.Length < 2)
                return;

            string errorMessage = "";
            int n = retryCount;

            for (; n > 0; --n)
            {
                if (C1Lib.C1.NET_C1_open_comm() != 1)
                {
                    errorMessage = "Please ensure that the Sirit reader is completely inserted";
                    Thread.Sleep(15);
                    continue;
                }
                else if (C1Lib.C1.NET_C1_enable() != 1)
                {
                    C1Lib.C1.NET_C1_disable();
                    errorMessage = "Unable to communicate with Sirit reader";
                    Thread.Sleep(15);
                    continue;
                }
                else // connection was successful
                {
                    break;
                }                
            }

            if (n == 0)
            {
                ReaderError(errorMessage);
                return;
            }

            readerRunning = true;

            while (readerRunning)
            {
                // acquire the tag
                while ((readerRunning == true) && (C1Lib.ISO_15693.NET_get_15693(0x00) == 0))
                {
                    Thread.Sleep(20);
                }
                // if it left the look due to the user canceling, quit
                if (readerRunning == false)
                    break;
                // when the tag is successfully written, quit
                if (C1Lib.ISO_15693.NET_write_multi_15693(0, outputBuffer.Length, outputBuffer) == 1)
                {
                    readerRunning = false;
                    DoneWriting("Success");
                }

            }
            C1Lib.C1.NET_C1_disable();
            C1Lib.C1.NET_C1_close_comm();
        }

        System.Text.Encoding enc = System.Text.Encoding.Unicode;

        public void readTag()
        {
            string errorMessage = "";
            int n = retryCount;

            for (; n > 0; --n)
            {
                if (C1Lib.C1.NET_C1_open_comm() != 1)
                {
                    errorMessage = "Please ensure that the Sirit reader is completely inserted";
                    Thread.Sleep(15);
                    continue;
                }
                else if (C1Lib.C1.NET_C1_enable() != 1)
                {
                    C1Lib.C1.NET_C1_disable();
                    errorMessage = "Unable to communicate with Sirit reader";
                    Thread.Sleep(15);
                    continue;
                }
                else // connection was successful
                {
                    break;
                }               
            }

            if (n == 0)
            {
                ReaderError(errorMessage);
                return;
            }

            readerRunning = true;

            while (readerRunning)
            {
                // acquire the tag
                while ((readerRunning == true) && (C1Lib.ISO_15693.NET_get_15693(0x00) == 0))
                {
                    Thread.Sleep(20);
                }
                // if it left the look due to the user canceling, quit
                if (readerRunning == false)
                    break;
                // when the tag is successfully written, quit
                if (C1Lib.ISO_15693.NET_read_multi_15693(0, C1Lib.ISO_15693.tag.blocks) == 1)
                {
                    readerRunning = false;
                    string input = System.Text.Encoding.Unicode.GetString(C1Lib.ISO_15693.tag.read_buff, 0, C1Lib.ISO_15693.tag.read_buff.Length);
                    ReturnTagContents(input);

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
                throw new NotSupportedException("Please ensure that the Sirit reader is completely inserted");
            }
            else if (C1Lib.C1.NET_C1_enable() != 1)
            {
                C1Lib.C1.NET_C1_disable();
                throw new NotSupportedException("Unable to communicate with Sirit reader");
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
