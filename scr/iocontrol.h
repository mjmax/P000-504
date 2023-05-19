/*
 * iocontrol.h
 *
 *  Created on: Aug 7, 2020
 *      Author: Janaka Madhusankha
 */

#ifndef IOCONTROL_H_
#define IOCONTROL_H_
#include "types.h"

typedef enum {A0 = 0, A1, A2, A3, A4, A5, A6, A7, D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13}pinName_t;

typedef enum {INPUT = 0, OUTPUT}pinMode_t;
typedef enum {LOW = 0, HIGH}pinStatus_t;

int8u pinMode(int8u pin, int8u mode);
int8u digitalRead(int8u pin);
int8u digitalWrite(int8u pin, int8u value);


#endif /* IOCONTROL_H_ */
