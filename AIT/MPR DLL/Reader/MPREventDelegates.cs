//==========================================================================================
//
//	WJ.MPR.Reader.MPREventDelegates
//	Copyright (c) 2005, WJ Communications, Inc.
//
//	Useful Event Handler type delegates.
//
//==========================================================================================
using System;

namespace WJ.MPR.Reader
{
	/// <summary>
	/// Delegate for events with a MPRMsg argument.
	/// </summary>
	public delegate void MPRMsgEventHandler(MPRMsg Message);

	/// <summary>
	/// For events that communicate events on Tags
	/// </summary>
	public delegate void TagEventHandler(RFIDTag Tag);
}

 

 