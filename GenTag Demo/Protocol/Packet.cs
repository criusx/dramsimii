using System;
using System.Collections.Generic;
using System.Text;

namespace Protocol
{
    #region PacketTypes
    public enum PacketTypes: byte
    {
        UnknownType = 0, DescriptionRequest = 1, CloseConnectionRequest = 2, DescriptionResponse = 3
    }
    #endregion

    #region Packet
    /// <summary>
    /// Provides a way to structure data so it may be sent and received efficiently
    /// </summary>
    public class Packet
    {
        private const int HEADER_SIZE = 5;

        /// <summary>
        /// Position in the packet of the type byte
        /// </summary>
        private const int TYPE = 4;

        /// <summary>
        /// Position in the packet of the first length byte
        /// </summary>
        private const int LENGTH = 0;

        public const int port = 1492;

        private byte[] data;

        System.IO.Stream stream = null;

        public PacketTypes Type
        {
            get
            {
                return (PacketTypes)data[TYPE];
            }
            set
            {
                data[TYPE] = (byte)value;
            }
        }

        public int Length
        {
            get
            { return BitConverter.ToInt32(data, LENGTH); }
        }

        public System.IO.Stream Stream
        {
            get { return stream; }
            set { stream = value; }
        }

        public byte[] Data
        {     
            get
            {
                return data;
            }
            set
            {
                byte[] newData = new byte[HEADER_SIZE + value.Length];                
                Buffer.BlockCopy(data, 0, newData, 0, HEADER_SIZE);
                Buffer.BlockCopy(value, 0, newData, HEADER_SIZE, value.Length);
                data = newData;
            }
        }

        /// <summary>
        /// Creates a standard Packet with no data
        /// </summary>
        public Packet()
        {
            data = new byte[HEADER_SIZE];

            data[TYPE] = (byte)PacketTypes.UnknownType;

            Buffer.BlockCopy(BitConverter.GetBytes(data.Length),0,data,LENGTH,4);
        }

        /// <summary>
        /// Creates a Packet of a certain type but with no data
        /// </summary>
        /// <param name="type"></param>
        public Packet(PacketTypes type)
        {
            data = new byte[HEADER_SIZE];

            data[TYPE] = (byte)type;

            Buffer.BlockCopy(BitConverter.GetBytes(data.Length), 0, data, LENGTH, BitConverter.GetBytes(data.Length).Length);
        }

        /// <summary>
        /// constructs a packet of a given type with the specified body
        /// </summary>
        /// <param name="type"></param>
        /// <param name="body"></param>
        /// <exception cref="ArgumentException">Thrown if the size of the body cannot be expressed in one int</exception>
        public Packet(PacketTypes type, byte[] body)
        {
            if (body.Length > int.MaxValue)
                throw new ArgumentOutOfRangeException("body","Was too large to be expressed as an int");

            data = new byte[HEADER_SIZE + body.Length];

            data[TYPE] = (byte)type;

            // set the bytes representing the length
            Buffer.BlockCopy(BitConverter.GetBytes(data.Length), 0, data, LENGTH, BitConverter.GetBytes(data.Length).Length);

            // copy the body over
            Buffer.BlockCopy(body, 0, data, HEADER_SIZE, body.Length);
        }

        public Packet(PacketTypes type, string body)
        {
            if (body.Length > int.MaxValue)
                throw new ArgumentOutOfRangeException("body", "Was too large to be expressed as an int");

            byte[] stringArray = System.Text.Encoding.ASCII.GetBytes(body);

            data = new byte[HEADER_SIZE + stringArray.Length];

            data[TYPE] = (byte)type;

            // copy the bytes representing the length over
            Buffer.BlockCopy(BitConverter.GetBytes(data.Length), 0, data, LENGTH, BitConverter.GetBytes(data.Length).Length);

            // copy the body over
            Buffer.BlockCopy(stringArray, 0, data, HEADER_SIZE, stringArray.Length);
        }

        public void SendPacket()
        {
            stream.Write(data, 0, HEADER_SIZE);

            if (Length > HEADER_SIZE)
            {
                stream.Write(data, HEADER_SIZE, Length - HEADER_SIZE);
            }
        }

        

        public void GetPacket()
        {
            stream.Read(data, 0, HEADER_SIZE);            

            if (Length > 0)
            {
                byte[] newData = new byte[Length];

                Buffer.BlockCopy(data, 0, newData, 0, HEADER_SIZE);

                stream.Read(newData, HEADER_SIZE, Length - HEADER_SIZE);

                data = newData;
            }
        }

        public override string ToString()
        {
            return System.Text.Encoding.ASCII.GetString(data, HEADER_SIZE, data.Length - HEADER_SIZE);
        }

    };
    #endregion
}
