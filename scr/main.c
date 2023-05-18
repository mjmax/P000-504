#include "avrdet.h"
#include "serial.h"

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRB |= 1<<5;
	uint8_t LocalData = 'b';
    SerialInit(115200);
	//USART_Init();
	//USART_TransmitInterrupt(LocalData);
    while(1)
    {
        _delay_ms(1);
        SerialHandler();
        //usart_send_character('b');
        //usart_send_string("Hello World!\r\n");
        //PORTB ^= 1<<5; 
    }
    return 0;
}