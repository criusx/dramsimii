//==========================================================================================
//
//	WJ.MPR.Util.byteList
//	Copyright (c) 2005, WJ Communications, Inc.
//
//==========================================================================================
using System;
using System.Text;

namespace WJ.MPR.Util
{
	/// <summary>
	///	A strongly-typed collection of byte objects.
	/// It can also return a hexstring and has subList capability.
	/// </summary>
	public class byteList : System.Collections.CollectionBase
	{
		#region constructors
		/// <summary>
		/// Construct a byteList from a subrange of another byteList.
		/// </summary>
		/// <param name="src">The byteList to copy from.</param>
		/// <param name="b">The index to start copying from.</param>
		/// <param name="e">One more than the Index of the last element to copy.</param>
		public byteList (byteList src, int b, int e) { for (int i=b; i < e; i++) Add(src[i]); }

		/// <summary>
		/// Construct a byteList from a subrange of another byteList.
		/// </summary>
		/// <param name="src">The byteList to copy from.</param>
		/// <param name="b">The index to start copying from.</param>
		public byteList (byteList src, int b) : this(src, b, src.Count) {}

		/// <summary>
 		/// Construct a byteList from a byte array.
		/// </summary>
		/// <param name="src">The byte[] to copy from.</param>
		public byteList (byte[] src) { Add(src); }

		/// <summary>
		/// Construct a byteList from a subrange of a byte array.
		/// </summary>
		/// <param name="src">The byte[] to copy from.</param>
		/// <param name="b">The index to start copying from.</param>
		/// <param name="e">One more than the Index of the last element to copy.</param>
		public byteList (byte[] src, int b, int e) { for (int i=b; i < e; i++) Add(src[i]); }

		/// <summary>
		/// Construct a byteList from a subrange of a byte array.
		/// </summary>
		/// <param name="src">The byte[] to copy from.</param>
		/// <param name="b">The index to start copying from.</param>
		public byteList (byte[] src, int b) : this(src, b, src.Length) {}

		/// <summary>
		/// Construct a byteList starting with a single byte.
		/// </summary>
		/// <param name="b">The byte that will be placed in index 0.</param>
		public byteList (byte b) { Add(b); }

		/// <summary>
		/// Construct a byteList filling it from the characters of a string.
		/// Each char of the string is first converted to a byte.
		/// If the string is null, 
		/// </summary>
		/// <param name="s">The string to copy from.</param>
		public byteList (string s) {if (s!=null) foreach (char b in s.ToCharArray()) {Add((byte)b);}}

		/// <summary>
		/// Construct a byteList with a N copies of the same byte (b).
		/// </summary>
		/// <param name="b">The value to copy.</param>
		/// <param name="N">The number of copies.</param>
		public byteList (byte b, int N) {while (N-- > 0) Add(b);}

		/// <summary>
		/// Construct an empty byteList.
		/// </summary>
		public byteList () {}
		#endregion

		/// <summary>
		/// Returns a new byteList consisting of a subset of the original.
		/// </summary>
		/// <param name="b">The index to start copying from.</param>
		/// <param name="e">One more than the Index of the last element to copy.</param>
		/// <returns>A new byteList seeded with elements of this byteList.</returns>
		public byteList subList (int b, int e) { return new byteList(this, b, e); }

		/// <summary>
		/// Returns a new byteList consisting of a subset of the original.
		/// </summary>
		/// <param name="b">The index to start copying from.</param>
		/// <returns>A new byteList seeded with elements of this byteList.</returns>
		public byteList subList (int b) { return new byteList(this, b); }

		/// <summary>
		/// Gets or sets the element at the specified index.
		/// This property is the indexer for the list.
		/// </summary>
		/// <param name="Index">The zero-based index of the element to get or set.</param>
		/// <value>The element at the specified index.</value>
		/// <exception cref="ArgumentOutOfRangeException">index is not a valid index in the list.</exception>
		public byte this[int Index] { get {return (byte) List[Index];} set {List[Index] = value;} }

		/// <summary>
		/// Add a single value to the list.
		/// </summary>
		/// <param name="val">the value to add.</param>
		/// <returns>Index of newly added element.</returns>
		public int Add(byte val) { return List.Add(val); }

