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

        private Color nextColor;

        public bool Blink
        {
            set
            {
                lastBlinkTime = Environment.TickCount;
            }
            get
            {
                return (Environment.TickCount - lastBlinkTime) < 1500;
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
                setPhoto(pictureBox1, refImages[currentImage]);
            }
        }
        
        public RFIDReadWaitCursor()
        {
            InitializeComponent();
            nextColor = Color.Black;
            eventTimer.Tick += new EventHandler(eventTimer_Tick);
            eventTimer.Interval = 750;
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
                this.Invoke(new eventTimer_TickDelegate(eventTimer_Tick), new object[] { sender, e });
            }
            else
            {
                if (Environment.TickCount - lastBlinkTime > 1500)
                    this.BackColor = Color.Black;
                else
                    this.BackColor = Color.Red;

                currentImage = (currentImage + 1) % refImages.Length;
                setPhoto(pictureBox1, refImages[currentImage]);
            }
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
                pB.Image = bA;
                pB.Refresh();
            }
            catch (ArgumentException)
            {

            }
        }
    }
}