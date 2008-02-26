#define WDREADER
using System;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.IO;
using System.Diagnostics;
using System.Windows.Forms;

[assembly: CLSCompliant(true)]
namespace RFIDReader
{
    // tag types
    public enum tagTypes { none, iso15693, iso14443a, iso14443b, iso14443bsri, iso14443bsr176, iso18000, felica, MiFareClassic, MiFareUltraLight, MiFareDESFire, INSIDE }

    public enum readType { none, readOne, readMany, readVarioSensLog, detectMany, readTagMemory, setVarioSensSettings, getVarioSensSettings, radiationScan, writeTagMemory }

    #region EventArgs
    public class TagReceivedEventArgs : EventArgs
    {
        private string tagID;
        private tagTypes type;

        public TagReceivedEventArgs(string _tag, tagTypes _type)
        {
            tagID = _tag;
            type = _type;
        }

        public string Tag
        {
            get
            {
                return tagID;
            }
        }

        public tagTypes Type
        {
            get
            {
                return type;
            }
        }
    }

    public class VarioSensSettingsEventArgs : EventArgs
    {
        private Single highLimit;
        private Single lowLimit;
        private short period;
        private short logMode;
        private short batteryCheckInterval;

        public VarioSensSettingsEventArgs(Single _high, Single _low, short _period, short _logMode, short _batteryCheckInterval)
        {
            highLimit = _high;
            lowLimit = _low;
            period = _period;
            logMode = _logMode;
            batteryCheckInterval = _batteryCheckInterval;
        }

        public Single HighLimit
        { get { return highLimit; } }
        public Single LowLimit
        { get { return lowLimit; } }
        public short Period
        { get { return period; } }
        public short LogMode
        { get { return logMode; } }
        public short BatteryCheckInterval
        { get { return batteryCheckInterval; } }
    }

    public class TagContentsEventArgs : EventArgs
    {
        private string contents;
        public TagContentsEventArgs(string value)
        { contents = value; }

        public string Contents
        { get { return contents; } }
    }

    public class FinishedWritingStatusEventArgs : EventArgs
    {
        private string status;
        public FinishedWritingStatusEventArgs(string value)
        { status = value; }
        public string Status
        { get { return status; } }
    }

    public class TagTypeEventArgs : EventArgs
    {
        private tagTypes type;
        private string tagID;
        public TagTypeEventArgs(tagTypes value, string ID)
        { type = value; tagID = ID; }

        public tagTypes Type
        { get { return type; } }

        public string TagID
        { get { return tagID; } }
    }

    public class VarioSensLogEventArgs : EventArgs
    {
        private Single upperLimit;
        private Single lowerLimit;
        private Int32 length;
        private short recordPeriod;
        private int[] recordDates;
        private Byte[] logModes;
        private Single[] temperatures;
        public VarioSensLogEventArgs(Single ul, Single ll, Int32 len, short rp, int[] rd, Byte[] lm, Single[] t)
        {
            upperLimit = ul;
            lowerLimit = ll;
            length = len;
            recordPeriod = rp;
            recordDates = rd;
            logModes = lm;
            temperatures = t;
        }

        public Single UpperLimit
        { get { return upperLimit; } }
        public Single LowerLimit
        { get { return lowerLimit; } }
        public Int32 Length
        { get { return length; } }
        public short RecordPeriod
        { get { return recordPeriod; } }
        public int[] RecordDate
        { get { return recordDates; } }
        public Byte[] LogMode
        { get { return logModes; } }
        public Single[] Temperature
        { get { return temperatures; } }
    }
    #endregion

    #region DLL Imports
    internal class NativeMethods
    {
        public delegate void VarioSensReadLogCallback(
           Single upperTempLimit,
           Single lowerTempLimit,
           Int32 len,
           short recordPeriod,
           [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] int[] dateTime,
           [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] Byte[] logMode,
           [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] Single[] temperatures);

