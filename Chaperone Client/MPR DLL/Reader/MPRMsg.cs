//==========================================================================================
//
//	WJ.MPR.Reader.MPRMsg
//	Copyright (c) 2005, WJ Communications, Inc.
//
//	This file defines the MPRMsg class.
//	It also contains useful enums (Status, Opcodes, Errors, etc.)
//
//==========================================================================================
using System;
using System.Threading;
using System.Collections;
using WJ.MPR.Util;

namespace WJ.MPR.Reader
{

	#region public message enums
	/// <summary>
	/// Message Command Opcodes
	/// </summary>
	public enum CmdCode : byte
	{
		/// <summary>
		/// 
		/// </summary>
		unknown = 0x00,

		/// <summary>
		/// Requests the MPR Manufacturing Information
		/// </summary>
		ReaderInfo = 0x01,

		/// <summary>
		/// Requests the Class 0 Inventory.
		/// </summary>
		Class0Read = 0x11,
		/// <summary>
		/// Kill Class 0 Tags.
		/// </summary>
		Class0Kill = 0x12,
		/// <summary>
		/// Command to access Class0 Zuma subcommands.
		/// <see cref="Class0ZumaSubcommands"/>
		/// </summary>
		Class0ZumaCommands = 0x18,
		/// <summary>
		/// Read Class 0+ (Matrics) Tag Data.
		/// </summary>
		Class0PlusRead = 0x1c,
		/// <summary>
		/// Write Class 0+ (Matrics) Tag Data.
		/// </summary>
		Class0PlusWrite = 0x1d,

		/// <summary>
		/// Requests the Class 1 Inventory.
		/// </summary>
		Class1Read = 0x21,
		/// <summary>
		/// Kill Class 1 Tags.
		/// </summary>
		Class1Kill = 0x22,
		/// <summary>
		/// A Class1 Programming Command.
		/// Write 16 bits to a Class 1 Tag Data.
		/// </summary>
		Class1Write = 0x23,
		/// <summary>
		/// A Class1 Programming Command.
		/// Verify a Class1 Tag.
		/// Reads and returns all data on tag, EPC and CRC.
		/// </summary>
		Class1IDVerify = 0x24,
		/// <summary>
		/// A Class1 Programming Command.
		/// Erase Tags.
		/// </summary>
		Class1IDErase = 0x25,

		/// <summary>
		/// Read and return the current values of all ADCs.
		/// </summary>
		ReadADCs = 0x83,
	}


	/// <summary>
	/// These are the subcommands for Impinj Zuma (Re-writeable Class 0), command 0x18
	/// </summary>
	public enum Class0ZumaSubcommands : byte
	{
		/// <summary>
		/// Write a Row (18 bits) to a Zuma Tag.
		/// </summary>
		WriteRow = 0x00,
		/// <summary>
		/// Read a Row (18 bits) from a Zuma Tag.
		/// </summary>
		ReadRow = 0x01,
		/// <summary>
		/// Initialize a Zuma Tag for re-writing.
		/// This RF-interface command sets the Fab Protect row to Golden Word.
		/// </summary>
		INIT = 0x02,
		/// <summary>
		/// Write a Row (18 bits) to a Zuma Tag, and wait for an acknowledgement.
		/// Singulated-only command.
		/// </summary>
		WriteAck = 0x03
	}

	/// <summary>
	/// Command Response Status Codes.
	/// These are the Status bytes from Response Frames (or Packets).
	/// </summary>
	public enum StatusCode : byte
	{
		/// <summary>
		/// MPR reports an Error.
		/// </summary>
		Error = 0xff,
		/// <summary>
		/// MPR complete the request.
		/// This is the last response frame.
		/// </summary>
		Complete = 0x00,
		/// <summary>
		/// This is not the last response frame;
		/// More frames will follow.
		/// </summary>
		InProgress = 0x01,
		/// <summary>
		/// Status Code could not be determined from a response frame.
		/// </summary>
		unknown
	}

