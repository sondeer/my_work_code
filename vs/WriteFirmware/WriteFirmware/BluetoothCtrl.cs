using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Windows.Devices.Bluetooth.Advertisement;
using Windows.Devices.Bluetooth.GenericAttributeProfile;
using Windows.Devices.Enumeration;
using Windows.Devices.Bluetooth;
using Windows.Foundation;

namespace WriteFirmware
{
    class BluetoothCtrl
    {
        static private BluetoothLEAdvertisementWatcher bleWatcher = new BluetoothLEAdvertisementWatcher();
        static private bool beginDetecteFlag = false;
        static private List<UInt64> bleAddrList;
        static private List<string> bleNameList;

        static public void OnAdvertisementReceived(BluetoothLEAdvertisementWatcher sender, BluetoothLEAdvertisementReceivedEventArgs args)
        {
            if (!bleAddrList.Contains(args.BluetoothAddress)) //&& args.Advertisement.ServiceUuids.Count != 0)
            {
                bleAddrList.Add(args.BluetoothAddress);
                bleNameList.Add(args.Advertisement.LocalName);
            }
        }

        static public void DetecteBluetoothDevice()
        {
            if (beginDetecteFlag)
                return;
            bleAddrList = new List<UInt64>();
            bleNameList = new List<string>();
       
            //bleWatcher.Received += new TypedEventHandler < BluetoothLEAdvertisementWatcher, BluetoothLEAdvertisementReceivedEventArgs >(OnAdvertisementReceived);
            bleWatcher.Start();
            beginDetecteFlag = true;
        }
    }
}
