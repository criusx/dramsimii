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
			c.Close();
		}

		#region Connect
		/// <summary>
		/// Send a connect packet to the server.  This initializes the handshake.
		/// </summary>
		public void SendConnectPacket()
		{
			TLV connectPacket = new TLV(0);
			connectPacket.WriteToStream(c.GetStream());
		}

		/// <summary>
		/// Wait for the response from the server saying we're ready to communicate.
		/// </summary>
		public void WaitForConnectResponsePacket()
		{
			TLV connectResponsePacket = new TLV();
			while (connectResponsePacket.Type != 1)
				connectResponsePacket.ReadFromStream(c.GetStream());
		}
		#endregion

        #region Query
        /// <summary>
        /// Sent when the PDA quryies an RFID tag for info about it.
        /// </summary>
        /// <param name="req">The query request.</param>
        public void SendQueryPacket(QueryRequest req)
        {
            TLV packet = new TLV(QueryRequest.Type, req.ToTLVList().GetBytes());
            packet.WriteToStream(c.GetStream());
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
            while (responsePacket.Type != QueryResponse.Type)
                responsePacket.ReadFromStream(c.GetStream());

            return new QueryResponse(responsePacket.Value);
        }
        #endregion

        #region SetPhoneNumber
        public void SendSetPhoneNumberPacket(ReconcileFinishedRequest req)
        {
            TLV packet = new TLV(ReconcileFinishedRequest.Type, req.ToTLVList().GetBytes());
            packet.WriteToStream(c.GetStream());
        }

        public void WaitForSetPhoneNumberResponsePacket()
        {
            TLV responsePacket = new TLV();
            while (responsePacket.Type != SetPhoneNumberResponse.Type)
                responsePacket.ReadFromStream(c.GetStream());
        }
        #endregion

        #region RaiseAlert
        public void SendRaiseAlertPacket(RaiseAlertRequest req)
        {
            TLV packet = new TLV(RaiseAlertRequest.Type, req.ToTLVList().GetBytes());
            packet.WriteToStream(c.GetStream());
        }

        public void WaitForRaiseAlertResponsePacket()
        {
            TLV responsePacket = new TLV();
            while (responsePacket.Type != RaiseAlertResponse.Type)
                responsePacket.ReadFromStream(c.GetStream());
        }
        #endregion

#if DONTUSETHIS
        #region Scan
		/// <summary>
		/// Sent when the PDA scan's an RFID tag.  This updates the DB saying
		/// that a scan happened.
		/// </summary>
		/// <param name="req">The scan request.</param>
		public void SendScanPacket(ScanRequest req)
		{
			TLV scanPacket = new TLV(ScanRequest.Type, req.ToTLVList().GetBytes());
			scanPacket.WriteToStream(c.GetStream());
		}

		/// <summary>
		/// Sent from the server back after a Scan packet is sent.
		/// This doesn't do it yet, but will throw an exception if it receives
		/// an ERROR packet.
		/// </summary>
		public void WaitForScanResponsePacket()
		{
			TLV scanResponsePacket = new TLV();
			while (scanResponsePacket.Type != ScanResponse.Type)
				scanResponsePacket.ReadFromStream(c.GetStream());
		}
        #endregion

        #region Query
		/// <summary>
		/// Sent when the PDA quryies an RFID tag for info about it.
		/// </summary>
		/// <param name="req">The query request.</param>
		public void SendQueryPacket(QueryRequest req)
		{
			TLV packet = new TLV(QueryRequest.Type, req.ToTLVList().GetBytes());
			packet.WriteToStream(c.GetStream());
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
			while (responsePacket.Type != QueryResponse.Type)
				responsePacket.ReadFromStream(c.GetStream());

			return new QueryResponse(responsePacket.Value);
		}
        #endregion

        #region Alert
        /// <summary>
        /// Sent by the PDA when there is an alert.
        /// </summary>
        /// <param name="req">The alert request.</param>
        public void SendAlertPacket(AlertRequest req)
        {
            TLV packet = new TLV(AlertRequest.Type, req.ToTLVList().GetBytes());
            packet.WriteToStream(c.GetStream());
        }

        /// <summary>
        /// Sent from the server back after an alert is sent.
        /// This doesn't do it yet, but will throw an exception if it receives
        /// an ERROR packet.
        /// </summary>
        /// <returns>A QueryResponse describing the RFID.</returns>
        public AlertResponse WaitForAlertResponsePacket()
        {
            TLV responsePacket = new TLV();
            while (responsePacket.Type != AlertResponse.Type)
                responsePacket.ReadFromStream(c.GetStream());

            return new AlertResponse(responsePacket.Value);
        }
        #endregion

        #region CheckManifest
        /// <summary>
        /// Sent by the PDA to do a manifest check.
        /// </summary>
        /// <param name="req">The request.</param>
        public void SendCheckManifestPacket(CheckManifestRequest req)
        {
            TLV packet = new TLV(CheckManifestRequest.Type, req.ToTLVList().GetBytes());
            packet.WriteToStream(c.GetStream());
        }

        /// <summary>
        /// Sent from the server back after a manifest check.
        /// This doesn't do it yet, but will throw an exception if it receives
        /// an ERROR packet.
        /// </summary>
        /// <returns>A CheckManifestResponse.</returns>
        public CheckManifestResponse WaitForCheckManifestResponsePacket()
        {
            TLV responsePacket = new TLV();
            while (responsePacket.Type != CheckManifestResponse.Type)
                responsePacket.ReadFromStream(c.GetStream());

            return new CheckManifestResponse(responsePacket.Value);
        }
        #endregion
#endif
    }
}
