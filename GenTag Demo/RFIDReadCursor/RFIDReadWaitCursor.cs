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

        private int lastTickTime = 0;

        public bool Blink
        {
            set
            {
                this.BackColor = value == true ? Color.Red : Color.Black;
                if (value)
                    lastTickTime = Environment.TickCount;
            }
            get
            {
                return this.BackColor == Color.Red;
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

        void eventTimer_Tick(object sender, EventArgs e)
        {
            if (Environment.TickCount - lastTickTime > 500)
                Blink = false;

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
                pB.Image = bA;
                pB.Refresh();
            }
            catch (ArgumentException)
            {

            }
        }
    }
}