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
// Function 	: FifoIsEmpty
// Purpose	: Checks if data available from buffer
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: int8u = 1, Buffer empty
//                      = 0, Buffer not empty
// Notes:
//

int8u FifoIsEmpty(struct t_fifo_ctl *psBufferStruct)
{
  int8u ucIsEmpty;
  int8u sReg;

//  #ifdef DEBUG_FIFO_FUNC_CALLS
//  DebugPutStr_P(PSTR("FifoIsEmpty\n"));
//  #endif

  /* disable interrupts globally */
  sReg = SREG;
  SREG = 0;

  if (psBufferStruct->ucFree < psBufferStruct->ucBufferSize)
  {
    ucIsEmpty = 0;
  }
  else
  {
    ucIsEmpty = 1;
  }

  /* reenable interrupts globally */
  SREG = sReg;

  #ifdef DEBUG_FIFO_IS_EMPTY
  DebugPutStr_P(PSTR("buffer "));
  DebugPutStr_P((ucIsEmpty == 1) ? PSTR("empty\n") : PSTR("not empty\n"));
  #endif

  return ucIsEmpty;
}
