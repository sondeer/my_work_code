
#include "stdafx.h"
#include <time.h>
#include <stdio.h>
#include "wifidevice.h"
#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")

#define USING_TCP 1

#define HEAD_HEX   0xEA

#define SEND_HEARTBEET_STATE   0x00
#define SEND_AUDIOFILE_HEAD    0x01
#define RET_AUDIOFILE_HEAD     0x11
#define SEND_AUDIOFILE_DATA    0x02
#define RET_AUDIOFILE_DATA     0x12
#define GET_FRIEND_LIST        0x03
#define SET_AUDIO_VOLUME       0x04
#define GET_AUDIO_VOLUME       0x05

#define SEND_FILE_TIMEOUT      5000

#define IP_UNSIGNED_INT(a,b,c,d) (unsigned int)((a << 24) | (b << 16) | (c << 8) | d)
#define TAGET_ADDR  IP_UNSIGNED_INT(192,168,3,26)
#define SEND_PACKET_SIZE  (320 * 3)


char g_devid[] = "gh_1683dbbff1fc_4b58275c2b2572e8";

#pragma pack(1)

typedef struct
{
  unsigned char head;
  unsigned short serialNum;
  unsigned char cmd;
  unsigned short len;
  unsigned char data[1500];
}PacketStruct;

typedef struct
{
  int srcId;
  int disId;
  char fileType;
  char needRet;
  char fromWhere;
  int fileLen;
  int packetNum;
  char talkerId[32];
}AudioHeadStruct;

typedef struct
{
  int srcId;
  int disId;
  int retCode;
}RetAudioHeadStruct;

typedef struct
{
  int srcId;
  int disId;
  int packetIndex;
  short dataLen;
  char data[1500];
}AudioDataStruct;

typedef struct
{
  int srcId;
  int disId;
  int packetIndex;
  int retCode;
}RetAudioDataStruct;


#pragma pack()

typedef struct
{
  char activeStep;
  char needRet;
  int srcId;
  int disId;
  FILE* fileHandle;
  int fileLen;
  int receivedLen;
  int packetNum;
  int curNum;
  unsigned long creatTime;
  int activeTime;
}ReceiveFileStruct;


typedef struct
{
  char activeStep;
  char needRet;
  int srcId;
  int disId;
  FILE* fileHandle;
  int fileLen;
  int sendedLen;
  int packetNum;
  int curNum;
  unsigned long creatTime;
  unsigned long lastSendTime;
  int activeTime;
}SendFileStruct;

static unsigned long  lastActiveTime;
static unsigned short serialNum;
static SOCKET  seviceSocket;
static SOCKET  clientSocket = INVALID_SOCKET;

static struct sockaddr_in deviceAddr;
ReceiveFileStruct recFile[4];
SendFileStruct   sendFile[4];

bool deviceConnectFlag = false;

static unsigned long SysTickGetMs(void)
{
	return clock();
}

static int ReceiveAudioFileHead(PacketStruct *pRec,PacketStruct *pSend)
{
  printf("ReceiveAudioFileHead\n");
  AudioHeadStruct *p_audioHead = (AudioHeadStruct *)pRec->data;
  RetAudioHeadStruct *p_retAudioHead = (RetAudioHeadStruct *)pSend->data;
  int i = 0;
  for(i = 0 ; i < 4 ; i ++)
  {
    if(recFile[i].activeStep == 0)
      break;
  }
  
   pSend->head = HEAD_HEX;
   pSend->cmd = RET_AUDIOFILE_HEAD;
   pSend->len = sizeof(RetAudioDataStruct);
   pSend->serialNum = pRec->serialNum;
   p_retAudioHead->disId = p_audioHead->srcId;
  
  if(i == 4 )
  {
       p_retAudioHead->srcId = -1;
       p_retAudioHead->retCode = -1;
  }
  else 
  {
    int res = fopen_s(&recFile[i].fileHandle,"receive.amr","wb");
    if(res != 0)
    {
       p_retAudioHead->srcId = i;
       p_retAudioHead->retCode = -2;
       return pSend->len + 6;
    }
     recFile[i].disId = p_retAudioHead->disId;
     recFile[i].srcId = i;
     recFile[i].activeStep = 1;
     recFile[i].needRet = p_audioHead->needRet;
     recFile[i].fileLen = p_audioHead->fileLen;
     recFile[i].receivedLen = 0;
     recFile[i].packetNum = p_audioHead->packetNum;
     recFile[i].curNum = 0;
     recFile[i].creatTime = SysTickGetMs();
     recFile[i].activeTime = SEND_FILE_TIMEOUT;
     
     p_retAudioHead->srcId = i;
     p_retAudioHead->retCode = 0;
  }
  
  return pSend->len + 6;
}

