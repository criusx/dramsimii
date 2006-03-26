using System;
using System.Collections;

namespace RFIDProtocolLib
{
    public struct QueryRequest
    {
        public const ushort Type = 2;

        public RFID Rfid;
        public string Latitude;
        public string Longitude;
        public byte Location;

        public QueryRequest(RFID rfid, string lat, string longitude, byte location)
        {
            Rfid = rfid;
            Latitude = lat;
            Longitude = longitude;
            Location = location;
        }

        public QueryRequest(byte[] buf)
        {
            Rfid = null;
            Latitude = null;
            Longitude = null;
            Location = 0;

            TLVList l = new TLVList(buf);
            IEnumerator e = l.GetEnumerator();
            while (e.MoveNext())
            {
                TLV tlv = (TLV)e.Current;
                switch (tlv.Type)
                {
                    case 0: Rfid = new RFID(tlv.Value); break;
                    case 1: Latitude = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
                    case 2: Longitude = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
                    case 3: Location = tlv.Value[0]; break;
                }
            }
        }

        public TLVList ToTLVList()
        {
            TLVList l = new TLVList(Type);
            l.Add(new TLV(0, Rfid.GetBytes()));
            l.Add(new TLV(1, System.Text.Encoding.ASCII.GetBytes(Latitude)));
            l.Add(new TLV(2, System.Text.Encoding.ASCII.GetBytes(Longitude)));
            l.Add(new TLV(3, new byte[] { Location }));

            return l;
        }
    }

    /// <summary>
    /// A Query Response.
    /// </summary>
    public struct QueryResponse
    {
        public const ushort Type = 3;

        /// <summary>
        /// The ShortDesc.
        /// </summary>
        public string ShortDesc;
        /// <summary>
        /// The long description.
        /// </summary>
        public string LongDesc;
        /// <summary>
        /// A pointer to the icon.
        /// </summary>
        public string IconName;

        /// <summary>
        /// Server uses this to construct a query response.
        /// </summary>
        public QueryResponse(string shortDesc, string longDesc, string iconName)
        {
            ShortDesc = shortDesc;
            LongDesc = "doesnt' matter";
            IconName = "doesn't matter";
        }

        /// <summary>
        /// Client uses this to parse the incoming query response.
        /// </summary>
        /// <param name="buf"></param>
        public QueryResponse(byte[] buf)
        {
            ShortDesc = null;
            LongDesc = null;
            IconName = null;

            TLVList l = new TLVList(buf);
            IEnumerator e = l.GetEnumerator();
            while (e.MoveNext())
            {
                TLV tlv = (TLV)e.Current;
                switch (tlv.Type)
                {
                    case 0: ShortDesc = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
                    case 1: LongDesc = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
                    case 2: IconName = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
                }
            }
        }

        /// <summary>
        /// Server backend uses this to construct the packet to send.
        /// </summary>
        /// <returns>A TLV list describing the query request.</returns>
        public TLVList ToTLVList()
        {
            TLVList l = new TLVList(Type);
            l.Add(new TLV(0, System.Text.Encoding.ASCII.GetBytes(ShortDesc)));
            l.Add(new TLV(1, System.Text.Encoding.ASCII.GetBytes(LongDesc)));
            l.Add(new TLV(2, System.Text.Encoding.ASCII.GetBytes(IconName)));

            return l;
        }
    }

#if DONTUSETHIS
	/// <summary>
	/// A Scan Request.
	/// </summary>
	public struct ScanRequest
	{
        public static ushort Type = 2;

		/// <summary>
		/// The RFID.
		/// </summary>
		public RFID Rfid;
		/// <summary>
		/// The timestamp.  This is written by the server.
		/// </summary>
		public DateTime Timestamp;
		/// <summary>
		/// Set to false by the client, the server sets this true.
		/// </summary>
		public bool TimestampValid;
		/// <summary>
		/// A string specifying the scanner location and who scanned it.
		/// </summary>
		public string ScannerID;
		/// <summary>
		/// The latitude.
		/// </summary>
		public string Latitude;
		/// <summary>
		/// The longitude.
		/// </summary>
		public string Longitude;

