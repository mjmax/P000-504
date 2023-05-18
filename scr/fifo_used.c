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
// Function 	: FifoUsed
// Purpose	: Returns amount of used buffer space
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: int8u
// Notes:
//

int8u FifoUsed(struct t_fifo_ctl *psBufferStruct)
{
  int8u ucUsed;
  int8u sReg;

  #ifdef DEBUG_FIFO_FUNC_CALLS
  DebugPutStr_P(PSTR("FifoUsed\n"));
  #endif

  /* disable interrupts globally */
  sReg = SREG;
  SREG = 0;

  ucUsed = psBufferStruct->ucBufferSize - psBufferStruct->ucFree;

  /* reenable interrupts globally */
  SREG = sReg;

  #ifdef DEBUG_FIFO_USED
  DebugPutStr_P(PSTR("buffer "));
  DebugPutStr_P(PSTR(" n="));
  DebugPutChar(ucUsed);
  DebugPutChar('\n');
  #endif

  return ucUsed;
}
