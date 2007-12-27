using System;
using System.Collections.Generic;
using System.Text;
using System.Globalization;
using System.Windows.Forms;
using System.IO.Ports;
using System.Net;

namespace GentagDemo
{
    class nmeaInterpreter
    {
        #region Member Variables

        private string deviceUID;

        private System.Threading.Timer reportGPSTimer;

        const int checkGPSRate = 30;

        const int minimumGPSCoordinatesToReport = 30;

        private System.IO.Ports.SerialPort gpsSerialPort = new System.IO.Ports.SerialPort();

        // Represents the EN-US culture, used for numbers in NMEA sentences
        private static CultureInfo NmeaCultureInfo = new CultureInfo("en-US");
        // Used to convert knots into miles per hour
        private static float MPHPerKnot = float.Parse("1.150779", NmeaCultureInfo);

        private const double maximumHDOPValue = 7.0;

        private float currentLatitude;

        private float currentLongitude;

        private float currentSpeed;

        private int currentElevation;

        private float currentBearing;

        private DateTime lastGPSUpdateTime = new DateTime(0);

        private double currentHDOPValue = 15;

        private double currentPDOPValue;

        private double currentVDOPValue;

        private bool trackingGPS;
#endregion

        #region Delegates
        public delegate void PositionReceivedEventHandler(string latitude, string longitude);
        public delegate void DateTimeChangedEventHandler(System.DateTime dateTime);
        public delegate void BearingReceivedEventHandler(double bearing);
        public delegate void SpeedReceivedEventHandler(double speed);        
        public delegate void FixObtainedEventHandler();
        public delegate void FixLostEventHandler();
        public delegate void SatelliteReceivedEventHandler(int pseudoRandomCode, int azimuth, int elevation, int signalToNoiseRatio, bool firstMessage);
        public delegate void HDOPReceivedEventHandler(double value);
        public delegate void VDOPReceivedEventHandler(double value);
        public delegate void PDOPReceivedEventHandler(double value);
        public delegate void NumberOfSatellitesInViewEventHandler(int numSats);
        public delegate void SetQueuedRequestsEventHandler(int queueSize, int maxQueueSize);
        #endregion

        #region Events
        public event PositionReceivedEventHandler PositionReceived;
        public event DateTimeChangedEventHandler DateTimeChanged;
        public event BearingReceivedEventHandler BearingReceived;
        public event SpeedReceivedEventHandler SpeedReceived;        
        public event FixObtainedEventHandler FixObtained;
        public event FixLostEventHandler FixLost;
        public event SatelliteReceivedEventHandler SatelliteReceived;
        public event HDOPReceivedEventHandler HDOPReceived;
        public event VDOPReceivedEventHandler VDOPReceived;
        public event PDOPReceivedEventHandler PDOPReceived;
        public event NumberOfSatellitesInViewEventHandler NumSatsReceived;
        public event SetQueuedRequestsEventHandler QueueUpdated;
        #endregion

        public nmeaInterpreter(CultureInfo cultureInfo, string deviceUniqueID)
        {
            reportGPSTimer =
                new System.Threading.Timer(reportGPSPosition, null, checkGPSRate * 1000, checkGPSRate * 1000);
           
            deviceUID = deviceUniqueID;

            NmeaCultureInfo = cultureInfo;

            gpsSerialPort.BaudRate = 4800;
            gpsSerialPort.PortName = "COM7";
            gpsSerialPort.ReadBufferSize = 8192;
            gpsSerialPort.ReceivedBytesThreshold = 512;
            gpsSerialPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(gpsSerialPort_DataReceived);
            
        }

        public void Open(string comPort)
        {
            gpsSerialPort.PortName = comPort;
            gpsSerialPort.Open();
        }

        public void Close()
        {
            gpsSerialPort.Close();
        }

        public bool IsOpen()
        {
            return gpsSerialPort.IsOpen;
        }

        public void setTracking(bool isTracking)
        {
            trackingGPS = isTracking;
        }

        public bool IsTracking()
        {
            return trackingGPS;
        }

        /// <summary>
        /// Get the latitude
        /// </summary>
        /// <returns>float representing current latitude</returns>
        /// <throws>NotSupportedException</throws>
        public float getLatitude()
        {
            //if (lastGPSUpdateTime.Ticks == 0)
            //    throw new NotSupportedException("No GPS values yet");
            return currentLatitude;
        }

