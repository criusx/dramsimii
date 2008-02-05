﻿using System;
using System.Linq;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace RFIDReadCursor
{
    public partial class UserControl1 : UserControl
    {
        private Timer eventTimer = new Timer();

        private Image[] refImages;

        private int currentImage;

        public int Interval
        {
            get
            {
                return eventTimer.Interval;
            }
            set
            {
                eventTimer.Interval = value;
            }
        }

        public bool Blink
        {
            set
            {
                this.BackColor = value == true ? Color.Red : Color.Black;
            }
        }

        public bool TimerEnabled
        {
            get
            {
                return eventTimer.Enabled;
            }
            set
            {
                eventTimer.Enabled = value;
                currentImage = 0;
            }
        }

        public UserControl1()
        {
            InitializeComponent();
            eventTimer.Tick += new EventHandler(eventTimer_Tick);
            eventTimer.Interval = 750;
            refImages = new Image[4];
            refImages[0] = Image.FromHbitmap(RFIDReadCursor.Properties.Resources.RadioAnimation0.GetHbitmap());
            refImages[1] = Image.FromHbitmap(RFIDReadCursor.Properties.Resources.RadioAnimation1.GetHbitmap());
            refImages[2] = Image.FromHbitmap(RFIDReadCursor.Properties.Resources.RadioAnimation2.GetHbitmap());
            refImages[3] = Image.FromHbitmap(RFIDReadCursor.Properties.Resources.RadioAnimation3.GetHbitmap());
        }

        void eventTimer_Tick(object sender, EventArgs e)
        {
            currentImage = (currentImage + 1) % refImages.Length;
            setPhoto(pictureBox1, refImages[currentImage]);
        }

        private void UserControl1_Click(object sender, EventArgs e)
        {
            eventTimer.Enabled = !eventTimer.Enabled;
        }

        private delegate void setPhotoDelegateB(PictureBox pB, Image bA);

        private void setPhoto(PictureBox pB, Image bA)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new setPhotoDelegateB(setPhoto), new object[] { pB, bA });
                return;
            }
            try
            {
                //if (pB.Image != null)
                //    pB.Image.Dispose();
                pB.Image = bA;
                pB.Refresh();
            }
            catch (ArgumentException)
            {

            }
        }
    }
}