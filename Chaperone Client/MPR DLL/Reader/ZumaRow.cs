//==========================================================================================
//
//		WJ.MPR.Reader.TagFilter
//		Copyright (c) 2005, WJ Communications, Inc.
//
//==========================================================================================
using System;

namespace WJ.MPR.Reader
{
	/// <summary>
	///	A class for a single row of data in a Zuma (Impinj Class 0 Re-writeable) Tag.
	///	Each row has 18 bits, 0-17, left justified in three bytes. (only bits 0 and 1 of MSB used)
	///	There are 16 rows in a Zuma tag.
	///		Row #			Function
	///		0				Fab Protect
	///		1				Control Word
	///		2..3			Kill Passcode
	///		4..N			EPC Data
	///		N+1				EPC CRC
	///		N+2..15			User Data
	///
	///		EPC length		 N
	///		64				 7
	///		96				 9
	///		128				11
	///	
	///	Includes methods to initialize a ZumaRow object as the correct type of row.
	/// </summary>
	public class ZumaRow
	{
		/// <summary>
		/// Row Number, 0-15, of this row of the Zuma Tag.
		/// </summary>
		public byte RowNumber;

		/// <summary>
		/// The MSB, consisting of only 2 bits (17 and 16), of the Row.
		/// </summary>
		public byte Byte0;
		/// <summary>
		/// Bits 15-8 of the Row.
		/// </summary>
		public byte Byte1;
		/// <summary>
		/// Bits 7-0 of the Row.
		/// </summary>
		public byte Byte2;

		/// <summary>
		/// Converts the ZumaRow to an array of three bytes.
		/// </summary>
		public byte[] ToArray
		{
			get { return new byte[] { Byte0, Byte1, Byte2 }; }
			set 
			{ 
				if (value.Length == 3) 
				{
					Byte0 = value[0]; 
					Byte1 = value[1]; 
					Byte2 = value[2]; 
				}
			}
		}

		/// <summary>
		/// Fills this Row's bits with EPC data.
		/// </summary>
		/// <param name="MSB">The MSB of the EPC data for this Row.</param>
		/// <param name="LSB">The LSB of the EPC data for this Row.</param>
		/// <param name="LockBit">Whether this row is locked or not (bit17). </param>
		public void EPCRow(byte MSB, byte LSB, bool LockBit)
		{
			Byte0 = (byte)((MSB >> 7) | (LockBit?2:0));
			Byte1 = (byte)((MSB << 1) | (LSB >> 7));
			Byte2 = (byte)(LSB << 1);
		}

		/// <summary>
		/// Interpret the Row as an EPC Row and get the MSB.
		/// </summary>
		public byte EPCMSB { get { return (byte)((Byte0 << 7) | (Byte1 >> 1)); } }
		/// <summary>
		/// Interpret the Row as an EPC Row and get the LSB.
		/// </summary>
		public byte EPCLSB { get { return (byte)((Byte1 << 7) | (Byte2 >> 1)); } }
		/// <summary>
		/// Interpret the Row as an EPC Row and get the Lock Bit.
		/// </summary>
		public bool LockBit { get { return (bool)((Byte0 & 0x02) == 0x02); } }

		/// <summary>
		/// Fill the Row's bits with half a Kill PassCode (12 bits).
		/// </summary>
		/// <param name="PassCode">Half a Kill PassCode (12-bits).</param>
		/// <param name="LockBit">Whether this row is locked or not (bit 17).</param>
		public void PassCodeRow(ushort PassCode, bool LockBit)
		{
			PassCode &= 0x0FFF;
			Byte0 = (byte)((PassCode >> 11) | (LockBit?2:0));
			Byte1 = (byte)(PassCode >> 3);
			Byte2 = (byte)(PassCode << 5);
		}

		/// <summary>
		/// If Clear == true, fill the Fab Protect bits with 0x0000,
		/// else fill it with the Golden Word = 0x997A.
		/// Always Row 0.
		/// </summary>
		/// <param name="Clear">Whether to clear this row, or fill it with the Golden Word.</param>
		/// <param name="LockBit">Whether this row is locked.</param>
		public void FabProtect(bool Clear, bool LockBit)
		{
			RowNumber = 0;
			Byte0 = (byte)(LockBit?2:0);
			Byte1 = (byte)((Clear)?0:0x99);
			Byte2 = (byte)((Clear)?0:0x7A);
		}

		/// <summary>
		/// Fill the Row's bits with the proper values for a given
		/// EPC length (in bytes).
		/// </summary>
		/// <param name="Length">Length of the EPC, in bytes: [8, 12, or 16].</param>
		/// <param name="LockBit">Whether this row is locked.</param>
		/// <returns>true if Length is Valid; otherwise false.</returns>
		public bool ControlWord(int Length, bool LockBit)
		{
			if ((Length == 8) || (Length == 12) || (Length == 16)) 
			{
				Length = Length/2+1;
				RowNumber = 1;
				Byte0 = (byte)(LockBit?1:0);
				Byte1 = (byte)(Length << 4);
				Byte2 = 0;
				return true;
			} 
			else 
				return false;
		}

		/// <summary>
		/// Fill the Row with 16 bits of user data.
		/// bit 16 can be used as a WriteFlag.
		/// bit 17 is the lock bit for this row.
		/// </summary>
		/// <param name="MSB">Most Significant 8 bits of the user data in this row.</param>
		/// <param name="LSB">Least Significant 8 bits of the user data in this row.</param>
		/// <param name="WriteFlag">Can be used to signify that this row has been written.</param>
		/// <param name="LockBit">Whether this row is locked.</param>
		public void UserData(byte MSB, byte LSB, bool WriteFlag, bool LockBit)
		{
			Byte0 = (byte)((LockBit?2:0) | (WriteFlag?1:0));
			Byte1 = MSB;
			Byte2 = LSB;
		}


		/// <summary>
		/// Construct a Zuma Row, with just a RowNumber
		/// </summary>
		/// <param name="RowNumber">The Row Number (0-15).</param>
		public ZumaRow(byte RowNumber) { this.RowNumber = RowNumber; }

		/// <summary>
		/// Construct a Zuma Row with given its 3 bytes of data.
		/// </summary>
		/// <param name="RowNumber">The Row Number (0-15).</param>
		/// <param name="Byte0">Bits 16-17.</param>
		/// <param name="Byte1">Bits 8-15.</param>
		/// <param name="Byte2">Bits 0-7.</param>
		public ZumaRow(byte RowNumber, byte Byte0, byte Byte1, byte Byte2) 
		{ this.RowNumber = RowNumber; this.Byte0 = Byte0; this.Byte1 = Byte1; this.Byte2 = Byte2; }

		/// <summary>
		/// Construct an empty Zuma Row.
		/// </summary>
		public ZumaRow() {}
	}
}
