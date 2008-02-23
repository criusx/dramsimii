using System;
using System.Drawing;
using System.Windows.Forms;

[assembly: CLSCompliant(true)]
namespace RFIDReadCursor
{
    public partial class RFIDReadWaitCursor : UserControl
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

        private int lastBlinkTime = 0;

        public void Blink()
        {
            lastBlinkTime = Environment.TickCount;

        }

        public bool TimerEnabled
        {
            get
            {
                return eventTimer.Enabled;
            }
            set
            {
                currentImage = 0;
                pictureBox1.Image = refImages[currentImage];
                eventTimer.Enabled = value;
            }
        }

        public RFIDReadWaitCursor()
        {
            InitializeComponent();
            eventTimer.Tick += new EventHandler(eventTimer_Tick);
            eventTimer.Interval = 500;
            refImages = new Image[4];
            refImages[0] = Image.FromHbitmap(RFIDReadCursor.Properties.Resources.RadioAnimation0.GetHbitmap());
            refImages[1] = Image.FromHbitmap(RFIDReadCursor.Properties.Resources.RadioAnimation1.GetHbitmap());
            refImages[2] = Image.FromHbitmap(RFIDReadCursor.Properties.Resources.RadioAnimation2.GetHbitmap());
            refImages[3] = Image.FromHbitmap(RFIDReadCursor.Properties.Resources.RadioAnimation3.GetHbitmap());
        }

        ~RFIDReadWaitCursor()
        {
            eventTimer.Enabled = false;
        }

        private delegate void eventTimer_TickDelegate(object sender, EventArgs e);

        void eventTimer_Tick(object sender, EventArgs e)
        {
            if (this.InvokeRequired)
            {
                this.BeginInvoke(new eventTimer_TickDelegate(eventTimer_Tick), new object[] { sender, e });
            }
            else
            {
                if (Environment.TickCount - lastBlinkTime > 1500)
                    this.BackColor = Color.Black;
                else
                    this.BackColor = Color.Red;

                currentImage = (currentImage + 1) % refImages.Length;
                pictureBox1.Image = refImages[currentImage];
            }
        }
    }
}