static int ReceiveAudioData(PacketStruct *pRec,PacketStruct *pSend)
{
	
   AudioDataStruct *pAudioData = ( AudioDataStruct *)pRec->data;
   RetAudioDataStruct *pRetAudioData = ( RetAudioDataStruct *)pSend->data;
   printf("ReceiveAudioData:%d,%d\n", pAudioData->packetIndex, pAudioData->dataLen);

   pSend->head = HEAD_HEX;
   pSend->cmd = RET_AUDIOFILE_DATA;
   pSend->len = sizeof(RetAudioDataStruct);
   pSend->serialNum = pRec->serialNum;
   
   pRetAudioData->disId = pAudioData->srcId;
   pRetAudioData->packetIndex = pAudioData->packetIndex;
   
   if(pAudioData->disId >= 4 || pAudioData->disId < 0)
   {
     pRetAudioData->srcId = -1;
     pRetAudioData->retCode = -1;
     return pSend->len + 6;
   }
   pRetAudioData->srcId = pAudioData->disId;
   ReceiveFileStruct *pReceiveFile = recFile + pAudioData->disId;
   
   if(pReceiveFile->activeStep != 1)
   {
      pRetAudioData->retCode = -2;
      return pSend->len + 6;
   }
   else if(pReceiveFile->disId != pAudioData->srcId)
   {
      pRetAudioData->retCode = -3;
      return pSend->len + 6;
   }
   else if(pReceiveFile->curNum != pAudioData->packetIndex)
   {
      pRetAudioData->packetIndex = pReceiveFile->curNum;
      pRetAudioData->retCode = -4;
      return pSend->len + 6;
   }
   fwrite(pAudioData->data,1, pAudioData->dataLen, pReceiveFile->fileHandle);
   pReceiveFile->receivedLen += pAudioData->dataLen;
   pReceiveFile->curNum ++;
   
   pRetAudioData->retCode = 0;
   if(pReceiveFile->needRet || pReceiveFile->receivedLen >= pReceiveFile->fileLen)
     return pSend->len + 6;
   else 
     return 0;
}

static void RecAudioDataUpdate(void)
{
   for(int i = 0 ; i < 4 ; i ++)
   {
     if(recFile[i].activeStep != 0)
     {
       if(recFile[i].receivedLen >= recFile[i].fileLen)
       {
         fclose(recFile[i].fileHandle);
         recFile[i].activeStep = 0;
       }
       else if(SysTickGetMs() - recFile[i].creatTime >= recFile[i].activeTime)
       {
		 fclose(recFile[i].fileHandle);
         recFile[i].activeStep = 0;
       }
     }
   }
}


static int ReadSendFile(SendFileStruct *pSendFile,PacketStruct *pSend,bool reSendFlag = false)
{
	
  AudioDataStruct *p_audioData = (AudioDataStruct *)pSend->data;
  
  
   pSend->head = HEAD_HEX;
   pSend->cmd = SEND_AUDIOFILE_DATA;
   pSend->serialNum = serialNum ++;
  
   p_audioData->srcId = pSendFile->srcId;
   p_audioData->disId = pSendFile->disId;
   p_audioData->packetIndex = pSendFile->curNum;
  //if(reSendFlag)
  fseek(pSendFile->fileHandle,pSendFile->sendedLen, SEEK_SET);
  p_audioData->dataLen = fread(p_audioData->data,1,SEND_PACKET_SIZE, pSendFile->fileHandle);

  printf("p_audioData->dataLen%d\n", p_audioData->dataLen);
  if (p_audioData->dataLen == 0)
	  return 0;
  pSend->len = p_audioData->dataLen + sizeof(AudioDataStruct) - 1500;
  pSendFile->lastSendTime = SysTickGetMs();
  return pSend->len + 6;
}

static int RetAudioFileHead(PacketStruct *pRec,PacketStruct *pSend)
{
  RetAudioHeadStruct *pRetAudioHead = ( RetAudioHeadStruct *)pRec->data;
  printf("RetAudioFileHead:%d,%d\n", pRetAudioHead->disId, pRetAudioHead->retCode);
  
  if (pRetAudioHead->retCode < 0)
  {
	  return -1;
  }
  if(pRetAudioHead->disId >= 4)
  {
    return - 2;
  }
  SendFileStruct *pSendFile = sendFile + pRetAudioHead->disId;
  
  if(pSendFile->activeStep != 1)
    return -3;
  
  pSendFile->disId = pRetAudioHead->srcId;
  if (pSendFile->needRet)
	  return ReadSendFile(pSendFile, pSend);
  else
	  return 0;
}