		/// <summary>
		/// Decomposes a ushort to two bytes, and adds them to the list.
		/// MSB first, then LSB.  Returns the Index of the LSB.
		/// </summary>
		/// <param name="us">16-bits to add as two bytes to the list</param>
		/// <returns>The index of the Least Significant Byte</returns>
		public int Add(ushort us) { List.Add((byte)(us >> 8)); return List.Add((byte)(us & 0xFF)); }

		/// <summary>
		/// Adds all the bytes of a byte[].
		/// </summary>
		/// <param name="bArray">The array to copy from.</param>
		/// <returns>The Index of the last byte added.</returns>
		public int Add(byte[] bArray) { int i=Count; foreach (byte b in bArray) i = List.Add(b); return i; }

		/// <summary>
		/// Add a subrange from an array to the end of the list.
		/// </summary>
		/// <param name="bArray">The array to copy from.</param>
		/// <param name="b">The index to start copying from.</param>
		/// <param name="e">One more than the Index of the last element to copy.</param>
		/// <returns>The Index of the last element added.</returns>
		public int Add(byte[] bArray, int b, int e) { int i=b; while (b < e) i = List.Add(bArray[b++]); return i; }

		/// <summary>
		/// Add the elements from one list to this list.
		/// </summary>
		/// <param name="BL">The list to copy from.</param>
		/// <returns>The Index of the last element added.</returns>
		public int Add(byteList BL) { int i=Count; if (BL != null) foreach (byte b in BL) i = Add(b); return i; }

		/// <summary>
		/// Determines the index of a specific element in the list.
		/// </summary>
		/// <param name="val">The element to locate in the list.</param>
		/// <returns>The index of b if found in the list; otherwise, -1.</returns>
		public int IndexOf(byte val) { return List.IndexOf(val); }
		
		/// <summary>
		/// Inserts an element into the list at the specified Index.
		/// </summary>
		/// <param name="Index">The zero-based index at which the value should be inserted.</param>
		/// <param name="val">The value to insert into the list.</param>
		public void Insert(int Index, byte val) { List.Insert(Index, val); }

		/// <summary>
		/// Removes the first occurrence of a specific value from the list.
		/// </summary>
		/// <param name="val">The value to remove from the list.</param>
		public void Remove(byte val) { List.Remove(val); }

		/// <summary>
		/// Determines whether the list contains a specific value.
		/// </summary>
		/// <param name="val">The value to locate in the list.</param>
		/// <returns>true if val is found in the list; otherwise, false.</returns>
		public bool Contains(byte val) { return List.Contains(val); }

		/// <summary>
		/// Convert the list to an array.
		/// </summary>
		/// <returns>The values of this list, as an array.</returns>
		public byte[] ToArray() { byte[] A = new byte[this.Count]; List.CopyTo(A,0); return A; }

		/// <summary>
		/// Converts a sublist of this list to an array.
		/// </summary>
		/// <param name="b">The index to start copying from.</param>
		/// <param name="e">One more than the Index of the last element to copy.</param>
		/// <returns>The values of the sublist, as an array.</returns>
		public byte[] ToArray (int b, int e) 
		{ 
			if (b > e) b = e;
			byte[] A = new byte[e-b];
			for (int i=b, j=0; i < e; i++, j++) 
				A[j] = this[i];
			return A;
		}

		/// <summary>
		/// Method overloaded, defaults sep = " "
		/// </summary>
		/// <returns>A string of Hexadecimal digits.</returns>
		public string ToHexString() { return ToHexString(" "); }

		/// <summary>
		/// Convert byteList to string of hexadecimal octets, each pair separated by sep.
		/// </summary>
		/// <param name="sep">The string to separate each byte (pair of hex digits).</param>
		/// <returns>The byteList as a string of hex digits pairs.</returns>
		public string ToHexString(string sep)
		{
			StringBuilder SB = new System.Text.StringBuilder();
			foreach	(byte b	in List)
				SB.AppendFormat(null,"{0:X2}{1}", b, sep);
			// Remove the trailing sep
			return SB.ToString().TrimEnd(sep.ToCharArray());
		}

		/// <summary>
		/// Converts byteList to a string.  Each byte is converted to a char.
		/// </summary>
		/// <returns>A string.</returns>
		public override string ToString()
		{
			StringBuilder aString = new	StringBuilder(this.Count);
			foreach(byte b in this)
				aString.Append((char)b);
			return aString.ToString();
		}
	}
}