        /// <summary>
        /// Get the longitude
        /// </summary>
        /// <returns>float representing current longitude</returns>
        /// <throws>NotSupportedException</throws>
        public float getLongitude()
        {
            //if (lastGPSUpdateTime.Ticks == 0)
            //    throw new NotSupportedException("No GPS values yet");
            return currentLongitude;
        }

        // Processes information from the GPS receiver
        private bool Parse(string sentence)
        {
            // Discard the sentence if its checksum 
            // does not match our calculated checksum
            if (!IsValid(sentence)) return false;
            // Look at the first word to decide where to go next
            switch (GetWords(sentence)[0])
            {
                case "$GPRMC":
                    // A "Recommended Minimum" sentence was found!
                    return ParseGPRMC(sentence);
                case "$GPGSV":
                    // A "Satellites in View" sentence was received
                    return ParseGPGSV(sentence);
                case "$GPGSA":
                    return ParseGPGSA(sentence);
                default:
                    // Indicate that the sentence was not recognized
                    return false;
            }
        }

        private static char[] splitter = new char[]{ ',', '*'};

        // Divides a sentence into individual words
        private static string[] GetWords(string sentence)
        {
            return sentence.Split(splitter);
        }

        // Interprets a $GPRMC message
        private bool ParseGPRMC(string sentence)
        {
            // Divide the sentence into words
            string[] Words = GetWords(sentence);

            // Do we have enough values to describe our location?
            if (!string.IsNullOrEmpty(Words[3]) &&
                !string.IsNullOrEmpty(Words[4]) &&
                !string.IsNullOrEmpty(Words[5]) &&
                !string.IsNullOrEmpty(Words[6]))
            {
                // Yes. Extract latitude and longitude
                
                // Append hours
                string Latitude = Words[3].Substring(0, 2) + @"°";
                currentLatitude = float.Parse(Words[3].Substring(0, 2),CultureInfo.CurrentUICulture);
                
                // Append minutes
                Latitude = Latitude + Words[3].Substring(2) + "\"";
                currentLatitude += float.Parse(Words[3].Substring(2),CultureInfo.CurrentUICulture) / 60;

                // Append the hemisphere
                Latitude = Latitude + Words[4]; 
                currentLatitude = Words[4] == "N" ? currentLatitude : -1 * currentLatitude;

                // Append hours
                string Longitude = Words[5].Substring(0, 3) + "°";
                currentLongitude = float.Parse(Words[5].Substring(0, 3),CultureInfo.CurrentUICulture);

                // Append minutes
                Longitude = Longitude + Words[5].Substring(3) + "\"";
                currentLongitude += float.Parse(Words[5].Substring(3),CultureInfo.CurrentUICulture) / 60;
                
                // Append the hemisphere
                Longitude = Longitude + Words[6];
                currentLongitude = Words[6] == "W" ? -1 * currentLongitude : currentLongitude;
                // Notify the calling application of the change
                if ((PositionReceived != null) && (currentHDOPValue < maximumHDOPValue))
                   PositionReceived(Latitude, Longitude);
                
            }
            // Do we have enough values to parse satellite-derived time?
            if (!string.IsNullOrEmpty(Words[1]))
            {
                // Yes. Extract hours, minutes, seconds and milliseconds
                int UtcHours = Convert.ToInt32(Words[1].Substring(0, 2), NmeaCultureInfo);
                int UtcMinutes = Convert.ToInt32(Words[1].Substring(2, 2), NmeaCultureInfo);
                int UtcSeconds = Convert.ToInt32(Words[1].Substring(4, 2), NmeaCultureInfo);
                
                // Extract milliseconds if it is available
                int UtcMilliseconds = (Words[1].Length > 7) ? Convert.ToInt32(Words[1].Substring(7), NmeaCultureInfo) : 0;
                
                // Now build a DateTime object with all values
                System.DateTime Today = System.DateTime.Now.ToUniversalTime();
                System.DateTime SatelliteTime = new System.DateTime(Today.Year,Today.Month, Today.Day, UtcHours, UtcMinutes, UtcSeconds,UtcMilliseconds);
                // Notify of the new time, adjusted to the local time zone
                if (DateTimeChanged != null)
                {
                    lastGPSUpdateTime = SatelliteTime.ToLocalTime();
                    DateTimeChanged(SatelliteTime.ToLocalTime());
                }
            }
            // Do we have enough information to extract the current speed?
            if (!string.IsNullOrEmpty(Words[7]))
            {
                // Yes. Parse the speed and convert it to MPH
                currentSpeed = float.Parse(Words[7], NmeaCultureInfo) * MPHPerKnot;
                // Notify of the new speed
                if (SpeedReceived != null)
                    SpeedReceived(currentSpeed);
            }
            // Do we have enough information to extract bearing?
            if (!string.IsNullOrEmpty(Words[8]))
            {
                // Indicate that the sentence was recognized
                currentBearing = float.Parse(Words[8], NmeaCultureInfo);
                if (BearingReceived != null)
                    BearingReceived(currentBearing);
            }
            // Does the device currently have a satellite fix?
            if (!string.IsNullOrEmpty(Words[2]))
            {
                switch (Words[2])
                {
                    case "A":
                        if (FixObtained != null)
                            FixObtained();
                        break;
                    case "V":
                        if (FixLost != null)
                            FixLost();
                        break;
                }
            }
            // Indicate that the sentence was recognized
            return true;
        }
        // Interprets a "Satellites in View" NMEA sentence
        private bool ParseGPGSV(string sentence)
        {
            int PseudoRandomCode = 0;
            int Azimuth = 0;            
            int SignalToNoiseRatio = 0;
            // Divide the sentence into words
            string[] Words = GetWords(sentence);

            bool firstMessage = (Words[2].CompareTo(@"1") == 0) ? true : false;

            NumSatsReceived(Convert.ToInt32(Words[3], NmeaCultureInfo));
            // Each sentence contains four blocks of satellite information. 
            // Read each block and report each satellite's information
            int Count = 0;
            for (Count = 1; Count <= 4; Count++)
            {
                // Does the sentence have enough words to analyze?
                if ((Words.Length - 1) >= (Count * 4 + 3))
                {
                    // Yes. Proceed with analyzing the block. 
                    // Does it contain any information?
                    if (!string.IsNullOrEmpty(Words[Count * 4]) &&
                        !string.IsNullOrEmpty(Words[Count * 4 + 1]) &&
                        !string.IsNullOrEmpty(Words[Count * 4 + 2]) &&
                        !string.IsNullOrEmpty(Words[Count * 4 + 3]))
                    {
                        // Yes. Extract satellite information and report it
                        PseudoRandomCode = System.Convert.ToInt32(Words[Count * 4], NmeaCultureInfo);
                        currentElevation = Convert.ToInt32(Words[Count * 4 + 1], NmeaCultureInfo);
                        Azimuth = Convert.ToInt32(Words[Count * 4 + 2], NmeaCultureInfo);
                        SignalToNoiseRatio = Convert.ToInt32(Words[Count * 4 + 3], NmeaCultureInfo);
                        // Notify of this satellite's information
                        if (SatelliteReceived != null)
                        {
                            SatelliteReceived(PseudoRandomCode, Azimuth,
                            currentElevation, SignalToNoiseRatio, firstMessage);
                            firstMessage = false;
                        }
                    }
                }
            }
            // Indicate that the sentence was recognized
            return true;
        }
        // Interprets a "Fixed Satellites and DOP" NMEA sentence
        private bool ParseGPGSA(string sentence)
        {
            // Divide the sentence into words
            string[] Words = GetWords(sentence);
            try
            {
                // Update the DOP values
                if (!string.IsNullOrEmpty(Words[15]))
                {
                    if (PDOPReceived != null)
                    {
                        currentPDOPValue = double.Parse(Words[15], NmeaCultureInfo);
                        PDOPReceived(currentPDOPValue);
                    }
                }
                if (!string.IsNullOrEmpty(Words[16]))
                {
                    if (HDOPReceived != null)
                    {
                        currentHDOPValue = double.Parse(Words[16], NmeaCultureInfo);
                        HDOPReceived(currentHDOPValue);
                    }
                }
                if (!string.IsNullOrEmpty(Words[17]))
                {
                    if (VDOPReceived != null)
                    {
                        currentVDOPValue = double.Parse(Words[17], NmeaCultureInfo);
                        VDOPReceived(currentVDOPValue);
                    }
                }
            }
            catch (FormatException)
            {;}
            return true;
        }
        // Returns True if a sentence's checksum matches the calculated checksum
        private static bool IsValid(string sentence)
        {
            // Compare the characters after the asterisk to the calculation
            return sentence.Substring(sentence.IndexOf("*") + 1) == GetChecksum(sentence);
        }

