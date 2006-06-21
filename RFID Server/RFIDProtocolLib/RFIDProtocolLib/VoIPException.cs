using System;

namespace VoIP
{
	/// <summary>
	/// A VoIP Exception
	/// </summary>
	public class VoIPException : Exception
	{
		String message;
		public VoIPException(String message)
		{
			this.message = message;
		}
	}
}
