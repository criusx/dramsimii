//==========================================================================================
//
//		WJ.MPR.Reader.MPRReader
//		Copyright (c) 2005, WJ Communications, Inc.
//
//==========================================================================================
using System;
using System.Threading;
using System.IO;
using WJ.MPR.WinAPI;
using WJ.MPR.Util;

namespace WJ.MPR.Reader
{
	/// <summary>
	///		This is the main class instantiated by Applications.
	///		- Manages a single WJ Multi-Protocol Reader.
	///		- Provides properties and methods for accessing features of the MPR.
	///		- Talks to the MPR via an MPRComm object.
	///		- Generates request frame payloads for MPR API commands.
	///		- Parses response frame payloads from MPR API commands.
	///		- Fires events when MPR public properties change
	///		- Manages an Inventory of Tags in the MPRs field of view.
	///		- Inventory can consist of Class 0 or Class 1 tags.
	///		- Periodically polls the MPR for its current Inventory to:
	///			1) add new tags when found
	///			2) expire old tags
	///		- Old Tags are expired if they have not been seen for at least "PersistTime"
	///		- InventoryUpdateGap is how long to wait between inventory requests to the MPR.
	///		- Has methods to enable bootloading new FW to the MPR.
	///		- Exposes MPR methods to Read/Program/Erase/Kill/Lock Tags.
	/// </summary>
	public class MPRReader : IDisposable 
	{
		#region Events
		/// <summary>
		/// Raised when the communication state changes
		/// </summary>
		public event EventHandler DisconnectEvent;
		
		/// <summary>
		/// Fired when a new tag is added to the MPRReader's inventory.
		/// </summary>
		public event TagEventHandler TagAdded;
		/// <summary>
		/// Fired when a tag expires, ie hasn't been read for the persist time period.
		/// </summary>
		public event TagEventHandler TagRemoved;

		/// <summary>
		/// Fired when the Active Antenna changes.
		/// </summary>
		public event EventHandler ActiveAntennaChanged;
		/// <summary>
		/// Fired when the Tx Power changes.
		/// </summary>
		public event EventHandler TxPowerChanged;
		/// <summary>
		/// Fired when the Class 0 Singulation Field changes.
		/// </summary>
		public event EventHandler Class0SingulationFieldChanged;
		/// <summary>
		/// Fired when the Persist Time changes.
		/// </summary>
		public event EventHandler PersistTimeChanged;
		/// <summary>
		/// Fired when the Inventory Update Period changes.
		/// </summary>
		public event EventHandler InvUpdateGapChanged;
		/// <summary>
		/// Fired when the Inventory Timer Enabled property changes.
		/// </summary>
		public event EventHandler InvTimerEnabledChanged;
		/// <summary>
		/// Fired when manufacturing information is read from the MPR.
		/// </summary>
		public event EventHandler ManufacturingInformationChanged;

		/// <summary>
		/// Fired when an Inventory Update occurs.
		/// </summary>
		public event EventHandler InvPollEvent;

		/// <summary>
		/// Fired when a message is recevied from the bootloader.
		/// </summary>
		public event StringEventHandler BootloaderBytesReceivedEvent;

		/// <summary>
		/// Fired when a string message is generated.
		/// These messages are generated contain:
		///		- Communication log information
		///		- Command results
		///		- Error Messages
		/// </summary>
		public event StringEventHandler StatusEvent;

		#endregion

		#region Constructor

		/// <summary>
		/// Construct a new MPRReader.
		/// </summary>
		public MPRReader()
		{
			try 
			{
				pt.Start();
				InventoryUpdateTimer = new System.Threading.Timer(
					new TimerCallback(InventoryUpdateCallback), null, Timeout.Infinite, Timeout.Infinite);
			} 
			catch(ApplicationException ex)
			{
				LogIt(ex.Message);
			}
		}
		#endregion

		#region Public Functions

		/// <summary>
		/// Destroys any existing connection, 
		/// then starts the attempt to connect to an MPR at the requested Serial Port and Baud Rate.
		/// </summary>
		/// <param name="SerialPortName">"COMx" style serial port name</param>
		/// <param name="BaudRate">The Baud Rate at which to connect (usually 57600)</param>
		/// <returns>Whether an MPR was found at requested Port and BaudRate</returns>
		public bool Connect(string SerialPortName, string BaudRate)
		{
			this.SerialPortName = SerialPortName;
			this.BaudRate = BaudRate;

			isConnected = false;

			// First destroy the existing comm
			if (comm != null)
				comm.Dispose();

			// Then, make a new one
			comm = new MPRComm(SerialPortName, BaudRate);

			// Define MPRComm event handlers
			comm.IsOpenChanged += new BoolEventHandler(comm_IsOpenChanged);
			comm.BLDataArrived += new StringEventHandler(comm_BLDataArrived);

			// Open the serial port connection to the MPR.
			comm.Open();

			// Try to read the Reader Information,
			// if successfull, set isConnected to true.
			isConnected = (comm.IsOpen && UpdateReaderInfo());

			return isConnected;
		}

		/// <summary>
		/// Connect using the last (or default) SerialPortName and BaudRate
		/// </summary>
		/// <returns>Whether an MPR was found at default Port and BaudRate</returns>
		public bool Connect() { return Connect(this.SerialPortName, this.BaudRate); }

		/// <summary>
		/// Disable Inventory Timer and close MPRComm.
		/// </summary>
		public void Disconnect()
		{
			InvTimerEnabled = false;
			isConnected = false;
			if (comm != null)
				comm.Close();
		}

		/// <summary>
		/// Close MPRComm, but record that we should reconnect on Resume.
		/// </summary>
		public void Suspend()
		{
			ReconnectOnResume = IsConnected;
			comm.Close();
		}

		/// <summary>
		/// Re-connect to the MPR if we were supposed to (as recorded in Suspend())
		/// </summary>
		public void Resume() 
		{
			if (ReconnectOnResume)
				Connect();
		}


