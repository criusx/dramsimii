namespace NMEA
{
    using System;
    using System.Collections;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Globalization;
    using System.IO;
    using System.IO.Ports;
    using System.Net;

    public class nmeaInterpreter
    {        
        #region Member Variables

        private string deviceUID;

        private System.Threading.Timer reportGPSTimer;

        private const int checkGPSRate = 30;

        private const int minimumGPSCoordinatesToReport = 30;

        private System.IO.Ports.SerialPort gpsSerialPort = new System.IO.Ports.SerialPort();

        private bool trackingGPS;

        private static char[] fieldSplitter = new char[] { ',', '*' };

        private AuthenticationWebService.AuthenticationWebService ws = new AuthenticationWebService.AuthenticationWebService();

        private Queue<float> latitudeQueue = new Queue<float>();
        private Queue<float> longitudeQueue = new Queue<float>();
        private Queue<long> elapsedSinceReadQueue = new Queue<long>();
        private Queue<long> reportedTimeQueue = new Queue<long>();
        private Queue<float> bearingQueue = new Queue<float>();
        private Queue<float> speedQueue = new Queue<float>();
        private Queue<float> elevationQueue = new Queue<float>();    

        // Used to convert knots into miles per hour
        private const float MPHPerKnot = 1.150779F;

        private const float maximumHDOPValue = 5.0F;
        
        private DateTime lastGPSUpdateTime = new DateTime(0);
        
        // current variables, updated by messages
        private float currentLatitude;

        private string currentLatitudeString;

        private float currentLongitude;

        private string currentLongitudeString;

        private float currentSpeed;

        private float currentAltitude;

        private float currentBearing;

        private float currentHDOPValue = 15;

        private float currentPDOPValue;

        private float currentVDOPValue;

        private DateTime currentTime;

        private int currentSatellitesUsed;

        private bool validData;

        private Hashtable satellitesInView = new Hashtable();
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
        public delegate void AltitudeReceivedEventHandler(float altitude);
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
        public event AltitudeReceivedEventHandler AltitudeReceived;
        #endregion

        public nmeaInterpreter(string deviceUniqueID)
        {
            reportGPSTimer =
                new System.Threading.Timer(reportGPSPosition, null, checkGPSRate * 1000, checkGPSRate * 1000);

            deviceUID = deviceUniqueID;

            //gpsSerialPort.BaudRate = 4800;
            gpsSerialPort.BaudRate = 57600;
            gpsSerialPort.ReadBufferSize = 8192;
            gpsSerialPort.ReceivedBytesThreshold = 512;
            gpsSerialPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(gpsSerialPort_DataReceived);
        }

        ~nmeaInterpreter()
        {
            Close();
        }

        /// <summary>
        /// Attempts to open the specified COM port
        /// </summary>
        /// <param name="comPort"></param>
        /// <returns></returns>
        public bool Open(string comPort)
        {
            try
            {
                gpsSerialPort.PortName = comPort;
                gpsSerialPort.Open();
                gpsSerialPort.WriteLine("$PSRF151,01*0F");
                return true;
            }
            catch (InvalidOperationException)
            {
            }
            catch (ArgumentOutOfRangeException)
            {
            }            
            catch (IOException e)
            {
                Debug.WriteLine(e.Message);
            }
            catch (UnauthorizedAccessException)
            {
            }
            catch (ArgumentNullException)
            {
            }
            catch (ArgumentException)
            {
            }
            return false;
        }

        /// <summary>
        /// Tries to close the specified COM port
        /// </summary>
        /// <returns></returns>
        public bool Close()
        {
            try
            {
                gpsSerialPort.Close();
                return true;
            }
            catch (InvalidOperationException)
            { 
            }
            return false;
        }

        /// <summary>
        /// Returns the open status of the COM port
        /// </summary>
        /// <returns></returns>
        public bool IsOpen()
        {
            return gpsSerialPort.IsOpen;
        }

        /// <summary>
        /// Gets or sets tracking
        /// </summary>
        public bool Tracking
        {
            get
            {
                return trackingGPS;
            }
            set
            {
                trackingGPS = value;
            }
        }

        /// <summary>
        /// Returns the most recent known latitude value
        /// </summary>
        public float getLatitude
        {
            get
            {
                return currentLatitude;
            }
        }

        /// <summary>
        /// Returns the most recent known longitude value
        /// </summary>
        public float Longitude
        {
            get
            {
            return currentLongitude;
            }
        }               

        /// <summary>
        /// Divides a sentence into individual words
        /// </summary>
        /// <param name="sentence"></param>
        /// <returns></returns> 
        private static string[] GetWords(string sentence)
        {
            return sentence.Split(fieldSplitter);
        }

        
        /// <summary>
        /// Interprets a GPRMC sentence
        /// Recommended Minimum Specific GPS/TRANSIT Data
        /// </summary>
        /// <param name="sentence"></param>
        /// <returns></returns>
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
                // Append hours
                currentLatitudeString = Words[3].Substring(0, 2) + @"°";
                currentLatitude = float.Parse(Words[3].Substring(0, 2), CultureInfo.InvariantCulture);

                // Append minutes
                currentLatitudeString += Words[3].Substring(2) + "\"";
                currentLatitude += float.Parse(Words[3].Substring(2), CultureInfo.InvariantCulture) / 60;

                // Append the hemisphere
                currentLatitudeString += Words[4];
                currentLatitude = Words[4] == "N" ? currentLatitude : -1 * currentLatitude;

                // Append hours
                currentLongitudeString = Words[5].Substring(0, 3) + "°";
                currentLongitude = float.Parse(Words[5].Substring(0, 3), CultureInfo.InvariantCulture);

                // Append minutes
                currentLongitudeString += Words[5].Substring(3) + "\"";
                currentLongitude += float.Parse(Words[5].Substring(3), CultureInfo.InvariantCulture) / 60;

                // Append the hemisphere
                currentLongitudeString += Words[6];
                currentLongitude = Words[6] == "W" ? -1 * currentLongitude : currentLongitude;
            }
            // Do we have enough values to parse satellite-derived time?
            if (!string.IsNullOrEmpty(Words[1]) &&
                !string.IsNullOrEmpty(Words[9]))
            {
                // Yes. Extract hours, minutes, seconds and milliseconds, etc.
                currentTime = new DateTime(Convert.ToInt32(Words[9].Substring(4, 2), CultureInfo.InvariantCulture),
                    Convert.ToInt32(Words[9].Substring(2, 2), CultureInfo.InvariantCulture),
                    Convert.ToInt32(Words[9].Substring(0, 2), CultureInfo.InvariantCulture),
                    Convert.ToInt32(Words[1].Substring(0, 2), CultureInfo.InvariantCulture),
                    Convert.ToInt32(Words[1].Substring(2, 2), CultureInfo.InvariantCulture),
                    Convert.ToInt32(Words[1].Substring(4, 2), CultureInfo.InvariantCulture),
                    Convert.ToInt32(Words[1].Substring(7), CultureInfo.InvariantCulture));
            }
            // Do we have enough information to extract the current speed?
            if (!string.IsNullOrEmpty(Words[7]))
            {
                // Yes. Parse the speed and convert it to MPH
                currentSpeed = float.Parse(Words[7], CultureInfo.InvariantCulture) * MPHPerKnot;
            }
            // Do we have enough information to extract bearing?
            if (!string.IsNullOrEmpty(Words[8]))
            {
                // Indicate that the sentence was recognized
                currentBearing = float.Parse(Words[8], CultureInfo.InvariantCulture);
            }
            // Does the device currently have a satellite fix?
            if (!string.IsNullOrEmpty(Words[2]))
            {
                switch (Words[2])
                {
                    case "A":
                        validData = true;
                        break;
                    case "V":
                        validData = false;
                        break;
                }
            }
            // Indicate that the sentence was recognized
            return true;
        }
        
        /// <summary>
        /// Interprets a GPS Satellites in View sentence
        /// </summary>
        /// <param name="sentence"></param>
        /// <returns></returns>
        private bool ParseGPGSV(string sentence)
        {
            int PseudoRandomCode = 0;
            int Azimuth = 0;
            int SignalToNoiseRatio = 0;
            // Divide the sentence into words
            string[] Words = GetWords(sentence);

            // is this the first GPGSV message?
            if (Words[2].Equals(@"1", StringComparison.Ordinal) == 0)
                satellitesInView.Clear();

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
                        // associate this satellite's Pseudo Random Code with it's SNR
                        satellitesInView[System.Convert.ToInt32(Words[Count * 4], CultureInfo.InvariantCulture)] =
                            Convert.ToInt32(Words[Count * 4 + 3], CultureInfo.InvariantCulture);
                        // ignore elevation and azimuth for now
                        //currentElevation = Convert.ToInt32(Words[Count * 4 + 1], NmeaCultureInfo);
                        //Azimuth = Convert.ToInt32(Words[Count * 4 + 2], NmeaCultureInfo);
                    }
                }
            }
            // Indicate that the sentence was recognized
            return true;
        }

        /// <summary>
        /// Interprets a "Fixed Satellites and DOP" NMEA sentence
        /// </summary>
        /// <param name="sentence"></param>
        /// <returns></returns>
        private bool ParseGPGSA(string sentence)
        {
            // Divide the sentence into words
            string[] Words = GetWords(sentence);
            try
            {
                // Update the DOP values
                if (!string.IsNullOrEmpty(Words[15]))
                {
                    currentPDOPValue = float.Parse(Words[15], CultureInfo.InvariantCulture);
                }
                else
                {
                    currentPDOPValue = -1.0F;
                }

                if (!string.IsNullOrEmpty(Words[16]))
                {
                    currentHDOPValue = float.Parse(Words[16], CultureInfo.InvariantCulture);
                }
                else
                {
                    currentHDOPValue = -1.0F;
                }

                if (!string.IsNullOrEmpty(Words[17]))
                {
                    currentVDOPValue = float.Parse(Words[17], CultureInfo.InvariantCulture);
                }
                else
                {
                    currentVDOPValue = -1.0F;
                }
            }
            catch (FormatException)
            { }
            return true;
        }

        /// <summary>
        /// Parses and sets values from a GPS Fixed Data sentence
        /// </summary>
        /// <param name="sentence"></param>
        /// <returns></returns>
        private bool ParseGPGGA(string sentence)
        {
            // Divide the sentence into words
            string[] Words = GetWords(sentence);
            try
            {            
                // get position
                // Do we have enough values to describe our location?
                if (!string.IsNullOrEmpty(Words[2]) &&
                    !string.IsNullOrEmpty(Words[3]) &&
                    !string.IsNullOrEmpty(Words[4]) &&
                    !string.IsNullOrEmpty(Words[5]))
                {
                    // Append hours
                    currentLatitudeString = Words[2].Substring(0, 2) + @"°";
                    currentLatitude = float.Parse(Words[2].Substring(0, 2), CultureInfo.InvariantCulture);

                    // Append minutes
                    currentLatitudeString += Words[2].Substring(2) + "\"";
                    currentLatitude += float.Parse(Words[2].Substring(2), CultureInfo.InvariantCulture) / 60;

                    // Append the hemisphere
                    currentLatitudeString += Words[3];
                    currentLatitude = Words[3] == "N" ? currentLatitude : -1 * currentLatitude;

                    // Append hours
                    currentLongitudeString = Words[4].Substring(0, 3) + "°";
                    currentLongitude = float.Parse(Words[4].Substring(0, 3), CultureInfo.InvariantCulture);

                    // Append minutes
                    currentLongitudeString += Words[4].Substring(3) + "\"";
                    currentLongitude += float.Parse(Words[4].Substring(3), CultureInfo.InvariantCulture) / 60;

                    // Append the hemisphere
                    currentLongitudeString += Words[5];
                    currentLongitude = Words[5] == "W" ? -1 * currentLongitude : currentLongitude;
                }

                // send the number of satellites used in the calculation
                if (!string.IsNullOrEmpty(Words[7]))
                {
                    currentSatellitesUsed = Convert.ToInt32(Words[7], CultureInfo.InvariantCulture);
                }
                else
                {
                    currentSatellitesUsed = -1;
                }

                if (!string.IsNullOrEmpty(Words[9]))
                {
                    currentAltitude = Convert.ToSingle(Words[9], CultureInfo.InvariantCulture);
                }
                else
                {
                    currentAltitude = -1;
                }
            }
            catch (FormatException)
            { }
            return true;
        }


        // Returns True if a sentence's checksum matches the calculated checksum
        private static bool IsValid(string sentence)
        {
            try
            {
                if (sentence.Length < 8)
                    return false;
                // Compare the characters after the asterisk to the calculation
                return byte.Parse(sentence.Substring(sentence.IndexOf('*') + 1, 2), NumberStyles.AllowHexSpecifier, CultureInfo.InvariantCulture) == GetChecksum(sentence);
            }
            catch (ArgumentOutOfRangeException)
            {
                // the sentence did not have a * at the end
                return false;
            }
            catch (FormatException)
            {
                return false;
            }
            catch (OverflowException)
            {
                return false;
            }
            catch (ArgumentException)
            {
                return false;
            }
        }
        
        /// <summary>
        /// Calculates the checksum for a sentence
        /// This is useful for verifying that a sentence is complete and there were no data transmission errors
        /// </summary>
        /// <param name="sentence"></param>
        /// <returns></returns>
        private static byte GetChecksum(string sentence)
        {
            // Loop through all chars to get a checksum
            byte Checksum = 0x00;

            foreach (char Character in sentence)
            {
                switch (Character)
                {
                    case '$':
                        break;
                    case '*':
                        return Checksum;
                    default:
                        Checksum ^= Convert.ToByte(Character);
                        break;
                }
            }
            // Return the checksum formatted as a two-character hexadecimal if no end was found for the string
            return Checksum;
        }

       

        /// <summary>
        /// Sends the GPS queued GPS data to a server for tracking purposes
        /// </summary>
        /// <param name="stateInfo"></param>
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
                    elevationQueue.Enqueue(currentAltitude);
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

        /// <summary>
        /// Event fired once enough data is received
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void gpsSerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            string[] buffer = gpsSerialPort.ReadExisting().Split('\n');
            gpsSerialPort.DiscardInBuffer();
            foreach (string inString in buffer)
            {                
                // only want to look at GPRMC, GPGSV and GPGSA messages
                if (inString.StartsWith(@"$GPRMC") || inString.StartsWith(@"$GPGSV") || inString.StartsWith(@"$GPGSA") || inString.StartsWith(@"$GPGGA"))
                {                    
                    // Discard the sentence if its checksum does not match our calculated checksum
                    if (IsValid(inString))
                    {
                        // Look at the first word to decide where to go next
                        switch (GetWords(inString)[0])
                        {
                            case "$GPRMC":
                                ParseGPRMC(inString);
                                break;
                            case "$GPGSV":
                                ParseGPGSV(inString);
                                break;
                            case "$GPGSA":
                                ParseGPGSA(inString);
                                break;
                            case "$GPGGA":
                                ParseGPGGA(inString);
                                break;
                            default:
                                // Indicate that the sentence was not recognized
                                break;                                
                        }
                    }
                }
                else
                {
                    Debug.WriteLine(inString);
                }
            }
        }
    }
}
