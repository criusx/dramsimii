//==========================================================================================
//
//	WJ.MPR.Reader.MPRComm
//	Copyright (c) 2005, WJ Communications, Inc.
//
//==========================================================================================
using System;
using System.Text;
using System.Threading;
using WJ.Serial;
using WJ.MPR.Util;

namespace WJ.MPR.Reader
{
	/// <summary>
	///	Responsible for handling messages bound for the MPR over a serial port.
	///		- Manages an underlying Serial Port object
	///		- Handles Serial Port Events
	///		- Generates Request Frames and parses Response Frames, and stores all frames for
	///			a single transaction in an MPRMsg.
	///		- Handles and reports Serial Port or MPR API errors.
	///		- Fires two events,
	///			1) BLData, when data arrives (always asynchronously) from the bootloader.
	///			2) IsOpenChanged, when the underlying serial port closes.
	///		- Performs the twiddling needed for Reseting the MPR and entering the bootloader.
	/// </summary>
	public class MPRComm : IDisposable
	{
		#region delegates and events
		/// <summary>
		/// Raised when the underlying serial port is opened or closed.
		/// </summary>
		public event BoolEventHandler IsOpenChanged;

		/// <summary>
		/// Fired when data is received from the MPR's bootloader.
		/// </summary>
		public event StringEventHandler BLDataArrived;
		#endregion

		#region Local Serial Port Properties
		
		/// <summary>
		/// create the port settings:
		///		default has no Handshake
		///		Bootloader uses XonXoff
		/// </summary>
		private DetailedPortSettings portSettings = new HandshakeNone();

		/// <summary>
		/// The underlying Serial Port which we'll use to talk to the MPR
		/// </summary>
		private Port SerialPort;

		/// <summary>
		/// The Baud Rate of the underlying Serial Port (default = 57600)
		/// </summary>
		public BaudRates BaudRate = BaudRates.CBR_57600;
		#endregion

		#region Message Timeout timer

		private System.Threading.Timer TimeOutTimer;

		/// <summary>
		/// Reset the TimeOutTimer, by changing it to expire, just once, in 750 milliseconds.
		/// </summary>
		public void KickTimer()
		{
			// .75 second comm timer, that never repeats
			TimeOutTimer.Change(750, Timeout.Infinite);
		}

		/// <summary>
		/// Kill the TimeOutTimer, by changing it to never expire.
		/// </summary>
		public void KillTimer()
		{
			TimeOutTimer.Change(Timeout.Infinite, Timeout.Infinite);
		}

		/// <summary>
		/// The timer callback function.  Notify the current Msg that a comm timeout has occurred.
		/// </summary>
		/// <param name="ignore">Not used. Use null.</param>
		private void TimeOutFunction(object ignore)
		{
			if (CurrentMessage != null)
				CurrentMessage.Timeout();
			KillTimer();
		}
		#endregion

		#region Private Properties
		/// <summary>
		/// The currently active Message.
		/// </summary>
		private MPRMsg CurrentMessage;

		/// <summary>
		/// A temporary frame within which to store arriving bytes.
		/// </summary>
		private MPRFrame rxTmpFrame = new MPRFrame();
		#endregion

		/// <summary>
		/// Constructor that initializes, but doesn't open, the underlying serial port
		/// through which to communicate with an MPR.  
		/// </summary>
		/// <param name="SerialPortName">The "COMx" port to use.</param>
		/// <param name="BaudRate">The BaudRate to talk at, always 57600 for MPR series readers.</param>
		public MPRComm(string SerialPortName, string BaudRate)
		{
			TimeOutTimer = new System.Threading.Timer(new TimerCallback(TimeOutFunction), null, Timeout.Infinite, Timeout.Infinite);

			this.SerialPort = new Port(SerialPortName);

			SerialPort.Settings.BaudRate = this.BaudRate;
			SerialPort.SThreshold = 1;
			SerialPort.RThreshold = 1;

			// define event handlers
			SerialPort.DataReceived += new Port.CommEvent(SerialPort_DataReceived);
			SerialPort.TxDone += new Port.CommEvent(SerialPort_TxDone);
			SerialPort.OnError += new Port.CommErrorEvent(SerialPort_OnError);
			SerialPort.IsOpenChanged += new Port.CommChangeEvent(SerialPort_IsOpenChanged);
		}

