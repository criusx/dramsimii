using System;
using System.Collections;

#if USING_NUNIT
using NUnit.Framework;
#endif

namespace RFIDProtocolLib
{
    /// <summary>
    /// A Type, Length, Value (TLV).
    /// </summary>
    public class TLV
    {
        private const int HEADER_SIZE = 4;
        private const int TYPE_OFFSET = 0;
        private const int LENGTH_OFFSET = 2;

        private byte[] header;
        private byte[] data;

        /// <summary>
        /// Standard constructor.  Sets the type to 0 and the length to 0.
        /// </summary>
        public TLV()
        {
            header = new byte[HEADER_SIZE];
            Type = 0;
            Value = null;
        }

        /// <summary>
        /// Constructor with specifying type.
        /// </summary>
        /// <param name="type">The type to make the TLV.</param>
        public TLV(ushort type)
        {
            header = new byte[HEADER_SIZE];
            Type = type;
            Value = null;
        }

        /// <summary>
        /// Construct from type and data.
        /// </summary>
        /// <param name="type">The type of the TLV.</param>
        /// <param name="data">The value of the TLV.</param>
        public TLV(ushort type, byte[] data)
        {
            header = new byte[HEADER_SIZE];
            Type = type;
            Value = data;
        }

        public TLV(byte[] tlv, int offset)
        {
            header = new Byte[HEADER_SIZE];
            Buffer.BlockCopy(tlv, offset, header, 0, HEADER_SIZE);
            if (Length > 0)
            {
                data = new Byte[Length];
                Buffer.BlockCopy(tlv, offset + HEADER_SIZE, data, 0, Length);
            }
            else
                data = null;
        }

        /// <summary>
        /// The type.
        /// </summary>
        public ushort Type
        {
            get { return BitConverter.ToUInt16(header, TYPE_OFFSET); }

            set
            {
                byte[] type = BitConverter.GetBytes(value);
                Buffer.BlockCopy(type, 0, header, TYPE_OFFSET, 2);
            }
        }

        /// <summary>
        /// The length.  To modify this you must modify the Value.
        /// </summary>
        public ushort Length
        {
            get { return BitConverter.ToUInt16(header, LENGTH_OFFSET); }
        }

        /// <summary>
        /// The value.  This automatically modifies the length of the TLV.
        /// </summary>
        public byte[] Value
        {
            get { return data; }

            set
            {
                if (value == null)
                    data = null;
                else
                {
                    byte[] length = BitConverter.GetBytes(value.Length);
                    Buffer.BlockCopy(length, 0, header, LENGTH_OFFSET, 2);
                    data = value;
                }
            }
        }

        /// <summary>
        /// Read this TLV in from a stream (most likley a NetStream).
        /// NOTE: this is blocking!
        /// </summary>
        /// <param name="s">The stream to read from.</param>
        public void ReadFromStream(System.IO.Stream s)
        {
            s.Read(header, 0, HEADER_SIZE);

            if (Length > 0)
            {
                data = new byte[Length];
                s.Read(data, 0, Length);
            }
        }

        /// <summary>
        /// Write a TLV to a stream (most likley a NetStream).
        /// NOTE: this is blocking!
        /// </summary>
        /// <param name="s">The stream to write to.</param>
        public void WriteToStream(System.IO.Stream s)
        {
            s.Write(header, 0, HEADER_SIZE);
            if (Length > 0)
                s.Write(data, 0, Length);
        }
    }


    public class TLVList : IList
    {
        private ArrayList list;

        private const int HEADER_SIZE = 4;

        public TLVList()
        {
            list = new ArrayList();
        }

        public TLVList(ushort type)
        {
            list = new ArrayList();
        }

        public TLVList(byte[] buf)
        {
            list = new ArrayList();

            Type = BitConverter.ToUInt16(buf, 0);
            uint length = BitConverter.ToUInt16(buf, 2);

            int index = 0;

            while (index < length)
            {
                TLV tlv = new TLV(buf, HEADER_SIZE + index);
                index += tlv.Length + HEADER_SIZE;
                Add(tlv);
            }
        }

        public ushort Type;

        public void ReadFromStream(System.IO.Stream s)
        {
            byte[] minibuf = new byte[2];
            s.Read(minibuf, 0, 2);
            Type = BitConverter.ToUInt16(minibuf, 0);

            s.Read(minibuf, 0, 2);
            uint length = BitConverter.ToUInt16(minibuf, 0);

            byte[] data = new byte[length];

            while (length > 0)
            {
                TLV tlv = new TLV();
                tlv.ReadFromStream(s);
                length -= tlv.Length + (uint)HEADER_SIZE;
                Add(tlv);
            }
        }

        public void WriteToStream(System.IO.Stream s)
        {
            System.IO.MemoryStream buf = new System.IO.MemoryStream();

            // serialize every object into a buffer
            IEnumerator e = list.GetEnumerator();
            while (e.MoveNext())
            {
                TLV tlv = (TLV)e.Current;
                tlv.WriteToStream(buf);
            }

            byte[] type = BitConverter.GetBytes(Type);
            byte[] length = BitConverter.GetBytes(buf.Length);
            byte[] finalbuf = buf.GetBuffer();

            // write it all in
            s.Write(type, 0, 2);
            s.Write(length, 0, 2);
            s.Write(finalbuf, 0, finalbuf.Length);
        }

