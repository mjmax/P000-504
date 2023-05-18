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
// Function 	: FifoGetChar
// Purpose	: Returns a byte off the buffer (if available)
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: int8u - data byte
// Notes:
//   It is the callers responsibility to check if data available.  If buffer empty, 0 is
//   returned
//                !!Disables interrupts globally!!
//

int8u FifoGetChar(struct t_fifo_ctl *psBufferStruct)
{
  int8u ucData = 0;
  int8u sReg;

  #ifdef DEBUG_FIFO_FUNC_CALLS
  DebugPutStr_P(PSTR("FifoGetChar\n"));
  #endif

  #ifdef DEBUG_FIFO_GET_CHAR
  DebugPutStr_P(PSTR("GetCh entry @ "));
  DebugPutChar(psBufferStruct->ucHeadIndex);
  DebugPutStr_P(PSTR(" free="));
  DebugPutChar(psBufferStruct->ucFree);
  DebugPutChar('\n');
  #endif

  /* disable interrupts globally */
  sReg = SREG;
  SREG = 0;

  if (psBufferStruct->ucFree < psBufferStruct->ucBufferSize)
  { /* data is available */
    ucData = psBufferStruct->pucBuffer[psBufferStruct->ucHeadIndex];
    psBufferStruct->ucFree++;
    psBufferStruct->ucHeadIndex = (int8u)((psBufferStruct->ucHeadIndex+1)%
                                           psBufferStruct->ucBufferSize);
  }

  /* reenable interrupts globally */
  SREG = sReg;

  #ifdef DEBUG_FIFO_GET_CHAR
  DebugPutStr_P(PSTR("GetCh exit @ "));
  DebugPutChar(psBufferStruct->ucHeadIndex);
  DebugPutStr_P(PSTR(" free="));
  DebugPutChar(psBufferStruct->ucFree);
  DebugPutStr_P(PSTR(" ch="));
  DebugPutChar(ucData);
  DebugPutChar('\n');
  #endif


  return ucData;
}

bool FifoGetLastChar(struct t_fifo_ctl *psBufferStruct, uint8_t *data)
{
  int8u sReg;
  bool data_avail;

  /* disable interrupts globally */
  sReg = SREG;
  SREG = 0;

  if (psBufferStruct->ucFree < psBufferStruct->ucBufferSize)
  { /* data is available */
    if (psBufferStruct->ucTailIndex > 0)
      *data = psBufferStruct->pucBuffer[psBufferStruct->ucTailIndex - 1];
    else
      *data = psBufferStruct->pucBuffer[psBufferStruct->ucBufferSize - 1];
    data_avail = true;
  }
  else
  {
    data_avail = false;
  }

  /* reenable interrupts globally */
  SREG = sReg;
  return data_avail;
}
