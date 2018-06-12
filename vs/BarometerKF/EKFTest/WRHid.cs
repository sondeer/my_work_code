using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace EKFTest
{
    class WRHid
    {
        private IntPtr handle = IntPtr.Zero;

        public bool IsOpened
        {
            get
            {
                return (this.handle != IntPtr.Zero);
            }
        }

        public int Open(ushort vID,ushort pID)
        {
           this.handle = hid_open(vID, pID, IntPtr.Zero);
           if(this.handle == IntPtr.Zero)
             return -1;
           return 0;
        }

        public unsafe int Write(byte[] buff , int num)
        {
            byte[] temp = new byte[num + 1];
            int ret = 0;

            if(this.handle == IntPtr.Zero)
             return -1;
            Array.Copy(buff,0,temp,1,num);
            temp[0] = 0;
            fixed(byte*p = temp)
            {
                ret = hid_write(this.handle, p,num + 1);
            }

            return ret - 1;
        }

        public unsafe int Read(byte[] buff, int num)
        {
            int ret = 0;
            if(this.handle == IntPtr.Zero)
               return -1;

            fixed(byte*p = buff)
            {
                ret = hid_read(this.handle, p, num);
            }

             return ret;
        }

        public unsafe int ReadTimeOut(byte[] buff, int num, int readTimeOut = 0)
        {
            int ret = 0;
            if (this.handle == IntPtr.Zero)
                return -1;

            fixed (byte* p = buff)
            {
                ret = hid_read_timeout(this.handle, p, num, readTimeOut);
            }

            return ret;
        }

        public int Close()
        {
            if (this.handle == IntPtr.Zero)
                return -1;
            hid_close(this.handle);
            this.handle = IntPtr.Zero;
            return 0;
        }

        [DllImport("hidapi.dll" ,CallingConvention=CallingConvention.Cdecl)]
        private static extern IntPtr hid_open(ushort vendor_id, ushort product_id, IntPtr serial_number);

        [DllImport("hidapi.dll", CallingConvention = CallingConvention.Cdecl)]
        unsafe private static extern  int hid_write(IntPtr device, byte *data, int length);

        [DllImport("hidapi.dll", CallingConvention = CallingConvention.Cdecl)]
        unsafe private static extern int hid_read(IntPtr device, byte*  data, int length);

        [DllImport("hidapi.dll", CallingConvention = CallingConvention.Cdecl)]
        unsafe private static extern int hid_read_timeout(IntPtr device, byte* data, int length, int milliseconds);

        [DllImport("hidapi.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void hid_close(IntPtr device);
    }
}
