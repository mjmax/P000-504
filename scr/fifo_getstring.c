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

int8u FifoGetString(struct t_fifo_ctl *psBufferStruct, int8u *pucData, int8u ucSize)
{
  int8u ucCount = 0;
  int8u sReg;

  #ifdef DEBUG_FIFO_FUNC_CALLS
  DebugPutStr_P(PSTR("FifoGetString\n"));
  #endif

  #ifdef DEBUG_FIFO_GET_STRING
  DebugPutStr_P(PSTR("GetStr entry @ "));
  DebugPutChar(psBufferStruct->ucHeadIndex);
  DebugPutStr_P(PSTR(" free="));
  DebugPutChar(psBufferStruct->ucFree);
  DebugPutStr_P(PSTR(" n="));
  DebugPutChar(ucSize);
  DebugPutChar('\n');
  #endif

  /* disable interrupts globally */
  sReg = SREG;
  SREG = 0;

  while ((ucCount < ucSize) && (psBufferStruct->ucFree < psBufferStruct->ucBufferSize))
  { /* data is wanted && data is available */
    pucData[ucCount] = psBufferStruct->pucBuffer[psBufferStruct->ucHeadIndex];
    ucCount++;
    psBufferStruct->ucFree++;
    psBufferStruct->ucHeadIndex = (int8u)((psBufferStruct->ucHeadIndex+1)%
                                           psBufferStruct->ucBufferSize);
    wdt_reset();
  }

  /* reenable interrupts globally */
  SREG = sReg;

  #ifdef DEBUG_FIFO_GET_STRING
  DebugPutStr_P(PSTR("GetStr exit @ "));
  DebugPutChar(psBufferStruct->ucHeadIndex);
  DebugPutStr_P(PSTR(" free="));
  DebugPutChar(psBufferStruct->ucFree);
  DebugPutChar('\n');
  #endif

  return ucCount;
}
