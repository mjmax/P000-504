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

int8u FifoPutString(struct t_fifo_ctl *psBufferStruct, const int8u *pucData, int8u ucSize)
{
  int8u ucCount = 0;
  int8u sReg;

  #ifdef DEBUG_FIFO_FUNC_CALLS
  DebugPutStr_P(PSTR("FifoPutString\n"));
  #endif

  /* disable interrupts globally */
  sReg = SREG;
  SREG = 0;

  #ifdef DEBUG_FIFO_PUT_STRING
  DebugPutStr_P(PSTR("PutStr entry @ "));
  DebugPutChar(psBufferStruct->ucTailIndex);
  DebugPutStr_P(PSTR(" free="));
  DebugPutChar(psBufferStruct->ucFree);
  DebugPutStr_P(PSTR(" n="));
  DebugPutChar(ucSize);
  DebugPutChar('\n');
  #endif


  while ((ucCount < ucSize) && (psBufferStruct->ucFree > 0))
  { /* data available && space remains */
    psBufferStruct->pucBuffer[psBufferStruct->ucTailIndex] = pucData[ucCount];
    ucCount++;
    psBufferStruct->ucFree--;
    psBufferStruct->ucTailIndex = (int8u)((psBufferStruct->ucTailIndex+1)%
                                           psBufferStruct->ucBufferSize);
    wdt_reset();
  }

  #ifdef DEBUG_FIFO_PUT_STRING
  DebugPutStr_P(PSTR("PutStr exit @ "));
  DebugPutChar(psBufferStruct->ucTailIndex);
  DebugPutStr_P(PSTR(" free="));
  DebugPutChar(psBufferStruct->ucFree);
  DebugPutChar('\n');
  #endif

  /* reenable interrupts globally */
  SREG = sReg;

  return ucCount;
}
