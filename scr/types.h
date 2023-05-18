/***************************************************************************************************
MODULE:    TYPES
VERSION:   0.01
CONTAINS:  typedefs
COPYRIGHT: 
DATE:      9 Mar 2004
           28 Sep 2007 AGL Added int64s & int64u.
NOTES:
***************************************************************************************************/

#ifndef _TYPESH_
#define _TYPESH_

#include <stddef.h>

typedef unsigned char       int8u;
typedef unsigned char   		byte;
typedef signed   char       int8s;
typedef unsigned short      int16u;
typedef signed   short      int16s;
typedef unsigned long       int32u;
typedef signed   long       int32s;
typedef unsigned long long  int64u;
typedef signed   long long  int64s;
typedef unsigned char       bool;

typedef void (*t_vpf)(void);

#define TRUE  1
#define FALSE 0

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTES: Handy macros HIGH(), LOW()
//
//

#define HIGH(uiValue) (int8u)((uiValue) >> 8)
#define LOW(uiValue)  (int8u)(uiValue)


////////////////////////////////////////////////////////////////////////////////////////////////////
// NOTES: SIZEOF() for use in determining element sizes in structures
//        that may not be instantiated (no actual memory allocated)
//        Use with offsetof() in order to get around any packing issues
//

#define SIZEOF(s,m) ((size_t) sizeof(((s *)0)->m))

typedef union
{
  struct
  {
    unsigned char b7 : 1;
    unsigned char b6 : 1;
    unsigned char b5 : 1;
    unsigned char b4 : 1;
    unsigned char b3 : 1;
    unsigned char b2 : 1;
    unsigned char b1 : 1;
    unsigned char b0 : 1;
  } bits;
  unsigned char data;
} Bits;



#endif // _TYPESH_