        public delegate void VarioSensSettingsCallback(
            Single upperTempLimit,
            Single lowerTempLimit,
            short recordPeriod,
            short logMode,
            short batteryCheckInterval);

        [DllImport("coredll.dll")]
        public extern static int GetDeviceUniqueID(
            [In, Out] byte[] appdata,
            int cbApplictionData,
            int dwDeviceIDVersion,
            [In, Out] byte[] deviceIDOuput,
            out uint pcbDeviceIDOutput);

        [DllImport("VarioSens Lib.dll")]
        public static extern int getVarioSensLog(VarioSensReadLogCallback cb);

        [DllImport("VarioSens Lib.dll")]
        public static extern int setVarioSensSettings(float lowTemp, float hiTemp, int interval, int mode, int batteryCheckInterval);

        [DllImport("VarioSens Lib.dll")]
        public static extern IntPtr getVarioSensTagID();

        [DllImport("VarioSens Lib.dll")]
        public static extern int getVarioSensSettings(VarioSensSettingsCallback cb);
    }
    #endregion

    public class Reader
    {
        #region Events
        public event EventHandler<TagReceivedEventArgs> TagReceived;
        public event EventHandler<VarioSensSettingsEventArgs> VarioSensSettingsReceived;
        public event EventHandler<VarioSensLogEventArgs> VarioSensLogReceived;
        public event EventHandler<TagContentsEventArgs> ReturnTagContents;
        public event EventHandler<FinishedWritingStatusEventArgs> DoneWriting;
        public event EventHandler<TagTypeEventArgs> TagTypeDetected;
        #endregion

        public Reader()
        {
            if (existingNativeMethods == null)
                existingNativeMethods = this;
            else
                throw new ArgumentException(Properties.Resources.TooManyNativeMethodsClassesCreated);

            timerAutoEvent = new AutoResetEvent(false);

            timerDelegate = new TimerCallback(siritReadTick);
        }

        /// <summary>
        /// The previous tag value, helps prevent sending the same tag twice in a row since a 
        /// single scan may create dozens of reads of a single tag
        /// </summary>
        private byte[] oldTag = new byte[8];

        /// <summary>
        /// The timer thread that does tag reads
        /// </summary>
        private System.Threading.Timer readTimer = null;

        private AutoResetEvent timerAutoEvent;

        private TimerCallback timerDelegate;

        private static Reader existingNativeMethods;

        private readType readThreadType = readType.none;

        private Thread readerThread = null;

        public readType ReadThreadType
        {
            get { return readThreadType; }
            set
            {
                readThreadType = value;

                switch (value)
                {
                    case readType.none:
                        break;
                    case readType.readOne:
#if (WDREADER)
                        timerDelegate = new TimerCallback(wdReadTick);
#else
                        timerDelegate = new TimerCallback(siritReadTick);
#endif                        
                        break;
                    case readType.readMany:                        
#if (WDREADER)
                        timerDelegate = new TimerCallback(wdReadTick);
#else
                        timerDelegate = new TimerCallback(siritReadTick); 
#endif                        
                        break;
                    case readType.detectMany:
#if (WDREADER)
                        timerDelegate = new TimerCallback(wdReadTick);
#else
                        timerDelegate = new TimerCallback(siritReadTick);
#endif                        
                        break;
                    case readType.getVarioSensSettings:
                        readerThread = new Thread(getVSSettings);
                        break;
                    case readType.setVarioSensSettings:
                        readerThread = new Thread(launchSetVarioSensSettings);
                        break;
                    case readType.radiationScan:
                        readerThread = new Thread(radiationScan);
                        break;
                    case readType.readVarioSensLog:
                        readerThread = new Thread(readLog);
                        break;
                    case readType.readTagMemory:
                        readerThread = new Thread(readTag);
                        break;
                    case readType.writeTagMemory:
                        readerThread = new Thread(launchWriteTagMemory);
                        break;
                    default:
                        break;
                }
                //readerThread.IsBackground = true;
                //readerThread.Priority = ThreadPriority.Normal;
            }
        }

