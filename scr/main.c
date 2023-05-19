#include "avrdet.h"
#include "serial.h"
#include "timer0.h"
#include "types.h"
#include "iocontrol.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void bg_10(void)
{

}

void bg_100(void)
{
    static bool flip = true;

    digitalWrite(D13, flip);
    flip = !flip; 
}

void bg_1000(void)
{
    SerialHandler();
}

void bg_tick(void)
{

}

void time_update(void)
{
	static int8u tick = 0;
	static int8u deci = 0;
	static int8u sec = 0;

	bg_tick();

	if(tick++ >= 10)
	{
		bg_10();
		deci++;
		tick = 0;
	}
	if(deci >= 10)
	{
		bg_100();
		sec++;
		deci = 0;
	}
	if(sec >= 10)
	{
		bg_1000();
		sec = 0;
	}
}

int main(void)
{
    pinMode(D13, OUTPUT);
    SerialInit(115200,NONE,EIGHT,ONE);
    Timer0Init(time_update);

    sei(); // Enable global interrupts

    while(1)
    {
        //_delay_ms(1000);
        //SerialHandler();
        //PORTB ^= 1<<5; 
        
    }
    return 0;
}