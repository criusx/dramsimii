using System;

namespace RFIDProtocolLib
{
    /// <summary>
    /// An RFID.
    /// </summary>
    public class RFID
    {
        private byte[] rfid;

        /// <summary>
        /// Construct it from a byte array.
        /// </summary>
        /// <param name="data">The array.</param>
        public RFID(byte[] data)
        {
            if (data.Length != 12)
                throw new ArgumentException("RFID of invalid length!", "data");

            rfid = data;
        }

        /// <summary>
        /// Construct it from a byte array.
        /// </summary>
        /// <param name="data">The array.</param>
        public RFID(char[] data)
        {
            if (data.Length != 24)
                throw new ArgumentException("RFID of invalid length!", "data");

            rfid = new byte[12];
            
            for (int i = 0; i < 12; i++)
                rfid[i] = byte.Parse(new string(new char[] { data[2*i], data[2*i+1] }),System.Globalization.NumberStyles.HexNumber);
        }

        /// <summary>
        /// Construct it from a string.  Can be with or without spaces.
        /// </summary>
        /// <param name="hexString">The string.</param>
        public RFID(string hexString)
        {
            string newString = "";
            char c;
            // remove all none A-F, 0-9, characters
            for (int i = 0; i < hexString.Length; i++)
            {
                c = hexString[i];
                if (IsHexDigit(c))
                    newString += c;
                else
                    throw new ArgumentException("Non-hex digit in RFID tag value", "data");
            }            

            int byteLength = newString.Length / 2;
            byte[] bytes = new byte[byteLength];
            string hex;
            int j = 0;
            for (int i = 0; i < bytes.Length; i++)
            {
                hex = new String(new Char[] { newString[j], newString[j + 1] });
                bytes[i] = HexToByte(hex);
                j = j + 2;
            }
            rfid = bytes;
        }

        /// <summary>
        /// Gets the binary represenation of the RFID.
        /// </summary>
        /// <returns>The RFID.</returns>
        public byte[] GetBytes()
        {
            return rfid;
        }

        public override string ToString()
        {
            string hexString = "";
            for (int i = 0; i < rfid.Length; i++)
            {
                hexString += rfid[i].ToString("X2");
            }
            return hexString;
        }

        private bool IsHexDigit(char c)
        {
            int numChar;
            int numA = Convert.ToInt32('A');
            int num1 = Convert.ToInt32('0');
            c = Char.ToUpper(c);
            numChar = Convert.ToInt32(c);
            if (numChar >= numA && numChar < (numA + 6))
                return true;
            if (numChar >= num1 && numChar < (num1 + 10))
                return true;
            return false;
        }

        private byte HexToByte(string hex)
        {
            if (hex.Length > 2 || hex.Length <= 0)
                throw new ArgumentException("hex must be 1 or 2 characters in length");
            byte newByte = byte.Parse(hex, System.Globalization.NumberStyles.HexNumber);
            return newByte;
        }
    }
}
