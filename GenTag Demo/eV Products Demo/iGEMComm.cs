using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

using FT_HANDLE = System.UInt32;
using MP_HANDLE = System.UInt32;
using FT_STATUS = System.UInt32;
using MP_STATUS = System.UInt32;
using System.IO;
using System.Windows.Forms;
using System.Drawing;
using System.Threading;

namespace eV_Products_Demo
{
    unsafe class iGEMComm
    {
        public iGEMComm(eVDemo mF)
        {

            this.mF_Form = mF;

        }
        private eVDemo mF_Form;

        public const UInt32 FT_OK = 0;
        public const UInt32 FT_INVALID_HANDLE = 1;
        public const UInt32 FT_DEVICE_NOT_FOUND = 2;
        public const UInt32 FT_DEVICE_NOT_OPENED = 3;
        public const UInt32 FT_IO_ERROR = 4;
        public const UInt32 FT_INSUFFICIENT_RESOURCES = 5;
        public const UInt32 FT_INVALID_PARAMETER = 6;
        public const UInt32 FT_INVALID_BAUD_RATE = 7;
        public const UInt32 FT_DEVICE_NOT_OPENED_FOR_ERASE = 8;
        public const UInt32 FT_DEVICE_NOT_OPENED_FOR_WRITE = 9;
        public const UInt32 FT_FAILED_TO_WRITE_DEVICE = 10;
        public const UInt32 FT_EEPROM_READ_FAILED = 11;
        public const UInt32 FT_EEPROM_WRITE_FAILED = 12;
        public const UInt32 FT_EEPROM_ERASE_FAILED = 13;
        public const UInt32 FT_EEPROM_NOT_PRESENT = 14;
        public const UInt32 FT_EEPROM_NOT_PROGRAMMED = 15;
        public const UInt32 FT_INVALID_ARGS = 16;
        public const UInt32 FT_OTHER_ERROR = 17;

        public const UInt32 FT_LIST_NUMBER_ONLY = 0x80000000;
        public const UInt32 FT_LIST_BY_INDEX = 0x40000000;
        public const UInt32 FT_LIST_ALL = 0x20000000;
        public const UInt32 FT_OPEN_BY_SERIAL_NUMBER = 1;
        public const UInt32 FT_OPEN_BY_DESCRIPTION = 2;

        // Word Lengths
        public const byte FT_BITS_8 = 8;
        public const byte FT_BITS_7 = 7;
        public const byte FT_BITS_6 = 6;
        public const byte FT_BITS_5 = 5;

        // Stop Bits
        public const byte FT_STOP_BITS_1 = 0;
        public const byte FT_STOP_BITS_1_5 = 1;
        public const byte FT_STOP_BITS_2 = 2;

        // Parity
        public const byte FT_PARITY_NONE = 0;
        public const byte FT_PARITY_ODD = 1;
        public const byte FT_PARITY_EVEN = 2;
        public const byte FT_PARITY_MARK = 3;
        public const byte FT_PARITY_SPACE = 4;

        // Flow Control
        public const UInt16 FT_FLOW_NONE = 0;
        public const UInt16 FT_FLOW_RTS_CTS = 0x0100;
        public const UInt16 FT_FLOW_DTR_DSR = 0x0200;
        public const UInt16 FT_FLOW_XON_XOFF = 0x0400;

        // Purge rx and tx buffers
        public const byte FT_PURGE_RX = 1;
        public const byte FT_PURGE_TX = 2;