		#region Class0 Commands
		#region Matrics Class0+
		/// <summary>
		/// Read bits from a Matrics Class 0+ Tag.
		/// </summary>
		/// <param name="filter">a TagFilter specifying the tags to read from</param>
		/// <param name="ReadBitCount">How many bits to read</param>
		/// <param name="TagBits">A bytelist in which to return the bits read</param>
		/// <returns>The number of Tags Read</returns>
		public int Class0PlusRead(TagFilter filter, byte ReadBitCount, out byteList TagBits)
		{
			TagBits = new byteList();
			// Can read at most 120 bits
			if (ReadBitCount > 120) return 0;

			byteList Params = new byteList(
				new byte[] { ActiveAntenna, TxPower, Class0SingulationField, ReadBitCount } );

			Params.Add(filter.Length);
			Params.Add(filter.Bits);

			MPRMsg aMsg = comm.Send(CmdCode.Class0PlusRead, Params);

			if (!ProcMsgStat(aMsg))
				return 0;

			return Class0PlusReadParse(out TagBits, aMsg.DataBytes);
		}


		/// <summary>
		/// Write a Row of data to a Matrics Class 0+ Tag.
		/// </summary>
		/// <param name="filter">A Filter to match the Tags to write.</param>
		/// <param name="programData">The bits to write to the Tags.</param>
		/// <param name="IDPage">The IDPage (ID0-ID15) to Write.</param>
		/// <param name="eraseFirst">Whether to Erase the IDPage first.</param>
		/// <param name="lockAfter">Whether to Lock the IDPage after writing.</param>
		/// <param name="clearTIB">Clear the Traversal Inhibit Bit (ID2 only).</param>
		/// <param name="setTIB">Set the Traversal Inhibit Bit (ID2 only).</param>
		/// <returns>The Number of Tags written.</returns>
		public int Class0PlusWrite(TagFilter filter, TagFilter programData,
			byte IDPage, bool eraseFirst, bool lockAfter, bool clearTIB, bool setTIB)
		{
			// Can write at most 120 bits
			if (programData.Length > 120) return 0;
			if (IDPage > 15) return 0;

			byteList Params = new byteList(
				new byte[] { ActiveAntenna, TxPower, Class0SingulationField } );

			byte subCommand = IDPage;
			if (eraseFirst) subCommand |= 0x10;
			if (lockAfter) subCommand |= 0x20;
			if (clearTIB) subCommand |= 0x40;
			if (setTIB) subCommand |= 0x80;

			Params.Add(subCommand);

			Params.Add(programData.Length);
			Params.Add(programData.Bits);

			Params.Add(filter.Length);
			Params.Add(filter.Bits);

			MPRMsg aMsg = comm.Send(CmdCode.Class0PlusWrite, Params);

			if (!ProcMsgStat(aMsg))
				return 0;

			return ProcessSummary(aMsg.DataBytes.subList(aMsg.DataBytes.Count - 6));
		}
		#endregion

		#region Zuma SubCommands
		/// <summary>
		/// Zuma command to Read a row of data (or Tag ID).
		/// </summary>
		/// <param name="row"></param>
		/// <param name="filter"></param>
		/// <returns></returns>
		public int Class0ZumaRowRead(ref ZumaRow row, TagFilter filter)
		{
			// Must have exactly 3 data bytes in RowData.
			//			if (row == null) return false;

			// Rows between 0 and 15
			if (row.RowNumber > 15) return 0;

			byteList Params = new byteList(
				new byte[] { ActiveAntenna, TxPower, Class0SingulationField, 
							   (byte)Class0ZumaSubcommands.ReadRow } );

			Params.Add(row.RowNumber);
			Params.Add(filter.Length);
			Params.Add(filter.Bits);

			MPRMsg aMsg = comm.Send(CmdCode.Class0ZumaCommands, Params);

			if (!ProcMsgStat(aMsg))
				return 0;

			return Class0ZumaRowParse(ref row, aMsg.DataBytes);
		}

		/// <summary>
		/// Zuma command to Write a row of data (or Tag ID).
		/// </summary>
		/// <param name="row"></param>
		/// <param name="filter"></param>
		/// <param name="Ack"></param>
		/// <returns>int : # Tags written</returns>
		public int Class0ZumaRowWrite(ZumaRow row, TagFilter filter, bool Ack)
		{
			// Rows between 0 and 15
			if (row.RowNumber > 15) return 0;

			byteList Params = new byteList(
				new byte[] { ActiveAntenna, TxPower, Class0SingulationField, (byte)((Ack) ? 
							   Class0ZumaSubcommands.WriteAck :
							   Class0ZumaSubcommands.WriteRow) } );

			Params.Add(row.RowNumber);
			Params.Add(row.ToArray);
			Params.Add(filter.Length);
			Params.Add(filter.Bits);

			MPRMsg aMsg = comm.Send(CmdCode.Class0ZumaCommands, Params);

			if (!ProcMsgStat(aMsg))
				return 0;

			return ProcessSummary(aMsg.DataBytes.subList(aMsg.DataBytes.Count - 6));
		}

		/// <summary>
		/// Zuma command to Initialize a tag.
		/// </summary>
		/// <param name="filter"></param>
		/// <param name="SetLock">Whether or not to set the lock bit</param>
		/// <returns>The number of Tags the responded to the INIT</returns>
		public int Class0ZumaInit(TagFilter filter, bool SetLock)
		{
			byteList Params = new byteList(
				new byte[] { ActiveAntenna, TxPower, Class0SingulationField, 
							   (byte)Class0ZumaSubcommands.INIT } );

			Params.Add((byte)(SetLock?1:0));
			Params.Add(filter.Length);
			Params.Add(filter.Bits);

			// Send message
			MPRMsg aMsg = comm.Send(CmdCode.Class0ZumaCommands, Params);

			if (!ProcMsgStat(aMsg))
				return 0;

			return ProcessSummary(aMsg.DataBytes.subList(aMsg.DataBytes.Count - 6));
		}
		#endregion

