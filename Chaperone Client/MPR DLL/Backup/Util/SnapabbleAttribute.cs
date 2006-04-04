//==========================================================================================
//
//		WJ.MPR.Util.SnappableAttribute
//		Copyright (c) 2005, WJ Communications, Inc.
//
//==========================================================================================
using System;
using WJ.MPR.Reader;

namespace WJ.MPR.Util
{
	/// <summary>
	///	Use this Interface to write a "Snap-In" For the WJ MPR Demo GUIs.
	///
	///	Usually this Interface is implented on a Form, and the OnClick EventHandler
	///	Shows the Form, which is accessible from the "Snap-In" Menu on the Demo.	
	/// </summary>
	public interface ISnapIn
	{
		/// <summary>
		/// Initialization Routine, that gives the Snap-In a reference to a Reader object.
		/// </summary>
		/// <param name="Reader">The Reader on which the Snap-In should operate</param>
		void Init(MPRReader Reader);

		/// <summary>
		/// The name of the Snap-In as it should appear on the "Snap-Ins" menu of the Demo.
		/// </summary>
		string MenuName { get; }

		/// <summary>
		/// The Event Handler to fire when the user selects the SnapIn's menuItem.
		/// </summary>
		EventHandler OnOpen { get; }
	}

	/// <summary>
	/// This Attribute, when applied to a class, will enable it to be recognized
	/// as an MPR SnapIn.
	/// </summary>
	[AttributeUsage(AttributeTargets.Class)]
	public class SnappableAttribute : System.Attribute
	{ 
		/// <summary>
		/// Applying this Attribute to a class will make it snappable (for use as a Demo Snap-In).
		/// </summary>
		public SnappableAttribute(){} 
	}
}
