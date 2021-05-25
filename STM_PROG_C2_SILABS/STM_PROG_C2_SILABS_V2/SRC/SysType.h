//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __SysType_H
#define __SysType_H

#include "stm32f10x_lib.h"
#include "stm_GpioBit.h"
#include "STM32_Reg.h"                            // missing bit definitions
//#include "stdint.h"
#include "stdbool.h"

typedef u8 uint8_t;
typedef u16 uint16_t;
typedef u32 uint32_t;
typedef u64 uint64_t;

typedef s8 int8_t;
typedef s16 int16_t;
typedef s32 int32_t;
typedef s64 int64_t;


#define structof(_st, _memb) ((u32)(&(((_st *)0)->_memb)))

typedef struct {
    u64  d[1];
}tUU64;
typedef struct {
    s64  d[1];
}tSS64;
typedef struct {
    u32  d[1];
}tUU32;
typedef struct {
    s32  d[1];
}tSS32;
typedef struct {
    u16  w[1];
}tUU16;
typedef struct {
    s16  w[1];
}tSS16;
typedef struct {
    u8   b[1];
}tUU8;
typedef struct {
    s8   b[1];
}tSS8;

typedef struct {
    vu64  d[1];
}volatile tVUU64;
typedef struct {
    vs64  d[1];
}volatile tVSS64;
typedef struct {
    vu32  d[1];
}volatile tVUU32;
typedef struct {
    vs32  d[1];
}volatile tVSS32;
typedef struct {
    vu16  w[1];
}volatile tVUU16;
typedef struct {
    vs16  w[1];
}volatile tVSS16;
typedef struct {
    vu8   b[1];
}volatile tVUU8;
typedef struct {
    vs8   b[1];
}volatile tVSS8;


//#define uu32(_val) ((u32)((((tUU32 *)&_val)->d)))
//#define ss32(_val) ((s32)((((tUU32 *)&_val)->d)))
//#define uu16(_val) ((u16)((((tUU16 *)&_val)->w)))
//#define ss16(_val) ((s16)((((tUU16 *)&_val)->w)))

#define au64(_val) ((tUU64 *)&_val)->d
#define as64(_val) ((tSS64 *)&_val)->d
#define au32(_val) ((tUU32 *)&_val)->d
#define as32(_val) ((tSS32 *)&_val)->d
#define au16(_val) ((tUU16 *)&_val)->w
#define as16(_val) ((tSS16 *)&_val)->w
#define au8(_val)  ((tUU8  *)&_val)->b
#define as8(_val)  ((tSS8  *)&_val)->b

#define uu64(_val) au64(_val)[0]
#define ss64(_val) as64(_val)[0]
#define uu32(_val) au32(_val)[0]
#define ss32(_val) as32(_val)[0]
#define uu16(_val) au16(_val)[0]
#define ss16(_val) as16(_val)[0]
#define uu8(_val)  au8(_val)[0]
#define ss8(_val)  as8(_val)[0]

#define vau64(_val) ((tVUU64 *)&_val)->d
#define vas64(_val) ((tVSS64 *)&_val)->d
#define vau32(_val) ((tVUU32 *)&_val)->d
#define vas32(_val) ((tVSS32 *)&_val)->d
#define vau16(_val) ((tVUU16 *)&_val)->w
#define vas16(_val) ((tVSS16 *)&_val)->w
#define vau8(_val)  ((tVUU8  *)&_val)->b
#define vas8(_val)  ((tVSS8  *)&_val)->b

#define vuu64(_val) vau64(_val)[0]
#define vss64(_val) vas64(_val)[0]
#define vuu32(_val) vau32(_val)[0]
#define vss32(_val) vas32(_val)[0]
#define vuu16(_val) vau16(_val)[0]
#define vss16(_val) vas16(_val)[0]
#define vuu8(_val)  vau8(_val)[0]
#define vss8(_val)  vas8(_val)[0]



#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
//typedef unsigned int   BOOL;

#ifdef __cplusplus
  #define   __I     volatile             //!< Defines 'read only' permissions                 
#else
  #define   __I     volatile const       //!< Defines 'read only' permissions                 
#endif
#define     __O     volatile             //!< Defines 'write only' permissions                
#define     __IO    volatile             //!< Defines 'read / write' permissions              

#endif //__SysType_H