		/// <summary>
		/// Kills a Class 0 Tag.
		/// </summary>
		/// <param name="TagIDBits">The TagID of the Class 0 tag to Kill.</param>
		/// <param name="KillPasscode">The 24-bit Passcode of the Class 0 tag to Kill.</param>
		/// <returns></returns>
		public bool Class0Kill(byteList TagIDBits, uint KillPasscode)
		{
			// TagIDBits must be a complete 8 or 12 byte ePC Tag ID
			if ((TagIDBits.Count != 8) && (TagIDBits.Count != 12))
				return false;

			byteList Params = new byteList(
				new byte[] { ActiveAntenna, TxPower, Class0SingulationField } );

			// Add the 3-byte (24-bit) Kill Passcode
			Params.Add(new byte[] {(byte)(KillPasscode >> 16), (byte)(KillPasscode >> 8), (byte)KillPasscode} );
			Params.Add(TagIDBits);

			// Send message
			MPRMsg aMsg = comm.Send(CmdCode.Class0Kill, Params);

			// There should be exactly one response data byte, equal to 1 if tag was killed; otherwise 0
			return (ProcMsgStat(aMsg) && (aMsg.DataBytes.Count == 1) && (aMsg.DataBytes[0] == 1));		
		}
		#endregion

		#region Class1 Commands
		/// <summary>
		/// Kill Class1 Tag.
		/// TagID represented as a byteList.
		/// </summary>
		/// <param name="TagIDBits">The bits to match to singulate the Tag to kill.</param>
		/// <param name="Passcode">The Kill Passcode of the Tag to kill.  Must be 8 or 12 bytes long.</param>
		/// <returns>true if MPR responds as expected; false if invalid TagIDBits.Length, or MPR responds oddly.</returns>
		public bool Class1Kill(byteList TagIDBits, byte Passcode)
		{
			// TagIDBits must be a complete 8 or 12 byte ePC Tag ID
			if ((TagIDBits.Count != 8) && (TagIDBits.Count != 12))
				return false;

			byteList Params = new byteList(
				new byte[] { ActiveAntenna, TxPower } );

			Params.Add(Passcode);
			Params.Add(TagIDBits);

			// Send message
			MPRMsg aMsg = comm.Send(CmdCode.Class1Kill, Params);

			// There should be no response data
			return (ProcMsgStat(aMsg) && (aMsg.DataBytes.Count == 0));	
		}

		/// <summary>
		/// Write 16 bits to a Class1 Tag.
		/// Word Addresses address 16 bits.
		/// </summary>
		/// <param name="WordAddress">The word Address.</param>
		/// <param name="Word">The 16-bit value with which to program the tag.</param>
		/// <returns>true if MPR responds properly; otherwise false.</returns>
		public bool Class1Write(byte WordAddress, ushort Word)
		{
			return Class1Write(WordAddress, (byte)(Word >> 8), (byte)(Word));
		}

		/// <summary>
		/// Write 16 bits to a Class1 Tag.
		/// Word Addresses address 16 bits.
		/// </summary>
		/// <param name="WordAddress">The word Address.</param>
		/// <param name="MSB">Most Significant byte to program the tag.</param>
		/// <param name="LSB">Least Significant byte to program the tag.</param>
		/// <returns>true if MPR responds properly; otherwise false.</returns>
		public bool Class1Write(byte WordAddress, byte MSB, byte LSB)
		{
			byteList Params = new byteList(
				new byte[] { ActiveAntenna, TxPower, (byte)(WordAddress * 16), MSB, LSB } );

			// Send message
			MPRMsg aMsg = comm.Send(CmdCode.Class1Write, Params);

			// There should be no response data
			return (ProcMsgStat(aMsg) && aMsg.DataBytes.Count == 0);	
		}

		/// <summary>
		/// Read all bits of a Class1 tag's EPC and CRC.
		/// </summary>
		/// <param name="Time">The Time it took to perform the command (in seconds).</param>
		/// <returns>A Class 1 Tag with its data and CRC populated with the read data.  null if no tag read.</returns>
		public RFIDTag Class1Verify(out double Time)
		{
			byteList Params = new byteList(
				new byte[] { ActiveAntenna, TxPower } );

			// Send message
			HiPerfTimer pt = new HiPerfTimer();
			pt.Start();
			MPRMsg aMsg = comm.Send(CmdCode.Class1IDVerify, Params);
			pt.Stop();
			Time = pt.Duration;

			if (!ProcMsgStat(aMsg))
				return null;

			// Process response data
			return Class1VerifyParse(aMsg.DataBytes);
		}

		/// <summary>
		/// Read all bits of a Class1 tag's EPC and CRC.
		/// </summary>
		/// <returns>A Class 1 Tag with its data and CRC populated with the read data.  null if no tag read.</returns>
		public RFIDTag Class1Verify()
		{
			double ignore;
			return Class1Verify(out ignore);
		}

		/// <summary>
		/// Erase a Class1 Tag.
		/// Sets all bits of identifier, CRC and Passcode to all 0s.
		/// </summary>
		/// <returns>true if MPR Responded properly; otherwise false.</returns>
		public bool Class1Erase()
		{
			byteList Params = new byteList(
				new byte[] { ActiveAntenna, TxPower } );

			// Send message
			MPRMsg aMsg = comm.Send(CmdCode.Class1IDErase, Params);

			// There should be no response data
			return (ProcMsgStat(aMsg) && (aMsg.DataBytes.Count == 0));	
		}
		#endregion

		/// <summary>
		/// The MPR Series Reader can be reset by twiddling with the serial port.
		/// </summary>
		/// <param name="EnterBootloader">Whether to enter the bootloader.</param>
		public void Reset(bool EnterBootloader)
		{
			comm.ResetMPR(EnterBootloader);
		}

		#region Bootloader Commands
		
		/// <summary>
		/// A shortcut for entering the Bootloader (same as Reset(true))
		/// </summary>
		public void EnterBootloader() { Reset(true); }

		/// <summary>
		/// Given a Stream, enter the bootloader 
		/// launch a new Thread (from the ThreadPool)
		/// and send the Stream to the MPR's bootloader.
		/// </summary>
		/// <param name="FWStream">The Stream to send to the MPR bootloader.</param>
		public void BeginBootloadFromStream(Stream FWStream)
		{
			EnterBootloader();
			FWStream.Seek(0, SeekOrigin.Begin);

			ThreadPool.QueueUserWorkItem(new WaitCallback(BootloadFromStream), FWStream);
		}

