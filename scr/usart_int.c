/*
 * usart_int.c
 *
 *  Created on: 17/05/2023
 *      Author: Janaka
 */
#include "avrdet.h"
#include "usart_int.h"

#include <avr/io.h>      // Contains all the I/O Register Macros
#include <util/delay.h>  // Generates a Blocking Delay
#include <avr/interrupt.h> // Contains all interrupt vectors

#define BAUD_TO_BAUDCTRL(baud, freq, bscale) ((freq / ((1 << bscale) * 16) / baud) - 1)
#define BAUD_TO_BAUDCTRL_CLK2X(baud, freq, bscale) ((freq / ((1 << bscale) * 8) / baud) - 1)

#define BSCALE 0
//#define BACALE_LOW_BAUD	2

//#define BAUD_TO_BAUDCTRL_LOW_BAUD(baud) BAUD_TO_BAUDCTRL(baud, FREQ, BACALE_LOW_BAUD)
#define BAUD_TO_BAUDCTRL_FIXED(baud) BAUD_TO_BAUDCTRL(baud, F_CPU, BSCALE)
#define BAUD_TO_BAUDCTRL_FIXED_CLK2X(baud) BAUD_TO_BAUDCTRL_CLK2X(baud, FREQ, BSCALE)

#define USART_BAUDRATE 9600 // Desired Baud Rate
#define BAUD_PRESCALER (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define ASYNCHRONOUS (0<<UMSEL00) // USART Mode Selection

#define DISABLED    (0<<UPM00)
#define EVEN_PARITY (2<<UPM00)
#define ODD_PARITY  (3<<UPM00)
#define PARITY_MODE  DISABLED // USART Parity Bit Selection

#define ONE_BIT (0<<USBS0)
#define TWO_BIT (1<<USBS0)
#define STOP_BIT ONE_BIT      // USART Stop Bit Selection

#define FIVE_BIT  (0<<UCSZ00)
#define SIX_BIT   (1<<UCSZ00)
#define SEVEN_BIT (2<<UCSZ00)
#define EIGHT_BIT (3<<UCSZ00)
#define DATA_BIT   EIGHT_BIT  // USART Data Bit Selection

#define RX_COMPLETE_INTERRUPT         (1<<RXCIE0)
#define DATA_REGISTER_EMPTY_INTERRUPT (1<<UDRIE0)

volatile uint8_t USART_TransmitBuffer; // Global Buffer

// USART hardware buffer for first USART
struct s_usart usart0;
uint8_t rxbuf0[USART_RX_BUFSIZE];
uint8_t txbuf0[USART_TX_BUFSIZE];

void USART_Init()
{
	// Set Baud Rate

	//UBRR0H = BAUD_PRESCALER >> 8;
	//UBRR0L = BAUD_PRESCALER;
	
	// Set Frame Format

	//UCSR0C = ASYNCHRONOUS | PARITY_MODE | STOP_BIT | DATA_BIT;
	
	// Enable Receiver and Transmitter

	//UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	
	//Enable Global Interrupts

	//sei();

  struct s_usart *usart = &usart0;

  // initialize the hardware buffer
  usart_int_initbuf(usart, rxbuf0, USART_RX_BUFSIZE, txbuf0, USART_TX_BUFSIZE);
  // configure the hardware
  usart_int_configure(usart, 9600, 'n', 8, 1);
}

void USART_TransmitInterrupt(uint8_t Buffer)
{
	USART_TransmitBuffer = Buffer;
	UCSR0B |= DATA_REGISTER_EMPTY_INTERRUPT; // Enables the Interrupt
}

/*
int main()
{
	DDRB |= 1 << 5; // Configuring PB5 / D13 as Output
	uint8_t LocalData = 'a';
	USART_Init();
	USART_TransmitInterrupt(LocalData);
	
	while (1)
	{
		PORTB |= 1<<5; // Writing HIGH to glow LED
		_delay_ms(500);
		PORTB &= ~(1<<5); // Writing LOW
		_delay_ms(500);		
	}
	
	return 0;
}
*/

/*
 * Initialise usart as specified in the parameters
 *
 * @param usart is the usart structure to initialise
 * @param hardware is usart hardware pointer
 * @param rxbuf is the receive buffer
 * @param rxbuf_size is the size of the receive buffer
 * @param txbuf is the transmit buffer
 * @param txbuf_size is the size of the transmit buffer
 */
