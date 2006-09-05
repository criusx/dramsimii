using System;
using System.Net.Sockets;

namespace RFIDProtocolLib
{
    /// <summary>
    /// This is a client connection to the server.
    /// </summary>
    public class ClientConnection
    {
        private TcpClient c;

        /// <summary>
        /// Normal constructor.
        /// </summary>
        public ClientConnection()
        {
            c = new TcpClient();
        }

        /// <summary>
        /// Connects to a remote host.
        /// </summary>
        /// <param name="host">The host to connect to.</param>
        /// <param name="port">Its port.</param>
        public void Connect(string host, int port)
        {
            c.Connect(host, port);
        }

        /// <summary>
        /// Close the connection.
        /// </summary>
        public void Close()
        {
			c.Client.Close(); 
			c.Close();
        }

         public void SendPacket(Packet req)
         {
             TLV connectPacket = new TLV(req.Type,req.ToTLVList().GetBytes());
             connectPacket.WriteToStream(c.GetStream());
         }

         /// <summary>
         /// Sent from the server back after a Query packet is sent.
         /// This doesn't do it yet, but will throw an exception if it receives
         /// an ERROR packet.
         /// </summary>
         /// <returns>A QueryResponse describing the RFID.</returns>
         public QueryResponse WaitForQueryResponsePacket()
         {
             TLV responsePacket = new TLV();
             while (responsePacket.Type != (ushort)Packets.QueryResponse)
                 responsePacket.ReadFromStream(c.GetStream());

             return new QueryResponse(responsePacket.Value);
         }

    }
}
