/***************************************************************************************************
Module:  ASCII Definitions of Control Characters.
Copyright:

***************************************************************************************************/

#ifndef ASCIIDEF_DOT_H
#define ASCIIDEF_DOT_H

#define H_NUL 0x00
#define H_ETX 0x03
#define H_DLE 0x10
#define H_0	0x30
#define H_1 0x31
#define H_DC2 0x12
#define H_DC4 0x14
#define H_NL 0x0A
#define H_CR 0x0D

#define S_CR ((char) H_CR)
#define S_LF ((char) H_NL)
#define S_STX ('\x02')
#define S_ETX ('\x03')
#define S_DEL ('\x10')
#define S_DC2 ('\x12')
#define S_DC4 ('\x14')

#endif

