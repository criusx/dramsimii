using System;
using System.Collections;

namespace RFIDProtocolLibrary
{
	public enum Packets : ushort
	{
		QueryRequest = 1, QueryResponse = 2, ReconcileFinished = 3, Info = 4, addRemoveItem = 5, RaiseAlert = 6, Ack = 7, CloseConnection = 8
	}

	#region Packet
	public abstract class Packet
	{
		public ushort Type;

		public abstract TLVList ToTLVList();
	};
	#endregion

	#region QueryRequest

	public class QueryRequest : Packet
	{
		public RFID Rfid;

		public QueryRequest(RFID rfid)
		{
			Type = (ushort)Packets.QueryRequest;
			Rfid = rfid;
		}

		public QueryRequest(byte[] buf)
		{
			Type = (ushort)Packets.QueryRequest;

			Rfid = null;

			TLVList l = new TLVList(buf);

			IEnumerator e = l.GetEnumerator();

			while (e.MoveNext())
			{
				TLV tlv = (TLV)e.Current;

				switch (tlv.Type)
				{
					case 0: Rfid = new RFID(tlv.Value); break;
				}
			}
		}

		public override TLVList ToTLVList()
		{
			TLVList l = new TLVList(Type);

			l.Add(new TLV(0, Rfid.GetBytes()));

			return l;
		}
	}

	/// <summary>
	/// A Query Response.
	/// </summary>
	public class QueryResponse : Packet
	{
		/// <summary>
		/// The ShortDesc.
		/// </summary>
		public string ShortDesc;

		/// <summary>
		/// The manifest number, for future reference.
		/// </summary>
		public int manifestNum;

		/// <summary>
		/// A pointer to the icon.
		/// </summary>
		public string IconName;

		/// <summary>
		/// Added(1), missing(-1) or expected(0)
		/// </summary>
		public int addedRemoved;

		/// <summary>
		/// the actual RFID number
		/// </summary>
		public string rfidNum;

		/// <summary>
		/// Server uses this to construct a query response.
		/// </summary>
		public QueryResponse(string shortDesc, int man, string iconName, int type, string rf)
		{
			Type = (ushort)Packets.QueryResponse;
			ShortDesc = shortDesc;
			manifestNum = man;
			IconName = "doesn't matter";
			rfidNum = "0";
			addedRemoved = type;
			rfidNum = rf;
		}

