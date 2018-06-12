#include "stdafx.h"
#include "HexFile.h"
#include <string.h>


HexFile::HexFile()
{
	head = NULL;
}


HexFile::~HexFile()
{
	Clear();
}

int HexFile::OpenHexFile(const char *path)
{
	FILE *fp;
	int ret;
	fopen_s(&fp, path, "r");
	fseek(fp, 0L, SEEK_END);
	int length = ftell(fp);
	char *buff = new char[length + 1];
	fseek(fp, 0L, SEEK_SET);
	fread(buff, length, 1, fp);
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
			}
			memcpy(p_nowSection->data + offsetAddr, data, dataLenth);
			p_nowSection->length += dataLenth;
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
			}
			else
			{
				p_nowSection->next = new HexDataSection;
				p_nowSection->next->sectionId = p_nowSection->sectionId + 1;
				p_nowSection = p_nowSection->next;
				p_nowSection->next = NULL;
				p_nowSection->addr = baseAddr;
				p_nowSection->length = 0;
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
	*addr = p_nowSection->addr;
	*length = p_nowSection->length;
	memcpy(buff, p_nowSection->data, p_nowSection->length);

	return p_nowSection->length;
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