	/// <summary>
	/// Command Response Error Codes.
	/// These Error Codes follow the Status Byte in Frames
	/// with a Status Byte = 0xff (StatusCode.Error)
	/// <see cref="StatusCode"/>
	/// </summary>
	public enum ErrorCode : byte
	{
		/// <summary>
		/// No error reported.  
		/// This is not received from the MPR, but inferred for Frames with non-Error statuses.
		/// </summary>
		NoError,
		/// <summary>
		/// MPR Current Sense ADC reports out-of-range condition.
		/// </summary>
		CurrentOutOfRange = 0xDD,
		/// <summary>
		/// MPR Voltage Sense ADC reports out-of-range condition.
		/// </summary>
		VoltageOutOfRange = 0xDE,
		/// <summary>
		/// MPR Could not verify the CRC of the Application Header.
		/// </summary>
		AppHeaderAccessError = 0xDF,
		/// <summary>
		/// Error Programming page (EPC Class 0+ tags only)
		/// </summary>
		Class0PlusErrorErasingPage = 0xE0,
		/// <summary>
		/// Error Erasing page (EPC Class 0+ tags only)
		/// </summary>
		Class0PlusErrorProgrammingPage = 0xE1,
		/// <summary>
		/// Error Programming Traversal Inhibit Bit (EPC Class 0+ tags only)
		/// </summary>
		Class0PlusErrorProgrammingTIB = 0xE2,
		/// <summary>
		/// Error Locking page (EPC Class 0+ tags only)
		/// </summary>
		Class0PlusErrorLockingPage = 0xE3,
		/// <summary>
		/// Page cannot be accessed because it is already locked (EPC Class 0+ tags only)
		/// </summary>
		Class0PlusErrorPageLocked = 0xE4,
		/// <summary>
		/// A parameter is out of range for the issued command.
		/// </summary>
		InvalidParams = 0xF0,
		/// <summary>
		/// Based on the command issued and the message length, there is not enough data in the message to support the command.
		/// </summary>
		InsufficientData = 0xF1,
		/// <summary>
		/// The command is not supported or is unable to be processed by the Reader at this time.
		/// </summary>
		UnsupportedCommand = 0xF2,
		/// <summary>
		/// A command has been issued with an unsupported power level.
		/// </summary>
		UnsupportedPowerLevel = 0xF3,
		/// <summary>
		/// On commanding the transmit frequency, the PLL which generates the RF up conversion failed to converge on the required frequency.
		/// </summary>
		PLLLockFail = 0xF4,
		/// <summary>
		/// The antenna was either not present or shorted.
		/// </summary>
		AntennaFault = 0xF5,
		/// <summary>
		/// The subcommand issued is not one of the subcommands that can be processed by the reader.
		/// </summary>
		UnsupportedSubcommand = 0xF6,
		/// <summary>
		/// A subcommand parameter is out of range for the issued subcommand.
		/// </summary>
		InvalidSubCommandParameter = 0xF7,
		/// <summary>
		/// The Non-Volatile Storage table CRC is invalid.
		/// </summary>
		InvalidNVSTable = 0xF8,
		/// <summary>
		/// The NVS address is not a word address.
		/// </summary>
		NVSAlignError = 0xF9,
		/// <summary>
		/// An attempt was made to program(write) or erase a protect address in the flash.
		/// </summary>
		NVSProtectionViolation = 0xFA,
		/// <summary>
		/// Indicates an illegal access to the flash during a program(write) or erase commands.
		/// </summary>
		NVSAccessError = 0xFB,
		/// <summary>
		/// Indicates an unsuccessful write sequence; data written does not match data read back from flash.
		/// </summary>
		NVSWriteVerifyFail = 0xFC,
		/// <summary>
		/// The Passcode to enter Maintenance Mode was incorrect.
		/// </summary>
		IncorrectPasscode = 0xFD,
		/// <summary>
		/// The command entered is not supported in the current mode, e.g., maintenance mode commands are only valid while in maintenance mode.
		/// </summary>
		InvalidCommand = 0xFE,
		/// <summary>
		/// Some other, unspecified error occured.
		/// </summary>
		UndefinedError = 0xFF
	}

	/// <summary>
	/// Indicates the possible error sources for an MPRMsg.
	/// </summary>
	public enum RspStatus
	{
		/// <summary>
		/// The MPRMsg has no status (yet).
		/// </summary>
		NoStatus,
		/// <summary>
		/// No error associated with an MPRMsg.
		/// </summary>
		OK,
		/// <summary>
		/// An error reported in the status of a response frame.
		/// </summary>
		ErrorRsp,
		/// <summary>
		/// An error occured and was reported by the communication layer (serial port).
		/// </summary>
		ErrorComm,
		/// <summary>
		/// A timeout occured = no final response frame received for the MPRMsg.
		/// </summary>
		TimeOut,
		/// <summary>
		/// A message was recevied, but has not been processed yet.
		/// </summary>
		MsgRcvd
	}

	#endregion

	/// <summary>
	/// The Message class bundles a host-to-reader request and the reader-to-host responses
	/// for a single command exchange.
	///		- The Request Frame is stored as an MPRFrame, txFrame.
	///		- The Response Frames are stored as an MPRFrameList, rxFrames.
	///		- Stores various forms of Status Information
	///			- MPR API status or errors
	///			- Serial Communications errors
	///			- Message Timeouts
	///		- Does the work of waiting for a single "Complete" Request Frame to arrive.
	///		- The .Receive method is where the thread blocks while waiting for the serial port
	///			thread to wake it up via one of three methods:
	///			1) .AddRxFrame()
	///			2) .CommError()
	///			3) .Timeout()
	///	
	/// </summary>
	public class MPRMsg
	{
		// Host-to-Reader "Request" Frame
		private MPRFrame txFrame;
		/// <summary>
		/// The Request Frame.
		/// </summary>
		public MPRFrame TxFrame { get { return txFrame; } }
		
		// Reader-to-Host "Response" Frames
		private MPRFrameList rxFrames = new MPRFrameList();
		/// <summary>
		/// A list of Response Frames.
		/// A single response could be composed of multiple "InProgress" Frames,
		/// but always a single "Complete" Frame.
		/// </summary>
		public MPRFrameList RxFrames { get { return rxFrames; } }

