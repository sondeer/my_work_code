// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "debug.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
		AllocConsole();//AttachConsole(ATTACH_PARENT_PROCESS);
		GetStdHandle(STD_OUTPUT_HANDLE);
		FILE  *fileHandle;
		freopen_s(&fileHandle, "CONOUT$", "w", stdout);
		Debug_Printf("Debug...\n");
#endif
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
#ifdef _DEBUG
		FreeConsole();
#endif
		break;
	}
	return TRUE;
}

