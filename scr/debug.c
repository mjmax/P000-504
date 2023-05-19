/*
 * debug.c
 *
 *  Created on: May 19, 2023
 *      Author: Janaka Madhusankha
 */

#include "avrdet.h"
#include "iocontrol.h"

void degug_init(void)
{
    pinMode(D13, OUTPUT);
}

void debug_blink(void)
{
    static bool flip = true;

    digitalWrite(D13, flip);
    flip = !flip; 
}