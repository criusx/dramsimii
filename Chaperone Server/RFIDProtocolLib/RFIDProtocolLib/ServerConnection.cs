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

        #region SetPhoneNumber
        public SetPhoneNumberRequest WaitForSetPhoneNumberPacket()
        {
            if (!c.GetStream().DataAvailable)
                throw new IOException("Data not there!");

            TLV packet = new TLV();
            while (packet.Type != QueryRequest.Type)
                packet.ReadFromStream(c.GetStream());

            return new SetPhoneNumberRequest(packet.Value);
        }

        public void SendSetPhoneNumberPacket(SetPhoneNumberResponse resp)
        {
            TLV responsePacket = new TLV(SetPhoneNumberResponse.Type, resp.ToTLVList().GetBytes());
            responsePacket.WriteToStream(c.GetStream());
        }
        #endregion

        #region RaiseAlert
        public RaiseAlertRequest WaitForSRaiseAlertPacket()
        {
            if (!c.GetStream().DataAvailable)
                throw new IOException("Data not there!");

            TLV packet = new TLV();
            while (packet.Type != RaiseAlertRequest.Type)
                packet.ReadFromStream(c.GetStream());

            return new RaiseAlertRequest(packet.Value);
        }

        public void SendRaiseAlertPacket(RaiseAlertResponse resp)
        {
            TLV responsePacket = new TLV(RaiseAlertResponse.Type, resp.ToTLVList().GetBytes());
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
