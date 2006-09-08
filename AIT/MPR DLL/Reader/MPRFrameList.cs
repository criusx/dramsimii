//==========================================================================================
//
//	WJ.MPR.Reader.MPRFrameList
//	Copyright (c) 2005, WJ Communications, Inc.
//
//==========================================================================================
using System;

namespace WJ.MPR.Reader
{
	/// <summary>
	/// A strongly-typed collection of MPRFrame objects.
	/// </summary>
	public class MPRFrameList : System.Collections.CollectionBase
	{
		/// <summary>
		/// Gets or sets the element at the specified index.
		/// This property is the indexer for the MPRFrameList class.
		/// </summary>
		/// <param name="Index">The zero-based index of the MPRFrame to get or set.</param>
		/// <value>The MPRFrame at the specified index.</value>
		/// <exception cref="ArgumentOutOfRangeException">index is not a valid index in the MPRFrameList.</exception>
		public MPRFrame this[int Index] { get {return (MPRFrame) List[Index];} set {List[Index] = value;} }

		/// <summary>
		/// Add an MPRFrame to the list.
		/// </summary>
		/// <param name="aFrame">The frame to add.</param>
		/// <returns>Index of newly added frame.</returns>
		public int Add(MPRFrame aFrame) { return List.Add(aFrame); }

		/// <summary>
		/// Determines the index of a specific MPRFrame in the MPRFrameList.
		/// </summary>
		/// <param name="aFrame">The MPRFrame to locate in the MPRFrameList.</param>
		/// <returns>The index of aFrame if found in the MPRFrameList; otherwise, -1.</returns>
		public int IndexOf(MPRFrame aFrame) { return List.IndexOf(aFrame); }

		/// <summary>
		/// Inserts an MPRFrame to the MPRFrameList at the specified Index.
		/// </summary>
		/// <param name="Index">The zero-based index at which the value should be inserted. </param>
		/// <param name="aFrame">The MPRFrame to insert into the MPRFrameList.</param>
		public void Insert(int Index, MPRFrame aFrame) { List.Insert(Index, aFrame); }

		/// <summary>
		/// Removes the first occurrence of a specific MPRFrame from the MPRFrameList
		/// </summary>
		/// <param name="aFrame">The MPRFrame to remove from the MPRFrameList.</param>
		public void Remove(MPRFrame aFrame) { List.Remove(aFrame); }
		
		/// <summary>
		/// Determines whether the MPRFrameList contains a specific value.
		/// </summary>
		/// <param name="aFrame">The MPRFrame to locate in the MPRFrameList.</param>
		/// <returns>true if the MPRFrame is found in the MPRFrameList; otherwise, false.</returns>
		public bool Contains(MPRFrame aFrame) { return List.Contains(aFrame); }
	}
}