        // Events
        public const UInt32 FT_EVENT_RXCHAR = 1;
        public const UInt32 FT_EVENT_MODEM_STATUS = 2;

        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_ListDevices([MarshalAs(UnmanagedType.LPArray)]byte[] pvArg1, [MarshalAs(UnmanagedType.LPArray)]byte[] pvArg2, UInt32 dwFlags);	// FT_ListDevices by number only
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_ListDevices(UInt32 pvArg1, [MarshalAs(UnmanagedType.LPArray)]byte[] pvArg2, UInt32 dwFlags);	// FT_ListDevcies by serial number or description by index only
        [DllImport("FTD2XX.dll")]
        static extern FT_STATUS FT_Open(UInt32 uiPort, ref FT_HANDLE ftHandle);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_OpenEx([MarshalAs(UnmanagedType.LPArray)]byte[] pvArg1, UInt32 dwFlags, ref FT_HANDLE ftHandle);
        [DllImport("FTD2XX.dll")]
        static extern FT_STATUS FT_Close(FT_HANDLE ftHandle);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_Read(FT_HANDLE ftHandle, [MarshalAs(UnmanagedType.LPArray)]byte[] lpBuffer, UInt32 dwBytesToRead, ref UInt32 lpdwBytesReturned);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_Write(FT_HANDLE ftHandle, [MarshalAs(UnmanagedType.LPArray)]byte[] lpBuffer, UInt32 dwBytesToWrite, ref UInt32 lpdwBytesWritten);

        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_SetBaudRate(FT_HANDLE ftHandle, UInt32 dwBaudRate);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_SetDataCharacteristics(FT_HANDLE ftHandle, byte uWordLength, byte uStopBits, byte uParity);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_SetFlowControl(FT_HANDLE ftHandle, char usFlowControl, byte uXon, byte uXoff);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_SetDtr(FT_HANDLE ftHandle);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_ClrDtr(FT_HANDLE ftHandle);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_SetRts(FT_HANDLE ftHandle);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_ClrRts(FT_HANDLE ftHandle);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_GetModemStatus(FT_HANDLE ftHandle, ref UInt32 lpdwModemStatus);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_SetChars(FT_HANDLE ftHandle, byte uEventCh, byte uEventChEn, byte uErrorCh, byte uErrorChEn);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_Purge(FT_HANDLE ftHandle, UInt32 dwMask);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_SetTimeouts(FT_HANDLE ftHandle, UInt32 dwReadTimeout, UInt32 dwWriteTimeout);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_GetQueueStatus(FT_HANDLE ftHandle, ref UInt32 lpdwAmountInRxQueue);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_SetBreakOn(FT_HANDLE ftHandle);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_SetBreakOff(FT_HANDLE ftHandle);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_GetStatus(FT_HANDLE ftHandle, ref UInt32 lpdwAmountInRxQueue, ref UInt32 lpdwAmountInTxQueue, ref UInt32 lpdwEventStatus);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_SetEventNotification(FT_HANDLE ftHandle, UInt32 dwEventMask, [MarshalAs(UnmanagedType.LPArray)]byte[] pvArg);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_ResetDevice(FT_HANDLE ftHandle);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_SetDivisor(FT_HANDLE ftHandle, char usDivisor);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_GetLatencyTimer(FT_HANDLE ftHandle, ref byte pucTimer);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_SetLatencyTimer(FT_HANDLE ftHandle, byte ucTimer);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_GetBitMode(FT_HANDLE ftHandle, ref byte pucMode);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_SetBitMode(FT_HANDLE ftHandle, byte ucMask, byte ucEnable);
        [DllImport("FTD2XX.dll")]
        static extern unsafe FT_STATUS FT_SetUSBParameters(FT_HANDLE ftHandle, UInt32 dwInTransferSize, UInt32 dwOutTransferSize);
        /// <summary>
        /// /////////////////////////////////////////////////////////////////////////////
        /// </summary>
        /// 

        public const UInt32 MP_SUCCESS = 0; //FT_OK
        public const UInt32 MP_INVALID_HANDLE = 1; //FT_INVALID_HANDLE
        public const UInt32 MP_DEVICE_NOT_FOUND = 2; //FT_DEVICE_NOT_FOUND
        public const UInt32 MP_DEVICE_NOT_OPENED = 3; //FT_DEVICE_NOT_OPENED
        public const UInt32 MP_IO_ERROR = 4; //FT_IO_ERROR
        public const UInt32 MP_INSUFFICIENT_RESOURCES = 5; //FT_INSUFFICIENT_RESOURCES

        public const UInt32 MP_DEVICE_NAME_BUFFER_TOO_SMALL = 20;
        public const UInt32 MP_INVALID_DEVICE_NAME = 21;
        public const UInt32 MP_INVALID_DEVICE_INDEX = 22;
        public const UInt32 MP_TOO_MANY_DEVICES = 23;
        public const UInt32 MP_DEVICE_IN_USE = 24;
        public const UInt32 MP_FAILED_TO_COMPLETE_COMMAND = 25;
        public const UInt32 MP_FAILED_TO_CLEAR_DEVICE = 26;
        public const UInt32 MP_FAILED_TO_PROGRAM_DEVICE = 27;
        public const UInt32 MP_FAILED_TO_SYNCHRONIZE_DEVICE = 28;
        public const UInt32 MP_SERIAL_NUMBER_BUFFER_TOO_SMALL = 29;
        public const UInt32 MP_DLL_VERSION_BUFFER_TOO_SMALL = 30;
        public const UInt32 MP_INVALID_LANGUAGE = 31;
        public const UInt32 MP_INVALID_STATUS_CODE = 32;
        public const UInt32 MP_ERROR_MESSAGE_BUFFER_TOO_SMALL = 33;
        public const UInt32 MP_INVALID_SERIAL_NUMBER = 34;
        public const UInt32 MP_INVALID_SERIAL_NUMBER_INDEX = 35;
        public const UInt32 MP_INVALID_FILE_NAME = 36;
        public const UInt32 MP_INVALID_FILE_EXTENSION = 37;
        public const UInt32 MP_FILE_NOT_FOUND = 38;
        public const UInt32 MP_FAILED_TO_OPEN_FILE = 39;
        public const UInt32 MP_FILE_READ_ERROR = 40;
        public const UInt32 MP_FILE_CORRUPT = 41;


