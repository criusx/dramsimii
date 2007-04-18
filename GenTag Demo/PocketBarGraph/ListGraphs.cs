using System;
using System.Collections;
namespace PocketGraphBar
{
	/// <summary>
	/// Summary description for ListGraphs.
	/// </summary>
	public class ListGraphs: System.Collections.ICollection
	{

      private ArrayList _innerColl;
      private int _Total;
      

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

      

      public ListData this[int i]
      {
         get
         {
            return((ListData)this._innerColl[i]);
         }
      }

      public ListGraphs()
      {
         this._innerColl = new ArrayList();
         _Total = 0;
      }

      public void Add(ListData value)
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

      

	}
}
