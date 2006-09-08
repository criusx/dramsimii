//==========================================================================================
//
//	WJ.MPR.Util.Helper
//	Copyright (c) 2005, WJ Communications, Inc.
//
//	A convenient file in which to store some Helper classes, enums and functions.
//
//==========================================================================================
using System;

namespace WJ.MPR.Util
{
	/// <summary>
	/// The MsgCmd byte for TCP/IP messages containing RFIDTag Add and Remove Tag Events.
	/// </summary>
	public enum TCPMsgCmd : byte
	{
		/// <summary>
		/// A Tag has been added to the Inventory.
		/// </summary>
		AddTag = 0x01,

		/// <summary>
		/// A Tag has been removed from the Inventory.
		/// </summary>
		RemoveTag = 0x02
	}

	/// <summary>
	/// Contains just a function to convert two bytes to an unsigned short (ushort in C# parlance).
	/// </summary>
	public class Helpers 
	{
		/// <summary>
		/// b2us = "byte to ushort"  Combine two (8-bit) bytes into a single (16-bit) ushort
		/// </summary>
		/// <param name="a">MSB</param>
		/// <param name="b">LSB</param>
		/// <returns></returns>
		public static ushort b2us(byte a, byte b)
		{
			return (ushort)(a * 256 + b);
		}
	}
}
