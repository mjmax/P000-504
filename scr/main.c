#define F_CPU 16000000UL
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRB |= 1<<5;
    while(1)
    {
        _delay_ms(500);
        PORTB ^= 1<<5; 
    }
    return 0;
}