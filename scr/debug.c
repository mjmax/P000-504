/*
 * debug.c
 *
 *  Created on: May 19, 2023
 *      Author: Janaka Madhusankha
 */

#include "avrdet.h"
#include "iocontrol.h"

#define DEBUG_PIN D12

void degug_init(void)
{
    pinMode(D13, OUTPUT);
    pinMode(DEBUG_PIN, OUTPUT);
}

void debug_blink(void)
{
    static bool flip = true;

    digitalWrite(D13, flip);
    flip = !flip; 
}

void debug_debug_pin(void)
{
    static bool flip = true;

    digitalWrite(DEBUG_PIN, flip);
    flip = !flip; 
}