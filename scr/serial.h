#ifndef SERIAL_DOT_H
#define SERIAL_DOT_H

#include "types.h"

typedef enum {FIVE = 5, SIX ,SEVEN, EIGHT} usartDataBits_t;
typedef enum {ONE = 1, TWO } usartStopBits_t;
typedef enum {NONE, ODD, EVEN } usartParity_t;
typedef enum {BUFFER_RX, BUFFER_TX } usartBuffType_t ;


///.............Imports from X300-509 X350 rear board code.........//
#define BUFFER_SIZE 254
#define BUFFER_OFFSET 3

#define SIO_RX1_BUF_SIZE	BUFFER_SIZE
#define SIO_TX1_BUF_SIZE	BUFFER_SIZE

#define PORT_CMD 0x30
#define PORT_BT 0x31
#define PORT_EXT 0x32
#define PORT_R320 0x33
#define SCI_DEFAULT_PORT SCI_PORT_OUT
#define SPECIAL_CHAR_MASK	0x80

#define MAX_CMD_LENGTH 8
#define MIN_CMD_LENGTH 6
#define MAX_HEADER_LENGTH 3
#define COMMAND_BUFFER_LENGTH 35
#define REPLY_BUFFER_LENGTH SIO_RX1_BUF_SIZE
#define MAX_MODULE_COMM_BUFFER	65

#define MAIN_SERIAL_PORT 1

#define arlen(array) (sizeof((array))/sizeof((array)[0]))

extern char message[BUFFER_SIZE];


bool is_last_char(int8u ucCh);
void sci_set_new_message(int8u count);
int8u sci_get_new_message(void);
void TrySendCh(void);
//void DefaultMsgProcess(void);

/*serial communication blocks
 *Serial read & write functions for communication with atmega8A
 *
 */
void SerialHandler(void);
void SerialInit(int32u baud, int8u parity, int8u databits, int8u stopbits);
void SerialPutChar(char ch);
void SerialPut(unsigned char *ptr, unsigned char length);
void SerialPutStr(char *str);
void CommsSendString(char *reply);
char SerialGetChar(void);
void SerialGetMsg(char *ptr);
void SerialGet(char *ptr, int8u length);
unsigned long SerialGetLong(void);

void set_tx_status(int8u status);
bool get_tx_status(void);

#endif

