using System;
using System.Collections.Generic;
using System.Text;

namespace eV_Products_Demo
{
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
}
