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

#ifdef AVR_ATmega2560
#define DIRA DDRA
#define DIRE DDRE
#define DIRF DDRF
#define DIRG DDRG
#define DIRH DDRH
#define DIRJ DDRJ
#define DIRK DDRK
#define DIRL DDRL
#endif


#define RPINB	PINB
#define RPINC	PINC
#define RPIND	PIND

#ifdef AVR_ATmega2560
#define RPINA	PINA
#define RPINE	PINE
#define RPINF	PINF
#define RPING	PING
#define RPINH	PINH
#define RPINJ	PINJ
#define RPINK	PINK
#define RPINL	PINL
#endif


#define ROUTB PORTB
#define ROUTC PORTC
#define ROUTD PORTD

#ifdef AVR_ATmega2560
#define ROUTA PORTA
#define ROUTE PORTE
#define ROUTF PORTF
#define ROUTG PORTG
#define ROUTH PORTH
#define ROUTJ PORTJ
#define ROUTK PORTK
#define ROUTL PORTL
#endif

// don't change the order of the following array
const int8u PinMap[] = {PIN0, PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7, PIN0, PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7, PIN0, PIN1, PIN2, PIN3, PIN4, PIN5};

int8u pinMode(int8u pin, int8u mode)
{
	int8u status = 0;
	int8u port = 0;
	int8u pinNum = 0;
#ifdef AVR_ATmega328P
	port = pin >> 3;
	pinNum = pin & 0x07;
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
#elif AVR_ATmega2560
	port = pin >> 3;
	pinNum = pin & 0x07;
	switch(port)
	{
	case 0:
		if(mode)
			DIRF |= (1 << pinNum);
		else
			DIRF &= ~(1 << pinNum);
		status = 1;
		break;
	case 1:
		if(mode)
			DIRK |= (1 << pinNum);
		else
			DIRK &= ~(1 << pinNum);
		status = 1;
		break;
	case 2:
		switch(pinNum)
		{
		case 0:
		case 1:
			if(mode)
				DIRE |= (1 << pinNum);
			else
				DIRE &= ~(1 << pinNum);
			status = 1;
			break;
		case 2:
		case 3:
			if(mode)
				DIRE |= (1 << (pinNum + 2));
			else
				DIRE &= ~(1 << (pinNum + 2));
			status = 1;
			break;
		case 4:
			if(mode)
				DIRG |= (1 << (pinNum + 1));
			else
				DIRG &= ~(1 << (pinNum + 1));
			status = 1;
			break;
		case 5:
			if(mode)
				DIRE |= (1 << (pinNum - 2));
			else
				DIRE &= ~(1 << (pinNum - 2));
			status = 1;
			break;
		case 6:
		case 7:
			if(mode)
				DIRH |= (1 << (pinNum - 3));
			else
				DIRH &= ~(1 << (pinNum - 3));
			status = 1;
			break;
		}
		break;	
	case 3:
		switch(pinNum)
		{
			case 0:
			case 1:
				if(mode)
					DIRH |= (1 << (pinNum + 5));
				else
					DIRH &= ~(1 << (pinNum + 5));
				status = 1;
				break;
			case 2:
			case 3:
			case 4:
			case 5:
				if(mode)
					DIRB |= (1 << (pinNum + 2));
				else
					DIRB &= ~(1 << (pinNum + 2));
				status = 1;
				break;
			case 6:
				if(mode)
					DIRJ |= (1 << (pinNum - 5));
				else
					DIRJ &= ~(1 << (pinNum - 5));
				status = 1;
				break;
			case 7:
				if(mode)
					DIRJ |= (1 << (pinNum - 7));
				else
					DIRJ &= ~(1 << (pinNum - 7));
				status = 1;
				break;
		}
		break;
	case 4:
		switch (pinNum)
		{
			case 0:
				if(mode)
					DIRH |= (1 << (pinNum + 1));
				else
					DIRH &= ~(1 << (pinNum + 1));
				status = 1;
				break;
			case 1:
				if(mode)
					DIRH |= (1 << (pinNum - 1));
				else
					DIRH &= ~(1 << (pinNum - 1));
				status = 1;
				break;
			case 2:
			case 3:
			case 4:
			case 5:
				if(mode)
					DIRD |= (1 << (5 - pinNum));
				else
					DIRD &= ~(1 << (5 - pinNum));
				status = 1;
				break;
			case 6:
			case 7:
				if(mode)
					DIRA |= (1 << (pinNum - 6));
				else
					DIRA &= ~(1 << (pinNum - 6));
				status = 1;
				break;
		}
		break;
	case 5:
		switch(pinNum)
		{
			case 6:
			case 7:
				if(mode)
					DIRC |= (1 << (13 - pinNum));
				else
					DIRC &= ~(1 << (13 - pinNum));
				status = 1;
				break;
			default:
				if(mode)
					DIRA |= (1 << (pinNum + 2));
				else
					DIRA &= ~(1 << (pinNum + 2));
				status = 1;
				break;
		}
		break;
	case 6:
		switch(pinNum)
		{
			case 6:
				if(mode)
					DIRD |= (1 << (pinNum + 1));
				else
					DIRD &= ~(1 << (pinNum + 1));
				status = 1;
				break;
			case 7:
				if(mode)
					DIRG |= (1 << (pinNum - 5));
				else
					DIRG &= ~(1 << (pinNum - 5));
				status = 1;
				break;
			default:
				if(mode)
					DIRC |= (1 << (5 - pinNum));
				else
					DIRC &= ~(1 << (5 - pinNum));
				status = 1;
				break;
		}
		break;
	case 7:
		switch (pinNum)
		{
			case 0:
			case 1:
				if(mode)
					DIRG |= (1 << (1 - pinNum));
				else
					DIRG &= ~(1 << (1 - pinNum));
				status = 1;
				break;
			default:
				if(mode)
					DIRL |= (1 << (9 - pinNum));
				else
					DIRL &= ~(1 << (9 - pinNum));
				status = 1;
				break;
		}
		break;
	case 8:
		switch(pinNum)
		{
			case 0:
			case 1:
				if(mode)
					DIRL |= (1 << (1 - pinNum));
				else
					DIRL &= ~(1 << (1 - pinNum));
				status = 1;
				break;
			case 2:
			case 3:
			case 4:
			case 5:
				if(mode)
					DIRB |= (1 << (5 - pinNum));
				else
					DIRB &= ~(1 << (5 - pinNum));
				status = 1;
				break;
			case 6:
			case 7:
				if(mode)
					DIRF |= (1 << (6 - pinNum));
				else
					DIRF &= ~(1 << (6 - pinNum));
				status = 1;
				break;
		}
		break;
	case 9:
		switch (pinNum)
		{
			case 6:
			case 7:
				if(mode)
					DIRK |= (1 << (6 - pinNum));
				else
					DIRK &= ~(1 << (6 - pinNum));
				status = 1;
				break;
			default:
				if(mode)
					DIRF |= (1 << (pinNum + 2));
				else
					DIRF &= ~(1 << (pinNum + 2));
				status = 1;
				break;
		}
		break;
	case 10:
		switch (pinNum)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				if(mode)
					DIRK |= (1 << (pinNum + 2));
				else
					DIRK &= ~(1 << (pinNum + 2));
				status = 1;
				break;
		}
		break;
	}

