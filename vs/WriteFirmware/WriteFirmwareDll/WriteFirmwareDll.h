// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 WRITEFIRMWAREDLL_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// WRITEFIRMWAREDLL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef WRITEFIRMWAREDLL_EXPORTS
#define WRITEFIRMWAREDLL_API __declspec(dllexport)
#else
#define WRITEFIRMWAREDLL_API __declspec(dllimport)
#endif


extern "C"
{
	WRITEFIRMWAREDLL_API int HexFileOpenHexFile(const char *path);
	WRITEFIRMWAREDLL_API int HexFileAddDataFromBuff(const char *buff, int length);
	WRITEFIRMWAREDLL_API int HexFileGetSectionNum(void);
	WRITEFIRMWAREDLL_API int HexFileGetSectionData(int sectionId, unsigned long *addr, char *buff, int *length);
	WRITEFIRMWAREDLL_API void HexFileClear(void);
	WRITEFIRMWAREDLL_API void HexFileSetFileType(int fileType);
	WRITEFIRMWAREDLL_API unsigned long HexFileGetFileVer(void);
	WRITEFIRMWAREDLL_API int HexFileGetFirmwareInf(unsigned char *buff);

	WRITEFIRMWAREDLL_API int GenerateGotoAppPacket(void *buff);
	WRITEFIRMWAREDLL_API int GenerateSetAddrPacket(void *buff, unsigned long addr);
	WRITEFIRMWAREDLL_API int GenerateSetDataPacket(void *buff, int sNum, void *data, int length);
	WRITEFIRMWAREDLL_API int GenerateGetVerPacket(void *buff);
	WRITEFIRMWAREDLL_API int GenerateCkAndProPacket(void *buff, void *data, unsigned long num);
	WRITEFIRMWAREDLL_API int GenerateSetInfPacket(void *buff, void *data, unsigned long length, unsigned long addr, unsigned long ver);
	WRITEFIRMWAREDLL_API int CheckAckPacket(void *data, int length);
	WRITEFIRMWAREDLL_API unsigned long GetSoftVersion(void *data, int length);
	

	WRITEFIRMWAREDLL_API unsigned long WriteFirmwareUpdate(unsigned char *buff, int size, unsigned char *outBuff, int *outLength, unsigned long timeMil);
	WRITEFIRMWAREDLL_API unsigned long WriteFirmwareGetDeviceSoftVer(void);
	WRITEFIRMWAREDLL_API int WriteFirmwareBeginWrite(void);
	WRITEFIRMWAREDLL_API void WriteFirmwareInit(void);
}