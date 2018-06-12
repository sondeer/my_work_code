// amrtest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "interf_dec.h"
#include "interf_enc.h"
#include "typedef.h"

#ifndef amrFileCodec_h
#define amrFileCodec_h

#define AMR_MAGIC_NUMBER "#!AMR\n"

#define PCM_FRAME_SIZE 160 // 8khz 8000*0.02=160
#define MAX_AMR_FRAME_SIZE 32
#define AMR_FRAME_COUNT_PER_SECOND 50
int amrEncodeMode[] = {4750, 5150, 5900, 6700, 7400, 7950, 10200, 12200}; // amr 编码方式

typedef struct
{
	char chChunkID[4];
	int nChunkSize;
}XCHUNKHEADER;

typedef struct
{
	short nFormatTag;
	short nChannels;
	int nSamplesPerSec;
	int nAvgBytesPerSec;
	short nBlockAlign;
	short nBitsPerSample;
}WAVEFORMAT;

typedef struct
{
	short nFormatTag;
	short nChannels;
	int nSamplesPerSec;
	int nAvgBytesPerSec;
	short nBlockAlign;
	short nBitsPerSample;
	short nExSize;
}WAVEFORMATX;

typedef struct
{
	char chRiffID[4];
	int nRiffSize;
	char chRiffFormat[4];
}RIFFHEADER;

typedef struct
{
	char chFmtID[4];
	int nFmtSize;
	WAVEFORMAT wf;
}FMTBLOCK;

// WAVE音频采样频率是8khz 
// 音频样本单元数 = 8000*0.02 = 160 (由采样频率决定)
// 声道数 1 : 160
//        2 : 160*2 = 320
// bps决定样本(sample)大小
// bps = 8 --> 8位 unsigned char
//       16 --> 16位 unsigned short
int EncodeWAVEFileToAMRFile(const char* pchWAVEFilename, const char* pchAMRFileName, int nChannels, int nBitsPerSample);

// 将AMR文件解码成WAVE文件
int DecodeAMRFileToWAVEFile(const char* pchAMRFileName, const char* pchWAVEFilename);

#endif


void SkipToPCMAudioData(FILE* fpwave)
{
	RIFFHEADER riff;
	FMTBLOCK fmt;
	XCHUNKHEADER chunk;
	WAVEFORMATX wfx;
	int bDataBlock = 0;

	// 1. 读RIFF头
	fread(&riff, 1, sizeof(RIFFHEADER), fpwave);

	// 2. 读FMT块 - 如果 fmt.nFmtSize>16 说明需要还有一个附属大小没有读
	fread(&chunk, 1, sizeof(XCHUNKHEADER), fpwave);
	if (chunk.nChunkSize>16)
	{
		fread(&wfx, 1, sizeof(WAVEFORMATX), fpwave);
	}
	else
	{
		memcpy(fmt.chFmtID, chunk.chChunkID, 4);
		fmt.nFmtSize = chunk.nChunkSize;
		fread(&fmt.wf, 1, sizeof(WAVEFORMAT), fpwave);
	}

	// 3.转到data块 - 有些还有fact块等。
	while (!bDataBlock)
	{
		fread(&chunk, 1, sizeof(XCHUNKHEADER), fpwave);
		if (!memcmp(chunk.chChunkID, "data", 4))
		{
			bDataBlock = 1;
			break;
		}
		// 因为这个不是data块,就跳过块数据
		fseek(fpwave, chunk.nChunkSize, SEEK_CUR);
	}
}