#endif

	return status;
}

int8u digitalWrite(int8u pin, int8u value)
{
	int8u status = 0;
	int8u port = pin >> 3;
	int8u pinNum = pin & 0x07;

#ifdef AVR_ATmega328P
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
#elif AVR_ATmega2560
	port = pin >> 3;
	pinNum = pin & 0x07;
	switch(port)
	{
	case 0:
		if(value)
			ROUTF |= (1 << pinNum);
		else
			ROUTF &= ~(1 << pinNum);
		status = 1;
		break;
	case 1:
		if(value)
			ROUTK |= (1 << pinNum);
		else
			ROUTK &= ~(1 << pinNum);
		status = 1;
		break;
	case 2:
		switch(pinNum)
		{
		case 0:
		case 1:
			if(value)
				ROUTE |= (1 << pinNum);
			else
				ROUTE &= ~(1 << pinNum);
			status = 1;
			break;
		case 2:
		case 3:
			if(value)
				ROUTE |= (1 << (pinNum + 2));
			else
				ROUTE &= ~(1 << (pinNum + 2));
			status = 1;
			break;
		case 4:
			if(value)
				ROUTG |= (1 << (pinNum + 1));
			else
				ROUTG &= ~(1 << (pinNum + 1));
			status = 1;
			break;
		case 5:
			if(value)
				ROUTE |= (1 << (pinNum - 2));
			else
				ROUTE &= ~(1 << (pinNum - 2));
			status = 1;
			break;
		case 6:
		case 7:
			if(value)
				ROUTH |= (1 << (pinNum - 3));
			else
				ROUTH &= ~(1 << (pinNum - 3));
			status = 1;
			break;
		}
		break;	
	case 3:
		switch(pinNum)
		{
			case 0:
			case 1:
				if(value)
					ROUTH |= (1 << (pinNum + 5));
				else
					ROUTH &= ~(1 << (pinNum + 5));
				status = 1;
				break;
			case 2:
			case 3:
			case 4:
			case 5:
				if(value)
					ROUTB |= (1 << (pinNum + 2));
				else
					ROUTB &= ~(1 << (pinNum + 2));
				status = 1;
				break;
			case 6:
				if(value)
					ROUTJ |= (1 << (pinNum - 5));
				else
					ROUTJ &= ~(1 << (pinNum - 5));
				status = 1;
				break;
			case 7:
				if(value)
					ROUTJ |= (1 << (pinNum - 7));
				else
					ROUTJ &= ~(1 << (pinNum - 7));
				status = 1;
				break;
		}
		break;
	case 4:
		switch (pinNum)
		{
			case 0:
				if(value)
					ROUTH |= (1 << (pinNum + 1));
				else
					ROUTH &= ~(1 << (pinNum + 1));
				status = 1;
				break;
			case 1:
				if(value)
					ROUTH |= (1 << (pinNum - 1));
				else
					ROUTH &= ~(1 << (pinNum - 1));
				status = 1;
				break;
			case 2:
			case 3:
			case 4:
			case 5:
				if(value)
					ROUTD |= (1 << (5 - pinNum));
				else
					ROUTD &= ~(1 << (5 - pinNum));
				status = 1;
				break;
			case 6:
			case 7:
				if(value)
					ROUTA |= (1 << (pinNum - 6));
				else
					ROUTA &= ~(1 << (pinNum - 6));
				status = 1;
				break;
		}
		break;
	case 5:
		switch(pinNum)
		{
			case 6:
			case 7:
				if(value)
					ROUTC |= (1 << (13 - pinNum));
				else
					ROUTC &= ~(1 << (13 - pinNum));
				status = 1;
				break;
			default:
				if(value)
					ROUTA |= (1 << (pinNum + 2));
				else
					ROUTA &= ~(1 << (pinNum + 2));
				status = 1;
				break;
		}
		break;
	case 6:
		switch(pinNum)
		{
			case 6:
				if(value)
					ROUTD |= (1 << (pinNum + 1));
				else
					ROUTD &= ~(1 << (pinNum + 1));
				status = 1;
				break;
			case 7:
				if(value)
					ROUTG |= (1 << (pinNum - 5));
				else
					ROUTG &= ~(1 << (pinNum - 5));
				status = 1;
				break;
			default:
				if(value)
					ROUTC |= (1 << (5 - pinNum));
				else
					ROUTC &= ~(1 << (5 - pinNum));
				status = 1;
				break;
		}
		break;
	case 7:
		switch (pinNum)
		{
			case 0:
			case 1:
				if(value)
					ROUTG |= (1 << (1 - pinNum));
				else
					ROUTG &= ~(1 << (1 - pinNum));
				status = 1;
				break;
			default:
				if(value)
					ROUTL |= (1 << (9 - pinNum));
				else
					ROUTL &= ~(1 << (9 - pinNum));
				status = 1;
				break;
		}
		break;
	case 8:
		switch(pinNum)
		{
			case 0:
			case 1:
				if(value)
					ROUTL |= (1 << (1 - pinNum));
				else
					ROUTL &= ~(1 << (1 - pinNum));
				status = 1;
				break;
			case 2:
			case 3:
			case 4:
			case 5:
				if(value)
					ROUTB |= (1 << (5 - pinNum));
				else
					ROUTB &= ~(1 << (5 - pinNum));
				status = 1;
				break;
			case 6:
			case 7:
				if(value)
					ROUTF |= (1 << (6 - pinNum));
				else
					ROUTF &= ~(1 << (6 - pinNum));
				status = 1;
				break;
		}
		break;
	case 9:
		switch (pinNum)
		{
			case 6:
			case 7:
				if(value)
					ROUTK |= (1 << (6 - pinNum));
				else
					ROUTK &= ~(1 << (6 - pinNum));
				status = 1;
				break;
			default:
				if(value)
					ROUTF |= (1 << (pinNum + 2));
				else
					ROUTF &= ~(1 << (pinNum + 2));
				status = 1;
				break;
		}
		break;
	case 10:
		switch (pinNum)
		{
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
				if(value)
					ROUTK |= (1 << (pinNum + 2));
				else
					ROUTK &= ~(1 << (pinNum + 2));
				status = 1;
				break;
		}
		break;
	}

#endif
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