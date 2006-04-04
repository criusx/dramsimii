//==========================================================================================
//
//	WJ.MPR.Util.MPREventDelegates
//	Copyright (c) 2005, WJ Communications, Inc.
//
//	Useful Event Handler type delegates.
//
//==========================================================================================
using System;

namespace WJ.MPR.Util
{
	/// <summary>
	/// Delegate for events with a bool argument.
	/// </summary>
	public delegate void BoolEventHandler(bool Flag);

	/// <summary>
	/// Delegate for events with a string argument.
	/// </summary>
	public delegate void StringEventHandler(string Message);
}
