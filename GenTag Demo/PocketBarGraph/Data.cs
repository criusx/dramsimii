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
	public class Data : System.Windows.Forms.Form
	{

      private GraphMotor graph;
      
      private Microsoft.WindowsCE.Forms.InputPanel inputPanel1;
      private System.Windows.Forms.MainMenu mainMenu1;
      
   
		public Data()
		{			
			InitializeComponent();
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
         this.inputPanel1 = new Microsoft.WindowsCE.Forms.InputPanel();
         this.mainMenu1 = new System.Windows.Forms.MainMenu();
         // 
         // Data
         // 
         this.Menu = this.mainMenu1;
         this.Text = "Graph";
         this.Load += new System.EventHandler(this.Data_Load);
         this.Paint += new System.Windows.Forms.PaintEventHandler(this.Data_Paint);

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
         catch(Exception ee)
         {
            MessageBox.Show(ee.ToString());
         }

      }

      public static void Main()
      {
         Application.Run(new Data());
      }

      private void Data_Load(object sender, System.EventArgs e)
      {
            //A new motr
            graph = new GraphMotor();      

            //I add two series of data
            graph.Graphs.Add(new ListData());
            graph.Graphs.Add(new ListData());

            //Set the color for each one
            graph.Graphs[0].DisplayColor = Color.DarkBlue;
            graph.Graphs[1].DisplayColor = Color.DarkGreen;

            PocketGraphBar.GraphPoint p;

            //Generate de dumy data
            for(int i = 1; i < 11; i++)
            {
               //A new point
               p = new PocketGraphBar.GraphPoint();
               p.X = Convert.ToDecimal(i);
               p.Y = Convert.ToDecimal(i * 100);
               graph.Graphs[0].Add(p);

               //Another new point
               p = new PocketGraphBar.GraphPoint();
               p.X = Convert.ToDecimal(i);
               p.Y = Convert.ToDecimal(i * 50);
               graph.Graphs[1].Add(p);
            }            
      }

      private void Draw_Click(object sender, System.EventArgs e)
      {
         
      }
	}
}
