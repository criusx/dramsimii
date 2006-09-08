//==========================================================================================
//
//		WJ.MPR.Reader.TagFilter
//		Copyright (c) 2005, WJ Communications, Inc.
//
//==========================================================================================
using System;
using WJ.MPR.Util;

namespace WJ.MPR.Reader
{
	/// <summary>
	///	A Class that encapsulates an RFID Tag ID filter.
	///	TagFilters are used to select a subset of the Tag ID space.
	///	They are really just an array of bits.
	///	This class includes a static method to convert a string to an array of bits.
	/// </summary>
	public struct TagFilter
	{
		/// <summary>
		/// The number of bits to use from the Bits array.
		/// </summary>
		public byte Length;

		/// <summary>
		/// A byte array that stores the bits of the filter.
		/// </summary>
		public byte[] Bits;

		/// <summary>
		/// Constructs a TagFilter from a string of hexadecimal digits.
		/// </summary>
		/// <param name="length">The number of bits to store from the string.</param>
		/// <param name="filter">A string of hexadecimal digits from which to form the TagFilter.</param>
		public TagFilter(byte length, string filter)
		{
			this.Length = length;

			// Convert length in bits to length in bytes
			Bits = new byte[((length-1) / 8) + 1];

			if (filter.Length != Bits.Length * 2) return;

			for (int ByteCnt=0; ByteCnt < Bits.Length; ByteCnt++)
			{
				Bits[ByteCnt] = Byte.Parse(filter.Substring(2*ByteCnt,2), System.Globalization.NumberStyles.AllowHexSpecifier);
			}
		}

		/// <summary>
		/// Parse the TagID in a string, and convert it to a byteList.
		/// </summary>
		/// <param name="IDstring"></param>
		/// <returns></returns>
		public static byteList ParseID(string IDstring)
		{
			byteList IDbytes = new byteList();

			// Trim out leading & trailing whitespace, and convert to upper
			IDstring = IDstring.Trim().ToUpper();

			// Add one space at the end to trigger adding the last byte to the list.
			IDstring += " ";

			byte Val = 0;
			bool SecondChar = false;

			for(int i = 0; i < IDstring.Length; i++)
			{
				if (IDstring[i] == ' ')
				{
					if (SecondChar)
					{
						IDbytes.Add(Val);
						Val = 0;
						SecondChar = false;
					}
				}
				else if (IDstring[i] <= '9' && IDstring[i] >= '0' )
				{
					Val = (byte)((Val << 4) | (byte)(IDstring[i] - '0'));
					if (SecondChar)
					{
						IDbytes.Add(Val);
						Val = 0;
					}
					SecondChar = !SecondChar;
				}
				else if (IDstring[i] <= 'F' && IDstring[i] >= 'A')
				{
					Val = (byte)((Val << 4) | (byte)(IDstring[i] - 'A' + 10));
					if (SecondChar) 
					{
						IDbytes.Add(Val);
						Val = 0;
					}
					SecondChar = !SecondChar;
				}
				else // a non-hexdigit, non-space found, get out!
					return null;
			}
			return IDbytes;
		}
	}
}