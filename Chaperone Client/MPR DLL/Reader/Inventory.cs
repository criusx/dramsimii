//==========================================================================================
//
//		WJ.MPR.Reader.Inventory
//		Copyright (c) 2005, WJ Communications, Inc.
//
//==========================================================================================
using System;

namespace WJ.MPR.Reader
{
	/// <summary>
	/// A strongly-typed collection of RFIDTag objects.
	/// </summary>
	public class Inventory : System.Collections.CollectionBase
	{
		/// <summary>
		/// Gets or sets the element at the specified index.
		/// This property is the indexer for the list.
		/// </summary>
		/// <param name="Index">The zero-based index of the element to get or set.</param>
		/// <value>The element at the specified index.</value>
		/// <exception cref="ArgumentOutOfRangeException">index is not a valid index in the list.</exception>
		public RFIDTag this[int Index] { get {return (RFIDTag) List[Index];} set {List[Index] = value;} }

		/// <summary>
		/// Add a single value to the list.
		/// </summary>
		/// <param name="val">the value to add.</param>
		/// <returns>Index of newly added element.</returns>
		public int Add(RFIDTag val) { return List.Add(val); }

		/// <summary>
		/// Determines the index of a specific element in the list.
		/// </summary>
		/// <param name="val">The element to locate in the list.</param>
		/// <returns>The index of val if found in the list; otherwise, -1.</returns>
		public int IndexOf(RFIDTag val) { return List.IndexOf(val); }

		/// <summary>
		/// Inserts an element into the list at the specified Index.
		/// </summary>
		/// <param name="Index">The zero-based index at which the value should be inserted.</param>
		/// <param name="val">The value to insert into the list.</param>
		public void Insert(int Index, RFIDTag val) { List.Insert(Index, val); }


		/// <summary>
		/// Removes the first occurrence of a specific value from the list.
		/// </summary>
		/// <param name="val">The value to remove from the list.</param>
		public void Remove(RFIDTag val) { List.Remove(val); }

		/// <summary>
		/// Convert the list to an array.
		/// </summary>
		/// <returns>The values of this list, as an array.</returns>
		public RFIDTag[] ToArray() { RFIDTag[] A = new RFIDTag[Count]; List.CopyTo(A,0); return A; }

		/// <summary>
		/// Determines whether the Inventory contains a specific RFIDTag.
		/// </summary>
		/// <param name="val">The value to locate in the list.</param>
		/// <returns>true if val is found in the list; otherwise, false.</returns>
		public bool Contains(RFIDTag val) { return List.Contains(val); }

		/// <summary>
		/// Determines whether the Inventory contains a RFIDTag with a specific TagID.
		/// The TagID of each RFIDTag in the Inventory is compared to val.
		/// </summary>
		/// <param name="val">The value to locate in the list.</param>
		/// <returns>true if val is found in the list; otherwise, false.</returns>
		public bool Contains(string val)
		{
			foreach (RFIDTag t in List) if (t.TagID == val) return true; 
			return false;
		}

		/// <summary>
		/// Determines whether the Inventory contains a RFIDTag with a specific TagID.
		/// The TagID of each Tag in the Inventory is compared to val.
		/// </summary>
		/// <param name="val">The value to locate in the list.</param>
		/// <returns>The matching RFIDTag if val is found in the list; otherwise, null.</returns>
		public RFIDTag Find(string val)
		{
			foreach (RFIDTag t in List) if (t.TagID == val) return t;
			return null;
		}

		/// <summary>
		/// Combine two Inventories.
		/// If newTags is null, false is returned.
		/// Then newTags are added.  Lastly Old RFIDTags are Expired.
		/// If either adding or expiring changes this Inventory, true is returned.
		/// </summary>
		/// <param name="newTags">An Inventory containing RFIDTags to add to this Inventory.</param>
		/// <returns>Whether any new RFIDTags were added or deleted.</returns>
		public bool Merge(Inventory newTags)
		{
			DateTime MergeTime = DateTime.Now;
			if (newTags == null) return false;	// Remove?
			
			return Add(newTags) | ExpireTags(MergeTime);
		}

		/// <summary>
		/// Add an Inventory's RFIDTags to this Inventory.
		/// This Inventory is searched for each RFIDTag in newTags,
		///	if it is found, its LastRead timestamp is updated.
		///	if it is not found, it is added.
		/// </summary>
		/// <param name="newTags">An Inventory of RFIDTags to add.</param>
		/// <returns>Whether any RFIDTags were added.</returns>
		public bool Add(Inventory newTags)
		{
			if (newTags == null) return false;

			bool changed = false;
			foreach (RFIDTag tmpTag in newTags)
			{
				RFIDTag invTag = Find(tmpTag.ToString());

				if (invTag == null) 
				{
					Add(tmpTag);
					changed = true;
				}
				else
					invTag.ReRead(tmpTag.LastRead);
			}
			return changed;
		}

		/// <summary>
		/// The length of time that a RFIDTag can live in an inventory, 
		/// without being reread, before it is removed by ExpireTags().
		/// If persistTime == TimeSpan.MaxValue, then RFIDTags are never removed.
		/// </summary>
		public System.TimeSpan persistTime = new TimeSpan(0,0,0,1,0);

		/// <summary>
		/// The LastRead property of each RFIDTag in this Inventory 
		/// + the persistTime is compared against the "now" parameter.  
		/// If the sum is less, then the RFIDTag is removed from this Inventory.
		/// If persistTime == TimeSpan.MaxValue, then RFIDTags are never removed.
		/// </summary>
		/// <param name="now"></param>
		/// <returns></returns>
		public bool ExpireTags(DateTime now)
		{
			bool changed = false;
			for (int index = this.Count - 1; index >= 0; index--)
				if ((persistTime != TimeSpan.MaxValue) && (now > this[index].LastRead + persistTime))
				{
					RemoveAt(index);
					changed = true;
				}
			return changed;
		}

		/// <summary>
		/// Returns the number of Class0 and Class1 RFIDTags in this Inventory.
		/// </summary>
		/// <value>an array of two integers: [0] = # Class 0, [1] = # Class 1.</value>
		public int[] CountClass 
		{
			get 
			{
				int[] clscnt = new int[2] {0,0};
				foreach (RFIDTag t in List)
				{
					if (t.Protocol == RFIDProtocol.EPCClass0)
						clscnt[0]++;
					else if (t.Protocol == RFIDProtocol.EPCClass1)
						clscnt[1]++;
				}
				return clscnt;
			}
		}
	}
}