        // Calculates the checksum for a sentence
        private static string GetChecksum(string sentence)
        {
            // Loop through all chars to get a checksum
            int Checksum = 0;
            foreach (char Character in sentence)
            {
                if (Character == '$')
                {
                    // Ignore the dollar sign
                }
                else if (Character == '*')
                {
                    // Stop processing before the asterisk
                    break;
                }
                else
                {
                    // Is this the first value for the checksum?
                    if (Checksum == 0)
                    {
                        // Yes. Set the checksum to the value
                        Checksum = Convert.ToByte(Character);
                    }
                    else
                    {
                        // No. XOR the checksum with this character's value
                        Checksum = Checksum ^ Convert.ToByte(Character);
                    }
                }
            }
            // Return the checksum formatted as a two-character hexadecimal
            return Checksum.ToString("X2", NmeaCultureInfo);
        }

        private Queue<float> latitudeQueue = new Queue<float>();
        private Queue<float> longitudeQueue = new Queue<float>();
        private Queue<long> elapsedSinceReadQueue = new Queue<long>();
        private Queue<long> reportedTimeQueue = new Queue<long>();
        private Queue<float> bearingQueue = new Queue<float>();
        private Queue<float> speedQueue = new Queue<float>();
        private Queue<int> elevationQueue = new Queue<int>();