		/// <summary>
		/// Client uses this to construct a scan request.
		/// </summary>
		/// <param name="rfid"></param>
		/// <param name="scannerID"></param>
		/// <param name="latitude"></param>
		/// <param name="longitude"></param>
		public ScanRequest(RFID rfid, string scannerID, string latitude, string longitude)
		{
			Rfid = rfid;
			Timestamp = DateTime.Now.ToUniversalTime();
			TimestampValid = true;
			ScannerID = scannerID;
			Latitude = latitude;
			Longitude = longitude;
		}

		/// <summary>
		/// Server uses this to parse the incoming scan request.
		/// </summary>
		/// <param name="buf"></param>
		public ScanRequest(byte[] buf)
		{
			Rfid = null;
			TimestampValid = false;
			Timestamp = DateTime.Now;
			ScannerID = null;
			Latitude = null;
			Longitude = null;

			TLVList l = new TLVList(buf);
			IEnumerator e = l.GetEnumerator();
			while (e.MoveNext())
			{
				TLV tlv = (TLV)e.Current;
				switch(tlv.Type)
				{
					case 0: Rfid = new RFID(tlv.Value); break;
					case 1: Timestamp = new DateTime(BitConverter.ToInt64(tlv.Value, 0)); TimestampValid = true; break;
					case 2: ScannerID = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 3: Latitude = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 4: Longitude = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
				}
			}
		}

		/// <summary>
		/// Client backend uses this to construct the packet to send.
		/// </summary>
		/// <returns>A TLV list describing the scan request.</returns>
		public TLVList ToTLVList()
		{
			TLVList l = new TLVList(Type);
			l.Add(new TLV(0, Rfid.GetBytes()));
			l.Add(new TLV(1, BitConverter.GetBytes(Timestamp.Ticks)));
			l.Add(new TLV(2, System.Text.Encoding.ASCII.GetBytes(ScannerID)));
			l.Add(new TLV(3, System.Text.Encoding.ASCII.GetBytes(Latitude)));
			l.Add(new TLV(4, System.Text.Encoding.ASCII.GetBytes(Longitude)));

			return l;
		}
	}

    public struct ScanResponse
    {
        public static ushort Type = 3;
    }

	/// <summary>
	/// A Query Request.
	/// </summary>
	public struct QueryRequest
	{
        public static ushort Type = 4;

		/// <summary>
		/// The RFID.
		/// </summary>
		public RFID Rfid;
		
		/// <summary>
		/// Client uses this to construct a query request.
		/// </summary>
		/// <param name="rfid"></param>
		public QueryRequest(RFID rfid)
		{
			Rfid = rfid;
		}

		/// <summary>
		/// Server uses this to parse the incoming query request.
		/// </summary>
		/// <param name="buf"></param>
		public QueryRequest(byte[] buf)
		{
			Rfid = null;

			TLVList l = new TLVList(buf);
			IEnumerator e = l.GetEnumerator();
			while (e.MoveNext())
			{
				TLV tlv = (TLV)e.Current;
				switch(tlv.Type)
				{
					case 0: Rfid = new RFID(tlv.Value); break;
				}
			}
		}

		/// <summary>
		/// Client backend uses this to construct the packet to send.
		/// </summary>
		/// <returns>A TLV list describing the query request.</returns>
		public TLVList ToTLVList()
		{
			TLVList l = new TLVList(Type);
			l.Add(new TLV(0, Rfid.GetBytes()));
			
			return l;
		}
	}

	/// <summary>
	/// A Query Response.
	/// </summary>
	public struct QueryResponse
	{
        public static ushort Type = 5;

