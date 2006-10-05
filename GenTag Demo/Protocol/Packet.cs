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
            { return data.Length - HEADER_SIZE; }
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

            Buffer.BlockCopy(BitConverter.GetBytes(data.Length),0,data,LENGTH,BitConverter.GetBytes(data.Length).Length);
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

            Buffer.BlockCopy(BitConverter.GetBytes(data.Length), 0, data, LENGTH, BitConverter.GetBytes(data.Length).Length);
        }

        public void SendPacket()
        {
            stream.Write(data, 0, HEADER_SIZE);

            if (data.Length > HEADER_SIZE)
            {
                stream.Write(data, HEADER_SIZE, data.Length - HEADER_SIZE);
            }
        }

        public void GetPacket()
        {
            stream.Read(data, 0, HEADER_SIZE);

            if (data[LENGTH] > 0)
            {
                byte[] newData = new byte[data[LENGTH]];

                Buffer.BlockCopy(data, 0, newData, 0, HEADER_SIZE);

                stream.Read(newData, HEADER_SIZE, newData[LENGTH] - HEADER_SIZE);

                data = newData;
            }
        }
    };
    #endregion
}