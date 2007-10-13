using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Text;
using System.Windows.Forms;
using System.IO;

using Microsoft.VisualBasic.CompilerServices; // for String to int conv
//using Microsoft.VisualBasic.ApplicationServices; // for startup Splash Form
using GraphicsServer.GSNet.Charting; // for GSC
using GraphicsServer.GSNet.SeriesData;
using System.Threading;
using System.Runtime.InteropServices;

using FT_HANDLE = System.UInt32;
using MP_HANDLE = System.UInt32;
using FT_STATUS = System.UInt32;
using MP_STATUS = System.UInt32;


namespace iSpectrum
{
    public partial class Form1 : Form
    {
        private Series activespectrum = new Series();
        private Series staticspectrum = new Series();
        private DateTime startTime;
        private DateTime endTime;
        private TimeSpan elapsedTime;
        private Boolean updateSpectrum;
        private ulong prevTotal1;
        private ulong prevTotal2;
        private ulong prevTotal3;
        private ulong prevTotal4;
        private int currentHVSetting;
        private int targetHVSetting;
        private int xAxisMin = 0;
        private int xAxisMax = 4095;
        public double fwhm = 0;         //for updating the FWHM form when changing X axis units (channels/KeV)
        public int chl = 0;
        public int chr = 0;
        public Boolean ROI = false;

        public Form1()
        {
            InitializeComponent();
            this.TopMost = true; // for the after the splash, the Main Interface shows on the top.
            // initialize the 
            totalcounts = 0;
            totaltime = 0;
            ctoe = 0;
            d = -1.0;
            Com = new Comm(this);
            currentHVSetting = 1;   //forces initial setting of HV bias
            targetHVSetting = 0;

            Cfg = new Configure(this);
            Cal = new Calibration(this);
            fm = new FWHM(this);
            collecttime = new time();

            Staticdata = new iGEMData();
            Activedata = new iGEMData();
        }

        private bool GetSN(ref string sDSN)
        {
            byte cmd = 11;

            byte p1 = 0;
            byte p2 = 0;

            if (Com.SndRcv(cmd, ref p1, ref p2) == true)
            {
                int Low = p1;
                int High = p2;
                int Number = High * 256;
                Number += Low;
                sDSN = Number.ToString();
                return true;
            }
            else
            {
                return false;
            }
        }
        private bool GetDT(ref string sDT)
        {
            byte cmd = 13;

            byte p1 = 0;//low
            byte p2 = 0;

            if (Com.SndRcv(cmd, ref p1, ref p2) == true)
            {
                switch (p1)
                {
                    case 1:
                        sDT = "CAPture Plus";
                        break;
                    case 2:
                        sDT = "CPG";
                        break;
                    case 3:
                        sDT = "Planar";
                        break;
                    default:
                        sDT = "Unknown";
                        MessageBox.Show("Undefined Detector Type");
                        break;

                }
                return true;
            }
            else
            {
                return false;
            }
        }
        private bool GetGain(ref string s)
        {
            byte cmd = 23;

            byte p1 = 0;//low
            byte p2 = 0;
            if (Com.SndRcv(cmd, ref p1, ref p2) == true)
            {
                switch (p1)
                {
                    case 0:
                        s = "18";
                        break;
                    case 1:
                        s = "36";
                        break;

                    default:
                        s = "";
                        return false;

                }

                return true;

            }
            else
            {
                return false;
            }
        }
        private bool GetPT(ref string s)
        {
            byte cmd = 25;

            byte p1 = 0;//low
            byte p2 = 0;
            if (Com.SndRcv(cmd, ref p1, ref p2) == true)
            {

                switch (p1)
                {
                    case 0:
                        s = "1.2";
                        break;
                    case 1:
                        s = "2.4";
                        break;

                    default:
                        s = "";
                        return false;

                }
                return true;
            }
            else
            {
                return false;
            }
        }
        private bool GetBias(ref string s)
        {
            byte cmd = 15;

            byte p1 = 0;
            byte p2 = 0;

            if (Com.SndRcv(cmd, ref p1, ref p2) == true)
            {
                int Low = p1;
                int High = p2;
                int Number = High * 256;
                Number += Low;
                //Number = Number * 2;
                s = Number.ToString();
                return true;
            }
            else
            {
                return false;
            }
        }
        private bool GetLLD(ref string s)
        {
            byte cmd = 27;

            byte p1 = 0;
            byte p2 = 0;

            if (Com.SndRcv(cmd, ref p1, ref p2) == true)
            {
                int Low = p1;
                int High = p2;
                int Number = High * 256;
                Number += Low;
                if (Number < 4096)
                {
                    int lld = Conversions.ToInteger(Number * 2.5 / 4.096);
                    s = lld.ToString();
                    return true;
                }
                return false;


            }
            else
            {
                return false;
            }
        }

        private bool GetEvnt(ref byte Low, ref byte High)
        {
            byte cmd = 4;

            byte p1 = 0;
            byte p2 = 0;

            if (Com.SndRcv4(cmd, ref p1, ref p2) == true)
            {
                Low = p1;
                High = p2;

                return true;
            }
            else
            {
                return false;
            }
        }

        public bool SetPT(string sPT)
        {
            byte cmd = 17;
            byte p1 = 0;// high
            byte p2 = 0;//low
            if (sPT == "1.2")
                p2 = 0;
            else
                p2 = 1;
            if (Com.SndRcv(cmd, ref p1, ref p2))
            {
                if (p1 == 0)// receive p1 is low
                    return true;

            }
            return false;


        }
        public bool SetGain(string sGain)
        {
            byte cmd = 16;
            byte p2 = 0;
            byte p1 = 0;
            if (sGain == "18")
                p2 = 0;
            else if (sGain == "36")
                p2 = 1;
            else
                return false;
            if (Com.SndRcv(cmd, ref p1, ref p2))
            {
                if (p1 == 0)// receive p1 is low
                    return true;

            }
            return false;
        }

        public bool SetBias(string bias)
        {
            bool status;
            byte cmd = 2;
            byte p2;
            byte p1;
            int i;
            try
            {
                i = Conversions.ToInteger(bias);
            }
            catch (Exception)
            {
                MessageBox.Show("Set bias error");
                return false;
            }
            targetHVSetting = i;
            if (Math.Abs(targetHVSetting - currentHVSetting) <= 100)
            {
                i = i / 2;  //set value p1 is high
                if ((i % 256) > 255)
                    p2 = 255;
                else
                    p2 = Conversions.ToByte(i % 256);
                i = i - (i % 256);
                p1 = Conversions.ToByte(i / 256);
                currentHVSetting = targetHVSetting;
                if (Com.SndRcv(cmd, ref p1, ref p2))
                {
                    if (p2 == 0 && p1 == 0)
                        return true;
                }
                return false;
            }
            else
            {
                this.Button_Start.Text = "Ramping HV";
                this.Button_Start.Enabled = false;
                if (targetHVSetting > currentHVSetting)
                    currentHVSetting += 100;
                else
                    currentHVSetting -= 100;
                i = currentHVSetting / 2;
                if ((i % 256) > 255)
                    p2 = 255;
                else
                    p2 = Conversions.ToByte(i % 256);
                i = i - (i % 256);
                p1 = Conversions.ToByte(i / 256);
                status = Com.SndRcv(cmd, ref p1, ref p2);
                timerHVRamp.Enabled = true;
                if (status == true)
                {
                    if (p2 == 0 && p1 == 0)
                        return true;
                }
                return false;
            }
        }

        public bool SetLLD(string s)
        {
            byte cmd = 6;
            byte p1;
            byte p2;
            try
            {
                double i = Conversions.ToDouble(s);
                //set value/4096*2.5

                i = i * 4.096 / 2.5;


                if ((i % 256) > 255)// in case i%256 >255.5
                    p2 = 255;
                else
                    p2 = Conversions.ToByte(i % 256);
                i = i - i % 256;
                p1 = Conversions.ToByte(i / 256);
            }
            catch (Exception)
            {
                MessageBox.Show("Set threshold error");
                return false;
            }


            if (Com.SndRcv(cmd, ref p1, ref p2))
            {
                if (p1 == 0)
                    return true;

            }
            return false;
        }

        //------------Set Default values -----------------//
        //*--------Get the default values for the flash of DODM----*//
        //*--------Set the default values to DODM------------------*//
        public bool SetDefault()
        {
            string s = "";
            if (GetGain(ref s))
            {
                if (!SetGain(s))
                {
                    MessageBox.Show("Set default Gain error");
                    return false;
                }
                this.Cfg.cmbGain.Text = s;
                s = "";
            }
            else
            {
                MessageBox.Show("Get default gain error");
                return false;
            }

            if (GetPT(ref s))
            {
                if (!SetPT(s))
                {
                    MessageBox.Show("Set default peaking time error");
                    return false;
                }
                this.Cfg.cmbPeakingTime.Text = s;
                s = "";
            }
            else
            {
                MessageBox.Show("Get default peaking time error");
                return false;
            }
            if (GetLLD(ref s))
            {
                if (!SetLLD(s))
                {
                    MessageBox.Show("Set default LLD error");
                    return false;
                }
                this.Cfg.TextLLD.Text = s;
                s = "";
            }
            else
            {
                MessageBox.Show("Get default LLD error");
                return false;
            }
            if (GetBias(ref s))
            {
                if (!SetBias(s))
                {
                    MessageBox.Show("Set default bias error");
                    return false;
                }

                this.Cfg.TextVB.Text = s;
                s = "";
            }
            else
            {
                MessageBox.Show("Get default bias error");
                return false;
            }
            return true;

        }
        private void Form1_Load(object sender, EventArgs e)
        {
            // Thread for Splash
            Thread th = new Thread(new ThreadStart(DoSplash));

            // Show Splash
            th.Start();
            this.TopLevel = true;
            Thread.Sleep(100);
            if (this.Com.LoadHW() == true)//load hardware
            {
                this.ConfigureToolStripMenuItem.Enabled = true;
                string sDSN = "";
                SetBias("0");
                if (GetSN(ref sDSN) == true)// get  the serial number
                {
                    GetSN(ref sDSN);//send twice for get the right response
                    string FILE_NAME = sDSN + ".ser";// Get the name of *.ser file
                    this.Cfg.LabelCSN.Text = sDSN;
                    if (!File.Exists(FILE_NAME))
                    {
                        // File doesn't exist, create one
                        using (StreamWriter sw = File.CreateText(FILE_NAME))
                        {
                            string s = "";
                            if (GetDT(ref s) == true)
                            {
                                sw.WriteLine("Detector Type = " + s);
                                this.Cfg.LabelCDT.Text = s;
                                s = "";
                            }
                            else
                            {
                                sw.WriteLine("Detector Type = None");
                                this.Cfg.LabelCDT.Text = s;
                                s = "";
                                MessageBox.Show("Error reading Detector Type");
                            }
                            if (GetPT(ref s) == true)
                            {
                                sw.WriteLine("Peaking time = " + s);
                                this.Cfg.cmbPeakingTime.Text = s;
                                s = "";
                            }
                            else
                            {
                                sw.WriteLine("Peaking time = None");
                                this.Cfg.cmbPeakingTime.Text = s;
                                s = "";
                                MessageBox.Show("Error reading Peaking Time");
                            }
                            if (GetGain(ref s) == true)
                            {
                                sw.WriteLine("Channel Gain = " + s);
                                this.Cfg.cmbGain.Text = s;
                                s = "";
                            }
                            else
                            {
                                sw.WriteLine("Channel Gain = None");
                                this.Cfg.cmbGain.Text = s;
                                s = "";
                                MessageBox.Show("Error reading Channel Gain");
                            }
                            if (GetBias(ref s) == true)
                            {
                                sw.WriteLine("Detector Bias = " + s);
                                this.Cfg.TextVB.Text = s;
                                s = "";
                            }
                            else
                            {
                                sw.WriteLine("Detector Bias  = None");
                                this.Cfg.TextVB.Text = s;
                                s = "";
                                MessageBox.Show("Error reading Detector Bias");
                            }
                            if (GetLLD(ref s) == true)
                            {
                                sw.WriteLine("Threshold = " + s);
                                this.Cfg.TextLLD.Text = s;
                                s = "";
                            }
                            else
                            {
                                sw.WriteLine("Thresholds  = None");
                                this.Cfg.TextLLD.Text = s;
                                s = "";
                                MessageBox.Show("Error reading Threshold");
                            }
                            sw.Close();
                        }
                    }
                    else// have the configuration file already
                    {
                        using (StreamReader sr = File.OpenText(FILE_NAME))
                        {
                            String s;
                            if ((s = sr.ReadLine()) != null)
                            {
                                int i = s.IndexOf('=');
                                if (i != -1)
                                {
                                    s = s.Substring(i + 2);// Detector type in the ser file
                                    string sDT = "";
                                    if (GetDT(ref sDT) == true)// Get the Detector Type from the flash of DODM
                                    {
                                        if (sDT != s)
                                        {
                                            MessageBox.Show("Detector type from iGEM module and serial number file do not agree");
                                        }
                                        this.Cfg.LabelCDT.Text = s;
                                        s = "";
                                    }
                                }
                            }
                            if ((s = sr.ReadLine()) != null)
                            {
                                int i = s.IndexOf('=');
                                if (i != -1)
                                {
                                    s = s.Substring(i + 2);// Get the Peaking time in the ser file
                                    if (s == "1.2" || s == "2.4")
                                    {
                                        if (SetPT(s)) // Set the peaking time according to the recorder in the ser file
                                        {
                                            this.Cfg.cmbPeakingTime.Text = s;
                                        }
                                        else
                                        {
                                            MessageBox.Show("Peaking time command from serial number file rejected by iGEM module");
                                        }
                                    }
                                    else
                                        MessageBox.Show("Invalid peaking time from serial number file:" + s);
                                    s = "";
                                }
                            }
                            if ((s = sr.ReadLine()) != null)
                            {
                                int i = s.IndexOf('=');
                                if (i != -1)
                                {
                                    s = s.Substring(i + 2);
                                    if (s == "18" || s == "36")
                                    {
                                        if (SetGain(s))
                                            this.Cfg.cmbGain.Text = s;
                                        else
                                        {
                                            MessageBox.Show("Channel gain command from serial number file rejected by iGEM module");
                                        }
                                    }
                                    else
                                        MessageBox.Show("Invalid channel gain from serial number file" + s);
                                    s = "";
                                }
                            }
                            if ((s = sr.ReadLine()) != null)
                            {
                                int i = s.IndexOf('=');
                                if (i != -1)
                                {
                                    s = s.Substring(i + 2);//"= -"
                                    if (SetBias(s))
                                        this.Cfg.TextVB.Text = s;
                                    else
                                    {
                                        MessageBox.Show("Bias command from serial number file rejected by iGEM module");
                                    }
                                    s = "";
                                }
                            }
                            if ((s = sr.ReadLine()) != null)
                            {
                                int i = s.IndexOf('=');
                                if (i != -1)
                                {
                                    s = s.Substring(i + 2);
                                    if (SetLLD(s))
                                        this.Cfg.TextLLD.Text = s;
                                    else
                                    {
                                        MessageBox.Show("Threshold command from serial number file rejected by iGEM module");
                                    }
                                    s = "";
                                }
                            }
                            sr.Close();
                        }
                    }
                }
                this.Hide();
                this.TopMost = false;
                th.Abort();
                this.Show();
            }
            else
            {
                DialogResult result;
                this.Hide();
                result = MessageBox.Show("iGEM Not Found. Open in Offline Mode?", "No Communication", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation);
                if (result == DialogResult.Yes)
                {
                    this.TopMost = false;
                    try
                    {
                        th.Abort();
                    }
                    finally
                    {
                    }
                    this.Show();
                }
                else
                {
                    Application.Exit();
                }
            }
        }

        // for the Start up Screeen
        private void DoSplash()
        {
            Splash sp = new Splash();
            sp.ShowDialog();
        }

        // Config Form
        Configure Cfg;
        //Calibration Form
        Calibration Cal;
        //FWHM Form
        FWHM fm;

        //Communciation Interface between iSpectrum and Hardware
        Comm Com;

        //the factor convert the ADC to Energy
        public double ctoe;
        public double d;
        // collection couts
        public int totalcounts;
        //collection time
        public int totaltime;

        //these three pbulic static is for getting event 
        // For get events timer procedure
        public bool collecting;
        public int Nextstep;
        // counts number should be received
        public uint rcvcount;

        // for digital input examination
        public bool nonNumberEntered;

        public int ltime;

        //Set the X Axis value to Energy value
        public void SetAxisX()
        {
            if (ctoe != 0)// in Kev mode
            {
                if (this.Activedata.SeriesIndex >= 0)
                {
                    Activedata.factor = ctoe;
                    Activedata.intercept = d;
                    for (int i = 0; i < Activedata.iChannelNo; i++)
                        Activedata.dEn[i] = Activedata.iChannel[i] * ctoe + d;
                }
                if (this.Staticdata.SeriesIndex >= 0)
                {
                    Staticdata.factor = ctoe;
                    Staticdata.intercept = d;
                    for (int i = 0; i < Staticdata.iChannelNo; i++)
                        Staticdata.dEn[i] = Staticdata.iChannel[i] * ctoe + d;
                }
            }
        }