static int RetAudioData(PacketStruct *pRec, PacketStruct *pSend)
{
	
	RetAudioDataStruct *pRetAudioData = (RetAudioDataStruct *)pRec->data;
	printf("RetAudioData:%d\n", pRetAudioData->packetIndex);

	if (pRetAudioData->disId >= 4)
	{
		return -1;
	}
	SendFileStruct *pSendFile = sendFile + pRetAudioData->disId;
	if (pSendFile->activeStep != 1)
		return -2;
	else if (pRetAudioData->srcId != pSendFile->disId)
		return -3;
	else if (pRetAudioData->retCode != 0)
	{
		if (pSendFile->curNum != pRetAudioData->packetIndex)
		{
			pSendFile->curNum = pRetAudioData->packetIndex;
			pSendFile->sendedLen = pSendFile->curNum * SEND_PACKET_SIZE;
		}
	}
	else
	{
		if (pSendFile->needRet)
		{
			pSendFile->curNum++;
			pSendFile->sendedLen += SEND_PACKET_SIZE;
		}
		else if(pRetAudioData->packetIndex == pSendFile->packetNum - 1)
		{
			pSendFile->activeStep = 0;
			fclose(pSendFile->fileHandle);
		}
	}

	if (pSendFile->needRet)
		return ReadSendFile(pSendFile, pSend);
	else
		return 0;
}


static int SendAudioDataUpdate(PacketStruct *pSend)
{
   unsigned long nowTime = SysTickGetMs();
   //printf("nowTime=%d", nowTime);
   for(int i = 0 ; i < 4 ; i ++)
   {
     if(sendFile[i].activeStep != 0)
     {
       if((sendFile[i].sendedLen >= sendFile[i].fileLen) && sendFile[i].needRet == 1)
       {
         sendFile[i].activeStep = 0;
         fclose(sendFile[i].fileHandle);
       }
       else if(nowTime - sendFile[i].creatTime >= sendFile[i].activeTime)
       {
         sendFile[i].activeStep = 0;
         fclose(sendFile[i].fileHandle);
       }
       else if(nowTime - sendFile[i].lastSendTime >= 2000)
       {
		 sendFile[i].lastSendTime = SysTickGetMs();
         if(sendFile[i].disId == -1)
         {
			 printf("SendAudioHead\n");
           AudioHeadStruct *p_audioHead = (AudioHeadStruct *)pSend->data;
           pSend->head = HEAD_HEX;
           pSend->cmd = SEND_AUDIOFILE_HEAD;
           pSend->len = sizeof(AudioHeadStruct);
           pSend->serialNum = serialNum ++;
           
           p_audioHead->srcId = sendFile[i].srcId;
           p_audioHead->disId = sendFile[i].disId;
           p_audioHead->fileType = 0;
           p_audioHead->needRet = sendFile[i].needRet;
           p_audioHead->fromWhere = 0;
           p_audioHead->fileLen = sendFile[i].fileLen;
           p_audioHead->packetNum = sendFile[i].packetNum;
           memcpy(p_audioHead->talkerId,g_devid,32);
           return pSend->len + 6;
         }
         else if(sendFile[i].needRet)
         {
           return ReadSendFile(&sendFile[i],pSend);
         }
       }
	   else if (sendFile[i].needRet == 0 && (nowTime - sendFile[i].lastSendTime >= 4) && sendFile[i].disId >= 0)
	   {
		   if (sendFile[i].curNum == sendFile[i].packetNum)
		   {
			   sendFile[i].curNum--;
			   //return 0;
		   }
		   int len =  ReadSendFile(&sendFile[i], pSend);
		   sendFile[i].curNum++;
		   sendFile[i].sendedLen += SEND_PACKET_SIZE;
		   return len;
	   }
     }
   }
   return 0;
}

static int AnalysisPacket(PacketStruct *pRec,PacketStruct *pSend)
{
  int ret = -1;
  if(pRec->head != HEAD_HEX || pRec->len > 1500)
    return - 1;
  switch(pRec->cmd)
  {
  case SEND_HEARTBEET_STATE:
	  printf("SEND_HEARTBEET_STATE\n");
	break;
  case SEND_AUDIOFILE_HEAD:
    ret = ReceiveAudioFileHead(pRec,pSend);
    break;
  case RET_AUDIOFILE_HEAD:
    ret = RetAudioFileHead(pRec,pSend);
    break;
  case SEND_AUDIOFILE_DATA:
    ret = ReceiveAudioData(pRec,pSend);
    break;
  case RET_AUDIOFILE_DATA:
    ret = RetAudioData(pRec,pSend);
    break;
  case GET_FRIEND_LIST:
    break;
  case SET_AUDIO_VOLUME:
    break;
  case GET_AUDIO_VOLUME:
    break;
  default:
    break;
  }
  return ret;
}