		/// <summary>
		/// Send each row of the Stream to the MPR bootloader.
		/// Will fail if parameter oStream is not a valid Stream,
		/// or MPR not already in the bootloader.
		/// </summary>
		/// <param name="oStream">A Stream passed as an object</param>
		private void BootloadFromStream(object oStream)
		{
			try 
			{
				Stream FWStream = oStream as Stream;
				// Create an instance of StreamReader to read from a file.
				// The using statement also closes the StreamReader.
				using (StreamReader sr = new StreamReader(FWStream))
				{
					string line;

					// Read and display lines from the file until the end of 
					// the file is reached.
					while ((line = sr.ReadLine()) != null) 
					{
						comm.SendBLLine(line);
					}
				}
			} 
			catch (InvalidCastException) {}
		}

		#endregion

		#endregion

		#region Public Properties

		private byte activeAntenna = 0;
		/// <summary>
		/// The active antenna can be either A or B.  
		/// Antenna A is the left antenna port when looking down on the card, with the antenna ports facing up.
		/// </summary>
		public byte ActiveAntenna 
		{
			get { return activeAntenna; }
			set 
			{
				if (activeAntenna != value) 
				{
					activeAntenna = value; 
					if (ActiveAntennaChanged != null)
						ActiveAntennaChanged(this, new EventArgs());
				}
			}
		}
		

		private byte txPower = 255;
		/// <summary>
		/// The Tx Power can range from 15-27 dBm for MPR5000/MPR6000, 
		/// and 18-30 dBm for MPR7000.
		/// Ranges are stored in card NVS, and can be read back.
		/// </summary>
		public byte TxPower 
		{ 
			get { return txPower; }
			set 
			{
				if (txPower != value) 
				{ 
					txPower = value;
					if (TxPowerChanged != null)
						TxPowerChanged(this, new EventArgs());
				}
			}
		}

		private int txPowerMin = 0;
		/// <summary>
		/// The minimum TxPower, as read from the MPR.
		/// </summary>
		public int TxPowerMin { get { return txPowerMin; } }
		private int txPowerMax = 255;
		/// <summary>
		/// The maximum TxPower, as read from the MPR.
		/// </summary>
		public int TxPowerMax { get { return txPowerMax; } }


		/// <summary>
		/// Singulation Field for Class0 commands
		/// </summary>
		private byte singulationField = 0;
		/// <summary>
		/// The Class0 Singulation Field can be 0, 1 or 2 for 
		///		0 = ID0 (Generated Random ID)
		///		1 = ID1 (Static Random ID)
		///		2 = ID2 (the EPC Code itself)
		///	Used by the MPR to determine how to singulate tags.
		/// </summary>
		public byte Class0SingulationField
		{
			get { return singulationField; }
			set 
			{ 
				if (value != singulationField) 
				{
					singulationField = value;

					if (Class0SingulationFieldChanged != null)
						Class0SingulationFieldChanged(this, new EventArgs());
				}
			}
		}

		/// <summary>
		/// A string describing the COM port and baud rate, or "No Comm" 
		/// if the communications settings are not set.
		/// </summary>
		public string CommParams
		{
			get { return (comm != null)?comm.PortSetting:"No Comm"; }
		}

		private TimeSpan persistTime = new TimeSpan(0,0,0,1,0);
		/// <summary>
		/// How long a tag that has been read will persist in the inventory,
		/// without being read, while an inventory is running.
		/// 
		/// If an inventory is stopped, tags do not expire.
		/// If a tag is re-read, it will live at least another peristTime.
		/// </summary>
		public TimeSpan PersistTime
		{ 
			get { return persistTime; }
			set 
			{ 
				if (value != persistTime) 
				{
					persistTime = value;

					if (PersistTimeChanged != null)
						PersistTimeChanged(this, new EventArgs());
				}
			}
		}

		private TimeSpan invUpdateGap = new TimeSpan(0,0,0,0,250);
		/// <summary>
		/// How much time to wait between calls to UpdateInventory when
		/// an inventory loop is running.
		/// </summary>
		public int InvUpdateGap
		{ 
			get { return (int)invUpdateGap.TotalMilliseconds; }
			set 
			{ 
				if (value != (int)invUpdateGap.TotalMilliseconds) 
				{
					invUpdateGap = (value == int.MaxValue) ?
						TimeSpan.MaxValue : new TimeSpan(0, 0, 0, value / 1000, value % 1000);

					KickInvTimer();

					if (InvUpdateGapChanged != null)
						InvUpdateGapChanged(this, new EventArgs());
				}
			}
		}


		private bool invTimerEnabled = false;
		/// <summary>
		/// Whether an inventory is actively running.
		/// When set to true, a timer is started that will expire after InvUpdateGap and call UpdateInventory()
		/// </summary>
		public bool InvTimerEnabled
		{
			get { return invTimerEnabled; }
			set 
			{
				if (value != invTimerEnabled)
				{
					invTimerEnabled = value;
					if (InvTimerEnabledChanged != null)
						InvTimerEnabledChanged(this, new EventArgs());

					if (invTimerEnabled) // If enabling, have timer expire immediately
					{
						pt.Start();
						InventoryUpdateTimer.Change(0, Timeout.Infinite);

						Thread.Sleep(0);
					}
					else
						InventoryUpdateTimer.Change(Timeout.Infinite, Timeout.Infinite);
				}
			}
		}
	
		/// <summary>
		/// Returns the inventory (list of tags) that have been read by the reader,
		/// but not expired.
		/// </summary>
		public Inventory TagInventory
		{
			get { return inventory; }
		}

		#region Manufacturing Information

		private string _DeviceModelNumber;
		private string _DeviceSerialNumber;
		private string _HardwareVersion;
		private string _ManufactureDate;
		private string _ManufacturerName;
		private string _FirmwareVersion;
		private int _FirmwareMinorVersion;
		private int _FirmwareMajorVersion;
		private string _BootLoaderVersion;
		private int _BootLoaderMinorVersion;
		private int _BootLoaderMajorVersion;

