//==========================================================================================
//
//	WJ.MPR.Util.ControlInvoker
//	Copyright (c) 2005, WJ Communications, Inc.
//
//==========================================================================================
using System;
using System.Collections;
using System.Windows.Forms;

namespace WJ.MPR.Util
{
	/// <summary>
	/// A Delegate for a Method that takes an array of objects.  
	/// Used by ControlInvoker to Invoke methods that take parameters
	/// in the Compact Framework.
	/// </summary>
	public delegate void MethodCallInvoker (object[] o);

	/// <summary>
	/// A Delegate for a Method that takes a void parameter list.  
	/// Used by ControlInvoker to Invoke methods that don't take parameters
	/// in the Compact Framework.
	/// </summary>
	public delegate void MethodInvoker ();

	/// <summary>
	/// Control.Invoke allows a method to be invoked on the same thread as the one
	/// the control was created on.  Unlike in the full .NET Framework, the .NET
	/// Compact Framework does not support the Control.Invoke overload for passing an 
	/// array of objects to pass as arguments.  This ControlInvoker class overcomes
	/// this limitation.
	/// </summary>
	public class ControlInvoker
	{
		private abstract class MethodCaller
		{
			public bool HasArguments;
		}

		private class MethodCall : MethodCaller
		{
			public MethodCallInvoker invoker;
			public object[] arguments;

			public MethodCall (MethodCallInvoker invoker, object[] arguments) 
			{
				this.invoker = invoker;
				this.arguments = arguments;
				this.HasArguments = true;
			}
		}

		private class MethodCallNoArgs : MethodCaller
		{
			public MethodInvoker invoker;
			public MethodCallNoArgs (MethodInvoker invoker) 
			{
				this.invoker = invoker;
				this.HasArguments = false;
			}
		}

		private Control control;
		private Queue argumentInvokeList = new Queue ();

		/// <summary>
		/// The constructor typically takes a form
		/// </summary>
		/// <param name="control"></param>
		public ControlInvoker (Control control) 
		{
			this.control = control;
		}

		/// <summary>
		/// The delegate wrapping the method and its arguments 
		/// to be called on the same thread as the control.
		/// </summary>
		/// <param name="invoker">A delegate for the method to call.</param>
		/// <param name="arguments">Arguments for the method.</param>
		public void Invoke (MethodCallInvoker invoker, params object[] arguments) 
		{
			this.argumentInvokeList.Enqueue (new MethodCall (invoker, arguments));
			control.Invoke (new EventHandler (ControlInvoke));
			Application.DoEvents();		
			// MSDN notes you should always call DoEvents.  
			// From MSDN ( http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnnetcomp/html/netcfmultithreadedapp.asp )
			// An important point to note is that you must call Application.DoEvents() 
			// in your code if you are updating the UI in the worker thread. 
			// Calling Application.DoEvents() will make sure that any events raised by 
			// the worker thread are processed by the UI thread.
		}

		/// <summary>
		/// The delegate wrapping a method that takes no arguments
		/// to be called on the same thread as the control.
		/// </summary>
		/// <param name="invoker">A delegate for the method to call.</param>
		public void Invoke (MethodInvoker invoker)
		{
			lock (this.argumentInvokeList)
				this.argumentInvokeList.Enqueue(new MethodCallNoArgs (invoker));
			control.Invoke (new EventHandler (ControlInvoke));
			Application.DoEvents();
		}

		/// <summary>
		/// CF Invoke only works with EventHandler delegates, so this is the method
		/// that actually will be Invoked on the UI Thread.
		/// </summary>
		/// <param name="sender">These parameters aren't used.</param>
		/// <param name="e">These parameters aren't used.</param>
		private void ControlInvoke (object sender, EventArgs e) 
		{
			MethodCaller methodCaller;
			lock (this.argumentInvokeList)
				 methodCaller = (MethodCaller) argumentInvokeList.Dequeue();
			if (methodCaller.HasArguments) 
			{
                MethodCall methodCall = methodCaller as MethodCall;
				methodCall.invoker(methodCall.arguments);
			} 
			else 
			{
				MethodCallNoArgs methodCall = methodCaller as MethodCallNoArgs;
				methodCall.invoker();
			}
		}
	}
}