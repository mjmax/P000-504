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
// Function 	: FifoInit
// Purpose	: Initialises buffer
// Parameters	: struct t_fifo_ctl *psBufferStruct
//                int8u *pucBuffer
//                int8u ucBufferSize
// Return Type	: void
// Notes:
//                !!Disables interrupts globally!!
//

void FifoInit(struct t_fifo_ctl *psBufferStruct, int8u *pucBuffer, int8u ucBufferSize)
{
  int8u sReg;

  #ifdef DEBUG_FIFO_FUNC_CALLS
  DebugPutStr_P(PSTR("FifoInit\n"));
  #endif

  if (ucBufferSize > 254)
  {
    ucBufferSize = 254;
  }

  /* disable interrupts globally */
  sReg = SREG;
  SREG = 0;

  psBufferStruct->pucBuffer = pucBuffer;
  psBufferStruct->ucBufferSize = ucBufferSize;
  psBufferStruct->ucFree = ucBufferSize;
  psBufferStruct->ucHeadIndex = 0;
  psBufferStruct->ucTailIndex = 0;

  /* reenable interrupts globally */
  SREG = sReg;
}
