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
// Function 	: FifoFree
// Purpose	: Returns amount of free buffer space
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: int8u
// Notes:
//

int8u FifoFree(struct t_fifo_ctl *psBufferStruct)
{
  #ifdef DEBUG_FIFO_FUNC_CALLS
  DebugPutStr_P(PSTR("FifoFree\n"));
  #endif

  #ifdef DEBUG_FIFO_FREE
  DebugPutStr_P(PSTR("buffer "));
  DebugPutStr_P(PSTR(" free="));
  DebugPutChar(psBufferStruct->ucFree);
  DebugPutChar('\n');
  #endif

  return psBufferStruct->ucFree;
}
