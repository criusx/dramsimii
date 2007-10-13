using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace eV_Products_Demo
{
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
                factor = double.Parse(s.Substring(i + 2));
                s = "";
            }

            s = sr.ReadLine();
            i = s.IndexOf(',');
            if (i != -1)
            {
                intercept = double.Parse(s.Substring(i + 2));
                s = "";
            }

            sr.ReadLine();// read one more

            while (sr.Peek() >= 0)
            {
                for (i = 0; i < iChannelNo; i++)
                {
                    s = sr.ReadLine();
                    string[] split = s.Split(new Char[] { ',' });
                    iChannel[i] = int.Parse(split[0]);
                    dEn[i] = double.Parse(split[1]);
                    dCounts[i] = int.Parse(split[2]);
                    dSmth[i] = double.Parse(split[3]);
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
}
