using System;
using System.Collections;
using System.Drawing;

namespace PocketGraphBar
{
	/// <summary>
	/// Collection of key pair value data to graph
	/// </summary>
   public class ListDataCollection: System.Collections.ICollection
   {
      private ArrayList _innerColl;
      private int _Total;
      private System.Drawing.Color mDisplayColor;
      private decimal mMax;

       public void CopyTo(Exception[] array, int index)
       {
           ((ICollection)this).CopyTo(array, index);
       }


      public int TotalRecords
      {
         get
         {
            return _Total;
         }
         set
         {
            _Total = value;
         }
      }

      public decimal Max
      {
         get
         {
            return mMax;
         }
      }

      public System.Drawing.Color DisplayColor
      {
         get
         {
            return mDisplayColor;
         }
         set
         {
            mDisplayColor = value;
         }
      }

      public GraphPoint this[int i]
      {
         get
         {
            return((GraphPoint)this._innerColl[i]);
         }
      }

      public ListDataCollection()
      {
         this._innerColl = new ArrayList();
         //_Total;
      }

      public void Add(GraphPoint value)
      {
         this._innerColl.Add(value);
      }

      public int Count
      {
         get
         {
            return(this._innerColl.Count);
         }
      }

      public bool IsSynchronized
      {
         get
         {
            return(false);
         }
      }

      public IEnumerator GetEnumerator()
      {
         return(this._innerColl.GetEnumerator());
      }

      public object SyncRoot
      {
         get
         {
            return(this);
         }
      }

      public void CopyTo(Array array,int index)
      {
         this._innerColl.CopyTo(array,index);
      }

      public void CalculateMax()
      {
         mMax = 0;
         foreach(GraphPoint d in this._innerColl )
         {
            if(d.Y > mMax)
               mMax = d.Y;
         }
      }
	}
}