        // Set the type of cursor of the chart when FWHM or Cal is shown.
        public void SetChartCursor()
        {
            this.gsNetWinChart1.Cursor = Cursors.Arrow;
        }
        //------------------------set ROI Begin-------------------------//
        public class FWHMCondition : ILineCondition
        {
            #region Fields
            private double ulimitValue;
            private double llimitValue;
            #endregion Fields

            #region Constructor

            public FWHMCondition(double ulimit, double llimit)
            {
                this.ulimitValue = ulimit;
                this.llimitValue = llimit;
            }
            #endregion Constructor

            #region ILineCondition Members
            public bool CheckCondition(Series series, int dataPoint)
            {
                // check to see if the data point is in the region
                // and if it is then return true (which will use our LimitLine instead of the series' line) 
                bool returnValue = false;

                if (series.GetValue(SeriesComponent.X, dataPoint) > this.llimitValue && series.GetValue(SeriesComponent.X, dataPoint) < this.ulimitValue)
                {
                    returnValue = true;
                }

                return returnValue;
            }

            private Line propLine = new Line(Color.Red, 1.0F);
            public Line Line
            {
                get
                {
                    return propLine;
                }
                set
                {
                    propLine = value;
                }
            }

            private string propName = string.Empty;
            public string Name
            {
                get
                {
                    return propName;
                }
                set
                {
                    propName = value;
                }
            }
            #endregion
        }
        //-----------------------set ROI end----------------------------//


        //----------------------For the Collecion time----------------------//
        public class time
        {
            private int hours; //hour
            private int minutes; //minutes
            private int seconds; //seconds

            public time()
            {
                this.hours = 0;
                this.minutes = 0;
                this.seconds = 0;
            }

            public time(int hours, int minutes, int seconds)
            {
                this.hours = hours;
                this.minutes = minutes;
                this.seconds = seconds;
            }

            public void sethours(int hours)
            {
                this.hours = hours;
            }

            public void setminutes(int minutes)
            {
                this.minutes = minutes;
            }

            public void setseconds(int seconds)
            {
                this.seconds = seconds;
            }

            public int gethours()
            {
                return this.hours;
            }

            public int getminutes()
            {
                return this.minutes;
            }

            public int getseconds()
            {
                return this.seconds;
            }



            public static time operator ++(time time)
            {
                time.seconds++;
                if (time.seconds >= 60)
                {
                    time.minutes++;
                    time.seconds = 0;
                    if (time.minutes >= 60)
                    {
                        time.hours++;
                        time.minutes = 0;
                        time.seconds = 0;
                        if (time.hours >= 24)
                        {
                            time.hours = 0;
                            time.minutes = 0;
                            time.seconds = 0;
                        }
                    }
                }
                return new time(time.hours, time.minutes, time.seconds);
            }
        }

        time collecttime;


        //-----------------For Communication with the iGEM hardware------------- //

        public unsafe class Comm
        {
            public Comm(Form1 mF)
            {

                this.mF_Form = mF;

            }
            private Form1 mF_Form;

            public const UInt32 FT_OK = 0;
            public const UInt32 FT_INVALID_HANDLE = 1;
            public const UInt32 FT_DEVICE_NOT_FOUND = 2;
            public const UInt32 FT_DEVICE_NOT_OPENED = 3;
            public const UInt32 FT_IO_ERROR = 4;
            public const UInt32 FT_INSUFFICIENT_RESOURCES = 5;
            public const UInt32 FT_INVALID_PARAMETER = 6;
            public const UInt32 FT_INVALID_BAUD_RATE = 7;
            public const UInt32 FT_DEVICE_NOT_OPENED_FOR_ERASE = 8;
            public const UInt32 FT_DEVICE_NOT_OPENED_FOR_WRITE = 9;
            public const UInt32 FT_FAILED_TO_WRITE_DEVICE = 10;
            public const UInt32 FT_EEPROM_READ_FAILED = 11;
            public const UInt32 FT_EEPROM_WRITE_FAILED = 12;
            public const UInt32 FT_EEPROM_ERASE_FAILED = 13;
            public const UInt32 FT_EEPROM_NOT_PRESENT = 14;
            public const UInt32 FT_EEPROM_NOT_PROGRAMMED = 15;
            public const UInt32 FT_INVALID_ARGS = 16;
            public const UInt32 FT_OTHER_ERROR = 17;

            public const UInt32 FT_LIST_NUMBER_ONLY = 0x80000000;
            public const UInt32 FT_LIST_BY_INDEX = 0x40000000;
            public const UInt32 FT_LIST_ALL = 0x20000000;
            public const UInt32 FT_OPEN_BY_SERIAL_NUMBER = 1;
            public const UInt32 FT_OPEN_BY_DESCRIPTION = 2;

            // Word Lengths
            public const byte FT_BITS_8 = 8;
            public const byte FT_BITS_7 = 7;
            public const byte FT_BITS_6 = 6;
            public const byte FT_BITS_5 = 5;

            // Stop Bits
            public const byte FT_STOP_BITS_1 = 0;
            public const byte FT_STOP_BITS_1_5 = 1;
            public const byte FT_STOP_BITS_2 = 2;

            // Parity
            public const byte FT_PARITY_NONE = 0;
            public const byte FT_PARITY_ODD = 1;
            public const byte FT_PARITY_EVEN = 2;
            public const byte FT_PARITY_MARK = 3;
            public const byte FT_PARITY_SPACE = 4;

            // Flow Control
            public const UInt16 FT_FLOW_NONE = 0;
            public const UInt16 FT_FLOW_RTS_CTS = 0x0100;
            public const UInt16 FT_FLOW_DTR_DSR = 0x0200;
            public const UInt16 FT_FLOW_XON_XOFF = 0x0400;

            // Purge rx and tx buffers
            public const byte FT_PURGE_RX = 1;
            public const byte FT_PURGE_TX = 2;

            // Events
            public const UInt32 FT_EVENT_RXCHAR = 1;
            public const UInt32 FT_EVENT_MODEM_STATUS = 2;

            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_ListDevices([MarshalAs(UnmanagedType.LPArray)]byte[] pvArg1, [MarshalAs(UnmanagedType.LPArray)]byte[] pvArg2, UInt32 dwFlags);	// FT_ListDevices by number only
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_ListDevices(UInt32 pvArg1, [MarshalAs(UnmanagedType.LPArray)]byte[] pvArg2, UInt32 dwFlags);	// FT_ListDevcies by serial number or description by index only
            [DllImport("FTD2XX64.dll")]
            static extern FT_STATUS FT_Open(UInt32 uiPort, ref FT_HANDLE ftHandle);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_OpenEx([MarshalAs(UnmanagedType.LPArray)]byte[] pvArg1, UInt32 dwFlags, ref FT_HANDLE ftHandle);
            [DllImport("FTD2XX64.dll")]
            static extern FT_STATUS FT_Close(FT_HANDLE ftHandle);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_Read(FT_HANDLE ftHandle, [MarshalAs(UnmanagedType.LPArray)]byte[] lpBuffer, UInt32 dwBytesToRead, ref UInt32 lpdwBytesReturned);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_Write(FT_HANDLE ftHandle, [MarshalAs(UnmanagedType.LPArray)]byte[] lpBuffer, UInt32 dwBytesToWrite, ref UInt32 lpdwBytesWritten);

            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_SetBaudRate(FT_HANDLE ftHandle, UInt32 dwBaudRate);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_SetDataCharacteristics(FT_HANDLE ftHandle, byte uWordLength, byte uStopBits, byte uParity);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_SetFlowControl(FT_HANDLE ftHandle, char usFlowControl, byte uXon, byte uXoff);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_SetDtr(FT_HANDLE ftHandle);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_ClrDtr(FT_HANDLE ftHandle);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_SetRts(FT_HANDLE ftHandle);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_ClrRts(FT_HANDLE ftHandle);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_GetModemStatus(FT_HANDLE ftHandle, ref UInt32 lpdwModemStatus);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_SetChars(FT_HANDLE ftHandle, byte uEventCh, byte uEventChEn, byte uErrorCh, byte uErrorChEn);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_Purge(FT_HANDLE ftHandle, UInt32 dwMask);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_SetTimeouts(FT_HANDLE ftHandle, UInt32 dwReadTimeout, UInt32 dwWriteTimeout);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_GetQueueStatus(FT_HANDLE ftHandle, ref UInt32 lpdwAmountInRxQueue);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_SetBreakOn(FT_HANDLE ftHandle);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_SetBreakOff(FT_HANDLE ftHandle);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_GetStatus(FT_HANDLE ftHandle, ref UInt32 lpdwAmountInRxQueue, ref UInt32 lpdwAmountInTxQueue, ref UInt32 lpdwEventStatus);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_SetEventNotification(FT_HANDLE ftHandle, UInt32 dwEventMask, [MarshalAs(UnmanagedType.LPArray)]byte[] pvArg);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_ResetDevice(FT_HANDLE ftHandle);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_SetDivisor(FT_HANDLE ftHandle, char usDivisor);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_GetLatencyTimer(FT_HANDLE ftHandle, ref byte pucTimer);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_SetLatencyTimer(FT_HANDLE ftHandle, byte ucTimer);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_GetBitMode(FT_HANDLE ftHandle, ref byte pucMode);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_SetBitMode(FT_HANDLE ftHandle, byte ucMask, byte ucEnable);
            [DllImport("FTD2XX64.dll")]
            static extern unsafe FT_STATUS FT_SetUSBParameters(FT_HANDLE ftHandle, UInt32 dwInTransferSize, UInt32 dwOutTransferSize);
            /// <summary>
            /// /////////////////////////////////////////////////////////////////////////////
            /// </summary>
            /// 

            public const UInt32 MP_SUCCESS = 0; //FT_OK
            public const UInt32 MP_INVALID_HANDLE = 1; //FT_INVALID_HANDLE
            public const UInt32 MP_DEVICE_NOT_FOUND = 2; //FT_DEVICE_NOT_FOUND
            public const UInt32 MP_DEVICE_NOT_OPENED = 3; //FT_DEVICE_NOT_OPENED
            public const UInt32 MP_IO_ERROR = 4; //FT_IO_ERROR
            public const UInt32 MP_INSUFFICIENT_RESOURCES = 5; //FT_INSUFFICIENT_RESOURCES

            public const UInt32 MP_DEVICE_NAME_BUFFER_TOO_SMALL = 20;
            public const UInt32 MP_INVALID_DEVICE_NAME = 21;
            public const UInt32 MP_INVALID_DEVICE_INDEX = 22;
            public const UInt32 MP_TOO_MANY_DEVICES = 23;
            public const UInt32 MP_DEVICE_IN_USE = 24;
            public const UInt32 MP_FAILED_TO_COMPLETE_COMMAND = 25;
            public const UInt32 MP_FAILED_TO_CLEAR_DEVICE = 26;
            public const UInt32 MP_FAILED_TO_PROGRAM_DEVICE = 27;
            public const UInt32 MP_FAILED_TO_SYNCHRONIZE_DEVICE = 28;
            public const UInt32 MP_SERIAL_NUMBER_BUFFER_TOO_SMALL = 29;
            public const UInt32 MP_DLL_VERSION_BUFFER_TOO_SMALL = 30;
            public const UInt32 MP_INVALID_LANGUAGE = 31;
            public const UInt32 MP_INVALID_STATUS_CODE = 32;
            public const UInt32 MP_ERROR_MESSAGE_BUFFER_TOO_SMALL = 33;
            public const UInt32 MP_INVALID_SERIAL_NUMBER = 34;
            public const UInt32 MP_INVALID_SERIAL_NUMBER_INDEX = 35;
            public const UInt32 MP_INVALID_FILE_NAME = 36;
            public const UInt32 MP_INVALID_FILE_EXTENSION = 37;
            public const UInt32 MP_FILE_NOT_FOUND = 38;
            public const UInt32 MP_FAILED_TO_OPEN_FILE = 39;
            public const UInt32 MP_FILE_READ_ERROR = 40;
            public const UInt32 MP_FILE_CORRUPT = 41;


            [DllImport("morphprg.dll")]
            static extern unsafe MP_STATUS GetNumMorphICDevices(ref uint lpdwNumMorphICDevices);
            [DllImport("morphprg.dll")]
            static extern unsafe MP_STATUS GetMorphICDeviceName(uint dwDeviceIndex, [MarshalAs(UnmanagedType.LPArray)]byte[] lpDeviceNameBuffer, uint dwBufferSize);
            [DllImport("morphprg.dll")]
            static extern unsafe MP_STATUS GetNumMorphICDeviceSerialNumbers([MarshalAs(UnmanagedType.LPArray)]byte[] lpDeviceName, ref uint lpdwNumMorphICDeviceSerialNumbers);
            [DllImport("morphprg.dll")]
            static extern unsafe MP_STATUS GetMorphICDeviceSerialNumber([MarshalAs(UnmanagedType.LPArray)]byte[] lpDeviceName, uint dwSerialNumberIndex, [MarshalAs(UnmanagedType.LPArray)]byte[] lpSerialNumberBuffer, uint dwBufferSize);
            [DllImport("morphprg.dll")]
            static extern unsafe MP_STATUS OpenSpecifiedMorphICDevice([MarshalAs(UnmanagedType.LPArray)]byte[] lpDeviceName, [MarshalAs(UnmanagedType.LPArray)]byte[] lpSerialNumber, MP_HANDLE* pMpHandle);
            [DllImport("morphprg.dll")]
            static extern unsafe MP_STATUS OpenMorphICDevice(MP_HANDLE pMpHandle);
            [DllImport("morphprg.dll")]
            static extern unsafe MP_STATUS CloseMorphICDevice(MP_HANDLE pMpHandle);
            [DllImport("morphprg.dll")]
            static extern unsafe MP_STATUS InitMorphICDevice(MP_HANDLE pMpHandle);
            [DllImport("morphprg.dll")]
            static extern unsafe MP_STATUS ProgramMorphICDevice(MP_HANDLE pMpHandle, [MarshalAs(UnmanagedType.LPArray)]byte[] lpFileName);
            [DllImport("morphprg.dll")]
            static extern unsafe MP_STATUS IsMorphICDeviceProgrammed(MP_HANDLE pMpHandle, ref bool lpbProgramState);
            [DllImport("morphprg.dll")]
            static extern unsafe MP_STATUS GetMorphICDllVersion([MarshalAs(UnmanagedType.LPArray)]byte[] lpDllVersionBuffer, uint lpbProgramState);
            [DllImport("morphprg.dll")]
            static extern unsafe MP_STATUS GetMorphICErrorCodeString([MarshalAs(UnmanagedType.LPArray)]byte[] lpLanguage, MP_STATUS StatusCode, [MarshalAs(UnmanagedType.LPArray)]byte[] lpErrorMessageBuffer, uint dwBufferSize);

            /// <summary>
            /// //////////////////////////////////////////////
            /// </summary>
            /// 

            [DllImport("kernel32.dll")]
            static extern unsafe void Sleep(uint dwCount);


            MP_STATUS Status = MP_SUCCESS;
            string prDeviceName = "MORPH-IC A";
            uint dwDeviceNamesReturned = 0;
            byte[] szSerialNumber = new byte[50];
            uint dwSerialNumbersReturned = 0;
            MP_HANDLE mpHandle;
            bool bProgramState = false;
            byte[] szDllVersion = new byte[50];
            byte[] szErrorMessage = new byte[50];

            FT_HANDLE FTHandle;
            FT_STATUS FTStatus = FT_OK;
            byte[] IN_buf = new byte[4010];
            byte[] OUT_buf = new byte[8];
            byte[] prSerialNumber = new byte[50];
            uint BytesWriten = 0;
            uint BytesToRead = 0;
            uint BytesReaden = 0;
            int WordsReaden = 0;

