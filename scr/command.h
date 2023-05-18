/*
 * command.h
 *
 *  Created on: 08/05/2020
 *      Author: Janaka Madhusankha
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdio.h>
//#include "registers.h"
#include "types.h"
#include "serial.h"

// number of modules can be connected series
#define MAX_MODULE_COUNT 8

#define PROTOCOL_SEPERATOR (':')
#define MESSAGE_SEPERATOR  (',')
#define END_OF_LINE ('\n')
#define LINE_SEPERATOR ('\r')
#define PROTOCOL_X350_REAR_MSG  ('1')
#define PROTOCOL_BROADCAST_MSG ('0')

typedef struct{
 bool dataAvailable;
 char add;
 char cmd;
 char reg;
 char hexData[COMMAND_BUFFER_LENGTH-MIN_CMD_LENGTH];
}commandDetails;

typedef struct{
	int8u inputStatus;
	int8u outputStatus;
	int8u buzzerStatus;
	int16u analogOutStatus;
	int8u ledOutStatus;
}TempStatus_t;

typedef struct {
	TempStatus_t status;
	int8u incomingRegMode;
	int8u currentSciPort;
	int32u chksum;                // checksum will be checked before database use
}TempValueDB;

typedef enum {WRITE_ACK_DISABLE = 0, WRITE_ACK_ENABLE, MAX_WRITE_ACK}writeReplyAck_t;

typedef enum {
	EX_IO_MODULE_1 = 0,		// X300-007 board is first module and its fixed
	EX_IO_MODULE_2,
	EX_IO_MODULE_3,
	EX_IO_MODULE_4,
	EX_IO_MODULE_5,
	EX_IO_MODULE_6,
	EX_IO_MODULE_7,
	EX_IO_MODULE_MAX
}moduleCount_t; // note that X350 rear board is considered as the first IO module and so on


extern TempValueDB appVariables;		// this is only for X350 special variable store

//void setWriteReplyAck(int8u ackType);
//int8u getWriteReplyAck(void);
//void SetConfigAddress(int8u add);
//int8u GetConfigAddress(void);
//int8u char_to_byte(char ch);
//void ReadReg(int8u regId);
//void WriteReg(int8u regId);

//bool IsMessageValid(char *command);
//void SeperateCommand(commandDetails *pHeader, char command[]);
//bool IsMessageForMe(void);
//void MakeHeader(commandDetails *pHeader , char *message);

//int8u GetIncomingRegMode(void);
//void SetIncomingRegMode(int8u regMode);
//char *GetRegDataPointer(void);

//// main command handling function
//void CommandHandle(char command[]);

#endif /* COMMAND_H_ */
