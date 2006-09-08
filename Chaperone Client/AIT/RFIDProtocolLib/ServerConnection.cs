using System;
using System.Net.Sockets;

namespace RFIDProtocolLib
{
	/// <summary>
	/// This is a server connection that talks to the client.
	/// </summary>
	public class ServerConnection
	{
		private TcpClient c;

		public ServerConnection(TcpClient client)
		{
			c = client;
		}

		public void Close()
		{
			c.Close();
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
            TLV packet = new TLV();
            while (packet.Type != QueryRequest.Type)
                packet.ReadFromStream(c.GetStream());

            return new QueryRequest(packet.Value);
        }

        public void SendQueryResponsePacket(QueryResponse resp)
        {
            TLV responsePacket = new TLV(QueryResponse.Type, resp.ToTLVList().GetBytes());
            responsePacket.WriteToStream(c.GetStream());
        }
        #endregion

#if DONTUSETHIS 
		#region Scan
		public ScanRequest WaitForScanPacket()
		{
			TLV scanPacket = new TLV();
			while (scanPacket.Type != ScanRequest.Type)
				scanPacket.ReadFromStream(c.GetStream());

			return new ScanRequest(scanPacket.Value);
		}

		public void SendScanResponsePacket()
		{
			TLV scanResponsePacket = new TLV(ScanResponse.Type);
			scanResponsePacket.WriteToStream(c.GetStream());
		}
		#endregion

		#region Query
		public QueryRequest WaitForQueryPacket()
		{
			TLV packet = new TLV();
			while (packet.Type != QueryRequest.Type)
				packet.ReadFromStream(c.GetStream());

			return new QueryRequest(packet.Value);
		}

		public void SendQueryResponsePacket(QueryResponse resp)
		{
			TLV responsePacket = new TLV(QueryResponse.Type, resp.ToTLVList().GetBytes());
			responsePacket.WriteToStream(c.GetStream());
		}
		#endregion

        #region Alert
        public AlertRequest WaitForAlertPacket()
        {
            TLV packet = new TLV();
            while (packet.Type != AlertRequest.Type)
                packet.ReadFromStream(c.GetStream());

            return new AlertRequest(packet.Value);
        }

        public void SendAlertResponsePacket(AlertResponse resp)
        {
            TLV responsePacket = new TLV(AlertResponse.Type, resp.ToTLVList().GetBytes());
            responsePacket.WriteToStream(c.GetStream());
        }
        #endregion

        #region CheckManifest
        public CheckManifestRequest WaitForCheckManifestPacket()
        {
            TLV packet = new TLV();
            while (packet.Type != CheckManifestRequest.Type)
                packet.ReadFromStream(c.GetStream());

            return new CheckManifestRequest(packet.Value);
        }

        public void SendCheckManifestResponsePacket(CheckManifestResponse resp)
        {
            TLV responsePacket = new TLV(CheckManifestResponse.Type, resp.ToTLVList().GetBytes());
            responsePacket.WriteToStream(c.GetStream());
        }
        #endregion
#endif
	}
}