		/// <summary>
		/// Try to open the underlying Serial Port.
		/// </summary>
		/// <returns>Whether or not the port was successfully opened.</returns>
		public bool Open()
		{
			try 
			{
				SerialPort.Open();
			}
			catch (CommPortException)
			{
				// If Port cannot be opened for some reason, 
				// a CommPortException will be thrown, ignore it,
				// try to close the port and return false
				SerialPort.Close();
				return false;
			}
			return SerialPort.IsOpen;
		}

		/// <summary>
		/// Just map the underlying ports IsOpen property.
		/// </summary>
		public bool IsOpen { get { return SerialPort.IsOpen; } }

		/// <summary>
		/// Close the underlying serial port immediately...
		/// </summary>
		public void Close() { SerialPort.Close(); }

		/// <summary>
		/// Build an MPRMsg using command and parameters, 
		/// send the request frame, and wait for and receive
		/// response packets.
		/// </summary>
		/// <param name="command">The command to send.</param>
		/// <param name="parameters">The parameters for the command to send.</param>
		/// <returns>The MPRMsg will contain the request and response frames for further processing.</returns>
		public MPRMsg Send(CmdCode command, byteList parameters)
		{
			lock(this) 
			{
				try 
				{
					// First Convert the msg parameter to an MPRMessage (if possible)
					CurrentMessage = new MPRMsg(command, parameters);

					if (!SendFrame(CurrentMessage.TxFrame))
						CurrentMessage.CommError("Send Frame Failed.");

					CurrentMessage.Receive();
					KillTimer();
				}
				catch (Exception ex) 
				{
					if (CurrentMessage != null)
						CurrentMessage.CommError(ex.Message);
				}
				return CurrentMessage;
			}
		}

		/// <summary>
		/// Send a request frame to the MPR via the Serial Port
		/// Kicks the receiver timer after sending a frame
		/// </summary>
		/// <param name="txFrame">a Frame to send</param>
		/// <returns>true if port is still open at exit of function.
		///	false if port==null, or port isn't open.</returns>
		private bool SendFrame(MPRFrame txFrame)
		{
			// If null or not open return false
			if ((SerialPort == null) || (!SerialPort.IsOpen))
				return false;

			SerialPort.Output = txFrame.ToArray();						// Send the bytes
			KickTimer();

			return SerialPort.IsOpen;
		}

		/// <summary>
		/// Send one line of text
		/// </summary>
		/// <param name="line"></param>
		/// <returns>Whether the serial port is still open after sending the line.</returns>
		public bool SendBLLine(string line)
		{
			ASCIIEncoding encoding = new ASCIIEncoding();

			SerialPort.Output = encoding.GetBytes(line);
			//Console.WriteLine("Sent: {0}", line);

			return SerialPort.IsOpen;
		}
	
		#region IDisposable Members

		/// <summary>
		/// Free resources:
		///		Dispose the underlying Serial port.
		/// </summary>
		public void Dispose()
		{
			SerialPort.Dispose();
		}

		#endregion

		#region SerialPort Event Handlers
		
		private void SerialPort_DataReceived()
		{
			byte[] c = SerialPort.Input;

			//ASCIIEncoding encoding = new ASCIIEncoding();
			//Console.Write("{0}",encoding.GetString(c, 0, c.Length));

			if (rxTmpFrame != null)
				foreach(byte b in c) 
				{
					rxTmpFrame.Add(b);
					if (rxTmpFrame.CompleteFrame) 
					{
						// Let Msg class determine whether to kick or kill Timer
						CurrentMessage.AddRxFrame(rxTmpFrame);
						rxTmpFrame = new MPRFrame();
					}
				}
		}

		private void SerialPortBL_DataReceived()
		{
			byte[] c = SerialPort.Input;

			// Remove '\0' characters from byte array
			for (int i=0; i < c.Length; i++)
				if (c[i] == 0)
					c[i] = (byte)' ';
			
			ASCIIEncoding encoding = new ASCIIEncoding();
			string s = encoding.GetString(c, 0, c.Length);
			//s.Replace("\0","\n");
			//Console.Write("{0}",encoding.GetString(c, 0, c.Length));
			if (BLDataArrived != null) 
				BLDataArrived(s);
		}


		/// <summary>
		/// Not used:  Fired when the Serial Port is done transmitting the whole Input buffer.
		/// </summary>
		private void SerialPort_TxDone()
		{
			// Message just sent, start Receive Message timer
//			KickTimer();
		}