        [DllImport("morphprg.dll")]
        static extern unsafe MP_STATUS GetNumMorphICDevices(ref uint lpdwNumMorphICDevices);
        [DllImport("morphprg.dll")]
        static extern unsafe MP_STATUS GetMorphICDeviceName(uint dwDeviceIndex, [MarshalAs(UnmanagedType.LPArray)]byte[] lpDeviceNameBuffer, uint dwBufferSize);
        [DllImport("morphprg.dll")]
        static extern unsafe MP_STATUS GetNumMorphICDeviceSerialNumbers([MarshalAs(UnmanagedType.LPArray)]byte[] lpDeviceName, ref uint lpdwNumMorphICDeviceSerialNumbers);
        [DllImport("morphprg.dll")]
        static extern unsafe MP_STATUS GetMorphICDeviceSerialNumber([MarshalAs(UnmanagedType.LPArray)]byte[] lpDeviceName, uint dwSerialNumberIndex, [MarshalAs(UnmanagedType.LPArray)]byte[] lpSerialNumberBuffer, uint dwBufferSize);
        [DllImport("morphprg.dll")]
        static extern unsafe MP_STATUS OpenSpecifiedMorphICDevice([MarshalAs(UnmanagedType.LPArray)]byte[] lpDeviceName, [MarshalAs(UnmanagedType.LPArray)]byte[] lpSerialNumber, MP_HANDLE* pMpHandle);
        [DllImport("morphprg.dll")]
        static extern unsafe MP_STATUS OpenMorphICDevice(MP_HANDLE pMpHandle);
        [DllImport("morphprg.dll")]
        static extern unsafe MP_STATUS CloseMorphICDevice(MP_HANDLE pMpHandle);
        [DllImport("morphprg.dll")]
        static extern unsafe MP_STATUS InitMorphICDevice(MP_HANDLE pMpHandle);
        [DllImport("morphprg.dll")]
        static extern unsafe MP_STATUS ProgramMorphICDevice(MP_HANDLE pMpHandle, [MarshalAs(UnmanagedType.LPArray)]byte[] lpFileName);
        [DllImport("morphprg.dll")]
        static extern unsafe MP_STATUS IsMorphICDeviceProgrammed(MP_HANDLE pMpHandle, ref bool lpbProgramState);
        [DllImport("morphprg.dll")]
        static extern unsafe MP_STATUS GetMorphICDllVersion([MarshalAs(UnmanagedType.LPArray)]byte[] lpDllVersionBuffer, uint lpbProgramState);
        [DllImport("morphprg.dll")]
        static extern unsafe MP_STATUS GetMorphICErrorCodeString([MarshalAs(UnmanagedType.LPArray)]byte[] lpLanguage, MP_STATUS StatusCode, [MarshalAs(UnmanagedType.LPArray)]byte[] lpErrorMessageBuffer, uint dwBufferSize);

        /// <summary>
        /// //////////////////////////////////////////////
        /// </summary>
        /// 

        //[DllImport("kernel32.dll")]
        //static extern unsafe void Sleep(uint dwCount);


        MP_STATUS Status = MP_SUCCESS;
        string prDeviceName = "MORPH-IC A";
        uint dwDeviceNamesReturned = 0;
        byte[] szSerialNumber = new byte[50];
        uint dwSerialNumbersReturned = 0;
        MP_HANDLE mpHandle;
        bool bProgramState = false;
        byte[] szDllVersion = new byte[50];
        byte[] szErrorMessage = new byte[50];

        FT_HANDLE FTHandle;
        FT_STATUS FTStatus = FT_OK;
        byte[] IN_buf = new byte[4010];
        byte[] OUT_buf = new byte[8];
        byte[] prSerialNumber = new byte[50];
        uint BytesWriten = 0;
        uint BytesToRead = 0;
        uint BytesReaden = 0;
        int WordsReaden = 0;

        // Config Form
        Configure Cfg;
        //Calibration Form
        Calibration Cal;
        //FWHM Form
        FWHM fm;

