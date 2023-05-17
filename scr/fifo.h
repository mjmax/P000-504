/***********************************************************************
MODULE:    	FIFO BUFFER
VERSION:   	1.01
CONTAINS:  	FIFO buffer
COPYRIGHT: 	
DATE: 		9 Mar 2004
NOTES:
***********************************************************************/

#ifndef _FIFOH_
#define _FIFOH_

#include <stdint.h>
#include <stdbool.h>
#include "avrdet.h"


///////////////////////////////////////////////////////////////////////////////////////////
// Notes: FIFO buffer implementation
//  The buffer storage is explicitely declared, then a structure that contains
//  a pointer to the buffer, along with all other storage needed to manipulate the buffer
//  is created for each buffer.
//  This allows the size of each to be controlled without utilising malloc
//

struct s_fifo_ctl
{
  uint8_t *pucBuffer;
  uint8_t ucBufferSize;
  uint8_t ucHeadIndex;
  uint8_t ucTailIndex;
  uint8_t ucFree;
};


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoInit
// Purpose	: Initialises buffer
// Parameters	: struct t_fifo_ctl *psBufferStruct
//                int8u *pucBuffer
//                int8u ucBufferSize
// Return Type	: void
// Notes:
//
//

void FifoInit(struct s_fifo_ctl *psBufferStruct, uint8_t *pucBuffer, uint8_t ucBufferSize);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoFlushBuffer
// Purpose	: Initialises buffer
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: void
// Notes:
//

void FifoFlushBuffer(struct s_fifo_ctl *psBufferStruct);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoPutChar
// Purpose	: Places a byte into the buffer (if space)
// Parameters	: struct t_fifo_ctl *psBufferStruct
//                int8u ucData
// Return Type	: void
// Notes:
//   It is the callers responsibility to check if space exists.  If buffer full, char is
//   ignored (may be lost)
//

bool FifoPutChar(struct s_fifo_ctl *psBufferStruct, uint8_t ucData);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoPutString
// Purpose	: Places a byte into the buffer (if space)
// Parameters	: struct t_fifo_ctl *psBufferStruct
//                int8u *pucData
//                int8u ucSize
// Return Type	: int8u - actual size of data placed
// Notes:
//   It is the callers responsibility to check if space exists.  If buffer fills, data
//   ignored (may be lost)
//
//

uint8_t FifoPutString(struct s_fifo_ctl *psBufferStruct, const uint8_t *pucData, uint8_t ucSize);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoGetChar
// Purpose      : Returns a byte off the buffer (if available)
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: int8u - data byte
// Notes:
//   It is the callers responsibility to check if data available.  If buffer empty, 0 is
//   returned
//

bool FifoGetChar(struct s_fifo_ctl *psBufferStruct, uint8_t *data);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoGetString
// Purpose      : Returns a byte off the buffer (if available)
// Parameters	: struct t_fifo_ctl *psBufferStruct
//                int8u *pucData
//                int8u ucSize
// Return Type	: int8u - actual size of data got
// Notes:
//   It is the callers responsibility to check if data available.  If buffer empty, 0 is
//   returned
//
//

uint8_t FifoGetString(struct s_fifo_ctl *psBufferStruct, uint8_t *pucData, uint8_t ucSize);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoFree
// Purpose	: Returns amount of free buffer space
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: int8u - number of bytes available in buffer
// Notes:
//
//

uint8_t FifoFree(struct s_fifo_ctl *psBufferStruct);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoUsed
// Purpose	: Returns amount of used buffer space
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: int8u - number of bytes buffered
// Notes:
//
//

uint8_t FifoUsed(struct s_fifo_ctl *psBufferStruct);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoIsFull
// Purpose	: Checks if space available in buffer
// Parameters	: struct t_fifo_ctl *psBufferStruc
// Return Type	: int8u = 1, Buffer full
//                      = 0, Buffer not full
// Notes:
//
//

bool FifoIsFull(struct s_fifo_ctl *psBufferStruct);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoIsEmpty
// Purpose      : Checks if data available from buffer
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: int8u = 1, Buffer empty
//                      = 0, Buffer not empty
// Notes:
//
//

bool FifoIsEmpty(struct s_fifo_ctl *psBufferStruct);


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

bool FifoGetLastChar(struct s_fifo_ctl *psBufferStruct, uint8_t *data);


#endif // _FIFOH_
