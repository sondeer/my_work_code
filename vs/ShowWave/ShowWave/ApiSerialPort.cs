using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace ShowWave
{
    class ApiSerialPort
    {
        public string Port = "COM1";
        public string CommSetting = "115200,N,8,1";
        public int ReadTimeout = 200;
        public bool Opened = false;
        private int hComm = -1;
        private const string DLLPATH = "kernel32.dll"; // "kernel32";
        private const uint GENERIC_READ = 0x80000000;
        private const uint GENERIC_WRITE = 0x40000000;
        private const int OPEN_EXISTING = 3;
        private const int INVALID_HANDLE_VALUE = -1;
        private const int PURGE_RXABORT = 0x2;
        private const int PURGE_RXCLEAR = 0x8;
        private const int PURGE_TXABORT = 0x1;
        private const int PURGE_TXCLEAR = 0x4;

        [StructLayout(LayoutKind.Sequential)]
        public struct DCB

        {
            public int DCBlength;
            public int BaudRate;
            public uint flags;
            public ushort wReserved;
            public ushort XonLim;
            public ushort XoffLim;
            public byte ByteSize;
            public byte Parity;
            public byte StopBits;
            public byte XonChar;
            public byte XoffChar;
            public byte ErrorChar;
            public byte EofChar;
            public byte EvtChar;
            public ushort wReserved1;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct COMMTIMEOUTS
        {
            public int ReadIntervalTimeout;
            public int ReadTotalTimeoutMultiplier;
            public int ReadTotalTimeoutConstant;
            public int WriteTotalTimeoutMultiplier;
            public int WriteTotalTimeoutConstant;
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct OVERLAPPED
        {
            public int Internal;
            public int InternalHigh;
            public int Offset;
            public int OffsetHigh;
            public int hEvent;
        }

        private struct COMSTAT
        {
            internal const uint fCtsHold = 0x1;
            internal const uint fDsrHold = 0x2;
            internal const uint fRlsdHold = 0x4;
            internal const uint fXoffHold = 0x8;
            internal const uint fXoffSent = 0x10;
            internal const uint fEof = 0x20;
            internal const uint fTxim = 0x40;
            internal UInt32 Flags;
            public uint cbInQue;
            public uint cbOutQue;
        }

        [DllImport("Kernel32.dll")]
        private static extern int CreateFile(string lpFileName, uint dwDesiredAccess, int dwShareMode,
        int lpSecurityAttributes, int dwCreationDisposition, int dwFlagsAndAttributes, int hTemplateFile);

        [DllImport(DLLPATH)]
        private static extern bool GetCommState(int hFile, ref DCB lpDCB);

        [DllImport(DLLPATH)]
        private static extern bool BuildCommDCB(string lpDef, ref DCB lpDCB);

        [DllImport(DLLPATH)]
        private static extern bool SetCommState(int hFile, ref DCB lpDCB);

        [DllImport(DLLPATH)]
        private static extern bool GetCommTimeouts(int hFile, ref COMMTIMEOUTS lpCommTimeouts);

        [DllImport(DLLPATH)]
        private static extern bool SetCommTimeouts(int hFile, ref COMMTIMEOUTS lpCommTimeouts);

        [DllImport(DLLPATH)]
        private static extern bool ClearCommError(IntPtr hFile, ref uint lpErrors, ref COMSTAT lpStat);

        [DllImport(DLLPATH)]
        private static extern bool ReadFile(int hFile, byte[] lpBuffer, int nNumberOfBytesToRead,
        ref int lpNumberOfBytesRead, ref OVERLAPPED lpOverlapped);

        [DllImport(DLLPATH)]
        private static extern bool WriteFile(int hFile, byte[] lpBuffer, int nNumberOfBytesToWrite,
        ref int lpNumberOfBytesWritten, ref OVERLAPPED lpOverlapped);




        [DllImport(DLLPATH, SetLastError = true)]
        private static extern bool FlushFileBuffers(int hFile);




        [DllImport(DLLPATH, SetLastError = true)]
        private static extern bool PurgeComm(int hFile, uint dwFlags);

        [DllImport(DLLPATH)]
        private static extern bool CloseHandle(int hObject);

        [DllImport(DLLPATH)]
        private static extern uint GetLastError();

        internal void SetDcbFlag(int whichFlag, int setting, DCB dcb)
        {
            uint num;
            setting = setting << whichFlag;
            if ((whichFlag == 4) || (whichFlag == 12))
            {
                num = 3;
            }
            else if (whichFlag == 15)
            {
                num = 0x1ffff;
            }
           else
            {
                num = 1;
            }
            dcb.flags &= ~(num << whichFlag);
            dcb.flags |= (uint)setting;
        }

        public int Open()

        {

            DCB dcb = new DCB();

            COMMTIMEOUTS ctoCommPort = new COMMTIMEOUTS();

            hComm = CreateFile(Port, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

            if (hComm == INVALID_HANDLE_VALUE)
            {
                return -1;
            }

            GetCommTimeouts(hComm, ref ctoCommPort);
            ctoCommPort.ReadTotalTimeoutConstant = ReadTimeout;
            ctoCommPort.ReadTotalTimeoutMultiplier = 0;
            ctoCommPort.WriteTotalTimeoutMultiplier = 0;
            ctoCommPort.WriteTotalTimeoutConstant = 0;
            SetCommTimeouts(hComm, ref ctoCommPort);


            if (!GetCommState(hComm, ref dcb))
            {
                return -1;
            }


            if (!BuildCommDCB(CommSetting, ref dcb))
            {
                return -1;
            }
            if (!SetCommState(hComm, ref dcb))
            {
                return -1;
            }
            Opened = true;
            return 0;
        }

        public void Close()
        {
            if (hComm != INVALID_HANDLE_VALUE)
            {
                CloseHandle(hComm);
                this.Opened = false;
            }
        }

        public long ByteToRead()
        {
            if (hComm != INVALID_HANDLE_VALUE)
            {
                COMSTAT lSTAT = new COMSTAT();
                uint lerror = 0;
                bool f = ClearCommError((IntPtr)hComm, ref lerror, ref lSTAT);
                return lSTAT.cbInQue;
            }
            else
            {
                return -1;
            }
        }

        public int Read(ref byte[] bytData, int NumBytes)
        {
            if (hComm != INVALID_HANDLE_VALUE)
            {
                OVERLAPPED ovlCommPort = new OVERLAPPED();
                int BytesRead = 0;
                ReadFile(hComm, bytData, NumBytes, ref BytesRead, ref ovlCommPort);
                return BytesRead;
            }
            else
            {
                return -1;
            }
        }
        public int Write(byte[] WriteBytes, int intSize)
        {
            if (hComm != INVALID_HANDLE_VALUE)
            {
                OVERLAPPED ovlCommPort = new OVERLAPPED();
                int BytesWritten = 0;
                WriteFile(hComm, WriteBytes, intSize, ref BytesWritten, ref ovlCommPort);
                return BytesWritten;
            }
            else
            {
                return -1;
            }
        }

        public void ClearReceiveBuf()
        {
            if (hComm != INVALID_HANDLE_VALUE)
            {
                PurgeComm(hComm, PURGE_RXABORT | PURGE_RXCLEAR);
            }
        }

        public void ClearSendBuf()
        {
            if (hComm != INVALID_HANDLE_VALUE)
            {
                PurgeComm(hComm, PURGE_TXABORT | PURGE_TXCLEAR);
            }
        }
    }
}