static int SendHeartbeetPacket(char buff[])
{
  PacketStruct *packetStr = (PacketStruct *)buff;
  packetStr->head = HEAD_HEX;
  packetStr->cmd = SEND_HEARTBEET_STATE;
  packetStr->len = 32;
  packetStr->serialNum = serialNum ++;
  memcpy(packetStr->data,g_devid,32);
  return packetStr->len + 6;
}

void WifiDeviceInit(void)
{
   WSADATA  Ws;
   struct sockaddr_in LocalAddr;


   if (WSAStartup(MAKEWORD(2, 2), &Ws) != 0)
   {
	        printf( "Init Windows Socket Failed::%d\n", GetLastError());
	        return;
   }
#ifdef USING_TCP
   seviceSocket = socket(AF_INET, SOCK_STREAM,0);
#else 
   seviceSocket = socket(AF_INET, SOCK_DGRAM, 0);
#endif

   LocalAddr.sin_family = AF_INET;
   LocalAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
   LocalAddr.sin_port = htons(5010);
   memset(LocalAddr.sin_zero, 0x00, 8);

   bind(seviceSocket, (struct sockaddr*)&LocalAddr, sizeof(LocalAddr));
   unsigned long ul = 1;//只要是非0值都可以
   ioctlsocket(seviceSocket, FIONBIO, &ul);
   //sl_SetSockOpt(seviceSocket, SL_SOL_SOCKET, SL_SO_NONBLOCKING, &lNonBlocking, sizeof(lNonBlocking));
   
   //ASSERT_ON_ERROR(seviceSocket);
   
   lastActiveTime = 0;
   serialNum = 0;

#ifdef USING_TCP
   int sta = listen(seviceSocket, 5);
   if (sta < 0)
	   printf("sta = %d\n", sta);
#endif
}

void WifiDeviceUpdate(void)
{
  char sendBuff[1500],recBuff[1500];
  int sendLength = 0;
  int recLength;
  int length = sizeof(deviceAddr);

#ifdef USING_TCP
  if(clientSocket == INVALID_SOCKET)
      clientSocket = accept(seviceSocket, (SOCKADDR*)&deviceAddr, &length);
  if (clientSocket == INVALID_SOCKET)
	  return;
  recLength = recv(clientSocket, recBuff, 1500, 0);
#else
  recLength = recvfrom(seviceSocket, recBuff, 1500, 0, (SOCKADDR*)&deviceAddr, &length);
#endif

  if(recLength > 0)
  {
	deviceConnectFlag = true;
    lastActiveTime = SysTickGetMs();
    sendLength = AnalysisPacket((PacketStruct *)recBuff,(PacketStruct *)sendBuff);
	printf("receive data \n");
  }
  
  if(sendLength <= 0)
  {
	  sendLength = SendAudioDataUpdate((PacketStruct *)sendBuff);
  }
  /*
  if(sendLength == 0 && (SysTickGetMs() - lastActiveTime >= 5000))
  {
    sendLength = SendHeartbeetPacket(sendBuff);
    lastActiveTime = SysTickGetMs();
  }*/
   
  if(sendLength > 0)
  {
    lastActiveTime = SysTickGetMs();
#ifdef USING_TCP
	send(clientSocket, sendBuff, sendLength, 0);
#else
    int iStatus = sendto(seviceSocket, sendBuff,sendLength,0,(SOCKADDR*)&deviceAddr, sizeof(deviceAddr));
#endif
  }
  
  RecAudioDataUpdate();
}

void GetAllFriendList(void)
{
  
}

int SendPcmFile(const char *path)
{
  int i;

  if (!deviceConnectFlag)
	  return -1;

  for(i = 0 ; i < 4 ; i ++)
  {
    if(sendFile[i].activeStep == 0)
      break;
  }
  if(i == 4)
     return -1;
  
  int res = fopen_s(&sendFile[i].fileHandle,path,"rb");
  if(res != 0)
  {
     return -1;
  }
   sendFile[i].disId = -1;
   sendFile[i].srcId = i;
   sendFile[i].activeStep = 1;
   sendFile[i].needRet = 0;
   fseek(sendFile[i].fileHandle, 0, SEEK_END);
   
   sendFile[i].fileLen = ftell(sendFile[i].fileHandle);
   fseek(sendFile[i].fileHandle, 0, SEEK_SET);
   sendFile[i].sendedLen = 0;
   sendFile[i].packetNum = (sendFile[i].fileLen + (SEND_PACKET_SIZE - 1))/ SEND_PACKET_SIZE;
   sendFile[i].curNum = 0;
   sendFile[i].creatTime = SysTickGetMs();
   sendFile[i].activeTime = SEND_FILE_TIMEOUT;
   sendFile[i].lastSendTime = 0;
   

   return 0;
}

