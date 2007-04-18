using System;
using System.Collections;
using System.Windows.Forms;


namespace PocketGraphBar
{
	/// <summary>
	/// Controls data in diferent from a collection of series an retrieves proper graph output
	/// </summary>
	public class GraphMotor
	{

      ListGraphs mGraphs;
      int mDisplayTimes;
      decimal mMax;
      int mThick;
      int mMaxHeihgt;
      System.Drawing.Color mAxisColor;
      System.Drawing.Color mLegendColor;
      int mLeftMargin;
      System.Drawing.Font mFont;

      /// <summary>
      /// Collection of graphs
      /// </summary>
      public ListGraphs Graphs
      {
         get
         {
            return mGraphs;
         }
         set
         {
            mGraphs = value;
         }
      }

      /// <summary>
      /// Left margin 
      /// </summary>
      public int LeftMargin
      {
         get
         {
            return mLeftMargin;
         }
         set
         {
            mLeftMargin = value;
         }
      }

      /// <summary>
      /// How many iterations the graph has
      /// </summary>
      public int DisplayTimes
      {
         get
         {
            return mDisplayTimes;
         }
         set
         {
            if(value == 0) value = 1;
            mDisplayTimes = value;
         }
      }

      /// <summary>
      /// Maximum height
      /// </summary>
      public int MaxHeight
      {
         get
         {
            return mMaxHeihgt;
         }
         set
         {
            if(value == 0) value = 300;
            mMaxHeihgt = value;
         }
      }


      /// <summary>
      /// Thick for each bar
      /// </summary>
      public int Thick
      {
         get
         {
            return mThick;
         }
         set
         {
            mThick = value;
         }
      }

      /// <summary>
      /// Color of X Axis and Y Axis
      /// </summary>
      public System.Drawing.Color AxisColor
      {
         get
         {
            return mAxisColor;
         }
         set
         {
             mAxisColor = value;
         }

      }

      /// <summary>
      /// Color for data in X Axis and y Axis
      /// </summary>
      public System.Drawing.Color LegendColor
      {
         get
         {
            return mLegendColor;
         }
         set
         {
             mLegendColor = value;
         }

      }

      /// <summary>
      /// Color for data in X Axis and y Axis
      /// </summary>
      public System.Drawing.Font LegendFont
      {
         get
         {
            return mFont;
         }
         set
         {
            mFont = value;
         }
      }



      /// <summary>
      /// Default constructor
      /// </summary>
		public GraphMotor()
		{			
         mGraphs = new ListGraphs();
         mDisplayTimes = 1;
		}

      

      /// <summary>
      /// Here is where the fun begins
      /// </summary>
      /// <param name="e">The guy who actually does the drawing</param>
      public void DrawGraphs(System.Windows.Forms.PaintEventArgs e)
      {
         try
         {
            //Thre must a be at least a seies of data
            if(mGraphs.Count == 0)
               throw new Exception("You must add at least one ListData to the graphs collection in order to perform draw operation");


            //Lets find out how much data is in the graph
            int regs = mGraphs[0].Count;
            //This is how many bars I am to draw
            int xInterval = regs / mDisplayTimes;

            //I need to know the mac value entered in the series
            foreach(ListData series in mGraphs)
            {
               series.CalculateMax();
               if(series.Max > mMax)
                  mMax = series.Max;
            }

            //Any given data
            decimal val;
            
            //This is my bar
            System.Drawing.Rectangle r;

            int height;
            int x_val;
            int bar;
            System.Drawing.SolidBrush b;
            System.Drawing.SolidBrush legends = new System.Drawing.SolidBrush(mAxisColor);

            //I begin to draw each series of data
            foreach(ListData dat in mGraphs)
            {               
               //Each series has its own color
               b = new System.Drawing.SolidBrush(dat.DisplayColor);

               //Here I draw for a given series
               for(bar = 1; bar < mDisplayTimes; bar ++)
               {                  
                  val = dat[bar * xInterval].Y;
                  height = (int)((val / mMax) * mMaxHeihgt);
                  r = new System.Drawing.Rectangle(mLeftMargin + (1 * mThick) + (bar * 20),mMaxHeihgt - height,5,height);      
                  e.Graphics.FillRectangle(b, r);                  
               }
               mThick += mThick;
            }

            
            //I am going to write the data in the x axis
            int index = 0;
            for(index = 0; index < mDisplayTimes; index ++)
            {
               x_val = Convert.ToInt32( mGraphs[0][index * xInterval].X);
               e.Graphics.DrawString(x_val.ToString(),mFont,legends,(mLeftMargin + 10) + (index * 20),mMaxHeihgt + 10);               
            }

            //Any graph has at least two axis
            System.Drawing.Pen linea = new System.Drawing.Pen(mAxisColor);
            e.Graphics.DrawLine(linea,mLeftMargin + 10,mMaxHeihgt + 10,mLeftMargin + 10,5);
            e.Graphics.DrawLine(linea,mLeftMargin + 10,mMaxHeihgt + 10,500,mMaxHeihgt + 10);
            
         
            //This is done in order to get clean numbers from my maximum value
            decimal yInterval = GetYInterval(mMax);
            
            
            //I am going to write the data in the Y axis
            for(int z  = 1 ; z <= 10; z++)
            {
               e.Graphics.DrawString((z * yInterval).ToString(),mFont,legends,0,((mMaxHeihgt + 10) - ((mMaxHeihgt/10) * z) ));
            }
         }
         catch(System.Exception ee)
         {
            throw ee;
         }
      }

      /// <summary>
      /// Gets an interval (10, 100 , 1000, etc) from a given number
      /// </summary>
      /// <param name="max">Given max number to find out its interval</param>
      /// <returns>Interval in 10 multipliers</returns>
      private decimal GetYInterval(decimal max)
      {
         int yInterval = 1;
         while (yInterval < mMax / 10)
         {
            yInterval = yInterval * 10;            
         }
         return yInterval;

      }
      


	}
}