void usart_int_initbuf(struct s_usart *usart, uint8_t *rxbuf, uint8_t rxbuf_size, uint8_t *txbuf, uint8_t txbuf_size)
{
  FifoInit(&usart->in_fifo, rxbuf, rxbuf_size);
  FifoInit(&usart->out_fifo, txbuf, txbuf_size);
}

/*
 * Initialise usart as specified in the parameters
 *
 * @param usart is the hardware uart to use
 * @param baud is the baud rate
 * @param parity is 'n', 'o' or 'e'
 * @param databits is 5, 6, 7, 8, 9
 * @param stopbits is 1, 2
 *
 */
void usart_int_configure(struct s_usart *usart, uint32_t baud, char parity, uint8_t databits, uint8_t stopbits)
{
  const uint8_t chsize[] = { (0<<UCSZ00), (1<<UCSZ00), (2<<UCSZ00), (3<<UCSZ00) };
  uint8_t ctrlc;
  uint16_t baudpscaler;

  cli(); // Disable global interrupts

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
  UBRR0H = baudpscaler >> 8;
	UBRR0L = baudpscaler;

  /* Enable receive and transmit */
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);

  sei(); // Enable global interrupts
}

bool usart_send_string(char *ch)
{
  bool status = false;
  struct s_usart *usart = &usart0;

  while (*ch)
  {
    status = usart_int_putchar(usart, (uint8_t)*ch);
    if(!status)
    {
      PORTB |= 1<<5; 
      break;
    }
    ch++;
  }
  return status;
}

bool usart_send_character(char ch)
{
  bool status = false;
  struct s_usart *usart = &usart0;

  status = usart_int_putchar(usart, (uint8_t)ch);
  return status;
}

/** Send a character out the serial port */
bool usart_int_putchar(struct s_usart *usart, uint8_t ch)
{
  bool result;
  uint8_t bufchar;
  uint8_t sreg;
  uint8_t ctrlb;
  uint8_t txeninturrpt;

  sreg = SREG;
  SREG = 0;

  ctrlb = UCSR0B;
  txeninturrpt = (1<<UDRIE0);
  if ((ctrlb & txeninturrpt) == txeninturrpt)
  {
    /* Currently transmitting, so store into fifo */
    result = FifoPutChar(&usart->out_fifo, ch);
  }
  else
  {
    /* Not currently transmitting, so just write to transmit register and enable DRE interrupt */
    UDR0 = ch;
    ctrlb |= txeninturrpt;
    UCSR0B = ctrlb;
    result = true;
  }
 
  SREG = sreg;
  return result;
}

ISR(USART_UDRE_vect)
{
  uint8_t ch;
  uint8_t ctrlb;
  uint8_t txeninturrpt;

  ctrlb = UCSR0B;
  txeninturrpt = (1<<UDRIE0);
  if(FifoGetChar(&usart0.out_fifo, &ch))
	  UDR0 = ch;
  else
  {
    /* Disables the Interrupt if the FIFO is empty */
    ctrlb &= ~txeninturrpt;
    UCSR0B = ctrlb;
  }
}

/** Get a character from the serial port */
bool usart_int_getchar(struct s_usart *usart, uint8_t *ch)
{
  if (NULL == usart || NULL == ch)
    return false;

  return FifoGetChar(&usart->in_fifo, ch);
}


/** Get the number of characters in the receive buffer */
int usart_int_getrxlevel(struct s_usart *usart)
{
  if (NULL == usart)
    return 0;
  return FifoUsed(&usart->in_fifo);
}


/** Get if the receive buffer is full */
bool usart_int_getrxfull(struct s_usart *usart)
{
  if (NULL == usart)
    return 0;
  return (FifoFree(&usart->in_fifo) == 0);
}


/** Flush the receive buffer */
void usart_int_flushrx(struct s_usart *usart)
{
  if (NULL == usart)
      return;
  FifoFlushBuffer(&usart->in_fifo);
}

/** Get last character from the serial port */
bool usart_int_getlastchar(struct s_usart *usart, uint8_t *ch)
{
  if (NULL == usart || NULL == ch)
    return false;

  return FifoGetLastChar(&usart->in_fifo, ch);
}