        public byte[] GetBytes()
        {
            System.IO.MemoryStream buf = new System.IO.MemoryStream();
            WriteToStream(buf);
            return buf.GetBuffer();
        }

        #region ICollection Interface
        public void CopyTo(System.Array array, int arrayIndex)
        {
            list.CopyTo(array, arrayIndex);
        }

        public int Count
        {
            get { return list.Count; }
        }

        public bool IsSynchronized
        {
            get { return list.IsSynchronized; }
        }

        public object SyncRoot
        {
            get { return list.SyncRoot; }
        }
        #endregion

        #region IEnumerable Interface
        public IEnumerator GetEnumerator()
        {
            return list.GetEnumerator();
        }
        #endregion

        #region IList Interface
        public int Add(object obj)
        {
            return list.Add(obj);
        }

        public void Clear()
        {
            list.Clear();
        }

        public bool Contains(object obj)
        {
            return list.Contains(obj);
        }

        public int IndexOf(object obj)
        {
            return list.IndexOf(obj);
        }

        public void Insert(int index, object obj)
        {
            list.Insert(index, obj);
        }

        public bool IsFixedSize
        {
            get { return list.IsFixedSize; }
        }

        public bool IsReadOnly
        {
            get { return list.IsReadOnly; }
        }

        public void Remove(object obj)
        {
            list.Remove(obj);
        }

        public void RemoveAt(int index)
        {
            list.RemoveAt(index);
        }

        public object this[int index]
        {
            get { return list[index]; }
            set { list[index] = value; }
        }
        #endregion
    }


#if USING_NUNIT
	/// <summary>
	/// Tests for a TLV.
	/// </summary>
	[TestFixture]
	public class TLVTest
	{
		[Test]
		public void SimpleTest()
		{
			TLV t = new TLV(5, null);
			Assert.AreEqual(5, t.Type);
			Assert.AreEqual(0, t.Length);
			Assert.AreEqual(null, t.Value);

			t.Type = 6;
			Assert.AreEqual(6, t.Type);

			byte [] buf = {0, 0, 1, 1};
			t.Value = buf;
			Assert.AreEqual(buf.Length, t.Length);
			Assert.AreEqual(buf, t.Value);
		}

		[Test]
		public void StreamTest()
		{
			byte[] buf = { 0, 0, 1, 1 };
			TLV t = new TLV(5, buf);
			
			System.IO.MemoryStream s = new System.IO.MemoryStream(100);
			t.WriteToStream(s);


			s.Seek(0, System.IO.SeekOrigin.Begin);
			Assert.AreEqual(t.Length + 4, s.Length);

			TLV t2 = new TLV();
			t2.ReadFromStream(s);

			Assert.AreEqual(t.Type, t2.Type);
			Assert.AreEqual(t.Length, t2.Length);
			Assert.AreEqual(t.Value, t2.Value);
		}

		[Test]
		public void TLVListTest()
		{
			System.IO.MemoryStream s = new System.IO.MemoryStream(100);

			TLVList t = new TLVList(5);
			t.Add(new TLV(1, System.Text.Encoding.ASCII.GetBytes("Type 1")));
			t.Add(new TLV(2, System.Text.Encoding.ASCII.GetBytes("Type 2")));

			t.WriteToStream(s);
			
			s.Seek(0, System.IO.SeekOrigin.Begin);

			TLVList t2 = new TLVList();
			t2.ReadFromStream(s);

			Assert.AreEqual(t.Type, t2.Type);
			Assert.AreEqual(t.Count, t2.Count);
			
			IEnumerator e = t.GetEnumerator();
			IEnumerator e2 = t2.GetEnumerator();
			while (e.MoveNext() && e2.MoveNext())
			{
				TLV tlv = (TLV)e.Current;
				TLV tlv2 = (TLV)e2.Current;
				Assert.AreEqual(tlv.Type, tlv2.Type);
				Assert.AreEqual(tlv.Length, tlv2.Length);
				Assert.AreEqual(tlv.Value, tlv2.Value);
			}
		}

		[Test]
		public void TLVListByteTest()
		{
			TLVList t = new TLVList(1);
			t.Add(new TLV(1));
			t.Add(new TLV(2, System.Text.Encoding.ASCII.GetBytes("Type 2")));
			t.Add(new TLV(2, System.Text.Encoding.ASCII.GetBytes("Type 2")));
			
			TLVList t2 = new TLVList(t.GetBytes());
			
			Assert.AreEqual(t.Type, t2.Type);
			
			IEnumerator e = t.GetEnumerator();
			IEnumerator e2 = t2.GetEnumerator();
			while (e.MoveNext() && e2.MoveNext())
			{
				TLV tlv = (TLV)e.Current;
				TLV tlv2 = (TLV)e2.Current;
				Assert.AreEqual(tlv.Type, tlv2.Type);
				Assert.AreEqual(tlv.Length, tlv2.Length);
				Assert.AreEqual(tlv.Value, tlv2.Value);
			}
		}
	}
#endif
}