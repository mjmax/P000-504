/***************************************************************************************************
MODULE:         EEPROM
VERSION:   	1.01
CONTAINS:  	contains hardware access/definition for EEPROM access
COPYRIGHT: 	Rinstrum Pty Ltd 2004-2007
DATE: 		19 Aug 2004
          22 Oct 2007 AGL Removed EepromRead() & EepromWrite(), since they are not called within
                          any M400 module. Replaced with macros if you ever decide you want them.
                          Replaced _EEPROMH_ with Rinstrum standard EEPROM_DOT_H.

NOTES:  The macros have very little overhead compared with the block functions. The overhead only
consists of an extra int8u parameter, and the test for "while (length-- > 0)". In case you _do_
decide you want the original EepromRead() & EepromWrite(), you can turn these on by #defining
LOW_LEVEL_EEPROM_BYTE_FUNCTIONS in your Makefile or Project.

***************************************************************************************************/

#ifndef EEPROM_DOT_H
#define EEPROM_DOT_H 1


////////////////////////////////////////////////////////////////////////
// NAME: EepromWrite
//
// PARAMETERS: int16u addr - EEPROM address of data
//             int8u data - data byte to write
//
// RETURNS:    none
//
// NOTES:
//
#ifndef OLD_EEPROM_BYTE_FUNCTIONS
#define EepromWrite(addr,data) EepromWriteBlock((int8u*)&(data), addr, (int8u)1)
#else
void    EepromWrite(int16u addr, int8u data);
#endif


////////////////////////////////////////////////////////////////////////
// NAME: EepromWriteBlock
//
// PARAMETERS: int8u *buf - pointer to data to be written
//             int16u addr - EEPROM address of start of data
//             int8u length - length of data to be written
//
// RETURNS:    none
//
// NOTES:
//

void EepromWriteBlock(int8u *buf, int16u addr, int8u length);


////////////////////////////////////////////////////////////////////////
// NAME: EepromRead
//
// PARAMETERS: int16u addr - EEPROM address of data
//             int8u *data - address of data byte to read into
//
// RETURNS:    none
//
// NOTES:
//

#ifndef OLD_EEPROM_BYTE_FUNCTIONS
#define EepromRead(addr,data) EepromReadBlock((int8u*)&(data), addr, (int8u)1)
#else
void    EepromRead(int16u addr, int8u *data);
#endif


////////////////////////////////////////////////////////////////////////
// NAME: EepromReadBlock
//
// PARAMETERS: int8u *buf - pointer to data to be written
//             int16u addr - EEPROM address of start of data
//             int8u length - length of data to be read
//
// RETURNS:    none
//
// NOTES:
//

void EepromReadBlock(int8u *buf, int16u addr, int8u length);


#endif /* EEPROM_DOT_H */