            //-------------in the initialization load the hardware------------------//
            public unsafe bool LoadHW()
            {
                FileInfo fInfo = new FileInfo("335993.rbf");

                if (!fInfo.Exists)
                {
                    MessageBox.Show("The 335993.rbf file was not found.Please copy the 335993.rbf to the iSpectrum directory");
                    Application.Exit();
                }
                Status = GetNumMorphICDevices(ref dwDeviceNamesReturned);
                if ((Status == MP_SUCCESS) && (dwDeviceNamesReturned > 0))
                {
                    Status = GetNumMorphICDeviceSerialNumbers(System.Text.Encoding.ASCII.GetBytes(prDeviceName), ref dwSerialNumbersReturned);

                    if ((Status == MP_SUCCESS) && (dwSerialNumbersReturned > 0))
                    {
                        if (dwSerialNumbersReturned == 1)
                        {
                            Status = GetMorphICDeviceSerialNumber(System.Text.Encoding.ASCII.GetBytes(prDeviceName), 0, szSerialNumber, 50);
                            //MessageBox.Show(System.Text.Encoding.ASCII.GetString(szSerialNumber));
                            if (Status == MP_SUCCESS)
                            {
                                fixed (MP_HANDLE* lpmpHandle = &mpHandle)
                                {
                                    Status = OpenSpecifiedMorphICDevice(System.Text.Encoding.ASCII.GetBytes(prDeviceName), szSerialNumber, lpmpHandle);
                                }
                                prSerialNumber = szSerialNumber;
                                //MessageBox.Show( System.Text.Encoding.ASCII.GetString(prSerialNumber));
                                prSerialNumber[8] = (byte)66;
                                //MessageBox.Show(System.Text.Encoding.ASCII.GetString(prSerialNumber));
                            }
                        }
                        else
                        {
                            if (dwSerialNumbersReturned == 2)
                            {
                                Status = GetMorphICDeviceSerialNumber(System.Text.Encoding.ASCII.GetBytes(prDeviceName), 1, szSerialNumber, 50);
                                if (Status == MP_SUCCESS)
                                {
                                    fixed (MP_HANDLE* lpmpHandle = &mpHandle)
                                    {
                                        Status = OpenSpecifiedMorphICDevice(System.Text.Encoding.ASCII.GetBytes(prDeviceName), szSerialNumber, lpmpHandle);
                                    }
                                }
                            }
                        }
                        if (Status == MP_SUCCESS)
                        {
                            Status = IsMorphICDeviceProgrammed(mpHandle, ref bProgramState);
                            if (Status == MP_SUCCESS)
                            {
                                Status = ProgramMorphICDevice(mpHandle, System.Text.Encoding.ASCII.GetBytes("335993.rbf"));
                                this.mF_Form.label3.Text = "iGEM Online";
                                this.mF_Form.label3.ForeColor = Color.Green;
                            }
                            else
                            {
                                // Morph-IC board is not programmed
                                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (1)");
                                this.mF_Form.label3.Text = "Program FPGA Failed!";
                                this.mF_Form.label3.ForeColor = Color.Red;
                                //------------------------Shen begin----------------//
                                CloseMorphICDevice(mpHandle);
                                Application.Exit();
                                //------------------------Shen end----------------//
                            }
                            CloseMorphICDevice(mpHandle);
                        }
                        if (Status == MP_SUCCESS)
                            Status = GetMorphICDllVersion(szDllVersion, 10);
                    }
                    //------------------------Shen begin----------------//
                    return true;
                    //------------------------Shen end----------------//
                }
                else
                {
                    //------------------------Shen begin----------------//
                    this.mF_Form.label3.Text = "iGEM Offline!";
                    this.mF_Form.label3.ForeColor = Color.Red;
                    this.mF_Form.Button_Start.Enabled = false;
                    this.mF_Form.Cfg.TextLLD.Enabled = false;
                    this.mF_Form.Cfg.TextVB.Enabled = false;
                    this.mF_Form.Cfg.cmbGain.Enabled = false;
                    this.mF_Form.Cfg.cmbPeakingTime.Enabled = false;
                    this.mF_Form.Cfg.CommandReset.Enabled = false;
                    return false;
                    //------------------------Shen end----------------//
                }

                //if (Status != MP_SUCCESS)
                //Status = GetMorphICErrorCodeString(System.Text.Encoding.ASCII.GetBytes("EN"), Status, szErrorMessage, 100);

            }
            //-------------in the initialization load the hardware End------------------//


            //------------------------Get the events form DODM-------------------------//
            //-------------------------------------------------------------------------//
            /*  step 1, Initialz() open the hardware
             *  step 2, Snd4()   send command 4
             *  step 3, Retrieve3() receive the reply of command 4 according to which send cmd3
             *  setp 4,Retrievedata(ref iGEMData data) fetch the data and send cmd4 and back to to step 3
             * */

            public unsafe bool Initial()
            {
                FTStatus = FT_OpenEx(prSerialNumber, 1, ref FTHandle);
                if (FTStatus != FT_OK || FTHandle == FT_INVALID_HANDLE)
                {
                    FT_Close(FTHandle);
                    File.AppendAllText("iSpectrum.log", "FT Open FTStatus" + FTStatus.ToString() + Environment.NewLine);
                    mF_Form.timerspectrum.Enabled = false;
                    // device fails to open
                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (2)");
                    return false;
                }
                FT_SetLatencyTimer(FTHandle, 2);
                FT_SetTimeouts(FTHandle, 16, 16);
                return true;
            }

            public unsafe bool Snd4()
            {
                FTStatus = FT_Purge(FTHandle, 3);
                if (FTStatus != FT_OK)
                {
                    FTStatus = FT_Purge(FTHandle, 3);
                    if (FTStatus != FT_OK)
                    {
                        FT_Close(FTHandle);
                        File.AppendAllText("iSpectrum.log", "!FT Purge FTStatus" + FTStatus.ToString() + Environment.NewLine);
                        mF_Form.timerspectrum.Enabled = false;
                        // clear FIFO command failed
                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (3)");
                        return false;
                    }
                }
                //sendcmd(4, 0, 0);
                OUT_buf[0] = (byte)255;
                OUT_buf[1] = (byte)255;
                OUT_buf[2] = 4;
                OUT_buf[3] = 0;
                OUT_buf[4] = 0;

                FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                if (FTStatus != FT_OK)
                {
                    FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                    if (FTStatus != FT_OK)
                    {
                        FT_Close(FTHandle);
                        File.AppendAllText("iSpectrum.log", "!FT Write FTStatus" + FTStatus.ToString() + Environment.NewLine);
                        mF_Form.timerspectrum.Enabled = false;
                        // write command (get n events) failed with bad status returned 
                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (4)");
                        return false;
                    }
                }

                if (BytesWriten != 5)
                {
                    FT_Close(FTHandle);
                    File.AppendAllText("iSpectrum.log", "!FT Write Bytewrittedn" + BytesWriten.ToString() + Environment.NewLine);
                    mF_Form.timerspectrum.Enabled = false;
                    // write command (get n events) failed with byte written incorrect
                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (5)");
                    return false;
                }
                return true;
            }

            public unsafe bool Snd5()
            {
                FTStatus = FT_Purge(FTHandle, 3);
                if (FTStatus != FT_OK)
                {
                    FTStatus = FT_Purge(FTHandle, 3);
                    if (FTStatus != FT_OK)
                    {
                        FT_Close(FTHandle);
                        File.AppendAllText("iSpectrum.log", "!FT Purge FTStatus" + FTStatus.ToString() + Environment.NewLine);
                        mF_Form.timerspectrum.Enabled = false;
                        // command to purge DLL buffer failed
                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (6)");
                        return false;
                    }
                }
                //sendcmd(5, 0, 0);
                OUT_buf[0] = (byte)255;
                OUT_buf[1] = (byte)255;
                OUT_buf[2] = 5;
                OUT_buf[3] = 0;
                OUT_buf[4] = 0;

                FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                if (FTStatus != FT_OK)
                {
                    FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                    if (FTStatus != FT_OK)
                    {
                        FT_Close(FTHandle);
                        File.AppendAllText("iSpectrum.log", "!FT Write FTStatus" + FTStatus.ToString() + Environment.NewLine);
                        mF_Form.timerspectrum.Enabled = false;
                        // write command (clear FIFO) failed with bad status returned 
                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (7)");
                        return false;
                    }
                }

                if (BytesWriten != 5)
                {
                    FT_Close(FTHandle);
                    File.AppendAllText("iSpectrum.log", "!FT Write Bytewrittedn" + BytesWriten.ToString() + Environment.NewLine);
                    mF_Form.timerspectrum.Enabled = false;
                    // write command (clear FIFO) failed with byte written incorrect
                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (8)");
                    return false;
                }

                while (BytesToRead < 2)
                {
                    FTStatus = FT_GetQueueStatus(FTHandle, ref BytesToRead);
                    if (FTStatus != FT_OK)
                    {
                        FTStatus = FT_GetQueueStatus(FTHandle, ref BytesToRead);
                        if (FTStatus != FT_OK)
                        {
                            FT_Close(FTHandle);
                            mF_Form.timerspectrum.Enabled = false;
                            // get queue status failed on return status when waiting for clear FIFO reply 
                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (9)");
                            return false;
                        }
                    }
                }
                return true;
            }
            public unsafe int Retrieve3()
            {
                uint operand, res1, res2, left;
                uint loopCount;
                bool jump;
                loopCount = 0;
                BytesToRead = 0;
                // Get the response of Cmd4
                // Cmd4 reply Ready or not, there is a timeout for cmd4 so won't be dead-circle

                while (BytesToRead < 2)
                {
                    if (mF_Form.collecting == true)// in case drop in endless circle
                    {

                        FTStatus = FT_GetQueueStatus(FTHandle, ref BytesToRead);
                        if (FTStatus != FT_OK)
                        {
                            FT_Close(FTHandle);
                            mF_Form.timerspectrum.Enabled = false;
                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (10)");
                            return 0;
                            //continue;
                        }
                        //Application.DoEvents();
                    }
                    else
                    {
                        FT_Close(FTHandle);// stop the acquiring!
                        return 0;
                    }
                    loopCount += 1;
                    if (loopCount > 1000)
                    {
                        LoadHW();
                        BytesToRead = 2;
                        File.AppendAllText("iSpectrum.log", "Loop count 1000 reached" + Environment.NewLine);
                    }
                }


                // read the response of CMD4

                FTStatus = FT_Read(FTHandle, IN_buf, BytesToRead, ref BytesReaden);
                if (FTStatus != FT_OK)
                {
                    FTStatus = FT_Read(FTHandle, IN_buf, BytesToRead, ref BytesReaden);
                    if (FTStatus != FT_OK)
                    {
                        FT_Close(FTHandle);
                        File.AppendAllText("iSpectrum.log", "!Read Reply of Cmd4 Error" + FTStatus.ToString() + Environment.NewLine);
                        mF_Form.timerspectrum.Enabled = false;
                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (11)");
                        return 0;
                        //continue;
                    }
                }

                res1 = (uint)IN_buf[0];
                res2 = (uint)IN_buf[1];
                if (loopCount > 100)
                    this.mF_Form.rcvcount = 0;
                else
                    this.mF_Form.rcvcount = res2 * 256 + res1;
                // out of valid range
                if (this.mF_Form.rcvcount > 2001 || this.mF_Form.rcvcount == 0)
                {
                    // skip and send cmd4 again
                    uint ii = this.mF_Form.rcvcount;
                    // File.AppendAllText("iSpectrum.log", "!events number Error:" + ii.ToString() + " time:" + mF_Form.Label_Time.Text + " Counts: " + mF_Form.Label_TC.Text + Environment.NewLine);
                    return 1; // return to the step 1
                }
                // fetch 185 each time
                operand = this.mF_Form.rcvcount / 185;
                left = this.mF_Form.rcvcount % 185;
                if (left > 0)
                {
                    jump = false;
                    left--;
                }
                else
                {
                    jump = true;
                }
                OUT_buf[0] = (byte)255;
                OUT_buf[1] = (byte)255;
                OUT_buf[2] = 3;
                OUT_buf[3] = 0;
                OUT_buf[4] = 184;

                for (int t = 0; t < operand; t++)
                {
                    FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                    if (FTStatus != FT_OK)
                    {
                        FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                        if (FTStatus != FT_OK)
                        {
                            FT_Close(FTHandle);
                            File.AppendAllText("iSpectrum.log", "!Retrieve Event Error!: FTStatus=" + FTStatus.ToString() + " FTHandle=" + FTHandle.ToString() + " OUT_buf=" + OUT_buf.ToString() + "BytesWriten=" + BytesWriten.ToString() + Environment.NewLine);
                            mF_Form.timerspectrum.Enabled = false;
                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (12)");
                            return 0;
                            //continue;
                        }
                    }

                    if (BytesWriten != 5)
                    {
                        // send cmd4 again
                        //Nextstep = 1;
                        //timerCollect.Enabled = true;
                        File.AppendAllText("iSpectrum.log", "!Retrieve Event Error!:BytesWriten " + BytesWriten.ToString() + Environment.NewLine);
                        return 1;
                    }

                }

                if (jump == false)
                {
                    OUT_buf[4] = (byte)left;

                    FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                    if (FTStatus != FT_OK)
                    {
                        FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                        if (FTStatus != FT_OK)
                        {
                            LoadHW();
                            File.AppendAllText("iSpectrum.log", "Retrieve Last Event Error!:FTStatus " + FTStatus.ToString() + ", LoadHW executed" + Environment.NewLine);
                            FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                            if (FTStatus != FT_OK)
                            {
                                FT_Close(FTHandle);
                                File.AppendAllText("iSpectrum.log", "Retrieve Last Event Error!:FTStatus " + FTStatus.ToString() + Environment.NewLine);
                                mF_Form.timerspectrum.Enabled = false;
                                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (13)");
                                return 0;
                                //continue;
                            }
                        }
                    }

                    if (BytesWriten != 5)
                    {
                        // send cmd4 again
                        //Nextstep = 1;
                        //timerCollect.Enabled = true;
                        File.AppendAllText("iSpectrum.log", "Retrieve Last Event Error!:BytesWriten " + BytesWriten.ToString() + Environment.NewLine);
                        return 1;// begin from step 1
                    }
                }
                //Nextstep = 3;
                //timerCollect.Enabled = true;
                return 3;// begin from step 3

            }
            public unsafe int Retrievedata(ref iGEMData data)
            {
                int res1, res2, respon;
                BytesToRead = 0;
                int l = 0;// for testing 
                mF_Form.ltime++;

                // Cmd3 reply ready or not, repeat 20000 times
                while ((BytesToRead) / 2 < mF_Form.rcvcount && l < 20000)
                {
                    if (mF_Form.collecting == true)
                    {
                        FTStatus = FT_GetQueueStatus(FTHandle, ref BytesToRead);
                        if (FTStatus != FT_OK)
                        {
                            FT_Close(FTHandle);
                            File.AppendAllText("iSpectrum.log", "Retrievedata!:FTStatus " + FTStatus.ToString() + Environment.NewLine);
                            mF_Form.timerspectrum.Enabled = false;
                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (14)");
                            return 0;
                            //continue;
                        }
                    }
                    else
                    {
                        FT_Close(FTHandle);

                        return 0;// stop acquiring
                    }
                    l++;
                }
                //if (l > 5)
                //{
                //   uint count = mF_Form.rcvcount;
                //   mF_Form.label3.Text = "l:" + l.ToString() + " ToRead:" + BytesToRead.ToString() + " " + count.ToString();
                //}
                //---------------------- To increase efficiency, send cmd 4 here-----------//
                OUT_buf[0] = (byte)255;
                OUT_buf[1] = (byte)255;
                OUT_buf[2] = 4;
                OUT_buf[3] = 0;
                OUT_buf[4] = 0;

                FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                if (FTStatus != FT_OK)
                {
                    FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                    if (FTStatus != FT_OK)
                    {
                        LoadHW();
                        File.AppendAllText("iSpectrum.log", "Cmd 4 FT_Write failed" + FTStatus.ToString() + ", loadHW executed" + Environment.NewLine);
                        FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                        if (FTStatus != FT_OK)
                        {
                            FT_Close(FTHandle);
                            mF_Form.timerspectrum.Enabled = false;
                            File.AppendAllText("iSpectrum.log", "Cmd 4 FT_Write FTStatus=" + FTStatus.ToString() + Environment.NewLine);
                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (15)");
                            return 0;
                        }
                    }
                }

                if (BytesWriten != 5)
                {
                    FT_Close(FTHandle);
                    mF_Form.timerspectrum.Enabled = false;
                    File.AppendAllText("iSpectrum.log", "Command 4 FT_Write ByteWriten=" + BytesWriten.ToString() + Environment.NewLine);
                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (16)");
                    return 0;

                }
                //--------------------------------------------------- send cmd4 end---------------------------//// fetch the real data
                FTStatus = FT_Read(FTHandle, IN_buf, BytesToRead, ref BytesReaden);
                if (FTStatus != FT_OK)
                {
                    FT_Close(FTHandle);
                    File.AppendAllText("iSpectrum.log", "FT_Read:FTStatus " + FTStatus.ToString() + Environment.NewLine);
                    mF_Form.timerspectrum.Enabled = false;
                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (17)");
                    return 0;
                    //continue;
                }
                else
                {
                    //log the minor error
                    if (BytesReaden != BytesToRead)
                        File.AppendAllText("iSpectrum.log", "BytesReaden: " + BytesReaden.ToString() + " BytesToRead: " + BytesToRead.ToString() + Environment.NewLine);
                    WordsReaden = (int)BytesReaden >> 1;
                    //                    mF_Form.totalcounts += WordsReaden;
                    fixed (byte* pIN_Buf = IN_buf)
                    {
                        byte* ps = pIN_Buf;
                        for (int i = 0; i < WordsReaden; i++)
                        {
                            res1 = (int)(*ps);
                            ps++;
                            res2 = (int)(*ps);
                            ps++;
                            respon = (res2 << 8) + res1;

                            if (respon == 65535)
                            {
                                i = (int)BytesReaden;
                                File.AppendAllText("iSpectrum.log", "Rseponse=65535 " + " time:" + mF_Form.Label_Time + " Counts: " + mF_Form.Label_TC.Text + Environment.NewLine);
                            }
                            if (respon < 4096) //just for test
                            {
                                data.dCounts[respon]++;
                            }
                        }
                    }
                }
                return 1; // restart from step 1
            }

            //Close the Hardware
            public unsafe bool Cls()
            {
                FTStatus = FT_Close(FTHandle);
                if (FTStatus != FT_OK)
                    return false;
                return true;
            }

