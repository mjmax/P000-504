/***********************************************************************
MODULE:    	FIFO BUFFER
VERSION:   	1.01
CONTAINS:  	FIFO buffer implementation
COPYRIGHT: 	
DATE: 		9 Mar 2004
NOTES:
***********************************************************************/

#ifndef _FIFOH_
#define _FIFOH_

#include "avrdet.h"
#include "types.h"


///////////////////////////////////////////////////////////////////////////////////////////
// Notes: FIFO buffer implementation
//  The buffer storage is explicitely declared, then a structure that contains
//  a pointer to the buffer, along with all other storage needed to manipulate the buffer
//  is created for each buffer.
//  This allows the size of each to be controlled without utilising malloc
//

struct t_fifo_ctl
{
  int8u *pucBuffer;
  int8u ucBufferSize;
  int8u ucHeadIndex;
  int8u ucTailIndex;
  int8u ucFree;
  int8u ucInitialised;
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

void FifoInit(struct t_fifo_ctl *psBufferStruct, int8u *pucBuffer, int8u ucBufferSize);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoFlushBuffer
// Purpose	: Initialises buffer
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: void
// Notes:
//

void FifoFlushBuffer(struct t_fifo_ctl *psBufferStruct);


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

void FifoPutChar(struct t_fifo_ctl *psBufferStruct, int8u ucData);


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

int8u FifoPutString(struct t_fifo_ctl *psBufferStruct, const int8u *pucData, int8u ucSize);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoGetChar
// Purpose      : Returns a byte off the buffer (if available)
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: int8u - data byte
// Notes:
//   It is the callers responsibility to check if data available.  If buffer empty, 0 is
//   returned
//

int8u FifoGetChar(struct t_fifo_ctl *psBufferStruct);


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

int8u FifoGetString(struct t_fifo_ctl *psBufferStruct, int8u *pucData, int8u ucSize);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoFree
// Purpose	: Returns amount of free buffer space
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: int8u - number of bytes available in buffer
// Notes:
//
//

int8u FifoFree(struct t_fifo_ctl *psBufferStruct);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoUsed
// Purpose	: Returns amount of used buffer space
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: int8u - number of bytes buffered
// Notes:
//
//

int8u FifoUsed(struct t_fifo_ctl *psBufferStruct);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoIsFull
// Purpose	: Checks if space available in buffer
// Parameters	: struct t_fifo_ctl *psBufferStruc
// Return Type	: int8u = 1, Buffer full
//                      = 0, Buffer not full
// Notes:
//
//

int8u FifoIsFull(struct t_fifo_ctl *psBufferStruct);


///////////////////////////////////////////////////////////////////////////////////////////
// Function 	: FifoIsEmpty
// Purpose      : Checks if data available from buffer
// Parameters	: struct t_fifo_ctl *psBufferStruct
// Return Type	: int8u = 1, Buffer empty
//                      = 0, Buffer not empty
// Notes:
//
//

int8u FifoIsEmpty(struct t_fifo_ctl *psBufferStruct);

// Function   : FifoGetLastChar
// Purpose  : Returns last byte added to the buffer (if available) buffer contents are unchanged
// Parameters : struct t_fifo_ctl *psBufferStruct
// Return Type  : int8u - data byte
// Notes:
//   It is the callers responsibility to check if data available.  If buffer empty, 0 is
//   returned
//                !!Disables interrupts globally!!
//

bool FifoGetLastChar(struct t_fifo_ctl *psBufferStruct, int8u *data);


#endif // _FIFOH_
