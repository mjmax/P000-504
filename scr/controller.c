/*
 * controller.h
 *
 *  Created on: 06/01/2023
 *      Author: Janaka Madhusankha
 */

#include "avrdet.h"
#include "types.h"
#include "controller.h"
#include "float.h"
#include "math.h"
#include "serial.h"
#include "timer0.h"
#include "dyn_ax18a.h"

#include <stdlib.h>
#include <stdio.h>
#include <avr/wdt.h>

char buffer[100];
bool readyToSend = false;

void test_control(void)
{
    float p = 120.234, q = 2.345, r;
    int16u time_prev;
    int16u time_now;
    int32u i = 0;
    char temp[15];

    if(is_ready_to_send())
    {
        time_prev = millis();
        for(i = 0; i < 10; i++)
        {
            r = p*q;
            wdt_reset();
        }
        time_now = millis();

        dtostrf( r, 3, 6, temp );
        sprintf(buffer, "Answer: %s, Time taken: %d\r\n", temp, (time_now - time_prev));
        CommsSendString(SCI_PORT_0,buffer);
        //dyn_test_motor_pos_val();
        set_ready_to_send(false);
    }
}

bool is_ready_to_send(void)
{
    return readyToSend;
}

void set_ready_to_send(bool state)
{
    readyToSend = state;
}