		/// <summary>
		/// The ShortDesc.
		/// </summary>
		public string ShortDesc;
		/// <summary>
		/// The long description.
		/// </summary>
		public string LongDesc;
		/// <summary>
		/// A pointer to the icon.
		/// </summary>
		public string IconName;
		
		/// <summary>
		/// Server uses this to construct a query response.
		/// </summary>
		public QueryResponse(string shortDesc, string longDesc, string iconName)
		{
			ShortDesc = shortDesc;
			LongDesc = longDesc;
			IconName = iconName;
		}

		/// <summary>
		/// Client uses this to parse the incoming query response.
		/// </summary>
		/// <param name="buf"></param>
		public QueryResponse(byte[] buf)
		{
			ShortDesc = null;
			LongDesc = null;
			IconName = null;

			TLVList l = new TLVList(buf);
			IEnumerator e = l.GetEnumerator();
			while (e.MoveNext())
			{
				TLV tlv = (TLV)e.Current;
				switch(tlv.Type)
				{
					case 0: ShortDesc = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 1: LongDesc = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 2: IconName = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
				}
			}
		}

		/// <summary>
		/// Server backend uses this to construct the packet to send.
		/// </summary>
		/// <returns>A TLV list describing the query request.</returns>
		public TLVList ToTLVList()
		{
			TLVList l = new TLVList(Type);
			l.Add(new TLV(0, System.Text.Encoding.ASCII.GetBytes(ShortDesc)));
			l.Add(new TLV(1, System.Text.Encoding.ASCII.GetBytes(LongDesc)));
			l.Add(new TLV(2, System.Text.Encoding.ASCII.GetBytes(IconName)));
			
			return l;
		}
	}

    /// <summary>
    /// An Alert Request.
    /// </summary>
    public struct AlertRequest
    {
        public static ushort Type = 6;

        /// <summary>
        /// The RFID.
        /// </summary>
        public byte AlertType;

        /// <summary>
        /// Client uses this to construct an alert request.
        /// </summary>
        /// <param name="type"></param>
        public AlertRequest(byte type)
        {
            AlertType = type;
        }

        /// <summary>
        /// Server uses this to parse the incoming alert request.
        /// </summary>
        /// <param name="buf"></param>
        public AlertRequest(byte[] buf)
        {
            AlertType = 0;

            TLVList l = new TLVList(buf);
            IEnumerator e = l.GetEnumerator();
            while (e.MoveNext())
            {
                TLV tlv = (TLV)e.Current;
                switch (tlv.Type)
                {
                    case 0: AlertType = tlv.Value[0]; break;
                }
            }
        }

        /// <summary>
        /// Client backend uses this to construct the packet to send.
        /// </summary>
        /// <returns>A TLV list describing the query request.</returns>
        public TLVList ToTLVList()
        {
            TLVList l = new TLVList(Type);
            l.Add(new TLV(0, new byte[] { AlertType }));

            return l;
        }
    }

    /// <summary>
    /// An Alert Response.
    /// </summary>
    public struct AlertResponse
    {
        public static ushort Type = 7;

        /// <summary>
        /// Client uses this to parse the incoming alert response.
        /// </summary>
        /// <param name="buf"></param>
        public AlertResponse(byte[] buf)
        {
            TLVList l = new TLVList(buf);
        }

        /// <summary>
        /// Server backend uses this to construct the packet to send.
        /// </summary>
        /// <returns>A TLV list describing the alert request.</returns>
        public TLVList ToTLVList()
        {
            TLVList l = new TLVList(Type);

            return l;
        }
    }

    /// <summary>
    /// A manifest request.
    /// </summary>
    public struct CheckManifestRequest
    {
        public static ushort Type = 8;

        /// <summary>
        /// The RFID of the container.
        /// </summary>
        public RFID Rfid;
        /// <summary>
        /// The RFIDs of the sub objects.
        /// </summary>
        public ArrayList Manifest;
        /// <summary>
        /// The type of checkin.
        /// </summary>
        public byte CheckinType; // 0 = transit scan, 1 = checkin, 2 = checkout

