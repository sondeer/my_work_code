using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.IO.Ports;
using System.Threading;

namespace WriteFirmware
{
    class FirmwareManage
    {
        public enum FileType
        {
            HexFileType = 0,
            BinFileType
        } 

        private SerialPort com;

        public int LoadHexFile(string path)
        {
            byte[] pathArray = Encoding.ASCII.GetBytes(path);
            return HexFileOpenHexFile(pathArray);
        }

        public SerialPort ComPort
        {
            set { com = value; }
        }

        public int SectionNum
        {
            get { return HexFileGetSectionNum(); }
        }

        public FileType HexOrBin
        {
            set { HexFileSetFileType((int)value); }
        }

        public int GetSectionData(int sectionId, out UInt32 addr, byte[] buff, out int length)
        {
            return HexFileGetSectionData(sectionId, out addr, buff, out length);
        }

        public int GetFirmwareInf(byte[] buff)
        {
            return HexFileGetFirmwareInf(buff);
    }

        public int FirmwareGoToApp(int timeOut = 1000)
        {
            byte[] buff = new byte[32];

            int length = GenerateGotoAppPacket(buff);
            int ret = 0;

            for (int i = 0; i < 3; i++)
            {
                ret = WriteAndWaitAck(buff, length, timeOut);
                if (ret == 0)
                    return ret;
            }
            return ret;
        }

        public int FirmwareSetAddr(UInt32 addr , int timeOut = 50)
        {
            byte[] buff = new byte[32];
            int ret = 0;
            int length = GenerateSetAddrPacket(buff, addr);

            for (int i = 0; i < 3; i ++)
            {
                ret = WriteAndWaitAck(buff, length, timeOut);
                if (ret == 0)
                    return ret;
            }           
            return ret;
        }

        public int FirmwareCkAndPro(byte[] buff, int index, int size, int timeOut = 500)
        {
            byte[] buff1 = new byte[size];
            byte[] sendBuff = new byte[32];
            int ret = 0;

            Array.Copy(buff, index, buff1, 0, size);
            int length = GenerateCkAndProPacket(sendBuff, buff1, size);

            for (int i = 0; i < 3; i++)
            {
                ret = WriteAndWaitAck(sendBuff, length, timeOut);
                if (ret == 0)
                    return ret;
            }
            return ret;
        }

        public int FirmwareSetInf(byte[] data, UInt32 size, UInt32 addr, UInt32 ver,int timeOut = 1000)
        {
            byte[] buff = new byte[32];

            int length = GenerateSetInfPacket(buff, data, size, addr, ver);
            int ret = 0;

            for (int i = 0; i < 3; i++)
            {
                ret = WriteAndWaitAck(buff, length, timeOut);
                if (ret == 0)
                    return ret;
            }

            return ret;
        }

        public UInt32 GetSoftVersion(int timeOut = 50)
        {
            byte[] buff = new byte[32];
            int length = GenerateGetVerPacket(buff);
            UInt32 ver = 0;

            try
            {
                this.com.ReadExisting();
                this.com.Write(buff, 0, length);
                int nowMillisecond = Environment.TickCount;
                while(Environment.TickCount - nowMillisecond < timeOut)
                {
                    if(this.com.BytesToRead >= 8)
                    {
                        this.com.Read(buff, 0, 8);
                        ver = GetSoftVersion(buff, 8);
                        break;
                    }
                    Thread.Sleep(2);
                }
            }
            catch(Exception )
            {
                return 0;
            }

            return ver;
        }

        public int WriteDataToDevice(byte[] buff,int index,int size,int timeOut = 50)
        {
            byte[] buff1 = new byte[32];
            byte[] sendBuff = new byte[32];

            Array.Copy(buff, index, buff1, 0, size);

            int ret = 0;
            int length = GenerateSetDataPacket(sendBuff, (index & 0x7ff) / 16, buff1, size);

            for (int i = 0; i < 3; i++)
            {
                ret = WriteAndWaitAck(sendBuff, length, timeOut);
                if (ret == 0)
                    return ret;
            }
            return ret;
        }

        private int WriteAndWaitAck(byte[] buff,int size,int timeOut = 50)
        {
            int ret = -1;
            byte[] retBuff = new byte[32];
            try
            {
                this.com.ReadExisting();
                this.com.Write(buff, 0, size);
                int nowMillisecond = Environment.TickCount;
                while (Environment.TickCount - nowMillisecond < timeOut)
                {
                    if (this.com.BytesToRead >= 5)
                    {
                        this.com.Read(retBuff, 0, 5);
                        ret = CheckAckPacket(retBuff, 5);
                        break;
                    }
                    Thread.Sleep(2);
                }
            }
            catch (Exception)
            {
                return ret;
            }
            return ret;
        }


        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int HexFileOpenHexFile(byte[] path);
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int HexFileAddDataFromBuff(byte[] buff, int length);
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int HexFileGetSectionNum();
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int HexFileGetSectionData(int sectionId, out UInt32 addr, byte[] buff, out int length);
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void HexFileClear();
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void HexFileSetFileType(int fileType);
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 HexFileGetFileVer();
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int HexFileGetFirmwareInf(byte[] buff);

        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GenerateGotoAppPacket(byte[] buff);
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GenerateSetAddrPacket(byte[] buff, UInt32 addr);
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GenerateSetDataPacket(byte[] buff, int sNum, byte[] data, int length);
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GenerateGetVerPacket(byte[] buff);
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GenerateCkAndProPacket(byte[] buff, byte[] data, int num);
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GenerateSetInfPacket(byte[] buff, byte[] data, UInt32 length, UInt32 addr, UInt32 ver);
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern int CheckAckPacket(byte[] data, int length);
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern UInt32 GetSoftVersion(byte[] data, int length);
        

        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 WriteFirmwareUpdate(byte[] buff, int size, byte[] outBuff, out int outLength, UInt32 timeMil);
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 WriteFirmwareGetDeviceSoftVer();
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int WriteFirmwareBeginWrite();
        [DllImport("WriteFirmwareDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void WriteFirmwareInit();

    }
}