        //-------------in the initialization load the hardware------------------//
        public unsafe bool LoadHW()
        {
            FileInfo fInfo = new FileInfo("335993.rbf");

            if (!fInfo.Exists)
            {
                MessageBox.Show("The 335993.rbf file was not found.Please copy the 335993.rbf to the iSpectrum directory");
                Application.Exit();
            }
            Status = GetNumMorphICDevices(ref dwDeviceNamesReturned);
            if ((Status == MP_SUCCESS) && (dwDeviceNamesReturned > 0))
            {
                Status = GetNumMorphICDeviceSerialNumbers(System.Text.Encoding.ASCII.GetBytes(prDeviceName), ref dwSerialNumbersReturned);

                if ((Status == MP_SUCCESS) && (dwSerialNumbersReturned > 0))
                {
                    if (dwSerialNumbersReturned == 1)
                    {
                        Status = GetMorphICDeviceSerialNumber(System.Text.Encoding.ASCII.GetBytes(prDeviceName), 0, szSerialNumber, 50);
                        //MessageBox.Show(System.Text.Encoding.ASCII.GetString(szSerialNumber));
                        if (Status == MP_SUCCESS)
                        {
                            fixed (MP_HANDLE* lpmpHandle = &mpHandle)
                            {
                                Status = OpenSpecifiedMorphICDevice(System.Text.Encoding.ASCII.GetBytes(prDeviceName), szSerialNumber, lpmpHandle);
                            }
                            prSerialNumber = szSerialNumber;
                            //MessageBox.Show( System.Text.Encoding.ASCII.GetString(prSerialNumber));
                            prSerialNumber[8] = (byte)66;
                            //MessageBox.Show(System.Text.Encoding.ASCII.GetString(prSerialNumber));
                        }
                    }
                    else
                    {
                        if (dwSerialNumbersReturned == 2)
                        {
                            Status = GetMorphICDeviceSerialNumber(System.Text.Encoding.ASCII.GetBytes(prDeviceName), 1, szSerialNumber, 50);
                            if (Status == MP_SUCCESS)
                            {
                                fixed (MP_HANDLE* lpmpHandle = &mpHandle)
                                {
                                    Status = OpenSpecifiedMorphICDevice(System.Text.Encoding.ASCII.GetBytes(prDeviceName), szSerialNumber, lpmpHandle);
                                }
                            }
                        }
                    }
                    if (Status == MP_SUCCESS)
                    {
                        Status = IsMorphICDeviceProgrammed(mpHandle, ref bProgramState);
                        if (Status == MP_SUCCESS)
                        {
                            Status = ProgramMorphICDevice(mpHandle, System.Text.Encoding.ASCII.GetBytes("335993.rbf"));
                            this.mF_Form.label3.Text = "iGEM Online";
                            this.mF_Form.label3.ForeColor = Color.Green;
                        }
                        else
                        {
                            // Morph-IC board is not programmed
                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (1)");
                            this.mF_Form.label3.Text = "Program FPGA Failed!";
                            this.mF_Form.label3.ForeColor = Color.Red;
                            //------------------------Shen begin----------------//
                            CloseMorphICDevice(mpHandle);
                            Application.Exit();
                            //------------------------Shen end----------------//
                        }
                        CloseMorphICDevice(mpHandle);
                    }
                    if (Status == MP_SUCCESS)
                        Status = GetMorphICDllVersion(szDllVersion, 10);
                }
                //------------------------Shen begin----------------//
                return true;
                //------------------------Shen end----------------//
            }
            else
            {
                //------------------------Shen begin----------------//
                this.mF_Form.label3.Text = "iGEM Offline!";
                this.mF_Form.label3.ForeColor = Color.Red;
                this.mF_Form.Button_Start.Enabled = false;
                this.mF_Form.Cfg.TextLLD.Enabled = false;
                this.mF_Form.Cfg.TextVB.Enabled = false;
                this.mF_Form.Cfg.cmbGain.Enabled = false;
                this.mF_Form.Cfg.cmbPeakingTime.Enabled = false;
                this.mF_Form.Cfg.CommandReset.Enabled = false;
                return false;
                //------------------------Shen end----------------//
            }

            //if (Status != MP_SUCCESS)
            //Status = GetMorphICErrorCodeString(System.Text.Encoding.ASCII.GetBytes("EN"), Status, szErrorMessage, 100);

        }
        //-------------in the initialization load the hardware End------------------//


        //------------------------Get the events form DODM-------------------------//
        //-------------------------------------------------------------------------//
        /*  step 1, Initial() open the hardware
         *  step 2, Snd4()   send command 4
         *  step 3, Retrieve3() receive the reply of command 4 according to which send cmd3
         *  setp 4,Retrievedata(ref iGEMData data) fetch the data and send cmd4 and back to to step 3
         * */

        public unsafe bool Initial()
        {
            FTStatus = FT_OpenEx(prSerialNumber, 1, ref FTHandle);
            if (FTStatus != FT_OK || FTHandle == FT_INVALID_HANDLE)
            {
                FT_Close(FTHandle);
                File.AppendAllText("iSpectrum.log", "FT Open FTStatus" + FTStatus.ToString() + Environment.NewLine);
                mF_Form.timerspectrum.Enabled = false;
                // device fails to open
                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (2)");
                return false;
            }
            FT_SetLatencyTimer(FTHandle, 2);
            FT_SetTimeouts(FTHandle, 16, 16);
            return true;
        }

