//==========================================================================================
//
//		WJ.MPR.WinAPI.Sound
//		Copyright (c) 2005, WJ Communications, Inc.
//
//		Class to play a WAV sound, as a file or a resource, in a .NET way.
//
//==========================================================================================
using System;
using System.Runtime.InteropServices;
using System.Resources;
using System.IO;

namespace WJ.MPR.WinAPI
{
	#region Sound
	/// <summary>
	/// Play a WAV sound.  WAV can be either a file or resource.
	/// </summary>
	public class Sound 
	{
		[Flags]
		private enum PlaySoundFlags : int 
		{
			SND_SYNC = 0x0000,  /* play synchronously (default) */
			SND_ASYNC = 0x0001,  /* play asynchronously */
			SND_NODEFAULT = 0x0002,  /* silence (!default) if sound not found */
			SND_MEMORY = 0x0004,  /* pszSound points to a memory file */
			SND_LOOP = 0x0008,  /* loop the sound until next sndPlaySound */
			SND_NOSTOP = 0x0010,  /* don't stop any currently playing sound */
			SND_NOWAIT = 0x00002000, /* don't wait if the driver is busy */
			SND_ALIAS = 0x00010000, /* name is a registry alias */
			SND_ALIAS_ID = 0x00110000, /* alias is a predefined ID */
			SND_FILENAME = 0x00020000, /* name is file name */
			SND_RESOURCE = 0x00040004  /* name is resource name or atom */
		}


		/// <summary>
		/// Play a WAV File asynchronously.
		/// </summary>
		/// <param name="strFileName">The name of the .WAV file to play</param>
		public static void PlayFile( string strFileName )
		{
			PlaySound( strFileName, IntPtr.Zero, PlaySoundFlags.SND_FILENAME | PlaySoundFlags.SND_ASYNC );
		}

		/// <summary>
		/// Play a WAV Resource asynchronously.
		/// </summary>
		/// <param name="strFileName">The name of the .WAV resource to play</param>
		public static void PlayResource( string strFileName )
		{
			PlaySound( strFileName, IntPtr.Zero, PlaySoundFlags.SND_RESOURCE | PlaySoundFlags.SND_ASYNC );
		}

		/// <summary>
		/// Play a WAV File asynchronously.
		/// </summary>
		/// <param name="strFileName">The name of the .WAV file to play</param>
		public static void Play( string strFileName )
		{
			PlaySound( strFileName, IntPtr.Zero, PlaySoundFlags.SND_FILENAME | PlaySoundFlags.SND_ASYNC );
		}

		#region Static Constructor and Framework Selection
		// These functions wrap the P/Invoked API calls and make the correct call based on whether 
		// we're running under the full or compact framework
		static internal bool bFullFramework;

		static Sound()
		{
			bFullFramework = (System.Environment.OSVersion.Platform != PlatformID.WinCE);
		}

		static internal bool FullFramework { get { return bFullFramework; }	}

		private static bool PlaySound(string szSound, IntPtr hMod, PlaySoundFlags flags)
		{
			return (FullFramework)?
				PlaySoundWin(szSound, hMod, flags)
				: PlaySoundCE(szSound, hMod, flags);
		}
		#endregion
	
		#region P/Invokes
		[DllImport("coredll", EntryPoint = "PlaySound")]
		private static extern bool PlaySoundCE( string szSound, IntPtr hMod, PlaySoundFlags flags );

		[DllImport("winmm", EntryPoint = "PlaySound")]
		private static extern bool PlaySoundWin( string szSound, IntPtr hMod, PlaySoundFlags flags );
		#endregion
	}
	#endregion
}