		private ErrorCode error = ErrorCode.NoError;
		/// <summary>
		/// Any ErrorCode associated with this MPRMsg.
		/// If none, then ErrorCode.NoError.
		/// ErrorCodes are returned when the Status Byte
		/// of a response Frame = StatusCode.Error (0xFF).
		/// </summary>
		public ErrorCode Error { get { return error; } }

		private RspStatus stat = RspStatus.NoStatus;
		/// <summary>
		/// The Response Status of this MPRMsg:
		/// NoStatus, MsgRcvd, OK, or the source of an error condition.
		/// <see cref="RspStatus"/>
		/// </summary>
		public RspStatus Status { get { return stat; } }

		private byteList databytes = new byteList();
		/// <summary>
		/// The accumulated bytes of all Response Frames.
		/// </summary>
		public byteList DataBytes { get { return databytes; } }

		/// <summary>
		/// A synchronization variable for signalling the end of
		/// response frame reception, either due to an error,
		/// a timeout, or the correct number of bytes received.
		/// </summary>
		public AutoResetEvent MsgRcvd = new AutoResetEvent(false);

		static private uint msgcnt = 0;

		private uint msgID = msgcnt++;
		/// <summary>
		/// A static running total of all MPRMsgs sent since the first 
		/// MPRMsg was instantiated.
		/// </summary>
		public uint MsgID { get { return msgID; } }

		/// <summary>
		/// Construct a new MPRMsg with the given command and parameters.
		/// </summary>
		/// <param name="command">The command for the Request Frame.</param>
		/// <param name="parameters">The parameters of the Request Frame.</param>
		public MPRMsg(CmdCode command, byteList parameters)
		{
			txFrame = new MPRFrame(command, parameters);
		}

		/// <summary>
		/// Called by MPRComm when a response frame has been received.
		/// </summary>
		/// <param name="aFrame">A response packet that has arrived successfully.</param>
		public void AddRxFrame(MPRFrame aFrame) 
		{
			lock (this) 
			{
				rxFrames.Add(aFrame);
				stat = RspStatus.MsgRcvd;
				MsgRcvd.Set();	// Signal the end of a response frame reception.
			}
		}

		/// <summary>
		/// A string describing any communication errors that may have occured.
		/// </summary>
		public string commErrorString = string.Empty;

		/// <summary>
		/// called by MPRComm when a comm error occurs
		/// </summary>
		public void CommError(string Description)
		{
			lock(this) 
			{
				commErrorString = Description;

				stat = RspStatus.ErrorComm;
				MsgRcvd.Set();	// Signal the end of a response frame reception.
			}
		}

		/// <summary>
		/// called by MPRComm when a message times out
		/// </summary>
		public void Timeout()
		{
			lock (this) 
			{
				stat = RspStatus.TimeOut;
				MsgRcvd.Set();	// Signal the end of a response frame reception.
			}
		}

		/// <summary>
		/// Response Frame Receiver.
		/// Waits on an AutoResetEvent until the end of a response frame reception.
		///		- This can be either successfull completion of a frame.
		///		- A Timeout while waiting for a response that never arrives.
		///		- A communications error.
		///	If not an error,
		///	Process each received frame:
		///		if CRC errors return RspStatus.ErrorComm
		///		if StatusCode.Complete
		///			Add rxFrame.Data to databytes
		///			return RspStatus.OK
		///		if StatusCode.Error
		///			record error
		///			return RspStatus.ErrorRsp
		///		if StatusCode.InProgress
		///			Add rxFrame.Data to databytes
		///			and keep looking for more frames. 
		/// </summary>
		/// <returns></returns>
		public RspStatus Receive()
		{
			try
			{
				bool MsgDone = false;
				while (!MsgDone) 
				{
					if (MsgRcvd.WaitOne()) 
					{ 
						if (stat == RspStatus.MsgRcvd)
						{
							lock (this) 
								foreach(MPRFrame rxFrame in rxFrames) 
									if (!rxFrame.Processed) 
									{
										rxFrame.Processed = true;

										if (!rxFrame.CRCValid)
										{
											CommError("CRC Failed.");
											return RspStatus.ErrorComm;
										}

										switch (rxFrame.Status)
										{
											case StatusCode.Complete:
												MsgDone = true;
												databytes.Add(rxFrame.Data);
												stat = RspStatus.OK;
												break;

											case StatusCode.Error:
												if ((rxFrame.Data != null) && (rxFrame.Data.Count > 0))
													error = (ErrorCode)rxFrame.Data[0];
												return RspStatus.ErrorRsp;

											case StatusCode.InProgress:
												databytes.Add(rxFrame.Data);
												break;

											default:	// Any other CommType => Abort Msg
												MsgDone = true;
												return RspStatus.ErrorComm;
										}
									}
						}
						else	// stat != RspStatus.MsgRcvd
						{
							return stat;
						}
					}
					else // MsgRcvd.WaitOne() == false
					{
						return RspStatus.TimeOut;
					}
				}
			}
			catch (ApplicationException ex)
			{
				CommError(ex.Message);
				return RspStatus.ErrorComm;
			}

			return RspStatus.OK;
		}
	}
}