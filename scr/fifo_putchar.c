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
#include "types.h"       /* typedefs                                  */
#include <avr/io.h>
#include <avr/wdt.h>
//#include "debug.h"
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

void FifoPutChar(struct t_fifo_ctl *psBufferStruct, int8u ucData)
{
  int8u sReg;

  #ifdef DEBUG_FIFO_FUNC_CALLS
  DebugPutStr_P(PSTR("FifoPutChar\n"));
  #endif

  /* disable interrupts globally */
  sReg = SREG;
  SREG = 0;

  if (psBufferStruct->ucFree > 0)
  { /* space is available */
    psBufferStruct->pucBuffer[psBufferStruct->ucTailIndex] = ucData;
    psBufferStruct->ucFree--;
    psBufferStruct->ucTailIndex = (int8u)((psBufferStruct->ucTailIndex+1)%
                                           psBufferStruct->ucBufferSize);
  }

  #ifdef DEBUG_FIFO_PUT_CHAR
  DebugPutStr_P(PSTR("PutCh "));
  DebugPutChar(psBufferStruct->pucBuffer[psBufferStruct->ucTailIndex]);
  DebugPutStr_P(PSTR("@ "));
  DebugPutChar(psBufferStruct->ucTailIndex);
  DebugPutStr_P(PSTR(" free="));
  DebugPutChar(psBufferStruct->ucFree);
  DebugPutChar('\n');
  #endif

  /* reenable interrupts globally */
  SREG = sReg;
}
