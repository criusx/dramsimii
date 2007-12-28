using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace GentagDemo
{
    public partial class demoClient
    {
        void gpsNmea_DateTimeChanged(DateTime newTime)
        {
            setTextBox(timeTextBox, newTime.ToLocalTime().ToShortTimeString());
        }

        void gpsNmea_FixObtained()
        {
            setTextBox(statusTextBox, "Locked");
        }

        void gpsNmea_FixLost()
        {
            setTextBox(statusTextBox, "Searching...");
        }

        void gpsNmea_PositionReceived(string latitude, string longitude)
        {
            setTextBox(latitudeTextBox, latitude);
            setTextBox(longitudeTextBox, longitude);
        }

        void gpsNmea_SpeedReceived(double speed)
        {
            setTextBox(speedTextBox, speed.ToString());
        }

        void gpsNmea_BearingReceived(double bearing)
        {
            setTextBox(directionTextBox, bearing.ToString());
        }

        int currentProgressBar = 0;

        void gpsNmea_SatelliteReceived(int PRC, int azimuth, int elevation, int SNR, bool firstMessage)
        {
            if (firstMessage)
            {
                currentProgressBar = 0;
                //setLabel(satLabel1, "");
                setLabel(satLabel2, "");
                setLabel(satLabel3, "");
                setLabel(satLabel4, "");
                setLabel(satLabel5, "");
                setLabel(satLabel6, "");
                setLabel(satLabel7, "");
                setLabel(satLabel8, "");
                //setProgressBar(progressBar1, 0);
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
                setLabel(satLabel1, PRC.ToString());
                setProgressBar(progressBar1, SNR);
            }
            else if (currentProgressBar == 1)
            {
                setLabel(satLabel2, PRC.ToString());
                setProgressBar(progressBar2, SNR);
            }
            else if (currentProgressBar == 2)
            {
                setLabel(satLabel3, PRC.ToString());
                setProgressBar(progressBar3, SNR);
            }
            else if (currentProgressBar == 3)
            {
                setLabel(satLabel4, PRC.ToString());
                setProgressBar(progressBar4, SNR);
            }
            else if (currentProgressBar == 4)
            {
                setLabel(satLabel5, PRC.ToString());
                setProgressBar(progressBar5, SNR);
            }
            else if (currentProgressBar == 5)
            {
                setLabel(satLabel6, PRC.ToString());
                setProgressBar(progressBar6, SNR);
            }
            else if (currentProgressBar == 6)
            {
                setLabel(satLabel7, PRC.ToString());
                setProgressBar(progressBar7, SNR);
            }
            else if (currentProgressBar == 7)
            {
                setLabel(satLabel8, PRC.ToString());
                setProgressBar(progressBar8, SNR);
            }

            currentProgressBar++;
        }

        void gpsNmea_PDOPReceived(double PDOP)
        {

        }

        void gpsNmea_HDOPReceived(double HDOP)
        {
            setTextBox(hdopTextBox, HDOP.ToString());
        }

        void gpsNmea_VDOPReceived(double VDOP)
        {
            setTextBox(vdopTextBox, VDOP.ToString());
        }

        void gpsNmea_NumSatsReceived(int value)
        {
            setTextBox(satellitesUsedTextBox, value.ToString());
        }

        void pendingQueueUpdate(int size, int maxSize)
        {
            setProgressBar(queueSizeBar, size);
            setProgressBarMax(queueSizeBar, maxSize);
        }
    }
}