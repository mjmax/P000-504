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
// Function 	: FifoIsEmpty
// Purpose	: Checks if data available from buffer
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: int8u = 1, Buffer empty
//                      = 0, Buffer not empty
// Notes:
//

bool FifoIsEmpty(struct s_fifo_ctl *psBufferStruct)
{
  bool bIsEmpty;
  uint8_t sReg;

  /* disable interrupts globally */
  sReg = SREG;
  SREG = 0;

  //ucIsEmpty = (psBufferStruct->ucFree > psBufferStruct->ucBufferSize);
  if (psBufferStruct->ucFree > psBufferStruct->ucBufferSize)
  {
    bIsEmpty = 0;
  }
  else
  {
    bIsEmpty = 1;
  }

  /* reenable interrupts globally */
  SREG = sReg;
  return bIsEmpty;
}
