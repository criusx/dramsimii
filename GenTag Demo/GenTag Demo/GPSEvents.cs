using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace GentagDemo
{
    public partial class demoClient : Form
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

        void gpsNmea_SatelliteReceived(int PRC, int azimuth, int elevation, int SNR, bool firstMessage)
        {

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
