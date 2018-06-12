#pragma once

typedef struct HexDataSection
{
	int sectionId;
	unsigned long addr;
	unsigned long length;
	unsigned long offset;
	char data[0x10000];
	struct HexDataSection *next;
}HexDataSection;

class HexFile
{
public:
	HexFile();
	~HexFile();
	int OpenHexFile(const char *path);
	int AddDataFromBuff(const char *buff, int length);
	int GetSectionNum(void);
	int GetSectionData(int sectionId,unsigned long *addr, char *buff, int *length);
	void SetFileType(int type);
	unsigned long GetFileVer(void);
	int GetFirmwareInf(unsigned char *buff);
	void Clear(void);
protected:
	unsigned long HexStringToBin(const unsigned char *str, int num);
	unsigned long CheckSum(void *buff,int num);

private:
	HexDataSection *head;
	int fileType;
};

