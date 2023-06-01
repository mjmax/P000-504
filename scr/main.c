#include "avrdet.h"
#include "serial.h"
#include "timer0.h"
#include "types.h"
#include "iocontrol.h"
#include "dyn_ax18a.h"
#include "debug.h"
#include "controller.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void bg_10(void)
{

}

void bg_100(void)
{
    //debug_blink();
    //static bool flip = true;

    //digitalWrite(D13, flip);
    //flip = !flip; 
}

void bg_1000(void)
{
    static int8u sec = 0;
    int8u samplingTime = 20;
    //CommsSendString(SCI_PORT_0,"SerialHandler\r\n");
    //SerialHandler();
    //dyn_test_servo();
    //dyn_test_received_position();
    sec++;
    if(sec>=samplingTime)
    {
        set_ready_to_sample(SCI_PORT_1, true);
        set_ready_to_sample(SCI_PORT_2, true);
        set_ready_to_sample(SCI_PORT_3, true);
        set_ready_to_send(true);
        sec = 0;
    }

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
    //pinMode(D13, OUTPUT);
    degug_init();
    DynAx18aInit();
    //SerialInit(9600,NONE,EIGHT,ONE);
    SerialInit(SCI_PORT_0,57600,NONE,EIGHT,ONE);
    SerialInit(SCI_PORT_1,57600,NONE,EIGHT,ONE);
    SerialInit(SCI_PORT_2,57600,NONE,EIGHT,ONE);
    SerialInit(SCI_PORT_3,57600,NONE,EIGHT,ONE);
    //SerialInit(115200,NONE,EIGHT,ONE);
    //SerialInit(1000000,NONE,EIGHT,ONE);
    //SerialInit(2000000,NONE,EIGHT,ONE);
    Timer0Init(time_update);

    sei(); // Enable global interrupts

    while(1)
    {
        //_delay_ms(1000);
        test_control();
        SerialIncommingHandler();
    	DynTxPacketProcess();
        
	    TrySendCh();				// do not move this line (keep at last)
    }
    return 0;
}