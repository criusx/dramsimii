using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Globalization;
using System.Collections;

namespace GentagDemo
{
    public partial class demoClient
    {
        private delegate void updateGPSDelegate();

        void updateGPS()
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new updateGPSDelegate(updateGPS));
            }
            else
            {
                if (gpsInterpreter.IsOpen())
                {
                    if (gpsInterpreter.Time.Year > 0)
                    {
                        timeTextBox.Text = gpsInterpreter.Time.ToLocalTime().ToShortTimeString();
                        dateTextBox.Text = gpsInterpreter.Time.Date.ToShortDateString();
                    }

                    if (gpsInterpreter.Fixed)
                        statusTextBox.Text = Properties.Resources.Locked;
                    else
                        statusTextBox.Text = Properties.Resources.Searching;


                    latitudeTextBox.Text = gpsInterpreter.LatitudeString;
                    longitudeTextBox.Text = gpsInterpreter.LongitudeString;

                    speedTextBox.Text = gpsInterpreter.Speed > 0 ? gpsInterpreter.Speed.ToString(CultureInfo.CurrentCulture) : string.Empty;
                    directionTextBox.Text = gpsInterpreter.Bearing > 0 ? gpsInterpreter.Bearing.ToString(CultureInfo.CurrentCulture) : string.Empty;

                    altitudeTextBox.Text = gpsInterpreter.Altitude > 0 ? gpsInterpreter.Altitude.ToString(CultureInfo.CurrentCulture) : string.Empty;
                    hdopTextBox.Text = gpsInterpreter.HDOP > 0 ? gpsInterpreter.HDOP.ToString(CultureInfo.CurrentCulture) : string.Empty;
                    satellitesUsedTextBox.Text = gpsInterpreter.Satellites > 0 ? gpsInterpreter.Satellites.ToString(CultureInfo.CurrentCulture) : string.Empty;
                    //queueSizeBar, size);
                    //setProgressBarMax(queueSizeBar, maxSize);

                    int i = 0;

                    IDictionaryEnumerator hashtableEnumerator = gpsInterpreter.SatellitesInView.GetEnumerator();

                    for (; (hashtableEnumerator.MoveNext()) && (i < 8); i++)
                    {
                        satLabelList[i].Text = hashtableEnumerator.Key.ToString();
                        satProgressBarList[i].Value = int.Parse(hashtableEnumerator.Value.ToString());
                    }

                    for (; i < 8; i++)
                    {
                        satLabelList[i].Text = string.Empty;
                        satProgressBarList[i].Value = 0;
                    }
                }
            }
        }
    }
}