        public unsafe bool Snd4()
        {
            FTStatus = FT_Purge(FTHandle, 3);
            if (FTStatus != FT_OK)
            {
                FTStatus = FT_Purge(FTHandle, 3);
                if (FTStatus != FT_OK)
                {
                    FT_Close(FTHandle);
                    File.AppendAllText("iSpectrum.log", "!FT Purge FTStatus" + FTStatus.ToString() + Environment.NewLine);
                    mF_Form.timerspectrum.Enabled = false;
                    // clear FIFO command failed
                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (3)");
                    return false;
                }
            }
            //sendcmd(4, 0, 0);
            OUT_buf[0] = (byte)255;
            OUT_buf[1] = (byte)255;
            OUT_buf[2] = 4;
            OUT_buf[3] = 0;
            OUT_buf[4] = 0;

            FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
            if (FTStatus != FT_OK)
            {
                FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                if (FTStatus != FT_OK)
                {
                    FT_Close(FTHandle);
                    File.AppendAllText("iSpectrum.log", "!FT Write FTStatus" + FTStatus.ToString() + Environment.NewLine);
                    mF_Form.timerspectrum.Enabled = false;
                    // write command (get n events) failed with bad status returned 
                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (4)");
                    return false;
                }
            }

            if (BytesWriten != 5)
            {
                FT_Close(FTHandle);
                File.AppendAllText("iSpectrum.log", "!FT Write Bytewrittedn" + BytesWriten.ToString() + Environment.NewLine);
                mF_Form.timerspectrum.Enabled = false;
                // write command (get n events) failed with byte written incorrect
                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (5)");
                return false;
            }
            return true;
        }

        public unsafe bool Snd5()
        {
            FTStatus = FT_Purge(FTHandle, 3);
            if (FTStatus != FT_OK)
            {
                FTStatus = FT_Purge(FTHandle, 3);
                if (FTStatus != FT_OK)
                {
                    FT_Close(FTHandle);
                    File.AppendAllText("iSpectrum.log", "!FT Purge FTStatus" + FTStatus.ToString() + Environment.NewLine);
                    mF_Form.timerspectrum.Enabled = false;
                    // command to purge DLL buffer failed
                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (6)");
                    return false;
                }
            }
            //sendcmd(5, 0, 0);
            OUT_buf[0] = (byte)255;
            OUT_buf[1] = (byte)255;
            OUT_buf[2] = 5;
            OUT_buf[3] = 0;
            OUT_buf[4] = 0;

            FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
            if (FTStatus != FT_OK)
            {
                FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                if (FTStatus != FT_OK)
                {
                    FT_Close(FTHandle);
                    File.AppendAllText("iSpectrum.log", "!FT Write FTStatus" + FTStatus.ToString() + Environment.NewLine);
                    mF_Form.timerspectrum.Enabled = false;
                    // write command (clear FIFO) failed with bad status returned 
                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (7)");
                    return false;
                }
            }

            if (BytesWriten != 5)
            {
                FT_Close(FTHandle);
                File.AppendAllText("iSpectrum.log", "!FT Write Bytewrittedn" + BytesWriten.ToString() + Environment.NewLine);
                mF_Form.timerspectrum.Enabled = false;
                // write command (clear FIFO) failed with byte written incorrect
                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (8)");
                return false;
            }

            while (BytesToRead < 2)
            {
                FTStatus = FT_GetQueueStatus(FTHandle, ref BytesToRead);
                if (FTStatus != FT_OK)
                {
                    FTStatus = FT_GetQueueStatus(FTHandle, ref BytesToRead);
                    if (FTStatus != FT_OK)
                    {
                        FT_Close(FTHandle);
                        mF_Form.timerspectrum.Enabled = false;
                        // get queue status failed on return status when waiting for clear FIFO reply 
                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (9)");
                        return false;
                    }
                }
            }
            return true;
        }

        


