/***********************************************************************
MODULE:    	SERIAL
VERSION:   	1.00
CONTAINS:  	USART configurations for atmega328p
COPYRIGHT: 	
DATE: 		9 Mar 2004
NOTES:          Due to potential corruption, many of these functions will
                disable interrupts globally.
***********************************************************************/

#include "avrdet.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "types.h"
//#include "debug.h"
#include "fifo.h"
//#include "hw_spec.h"
//#include "main.h"
#include "asciidef.h"
#include "iocontrol.h"
#include "command.h"


#define BAUD_TO_BAUDCTRL(baud, freq, bscale) ((freq / ((1 << bscale) * 16) / baud) - 1)
#define BAUD_TO_BAUDCTRL_CLK2X(baud, freq, bscale) ((freq / ((1 << bscale) * 8) / baud) - 1)

#define BSCALE 0
//#define F_CLOCK 16588800UL
//#define BACALE_LOW_BAUD	2

//#define BAUD_TO_BAUDCTRL_LOW_BAUD(baud) BAUD_TO_BAUDCTRL(baud, FREQ, BACALE_LOW_BAUD)
#define BAUD_TO_BAUDCTRL_FIXED(baud) BAUD_TO_BAUDCTRL(baud, F_CPU, BSCALE)
#define BAUD_TO_BAUDCTRL_FIXED_CLK2X(baud) BAUD_TO_BAUDCTRL_CLK2X(baud, FREQ, BSCALE)


/* the A receiver buffer (RX1) */
static int8u aucSioRx1Buffer[BUFFER_SIZE];
struct t_fifo_ctl rSioRx1BufferCtl;

/* the A transmitter buffer (TX1) */
static int8u aucSioTx1Buffer[BUFFER_SIZE];
struct t_fifo_ctl rSioTx1BufferCtl;

//static bool  bTxIdle = 0;

int8u finalMsgCount = 0;
char message[BUFFER_SIZE];



/*
 * SerialInit -
 *
 * Initializes UART for mode 1
 * Baudrate: 9600 @ 8MHz internal RC, CKDIV8=0
 *
 * CAUTION: If interrupts are being used then EA must be set to 1
 *       after calling this function
 */
void SerialInit(int32u baud)
{
  	const uint8_t chsize[] = { (0<<UCSZ00), (1<<UCSZ00), (2<<UCSZ00), (3<<UCSZ00) };
  	uint8_t ctrlc;
  	uint16_t baudpscaler;
	int16u uiBRR;
	char parity = 'n';
	int8u databits = 8;
	int8u stopbits = 1;

  	FifoInit(&rSioRx1BufferCtl, aucSioRx1Buffer, SIO_RX1_BUF_SIZE);
  	FifoInit(&rSioTx1BufferCtl, aucSioTx1Buffer, SIO_TX1_BUF_SIZE);

	cli(); // Disable global interrupts
	UCSR0B = 0x00; 				  //disable while setting baud rate

	/* Set Async, parity, data and stop bits */
	ctrlc = UCSR0C;
	ctrlc |= ('n' == parity) ? (0<<UPM00) : ('e' == parity) ? (2<<UPM00) : (3<<UPM00);
	ctrlc |= (1 == stopbits) ? (0<<USBS0) : (1<<USBS0);
	ctrlc |= chsize[databits - 5];
	UCSR0C = ctrlc;

	/* Set baud with CLK2X = 0 (BSEL = fper/(2^bscale*16*fbaud) - 1 */
	//if(baud >= 4800)
	//{
	//	usart->hardware->BAUDCTRLA = BAUD_TO_BAUDCTRL_FIXED(baud);
	//	usart->hardware->BAUDCTRLB = (0 << USART_BSCALE_gp) | 0;
	//}
	//else
	//{
	//	usart->hardware->BAUDCTRLA = BAUD_TO_BAUDCTRL_LOW_BAUD(baud);
	//	usart->hardware->BAUDCTRLB = (BACALE_LOW_BAUD << USART_BSCALE_gp) | 0;
	//}
  	baudpscaler = BAUD_TO_BAUDCTRL_FIXED(baud);
  	UBRR0H = (int8u) (baudpscaler >> 8) & 0x0F;
	UBRR0L = (int8u) baudpscaler;

  	/* Enable receive and transmit */
	//UCSR0B = (1<<RXEN0) | (1<<TXEN0);
  	UCSR0B = ((1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0));

	sei(); // Enable global interrupts
}

bool is_last_char(int8u ucCh)
{
	bool status = false;
	switch ((char)ucCh)
	{
  	//case '\r':   // uncommit when message ends with \r character
  	case '\n':
  	case ';':
  		status = true;
  		break;
	}
	return status;
}

void sci_set_new_message(int8u count)
{
	finalMsgCount = count;
}

int8u sci_get_new_message(void)
{
	return	finalMsgCount;
}


void SerialPutChar(char ch)
{
	FifoPutChar(&rSioTx1BufferCtl, (int8u)ch);
}


void SerialPut(unsigned char *ptr, unsigned char length)
{
	while (length--)
	{
		SerialPutChar(*ptr++);
	}
}


void SerialPutStr(char *str)
{
	while (!(*str == '\0'))
	{
			//UDR = *str++;
		SerialPutChar(*str++);
	}
}

void CommsSendString(char *reply) // this is to match the current code
{
	SerialPutStr(reply);
}

