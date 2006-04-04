using System;
using System.Data.Odbc;

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
		private const string DBSERVER = "tbk.ece.umd.edu";
		private const string DATABASE = "rfid";

		private static OdbcConnection c;

		private static void Open()
		{
			string MyConString = "DRIVER={MySQL ODBC 3.51 Driver};" +
				"SERVER=" + DBSERVER + ";" +
				"DATABASE=" + DATABASE + ";" +
				"UID=root;" +
				"PASSWORD=bello;" +
				"OPTION=3";

			c = new OdbcConnection(MyConString);
			c.Open();
		}
        /*
		public static void ScanRFID(ScanRequest r)
		{
			string str = "INSERT INTO scanHistory " +
				"(RFID, scannerID, timestamp, latitude, longitude) " +
				"VALUES (" +
				"'" + r.Rfid.ToString() + "', " +
				"'" + r.ScannerID + "', " +
				"'" + r.Timestamp.ToString("yyy-MM-dd HH:mm:ss") + "', " +
				"'" + r.Latitude + "', " + 
				"'" + r.Longitude + "')";
			Console.Out.WriteLine(str);
			
			Open();
			OdbcCommand cmd = new OdbcCommand(str, c);
			if (cmd.ExecuteNonQuery() != 1)
				throw new Exception("Something is wrong");
			Close();
		}*/

		public static QueryResponse QueryRFID(QueryRequest req)
		{
			string str = "SELECT shortDescription, longDescription, iconName FROM descriptions WHERE RFID='" + req.Rfid.ToString() + "'";
			Console.Out.WriteLine(str);

			Open();
			OdbcCommand cmd = new OdbcCommand(str, c);
			OdbcDataReader reader = cmd.ExecuteReader();

			string sdesc = null;
			string ldesc = null;
			string icon = null;

			while (reader.Read())
			{
				sdesc = reader.GetString(0);
				ldesc = reader.GetString(1);
				icon = reader.GetString(2);
			}

			if (sdesc == null)
				throw new Exception("RFID invalid.");

			return new QueryResponse(sdesc, ldesc, icon);
		}

		private static void Close()
		{
			c.Close();
		}
	}

#if USING_NUNIT
	[TestFixture]
	public class DBBackendTest
	{
		private string DBSERVER = "tbk.ece.umd.edu";
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
