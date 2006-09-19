using System;
using System.IO;
using System.Net.Sockets;
using System.Collections;

namespace RFIDProtocolLib
{
	/// <summary>
	/// This is a server connection that talks to the client.
	/// </summary>
	public class ServerConnection
	{
		private TcpClient c;		

        public static int SessionIdStat = 0;
        public int SessionId;

		public ServerConnection(TcpClient client)
		{
            SessionId = SessionIdStat++;
			c = client;
		}

		public void Close()
		{
			c.Close();
		}

        public bool Connected
        {
            get { return c.Connected; }
        }

        public NetworkStream GetStream()
        {
            return c.GetStream();
        }

		#region Connect
		public void WaitForConnectPacket()
		{
			TLV connectPacket = new TLV();
			while (connectPacket.Type != 0)
				connectPacket.ReadFromStream(c.GetStream());
		}

		public void SendConnectResponsePacket()
		{
			TLV connectResponsePacket = new TLV(1);
			connectResponsePacket.WriteToStream(c.GetStream());
		}
		#endregion

        #region Query
        public QueryRequest WaitForQueryPacket()
        {
            if (!c.GetStream().DataAvailable)
                throw new IOException("Data not there!");

            TLV packet = new TLV();
            while (packet.Type != (ushort)Packets.QueryRequest)
                packet.ReadFromStream(c.GetStream());


            return new QueryRequest(packet.Value);
        }

        public void SendQueryResponsePacket(QueryResponse resp)
        {
            TLV responsePacket = new TLV((ushort)Packets.QueryResponse, resp.ToTLVList().GetBytes());
            responsePacket.WriteToStream(c.GetStream());
        }
        #endregion

		#region sendAck
		public void sendAck()
		{
			TLV ackPacket = new TLV((ushort)Packets.Ack);
			ackPacket.WriteToStream(c.GetStream());
		}
		#endregion 

		#region SetPhoneNumber
		public ReconcileFinished WaitForSetPhoneNumberPacket()
        {
            if (!c.GetStream().DataAvailable)
                throw new IOException("Data not there!");

            TLV packet = new TLV();
            while (packet.Type != (ushort)Packets.QueryRequest)
                packet.ReadFromStream(c.GetStream());

            return new ReconcileFinished(packet.Value);
        }

		//public void SendSetPhoneNumberPacket(SetPhoneNumberResponse resp)
		//{
		//    TLV responsePacket = new TLV(SetPhoneNumberResponse.Type, resp.ToTLVList().GetBytes());
		//    responsePacket.WriteToStream(c.GetStream());
		//}
        #endregion

        #region RaiseAlert
        public RaiseAlert WaitForSRaiseAlertPacket()
        {
            if (!c.GetStream().DataAvailable)
                throw new IOException("Data not there!");

            TLV packet = new TLV();
            while (packet.Type != (ushort)Packets.RaiseAlert)
                packet.ReadFromStream(c.GetStream());

            return new RaiseAlert(packet.Value);
        }

		//public void SendRaiseAlertPacket(RaiseAlertResponse resp)
		//{
		//    TLV responsePacket = new TLV(RaiseAlertResponse.Type, resp.ToTLVList().GetBytes());
		//    responsePacket.WriteToStream(c.GetStream());
		//}
        #endregion
	}
}