// 从WAVE文件读一个完整的PCM音频帧
// 返回值: 0-错误 >0: 完整帧大小
int ReadPCMFrame(short speech[], FILE* fpwave, int nChannels, int nBitsPerSample)
{
	int nRead = 0;
	int x = 0, y = 0;
	unsigned short ush1 = 0, ush2 = 0, ush = 0;

	// 原始PCM音频帧数据
	unsigned char pcmFrame_8b1[PCM_FRAME_SIZE];
	unsigned char pcmFrame_8b2[PCM_FRAME_SIZE << 1];
	unsigned short pcmFrame_16b1[PCM_FRAME_SIZE];
	unsigned short pcmFrame_16b2[PCM_FRAME_SIZE << 1];

	if (nBitsPerSample == 8 && nChannels == 1)
	{
		nRead = fread(pcmFrame_8b1, (nBitsPerSample / 8), PCM_FRAME_SIZE*nChannels, fpwave);
		for (x = 0; x<PCM_FRAME_SIZE; x++)
		{
			speech[x] = (short)((short)pcmFrame_8b1[x] << 7);
		}
	}
	else
		if (nBitsPerSample == 8 && nChannels == 2)
		{
			nRead = fread(pcmFrame_8b2, (nBitsPerSample / 8), PCM_FRAME_SIZE*nChannels, fpwave);
			for (x = 0, y = 0; y<PCM_FRAME_SIZE; y++, x += 2)
			{
				// 1 - 取两个声道之左声道
				speech[y] = (short)((short)pcmFrame_8b2[x + 0] << 7);
				// 2 - 取两个声道之右声道
				//speech[y] =(short)((short)pcmFrame_8b2[x+1] << 7);
				// 3 - 取两个声道的平均值
				//ush1 = (short)pcmFrame_8b2[x+0];
				//ush2 = (short)pcmFrame_8b2[x+1];
				//ush = (ush1 + ush2) >> 1;
				//speech[y] = (short)((short)ush << 7);
			}
		}
		else
			if (nBitsPerSample == 16 && nChannels == 1)
			{
				nRead = fread(pcmFrame_16b1, (nBitsPerSample / 8), PCM_FRAME_SIZE*nChannels, fpwave);
				for (x = 0; x<PCM_FRAME_SIZE; x++)
				{
					speech[x] = (short)pcmFrame_16b1[x + 0];
				}
			}
			else
				if (nBitsPerSample == 16 && nChannels == 2)
				{
					nRead = fread(pcmFrame_16b2, (nBitsPerSample / 8), PCM_FRAME_SIZE*nChannels, fpwave);
					for (x = 0, y = 0; y<PCM_FRAME_SIZE; y++, x += 2)
					{
						//speech[y] = (short)pcmFrame_16b2[x+0];
						speech[y] = (short)((int)((int)pcmFrame_16b2[x + 0] + (int)pcmFrame_16b2[x + 1])) >> 1;
					}
				}

	// 如果读到的数据不是一个完整的PCM帧, 就返回0
	if (nRead<PCM_FRAME_SIZE*nChannels) return 0;

	return nRead;
}

// WAVE音频采样频率是8khz 
// 音频样本单元数 = 8000*0.02 = 160 (由采样频率决定)
// 声道数 1 : 160
//        2 : 160*2 = 320
// bps决定样本(sample)大小
// bps = 8 --> 8位 unsigned char
//       16 --> 16位 unsigned short
int EncodeWAVEFileToAMRFile(const char* pchWAVEFilename, const char* pchAMRFileName, int nChannels, int nBitsPerSample)
{
	FILE* fpwave;
	FILE* fpamr;

	short speech[160];

	int byte_counter, frames = 0, bytes = 0;

	int *enstate;

	enum Mode req_mode = MR122;
	int dtx = 0;

	unsigned char amrFrame[MAX_AMR_FRAME_SIZE];

    fopen_s(&fpwave,pchWAVEFilename, "rb");
	if (fpwave == NULL)
	{
		return 0;
	}

	// 创建并初始化amr文件
	 fopen_s(&fpamr,pchAMRFileName, "wb");
	if (fpamr == NULL)
	{
		fclose(fpwave);
		return 0;
	}

	bytes = fwrite(AMR_MAGIC_NUMBER, sizeof(char), strlen(AMR_MAGIC_NUMBER), fpamr);


	//SkipToPCMAudioData(fpwave);

	enstate = (int*)Encoder_Interface_init(dtx);

	while (1)
	{
		// read one pcm frame
		if (!ReadPCMFrame(speech, fpwave, nChannels, nBitsPerSample)) break;

		frames++;
		byte_counter = Encoder_Interface_Encode(enstate, req_mode, speech, amrFrame, 0);

		bytes += byte_counter;
		fwrite(amrFrame, sizeof(unsigned char), byte_counter, fpamr);
	}

	Encoder_Interface_exit(enstate);

	fclose(fpamr);
	fclose(fpwave);

	return frames;
}

