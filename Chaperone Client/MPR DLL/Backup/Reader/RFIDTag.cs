//==========================================================================================
//
//		WJ.MPR.Reader.RFIDTag
//		Copyright (c) 2005, WJ Communications, Inc.
//
//==========================================================================================
using System;
using WJ.MPR.Util;

namespace WJ.MPR.Reader
{
	/// <summary>
	/// Enumeration for the known RFID protocols.
	/// </summary>
	public enum RFIDProtocol
	{
		/// <summary>
		/// The RFID Protocol for a tag is not known or specified.
		/// </summary>
		Unknown,
		/// <summary>
		/// EPC Gen 1 Class 0.
		/// </summary>
		EPCClass0,
		/// <summary>
		/// EPC Gen 1 Class 1.
		/// </summary>
		EPCClass1,
		/// <summary>
		/// EPC Gen 2 Class 1.
		/// </summary>
		EPCGen2
	}

	/// <summary>
	///	Encapsulates an RFID Tag.
	///	Includes properties for the Tag ID, the RFID Protocol (enum), "First Read Time" and 
	///	"Last Read Time" 
	///	Includes methods for computing Class 0 and Class 1 CRCs.
	///	Incomplete: method to convert raw TagID to EPC.
	/// </summary>
	public class RFIDTag
	{
		#region properties
		private byteList tagID;

		private RFIDProtocol protocol;

		private DateTime firstRead;
		/// <summary>
		/// A timestamp for the first time this tag was read.
		/// </summary>
		public DateTime FirstRead { get {return firstRead;} }

		private DateTime lastRead;
		/// <summary>
		/// A timestamp for the most recent time this tag was read.
		/// </summary>
		public DateTime LastRead { get {return lastRead;} set { lastRead = value; }}

		/// <summary>
		/// The CRC of this Tag.
		/// </summary>
		public ushort CRC;
		
		#region protocol specific properties
		/// <summary>
		/// Kill Passcode is one byte for EPC UHF Class 1,
		/// and is the LSB of the row (16-bits) also containing
		/// the LockCode
		/// </summary>
		public byte KillCode;	// Class 1
		/// <summary>
		/// The Lock Code is one byte for EPC UHF Class 1, 
		/// and is the MSB of the row (16-bits) also containing
		/// the Kill Passcode.
		/// Program to A5 to lock a tag.
		/// </summary>
		public byte LockCode;	// Class 1

		/// <summary>
		/// This value, when programmed into the MSB of the last row of a Class 1 Tag,
		/// will lock the tag so that Kills are permanent, and all programming commands
		/// {Program, Verify, Erase} are disabled.
		/// </summary>
		public const byte Class1LockCodeValue = 0xA5;

		/// <summary>
		/// ePC UHF Class 0+ (Zuma)
		/// Class0+ Impinj 16 3-byte rows of row data
		/// </summary>
		public byte[] RowData;	
		#endregion

		/// <summary>
		/// The RFID Protocol of this Tag.
		/// </summary>
		public RFIDProtocol Protocol { get { return protocol; } }
		/// <summary>
		/// The ID of the tag, as a string of Hexadecimal digits.
		/// </summary>
		public string TagID { get { return tagID.ToHexString(); } }
	
		/// <summary>
		/// The ID of the tag as a byte array.
		/// </summary>
		public byte[] ToByteArray { get { return tagID.ToArray(); } }
		
		#endregion

		/// <summary>
		/// Construct a Tag object with the given tag ID and Protocol.
		/// </summary>
		/// <param name="tagID">A list of bytes that form the Tag ID.</param>
		/// <param name="protocol">The RFID protocol that this tag uses.</param>
		public RFIDTag(byteList tagID, RFIDProtocol protocol)
		{
			firstRead = DateTime.Now;
			lastRead = DateTime.Now;
			this.tagID = tagID;
			this.protocol = protocol;
		}

		/// <summary>
		/// Converts the Tag ID to a string of hexadecimal digits.
		/// </summary>
		/// <returns></returns>
		override public string ToString()
		{
			return tagID.ToHexString();
		}

		/// <summary>
		/// Update the Tag's lastRead property.
		/// </summary>
		/// <param name="lastread">The time the Tag was last read.</param>
		public void ReRead(DateTime lastread)
		{
			this.lastRead = lastread;
		}

