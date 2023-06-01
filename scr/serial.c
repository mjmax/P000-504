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
#include "types.h"
#include "fifo.h"
#include "asciidef.h"
#include "iocontrol.h"
#include "serial.h"
#include "dyn_ax18a.h"
#include "debug.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>

#ifdef AVR_ATmega2560
#define USART_UDRE_vect USART0_UDRE_vect
#define USART_RX_vect USART0_RX_vect
#define USART_TX_vect USART0_TX_vect
#endif


#define BAUD_TO_BAUDCTRL(baud, freq, bscale) ((freq / ((1 << bscale) * 16) / baud) - 1)
#define BAUD_TO_BAUDCTRL_CLK2X(baud, freq, bscale) ((freq / ((1 << bscale) * 8) / baud) - 1)

#define BSCALE 0
//#define BACALE_LOW_BAUD	2

//#define BAUD_TO_BAUDCTRL_LOW_BAUD(baud) BAUD_TO_BAUDCTRL(baud, FREQ, BACALE_LOW_BAUD)
#define BAUD_TO_BAUDCTRL_FIXED(baud) BAUD_TO_BAUDCTRL(baud, F_CPU, BSCALE)
#define BAUD_TO_BAUDCTRL_FIXED_CLK2X(baud) BAUD_TO_BAUDCTRL_CLK2X(baud, F_CPU, BSCALE)

/* the A receiver buffer (RX1) */
static int8u aucSioRx0Buffer[BUFFER_SIZE];
struct t_fifo_ctl rSioRx0BufferCtl;

/* the A transmitter buffer (TX1) */
static int8u aucSioTx0Buffer[BUFFER_SIZE];
struct t_fifo_ctl rSioTx0BufferCtl;

#ifdef AVR_ATmega2560
/* the A receiver buffer (RX1) */
static int8u aucSioRx1Buffer[BUFFER_SIZE];
struct t_fifo_ctl rSioRx1BufferCtl;

/* the A transmitter buffer (TX1) */
static int8u aucSioTx1Buffer[BUFFER_SIZE];
struct t_fifo_ctl rSioTx1BufferCtl;

/* the A receiver buffer (RX1) */
static int8u aucSioRx2Buffer[BUFFER_SIZE];
struct t_fifo_ctl rSioRx2BufferCtl;

/* the A transmitter buffer (TX1) */
static int8u aucSioTx2Buffer[BUFFER_SIZE];
struct t_fifo_ctl rSioTx2BufferCtl;

/* the A receiver buffer (RX1) */
static int8u aucSioRx3Buffer[BUFFER_SIZE];
struct t_fifo_ctl rSioRx3BufferCtl;

/* the A transmitter buffer (TX1) */
static int8u aucSioTx3Buffer[BUFFER_SIZE];
struct t_fifo_ctl rSioTx3BufferCtl;
#endif

static bool  bTxIdle = 0;

int8u currentSciPort = SCI_PORT_0;

int8u finalMsgCount = 0;
char message[BUFFER_SIZE];



/*
 * SerialInit -
 *
 * Initializes UART for given baudrate, parity, databits and stopbits
 * @param Baudrate: 9600, 19200, 38400, 57600, 115200
 * @param Parity: NONE, ODD, EVEN
 * @param Databits: 5, 6, 7, 8
 * @param Stopbits: 1, 2
 * @return none
 * @note
 * CAUTION: If interrupts are being used then EA must be set to 1
 *       after calling this function
 */
