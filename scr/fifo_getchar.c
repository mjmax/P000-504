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
// Function 	: FifoGetChar
// Purpose	: Returns a byte off the buffer (if available)
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: int8u - data byte
// Notes:
//   It is the callers responsibility to check if data available.  If buffer empty, 0 is
//   returned
//                !!Disables interrupts globally!!
//

bool FifoGetChar(struct s_fifo_ctl *psBufferStruct, uint8_t *data)
{
  uint8_t sReg;
  bool data_avail;

  /* disable interrupts globally */
  sReg = SREG;
  SREG = 0;

  if (psBufferStruct->ucFree < psBufferStruct->ucBufferSize)
  { /* data is available */
    *data = psBufferStruct->pucBuffer[psBufferStruct->ucHeadIndex];
    psBufferStruct->ucFree++;
    psBufferStruct->ucHeadIndex = (uint8_t) ((psBufferStruct->ucHeadIndex + 1) % psBufferStruct->ucBufferSize);
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


///////////////////////////////////////////////////////////////////////////////////////////
// Function   : FifoGetLastChar
// Purpose  : Returns last byte added to the buffer (if available) buffer contents are unchanged
// Parameters : struct t_fifo_ctl *psBufferStruct
// Return Type  : int8u - data byte
// Notes:
//   It is the callers responsibility to check if data available.  If buffer empty, 0 is
//   returned
//                !!Disables interrupts globally!!
//

bool FifoGetLastChar(struct s_fifo_ctl *psBufferStruct, uint8_t *data)
{
  uint8_t sReg;
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
