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
// Function 	: FifoGetString
// Purpose	: Returns a byte off the buffer (if available)
// Parameters	: struct t_fifo_ctl *psBufferStruct
//                int8u *pucData
//                int8u ucSize
// Return Type	: int8u - actual size of data got
// Notes:
//   It is the callers responsibility to check if data available.  If buffer empty, 0 is
//   returned
//                !!Disables interrupts globally!!
//

uint8_t FifoGetString(struct s_fifo_ctl *psBufferStruct, uint8_t *pucData, uint8_t ucSize)
{
  uint8_t ucCount = 0;
  uint8_t sReg;

  /* disable interrupts globally */
  sReg = SREG;
  SREG = 0;

  while ((ucCount < ucSize) && (psBufferStruct->ucFree < psBufferStruct->ucBufferSize))
  { /* data is wanted && data is available */
    pucData[ucCount] = psBufferStruct->pucBuffer[psBufferStruct->ucHeadIndex];
    ucCount++;
    psBufferStruct->ucFree++;
    psBufferStruct->ucHeadIndex = (uint8_t)((psBufferStruct->ucHeadIndex+1)%
                                           psBufferStruct->ucBufferSize);
    wdt_reset();
  }

  /* reenable interrupts globally */
  SREG = sReg;
  return ucCount;
}