        private authenticationWS.AuthenticationWebService ws = new authenticationWS.AuthenticationWebService();

        private void reportGPSPosition(Object stateInfo)
        {
            // if tracking is disabled and there are no pending transfers
            if (!trackingGPS && latitudeQueue.Count == 0)
                return;
            if (gpsSerialPort.IsOpen && (lastGPSUpdateTime.Ticks > 0))
            {
                // add all current values to a queue to be sent
                lock (latitudeQueue)
                {
                    latitudeQueue.Enqueue(currentLatitude);
                    longitudeQueue.Enqueue(currentLongitude);
                    bearingQueue.Enqueue(currentBearing);
                    speedQueue.Enqueue(currentSpeed);
                    elevationQueue.Enqueue(currentElevation);
                    TimeSpan t = DateTime.UtcNow - new DateTime(1970, 1, 1).ToUniversalTime();
                    elapsedSinceReadQueue.Enqueue((DateTime.Now.Ticks - lastGPSUpdateTime.Ticks) / 1000);
                    reportedTimeQueue.Enqueue((long)t.TotalMilliseconds);
                    QueueUpdated(latitudeQueue.Count, minimumGPSCoordinatesToReport);
                }
            }
            if ((latitudeQueue.Count > minimumGPSCoordinatesToReport) || (!trackingGPS && latitudeQueue.Count > 0))
            {
                lock (latitudeQueue)
                {
                    try
                    {
                        // attempt to unload the queues each time                            
                        if (ws.callHome(deviceUID, latitudeQueue.ToArray(), longitudeQueue.ToArray(), elapsedSinceReadQueue.ToArray(), reportedTimeQueue.ToArray(), bearingQueue.ToArray(), speedQueue.ToArray(), elevationQueue.ToArray()))
                        {
                            latitudeQueue.Clear();
                            longitudeQueue.Clear();
                            elapsedSinceReadQueue.Clear();
                            reportedTimeQueue.Clear();
                            bearingQueue.Clear();
                            elevationQueue.Clear();
                            speedQueue.Clear();
                            QueueUpdated(latitudeQueue.Count, minimumGPSCoordinatesToReport);
                        }
                    }
                    catch (WebException)
                    {
                        //MessageBox.Show("Problem connecting to web service: " + ex.Message);
                    }
                }
            }
        }

        // fired when enough data has been received
        private void gpsSerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            //if (InvokeRequired)
            //    Invoke(new SerialDataReceivedEventHandler(gpsSerialPort_DataReceived), new object[] { sender, e });
            //else
            {
                string[] buffer = gpsSerialPort.ReadExisting().Split('$');
                gpsSerialPort.DiscardInBuffer();
                foreach (string inString in buffer)
                {
                    // only want to look at GPRMC, GPGSV and GPGSA messages
                    if (inString.StartsWith(@"GPRMC") || inString.StartsWith(@"GPGSV") || inString.StartsWith(@"GPGSA"))
                    {
                        Parse(@"$" + inString.Substring(0, inString.Length - 2));
                    }
                }
            }

        }
    }
}
