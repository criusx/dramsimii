using System;
using System.Collections;
namespace PocketGraphBar
{
    /// <summary>
    /// Summary description for ListGraphs.
    /// </summary>
    public class ListGraphsCollection : System.Collections.ICollection
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

        public void CopyTo(Exception[] exArray, int index)
        {
            ((ICollection)this).CopyTo(exArray, index);
        }

        public ListDataCollection this[int i]
        {
            get
            {
                return ((ListDataCollection)this._innerColl[i]);
            }
        }

        public ListGraphsCollection()
        {
            this._innerColl = new ArrayList();
            //_Total;
        }

        public void Add(ListDataCollection value)
        {
            this._innerColl.Add(value);
        }

        public void Clear()
        {
            this._innerColl.Clear();
        }

        public int Count
        {
            get
            {
                return (this._innerColl.Count);
            }
        }

        public bool IsSynchronized
        {
            get
            {
                return (false);
            }
        }

        public IEnumerator GetEnumerator()
        {
            return (this._innerColl.GetEnumerator());
        }

        public object SyncRoot
        {
            get
            {
                return (this);
            }
        }

        public void CopyTo(Array array, int index)
        {
            this._innerColl.CopyTo(array, index);
        }
    }
}
