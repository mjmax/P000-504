/*
 * usart_int.h
 *
 *  Created on: 16/09/2015
 *      Author: IanB
 */

#ifndef USART_INT_H_
#define USART_INT_H_

#include "avrdet.h"
#include "fifo.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#define USART_RX_BUFSIZE 254
#define USART_TX_BUFSIZE 254

struct s_usart
{
  //volatile USART_t *hardware;             // will be usefull in other MCU
  struct s_fifo_ctl in_fifo;
  struct s_fifo_ctl out_fifo;
};

void usart_int_initbuf(struct s_usart *usart, uint8_t *rxbuf, uint8_t rxbuf_size, uint8_t *txbuf, uint8_t txbuf_size);
void usart_int_configure(struct s_usart *usart, uint32_t baud, char parity, uint8_t databits, uint8_t stopbits);

bool usart_send_string(char *ch);
bool usart_send_character(char ch);

bool usart_int_putchar(struct s_usart *usart, uint8_t ch);
bool usart_int_getchar(struct s_usart *usart, uint8_t *ch);
int usart_int_getrxlevel(struct s_usart *usart);
bool usart_int_getrxfull(struct s_usart *usart);
void usart_int_flushrx(struct s_usart *usart);
bool usart_int_getlastchar(struct s_usart *usart, uint8_t *ch);

void USART_Init();
void USART_TransmitInterrupt(uint8_t Buffer);

#endif /* USART_INT_H_ */