        public unsafe int Retrieve3()
        {
            uint operand, res1, res2, left;
            uint loopCount;
            bool jump;
            loopCount = 0;
            BytesToRead = 0;
            // Get the response of Cmd4
            // Cmd4 reply Ready or not, there is a timeout for cmd4 so won't be dead-circle

            while (BytesToRead < 2)
            {
                if (mF_Form.collecting == true)// in case drop in endless circle
                {

                    FTStatus = FT_GetQueueStatus(FTHandle, ref BytesToRead);
                    if (FTStatus != FT_OK)
                    {
                        FT_Close(FTHandle);
                        mF_Form.timerspectrum.Enabled = false;
                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (10)");
                        return 0;
                        //continue;
                    }
                    //Application.DoEvents();
                }
                else
                {
                    FT_Close(FTHandle);// stop the acquiring!
                    return 0;
                }
                loopCount += 1;
                if (loopCount > 1000)
                {
                    LoadHW();
                    BytesToRead = 2;
                    File.AppendAllText("iSpectrum.log", "Loop count 1000 reached" + Environment.NewLine);
                }
            }


            // read the response of CMD4

       

            FTStatus = FT_Read(FTHandle, IN_buf, BytesToRead, ref BytesReaden);
            if (FTStatus != FT_OK)
            {
                FTStatus = FT_Read(FTHandle, IN_buf, BytesToRead, ref BytesReaden);
                if (FTStatus != FT_OK)
                {
                    FT_Close(FTHandle);
                    File.AppendAllText("iSpectrum.log", "!Read Reply of Cmd4 Error" + FTStatus.ToString() + Environment.NewLine);
                    mF_Form.timerspectrum.Enabled = false;
                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (11)");
                    return 0;
                    //continue;
                }
            }

            res1 = (uint)IN_buf[0];
            res2 = (uint)IN_buf[1];
            if (loopCount > 100)
                this.mF_Form.rcvcount = 0;
            else
                this.mF_Form.rcvcount = res2 * 256 + res1;
            // out of valid range
            if (this.mF_Form.rcvcount > 2001 || this.mF_Form.rcvcount == 0)
            {
                // skip and send cmd4 again
                uint ii = this.mF_Form.rcvcount;
                // File.AppendAllText("iSpectrum.log", "!events number Error:" + ii.ToString() + " time:" + mF_Form.Label_Time.Text + " Counts: " + mF_Form.Label_TC.Text + Environment.NewLine);
                return 1; // return to the step 1
            }
            // fetch 185 each time
            operand = this.mF_Form.rcvcount / 185;
            left = this.mF_Form.rcvcount % 185;
            if (left > 0)
            {
                jump = false;
                left--;
            }
            else
            {
                jump = true;
            }
            OUT_buf[0] = (byte)255;
            OUT_buf[1] = (byte)255;
            OUT_buf[2] = 3;
            OUT_buf[3] = 0;
            OUT_buf[4] = 184;

            for (int t = 0; t < operand; t++)
            {
                FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                if (FTStatus != FT_OK)
                {
                    FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                    if (FTStatus != FT_OK)
                    {
                        FT_Close(FTHandle);
                        File.AppendAllText("iSpectrum.log", "!Retrieve Event Error!: FTStatus=" + FTStatus.ToString() + " FTHandle=" + FTHandle.ToString() + " OUT_buf=" + OUT_buf.ToString() + "BytesWriten=" + BytesWriten.ToString() + Environment.NewLine);
                        mF_Form.timerspectrum.Enabled = false;
                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (12)");
                        return 0;
                        //continue;
                    }
                }

                if (BytesWriten != 5)
                {
                    // send cmd4 again
                    //Nextstep = 1;
                    //timerCollect.Enabled = true;
                    File.AppendAllText("iSpectrum.log", "!Retrieve Event Error!:BytesWriten " + BytesWriten.ToString() + Environment.NewLine);
                    return 1;
                }

            }

            if (jump == false)
            {
                OUT_buf[4] = (byte)left;

                FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                if (FTStatus != FT_OK)
                {
                    FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                    if (FTStatus != FT_OK)
                    {
                        LoadHW();
                        File.AppendAllText("iSpectrum.log", "Retrieve Last Event Error!:FTStatus " + FTStatus.ToString() + ", LoadHW executed" + Environment.NewLine);
                        FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                        if (FTStatus != FT_OK)
                        {
                            FT_Close(FTHandle);
                            File.AppendAllText("iSpectrum.log", "Retrieve Last Event Error!:FTStatus " + FTStatus.ToString() + Environment.NewLine);
                            mF_Form.timerspectrum.Enabled = false;
                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (13)");
                            return 0;
                            //continue;
                        }
                    }
                }

                if (BytesWriten != 5)
                {
                    // send cmd4 again
                    //Nextstep = 1;
                    //timerCollect.Enabled = true;
                    File.AppendAllText("iSpectrum.log", "Retrieve Last Event Error!:BytesWriten " + BytesWriten.ToString() + Environment.NewLine);
                    return 1;// begin from step 1
                }
            }
            //Nextstep = 3;
            //timerCollect.Enabled = true;
            return 3;// begin from step 3

        }

        public int ltime;

