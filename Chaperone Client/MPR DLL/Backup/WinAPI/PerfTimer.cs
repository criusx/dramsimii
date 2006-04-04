//==========================================================================================
//
//		WJ.MPR.WinAPI.HiPerfTimer
//		Copyright (c) 2005, WJ Communications, Inc.
//
//==========================================================================================
using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Threading;

namespace WJ.MPR.WinAPI
{
	/// <summary>
	/// A High Resolution Timer for measuring execution time.
	/// </summary>
	internal sealed class HiPerfTimer
	{
		// These functions wrap the P/Invoked API calls and:
		// - make the correct call based on whether we're running under the full or compact framework
		// - eliminate empty parameters and defaults
		//	
		static internal bool bFullFramework;

		static HiPerfTimer()
		{
			bFullFramework = (System.Environment.OSVersion.Platform != PlatformID.WinCE);
		}

		static internal bool FullFramework { get { return bFullFramework; }	}

		[DllImport("coredll", EntryPoint="QueryPerformanceFrequency")]
		private static extern bool QueryPerformanceFrequencyCE(out long lpFrequency);

		[DllImport("kernel32", EntryPoint="QueryPerformanceFrequency")]
		private static extern bool QueryPerformanceFrequencyWin(out long lpFrequency);
		
		[DllImport("coredll", EntryPoint="QueryPerformanceCounter")]
		private static extern bool QueryPerformanceCounterCE(out long lpPerformanceCount);

		[DllImport("kernel32", EntryPoint="QueryPerformanceCounter")]
		private static extern bool QueryPerformanceCounterWin(out long lpPerformanceCount);

		private static bool QueryPerformanceFrequency(out long lpFrequency)
		{
			return (FullFramework) ? QueryPerformanceFrequencyWin(out lpFrequency) : QueryPerformanceFrequencyCE(out lpFrequency);
		}
	
		private static bool QueryPerformanceCounter(out long lpFrequency)
		{
			return (FullFramework) ? QueryPerformanceCounterWin(out lpFrequency) : QueryPerformanceCounterCE(out lpFrequency);
		}	


		private long startTime, stopTime;
		private long freq;
		
		// Constructor
		public HiPerfTimer()
		{
			startTime = 0;
			stopTime  = 0;

			if (QueryPerformanceFrequency(out freq) == false)
			{
				// high-performance counter not supported 
				throw new Win32Exception(); 
			}
		}
		
		// Start the timer
		public void Start()
		{
			// put this thread to sleep so the waiting threads can do some work
			Thread.Sleep(0);  

			QueryPerformanceCounter(out startTime);
		}
		
		// Stop the timer
		public void Stop()
		{
			QueryPerformanceCounter(out stopTime);
		}
		
		// Returns the duration of the timer (in seconds)
		public double Duration
		{
			get
			{
				return (double)(stopTime - startTime) / (double) freq;
			}
		}
	}
}