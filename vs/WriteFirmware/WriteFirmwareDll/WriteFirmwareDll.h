// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� WRITEFIRMWAREDLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// WRITEFIRMWAREDLL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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