		/// <summary>
		/// Incompletely implemented property to convert TagID to EPC 
		/// dotted.decimal notation.
		/// </summary>
		public string ePC 
		{
			get 
			{
				string Manager = "";
				string ObjectClass = "";
				string SerialNumber = "";
				
				int manVal, objVal, snVal;
				
				switch (tagID.Count) 
				{
					case 8:
					switch (tagID[0] & 0xC0) 
					{
						case 0x00:
							break;
						case 0x40:	// class I: 21.17.24
							manVal = tagID[2] + (tagID[1] << 8) + (tagID[0] << 16);
							manVal >>= 1;
							manVal &= 0x001fffff;	// 21 bits
							Manager = manVal.ToString();

							objVal = tagID[4] + (tagID[3] << 8) + (tagID[2] << 16);
							objVal  &= 0x0001ffff;	// 17 bits
							ObjectClass = objVal.ToString();

							snVal = tagID[7] + (tagID[6] << 8) + (tagID[5] << 16);
							SerialNumber = snVal.ToString();

							break;
						case 0x80:	// class II: 15.13.34
							manVal = tagID[2] + (tagID[1] << 8) + (tagID[0] << 16);
							manVal >>= 7;
							manVal &= 0x00007fff;	// 15 bits
							Manager = manVal.ToString();

							objVal = tagID[4] + (tagID[3] << 8) + (tagID[2] << 16);
							objVal >>= 2;
							objVal &= 0x00001fff;	// 13 bits
							ObjectClass = objVal.ToString();

							long LsnVal = tagID[7] + (tagID[6] << 8) + (tagID[5] << 16) + (tagID[4] << 24) + (tagID[3] << 32);
							LsnVal &= 0x00000003ffffffff;	// 34 bits!
							SerialNumber = LsnVal.ToString();

							break;
						case 0xC0:	// class III: 26.13.23
							manVal = tagID[3] + (tagID[2] << 8) + (tagID[1] << 16) + (tagID[0] << 24);
							manVal >>= 4;
							manVal &= 0x03ffffff;	// 26 bits
							Manager = manVal.ToString();

							objVal = tagID[5] + (tagID[4] << 8) + (tagID[3] << 16);
							objVal >>= 7;
							objVal &= 0x00001fff;	// 13 bits
							ObjectClass = objVal.ToString();

							snVal = tagID[7] + (tagID[6] << 8) + (tagID[5] << 16);
							snVal &= 0x007fffff;	// 13 bits
							SerialNumber = snVal.ToString();

							break;
					}
						break;
					case 12:

						break;
					default:
						break;
				}
				return Manager + "." + ObjectClass + "." + SerialNumber;
			}
		}


		/// <summary>
		/// Computes the EPC UHF Gen 1 Class 1 CRC for the TagID bits of an RFIDTag.
		/// </summary>
		/// <param name="mTag">The Tag whose Tag ID to compute the CRC of.</param>
		/// <returns>The 16bit CRC.</returns>
		public static ushort Class1CRC(RFIDTag mTag)
		{
			return Class1CRC(new byteList(mTag.ToByteArray));
		}

		/// <summary>
		/// Compute the CRC for a byteList representing the TagID of a Class 1 Tag.
		/// </summary>
		/// <param name="TagID">The Tag whose Tag ID to compute the CRC of.</param>
		/// <returns>The 16bit CRC.</returns>
		public static ushort Class1CRC(byteList TagID)
		{
			const ushort POLY = 0x1021;
			if (TagID == null) return 0;

			ushort CRCReg = 0xFFFF;
			TagID.Add(0);
			TagID.Add(0);
			for (int index = 0; index < TagID.Count; index++)
				for (byte Mask = 0x80; Mask != 0; Mask >>= 1) 
				{
					bool C = ((CRCReg & 0x8000) == 0x8000);
					CRCReg <<= 1;
					CRCReg |= (ushort)(((TagID[index] & Mask) != 0)?1:0);
					if (C) CRCReg ^= POLY;
				}
			return CRCReg;
		}


		/// <summary>
		/// The Class0 TagID CRC calculation algorithm is table based.
		/// The following is the table computed for the x^16+x^12+x^5+1
		/// polynomial.
		/// According to the Class0 spec, this is the CCITT CRC.
		/// </summary>
		private static readonly ushort[] crcTable =
			{
				0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
				0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
				0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
				0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
				0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
				0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D, 
				0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4, 
				0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC, 
				0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823, 
				0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B, 
				0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, 
				0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A, 
				0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, 
				0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49, 
				0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70, 
				0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78, 
				0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F, 
				0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067, 
				0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E, 
				0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256, 
				0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D, 
				0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 
				0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C, 
				0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634, 
				0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB, 
				0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3, 
				0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A, 
				0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92, 
				0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9, 
				0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1, 
				0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, 
				0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0 
			};

		/// <summary>
		/// Calculate the EPC Class 0 CRC.
		/// A table computed for the x^16+x^12+x^5+1 polynomial is used.
		/// </summary>
		/// <param name="TagID">The bytes to compute the CRC of.</param>
		/// <returns>The 16-bit CRC.</returns>
		public static ushort Class0CRC(byteList TagID)
		{
			ushort crc = 0xFFFF;
			if (TagID == null) return 0;
			foreach (byte b in TagID)
				crc = (ushort)((crc << 8) ^ crcTable[(byte)((crc >> 8) ^ b)]);
			return crc;
		}
	}
}
