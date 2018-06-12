#include "stdafx.h"
#include "HexFile.h"
#include <string.h>
#include <stdio.h>
#include "debug.h"


typedef struct
{
	unsigned long activeFlag;
	unsigned long appBeginAddr;
	unsigned long appImageLength;
	unsigned long appVersion;
	unsigned long appCheckSum;
	unsigned long infCheckSum;
}FirmwareInfStruct;

HexFile::HexFile()
{
	head = NULL;
	this->fileType = 0;
}


HexFile::~HexFile()
{
	Clear();
}

int HexFile::OpenHexFile(const char *path)
{
	FILE *fp;
	int ret;

	fopen_s(&fp, path, "rb");
	if (fp == NULL)
		Debug_Printf("Read file \"%s\" error\n", path);
	fseek(fp, 0L, SEEK_END);
	int length = ftell(fp);
	char *buff = new char[length + 1];
	fseek(fp, 0L, SEEK_SET);
	length = fread(buff,1 , length, fp);
	fclose(fp);
	ret = AddDataFromBuff(buff, length);
	delete[]buff;
	return ret;
}

int HexFile::AddDataFromBuff(const char *buff, int length)
{
	int index = 0;
	unsigned long baseAddr = 0;
	unsigned char data[256];
	unsigned char sum = 0;
	HexDataSection *p_nowSection = NULL;
	Clear();

	if (this->fileType)
	{
		Debug_Printf("length=%x\n", length);
		if (length <= 0xc000)
		{
			head = new HexDataSection;
			head->next = NULL;
			head->addr = 0x8000000;
			head->sectionId = 0;
			head->length = length;
			head->offset = 0x4000;
			memcpy(head->data + head->offset, buff, head->length);
		}
		else if (length <= 0x1c000)
		{
			head = new HexDataSection;
			head->next = NULL;
			head->addr = 0x8000000;
			head->sectionId = 0;
			head->length = 0xc000;
			head->offset = 0x4000;
			memcpy(head->data + head->offset, buff, head->length);

			head->next = new HexDataSection;
			head->next->next = NULL;
			head->next->addr = 0x8010000;
			head->next->sectionId = 1;
			head->next->length = length - 0xc000;
			head->next->offset = 0x0;

			memcpy(head->next->data + head->next->offset, buff, head->next->length);
		}

		return 0;
	}

	while (index + 12 < length)
	{
		unsigned char *p = (unsigned char*)(buff + index);
		sum = 0;
		if (p[0] != ':')
		{
			Clear();
			return -1;
		}
		int dataLenth = HexStringToBin(p + 1, 2);
		sum += dataLenth;
		int offsetAddr = HexStringToBin(p + 3, 4);
		sum += (offsetAddr & 0x0000000ff) + ((offsetAddr >> 8) & 0x000000ff);
		int dataType = HexStringToBin(p + 7, 2);
		sum += dataType;

		if (index + 12 + dataLenth * 2  > length)
		{
			Clear();
			return -1;
		}

		for (int i = 0; i < dataLenth + 1; i++)
		{
			data[i] = HexStringToBin(p + 9 + i * 2, 2);
			sum += data[i];
		}

		if(sum != 0)
		{
			Clear();
			return -1;
		}

		switch (dataType)
		{
		case 00:
		{
			if (head == NULL)
			{
				head = p_nowSection = new HexDataSection;
				head->next = NULL;
				head->addr = 0;
				head->sectionId = 0;
				head->length = 0;
				head->offset = 0xffff;
			}
			memcpy(p_nowSection->data + offsetAddr, data, dataLenth);
			p_nowSection->length += dataLenth;
			if (p_nowSection->offset > offsetAddr)
				p_nowSection->offset = offsetAddr;
		}
			break;
		case 01:
		{
			return 0;
		}
			break;
		case 02:
		{

		}
			break;
		case 03:
			break;
		case 04:
		{
			baseAddr = ((data[0] << 8) + data[1]) << 16;
			if (head == NULL)
			{
				head = p_nowSection = new HexDataSection;
				head->next = NULL;
				head->addr = baseAddr;
				head->length = 0;
				head->sectionId = 0;
				head->offset = 0xffff;
			}
			else
			{
				p_nowSection->next = new HexDataSection;
				p_nowSection->next->sectionId = p_nowSection->sectionId + 1;
				p_nowSection = p_nowSection->next;
				p_nowSection->next = NULL;
				p_nowSection->addr = baseAddr;
				p_nowSection->length = 0;
				p_nowSection->offset = 0xffff;
			}
		}
			break;
		case 05:
			break;
		}
		index = index + 11 + dataLenth * 2;

		while (index < length)
		{
			if (buff[index] == ':')
				break;
			index ++;
		}
	}

	return 0;
}