            public unsafe bool SndRcv(byte cmd, ref byte p1, ref byte p2)
            {
                int wrong = 0;
                FTStatus = FT_OpenEx(prSerialNumber, 1, ref FTHandle);
                if (FTStatus != FT_OK || FTHandle == FT_INVALID_HANDLE)
                {
                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (18) CMD=" + cmd.ToString());
                    wrong = 1;
                }
                else
                {
                    FTStatus = FT_Purge(FTHandle, 3);
                    if (FTStatus != FT_OK)
                    {
                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (19) CMD=" + cmd.ToString());
                        wrong = 1;
                    }
                    else
                    {
                        FT_SetTimeouts(FTHandle, 100, 100);
                        OUT_buf[0] = (byte)255;
                        OUT_buf[1] = (byte)255;
                        OUT_buf[2] = cmd;
                        OUT_buf[3] = p1;
                        OUT_buf[4] = p2;
                        //File.AppendAllText("iSpectrum.log", "Write CMD" + cmd.ToString() + " , " + p1.ToString() + " '" + p2.ToString() + Environment.NewLine);

                        FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                        if (FTStatus != FT_OK)
                        {
                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (20) CMD=" + cmd.ToString());
                            wrong = 1;
                        }
                        else
                        {
                            if (BytesWriten == 5)
                            {
                                //this.mF_Form.label3.Text = "Command send OK!";
                                int wait = 0;
                                do
                                {
                                    wait++;
                                    Sleep(2);
                                    FTStatus = FT_GetQueueStatus(FTHandle, ref BytesToRead);
                                    if (FTStatus != FT_OK)
                                    {
                                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (21) CMD=" + cmd.ToString());
                                        wrong = 1;
                                        wait = 10;
                                    }
                                } while (BytesToRead < 2 && wait < 10);
                                if (FTStatus != FT_OK)
                                {
                                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (22) CMD=" + cmd.ToString());
                                    wrong = 1;
                                }
                                else
                                {
                                    if (BytesToRead >= 2)
                                    {
                                        //MessageBox.Show(BytesToRead.ToString());
                                        FTStatus = FT_Read(FTHandle, IN_buf, 2, ref BytesReaden);
                                        if (FTStatus != FT_OK)
                                        {
                                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (23) CMD=" + cmd.ToString());
                                            wrong = 1;
                                        }
                                        else
                                        {
                                            if (BytesReaden != 2)
                                            {
                                                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (24) CMD=" + cmd.ToString());
                                                wrong = 1;
                                            }
                                            else
                                            {
                                                p1 = IN_buf[0];
                                                p2 = IN_buf[1];
                                                File.AppendAllText("iSpectrum.log", "Read " + cmd.ToString() + " , " + p1.ToString() + " '" + p2.ToString() + Environment.NewLine);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        File.AppendAllText("iSpectrum.log", "Write CMD Byte write <2 FT status" + FTStatus.ToString() + Environment.NewLine);// add 2007-2-2
                                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (25) CMD=" + cmd.ToString());
                                        wrong = 1;
                                    }
                                }
                            }
                            else
                            {
                                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (26) CMD=" + cmd.ToString());
                                File.AppendAllText("iSpectrum.log", "No.3 Write Error! Please resend it!" + Environment.NewLine);
                                wrong = 1;
                            }
                        }
                    }
                }
                FT_Close(FTHandle);
                if (wrong == 1)
                    return false;
                return true;
            }

            public unsafe bool SndRcv4(byte cmd, ref byte p1, ref byte p2)
            {
                FTStatus = FT_OpenEx(prSerialNumber, 1, ref FTHandle);
                int wrong = 0;
                if (FTStatus != FT_OK || FTHandle == FT_INVALID_HANDLE)
                {
                    mF_Form.stop();
                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (27) CMD=" + cmd.ToString());
                    wrong = 1;
                }
                else
                {
                    FTStatus = FT_Purge(FTHandle, 3);
                    if (FTStatus != FT_OK)
                    {
                        mF_Form.stop();
                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (28) CMD=" + cmd.ToString());
                        wrong = 1;
                    }
                    else
                    {
                        FT_SetTimeouts(FTHandle, 100, 100);
                        OUT_buf[0] = (byte)255;
                        OUT_buf[1] = (byte)255;
                        OUT_buf[2] = cmd;
                        OUT_buf[3] = p1;
                        OUT_buf[4] = p2;
                        //File.AppendAllText("iSpectrum.log", "Write CMD" + cmd.ToString() + " , " + p1.ToString() + " '" + p2.ToString() + Environment.NewLine);

                        FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                        if (FTStatus != FT_OK)
                        {
                            mF_Form.stop();
                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (29) CMD=" + cmd.ToString());
                            wrong = 1;
                        }
                        else
                        {
                            if (BytesWriten == 5)
                            {
                                //this.mF_Form.label3.Text = "Command send OK!";
                                int wait = 0;
                                do
                                {
                                    wait++;
                                    Sleep(2);
                                    FTStatus = FT_GetQueueStatus(FTHandle, ref BytesToRead);
                                    if (FTStatus != FT_OK)
                                    {
                                        mF_Form.stop();
                                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (30) CMD=" + cmd.ToString());
                                        return false;
                                    }
                                } while (BytesToRead < 2 && wait < 10);
                                if (FTStatus != FT_OK)
                                {
                                    mF_Form.stop();
                                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (31) CMD=" + cmd.ToString());
                                    return false;
                                }
                                else
                                {
                                    if (BytesToRead >= 2)
                                    {
                                        FTStatus = FT_Read(FTHandle, IN_buf, 2, ref BytesReaden);
                                        if (FTStatus != FT_OK)
                                        {
                                            mF_Form.stop();
                                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (33) CMD=" + cmd.ToString());
                                            wrong = 1;
                                        }
                                        else
                                        {
                                            if (BytesReaden != 2)
                                            {
                                                mF_Form.stop();
                                                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (34) CMD=" + cmd.ToString());
                                                wrong = 1;
                                            }
                                            else
                                            {
                                                p1 = IN_buf[0];
                                                p2 = IN_buf[1];
                                                File.AppendAllText("iSpectrum.log", "Read " + cmd.ToString() + " , " + p1.ToString() + " '" + p2.ToString() + Environment.NewLine);
                                            }
                                        }
                                    }
                                    else
                                    {
                                        mF_Form.stop();
                                        File.AppendAllText("iSpectrum.log", "Write CMD4 Byte write <2 FT status" + FTStatus.ToString() + Environment.NewLine);// add 2007-2-2
                                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (35) CMD=" + cmd.ToString());
                                        wrong = 1;
                                    }
                                }
                            }
                            else
                            {
                                mF_Form.stop();
                                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (36) CMD=" + cmd.ToString());
                                File.AppendAllText("iSpectrum.log", "No.3 Write Error! Please resend it!" + Environment.NewLine);
                                wrong = 1;
                            }
                        }
                    }
                }
                FT_Close(FTHandle);
                if (wrong == 1)
                    return false;
                return true;
            }
        }


        //------------------------------ iGEMData ------------------------------------//
        //----------/ For handle the data saved and loaed from cvs file---------------//
        //--------------------- by Shen 2006-12-08------------------------------------//

        public class iGEMData
        {
            public iGEMData()
            {
                SeriesIndex = -1;
                LineCon = -1;
                iChannelNo = 4096;
                iChannel = new int[iChannelNo];
                dCounts = new int[iChannelNo];
                dCountsDbl = new double[iChannelNo];
                dEn = new double[iChannelNo];
                dSmth = new double[iChannelNo];
                sData = new string[iChannelNo];
                // simulate some data, should be used to initialize the class member.

                sDSN = "";
                sDT = "";
                sLLD = "";
                sBias = "";
                sPeakT = "";
                dGain = "";
                time = "00:00:00";
                factor = 0;
                intercept = 0;
                sCPS = "0";
                TC = "0";

                for (int i = 0; i < iChannelNo; i++)
                {
                    iChannel[i] = i;
                    dCounts[i] = 0;
                    dCountsDbl[i] = 0;
                    dEn[i] = 0;
                    dSmth[i] = 0;
                }
            }

            public void Reset()
            {
                SeriesIndex = -1;
                LineCon = -1;

                // simulate some data, should be used to initialize the class member.
                for (int i = 0; i < iChannelNo; i++)
                {
                    iChannel[i] = i;
                    dCounts[i] = 0;
                    dCountsDbl[i] = 0;
                    dEn[i] = 0;
                    dSmth[i] = 0;
                }
                sDSN = "";
                sDT = "";
                sLLD = "";
                sBias = "";
                sPeakT = "";
                dGain = "";
                time = "00:00:00";
                sCPS = "";
                factor = 0;
                intercept = 0;
                TC = "0";
            }


            // the series index of the spectrum
            public int SeriesIndex;
            //for ROI 
            public int LineCon;
            //Detector Serial No.
            public string sDSN;
            // Detector Type
            public string sDT;
            // Threshold
            public string sLLD;
            //Bias
            public string sBias;
            //peak time
            public string sPeakT;
            //gain
            public string dGain;
            //collection  time
            public string time;
            //CPS
            public string sCPS;


            //DAC to KeV Conversion factor
            public double factor;
            public double intercept;
            //Total number of counts
            public string TC;

            // the total channel number
            public int iChannelNo;
            // the channel number of each channel
            public int[] iChannel;
            // the counts number in the coorespoding channel
            public double[] dCountsDbl;
            // the counts number as doubles in the coorespoding channel
            public int[] dCounts;
            // the Energy of the coorespoding channel
            public double[] dEn;
            // smoothed counts number
            public double[] dSmth;

            // just for save the data to cvs file.
            private string[] sData;


            // combine all spectrum data to a string for cvs file
            public string[] toCSVdata()
            {
                for (int i = 0; i < iChannelNo; i++)
                {
                    this.sData[i] = iChannel[i] + "," + dEn[i] + "," + dCounts[i] + "," + dSmth[i];
                    //this.sData[i].TrimStart('"');
                    //this.sData[i].TrimEnd('"');
                }
                return sData;
            }

            //write spectrum data to csv
            private void WriteiDatacsv(StreamWriter sw)
            {
                sw.WriteLine("DAC channel, Kev, Counts, Smoothed counts");
                string[] csvstring = new string[iChannelNo];
                csvstring = toCSVdata();

                for (int i = 0; i < iChannelNo; i++)
                    sw.WriteLine(csvstring[i]);
            }
            // write all parameter of spectrum to csv       
            private void WriteiGEMcsv(StreamWriter sw)
            {
                sw.WriteLine("Serial number, " + sDSN);
                sw.WriteLine("Detector type, " + sDT);
                sw.WriteLine("Peaking time, " + sPeakT);
                sw.WriteLine("Channel Gain, " + dGain);
                sw.WriteLine("Voltage bias, " + sBias);
                sw.WriteLine("Lower level discriminator, " + sLLD);
                sw.WriteLine("Counts per second, " + sCPS);
                sw.WriteLine("Total Collection time, " + time);
                sw.WriteLine("Total number of counts, " + TC);
                sw.WriteLine("DAC channel to Kev conversion factor, " + factor);
                sw.WriteLine("DAC channel to Kev conversion intercept, " + intercept);
            }

            // for csv
            public void Writecsv(StreamWriter sw)
            {
                WriteiGEMcsv(sw);
                WriteiDatacsv(sw);
            }

            //read the all data from cvs
            public void Readcsv(StreamReader sr)
            {
                String s;
                int i = 0;
                s = sr.ReadLine();
                i = s.IndexOf(',');
                if (i != -1)
                {
                    sDSN = s.Substring(i + 2);
                    s = "";
                }

                s = sr.ReadLine();
                i = s.IndexOf(',');
                if (i != -1)
                {
                    sDT = s.Substring(i + 2);
                    s = "";
                }

                s = sr.ReadLine();
                i = s.IndexOf(',');
                if (i != -1)
                {
                    sPeakT = s.Substring(i + 2);
                    s = "";
                }

                s = sr.ReadLine();
                i = s.IndexOf(',');
                if (i != -1)
                {
                    dGain = s.Substring(i + 2);
                    s = "";
                }
                s = sr.ReadLine();
                i = s.IndexOf(',');
                if (i != -1)
                {
                    sBias = s.Substring(i + 2);
                    s = "";
                }
                s = sr.ReadLine();
                i = s.IndexOf(',');
                if (i != -1)
                {
                    sLLD = s.Substring(i + 2);
                    s = "";
                }
                s = sr.ReadLine();
                i = s.IndexOf(',');
                if (i != -1)
                {
                    sCPS = s.Substring(i + 2);
                    s = "";
                }

                s = sr.ReadLine();
                i = s.IndexOf(',');
                if (i != -1)
                {
                    time = s.Substring(i + 2);
                    if (time[1] == ':')
                        time = "0" + time;
                    s = "";
                }

                s = sr.ReadLine();
                i = s.IndexOf(',');
                if (i != -1)
                {
                    TC = s.Substring(i + 2);
                    s = "";
                }

                s = sr.ReadLine();
                i = s.IndexOf(',');
                if (i != -1)
                {
                    factor = Conversions.ToDouble(s.Substring(i + 2));
                    s = "";
                }

                s = sr.ReadLine();
                i = s.IndexOf(',');
                if (i != -1)
                {
                    intercept = Conversions.ToDouble(s.Substring(i + 2));
                    s = "";
                }

                sr.ReadLine();// read one more

                while (sr.Peek() >= 0)
                {
                    for (i = 0; i < iChannelNo; i++)
                    {
                        s = sr.ReadLine();
                        string[] split = s.Split(new Char[] { ',' });
                        iChannel[i] = Conversions.ToInteger(split[0]);
                        dEn[i] = Conversions.ToDouble(split[1]);
                        dCounts[i] = Conversions.ToInteger(split[2]);
                        dSmth[i] = Conversions.ToDouble(split[3]);
                    }
                }
            }

            //calculate the full width half max (FWHM)
            public double calcFWHM(int chl, int chr)
            {
                // first compute centroid of selected region...
                int sum = 0;
                int peakch = 0;
                int peakval = 0;
                for (int i = chl; i <= chr; i++)
                {
                    sum += dCounts[i];
                    if (dCounts[i] > peakval)
                    {
                        peakval = dCounts[i];
                        peakch = i;
                    }
                }
                // now compute fwhm...
                int left = peakch;
                int right = peakch;
                for (int i = peakch; i >= chl; i--)
                {
                    if (dCounts[i] >= peakval / 2)
                    {
                        left--;
                    }
                    else
                    {
                        break;
                    }
                }
                for (int i = peakch; i <= chr; i++)
                {
                    if (dCounts[i] >= peakval / 2)
                    {
                        right++;
                    }
                    else
                    {
                        break;
                    }
                }
                if (sum > 0)
                    return (double)(right - left); // active spectrum has not been cleared
                else
                    return 0;   // region of interest contains no counts
            }

            //calculate the spectrum with a 5 point moving average smoothing function
            public void smooth()
            {
                int nl = 2;
                int nr = 2;
                /*
                  in this routine the averaging is a symmetric stencil that
                  extends 'nl' points to the left and 'nr points to the right.
                  input :
                  'in' is a pointer to integer array of raw spectrum (length chnls)
                  'chnls' is an integer that is the length of the spectrum array
                  'nl' is an integer that determines the size of the stencil to the left
                  'nr' is an integer that determines the size of the stencil to the right
                  output : 
                  'out' is a pointer to the smoothed spectrum array (also length chnls)
                */
                // main smoothing...
                for (int i = nl; i < iChannelNo - nr; i++)
                {
                    dSmth[i] = 0;
                    for (int j = i - nl; j <= i + nr; j++) dSmth[i] += (double)dCounts[j];
                    dSmth[i] /= (nl + nr + 1);
                }
                // now we take care of the edge bins...
                int thisnl = nl;
                // left edge
                for (int i = nl - 1; i >= 0; i--)
                {
                    dSmth[i] = 0;
                    thisnl--;
                    for (int j = i - thisnl; j <= i + nr; j++) dSmth[i] += (double)dCounts[j];
                    dSmth[i] /= (thisnl + nr + 1);
                }
                int thisnr = nr;
                // right edge
                for (int i = iChannelNo - nr; i < iChannelNo; i++)
                {
                    dSmth[i] = 0;
                    thisnr--;
                    for (int j = i - nl; j <= i + thisnr; j++) dSmth[i] += (double)dCounts[j];
                    dSmth[i] /= (nl + thisnr + 1);
                }
            }
        }

        // declare a member of iGEMData for using
        iGEMData igemdata;// for temporary use
        public iGEMData Staticdata; // for static spectrum
        public iGEMData Activedata; // for active spectrum

        private void Button_Start_Click(object sender, EventArgs e)
        {
            this.Check_SpectrumS.Checked = false;
            this.gsNetWinChart1.Chart.SetGridBottomTitle("ADC Channel");
            Label_FWHM.Text = "";
            startTime = DateTime.Now;
            startTime = startTime - elapsedTime;
            elapsedTime = elapsedTime - elapsedTime;
            updateSpectrum = false;

            int channelNo = Activedata.iChannelNo;
            prevTotal1 = 0;
            for (int i = 0; i < channelNo; i++)
            {
                prevTotal1 += (ulong)Activedata.dCounts[i];
            }

            prevTotal2 = 0;
            prevTotal3 = 0;
            prevTotal4 = 0;
            int h = 0;
            int m = 0;
            int s = 0;
            // Get the current time for h, m and s
            try
            {
                if (this.Label_Time.Text == "")
                    this.Label_Time.Text = "00:00:00";
                h = Convert.ToInt16(this.Label_Time.Text.Substring(0, 2));
                m = Convert.ToInt16(this.Label_Time.Text.Substring(3, 2));
                s = Convert.ToInt16(this.Label_Time.Text.Substring(6, 2));
            }
            catch (Exception)
            {
                MessageBox.Show("Collection time Error");
                // If Exception, all set to 0
                h = 0;
                m = 0;
                s = 0;
            }
            collecttime.sethours(h);
            collecttime.setminutes(m);
            collecttime.setseconds(s);

            if (this.Check_EenergyD.Checked == true)
            {
                MessageBox.Show("Please Disable the Kev Display Mode, before pressing Start");
            }
            else
            {
                if (((collecttime.gethours() * 3600 + collecttime.getminutes() * 60 + collecttime.getseconds()) > totaltime && totaltime > 0)
                        || (Conversions.ToInteger(Activedata.TC) >= totalcounts && totalcounts > 0))
                    MessageBox.Show("The collection time or counts has been reached");
                else
                {
                    this.Button_Start.Enabled = false;
                    this.Button_Stop.Enabled = true;
                    rcvcount = 0;
                    Nextstep = 0;
                    collecting = true;
                    this.timerspectrum.Enabled = true;
                    this.Check_SpectrumS.Enabled = false;
                    this.LoadStaticSpectrumToolStripMenuItem.Enabled = false;
                    this.LoadActiveSpectrumToolStripMenuItem.Enabled = false;
                    if (this.Check_EenergyD.Enabled == true)
                        this.Check_EenergyD.Enabled = false;
                    this.timerCollect.Enabled = true;
                }
            }
        }

        //------------For Energy Calibration-------------------------//
        private void CalibrateToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if ((Activedata.SeriesIndex >= 0 && this.Check_DisLive.Checked == true) || (Staticdata.SeriesIndex >= 0 && this.Check_DisStatic.Checked == true))
            {
                if (this.Check_EenergyD.Checked == false)
                {
                    this.gsNetWinChart1.Cursor = Cursors.Cross; // the proper of LockchartObj should be set true
                    this.AddOwnedForm(Cal); // for top show
                    Cal.Show();
                }
                else
                    MessageBox.Show("Please Disable the Kev Display Mode, before pressing Start");
            }
            else
                MessageBox.Show("A spectrum must be displayed to perform a calibration");
        }