void SerialInit(int8u port, int32u baud, int8u parity, int8u databits, int8u stopbits)
{
  	const uint8_t chsize[] = {(0<<UCSZ00), (1<<UCSZ00), (2<<UCSZ00), (3<<UCSZ00)};
  	uint8_t ctrlc;
  	uint16_t baudpscaler;
	bool clk2xmode = false;

	/*Disable global interrupts*/
	cli();

  	/*Baud rate prescaler calculator*/
	if(baud < 1000001)
  		baudpscaler = BAUD_TO_BAUDCTRL_FIXED(baud);
	else
	{
		baudpscaler = BAUD_TO_BAUDCTRL_FIXED_CLK2X(baud);
		clk2xmode = true;
	}

	#ifdef AVR_ATmega328P
		port = SCI_PORT_0;
	#endif

	switch (port)
	{
		case SCI_PORT_0:
			UCSR0B = 0x00; 	//disable while setting baud rate
			/* Initialize the buffers */
  			FifoInit(&rSioRx0BufferCtl, aucSioRx0Buffer, SIO_RX0_BUF_SIZE);
  			FifoInit(&rSioTx0BufferCtl, aucSioTx0Buffer, SIO_TX0_BUF_SIZE);
			/* Set Async, parity, data and stop bits */
			ctrlc = UCSR0C;
			ctrlc |= (NONE == parity) ? (0<<UPM00) : (EVEN == parity) ? (2<<UPM00) : (3<<UPM00);
			ctrlc |= (1 == stopbits) ? (0<<USBS0) : (1<<USBS0);
			ctrlc |= chsize[databits - 5];
			UCSR0C = ctrlc;
			/*set baud prescaler*/
			UBRR0H = (int8u) (baudpscaler >> 8) & 0x0F;
			UBRR0L = (int8u) baudpscaler;
			/* Enable 2X mode*/
			UCSR0A |= (clk2xmode)? (1 << U2X0) : UCSR0A;
			/* Enable receive and transmit */
			UCSR0B |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
			break;
		#ifdef AVR_ATmega2560
		case SCI_PORT_1:
			UCSR1B = 0x00; 	//disable while setting baud rate
			/* Initialize the buffers */
  			FifoInit(&rSioRx1BufferCtl, aucSioRx1Buffer, SIO_RX1_BUF_SIZE);
  			FifoInit(&rSioTx1BufferCtl, aucSioTx1Buffer, SIO_TX1_BUF_SIZE);
			/* Set Async, parity, data and stop bits */
			ctrlc = UCSR1C;
			ctrlc |= (NONE == parity) ? (0<<UPM10) : (EVEN == parity) ? (2<<UPM10) : (3<<UPM10);
			ctrlc |= (1 == stopbits) ? (0<<USBS1) : (1<<USBS1);
			ctrlc |= chsize[databits - 5];
			UCSR1C = ctrlc;
			/*set baud prescaler*/
			UBRR1H = (int8u) (baudpscaler >> 8) & 0x0F;
			UBRR1L = (int8u) baudpscaler;
			/* Enable 2X mode*/
			UCSR1A |= (clk2xmode)? (1 << U2X1) : UCSR1A;
			/* Enable receive and transmit */
			UCSR1B |= (1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1);
			break;
		case SCI_PORT_2:
			UCSR2B = 0x00; 	//disable while setting baud rate
			/* Initialize the buffers */
  			FifoInit(&rSioRx2BufferCtl, aucSioRx2Buffer, SIO_RX2_BUF_SIZE);
  			FifoInit(&rSioTx2BufferCtl, aucSioTx2Buffer, SIO_TX2_BUF_SIZE);
			/* Set Async, parity, data and stop bits */
			ctrlc = UCSR2C;
			ctrlc |= (NONE == parity) ? (0<<UPM20) : (EVEN == parity) ? (2<<UPM20) : (3<<UPM20);
			ctrlc |= (1 == stopbits) ? (0<<USBS2) : (1<<USBS2);
			ctrlc |= chsize[databits - 5];
			UCSR2C = ctrlc;
			/*set baud prescaler*/
			UBRR2H = (int8u) (baudpscaler >> 8) & 0x0F;
			UBRR2L = (int8u) baudpscaler;
			/* Enable 2X mode*/
			UCSR2A |= (clk2xmode)? (1 << U2X2) : UCSR2A;
			/* Enable receive and transmit */
			UCSR2B |= (1 << RXCIE2) | (1 << RXEN2) | (1 << TXEN2);
			break;
		case SCI_PORT_3:
			UCSR3B = 0x00; 	//disable while setting baud rate
			/* Initialize the buffers */
  			FifoInit(&rSioRx3BufferCtl, aucSioRx3Buffer, SIO_RX3_BUF_SIZE);
  			FifoInit(&rSioTx3BufferCtl, aucSioTx3Buffer, SIO_TX3_BUF_SIZE);
			/* Set Async, parity, data and stop bits */
			ctrlc = UCSR3C;
			ctrlc |= (NONE == parity) ? (0<<UPM30) : (EVEN == parity) ? (2<<UPM30) : (3<<UPM30);
			ctrlc |= (1 == stopbits) ? (0<<USBS3) : (1<<USBS3);
			ctrlc |= chsize[databits - 5];
			UCSR3C = ctrlc;
			/*set baud prescaler*/
			UBRR3H = (int8u) (baudpscaler >> 8) & 0x0F;
			UBRR3L = (int8u) baudpscaler;
			/* Enable 2X mode*/
			UCSR3A |= (clk2xmode)? (1 << U2X3) : UCSR3A;
			/* Enable receive and transmit */
			UCSR3B |= (1 << RXCIE3) | (1 << RXEN3) | (1 << TXEN3);
			break;
		#endif
		default:
			break;
	}

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

void set_tx_status(int8u port,int8u status)
{
	bTxIdle = (status << port);
}

bool get_tx_status(int8u port)
{
	return (bTxIdle & (1 << port))? true : false;
}

void set_current_sci_port(int8u port)
{
	currentSciPort = port;
}

int8u get_current_sci_port(void)
{
	return currentSciPort;
}

void SerialPutChar(int8u port,char ch)
{
	switch (port)
	{
		case SCI_PORT_0:
			FifoPutChar(&rSioTx0BufferCtl, (int8u)ch);
			break;
		case SCI_PORT_1:
			FifoPutChar(&rSioTx1BufferCtl, (int8u)ch);
			break;
		case SCI_PORT_2:
			FifoPutChar(&rSioTx2BufferCtl, (int8u)ch);
			break;
		case SCI_PORT_3:
			FifoPutChar(&rSioTx3BufferCtl, (int8u)ch);
			break;
		default:
			FifoPutChar(&rSioTx0BufferCtl, (int8u)ch);
			break;
	}
}


void SerialPut(int8u port, unsigned char *ptr, unsigned char length)
{
	while (length--)
	{
		SerialPutChar(port, *ptr++);
	}
}


void SerialPutStr(int8u port,char *str)
{
	while (!(*str == '\0'))
	{
			//UDR = *str++;
		SerialPutChar(port, *str++);
	}
}

void CommsSendString(int8u port,char *reply) // this is to match the current code
{
	SerialPutStr(port, reply);
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


/*
 * TrycSendCh -
 *
 * Try to send a character out the serial port
 * @param None
 * @return none
 * @note
 * IMPORTANT: Call this function in the main loop to send characters
 *	   out the serial port. This function will enable the UDRE
 */
void TrySendCh(void)
{

  	if(FifoIsEmpty(&rSioTx0BufferCtl) == 0) /* channel has data */ /* channel is 'allowed' to tx */
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

#ifdef AVR_ATmega2560
  	if(FifoIsEmpty(&rSioTx1BufferCtl) == 0) /* channel has data */ /* channel is 'allowed' to tx */
  	{
  		if(UCSR1A & (1 << UDRE1))
  		{
  			/* enable the UDRE interrupt */
  			UCSR1B |= (1<< UDRIE1);
  		}
  		else
  			UCSR1B |= (1 << TXCIE1);
  	}
  	else
  		UCSR1B &= ~(1 << UDRIE1);

  	if(FifoIsEmpty(&rSioTx2BufferCtl) == 0) /* channel has data */ /* channel is 'allowed' to tx */
  	{
  		if(UCSR2A & (1 << UDRE2))
  		{
  			/* enable the UDRE interrupt */
  			UCSR2B |= (1<< UDRIE2);
  		}
  		else
  			UCSR2B |= (1 << TXCIE2);
  	}
  	else
  		UCSR2B &= ~(1 << UDRIE2);

  	if(FifoIsEmpty(&rSioTx3BufferCtl) == 0) /* channel has data */ /* channel is 'allowed' to tx */
  	{
  		if(UCSR3A & (1 << UDRE3))
  		{
  			/* enable the UDRE interrupt */
  			UCSR3B |= (1<< UDRIE3);
  		}
  		else
  			UCSR3B |= (1 << TXCIE3);
	}
  	else
  		UCSR3B &= ~(1 << UDRIE3);
#endif
}


void SerialGet(char *ptr, int8u length)
{
	while (length--)
	{
		*ptr++ = SerialGetChar();
	}
}

void ReadSerial(void)
{
	int8u ch;
	int8u temp;
	int8u cport;
	bool bDynPacket = false;


	if(FifoIsEmpty(&rSioRx0BufferCtl) == 0)
	{
		ch = FifoGetChar(&rSioRx0BufferCtl);
		//state machine implmentation to detect dynamixel packet
		cport = get_current_sci_port();
		set_current_sci_port(SCI_PORT_0);
		runDynStateMachine(ch);
		set_current_sci_port(cport);
	}
#ifdef AVR_ATmega2560
	if(FifoIsEmpty(&rSioRx1BufferCtl) == 0)
	{
		ch = FifoGetChar(&rSioRx1BufferCtl);
		//state machine implmentation to detect dynamixel packet
		cport = get_current_sci_port();
		set_current_sci_port(SCI_PORT_1);
		runDynStateMachine(ch);
		set_current_sci_port(cport);
	}

	if(FifoIsEmpty(&rSioRx2BufferCtl) == 0)
	{
		ch = FifoGetChar(&rSioRx2BufferCtl);
		//state machine implmentation to detect dynamixel packet
		cport = get_current_sci_port();
		set_current_sci_port(SCI_PORT_2);
		runDynStateMachine(ch);
		set_current_sci_port(cport);
	}

	if(FifoIsEmpty(&rSioRx3BufferCtl) == 0)
	{
		ch = FifoGetChar(&rSioRx3BufferCtl);
		//state machine implmentation to detect dynamixel packet
		cport = get_current_sci_port();
		set_current_sci_port(SCI_PORT_3);
		runDynStateMachine(ch);
		set_current_sci_port(cport);
	}
#endif
}

void ProcessRxPacket(void)
{
	int8u cport;

	if(is_dyn_msg_received(SCI_PORT_0))
	{
		cport = get_current_sci_port();
		set_current_sci_port(SCI_PORT_0);
		dynRxPacketProcess();
		set_current_sci_port(cport);
		//CommsSendString("TesPass\r\n");
	}
#ifdef AVR_ATmega2560
	if(is_dyn_msg_received(SCI_PORT_1))
	{
		cport = get_current_sci_port();
		set_current_sci_port(SCI_PORT_1);
		dynRxPacketProcess();
		set_current_sci_port(cport);
		//CommsSendString("TesPass\r\n");
	}
	if(is_dyn_msg_received(SCI_PORT_2))
	{
		cport = get_current_sci_port();
		set_current_sci_port(SCI_PORT_2);
		dynRxPacketProcess();
		set_current_sci_port(cport);
		//CommsSendString("TesPass\r\n");
	}
	if(is_dyn_msg_received(SCI_PORT_3))
	{
		cport = get_current_sci_port();
		set_current_sci_port(SCI_PORT_3);
		dynRxPacketProcess();
		set_current_sci_port(cport);
		//CommsSendString("TesPass\r\n");
	}
#endif
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

void SerialIncommingHandler(void)
{
	//memset(message, '\0', arlen(message));
	ReadSerial();
	ProcessRxPacket();
}


/////////..............INTERRUPT...............///////

/* USART0, Rx Complete Interrupt handler*/
ISR(USART_RX_vect)
{
	int8u ucStatus;
	int8u ucCh;
	int8u err;

	cli();

	ucStatus = UCSR0A;
	ucCh = UDR0;
	err = ((1 << FE0) | (1 << UPE0) | (1 << DOR0));
	//debug_blink();
	//debug_debug_pin();
	//PORTB ^= (1 << PB4);
	if ((ucStatus & err) == 0x00)
	{ 
		/* only store if no errors (any of FE, DOR, PE will fail write) */
		FifoPutChar(&rSioRx0BufferCtl, ucCh); /* if no space, this will not store */
		/* Count the number of messages received */
		//SerialPutChar(ucCh);
		//if(is_last_char(ucCh))
		//	sci_set_new_message(sci_get_new_message() + 1);
	}

	sei(); // Enable global interrupts
}

/* USART0, Data Register Empty Interrupt handler*/
ISR(USART_UDRE_vect)
{
	if (FifoIsEmpty(&rSioTx0BufferCtl) == 0)
	{ /* data still in current stream */
		dyn_ax_18a_start_tx(SCI_PORT_0);
		UDR0 = FifoGetChar(&rSioTx0BufferCtl);
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

/* USART0, Tx Complete Interrupt handler*/
ISR(USART_TX_vect)
	{ /* whatever was being transmitted has completed */
	set_tx_status(SCI_PORT_0,TRUE);
	dyn_ax_18a_end_tx(SCI_PORT_0);
	/* disable this interrupt */
	UCSR0B &= ~(1 << TXCIE0);
}

#ifdef AVR_ATmega2560
/* USART1, Rx Complete Interrupt handler*/
ISR(USART1_RX_vect)
{
	int8u ucStatus;
	int8u ucCh;
	int8u err;

	cli();

	ucStatus = UCSR1A;
	ucCh = UDR1;
	err = ((1 << FE1) | (1 << UPE1) | (1 << DOR1));
	if ((ucStatus & err) == 0x00)
	{ 
		/* only store if no errors (any of FE, DOR, PE will fail write) */
		FifoPutChar(&rSioRx1BufferCtl, ucCh); /* if no space, this will not store */
	}

	sei(); // Enable global interrupts
}

/* USART1, Data Register Empty Interrupt handler*/
ISR(USART1_UDRE_vect)
{
	if (FifoIsEmpty(&rSioTx1BufferCtl) == 0)
	{ /* data still in current stream */
		dyn_ax_18a_start_tx(SCI_PORT_1);
		UDR1 = FifoGetChar(&rSioTx1BufferCtl);
		UCSR1A |= (1 << TXC1);
	}
	else
	{ /* DTR is holding things up on this channel */
		/* disable this (the UDR) interrupt */
		UCSR1B &= ~(1 << UDRIE1);
		/* enable the TXC flag */
		UCSR1B |= (1 << TXCIE1);
	}
}

/* USART1, Tx Complete Interrupt handler*/
ISR(USART1_TX_vect)
	{ /* whatever was being transmitted has completed */
	set_tx_status(SCI_PORT_1,TRUE);
	dyn_ax_18a_end_tx(SCI_PORT_1);
	/* disable this interrupt */
	UCSR1B &= ~(1 << TXCIE1);
}

/* USART2, Rx Complete Interrupt handler*/
ISR(USART2_RX_vect)
{
	int8u ucStatus;
	int8u ucCh;
	int8u err;

	cli();

	ucStatus = UCSR2A;
	ucCh = UDR2;
	err = ((1 << FE2) | (1 << UPE2) | (1 << DOR2));
	if ((ucStatus & err) == 0x00)
	{ 
		/* only store if no errors (any of FE, DOR, PE will fail write) */
		FifoPutChar(&rSioRx2BufferCtl, ucCh); /* if no space, this will not store */
	}

	sei(); // Enable global interrupts
}

/* USART2, Data Register Empty Interrupt handler*/
ISR(USART2_UDRE_vect)
{
	if (FifoIsEmpty(&rSioTx2BufferCtl) == 0)
	{ /* data still in current stream */
		dyn_ax_18a_start_tx(SCI_PORT_2);
		UDR2 = FifoGetChar(&rSioTx2BufferCtl);
		UCSR2A |= (1 << TXC2);
	}
	else
	{ /* DTR is holding things up on this channel */
		/* disable this (the UDR) interrupt */
		UCSR2B &= ~(1 << UDRIE2);
		/* enable the TXC flag */
		UCSR2B |= (1 << TXCIE2);
	}
}

/* USART2, Tx Complete Interrupt handler*/
ISR(USART2_TX_vect)
	{ /* whatever was being transmitted has completed */
	set_tx_status(SCI_PORT_2,TRUE);
	dyn_ax_18a_end_tx(SCI_PORT_2);
	/* disable this interrupt */
	UCSR2B &= ~(1 << TXCIE2);
}

/* USART3, Rx Complete Interrupt handler*/
ISR(USART3_RX_vect)
{
	int8u ucStatus;
	int8u ucCh;
	int8u err;

	cli();

	ucStatus = UCSR3A;
	ucCh = UDR3;
	err = ((1 << FE3) | (1 << UPE3) | (1 << DOR3));
	if ((ucStatus & err) == 0x00)
	{ 
		/* only store if no errors (any of FE, DOR, PE will fail write) */
		FifoPutChar(&rSioRx3BufferCtl, ucCh); /* if no space, this will not store */
	}

	sei(); // Enable global interrupts
}

/* USART3, Data Register Empty Interrupt handler*/
ISR(USART3_UDRE_vect)
{
	if (FifoIsEmpty(&rSioTx3BufferCtl) == 0)
	{ /* data still in current stream */
		dyn_ax_18a_start_tx(SCI_PORT_3);
		UDR3 = FifoGetChar(&rSioTx3BufferCtl);
		UCSR3A |= (1 << TXC3);
	}
	else
	{ /* DTR is holding things up on this channel */
		/* disable this (the UDR) interrupt */
		UCSR3B &= ~(1 << UDRIE3);
		/* enable the TXC flag */
		UCSR3B |= (1 << TXCIE3);
	}
}

/* USART3, Tx Complete Interrupt handler*/
ISR(USART3_TX_vect)
	{ /* whatever was being transmitted has completed */
	set_tx_status(SCI_PORT_3,TRUE);
	dyn_ax_18a_end_tx(SCI_PORT_3);
	/* disable this interrupt */
	UCSR3B &= ~(1 << TXCIE3);
}
#endif


