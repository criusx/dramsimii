using System;
using System.Collections.Generic;
using System.Text;
using System.Globalization;
using System.Windows.Forms;

namespace GentagDemo
{
    class nmeaInterpreter
    {
        // Represents the EN-US culture, used for numbers in NMEA sentences
        private static CultureInfo NmeaCultureInfo = new CultureInfo("en-US");
        // Used to convert knots into miles per hour
        private double MPHPerKnot;

        #region Delegates
        public delegate void PositionReceivedEventHandler(
            string latitude, string longitude);
        public delegate void DateTimeChangedEventHandler(System.DateTime dateTime);
        public delegate void BearingReceivedEventHandler(double bearing);
        public delegate void SpeedReceivedEventHandler(double speed);
        public delegate void SpeedLimitReachedEventHandler();
        public delegate void FixObtainedEventHandler();
        public delegate void FixLostEventHandler();
        public delegate void SatelliteReceivedEventHandler(
            int pseudoRandomCode, int azimuth, int elevation, int signalToNoiseRatio, bool firstMessage);
        public delegate void HDOPReceivedEventHandler(double value);
        public delegate void VDOPReceivedEventHandler(double value);
        public delegate void PDOPReceivedEventHandler(double value);
        public delegate void NumberOfSatellitesInViewEventHandler(int numSats);
        #endregion

        #region Events
        public event PositionReceivedEventHandler PositionReceived;
        public event DateTimeChangedEventHandler DateTimeChanged;
        public event BearingReceivedEventHandler BearingReceived;
        public event SpeedReceivedEventHandler SpeedReceived;
        public event SpeedLimitReachedEventHandler SpeedLimitReached;
        public event FixObtainedEventHandler FixObtained;
        public event FixLostEventHandler FixLost;
        public event SatelliteReceivedEventHandler SatelliteReceived;
        public event HDOPReceivedEventHandler HDOPReceived;
        public event VDOPReceivedEventHandler VDOPReceived;
        public event PDOPReceivedEventHandler PDOPReceived;
        public event NumberOfSatellitesInViewEventHandler NumSatsReceived;
        #endregion

        public nmeaInterpreter(CultureInfo cultureInfo)
        {
            NmeaCultureInfo = cultureInfo;
            MPHPerKnot = double.Parse("1.150779", NmeaCultureInfo);
        }

        // Processes information from the GPS receiver
        public bool Parse(string sentence)
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
                    // A "Satellites in View" sentence was recieved
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
        public static string[] GetWords(string sentence)
        {
            return sentence.Split(splitter);
        }
        // Interprets a $GPRMC message
        public bool ParseGPRMC(string sentence)
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
                string Latitude = Words[3].Substring(0, 2) + "°";
                // Append minutes
                Latitude = Latitude + Words[3].Substring(2) + "\"";
                // Append hours 
                Latitude = Latitude + Words[4]; // Append the hemisphere
                string Longitude = Words[5].Substring(0, 3) + "°";
                // Append minutes
                Longitude = Longitude + Words[5].Substring(3) + "\"";
                // Append the hemisphere
                Longitude = Longitude + Words[6];
                // Notify the calling application of the change
                if (PositionReceived != null)
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
                System.DateTime SatelliteTime = new System.DateTime(Today.Year,
                Today.Month, Today.Day, UtcHours, UtcMinutes, UtcSeconds,
                UtcMilliseconds);
                // Notify of the new time, adjusted to the local time zone
                if (DateTimeChanged != null)
                    DateTimeChanged(SatelliteTime.ToLocalTime());
            }
            // Do we have enough information to extract the current speed?
            if (!string.IsNullOrEmpty(Words[7]))
            {
                // Yes. Parse the speed and convert it to MPH
                double Speed = double.Parse(Words[7], NmeaCultureInfo) * MPHPerKnot;
                // Notify of the new speed
                if (SpeedReceived != null)
                    SpeedReceived(Speed);
                // Are we over the highway speed limit?
                if (Speed > 55)
                    if (SpeedLimitReached != null)
                        SpeedLimitReached();
            }
            // Do we have enough information to extract bearing?
            if (!string.IsNullOrEmpty(Words[8]))
            {
                // Indicate that the sentence was recognized
                double Bearing = double.Parse(Words[8], NmeaCultureInfo);
                if (BearingReceived != null)
                    BearingReceived(Bearing);
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
        public bool ParseGPGSV(string sentence)
        {
            int PseudoRandomCode = 0;
            int Azimuth = 0;
            int Elevation = 0;
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
                        Elevation = Convert.ToInt32(Words[Count * 4 + 1], NmeaCultureInfo);
                        Azimuth = Convert.ToInt32(Words[Count * 4 + 2], NmeaCultureInfo);
                        SignalToNoiseRatio = Convert.ToInt32(Words[Count * 4 + 3], NmeaCultureInfo);
                        // Notify of this satellite's information
                        if (SatelliteReceived != null)
                        {
                            SatelliteReceived(PseudoRandomCode, Azimuth,
                            Elevation, SignalToNoiseRatio, firstMessage);
                            firstMessage = false;
                        }
                    }
                }
            }
            // Indicate that the sentence was recognized
            return true;
        }
        // Interprets a "Fixed Satellites and DOP" NMEA sentence
        public bool ParseGPGSA(string sentence)
        {
            // Divide the sentence into words
            string[] Words = GetWords(sentence);
            try
            {
                // Update the DOP values
                if (!string.IsNullOrEmpty(Words[15]))
                {
                    if (PDOPReceived != null)
                        PDOPReceived(double.Parse(Words[15], NmeaCultureInfo));
                }
                if (!string.IsNullOrEmpty(Words[16]))
                {
                    if (HDOPReceived != null)
                        HDOPReceived(double.Parse(Words[16], NmeaCultureInfo));
                }
                if (!string.IsNullOrEmpty(Words[17]))
                {
                    if (VDOPReceived != null)
                        VDOPReceived(double.Parse(Words[17], NmeaCultureInfo));
                }
            }
            catch (FormatException)
            {
            }
            return true;
        }
        // Returns True if a sentence's checksum matches the calculated checksum
        public static bool IsValid(string sentence)
        {
            // Compare the characters after the asterisk to the calculation
            return sentence.Substring(sentence.IndexOf("*") + 1) ==
            GetChecksum(sentence);
        }

        // Calculates the checksum for a sentence
        public static string GetChecksum(string sentence)
        {
            // Loop through all chars to get a check
            //     sum
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
    }
}