        public delegate void writeViolationsDelegate(
            Single upperTempLimit,
            Single lowerTempLimit,
            Int32 len,
            short recordPeriod,
            int[] dateTime,
            Byte[] logMode,
            Single[] temperatures);

        private bool readerRunning;

        public bool Running
        {
            get { return readTimer != null; }
            set
            {

                if (value)
                {
#if (!WDREADER)
                    string errorMessage = "";
                    int n = retryCount;

                    for (; n > 0; --n)
                    {
                        if (C1Lib.C1.NET_C1_open_comm() != 1)
                        {
                            C1Lib.C1.NET_C1_close_comm();
                            errorMessage = Properties.Resources.PleaseEnsureThatTheSiritReaderIsCompletelyInserted;
                        }
                        else if (C1Lib.C1.NET_C1_enable() != 1)
                        {
                            C1Lib.C1.NET_C1_close_comm();
                            errorMessage = Properties.Resources.UnableToCommunicateWithSiritReader;
                        }
                        else // connection was successful
                        {
                            break;
                        }
                        Thread.Sleep(10);
                    }

                    if (n == 0)
                    {
                        throw new IOException(errorMessage);
                        //readerRunning = false;
                        
                    }
                    else
#endif
                    {
                        // restart the thread if it somehow already exists
                        if (readTimer != null)
                        {
                            Debug.WriteLine("restarting read thread");
                            readerRunning = false;
                            if (!timerAutoEvent.WaitOne(5000, false))
                                Debug.WriteLine("didn't make it");
                            readTimer.Dispose();
                            readTimer = null;
                        }
                        readerRunning = true;
                        readTimer = new System.Threading.Timer(timerDelegate, timerAutoEvent, 50, 50);
                    }
                }
                else
                {

                    if (readTimer != null)
                    {
                        readerRunning = false;
                        if (!timerAutoEvent.WaitOne(3500, false))
                        {
                            Debug.WriteLine("didn't make it");
                            MessageBox.Show("killing reader thread");
                        }
                        lock (this)
                        {                      
                        readTimer.Dispose();
                        readTimer = null;
                        for (int i = 0; i < oldTag.Length; i++)
                            oldTag[i] = 0x00;
#if (!WDREADER)
                        //readTimer.Enabled = false;
                            C1Lib.C1.NET_C1_disable();
                            C1Lib.C1.NET_C1_close_comm();
                        
#endif
                        }
                    }
                }

            }
        }

        private void siritReadTick(object stateInfo)
        {
            if (Monitor.TryEnter(this))
            {
                try
                {
                    //Thread.Sleep(20);
                    C1Lib.C1.NET_C1_disable();
                    //Thread.Sleep(40);
                    C1Lib.C1.NET_C1_enable();

                    if (C1Lib.ISO_15693.NET_get_15693(0x00) != 0)
                    {
                        bool match = true;
                        for (int i = 0; i < C1Lib.ISO_15693.tag.id_length; i++)
                        {
                            if (oldTag[i] != C1Lib.ISO_15693.tag.tag_id[i])
                                match = false;
                            oldTag[i] = C1Lib.ISO_15693.tag.tag_id[i];
                            newTagBuilder.Append(C1Lib.ISO_15693.tag.tag_id[i].ToString("X", CultureInfo.InvariantCulture).PadLeft(2, '0'));
                        }

                        if (!match)
                        {
                            // when asynchronous delegates are supported in CF
                            //TagReceived.BeginInvoke(newTag.ToString(), !repeat, null, null);                       
                            TagReceived(this, new TagReceivedEventArgs(newTagBuilder.ToString(),tagTypes.iso15693));
                            
                            // disable and enable
                            //C1Lib.C1.NET_C1_disable();
                            //C1Lib.C1.NET_C1_enable();

                            if ((readThreadType != readType.readMany) && (readThreadType != readType.detectMany))
                            {
                                readTimer.Dispose();
                                readTimer = null;
                            }
                        }
                        newTagBuilder.Remove(0, newTagBuilder.Length);
                    }

                    if (!readerRunning)
                        timerAutoEvent.Set();
                }
                finally
                {
                    Monitor.Exit(this);
                }                
            }
        }