int HexFile::GetSectionNum(void)
{
	HexDataSection *p_nowSection = head;
	int i = 0;

	while (p_nowSection != NULL)
	{
		p_nowSection = p_nowSection->next;
		i++;
	}

	return i;
}

int HexFile::GetSectionData(int sectionId ,unsigned long *addr, char *buff, int *length)
{
	HexDataSection *p_nowSection = head;

	while (p_nowSection != NULL && p_nowSection->sectionId != sectionId)
	{
		p_nowSection = p_nowSection->next;
	}
	if (p_nowSection == NULL)
		return 0;
	*addr = p_nowSection->addr + p_nowSection->offset;
	*length = p_nowSection->length;
	memcpy(buff, p_nowSection->data + p_nowSection->offset, p_nowSection->length);

	return p_nowSection->length;
}

void HexFile::SetFileType(int type)
{
	this->fileType = type;
}

unsigned long HexFile::GetFileVer(void)
{
	unsigned int ver;
	memcpy(&ver, head->data + head->offset + 0xc0, 4);
	return ver;
}

int HexFile::GetFirmwareInf(unsigned char *buff)
{
	FirmwareInfStruct *p_firmwareInf = (FirmwareInfStruct *)buff;
	p_firmwareInf->activeFlag = 0x55aa5a5a;
	p_firmwareInf->appBeginAddr = head->addr + head->offset;
	p_firmwareInf->appImageLength = 0;
	p_firmwareInf->appCheckSum = 0; 

	int sectionNum = GetSectionNum();
	HexDataSection *p_nowSection = head;
	while (p_nowSection != NULL)
	{
		p_firmwareInf->appImageLength += p_nowSection->length;
		p_firmwareInf->appCheckSum ^= CheckSum(p_nowSection->data + p_nowSection->offset, p_nowSection->length);
		p_nowSection = p_nowSection->next;
	}

	p_firmwareInf->appVersion = GetFileVer();
	p_firmwareInf->infCheckSum = CheckSum(buff, sizeof(FirmwareInfStruct) - 4);
	
	return sizeof(FirmwareInfStruct);
}

void HexFile::Clear(void)
{
	while (head != NULL)
	{
		HexDataSection *next = head->next;
		delete head;
		head = next;
	}
}

unsigned long HexFile::HexStringToBin(const unsigned char *str, int num)
{
	unsigned long ret = 0;
	for (int i = 0; i < num; i++)
	{
		if (str[i] <= '9' && str[i] >= '0')
		{
			ret = (ret << 4) | (str[i] - '0');
		}
		else if (str[i] <= 'F' && str[i] >= 'A')
		{
			ret = (ret << 4) | (str[i] - 'A' + 10);
		}
		else if (str[i] <= 'f' && str[i] >= 'a')
		{
			ret = (ret << 4) | (str[i] - 'a' + 10);
		}
		else
		{
			return 0;
		}
	}
	return ret;
}

unsigned long HexFile::CheckSum(void *buff, int num)
{
	unsigned char *p = (unsigned char *)buff;
	unsigned char ckSum[4] = { 0 ,0,0,0 };
	unsigned long ret;
	for (int i = 0; i < num / 4; i++)
	{
		ckSum[0] ^= p[i * 4 + 0];
		ckSum[1] ^= p[i * 4 + 1];
		ckSum[2] ^= p[i * 4 + 2];
		ckSum[3] ^= p[i * 4 + 3];
	}
	memcpy(&ret, ckSum, 4);
	return ret;
}
