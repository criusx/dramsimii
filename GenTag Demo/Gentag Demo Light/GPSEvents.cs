using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Globalization;

namespace GentagDemo
{
    public partial class demoClient
    {
        void gpsNmea_DateTimeChanged(DateTime newTime)
        {
            setTextBox(timeTextBox, newTime.ToLocalTime().ToShortTimeString());
            setTextBox(dateTextBox, newTime.Date.ToShortDateString());
        }

        void gpsNmea_FixObtained()
        {
            setTextBox(statusTextBox, Properties.Resources.Locked);
        }

        void gpsNmea_FixLost()
        {
            setTextBox(statusTextBox, Properties.Resources.Searching);
        }

        void gpsNmea_PositionReceived(string latitude, string longitude)
        {
            setTextBox(latitudeTextBox, latitude);
            setTextBox(longitudeTextBox, longitude);
        }

        void gpsNmea_SpeedReceived(double speed)
        {
            setTextBox(speedTextBox, speed.ToString(CultureInfo.CurrentCulture));
        }

        void gpsNmea_BearingReceived(double bearing)
        {
            setTextBox(directionTextBox, bearing.ToString(CultureInfo.CurrentCulture));
        }

        int currentProgressBar;

        void gpsNmea_SatelliteReceived(int PRC, int azimuth, int elevation, int SNR, bool firstMessage)
        {
            if (firstMessage)
            {
                currentProgressBar = 0;
                setLabel(satLabel2, string.Empty);
                setLabel(satLabel3, string.Empty);
                setLabel(satLabel4, string.Empty);
                setLabel(satLabel5, string.Empty);
                setLabel(satLabel6, string.Empty);
                setLabel(satLabel7, string.Empty);
                setLabel(satLabel8, string.Empty);
                setProgressBar(progressBar2, 0);
                setProgressBar(progressBar3, 0);
                setProgressBar(progressBar4, 0);
                setProgressBar(progressBar5, 0);
                setProgressBar(progressBar6, 0);
                setProgressBar(progressBar7, 0);
                setProgressBar(progressBar8, 0);
            }

            if (currentProgressBar == 0)
            {
                setLabel(satLabel1, PRC.ToString(CultureInfo.CurrentCulture));
                setProgressBar(progressBar1, SNR);
            }
            else if (currentProgressBar == 1)
            {
                setLabel(satLabel2, PRC.ToString(CultureInfo.CurrentCulture));
                setProgressBar(progressBar2, SNR);
            }
            else if (currentProgressBar == 2)
            {
                setLabel(satLabel3, PRC.ToString(CultureInfo.CurrentCulture));
                setProgressBar(progressBar3, SNR);
            }
            else if (currentProgressBar == 3)
            {
                setLabel(satLabel4, PRC.ToString(CultureInfo.CurrentCulture));
                setProgressBar(progressBar4, SNR);
            }
            else if (currentProgressBar == 4)
            {
                setLabel(satLabel5, PRC.ToString(CultureInfo.CurrentCulture));
                setProgressBar(progressBar5, SNR);
            }
            else if (currentProgressBar == 5)
            {
                setLabel(satLabel6, PRC.ToString(CultureInfo.CurrentCulture));
                setProgressBar(progressBar6, SNR);
            }
            else if (currentProgressBar == 6)
            {
                setLabel(satLabel7, PRC.ToString(CultureInfo.CurrentCulture));
                setProgressBar(progressBar7, SNR);
            }
            else if (currentProgressBar == 7)
            {
                setLabel(satLabel8, PRC.ToString(CultureInfo.CurrentCulture));
                setProgressBar(progressBar8, SNR);
            }

            currentProgressBar++;
        }

        void gpsNmea_PDOPReceived(double PDOP)
        {

        }

        void gpsNmea_AltitudeReceived(float altitude)
        {
            setTextBox(altitudeTextBox, altitude.ToString(CultureInfo.CurrentCulture));
        }

        void gpsNmea_HDOPReceived(double HDOP)
        {
            setTextBox(hdopTextBox, HDOP.ToString(CultureInfo.CurrentCulture));
        }

        void gpsNmea_VDOPReceived(double VDOP)
        {
            // nowhere to display right now
        }

        void gpsNmea_NumSatsReceived(int value)
        {
            setTextBox(satellitesUsedTextBox, value.ToString(CultureInfo.CurrentCulture));
        }

        void pendingQueueUpdate(int size, int maxSize)
        {
            setProgressBar(queueSizeBar, size);
            setProgressBarMax(queueSizeBar, maxSize);
        }
    }
}
