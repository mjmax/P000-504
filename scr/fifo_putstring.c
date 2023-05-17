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
// Function 	: FifoPutString
// Purpose	: Places a byte into the buffer (if space)
// Parameters	: struct t_fifo_ctl *psBufferStruct
//                int8u *pucData
//                int8u ucSize
// Return Type	: int8u - actual size of data placed
// Notes:
//   It is the callers responsibility to check if space exists.  If buffer fills, data
//   ignored (may be lost).  this is safe to do
//                !!Disables interrupts globally!!
//

uint8_t FifoPutString(struct s_fifo_ctl *psBufferStruct, const uint8_t *pucData, uint8_t ucSize)
{
  uint8_t ucCount = 0;
  uint8_t sReg;

  /* disable interrupts globally */
  sReg = SREG;
  SREG = 0;

  while ((ucCount < ucSize) && (psBufferStruct->ucFree > 0))
  { /* data available && space remains */
    psBufferStruct->pucBuffer[psBufferStruct->ucTailIndex] = pucData[ucCount];
    ucCount++;
    psBufferStruct->ucFree--;
    psBufferStruct->ucTailIndex = (uint8_t)((psBufferStruct->ucTailIndex+1)%
                                           psBufferStruct->ucBufferSize);
    wdt_reset();
  }

  /* reenable interrupts globally */
  SREG = sReg;
  return ucCount;
}