		/// <summary>
		/// Device Model Number is read and returned by the MPR "Get Information" Command.
		/// A string of 8 ASCII bytes.
		/// </summary>
		public string DeviceModelNumber { get { return _DeviceModelNumber; } }
		/// <summary>
		/// Device Serial Number is read and returned by the MPR "Get Information" Command.
		/// A string of 12 ASCII bytes.
		/// </summary>
		public string DeviceSerialNumber { get { return _DeviceSerialNumber; } }
		/// <summary>
		/// Hardware Version Number is read and returned by the MPR "Get Information" Command.
		/// A string of 8 ASCII bytes.
		/// </summary>
		public string HardwareVersion  { get { return _HardwareVersion; } }
		/// <summary>
		/// Manufacture Date is read and returned by the MPR "Get Information" Command.
		/// A string of 8 ASCII bytes.
		/// </summary>
		public string ManufactureDate  { get { return _ManufactureDate; } }
		/// <summary>
		/// Manufacturer Name is read and returned by the MPR "Get Information" Command.
		/// A string of 16 ASCII bytes.
		/// </summary>
		public string ManufacturerName  { get { return _ManufacturerName; } }
		/// <summary>
		/// Firmware Version Number is read and returned by the MPR "Get Information" Command as 2 bytes.
		/// This is converted to a string with dotted decimal notation.
		/// </summary>
		public string FirmwareVersion	{ get { return _FirmwareVersion; } }

		/// <summary>
		/// Firmware Major Version is MSB of the FirmwareVersion
		/// </summary>
		public int FirmwareMajorVersion	{ get { return _FirmwareMajorVersion; } }
		/// <summary>
		/// Firmware Major Version is LSB of the FirmwareVersion
		/// </summary>
		public int FirmwareMinorVersion	{ get { return _FirmwareMinorVersion; } }

		/// <summary>
		/// Bootloader Version Number is read and returned by the MPR "Get Information" Command as 2 bytes.
		/// This is converted to a string with dotted decimal notation.
		/// </summary>
		public string BootLoaderVersion  { get { return _BootLoaderVersion; } }

		/// <summary>
		/// Bootloader Major Version is MSB of the BootloaderVersion
		/// </summary>
		public int BootloaderMajorVersion	{ get { return _BootLoaderMajorVersion; } }
		/// <summary>
		/// Bootloader Major Version is LSB of the BootloaderVersion
		/// </summary>
		public int BootloaderMinorVersion	{ get { return _BootLoaderMinorVersion; } }
        
		/// <summary>
		/// Update the Reader's Manufacturing Information
		/// </summary>
		/// <returns>Whether the reader's manu info read was successful</returns>
		public bool UpdateReaderInfo()
		{
			MPRMsg aMsg = comm.Send(CmdCode.ReaderInfo, null);

			bool CouldParseMsg = false;

			if (!ProcMsgStat(aMsg)) 
			{
				_DeviceModelNumber = "unknown";
				_DeviceSerialNumber = "unknown";
				_HardwareVersion = "unknown";
				_ManufactureDate = "unknown";
				_ManufacturerName = "unknown";
				_FirmwareVersion = "unknown";
				_BootLoaderVersion = "unknown";

				CouldParseMsg = false;
			} 
			else if (aMsg.DataBytes.Count == 10)	// For Pre-production firmware
			{
				_DeviceModelNumber = "";
				_DeviceSerialNumber = aMsg.DataBytes.subList(0,8).ToHexString();
				_HardwareVersion = "";
				_ManufactureDate = "";
				_ManufacturerName = "";
				_FirmwareVersion = aMsg.DataBytes.subList(8,10).ToHexString(".");
				try 
				{
					string[] splits = _FirmwareVersion.Split(new char[] {'.'});
					_FirmwareMinorVersion = int.Parse(splits[1], System.Globalization.NumberStyles.AllowHexSpecifier);
					_FirmwareMajorVersion = int.Parse(splits[0], System.Globalization.NumberStyles.AllowHexSpecifier);
				} 
				catch 
				{
					_FirmwareMinorVersion = 0;
					_FirmwareMajorVersion = 0;
				}
				_BootLoaderVersion = "";
				CouldParseMsg = true;
			}
			else if (aMsg.DataBytes.Count == 56)	// Production firmware ( >= 01.00)
			{
				_DeviceModelNumber = aMsg.DataBytes.subList(0,8).ToString();
				_DeviceSerialNumber = aMsg.DataBytes.subList(8,20).ToString();
				_HardwareVersion = aMsg.DataBytes.subList(20,28).ToString();
				_ManufactureDate = aMsg.DataBytes.subList(28,36).ToString();
				_ManufacturerName = aMsg.DataBytes.subList(36,52).ToString();
				_FirmwareVersion = aMsg.DataBytes.subList(52,54).ToHexString(".");
				try 
				{
					string[] splits = _FirmwareVersion.Split(new char[] {'.'});
					_FirmwareMinorVersion = int.Parse(splits[1], System.Globalization.NumberStyles.AllowHexSpecifier);
					_FirmwareMajorVersion = int.Parse(splits[0], System.Globalization.NumberStyles.AllowHexSpecifier);
					splits = _BootLoaderVersion.Split(new char[] {'.'});
					_BootLoaderMinorVersion = int.Parse(splits[1], System.Globalization.NumberStyles.AllowHexSpecifier);
					_BootLoaderMajorVersion = int.Parse(splits[0], System.Globalization.NumberStyles.AllowHexSpecifier);
				} 
				catch 
				{
					_BootLoaderMinorVersion = 0;
					_BootLoaderMajorVersion = 0;
				}
				_BootLoaderVersion = aMsg.DataBytes.subList(54,56).ToHexString(".");
				CouldParseMsg = true;
			}

			RecomputeTxPowerLimits();

			if (ManufacturingInformationChanged != null)
				ManufacturingInformationChanged(this, new EventArgs());
			
			return CouldParseMsg;
		}

		/// <summary>
		/// Tx Power Limits are a function of the model type, as reported by the MPR
		/// during a "Read Information" Command.
		/// </summary>
		private void RecomputeTxPowerLimits() 
		{
			if ((FirmwareMajorVersion >= 1) || (FirmwareMinorVersion >= 24)) 
			{
				if ((_DeviceModelNumber.Trim() == "MPR5000") || (_DeviceModelNumber.Trim() == "MPR6000"))
				{
					txPowerMin = 15;
					txPowerMax = 27;
				} 
				else if (_DeviceModelNumber.Trim() == "MPR7000")
				{
					txPowerMin = 18;
					txPowerMax = 30;
				}
				else 
				{
					txPowerMin = 0;
					txPowerMax = 255;
				}
			}
		}

		#endregion