void WriteWAVEFileHeader(FILE* fpwave, int nFrame)
{
	RIFFHEADER riff;
	XCHUNKHEADER chunk;
	WAVEFORMATX wfx;

	char tag[10] = "";

	// 1. 写RIFF头
	strcpy_s(tag, "RIFF");
	memcpy(riff.chRiffID, tag, 4);
	riff.nRiffSize = 4 + sizeof(XCHUNKHEADER)+ sizeof(WAVEFORMATX)+ sizeof(XCHUNKHEADER)+ nFrame * 160 * sizeof(short);
	strcpy_s(tag, "WAVE");
	memcpy(riff.chRiffFormat, tag, 4);
	fwrite(&riff, 1, sizeof(RIFFHEADER), fpwave);

	// 2. 写FMT块
	strcpy_s(tag, "fmt ");
	memcpy(chunk.chChunkID, tag, 4);
	chunk.nChunkSize = sizeof(WAVEFORMATX);
	fwrite(&chunk, 1, sizeof(XCHUNKHEADER), fpwave);
	memset(&wfx, 0, sizeof(WAVEFORMATX));
	wfx.nFormatTag = 1;
	wfx.nChannels = 1; // 单声道
	wfx.nSamplesPerSec = 8000; // 8khz
	wfx.nAvgBytesPerSec = 16000;
	wfx.nBlockAlign = 2;
	wfx.nBitsPerSample = 16; // 16位
	fwrite(&wfx, 1, sizeof(WAVEFORMATX), fpwave);

	// 3. 写data块头
	strcpy_s(tag, "data");
	memcpy(chunk.chChunkID, tag, 4);
	chunk.nChunkSize = nFrame * 160 * sizeof(short);
	fwrite(&chunk, 1, sizeof(XCHUNKHEADER), fpwave);
}

const int round(const double x)
{
	return((int)(x + 0.5));
}

// 根据帧头计算当前帧大小
int caclAMRFrameSize(unsigned char frameHeader)
{
	int mode;
	int temp1 = 0;
	int temp2 = 0;
	int frameSize;

	temp1 = frameHeader;

	// 编码方式编号 = 帧头的3-6位
	temp1 &= 0x78; // 0111-1000
	temp1 >>= 3;

	mode = amrEncodeMode[temp1];

	// 计算amr音频数据帧大小
	// 原理: amr 一帧对应20ms，那么一秒有50帧的音频数据
	temp2 = round((double)(((double)mode / (double)AMR_FRAME_COUNT_PER_SECOND) / (double)8));

	frameSize = round((double)temp2 + 0.5);
	return frameSize;
}

// 读第一个帧 - (参考帧)
// 返回值: 0-出错; 1-正确
int ReadAMRFrameFirst(FILE* fpamr, unsigned char frameBuffer[], int* stdFrameSize, unsigned char* stdFrameHeader)
{
	memset(frameBuffer, 0, sizeof(frameBuffer));

	// 先读帧头
	fread(stdFrameHeader, 1, sizeof(unsigned char), fpamr);
	if (feof(fpamr)) return 0;

	// 根据帧头计算帧大小
	*stdFrameSize = caclAMRFrameSize(*stdFrameHeader);

	// 读首帧
	frameBuffer[0] = *stdFrameHeader;
	fread(&(frameBuffer[1]), 1, (*stdFrameSize - 1)*sizeof(unsigned char), fpamr);
	if (feof(fpamr)) return 0;

	return 1;
}

// 返回值: 0-出错; 1-正确
int ReadAMRFrame(FILE* fpamr, unsigned char frameBuffer[], int stdFrameSize, unsigned char stdFrameHeader)
{
	int bytes = 0;
	unsigned char frameHeader; // 帧头

	memset(frameBuffer, 0, sizeof(frameBuffer));

	// 读帧头
	// 如果是坏帧(不是标准帧头)，则继续读下一个字节，直到读到标准帧头
	while (1)
	{
		bytes = fread(&frameHeader, 1, sizeof(unsigned char), fpamr);
		if (feof(fpamr)) return 0;
		if (frameHeader == stdFrameHeader) break;
	}

	// 读该帧的语音数据(帧头已经读过)
	frameBuffer[0] = frameHeader;
	bytes = fread(&(frameBuffer[1]), 1, (stdFrameSize - 1)*sizeof(unsigned char), fpamr);
	if (feof(fpamr)) return 0;

	return 1;
}

