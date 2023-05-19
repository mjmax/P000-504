/*
 * iocontrol.c
 *
 *  Created on: May 19,2023
 *      Author: Janaka Madhusankha
 */
#include "avrdet.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "types.h"
#include "iocontrol.h"
#include "command.h"
//#include "registers.h"
//#include "eeprom.h"
//#include "eeprom_map.h"

#define DIRB DDRB
#define DIRC DDRC
#define DIRD DDRD

#define RPINB	PINB
#define RPINC	PINC
#define RPIND	PIND

#define ROUTB PORTB
#define ROUTC PORTC
#define ROUTD PORTD

// don't change the order of the following array
const int8u PinMap[] = {PIN0, PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7, PIN0, PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7, PIN0, PIN1, PIN2, PIN3, PIN4, PIN5};

int8u pinMode(int8u pin, int8u mode)
{
	int8u status = 0;
	int8u port = pin >> 3;
	int8u pinNum = pin & 0x07;

	switch(port)
	{
	case 0:
		if(mode)
			DIRC |= (1 << pinNum);
		else
			DIRC &= ~(1 << pinNum);
		status = 1;
		break;
	case 1:
		if(mode)
			DIRD |= (1 << pinNum);
		else
			DIRD &= ~(1 << pinNum);
		status = 1;
		break;
	case 2:
		if(mode)
			DIRB |= (1 << pinNum);
		else
			DIRB &= ~(1 << pinNum);
		status = 1;
		break;
	}
	return status;
}

int8u digitalWrite(int8u pin, int8u value)
{
	int8u status = 0;
	int8u port = pin >> 3;
	int8u pinNum = pin & 0x07;

	switch(port)
	{
	case 0:
		if(value)
			ROUTC |= (1 << pinNum);
		else
			ROUTC &= ~(1 << pinNum);
		status = 1;
		break;
	case 1:
		if(value)
			ROUTD |= (1 << pinNum);
		else
			ROUTD &= ~(1 << pinNum);
		status = 1;
		break;
	case 2:
		if(value)
			ROUTB |= (1 << pinNum);
		else
			ROUTB &= ~(1 << pinNum);
		status = 1;
		break;
	}
	return status;
}

int8u digitalRead(int8u pin)
{
	int8u status = 0;
	int8u port = pin >> 3;
	int8u pinNum = pin & 0x07;

	switch(port)
	{
	case 0:
		status = (RPINC & (1 << pinNum)) >> pinNum;
		break;
	case 1:
		status = (RPIND & (1 << pinNum)) >> pinNum;
		break;
	case 2:
		status = (RPINB & (1 << pinNum)) >> pinNum;
		break;
	}

	return status;
}