		/// <summary>
		/// Fired when the Serial Port detects an error condition.
		/// </summary>
		/// <param name="Description">A string describing the error condition.</param>
		private void SerialPort_OnError(string Description)
		{
			KillTimer();

			if (CurrentMessage != null)
				CurrentMessage.CommError(Description);
			rxTmpFrame = new MPRFrame();
		}


		/// <summary>
		/// Just a thin wrapper around the underlying Port's
		/// IsOpenChanged event
		/// </summary>
		/// <param name="NewState">true if Serial Port is now open; otherwise false.</param>
		private void SerialPort_IsOpenChanged(bool NewState)
		{
			if (!NewState && (CurrentMessage != null))
				CurrentMessage.CommError("COM Port disconnected");

			if (!NewState)
				if (IsOpenChanged != null) 
					IsOpenChanged(NewState);
		}
		#endregion

		/// <summary>
		/// Returns a string with the serial port and baud rate : "COMx 57600".
		/// </summary>
		public string PortSetting
		{
			get { return SerialPort.PortName.Trim(new char[] {'\\','.'}) + " " + SerialPort.Settings.BaudRate.ToString().Remove(0,4); }
		}


		/// <summary>
		/// Hard reboot the MPR's processor.
		/// Resetting can bring the MPR up in one of two programs.
		///		If the Bootloader line is high, we enter the bootloader.
		///		Else, the normal Application is started.
		///	The Application uses no Handshaking, 57600 baud, and communicates through MPRMsg Frames.
		///	The Bootloader uses XonXoff Handshaking, 115200 baud, and communicates through ASCII text.
		///	The two different types of message require different Message handlers 
		///	(SerialPort_DataRecevied and SerialPortBL_DataRecevied).
		///	
		///	This Method waits 100 msec after toggling the Reset Line, for the MPR to reboot.
		/// </summary>
		/// <param name="BootLoader">Whether the MPR should start in the boot loader (true) 
		/// or the application (false).</param>
		public void ResetMPR(bool BootLoader)
		{
			try 
			{
				BasicPortSettings PBS = new BasicPortSettings();
				PBS.BaudRate = BaudRates.CBR_57600;
				PBS.ByteSize = 5;
				PBS.Parity = Parity.none;
				PBS.StopBits = StopBits.onePointFive;

				SerialPort.Settings = PBS;

				byte[] Data;
				if (BootLoader)
					Data = new byte[] {0x11, 0x01};
				else
					Data = new byte[] {0x10, 0x00};

				SerialPort.Output = Data;

				if (BootLoader) 
				{
					// Remove any previously assigned Data Received Handlers
					SerialPort.DataReceived -= new Port.CommEvent(SerialPort_DataReceived);
					SerialPort.DataReceived -= new Port.CommEvent(SerialPortBL_DataReceived);

					// Create new Basic Port Settings
					PBS = new BasicPortSettings();
					PBS.BaudRate = BaudRates.CBR_115200;
					PBS.ByteSize = 8;
					PBS.Parity = Parity.none;
					PBS.StopBits = StopBits.one;

					// Create a new HandshakeXonXoff DetailedPortSettings
					DetailedPortSettings DPS = new HandshakeXonXoff();
					DPS.BasicSettings = PBS;
					SerialPort.DetailedSettings = DPS;

					// Attach Serial Port Data Received Handler to the BL handler
					SerialPort.DataReceived += new Port.CommEvent(SerialPortBL_DataReceived);
				} 
				else 
				{
					// Remove any previously assigned Data Received Handlers
					SerialPort.DataReceived -= new Port.CommEvent(SerialPort_DataReceived);
					SerialPort.DataReceived -= new Port.CommEvent(SerialPortBL_DataReceived);

					PBS = new BasicPortSettings();
					PBS.BaudRate = BaudRates.CBR_57600;
					PBS.ByteSize = 8;
					PBS.Parity = Parity.none;
					PBS.StopBits = StopBits.one;

					// Create a new HandshakeNone DetailedPortSettings
					DetailedPortSettings DPS = new HandshakeNone();
					DPS.BasicSettings = PBS;
					SerialPort.DetailedSettings = DPS;

					// Attach Serial Port Data Received Handler to the Application handler
					SerialPort.DataReceived += new Port.CommEvent(SerialPort_DataReceived);
				}

				// Sleep for 100 msec to give MPR time to reboot
				Thread.Sleep(100);
			}
			catch {}
		}
	}
}