		#region Inventories
		/// <summary>
		/// Request the EPC Gen 1 Class 0 Inventory from the MPR.
		/// </summary>
		public Inventory Class0Inventory
		{
			get 
			{
				byteList InvParams;

				// If unknown FWVersion, just return an empty inventory

				if (this.FirmwareVersion == "unknown")
					return new Inventory();

				// >= Rev 8 FW Class0Inventory parameter change:
				// Removed "Data Bit Flag/Cnt" field
				// byteList InvParams = new byteList(new byte[] {ActiveAntenna, TxPower, SingulationField, 0});

				if ((FirmwareMajorVersion > 0) || (FirmwareMinorVersion <= 7))
					InvParams = new byteList(new byte[] {ActiveAntenna, TxPower, Class0SingulationField, 0, 0});
				else
					InvParams = new byteList(new byte[] {ActiveAntenna, TxPower, Class0SingulationField, 0});

				MPRMsg aMsg = comm.Send(CmdCode.Class0Read, InvParams);
				if (!ProcMsgStat(aMsg))
					return new Inventory();

				return InventoryParse(aMsg.DataBytes, RFIDProtocol.EPCClass0);
			}
		}

		/// <summary>
		/// Request the EPC Gen 1 Class 1 Inventory from the MPR.
		/// </summary>
		public Inventory Class1Inventory
		{
			get 
			{
				byteList InvParams = new byteList(new byte[] {ActiveAntenna, TxPower, 0});
				MPRMsg aMsg = comm.Send(CmdCode.Class1Read, InvParams);
				if (!ProcMsgStat(aMsg))
					return new Inventory();

				return InventoryParse(aMsg.DataBytes, RFIDProtocol.EPCClass1);
			}
		}

		/// <summary>
		/// Request the UHF Gen 2 Class 1 Inventory from the MPR.
		/// - not implemented yet.
		/// </summary>
		public Inventory Gen2Inventory { get { return new Inventory(); } }
		#endregion

		/// <summary>
		/// The method called when the InventoryUpdateTimer expires.
		/// Updates the Inventories,
		/// Fires an InvPollEvent,
		/// and restarts the InventoryUpdateTimer, if still enabled.
		/// </summary>
		/// <param name="o"></param>
		private void InventoryUpdateCallback(object o)
		{
			try
			{
				UpdateInventory();
				pt.Stop();
				// Console.WriteLine("{0}",pt.Duration);

				// A bit kludgy:
				// "don't fire event if it is occuring, 
				// but shouldn't be (InvTimerEnabled == false)...
				if ((InvTimerEnabled) && (InvPollEvent != null))
					InvPollEvent(this, new EventArgs());
				KickInvTimer();
			}
			catch (ObjectDisposedException)
			{
				// Turn off timer
				InvTimerEnabled = false;
			}
			catch (Exception ex)
			{
				// Turn off timer
				InvTimerEnabled = false;
				LogIt(ex.Message);
			}
		}

		/// <summary>
		/// Form's copy of current persisted inventory
		/// </summary>
		private Inventory TagsRead = new Inventory();
		/// <summary>
		/// Returns an Inventory Object populated with the tags that have been read and not expired.
		/// </summary>
		public Inventory CurrentInventory
		{
			get 
			{
				Inventory TempInventory = new Inventory();
				lock (TagsRead) 
				{
					foreach(RFIDTag T in TagsRead)
					{
						TempInventory.Add(T);
					}
				}
				return TempInventory;
			}
		}

		/// <summary>
		/// Whether or not to perform EPC Class 0 inventories.
		/// </summary>
		public bool Class0InventoryEnabled = true;
		/// <summary>
		/// Whether to perform EPC Class 1 inventories.
		/// </summary>
		public bool Class1InventoryEnabled = true;
		/// <summary>
		/// Whether to perform EPC UHF Gen2 inventories.
		/// </summary>
		public bool Gen2InventoryEnabled = false;

		/// <summary>
		/// A running inventory is called in a loop:
		///		A timer expires after InvUpdateGap.
		///		All enabled inventories are queried from the MPR.
		///		New tags are added, 
		///		and expired tags are removed.
		/// </summary>
		private void UpdateInventory()
		{
			// create new inventory
			Inventory InvTags = new Inventory();

			DateTime invTime = DateTime.Now;

			if (Class0InventoryEnabled)
				InvTags.Add(this.Class0Inventory);

			if (Class1InventoryEnabled)
				InvTags.Add(this.Class1Inventory);
			
			if (Gen2InventoryEnabled)
				InvTags.Add(this.Gen2Inventory);

			int[] clscnt = InvTags.CountClass;

			LogIt(String.Format("Tags Read: (Class0 , Class1) = ({0} , {1})\n", clscnt[0], clscnt[1]));

			if (InvTags == null) return;

			foreach (RFIDTag tmpTag in InvTags)
				lock (TagsRead) 
				{
					RFIDTag invTag = TagsRead.Find(tmpTag.TagID);

					if (invTag == null) 
					{
						RFIDTag AddedTag = tmpTag;
						TagsRead.Add(tmpTag);
						if (TagAdded != null)
							TagAdded(AddedTag);
					}
					else
						invTag.ReRead(invTime);
				}
		
			if (PersistTime != TimeSpan.MaxValue) 
				lock (TagsRead) 
				{
					// Start at end of Inventory and work back towards beginning,
					// removing all expired tags.
					for (int index = TagsRead.Count - 1; index >= 0; index--)
						if (invTime > TagsRead[index].LastRead + PersistTime)
						{
							RFIDTag RemovedTag = TagsRead[index];
							TagsRead.RemoveAt(index);
							if (TagRemoved != null)
								TagRemoved(RemovedTag);
						}
				}
		}

		/// <summary>
		/// Clear the Inventory by removing all tags.
		/// </summary>
		public void ClearInventory()
		{
			lock (TagsRead) 
			{
				// Start at end of Inventory and work back towards beginning,
				// removing all expired tags.
				for (int index = TagsRead.Count - 1; index >= 0; index--) 
				{
					RFIDTag RemovedTag = TagsRead[index];
					TagsRead.RemoveAt(index);
					if (TagRemoved != null)
						TagRemoved(RemovedTag);
				}
			}
		}

