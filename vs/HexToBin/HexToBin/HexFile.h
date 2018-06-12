#pragma once

typedef struct HexDataSection
{
	int sectionId;
	unsigned long addr;
	unsigned long length;
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
	void Clear(void);
protected:
	unsigned long HexStringToBin(const unsigned char *str, int num);
private:
	HexDataSection *head;
};

