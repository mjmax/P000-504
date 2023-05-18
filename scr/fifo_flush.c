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
// Function 	: FifoFlushBuffer
// Purpose	: Initialises buffer
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: void
// Notes:
//                !!Disables interrupts globally!!
//

void FifoFlushBuffer(struct t_fifo_ctl *psBufferStruct)
{
  int8u sReg;

  #ifdef DEBUG_FIFO_FUNC_CALLS
  DebugPutStr_P(PSTR("FifoFlushBuffer\n"));
  #endif

  /* disable interrupts globally */
  sReg = SREG;
  SREG = 0;

  psBufferStruct->ucFree = psBufferStruct->ucBufferSize;
  psBufferStruct->ucHeadIndex = 0;
  psBufferStruct->ucTailIndex = 0;

  /* reenable interrupts globally */
  SREG = sReg;
}