        public unsafe int Retrievedata(ref iGEMData data)
        {
            int res1, res2, respon;
            BytesToRead = 0;
            int l = 0;// for testing 
            mF_Form.ltime++;

            // Cmd3 reply ready or not, repeat 20000 times
            while ((BytesToRead) / 2 < mF_Form.rcvcount && l < 20000)
            {
                if (mF_Form.collecting == true)
                {
                    FTStatus = FT_GetQueueStatus(FTHandle, ref BytesToRead);
                    if (FTStatus != FT_OK)
                    {
                        FT_Close(FTHandle);
                        File.AppendAllText("iSpectrum.log", "Retrievedata!:FTStatus " + FTStatus.ToString() + Environment.NewLine);
                        mF_Form.timerspectrum.Enabled = false;
                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (14)");
                        return 0;
                        //continue;
                    }
                }
                else
                {
                    FT_Close(FTHandle);

                    return 0;// stop acquiring
                }
                l++;
            }
            //if (l > 5)
            //{
            //   uint count = mF_Form.rcvcount;
            //   mF_Form.label3.Text = "l:" + l.ToString() + " ToRead:" + BytesToRead.ToString() + " " + count.ToString();
            //}
            //---------------------- To increase efficiency, send cmd 4 here-----------//
            OUT_buf[0] = (byte)255;
            OUT_buf[1] = (byte)255;
            OUT_buf[2] = 4;
            OUT_buf[3] = 0;
            OUT_buf[4] = 0;

            FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
            if (FTStatus != FT_OK)
            {
                FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                if (FTStatus != FT_OK)
                {
                    LoadHW();
                    File.AppendAllText("iSpectrum.log", "Cmd 4 FT_Write failed" + FTStatus.ToString() + ", loadHW executed" + Environment.NewLine);
                    FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                    if (FTStatus != FT_OK)
                    {
                        FT_Close(FTHandle);
                        mF_Form.timerspectrum.Enabled = false;
                        File.AppendAllText("iSpectrum.log", "Cmd 4 FT_Write FTStatus=" + FTStatus.ToString() + Environment.NewLine);
                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (15)");
                        return 0;
                    }
                }
            }

            if (BytesWriten != 5)
            {
                FT_Close(FTHandle);
                mF_Form.timerspectrum.Enabled = false;
                File.AppendAllText("iSpectrum.log", "Command 4 FT_Write ByteWriten=" + BytesWriten.ToString() + Environment.NewLine);
                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (16)");
                return 0;

            }
            //--------------------------------------------------- send cmd4 end---------------------------//// fetch the real data
            FTStatus = FT_Read(FTHandle, IN_buf, BytesToRead, ref BytesReaden);
            if (FTStatus != FT_OK)
            {
                FT_Close(FTHandle);
                File.AppendAllText("iSpectrum.log", "FT_Read:FTStatus " + FTStatus.ToString() + Environment.NewLine);
                mF_Form.timerspectrum.Enabled = false;
                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (17)");
                return 0;
                //continue;
            }
            else
            {
                //log the minor error
                if (BytesReaden != BytesToRead)
                    File.AppendAllText("iSpectrum.log", "BytesReaden: " + BytesReaden.ToString() + " BytesToRead: " + BytesToRead.ToString() + Environment.NewLine);
                WordsReaden = (int)BytesReaden >> 1;
                //                    mF_Form.totalcounts += WordsReaden;
                fixed (byte* pIN_Buf = IN_buf)
                {
                    byte* ps = pIN_Buf;
                    for (int i = 0; i < WordsReaden; i++)
                    {
                        res1 = (int)(*ps);
                        ps++;
                        res2 = (int)(*ps);
                        ps++;
                        respon = (res2 << 8) + res1;

                        if (respon == 65535)
                        {
                            i = (int)BytesReaden;
                            //File.AppendAllText("iSpectrum.log", "Response=65535 " + " time:" + mF_Form.Label_Time + " Counts: " + mF_Form.Label_TC.Text + Environment.NewLine);
                        }
                        if (respon < 4096) //just for test
                        {
                            data.dCounts[respon]++;
                        }
                    }
                }
            }
            return 1; // restart from step 1
        }

        //Close the Hardware
        public unsafe bool Cls()
        {
            FTStatus = FT_Close(FTHandle);
            if (FTStatus != FT_OK)
                return false;
            return true;
        }

        public unsafe bool SndRcv(byte cmd, ref byte p1, ref byte p2)
        {
            int wrong = 0;
            FTStatus = FT_OpenEx(prSerialNumber, 1, ref FTHandle);
            if (FTStatus != FT_OK || FTHandle == FT_INVALID_HANDLE)
            {
                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (18) CMD=" + cmd.ToString());
                wrong = 1;
            }
            else
            {
                FTStatus = FT_Purge(FTHandle, 3);
                if (FTStatus != FT_OK)
                {
                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (19) CMD=" + cmd.ToString());
                    wrong = 1;
                }
                else
                {
                    FT_SetTimeouts(FTHandle, 100, 100);
                    OUT_buf[0] = (byte)255;
                    OUT_buf[1] = (byte)255;
                    OUT_buf[2] = cmd;
                    OUT_buf[3] = p1;
                    OUT_buf[4] = p2;
                    //File.AppendAllText("iSpectrum.log", "Write CMD" + cmd.ToString() + " , " + p1.ToString() + " '" + p2.ToString() + Environment.NewLine);

                    FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                    if (FTStatus != FT_OK)
                    {
                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (20) CMD=" + cmd.ToString());
                        wrong = 1;
                    }
                    else
                    {
                        if (BytesWriten == 5)
                        {
                            //this.mF_Form.label3.Text = "Command send OK!";
                            int wait = 0;
                            do
                            {
                                wait++;
                                Thread.Sleep(2);
                                FTStatus = FT_GetQueueStatus(FTHandle, ref BytesToRead);
                                if (FTStatus != FT_OK)
                                {
                                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (21) CMD=" + cmd.ToString());
                                    wrong = 1;
                                    wait = 10;
                                }
                            } while (BytesToRead < 2 && wait < 10);
                            if (FTStatus != FT_OK)
                            {
                                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (22) CMD=" + cmd.ToString());
                                wrong = 1;
                            }
                            else
                            {
                                if (BytesToRead >= 2)
                                {
                                    //MessageBox.Show(BytesToRead.ToString());
                                    FTStatus = FT_Read(FTHandle, IN_buf, 2, ref BytesReaden);
                                    if (FTStatus != FT_OK)
                                    {
                                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (23) CMD=" + cmd.ToString());
                                        wrong = 1;
                                    }
                                    else
                                    {
                                        if (BytesReaden != 2)
                                        {
                                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (24) CMD=" + cmd.ToString());
                                            wrong = 1;
                                        }
                                        else
                                        {
                                            p1 = IN_buf[0];
                                            p2 = IN_buf[1];
                                            File.AppendAllText("iSpectrum.log", "Read " + cmd.ToString() + " , " + p1.ToString() + " '" + p2.ToString() + Environment.NewLine);
                                        }
                                    }
                                }
                                else
                                {
                                    File.AppendAllText("iSpectrum.log", "Write CMD Byte write <2 FT status" + FTStatus.ToString() + Environment.NewLine);// add 2007-2-2
                                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (25) CMD=" + cmd.ToString());
                                    wrong = 1;
                                }
                            }
                        }
                        else
                        {
                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (26) CMD=" + cmd.ToString());
                            File.AppendAllText("iSpectrum.log", "No.3 Write Error! Please resend it!" + Environment.NewLine);
                            wrong = 1;
                        }
                    }
                }
            }
            FT_Close(FTHandle);
            if (wrong == 1)
                return false;
            return true;
        }

