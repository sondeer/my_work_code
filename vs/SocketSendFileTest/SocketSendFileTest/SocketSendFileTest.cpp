// SocketSendFileTest.cpp : �������̨Ӧ�ó������ڵ㡣
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