        private void ExitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void AboutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            AboutBox about = new AboutBox();
            about.ShowDialog();
        }

        private void ConfigureToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.AddOwnedForm(Cfg);
            if (this.Button_Stop.Enabled == true)// when acquiring,disable the settings
            {
                Cfg.TextLLD.Enabled = false;
                Cfg.TextVB.Enabled = false;
                Cfg.cmbGain.Enabled = false;
                Cfg.cmbPeakingTime.Enabled = false;
                Cfg.CommandReset.Enabled = false;
            }
            else
            {
                if (this.label3.Text == "iGEM Online")
                {
                    Cfg.TextLLD.Enabled = true;
                    Cfg.TextVB.Enabled = true;
                    Cfg.cmbGain.Enabled = true;
                    Cfg.cmbPeakingTime.Enabled = true;
                    Cfg.CommandReset.Enabled = true;
                }
            }
            Cfg.obias = Cfg.TextVB.Text;
            Cfg.ogain = Cfg.cmbGain.Text;
            Cfg.olld = Cfg.TextLLD.Text;
            Cfg.opt = Cfg.cmbPeakingTime.Text;
            Cfg.Show();
        }

        private void Check_SpectrumS_CheckedChanged(object sender, EventArgs e)
        {
            smoothSpectrum();
        }

        //-------------------spectrum smoothing------------------------------------//
        public void smoothSpectrum()
        {
            Chart chart = this.gsNetWinChart1.Chart;
            int points = 0;
            int startPoint = 0;
            int endPoint = 0;
            int decimate = 0;
            int step = 0;

            if (this.Check_XFull.Checked == true)
            {
                startPoint = 0;
                endPoint = 4095;
                decimate = 2;
                points = endPoint - startPoint + 1;
            }
            else
            {
                if (Check_EenergyD.Checked)
                {
                    if (Activedata.SeriesIndex >= 0)
                    {
                        endPoint = (int)((Conversions.ToDouble(TextBox_XMax.Text) - Activedata.intercept) / Activedata.factor);
                        if (endPoint > 4095)
                            endPoint = 4095;
                        startPoint = (int)((Conversions.ToDouble(TextBox_XMin.Text) - Activedata.intercept) / Activedata.factor);
                        if (startPoint < 0)
                            startPoint = 0;
                    }
                    else if (Staticdata.SeriesIndex >= 0)
                    {
                        endPoint = (int)((Conversions.ToDouble(TextBox_XMax.Text) - Staticdata.intercept) / Staticdata.factor);
                        if (endPoint > 4095)
                            endPoint = 4095;
                        startPoint = (int)((Conversions.ToDouble(TextBox_XMin.Text) - Staticdata.intercept) / Staticdata.factor);
                        if (startPoint < 0)
                            startPoint = 0;
                    }
                }
                else
                {
                    endPoint = Conversions.ToInteger(TextBox_XMax.Text);
                    startPoint = Conversions.ToInteger(TextBox_XMin.Text);
                }
                points = endPoint - startPoint + 1;
                if (points < 1)
                {
                    startPoint = 0;
                    endPoint = Activedata.iChannelNo >> 2;
                    decimate = 2;
                }
                else if (points > 2047)
                {
                    decimate = 2;
                }
                else if (points > 1023)
                {
                    decimate = 1;
                }
                else
                {
                    decimate = 0;
                }
            }
            step = 1 << decimate;
            points = (points >> decimate);

            if (Check_SpectrumS.Checked == true)
            {
                if (Activedata.SeriesIndex >= 0)
                {
                    Activedata.smooth();
                    Series activespectrum = new Series();
                    activespectrum.SeriesName = "Active spectrum";
                    int j = startPoint;
                    for (int i = 0; i < points; i++)
                    {
                        if (this.Check_EenergyD.Checked == true)
                        {
                            activespectrum.SetValue(SeriesComponent.X, i, Activedata.dEn[j]);
                        }
                        else
                            activespectrum.SetValue(SeriesComponent.X, i, Activedata.iChannel[j]);
                        activespectrum.SetValue(SeriesComponent.Y, i, Activedata.dSmth[j]);
                        j += step;
                    }
                    if (j < 4095)
                    {
                        for (int i = points; i < 1024; i++)
                        {
                            if (this.Check_EenergyD.Checked == true)
                                activespectrum.SetValue(SeriesComponent.X, i, Activedata.dEn[j]);
                            else
                                activespectrum.SetValue(SeriesComponent.X, i, Activedata.iChannel[j]);
                            j += step;
                            if (j > 4095)   // prevent subscript out of range error
                                i = 1024;
                        }
                    }
                    chart.ReplaceSeries(Activedata.SeriesIndex, activespectrum);
                }
                if (Staticdata.SeriesIndex >= 0)
                {
                    Staticdata.smooth();
                    Series staticspectrum = new Series();
                    staticspectrum.SeriesName = "Static spectrum";
                    int j = startPoint;
                    for (int i = 0; i < points; i++)
                    {
                        if (this.Check_EenergyD.Checked == true)
                        {
                            if (Staticdata.factor == 0)
                                staticspectrum.SetValue(SeriesComponent.X, i, Activedata.dEn[j]);
                            else
                                staticspectrum.SetValue(SeriesComponent.X, i, Staticdata.dEn[j]);
                        }
                        else
                            staticspectrum.SetValue(SeriesComponent.X, i, Staticdata.iChannel[j]);
                        staticspectrum.SetValue(SeriesComponent.Y, i, Staticdata.dSmth[j]);
                        j += step;
                    }
                    if (j < 4095)
                    {
                        for (int i = points; i < 1024; i++)
                        {
                            if (this.Check_EenergyD.Checked == true)
                                if (Staticdata.factor == 0)
                                    staticspectrum.SetValue(SeriesComponent.X, i, Activedata.dEn[j]);
                                else
                                    staticspectrum.SetValue(SeriesComponent.X, i, Staticdata.dEn[j]);
                            else
                                staticspectrum.SetValue(SeriesComponent.X, i, Staticdata.iChannel[j]);
                            j += step;
                            if (j > 4095)   // prevent subscript out of range error
                                i = 1024;
                        }
                    }
                    chart.ReplaceSeries(Staticdata.SeriesIndex, staticspectrum);
                }
            }
            else
            {
                if (Activedata.SeriesIndex >= 0)
                {
                    Series activespectrum = new Series();
                    activespectrum.SeriesName = "Active spectrum";
                    int j = startPoint;
                    for (int i = 0; i < points; i++)
                    {
                        if (this.Check_EenergyD.Checked == true)
                            activespectrum.SetValue(SeriesComponent.X, i, Activedata.dEn[j]);
                        else
                            activespectrum.SetValue(SeriesComponent.X, i, Activedata.iChannel[j]);
                        activespectrum.SetValue(SeriesComponent.Y, i, Activedata.dCounts[j]);
                        j += step;
                    }
                    if (j < 4095)
                    {
                        for (int i = points; i < 1024; i++)
                        {
                            if (this.Check_EenergyD.Checked == true)
                                activespectrum.SetValue(SeriesComponent.X, i, Activedata.dEn[j]);
                            else
                                activespectrum.SetValue(SeriesComponent.X, i, Activedata.iChannel[j]);
                            j += step;
                            if (j > 4095)   // prevent subscript out of range error
                                i = 1024;
                        }
                    }
                    chart.ReplaceSeries(Activedata.SeriesIndex, activespectrum);
                }
                if (Staticdata.SeriesIndex >= 0)
                {
                    Series staticspectrum = new Series();
                    staticspectrum.SeriesName = "Static spectrum";
                    int j = startPoint;
                    for (int i = 0; i < points; i++)
                    {
                        if (this.Check_EenergyD.Checked == true)
                            if (Staticdata.factor == 0)
                                staticspectrum.SetValue(SeriesComponent.X, i, Activedata.dEn[j]);
                            else
                                staticspectrum.SetValue(SeriesComponent.X, i, Staticdata.dEn[j]);
                        else
                            staticspectrum.SetValue(SeriesComponent.X, i, Staticdata.iChannel[j]);
                        staticspectrum.SetValue(SeriesComponent.Y, i, Staticdata.dCounts[j]);
                        j += step;
                    }
                    if (j < 4095)
                    {
                        for (int i = points; i < 1024; i++)
                        {
                            if (this.Check_EenergyD.Checked == true)
                                if (Staticdata.factor == 0)
                                    staticspectrum.SetValue(SeriesComponent.X, i, Activedata.dEn[j]);
                                else
                                    staticspectrum.SetValue(SeriesComponent.X, i, Staticdata.dEn[j]);
                            else
                                staticspectrum.SetValue(SeriesComponent.X, i, Staticdata.iChannel[j]);
                            j += step;
                            if (j > 4095)   // prevent subscript out of range error
                                i = 1024;
                        }
                    }
                    chart.ReplaceSeries(Staticdata.SeriesIndex, staticspectrum);
                }
            }
            if (ROI)
                paintROI();
            else
                this.gsNetWinChart1.Chart.RecalcLayout();
        }

        //--------------------------set the scale of chart------------------------------//
        private void button_setAxis_Click(object sender, EventArgs e)
        {
            setAxis();
        }