        public CheckManifestRequest(RFID rfid, ArrayList manifest, byte checkinType)
        {
            Rfid = rfid;
            Manifest = manifest;
            CheckinType = checkinType;
        }

        public CheckManifestRequest(byte[] buf)
        {
            Rfid = null;
            Manifest = new ArrayList();
            CheckinType = 0;

			TLVList l = new TLVList(buf);
			IEnumerator e = l.GetEnumerator();
			while (e.MoveNext())
			{
				TLV tlv = (TLV)e.Current;
				switch(tlv.Type)
				{
					case 0: Rfid = new RFID(tlv.Value); break;
					case 1: Manifest.Add(new RFID(tlv.Value)); break;
                    case 2: CheckinType = tlv.Value[0]; break;
				}
			}
        }

        public TLVList ToTLVList()
        {
            TLVList l = new TLVList(Type);

            l.Add(new TLV(0, Rfid.GetBytes()));
            l.Add(new TLV(2, new byte[] { CheckinType }));

            IEnumerator e = Manifest.GetEnumerator();
            while (e.MoveNext())
            {
                RFID rfid = (RFID)e.Current;
                l.Add(new TLV(1, rfid.GetBytes()));
            }

            return l;
        }
    }

    public struct CheckManifestResponse
    {
        public static ushort Type = 9;

        /// <summary>
        /// Client uses this to parse the incoming check manifest response.
        /// </summary>
        /// <param name="buf"></param>
        public CheckManifestResponse(byte[] buf)
        {
            TLVList l = new TLVList(buf);
        }

        /// <summary>
        /// Server backend uses this to construct the packet to send.
        /// </summary>
        /// <returns>A TLV list describing the check manifest response.</returns>
        public TLVList ToTLVList()
        {
            TLVList l = new TLVList(Type);

            return l;
        }
    }

    public struct UpdateTransitHistoryRequest
	{
		/// <summary>
		/// The RFID to update.
		/// </summary>
		public RFID Rfid;
		/// <summary>
		/// The name of the waypoint we're at.
		/// </summary>
		public string WaypointName;
		/// <summary>
		/// Our current latitude.
		/// </summary>
		public string Latitude;
		/// <summary>
		/// Our current longitude.
		/// </summary>
		public string Longitude;

		/// <summary>
		/// Client uses this to create transit history update request.
		/// </summary>
		/// <param name="rfid"></param>
		/// <param name="waypointName"></param>
		/// <param name="latitude"></param>
		/// <param name="longitude"></param>
		public UpdateTransitHistoryRequest(RFID rfid, string waypointName, string latitude, string longitude)
		{
			Rfid = rfid;
			WaypointName = waypointName;
			Latitude = latitude;
			Longitude = longitude;
		}

		/// <summary>
		/// Server uses this to parse the transit history update request.
		/// </summary>
		/// <param name="buf"></param>
		public UpdateTransitHistoryRequest(byte[] buf)
		{
			Rfid = null;
			WaypointName = null;
			Latitude = null;
			Longitude = null;

			TLVList l = new TLVList(buf);
			IEnumerator e = l.GetEnumerator();
			while (e.MoveNext())
			{
				TLV tlv = (TLV)e.Current;
				switch(tlv.Type)
				{
					case 0: Rfid = new RFID(tlv.Value); break;
					case 1: WaypointName = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 2: Latitude = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 3: Longitude = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
				}
			}
		}

		public TLVList ToTLVList()
		{
			TLVList l = new TLVList(6);
			l.Add(new TLV(0, Rfid.GetBytes()));
			l.Add(new TLV(1,System.Text.Encoding.ASCII.GetBytes(WaypointName)));
			l.Add(new TLV(2,System.Text.Encoding.ASCII.GetBytes(Latitude)));
			l.Add(new TLV(3,System.Text.Encoding.ASCII.GetBytes(Longitude)));
			
			return l;
		}
	}
#endif
}
