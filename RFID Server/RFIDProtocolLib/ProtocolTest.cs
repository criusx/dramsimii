using System;
using System.Collections;
using System.Net.Sockets;

#if USING_NUNIT

using NUnit.Framework;

namespace RFIDProtocolLib
{
	/// <summary>
	/// These are just protocol tests of the backend.
	/// </summary>
	[TestFixture]
	public class ProtocolTest
	{
		private const string HOST = "localhost";
		private const int PORT = 1555;

		[Test]
		public void TcpListenerClientScenario()
		{
			TcpListener s = new TcpListener(System.Net.IPAddress.Loopback, PORT);
			s.Start();

			TcpClient cc = new TcpClient();
			cc.Connect(HOST, PORT);
			
			TcpClient sc = s.AcceptTcpClient();

			TLV tlv = new TLV(0, null);
			tlv.WriteToStream(cc.GetStream());

			TLV tlv2 = new TLV();
			tlv2.ReadFromStream(sc.GetStream());

			Assert.AreEqual(tlv.Type, tlv2.Type);

			s.Stop();
		}

		[Test]
		public void ClientServerConnect()
		{
			Daemon daemon = new Daemon(PORT);
			daemon.Start();

			ClientConnection c = new ClientConnection();
			c.Connect(HOST, PORT);

			ServerConnection s = daemon.AcceptClientConnection();

			c.SendConnectPacket();
			s.WaitForConnectPacket();
			s.SendConnectResponsePacket();
			c.WaitForConnectResponsePacket();

			c.Close();
			s.Close();

			daemon.Stop();
		}

		[Test]
		public void ScanProtocolTest()
		{
			Daemon daemon = new Daemon(PORT);
			daemon.Start();

			ClientConnection c = new ClientConnection();
			c.Connect(HOST, PORT);

			ServerConnection s = daemon.AcceptClientConnection();

			c.SendConnectPacket();
			s.WaitForConnectPacket();
			s.SendConnectResponsePacket();
			c.WaitForConnectResponsePacket();

			RFID rfid = new RFID("31 32 33 34 35 36 00 00 00 00 00 42");
			ScanRequest req = new ScanRequest(rfid, "Person 1", "100.553321", "10.55324");
			c.SendScanPacket(req);
			ScanRequest req2 = s.WaitForScanPacket();
			
			Assert.AreEqual(req.Rfid.GetBytes(), req2.Rfid.GetBytes());
			Assert.AreEqual(req.Timestamp, req2.Timestamp);
			Assert.AreEqual(req.ScannerID, req2.ScannerID);
			Assert.AreEqual(req.Latitude, req2.Latitude);
			Assert.AreEqual(req.Longitude, req2.Longitude);

			//DBBackend.ScanRFID(req2);
			
			s.SendScanResponsePacket();
			c.WaitForScanResponsePacket();

			c.Close();
			s.Close();

			daemon.Stop();
		}

		[Test]
		public void QueryProtocolTest()
		{
			Daemon daemon = new Daemon(PORT);
			daemon.Start();

			ClientConnection c = new ClientConnection();
			c.Connect(HOST, PORT);

			ServerConnection s = daemon.AcceptClientConnection();

			c.SendConnectPacket();
			s.WaitForConnectPacket();
			s.SendConnectResponsePacket();
			c.WaitForConnectResponsePacket();

			RFID rfid = new RFID("31 32 33 34 35 36 00 00 00 00 00 42");
			QueryRequest req = new QueryRequest(rfid);
			c.SendQueryPacket(req);
			QueryRequest req2 = s.WaitForQueryPacket();
			
			Assert.AreEqual(req.Rfid.GetBytes(), req2.Rfid.GetBytes());
			
			QueryResponse resp = DBBackend.QueryRFID(req2);
			
			s.SendQueryResponsePacket(resp);
			QueryResponse resp2 = c.WaitForQueryResponsePacket();

			Assert.AreEqual(resp.ShortDesc, resp2.ShortDesc);
			Assert.AreEqual(resp.LongDesc, resp2.LongDesc);
			Assert.AreEqual(resp.IconName, resp2.IconName);

			c.Close();
			s.Close();

			daemon.Stop();
		}