		private bool isConnected = false;
		/// <summary>
		/// Whether we are connected to the MPR or not.
		/// </summary>
		public bool IsConnected { get { return isConnected; } }

		#endregion

		#region Private Properties

		private MPRComm comm;

		/// <summary>
		/// High Performance Timer
		/// </summary>
		private HiPerfTimer pt = new HiPerfTimer();

		/// <summary>
		/// Inventory loop timer
		/// </summary>
		private System.Threading.Timer InventoryUpdateTimer;

		private bool ReconnectOnResume = false;

		// Properties for connecting to an MPR via a COM Port
		private string SerialPortName;
		private string BaudRate;

		private Inventory inventory = new Inventory();

		#endregion

		#region Private Functions

		/// <summary>
		/// Asynchronously log some status by calling the commlogger
		/// delegate (if it has been assigned).
		/// </summary>
		/// <param name="s">string: The string to log.</param>
		private void LogIt(string s)
		{
			if (StatusEvent != null)
				StatusEvent(s);
		}


		#region Message Processing
		/// <summary>
		/// Process the Status byte of a message response, and 
		/// log an error message if the message is not OK.
		/// </summary>
		/// <param name="aMsg">The Msg to process</param>
		/// <param name="enableLogging">Whether to log the raw serial port transaction.</param>
		/// <returns>Whether the message response status was OK</returns>
		private bool ProcMsgStat(MPRMsg aMsg, bool enableLogging)
		{
			// Display the serial port traffic
			if (enableLogging) LogIt("Txed: " + aMsg.TxFrame.ToHexString());
			foreach(MPRFrame rxFrame in aMsg.RxFrames) 
				if (enableLogging) LogIt("Rxed: " + rxFrame.ToHexString());

			// Process response status
			if (aMsg.Status != RspStatus.OK)
			{
				if (enableLogging) LogIt(aMsg.MsgID.ToString() 
									   + ": " + aMsg.Status.ToString() 
									   + " " + aMsg.Error.ToString());

				if (aMsg.commErrorString != null)
					if (enableLogging) LogIt(aMsg.commErrorString);
				return false;
			} 
			else 
				return true;
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="aMsg"></param>
		/// <returns></returns>
		private bool ProcMsgStat(MPRMsg aMsg) { return ProcMsgStat(aMsg, true);	}

		/// <summary>
		/// Most frames end with three counts,
		/// Tags Processed,
		/// Under-run errors (Tags whose TagID had less bits than the EPC header suggests it should have had)
		/// CRC errors
		/// </summary>
		/// <param name="Summary">A bytelist of the 6 bytes that comprise the Message Summary</param>
		/// <returns>The number of tags successfully processed; 0 if Summary wasn't exactly 6 bytes.</returns>
		private ushort ProcessSummary(byteList Summary) 
		{
			if (Summary.Count != 6) return 0;
			ushort TagsProcessed = Helpers.b2us(Summary[0], Summary[1]);
			LogIt("Tags Processed: " + TagsProcessed);
			LogIt("Under-run error count: " + Helpers.b2us(Summary[2], Summary[3]));
			LogIt("Tag CRC error count: " + Helpers.b2us(Summary[4], Summary[5]));
			LogIt("");
			return TagsProcessed;
		}


		/// <summary>
		/// Decode the Tag IDs in the accumulated frame payloads received in response to an inventory command.
		/// </summary>
		/// <param name="rawBytes">The accumulated payload bytes.</param>
		/// <param name="Protocol">The RFID Protocol of the inventory being processed.</param>
		/// <returns>An Inventory (Tag List) containing the Tags parsed from the payload.</returns>
		private Inventory InventoryParse(byteList rawBytes, RFIDProtocol Protocol)
		{
			// If there are less than 7 bytes, there cannot be a valid (even empty) inventory in this byteList
			if (rawBytes.Count < 6) return new Inventory();

			LogIt(Protocol.ToString() + " Inventory");
			ProcessSummary(rawBytes.subList(rawBytes.Count - 6));

			Inventory mInv = new Inventory();

			int bPtr = 0;
			while (bPtr < rawBytes.Count - 6) 
			{
				byte TagsInPkt = rawBytes[bPtr++];
				while (TagsInPkt-- > 0) 
				{
					byte TagType = rawBytes[bPtr];
					// TagType MSbits = 00b for ePC-96 (12 bytes), all other 2MSbits => ePC-64 (8 bytes)
					int TagLen = ((TagType & 0xC0) == 0x00)?12:8;
					RFIDTag aTag = new RFIDTag(rawBytes.subList(bPtr, bPtr + TagLen), Protocol);
					mInv.Add(aTag);
					LogIt(string.Format("{0} Tag Found: {1}", Protocol.ToString(), aTag.TagID));
					bPtr += TagLen;
				}
			}
			LogIt("");
			return mInv;
		}

		/// <summary>
		/// Parse the response of a Class0+ read row command.
		/// Each response frame consists of:
		///		The number of tags in its payload (1 byte)
		///		For each tag in the frame:
		///			the Tag ID
		///			3 bytes from the row being read.
		/// </summary>
		/// <param name="TagBits"></param>
		/// <param name="rawBytes">The combined payloads of all response packets for this message.</param>
		/// <returns>The number of Tags read by this command.</returns>
		private int Class0PlusReadParse(out byteList TagBits, byteList rawBytes)
		{
			TagBits = new byteList();

			// If there are less than 7 bytes, there cannot be a valid (even empty) inventory in this byteList
			if (rawBytes.Count < 6) return 0;

			ProcessSummary(rawBytes.subList(rawBytes.Count - 6));

			int bPtr = 0;
			int TagsRead = 0;

			while (bPtr < rawBytes.Count - 6) 
			{
				byte TagsInPkt = rawBytes[bPtr++];
				TagsRead += TagsInPkt;
				while (TagsInPkt-- > 0) 
				{
					byte TagType = rawBytes[bPtr];
					// TagType MSbits = 00b for ePC-96 (12 bytes), all other 2MSbits => ePC-64 (8 bytes)
					int TagLen = ((TagType & 0xC0) == 0x00)?12:8;
					// mInv.Add(new Tag(rawBytes.subList(bPtr, bPtr + TagLen), TagClass));
					RFIDTag newTag = new RFIDTag(rawBytes.subList(bPtr, bPtr + TagLen), RFIDProtocol.EPCClass0);
					bPtr += TagLen;

					byte[] data = rawBytes.ToArray(bPtr, bPtr+3);
					TagBits.Add(data);
					bPtr += 3;
				}
			}
			return TagsRead;
		}

		/// <summary>
		/// Parse the response from a Class 0 Zuma "Read Row" request message.
		/// </summary>
		/// <param name="row">The row to which this data is supposed to correspond.</param>
		/// <param name="rawBytes">The response packet payload</param>
		/// <returns>The number of tags received and parsed.</returns>
		private int Class0ZumaRowParse(ref ZumaRow row, byteList rawBytes)
		{
			// If there are less than 7 bytes, there cannot be a valid (even empty) inventory in this byteList
			if (rawBytes.Count < 6) return 0;

			ProcessSummary(rawBytes.subList(rawBytes.Count - 6));

			int bPtr = 0;
			int TagsRead = 0;

			// A new Inventory to hold the response tags and rows
			//Inventory mInv = new Inventory();
			
			while (bPtr < rawBytes.Count - 6) 
			{
				byte TagsInPkt = rawBytes[bPtr++];
				TagsRead += TagsInPkt;
				while (TagsInPkt-- > 0) 
				{
					byte TagType = rawBytes[bPtr];
					// TagType MSbits = 00b for ePC-96 (12 bytes), all other 2MSbits => ePC-64 (8 bytes)
					int TagLen = ((TagType & 0xC0) == 0x00)?12:8;
					//					mInv.Add(new Tag(rawBytes.subList(bPtr, bPtr + TagLen), TagClass));
					RFIDTag newTag = new RFIDTag(rawBytes.subList(bPtr, bPtr + TagLen), RFIDProtocol.EPCClass0);
					bPtr += TagLen;

					// Assign row data to row!
					row.ToArray = rawBytes.ToArray(bPtr, bPtr+3);
					bPtr += 3;
				}
			}

			return TagsRead;
		}

		/// <summary>
		/// Parse the response to a Class 1 Verify Command.
		/// Each response frame of a Verify command's response consists of:
		///		The number of tags in this frame.
		///		The whole TagID of the tag.
		///		The Kill Passcode of the tag.
		///		The Lock Code of the tag.
		/// </summary>
		/// <param name="rawBytes"></param>
		/// <returns>An RFIDTag with the TagID, Passcode and Lockcode decoded from the response.</returns>
		private RFIDTag Class1VerifyParse(byteList rawBytes)
		{
			// If there are less than 7 bytes, there cannot be a valid (even empty) inventory in this byteList
			if (rawBytes.Count < 6) return null;

			ProcessSummary(rawBytes.subList(rawBytes.Count - 6));

			RFIDTag mTag = null;

			// Parse the number of Tags in this packet.
			byte TagsInPkt = rawBytes[0];

			// Only parse the tag info if one and only one tag is being verified
			if (TagsInPkt == 1) 
			{
				byteList TagData = new byteList(rawBytes, 1, rawBytes.Count - 5);
				//	Console.WriteLine("\tTags in Packet: {0}", TagsInPkt);
				//	Console.WriteLine("\tPacket Length: {0}", rawBytes.Count);
				//	Console.WriteLine("\tPayload Length: {0}", TagData.Count);

				// Parse CRC from response bytes
				ushort CRC = Helpers.b2us(TagData[0], TagData[1]);

				//// Parse Tag ID from response bytes
				// byte TagType = rawBytes[bPtr];	// First parse the tag length from MSByte of Tag ID
				//// TagType MSbits = 00b for ePC-96 (12 bytes), all other 2MSbits => ePC-64 (8 bytes)
				// int TagLen = ((TagType & 0xC0) == 0x00)?12:8;
				//// If, for some reason, there are not enough bytes in the pkt to fill the whole TagID,
				//// only grab the bytes that are there. (and leave one byte as the password?)

				byteList TagID = new byteList(TagData, 2, TagData.Count - 2);
				mTag = new RFIDTag(TagID, RFIDProtocol.EPCClass1);
				mTag.CRC = CRC;

				// Parse Kill Code from response bytes
				mTag.KillCode = TagData[TagData.Count - 2];

				// Parse Lock Code from response bytes
				mTag.LockCode = TagData[TagData.Count - 1];
			}
			return mTag;
		}
		#endregion

		/// <summary>
		/// Reset 
		/// </summary>
		private void KickInvTimer()
		{
			if (invTimerEnabled)
				InventoryUpdateTimer.Change(invUpdateGap, new TimeSpan(0,0,0,0,-1));
			else
				InventoryUpdateTimer.Change(Timeout.Infinite, Timeout.Infinite);
		}

		#endregion

		#region IDisposable Members

		/// <summary>
		/// Release resources:
		/// InventoryUpdateTimer and the MPRComm objects
		/// </summary>
		public void Dispose()
		{
			Disconnect();

			if (InventoryUpdateTimer != null)
				InventoryUpdateTimer.Dispose();

			if (comm != null)
				comm.Dispose();
		}

		#endregion

		#region MPRComm Event Handlers
		/// <summary>
		/// Event Handler is called when comm up status changes
		/// </summary>
		/// <param name="Flag">true if comm now open; else false.</param>
		private void comm_IsOpenChanged(bool Flag)
		{
			// Disable the inventory timer if the comm is closed
			if (!Flag)
			{
				InvTimerEnabled = false;
				// If we were connected, but now aren't, fire a DisconnectEvent.
				if (isConnected) 
				{
					isConnected = false;

					if (DisconnectEvent != null)
						DisconnectEvent(this, new EventArgs());
				}
			} 
			else // If comm is now open, just update isConnected.
				isConnected = Flag;
		}

		/// <summary>
		/// Fires when ASCII data arrives from the bootloader.
		/// Passes this data up by firing its own event, BootloaderBytesReceivedEvent.
		/// </summary>
		/// <param name="Data">Contains the string of ASCII data that arrived from the Bootloader.</param>
		private void comm_BLDataArrived(string Data)
		{
			Console.WriteLine("{0}", Data);
			if (BootloaderBytesReceivedEvent != null) 
			{
				BootloaderBytesReceivedEvent(Data);
			} 
		}
		#endregion
	}
}