        public void setAxis()
        {
            if (Option_Linear.Checked == true)
                this.gsNetWinChart1.Chart.Grid.AxisY.AxisMode = AxisMode.ValueLinear;
            else
                this.gsNetWinChart1.Chart.Grid.AxisY.AxisMode = AxisMode.ValueLog;

            if (Check_AutoScale.Checked == true)
                this.gsNetWinChart1.Chart.Grid.AxisY.AxisScale = AxisScaleType.AutoRound;
            else
            {
                try
                {
                    if (Conversions.ToDouble(TextBox_YMax.Text) > Conversions.ToDouble(TextBox_YMin.Text) && Conversions.ToDouble(TextBox_YMin.Text) >= 0 && Conversions.ToDouble(TextBox_YMax.Text) <= 1000000)
                    {
                        this.gsNetWinChart1.Chart.Grid.AxisY.MaxAxisValueUser = Conversions.ToDouble(TextBox_YMax.Text);
                        this.gsNetWinChart1.Chart.Grid.AxisY.MinAxisValueUser = Conversions.ToDouble(TextBox_YMin.Text);
                        this.gsNetWinChart1.Chart.Grid.AxisY.AxisScale = AxisScaleType.UserDefined;
                    }
                    else
                    {
                        MessageBox.Show("Please enter Y Max and Y Min with Y Max > Y Min and Y Min >= 0", "Input Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                        TextBox_YMax.Text = "";
                        TextBox_YMin.Text = "";
                    }
                }
                catch (Exception)
                {
                    MessageBox.Show("Please enter Y Max and Y Min with Y Max > Y Min and Y Min >= 0", "Input Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    TextBox_YMax.Text = "";
                    TextBox_YMin.Text = "";
                }
            }
            if (Check_XFull.Checked == true)
            {
                this.gsNetWinChart1.Chart.Grid.AxisX.AxisScale = AxisScaleType.BestFit;
            }
            else
            {
                try
                {
                    if (Conversions.ToDouble(TextBox_XMax.Text) > Conversions.ToDouble(TextBox_XMin.Text) & Conversions.ToDouble(TextBox_XMin.Text) >= 0 & Conversions.ToDouble(TextBox_XMax.Text) <= 4095)
                    {
                        this.gsNetWinChart1.Chart.Grid.AxisX.MaxAxisValueUser = Conversions.ToDouble(TextBox_XMax.Text);
                        this.gsNetWinChart1.Chart.Grid.AxisX.MinAxisValueUser = Conversions.ToDouble(TextBox_XMin.Text);
                        this.gsNetWinChart1.Chart.Grid.AxisX.AxisScale = AxisScaleType.UserDefined;
                        if (Check_EenergyD.Checked)
                        {
                            if (Activedata.SeriesIndex >= 0)
                            {
                                xAxisMax = (int)((Conversions.ToDouble(TextBox_XMax.Text) - Activedata.intercept) / Activedata.factor);
                                if (xAxisMax > 4095)
                                    xAxisMax = 4095;
                                xAxisMin = (int)((Conversions.ToDouble(TextBox_XMin.Text) - Activedata.intercept) / Activedata.factor);
                                if (xAxisMin < 0)
                                    xAxisMin = 0;
                            }
                            else if (Staticdata.SeriesIndex >= 0)
                            {
                                xAxisMax = (int)((Conversions.ToDouble(TextBox_XMax.Text) - Staticdata.intercept) / Staticdata.factor);
                                if (xAxisMax > 4095)
                                    xAxisMax = 4095;
                                xAxisMin = (int)((Conversions.ToDouble(TextBox_XMin.Text) - Staticdata.intercept) / Staticdata.factor);
                                if (xAxisMin < 0)
                                    xAxisMin = 0;
                            }
                        }
                        else
                        {
                            xAxisMax = Conversions.ToInteger(TextBox_XMax.Text);
                            xAxisMin = Conversions.ToInteger(TextBox_XMin.Text);
                        }
                    }
                    else
                    {
                        MessageBox.Show("Please enter X Max and X Min with X Max > X Min, XMin >= 0 and XMax < 4096", "Input Error.", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                        Check_EenergyD.Checked = false;
                        TextBox_XMin.Text = "0";
                        TextBox_XMax.Text = "4095";
                    }
                }
                catch (Exception)
                {
                    MessageBox.Show("Please enter X Max and X Min with X Max > X Min, XMin >= 0 and XMax < 4096", "Input Error.", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    Check_EenergyD.Checked = false;
                    TextBox_XMin.Text = "0";
                    TextBox_XMax.Text = "4095";
                }
            }

            Chart chart = this.gsNetWinChart1.Chart;
            if (Check_SpectrumS.Checked)
            {
                smoothSpectrum();
                return;
            }
            int points = 0;
            int startPoint = 0;
            int endPoint = 0;
            int decimate = 0;
            int step = 0;
            if (this.Check_XFull.Checked == true)
            {
                startPoint = 0;
                endPoint = 4095;
                decimate = 2;
                points = endPoint - startPoint + 1;
            }
            else
            {
                startPoint = xAxisMin;
                endPoint = xAxisMax;
                points = endPoint - startPoint + 1;
                if (points < 1)
                {
                    startPoint = 0;
                    endPoint = Activedata.iChannelNo >> 2;
                    decimate = 2;
                }
                else if (points > 2047)
                {
                    decimate = 2;
                }
                else if (points > 1023)
                {
                    decimate = 1;
                }
                else
                {
                    decimate = 0;
                }
            }
            step = 1 << decimate;
            points = (points >> decimate);
            if (Activedata.SeriesIndex < 0)
            {
                if (collecting == true)
                {
                    //instantiate the Active Data
                    Activedata.sDSN = this.Cfg.LabelCSN.Text;
                    Activedata.sDT = this.Cfg.LabelCDT.Text;
                    Activedata.sBias = this.Cfg.TextVB.Text;
                    Activedata.sLLD = this.Cfg.TextLLD.Text;
                    Activedata.sPeakT = this.Cfg.cmbPeakingTime.Text;
                    Activedata.dGain = this.Cfg.cmbGain.Text;
                    int j = startPoint;
                    for (int i = 0; i < points; i++)
                    {
                        activespectrum.SetValue(SeriesComponent.Y, i, Activedata.dCounts[j]);
                        if (Check_EenergyD.Checked)
                            activespectrum.SetValue(SeriesComponent.X, i, Activedata.dEn[j]);
                        else
                            activespectrum.SetValue(SeriesComponent.X, i, Activedata.iChannel[j]);
                        j += step;
                    }
                    Activedata.SeriesIndex = chart.AddSeries(activespectrum);
                }
            }
            else
            {
                int j = startPoint;
                for (int i = 0; i < points; i++)
                {
                    activespectrum.SetValue(SeriesComponent.Y, i, Activedata.dCounts[j]);
                    if (Check_EenergyD.Checked)
                        activespectrum.SetValue(SeriesComponent.X, i, Activedata.dEn[j]);
                    else
                        activespectrum.SetValue(SeriesComponent.X, i, Activedata.iChannel[j]);
                    j += step;
                }
                if (j < 4095)
                {
                    for (int i = points; i < 1024; i++)
                    {
                        if (Check_EenergyD.Checked)
                            activespectrum.SetValue(SeriesComponent.X, i, Activedata.dEn[j]);
                        else
                            activespectrum.SetValue(SeriesComponent.X, i, Activedata.iChannel[j]);
                        j += step;
                        if (j > 4095)   // prevent subscript out of range error
                            i = 1024;
                    }
                }
                chart.ReplaceSeries(Activedata.SeriesIndex, activespectrum);
            }
            if (Activedata.SeriesIndex >= 0)
            {
                chart.GetSeriesDrawing(Activedata.SeriesIndex).MarkerLine.Color = Color.Blue;
                chart.GetSeriesDrawing(Activedata.SeriesIndex).MarkerLabelsOn = false;
                chart.GetSeriesDrawing(Activedata.SeriesIndex).SymbolsOn = false;
            }
            if (Staticdata.SeriesIndex >= 0)
            {
                int j = startPoint;
                for (int i = 0; i < points; i++)
                {
                    staticspectrum.SetValue(SeriesComponent.Y, i, Staticdata.dCounts[j]);
                    if (Check_EenergyD.Checked)
                        if (Staticdata.factor == 0)
                            staticspectrum.SetValue(SeriesComponent.X, i, Activedata.dEn[j]);
                        else
                            staticspectrum.SetValue(SeriesComponent.X, i, Staticdata.dEn[j]);
                    else
                        staticspectrum.SetValue(SeriesComponent.X, i, Staticdata.iChannel[j]);
                    j += step;
                }
                if (j < 4095)
                {
                    for (int i = points; i < 1024; i++)
                    {
                        if (Check_EenergyD.Checked)
                            if (Staticdata.factor == 0)
                                staticspectrum.SetValue(SeriesComponent.X, i, Activedata.dEn[j]);
                            else
                                staticspectrum.SetValue(SeriesComponent.X, i, Staticdata.dEn[j]);
                        else
                            staticspectrum.SetValue(SeriesComponent.X, i, Staticdata.iChannel[j]);
                        j += step;
                        if (j > 4095)   // prevent subscript out of range error
                            i = 1024;
                    }
                }
                chart.ReplaceSeries(Staticdata.SeriesIndex, staticspectrum);
                chart.GetSeriesDrawing(Staticdata.SeriesIndex).MarkerLine.Color = Color.Green;
                chart.GetSeriesDrawing(Staticdata.SeriesIndex).MarkerLabelsOn = false;
                chart.GetSeriesDrawing(Staticdata.SeriesIndex).SymbolsOn = false;
            }
            if (ROI)
                paintROI();
            else
                chart.RecalcLayout();
        }

        //----------------------- remove the Active spectrum and reset all the parameters in UI---------------//
        public void Clear()
        {
            elapsedTime = elapsedTime - elapsedTime;
            this.Label_Time.Text = "00:00:00";
            collecttime.sethours(0);
            collecttime.setminutes(0);
            collecttime.setseconds(0);

            this.Label_CiS.Text = "0";
            this.Label_CPS.Text = "0";
            this.Label_TC.Text = "0";
            this.Label_Time.Text = "00:00:00";

            for (int i = 0; i < Activedata.iChannelNo; i++)
            {
                Activedata.dCounts[i] = 0;
                Activedata.dCountsDbl[i] = 0;
                Activedata.dSmth[i] = 0;
            }
            Activedata.time = "00:00:00";
            Activedata.sCPS = "";
            Activedata.TC = "0";
            setAxis();
        }

        private void ClearActiveSpectrumToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Clear();
            setfunction();
        }

        //---------------------for spectrum drawing-----------------------//
        private void drawactive(iGEMData igemdata)
        {
            Chart chart = this.gsNetWinChart1.Chart;
            AxisX axisX = chart.Grid.AxisX;
            Series activespectrum = new Series();
            activespectrum.SeriesName = "Active spectrum";
            for (int i = 0; i < igemdata.iChannelNo; i++)
            {
                activespectrum.SetValue(SeriesComponent.X, i, igemdata.iChannel[i]);
                activespectrum.SetValue(SeriesComponent.Y, i, igemdata.dCounts[i]);
            }
            //Add the new data series to the chart
            if (Activedata.SeriesIndex < 0) // if no active spectrum exists, add a new one
            {
                if (Staticdata.SeriesIndex < 0)//no static spectrum exists
                {
                    if (igemdata.factor != 0) //Active spectrum has been calibrated
                        this.Check_EenergyD.Enabled = true;
                    else
                        this.Check_EenergyD.Enabled = false;//in case the checkbox has been enabled
                }
                else // Static Spectrum has been loaded already
                {
                    if (igemdata.factor != 0) //Active spectrum has been calibrated
                    {
                        this.Check_EenergyD.Enabled = true;
                        if (this.Check_EenergyD.Checked == true) //Now display mode is kev mode
                        {
                            //draw the active spectrum with Energy X Axis
                            for (int i = 0; i < igemdata.iChannelNo; i++)
                            {
                                activespectrum.SetValue(SeriesComponent.X, i, igemdata.dEn[i]);
                            }
                        }
                    }
                    else //Active spectrum is not calibrated.
                    {
                        if (this.Check_EenergyD.Checked == true) //Now display mode is kev mode
                        {
                            //change the Energy X Axis of Static Spectrum to ADC Channel 
                            Series staticspectrum = new Series();
                            staticspectrum.SeriesName = "Static spectrum";
                            for (int i = 0; i < Staticdata.iChannelNo; i++)
                            {
                                staticspectrum.SetValue(SeriesComponent.X, i, Staticdata.iChannel[i]);
                            }
                            chart.ReplaceSeries(Staticdata.SeriesIndex, staticspectrum);
                            this.Check_EenergyD.Checked = false;
                            this.Check_EenergyD.Enabled = false;
                            chart.SetGridBottomTitle("ADC Channel");
                        }
                    }
                }
                Activedata = igemdata;
                Activedata.SeriesIndex = chart.AddSeries(activespectrum);
            }
            else// if an active spectrum exists, replace it with new one
            {
                if (igemdata.factor != 0)// the new active spectrum has been calibrated
                {
                    this.Check_EenergyD.Enabled = true;
                    if (this.Check_EenergyD.Checked == true) // displayed in kev mode
                    {
                        // Display Active spectrum with Energy Axis X
                        for (int i = 0; i < igemdata.iChannelNo; i++)
                        {
                            activespectrum.SetValue(SeriesComponent.X, i, igemdata.dEn[i]);
                        }
                    }
                }
                else// the new active spectrum has not been calibrated
                {
                    this.Check_EenergyD.Enabled = false;
                    if (this.Check_EenergyD.Checked == true)
                    {
                        this.Check_EenergyD.Checked = false;
                        chart.SetGridBottomTitle("ADC Channel");
                        if (Staticdata.SeriesIndex >= 0) // Static spectrum exists in kev mode
                        {
                            //adjust the Energy X Axis of Static Spectrum to ADC channel
                            Series staticspectrum = new Series();
                            staticspectrum.SeriesName = "Static spectrum";
                            for (int i = 0; i < Staticdata.iChannelNo; i++)
                            {
                                staticspectrum.SetValue(SeriesComponent.X, i, Staticdata.iChannel[i]);
                                staticspectrum.SetValue(SeriesComponent.Y, i, Staticdata.dCounts[i]);
                            }
                            chart.ReplaceSeries(Staticdata.SeriesIndex, staticspectrum);
                        }
                    }
                }
                igemdata.SeriesIndex = Activedata.SeriesIndex;
                Activedata = igemdata;
                chart.ReplaceSeries(Activedata.SeriesIndex, activespectrum);
            }
            // set Active Spectrum color to blue.
            chart.GetSeriesDrawing(Activedata.SeriesIndex).MarkerLine.Color = Color.Blue;
            this.Check_DisLive.Checked = true;
            //Draw it!
            smoothSpectrum();   //displays the spectrum whether it is smoothed or not    
        }

        //-------------------------set the active spectrum values in the UI.---------------------//
        private void setactivevalue(iGEMData igemdata)
        {
            //fill the parameter in Configure Form

            Cfg.LabelASN.Text = igemdata.sDSN;
            Cfg.LabelADT.Text = igemdata.sDT;
            Cfg.LabelAPT.Text = igemdata.sPeakT;
            Cfg.LabelAG.Text = igemdata.dGain;
            Cfg.LabelAB.Text = igemdata.sBias;
            Cfg.LabelALLD.Text = igemdata.sLLD;
            //fill the parameter in Main Form

            this.Label_CPS.Text = igemdata.sCPS;
            this.Label_TC.Text = igemdata.TC;
            this.Label_Time.Text = igemdata.time;
        }

        private void gsNetWinChart1_MouseClickChartEvent(object sender, ChartMouseEventArgs e)
        {
            if (this.TopLevel)// the Main Windows is on the top
            {
                double dx = e.AxisValues.XValue;
                double dy = e.AxisValues.YValue;
                if ((dx >= 0) && (dx <= 4095) && (dy > 0) && (dy < 100000))
                {
                    if (this.Check_EenergyD.Checked)
                    {
                        string s;
                        int i;
                        s = dx.ToString();
                        i = s.IndexOf(".");
                        if (i >= 0)
                            s = s.Substring(0, i + 2);
                        this.Label_ESC.Text = s;
                        dx = (dx - this.d) / this.ctoe;
                        if (dx < 0)  // prevent subscript out of range exceptions
                            dx = 0;
                        else if (dx > 4095)
                            dx = 4095;
                    }
                    else
                    {
                        this.Label_ESC.Text = (Conversions.ToInteger(dx)).ToString();
                    }

                    if (this.Check_SpectrumS.Checked == false)
                    {
                        if (Activedata.SeriesIndex >= 0)
                        {
                            string temp = Activedata.dCounts[Conversions.ToInteger(dx)].ToString();
                            if (Staticdata.SeriesIndex >= 0)
                            {
                                //choose the static or active point according the distance from the clicking points.
                                if (Math.Abs(Activedata.dCounts[Conversions.ToInteger(dx)] - dy) > Math.Abs(Staticdata.dCounts[Conversions.ToInteger(dx)] - dy))
                                {
                                    temp = Staticdata.dCounts[Conversions.ToInteger(dx)].ToString();
                                }
                            }
                            this.Label_CiS.Text = temp;
                        }
                        else
                        {
                            if (Staticdata.SeriesIndex >= 0)
                                this.Label_CiS.Text = Staticdata.dCounts[Conversions.ToInteger(dx)].ToString();
                        }
                    }
                    else
                    {
                        if (Activedata.SeriesIndex >= 0)
                        {
                            string temp = Activedata.dSmth[Conversions.ToInteger(dx)].ToString();
                            if (Staticdata.SeriesIndex >= 0)
                            {
                                //choose the static or active point according the distance from the clicking points.
                                if (Math.Abs(Activedata.dSmth[Conversions.ToInteger(dx)] - dy) > Math.Abs(Staticdata.dSmth[Conversions.ToInteger(dx)] - dy))
                                {
                                    temp = Staticdata.dSmth[Conversions.ToInteger(dx)].ToString();
                                }
                            }
                            this.Label_CiS.Text = temp;
                        }
                        else
                        {
                            if (Staticdata.SeriesIndex >= 0)
                                this.Label_CiS.Text = Staticdata.dSmth[Conversions.ToInteger(dx)].ToString();
                        }
                    }
                }
            }

            if (Cal.CanSelect)// Cal is visalbe
            {
                if (Cal.Option_P1.Checked == true)
                    Cal.Text_Ch1.Text = e.AxisValues.XValue.ToString("0.0");
                if (Cal.Option_P2.Checked == true)
                    Cal.Text_Ch2.Text = e.AxisValues.XValue.ToString("0.0");
            }
            if (fm.CanSelect)
            {
                double lower = -10;
                double upper = 0;

                if (fm.Option_RB.Checked == true)
                {
                    lower = e.AxisValues.XValue;
                    fm.Text_Ch1.Text = lower.ToString("0.0");
                }
                if (fm.Option_RE.Checked == true)
                {
                    upper = e.AxisValues.XValue;
                    fm.Text_Ch2.Text = upper.ToString("0.0");
                }
            }
        }

        //---------------------------paint the ROI------------------------------------//
        public bool paintROI()
        {
            double ch2;
            double ch1;
            try
            {
                ch2 = Conversions.ToDouble(fm.Text_Ch2.Text);
                ch1 = Conversions.ToDouble(fm.Text_Ch1.Text);
            }
            catch (Exception)
            {
                MessageBox.Show("Invalid Input for ROI!");
                return false;
            }
            if (ch2 > ch1 && ch1 >= 0)
            {
                ROI = true;
                FWHMCondition lineCond1 = new FWHMCondition(ch2, ch1);
                lineCond1.Line.Color = Color.Orange; // change line color 

                // add line condition to the series 
                Chart chart = gsNetWinChart1.Chart;
                if (Activedata.LineCon >= 0)
                    chart.GetSeriesDrawing(Activedata.SeriesIndex).RemoveLineCondition(Activedata.LineCon);

                Activedata.LineCon =
                    chart.GetSeriesDrawing(Activedata.SeriesIndex).AddLineCondition(lineCond1);

                // **************************************************** 
                // **** Important: we must refresh line conditions **** 
                // **************************************************** 
                chart.ReconcileLineConditions();

                // refresh chart 
                chart.RecalcLayout();
                return true;
            }
            else
            {
                MessageBox.Show("The Beginning of the ROI should be less than the of the End of ROI!");
                return false;
            }
        }

        private void Check_AutoScale_CheckStateChanged(object sender, EventArgs e)
        {
            if (Check_AutoScale.Checked == true)
            {
                this.TextBox_YMax.Enabled = false;
                this.TextBox_YMin.Enabled = false;
            }
            else
            {
                this.TextBox_YMax.Enabled = true;
                this.TextBox_YMin.Enabled = true;
            }
        }

        private void Check_XFull_CheckStateChanged(object sender, EventArgs e)
        {
            if (Check_XFull.Checked == true)
            {
                this.TextBox_XMax.Enabled = false;
                this.TextBox_XMin.Enabled = false;
            }
            else
            {
                this.TextBox_XMax.Enabled = true;
                this.TextBox_XMin.Enabled = true;
            }
        }

        private void Check_EenergyD_MouseUp(object sender, MouseEventArgs e)
        {
            Chart chart = this.gsNetWinChart1.Chart;  // for spectrum drawing
            AxisX axisX = chart.Grid.AxisX;
            double dtemp1;
            double dtemp2;
            //axisX.MinAxisValueUser = 0;
            if (this.Check_EenergyD.Checked == true)
            {
                chart.SetGridBottomTitle("Photon Energy(keV)");
                Label_FWHM.Text = (fwhm * Activedata.factor).ToString("0.0") + " KeV";
                fm.lblFWHM.Text = (fwhm * Activedata.factor).ToString("0.0") + " KeV";
                if (Activedata.SeriesIndex >= 0)
                {
                    dtemp1 = Activedata.factor;
                    dtemp2 = Activedata.intercept;
                }
                else
                {
                    dtemp1 = Staticdata.factor;
                    dtemp2 = Staticdata.intercept;
                }
                chl = Conversions.ToInteger((chl * dtemp1) + dtemp2);
                chr = Conversions.ToInteger((chr * dtemp1) + dtemp2);
                fm.Text_Ch1.Text = chl.ToString("0.0");
                fm.Text_Ch2.Text = chr.ToString("0.0");
            }
            else
            {
                this.gsNetWinChart1.Chart.SetGridBottomTitle("ADC Channel");
                Label_FWHM.Text = fwhm.ToString("0.0") + " Channels";
                fm.lblFWHM.Text = fwhm.ToString("0.0") + " Channels";
                if (Activedata.SeriesIndex >= 0)
                {
                    dtemp1 = Activedata.factor;
                    dtemp2 = Activedata.intercept;
                }
                else
                {
                    dtemp1 = Staticdata.factor;
                    dtemp2 = Staticdata.intercept;
                }
                chl = Conversions.ToInteger((chl - dtemp2) / dtemp1);
                chr = Conversions.ToInteger((chr - dtemp2) / dtemp1);
                fm.Text_Ch1.Text = chl.ToString("0");
                fm.Text_Ch2.Text = chr.ToString("0");
            }
            setAxis();
        }

        private void Button_SAS_Click(object sender, EventArgs e)
        {
            saveas();
        }

        private void saveas()
        {
            SaveFileDialog saveFileDialog1 = new SaveFileDialog();
            saveFileDialog1.Filter = "csv file|*.csv";
            saveFileDialog1.Title = "Save a CSV File";
            saveFileDialog1.RestoreDirectory = true;

            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                if (saveFileDialog1.FileName != "")
                {
                    // write file in ASCII
                    System.IO.StreamWriter sw =
                        new StreamWriter(saveFileDialog1.FileName, false, Encoding.ASCII);

                    // set the data in iGEMData
                    Activedata.sCPS = this.Label_CPS.Text;
                    Activedata.TC = this.Label_TC.Text;
                    Activedata.time = this.Label_Time.Text;

                    igemdata = new iGEMData();
                    igemdata = Activedata;

                    //write to file
                    igemdata.Writecsv(sw);

                    sw.Close();
                }
        }

        private void SaveActiveSpectrumToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveas();
        }

        //--------------------load the static spectrum--------------------//
        private void loadss()
        {
            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            Stream spectrum;

            openFileDialog1.Filter = "csv files (*.txt)|*.csv";
            openFileDialog1.Title = "Open a CSV file";

            openFileDialog1.RestoreDirectory = true;

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                if ((spectrum = openFileDialog1.OpenFile()) != null)
                {
                    spectrum.Close();
                    StreamReader sr = new StreamReader(openFileDialog1.FileName);
                    igemdata = new iGEMData();
                    igemdata.Readcsv(sr);
                    sr.Close();
                    if (Cfg.LabelCSN.Text != "")// Online mode
                    {
                        if (igemdata.sDSN != Cfg.LabelCSN.Text || igemdata.sDT != Cfg.LabelCDT.Text)//different hardware
                        {
                            DialogResult result;
                            result = MessageBox.Show("The spectrum loaded was not produced by this detector! To proceed press 'Yes'", "Detector Error", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation);
                            if (result != DialogResult.Yes)
                                return;
                            else
                            {
                                if (igemdata.sBias != Cfg.TextVB.Text || igemdata.sLLD != Cfg.TextLLD.Text || igemdata.sPeakT != Cfg.cmbPeakingTime.Text || igemdata.dGain != Cfg.cmbGain.Text)
                                {
                                    result = MessageBox.Show("Some configuration parameters of the active spectrum are not the same as the current detector! To proceed press 'Yes' ", "Detector Error", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation);
                                    if (result != DialogResult.Yes)
                                        return;
                                }
                            }
                        }
                        else// same hardware
                        {
                            if (igemdata.sBias != Cfg.TextVB.Text || igemdata.sLLD != Cfg.TextLLD.Text || igemdata.sPeakT != Cfg.cmbPeakingTime.Text || igemdata.dGain != Cfg.cmbGain.Text)
                            {
                                DialogResult result;
                                result = MessageBox.Show("Some configuration parameters of the active spectrum are not the same as the current detector! To proceed press 'Yes' ", "Detector Error", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation);
                                if (result != DialogResult.Yes)
                                    return;
                            }
                        }
                    }
                    //offline mode or don't care about the difference of iGEM
                    Button_CSS.Enabled = true;
                    drawstatic(igemdata);
                }
                else
                    MessageBox.Show("Static spectrum file could not be opened");
            }
        }
        private void LoadStaticSpectrumToolStripMenuItem_Click(object sender, EventArgs e)
        {
            loadss();
        }

        private void saveJPEG()
        {
            SaveFileDialog saveFileDialog1 = new SaveFileDialog();
            saveFileDialog1.Filter = "JPEG file|*.jpg";
            saveFileDialog1.Title = "Save a graph to JPEG File";
            saveFileDialog1.RestoreDirectory = true;
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                if (saveFileDialog1.FileName != "")
                {
                    // write graph to JPEG
                    GraphicsServer.GSNet.Charting.Chart chart = this.gsNetWinChart1.Chart;
                    chart.Export(this.CreateGraphics(), saveFileDialog1.FileName, System.Drawing.Imaging.ImageFormat.Jpeg);
                }
        }

        //--------------------draw the static spectrum-----------------//
        private void drawstatic(iGEMData igemdata)
        {
            // for spectrum drawing
            Chart chart = this.gsNetWinChart1.Chart;
            AxisX axisX = chart.Grid.AxisX;
            axisX.AxisMode = AxisMode.ValueLinear;
            Series staticspectrum = new Series();
            for (int i = 0; i < igemdata.iChannelNo; i++)
            {
                staticspectrum.SetValue(SeriesComponent.X, i, igemdata.iChannel[i]);
                staticspectrum.SetValue(SeriesComponent.Y, i, igemdata.dCounts[i]);
            }
            //Add the new data series to the chart
            if (Staticdata.SeriesIndex < 0) // if no static spectrum exists, add a new one
            {
                if (Activedata.SeriesIndex < 0) //no active spectrum exists
                {
                    if (igemdata.factor > 0)
                        this.Check_EenergyD.Enabled = true;
                    else
                        this.Check_EenergyD.Enabled = false;
                }
                else
                {
                    if (this.Check_EenergyD.Enabled == true & this.Check_EenergyD.Checked == true)
                    {
                        if (igemdata.factor > 0 || Activedata.factor > 0)
                        {
                            for (int i = 0; i < igemdata.iChannelNo; i++)
                            {
                                staticspectrum.SetValue(SeriesComponent.X, i, igemdata.dEn[i]);
                            }
                        }
                        else
                        {
                            MessageBox.Show("The static spectrum has not been calibrated", "KeV mode Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            return;
                        }
                    }
                }
                Staticdata = igemdata;
                Staticdata.SeriesIndex = chart.AddSeries(staticspectrum);
                this.Check_DisStatic.Enabled = true;
            }
            else // if one static spectrum exits, replace it with new one.
            {
                if (Activedata.SeriesIndex < 0) //no active spectrum exists
                {
                    if (igemdata.factor > 0)// calibrated
                    {
                        if (this.Check_EenergyD.Enabled == true & this.Check_EenergyD.Checked == true)// shown in kev mode
                            for (int i = 0; i < igemdata.iChannelNo; i++)
                            {
                                staticspectrum.SetValue(SeriesComponent.X, i, igemdata.dEn[i]);
                            }
                        else
                            this.Check_EenergyD.Enabled = true;
                    }
                    else
                    {
                        this.Check_EenergyD.Enabled = false;
                        this.Check_EenergyD.Checked = false;
                        this.gsNetWinChart1.Chart.SetGridBottomTitle("ADC Channel");
                    }
                }
                else// active specturm exists.
                {
                    if (this.Check_EenergyD.Enabled == true & this.Check_EenergyD.Checked == true)
                    {
                        if (igemdata.factor > 0)
                        {
                            for (int i = 0; i < igemdata.iChannelNo; i++)
                            {
                                staticspectrum.SetValue(SeriesComponent.X, i, igemdata.dEn[i]);
                            }
                        }
                        else
                        {
                            MessageBox.Show("The static spectrum has not been calibrated", "Calibration Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                            return;
                        }
                    }
                }
                chart.ReplaceSeries(Staticdata.SeriesIndex, staticspectrum);
                igemdata.SeriesIndex = Staticdata.SeriesIndex;
                Staticdata = igemdata;
            }
            // set Static Spectrum color to Green.
            chart.GetSeriesDrawing(Staticdata.SeriesIndex).MarkerLine.Color = Color.Green;
            this.Check_DisStatic.Checked = true;
            //Draw it!
            smoothSpectrum();   //displays the spectrum whether it is smoothed or not    

            //fill the parameter in Configure Form
            Cfg.LabelSSN.Text = igemdata.sDSN;
            Cfg.LabelSDT.Text = igemdata.sDT;
            Cfg.LabelSPT.Text = igemdata.sPeakT;
            Cfg.LabelSG.Text = igemdata.dGain;
            Cfg.LabelSB.Text = igemdata.sBias;
            Cfg.LabelSLLD.Text = igemdata.sLLD;
            if (Activedata.SeriesIndex < 0)
            {
                this.Label_CiS.Text = "0";
                this.Label_CPS.Text = "0";
                this.Label_TC.Text = "0";
                this.Label_Time.Text = "00:00:00";
            }
        }

        //*--------------------------------------------------
        //*----------------Load the active spectrum---------
        //*--------------------Shen Yue 2007-2-7------------

        private void loadas()
        {
            OpenFileDialog openFileDialog1 = new OpenFileDialog();
            Stream spectrum;

            openFileDialog1.Filter = "csv files (*.txt)|*.csv";
            openFileDialog1.Title = "Open a CSV file";
            openFileDialog1.RestoreDirectory = true;

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                if ((spectrum = openFileDialog1.OpenFile()) != null)
                {
                    spectrum.Close();
                    StreamReader sr = new StreamReader(openFileDialog1.FileName);
                    igemdata = new iGEMData();
                    igemdata.Readcsv(sr);// put the data into igemdata
                    sr.Close();
                    if (Cfg.LabelCSN.Text != "")// Online mode
                    {
                        if (igemdata.sDSN != Cfg.LabelCSN.Text || igemdata.sDT != Cfg.LabelCDT.Text)//different hardware
                        {
                            DialogResult result;
                            result = MessageBox.Show("The spectrum loaded was not produced by this detector! To proceed press 'Yes'", "Detector Error", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation);
                            if (result != DialogResult.Yes)
                                return;
                            else
                            {
                                if (igemdata.sBias != Cfg.TextVB.Text || igemdata.sLLD != Cfg.TextLLD.Text || igemdata.sPeakT != Cfg.cmbPeakingTime.Text || igemdata.dGain != Cfg.cmbGain.Text)
                                {
                                    result = MessageBox.Show("Some configuration parameters of the active spectrum are not same as the current detector! To proceed press 'Yes' ", "Detector Error", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation);
                                    if (result != DialogResult.Yes)
                                        return;
                                }
                            }
                        }
                        else// same hardware
                        {
                            if (igemdata.sBias != Cfg.TextVB.Text || igemdata.sLLD != Cfg.TextLLD.Text || igemdata.sPeakT != Cfg.cmbPeakingTime.Text || igemdata.dGain != Cfg.cmbGain.Text)
                            {
                                DialogResult result;
                                result = MessageBox.Show("Some configuration parameters of the active spectrum are not same as the current detector! To proceed press 'Yes'", "Detector Error", MessageBoxButtons.YesNo, MessageBoxIcon.Exclamation);
                                if (result != DialogResult.Yes)
                                    return;
                            }
                        }
                    }
                    //offline mode or don't care about the difference of iGEM
                    setactivevalue(igemdata);
                    drawactive(igemdata);
                    elapsedTime = TimeSpan.Parse(igemdata.time);
                }
                else
                    MessageBox.Show("Active spectrum file could not be opened");
            }
        }

        private void LoadActiveSpectrumToolStripMenuItem_Click(object sender, EventArgs e)
        {
            loadas();
            setfunction();
        }

        private void Check_DisLive_CheckStateChanged(object sender, EventArgs e)
        {
            if (Activedata.SeriesIndex >= 0)
            {
                if (Check_DisLive.Checked == true)
                {
                    this.gsNetWinChart1.Chart.GetSeriesDrawing(Activedata.SeriesIndex).MarkerLine.Transparency = 0;
                    if (ROI)
                        paintROI();
                }
                if (Check_DisLive.Checked == false)
                {
                    this.gsNetWinChart1.Chart.GetSeriesDrawing(Activedata.SeriesIndex).MarkerLine.Transparency = 100;
                    if (ROI)
                    {
                        if (Activedata.LineCon >= 0)
                            this.gsNetWinChart1.Chart.GetSeriesDrawing(Activedata.SeriesIndex).RemoveLineCondition(Activedata.LineCon);
                        this.gsNetWinChart1.Chart.ReconcileLineConditions();
                    }
                }
                this.gsNetWinChart1.Chart.RecalcLayout();
            }
        }

        private void Check_DisStatic_CheckedChanged(object sender, EventArgs e)
        {
            if (Staticdata.SeriesIndex >= 0)
            {
                if (Check_DisStatic.Checked == true)
                {
                    this.gsNetWinChart1.Chart.GetSeriesDrawing(Staticdata.SeriesIndex).MarkerLine.Transparency = 0;
                }
                if (Check_DisStatic.Checked == false)
                {
                    this.gsNetWinChart1.Chart.GetSeriesDrawing(Staticdata.SeriesIndex).MarkerLine.Transparency = 100;
                }
                this.gsNetWinChart1.Chart.RecalcLayout();
            }
        }



        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            e.Cancel = false; // cancel is canceled
        }

        private void timerspectrum_Tick(object sender, EventArgs e)
        {
            DateTime endTime = DateTime.Now;
            TimeSpan span = endTime.Subtract(startTime);

            //            DateTime chartStart, chartEnd;
            //            chartStart = DateTime.Now;

            string ct;
            bool stopped = false; // indicates spectrum collection has stopped during this timer tick
            int d = span.Days;
            int h = (d * 24) + span.Hours;
            int m = span.Minutes;
            int s = span.Seconds;
            if (h < 10)
                ct = "0" + h + ":";
            else
                ct = h + ":";
            if (m < 10)
                ct += "0" + m + ":";
            else
                ct += m + ":";
            if (s < 10)
                ct += "0" + s;
            else
                ct += s;

            if ((h * 3600 + m * 60 + s) >= totaltime && totaltime > 0)
            {
                stop();
                stopped = true;
            }

            ulong total = 0;
            int channelNo = Activedata.iChannelNo;
            for (int i = 0; i < channelNo; i++)
            {
                total += (ulong)Activedata.dCounts[i];
            }
            if (((int)total > totalcounts) && (totalcounts > 0))
            {
                stop();
                stopped = true;
            }

            this.Label_Time.Text = ct;

            if (updateSpectrum == false && stopped == false) // update spectrum every other second
            {
                updateSpectrum = true;
                return;
            }
            updateSpectrum = false;

            ulong currentInterval;
            ulong cps;
            if (prevTotal4 != 0)
                cps = (total - prevTotal4) >> 3;  // divide by 8 seconds
            else
            {
                currentInterval = (ulong)total - prevTotal1;
                cps = (currentInterval) >> 1;               // divide by 2 seconds
            }
            this.Label_CPS.Text = cps.ToString();
            this.Label_TC.Text = total.ToString();
            prevTotal4 = prevTotal3;
            prevTotal3 = prevTotal2;
            prevTotal2 = prevTotal1;
            prevTotal1 = (ulong)total;

            Chart chart = this.gsNetWinChart1.Chart;
            int n = 0;
            int points = 0;
            int startPoint = 0;
            int endPoint = 0;
            int decimate = 0;
            int step = 0;
            if (Activedata.SeriesIndex < 0)
            {
                //instantiate the Active Data
                Activedata.sDSN = this.Cfg.LabelCSN.Text;
                Activedata.sDT = this.Cfg.LabelCDT.Text;
                Activedata.sBias = this.Cfg.TextVB.Text;
                Activedata.sLLD = this.Cfg.TextLLD.Text;
                Activedata.sPeakT = this.Cfg.cmbPeakingTime.Text;
                Activedata.dGain = this.Cfg.cmbGain.Text;
                if (this.Check_XFull.Checked == true)
                {
                    startPoint = 0;
                    endPoint = 4095;
                    decimate = 2;
                    points = endPoint - startPoint + 1;
                }
                else
                {
                    startPoint = xAxisMin;
                    endPoint = xAxisMax;
                    points = endPoint - startPoint + 1;
                    if (points < 1)
                    {
                        startPoint = 0;
                        endPoint = Activedata.iChannelNo >> 2;
                        decimate = 2;
                    }
                    else if (points > 2047)
                    {
                        decimate = 2;
                    }
                    else if (points > 1023)
                    {
                        decimate = 1;
                    }
                    else
                    {
                        decimate = 0;
                    }
                }
                step = 1 << decimate;
                points = (points >> decimate);
                int j = startPoint;
                for (int i = 0; i < points; i++)
                {
                    if (Activedata.dCounts[i] > 1)
                        n++;
                    activespectrum.SetValue(SeriesComponent.Y, i, Activedata.dCounts[j]);
                    activespectrum.SetValue(SeriesComponent.X, i, Activedata.iChannel[j]);
                    j += step;
                }
                Activedata.SeriesIndex = chart.AddSeries(activespectrum);
            }
            else
            {
                if (this.Check_XFull.Checked == true)
                {
                    startPoint = 0;
                    endPoint = 4095;
                    decimate = 2;
                    points = endPoint - startPoint + 1;
                }
                else
                {
                    startPoint = xAxisMin;
                    endPoint = xAxisMax;
                    points = endPoint - startPoint + 1;
                    if (points < 1)
                    {
                        startPoint = 0;
                        endPoint = Activedata.iChannelNo >> 2;
                        decimate = 2;
                    }
                    else if (points > 2047)
                    {
                        decimate = 2;
                    }
                    else if (points > 1023)
                    {
                        decimate = 1;
                    }
                    else
                    {
                        decimate = 0;
                    }
                }
                step = 1 << decimate;
                points = (points >> decimate);
                int j = startPoint;
                for (int i = 0; i < points; i++)
                {
                    if (Activedata.dCounts[i] > 1)
                        n++;
                    activespectrum.SetValue(SeriesComponent.Y, i, Activedata.dCounts[j]);
                    activespectrum.SetValue(SeriesComponent.X, i, Activedata.iChannel[j]);
                    j += step;
                }
                chart.ReplaceSeries(Activedata.SeriesIndex, activespectrum);
            }
            chart.GetSeriesDrawing(Activedata.SeriesIndex).MarkerLine.Color = Color.Blue;
            chart.GetSeriesDrawing(Activedata.SeriesIndex).MarkerLabelsOn = false;
            chart.GetSeriesDrawing(Activedata.SeriesIndex).SymbolsOn = false;

            //Draw it!
            //                chartEnd = DateTime.Now;
            chart.RecalcLayout();
            /*                timerspectrum.Enabled = false;
                        timerCollect.Enabled = false;
                        TimeSpan span2 = chartEnd.Subtract(chartStart);
                        int sec = span2.Seconds;
                        int mil = span2.Milliseconds;
                        ct = "milliseconds =" + ((sec * 1000) + mil);
                        MessageBox.Show(ct);
                        timerspectrum.Enabled = true;
                        timerCollect.Enabled = true; */
        }

        // Stop collecting and set the proper status of UI
        private void stop()
        {
            if (collecting == true)
            {
                endTime = DateTime.Now;
                elapsedTime = endTime.Subtract(startTime);
            }
            collecting = false;
            this.Button_Start.Enabled = true;
            this.Button_Stop.Enabled = false;
            this.timerspectrum.Enabled = false;
            this.Check_SpectrumS.Enabled = true;
            this.LoadStaticSpectrumToolStripMenuItem.Enabled = true;
            this.LoadActiveSpectrumToolStripMenuItem.Enabled = true;
            this.SaveActiveSpectrumToolStripMenuItem.Enabled = true;
            this.ClearActiveSpectrumToolStripMenuItem.Enabled = true;
            this.Button_CAS.Enabled = true;
            this.Button_SAS.Enabled = true;
            this.Check_DisLive.Enabled = true;
            if (this.Activedata.factor != 0)
                this.Check_EenergyD.Enabled = true;
        }

        private void Button_Stop_Click(object sender, EventArgs e)
        {
            stop();
        }

        //enable or disable some funtions in menu according the operation, after the clear spectrum or load spectrum.
        private void setfunction()
        {
            if (Activedata.SeriesIndex < 0)
            {
                this.SaveActiveSpectrumToolStripMenuItem.Enabled = false;
                this.ClearActiveSpectrumToolStripMenuItem.Enabled = false;
                this.Button_CAS.Enabled = false;
                this.Button_SAS.Enabled = false;
                this.Check_DisLive.Enabled = false;
                if (Staticdata.SeriesIndex < 0 || Staticdata.factor == 0)
                {
                    this.Check_EenergyD.Checked = false;
                    this.Check_EenergyD.Enabled = false;
                }
            }
            else
            {
                this.SaveActiveSpectrumToolStripMenuItem.Enabled = true;
                this.ClearActiveSpectrumToolStripMenuItem.Enabled = true;
                this.Button_CAS.Enabled = true;
                this.Button_SAS.Enabled = true;
                this.Check_DisLive.Enabled = true;
            }
            if (Staticdata.SeriesIndex < 0)
            {
                this.ClearStaticSpectrumToolStripMenuItem.Enabled = false;
                this.Button_CSS.Enabled = false;
                this.Check_DisStatic.Enabled = false;
            }
            else
            {
                this.ClearStaticSpectrumToolStripMenuItem.Enabled = true;
                this.Button_CSS.Enabled = true;
                this.Check_DisStatic.Enabled = true;
            }
        }

        private void Button_CAS_Click(object sender, EventArgs e)
        {
            if (Button_Stop.Enabled == true)
                MessageBox.Show("To clear the spectrum, first press the Stop button");
            else
            {
                Clear();
                setfunction();
            }
        }

        private void Button_LAS_Click(object sender, EventArgs e)
        {
            loadas();
            setfunction();
        }

        private void Button_LSS_Click(object sender, EventArgs e)
        {
            loadss();
        }

        private void FWHMToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Activedata.SeriesIndex >= 0)
            {
                this.gsNetWinChart1.Cursor = Cursors.Cross;
                this.AddOwnedForm(fm); // for top show
                fm.Show();
            }
            else
            {
                MessageBox.Show("A FWHM can only be calculated for the active spectrum");
            }
        }

        //-------------------------for transferring the data from DODM--------------------------------//
        private void timerCollect_Tick(object sender, EventArgs e)
        {
            //disable the time
            timerCollect.Enabled = false;
            if (collecting == false)
            {
                if (!Com.Cls())
                    MessageBox.Show("An error occurred closing communications with iGEM");
            }
            else
            {
                // Initialize for the reading data
                if (Nextstep == 0)
                {
                    File.AppendAllText("iSpectrum.log", "Nextstep: " + Nextstep.ToString() + Environment.NewLine);
                    if (!Com.Initial())
                    {
                        File.AppendAllText("iSpectrum.log", "Initial false!" + Environment.NewLine);
                        stop();
                        return;
                    }
                    else
                    {
                        if (!Com.Snd5())
                        {
                            File.AppendAllText("iSpectrum.log", "Snd5 failed!" + Environment.NewLine);
                            stop();
                            return;
                        }
                        else
                            Nextstep = 1;
                    }
                }
                // send command 4
                if (Nextstep == 1)
                {

                    if (!Com.Snd4())
                    {
                        File.AppendAllText("iSpectrum.log", "Snd4 false!" + Environment.NewLine);
                        stop();
                        return;
                    }
                    else
                    {
                        Nextstep = 2;
                        timerCollect.Enabled = true;
                        return;
                    }
                }
                // receive the reply of cmd4 and send command 3s
                if (Nextstep == 2)
                {
                    if (this.Label_TC.Text != "")
                    {
                        if (Conversions.ToInteger(this.Label_TC.Text) % 100000 == 0)
                        {
                            //                            File.AppendAllText("iSpectrum.log", this.Label_TC.Text + Environment.NewLine);
                        }
                    }
                    int result = Com.Retrieve3();
                    if (result == 0)
                    {
                        File.AppendAllText("iSpectrum.log", "Retrieve3 result=0!" + Environment.NewLine);
                        stop();
                        return;
                    }
                    if (result == 1)
                    {
                        timerCollect.Enabled = true;
                        Nextstep = 1;
                        return;
                    }
                    if (result == 3)
                    {
                        Nextstep = 3;
                        result = Com.Retrievedata(ref Activedata);
                        if (result == 1)
                        {
                            Nextstep = 2;// for cmd4 has been sent in Retrievedata, here nextstep is 2
                            timerCollect.Enabled = true;
                            return;
                        }
                        else
                        {
                            File.AppendAllText("iSpectrum.log", "Retrievedata  result=" + result.ToString() + Environment.NewLine);
                            stop();
                            return;
                        }
                    }
                    return;
                }

                // retrieve the events 
                if (Nextstep == 3)
                {
                    int result = Com.Retrievedata(ref Activedata);
                    if (result == 1)
                    {
                        Nextstep = 2;
                        timerCollect.Enabled = true;
                        return;
                    }
                    else
                    {
                        File.AppendAllText("iSpectrum.log", "Retrievedata  result=" + result.ToString() + Environment.NewLine);
                        stop();
                        return;
                    }
                }
            }
        }

        private void checkBoxTC_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxTC.Checked == true)
            {
                try
                {
                    if (Conversions.ToInteger(this.TextBox_Counts.Text) > 10000000 || Conversions.ToInteger(this.TextBox_Counts.Text) < 0)
                    {
                        MessageBox.Show("The valid range of counts number is 0 to 10,000,000");
                        this.TextBox_Counts.Text = "0";
                        checkBoxTC.Checked = false;
                    }
                    else
                        totalcounts = Conversions.ToInteger(this.TextBox_Counts.Text);
                }
                catch (Exception)
                {
                    MessageBox.Show("The valid range of counts number is 0 to 10,000,000");
                    this.TextBox_Counts.Text = "0";
                    checkBoxTC.Checked = false;
                }
            }
            else
            {
                totalcounts = 0;
            }
        }

        private void checkBoxCT_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxCT.Checked == true)
            {
                try
                {
                    if (Conversions.ToInteger(this.TextBox_Time.Text) > 99999 || Conversions.ToInteger(this.TextBox_Time.Text) < 0)
                    {
                        MessageBox.Show("The valid range of time is 0 to 99999 seconds");
                        this.TextBox_Time.Text = "0";
                        checkBoxCT.Checked = false;
                    }
                    else
                        totaltime = Conversions.ToInteger(this.TextBox_Time.Text);
                }
                catch (Exception)
                {
                    MessageBox.Show("The valid range of time is 0 to 9999 seconds");
                    this.TextBox_Time.Text = "0";
                    checkBoxCT.Checked = false;
                }
            }
            else
            {
                totaltime = 0;
            }
        }

        private void TextBox_Time_KeyDown(object sender, KeyEventArgs e)
        {
            nonNumberEntered = false;
            if (e.KeyCode < Keys.D0 || e.KeyCode > Keys.D9)
            {
                if (e.KeyCode < Keys.NumPad0 || e.KeyCode > Keys.NumPad9)
                    if (e.KeyCode != Keys.Back)
                        nonNumberEntered = true;
            }
        }

        private void TextBox_Time_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (this.checkBoxCT.Checked == true)
                this.checkBoxCT.Checked = false;
            if (nonNumberEntered == true)
                e.Handled = true;
        }

        private void TextBox_Counts_KeyDown(object sender, KeyEventArgs e)
        {
            nonNumberEntered = false;
            if (e.KeyCode < Keys.D0 || e.KeyCode > Keys.D9)
            {
                if (e.KeyCode < Keys.NumPad0 || e.KeyCode > Keys.NumPad9)
                    if (e.KeyCode != Keys.Back)
                        nonNumberEntered = true;
            }
        }

        private void TextBox_Counts_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (this.checkBoxTC.Checked == true)
                this.checkBoxTC.Checked = false;
            if (nonNumberEntered == true)
                e.Handled = true;
        }

        private void TextBox_YMax_KeyDown(object sender, KeyEventArgs e)
        {
            nonNumberEntered = false;
            if (e.KeyCode < Keys.D0 || e.KeyCode > Keys.D9)
            {
                if (e.KeyCode < Keys.NumPad0 || e.KeyCode > Keys.NumPad9)
                    if (e.KeyCode != Keys.Back)
                        nonNumberEntered = true;
            }
        }

        private void TextBox_YMax_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (nonNumberEntered == true)
                e.Handled = true;
        }

        private void TextBox_YMin_KeyDown(object sender, KeyEventArgs e)
        {
            nonNumberEntered = false;
            if (e.KeyCode < Keys.D0 || e.KeyCode > Keys.D9)
            {
                if (e.KeyCode < Keys.NumPad0 || e.KeyCode > Keys.NumPad9)
                    if (e.KeyCode != Keys.Back)
                        nonNumberEntered = true;
            }
        }

        private void TextBox_YMin_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (nonNumberEntered == true)
                e.Handled = true;
        }

        private void TextBox_XMax_KeyDown(object sender, KeyEventArgs e)
        {
            nonNumberEntered = false;
            if (e.KeyCode < Keys.D0 || e.KeyCode > Keys.D9)
            {
                if (e.KeyCode < Keys.NumPad0 || e.KeyCode > Keys.NumPad9)
                    if (e.KeyCode != Keys.Back)
                        if (e.KeyValue != 190 && e.KeyCode != Keys.Decimal)
                            nonNumberEntered = true;
            }
        }

        private void TextBox_XMax_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (nonNumberEntered == true)
                e.Handled = true;
        }

        private void TextBox_XMin_KeyDown(object sender, KeyEventArgs e)
        {
            nonNumberEntered = false;
            if (e.KeyCode < Keys.D0 || e.KeyCode > Keys.D9)
            {
                if (e.KeyCode < Keys.NumPad0 || e.KeyCode > Keys.NumPad9)
                    if (e.KeyCode != Keys.Back)
                        if (e.KeyValue != 190 && e.KeyCode != Keys.Decimal)
                            nonNumberEntered = true;
            }
        }

        private void TextBox_XMin_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (nonNumberEntered == true)
                e.Handled = true;
        }

        private void Check_EenergyD_CheckedChanged(object sender, EventArgs e)
        {
            if ((this.TextBox_XMin.Text != "") && (this.TextBox_XMax.Text != ""))
            {
                double min, minConverted;
                double max, maxConverted;
                try
                {
                    min = (Conversions.ToDouble(this.TextBox_XMin.Text));
                    max = (Conversions.ToDouble(this.TextBox_XMax.Text));
                }
                catch (Exception)
                {
                    return;
                }
                if (Activedata.SeriesIndex >= 0)
                {
                    if (Check_EenergyD.Checked)
                    {
                        minConverted = (min * Activedata.factor) + Activedata.intercept;
                        if (minConverted < 0)
                            minConverted = 0;
                        maxConverted = (max * Activedata.factor) + Activedata.intercept;
                        this.TextBox_XMin.Text = minConverted.ToString("0.00");
                        this.TextBox_XMax.Text = maxConverted.ToString("0.00");
                    }
                    else
                    {
                        if (Activedata.factor > 0)
                        {
                            minConverted = (min - Activedata.intercept) / Activedata.factor;
                            if (minConverted < 0)
                                minConverted = 0;
                            maxConverted = (max - Activedata.intercept) / Activedata.factor;
                        }
                        else
                        {
                            minConverted = 0;
                            maxConverted = 4095;
                        }
                        this.TextBox_XMin.Text = minConverted.ToString("0");
                        this.TextBox_XMax.Text = maxConverted.ToString("0");
                    }
                }
                else if (Staticdata.SeriesIndex >= 0)
                {
                    if (Check_EenergyD.Checked)
                    {
                        minConverted = (min * Staticdata.factor) + Staticdata.intercept;
                        if (minConverted < 0)
                            minConverted = 0;
                        maxConverted = (max * Staticdata.factor) + Staticdata.intercept;
                        this.TextBox_XMin.Text = minConverted.ToString("0.00");
                        this.TextBox_XMax.Text = maxConverted.ToString("0.00");
                    }
                    else
                    {
                        minConverted = (min - Staticdata.intercept) / Staticdata.factor;
                        if (minConverted < 0)
                            minConverted = 0;
                        maxConverted = (max - Staticdata.intercept) / Staticdata.factor;
                        this.TextBox_XMin.Text = minConverted.ToString("0");
                        this.TextBox_XMax.Text = maxConverted.ToString("0");
                    }
                }
            }
        }

        private void saveToJPEGToolStripMenuItem_Click(object sender, EventArgs e)
        {
            saveJPEG();
        }

        private void timerHVRamp_Tick(object sender, EventArgs e)
        {
            bool status;
            byte cmd = 2;
            byte p2;
            byte p1;
            int i;
            if (Math.Abs(targetHVSetting - currentHVSetting) <= 100)
            {
                timerHVRamp.Enabled = false;
                i = targetHVSetting / 2;  //set value p1 is high
                if ((i % 256) > 255)
                    p2 = 255;
                else
                    p2 = Conversions.ToByte(i % 256);
                i = i - (i % 256);
                p1 = Conversions.ToByte(i / 256);
                currentHVSetting = targetHVSetting;
                status = Com.SndRcv(cmd, ref p1, ref p2);
                this.Button_Start.Text = "Start";
                this.Button_Start.Enabled = true;
                if (status == false)
                {
                    timerHVRamp.Enabled = false;
                    MessageBox.Show("HV Ramp Failed");
                }
            }
            else
            {
                if (targetHVSetting > currentHVSetting)
                    currentHVSetting += 100;
                else
                    currentHVSetting -= 100;
                i = currentHVSetting / 2;
                if ((i % 256) > 255)
                    p2 = 255;
                else
                    p2 = Conversions.ToByte(i % 256);
                i = i - (i % 256);
                p1 = Conversions.ToByte(i / 256);
                status = Com.SndRcv(cmd, ref p1, ref p2);
                if (status == false)
                {
                    timerHVRamp.Enabled = false;
                    MessageBox.Show("HV Ramp Failed");
                }
            }
        }

        private void IndexToolStripMenuItem_Click(object sender, EventArgs e)
        {
            string path = Directory.GetCurrentDirectory();
            path = path + "/Help/Frameset.html";
            Help.ShowHelp(this, path);
        }

        private void Button_CSS_Click(object sender, EventArgs e)
        {
            if (Button_Stop.Enabled == true)
                MessageBox.Show("To clear the spectrum, first press the Stop button");
            else
            {
                ClearStatic();
                setfunction();
            }
        }

        //----------------------- remove the Active spectrum and reset all the parameters in UI---------------//
        public void ClearStatic()
        {
            gsNetWinChart1.Chart.SeriesDrawingList.RemoveAt(Staticdata.SeriesIndex);
            if (Activedata.SeriesIndex == 1)
                Activedata.SeriesIndex = 0;
            this.gsNetWinChart1.Chart.RecalcLayout();
            Staticdata.Reset();
        }

        private void clearStaticSpectrumToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ClearStatic();
            setfunction();
        }

        private void Check_AutoScale_CheckedChanged(object sender, EventArgs e)
        {
            setAxis();
        }

        private void Check_XFull_CheckedChanged(object sender, EventArgs e)
        {
            setAxis();
        }

        private void Option_Log_CheckedChanged(object sender, EventArgs e)
        {
            setAxis();
        }

        private void Option_Linear_CheckedChanged(object sender, EventArgs e)
        {
            setAxis();
        }
    }
}