		/*public void UpdateTransitHistoryTest()
		{
			Daemon daemon = new Daemon(PORT);
			daemon.Start();

			ClientConnection c = new ClientConnection();
			c.Connect(HOST, PORT);

			ServerConnection s = daemon.AcceptClientConnection();

			c.SendConnectPacket();
			s.WaitForConnectPacket();
			s.SendConnectResponsePacket();
			c.WaitForConnectResponsePacket();

			RFID rfid = new RFID("31 32 33 34 35 36 00 00 00 00 00 42");
			UpdateTransitHistoryRequest req = new UpdateTransitHistoryRequest(rfid, "Place 1", "100", "200");
			c.SendQueryPacket(req);
			QueryRequest req2 = s.WaitForQueryPacket();
			
			Assert.AreEqual(req.Rfid.GetBytes(), req2.Rfid.GetBytes());
			
			QueryResponse resp = DBBackend.QueryRFID(req2);
			
			s.SendQueryResponsePacket(resp);
			QueryResponse resp2 = c.WaitForQueryResponsePacket();

			Assert.AreEqual(resp.ShortDesc, resp2.ShortDesc);
			Assert.AreEqual(resp.LongDesc, resp2.LongDesc);
			Assert.AreEqual(resp.IconName, resp2.IconName);

			c.Close();
			s.Close();

			daemon.Stop();
		}*/

        [Test]
        public void VoIPTest()
        {
            //VoIP.VoIPConnection conn = new VoIP.VoIPConnection("3796002", "bello", "inphonex.com", "5060", "3000");
            //conn.call("6318273880", "temp1.raw");
        }

        [Test]
        public void AlertProtocolTest()
        {
            Daemon daemon = new Daemon(PORT);
            daemon.Start();

            ClientConnection c = new ClientConnection();
            c.Connect(HOST, PORT);

            ServerConnection s = daemon.AcceptClientConnection();

            c.SendConnectPacket();
            s.WaitForConnectPacket();
            s.SendConnectResponsePacket();
            c.WaitForConnectResponsePacket();

            AlertRequest req = new AlertRequest(0);
            c.SendAlertPacket(req);
            AlertRequest req2 = s.WaitForAlertPacket();

            Assert.AreEqual(req.AlertType, req2.AlertType);

            AlertResponse resp = new AlertResponse();

            /*try
            {
                VoIP.VoIPConnection conn = new VoIP.VoIPConnection("3796002", "bello", "inphonex.com", "5060", "3000");
                conn.call("6318273880", "temp1.raw");
            }
            catch
            {
            }*/

            s.SendAlertResponsePacket(resp);
            AlertResponse resp2 = c.WaitForAlertResponsePacket();

            c.Close();
            s.Close();

            daemon.Stop();
        }

        [Test]
        public void CheckManifestProtocolTest()
        {
            Daemon daemon = new Daemon(PORT);
            daemon.Start();

            ClientConnection c = new ClientConnection();
            c.Connect(HOST, PORT);

            ServerConnection s = daemon.AcceptClientConnection();

            c.SendConnectPacket();
            s.WaitForConnectPacket();
            s.SendConnectResponsePacket();
            c.WaitForConnectResponsePacket();

            RFID rfid = new RFID("31 32 33 34 35 36 00 00 00 00 00 42");
            ArrayList manifest = new ArrayList();
            manifest.Add(new RFID("31 32 33 34 35 36 00 00 00 00 00 43"));
            manifest.Add(new RFID("31 32 33 34 35 36 00 00 00 00 00 44"));
            CheckManifestRequest req = new CheckManifestRequest(rfid, manifest, 0);

            c.SendCheckManifestPacket(req);
            CheckManifestRequest req2 = s.WaitForCheckManifestPacket();
            Assert.AreEqual(req.Manifest.Count, req2.Manifest.Count);

            // find out what waypoint i'm at
            // for each rfid see if it should be there or not

            s.SendCheckManifestResponsePacket(new CheckManifestResponse());
            CheckManifestResponse resp = c.WaitForCheckManifestResponsePacket();

            c.Close();
            s.Close();

            daemon.Stop();
        }
	}
}

#endif // USING_NUNIT