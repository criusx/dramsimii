using System;
using System.Collections;
using System.Windows.Forms;
using System.Drawing;
using System.Globalization;


namespace PocketGraphBar
{
    /// <summary>
    /// Controls data in different from a collection of series an retrieves proper graph output
    /// </summary>
    public class GraphMotor
    {

        ListGraphsCollection mGraphs;
        int mDisplayTimes;
        decimal mMax;
        int mThick;
        int mMaxHeight;
        Color mAxisColor;
        Color mLegendColor;
        int mLeftMargin;
        Font mFont;

        /// <summary>
        /// Collection of graphs
        /// </summary>
        public ListGraphsCollection Graphs
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
                if (value == 0) value = 1;
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
                return mMaxHeight;
            }
            set
            {
                if (value == 0) value = 300;
                mMaxHeight = value;
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
        public Color AxisColor
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
        public Color LegendColor
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
        public Font LegendFont
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
            mGraphs = new ListGraphsCollection();
            mDisplayTimes = 1;
        }



        /// <summary>
        /// Here is where the fun begins
        /// </summary>
        /// <param name="e">The guy who actually does the drawing</param>
        public void DrawGraphs(PaintEventArgs e)
        {
            try
            {
                //There must a be at least a series of data
                if (mGraphs.Count == 0)
                    throw new ArgumentException("You must add at least one ListData to the graphs collection in order to perform draw operation");


                //Lets find out how much data is in the graph
                int regs = mGraphs[0].Count;
                //This is how many bars I am to draw
                int xInterval = regs / mDisplayTimes;

                //I need to know the mac value entered in the series
                foreach (ListDataCollection series in mGraphs)
                {
                    series.CalculateMax();
                    if (series.Max > mMax)
                        mMax = series.Max;
                }

                //Any given data
                decimal val;

                //This is my bar
                Rectangle r;

                int height;
                //int x_val;
                int bar;
                SolidBrush b;
                SolidBrush legends = new System.Drawing.SolidBrush(mAxisColor);

                //I begin to draw each series of data
                foreach (ListDataCollection dat in mGraphs)
                {
                    //Each series has its own color
                    //b = new SolidBrush(dat.DisplayColor);

                    //Here I draw for a given series
                    for (bar = 1; bar < mDisplayTimes; bar++)
                    {
                        val = dat[bar * xInterval].Y;
                        height = (int)((val / mMax) * mMaxHeight);
                        //r = new System.Drawing.Rectangle(mLeftMargin + (1 * mThick) + (bar * 20), mMaxHeight - height, 5, height);
                        //e.Graphics.FillRectangle(b, r);                  
                    }
                    mThick += mThick;
                }

                foreach (ListDataCollection ld in mGraphs)
                {
                    Pen p = new Pen(ld.DisplayColor, 2);

                    Point[] points = new Point[ld.Count];

                    //RectangleF limit = e.Graphics.ClipBounds;
                    Rectangle limit = e.ClipRectangle;
                    double spacing = (1.0 * limit.Width - mLeftMargin) / points.Length;

                    for (int i = 0; i < ld.Count; i++)
                    {
                        points[i].X = Convert.ToInt32(mLeftMargin + 10 + i * spacing);
                        points[i].Y = 5 + mMaxHeight - Convert.ToInt32((Convert.ToDouble(ld[i].Y) / Convert.ToDouble(mMax)) * mMaxHeight);
                    }
                    e.Graphics.DrawLines(p, points);
                }


                // X axis numbering            
                //for (int index = 0; index < mDisplayTimes; index++)
                //{
                //   x_val = Convert.ToInt32( mGraphs[0][index * xInterval].X);
                //   e.Graphics.DrawString(x_val.ToString(),mFont,legends,(mLeftMargin + 10) + (index * 20),mMaxHeight + 10);               
                //}

                // draw the axes
                System.Drawing.Pen linea = new System.Drawing.Pen(mAxisColor);
                e.Graphics.DrawLine(linea, mLeftMargin + 10, mMaxHeight + 10, mLeftMargin + 10, 5);
                e.Graphics.DrawLine(linea, mLeftMargin + 10, mMaxHeight + 10, 500, mMaxHeight + 10);
                linea.Dispose();

                //This is done in order to get clean numbers from my maximum value
                //decimal yInterval = GetYInterval(mMax);
                decimal yInterval = mMax / 10;


                // Y axis numbering
                for (int z = 1; z <= 10; z++)
                {
                    e.Graphics.DrawString((z * yInterval).ToString("F", CultureInfo.CurrentUICulture), mFont, legends, 0, ((mMaxHeight + 10) - ((mMaxHeight / 10) * z)));
                }
            }
            catch (Exception ee)
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
