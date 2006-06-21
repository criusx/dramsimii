using System;
using System.Diagnostics;
using Oracle.DataAccess.Client;
using System.Net;

#if USING_NUNIT
using NUnit.Framework;
#endif

namespace RFIDProtocolLib
{
	/// <summary>
	/// This is the wrapper for the database backend.
	/// This will all be thrown away after Monday.
	/// </summary>
	public class DBBackend
	{
		//private const string DBSERVER = "tbk.ece.umd.edu";
		private const string DBSERVER = "SRL";
		private const string DATABASE = "RFID";

		private static OracleConnection c;
		//private static OdbcConnection c;

        public static int Status = 0;

		private static void Open()
		{
			//string MyConString = "DRIVER={MySQL ODBC 3.51 Driver};" +
			//	"SERVER=" + DBSERVER + ";" +
			//	"DATABASE=" + DATABASE + ";" +
			//	"UID=root;" +
			//	"PASSWORD=bello;" +
			//	"OPTION=3";
			string MyConString = "User Id=rfid;" +
				"Password=rfid#srl#13rfid;" +
				"Data Source=" + DBSERVER;

			//c = new OdbcConnection(MyConString);
			c = new OracleConnection(MyConString);
			try
			{
				c.Open();
				Console.WriteLine("Connected to Oracle DB " + c.ServerVersion);
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
			}
			
		}


		public static QueryResponse QueryRFID(QueryRequest req, ServerConnection serverconn)
		{
            byte[] rfidBytes = req.Rfid.GetBytes();
            req.Rfid = new RFID(rfidBytes);
            Open();

            /* update the database */
			OracleCommand cmd = new OracleCommand();
			cmd.Connection = c;

            if (true)//req.IsScan == 1) // subsequent scan, check against the manifest
            {
				//cmd.CommandText = "INSERT INTO itemScanHistory (rfid, latitude, longitude) VALUES (\"" +
				//	req.Rfid.ToString() + "\", \"" +
				//	req.Latitude + "\", \"" +
				//	req.Longitude + "\")";
                cmd.ExecuteNonQuery();
            }
            else // the initial scan, create the manifest
            {
				// first identify which rfid tag is the container

                if (req.Rfid.GetBytes()[11] == 0x46)
                {
                //    cmd.CommandText = "INSERT INTO manifestScanHistory (rfid, latitude, longitude) VALUES (\"" +
				//		req.Rfid.ToString() + "\", \"" +
				//		req.Latitude + "\", \"" + req.Longitude + "\")";

					Console.WriteLine(cmd.CommandText);

                    cmd.ExecuteNonQuery();
                }
				
                //cmd.CommandText = "UPDATE manifestActual SET itemStatus=" + Status.ToString() + " WHERE itemRfid=\"" + req.Rfid.ToString() + "\"";
				cmd.CommandText = "INSERT INTO manifestActual SET itemStatus=" + Status.ToString() + " WHERE itemRfid=\"" + req.Rfid.ToString() + "\"";
                
                cmd.ExecuteNonQuery();
            }

            // send back the description string 
			cmd.CommandText = "SELECT shortDesc, longDesc FROM descriptions WHERE rfid='" + req.Rfid.ToString() + "'";
			OracleDataReader reader = cmd.ExecuteReader();
			Console.Out.WriteLine(cmd.CommandText);
			// if there was no description, then send back this information

			string sdesc = null;
			string ldesc = null;

			while (reader.Read())
			{
				sdesc = reader.GetString(0);
				ldesc = reader.GetString(1);
			}

            reader.Close();
            Close();

			if (sdesc == null)
			{
				sdesc = "Null entry";
				ldesc = "No entry for this RFID";
			}

			return new QueryResponse(sdesc, ldesc, "doesn't matter",false,-1,"0");
		}

		private static void Close()
		{
			c.Close();
			c.Dispose();
		}
	}

#if USING_NUNIT
	[TestFixture]
	public class DBBackendTest
	{
		private string DBSERVER = "128.8.46.235";
		private string DATABASE = "rfid";

		[Test]
		public void DBTest()
		{
			string MyConString = "DRIVER={MySQL ODBC 3.51 Driver};" +
				"SERVER=" + DBSERVER + ";" +
				"DATABASE=" + DATABASE + ";" +
				"UID=root;" +
				"PASSWORD=bello;" +
				"OPTION=3";

            Console.Out.WriteLine(MyConString);


			OdbcConnection connection = new OdbcConnection(MyConString);

            try
            {

                connection.Open();

                OdbcCommand cmd = new OdbcCommand("SELECT * FROM scanHistory", connection);
                OdbcDataReader reader = cmd.ExecuteReader();

                reader.Close();
            }
            finally
            {
                connection.Close();
            }
		}
	}
#endif
}
