using System;

namespace PocketGraphBar
{
    /// <summary>
    /// Each point to represent in the graph
    /// </summary>
    public class GraphPoint
    {
        decimal mX;
        decimal mY;

        public decimal X
        {
            get
            {
                return mX;
            }
            set
            {
                mX = value;
            }
        }

        public decimal Y
        {
            get
            {
                return mY;
            }
            set
            {
                mY = value;
            }
        }

        public GraphPoint()
        {
        }

        public GraphPoint(decimal X, decimal Y)
        {
            mX = X;
            mY = Y;
        }
    }
}
