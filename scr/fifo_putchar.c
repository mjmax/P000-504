/***********************************************************************
MODULE:    	FIFO BUFFER
VERSION:   	1.01
CONTAINS:  	FIFO buffer implementation
COPYRIGHT: 	
DATE: 		9 Mar 2004
NOTES:          Due to potential corruption, many of these functions will
                disable interrupts globally.
***********************************************************************/
#include "avrdet.h"
#include <avr/io.h>
#include <avr/wdt.h>

#include "fifo.h"        /* FIFO buffer handler                       */


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoPutChar
// Purpose	: Places a byte into the buffer (if space)
// Parameters	: struct t_fifo_ctl *psBufferStruct
//                int8u ucData
// Return Type	: void
// Notes:
//   It is the callers responsibility to check if space exists.  If buffer full, char is
//   ignored (may be lost).  this is 'safe' to do
//                !!Disables interrupts globally!!
//

bool FifoPutChar(struct s_fifo_ctl *psBufferStruct, uint8_t ucData)
{
  bool result = false;
  uint8_t sReg;

  /* disable interrupts globally */
  sReg = SREG;
  SREG = 0;

  if (psBufferStruct->ucFree > 0)
  { /* space is available */
    psBufferStruct->pucBuffer[psBufferStruct->ucTailIndex] = ucData;
    psBufferStruct->ucFree--;
    psBufferStruct->ucTailIndex = (uint8_t)((psBufferStruct->ucTailIndex+1)%
                                           psBufferStruct->ucBufferSize);
    result = true;
  }

  /* reenable interrupts globally */
  SREG = sReg;
  return result;
}