        public unsafe bool SndRcv4(byte cmd, ref byte p1, ref byte p2)
        {
            FTStatus = FT_OpenEx(prSerialNumber, 1, ref FTHandle);
            int wrong = 0;
            if (FTStatus != FT_OK || FTHandle == FT_INVALID_HANDLE)
            {
                mF_Form.stop();
                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (27) CMD=" + cmd.ToString());
                wrong = 1;
            }
            else
            {
                FTStatus = FT_Purge(FTHandle, 3);
                if (FTStatus != FT_OK)
                {
                    mF_Form.stop();
                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (28) CMD=" + cmd.ToString());
                    wrong = 1;
                }
                else
                {
                    FT_SetTimeouts(FTHandle, 100, 100);
                    OUT_buf[0] = (byte)255;
                    OUT_buf[1] = (byte)255;
                    OUT_buf[2] = cmd;
                    OUT_buf[3] = p1;
                    OUT_buf[4] = p2;
                    //File.AppendAllText("iSpectrum.log", "Write CMD" + cmd.ToString() + " , " + p1.ToString() + " '" + p2.ToString() + Environment.NewLine);

                    FTStatus = FT_Write(FTHandle, OUT_buf, 5, ref BytesWriten);
                    if (FTStatus != FT_OK)
                    {
                        mF_Form.stop();
                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (29) CMD=" + cmd.ToString());
                        wrong = 1;
                    }
                    else
                    {
                        if (BytesWriten == 5)
                        {
                            //this.mF_Form.label3.Text = "Command send OK!";
                            int wait = 0;
                            do
                            {
                                wait++;
                                Thread.Sleep(2);
                                FTStatus = FT_GetQueueStatus(FTHandle, ref BytesToRead);
                                if (FTStatus != FT_OK)
                                {
                                    mF_Form.stop();
                                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (30) CMD=" + cmd.ToString());
                                    return false;
                                }
                            } while (BytesToRead < 2 && wait < 10);
                            if (FTStatus != FT_OK)
                            {
                                mF_Form.stop();
                                MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (31) CMD=" + cmd.ToString());
                                return false;
                            }
                            else
                            {
                                if (BytesToRead >= 2)
                                {
                                    FTStatus = FT_Read(FTHandle, IN_buf, 2, ref BytesReaden);
                                    if (FTStatus != FT_OK)
                                    {
                                        mF_Form.stop();
                                        MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (33) CMD=" + cmd.ToString());
                                        wrong = 1;
                                    }
                                    else
                                    {
                                        if (BytesReaden != 2)
                                        {
                                            mF_Form.stop();
                                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (34) CMD=" + cmd.ToString());
                                            wrong = 1;
                                        }
                                        else
                                        {
                                            p1 = IN_buf[0];
                                            p2 = IN_buf[1];
                                            File.AppendAllText("iSpectrum.log", "Read " + cmd.ToString() + " , " + p1.ToString() + " '" + p2.ToString() + Environment.NewLine);
                                        }
                                    }
                                }
                                else
                                {
                                    mF_Form.stop();
                                    File.AppendAllText("iSpectrum.log", "Write CMD4 Byte write <2 FT status" + FTStatus.ToString() + Environment.NewLine);// add 2007-2-2
                                    MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (35) CMD=" + cmd.ToString());
                                    wrong = 1;
                                }
                            }
                        }
                        else
                        {
                            mF_Form.stop();
                            MessageBox.Show("Remove and insert USB cable to iGEM and restart iSpectrum program (36) CMD=" + cmd.ToString());
                            File.AppendAllText("iSpectrum.log", "No.3 Write Error! Please resend it!" + Environment.NewLine);
                            wrong = 1;
                        }
                    }
                }
            }
            FT_Close(FTHandle);
            if (wrong == 1)
                return false;
            return true;
        }
    }
}