        enum ERR
        {
            NONE = 0,
            CARD_AVAILABLE = 1,
            EJECTED = 150102,
        }

        private bool shouldReinit = true;

        private SDiD1020.ISO15693.ISO15693Card card = new SDiD1020.ISO15693.ISO15693Card();

        private SDiD1020.Utility.WDIUtility wdiUtil = new SDiD1020.Utility.WDIUtility();

        private const int ICODE_SLI_UID_SIZE = 8;
        private const int ICODE_SLI_BLOCK_SIZE = 4;
        private const int ICODE_SLI_NUMBER_OF_BLOCKS = 16;
        byte[] uid = new byte[ICODE_SLI_UID_SIZE];

        private void wdReadTick(object stateInfo)
        {
            if (Monitor.TryEnter(this))
            {
                try
                {
                    if (shouldReinit)
                    {
                        wdiUtil.ChangeProtocol(2, 1);
                        shouldReinit = false;
                    }

                    ERR ec = (ERR)card.IsCardAvailable(true);
                    

                    switch (ec)
                    {
                        case ERR.NONE:
                            //No card available do nothing

                            break;

                        case ERR.CARD_AVAILABLE:
                            //Card available
                            
                            //card.GetUID(uid);
                            for (int n = 0; n < card.numUID; n++)
                            {
                                uid = card.GetCardUID(n);
                                for (int i = 0; i < uid.Length; i++)
                                    newTagBuilder.Append(uid[i].ToString("X", CultureInfo.InvariantCulture).PadLeft(2, '0'));

                                string newTag = newTagBuilder.ToString();

                                newTagBuilder.Remove(0, newTagBuilder.Length);
                                TagReceived(this, new TagReceivedEventArgs(newTag.ToString(), tagTypes.iso15693));
                            }
                            if ((readThreadType != readType.readMany) && (readThreadType != readType.detectMany))
                            {
                                readTimer.Dispose();
                                readTimer = null;
                            }
                            break;

                        case ERR.EJECTED:
                            //SDiD in not inserted
                            //throw new IOException(Properties.Resources.PleaseEnsureThatTheSiritReaderIsCompletelyInserted);
                            Debug.WriteLine("Card not inserted");
                            break;

                        default:
                            //Some other error
                            //Ignore for now
                            break;
                    }



                    if (!readerRunning)
                        timerAutoEvent.Set();
                }
                finally
                {
                    Monitor.Exit(this);
                }
            }
        }

        private StringBuilder newTagBuilder = new StringBuilder(16);

        private const int retryCount = 1;

        public void readTagID()
        {
            readTagLoop(true);
        }

