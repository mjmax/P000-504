#ifndef AVRDET_H_
#define AVRDET_H_

/*Config Microcontroller*/
//#define AVR_ATmega328P  0x01
#define AVR_ATmega2560  0x01

#ifdef AVR_ATmega328P
#define F_CPU 16588800UL
#define __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif

//#ifdef AVR_ATmega2560
#ifdef AVR_ATmega2560
#define F_CPU 16588800UL
#ifndef __AVR_ATmega2560__
#define __AVR_ATmega2560__
#endif
#endif

#define NULL ((void *)0)

#endif /* AVRDET_H_ */
