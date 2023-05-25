/*
 * iocontrol.h
 *
 *  Created on: May 19, 2023
 *      Author: Janaka Madhusankha
 */

#ifndef IOCONTROL_H_
#define IOCONTROL_H_
#include "types.h"

#ifdef AVR_ATmega328P
    typedef enum {A0 = 0, A1, A2, A3, A4, A5, A6, A7, D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13}pinName_t;
#elif AVR_ATmega2560
    typedef enum {A0 = 0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, D0, D1, D2, D3, D4, D5, D6, D7, D8, D9, D10,
                  D11, D12, D13, D14, D15, D16, D17, D18, D19, D20, D21, D22, D23, D24, D25, D26, D27, D28, D29, D30, D31, D32, D33, D34, D35,
                  D36, D37, D38, D39, D40, D41, D42, D43, D44, D45, D46, D47, D48, D49, D50, D51, D52, D53, D54, D55, D56, D57, D58, D59, D60,
                  D61, D62, D63, D64, D65, D66, D67, D68, D69}pinName_t;
#endif

typedef enum {INPUT = 0, OUTPUT}pinMode_t;
typedef enum {LOW = 0, HIGH}pinStatus_t;

int8u pinMode(int8u pin, int8u mode);
int8u digitalRead(int8u pin);
int8u digitalWrite(int8u pin, int8u value);


#endif /* IOCONTROL_H_ */
