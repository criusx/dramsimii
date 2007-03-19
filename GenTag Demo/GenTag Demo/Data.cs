using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using PocketGraphBar;

namespace TestPocketGraphBar
{
    /// <summary>
    /// Summary description for Graph.
    /// </summary>
    public class Graph : System.Windows.Forms.Control
    {

        private GraphMotor graph;


        public Graph()
        {
            InitializeComponent();
            //object sender, System.EventArgs e
            Data_Load();
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        protected override void Dispose(bool disposing)
        {
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.SuspendLayout();
            // 
            // Data
            //             
            this.ClientSize = new System.Drawing.Size(214, 195);
            this.Name = "Data";
            this.Text = "Graph";
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.Data_Paint);
            this.Click += new System.EventHandler(this.Data_Click);
            //this.Load += new System.EventHandler(this.Data_Load);
            this.ResumeLayout(false);

        }
        #endregion

        private void Data_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
        {
            try
            {
                //In the load event the the object was filled
                //Here wa only set its properties

                graph.LeftMargin = 20;
                graph.LegendFont = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular);
                graph.AxisColor = Color.Black;
                graph.MaxHeight = 200;
                //The width of each bar
                graph.Thick = 6;
                //The number of bars wa want to see for each series of data
                graph.DisplayTimes = 10;
                //Now solve this
                graph.DrawGraphs(e);

            }
            catch (Exception ee)
            {
                throw;
            }

        }

        //public static void Main()
        //{
        //   Application.Run(new Data());
        //}

        private void Data_Load()
        {
            //A new motor
            graph = new GraphMotor();

            //I add two series of data
            graph.Graphs.Add(new ListDataCollection());
            graph.Graphs[0].DisplayColor = Color.Red;
            return;
        }

        public void Add(int X, float Y)
        {
            GraphPoint p = new GraphPoint(Convert.ToDecimal(X), Convert.ToDecimal(Y));
            graph.Graphs[0].Add(p);
        }

        public void Clear()
        {
            graph.Graphs[0].Clear();
        }       

        private void Data_Click(object sender, EventArgs e)
        {
            Hide();
        }
    }
}