        public void readOneTagID()
        {
            readTagLoop(false);
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void readTagLoop(bool repeat)
        {
            string errorMessage = "";
            int n = retryCount;

            for (; n > 0; --n)
            {
                if (C1Lib.C1.NET_C1_open_comm() != 1)
                {
                    C1Lib.C1.NET_C1_close_comm();
                    errorMessage = Properties.Resources.PleaseEnsureThatTheSiritReaderIsCompletelyInserted;
                }
                else if (C1Lib.C1.NET_C1_enable() != 1)
                {
                    C1Lib.C1.NET_C1_close_comm();
                    errorMessage = Properties.Resources.UnableToCommunicateWithSiritReader;
                }
                else // connection was successful
                {
                    break;
                }
                Thread.Sleep(10);
            }

            if (n == 0)
            {
                //C1Lib.C1.NET_C1_disable();
                //C1Lib.C1.NET_C1_close_comm();
                //ReaderError(this, new ReaderErrorEventArgs(errorMessage));
                readerRunning = false;
                return;
            }

            string oldTag = "";

            readerRunning = true;

            while (readerRunning)
            {
                //Random rnd = new Random();
                // wait while a tag is read
                if (C1Lib.ISO_15693.NET_get_15693(0x00) != 0)
                {
                    for (int i = 0; i < C1Lib.ISO_15693.tag.id_length; i++)
                        newTagBuilder.Append(C1Lib.ISO_15693.tag.tag_id[i].ToString("X", CultureInfo.InvariantCulture).PadLeft(2, '0'));

                    string newTag = newTagBuilder.ToString();

                    newTagBuilder.Remove(0, newTagBuilder.Length);

                    if ((string.Compare(newTag, oldTag, StringComparison.InvariantCulture) != 0) && (newTag.Length == 16))
                    {
                        // when asynchronous delegates are supported in CF
                        //TagReceived.BeginInvoke(newTag.ToString(), !repeat, null, null);
                        TagReceived(this, new TagReceivedEventArgs(newTag.ToString(), tagTypes.iso15693));
                        oldTag = newTag;
                    }

                    // disable and enable
                    C1Lib.C1.NET_C1_disable();
                    C1Lib.C1.NET_C1_enable();

                    if (!repeat)
                    {
                        readerRunning = false;
                        break;
                    }
                }
                //// else // randomly generate a tag
                // {
                //    // Thread.Sleep(40);
                //     //C1Lib.C1.NET_C1_disable();
                //     //C1Lib.C1.NET_C1_enable();
                //     //////////////////////////////////////////////////////////////////////////                    
                //     // for random tag generation
                //     //////////////////////////////////////////////////////////////////////////                    
                //     string basic = "";
                //     byte[] newTag0 = new byte[8];
                //     rnd.NextBytes(newTag0);

                //     for (int i = 0; i < 8; i++)
                //     {
                //         newTagBuilder.Append(newTag0[i].ToString("X").PadLeft(2, '0'));
                //     }

                //     TagReceived(this, new TagReceivedEventArgs(newTagBuilder.ToString(), false));
                //     newTagBuilder.Remove(0, newTagBuilder.Length);
                //     //////////////////////////////////////////////////////////////////////////
                // }



                Thread.Sleep(10);

            }
            C1Lib.C1.NET_C1_disable();
            C1Lib.C1.NET_C1_close_comm();
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        unsafe public static string readOneVSTagID()
        {
            return new string((char*)NativeMethods.getVarioSensTagID());
        }

        private void VarioSensReadLog(Single upperTempLimit, Single lowerTempLimit, Int32 len, short recordPeriod, int[] dateTime, Byte[] logMode, Single[] temperatures)
        {
            VarioSensLogReceived(this, new VarioSensLogEventArgs(upperTempLimit, lowerTempLimit, len, recordPeriod, dateTime, logMode, temperatures));
        }

        private void radiationScan()
        {
        }

        [MethodImpl(MethodImplOptions.Synchronized)]
        public void readLog()
        {
            NativeMethods.VarioSensReadLogCallback mycb = new NativeMethods.VarioSensReadLogCallback(VarioSensReadLog);
            readerRunning = true;
            int errorCode = 0;
            while (readerRunning == true)
            {
                if ((errorCode = NativeMethods.getVarioSensLog(mycb)) == 0)
                    readerRunning = false;
                else if ((errorCode == -1) || (errorCode == -2) || (errorCode == -6))
                    readerRunning = false;
                Thread.Sleep(100);
            }

            switch (errorCode)
            {
                case -1:
                    //ReaderError(this, new ReaderErrorEventArgs(Properties.Resources.PleaseEnsureThatTheSiritReaderIsCompletelyInserted));
                    break;
                case -2:
                    //ReaderError(this, new ReaderErrorEventArgs(Properties.Resources.UnableToCommunicateWithSiritReader));
                    break;
                default:
                    break;
            }
        }

        public static string getDeviceUniqueID(string AppString)
        {
            byte[] AppData = new byte[AppString.Length];

            for (int count = 0; count < AppString.Length; count++)
                AppData[count] = (byte)AppString[count];

            int appDataSize = AppData.Length;

            byte[] dUID = new byte[20];

            uint SizeOut = 20;

            NativeMethods.GetDeviceUniqueID(AppData, appDataSize, 1, dUID, out SizeOut);

            string DeviceUID = "";

            for (int i = 0; i < 20; i++)
            {
                DeviceUID += dUID[i].ToString("X", CultureInfo.InvariantCulture);
            }

            return DeviceUID;
        }

        private bool tagDataSet = false;

        private byte[] tagData;

        // because there are no ParameterizedThreads in .NET CF
        public void launchWriteTagMemory()
        {
            writeTag(tagData);
            tagDataSet = false;
        }

        private bool VSsettingsSet = false;

        private int VSmode;
        private float VShighTemp;
        private float VSlowTemp;
        private int VSinterval;
        private int VSbatteryCheckInt;

        // because there are no ParameterizedThreads in .NET CF
        public void launchSetVarioSensSettings()
        {
            setVSSettings(VSmode, VShighTemp, VSlowTemp, VSinterval, VSbatteryCheckInt);
            VSsettingsSet = false;
        }


        public int setVSSettings(int mode, float hiTemp, float loTemp, int interval, int batteryCheckInterval)
        {
            int errorCode = -1;
            readerRunning = true;
            try
            {
                errorCode = -1;

                while ((readerRunning == true) &&
                    ((errorCode = NativeMethods.setVarioSensSettings(loTemp, hiTemp, interval, mode, batteryCheckInterval)) != 0))
                    Thread.Sleep(100);
            }
            catch (ArgumentException ex)
            {
                //ReaderError(this, new ReaderErrorEventArgs(Properties.Resources.ErrorParsing + ex.ToString()));
            }
            catch (FormatException ex)
            {
                //ReaderError(this, new ReaderErrorEventArgs(Properties.Resources.ASettingIsNotInAValidFormat + ex.ToString()));
            }
            catch (OverflowException ex)
            {
                //ReaderError(this, new ReaderErrorEventArgs(Properties.Resources.AValueIsTooLargeOrSmall + ex.ToString()));
            }

            readerRunning = false;
            return errorCode;
        }

        // callbacks must be static
        private static void getVSSettingsCB(Single upper, Single lower, short period, short logMode, short batteryCheckInterval)
        {
            // yet static methods must have a way to access existing instances
            existingNativeMethods.VarioSensSettingsReceived(null, new VarioSensSettingsEventArgs(upper, lower, period, logMode, batteryCheckInterval));
        }

        public void getVSSettings()
        {
            NativeMethods.VarioSensSettingsCallback writeVSCB = new NativeMethods.VarioSensSettingsCallback(getVSSettingsCB);

            int errorCode = -1;

            while ((readerRunning == true) && ((errorCode = NativeMethods.getVarioSensSettings(writeVSCB)) != 0))
            {
                Thread.Sleep(100);
            }

            if (errorCode != 0)
                //ReaderError(this, new ReaderErrorEventArgs(Properties.Resources.ErrorReadingVarioSensTag + errorCode));

                readerRunning = false;
        }

        public static void initialize()
        {
            if (C1Lib.C1.NET_C1_open_comm() == 1)
            {
                C1Lib.C1.NET_C1_close_comm();
            }
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
                    errorMessage = Properties.Resources.PleaseEnsureThatTheSiritReaderIsCompletelyInserted;
                    Thread.Sleep(15);
                    continue;
                }
                else if (C1Lib.C1.NET_C1_enable() != 1)
                {
                    C1Lib.C1.NET_C1_disable();
                    errorMessage = Properties.Resources.UnableToCommunicateWithSiritReader;
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
                //ReaderError(this, new ReaderErrorEventArgs(errorMessage));
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
                    DoneWriting(this, new FinishedWritingStatusEventArgs(Properties.Resources.Success));
                }

            }
            C1Lib.C1.NET_C1_disable();
            C1Lib.C1.NET_C1_close_comm();
        }

