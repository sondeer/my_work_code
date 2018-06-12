// SocketSendFileTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "wifidevice.h"


int main()
{
	int flag = -1;
	WifiDeviceInit();
	while (1)
	{
		WifiDeviceUpdate();
		//if (flag < 0)
		//	flag = SendPcmFile("2.amr");
	}
    return 0;
}