void flushBuffer(int8u buffer)
{
	switch(buffer)
	{
		case BUFFER_RX:
			if((FifoUsed(&rSioRx1BufferCtl) + BUFFER_OFFSET) > BUFFER_SIZE)
				FifoFlushBuffer(&rSioRx1BufferCtl);
			break;
		case BUFFER_TX:
			if((FifoUsed(&rSioTx1BufferCtl) + BUFFER_OFFSET) > BUFFER_SIZE)
				FifoFlushBuffer(&rSioTx1BufferCtl);
			break;
	}
}


char SerialGetChar(void)
{
	int8u data;

	data = FifoGetChar(&rSioRx1BufferCtl);
	return (char)data;
}

void TrySendCh(void)
{
  if(FifoIsEmpty(&rSioTx1BufferCtl) == 0) /* channel has data */ /* channel is 'allowed' to tx */
  {
  	if(UCSR0A & (1 << UDRE0))
  	{
  		/* enable the UDRE interrupt */
  		UCSR0B |= (1<< UDRIE0);
  	}
  	else
  		UCSR0B |= (1 << TXCIE0);
  }
  else
  	UCSR0B &= ~(1 << UDRIE0);
}


void SerialGet(char *ptr, int8u length)
{
	while (length--)
	{
		*ptr++ = SerialGetChar();
	}
}

void SerialGetMsg(char *ptr)
{
	char ch;
	if(sci_get_new_message() > 0)
	{
		sci_set_new_message(sci_get_new_message() - 1);
		ch = SerialGetChar();
		*ptr++ = ch;
		while (!is_last_char((int8u)ch))
		{
			ch = SerialGetChar();
			*ptr++ = ch;
		}
	}
	else
		*ptr++ = '\0';
}


unsigned long SerialGetLong(void)
{
#if 1
	union { unsigned long l; unsigned char b[4]; } U;

	U.b[3] = SerialGetChar();
	U.b[2] = SerialGetChar();
	U.b[1] = SerialGetChar();
	U.b[0] = SerialGetChar();
	return U.l;
#else
	unsigned long l;

	SerialGet((unsigned char *) &l, 4);
	return l;
#endif
}

//void DefaultMsgProcess(void)
//{
//	char commandToExecute[MAX_MODULE_COMM_BUFFER];
//	char tempMessage[5][BUFFER_SIZE]; // 5 is maximum numbers of messages contain in one buffer lenth
//	int8u count = 0;
//	int8u msgCount = 0;
//	int8u poniCount = 0;
//	int8u strlength;
//	bool allMessageProcessed = true;
//
//	memset(tempMessage,'\0',sizeof(tempMessage));
//
//	strlength = strlen(message);
//
//	for(count = 0; count <= strlength; count++) // search for DLEs
//	{
//		tempMessage[msgCount][poniCount++] = message[count];
//		if(message[count+1] == (char)H_DLE)
//		{
//			msgCount += 1; // more than one message
//			poniCount = 0;
//		}
//	}
//
//	while (allMessageProcessed)
//	{
//		switch (tempMessage[msgCount][1])	// multiplex the message according to the port
//		{
//			case (char)PORT_CMD:
//				memset(commandToExecute,'\0',arlen(commandToExecute));
//				strncpy(commandToExecute,&tempMessage[msgCount][2],strlen(&tempMessage[msgCount][2]));
//				//CommandHandle(commandToExecute);
//				//CommsSendString(commandToExecute,SCI_PORT_RS232); // test
//				break;
//			case (char)PORT_BT:
//				//CommsSendString(&tempMessage[msgCount][2],SCI_PORT_BT);
//				break;
//			case (char)PORT_EXT:
//				//CommsSendString(&tempMessage[msgCount][2],SCI_PORT_RS232); // remove first two characters
//				break;
//		}
//		if(msgCount <= 0)
//			allMessageProcessed = false;
//
//		msgCount -= 1;
//	}
//}


void SerialHandler(void)
{
	memset(message, '\0', arlen(message));
	CommsSendString("SerialHandler\r\n");
	if(sci_get_new_message() > 0)
	{
		//SerialGetMsg(message);
		//SerialPutStr(message);
		//DefaultMsgProcess();
		//CommandHandle(&message[2]);

		//flushBuffer(BUFFER_RX);
	}
	TrySendCh();				// do not move this line (keep at last)
}


/////////..............INTERRUPT...............///////


ISR(USART_RX_vect)
{
  int8u ucStatus;
  int8u ucCh;
  int8u err;

  ucStatus = UCSR0A;
  ucCh = UDR0;
  err = ((1 << FE0) | (1 << UPE0) | (1 << DOR0));

  if ((ucStatus & err) == 0x00)
  { /* only store if no errors (any of FE, DOR, PE will fail write) */
    FifoPutChar(&rSioRx1BufferCtl, ucCh); /* if no space, this will not store */
    if(is_last_char(ucCh))
    	sci_set_new_message(sci_get_new_message() + 1);
  }
}

ISR(USART_UDRE_vect)
{
  if (FifoIsEmpty(&rSioTx1BufferCtl) == 0)
  { /* data still in current stream */
     UDR0 = FifoGetChar(&rSioTx1BufferCtl);
     UCSR0A |= (1 << TXC0);
  }
  else
  { /* DTR is holding things up on this channel */
      /* disable this (the UDR) interrupt */
     UCSR0B &= ~(1 << UDRIE0);
     /* enable the TXC flag */
     UCSR0B |= (1 << TXCIE0);
  }
}

ISR(USART_TX_vect)
{ /* whatever was being transmitted has completed */
  //bTxIdle = 1;
  /* disable this interrupt */
  UCSR0B &= ~(1 << TXCIE0);
}


