// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <iostream>  
#include <io.h>
#include "debug.h"

FILE *fp = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
///#ifdef _DEBUG
		AllocConsole();//AttachConsole(ATTACH_PARENT_PROCESS);
		GetStdHandle(STD_OUTPUT_HANDLE);
		FILE  *fileHandle;
		freopen_s(&fileHandle,"CONOUT$", "w", stdout);
		fopen_s(&fp, "log.txt", "a + ");
		Debug_Printf("Debug...\n");
		//fopen_s(&fp,"log.txt", "a + ");
//#endif
	    break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
//#ifdef _DEBUG
		fclose(fp);
		FreeConsole();
//#endif
		break;
	}
	return TRUE;
}

