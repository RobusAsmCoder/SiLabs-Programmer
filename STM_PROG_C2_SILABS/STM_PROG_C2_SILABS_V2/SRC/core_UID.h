//////////////////////////////////////////////////////////////////////////////////////
// By Rob F. / Entire Group
//////////////////////////////////////////////////////////////////////////////////////

#ifndef __CORE_UID_H
#define __CORE_UID_H

#include "SysType.h"

typedef struct
{
  vu16 FlashSizeK;
  vu16 Free[3];
  vu32 U_ID[3];
} tcore_UID;
#define CORE_UID               ((tcore_UID *) ((u32)0x1FFFF7E0))


#endif
