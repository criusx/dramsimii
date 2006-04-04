using System;

namespace WJ.MPR.Reader
{
	/// <summary>
	/// For events that communicate events on Tags
	/// </summary>
	public delegate void TagEventHandler(object sender, TagEventArgs te);

	/// <summary>
	/// Arguments to Tag Events (ie Events that require a WJ.MPR.Reader.Tag object as an argument)
	/// </summary>
	public class TagEventArgs : EventArgs 
	{
		/// <summary>
		/// Construct a TagEventArg with the specified Tag as its argument.
		/// </summary>
		/// <param name="tag">The Tag to send to the TagEventHandler</param>
		public TagEventArgs(RFIDTag tag)
		{
			this.mTag = tag;
		}

		private RFIDTag mTag;

		/// <summary>
		/// The Tag argument of the event.
		/// </summary>
		public RFIDTag Tag { get { return mTag; } }
	}
}
