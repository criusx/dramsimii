using System;
using System.Collections.Generic;
using System.Text;

namespace eV_Products_Demo
{
    //------------------------set ROI Begin-------------------------//
    public class FWHMCondition// : ILineCondition
    {
        #region Fields
        private double ulimitValue;
        private double llimitValue;
        #endregion Fields

        #region Constructor

        public FWHMCondition(double ulimit, double llimit)
        {
            this.ulimitValue = ulimit;
            this.llimitValue = llimit;
        }
        #endregion Constructor

        #region ILineCondition Members
        //public bool CheckCondition(Series series, int dataPoint)
        //{
        //    // check to see if the data point is in the region
        //    // and if it is then return true (which will use our LimitLine instead of the series' line) 
        //    bool returnValue = false;

        //    if (series.GetValue(SeriesComponent.X, dataPoint) > this.llimitValue && series.GetValue(SeriesComponent.X, dataPoint) < this.ulimitValue)
        //    {
        //        returnValue = true;
        //    }

        //    return returnValue;
        //}

        //private Line propLine = new Line(Color.Red, 1.0F);
        //public Line Line
        //{
        //    get
        //    {
        //        return propLine;
        //    }
        //    set
        //    {
        //        propLine = value;
        //    }
        //}

        private string propName = string.Empty;
        public string Name
        {
            get
            {
                return propName;
            }
            set
            {
                propName = value;
            }
        }
        #endregion
    }
    //-----------------------set ROI end----------------------------//
}
