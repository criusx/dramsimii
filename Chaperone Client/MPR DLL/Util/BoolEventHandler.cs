using System;

namespace WJ.Util
{
	/// <summary>
	/// For events that pass strings
	/// </summary>
	public delegate void BoolEventHandler(object sender, BoolEventArgs be);

	/// <summary>
	/// Arguments to Bool Events (ie Events that pass a bool argument)
	/// </summary>
	public class BoolEventArgs : EventArgs
	{
		/// <summary>
		/// Construct a BoolEventArgs with the given bool as its argument.
		/// </summary>
		/// <param name="flag">The bool argument.</param>
		public BoolEventArgs (bool flag)
		{
			this.flag = flag;
		}

		private bool flag;

		/// <summary>
		/// The Tag argument of the event.
		/// </summary>
		public bool Flag { get { return flag; } }
	}
}