// 将AMR文件解码成WAVE文件
int DecodeAMRFileToWAVEFile(const char* pchAMRFileName, const char* pchWAVEFilename)
{
	FILE* fpamr = NULL;
	FILE* fpwave = NULL;
	char magic[8];
	int * destate;
	int nFrameCount = 0;
	int stdFrameSize;
	unsigned char stdFrameHeader;

	unsigned char amrFrame[MAX_AMR_FRAME_SIZE];
	short pcmFrame[PCM_FRAME_SIZE];

	fopen_s(&fpamr,pchAMRFileName, "rb");
	if (fpamr == NULL) return 0;

	// 检查amr文件头
	fread(magic, sizeof(char), strlen(AMR_MAGIC_NUMBER), fpamr);
	if (strncmp(magic, AMR_MAGIC_NUMBER, strlen(AMR_MAGIC_NUMBER)))
	{
		fclose(fpamr);
		return 0;
	}

	// 创建并初始化WAVE文件
	fopen_s(&fpwave,pchWAVEFilename, "wb");
	WriteWAVEFileHeader(fpwave, nFrameCount);

	destate = (int*)Decoder_Interface_init();
	
	// 读第一帧 - 作为参考帧
	memset(amrFrame, 0, sizeof(amrFrame));
	memset(pcmFrame, 0, sizeof(pcmFrame));
	ReadAMRFrameFirst(fpamr, amrFrame, &stdFrameSize, &stdFrameHeader);

	// 解码一个AMR音频帧成PCM数据
	Decoder_Interface_Decode(destate, amrFrame, pcmFrame, 0);
	nFrameCount++;
	fwrite(pcmFrame, sizeof(short), PCM_FRAME_SIZE, fpwave);

	// 逐帧解码AMR并写到WAVE文件里
	while (1)
	{
		memset(amrFrame, 0, sizeof(amrFrame));
		memset(pcmFrame, 0, sizeof(pcmFrame));
		if (!ReadAMRFrame(fpamr, amrFrame, stdFrameSize, stdFrameHeader)) break;

		// 解码一个AMR音频帧成PCM数据 (8k-16b-单声道)
		Decoder_Interface_Decode(destate, amrFrame, pcmFrame, 0);
		nFrameCount++;
		fwrite(pcmFrame, sizeof(short), PCM_FRAME_SIZE, fpwave);
	}

	Decoder_Interface_exit(destate);

	fclose(fpwave);

	// 重写WAVE文件头
	fopen_s(&fpwave,pchWAVEFilename, "r+");
	WriteWAVEFileHeader(fpwave, nFrameCount);
	fclose(fpwave);

	return nFrameCount;
}


int main(int argc, char * argv[]) {

	DecodeAMRFileToWAVEFile("amr.amr", "wav.wav");
	EncodeWAVEFileToAMRFile("test", "x.amr", 1, 16);
	getchar();
	return 0;

#if 0
	FILE * file_speech, *file_analysis;

	short synth[160];
	int frames = 0;
	int * destate;
	int read_size;
#ifndef ETSI
	unsigned char analysis[31];
	enum Mode dec_mode;
#else
	short analysis[250];
#endif

	/* Process command line options */

		fopen_s(&file_speech,"temp", "wb");
		if (file_speech == NULL) {
			fprintf(stderr, "%s%s%s\n", "Use: ", argv[0], " input.file output.file ");
			return 1;
		}

		fopen_s(&file_analysis,"amr.amr", "rb");
		if (file_analysis == NULL) {
			printf("%s%s%s\n", "Use: ", argv[0], " input.file output.file ");
			fclose(file_speech);
			return 1;
		}

	/* init decoder */
	destate = (int*)Decoder_Interface_init();

#ifndef ETSI
	fread(analysis, 1, 6, file_analysis);
	/* find mode, read file */
	while (fread(analysis, sizeof(unsigned char), 1, file_analysis) > 0)
	{
		dec_mode = (Mode)(analysis[0] >> 3);
		switch (dec_mode) {
		case 0:
			read_size = 12;
			break;
		case 1:
			read_size = 13;
			break;
		case 2:
			read_size = 15;
			break;
		case 3:
			read_size = 17;
			break;
		case 4:
			read_size = 18;
			break;
		case 5:
			read_size = 20;
			break;
		case 6:
			read_size = 25;
			break;
		case 7:
			read_size = 31;
			break;
		case 8:
			read_size = 5;
			break;
		case 15:
			read_size = 0;
		default:
			read_size = 0;
			break;
		};
		fread(&analysis[1], sizeof(char), read_size, file_analysis);
#else

	read_size = 250;
	/* read file */
	while (fread(analysis, sizeof(short), read_size, file_analysis) > 0)
	{
#endif

		frames++;

		/* call decoder */
		Decoder_Interface_Decode(destate, analysis, synth, 0);

		fwrite(synth, sizeof(short), 160, file_speech);
	}

	Decoder_Interface_exit(destate);

	fclose(file_speech);
	fclose(file_analysis);
	fprintf(stderr, "\n%s%i%s\n", "Decoded ", frames, " frames.");

	return 0;
#endif
}
