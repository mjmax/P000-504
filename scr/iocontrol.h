/*
 * iocontrol.h
 *
 *  Created on: Aug 7, 2020
 *      Author: Janaka Madhusankha
 */

#ifndef IOCONTROL_H_
#define IOCONTROL_H_
#include "types.h"


#define CHECKSUM 0x55

//// these bits should match with the schematic. do not change unless you change the schematic
#define OUTPUT_MASK_PORTB 0x27	//output 1 to 4 bit pattern (00100111) (in PORTB -> PB0,PB1,PB2,PB5 are connected to output)
//#define OUTPUT_COUNT_PORTB	4
#define OUTPUT_MASK_PORTC 0x0F	//output 5 to 8 bit pattern (00001111) (in PORTC -> PC0,PC1,PC2,PC3 are connected to output)
//#define OUTPUT_COUNT_PORTC	4
#define OUTPUT_MASK_PORTD 0x00
//#define OUTPUT_COUNT_PORTD	MAX_IO_COUNT - OUTPUT_COUNT_PORTC - OUTPUT_COUNT_PORTB
//
//#define INPUT_MASK_PORTB 	0x00
//#define INPUT_COUNT_PORTB		0
//#define INPUT_MASK_PORTC  0x30	//input 7 to 8 bit pattern (00110000) (in PORTC -> PC4,PC5 are connected to input)
//#define INPUT_COUNT_PORTC		2
//#define INPUT_MASK_PORTD	0xFC	//input 1 to 6 bit pattern (11111100) (in PORTD -> PD2,PD3,PD4,PD5,PD6,PD7 are connected to input)
//#define INPUT_COUNT_PORTD		MAX_IO_COUNT - INPUT_COUNT_PORTB - INPUT_COUNT_PORTC

typedef enum {OUT_1 = 1, OUT_2, OUT_3, OUT_4, OUT_5, OUT_6, OUT_7, OUT_8}outptuNumber_t;

typedef enum {IN_1 = 1, IN_2, IN_3, IN_4, IN_5, IN_6, IN_7, IN_8}inputNumber_t;

//int8u getIODirection(void);
//void setIODirection(int8u dir);

//void defaultDB(void);
//void initIO(void);
//int8u	readIO(void);
//int8u readInput(void);
//int8u readOutput(void);
//bool readOneInput(int8u input);
//void syncInput(void);
//void setOutput(int8u status);
//void setOneOutput(int8u output, bool status);


#endif /* IOCONTROL_H_ */