        public void readTag()
        {
            string errorMessage = "";
            int n = retryCount;

            for (; n > 0; --n)
            {
                if (C1Lib.C1.NET_C1_open_comm() != 1)
                {
                    errorMessage = Properties.Resources.PleaseEnsureThatTheSiritReaderIsCompletelyInserted;
                    Thread.Sleep(15);
                    continue;
                }
                else if (C1Lib.C1.NET_C1_enable() != 1)
                {
                    //C1Lib.C1.NET_C1_disable();
                    errorMessage = Properties.Resources.UnableToCommunicateWithSiritReader;
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
                //ReaderError(this, new ReaderErrorEventArgs(errorMessage));
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
                    ReturnTagContents(this, new TagContentsEventArgs(input));

                }

            }
            C1Lib.C1.NET_C1_disable();
            C1Lib.C1.NET_C1_close_comm();
        }


        public void detectTag()
        {
            string errorMessage = "";
            int n = retryCount;

            for (; n > 0; --n)
            {
                if (C1Lib.C1.NET_C1_open_comm() != 1)
                {
                    C1Lib.C1.NET_C1_close_comm();
                    errorMessage = Properties.Resources.PleaseEnsureThatTheSiritReaderIsCompletelyInserted;
                }
                else if (C1Lib.C1.NET_C1_enable() != 1)
                {
                    C1Lib.C1.NET_C1_close_comm();
                    errorMessage = Properties.Resources.UnableToCommunicateWithSiritReader;
                }
                else // connection was successful
                {
                    break;
                }
                Thread.Sleep(10);
            }

            if (n == 0)
            {
                //ReaderError(this, new ReaderErrorEventArgs(errorMessage));
                readerRunning = false;
                return;
            }

            readerRunning = true;


            while (readerRunning)
            {
                // disable and enable
                C1Lib.C1.NET_C1_disable();
                C1Lib.C1.NET_C1_enable();

                if (C1Lib.ISO_14443A.NET_get_14443A() == 1)
                {
                    for (int i = 0; i < 7; ++i)
                        newTagBuilder.Append(C1Lib.ISO_14443A.tag.tag_id[i].ToString("X", CultureInfo.InvariantCulture).PadLeft(2, '0'));

                    if (C1Lib.ISO_14443A.tag.type == C1Lib.ISO_14443A.MIFARE_CL)
                        TagTypeDetected(this, new TagTypeEventArgs(tagTypes.MiFareClassic, newTagBuilder.ToString()));
                    else if (C1Lib.ISO_14443A.tag.type == C1Lib.ISO_14443A.MIFARE_UL)
                        TagTypeDetected(this, new TagTypeEventArgs(tagTypes.MiFareUltraLight, newTagBuilder.ToString()));
                    else if (C1Lib.ISO_14443A.tag.type == C1Lib.ISO_14443A.MIFARE_DF)
                        TagTypeDetected(this, new TagTypeEventArgs(tagTypes.MiFareDESFire, newTagBuilder.ToString()));
                    else
                        TagTypeDetected(this, new TagTypeEventArgs(tagTypes.iso14443a, newTagBuilder.ToString()));
                    newTagBuilder.Remove(0, newTagBuilder.Length);
                }
                else if (C1Lib.ISO_15693.NET_get_15693(0x00) == 1)
                {
                    for (int i = 0; i < C1Lib.ISO_15693.tag.id_length; i++)
                        newTagBuilder.Append(C1Lib.ISO_15693.tag.tag_id[i].ToString("X", CultureInfo.InvariantCulture).PadLeft(2, '0'));

                    TagTypeDetected(this, new TagTypeEventArgs(tagTypes.iso15693, newTagBuilder.ToString()));

                    newTagBuilder.Remove(0, newTagBuilder.Length);
                }
                else if (C1Lib.ISO_18000.NET_get_18000() == 1)
                {
                    for (int i = 0; i < C1Lib.ISO_18000.tag.data_length; i++)
                        newTagBuilder.Append(C1Lib.ISO_18000.tag.data[i].ToString("X", CultureInfo.InvariantCulture).PadLeft(2, '0'));

                    TagTypeDetected(this, new TagTypeEventArgs(tagTypes.iso18000, newTagBuilder.ToString()));

                    newTagBuilder.Remove(0, newTagBuilder.Length);
                }
                else if (C1Lib.IC.NET_get_IC(C1Lib.IC.PROTO_15693, false, false) == 1)
                {
                    for (int i = 0; i < 8; ++i)
                        newTagBuilder.Append(C1Lib.IC.tag.tag_id[i].ToString("X", CultureInfo.InvariantCulture).PadLeft(2, '0'));

                    TagTypeDetected(this, new TagTypeEventArgs(tagTypes.INSIDE, newTagBuilder.ToString()));

                    newTagBuilder.Remove(0, newTagBuilder.Length);
                }
                else if (C1Lib.IC.NET_get_IC(C1Lib.IC.PROTO_14443B, false, false) == 1)
                {
                    for (int i = 0; i < C1Lib.ISO_14443B.tag.id_length; ++i)
                        newTagBuilder.Append(C1Lib.IC.tag.tag_id[i].ToString("X", CultureInfo.InvariantCulture).PadLeft(2, '0'));

                    TagTypeDetected(this, new TagTypeEventArgs(tagTypes.INSIDE, newTagBuilder.ToString()));

                    newTagBuilder.Remove(0, newTagBuilder.Length);
                }
                else if (C1Lib.ISO_14443B.NET_get_14443B() == 1)
                {
                    for (int i = 0; i < C1Lib.ISO_14443B.tag.id_length; ++i)
                        newTagBuilder.Append(C1Lib.ISO_14443B.tag.tag_id[i].ToString("X", CultureInfo.InvariantCulture).PadLeft(2, '0'));

                    TagTypeDetected(this, new TagTypeEventArgs(tagTypes.iso14443b, newTagBuilder.ToString()));

                    newTagBuilder.Remove(0, newTagBuilder.Length);
                }
                else if (C1Lib.ISO_14443B.NET_get_14443B_SRI() == 1)
                {
                    for (int i = 0; i < C1Lib.ISO_14443B.tag.id_length; ++i)
                        newTagBuilder.Append(C1Lib.ISO_14443B.tag.tag_id[i].ToString("X", CultureInfo.InvariantCulture).PadLeft(2, '0'));

                    TagTypeDetected(this, new TagTypeEventArgs(tagTypes.iso14443bsri, newTagBuilder.ToString()));

                    newTagBuilder.Remove(0, newTagBuilder.Length);
                }
                else if (C1Lib.ISO_14443B.NET_get_14443B_SR176() == 1)
                {
                    for (int i = 0; i < C1Lib.ISO_14443B.tag.id_length; ++i)
                        newTagBuilder.Append(C1Lib.ISO_14443B.tag.tag_id[i].ToString("X", CultureInfo.InvariantCulture).PadLeft(2, '0'));

                    TagTypeDetected(this, new TagTypeEventArgs(tagTypes.iso14443bsr176, newTagBuilder.ToString()));

                    newTagBuilder.Remove(0, newTagBuilder.Length);
                }
                else if (C1Lib.FeliCa.NET_get_FeliCa() == 1)
                {
                    for (int i = 0; i < C1Lib.FeliCa.tag.id_length; ++i)
                        newTagBuilder.Append(C1Lib.FeliCa.tag.tag_id[i].ToString("X", CultureInfo.InvariantCulture).PadLeft(2, '0'));

                    TagTypeDetected(this, new TagTypeEventArgs(tagTypes.felica, newTagBuilder.ToString()));

                    newTagBuilder.Remove(0, newTagBuilder.Length);
                }
            }

            C1Lib.C1.NET_C1_disable();
            C1Lib.C1.NET_C1_close_comm();
        }
    }
}
