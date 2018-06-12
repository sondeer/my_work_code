
#ifndef __CMD_H
#define __CMD_H

#include "stm32f0xx_hal.h"

#define MAX_NUM_SIZE 16
#define DATA_BUFF_SIZE 256

typedef __packed struct {
    uint8_t header1;
	  uint8_t header2;
	  uint8_t dataLength;
	  uint8_t idx;
	  uint8_t action;
	  uint8_t device;
	  uint8_t  port;
	  uint8_t data[MAX_NUM_SIZE + 1];
}BootPacketStruct;

void CmdInit(void);
void CmdUpdate(uint8_t *buff,int size);
int  CmdGetSendData(uint8_t *buff);


#endif
