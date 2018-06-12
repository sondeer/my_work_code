// WriteFirmwareDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "WriteFirmwareDll.h"
#include "CmdHandle.h"
#include "HexFile.h"
#include "debug.h"
#include "WriteFirmwareManage.h"

WriteFirmwareManage writeFirmwareManage;

extern HexFile hexFile;
extern CmdHandle cmdHandle;

WRITEFIRMWAREDLL_API int HexFileOpenHexFile(const char *path)
{
	//Debug_Printf(path);
	return hexFile.OpenHexFile(path);
}

WRITEFIRMWAREDLL_API int HexFileAddDataFromBuff(const char *buff, int length)
{
	return hexFile.AddDataFromBuff(buff, length);
}

WRITEFIRMWAREDLL_API int HexFileGetSectionNum(void)
{
	return hexFile.GetSectionNum();
}

WRITEFIRMWAREDLL_API int HexFileGetSectionData(int sectionId, unsigned long *addr, char *buff, int *length)
{
	//Debug_Printf("HexFileGetSectionData");
	return hexFile.GetSectionData(sectionId, addr, buff, length);
}

WRITEFIRMWAREDLL_API void HexFileClear(void)
{
	hexFile.Clear();
}

WRITEFIRMWAREDLL_API void HexFileSetFileType(int fileType)
{
	hexFile.SetFileType(fileType);
}

WRITEFIRMWAREDLL_API unsigned long HexFileGetFileVer(void)
{
	return hexFile.GetFileVer();
}

WRITEFIRMWAREDLL_API int HexFileGetFirmwareInf(unsigned char *buff)
{
	return hexFile.GetFirmwareInf(buff);
}

WRITEFIRMWAREDLL_API int GenerateGotoAppPacket(void *buff)
{
	return cmdHandle.GenerateGotoAppPacket(buff);
}

WRITEFIRMWAREDLL_API int GenerateSetAddrPacket(void *buff, unsigned long addr)
{
	return cmdHandle.GenerateSetAddrPacket(buff, addr);
}

WRITEFIRMWAREDLL_API int GenerateSetDataPacket(void *buff, int sNum, void *data, int length)
{
	return cmdHandle.GenerateSetDataPacket(buff, sNum, data, length);
}

WRITEFIRMWAREDLL_API int GenerateGetVerPacket(void *buff)
{
	return cmdHandle.GenerateGetVerPacket(buff);
}

WRITEFIRMWAREDLL_API int GenerateCkAndProPacket(void *buff, void *data, unsigned long num)
{
	return cmdHandle.GenerateCkAndProPacket(buff, data, num);
}

WRITEFIRMWAREDLL_API int GenerateSetInfPacket(void *buff, void *data, unsigned long length, unsigned long addr, unsigned long ver)
{
	return cmdHandle.GenerateSetInfPacket(buff, data, length, addr, ver);
}

WRITEFIRMWAREDLL_API int CheckAckPacket(void *data, int length)
{
	return cmdHandle.CheckAckPacket(data, length);
}

WRITEFIRMWAREDLL_API unsigned long GetSoftVersion(void *data, int length)
{
	return cmdHandle.GetSoftVersion(data, length);
}



WRITEFIRMWAREDLL_API unsigned long WriteFirmwareUpdate(unsigned char *buff, int size, unsigned char *outBuff, int *outLength, unsigned long timeMil)
{
	return writeFirmwareManage.Update(buff, size, outBuff, outLength, timeMil);
}

WRITEFIRMWAREDLL_API unsigned long WriteFirmwareGetDeviceSoftVer(void)
{
	return writeFirmwareManage.GetDeviceSoftVer();
}

WRITEFIRMWAREDLL_API int WriteFirmwareBeginWrite(void)
{
	//Debug_Printf("WriteFirmwareBeginWrite");
	return writeFirmwareManage.BeginWrite();
}

WRITEFIRMWAREDLL_API void WriteFirmwareInit(void)
{
	writeFirmwareManage.Init();
}