		/// <summary>
		/// Client uses this to parse the incoming query response.
		/// </summary>
		/// <param name="buf"></param>
		public QueryResponse(byte[] buf)
		{
			Type = (ushort)Packets.QueryResponse;
			ShortDesc = null;
			manifestNum = 0;
			IconName = null;
			addedRemoved = -2;
			rfidNum = null;

			TLVList l = new TLVList(buf);
			IEnumerator e = l.GetEnumerator();
			while (e.MoveNext())
			{
				TLV tlv = (TLV)e.Current;
				switch (tlv.Type)
				{
					case 0: ShortDesc = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 1: manifestNum = int.Parse(System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length)); break;
					case 2: IconName = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 3: addedRemoved = int.Parse(System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length)); break;
					case 4: rfidNum = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
				}
			}
		}

		/// <summary>
		/// Server backend uses this to construct the packet to send.
		/// </summary>
		/// <returns>A TLV list describing the query request.</returns>
		public override TLVList ToTLVList()
		{
			TLVList l = new TLVList(Type);
			l.Add(new TLV(0, System.Text.Encoding.ASCII.GetBytes(ShortDesc)));
			l.Add(new TLV(1, System.Text.Encoding.ASCII.GetBytes(manifestNum.ToString())));
			l.Add(new TLV(2, System.Text.Encoding.ASCII.GetBytes(IconName)));
			l.Add(new TLV(3, System.Text.Encoding.ASCII.GetBytes(addedRemoved.ToString())));
			l.Add(new TLV(4, System.Text.Encoding.ASCII.GetBytes(rfidNum)));

			return l;
		}
	}
	#endregion

	#region ReconcileFinished
	public class ReconcileFinished : Packet
	{
		public string dateTime;

		public string containerRFID;

		public string numberToCall;

		public ReconcileFinished(string dT, string containerID, string num)
		{
			Type = (ushort)Packets.ReconcileFinished;
			containerRFID = containerID;
			dateTime = dT;
			numberToCall = num;
		}

		public ReconcileFinished(byte[] buf)
		{
			Type = (ushort)Packets.ReconcileFinished;
			dateTime = "";
			containerRFID = "";
			numberToCall = "";

			TLVList l = new TLVList(buf);

			IEnumerator e = l.GetEnumerator();

			while (e.MoveNext())
			{
				TLV tlv = (TLV)e.Current;
				switch (tlv.Type)
				{
					case 0: dateTime = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 1: containerRFID = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 2: numberToCall = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
				}
			}
		}

		public override TLVList ToTLVList()
		{
			TLVList l = new TLVList(Type);

			l.Add(new TLV(0, System.Text.Encoding.ASCII.GetBytes(dateTime)));
			l.Add(new TLV(1, System.Text.Encoding.ASCII.GetBytes(containerRFID)));
			l.Add(new TLV(2, System.Text.Encoding.ASCII.GetBytes(numberToCall)));

			return l;
		}
	}
	#endregion

	#region Info

	public class Info : Packet
	{
		public string Latitude;

		public string Longitude;

		public string dateTime;

		public int isScan;

		public Info(string La, string Lo, string dT, int iS)
		{
			Type = (ushort)Packets.Info;
			Latitude = La;
			Longitude = Lo;
			isScan = iS;
			dateTime = dT;
		}

		/// <summary>
		/// Client uses this to parse the incoming info packet.
		/// </summary>
		/// <param name="buf"></param>
		public Info(byte[] buf)
		{
			Type = (ushort)Packets.Info;
			Latitude = "";
			Longitude = "";
			dateTime = "";
			isScan = -1;

			TLVList l = new TLVList(buf);

			IEnumerator e = l.GetEnumerator();

			while (e.MoveNext())
			{
				TLV tlv = (TLV)e.Current;
				switch (tlv.Type)
				{
					case 0: Latitude = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 1: Longitude = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 2: isScan = int.Parse(System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length)); break;
					case 3: dateTime = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
				}
			}
		}

		public override TLVList ToTLVList()
		{
			TLVList l = new TLVList(Type);

			l.Add(new TLV(0, System.Text.Encoding.ASCII.GetBytes(Latitude)));
			l.Add(new TLV(1, System.Text.Encoding.ASCII.GetBytes(Longitude)));
			l.Add(new TLV(2, System.Text.Encoding.ASCII.GetBytes(isScan.ToString())));
			l.Add(new TLV(3, System.Text.Encoding.ASCII.GetBytes(dateTime)));

			return l;
		}
	}


	#endregion

	#region addRemoveItem
	public class addRemoveItem : Packet
	{
		public string dateTime;

		public string hostID;

		public string containerID;

		public bool remove;

		public int manifestNum;

		public addRemoveItem(string dT, string hID, string cID, bool rem, int manifest_num)
		{
			Type = (ushort)Packets.addRemoveItem;
			dateTime = dT;
			hostID = hID;
			containerID = cID;
			manifestNum = manifest_num;
			remove = rem;
		}

		public addRemoveItem(byte[] buf)
		{
			Type = (ushort)Packets.addRemoveItem;
			dateTime = "";
			hostID = "";
			containerID = "";
			remove = false;
			manifestNum = 0;

			TLVList l = new TLVList(buf);

			IEnumerator e = l.GetEnumerator();

			while (e.MoveNext())
			{
				TLV tlv = (TLV)e.Current;

				switch (tlv.Type)
				{
					case 0: dateTime = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 1: hostID = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 2: remove = bool.Parse(System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length)); break;
					case 3: containerID = System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length); break;
					case 4: manifestNum = int.Parse(System.Text.Encoding.ASCII.GetString(tlv.Value, 0, tlv.Length)); break;
				}
			}
		}

		public override TLVList ToTLVList()
		{
			TLVList l = new TLVList(Type);

			l.Add(new TLV(0, System.Text.Encoding.ASCII.GetBytes(dateTime)));
			l.Add(new TLV(1, System.Text.Encoding.ASCII.GetBytes(hostID)));
			l.Add(new TLV(2, System.Text.Encoding.ASCII.GetBytes(remove.ToString())));
			l.Add(new TLV(3, System.Text.Encoding.ASCII.GetBytes(containerID)));
			l.Add(new TLV(4, System.Text.Encoding.ASCII.GetBytes(manifestNum.ToString())));

			return l;
		}
	};

	#endregion

	#region RaiseAlert
	public class RaiseAlert : Packet
	{
		public RaiseAlert()
		{
			Type = (ushort)Packets.RaiseAlert;
		}

		public RaiseAlert(byte[] buf)
		{
			Type = (ushort)Packets.RaiseAlert;
		}

		public override TLVList ToTLVList()
		{
			return new TLVList(Type);
		}
	}
	#endregion

	#region ack
	public class ack : Packet
	{
		public ack()
		{
			Type = (ushort)Packets.Ack;
		}

		public ack(byte[] buf)
        {
			Type = (ushort)Packets.Ack;
        }

		public override TLVList ToTLVList()
		{
			TLVList l = new TLVList(Type);
			return l;
		}
	};
	#endregion

    #region ack
    public class CloseConnection : Packet
    {
        public CloseConnection()
        {
            Type = (ushort)Packets.CloseConnection;
        }

        public CloseConnection(byte[] buf)
        {
            Type = (ushort)Packets.CloseConnection;
        }

        public override TLVList ToTLVList()
        {
            TLVList l = new TLVList(Type);
            return l;
        }
    };
    #endregion
}
