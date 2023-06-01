/***************************************************************************************************
MODULE:         EEPROM
VERSION:   	1.01
CONTAINS:  	contains hardware access/definition for EEPROM access
COPYRIGHT: 
DATE: 		19 Aug 2004
          22 Oct 2007 AGL Removed EepromRead() & EepromWrite(), since they are not called within
                          any M400 module. Replaced with macros if you ever decide you want them.

NOTES:  The macros have very little overhead compared with the block functions. The overhead only
consists of an extra int8u parameter, and the test for "while (length-- > 0)". In case you _do_
decide you want the original EepromRead() & EepromWrite(), you can turn these on by #defining
LOW_LEVEL_EEPROM_BYTE_FUNCTIONS in your Makefile or Project.

***************************************************************************************************/
#include "avrdet.h"
#include "types.h"
#include "eeprom.h"

#include <stddef.h>
#include <string.h>
#include <avr/io.h>
#include <avr/wdt.h>


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
#ifdef LOW_LEVEL_EEPROM_BYTE_FUNCTIONS
void EepromWrite(int16u addr, int8u data)
{
  int8u Sreg; /* used to store status of global interrupt bit */

  /* disable interrupts */
  Sreg = SREG;
  SREG = 0;

  /* wait for completion of previous write */
  do
  {
    wdt_reset();
  } while(EECR & (1<<EEPE));

  /* set up address and data registers */
  EEAR = addr;
  EEDR = data;
  EECR |= (1<<EEMPE); /* enable writes */
  EECR |= (1<<EEPE); /* start eeprom write */

  /* re enable interrupts */
  SREG = Sreg;
}
#endif


////////////////////////////////////////////////////////////////////////
// NAME: EepromWriteBlock
//
// PARAMETERS: int8u *buf - pointer to data to be written
//             int16u addr - EEPROM address of start of data
//             int8u length - length of data in i2c message
//
// RETURNS:    none
//
// NOTES:
//

void EepromWriteBlock(int8u *buf, int16u addr, int8u length)
{
  int8u Sreg; /* used to store status of global interrupt bit */

  while (length-- > 0)
  {
    /* disable interrupts */
    Sreg = SREG;
    SREG = 0;

    /* wait for completion of previous write */
    do
    {
      wdt_reset();
    } while(EECR & (1<<EEPE));

    /* set up address and data registers */
    EEAR = addr++;
    EEDR = *buf++;
    EECR |= (1<<EEMPE); /* enable writes */
    EECR |= (1<<EEPE); /* start eeprom write */

    /* re enable interrupts */
    SREG = Sreg;
  }
}


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

#ifdef LOW_LEVEL_EEPROM_BYTE_FUNCTIONS
void EepromRead(int16u addr, int8u *data)
{
  int8u Sreg; /* used to store status of global interrupt bit */

  /* disable interrupts */
  Sreg = SREG;
  SREG = 0;

  /* wait for completion of previous write */
  do
  {
    wdt_reset();
  } while(EECR & (1<<EEPE));

  /* set up address register */
  EEAR = addr;
  EECR |= (1<<EERE); /* start eeprom read */

  *data = EEDR; /* transfer data */

  /* re enable interrupts */
  SREG = Sreg;
}
#endif


////////////////////////////////////////////////////////////////////////
// NAME: EepromReadBlock
//
// PARAMETERS: int8u *buf - pointer to data to be written
//             int16u addr - EEPROM address of start of data
//             int8u length - length of data in i2c message
//
// RETURNS:    none
//
// NOTES:
//

void EepromReadBlock(int8u *buf, int16u addr, int8u length)
{
  int8u Sreg; /* used to store status of global interrupt bit */

  while (length-- > 0)
  {
    /* disable interrupts */
    Sreg = SREG;
    SREG = 0;

    /* wait for completion of previous write */
    do
    {
      wdt_reset();
    } while(EECR & (1<<EEPE));

    /* set up address registers */
    EEAR = addr++;
    EECR |= (1<<EERE); /* start eeprom read */

    *buf++ = EEDR; /* transfer data */

    /* re enable interrupts */
    SREG = Sreg;
  }
}
