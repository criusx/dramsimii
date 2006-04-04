using System;

namespace WJ.Util
{
	/// <summary>
	/// For events that pass strings
	/// </summary>
	public delegate void StringEventHandler(object sender, StringEventArgs se);

	/// <summary>
	/// Arguments to Status Events (ie Events that require a string as an argument)
	/// </summary>
	public class StringEventArgs : EventArgs
	{
		/// <summary>
		/// Construct a StatusEventArgs with the given string as its argument.
		/// </summary>
		/// <param name="status">The string argument.</param>
		public StringEventArgs (string status)
		{
			this.message = status;
		}

		private string message;

		/// <summary>
		/// The Tag argument of the event.
		/// </summary>
		public string Message { get { return message; } }
	}
}
